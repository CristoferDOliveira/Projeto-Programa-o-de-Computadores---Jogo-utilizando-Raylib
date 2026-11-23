#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ScreenManadgement/combat.h"

#define ENEMIES_BASE_PATH "GameScript/GameData/Database/Characters/"
#define MAX_PATH_LEN 256
#define MAX_LINE_LEN 256
#define FOOD_HEAL_AMOUNT 3
#define MIN_DAMAGE 1
#define QTE_DURATION_SEC 0.7f

static void TrimNewline(char *str) {
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
}

static char* TrimSpaces(char *str) {
    while (*str == ' ') str++;
    int len = (int)strlen(str);
    while (len > 0 && str[len - 1] == ' ') {
        str[--len] = '\0';
    }
    return str;
}

static void StartPlayerTurn(CombatState *combat, Player *player) {
    combat->phase = TURN_PLAYER;
    combat->playerTurns++;
    combat->itemUsedThisTurn = false;
    TickStaffBuff(player);
}

static void StartEnemyTurn(CombatState *combat) {
    combat->phase = TURN_ENEMY_QTE;
    combat->enemyTurns++;
    combat->qteMaxTime = QTE_DURATION_SEC;
    combat->qteTimer = QTE_DURATION_SEC;
    combat->targetDirection = (GetRandomValue(0, 1) == 0) ? DODGE_LEFT : DODGE_RIGHT;
    snprintf(combat->logText, sizeof(combat->logText), "%s esta atacando! Desvie!", combat->enemy.name);
}

static void LoadEnemyFile(EnemyData *enemy, const char *enemyFile) {
    strcpy(enemy->name, "Inimigo");
    enemy->maxHp = 10;
    enemy->currentHp = 10;
    enemy->attackDamage = 3;
    enemy->agility = 0;
    enemy->resistance = 0;
    enemy->rewardCoins = 0;
    strcpy(enemy->rewardItemId, "none");

    char fullPath[MAX_PATH_LEN];
    snprintf(fullPath, sizeof(fullPath), "%s%s", ENEMIES_BASE_PATH, enemyFile);

    FILE *file = fopen(fullPath, "r");
    if (file == NULL) return;

    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), file) != NULL) {
        TrimNewline(line);
        if (strncmp(line, "NAME:", 5) == 0) {
            strncpy(enemy->name, TrimSpaces(line + 5), ENEMY_NAME_LEN - 1);
            enemy->name[ENEMY_NAME_LEN - 1] = '\0';
        } else if (strncmp(line, "HP:", 3) == 0) {
            enemy->maxHp = atoi(TrimSpaces(line + 3));
            enemy->currentHp = enemy->maxHp;
        } else if (strncmp(line, "DAMAGE:", 7) == 0) {
            enemy->attackDamage = atoi(TrimSpaces(line + 7));
        } else if (strncmp(line, "AGILITY:", 8) == 0) {
            enemy->agility = atoi(TrimSpaces(line + 8));
        } else if (strncmp(line, "RESISTANCE:", 11) == 0) {
            enemy->resistance = atoi(TrimSpaces(line + 11));
        } else if (strncmp(line, "REWARD_COINS:", 13) == 0) {
            enemy->rewardCoins = atoi(TrimSpaces(line + 13));
        } else if (strncmp(line, "REWARD_ITEM:", 12) == 0) {
            strncpy(enemy->rewardItemId, TrimSpaces(line + 12), ID_LEN - 1);
            enemy->rewardItemId[ID_LEN - 1] = '\0';
        }
    }

    fclose(file);
}

static void CheckCombatEnd(CombatState *combat, Player *player) {
    if (combat->enemy.currentHp <= 0) {
        combat->enemy.currentHp = 0;
        combat->phase = COMBAT_VICTORY;
        player->coins += combat->enemy.rewardCoins;
        if (strcmp(combat->enemy.rewardItemId, "none") != 0) {
            AddItemById(player, combat->enemy.rewardItemId, 1);
        }
        snprintf(combat->logText, sizeof(combat->logText), "Vitoria! +%d moedas", combat->enemy.rewardCoins);
    } else if (player->currentHp <= 0) {
        player->currentHp = 0;
        combat->phase = COMBAT_DEFEAT;
        snprintf(combat->logText, sizeof(combat->logText), "Derrota...");
    }
}

void InitCombat(CombatState *combat, Player *player, const char *enemyFile, const char *winTarget, const char *loseTarget) {
    combat->active = true;
    combat->playerTurns = 0;
    combat->enemyTurns = 0;
    combat->itemUsedThisTurn = false;
    combat->qteTimer = 0.0f;
    combat->qteMaxTime = QTE_DURATION_SEC;
    combat->targetDirection = DODGE_LEFT;

    LoadEnemyFile(&combat->enemy, enemyFile);

    strncpy(combat->winTarget, winTarget ? winTarget : "Prologue/P1.txt", TARGET_LEN - 1);
    combat->winTarget[TARGET_LEN - 1] = '\0';

    strncpy(combat->loseTarget, loseTarget ? loseTarget : "Prologue/P1.txt", TARGET_LEN - 1);
    combat->loseTarget[TARGET_LEN - 1] = '\0';

    if (player->totalAgility >= combat->enemy.agility) {
        StartPlayerTurn(combat, player);
        snprintf(combat->logText, sizeof(combat->logText), "Combate iniciado! Turno de Marcus.");
    } else {
        StartEnemyTurn(combat);
    }
}

