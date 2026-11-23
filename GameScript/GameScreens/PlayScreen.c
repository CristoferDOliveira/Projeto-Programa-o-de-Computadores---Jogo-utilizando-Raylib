#include <string.h>
#include "raylib.h"
#include "../ScreenManadgement/playscr.h"
#include "../ScreenManadgement/player.h"
#include "../ScreenManadgement/story.h"
#include "../ScreenManadgement/combat.h"

#define FIRST_IDX 0
#define TEXT_BUF_LEN 512
#define LINE_BUF_LEN 256
#define WORD_BUF_LEN 64

static Player player;
static CombatState combat;
static bool isCharacterWindowOpen = false;
static int selectedDescItemIndex = -1;

static int winWidth = 800;
static int winHeight = 450;

static const char *charBtnText = "PERSONAGEM";
static Rectangle charBtnBounds = { 0 };
static bool isHoveringCharBtn = false;

static const char *backStoryBtnText = "VOLTAR PARA HISTORIA";
static Rectangle backStoryBtnBounds = { 0 };
static bool isHoveringBackStoryBtn = false;

static void TrimLeadingSpaces(char *str) {
    while (*str == ' ') {
        memmove(str, str + 1, strlen(str));
    }
}

static void TryStartCombatFromTarget(const char *target) {
    if (strncmp(target, "COMBAT:", 7) != 0) return;

    char buf[MAX_TARGET_LEN];
    strncpy(buf, target + 7, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char *enemyFile = strtok(buf, ";");
    char *winTarget = strtok(NULL, ";");
    char *loseTarget = strtok(NULL, ";");

    if (enemyFile != NULL) {
        TrimLeadingSpaces(enemyFile);
        if (winTarget) TrimLeadingSpaces(winTarget);
        if (loseTarget) TrimLeadingSpaces(loseTarget);
        InitCombat(&combat, &player, enemyFile, winTarget, loseTarget);
    }
}

static void DrawTextWrapped(const char *text, int posX, int posY, int maxWidth, int fontSize, Color color) {
    char buffer[TEXT_BUF_LEN];
    strncpy(buffer, text, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    int currentY = posY;
    char *line = strtok(buffer, "\n");

    while (line != NULL) {
        char currentLine[LINE_BUF_LEN] = "";
        char word[WORD_BUF_LEN];
        int i = 0, wIdx = 0;

        while (true) {
            char c = line[i];
            if (c == ' ' || c == '\0') {
                word[wIdx] = '\0';
                char testLine[LINE_BUF_LEN];
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
            } else if (wIdx < WORD_BUF_LEN - 1) {
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
    combat.active = false;
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

    if (isCharacterWindowOpen) {
        isHoveringBackStoryBtn = CheckCollisionPointRec(mousePos, backStoryBtnBounds);
        if ((isHoveringBackStoryBtn && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) || IsKeyPressed(KEY_ESCAPE)) {
            isCharacterWindowOpen = false;
            selectedDescItemIndex = -1;
            return SCREEN_PLAY;
        }

        Rectangle unequipWeaponBtn = { 230.0f, 285.0f, 22.0f, 18.0f };
        Rectangle unequipArmorBtn  = { 230.0f, 310.0f, 22.0f, 18.0f };
        Rectangle unequipShieldBtn = { 230.0f, 335.0f, 22.0f, 18.0f };

        if (CheckCollisionPointRec(mousePos, unequipWeaponBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            UnequipSlot(&player, ITEM_WEAPON);
        }
        if (CheckCollisionPointRec(mousePos, unequipArmorBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            UnequipSlot(&player, ITEM_ARMOR);
        }
        if (CheckCollisionPointRec(mousePos, unequipShieldBtn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            UnequipSlot(&player, ITEM_SHIELD);
        }

        for (int i = 0; i < player.uniqueItemCount; i++) {
            float rowY = 95.0f + (i * 21.0f);
            Rectangle btnUse  = { 650.0f, rowY, 26.0f, 18.0f };
            Rectangle btnDel  = { 682.0f, rowY, 26.0f, 18.0f };
            Rectangle btnDesc = { 714.0f, rowY, 26.0f, 18.0f };

            const ItemDef *def = GetItemDef(player.inventory[i].itemId);

            if (CheckCollisionPointRec(mousePos, btnUse) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (combat.active && combat.phase == TURN_PLAYER && combat.itemUsedThisTurn) {
                    break;
                }
                if (UseOrEquipItemAt(&player, i)) {
                    if (combat.active && combat.phase == TURN_PLAYER) {
                        NotifyPlayerItemUsed(&combat);
                    }
                }
                selectedDescItemIndex = -1;
                break;
            }
            if (CheckCollisionPointRec(mousePos, btnDel) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                RemoveItemAt(&player, i, 1);
                selectedDescItemIndex = -1;
                break;
            }
            if (def != NULL && def->description[FIRST_IDX] != '\0') {
                if (CheckCollisionPointRec(mousePos, btnDesc) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    selectedDescItemIndex = (selectedDescItemIndex == i) ? -1 : i;
                }
            }
        }
        return SCREEN_PLAY;
    }

    if (combat.active) {
        if (combat.phase == COMBAT_VICTORY || combat.phase == COMBAT_DEFEAT) {
            Rectangle btnContinue = { 50.0f, 300.0f, 220.0f, 36.0f };
            if (CheckCollisionPointRec(mousePos, btnContinue) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                char nextTarget[TARGET_LEN];
                if (combat.phase == COMBAT_VICTORY) {
                    strcpy(nextTarget, combat.winTarget);
                } else {
                    strcpy(nextTarget, combat.loseTarget);
                }
                combat.active = false;
                if (strncmp(nextTarget, "COMBAT:", 7) == 0) {
                    TryStartCombatFromTarget(nextTarget);
                } else {
                    NavigateToTarget(nextTarget, &player);
                }
            }
            return SCREEN_PLAY;
        }

        bool openInv = false;
        UpdateCombat(&combat, &player, mousePos, &openInv);
        if (openInv) {
            isCharacterWindowOpen = true;
        }
        return SCREEN_PLAY;
    }

    isHoveringCharBtn = CheckCollisionPointRec(mousePos, charBtnBounds);
    if (isHoveringCharBtn && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        isCharacterWindowOpen = true;
        return SCREEN_PLAY;
    }

    const StoryNode *node = GetCurrentStoryNode();
    for (int i = 0; i < node->choiceCount; i++) {
        Rectangle choiceBounds = { 50.0f, 260.0f + (i * 42.0f), 320.0f, 32.0f };
        if (CheckCollisionPointRec(mousePos, choiceBounds) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            const char *target = node->choices[i].nextTarget;
            if (strncmp(target, "COMBAT:", 7) == 0) {
                TryStartCombatFromTarget(target);
            } else {
                SelectStoryChoice(i, &player);
            }
            break;
        }
    }

    return SCREEN_PLAY;
}

void DrawPlayScreen(void) {
    ClearBackground(BLACK);
    Vector2 mousePos = GetMousePosition();

    if (isCharacterWindowOpen) {
        DrawRectangle(20, 15, winWidth - 40, 40, DARKGRAY);
        DrawText(TextFormat("ITENS UNICOS: %d/%d", player.uniqueItemCount, MAX_UNIQUE_ITEMS), 35, 25, 20, GOLD);
        DrawText(TextFormat("MOEDAS: %d", player.coins), 340, 25, 20, YELLOW);
        DrawText(TextFormat("COMIDA: %d", player.food), 540, 25, 20, GREEN);

        DrawRectangleLines(20, 65, 240, 325, GRAY);
        DrawText("STATUS", 30, 72, 18, SKYBLUE);
        DrawText(TextFormat("Forca: %d", player.totalStrength), 30, 98, 17, WHITE);
        DrawText(TextFormat("Agilidade: %d", player.totalAgility), 30, 122, 17, WHITE);
        DrawText(TextFormat("Sabedoria: %d", player.totalWisdom), 30, 146, 17, WHITE);
        DrawText(TextFormat("Carisma: %d", player.totalCarisma), 30, 170, 17, WHITE);
        DrawText(TextFormat("Resistencia: %d", player.totalResistance), 30, 194, 17, WHITE);

        DrawLine(25, 235, 255, 235, GRAY);
        DrawText("EQUIPADOS", 30, 250, 18, SKYBLUE);

        const ItemDef *wDef = GetItemDef(player.equippedWeaponId);
        const ItemDef *aDef = GetItemDef(player.equippedArmorId);
        const ItemDef *sDef = GetItemDef(player.equippedShieldId);

        DrawText(TextFormat("Arma: %s", wDef ? wDef->name : "Nenhuma"), 30, 285, 14, LIGHTGRAY);
        if (wDef) DrawText("[X]", 230, 285, 15, RED);

        DrawText(TextFormat("Armad.: %s", aDef ? aDef->name : "Nenhuma"), 30, 310, 14, LIGHTGRAY);
        if (aDef) DrawText("[X]", 230, 310, 15, RED);

        DrawText(TextFormat("Escudo: %s", sDef ? sDef->name : "Nenhum"), 30, 335, 14, LIGHTGRAY);
        if (sDef) DrawText("[X]", 230, 335, 15, RED);

        DrawRectangleLines(275, 65, 505, 325, GRAY);
        DrawText("INVENTARIO ([O] Usar/Equipar | [X] Apagar | [S] Info)", 290, 72, 15, SKYBLUE);

        for (int i = 0; i < player.uniqueItemCount; i++) {
            int rowY = 95 + (i * 21);
            const ItemDef *def = GetItemDef(player.inventory[i].itemId);
            const char *displayName = (def != NULL) ? def->name : player.inventory[i].itemId;

            DrawText(TextFormat("%s (%d/%d)", displayName, player.inventory[i].quantity, MAX_ITEM_STACK), 290, rowY, 17, WHITE);

            Rectangle btnUse = { 650.0f, (float)rowY, 26.0f, 18.0f };
            bool hoverUse = CheckCollisionPointRec(mousePos, btnUse);
            bool itemLocked = combat.active && combat.phase == TURN_PLAYER && combat.itemUsedThisTurn;
            Color useColor = itemLocked ? DARKGRAY : (hoverUse ? GREEN : LIGHTGRAY);
            DrawText("[O]", (int)btnUse.x, rowY, 17, useColor);

            Rectangle btnDel = { 682.0f, (float)rowY, 26.0f, 18.0f };
            bool hoverDel = CheckCollisionPointRec(mousePos, btnDel);
            DrawText("[X]", (int)btnDel.x, rowY, 17, hoverDel ? RED : LIGHTGRAY);

            if (def != NULL && def->description[FIRST_IDX] != '\0') {
                Rectangle btnDesc = { 714.0f, (float)rowY, 26.0f, 18.0f };
                bool hoverDesc = CheckCollisionPointRec(mousePos, btnDesc);
                DrawText("[S]", (int)btnDesc.x, rowY, 17, (hoverDesc || selectedDescItemIndex == i) ? GOLD : SKYBLUE);
            }
        }

        if (selectedDescItemIndex >= 0 && selectedDescItemIndex < player.uniqueItemCount) {
            const ItemDef *descDef = GetItemDef(player.inventory[selectedDescItemIndex].itemId);
            if (descDef != NULL) {
                DrawRectangle(40, 310, winWidth - 80, 70, BLACK);
                DrawRectangleLines(40, 310, winWidth - 80, 70, GOLD);
                DrawText(TextFormat("%s:", descDef->name), 55, 320, 18, GOLD);
                DrawText(descDef->description, 55, 345, 17, WHITE);
            }
        }

        DrawText(backStoryBtnText, (int)backStoryBtnBounds.x, (int)backStoryBtnBounds.y, 20, isHoveringBackStoryBtn ? GOLD : WHITE);
        return;
    }

    const char *hpText = TextFormat("VIDA: %d/%d", player.currentHp, player.maxHp);
    int hpWidth = MeasureText(hpText, 22);
    DrawText(hpText, winWidth - hpWidth - 20, 20, 22, RED);

    if (combat.active) {
        DrawCombat(&combat, &player, winWidth, mousePos);
        DrawText(charBtnText, (int)charBtnBounds.x, (int)charBtnBounds.y, 20, isHoveringCharBtn ? GOLD : WHITE);
        return;
    }

    const StoryNode *node = GetCurrentStoryNode();
    DrawTextWrapped(node->text, 50, 70, winWidth - 100, 21, WHITE);

    for (int i = 0; i < node->choiceCount; i++) {
        Rectangle choiceBounds = { 50.0f, 260.0f + (i * 42.0f), 320.0f, 32.0f };
        bool isHoveringChoice = CheckCollisionPointRec(mousePos, choiceBounds);

        DrawRectangleLinesEx(choiceBounds, 1, isHoveringChoice ? GOLD : GRAY);
        DrawText(TextFormat("> %s", node->choices[i].text), (int)choiceBounds.x + 10, (int)choiceBounds.y + 6, 20, isHoveringChoice ? GOLD : WHITE);
    }

    DrawText(charBtnText, (int)charBtnBounds.x, (int)charBtnBounds.y, 20, isHoveringCharBtn ? GOLD : WHITE);
}