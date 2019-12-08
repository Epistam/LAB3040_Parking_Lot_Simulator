#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // For debugging purposes
#include <termios.h> // Terminal attributes
#include <unistd.h> // For STDIN_FILENO
#include <sys/ioctl.h> // For term interaction / sending flags to it 
#include "include/term.h"
#include "include/const.h"
#include "include/struct.h"
#include "include/car.h"

// use orientation on display and map buffer integraiton

// Struct for body or just char array 
char* car_load_body(char *model_filepath) {
		
		// Allocate the string for the model
		char *car_model = malloc((CAR_WIDTH*CAR_LENGTH+1)*sizeof(char)); // Leave space for the '\0' terminator
		
		// Open the model
		FILE *f_pointer = fopen(model_filepath, "r");
		
		int curs = 0; // Reset cursor to 0
		char c;
		for(; (c = fgetc(f_pointer)) != EOF ;) { // Parse the string
			if(curs > (CAR_WIDTH*CAR_LENGTH)) { // Handle malformed model
				puts("Error : malformed car model file.");
				exit(-1);
			}
			if(c != '\n') { // Ommit newlines
				car_model[curs] = c;
				curs++;
			}
		}
		// Add the string terminator
		car_model[curs] = '\0';
		
		// Close the model cleanly
		fclose(f_pointer);

		return car_model;
}

Car_t* car_init(int orientation, Vect_2di_t* pos, int speed, int color, Car_t** car_list) {
	
	// Initialize the model paths from the constants
	char** car_model_paths = malloc(4*sizeof(char*));
	car_model_paths[0] = strcat(strdup(ASSETS_PREF), strdup(CAR_U));
	car_model_paths[1] = strcat(strdup(ASSETS_PREF), strdup(CAR_R));
	car_model_paths[2] = strcat(strdup(ASSETS_PREF), strdup(CAR_D));
	car_model_paths[3] = strcat(strdup(ASSETS_PREF), strdup(CAR_L));
	
	// Create the models array and fill it
	int i;
	char** car_models = malloc(4*sizeof(char*));
	for(i = 0 ; i < 4 ; i++) car_models[i] = car_load_body(car_model_paths[i]);
	
	// Filling up the struct
	Car_t* car = malloc(sizeof(Car_t));
	// TODO find ID from car_list
	car->models = car_models;
	car->orientation = orientation;
	car->pos = pos;
	car->speed = speed;
	car->elapsed_time = 0;
	car->color = color;
	car->next_car = NULL;
	
	// Add to car_list
	if(*car_list == NULL) { // If car_list is null, then the current car becomes the first link of the chain
		*car_list = car;
	} else { // If not, we parse through all of the chain to attach it to the last link
		Car_t* car_buf = *car_list;
		while(car_buf->next_car != NULL) car_buf = car_buf->next_car;
		car_buf->next_car = car;
	}

	return car;
}

void car_debug(Car_t* car) {
	FILE* fd = fopen(LOGFILE, "a");
	
	// Space from the previous record
	fprintf(fd, "\n\n");

	// time_t is arithmetic time type
	time_t now;
	time(&now);
	fprintf(fd, "Datetime : %s", ctime(&now));
	// Debugging start
	fprintf(fd, "Car ID : %d\n", car->id);
	fprintf(fd, "\tModels : \n");
	int i,j;
	for(i = 0 ; i < 4 ; i++) {
		fprintf(fd, "\t\tFacing direction %d :\n", i);

		if(!(i%2)) { // Odd number, which means up or down
			for(j = 0 ; j < CAR_LENGTH ; j++) {
				fprintf(fd, "\t\t\t");
				fprintf(fd, "%c", car->models[i][j*CAR_WIDTH]);
				fprintf(fd, "%c", car->models[i][j*CAR_WIDTH+1]);
				fprintf(fd, "%c\n", car->models[i][j*CAR_WIDTH+2]);
			}
		} else {
			for(j = 0 ; j < CAR_WIDTH ; j++) {
				fprintf(fd, "\t\t\t");
				fprintf(fd, "%c", car->models[i][j*CAR_LENGTH]);
				fprintf(fd, "%c", car->models[i][j*CAR_LENGTH+1]);
				fprintf(fd, "%c", car->models[i][j*CAR_LENGTH+2]);
				fprintf(fd, "%c\n", car->models[i][j*CAR_LENGTH+3]);
			}
		}

	}
	fprintf(fd, "\tOrientation : %d\n", car->orientation);
	fprintf(fd, "\tPosition : (%d,%d)\n", car->pos->x, car->pos->y);
	fprintf(fd, "\tSpeed : %d\n", car->speed);
	fprintf(fd, "\tColor : %d\n", car->color);
	fprintf(fd, "\tTime in parking : %d\n", car->elapsed_time);
	fprintf(fd, "\tNext car ID : ");
	if(car->next_car == NULL) fprintf(fd, "NULL\n");
	else fprintf(fd, "\t%d", car->next_car->id);

	fclose(fd);
}

