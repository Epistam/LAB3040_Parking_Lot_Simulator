#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h> // Terminal attributes
#include <unistd.h> // For STDIN_FILENO
#include <sys/ioctl.h> // For term interaction / sending flags to it 
#include <sys/signal.h> 
#include <fcntl.h> 
#include "include/const.h"
#include "include/struct.h"
#include "include/term.h"
#include "include/car.h"
#include "include/map.h"
#include "include/menu.h"
#include "include/parking.h"



// Système de map buffer ou on map que les changements et on affiche le truc

int main(int argc, char *argv[]) {

	// Setting up the terminal 
	term_init();
	term_clear();
	
	int cont = 1;
	int menu_option;
	
	while(cont) { 
		int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
		fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK); // Make read blocking again 
		menu_option = menu_loop();
		term_clear();

		switch(menu_option) {
			case 2: 
				cont = 0;
				break;
			case 0:
				parking_loop(); // TODO recover return value
				break;
				

		}

	}
	

	return EXIT_SUCCESS;
}
