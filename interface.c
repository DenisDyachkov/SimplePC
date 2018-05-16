#include <myBigChars.h>
#include <mySimpleComputer.h>
#include <string.h>
#include <stdio.h>
#include  "interface.h"

void printKeys();

int big[][2] = {
        {0xC3C3C3FF, 0xFFC3C3C3}, //0
        {0xC0F0E0C0, 0xC0C0C0C0}, //1
        {0x3060C37E, 0xFF03060C}, //2
        {0x78C0C37E, 0x7EC3C0C0}, //3
        {0x666C7870, 0x606060FF}, //4
        {0xC73B03FF, 0x3E63C0C0}, //5
        {0x0306CC78, 0xFFC3C3FF}, //6
        {0x3060C0FF, 0x03060C18}, //7
        {0x7EC3C37E, 0x7EC3C3C3}, //8
        {0xFFC3C3FF, 0x0E3160C0}, //9
        {0xFF3C3C00, 0x003C3CFF}  //+
};

static eColors tColor, bColor;

void printBox(const char* title, int x, int y, int width, int height) {
    bc_box(x, y, height, width);
    mt_gotoXY(x, y + width / 2 - strlen(title) / 2);
    printf(title);
}

void interface_load(eColors textColor, eColors background) {
    mt_setfgcolor(textColor);
    mt_setbgcolor(background);
    mt_clrscr();

    tColor = textColor;
    bColor= background;

    printRam();
    printBox("Memory", 1, 1, 61, 12);
    printAccum();
    printBox("Accumulator", 1, 63, 20, 3);
    printCounter();
    printBox("instructionCounter", 4, 63, 20, 3);
    printOper();
    printBox("Operation", 7, 63, 20, 3);
    printFlags();
    printBox("Flags", 10, 63, 20, 3);
    printKeys();
    printCell();
    printIO();
}

void printRam() {
    int row, column, value;
    for (row = 0; row < 100; row += 10) {
        mt_gotoXY(2 + row / 10, 2);
        for (column = 0; column < 10; ++column) {
            sc_memoryGet(row + column, &value);
            printf("%c%04X ", (sc_isCommand(value) ? '+' : ' '), value & 0x3FFF);
        }
    }
}

void printAccum() {
    mt_gotoXY(2, 71);
    printf("%d", accumulator);
}

void printCounter() {
    mt_gotoXY(5, 71);
    printf("%d", instructionCounter);
}

void printOper() {
    //TODO: print value
}

void printFlags() {
    char flags[13] = { 0 };
    int flagStatus = 0;
    if (!sc_regGet(FLAG_IGNORE_CLOCK, &flagStatus) && flagStatus)
        strcat(flags, "Т");
    if (!sc_regGet(FLAG_INVALID_COMMAND, &flagStatus) && flagStatus)
        strcat(flags, " Е");
    if (!sc_regGet(FLAG_OUT_RANGE, &flagStatus) && flagStatus)
        strcat(flags, " М");
    if (!sc_regGet(FLAG_OVERFLOW, &flagStatus) && flagStatus)
        strcat(flags, " П");
    if (!sc_regGet(FLAG_DIV_ZERO, &flagStatus) && flagStatus)
        strcat(flags, " 0");
    mt_gotoXY(11, 74 - strlen(flags) / 2);
    printf(flags);
}

void printKeys() {
    printBox("Keys", 13, 48, 35, 10);
    mt_gotoXY(14, 49);
    printf("l  - load");
    mt_gotoXY(15, 49);
    printf("s  - save");
    mt_gotoXY(16, 49);
    printf("r  - run");
    mt_gotoXY(17, 49);
    printf("t  - step");
    mt_gotoXY(18, 49);
    printf("i  - reset");
    mt_gotoXY(19, 49);
    printf("F5 - accumulator");
    mt_gotoXY(20, 49);
    printf("F6 - instructionCounter");
}

void printCell() {
    bc_box(13, 1, 10, 47);
    int row;
    for (row = 0; row < 5; ++row) {
        bc_printbigchar(big[row], 14, 2 + row * 9, tColor, bColor);
    }
}

void printIO() {
    mt_gotoXY(23, 1);
    printf("Input\\Output:\n");
    printf("log data\nlog data\n");
}