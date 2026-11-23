#ifndef COMBAT_H
#define COMBAT_H

#include <stdbool.h>
#include "raylib.h"
#include "player.h"

#define ENEMY_NAME_LEN 40
#define COMBAT_LOG_LEN 160
#define TARGET_LEN 64

typedef enum {
    TURN_PLAYER,
    TURN_ENEMY_QTE,
    COMBAT_VICTORY,
    COMBAT_DEFEAT
} CombatPhase;

typedef enum {
    DODGE_LEFT,
    DODGE_RIGHT
} DodgeDirection;

typedef struct {
    char name[ENEMY_NAME_LEN];
    int currentHp;
    int maxHp;
    int attackDamage;
    int agility;
    int resistance;
    int rewardCoins;
    char rewardItemId[ID_LEN];
} EnemyData;

typedef struct {
    bool active;
    EnemyData enemy;
    CombatPhase phase;
    int playerTurns;
    int enemyTurns;
    bool itemUsedThisTurn;
    DodgeDirection targetDirection;
    float qteTimer;
    float qteMaxTime;
    char winTarget[TARGET_LEN];
    char loseTarget[TARGET_LEN];
    char logText[COMBAT_LOG_LEN];
} CombatState;

void InitCombat(CombatState *combat, Player *player, const char *enemyFile, const char *winTarget, const char *loseTarget);
void ExecutePlayerAttack(CombatState *combat, Player *player);
void ExecutePlayerFood(CombatState *combat, Player *player);
void NotifyPlayerItemUsed(CombatState *combat);
void ResolveEnemyAttack(CombatState *combat, Player *player, bool dodged);
void UpdateCombat(CombatState *combat, Player *player, Vector2 mousePos, bool *openInventory);
void DrawCombat(const CombatState *combat, const Player *player, int screenWidth, Vector2 mousePos);

#endif