/*  #define __LIBRARY__， syscall才能用   */
#define __LIBRARY__
/*open在这个头文件中*/
#include <fcntl.h>
#include <linux/sem.h>

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
    mutex = sem_open("mutex", 1);
    full = sem_open("full", 0);
    empty = sem_open("empty", 10);

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
        close(fi);
        exit(0);
    }
    /*父进程创建消费者*/
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
                    /*文件描述符已经关闭，有个进程已经读到500*/
                    else if (numofread == -1)
                    {
                        exit(0);
                    }
                    pid = getpid();
                    printf("%d:\t%d\n", pid, item_used);
                    fflush(stdout);
                    if (item_used == 500)
                    {
                        close(fo);
                        exit(0);
                    }

                    sem_post(mutex);
                    sem_post(empty);
                }
                break;
            }
        }
    }

    sem_unlink("mutex");
    sem_unlink("full");
    sem_unlink("empty");

    /*父进程退出*/
    return 0;
}
