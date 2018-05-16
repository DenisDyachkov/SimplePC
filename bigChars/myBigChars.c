#include "myBigChars.h"
#include <stdio.h>
#include <unistd.h>

#define enter_alt_charset_mode "\E(0"
#define exit_alt_charset_mode "\E(B"

int bc_printA (char *str) {
    if (!str)
        return -1;
    printf(enter_alt_charset_mode"%s"exit_alt_charset_mode, str);
    return 0;
}

int bc_box(int x1, int y1, int x2, int y2) {
    //TODO: fix x1, y1 ? (min [1, 1])
    if (x1 <= 0 || y1 <= 0 || x2 < 1 || y2 < 1)
        return -1;
    --y2, --x2;
    int mx, my;
    if (mt_getscreensize(&mx, &my) || mx < x1 + x2 || my < y1 + y2)
        return -1;
    int x = x1, y = 0;
    
    char hline[y2];
    for (; y < y2 - 1; ++y)
        hline[y] = ACS_HLINE[0];
    hline[y] = 0;

    mt_gotoXY(x1, y1);
    bc_printA(ACS_ULCORNER);//corner ┌
    bc_printA(hline);       //upper hline
    bc_printA(ACS_URCORNER);//corner ┐
    mt_gotoXY(x1 + x2, y1);
    bc_printA(ACS_LLCORNER);//corner └
    bc_printA(hline);       //lower hline
    bc_printA(ACS_LRCORNER);//corner ┘
    
    for (y = y1 + y2, ++x; x < x1 + x2; ++x) {  //vertical lines
        mt_gotoXY(x, y);
        bc_printA(ACS_VLINE);
        mt_gotoXY(x, y1);
        bc_printA(ACS_VLINE);
    }
    return 0;
}

/*
10011110    //0
10100001    //8
10100001    //16
10011110    //24
10100001    //0
10100001    //8
10100001    //16
10011110    //24

big[2] = {0x79858579, 0x79858585};//18
*/

int bc_printbigchar (int *big, int x, int y, eColors color, eColors background) {
    mt_setbgcolor(background);
    mt_setfgcolor(color);
    int x2, y2, val;
    for (x2 = 0; x2 < 8; ++x2) {
        mt_gotoXY(x + x2, y);
        for (y2 = 0; y2 < 8; ++y2) {
            bc_getbigcharpos(big, x2, y2, &val);
            bc_printA(val ? ACS_CKBOARD : " ");
        }
    }
}

int bc_setbigcharpos (int *big, int x, int y, int value) {
    if (!big || x & ~7 || y & ~7)
        return -1;
    big += (x >> 2) & 1;
    if (value & 1)
        *big |= 1 << ((x & 3) * 8 + y);
    else
        *big &= ~(1 << ((x & 3) * 8 + y));
    return 0;
}

int bc_getbigcharpos(int *big, int x, int y, int *value) {
    if (!big || !value || x & ~7 || y & ~7)
        return -1;
    *value = (big[(x >> 2) & 1] >> ((x & 3) * 8 + y)) & 1;
    return 0;
}

int bc_bigcharwrite (int fd, int *big, int count) {
    if (fd == -1 || write(fd, big, (size_t)(count * 2 * 4)) == -1)
        return -1;
    return 0;
}

int bc_bigcharread (int fd, int *big, int need_count, int *count) {
    if (fd == -1)
        return -1;
    if ((*count = read(fd, big, (size_t)(need_count * 2 * 4))) == -1) {
        *count = 0;
        return -1;
    }
    return 0;
}
