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
#include "include/map.h"

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
	car->models = car_models;
	car->orientation = orientation;
	car->pos = pos;
	car->speed = speed;
	car->elapsed_time = 0;
	car->status = 0;
	car->color = color;
	car->next_car = NULL;
	
	// Add to car_list
	if(*car_list == NULL) { // If car_list is null, then the current car becomes the first link of the chain
		*car_list = car;
		car->prev_car = NULL;
	} else { // If not, we parse through all of the chain to attach it to the last link
		Car_t* car_buf = *car_list;
		while(car_buf->next_car != NULL) {
			car_buf = car_buf->next_car;
		}
		car->prev_car = car_buf;
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
	fprintf(fd, "\tPrevious car ID : ");
	if(car->prev_car == NULL) fprintf(fd, "Has NO prev car\n");
	else fprintf(fd, "Has prev car\n");
	fprintf(fd, "\tNext car ID : ");
	if(car->next_car == NULL) fprintf(fd, "Has NO next car\n");
	else fprintf(fd, "Has next car\n");

	fclose(fd);
}

void car_spawn(Car_t** car_list, char **map, char **fg_colormap, Vect_2di_t* pos, int color, int orientation) {

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
				map[lin+car->pos->y - 1][col+car->pos->x - 2] = 'C';
			}
			break;
		case 2: 
			for(lin = 0 ; lin < CAR_LENGTH ; lin++) for(col = 0 ; col < CAR_WIDTH ; col++) {
				map[lin+car->pos->y - 2][col+car->pos->x - 1] = 'C';
			}
			break;
		case 3: 
			for(lin = 0 ; lin < CAR_WIDTH ; lin++) for(col = 0 ; col < CAR_LENGTH ; col++) {
					map[lin+car->pos->y - 1][col+car->pos->x - 1] = 'C';
			}
			break;
	}
	// Mark the center of the car for debugging purposes
	map[car->pos->y][car->pos->x] = '0';


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

int car_ahead(char **map, Car_t* car) {
	char fwd_cell;
	int lin,col;


/*	switch(car->orientation) { // CAN CAUSE DEADLOCK 
		case 0: 
			for(lin = 0 ; lin < CAR_LENGTH ; lin++) {
				for(col = 0 ; col < CAR_WIDTH + 2 ; col++) {
					fwd_cell = map[lin+car->pos->y - 5][col+car->pos->x - 2];

					if(fwd_cell == 'C' || fwd_cell == '0') return 1;
				}
			}

			break;
		case 1: 
			for(lin = 0 ; lin < CAR_WIDTH + 2 ; lin++) {
				for(col = 0 ; col < CAR_LENGTH ; col++) {
					fwd_cell = map[lin+car->pos->y - 2][col+car->pos->x + 3];
					if(fwd_cell == 'C' || fwd_cell == '0') return 1;
				}
			}
			break;
		case 2: 

			for(lin = 0 ; lin < CAR_LENGTH ; lin++) {
				for(col = 0 ; col < CAR_WIDTH + 2; col++) {
					fwd_cell = map[lin+car->pos->y + 3][col+car->pos->x - 2];
					if(fwd_cell == 'C' || fwd_cell == '0') return 1;
				}
			}

			break;

		case 3: 
			for(lin = 0 ; lin < CAR_WIDTH + 2 ; lin++) { 
				for(col = 0 ; col < CAR_LENGTH ; col++) {
					fwd_cell = map[lin+car->pos->y - 2][col+car->pos->x - 5];
					if(fwd_cell == 'C' || fwd_cell == '0') return 1;
				}
			}
			break;
	}*/


	switch(car->orientation) {
		case 0: 
			for(lin = 0 ; lin < CAR_LENGTH ; lin++) {
				for(col = 0 ; col < CAR_WIDTH ; col++) {
					fwd_cell = map[lin+car->pos->y - 5][col+car->pos->x - 1];

					if(fwd_cell == 'C' || fwd_cell == '0') return 1;
				}
			}

			break;
		case 1: 
			for(lin = 0 ; lin < CAR_WIDTH  ; lin++) {
				for(col = 0 ; col < CAR_LENGTH ; col++) {
					fwd_cell = map[lin+car->pos->y - 1][col+car->pos->x + 3];
					if(fwd_cell == 'C' || fwd_cell == '0') return 1;
				}
			}
			break;
		case 2: 

			for(lin = 0 ; lin < CAR_LENGTH ; lin++) {
				for(col = 0 ; col < CAR_WIDTH ; col++) {
					fwd_cell = map[lin+car->pos->y + 3][col+car->pos->x - 1];
					if(fwd_cell == 'C' || fwd_cell == '0') return 1;
				}
			}

			break;

		case 3: 
			for(lin = 0 ; lin < CAR_WIDTH ; lin++) { 
				for(col = 0 ; col < CAR_LENGTH ; col++) {
					fwd_cell = map[lin+car->pos->y - 1][col+car->pos->x - 5];
					if(fwd_cell == 'C' || fwd_cell == '0') return 1;
				}
			}
			break;
	}

	return 0;
}

