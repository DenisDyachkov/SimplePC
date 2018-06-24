#include <stdio.h>
#include <myBigChars.h>
#include <mySimpleComputer.h>
#include <myReadKey.h>
#include <asm.h>
#include <basic.h>
#include "cpu.h"
#include "interface.h"

#include <signal.h>
#include <sys/time.h>
#include <memory.h>

void create_timer(double interval);
void signalhangle(int signal);

int main(int argc, char **argv) {
    if (argc > 1) {
        if (strcmp(argv[1], "-sat") == 0) {
            if (argc != 4) {
                printf("Error\n");
                return 1;
            }
            if (asm_to_object(argv[2], argv[3]) == 0)
                printf("Successful!\n");
            else
                printf("Fail!\n");
            return 0;
        } else if (strcmp(argv[1], "-sbt") == 0) {
            if (argc != 4) {
                printf("Error\n");
                return 1;
            }
            if (basic_to_asm(argv[2], argv[3]) == 0)
                printf("Successful!\n");
            else
                printf("Fail!\n");
            return 0;
        }
    }

    eKeys key_down;
    int flag;
    signal(SIGUSR1, signalhangle);
    signal(SIGALRM, signalhangle);
    sc_memoryInit();
    sc_regInit();
    interface_load(WHITE, PURPLE, CYAN);
    sc_regSet(FLAG_IGNORE_CLOCK, 1);

    while (1) {
        interface_print();
        rk_readkey(&key_down);
        if (key_down == VK_RUN) {
            sc_regSet(FLAG_IGNORE_CLOCK, 0);
            create_timer(0.25);
        } else if (key_down == VK_RESET) {
            create_timer(0);
            raise(SIGUSR1);
        } else if (key_down == VK_QUIT) {
            break;
        } else if (!sc_regGet(FLAG_IGNORE_CLOCK, &flag) && flag) {
            switch (key_down) {
                case VK_DARROW:
                    if (select_cell < 90)
                        select_cell += 10;
                    break;
                case VK_UARROW:
                    if (select_cell > 9)
                        select_cell -= 10;
                    break;
                case VK_LARROW:
                    if (select_cell > 0)
                        --select_cell;
                    break;
                case VK_RARROW:
                    if (select_cell < 99)
                        ++select_cell;
                    break;
                case VK_STEP:
                    CU();
                    break;
                case VK_LOAD: {
                    char filename[64];
                    read_console_filename(filename, 63);
                    sc_memoryLoad(filename);
                }
                    break;
                case VK_SAVE: {
                    char filename[64];
                    read_console_filename(filename, 63);
                    sc_memorySave(filename);
                }
                    break;
            }
        }
    }
    mt_reset();
    return 0;
}

void signalhangle(int signal) {
    switch (signal) {
        case SIGALRM: {
            int val;
            if (!sc_regGet(FLAG_IGNORE_CLOCK, &val) && !val) {
                CU();
                interface_print();
            }
        }
            break;
        case SIGUSR1: {
            sc_memoryInit();
            sc_regInit();
            registers.instruction_counter = 0;
            registers.accumulator = 0;
            sc_regSet(FLAG_IGNORE_CLOCK, 1);
        }
            break;
    }
}

void create_timer(double interval) {
    struct itimerval nval;
    nval.it_value.tv_sec = nval.it_interval.tv_sec = (long)interval;
    nval.it_value.tv_usec = nval.it_interval.tv_usec = (long)((interval - (long)interval) * 1000000);
    setitimer (ITIMER_REAL, &nval, NULL);
}