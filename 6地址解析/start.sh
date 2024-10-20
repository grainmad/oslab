
# 初始化环境
sudo rm -rf ~/oslab/*
cp -r /home/teacher/oslab/* ~/oslab/


# 内核代码修改


# 文件系统修改
sudo bash ~/oslab/mount-hdc

sudo cp test.c ~/oslab/hdc/usr/root/test.c
sudo chown root:root ~/oslab/hdc/usr/root/test.c

sudo umount ~/oslab/hdc

# 编译操作系统，运行

cd ~/oslab/linux-0.11

make clean

make all

cd ..

./dbg-asm
