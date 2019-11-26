#include <stdio.h>
#include <stdlib.h>
#include <termios.h> // Terminal attributes
#include <sys/ioctl.h>
#include <unistd.h> // For STDIN_FILENO
#include <sys/ioctl.h> // For term interaction / sending flags to it 
#include "include/term.h"

void map_load(char *mapfile_path, char **map) {
	FILE *f_pointer = fopen(mapfile_path, "r");

}

void map_display(char **map) {

}
