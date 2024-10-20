#include<stdio.h>
#include<linux/sem.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>

#define THREAD_N 4

int main() {

    sem_t* abc = sem_open("abc", 0);
    int pid = fork();
    if (pid < 0) {
        printf("Failed to fork child process!\n");
        return -1;
    } else if (pid == 0) { 
        printf("sub pid : %d\n", getpid());
        sem_post(abc);
        return 0;
    }
    sem_wait(abc);
    printf("father pid : %d\n", getpid());
    sem_unlink("abc");
    
    return 0;
}

