#include <unistd.h>
#include <asm/segment.h>
#include <errno.h>

char _my_name[24] = "\0";
int _my_name_size = 0;

int sys_iam(const char* name)
{	
	int sz = 0;
	while (sz<24 && get_fs_byte(name+sz) != '\0') sz++;

	if (sz > 23) {
		return -(EINVAL);
	}
	for (_my_name_size = 0; _my_name_size<sz; _my_name_size++) {
		_my_name[_my_name_size] = get_fs_byte(name+_my_name_size);
	}
	_my_name[_my_name_size] = '\0';
	return _my_name_size;
}

int sys_whoami(char* name, unsigned int size)
{	
	if (_my_name_size > size) {
		return -(EINVAL);
	}
	size = 0;
	for (size=0; size<=_my_name_size; size++) {
		put_fs_byte(_my_name[size], name+size);
	}
	return _my_name_size;
}