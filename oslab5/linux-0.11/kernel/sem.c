#include <linux/sem.h>
/*有get_fs_byte函数*/
#include <asm/segment.h>
/*定义了NULL*/
#include <stddef.h>
/*定义了cli sti*/
#include <asm/system.h>
/*有wake_up和sleep_on*/
#include <linux/sched.h>


sem_t sem_table[SEM_TABLE_LEN];
/*已创建的信号量数量*/
int numofsem = 0;


sem_t * sys_sem_open(const char* name, unsigned int value)
{
    char kernelname[SEM_NAME_LEN];
    /*不算'\0'的字符串长度*/
    int name_cnt = 0;
    while (get_fs_byte(name+name_cnt)!='\0')
    {
        name_cnt++;
    }
    /*name_cnt不符合规范*/
    if(name_cnt>SEM_NAME_LEN-1)
    {
        return NULL;
    }
    /*name_cnt符合规范*/
    int i=0;
    for(i=0; i<=name_cnt; i++)
    {
        /*'\0'也取到kernelname中*/
        kernelname[i] = get_fs_byte(name+i);
    }
    int is_exist=0;
    for(i=0; i<=numofsem-1; i++)
    {
        if(!strcmp(kernelname,sem_table[i].name))
        {
            is_exist = 1;
            break;
        }
    }
    sem_t * p = NULL;
    if(is_exist==1)
    {
        p = & sem_table[i];
    }
    else
    {
        /*创建新的信号量*/
        /*dest中有'\0'*/
        strcpy(sem_table[numofsem].name, kernelname);
        sem_table[numofsem].value = value;
        sem_table[numofsem].queue = NULL;
        p = & sem_table[numofsem];
        numofsem ++;
    }
    return p;

}

int sys_sem_wait(sem_t *sem)
{
    cli();
    /*因为利用sleep_on函数，sleep_on被唤醒后，会将队列中所有的进程都唤醒*/
    /*所以用的是while*/
    while((sem->value)==0)
    {
        /*queue是队首指针*/
        sleep_on(&(sem->queue));
    }
    sem->value--;
    sti();
    return 0;
}


int sys_sem_post(sem_t *sem)
{

    cli();
    sem->value++;
    wake_up(&(sem->queue));
    sti();
    return 0;
}

int sys_sem_unlink(const char *name)
{
    char kernelname[SEM_NAME_LEN];
    /*不算'\0'的字符串长度*/
    int name_cnt = 0;
    while (get_fs_byte(name+name_cnt)!='\0')
    {
        name_cnt++;
    }
    /*name_cnt不符合规范*/
    if(name_cnt>SEM_NAME_LEN-1)
    {
        return -1;
    }
    /*name_cnt符合规范*/
    int i=0;
    for(i=0; i<=name_cnt; i++)
    {
        /*'\0'也取到kernelname中*/
        kernelname[i] = get_fs_byte(name+i);
    }
    int is_exist = 0;
    for(i=0; i<=numofsem-1; i++)
    {
        if(!strcmp(kernelname,sem_table[i].name))
        {
            is_exist = 1;
            break;
        }
    }
    if(is_exist==1)
    {
        sem_table[i] = sem_table[numofsem-1];
        numofsem --; 
        return 0; 
    }
    /*信号量不存在，删除失败*/
    else
    {
        return -1;
    }
}