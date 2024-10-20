
# 初始化环境
sudo rm -rf ~/oslab/*
cp -r /home/teacher/oslab/* ~/oslab/


# 内核代码修改
cp stat.h ~/oslab/linux-0.11/include/sys/stat.h

cp main.c ~/oslab/linux-0.11/init/main.c

cp namei.c ~/oslab/linux-0.11/fs/namei.c

cp proc.c ~/oslab/linux-0.11/fs/proc.c

cp read_write.c ~/oslab/linux-0.11/fs/read_write.c

cp Makefile ~/oslab/linux-0.11/fs/Makefile

# 文件系统修改
sudo bash ~/oslab/mount-hdc

sudo cp read_proc.c ~/oslab/hdc/usr/root/read_proc.c
sudo chown root:root ~/oslab/hdc/usr/root/read_proc.c

sudo umount ~/oslab/hdc

# 编译操作系统，运行

cd ~/oslab/linux-0.11

make clean

make all

cd ..

./run
