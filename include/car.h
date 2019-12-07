#ifndef CAR_H
#define CAR_H

#define CAR_WIDTH 3
#define CAR_LENGTH 4

// Typedef the struct as car_t instead of putting it the end so the type is declared before the struct
typedef struct Car Car_t;

struct Car {
	unsigned int id;
	// Each body corresponds to an orientation and 
	// a string is used to avoid problems when rotating 
	// the model;
	char** models; 
	char orientation;
	Vect_2di_t* pos;
	int speed;
	// char type;
	int color; // Shell codes based color
	int elapsed_time;
	Car_t* next_car;
};

void car_debug(Car_t* car);

// Struct for body or just char array 
char* car_load_body(char* body_filepath);

unsigned int car_newid(Car_t* car_file);

Car_t* car_init(int orientation, Vect_2di_t* pos, int speed, int color, Car_t* car_file);

// Spawn a car into the car list with given initial 
// conditions and return its id + display it
int spawn_car(char** map, Car_t *car_list, Vect_2di_t* pos, int color);

// Delete car from the car list
void car_delete(int car_id, Car_t* car_list);

// Display car at its current position
void car_display(Car_t* car);

// Erase car from the display
void car_erase(Car_t* car);

// Iterate the car movement for the car and the display
void car_move(Car_t* car);


#endif
