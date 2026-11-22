#ifndef SCRMGR_H
#define SCRMGR_H

// Telas principais do jogo (mesmo nome dos botoes)
typedef enum {
    SCREEN_MENU,
    SCREEN_PLAY,
    SCREEN_CREATORS
} GameScreen;

void InitScreens(int screenWidth, int screenHeight);
void ChangeScreen(GameScreen newScreen);
void UpdateCurrentScreen(void);
void DrawCurrentScreen(void);

#endif
