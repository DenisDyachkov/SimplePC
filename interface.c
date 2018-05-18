#include <mySimpleComputer.h>
#include <myReadKey.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include  "interface.h"

int select_cell;
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
        {0x6666663C, 0x66667E66}, //A
        {0x7FC3C37F, 0x7FC3C3C3}, //B
        {0x0303C37E, 0x7EC30303}, //C
        {0xC3C3C37F, 0x7FC3C3C3}, //D
        {0x1F03037F, 0x7F030303}, //E
        {0x1F03037F, 0x03030303}, //F
        {0xFF3C3C00, 0x003C3CFF}  //+
};
char io_msg[1024];

struct {
    eColors text_color;
    eColors back_color;
    eColors select_color;
} colors;

void log_console(const char *msg) {
    strcat(io_msg, msg);
    strcat(io_msg, "\n");
}

void read_console_value(int addr, int *value) {
    printf("Enter: ");
    rk_mytermregime(0, 0, 1, 1, 1);
    scanf("%X", value);
    char print[16];
    sprintf(print, "%d<\t%0X", addr, *value);
    log_console(print);
}

void read_console_filename(char *filename, int max) {
    printf("Enter filename: ");
    rk_mytermregime(0, 0, 1, 1, 1);
    fgets(filename, max, stdin);
    filename[strlen(filename) - 1] = 0;
}

void write_console_value(int addr, int value) {
    char print[16];
    sprintf(print, "%d>\t%0X", addr, value);
    log_console(print);
}

void interface_load(eColors textColor, eColors background, eColors selectColor) {
    mt_setfgcolor(textColor);
    mt_setbgcolor(background);
    colors.text_color = textColor;
    colors.back_color = background;
    colors.select_color = selectColor;
    select_cell = 0;
    io_msg[0] = 0;
    int count_write;
    int tmp_chars[17 * 2];
    if (bc_bigcharread(open("chars.font", O_RDONLY), tmp_chars, 17, &count_write))
        bc_bigcharwrite(open("chars.font", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR), (int*)big, 17);//write default
    else
        memcpy(big, tmp_chars, sizeof(tmp_chars));
    interface_print();
}

void printBox(const char* title, int x, int y, int width, int height) {
    bc_box(x, y, height, width);
    mt_gotoXY(x, y + width / 2 - strlen(title) / 2);
    puts(title);
}

void printCell(int plus, int d) {
    bc_box(13, 1, 10, 47);
    int digit;
    if (plus)
        bc_printbigchar(big[16], 14, 2, colors.text_color, colors.back_color);
    for (digit = 3; digit >= 0; --digit)
        bc_printbigchar(big[(d >> (digit * 4)) & 0xF], 14, 11 + (3 - digit) * 9, colors.text_color, colors.back_color);
}

void printOperation(int value) {
    int command = 0, operand = 0;
    mt_gotoXY(8, 69);
    sc_commandDecode(value, &command, &operand);
    printf("%c%02X : %02X", command != 0 ? '+' : ' ', command, operand);
    printBox("Operation", 7, 63, 20, 3);
}

void printSelectCell(int is_cmd, int value) {
    mt_setbgcolor(colors.select_color);
    if (is_cmd) {
        int cmd, operand;
        sc_commandDecode(value, &cmd, &operand);
        printf("+%02X%02X ", cmd, operand);
    }
    else
        printf(" %04X ", value & 0x3FFF);
    //printf("%c%04X", (is_cmd ? '+' : ' '), value & 0x3FFF);
    mt_setbgcolor(colors.back_color);
    printf(" ");
    printOperation(value);//TODO: change
    printCell(is_cmd, value & 0x3FFF);
}

void printRam() {
    int row, column, value;
    for (row = 0; row < 100; row += 10) {
        mt_gotoXY(2 + row / 10, 2);
        for (column = 0; column < 10; ++column) {
            sc_memoryGet(row + column, &value);
            if ((row + column) == select_cell) {
                printSelectCell(sc_isCommand(value), value);
                mt_gotoXY(2 + row / 10, 2 + (column + 1) * 6);
                continue;
            }
            if (sc_isCommand(value)) {
                int cmd, operand;
                sc_commandDecode(value, &cmd, &operand);
                printf("+%02X%02X ", cmd, operand);
            }
            else
                printf(" %04X ", value & 0x3FFF);
        }
    }
    printBox("Memory", 1, 1, 61, 12);
}

void printAccum() {
    mt_gotoXY(2, 71);
    printf("%hd", registers.accumulator);
    printBox("Accumulator", 1, 63, 20, 3);
}

void printCounter() {
    mt_gotoXY(5, 71);
    printf("%hhu", registers.instruction_counter);
    printBox("instructionCounter", 4, 63, 20, 3);
}

void printFlags() {
    char flags[10];
    int flagStatus[FLAGS_END] = { 0 }, flag;
    for (flag = 0; flag < FLAGS_END; ++flag)
        sc_regGet(flag, flagStatus + flag);

    sprintf(flags, "%c %c %c %c %c",
            flagStatus[0] ? 'M' : '-',
            flagStatus[1] ? 'C' : '-',
            flagStatus[2] ? 'F' : '-',
            flagStatus[3] ? '0' : '-',
            flagStatus[4] ? 'T' : '-');
    mt_gotoXY(11, 69);
    puts(flags);
    printBox("Flags", 10, 63, 20, 3);
}

void printKeys() {
    static const char *text[] = {
        "l  - load",
        "s  - save",
        "r  - run",
        "t  - step",
        "i  - reset",
        "F5 - accumulator",
        "F6 - instructionCounter"
    };
    int line = 14;
    printBox("Keys", 13, 48, 35, 10);
    for (; line < 21; ++line) {
        mt_gotoXY(line, 49);
        puts(text[line - 14]);
    }
}

void printIO() {
    mt_gotoXY(23, 1);
    fputs("Input\\Output:\n", stdout);
    fputs(io_msg, stdout);
}

void interface_print() {
    mt_clrscr();
    printRam();
    printAccum();
    printCounter();
    printFlags();
    printKeys();
    printIO();
    fflush(stdout);
}