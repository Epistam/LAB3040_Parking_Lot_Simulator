#include <stdio.h>
#include <stdlib.h>
#include <termios.h> // Terminal attributes
#include <sys/ioctl.h>
#include <unistd.h> // For STDIN_FILENO
#include <sys/ioctl.h> // For term interaction / sending flags to it 

int main(int argc, *char[] argv) {
	
	term_init();

	return EXIT_SUCCESS;
}
