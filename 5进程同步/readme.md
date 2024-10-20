## 信号量

概念： 信号量是一个计数器，用来表示某种资源的数量。
作用： 用来控制多个进程对共享资源的访问。
类型：
二元信号量： 取值只有 0 和 1，常用于互斥。
一般信号量： 取值可以是任意非负整数，常用于同步。

PV操作的定义

P操作（wait）：
将信号量的值减1。
如果减1后信号量的值小于0，则该进程阻塞，等待被唤醒。
否则，该进程继续执行。

V操作（signal）：
将信号量的值加1。
如果有进程阻塞在该信号量上，则唤醒一个阻塞进程。

## POSIX标准

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

close(fd);
