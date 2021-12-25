#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#define shmlen 10 //共享缓冲区大小

union semun
{
	int 			val;
	struct semid_ds *buf;
	unsigned short *array;
	struct seminfo *__buf;
};

//共享缓冲区结构体
typedef struct SHM_STRUCT{
    int in;    //读缓冲区指针
    int out;   //写缓冲区指针
    char buf[shmlen][8];  //缓冲区容器
    int flag;    //结束标志，1表示写完成
} shm_struct;

const char *src_path = "src.jpg";
const char *des_path = "des.jpg";
const int empty = 0, full = 1, mutex4flag = 2; //信号灯编号，empty代表空缓冲区，full代表满缓冲区，mutex4flag代表结束标志

void P(int semid, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = 0;       //操作标记：0或IPC_NOWAIT等
    semop(semid, &sem, 1); // 1:表示执行命令的个数
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