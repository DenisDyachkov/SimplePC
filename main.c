#include <stdio.h>
#include <myBigChars.h>
#include <mySimpleComputer.h>
#include <myReadKey.h>
#include "interface.h"

#include <signal.h>
#include <sys/time.h>

void signalhangle(int signal) {
    switch (signal) {
        case SIGALRM: {
            int val;
            if (!sc_regGet(FLAG_IGNORE_CLOCK, &val) && !val) {
                ++instructionCounter;
                printCounter();
                printf("\n");//fix refresh
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

int main() {
    eKeys key_down;
    int flag;
    sc_memoryInit();
    sc_regInit();

    sc_memorySet(36, 0x7FFF);

    interface_load(WHITE, PURPLE);

    signal(SIGUSR1, signalhangle);
    signal(SIGALRM, signalhangle);

    while (1) {
      //pause();
        rk_readkey(&key_down);
        if (key_down == VK_RUN) {
            create_timer(1);
        } else if (key_down == VK_RESET) {
            create_timer(0);
            raise(SIGUSR1);
        } else if (key_down == VK_QUIT) {
            break;
        } else if (!sc_regGet(FLAG_IGNORE_CLOCK, &flag) && flag) {
            //TODO:
        }
    }
    return 0;
}