#include <errno.h>
#include <string.h>
#include <asm/segment.h>//put_fs_byte、get_fs_byte在这个头文件中

char myname[24];

int sys_iam(const char * name)
{
	int i;
	//临时存储 输入字符串 操作失败时不影响msg
	char tmp[30];
	for(i=0; i<30; i++)
	{
		//从用户态内存取得数据
		tmp[i] = get_fs_byte(name+i);
		if(tmp[i] == '\0') break;  //字符串结束
	}
	//printk(tmp);
	i=0;
	while(i<=29&&tmp[i]!='\0') i++;
	int numofchar = i;
	// int len = strlen(tmp);
	//字符长度大于23个
	if(numofchar > 23)
	{
		// printk("String too long!\n");
		return -EINVAL;  //具体见_syscalln宏展开!!!!
	}
	strcpy(myname,tmp);
	//printk(tmp);
	return numofchar;
}

int sys_whoami(char* name, unsigned int size)
{
    int len=0;//字符数，不包括'\0'
    while(myname[len]!='\0')
    {
        len++;
    }
    if(len>size-1)
    //需要存len个字符，size大的数组，可存size-1个字符
    {
        errno = EINVAL;
        return -1;
    }else{
        int i=0;
        for(i=0;i<=len;i++)
        {
            put_fs_byte(myname[i],name+i);
        }
        return len;
    }
}