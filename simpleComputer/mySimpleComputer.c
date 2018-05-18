#include <stdio.h>
#include "mySimpleComputer.h"
#define MAX_DIGIT 0x7FFF

static int ram[RAM_SIZE];
struct stRegisters registers;

int sc_memoryInit() {
    int i;
    for (i = 0; i < RAM_SIZE; ++i)
        ram[i] = 0;
    return 0;
}

int sc_memorySet(int address, int value) {
    if (address < 0 || address > RAM_SIZE - 1) {
        sc_regSet(FLAG_OUT_RANGE, 1);
        return 1;
    }
    ram[address] = value;
    return 0;
}

int sc_memoryGet(int address, int *value) {
    if (address < 0 || address > RAM_SIZE - 1) {
        sc_regSet(FLAG_OUT_RANGE, 1);
        return 1;
    }
    *value = ram[address] & MAX_DIGIT;
    return 0;
}

int sc_memorySave(const char *filename) {
    FILE *fout = fopen(filename, "wb");
    if (fout) {
        fwrite(ram, sizeof(int), RAM_SIZE, fout);
        fclose(fout);
        return 0;
    }
    return 1;
}

int sc_memoryLoad(const char *filename) {
    FILE *fin = fopen(filename, "rb");
    if (fin) {
        fread(ram, sizeof(int), RAM_SIZE, fin);
        fclose(fin);
        return 0;
    }
    return 1;
}

int sc_regInit() {
    return (registers.flags = 0);
}

int sc_regSet(int reg, int value) {
    if (reg < 0 || reg >= FLAGS_END)
        return 1;
    if (value == 1)
        registers.flags |= 1 << reg;
    else if (value == 0)
        registers.flags &= ~(1 << reg);
    else
        return 1;
    return 0;
}

int sc_regGet(int reg, int *value) {
    if (reg < 0 || reg >= FLAGS_END)
        return 1;
    *value = (registers.flags >> reg) & 0x1;
    return 0;
}

int sc_commandEncode(int command, int operand, int *value) {
    if (operand < 0 || operand >= RAM_SIZE)            //operand is incorrect
        return 1;
    if (command < 0x10 || command > 0x11 && command < 0x20 ||     //command is incorrect
        command > 0x21 && command < 0x30 || command > 0x33 && command < 0x40 ||
        command > 0x43 && command < 0x51 || command > 0x76)
        return 1;
    *value = 0x3FFF & (command << 7 | operand);
    return 0;
}

int sc_isCommand(int value) {
    return !(value & 0x4000);
}

int sc_commandDecode(int value, int *command, int *operand) {
    if (value & ~0x3FFF) {//value & 0x4000
        sc_regSet(FLAG_INVALID_COMMAND, 1);
        return 1;
    }
    *operand = value & 0x7F;
    *command = (value >> 7) & 0x7F;
    //is validate operand + command;
    return 0;
}
