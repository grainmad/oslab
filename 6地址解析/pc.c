#include<stdio.h>
#include<linux/sem.h>
#include<linux/shm.h>


#define SOURCE_N 500
#define HUB_N 20
#define THREAD_N 5
#define MEM_KEY 1234

int mem_id;

sem_t* mutex, *empty, *full;

int* share_mem;



void producer(int id) {
    sem_wait(empty);
    sem_wait(mutex);
    
    share_mem[id%HUB_N] = id;
    printf("prod %d: %d\n", getpid(), id);
    fflush(stdout);
    sem_post(mutex);
    sem_post(full);
}

int consumer() {
    int v;
    sem_wait(full);
    sem_wait(mutex);
    v = share_mem[share_mem[HUB_N]];
    printf("%d: %d\n", getpid(), v);
    share_mem[HUB_N] = (share_mem[HUB_N]+1)%HUB_N;
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

    for (i = 0; i < THREAD_N; i++) {
        proc_id_arr[i] = fork();
        if (proc_id_arr[i] < 0) {
            printf("Failed to fork child process %d!\n", i + 1);
            return -1;
        } else if (proc_id_arr[i] == 0) { 
            sem_wait(mutex);
            mem_id = shmget(MEM_KEY);
            share_mem = (int* ) shmat(mem_id);
            fflush(stdout);
            sem_post(mutex);
			
            while(consumer());
            
            sem_wait(mutex);
            shmdt(mem_id);
            sem_post(mutex);
            return 0;
        }
    }

    sem_wait(mutex);
    for (i = 0; i < THREAD_N; i++) {
        printf("Child PID: %d\n", proc_id_arr[i]);
        fflush(stdout);
    }
    mem_id = shmget(MEM_KEY);
    share_mem = (int* )shmat(mem_id);
    printf("mem_id %d, share_mem %X\n", mem_id, share_mem);
    fflush(stdout);
    sem_post(mutex);
    
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
    shmdt(mem_id);
    shmctl(mem_id);
    return 0;
}

