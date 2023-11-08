


#define SHM_TABLE_LEN 64

typedef struct shm
{
    unsigned long size;
    int key;
    unsigned long phyaddr;
    /*已经映射到几个进程空间中*/
    unsigned long numofprocess;
} shm;
