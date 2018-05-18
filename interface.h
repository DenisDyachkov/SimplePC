#ifndef SCOMPUTER_INTERFACE_H
#define SCOMPUTER_INTERFACE_H
#include <myBigChars.h>

void read_console_value(int addr, int *value);
void write_console_value(int addr, int value);
void read_console_filename(char *filename, int max);
void interface_load(eColors textColor, eColors background, eColors selectColor);
void interface_print();
extern int select_cell;

#endif //SCOMPUTER_INTERFACE_H