int car_spawn(Car_t** car_list, char **map, char **fg_colormap, Vect_2di_t* pos, int color, int orientation) {

	int lin, col;

	// Takes car of struct allocation and car_list updating
	Car_t* car = car_init(orientation, pos, 1, color, car_list);
	
	// Update the maps
	switch(car->orientation) {
		case 0:  // TODO test this first
			for(lin = 0 ; lin < CAR_LENGTH ; lin++) for(col = 0 ; col < CAR_WIDTH ; col++) {
				map[lin+car->pos->y - 1][col+car->pos->x - 1] = 'C';
				fg_colormap[lin+car->pos->y - 1][col+car->pos->x - 1] = car->color;
			}
			break;
		case 1: 
			for(lin = 0 ; lin < CAR_WIDTH ; lin++) for(col = 0 ; col < CAR_LENGTH ; col++) {
				map[lin+car->pos->y - 1][col+car->pos->x - 1] = 'C';
			}
			break;
		case 2: 
			for(lin = 0 ; lin < CAR_LENGTH ; lin++) for(col = 0 ; col < CAR_WIDTH ; col++) {
				map[lin+car->pos->y - 2][col+car->pos->x - 1] = 'C';
			}
			break;
		case 3: 
			for(lin = 0 ; lin < CAR_WIDTH ; lin++) for(col = 0 ; col < CAR_LENGTH ; col++) {
					map[lin+car->pos->y - 1][col+car->pos->x - 2] = 'C';
			}
			break;
	}
	// Mark the center of the car for debugging purposes
	map[car->pos->y][car->pos->x] = '0';


	return car->id;	
}

// TODO function to delete something from the list

void car_commit(char** map, char** fg_colormap, Car_t* car) { // Commit a car to the maps
	int col, lin;
	// Update the maps
	switch(car->orientation) {
		case 0:  // TODO test this first
			for(lin = 0 ; lin < CAR_LENGTH ; lin++) for(col = 0 ; col < CAR_WIDTH ; col++) {
				map[lin+car->pos->y - 1][col+car->pos->x - 1] = 'C';
				fg_colormap[lin+car->pos->y - 1][col+car->pos->x - 1] = car->color;
			}
			break;
		case 1: 
			for(lin = 0 ; lin < CAR_WIDTH ; lin++) for(col = 0 ; col < CAR_LENGTH ; col++) {
				map[lin+car->pos->y - 1][col+car->pos->x - 2] = 'C';
				fg_colormap[lin+car->pos->y - 1][col+car->pos->x - 2] = car->color;
			}
			break;
		case 2: 
			for(lin = 0 ; lin < CAR_LENGTH ; lin++) for(col = 0 ; col < CAR_WIDTH ; col++) {
				map[lin+car->pos->y - 2][col+car->pos->x - 1] = 'C';
				fg_colormap[lin+car->pos->y - 2][col+car->pos->x - 1] = car->color;
			}
			break;
		case 3: 
			for(lin = 0 ; lin < CAR_WIDTH ; lin++) for(col = 0 ; col < CAR_LENGTH ; col++) {
				map[lin+car->pos->y - 1][col+car->pos->x - 1] = 'C';
				fg_colormap[lin+car->pos->y - 1][col+car->pos->x - 1] = car->color;
			}
			break;
	}
	// Mark the center of the car for debugging purposes
	map[car->pos->y][car->pos->x] = '0';
}
void car_remove(char** orig_map, char** map, char** fg_colormap, Car_t* car) { // Remove a car from the maps
	int lin, col;
	// Update the maps
	switch(car->orientation) {
		case 0:  // TODO test this first
			for(lin = 0 ; lin < CAR_LENGTH ; lin++) for(col = 0 ; col < CAR_WIDTH ; col++) {
				// Reset original map
				map[lin+car->pos->y - 1][col+car->pos->x - 1] = orig_map[lin+car->pos->y - 1][col+car->pos->x - 1];
				// Reset to default color
				fg_colormap[lin+car->pos->y - 1][col+car->pos->x - 1] = 15;
			}
			break;
		case 1: 
			for(lin = 0 ; lin < CAR_WIDTH ; lin++) for(col = 0 ; col < CAR_LENGTH ; col++) {
				map[lin+car->pos->y - 1][col+car->pos->x - 2] = orig_map[lin+car->pos->y - 1][col+car->pos->x - 2];
				fg_colormap[lin+car->pos->y - 1][col+car->pos->x - 2] = 15;
			}
			break;
		case 2: 
			for(lin = 0 ; lin < CAR_LENGTH ; lin++) for(col = 0 ; col < CAR_WIDTH ; col++) {
				map[lin+car->pos->y - 2][col+car->pos->x - 1] = orig_map[lin+car->pos->y - 2][col+car->pos->x - 1];
				fg_colormap[lin+car->pos->y - 2][col+car->pos->x - 1] = 15;
			}
			break;
		case 3: 
			for(lin = 0 ; lin < CAR_WIDTH ; lin++) for(col = 0 ; col < CAR_LENGTH ; col++) {
				map[lin+car->pos->y - 1][col+car->pos->x - 1] = orig_map[lin+car->pos->y - 1][col+car->pos->x - 1];
				fg_colormap[lin+car->pos->y - 1][col+car->pos->x - 1] = 15;
			}
			break;
	}
}

