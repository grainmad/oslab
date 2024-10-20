信号量内核核心实现
``` c
typedef struct
{
    char name[SEM_LEN];
    int value;
    struct task_struct *queue;
} sem_t;


int sys_sem_wait(sem_t *sem) {
	/* printk("[sem_wait] before | %s %d\n", sem->name, sem->value); */
	cli();
	while (sem->value <= 0) 
		sleep_on(&sem->queue);
	sem->value--;
	sti();
	/* printk("[sem_wait] end | %s %d\n", sem->name, sem->value); */
	return 0;
}
int sys_sem_post(sem_t *sem) {
	/* printk("[sem_post] start | %s %d\n", sem->name, sem->value); */
	cli();
	if (++sem->value  <= 1) 
		wake_up(&sem->queue);
	sti();
	/* printk("[sem_post] end | %s %d\n", sem->name, sem->value); */
	return 0;
}
```
原理：阻塞进程在各自的内核栈中保存。被唤醒的进程会唤醒前一个被阻塞的进程。后阻塞的先被唤醒，类似一个链式栈。


pc.c 共生成500个产品，当前存在产品个数不超过20个，消费者为5个。

res.txt 运行pc.c的结果。实际的效果，只有两个消费者交替消费，因为每次都是唤醒最近阻塞的消费者。存在进程在阻塞队列中一直未唤醒。