void car_goahead(Car_t* car) {
	switch(car->orientation) {
		case 0: 
			car->pos->y -= 1;
			break;
		case 1: 
			car->pos->x += 1;
			break;
		case 2: 
			car->pos->y += 1;
			break;
		case 3: 
			car->pos->x -= 1;
			break;
	}
}
void car_goback(Car_t* car) {
	switch(car->orientation) {
		case 0: 
			car->pos->y += 1;
			break;
		case 1: 
			car->pos->x -= 1;
			break;
		case 2: 
			car->pos->y -= 1;
			break;
		case 3: 
			car->pos->x += 1;
			break;
	}
}

void car_step(char **orig_map, char **map, char **fg_colormap, Car_t* car, Car_t** car_list) { // Single atomic step for a single car
	// Start out by removing the old car
	car_remove(orig_map, map, fg_colormap, car); 

	char current_cell;
	if(car->status == 0 || car->status == 4) {
		switch(car->orientation) {
			case 0: 
				current_cell = orig_map[car->pos->y-1][car->pos->x];
				// Check for turns
				if(current_cell == 'W') car->orientation = 3;
				if(current_cell == 'E') car->orientation = 1;
				car->pos->y -= 1; // Keep going no matter what, but depending on next cell, orientation might change
				if(car->orientation == 1) car->pos->x += 1;
				if(car->orientation == 3) car->pos->x -= 1;

				// Check ahead to avoid collisions
				if(car_ahead(map, car)) car->speed = 0;
				else car->speed = 1;

				break;
			case 1: 
				current_cell = orig_map[car->pos->y][car->pos->x+1];
				if(current_cell == 'N') car->orientation = 0;
				if(current_cell == 'S') car->orientation = 2;
				car->pos->x += 1;
				if(car->orientation == 0) car->pos->y -= 1;
				if(car->orientation == 2) car->pos->y += 1;

				// Check ahead to avoid collisions
				if(car_ahead(map, car)) car->speed = 0;
				else car->speed = 1;

				break;
			case 2: 
				current_cell = orig_map[car->pos->y+1][car->pos->x];
				if(current_cell == 'E') car->orientation = 1;
				if(current_cell == 'W') car->orientation = 3;
				car->pos->y += 1;
				if(car->orientation == 1) car->pos->x += 1;
				if(car->orientation == 3) car->pos->x -= 1;

				// Check ahead to avoid collisions
				if(car_ahead(map, car)) car->speed = 0;
				else car->speed = 1;

				break;
			case 3: 
				current_cell = orig_map[car->pos->y][car->pos->x-1];
				if(current_cell == 'S') car->orientation = 2;
				if(current_cell == 'N') car->orientation = 0;
				car->pos->x -= 1;
				if(car->orientation == 0) car->pos->y -= 1;
				if(car->orientation == 2) car->pos->y += 1;

				// Check ahead to avoid collisions
				if(car_ahead(map, car)) car->speed = 0;
				else car->speed = 1;

				break;
			default: 
				current_cell = 0; // Leave me alone gcc :'(

		}
		// Handling special cells
		if(current_cell == 'X' || current_cell == 'Y') { // Divergence and convergence
			// Create a list of available options
			int i=0;
			char* available_options = malloc(3*sizeof(char)); // On ne peut pas avoir plus de 3 chemins partant d'un noeud
			if(orig_map[car->pos->y-1][car->pos->x] == 'N') {
				available_options[i] = 'N';
				i++;
			}
			if(orig_map[car->pos->y][car->pos->x+1] == 'E') {
				available_options[i] = 'E';
				i++;
			}
			if(orig_map[car->pos->y+1][car->pos->x] == 'S') {
				available_options[i] = 'S';
				i++;
			}
			if(orig_map[car->pos->y][car->pos->x-1] == 'W') {
				available_options[i] = 'W';
				i++;
			}

			// Choose a random direction among available ones
			char choice = available_options[rand()%i];
			if(choice == 'N') {
				if(car->orientation != 0) car->pos->y -= 1;
				car->orientation = 0;
			}
			if(choice == 'E') {
				if(car->orientation != 1) car->pos->x += 1;
				car->orientation = 1;
			}
			if(choice == 'S') {
				if(car->orientation != 2) car->pos->y += 1;
				car->orientation = 2;
			}
			if(choice == 'W') {
				if(car->orientation != 3) car->pos->x -= 1;
				car->orientation = 3;
			}
		}
		// Engage parking animation (only if seeking parking and there are available slots)
		if(car->status != 4 && orig_map[car->pos->y][car->pos->x] == 'P' && (car->park_target = car_wheretopark(map, car)) != 0) {
			car->status = 1;
			car->animation_step = 0;
			// place C barrier (not even necessary in the end)
		}
	}

	// Car parking once engaged in aniamtion
	if(car->status == 1) car_park(map, car);
	if(car->status == 2) if(!(rand()%UNPARK_CHANCE)) car->status = 3; 
	if(car->status == 3) car_unpark(map, orig_map, car);
	
	// Car getting out (must be last)
	if(current_cell != 'O') car_commit(map, fg_colormap, car);
	else {
		if(car->prev_car == NULL && car->next_car == NULL) *car_list = NULL; // If car is the first and last car in the list
		else if(car->prev_car == NULL && car->next_car != NULL) {
			*car_list = car->next_car;
			car->next_car->prev_car = NULL;
		}
		else if(car->prev_car != NULL && car->next_car == NULL) car->prev_car->next_car = NULL;
		else {
				car->prev_car->next_car = car->next_car;
				car->next_car->prev_car = car->prev_car;
		}
		car = NULL;
	}

}

