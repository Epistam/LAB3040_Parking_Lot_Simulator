#include <stdio.h>
#include <stdlib.h>
#include "include/struct.h"

// Create a vector and initialize it from two integer values
Vect_2di_t* vect_2di_init(int x, int y) {
	
	Vect_2di *v2d = malloc(2*sizeof(int));

	v2d->x = x;
	v2d->y = y;

	return v2d;
}
