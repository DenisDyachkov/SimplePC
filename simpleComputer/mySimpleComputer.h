#ifndef _SC_MEMORY_H_
#define _SC_MEMORY_H_

#define RAM_SIZE 100

enum {
    FLAG_OUT_RANGE,
    FLAG_INVALID_COMMAND,
    FLAG_OVERFLOW,
    FLAG_DIV_ZERO,
    FLAG_IGNORE_CLOCK,
    FLAGS_END    //always at the end, shows the end of the flags
};

int sc_memoryInit();
int sc_memorySet(int address, int value);
int sc_memoryGet(int address, int *value);
int sc_memorySave(const char *filename);
int sc_memoryLoad(const char *filename);
int sc_regInit();
int sc_regSet(int _register, int value);
int sc_regGet(int _register, int *value);
int sc_isCommand(int value);
int sc_commandEncode(int command, int operand, int *value);
int sc_commandDecode(int value, int *command, int *operand);

extern int instructionCounter;
extern int accumulator;

#endif