void car_park(char **map, Car_t* car) {
		switch(car->animation_step) {
			case 1:
				car_goahead(car);
				break;
			case 2:
				car_goahead(car);
				break;
			case 3:
				car_goahead(car);
				break;
			case 4:
				car_goback(car);
				break;
			case 5:
				car_goback(car);
				break;
			case 6:
				car_goback(car);
				if(car->park_target == 'R') car->orientation = (car->orientation+4-1)%4;
				else car->orientation = (car->orientation+4+1)%4;
				car_goback(car);
				break;
			case 7:
				car_goback(car);
				break;
			case 8:
				car_goback(car);
				break;
			case 9:
				car_goback(car);
				break;
			case 10:
				car->animation_step = 0;
				car->status = 2;
				break;
		}
		car->animation_step += 1;

}
void car_unpark(char **map, char** orig_map, Car_t* car) {
	char departure_direction = 0;
	switch(car->animation_step) {
		case 0:
			if(!car_ahead(map, car)) {
				car_goahead(car);
				car->animation_step = 1;
			}
			break;
		case 1:
			if(!car_ahead(map, car)) {
				car_goahead(car);
				car->animation_step = 2;
			}
			break;
		case 2:
			car_goahead(car);
			break;
		case 3:
			car_goahead(car);
			break;
		case 4:
			car_goahead(car);
			break;
		case 5:
			car_goahead(car);
			break;
		case 6:
			switch(car->orientation) {
				case 0: 
					departure_direction = orig_map[car->pos->y][car->pos->x-1];
					printf("AAAA : %c\n",departure_direction);
					break;
				case 1: 
					departure_direction = orig_map[car->pos->y-1][car->pos->x];
					break;
				case 2: 
					departure_direction = orig_map[car->pos->y][car->pos->x+1];
					break;
				case 3: 
					departure_direction = orig_map[car->pos->y+1][car->pos->x];
					break;
			}
			switch(departure_direction) {
				case 'N':
					car->orientation = 0;
					break;
				case 'E':
					car->orientation = 1;
					break;
				case 'S':
					car->orientation = 2;
					break;
				case 'W':
					car->orientation = 3;
					break;
			}
			car_goahead(car);
			break; 
		case 7:
			car_goahead(car);
			car->animation_step = 0;
			car->status = 4;
			break;
	}
	if(car->animation_step > 1) car->animation_step += 1; // step 0 and 1 are used to wait until the way is clear
}

