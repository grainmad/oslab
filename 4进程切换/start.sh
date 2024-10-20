
# 初始化环境
sudo rm -rf ~/oslab/*
cp -r /home/teacher/oslab/* ~/oslab/


# 内核代码修改

cp fork.c ~/oslab/linux-0.11/kernel/fork.c

cp system_call.s ~/oslab/linux-0.11/kernel/system_call.s

cp sched.c ~/oslab/linux-0.11/kernel/sched.c

cp sched.h ~/oslab/linux-0.11/include/linux/sched.h


# 编译操作系统，运行

cd ~/oslab/linux-0.11

make clean

make all

cd ..

./run
