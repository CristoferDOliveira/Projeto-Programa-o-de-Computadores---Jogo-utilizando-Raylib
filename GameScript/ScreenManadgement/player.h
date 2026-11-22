#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#define MAX_UNIQUE_ITEMS 15
#define MAX_ITEM_STACK 10

// Efeitos que um item pode ter ao apertar o botao [O]
typedef enum {
    EFFECT_NONE,
    EFFECT_ADD_RESISTANCE,
    EFFECT_BUFF_STAFF
} ItemEffectType;

typedef struct {
    char name[40];
    char description[128]; // Se vazio (""), o botao [S] nao aparece
    int quantity;
    ItemEffectType effect;
    int effectValue;
} InventoryItem;

typedef struct {
    int currentHp;
    int maxHp;
    int coins;
    int food;

    // Status iniciais (todos comecam em 0)
    int strength;   // Forca
    int agility;    // Agilidade
    int wisdom;     // Sabedoria
    int charisma;   // Carisma
    int resistance; // Resistencia

    InventoryItem inventory[MAX_UNIQUE_ITEMS];
    int uniqueItemCount; // Conta quantos slots unicos estao ocupados (ex: 1/15)
} Player;

void InitPlayer(Player *player);
void AddItem(Player *player, const char *name, const char *desc, int amount, ItemEffectType effect, int effectValue);
void RemoveItem(Player *player, int index, int amount);
void UseItem(Player *player, int index);

#endif
