#include "header.h"

int main(int argc, char const *argv[])
{
    /* 创建共享内存 */
    int shmid;
    if ((shmid = shmget(IPC_PRIVATE, sizeof(shm_struct), IPC_CREAT | 0666)) < 0)
    {
        fprintf(stderr, "shmget failed: %s\n", strerror(shmid));
        return 1;
    }
    shm_struct *shm_handle = (shm_struct *)shmat(shmid, NULL, SHM_R | SHM_W);
    shm_handle->in = -1;
    shm_handle->out = -1;
    shm_handle->flag = 0;

    /* 创建信号灯集 */
    int semid;                     //信号灯集合句柄
    if ((semid = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666)) < 0)
    {
        fprintf(stderr, "semget failed: %s\n", strerror(semid));
        return 1;
    }

    /* 信号灯赋予初值, 初始设置信号灯empty的值为shmlen, full的值为0，mutex4flag设为1 */
    union semun arg;
    arg.val = shmlen;
    if (semctl(semid, empty, SETVAL, arg) == -1)
    {
        fprintf(stderr, "can't set a value for 'empty'!\n");
        semctl(semid, empty, IPC_RMID, NULL);
        return 1;
    }
    arg.val = 0;
    if (semctl(semid, full, SETVAL, arg) == -1)
    {
        fprintf(stderr, "can't set a value for 'full'!\n");
        semctl(semid, full, IPC_RMID, NULL);
        return 1;
    }
    arg.val = 1;
    if (semctl(semid, mutex4flag, SETVAL, arg) == -1)
    {
        fprintf(stderr, "can't set a value for 'mutex4flag'!\n");
        semctl(semid, mutex4flag, IPC_RMID, NULL);
        return 1;
    }

    /* 创建读写进程 */
    pid_t pr, pw;
    char _argv[2][10];
    sprintf_s(_argv[0], "%d", shmid, 10);
    sprintf_s(_argv[1], "%d", semid, 10);
    if ((pr = fork()) == 0)
    {
        execv("./readbuf.c", _argv);
    }
    else if ((pw = fork()) == 0)
    {
        execv("./writebuf.c", _argv);
    }
    
    /* 等待子进程结束 */
    wait(NULL);
    wait(NULL);

    /* 删除信号灯集和共享内存 */
    if (semctl(semid, 0, IPC_RMID, arg) == -1)
    {
        fprintf(stderr, "can't delete sems!\n");
        return 1;
    }
    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        fprintf(stderr, "can't delete shms!\n");
        return 1;
    }
    return 0;
}