#include <stdio.h>
#include <stdlib.h>
#include <termios.h> // Terminal attributes
#include <sys/ioctl.h>
#include <unistd.h> // For STDIN_FILENO
#include <sys/ioctl.h> // For term interaction / sending flags to it 
#include "include/term.h"
#include "include/const.h"
#include "include/struct.h"
#include "include/map.h"

char** map_init(Vect_2di_t *map_size) {
	int i;
	char** map = malloc((map_size->y)*sizeof(char*));
	
	for(i = 0 ; i < map_size->y ; i++) map[i] = malloc((map_size->x)*sizeof(char));

	return map;
}

Vect_2di_t* map_getsize(char *mapfile_path) { // todo use readline at some point
	FILE* f_pointer = fopen(mapfile_path, "r");

	char c;
	int col,lin=0, maxwidth=0;
	for(col = 0 ; (c = fgetc(f_pointer)) != EOF ;) {
		if(c != '\n') col++;
		else {
			maxwidth = (col > maxwidth) ? col : maxwidth; // Get the max column count
			col = 0;
			lin++;
		}
	}

	fclose(f_pointer);

	return vect_2di_init(maxwidth, lin);
}

// Load the map from the file and put it in an intermediate memory representation
void map_load(char *mapfile_path, char **map) {

	Vect_2di_t* size = map_getsize(mapfile_path);

	// Open the map
	FILE *f_pointer = fopen(mapfile_path, "r");

	char c;
	int col=0,lin=0;
	for(; (c = fgetc(f_pointer)) != EOF ;) {
		if(c != '\n') {
			map[lin][col] = c;
			col++;
		} else {
			for(; col < size->x ; col++) map[lin][col] = ' '; // Since the table is normalized, fill in the holes if they exist
			col = 0; // Reset column for next line
			lin++;
		}
	}

	fclose(f_pointer);
}

// TODO : 
// - DONE fix le délire de l'affichage caché + x/y size
// - DONE récupérer winsize et définir un coin de référence, drop le \n
// - gérer les doubles char pour fillup l'espace
void map_display(char **map, Vect_2di_t* map_size) {

	int col,lin;
	for(lin = 0 ; lin < map_size->y ; lin++) {
		termGoto(0,lin); // Line number is actually the y axis
		for(col = 0 ; col < map_size->x ; col++) {
			if(map[lin][col] == ' ') {
				termFwd();
				termFwd();
			}
			else {
				if(map[lin][col] == 'O') { // Si T horizontal, identifier sur barre à droite ou à gauche pour sélectionner si espace de droite ou espace de gauche
					printf("\u2588"); // Use only ASCII on map so the alignment is correct on the intermdiate table, but display unicode
					printf("\u2588");
				}
				else {
					printf("%c",map[lin][col]);
					printf("%c",map[lin][col]);
				}
			}
		}
	}
}
