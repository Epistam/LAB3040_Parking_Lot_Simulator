#include <stdio.h>
#include <stdlib.h>
#include <termios.h> // Terminal attributes
#include <sys/ioctl.h>
#include <unistd.h> // For STDIN_FILENO
#include <sys/ioctl.h> // For term interaction / sending flags to it 
#include "include/const.h"
#include "include/struct.h"
#include "include/term.h"
#include "include/map.h"
#include "include/car.h"

// Système de map buffer ou on map que les changements et on affiche le truc

int main(int argc, char *argv[]) {
	
	term_init();

	term_clear();

//	Vect_2di_t *map_size = map_getsize("maaaap.txt");
//
//	char** map = map_init(map_size);
//	map_load("maaaap.txt", map);
//	
//	// Display array size
//	//printf("%d,%d\n",map_size->x,map_size->y);
//	
//	map_display(map, map_size);
//
//	// Map buffer à remplir avec la nouvelle map et faire les modifs dessus, puis commit sur l'ancienne après affcichage
//
//	/////////////////////////////////////////////:
	
	Car_t* car = car_init(2, vect_2di_init(10,30), 42, 12, NULL);

	car_debug(car);

	return EXIT_SUCCESS;
}
