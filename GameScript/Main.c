#include "raylib.h"
#include "ScreenManadgement/scrmgr.h"

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Marcus Quest");
    SetTargetFPS(60);

    InitScreens(screenWidth, screenHeight);
    ChangeScreen(SCREEN_MENU);

    while (!WindowShouldClose()) {
        UpdateCurrentScreen();

        BeginDrawing();
            DrawCurrentScreen();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