void car_step(char **orig_map, char **map, char **fg_colormap, Car_t* car) { // Single atomic step for a single car
	// Start out by removing the old car
	car_remove(orig_map, map, fg_colormap, car); 

	// TODO handle intersections and disappearances (rabouter la liste dans ce cas)
	// TODO fucked up map oreintation
	char next_orr;
	switch(car->orientation) {
		case 0: 
			if((next_orr = orig_map[car->pos->y-1][car->pos->x]) == 'W') car->orientation = 3;
			if((next_orr = orig_map[car->pos->y-1][car->pos->x]) == 'E') car->orientation = 1;
			car->pos->y -= 1; // Keep going no matter what, but depending on next cell, orientation might change
			break;
		case 1: 
			if((next_orr = orig_map[car->pos->y][car->pos->x+1]) == 'N') car->orientation = 0;
			if((next_orr = orig_map[car->pos->y][car->pos->x+1]) == 'S') car->orientation = 2;
			car->pos->x += 1;
			break;
		case 2: 
			if((next_orr = orig_map[car->pos->y+1][car->pos->x]) == 'E') car->orientation = 1;
			if((next_orr = orig_map[car->pos->y+1][car->pos->x]) == 'W') car->orientation = 3;
			car->pos->y += 1;
			break;
		case 3: 
			if((next_orr = orig_map[car->pos->y][car->pos->x-1]) == 'S') car->orientation = 2;
			if((next_orr = orig_map[car->pos->y][car->pos->x-1]) == 'N') car->orientation = 0;
			car->pos->x -= 1;
			break;
	}

	car->elapsed_time++;

	car_commit(map, fg_colormap, car);
	
}

void cars_update(char **orig_map, char** map, char **fg_colormap, Car_t** car_list) { // interpolate steps with speed to skip steps, and this for eveyr car
	if(*car_list != NULL) { // If car_list is empty, there's nothing to update
		Car_t *current_car = *car_list; 
		
		int i;
		while(current_car->next_car != NULL) {
			// Skip as many steps as speed, and update the car itself and its location in the map
			car_debug(current_car);
			for(i = 0 ; i < current_car->speed ; i++) car_step(orig_map, map, fg_colormap, current_car); 
			current_car = current_car->next_car;
		}
		// Last car with next_car == NULL
		car_debug(current_car);
		for(i = 0 ; i < current_car->speed ; i++) car_step(orig_map, map, fg_colormap, current_car); 
	}
}

// TODO make all cars go
// fix aligment problems with east and west
