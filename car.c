#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // For debugging purposes
#include <termios.h> // Terminal attributes
#include <sys/ioctl.h>
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

Car_t* car_init(int orientation, Vect_2di_t* pos, int speed, int color, Car_t* car_file) {
	
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
	// TODO id
	car->models = car_models;
	car->orientation = orientation;
	car->pos = pos;
	car->speed = speed;
	car->elapsed_time = 0;
	car->color = color;
	car->next_car = NULL;

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
	int i,j,k; 
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
