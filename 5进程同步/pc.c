#include<stdio.h>
#include<linux/sem.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>



#define SOURCE_N 500
#define HUB_N 20
#define THREAD_N 5
sem_t* mutex, *empty, *full;
int fdw, fdr;

void producer(int id) {
    sem_wait(empty);
    sem_wait(mutex);
    if (id%HUB_N == 0) 
        lseek(fdw, 0, SEEK_SET);
    write(fdw, &id, 4);
    printf("prod %d: %d\n", getpid(), id);
    fflush(stdout);
    sem_post(mutex);
    sem_post(full);
}

int consumer() {
    int v;
    sem_wait(full);
    sem_wait(mutex);
    if (lseek(fdr, 0, SEEK_CUR) == 4*HUB_N)
        lseek(fdr, 0, SEEK_SET);
    read(fdr, &v, 4);
    printf("%d: %d\n", getpid(), v);
    fflush(stdout);
    sem_post(mutex);
    sem_post(empty);
    return v < SOURCE_N;
}

int main() {
    int i, t;
    int proc_id_arr[THREAD_N];
    mutex = sem_open("mutex", 1);
    empty = sem_open("empty", HUB_N);
    full = sem_open("full", 0);
    fdw = open("/usr/root/sem.txt", O_RDONLY|O_CREAT, 0644);
    fdr = open("/usr/root/sem.txt", O_WRONLY|O_CREAT, 0644);
    if (fdw == -1 || fdr == -1) {
        perror("open");
        return -1;
    }

    for (i = 0; i < THREAD_N; i++) {
        proc_id_arr[i] = fork();
        if (proc_id_arr[i] < 0) {
            printf("Failed to fork child process %d!\n", i + 1);
            return -1;
        } else if (proc_id_arr[i] == 0) { 
			while(consumer());
            return 0;
        }
    }

    for (i = 0; i < THREAD_N; i++) {
        printf("Child PID: %d\n", proc_id_arr[i]);
        fflush(stdout);
    }
    for (i=0; i<SOURCE_N+THREAD_N; i++) {
        producer(i);
    }
	for (i = 0; i < THREAD_N; i++) {
        t=waitpid(proc_id_arr[i], NULL, 0);
		printf("Child PID: %d, ws: %d\n", proc_id_arr[i], t);
        fflush(stdout);
    }
    sem_unlink("mutex");
    sem_unlink("full");
    sem_unlink("empty");
    close(fdw);
    close(fdr);
    return 0;
}

