#include <linux/sem.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <linux/kernel.h>


sem_t sem_list[SEM_LEN];
int sem_usd[SEM_LEN];

void u2k_strcpy(const char *src, char *dst) {
	int i = 0;
	while (get_fs_byte(src+i) != '\0') {
		dst[i] = get_fs_byte(src+i);
		i++;
	}
	dst[i] = '\0';
}

int u2k_strcmp(const char *src, char *dst) {
	int i = 0;
	while (get_fs_byte(src+i) != '\0') {
		if (get_fs_byte(src+i) < dst[i]) return -1;
		if (get_fs_byte(src+i) > dst[i]) return 1;
		i++;
	}
	if (get_fs_byte(src+i) < dst[i]) return -1;
	if (get_fs_byte(src+i) > dst[i]) return 1;
	return 0;
}

sem_t *sys_sem_open(const char *name, unsigned int value) {
	int i;
	for (i=0; i<SEM_LEN && get_fs_byte(name+i) != '\0'; i++);
	if (i%SEM_LEN == 0) return NULL;

	for (i=0; i<SEM_LEN; i++) {
		if (sem_usd[i] && u2k_strcmp(name, sem_list[i].name) == 0) {
			/* printk("[sem_open] has old sem | %s %d\n", sem_list[i].name, sem_list[i].value); */
			return (sem_t *) &sem_list[i];
		}
	}

	for (i=0; i<SEM_LEN; i++) {
		if (!sem_usd[i]) {
			sem_usd[i] = 1;
			u2k_strcpy(name, sem_list[i].name);
			sem_list[i].value = value;
			/* printk("[sem_open] create new sem | %s %d\n", sem_list[i].name, sem_list[i].value); */
			return (sem_t *) &sem_list[i];
		}
	}
	return NULL;
}

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
int sys_sem_unlink(const char *name) {
	int i;
	for (i=0; i<SEM_LEN && get_fs_byte(name+i) != '\0'; i++);
	if (i%SEM_LEN == 0) return -1;

	for (i=0; i<SEM_LEN; i++) {
		if (sem_usd[i] && u2k_strcmp(name, sem_list[i].name) == 0) {
			/* printk("[sem_unlink] success | %s %d\n", sem_list[i].name, sem_list[i].value); */
			sem_usd[i] = 0;
			sem_list[i].name[0] = '\0';
			return 0;
		}
	}
	return -1;
}