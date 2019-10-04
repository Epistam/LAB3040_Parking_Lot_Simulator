#ifndef TERM_H
#define TERM_H

/********************************
 * Terminal function / routines *
 ********************************/

typedef struct winsize Winsize;

// Whole terminal / screen routines
void termClear();
void updateTermSize(Winsize *ws);
void initTerm();
void resetTerm();

// Single line
void termClearLine();

// Cursor movement 
void termUp();
void termDown();
void termFwd();
void termBack();
void termGoto(int x, int y);

// Cursor color
void reset_colors();
void set_color(int color); 
void set_bg_color(int color);

#endif
