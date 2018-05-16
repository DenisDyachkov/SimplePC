#define F5 "\E[15~"
#define F6 "\E[17~"
#define UP_ARROW "\E[A"
#define DOWN_ARROW "\E[B"
#define RIGHT_ARROW "\E[C"
#define LEFT_ARROW "\E[D"

#include <termios.h>
#include <unistd.h>
#include <string.h>

typedef enum {
    VK_F5,
    VK_F6,
    VK_UARROW,
    VK_DARROW,
    VK_RARROW,
    VK_LARROW,
    VK_LOAD,
    VK_SAVE,
    VK_RUN,
    VK_STEP,
    VK_RESET
} eKeys;

static struct termios option;

int rk_mytermregime(int regime, int vtime, int vmin, int echo, int sigint) {
    struct termios option;
    if (tcgetattr(STDIN_FILENO, &option))
        return -1;
    if (regime)
        option.c_lflag |= ICANON;
    else {
        option.c_lflag &= ~ICANON;
        if (echo)
            option.c_lflag |= ECHO;
        else
            option.c_lflag &= ~ECHO;
        if (sigint)
            option.c_lflag |= ISIG;
        else
            option.c_lflag &= ~ISIG;
        option.c_cc[VMIN] = vmin;
        option.c_cc[VTIME] = vtime;

    }
    if (tcsetattr(STDIN_FILENO, TCSANOW, &option))
        return -1;
    return 0;
}

int rk_mytermsave (void) {
    return tcgetattr(STDIN_FILENO, &option);
}

int rk_mytermrestore (void) {
    return tcsetattr(STDIN_FILENO, TCSANOW, &option);
}

int rk_readkey(eKeys *press_key) {
    char key[8];
    struct termios old;
    if (rk_mytermsave() || rk_mytermregime(0, 0, 1, 0, 0))
        return -1;
    int reads = read(STDIN_FILENO, key, 7);
    if (reads <= 0)
        return -1;
    key[reads] = 0;
    if (key[0] == '\033') { // \E
        if (!strcmp(key, F5))
            *press_key = VK_F5;
        else if (!strcmp(key, F6))
            *press_key = VK_F6;
        else if (!strcmp(key, UP_ARROW))
            *press_key = VK_UARROW;
        else if (!strcmp(key, DOWN_ARROW))
            *press_key = VK_DARROW;
        else if (!strcmp(key, RIGHT_ARROW))
            *press_key = VK_RARROW;
        else if (!strcmp(key, LEFT_ARROW))
            *press_key = VK_LARROW;

    } else {
        // if (key[0] <= '9') //digit
        if (key[0] >= 'A' && key[0] <= 'Z')
            key[0] -= 'A' - 'a';
        switch (key[0]) {
            case 'l': *press_key = VK_LOAD; break;
            case 's': *press_key = VK_SAVE; break;
            case 'r': *press_key = VK_RUN; break;
            case 't': *press_key = VK_STEP; break;
            case 'i': *press_key = VK_RESET; break;
        }
    }
    return rk_mytermrestore();
}

int main() {
    eKeys key;
    rk_readkey(&key);
    return 0;
}
