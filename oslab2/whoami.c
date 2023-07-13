#define __LIBRARY__
#include <unistd.h>

_syscall2(int, whoami,char *,name,unsigned int,size);

int main()
{
    char str [30];
    whoami(str,30);
    printf("%s\n",str);
    return 0;
}