/*  #define __LIBRARY__， syscall才能用   */
#define __LIBRARY__
#include <linux/sem.h>

/*syscall在这里*/
#include <unistd.h>



_syscall2(sem_t *, sem_open, const char *, name, unsigned int, value)
_syscall1(int, sem_wait, sem_t *, sem)
_syscall1(int, sem_post, sem_t *, sem)
_syscall1(int, sem_unlink, const char *, name)
_syscall2(int, shmget, int, key, size_t,size)
_syscall1(void*, shmat, int, shmid)



int main()
{
    sem_t *mutex, *full, *empty;
    int shmid,i;
    int in = 0;
    int* p;
    
    mutex = sem_open("mutex", 1);
    full = sem_open("full", 0);
    empty = sem_open("empty", 10);

    shmid = shmget(1234,10*sizeof(int));
    p = (int*) shmat(shmid);
    
    for (i = 0; i <= 500; i++)
    {
        sem_wait(empty);
        sem_wait(mutex);
        *(p+in*sizeof(int)) = i;
        in = (in + 1)%10;
        sem_post(mutex);
        sem_post(full);
    }
    return 0;
}
