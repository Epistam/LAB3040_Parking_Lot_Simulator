#ifndef TERM_H
#define TERM_H

/********************************
 * Terminal function / routines *
 ********************************/

typedef struct winsize Winsize;

// Whole terminal / screen routines
void term_clear();
void term_size_update(Winsize *ws);
void term_init();
void term_reset();

// Single line
void term_clearline();

// Cursor movement 
void termUp();
void termDown();
void termFwd();
void termBack();
void term_goto(int x, int y);

// Cursor color
void term_color_reset();
void term_fgcolorset(int color);
void term_bgcolorset(int color);

#endif
