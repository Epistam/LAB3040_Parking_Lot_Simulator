#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h> // Terminal attributes
#include <unistd.h> // For STDIN_FILENO
#include <sys/ioctl.h> // For term interaction / sending flags to it 
#include <sys/signal.h> 
#include <fcntl.h> 
#include "include/const.h"
#include "include/struct.h"
#include "include/term.h"
#include "include/car.h"
#include "include/map.h"
#include "include/parking.h"

void diff_timespec(struct timespec *a, struct timespec *b, struct timespec *result) { 
	long long int buf_a = (a->tv_sec)*1000000000 + a->tv_nsec;
	long long int buf_b = (b->tv_sec)*1000000000 + b->tv_nsec;
	long long int buf_result = (buf_b > buf_a ? buf_b - buf_a : buf_a - buf_b);

	result->tv_sec = buf_result/1000000000;
	result->tv_nsec = buf_result%1000000000;
}

void parking_loop() { // Event loop : returns a code depending on how it was exited

	// Terminal handling
	term_clear();
	winsize_t ws;
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK); // Switch read to non blocking

	// Random generator intialization
	srand(time(NULL));

	// Select handling
	fd_set set; // FD set for select to listen on
	sigset_t sig_mask; // Signal mask, required argument, wdgaf whatsoever tho
	sigemptyset(&sig_mask);

	// Define the time structs used to keep track in the loop
	struct timespec frame_start; // Start time of the frame, absolute
	struct timespec current_time; // Current time, absolute
	struct timespec time_elapsed; // Delta between current and start time, relative
	struct timespec frame_duration; // Frame theoritical duration, computed from fps, relative
	frame_duration.tv_sec = 0; // Leave me the fuck alone GCC
	struct timespec timeout_duration; // Next pselect's timeout, e.g frame_duration - time_elapsed, relative
	
	// "global" variables
	double fps = 1; // Number of game frames calculated per second (starting at 1)
	int cont = 1, poll_cont;  // Boolean flags for main loop and polling loop
	int sel_buf; // pselect's retval buffer
	char c; // Keystroke buffer

	// TODO initialize the game level
	Vect_2di_t *map_size = map_getsize("assets/map.txt");

	char** orig_map = map_init(map_size); 
	char** current_map = map_init(map_size); 
	char** current_fgcolormap = map_init(map_size); 
	char** current_bgcolormap = map_init(map_size); 

	char** next_map = map_init(map_size); 
	char** next_fgcolormap = map_init(map_size); 
	char** next_bgcolormap = map_init(map_size); 

	map_load("assets/map.txt", orig_map);
	map_copy(orig_map, current_map, map_size);
	map_copy(orig_map, next_map, map_size);

	map_set(current_fgcolormap, 15, map_size);
	map_set(next_fgcolormap, 15, map_size);
	
	map_set(current_bgcolormap, 0, map_size);
	map_set(next_bgcolormap, 0, map_size);
	

	// TODO debug
	Car_t* car_list = NULL; // Pointer on the car_list pointer (so that it can be set to null by other functions)
	car_spawn(&car_list, current_map, current_fgcolormap, vect_2di_init(29,33), 9, 2);
	car_spawn(&car_list, current_map, current_fgcolormap, vect_2di_init(39,30), 9, 0);
	car_spawn(&car_list, current_map, current_fgcolormap, vect_2di_init(32,23), 9, 3);
	car_spawn(&car_list, current_map, current_fgcolormap, vect_2di_init(36,23), 9, 3);
	car_spawn(&car_list, current_map, current_fgcolormap, vect_2di_init(40,17), 9, 1);
	car_spawn(&car_list, current_map, current_fgcolormap, vect_2di_init(47,12), 9, 2);
	map_display_debug(current_map, NULL, map_size);

	// Main event loop, keeps refreshing the game state every so often
	while(cont) {
		// Frame start time management
		clock_gettime(CLOCK_MONOTONIC, &frame_start); // (monotonic doesn't change when time is re adjusted by NTP, and doesn't magically warp accross time when the PC is suspended)
		frame_duration.tv_nsec = (1/fps)*1000000000; // Updating frame duration if fps has changed during previous loop

		// Terminal handling
		term_size_update(&ws); // Update terminal size

		// TODO Level refresh
		/*
		 	// Resize the display if needed
			// Display the map / update things and all
		*/
		// Temporary, will move to delta buffer afterwards (TODO)
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		FILE* aaaa = fopen("parking.log", "a");
		fprintf(aaaa, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
		fclose(aaaa);
		Car_t* current_car = car_list;
		while(current_car != NULL) {
			// Skip as many steps as speed, and update the car itself and its location in the map
			car_debug(current_car);
			current_car = current_car->next_car;
		}


		// Last car with next_car == NULL
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		cars_update(orig_map, current_map, current_fgcolormap, &car_list, map_size); // interpolate steps with speed to skip steps, and this for eveyr car
		map_display_debug(current_map, NULL, map_size);

//	// Map buffer à remplir avec la nouvelle map et faire les modifs dessus, puis commit sur l'ancienne après affcichage
//
//	/////////////////////////////////////////////:
//	
//	Car_t* car = car_init(2, vect_2di_init(10,30), 42, 12, NULL);
//
//	car_debug(car);
		
		// Once the game is refreshed, we pause for a while and listen to commands until frame_duration is over
		clock_gettime(CLOCK_MONOTONIC, &current_time);
		diff_timespec(&current_time, &frame_start, &time_elapsed); // Calculate elapsed time since frame start

		// Poll keystrokes
		poll_cont = 1;
		while(poll_cont) { 
			// Refresh status bar
			
			//cars_refresh(); // Refresh the cars on map
			//lights_refresh();

			//status_update(); // Update the status bar
			
			// Compute timeout duration
			diff_timespec(&frame_duration, &time_elapsed, &timeout_duration); // timeout_duration = frame_duration - time_elapsed
			FD_ZERO(&set); // Reset the FD set
			FD_SET(STDIN_FILENO, &set); // Add stdin to the FD set (needs to be done before every pselect call
			sel_buf = pselect(STDIN_FILENO+1, &set, NULL, NULL, &timeout_duration, &sig_mask);

			if(sel_buf == 0) poll_cont = 0; // pselect timedout without a keystroke : next frame
			else if(sel_buf == 1) { // Keystroke detected
				while(read(STDIN_FILENO, &c, 1) == 1) { // Read input and empty the buffer (takes into account wide chars)
					/* 
						TODO process input
					*/
					if(c == 'z') fps *= (fps>=FPS_MAX) ? 1 : 1.25;
					if(c == 's') fps /= (fps<=FPS_MIN) ? 1 : 1.25;
					if(c == 'q') {
						fputc(c, stdout);
						cont = 0; 
					}
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
}
