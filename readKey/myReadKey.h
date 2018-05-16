#ifndef SCOMPUTER_MYREADKEY_H
#define SCOMPUTER_MYREADKEY_H

typedef enum {
    VK_UNKNOWN,
    VK_F5 = 1,
    VK_F6,
    VK_UARROW,
    VK_DARROW,
    VK_RARROW,
    VK_LARROW,
    VK_LOAD,
    VK_SAVE,
    VK_RUN,
    VK_STEP,
    VK_RESET,
    VK_QUIT
} eKeys;

int rk_mytermregime(int regime, int vtime, int vmin, int echo, int sigint);
int rk_mytermsave (void);
int rk_mytermrestore (void);
int rk_readkey(eKeys *key);

#endif //SCOMPUTER_MYREADKEY_H