void ExecutePlayerAttack(CombatState *combat, Player *player) {
    if (!combat->active || combat->phase != TURN_PLAYER) return;

    if (strcmp(player->equippedWeaponId, "arco") == 0) {
        int arrowIdx = -1;
        for (int i = 0; i < player->uniqueItemCount; i++) {
            if (strcmp(player->inventory[i].itemId, "flechas") == 0 && player->inventory[i].quantity > 0) {
                arrowIdx = i;
                break;
            }
        }
        if (arrowIdx == -1) {
            snprintf(combat->logText, sizeof(combat->logText), "Sem flechas para atirar!");
            return;
        }
        RemoveItemAt(player, arrowIdx, 1);
    }

    int rawDamage = GetWeaponDamage(player);
    int finalDamage = rawDamage - combat->enemy.resistance;
    if (finalDamage < MIN_DAMAGE) finalDamage = MIN_DAMAGE;

    combat->enemy.currentHp -= finalDamage;
    snprintf(combat->logText, sizeof(combat->logText), "Marcus causou %d de dano em %s!", finalDamage, combat->enemy.name);

    CheckCombatEnd(combat, player);
    if (combat->phase == TURN_PLAYER) {
        StartEnemyTurn(combat);
    }
}

void ExecutePlayerFood(CombatState *combat, Player *player) {
    if (!combat->active || combat->phase != TURN_PLAYER) return;

    if (player->food <= 0) {
        snprintf(combat->logText, sizeof(combat->logText), "Sem comida disponivel!");
        return;
    }

    if (player->currentHp >= player->maxHp) {
        snprintf(combat->logText, sizeof(combat->logText), "Vida ja esta cheia!");
        return;
    }

    player->food--;
    player->currentHp += FOOD_HEAL_AMOUNT;
    if (player->currentHp > player->maxHp) {
        player->currentHp = player->maxHp;
    }

    snprintf(combat->logText, sizeof(combat->logText), "Marcus comeu e recuperou %d de vida!", FOOD_HEAL_AMOUNT);
    StartEnemyTurn(combat);
}

void NotifyPlayerItemUsed(CombatState *combat) {
    if (combat->active && combat->phase == TURN_PLAYER) {
        combat->itemUsedThisTurn = true;
        snprintf(combat->logText, sizeof(combat->logText), "Item usado neste turno.");
    }
}

void ResolveEnemyAttack(CombatState *combat, Player *player, bool dodged) {
    if (!combat->active || combat->phase != TURN_ENEMY_QTE) return;

    int dmg = 0;
    if (dodged) {
        dmg = combat->enemy.attackDamage / 2;
        if (dmg < MIN_DAMAGE) dmg = MIN_DAMAGE;
        snprintf(combat->logText, sizeof(combat->logText), "Desvio! Dano reduzido para %d.", dmg);
    } else {
        dmg = combat->enemy.attackDamage - player->totalResistance;
        if (dmg < MIN_DAMAGE) dmg = MIN_DAMAGE;
        snprintf(combat->logText, sizeof(combat->logText), "%s causou %d de dano em Marcus!", combat->enemy.name, dmg);
    }

    player->currentHp -= dmg;
    CheckCombatEnd(combat, player);
    if (combat->phase == TURN_ENEMY_QTE) {
        StartPlayerTurn(combat, player);
    }
}

void UpdateCombat(CombatState *combat, Player *player, Vector2 mousePos, bool *openInventory) {
    if (!combat->active) return;

    if (combat->phase == TURN_ENEMY_QTE) {
        combat->qteTimer -= GetFrameTime();

        Rectangle btnLeft  = { 150.0f, 260.0f, 120.0f, 44.0f };
        Rectangle btnRight = { 310.0f, 260.0f, 120.0f, 44.0f };

        bool clickedLeft  = CheckCollisionPointRec(mousePos, btnLeft) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        bool clickedRight = CheckCollisionPointRec(mousePos, btnRight) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if (clickedLeft) {
            ResolveEnemyAttack(combat, player, combat->targetDirection == DODGE_LEFT);
            return;
        }
        if (clickedRight) {
            ResolveEnemyAttack(combat, player, combat->targetDirection == DODGE_RIGHT);
            return;
        }
        if (combat->qteTimer <= 0.0f) {
            ResolveEnemyAttack(combat, player, false);
            return;
        }
        return;
    }

    if (combat->phase == TURN_PLAYER) {
        Rectangle btnAttack = { 50.0f, 300.0f, 150.0f, 36.0f };
        Rectangle btnItems  = { 220.0f, 300.0f, 150.0f, 36.0f };
        Rectangle btnFood   = { 390.0f, 300.0f, 180.0f, 36.0f };

        if (CheckCollisionPointRec(mousePos, btnAttack) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            ExecutePlayerAttack(combat, player);
        } else if (CheckCollisionPointRec(mousePos, btnItems) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (combat->itemUsedThisTurn) {
                snprintf(combat->logText, sizeof(combat->logText), "Voce ja usou um item neste turno!");
            } else {
                *openInventory = true;
            }
        } else if (CheckCollisionPointRec(mousePos, btnFood) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            ExecutePlayerFood(combat, player);
        }
    }
}

