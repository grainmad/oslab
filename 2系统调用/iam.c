#define __LIBRARY__
#include <unistd.h>
#include <errno.h>

_syscall1(int, iam, const char*, name)

int main(int argc, char *argv[]) {
    if (argc == 2) {
        int res = iam(argv[1]);
        /* printf("set myname: %s, status: %d, errno: %d\n", argv[1], res, errno); */
    } else {
        /* printf("argv error\n"); */
    }
    return 0;
}
