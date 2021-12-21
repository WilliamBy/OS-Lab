#include <linux/sem.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>

int semid;
int ticket_num = 2000;

void P(int semid, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = 0;       //操作标记：0或IPC_NOWAIT等
    semop(semid, &sem, 1); //1:表示执行命令的个数
    return;
}

void V(int semid, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = 1;
    sem.sem_flg = 0;
    semop(semid, &sem, 1);
    return;
}

void *subp1()
{
    int sold_num = 0;
    while (ticket_num > 0)
    {
        P(semid, 0);
        if (ticket_num > 0) {
            ticket_num--;
            sold_num++;
            printf("subp1 just sold one ticket.\n");
        }
        V(semid, 0);
    }
    printf("subp1 totally sold %d tickets.\n", sold_num);
    return;
}

void *subp2()
{
    int sold_num = 0;
    while (ticket_num > 0)
    {
        P(semid, 0);
        if (ticket_num > 0)
        {
            ticket_num--;
            sold_num++;
            printf("subp2 just sold one ticket.\n");
        }
        V(semid, 0);
    }
    printf("subp2 totally sold %d tickets.\n", sold_num);
    return;
}

int main()
{
    if ((semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666)) == -1) {
        printf("can't get sem!\n");
        return 1;
    }
    //设置信号灯集合中第一个信号灯的值为1
    union semun arg;
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1)
    {
        printf("can't set a value for sem!\n");
        semctl(semid, 0, IPC_RMID, NULL);
        return 1;
    }
    pthread_t p1, p2;
    pthread_create(&p1, 0, subp1, NULL);
    pthread_create(&p2, 0, subp2, NULL);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    if (semctl(semid, 0, IPC_RMID, arg) == -1)
    {
        printf("can't delete sem!\n");
        return 1;
    }
    return 0;
}