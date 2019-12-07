#ifndef PARK_H
#define PARK_H

void diff_timespec(struct timespec *a, struct timespec *b, struct timespec *result);
int parking_loop();

#endif
