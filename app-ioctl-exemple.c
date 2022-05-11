#include <stdio.h>	/* printf */
#include <stdlib.h>	/* malloc, atoi, rand... */
#include <string.h>	/* memcpy, strlen... */
#include <stdint.h>	/* uints types */
#include <sys/types.h>	/* size_t ,ssize_t, off_t... */
#include <unistd.h>	/* close() read() write() */
#include <fcntl.h>	/* open() */
#include <sys/ioctl.h>	/* ioctl() */
#include <errno.h>	/* error codes */
#include <omp.h>
#include <SDL2/SDL_mixer.h>

#define NotesNUM    4
#define DISPLAY_L   1
#define DISPLAY_R   2
#define SWITCHES    3
#define PUSHBUTTON  4
#define GREENLEDS   5
#define REDLEDS     6

#include "ioctl_cmds.h"

void LoadPiano(Mix_Chunk **Notes);
void LoadPianoStretched(Mix_Chunk **Notes);

int main(int argc, char** argv)
{
	int fd, len, retval;
	char buf[255];
	char cmd;

	Mix_Chunk *Notes[NotesNUM];
	Mix_Chunk *NotesStretched[NotesNUM];

	LoadPiano(Notes);
	LoadPianoStretched(NotesStretched);

	if (argc < 2) {
		printf("Syntax: %s <device file path>\n", argv[0]);
		return -EINVAL;
	}

	if ((fd = open(argv[1], O_RDWR)) < 0) {
		fprintf(stderr, "Error opening file %s\n", argv[1]);
		return -EBUSY;
	}

	while(1) {
		write(fpga, &led_1, GREENLEDS);
		write(fpga, &led_2, GREENLEDS);
		write(fpga, &led_3, GREENLEDS);
		write(fpga, &led_4, GREENLEDS);

		if(read(fpga, &switches_rd, SWITCHES) > 0){
			// red_something = 1;

			// Notas normais
			if(read(fpga, &pbuttons_rd, PUSHBUTTON)){
				if(pbuttons_rd == 14){
					Mix_PlayChannel(2, Notes[3], 0);
				}
				if(pbuttons_rd == 13){
					Mix_PlayChannel(2, Notes[2], 0);
				}
				if(pbuttons_rd == 11){
					Mix_PlayChannel(2, Notes[1], 0);
				}
				if(pbuttons_rd == 7){
					Mix_PlayChannel(2, Notes[0], 0);
				}
			}

		} else {
			// red_something = 0;

			// Notas alongadas
			if(read(fpga, &pbuttons_rd, PUSHBUTTON)){
				if(pbuttons_rd == 14){
					Mix_PlayChannel(2, NotesStretched[3], 0);
				}
				if(pbuttons_rd == 13){
					Mix_PlayChannel(2, NotesStretched[2], 0);
				}
				if(pbuttons_rd == 11){
					Mix_PlayChannel(2, NotesStretched[1], 0);
				}
				if(pbuttons_rd == 7){
					Mix_PlayChannel(2, NotesStretched[0], 0);
				}
			}
		}
		
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

void LoadPiano(Mix_Chunk **Notes) {
	Notes[0] = Mix_LoadWAV("Samples/do.wav");
    Notes[1] = Mix_LoadWAV("Samples/re.wav");
    Notes[2] = Mix_LoadWAV("Samples/mi.wav");
    Notes[3] = Mix_LoadWAV("Samples/fa.wav");
}

void LoadPianoStretched(Mix_Chunk **Notes) {
	Notes[0] = Mix_LoadWAV("Samples/do-stretched.wav");
    Notes[1] = Mix_LoadWAV("Samples/re-stretched.wav");
    Notes[2] = Mix_LoadWAV("Samples/mi-stretched.wav");
    Notes[3] = Mix_LoadWAV("Samples/fa-stretched.wav");
}
