#include <linux/sem.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>

int semid; //信号灯集合句柄
//信号灯编号, is_empty代表a可以被赋值, is_full表示a的值待取走, mutex4flag表示完成标志的互斥信号灯
const int is_empty = 0, odd_ok = 1, mutex4flag = 2;
int a;        //公共缓冲区
int flag = 0; //计算已完成标志，0表示还没达到最后一个累加和，1表示最后一个累加和计算完成但结果尚未打印，2表示已经打印了最后一个累加和
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

void *subp3() //计算
{
    int amount = 0; //累加和
    for (int i = 1; i <= 100; i++)
    {
        amount += i;
        P(semid, is_empty);
        P(semid, mutex4flag);
        if (i == 100)
        {
            flag = 1;
        }
        V(semid, mutex4flag);
        a = amount;
        V(semid, odd_ok);
    }
}

void *subp2() //打印偶数
{
    while (1)
    {
        P(semid, odd_ok);
        P(semid, mutex4flag);
        if (flag == 2) {
            V(semid, mutex4flag);
            V(semid, odd_ok);
            break;
        }
        if (a % 2 == 0)
        {
            if (flag == 1) {
                flag = 2;
                V(semid, odd_ok);
            }
            V(semid, is_empty);
            printf("subp2: %d\n", a);
        }
        else
        {
            V(semid, odd_ok);
        }
        V(semid, mutex4flag);
    }
}

void *subp1() //打印奇数
{
    while (1)
    {
        P(semid, odd_ok);
        P(semid, mutex4flag);
        if (flag == 2)
        {
            V(semid, mutex4flag);
            V(semid, odd_ok);
            break;
        }
        if (a % 2 == 1)
        {
            if (flag == 1)
            {
                flag = 2;
                V(semid, odd_ok);
            }
            printf("subp1: %d\n", a);
            V(semid, is_empty);
        }
        else
        {
            V(semid, odd_ok);
        }
        V(semid, mutex4flag);
    }
}

int main()
{
    if ((semid = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666)) == -1)
    {
        printf("can't get sem!\n");
        return 1;
    }
    //初始设置信号灯is_full的值为0, is_empty为1, mutex4flag为1
    union semun arg;
    arg.val = 1;
    if (semctl(semid, mutex4flag, SETVAL, arg) == -1)
    {
        printf("can't set a value for 'mutex4cnt'!\n");
        semctl(semid, mutex4flag, IPC_RMID, NULL);
        return 1;
    }
    if (semctl(semid, is_empty, SETVAL, arg) == -1)
    {
        printf("can't set a value for 'is_empty'!\n");
        semctl(semid, is_empty, IPC_RMID, NULL);
        return 1;
    }
    arg.val = 0;
    if (semctl(semid, odd_ok, SETVAL, arg) == -1)
    {
        printf("can't set a value for 'is_full'!\n");
        semctl(semid, odd_ok, IPC_RMID, NULL);
        return 1;
    }
    pthread_t p1, p2, p3; // p3计算，p2、p1打印
    pthread_create(&p1, 0, subp1, NULL);
    pthread_create(&p2, 0, subp2, NULL);
    pthread_create(&p3, 0, subp3, NULL);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);
    //删除信号灯集
    if (semctl(semid, 0, IPC_RMID, arg) == -1)
    {
        printf("can't delete 'is_full'!\n");
        return 1;
    }
    return 0;
}