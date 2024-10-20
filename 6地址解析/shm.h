
#ifndef _SHM_H
#define _SHM_H

#include <linux/sched.h>

#define SHM_LEN 64

typedef struct
{
    int key;
    unsigned long page;
} shm_t;

#define __LIBRARY__
#include <unistd.h>

_syscall1(int, shmget, int, key);
_syscall1(void*, shmat, int, id);
_syscall1(int, shmdt, int, id);
_syscall1(int, shmctl, int, id);
_syscall1(int, physic, int, addr);

#endif