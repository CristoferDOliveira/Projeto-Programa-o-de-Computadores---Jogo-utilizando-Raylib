#ifndef SCRMGR_H
#define SCRMGR_H

// Lista de todas as telas do jogo
typedef enum {
    SCREEN_MENU,
    SCREEN_CREATORS
} GameScreen;

// Inicializa todas as telas do jogo
void InitScreens(int screenWidth);

// Muda manualmente a tela ativa
void ChangeScreen(GameScreen newScreen);

// Atualiza a lógica da tela que está ativa no momento
void UpdateCurrentScreen(void);

// Desenha a tela que está ativa no momento
void DrawCurrentScreen(void);

#endif
