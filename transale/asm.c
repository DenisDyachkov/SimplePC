#include "asm.h"
#include <mySimpleComputer.h>
#include <stdio.h>
#include <string.h>
#include <cpu.h>

int asm_to_object(const char* filename_asm, const char* filename_object) {
    FILE *fasm = fopen(filename_asm, "r");
    //FILE *fobj = fopen(filename".o", "wb");
    if (!fasm)// || !fobj)
        return 1;

    int address, operand, code, encode, ignore;
    char cmd[16] = { 0 };

    while (fscanf(fasm,"%d %[=a-zA-Z]", &address, cmd) != 0) {//"%d %s %d"
        if (cmd[0] != '=') {
            fscanf(fasm, "%d", &operand);
            code = cmd_search(cmd);
            if (code == -1 || sc_commandEncode(code, operand, &encode))
                return 1;
        } else {
            //char plus;
            //fscanf(fasm, "%c%x", &plus, &operand);
            fscanf(fasm, "%x", &encode);
            encode = 0x4000 | (encode & 0x3FFF);
        }
        //fwrite(&encode, sizeof(encode), 1, fobj);
        sc_memorySet(address, encode);

        do { ignore = fgetc(fasm); }
        while (ignore != '\n' && ignore != EOF);
        if (ignore == EOF)
            break;
    }
    fclose(fasm);
    if (cmd[0] == 0)
        return 1;
    //fclose(fobj);

    /*char object_file[strlen(filename)];
    strcpy(object_file, filename);
    object_file[strlen(filename) - 3] = 0;
    strcat(object_file, "o");*/
    sc_memorySave(filename_object);

    return 0;
}