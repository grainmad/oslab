
# 初始化环境
sudo rm -rf ~/oslab/*
cp -r /home/teacher/oslab/* ~/oslab/


# 内核代码修改

cp fork.c ~/oslab/linux-0.11/kernel/fork.c

cp kernel.h ~/oslab/linux-0.11/include/linux/kernel.h

cp main.c ~/oslab/linux-0.11/init/main.c

cp printk.c ~/oslab/linux-0.11/kernel/printk.c

cp sched.c ~/oslab/linux-0.11/kernel/sched.c

cp exit.c ~/oslab/linux-0.11/kernel/exit.c

# 文件系统修改

sudo bash ~/oslab/mount-hdc

sudo cp process.c ~/oslab/hdc/usr/root/process.c
sudo chown root:root ~/oslab/hdc/usr/root/process.c


sudo umount ~/oslab/hdc

# 编译操作系统，运行

cd ~/oslab/linux-0.11

make clean

make all

cd ..

./run
