#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h> // Terminal attributes
#include <unistd.h> // For STDIN_FILENO
#include <sys/ioctl.h> // For term interaction / sending flags to it 
#include <sys/signal.h> 
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
	/*
	while(cont) { 
		menu_option = menu_loop();

		switch(menu_option) {
			case 42: 
				cont = 0;
				break;
			case 69:
				parking_loop(); // TODO recover return value
				break;
				

		}

	}
	*/
	parking_loop();




	return EXIT_SUCCESS;
}
