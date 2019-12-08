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
	int status;
	int animation_step;
	int is_totalled; // Boolean for collisions
	Car_t* prev_car;
	Car_t* next_car;
};

void car_debug(Car_t* car);

int car_ahead(char **map, Car_t* car);

// Struct for body or just char array 
char* car_load_body(char* body_filepath);

unsigned int car_newid(Car_t** car_list);

Car_t* car_init(int orientation, Vect_2di_t* pos, int speed, int color, Car_t** car_list);

// Spawn a car into the car list with given initial 
// conditions and return its id + display it
int car_spawn(Car_t* *car_list, char** map, char **fg_colormap, Vect_2di_t* pos, int color, int orientation);

// Delete car from the car list
void car_delete(int car_id, Car_t** car_list);

// Erase car from the display
void car_erase(Car_t* car);

// Iterate the car movement for the car and the display
void car_move(Car_t* car);

void car_commit(char** map, char** fg_colormap, Car_t* car);
void car_remove(char** orig_map, char** map, char** fg_colormap, Car_t* car);
void car_step(char **orig_map, char **map, char **fg_colormap, Car_t* car, Car_t** car_list);
void cars_update(char **orig_map, char** map, char **fg_colormap, Car_t** car_list);

//void car_refresh_maps(old map, new map); // TODO
//void car_refresh_display(old map, new map); // Prints and erases -> on map


#endif
