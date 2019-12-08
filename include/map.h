#ifndef MAP_H
#define MAP_H
// Initialize an empty map
char** map_init(Vect_2di_t *map_size); 
// Sets the whole map to a specified default (useful for
// color maps e.g)
void map_set(char **map, char value, Vect_2di_t* map_size);
// Copies the whole map's data into the 2nd map, assuming
// they're both initialized
void map_copy(char **map_orig, char** map_dest, Vect_2di_t *map_size);
// Get a mapfile's size
Vect_2di_t* map_getsize(char *mapfile_path); 
// Load a map from its mapfile
void map_load(char *mapfile_path, char **map); 

void map_display_debug(char **map, Car_t* car_list, Vect_2di_t* map_size);

void map_display(char **map, char** fg_colormap, char **bg_colormap, Car_t* car_list, Vect_2di_t* map_size);

#endif
