
此文件夹的文件，包含实验需要修改的内核代码，测试程序

压缩此文件夹所有文件，然后上传至蓝桥实验楼，解压执行脚本start.sh

# 修改内核文件

linux-0.11/kernel/Makefile
添加who.c编译

linux-0.11/include/linux/sys.h
系统调用函数指针数组中添加新指针

linux-0.11/kernel/system_call.s
修改总系统调用个数

linux-0.11/kernel/who.c
编写两个系统调用函数

linux-0.11/include/unistd.h
添加宏定义，两个函数在函数指针数组中的偏移量


# 修改文件系统

iam.c 用户态程序，在内核内存中放字符串

whoami.c 读取内核内存字符串

testlab2.c 测试系统调用api

testlab2.sh 测试iam.c whoami.c

## 坑
gcc 1.4 注释不支持 `//`

系统调用，失败返回值设为`-1`,`errno=EINVAL`。
由于在宏里处理了，只需返回`-EINVAL`
``` c
#define _syscall0(type,name) \
type name(void) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name)); \
if (__res >= 0) \
	return (type) __res; \
errno = -__res; \
return -1; \
}
```

系统调用函数，接受了用户态程序传递的字符串参数`name`。  
由于数据段基址不一样，不能直接获取字符`name[0]`,或`strlen`,`strcpy`等库函数。这些底层汇编都是基于内核态段基址作的偏移。
需要用户态数据段基址偏移`name`得到内存中真正的位置。