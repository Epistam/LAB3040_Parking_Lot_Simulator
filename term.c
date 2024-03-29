#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <termios.h> // Terminal attributes
#include <unistd.h> // For STDIN_FILENO
#include <sys/ioctl.h> // For term interaction / sending flags to it 
#include <fcntl.h> 
#include "include/term.h"

// Save original termios configuration
struct termios orig_termios;

/********************************
 * Terminal function / routines *
 ********************************/

// Whole terminal / screen routines
void term_clear() {
	printf("\033[2J");
} // move to (0,0) too

void term_size_update(Winsize *ws) {
	ioctl(STDOUT_FILENO, TIOCGWINSZ, ws);
}

void term_init() {
	// Get current terminal attributes
	tcgetattr(STDIN_FILENO, &orig_termios);
	// Disable raw mode at exit
	atexit(term_reset);
	// Duplicate attributes
	struct termios raw = orig_termios;
	// Disable Ctrl S / Ctrl Q
	raw.c_iflag &= ~(IXON);
	// Disable echo and canonical on the duplicate attributes
	raw.c_lflag &= ~(ECHO | ICANON);
	// "commit" new attributes to shell
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	// Make the read calls non blocking


	
	// Disable output buffer 
	setvbuf(stdout, NULL, _IONBF, 0);
	//setlocale(LC_ALL, "ISO-8859-1");
//	if(setlocale(LC_ALL, "fr_FR.iso88591") == NULL) {
//		puts("Failed to set locale");
//		exit(1);
//	}

}

void term_reset() {
	// Resend original attributes to terminal
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
	// Restoring cursor
	fputs("\033[?25h",stdout);
	//term_clear(); // TODO uncomment after debugging is done
}

// Single line
void term_clearline() { printf("\033[K");} // move to (0,0) too

// Cursor movement 
void termUp() { printf("\033\[1A");}
void termDown() { printf("\033\[1B");}
void termFwd() { printf("\033\[1C");}
void termBack() { printf("\033\[1D");}
void term_goto(int x, int y) { printf("\033[%d;%dH", y+1, x+1);} // some fucking how coords are inverted in the escape seqs, and indexed on 1

// Cursor color
void term_color_reset() {
	fputs("\033[39m",stdout);
	//fputs("\033[49m",stdout);
	term_bgcolorset(236);
}

void term_fgcolorset(int color) {
	fputs("\033[38;5;",stdout);
	printf("%d",color);
	fputs("m",stdout);
}

void term_bgcolorset(int color) {
	fputs("\033[48;5;",stdout);
	printf("%d",color);
	fputs("m",stdout);
}
