
# 初始化环境
sudo rm -rf ~/oslab/*
cp -r /home/teacher/oslab/* ~/oslab/


# 内核代码修改

cp console.c ~/oslab/linux-0.11/kernel/chr_drv/console.c

cp keyboard.S ~/oslab/linux-0.11/kernel/chr_drv/keyboard.S


# 编译操作系统，运行

cd ~/oslab/linux-0.11

make clean

make all

cd ..

./run
