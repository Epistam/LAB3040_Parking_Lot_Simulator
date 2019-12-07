#ifndef MAP_H
#define MAP_H



char** map_init(Vect_2di_t *map_size);
Vect_2di_t* map_getsize(char *mapfile_path);
void map_load(char *mapfile_path, char **map);

void map_display(char **map, Vect_2di_t* map_size);

#endif
