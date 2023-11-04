#ifndef _SEM_H
#define _SEM_H

#include <linux/sched.h>

#define SEM_NAME_LEN 20
#define SEM_TABLE_LEN 50

typedef struct semaphore
{
    char name[SEM_NAME_LEN];
    int value;
    /*队首指针，指向PCB*/
    struct task_struct *queue;
} sem_t;

#endif
