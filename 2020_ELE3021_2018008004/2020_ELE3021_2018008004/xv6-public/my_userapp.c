#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

int
main(int argc, char *argv[]) {


	int fd1, fd2;
  char temp[] = "0123456789ABCDEFG";
	printf(1, "big files test\n");
	
  fd1 = open("file1", O_CREATE|O_RDWR);
	fd2 = open("file2", O_CREATE|O_RDWR);

	write(fd1, temp, 12);
	pwrite(fd2, temp, 12, 12);
	close(fd1);
	close(fd2);

	fd2 = open("file2", O_RDWR);

	pwrite(fd2, temp, 3,  100 - 12);
	pwrite(fd2, temp, 12, 255 - 12);
	char buf[100];
	int n = pread(fd2, buf, 12, 255);
	printf(1, "%d:%s\n", n, buf);
	close(fd1);
	exit();
}
