#include <stdarg.h>
/*LAST_TASK在这里*/
#include <linux/sched.h>
/*put_fs_byte在这里*/
#include <asm/segment.h>
/*定义了cli sti*/
#include <asm/system.h>


char proc_buf[4096]={'\0'};


int sprintf(char *buf, const char *fmt, ...)
{
    va_list args; int i;
    va_start(args, fmt);
    i=vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}

void get_psinfo()
{
    /*关中断，禁止schedule,禁止进程状态改变*/
    cli();
    struct task_struct ** p;
    /*下次向nread处填充*/
    int nread=0;
    nread = sprintf(proc_buf,"%s","pid\tstate\tfather\tcounter\tstart_time\n");
    for(p = &FIRST_TASK ; p<= &LAST_TASK ; p++)
    {
        if(*p)
        {
            nread=nread+sprintf(proc_buf+nread,"%d\t",(*p)->pid);
            nread=nread+sprintf(proc_buf+nread,"%d\t",(*p)->state);
            nread=nread+sprintf(proc_buf+nread,"%d\t",(*p)->father);
            nread=nread+sprintf(proc_buf+nread,"%d\t",(*p)->counter);
            nread=nread+sprintf(proc_buf+nread,"%d\n",(*p)->start_time);
        }
    }
    sti();
}


void get_procinfo(int dev)
{
    if (dev==0)
        get_psinfo();
}



int proc_read(int dev, char * buf, int count, off_t * pos)
{
    /* pos是指针 */
    int i;
    if(*pos==0)
    {
        get_procinfo(dev);
    }
    for(i=0;i<count;i++)
    {
        if(proc_buf[*pos+i]=='\0')
            break;
        put_fs_byte(proc_buf[*pos+i],buf+i);
    }
    *pos=*pos+i;
    return i;
}
