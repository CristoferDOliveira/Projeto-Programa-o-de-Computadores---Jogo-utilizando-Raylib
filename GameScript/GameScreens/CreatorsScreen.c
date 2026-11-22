#include "raylib.h"
#include "../ScreenManadgement/crtscr.h"

// Textos e tamanhos de fonte (privados deste arquivo)
static const char *titleText = "CRIADORES";
static const char *creatorName = "Cristofer Dvorecky Oliveira";
static const char *creatorId = "RA: 26020823";
static const char *backText = "VOLTAR";

static const int titleFontSize = 40;
static const int nameFontSize = 30;
static const int idFontSize = 20;

// Variáveis de posição e colisão
static int titlePosX = 0;
static int namePosX = 0;
static int idPosX = 0;
static Rectangle backBounds = { 0 };

// Estado de hover do mouse
static bool isHoveringBack = false;

void InitCreatorsScreen(int screenWidth) {
    titlePosX = (screenWidth - MeasureText(titleText, titleFontSize)) / 2;
    namePosX = (screenWidth - MeasureText(creatorName, nameFontSize)) / 2;
    idPosX = (screenWidth - MeasureText(creatorId, idFontSize)) / 2;

    int backWidth = MeasureText(backText, idFontSize);
    backBounds = (Rectangle){ (screenWidth - backWidth) / 2.0f, 380, (float)backWidth, (float)idFontSize };
}

GameScreen UpdateCreatorsScreen(void) {
    Vector2 mousePos = GetMousePosition();
    isHoveringBack = CheckCollisionPointRec(mousePos, backBounds);

    // Se clicar em voltar, avisa o gerenciador para ir ao menu
    if (isHoveringBack && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return SCREEN_MENU;
    }

    return SCREEN_CREATORS;
}

void DrawCreatorsScreen(void) {
    ClearBackground(BLACK);

    DrawText(titleText, titlePosX, 50, titleFontSize, WHITE);
    DrawText(creatorName, namePosX, 170, nameFontSize, LIGHTGRAY);
    DrawText(creatorId, idPosX, 220, idFontSize, GRAY);

    DrawText(backText, (int)backBounds.x, (int)backBounds.y, idFontSize, isHoveringBack ? GOLD : WHITE);
}
