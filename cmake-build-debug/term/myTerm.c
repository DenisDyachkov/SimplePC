#include "myTerm.h"

#include <stdio.h>
#include <sys/ioctl.h>

#define CLEAR_SCREEN "\E[H\E[J"
#define BACKGR_SET_FORMAT "\E[4%dm"
#define FOREGR_SET_FORMAT "\E[3%dm"
#define CURSOR_SET_FORMAT "\E[%d;%dH"
#define CURSOR_INV "\E[?25l\E[?1c"
#define CURSOR_VIS "\E[?25h\E[?8c"
#define GR_RESET "\E[0m"

int mt_clrscr() {
    printf(CLEAR_SCREEN);
    return 0;
}

int mt_gotoXY(int x, int y) {
    if (x < 0 || y < 0)
        return -1;
    int max_x, max_y;
    if (mt_getscreensize(&max_x, &max_y) || max_x < x || max_y < y)
        return -1;
    printf(CURSOR_SET_FORMAT, x, y);
    return 0;
}

int mt_getscreensize(int *rows, int *cols) {
    if (rows && cols) {
        struct winsize ws;
        if (!ioctl(1, TIOCGWINSZ, &ws)) {
            *rows = ws.ws_row;
            *cols = ws.ws_col;
            return 0;
        }
    }
    return -1;
}

int mt_setfgcolor(eColors color) {
    if (color < BLACK || color > WHITE)
        return -1;
    printf(FOREGR_SET_FORMAT, color);
    return 0;
}

int mt_setbgcolor(eColors color) {
    if (color < BLACK || color > WHITE)
        return -1;
    printf(BACKGR_SET_FORMAT, color);
    return 0;
}

int mt_reset() {
    printf(GR_RESET);
    mt_clrscr();
    return 0;
}
