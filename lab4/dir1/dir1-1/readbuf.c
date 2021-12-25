#include "header.h"

char buf[8]; //缓冲区

int main(int argc, char const *argv[])
{
    int shmid, semid;
    sscanf(argv[0], "%d", &shmid);
    sscanf(argv[1], "%d", &semid);
    printf("enter readbuf, argv: [%d, %d]\n", shmid, semid);
    shm_struct *shm_handle = (shm_struct *)shmat(shmid, NULL, SHM_R | SHM_W);
    FILE *fd;
    if ((fd = fopen(des_path, "w")) == NULL)
    {
        fprintf(stderr, "fopen failed: readbuf\n");
        return 1;
    }
    while (1)
    {
        P(semid, mutex4flag);   //检查读进程是否结束
        if (shm_handle->flag == 1) {
            break;
        }
        V(semid, mutex4flag);
        shm_handle->out = (shm_handle->out + 1) % shmlen;
        P(semid, full);
        memcpy(buf, shm_handle->buf[shm_handle->out], 8);
        V(semid, empty);
        fwrite(buf, sizeof(char), 8, fd);
    }
    while (shm_handle->in != shm_handle->out)
    {
        shm_handle->out = (shm_handle->out + 1) % shmlen;
        memcpy(buf, shm_handle->buf[shm_handle->out], 8);
        fwrite(buf, sizeof(char), 8, fd);
    }
    fclose(fd);
    return 0;
}
