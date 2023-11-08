#ifndef _MM_H
#define _MM_H

#define PAGE_SIZE 4096
#define LOW_MEM 0x100000

extern unsigned long get_free_page(void);
extern unsigned long put_page(unsigned long page,unsigned long address);
extern void free_page(unsigned long addr);
extern unsigned char mem_map[];


#endif
