#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h> // Terminal attributes
#include <unistd.h> // For STDIN_FILENO
#include <sys/ioctl.h> // For term interaction / sending flags to it 
#include <sys/signal.h> 
#include <string.h>
#include "include/const.h"
#include "include/struct.h"
#include "include/term.h"
#include "include/car.h"
#include "include/map.h"
#include "include/menu.h"

void menu_display(int choice) {
	winsize_t ws;
	term_size_update(&ws);

	char* title_1 = strdup("LAB3040");
	char* title_2 = strdup("The Parking");
	char **choices = malloc(10*sizeof(char*));
	choices[0] = strdup("Mode \"facile\"");
	choices[1] = strdup("Mode \"chargé\" (indisponible) ");
	choices[2] = strdup("Quitter");
	choices[3] = strdup("TEST");
	choices[4] = strdup("TEST");
	choices[5] = strdup("TEST");
	choices[6] = strdup("TEST");
	choices[7] = strdup("TEST");
	choices[8] = strdup("TEST");
	choices[9] = strdup("z/s pour accélerer / décélerer");
	int i,j,k;


	// Draw title
	// Draw top options

	int title_offset = 10;

	term_bgcolorset(240);
	for(i = 0 ; i < 4 ; i++) {
		term_goto(ws.ws_col/3 + 1, i);
		for(j = ws.ws_col/3 + 1 ; j < 2*ws.ws_col/3 ; j++) fputc(' ',stdout);
	}
	term_goto(ws.ws_col/3 + 1, i++);
	for(j = ws.ws_col/3 + 1 ; j < ws.ws_col/2 - strlen(title_1)/2 ; j++) fputc(' ',stdout); 
	printf("%s",title_1);
	for(j = ws.ws_col/2 + strlen(choices[i])/2 ; j < 2*ws.ws_col/3 - 2; j++) fputc(' ',stdout);

	term_goto(ws.ws_col/3 + 1, i++);
	for(j = ws.ws_col/3 + 1 ; j < ws.ws_col/2 - strlen(title_2)/2 ; j++) fputc(' ',stdout); 
	printf("%s",title_2);
	for(j = ws.ws_col/2 + strlen(choices[i])/2 ; j < 2*ws.ws_col/3 - 4; j++) fputc(' ',stdout);

	for(; i < 10 ; i++) {
		term_goto(ws.ws_col/3 + 1, i);
		for(j = ws.ws_col/3 + 1 ; j < 2*ws.ws_col/3 ; j++) fputc(' ',stdout);
	}


	term_bgcolorset(235);
	for(i = 0 ; i < choice; i++) {
		// Top line
		term_goto(ws.ws_col/3 + 1, 3*i + title_offset);
		for(j = ws.ws_col/3 + 1 ; j < 2*ws.ws_col/3 ; j++) fputc(' ',stdout);
		
		// middle line and choice name
		term_goto(ws.ws_col/3 + 1, 3*i + 1 + title_offset);
		for(j = ws.ws_col/3 + 1 ; j < ws.ws_col/2 - strlen(choices[i])/2 ; j++) fputc(' ',stdout); 
		printf("%s",choices[i]);
		for(j = ws.ws_col/2 + strlen(choices[i])/2 ; j < 2*ws.ws_col/3 + 1; j++) fputc(' ',stdout);

		// Bottom line
		term_goto(ws.ws_col/3 + 1, 3*i+2 + title_offset);
		for(j = ws.ws_col/3 + 1 ; j < 2*ws.ws_col/3 ; j++) fputc(' ',stdout);
	}

	term_bgcolorset(7);
	term_fgcolorset(235);
	// Top line
	term_goto(ws.ws_col/3 + 1, 3*i + title_offset);
	for(j = ws.ws_col/3 + 1 ; j < 2*ws.ws_col/3 ; j++) fputc(' ',stdout);
	
	// middle line and choice name
	term_goto(ws.ws_col/3 + 1, 3*i + 1 + title_offset);
	for(j = ws.ws_col/3 + 1 ; j < ws.ws_col/2 - strlen(choices[i])/2 ; j++) fputc(' ',stdout); 
	printf("%s",choices[i]);
	for(j = ws.ws_col/2 + strlen(choices[i])/2 ; j < 2*ws.ws_col/3 + 1; j++) fputc(' ',stdout);

	// Bottom line
	term_goto(ws.ws_col/3 + 1, 3*i+2 + title_offset);
	for(j = ws.ws_col/3 + 1 ; j < 2*ws.ws_col/3 ; j++) fputc(' ',stdout);

	term_bgcolorset(235);
	term_fgcolorset(15);

	i++;

	for(i = choice+1 ; i < 10; i++) {
		// Top line
		term_goto(ws.ws_col/3 + 1, 3*i + title_offset);
		for(j = ws.ws_col/3 + 1 ; j < 2*ws.ws_col/3 ; j++) fputc(' ',stdout);
		
		// middle line and choice name
		term_goto(ws.ws_col/3 + 1, 3*i + 1 + title_offset);
		for(j = ws.ws_col/3 + 1 ; j < ws.ws_col/2 - strlen(choices[i])/2 ; j++) fputc(' ',stdout); 
		printf("%s",choices[i]);
		for(j = ws.ws_col/2 + strlen(choices[i])/2 ; j < 2*ws.ws_col/3 + 1; j++) fputc(' ',stdout);

		// Bottom line
		term_goto(ws.ws_col/3 + 1, 3*i+2 + title_offset);
		for(j = ws.ws_col/3 + 1 ; j < 2*ws.ws_col/3 ; j++) fputc(' ',stdout);
	}

	// Draw vertical lines
	term_bgcolorset(245);
	for(i = 0 ; i < ws.ws_row +1 ; i++) {
		term_goto(ws.ws_col/3-1, i);
		printf("  ");
	}
	for(i = 0 ; i < ws.ws_row +1 ; i++) {
		term_goto(2*ws.ws_col/3, i);
		printf("  ");
	}
	term_bgcolorset(0);

}

int menu_loop() {
	winsize_t ws;
	int current_choice;
	char c;

	menu_display(0);

	while(read(STDIN_FILENO, &c, 1) == 1) { 
		if(c == '\033') { // Is an escape sequence
			getchar(); // Flush the '['
			switch(getchar()) {
				case 'A' :
					current_choice = (current_choice + 10 - 1)%10;
					break;
				case 'B' :
					current_choice = (current_choice + 10 + 1)%10;
					break;
			}
		}
		if(c == '\n') return current_choice;
		menu_display(current_choice);
	}



	return current_choice;
}
