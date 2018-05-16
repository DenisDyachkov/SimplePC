#ifndef TERM_LIBRARY_H
#define TERM_LIBRARY_H

typedef enum {
    BLACK = 0,  //always first
    RED,
    GREEN,
    YELLOW,
    BLUE,
    PURPLE,
    CYAN,
    WHITE       //always last
} eColors;

int mt_clrscr(void);
int mt_gotoXY(int x, int y);
int mt_getscreensize(int *rows, int *cols);
int mt_setfgcolor(eColors color);
int mt_setbgcolor(eColors color);
int mt_reset(void);

#endif