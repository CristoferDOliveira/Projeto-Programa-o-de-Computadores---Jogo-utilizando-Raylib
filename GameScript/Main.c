#include "raylib.h"
#include "./ScreenManadgement/scrmgr.h"

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    // 1. Inicia a janela e o FPS
    InitWindow(screenWidth, screenHeight, "Marcus Quest");
    SetTargetFPS(60);

    // 2. Prepara todas as telas e define o Menu como inicial
    InitScreens(screenWidth);
    ChangeScreen(SCREEN_MENU);

    // 3. Loop principal do jogo
    while (!WindowShouldClose()) {
        // Atualiza a tela atual
        UpdateCurrentScreen();

        // Desenha a tela atual
        BeginDrawing();
            DrawCurrentScreen();
        EndDrawing();
    }

    // 4. Fecha o jogo
    CloseWindow();
    return 0;
}
