#ifndef STRUCT_H
#define STRUCT_H

typedef struct Vect_2di {
	int x;
	int y;
} Vect_2di_t;

Vect_2di_t* vect_2di_init(int x, int y);

#endif
