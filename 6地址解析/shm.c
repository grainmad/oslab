#include<linux/shm.h>
#include<linux/mm.h>

shm_t shms[SHM_LEN];

/* 创建一页共享内存(如果key不存在)，key为共享内存的标识符(非0)，返回共享内存结构体的位置 */
int sys_shmget(int key) {
    int i;
    for (i=0; i<SHM_LEN; i++) {
        if (shms[i].key == key) {
            return i;
        }
    }
    for (i=0; i<SHM_LEN; i++) {
        if (shms[i].key == 0) {
            shms[i].key = key;
            shms[i].page = get_free_page();
            /*
            */
            printk("physic addr %X\n", shms[i].page);
            return i;
        }
    }
    return -1;
}
/* 将shm_t[id] 中物理内存 绑定到当前进程的 数据段基址+brk偏移地址 上， brk初始大小=代码段+数据段+bss段， brk大小加一页*/
void* sys_shmat(int id) {
    unsigned long p = current->start_code + current->brk;
    current->brk += PAGE_SIZE;
    /*
    */
    printk("start put page %X to addr %X\n", shms[id].page, p);
    put_page(shms[id].page, p);
    return (void *) current->brk - PAGE_SIZE; /*返回数据段的偏移量*/
}
/* 解绑定当前进程共享内存的虚拟地址，调用前确保之前有绑定*/
int sys_shmdt(int id) {
    unsigned long *pt, p = current->start_code + (current->brk -= PAGE_SIZE);
    /*
    printk("virtual %X\n", p);
    */
    pt = (unsigned long *) ((p>>20) & 0xffc); /* pt页表目录项指针*/
    /*
    printk("page dir item addr %X\n", pt);
    */
    pt = (unsigned long *) (0xfffff000 & *pt); /* pt页表首地址 */
    /*
    printk("page table addr %X\n", pt);
    printk("unlink addr %X\n", pt[(p>>12)&0x3ff]);
    */
    pt[(p>>12)&0x3ff] = 0; /*将对应项的 物理页首地址 置为空，这一页的虚拟地址不再映射物理地址*/
    return 0;
}

/* 删除共享内存 */
int sys_shmctl(int id) {
    if (shms[id].key != 0) {
        shms[id].key = 0;
        /*
        printk("start free page\n");
        */
        free_page(shms[id].page);
        shms[id].page = 0;
    }
    return 0;
}

int sys_physic(int addr) {
    unsigned long *pt, p = current->start_code + addr;
    /*
    */
    printk("virtual %X\n", p);
    pt = (unsigned long *) ((p>>20) & 0xffc); /* pt页表目录项指针*/
    /*
    */
    printk("page dir item addr %X\n", pt);
    pt = (unsigned long *) (0xfffff000 & *pt); /* pt页表首地址 */
    /*
    */
    printk("page table addr %X\n", pt);
    printk("physic addr %X\n", pt[(p>>12)&0x3ff]);
    return 0;
}