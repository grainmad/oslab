
#ifndef _SEM_H
#define _SEM_H

#include <linux/sched.h>

#define SEM_LEN 64

typedef struct
{
    char name[SEM_LEN];
    int value;
    struct task_struct *queue;
} sem_t;

#define __LIBRARY__
#include <unistd.h>

_syscall2(sem_t *, sem_open, const char *, name, unsigned int, value)
_syscall1(int, sem_wait, sem_t *, sem);
_syscall1(int, sem_post, sem_t *, sem);
_syscall1(int, sem_unlink, const char *, name);

#endif