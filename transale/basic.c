#include "basic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct stLines {
    unsigned line_number;
    unsigned start_address;
};

struct stVariables {
    unsigned address;
    int init_value;
    char name;
};

struct stGotoConflict {
    unsigned instratuction_address;
    unsigned goto_line;
    char calc;
};

int command_type(const char *cmd) {
    if (strcmp(cmd, "REM"))
        return 1;
    if (strcmp(cmd, "INPUT"))
        return 2;
    if (strcmp(cmd, "OUTPUT"))
        return 3;
    if (strcmp(cmd, "GOTO"))
        return 4;
    if (strcmp(cmd, "IF"))
        return 5;
    if (strcmp(cmd, "LET"))
        return 6;
    if (strcmp(cmd, "END"))
        return 7;
    return 0;
}

unsigned variable_id(const struct stVariables *vars, unsigned max, char var) {
    unsigned id = 0;
    for (; id < max; ++id)
        if (vars[id].name == var)
            return id;
    return max;
}

int basic_to_asm(const char* filename_bas, const char* filename_asm) {
    FILE *fbas = fopen(filename_bas, "r");
    if (!fbas) return 1;

    char *asm_code = (char *) malloc(100 * 16);
    asm_code[0] = 0;
    struct stLines *lines = (struct stLines *) malloc(sizeof(*lines) * 100);
    struct stVariables *var = (struct stVariables *) malloc(sizeof(*var) * 52);;
    struct stGotoConflict *_goto = (struct stGotoConflict *) malloc(sizeof(*_goto) * 100);;

    char *buffer = (char *) malloc(128);
    unsigned address = 0;
    unsigned line_id = 0;
    unsigned goto_id = 0;
    unsigned var_id = 0;
    unsigned line = 0;
    char fail = 0, end = 0, tmp_var = 'a';

    //A-Z - basic variables
    //a-z - temp variables

    while (end == 0 && fscanf(fbas, "%u %[A-Z]", &line, buffer) != 0) {
        if (line_id != 0 && line <= lines[line_id].line_number) {
            end = fail = 1;
            break;
        }
        int type = command_type(buffer);

        lines[line_id].line_number = line;
        lines[line_id].start_address = address;
        ++line_id;

        type_check:
        switch (type) {
            case 1:
                continue;
            case 2: {
                fscanf(fbas, "%[A-Z]", buffer);
                if (buffer[1] != 0 || !(buffer[0] >= 'A' && buffer[0] <= 'Z')) {
                    end = fail = 1;
                    break;
                }
                unsigned id = variable_id(var, var_id, buffer[0]);
                if (id == var_id) {
                    var[id].name = buffer[0];
                    var[id].address = 99 - id;
                    var[id].init_value = 0;
                    ++var_id;
                }
                sprintf(buffer, "%02u READ %02u\n", address, var[id].address);
                strcat(asm_code, buffer);
            }
                break;
            case 3: {
                fscanf(fbas, "%[A-Z]", buffer);
                if (buffer[1] != 0 || !(buffer[0] >= 'A' && buffer[0] <= 'Z')) {
                    end = fail = 1;
                    break;
                }
                unsigned id = variable_id(var, var_id, buffer[0]);
                if (id == var_id) {
                    var[id].name = buffer[0];
                    var[id].address = 99 - id;
                    var[id].init_value = 0;
                    ++var_id;
                }
                sprintf(buffer, "%02u WRITE %02u\n", address, var[id].address);
                strcat(asm_code, buffer);
            }
                break;
            case 4: {
                sprintf(buffer, "%02u JMP ", address);
                strcat(asm_code, buffer);
                fscanf(fbas, "%u", &line);
                if (line > lines[line_id - 1].line_number) {
                    _goto[goto_id].goto_line = line;
                    _goto[goto_id].instratuction_address = strlen(asm_code);
                    _goto[goto_id].calc = 0;
                    strcat(asm_code, "00\n");
                    ++goto_id;
                    continue;
                }
                int id = line_id - 1;
                while (id >= 0 && line != lines[id].line_number)
                    --id;
                if (id < 0) {
                    end = fail = 1;
                    break;
                }
                sprintf(buffer, "%02u\n", lines[id].start_address);
                strcat(asm_code, buffer);
            }
                break;
            case 5: {
                char op1[8], op2[8];
                fscanf(fbas, "%[0-9A-Z] %1[<=>] %[0-9A-Z]", op1, buffer, op2);
                if (isdigit(op1) && isdigit(op2)) {
                    int result = 0;
                    switch (buffer[0]) {
                        case '<':
                            result = atoi(op1) < atoi(op2);
                            break;
                        case '>':
                            result = atoi(op1) > atoi(op2);
                            break;
                        default:
                            result = atoi(op1) == atoi(op2);
                            break;
                    }
                    if (result == 0)
                        continue;
                    fscanf(fbas, "%[A-Z]", buffer);
                    type = command_type(buffer);
                    goto type_check;
                }

                //Определяем индексы переменных и регистрируем если требуется (могут быть времененные)
                int id1 = var_id, id2 = var_id;
                if (isdigit(op1)) {
                    var[id1].name = tmp_var;
                    var[id1].address = 99 - var_id;
                    var[id1].init_value = atoi(op1);
                    ++var_id;
                    ++tmp_var;

                    id2 = variable_id(var, var_id, op2[0]);
                    if (id2 == var_id) {
                        var[id2].name = op2[0];
                        var[id2].address = 99 - var_id;
                        var[id2].init_value = 0;
                        ++var_id;
                    }
                } else if (isdigit(op2)) {
                    var[id2].name = tmp_var;
                    var[id2].address = 99 - var_id;
                    var[id2].init_value = atoi(op2);
                    ++var_id;
                    ++tmp_var;

                    id1 = variable_id(var, var_id, op1[0]);
                    if (id1 == var_id) {
                        var[id1].name = op1[0];
                        var[id1].address = 99 - var_id;
                        var[id1].init_value = 0;
                        ++var_id;
                    }
                } else {
                    id1 = variable_id(var, var_id, op1[0]);
                    if (id1 == var_id) {
                        var[id1].name = op1[0];
                        var[id1].address = 99 - var_id;
                        var[id1].init_value = 0;
                        ++var_id;
                    }

                    id2 = variable_id(var, var_id, op2[0]);
                    if (id2 == var_id) {
                        var[id2].name = op2[0];
                        var[id2].address = 99 - var_id;
                        var[id2].init_value = 0;
                        ++var_id;
                    }
                }

                switch (buffer[0]) {
                    case '<':
                        //Если при вычитание из 2 1го число отрицательное, значит оно больше
                        sprintf(buffer, "%02u LOAD %02u\n%02u SUB %02u\n%02u JNEG 00\n",
                                address, var[id2].address,
                                address + 1, var[id1].address,
                                address + 2);
                        address += 4;
                        _goto[goto_id].goto_line = line + 1;
                        _goto[goto_id].instratuction_address = strlen(asm_code) + 29;
                        _goto[goto_id].calc = 1;
                        ++goto_id;
                        break;
                    case '>':
                        //Если при вычитание из 1 2e число отрицательное, значит оно больше
                        sprintf(buffer, "%02u LOAD %02u\n%02u SUB %02u\n%02u JNEG 00\n",
                                address, var[id1].address,
                                address + 1, var[id2].address,
                                address + 2);
                        address += 4;
                        _goto[goto_id].goto_line = line + 1;
                        _goto[goto_id].instratuction_address = strlen(asm_code) + 29;
                        _goto[goto_id].calc = 1;
                        ++goto_id;
                        break;
                    default:
                        //Если при вычитание из 2 1го число 0 - равны
                        sprintf(buffer, "%02u LOAD %02u\n%02u SUB %02u\n%02u JZ %02u\n%02u JUMP 00\n",
                                address, var[id2].address,
                                address + 1, var[id1].address,
                                address + 2, address + 4,
                                address + 3);
                        address += 4;
                        _goto[goto_id].goto_line = line + 1;
                        _goto[goto_id].instratuction_address = strlen(asm_code) + 38;
                        _goto[goto_id].calc = 1;
                        ++goto_id;
                        break;
                }
                strcat(asm_code, buffer);
                fscanf(fbas, "%[A-Z]", buffer);
                type = command_type(buffer);
                goto type_check;
            }
                break;
            case 6: {
                //LET
            }
                break;
            case 7:
                sprintf(buffer, "%02u HALT 00", address);
                strcat(asm_code, buffer);
                end = 1;
                break;
        }
        ++address;


        do { ignore = fgetc(fasm); }
        while (ignore != '\n' && ignore != EOF);
        if (ignore == EOF)
            break;
    }

    //Запись в память начальных значений + констатнт
    //Проход по конфликтным переходам и заполнение их

    free(buffer);
    free(_goto);
    free(var);
    free(lines);
    free(asm_code);
    return 0;
}