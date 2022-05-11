#include <stdio.h>	/* printf */
#include <stdlib.h>	/* malloc, atoi, rand... */
#include <string.h>	/* memcpy, strlen... */
#include <stdint.h>	/* uints types */
#include <sys/types.h>	/* size_t ,ssize_t, off_t... */
#include <unistd.h>	/* close() read() write() */
#include <fcntl.h>	/* open() */
#include <sys/ioctl.h>	/* ioctl() */
#include <errno.h>	/* error codes */

#include "ioctl_cmds.h"

int main(int argc, char** argv)
{
	int fd, len, retval;
	char buf[255];
	char cmd;

	if (argc < 2) {
		printf("Syntax: %s <device file path>\n", argv[0]);
		return -EINVAL;
	}

	if ((fd = open(argv[1], O_RDWR)) < 0) {
		fprintf(stderr, "Error opening file %s\n", argv[1]);
		return -EBUSY;
	}
	unsigned int data = 0;
	unsigned int data1 = 0x79404040;
	unsigned int data2 = 0x40404079;
	unsigned int data3 = 0xFFFFFFFF;
	unsigned int data4 = 0xFFFFFF55;

	ioctl(fd, WR_R_DISPLAY);
	retval = write(fd, &data2, sizeof(data));
	ioctl(fd, WR_L_DISPLAY);
	retval = write(fd, &data2, sizeof(data));
	ioctl(fd, WR_RED_LEDS);
	retval = write(fd, &data3, sizeof(data));
	ioctl(fd, WR_GREEN_LEDS);
	retval = write(fd, &data4, sizeof(data));

	ioctl(fd, RD_SWITCHES);
	read(fd, &data, 1);
	printf("new data: 0x%X\n", data);
	ioctl(fd, RD_PBUTTONS);
	read(fd, &data, 1);
	printf("new data: 0x%X\n", data);

	close(fd);

	return 0;
}
