
实现proc文件系统

/proc、/proc/psinfo、/proc/hdinfo、/proc/dirinfo

打印进程信息

当前文件系统信息

根节点目录

# minix 文件系统
6个部分组成，引导块，超级块，i节点位图，逻辑块位图，i节点，数据区

inode结构，盘上的**i节点部分**存放着文件系统中文件（或目录）的索引节点，每个文件（或目录）都有一个 i节点。每个 i 节点结构中存放着对应文件的相关信息。
inode 的i_mode如果是普通文件,那么zone[0]到zone[6]直接存储的盘块可以存储7*1024字节.而在zone[7]中的存储的盘块1024字节存储的全是盘块号,一个盘块号占用2字节,总共可以存储512个索引盘块号.

文件中的数据是放在磁盘块的**数据区**中的，而一个文件名则通过对应的 i 节点与这些数据磁盘块相联系，这些盘块的号码就存放在 i 节点的逻辑块数组 i_zone[]中。其中，i_zone[]数组用于存放 i 节点对应文件的盘块号。i_zone[0]到 i_zone[6]用于存放文件开始的 7 个磁盘块号，称为直接块。若文件长度小于等于 7K 字节，则根据其 i 节点可以很快就找到它所使用的盘块。若文件大一些时，就需要用到一次间接块了（i_zone[7]）
，这个盘块中存放着附加的盘块号。对于 MINIX 文件系统它可以存放 512 个盘块号，因此可以寻址 512 个盘块。若文件还要大，则需要使用二次间接盘块（i_zone[8]）。二次间接块的一级盘块的作用类似与一次间接盘块，因此使用二次间接盘块可以寻址 512*512 个盘块。

有些数据区的盘块存储的是目录结构体

```c
#define NAME_LEN 14
struct dir_entry {
	unsigned short inode;
	char name[NAME_LEN];
};
```

一个盘块1024KB，dir_entry 占用16字节,共可存储64个目录结构体,结构体中的inode号可以定位到i节点区的inode结构体

inode其i_mode类型是目录,则通过i_zone[0]存储的盘块号找到的盘块存储了64个dir_entry

i节点区的第一个inode是根目录i节点,存储到当前进程`current->root` 

```c
if (!(mi=iget(ROOT_DEV,ROOT_INO))) // 获取ROOT_DEV设备的第ROOT_INO=1个i节点
		panic("Unable to read root i-node");
	mi->i_count += 3 ;	/* NOTE! it is logically used 4 times, not 1 */
	p->s_isup = p->s_imount = mi;
	current->pwd = mi;
	current->root = mi;
```

特殊的目录项`.. .`