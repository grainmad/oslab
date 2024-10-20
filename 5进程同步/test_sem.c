#include<stdio.h>
#include<linux/sem.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>

#define THREAD_N 4

int main() {

    sem_t* abc = sem_open("abc", 1);
    int i, t;
    int proc_id_arr[THREAD_N];
    for (i = 0; i < THREAD_N; i++) {
        proc_id_arr[i] = fork();
        if (proc_id_arr[i] < 0) {
            printf("Failed to fork child process %d!\n", i + 1);
            return -1;
        } else if (proc_id_arr[i] == 0) { 
			sem_wait(abc);
            printf("%d pid : %d\n", i, getpid());
            fflush(stdout);
            sem_post(abc);
            return 0;
        }
    }
	for (i = 0; i < THREAD_N; i++) {
        t=waitpid(proc_id_arr[i], NULL, 0);
		printf("Child PID: %d, ws: %d\n", proc_id_arr[i], t);
        fflush(stdout);
    }
    printf("father pid : %d\n", getpid());

    sem_unlink("abc");
    
    
    return 0;
}

