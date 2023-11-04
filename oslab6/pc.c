/*  #define __LIBRARY__， syscall才能用   */
#define __LIBRARY__
/*open在这个头文件中*/
#include <fcntl.h>
#include <linux/sem.h>
/*EINTR在这个头文件中*/
#include <errno.h>


#include <unistd.h>
/*包含stdout*/
#include <stdio.h>

_syscall2(sem_t *, sem_open, const char *, name, unsigned int, value)
_syscall1(int, sem_wait, sem_t *, sem)
_syscall1(int, sem_post, sem_t *, sem)
_syscall1(int, sem_unlink, const char *, name)

int main()
{
    /*  C99之前的旧标准：局部变量必须定义在函数开头，必须放在所有执行语句之前   */
    /*  否则会出现类似报错parse error before 'const'    */
    sem_t *mutex, *full, *empty;
    int i, pid, numofread;
    int item_used;
    int fi = open("/usr/root/buffer", O_TRUNC | O_CREAT | O_WRONLY, 0666);
    int fo = open("/usr/root/buffer", O_RDONLY, 0666);
    const int numofconsumer = 3;
    int end_flag = open("/usr/root/flag", O_RDWR | O_CREAT, 0666);
    char end;
    int consumer_pid[numofconsumer];

    mutex = sem_open("mutex", 1);
    full = sem_open("full", 0);
    empty = sem_open("empty", 10);

    /*给end_flag设置初值*/
    lseek(end_flag, 0, SEEK_SET);
    write(end_flag, "n", 1);    
    if (!fork())
    {
        /*子进程是生产者*/
        for (i = 0; i <= 500; i++)
        {
            sem_wait(empty);
            sem_wait(mutex);
            if (!(i % 10))
            {
                lseek(fi, 0, SEEK_SET);
            }
            write(fi, (char *)&i, sizeof(i));
            sem_post(mutex);
            sem_post(full);
        }
        /*写完就关*/
        close(fi);
        exit(0);
    }
    /*父进程创建消费者，父进程不是消费者*/
    else
    {
        for (i = 0; i <= numofconsumer - 1; i++)
        {
            /*父进程创建3个子进程，子进程break出去，子进程不创建孙进程*/
            if (!(pid = fork()))
            {
                while (1)
                {
                    sem_wait(full);
                    sem_wait(mutex);
                    numofread = read(fo, (char *)&item_used, sizeof(item_used));
                    /*如果读到文件末尾，什么都没读到，返回实际读到的字节数为0*/
                    if (numofread == 0)
                    {
                        /*生产者循环写，消费者循环读*/
                        lseek(fo, 0, SEEK_SET);
                        read(fo, (char *)&item_used, sizeof(item_used));
                    }
                    pid = getpid();
                    printf("%d:\t%d\n", pid, item_used);
                    fflush(stdout);

                    if (item_used == 500)
                    {
                        lseek(end_flag, 0, SEEK_SET);
                        write(end_flag, "y", 1);
                        exit(0);
                    }
                    sem_post(mutex);
                    sem_post(empty);
                }
                break;
            }
            else
            {
                /*父进程记录消费者的pid*/
                consumer_pid[i] = pid;
            }


        }
    }




    /*所有子进程都exit，父进程才exit*/
    while (1)
    {
		int ret;
        /*父进程暂停自己的执行，等待子进程exit*/
    	ret = wait(NULL);
        /*子进程exit，父进程被唤醒*/
        /*如果是生产者子进程exit，父进程接着wait*/
        /*如果是消费者子进程exit，父进程kill所有消费者*/


        lseek(end_flag, 0, SEEK_SET);
        read(end_flag,&end,1);
        /*500已经printf，kill所有消费者*/
        if (end == 'y')
        {
            for (i = 0; i <= numofconsumer - 1; i++)
            {
                kill(consumer_pid[i],SIGKILL);
            }

            sem_unlink("mutex");
            sem_unlink("full");
            sem_unlink("empty");
            /*父进程exit*/
            exit(0);
        }

        if (ret == -1)
        {
            if (errno == EINTR)
            {
                /* 返回值为-1的时候有两种情况，一种是没有子进程了，还有一种是被中断了*/
                continue;
            }
            break;
        }
    }

    return 0; 
}
