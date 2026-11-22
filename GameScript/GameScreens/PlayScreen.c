#include <string.h>
#include "raylib.h"
#include "../ScreenManadgement/playscr.h"
#include "../ScreenManadgement/player.h"
#include "../ScreenManadgement/story.h"

static Player player;
static bool isCharacterWindowOpen = false;
static int selectedDescItemIndex = -1; // Guarda qual item teve o botao [S] clicado

static int winWidth = 800;
static int winHeight = 450;

// Botoes da tela padrao
static const char *charBtnText = "PERSONAGEM";
static Rectangle charBtnBounds = { 0 };
static bool isHoveringCharBtn = false;

// Botao de voltar dentro da tela de personagem
static const char *backStoryBtnText = "VOLTAR PARA HISTORIA";
static Rectangle backStoryBtnBounds = { 0 };
static bool isHoveringBackStoryBtn = false;

// Funcao auxiliar para quebrar linhas automaticamente no texto da historia
static void DrawTextWrapped(const char *text, int posX, int posY, int maxWidth, int fontSize, Color color) {
    char buffer[512];
    strncpy(buffer, text, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    int currentY = posY;
    char *line = strtok(buffer, "\n");

    while (line != NULL) {
        char currentLine[256] = "";
        char word[64];
        int i = 0, wIdx = 0;

        while (true) {
            char c = line[i];
            if (c == ' ' || c == '\0') {
                word[wIdx] = '\0';
                char testLine[256];
                if (strlen(currentLine) == 0) {
                    strcpy(testLine, word);
                } else {
                    TextCopy(testLine, TextFormat("%s %s", currentLine, word));
                }

                if (MeasureText(testLine, fontSize) > maxWidth && strlen(currentLine) > 0) {
                    DrawText(currentLine, posX, currentY, fontSize, color);
                    currentY += fontSize + 6;
                    strcpy(currentLine, word);
                } else {
                    strcpy(currentLine, testLine);
                }
                wIdx = 0;
                if (c == '\0') break;
            } else if (wIdx < 63) {
                word[wIdx++] = c;
            }
            i++;
        }
        if (strlen(currentLine) > 0) {
            DrawText(currentLine, posX, currentY, fontSize, color);
            currentY += fontSize + 8;
        }
        line = strtok(NULL, "\n");
    }
}

void ResetPlayScreen(void) {
    InitPlayer(&player);
    InitStory();
    isCharacterWindowOpen = false;
    selectedDescItemIndex = -1;
}

void InitPlayScreen(int screenWidth, int screenHeight) {
    winWidth = screenWidth;
    winHeight = screenHeight;
    ResetPlayScreen();

    int hudFontSize = 20;
    int btnWidth = MeasureText(charBtnText, hudFontSize);
    charBtnBounds = (Rectangle){
        (float)(winWidth - btnWidth - 20),
        (float)(winHeight - hudFontSize - 20),
        (float)btnWidth,
        (float)hudFontSize
    };

    int backWidth = MeasureText(backStoryBtnText, hudFontSize);
    backStoryBtnBounds = (Rectangle){
        (float)(winWidth - backWidth) / 2.0f,
        (float)(winHeight - 40),
        (float)backWidth,
        (float)hudFontSize
    };
}

GameScreen UpdatePlayScreen(void) {
    Vector2 mousePos = GetMousePosition();

    // --- LOGICA DA TELA DE PERSONAGEM ---
    if (isCharacterWindowOpen) {
        // Botao de voltar para a historia
        isHoveringBackStoryBtn = CheckCollisionPointRec(mousePos, backStoryBtnBounds);
        if ((isHoveringBackStoryBtn && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) || IsKeyPressed(KEY_ESCAPE)) {
            isCharacterWindowOpen = false;
            selectedDescItemIndex = -1;
            return SCREEN_PLAY;
        }

        // Botoes [O], [X] e [S] de cada item na coluna direita
        for (int i = 0; i < player.uniqueItemCount; i++) {
            float rowY = 95.0f + (i * 21.0f);
            Rectangle btnUse = { 650.0f, rowY, 26.0f, 18.0f };
            Rectangle btnDel = { 682.0f, rowY, 26.0f, 18.0f };
            Rectangle btnDesc = { 714.0f, rowY, 26.0f, 18.0f };

            if (CheckCollisionPointRec(mousePos, btnUse) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                UseItem(&player, i);
                selectedDescItemIndex = -1;
                break;
            }
            if (CheckCollisionPointRec(mousePos, btnDel) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                RemoveItem(&player, i, 1);
                selectedDescItemIndex = -1;
                break;
            }
            // Botao [S] so funciona se o item tiver descricao
            if (player.inventory[i].description[0] != '\0') {
                if (CheckCollisionPointRec(mousePos, btnDesc) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    selectedDescItemIndex = (selectedDescItemIndex == i) ? -1 : i;
                }
            }
        }
        return SCREEN_PLAY;
    }

    // --- LOGICA DA TELA PADRAO DE HISTORIA ---
    isHoveringCharBtn = CheckCollisionPointRec(mousePos, charBtnBounds);
    if (isHoveringCharBtn && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        isCharacterWindowOpen = true;
        return SCREEN_PLAY;
    }

    // Verifica cliques nos botoes de escolha da historia
    const StoryNode *node = GetCurrentStoryNode();
    for (int i = 0; i < node->choiceCount; i++) {
        Rectangle choiceBounds = { 50.0f, 260.0f + (i * 42.0f), 320.0f, 32.0f };
        if (CheckCollisionPointRec(mousePos, choiceBounds) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            SelectStoryChoice(i, &player);
            break;
        }
    }

    return SCREEN_PLAY;
}

void DrawPlayScreen(void) {
    ClearBackground(BLACK);
    Vector2 mousePos = GetMousePosition();

    // --- TELA DE PERSONAGEM ABERTA ---
    if (isCharacterWindowOpen) {
        // Parte Superior: Itens unicos (x/15), Moedas e Comida
        DrawRectangle(20, 15, winWidth - 40, 40, DARKGRAY);
        DrawText(TextFormat("ITENS UNICOS: %d/%d", player.uniqueItemCount, MAX_UNIQUE_ITEMS), 35, 25, 20, GOLD);
        DrawText(TextFormat("MOEDAS: %d", player.coins), 340, 25, 20, YELLOW);
        DrawText(TextFormat("COMIDA: %d", player.food), 540, 25, 20, GREEN);

        // Coluna Esquerda: Status
        DrawRectangleLines(20, 65, 240, 325, GRAY);
        DrawText("STATUS", 35, 75, 20, SKYBLUE);
        DrawText(TextFormat("Forca: %d", player.strength), 35, 115, 20, WHITE);
        DrawText(TextFormat("Agilidade: %d", player.agility), 35, 150, 20, WHITE);
        DrawText(TextFormat("Sabedoria: %d", player.wisdom), 35, 185, 20, WHITE);
        DrawText(TextFormat("Carisma: %d", player.charisma), 35, 220, 20, WHITE);
        DrawText(TextFormat("Resistencia: %d", player.resistance), 35, 255, 20, WHITE);

        // Coluna Direita: Inventario (Limite 15 itens, 10 de cada)
        DrawRectangleLines(275, 65, 505, 325, GRAY);
        DrawText("INVENTARIO ([O] Usar | [X] Apagar | [S] Info)", 290, 72, 16, SKYBLUE);

        for (int i = 0; i < player.uniqueItemCount; i++) {
            int rowY = 95 + (i * 21);
            InventoryItem *item = &player.inventory[i];

            DrawText(TextFormat("%s (%d/%d)", item->name, item->quantity, MAX_ITEM_STACK), 290, rowY, 17, WHITE);

            // Botao [O] Usar
            Rectangle btnUse = { 650.0f, (float)rowY, 26.0f, 18.0f };
            bool hoverUse = CheckCollisionPointRec(mousePos, btnUse);
            DrawText("[O]", (int)btnUse.x, rowY, 17, hoverUse ? GREEN : LIGHTGRAY);

            // Botao [X] Apagar
            Rectangle btnDel = { 682.0f, (float)rowY, 26.0f, 18.0f };
            bool hoverDel = CheckCollisionPointRec(mousePos, btnDel);
            DrawText("[X]", (int)btnDel.x, rowY, 17, hoverDel ? RED : LIGHTGRAY);

            // Botao [S] Descricao (aparece APENAS se o item tiver descricao)
            if (item->description[0] != '\0') {
                Rectangle btnDesc = { 714.0f, (float)rowY, 26.0f, 18.0f };
                bool hoverDesc = CheckCollisionPointRec(mousePos, btnDesc);
                DrawText("[S]", (int)btnDesc.x, rowY, 17, (hoverDesc || selectedDescItemIndex == i) ? GOLD : SKYBLUE);
            }
        }

        // Caixa flutuante de leitura da descricao quando [S] esta ativo
        if (selectedDescItemIndex >= 0 && selectedDescItemIndex < player.uniqueItemCount) {
            DrawRectangle(40, 310, winWidth - 80, 70, BLACK);
            DrawRectangleLines(40, 310, winWidth - 80, 70, GOLD);
            DrawText(TextFormat("%s:", player.inventory[selectedDescItemIndex].name), 55, 320, 18, GOLD);
            DrawText(player.inventory[selectedDescItemIndex].description, 55, 345, 17, WHITE);
        }

        // Botao inferior para voltar a historia
        DrawText(backStoryBtnText, (int)backStoryBtnBounds.x, (int)backStoryBtnBounds.y, 20, isHoveringBackStoryBtn ? GOLD : WHITE);
        return;
    }

    // --- TELA PADRAO DE HISTORIA ---
    // 1. Canto superior direito: Vida x/x
    const char *hpText = TextFormat("VIDA: %d/%d", player.currentHp, player.maxHp);
    int hpWidth = MeasureText(hpText, 22);
    DrawText(hpText, winWidth - hpWidth - 20, 20, 22, RED);

    // 2. Texto da historia atual
    const StoryNode *node = GetCurrentStoryNode();
    DrawTextWrapped(node->text, 50, 70, winWidth - 100, 21, WHITE);

    // 3. Botoes de escolha da historia
    for (int i = 0; i < node->choiceCount; i++) {
        Rectangle choiceBounds = { 50.0f, 260.0f + (i * 42.0f), 320.0f, 32.0f };
        bool isHoveringChoice = CheckCollisionPointRec(mousePos, choiceBounds);

        DrawRectangleLinesEx(choiceBounds, 1, isHoveringChoice ? GOLD : GRAY);
        DrawText(TextFormat("> %s", node->choices[i].text), (int)choiceBounds.x + 10, (int)choiceBounds.y + 6, 20, isHoveringChoice ? GOLD : WHITE);
    }

    // 4. Canto inferior direito: Botao "PERSONAGEM"
    DrawText(charBtnText, (int)charBtnBounds.x, (int)charBtnBounds.y, 20, isHoveringCharBtn ? GOLD : WHITE);
}
