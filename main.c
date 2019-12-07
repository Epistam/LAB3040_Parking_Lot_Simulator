#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h> // Terminal attributes
#include <unistd.h> // For STDIN_FILENO
#include <sys/ioctl.h> // For term interaction / sending flags to it 
#include <sys/types.h> 
#include <sys/signal.h> 
#include <sys/select.h> 
#include "include/const.h"
#include "include/struct.h"
#include "include/term.h"
#include "include/map.h"
#include "include/car.h"

void diff_timespec(struct timespec *a, struct timespec *b, struct timespec *result) { 
	long long int buf_a = (a->tv_sec)*1000000000 + a->tv_nsec;
	long long int buf_b = (b->tv_sec)*1000000000 + b->tv_nsec;
	long long int buf_result = (buf_b > buf_a ? buf_b - buf_a : buf_a - buf_b);

	result->tv_sec = buf_result/1000000000;
	result->tv_nsec = buf_result%1000000000;
}

// Système de map buffer ou on map que les changements et on affiche le truc

int main(int argc, char *argv[]) {

	// Setting up the terminal 
	term_init();
	term_clear();
	winsize_t ws;

	// Create file descriptors set
	fd_set set;

	sigset_t sig_mask;
	sigemptyset(&sig_mask);
	
	// Define the time structs used to keep track in the loop (timespec uses s, ns and timeval uses s, ms)
	struct timespec frame_start; // Start time of the frame, absolute
	struct timespec current_time; // Current time, absolute
	struct timespec time_elapsed; // Delta between current and start time, relative, ns only
	struct timespec frame_duration; // Frame theoritical duration, computed from fps, relative, ns only
	struct timespec timeout_duration; // Next pselect's timeout, e.g frame_duration - time_elapsed, relative, ns only
	
	double fps = 1; // Number of game frames calculated per second (starting at 1)

	// Since fps (and thus frame time) are bound by FPS_MIN, we'll only use the ns attribute
	time_elapsed.tv_sec = 0;
	timeout_duration.tv_sec = 0;
	frame_duration.tv_sec = 0;


	int cont = 1;  // Continue flag
	int poll_cont, sel_buf; // Polling continuation boolean and pselect retval buffer
	char c; // Keystroke buffer
	while(cont) { // TODO probably put the game part in a file somewhere
		puts("Entering main loop"); // TODO DEBUG
		// Getting time at the start of the frame
		clock_gettime(CLOCK_MONOTONIC, &frame_start); // Monotonic doesn't change when time is re adjusted by NTP, and doesn't warp accross time when suspend (despite POSIX standard lmao)
		// Updating frame duration against new fps if it changed
		frame_duration.tv_nsec = (1/fps)*1000000000; 

		// Terminal handling
		term_size_update(&ws); // Update the terminal size 
		/*
			// TODO Resize the display if needed
			// Display the map / update things and all
		*/

		// Write down time current time just before polling phase and calculate elapsed time
		clock_gettime(CLOCK_MONOTONIC, &current_time);
		
		diff_timespec(&current_time, &frame_start, &time_elapsed);

		// Poll keystrokes
		poll_cont = 1; // Continue to poll keypresses
		while(poll_cont) { 
			puts("\tpolling...");
			// Craft the timeout duration from elapsed time
			diff_timespec(&frame_duration, &time_elapsed, &timeout_duration);
			printf("\t\tTO duration :%ld,%ld\n",timeout_duration.tv_sec, timeout_duration.tv_nsec);
			
			// Needs to be reset at every call 
			FD_ZERO(&set); 
			FD_SET(STDIN_FILENO, &set); // Add stdin to the FD set
			sel_buf = pselect(STDIN_FILENO+1, &set, NULL, NULL, &timeout_duration, &sig_mask);

			printf("\t\tpselect retval : %d\n",sel_buf);
			
			if(sel_buf == 0) poll_cont = 0; // Timed out, no fd updated : go for next frame
			else if(sel_buf == 1) { // Keypress detected, stdin updated
				read(STDIN_FILENO, &c, 1); // Read input and empty the buffer
				puts("\t\tKeystroke detected !"); // TODO DEBUG
				/* 
					TODO process input
				*/
				if(c == 'q') {
					fputc(c, stdout);
					cont = 0; 
				}
				// Update elapsed time and timeout for next iteration
				clock_gettime(CLOCK_MONOTONIC, &current_time);
				diff_timespec(&current_time, &frame_start, &time_elapsed);
			} else {
				puts("I/O error. Exiting.");
				exit(-1);
			}
		}
	}
//	Vect_2di_t *map_size = map_getsize("maaaap.txt");
//
//	char** map = map_init(map_size);
//	map_load("maaaap.txt", map);
//	
//	// Display array size
//	//printf("%d,%d\n",map_size->x,map_size->y);
//	
//	map_display(map, map_size);
//
//	// Map buffer à remplir avec la nouvelle map et faire les modifs dessus, puis commit sur l'ancienne après affcichage
//
//	/////////////////////////////////////////////:
//	
//	Car_t* car = car_init(2, vect_2di_init(10,30), 42, 12, NULL);
//
//	car_debug(car);


	return EXIT_SUCCESS;
}
