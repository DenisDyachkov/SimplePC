#include "SimpleComputer.h"
#include <stdio.h>

int main() {
    sc_memoryInit();
    sc_regInit();
    int flag;
    printf("call sc_memorySet(0, 1234) return: %d\n", sc_memorySet(0, 1234));
    printf("call sc_memorySet(-4, 1) return: %d\n", sc_memorySet(-4, 1));
    printf("call sc_memorySet(1000, 1) return: %d\n", sc_memorySet(1000, 1));
    sc_regGet(FLAG_OUT_RANGE, &flag);
    printf("Flag OUT_RANGE: %d\n", flag);
    sc_regGet(FLAG_INVALID_COMMAND, &flag);
    printf("Flag INVALID_COMMAND: %d\n", flag);
    return 0;
}
