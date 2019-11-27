#include <stdio.h>
#include <stdlib.h>
#include <termios.h> // Terminal attributes
#include <sys/ioctl.h>
#include <unistd.h> // For STDIN_FILENO
#include <sys/ioctl.h> // For term interaction / sending flags to it 
#include "include/term.h"
#include "include/struct.h"
#include "include/map.h"

void map_display(char **map) {

}

void showMap(int **tab, int width, int height){
	printf("\n");
	for (int j = 0 ; j < height ; j++){
		for (int i = 0 ; i < width ; i++){
			printf("%d", tab[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

char** map_init(Vect_2di_t *map_size) {
	int i;
	char **map = malloc((map_size->y)*sizeof(char*));
	
	for(i = 0 ; i < map_size->y ; i++) map[i] = malloc((map_size->x)*size);

	return map;
}

Vect_2di_t* map_getsize(char *mapfile_path) { // todo use readline at some point
	FILE *f_pointer = fopen(mapfile_path, "r");

	char c;
	int col,lin=0, maxwidth=0;
	for(col = 0 ; c = fgetc(f_pointer) != EOF ;) {
		if(c != '\n') col++;
		else {
			maxwidth = (col > maxwidth) ? col : maxwidth; // Get the max column count
			col = 0;
			lin++;
		}
	}

	fclose(f_pointer);

	return vect_2d_init(lin, maxwidth);
}

// Load the map from the file and put it in an intermediate file
void map_load(char *mapfile_path, char **map) {

	// Open the map
	FILE *f_pointer = fopen(mapfile_path, "r");

	// Initialize a blank map
	char **map = map_init(map_getsize(mapfile_path));

	char c;
	int col,lin=0;
	for(col = 0 ; c = fgetc(f_pointer) != EOF ; col++) {
		if(c != '\n') {
			map[lin][col] = c;
			col++;
		} else {
			col = 0;
			lin++;
		}
	}

	fclose(f_pointer);
}