char car_wheretopark(char** map, Car_t* car) {

	switch(car->orientation) {
		case 0: 
			// On repère la lampe
			if(map[car->pos->y][car->pos->x-3] != 'C' && map[car->pos->y][car->pos->x-7] == 'L') return 'L';
			if(map[car->pos->y][car->pos->x+3] != 'C' && map[car->pos->y][car->pos->x+7] == 'L') return 'R';
			break;
		case 1: 
			if(map[car->pos->y-3][car->pos->x] != 'C' && map[car->pos->y-7][car->pos->x] == 'L') return 'L';
			if(map[car->pos->y+3][car->pos->x] != 'C' && map[car->pos->y+7][car->pos->x] == 'L') return 'R';
			break;
		case 2: 
			if(map[car->pos->y][car->pos->x+3] != 'C' && map[car->pos->y][car->pos->x+7] == 'L') return 'L';
			if(map[car->pos->y][car->pos->x-3] != 'C' && map[car->pos->y][car->pos->x-7] == 'L') return 'R';
			break;
		case 3: 
			if(map[car->pos->y+3][car->pos->x] != 'C' && map[car->pos->y+7][car->pos->x] == 'L') return 'L';
			if(map[car->pos->y-3][car->pos->x] != 'C' && map[car->pos->y-7][car->pos->x] == 'L') return 'R';
			break;
	}
	return 0;
}

void cars_update(char **orig_map, char** map, char **fg_colormap, Car_t** car_list, Vect_2di_t* map_size) { // interpolate steps with speed to skip steps, and this for eveyr car
	if(*car_list != NULL) { // If car_list is empty, there's nothing to update
		Car_t *current_car = *car_list; 
		
		int i;
		while(current_car != NULL) {
			// Skip as many steps as speed, and update the car itself and its location in the map
			current_car->elapsed_time++;
			if(!car_ahead(map, current_car)) current_car->speed = 1;
			for(i = 0 ; i < current_car->speed ; i++) car_step(orig_map, map, fg_colormap, current_car, car_list); 
			if(current_car != NULL) current_car = current_car->next_car;
		}
		//car_spawner(orig_map, map, fg_colormap, car_list, map_size); // Try to spawn a car
		if(!(rand()%SPAWN_CHANCE)) car_spawn(car_list, map, fg_colormap, vect_2di_init(53,37), 9, 3); // Just works(tm)
	}
}

// Non functionnal, TODO debug
void car_spawner(char** orig_map, char** map, char** fg_colormap, Car_t** car_list, Vect_2di_t* map_size) {
	FILE* log = fopen("log.log", "a");

	Vect_2di_t* spawn_point = map_get_spawnpoint(map, map_size);

	fprintf(log,"%d, %d\n", spawn_point->x, spawn_point->y);

	char* neighbors = malloc(4*sizeof(char));
	neighbors[0] = map[spawn_point->y][spawn_point->x-1]; 
	neighbors[1] = map[spawn_point->y-1][spawn_point->x];
	neighbors[2] = map[spawn_point->y][spawn_point->x+1];
	neighbors[3] = map[spawn_point->y+1][spawn_point->x];

	int i;
	for(; neighbors[i] == ' '; i++) ;

	fprintf(log,"%c",neighbors[i]);




	
	int orientation;
	switch(neighbors[i]) {
		case 'N': 
			orientation = 0;
			break;
		case 'E': 
			orientation = 1;
			break;
		case 'S': 
			orientation = 2;
			break;
		case 'W': 
			orientation = 3;
			break;
	}
	fprintf(log,"orientation :%d\n",orientation);
	fprintf(log,"END\n");
	fclose(log);
	if(!(rand()%SPAWN_CHANCE)) {
		car_spawn(car_list, map, fg_colormap, spawn_point, rand()%256, 3); 
	}
}
