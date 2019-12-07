#ifndef MAP_H
#define MAP_H
// Initialize an empty map
char** map_init(Vect_2di_t *map_size); 
// Get a mapfile's size
Vect_2di_t* map_getsize(char *mapfile_path); 
// Load a map from its mapfile
void map_load(char *mapfile_path, char **map); 

void map_display(char **map, Car_t* car_list, Vect_2di_t* map_size);

#endif
