#include "myBigChars.h"

#include <stdio.h>
int main() {
    int big[] = {0x79858579, 0x79858585};
    mt_clrscr();
    bc_box(5, 1, 10, 10);
    bc_printbigchar(big, 6, 2, RED, BLUE);
    printf("\n");
    mt_gotoXY(0,0);
    getchar();
    mt_reset();
    return 0;
}
