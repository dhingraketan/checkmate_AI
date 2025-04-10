#ifndef _GAME_ENGINE_H_
#define _GAME_ENGINE_H_

typedef enum {
    CMD_POSITION,
    CMD_GO,
    CMD_IS_READY,
    CMD_D,
    CMD_NUM_CMDS
} GAME_ENGINE_CMDS;

void gameEngine_init();
void gameEngine_sendCmd(GAME_ENGINE_CMDS cmd, char *fenString, char *returnLine);
void gameEngine_cleanup();

#endif