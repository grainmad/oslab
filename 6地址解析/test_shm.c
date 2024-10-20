#include<stdio.h>
#include<linux/shm.h>


int mem_id;

int* share_mem;


int main() {

    mem_id = shmget(123);
    share_mem = (int* )shmat(mem_id);
    printf("mem_id %d, share_mem %X\n", mem_id, share_mem);
    fflush(stdout);
    physic(share_mem);
    share_mem[512] = 12;
    printf("mem[512] = %d\n", share_mem[512]);
    fflush(stdout);
    shmdt(mem_id);
    printf("shmdt\n");
    fflush(stdout);
    printf("mem[512] = %d\n", share_mem[512]);
    physic(share_mem);
    shmctl(mem_id);
    printf("shmctl\n");
    fflush(stdout);
    return 0;
}

