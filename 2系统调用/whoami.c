#define __LIBRARY__
#include <unistd.h>
#include <errno.h>

_syscall2(int, whoami, char*, name, unsigned int, size)

int main(void) {
    char tmp[24] = "\0";
    int res = whoami(tmp, 24);
    /* printf("myname: %s, status: %d, errno: %d\n",tmp, res, errno); */
    printf("%s",tmp);
    return 0;
}