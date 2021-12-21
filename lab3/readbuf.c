#include "header.h"

char buf; //缓冲区

int main(int argc, char const *argv[])
{
    int shmid, semid;
    sscanf(argv[0], "%d", &shmid);
    sscanf(argv[1], "%d", &semid);
    printf("enter readbuf, argv: [%d, %d]\n", shmid, semid);
    shm_struct *shm_handle = (shm_struct *)shmat(shmid, NULL, SHM_R | SHM_W);
    int fd;
    if ((fd = fopen(filename, 'wb')) == NULL)
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
        buf = shm_handle->buf[shm_handle->out];
        V(semid, empty);
        fwrite(&buf, sizeof(char), 1, fd);
    }
    while (shm_handle->in != shm_handle->out)
    {
        shm_handle->out = (shm_handle->out + 1) % shmlen;
        buf = shm_handle->buf[shm_handle->out];
        fwrite(&buf, sizeof(char), 1, fd);
    }
    fclose(fd);
    return 0;
}
