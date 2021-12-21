#include "header.h"

char buf; //缓冲区

int main(int argc, char const *argv[])
{
    int shmid, semid;
    sscanf(argv[0], "%d", &shmid);
    sscanf(argv[1], "%d", &semid);
    printf("enter writebuf, argv: [%d, %d]\n", shmid, semid);
    shm_struct *shm_handle = (shm_struct*) shmat(shmid, NULL, SHM_R | SHM_W);
    FILE *fd;
    if ((fd = fopen(src_path, "r")) == NULL)
    {
        fprintf(stderr, "fopen failed: writebuf\n");
        return 1;
    }
    while (!feof(fd))
    {
        fread(&buf, sizeof(char), 1, fd);
        shm_handle->in = (shm_handle->in + 1) % shmlen;
        P(semid, empty);
        shm_handle->buf[shm_handle->in] = buf;
        V(semid, full);
    }
    P(semid, mutex4flag);
    shm_handle->flag = 1;
    V(semid, mutex4flag);
    fclose(fd);
    return 0;
}
