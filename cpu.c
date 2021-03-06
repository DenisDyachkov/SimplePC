#include "cpu.h"
#include "interface.h"
#include <mySimpleComputer.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

enum eCmdLIST {
    READ = 0x10,
    WRITE = 0x11,
    LOAD = 0x20,
    STORE = 0x21,
    ADD = 0x30,
    SUB = 0x31,
    DIV = 0x32,
    MUL = 0x33,
    JMP = 0x40,
    JNE = 0x41,
    JZ = 0x42,
    HALT = 0x43,
    JNC = 0x57,
    JNP = 0x59
};

typedef int(*instr_callback)(int);

struct stInstructionInfo {
    const char* name;
    instr_callback function;
    enum eCmdLIST code;
};

int __read(int operator);
int __write(int operator);
int __load(int operator);
int __store(int operator);
int __add(int operator);
int __sub(int operator);
int __div(int operator);
int __mul(int operator);
int __jmp(int operator);
int __jne(int operator);
int __jz(int operator);
int __halt(int operator);
int __jnc(int operator);
int __jnp(int operator);

static struct stInstructionInfo
        cmds[] = {
        {"READ", __read, READ},
        {"WRITE", __write, WRITE},
        {"LOAD", __load, LOAD},
        {"STORE", __store, STORE},
        {"ADD", __add, ADD},
        {"SUB", __sub, SUB},
        {"DIV", __div, DIV},
        {"MUL", __mul, MUL},
        {"JUMP", __jmp, JMP},
        {"JNEG", __jne, JNE},
        {"JZ", __jz, JZ},
        {"HALT", __halt, HALT},
        {"JNC", __jnc, JNC},
        {"JNP", __jnp, JNP}
};

struct stInstructionInfo* instruct_search(int code) {
    int r = sizeof(cmds) / sizeof(cmds[0]), l = 0;
    while (l < r) {
        int m = (r - l) / 2 + l;
        if (cmds[m].code == code)
            return cmds + m;
        else if (cmds[m].code > code)
            r = m;
        else
            l = m + 1;
    }
    return NULL;
}

int cmd_search(const char* cmd) {
    int size = sizeof(cmds) / sizeof(cmds[0]), i = 0;
    for (; i < size; ++i)
        if (!strcmp(cmds[i].name, cmd))
            return cmds[i].code;
    return -1;
}

int is_cmd_exist(int code) {
    return instruct_search(code) != NULL;
}

int is_cmd_arithmetic(int code) {
    return (code == ADD || code == SUB || code == DIV || code == MUL);
}

int ALU(int code, int operand) {
    struct stInstructionInfo* instruction = instruct_search(code);
    if (!instruction) {
        sc_regSet(FLAG_INVALID_COMMAND, 1);
        return -1;
    }
    return instruction->function(operand);
}

int CU() {
    int data, code, addr;
    if (sc_memoryGet(registers.instruction_counter, &data) ||
        sc_commandDecode(data, &code, &addr)) {
        sc_regSet(FLAG_IGNORE_CLOCK, 1);
        return -1;
    }
    if (is_cmd_exist(code) == 0) {
        sc_regSet(FLAG_INVALID_COMMAND, 1);
        sc_regSet(FLAG_IGNORE_CLOCK, 1);
        return -1;
    }
    if (addr < 0 || addr >= RAM_SIZE) {
        sc_regSet(FLAG_OUT_RANGE, 1);
        sc_regSet(FLAG_IGNORE_CLOCK, 1);
        return -1;
    }

    ++registers.instruction_counter;
    if (is_cmd_arithmetic(code)) {
        if (ALU(code, addr)) {
            sc_regSet(FLAG_IGNORE_CLOCK, 1);
            return -1;
        }
    } else
        instruct_search(code)->function(addr);
    select_cell = registers.instruction_counter;
    return 0;
}

int sign_number_from_memory(int value) {
    if (value & 0x2000)// negative
        value = -1 * (((~value) & 0x3FFF) + 1);
    return value;
}

int sign_number_to_memory(int value) {
    if (value < 0)//(value & (~0x3FFF)) == 0x3FFF
        value = 0x2000 | (value & 0x3FFF);
    return value;
}

int __read(int operator) {
    int result;
    read_console_value(operator, &result);
    result = sign_number_to_memory(result);
    if (result & (~0x3FFF)) {
        sc_regSet(FLAG_OVERFLOW, 1);
        result &= 0x3FFF;
    }
    sc_memorySet(operator, result | 0x4000);
    return 0;
}

int __write(int operator) {
    int result;
    sc_memoryGet(operator, &result);
    write_console_value(operator, sign_number_from_memory(result & 0x3FFF));
    return 0;
}

int __load(int operator) {
    sc_memoryGet(operator, (int*)&registers);//accumulator (0 offset)
    registers.accumulator = sign_number_from_memory(registers.accumulator & 0x3FFF);
    return 0;
}

int __store(int operator) {
    int value = sign_number_to_memory(registers.accumulator) | 0x4000;
    sc_memorySet(operator, value);
    return 0;
}

void accum_overflow_fix() {
    if (registers.accumulator > 0 &&
        registers.accumulator & (~0x3FFF)) {
        sc_regSet(FLAG_OVERFLOW, 1);
        registers.accumulator &= 0x3FFF;
    }
}

int __add(int operator) {
    int right;
    sc_memoryGet(operator, &right);
    registers.accumulator += sign_number_from_memory(right & 0x3FFF);
    accum_overflow_fix();
    return 0;
}

int __sub(int operator) {
    int right;
    sc_memoryGet(operator, &right);
    registers.accumulator -= sign_number_from_memory(right & 0x3FFF);
    accum_overflow_fix();
    return 0;
}

int __div(int operator) {
    int right;
    sc_memoryGet(operator, &right);
    if (right == 0) {
        sc_regSet(FLAG_DIV_ZERO, 1);
        return -1;
    }
    registers.accumulator /= sign_number_from_memory(right & 0x3FFF);
    return 0;
}

int __mul(int operator) {
    int right;
    sc_memoryGet(operator, &right);
    registers.accumulator *= sign_number_from_memory(right & 0x3FFF);
    accum_overflow_fix();
    return 0;
}

int __jmp(int operator) {
    registers.instruction_counter = operator;
    return 0;
}

int __jne(int operator) {
    if (registers.accumulator < 0)//TODO: старший бит (15) = 1 -> отрицательное
        registers.instruction_counter = operator;
    return 0;
}

int __jz(int operator) {
    if (registers.accumulator == 0)
        registers.instruction_counter = operator;
    return 0;
}

int __halt(int operator) {
    sc_regSet(FLAG_IGNORE_CLOCK, 1);
    registers.instruction_counter = 0;
    return 0;
}

int __jnc(int operator) {
    int of, instruction, cmd, addr;
    sc_regGet(FLAG_OVERFLOW, &of);
    sc_memoryGet(registers.instruction_counter - 1, &instruction);
    sc_commandDecode(instruction, &cmd, &addr);
    if (cmd == ADD && of)
        registers.instruction_counter = operator;
    //if (registers.accumulator & (~0x3FFF))
    //    registers.instruction_counter = operator;
    return 0;
}

int __jnp(int operator) {
    if (registers.accumulator & 1)
        registers.instruction_counter = operator;
    return 0;
}