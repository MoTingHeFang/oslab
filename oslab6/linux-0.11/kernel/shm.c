#include <linux/shm.h>
/*get_free_page在这里*/
#include <linux/mm.h>
/*ENOMEM在这里*/
#include <errno.h>
/*current在这里*/
#include <linux/sched.h>


shm shm_table[SHM_TABLE_LEN];
/*已创建的共享内存数量*/
int numofshm=0;

int sys_shmget(int key, size_t size)
{
    int i;
    unsigned long phyaddr;
    if(numofshm==SHM_TABLE_LEN)
    {
        return -ENOMEM;
    }
    if(size>PAGE_SIZE)
    {
        return -EINVAL;
    }
    for(i=0; i<=numofshm-1;i++)
    {
        if(shm_table[i].key==key)
        {
            return i;
        }
    }
    phyaddr = get_free_page();
    if (!phyaddr)
    {
        return -ENOMEM;
    }
    shm_table[numofshm].key=key;
    shm_table[numofshm].size=size;
    shm_table[numofshm].phyaddr=phyaddr;
    /*尚未映射到进程空间中*/
    shm_table[numofshm].numofprocess=0;
    numofshm++;
    return (numofshm-1);
}

void* sys_shmat(int shmid)
{
    unsigned long viraddr,logaddr,index;
    if(shmid>=numofshm)
    {
        return -EINVAL;
    }
    
    /*段基址+逻辑地址，brk对应的位置未使用*/
    viraddr = get_base(current->ldt[1])+current->brk;

    /*向上取整*/
    viraddr += PAGE_SIZE-1;
    viraddr &= 0xFFFFF000;

    /*设置页表*/
    put_page(shm_table[shmid].phyaddr, viraddr);

    shm_table[shmid].numofprocess++;
    
    /*映射到第一个进程空间不加，get_free_page已经设1了*/
    /*映射到第2、3...个进程空间就要加加了*/
    if (shm_table[shmid].numofprocess!=1)
    {
        index = (shm_table[shmid].phyaddr -LOW_MEM)>>12;
        mem_map[index]++;
    }
    		
    /*虚拟地址-段基址*/
    logaddr = viraddr - get_base(current->ldt[1]);

    /*修改brk*/
    current->brk=viraddr+4096;

    /*返回逻辑地址*/
    return logaddr;
}
