
#include <errno.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <stdarg.h>

int sprintf(char *buf, const char *fmt, ...)
{
    va_list args; int i;
    va_start(args, fmt);
    i=vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}

int get_psinfo(char* buf) {
	struct task_struct ** p;
	char* pre = buf;
	buf += sprintf(buf, "TaskList:\n\n");
	for(p = &LAST_TASK ; p >= &FIRST_TASK ; --p) if (*p) {
		buf += sprintf(buf, "pid=%d\nfather=%d\nstate=%d\npriority=%d\n", (*p)->pid, (*p)->father, (*p)->state, (*p)->priority);
		buf += sprintf(buf, "start_code=%d\nend_code=%d\nend_data=%d\nbrk=%d\nstart_stack=%d\n", (*p)->start_code, (*p)->end_code, (*p)->end_data, (*p)->brk, (*p)->start_stack);
		buf += sprintf(buf, "code_base=%d\ndata_base=%d\n\n", get_base((*p)->ldt[1]),get_base((*p)->ldt[2]));
	}
	return buf-pre;
}

int get_hdinfo(char* buf) {
	//return sprintf(buf, "get_hdinfo todo\n");
	struct super_block * sb;
	sb = get_super(current->root->i_dev);

	char* pre = buf;
	buf += sprintf(buf, "current dev %d supper node:\n\n", current->root->i_dev);
	buf += sprintf(buf, "s_ninodes=%d\n", sb->s_ninodes);
	buf += sprintf(buf, "s_nzones=%d\n", sb->s_nzones);
	buf += sprintf(buf, "s_imap_blocks=%d\n", sb->s_imap_blocks);
	buf += sprintf(buf, "s_zmap_blocks=%d\n", sb->s_zmap_blocks);
	buf += sprintf(buf, "s_firstdatazone=%d\n", sb->s_log_zone_size);
	buf += sprintf(buf, "s_max_size=%d\n", sb->s_max_size);
	buf += sprintf(buf, "s_magic=%d\n", sb->s_magic);
	
	return buf-pre;
}

void read_dir(struct m_inode* dir, char** buf) {
	int entries;
	int block,i;
	struct buffer_head * bh;
	struct dir_entry * de;

	if (!(block = dir->i_zone[0]))
		return ;
	if (!(bh = bread(dir->i_dev,block)))
		return ;
	entries = dir->i_size / (sizeof (struct dir_entry));
	*buf += sprintf(*buf, "total entries %d\n", entries);
	i = 0;
	de = (struct dir_entry *) bh->b_data;
	while (i < entries) {
		if ((char *)de >= BLOCK_SIZE+bh->b_data) {
			brelse(bh);
			bh = NULL;
			if (!(block = bmap(dir,i/DIR_ENTRIES_PER_BLOCK)) ||
			    !(bh = bread(dir->i_dev,block))) {
				i += DIR_ENTRIES_PER_BLOCK;
				continue;
			}
			de = (struct dir_entry *) bh->b_data;
		}
		*buf += sprintf(*buf, "%s\n", de->name);
		de++;
		i++;
	}
	brelse(bh);
}

int get_dirinfo(char* buf) {
	char* pre = buf;
	buf += sprintf(buf, "root dir info\n");
	struct m_inode * inode;
	if (!current->root || !current->root->i_count)
		panic("No root inode");
	if (!current->pwd || !current->pwd->i_count)
		panic("No cwd inode");
	inode = current->root;
	inode->i_count++;
	read_dir(inode, &buf);
	iput(inode);
	return buf-pre;
}


int proc_read(int dev, unsigned long * pos, char * buf, int count)
{
	char* info = (char*) malloc(2048);
	int info_len;
	int read = 0;
	switch (dev)
	{
	case 0:
		info_len = get_psinfo(info);
		break;
	case 1:
		info_len = get_hdinfo(info);
		break;
	case 2:
		info_len = get_dirinfo(info);
		break;
	default:
		break;
	}
	while (count-- && *pos < info_len) {
		put_fs_byte(info[(*pos)++], buf++);
		read++;
	}
	free(info);
	return read;

	return -EIO;
}
