#include "raylib.h"
#include "../ScreenManadgement/scrmgr.h"

// Textos e tamanhos de fonte (privados deste arquivo)
static const char *gameTitle = "Marcus Quest";
static const char *playText = "PLAY";
static const char *repoText = "REPOSITORIO";
static const char *creatorsText = "CRIADORES";

static const int titleFontSize = 70;
static const int menuFontSize = 40;

// Variáveis de posição e colisão
static int titlePosX = 0;
static Rectangle playBounds = { 0 };
static Rectangle repoBounds = { 0 };
static Rectangle creatorsBounds = { 0 };

// Estados de hover do mouse
static bool isHoveringPlay = false;
static bool isHoveringRepo = false;
static bool isHoveringCreators = false;

void InitMenuScreen(int screenWidth) {
    titlePosX = (screenWidth - MeasureText(gameTitle, titleFontSize)) / 2;
    
    int playWidth = MeasureText(playText, menuFontSize);
    int repoWidth = MeasureText(repoText, menuFontSize);
    int creatorsWidth = MeasureText(creatorsText, menuFontSize);

    playBounds = (Rectangle){ (screenWidth - playWidth) / 2.0f, 180, (float)playWidth, (float)menuFontSize };
    repoBounds = (Rectangle){ (screenWidth - repoWidth) / 2.0f, 250, (float)repoWidth, (float)menuFontSize };
    creatorsBounds = (Rectangle){ (screenWidth - creatorsWidth) / 2.0f, 320, (float)creatorsWidth, (float)menuFontSize };
}

GameScreen UpdateMenuScreen(void) {
    Vector2 mousePos = GetMousePosition();

    // Verifica se o mouse está em cima de cada botão
    isHoveringPlay = CheckCollisionPointRec(mousePos, playBounds);
    isHoveringRepo = CheckCollisionPointRec(mousePos, repoBounds);
    isHoveringCreators = CheckCollisionPointRec(mousePos, creatorsBounds);

    // Verifica os cliques do mouse
    if (isHoveringPlay && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // Retorna a tela de jogo quando você criá-la (ex: SCREEN_GAMEPLAY)
    }
    if (isHoveringRepo && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        OpenURL("https://github.com");
    }
    if (isHoveringCreators && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return SCREEN_CREATORS; // Avisa o gerenciador para trocar de tela
    }

    return SCREEN_MENU; // Continua no menu se nada foi clicado
}

void DrawMenuScreen(void) {
    ClearBackground(BLACK);

    DrawText(gameTitle, titlePosX, 30, titleFontSize, RED);
    DrawText(playText, (int)playBounds.x, (int)playBounds.y, menuFontSize, isHoveringPlay ? GOLD : WHITE);
    DrawText(repoText, (int)repoBounds.x, (int)repoBounds.y, menuFontSize, isHoveringRepo ? GOLD : WHITE);
    DrawText(creatorsText, (int)creatorsBounds.x, (int)creatorsBounds.y, menuFontSize, isHoveringCreators ? GOLD : WHITE);
}
