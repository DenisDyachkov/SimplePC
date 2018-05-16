#include "../myTerm.h"
#include <stdio.h>
#include <string.h>

void center_print(const char *msg) {
    int maxx, maxy;
    mt_getscreensize(&maxx, &maxy);
    mt_gotoXY(maxx/2, maxy/2 - strlen(msg)/2);
    printf(msg);
}

int main() {
    mt_setbgcolor(YELLOW);
    mt_setfgcolor(CYAN);
    mt_clrscr();
    getchar();
    center_print("Hello, world!");
    getchar();
    mt_reset();
    return 0;
}
