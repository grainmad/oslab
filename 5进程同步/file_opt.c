#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#define BUFFER_SIZE 1024

int main(int argc, char **argv)
{
/*
    使用open前需要先包含头文件
    #include <sys/types.h>
    #inlcude <sys/stat.h>
    #inlcude <fcntl.h>

    int fd = open（const char *pathname,int flags,mode_t mode）;
    
    fd 
    句柄file descriptor
    标准输入是0，标准输出是1，标准出错是2。
    打开一个文件会返回句柄来操作这个文件，一般是从3开始，然后4,5,6一直下去。
    close（fd）之后句柄就返回给系统

    pathname
    文件路径

    flags
    主类：只可以选一个
        O_RDONLY 以只读方式打开   
        O_WRONLY 以只写方式打开   
        O_RDWR 以可读可写方式打开
    副类: 可以组合使用
        O_CREAT 如果文件不存在则创建该文件
        O_EXCL 如果使用O_CREAT选项且文件存在，则返回错误消息
        O_NOCTTY 如果文件为终端，那么终端不可以调用open系统调用的那个进程的控制终端
        O_TRUNC 如果文件已经存在则删除文件中原有数据
        O_APPEND 以追加的方式打开主副可以配合使用，
    
    mode
    如果文件被新建，指定其权限mode
    八进制权限码，0777表示 文件所有者，该文件用户组，其他用户 都有可读可写可执行权限
    0644:
    -rw-r--r--：表示文件所有者有读写权限，同组用户有读权限，其他用户有读权限。这是创建普通文件的常用模式。
    0755:
    -rwxr-xr-x：表示文件所有者有读写执行权限，同组用户和其它用户有读执行权限。这是创建可执行文件的常用模式。
    0700:
    -rwx------：表示只有文件所有者有读写执行权限，其他用户没有任何权限。
    0000:
    --------：表示没有任何权限
    */
    int fd, count;
    fd = open("./test.txt", O_RDWR|O_CREAT|O_EXCL, 0644);
    printf("test fd:%d\n", fd);
    if (fd == -1) {
        perror("open");
        return 0;
    }
    /*
    使用write前需要先包含头文件
    #include <unistd.h>

    ssize_t count = write(int filed,const void *buf,size_t nbytes);
    
    count 
        写入的字节数
        返回值：返回实际写入的字节数，若出错则返回-1。

    filed
        文件描述符
    buf
        内存地址
    nbytes
        数据大小
    */
    char s[BUFFER_SIZE] = "1234loli";
    count = write(fd, s, 6);
    printf("write count: %d\n", count);
    if (count == -1) {
        perror("write");
    }
    /*
    off_t currpos = lseek(int fileds,off_t offset,int whence);
    
    currpos 新的文件偏移量
    返回值：若成功返回新的文件偏移量，若出错返回-1。
    

    fileds
        文件描述符
    offset
        偏移量
         -？  0   +？
    whence
        相对位置
        文件开始            当前位置            文件末尾
        SEEK_SET           SEEK_CUR          SEEK_END
    */
    lseek(fd, 0, SEEK_SET);
    /*
    使用read前需要先包含头文件
    #include <unistd.h>

    ssize_t count = read(int filedes,void *buf,size_t nbytes);

    count 
        返回读到的字节数
        若成功返回读到的字节数，若读到文件末尾返回0，若失败返回-1。
    filedes 
        文件描述符
    buf
        用户内存地址
    nbytes
        读取数据大小（字节）

    读取到buf的字符串不带有'\0'
    */
    count = read(fd, s, 4); // s 1234
    printf("read count: %d\n", count);
    printf("s: %s\n", s);
    if (count == -1) {
        perror("read");
    }
    
    count = read(fd, s, 2); // s lo
    printf("read count: %d\n", count);
    printf("s: %s\n", s);
    if (count == -1) {
        perror("read");
    }

    count = write(fd, s, 2); // file 1234lolo
    printf("write count: %d\n", count);
    if (count == -1) {
        perror("write");
    }
    close(fd);
    
    fd = open("./test2.txt", O_RDWR|O_CREAT|O_EXCL, 0644);
    printf("test2 fd:%d\n", fd);
    if (fd == -1) {
        perror("open");
        return 0;
    }
    
    int v = 1163284300; // (69<<24) + (86<<16) + (79<<8) + 76 EVOL
    count = write(fd, &v, sizeof(v));
    printf("write count: %d\n", count);
    if (count == -1) {
        perror("write");
    }

    lseek(fd, 0, SEEK_SET); // 回到开始

    v = 0;
    count = read(fd, &v, sizeof(v));
    printf("read count: %d\n", count);
    printf("v: %d\n", v);
    if (count == -1) {
        perror("read");
    }

    lseek(fd, 0, SEEK_SET); // 回到开始

    count = read(fd, s, sizeof(v));

    printf("read count: %d\n", count);
    printf("s: %s\n", s);
    if (count == -1) {
        perror("read");
    }

    close(fd);   
    return 0;
}
/*
test fd:3
write count: 6
read count: 4
s: 1234loli
read count: 2
s: lo34loli
write count: 2
test2 fd:3
write count: 4
read count: 4
v: 1163284300
read count: 4
s: LOVEloli
*/