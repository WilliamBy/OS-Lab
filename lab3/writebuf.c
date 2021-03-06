/* writebuf.h */
#include "header.h"

char buf[8]; //缓冲区

int main(int argc, char const *argv[])
{
    int shmid, semid;
    sscanf(argv[0], "%d", &shmid);
    sscanf(argv[1], "%d", &semid);
    shm_struct *shm_handle = (shm_struct*) shmat(shmid, NULL, SHM_R | SHM_W);
    FILE *fd;
    size_t read_size;
    if ((fd = fopen(src_path, "r")) == NULL)
    {
        fprintf(stderr, "fopen failed: writebuf\n");
        return 1;
    }
    while (!feof(fd))
    {
        read_size = fread(buf, sizeof(char), 8, fd);
        shm_handle->in = (shm_handle->in + 1) % shmlen;
        P(semid, empty);
        memcpy(shm_handle->buf[shm_handle->in], buf, read_size);
        V(semid, full);
    }
    P(semid, mutex4flag);
    shm_handle->flag = 1;
    V(semid, mutex4flag);
    fclose(fd);
    return 0;
}
