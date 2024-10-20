
# 初始化环境
sudo rm -rf ~/oslab/*
cp -r /home/teacher/oslab/* ~/oslab/


# 内核代码修改
cp Makefile ~/oslab/linux-0.11/kernel/Makefile

cp sys.h ~/oslab/linux-0.11/include/linux/sys.h

cp system_call.s ~/oslab/linux-0.11/kernel/system_call.s

cp sem.c ~/oslab/linux-0.11/kernel/sem.c

cp sem.h ~/oslab/linux-0.11/include/linux/sem.h

cp unistd.h ~/oslab/linux-0.11/include/unistd.h

cp sched.h ~/oslab/linux-0.11/include/linux/sched.h

# 文件系统修改
sudo bash ~/oslab/mount-hdc

sudo cp pc.c ~/oslab/hdc/usr/root/pc.c
sudo chown root:root ~/oslab/hdc/usr/root/pc.c

sudo cp test_sem.c ~/oslab/hdc/usr/root/test_sem.c
sudo chown root:root ~/oslab/hdc/usr/root/test_sem.c

sudo cp unistd.h ~/oslab/hdc/usr/include/unistd.h
sudo chown root:root ~/oslab/hdc/usr/include/unistd.h

sudo cp sem.h ~/oslab/hdc/usr/include/linux/sem.h
sudo chown root:root ~/oslab/hdc/usr/include/linux/sem.h

sudo umount ~/oslab/hdc

# 编译操作系统，运行

cd ~/oslab/linux-0.11

make clean

make all

cd ..

./run
