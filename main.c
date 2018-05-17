#include <stdio.h>
#include <myBigChars.h>
#include <mySimpleComputer.h>
#include <myReadKey.h>
#include "interface.h"

#include <signal.h>
#include <sys/time.h>

void create_timer(double interval);
void signalhangle(int signal);

int main() {
    eKeys key_down;
    int flag;
    signal(SIGUSR1, signalhangle);
    signal(SIGALRM, signalhangle);
    sc_memoryInit();
    sc_regInit();
    interface_load(WHITE, PURPLE, CYAN);
    sc_regSet(FLAG_IGNORE_CLOCK, 1);

    sc_memorySet(36, 0x7FFF);

    while (1) {
      //pause();
        interface_print();
        rk_readkey(&key_down);
        if (key_down == VK_RUN) {
            create_timer(1);
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
                ++instructionCounter;
                interface_print();
            }
        }
            break;
        case SIGUSR1: {
            sc_regInit();
            instructionCounter = 0;
            accumulator = 0;
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