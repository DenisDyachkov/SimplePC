#ifndef BIGCHARS_LIBRARY_H
#define BIGCHARS_LIBRARY_H

#include "myTerm.h"

#define ACS_ULCORNER "l" /* upper left corner */
#define ACS_LLCORNER "m" /* lower left corner */
#define ACS_URCORNER "k" /* upper right corner */
#define ACS_LRCORNER "j" /* lower right corner */
#define ACS_LTEE "t" /* tee pointing right */
#define ACS_RTEE "u" /* tee pointing left */
#define ACS_BTEE "v" /* tee pointing up */
#define ACS_TTEE "w" /* tee pointing down */
#define ACS_HLINE "q" /* horizontal line */
#define ACS_VLINE "x" /* vertical line */
#define ACS_PLUS "n" /* large plus or crossover */
#define ACS_S1 "o" /* scan line 1 */
#define ACS_S9 "s" /* scan line 9 */
#define ACS_DIAMOND "`" /* diamond */
#define ACS_CKBOARD "a" /* checker board (stipple) */
#define ACS_DEGREE "f" /* degree symbol */
#define ACS_PLMINUS "g" /* plus/minus */
#define ACS_BULLET "~" /* bullet */
#define ACS_S3 "p" /* scan line 3 */
#define ACS_S7 "r" /* scan line 7 */
#define ACS_LEQUAL "y" /* less/equal */
#define ACS_GEQUAL "z" /* greater/equal */
#define ACS_PI "{" /* Pi */
#define ACS_NEQUAL "|" /* not equal */
#define ACS_STERLING "}" /* UK pound sign */

int bc_printA(char * str);
int bc_box(int x1, int y1, int x2, int y2);
int bc_printbigchar(int *big, int x, int y, eColors color, eColors background);
int bc_setbigcharpos(int * big, int x, int y, int value);
int bc_getbigcharpos(int * big, int x, int y, int *value);
int bc_bigcharwrite(int fd, int * big, int count);
int bc_bigcharread(int fd, int * big, int need_count, int * count);

#endif