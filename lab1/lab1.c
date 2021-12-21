#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wait.h>
#include <string.h>

int p1 = 0, p2 = 0; //child proccess id
int pipefd[2];
char buffer[128]; //pipe buffer
int count;

void parent_int_handle(int sig_no)
{
    if (sig_no == SIGINT)
    {
        kill(p1, SIGUSR1);
        kill(p2, SIGUSR2);
        wait(NULL);
        wait(NULL);
        printf("Amount of msg sended: %d\n", count);
        printf("Parent Procees is Killed!\n");
        close(pipefd[1]);
        exit(0);
    }
    return;
}

void sigusr_int_handle(int sig_no)
{
    if (sig_no == SIGUSR1) {
        printf("p1's amount of msg recieved: %d\n", count);
        printf("Child Proccess 1 is Killed by Parent!\n");
        close(pipefd[0]);
        exit(0);
    }
    else if (sig_no == SIGUSR2)
    {
        printf("p2's amount of msg recieved: %d\n", count);
        printf("Child Proccess 2 is Killed by Parent!\n");
        close(pipefd[0]);
        exit(0);
    }
    return;
}

int proc1()
{
    close(pipefd[1]); //子进程关闭写端口
    count = 0;
    if (signal(SIGINT, SIG_IGN) == SIG_ERR)
    {
        printf("can't catch SIGINT\n"); //ignore SIGINT in child proccess
    }
    if (signal(SIGUSR1, sigusr_int_handle) == SIG_ERR)
    {
        printf("can't catch SIGUSR1\n");
    }
    while (1)
    {
        if (read(pipefd[0], buffer, 128) == 0)
        {
            printf("p1's amount of msg recieved: %d\n", count);
            printf("Child Proccess 1 is Killed by Parent!\n");
            close(pipefd[0]);
            exit(0);
        }
        printf("proc1: %s", buffer);
        count++;
    }
}

int proc2()
{
    close(pipefd[1]);   //子进程关闭写端口
    count = 0;
    if (signal(SIGINT, SIG_IGN) == SIG_ERR)
    {
        printf("can't catch SIGINT\n"); //ignore SIGINT in child proccess
    }
    if (signal(SIGUSR2, sigusr_int_handle) == SIG_ERR)
    {
        printf("can't catch SIGUSR2\n");
    }
    while (1)
    {
        if (read(pipefd[0], buffer, 128) == 0)
        {
            printf("p2's amount of msg recieved: %d\n", count);
            printf("Child Proccess 2 is Killed by Parent!\n");
            close(pipefd[0]);
            exit(0);
        }
        printf("proc2: %s", buffer);
        count++;
    }
}

int main()
{
    count = 0;
    if (pipe(pipefd) < 0)
    {
        printf("failed to creat pipe\n");
    }
    if (signal(SIGINT, parent_int_handle) == SIG_ERR) {
        printf("can't catch SIGINT\n");
    }
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        printf("can't catch SIGPIPE\n");
    }

    if ( (p1 = fork()) == 0) {
        proc1();
    }
    else if ((p2 = fork()) == 0)
    {
        proc2();
    }
    close(pipefd[0]); //父进程关闭读端口
    count = 0;
    while (count != 10)
    {
        sprintf(buffer, "I send you %d times.\n", ++count);
        write(pipefd[1], buffer, 128);
        sleep(1);
    }
    close(pipefd[1]);
    wait(NULL);
    wait(NULL);
    printf("Amount of msg sended: %d\n", count);
    printf("Parent Procees is Killed!\n");
    exit(0);
}