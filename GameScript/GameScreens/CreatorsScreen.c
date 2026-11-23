#include "raylib.h"
#include "../ScreenManadgement/crtscr.h"

static const char *titleText = "CRIADORES";
static const char *creator1Name = "Cristofer Dvorecky Oliveira";
static const char *creator1Id = "RA: 26020823";
static const char *creator2Name = "Augusto do Amarante";
static const char *creator2Id = "RA: 26018423";
static const char *backText = "VOLTAR";

static const int titleFontSize = 40;
static const int nameFontSize = 30;
static const int idFontSize = 20;

static int titlePosX = 0;
static int creator1NamePosX = 0;
static int creator1IdPosX = 0;
static int creator2NamePosX = 0;
static int creator2IdPosX = 0;
static Rectangle backBounds = { 0 };
static bool isHoveringBack = false;

void InitCreatorsScreen(int screenWidth) {
    titlePosX = (screenWidth - MeasureText(titleText, titleFontSize)) / 2;
    creator1NamePosX = (screenWidth - MeasureText(creator1Name, nameFontSize)) / 2;
    creator1IdPosX = (screenWidth - MeasureText(creator1Id, idFontSize)) / 2;
    creator2NamePosX = (screenWidth - MeasureText(creator2Name, nameFontSize)) / 2;
    creator2IdPosX = (screenWidth - MeasureText(creator2Id, idFontSize)) / 2;

    int backWidth = MeasureText(backText, idFontSize);
    backBounds = (Rectangle){ (screenWidth - backWidth) / 2.0f, 400, (float)backWidth, (float)idFontSize };
}

GameScreen UpdateCreatorsScreen(void) {
    Vector2 mousePos = GetMousePosition();
    isHoveringBack = CheckCollisionPointRec(mousePos, backBounds);

    if (isHoveringBack && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return SCREEN_MENU;
    }

    return SCREEN_CREATORS;
}

void DrawCreatorsScreen(void) {
    ClearBackground(BLACK);

    DrawText(titleText, titlePosX, 50, titleFontSize, WHITE);

    DrawText(creator1Name, creator1NamePosX, 150, nameFontSize, LIGHTGRAY);
    DrawText(creator1Id, creator1IdPosX, 190, idFontSize, GRAY);

    DrawText(creator2Name, creator2NamePosX, 250, nameFontSize, LIGHTGRAY);
    DrawText(creator2Id, creator2IdPosX, 290, idFontSize, GRAY);

    DrawText(backText, (int)backBounds.x, (int)backBounds.y, idFontSize, isHoveringBack ? GOLD : WHITE);
}