
# 初始化环境
sudo rm -rf ~/oslab/*
cp -r /home/teacher/oslab/* ~/oslab/


# 内核代码修改
cp Makefile ~/oslab/linux-0.11/kernel/Makefile

cp sys.h ~/oslab/linux-0.11/include/linux/sys.h

cp system_call.s ~/oslab/linux-0.11/kernel/system_call.s

cp who.c ~/oslab/linux-0.11/kernel/who.c

cp unistd.h ~/oslab/linux-0.11/include/unistd.h


# 文件系统修改
sudo bash ~/oslab/mount-hdc

sudo cp iam.c ~/oslab/hdc/usr/root/iam.c
sudo chown root:root ~/oslab/hdc/usr/root/iam.c

sudo cp whoami.c ~/oslab/hdc/usr/root/whoami.c
sudo chown root:root ~/oslab/hdc/usr/root/whoami.c

sudo cp testlab2.c ~/oslab/hdc/usr/root/testlab2.c
sudo chown root:root ~/oslab/hdc/usr/root/testlab2.c

sudo cp testlab2.sh ~/oslab/hdc/usr/root/testlab2.sh
sudo chown root:root ~/oslab/hdc/usr/root/testlab2.sh

sudo cp unistd.h ~/oslab/hdc/usr/include/unistd.h
sudo chown root:root ~/oslab/hdc/usr/include/unistd.h

sudo umount ~/oslab/hdc

# 编译操作系统，运行

cd ~/oslab/linux-0.11

make clean

make all

cd ..

./run