void DrawCombat(const CombatState *combat, const Player *player, int screenWidth, Vector2 mousePos) {
    (void)screenWidth;
    if (!combat->active) return;

    DrawText(TextFormat("INIMIGO: %s", combat->enemy.name), 50, 75, 24, RED);
    DrawText(TextFormat("VIDA INIMIGO: %d/%d", combat->enemy.currentHp, combat->enemy.maxHp), 50, 108, 20, WHITE);
    DrawText(TextFormat("TURNOS MARCUS: %d | TURNOS INIMIGO: %d", combat->playerTurns, combat->enemyTurns), 50, 140, 18, SKYBLUE);
    DrawText(combat->logText, 50, 185, 20, GOLD);

    if (combat->phase == TURN_ENEMY_QTE) {
        const char *cueArrow = (combat->targetDirection == DODGE_LEFT) ? "<-" : "->";
        int cueX = (combat->targetDirection == DODGE_LEFT) ? 195 : 355;
        DrawText(cueArrow, cueX, 222, 32, GREEN);

        Rectangle btnLeft  = { 150.0f, 260.0f, 120.0f, 44.0f };
        Rectangle btnRight = { 310.0f, 260.0f, 120.0f, 44.0f };

        bool hoverLeft  = CheckCollisionPointRec(mousePos, btnLeft);
        bool hoverRight = CheckCollisionPointRec(mousePos, btnRight);

        DrawRectangleLinesEx(btnLeft, 1, hoverLeft ? GOLD : WHITE);
        DrawText("<-", (int)btnLeft.x + 45, (int)btnLeft.y + 10, 24, hoverLeft ? GOLD : WHITE);

        DrawRectangleLinesEx(btnRight, 1, hoverRight ? GOLD : WHITE);
        DrawText("->", (int)btnRight.x + 45, (int)btnRight.y + 10, 24, hoverRight ? GOLD : WHITE);

        float ratio = combat->qteTimer / combat->qteMaxTime;
        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;

        Rectangle barBg = { 150.0f, 318.0f, 280.0f, 18.0f };
        Rectangle barFill = { 150.0f, 318.0f, 280.0f * ratio, 18.0f };
        DrawRectangleRec(barFill, GREEN);
        DrawRectangleLinesEx(barBg, 1, WHITE);
        return;
    }

    if (combat->phase == TURN_PLAYER) {
        Rectangle btnAttack = { 50.0f, 300.0f, 150.0f, 36.0f };
        Rectangle btnItems  = { 220.0f, 300.0f, 150.0f, 36.0f };
        Rectangle btnFood   = { 390.0f, 300.0f, 180.0f, 36.0f };

        bool hoverAttack = CheckCollisionPointRec(mousePos, btnAttack);
        bool hoverItems  = CheckCollisionPointRec(mousePos, btnItems);
        bool hoverFood   = CheckCollisionPointRec(mousePos, btnFood);

        DrawRectangleLinesEx(btnAttack, 1, hoverAttack ? GOLD : WHITE);
        DrawText("ATACAR", (int)btnAttack.x + 12, (int)btnAttack.y + 8, 20, hoverAttack ? GOLD : WHITE);

        Color itemsColor = combat->itemUsedThisTurn ? DARKGRAY : (hoverItems ? GOLD : WHITE);
        DrawRectangleLinesEx(btnItems, 1, itemsColor);
        DrawText("ITENS", (int)btnItems.x + 12, (int)btnItems.y + 8, 20, itemsColor);

        DrawRectangleLinesEx(btnFood, 1, hoverFood ? GOLD : WHITE);
        DrawText(TextFormat("COMIDA (%d)", player->food), (int)btnFood.x + 12, (int)btnFood.y + 8, 20, hoverFood ? GOLD : WHITE);
    } else if (combat->phase == COMBAT_VICTORY || combat->phase == COMBAT_DEFEAT) {
        Rectangle btnContinue = { 50.0f, 300.0f, 220.0f, 36.0f };
        bool hoverContinue = CheckCollisionPointRec(mousePos, btnContinue);
        DrawRectangleLinesEx(btnContinue, 1, hoverContinue ? GOLD : WHITE);
        DrawText("CONTINUAR", (int)btnContinue.x + 12, (int)btnContinue.y + 8, 20, hoverContinue ? GOLD : WHITE);
    }
}