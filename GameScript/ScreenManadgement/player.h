#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#define MAX_UNIQUE_ITEMS 15
#define MAX_ITEM_STACK 10
#define MAX_CATALOG_ITEMS 50

#define ID_LEN 32
#define NAME_LEN 40
#define DESC_LEN 128

typedef enum {
    ITEM_MISC,
    ITEM_CONSUMABLE,
    ITEM_WEAPON,
    ITEM_ARMOR,
    ITEM_SHIELD
} ItemType;

typedef enum {
    STAT_NONE,
    STAT_RESISTANCE,
    STAT_BUFF_STAFF,
    STAT_DAMAGE
} StatBonusType;

typedef struct {
    char id[ID_LEN];
    char name[NAME_LEN];
    ItemType type;
    StatBonusType statBonus;
    int bonusValue;
    char description[DESC_LEN];
} ItemDef;

typedef struct {
    char itemId[ID_LEN];
    int quantity;
} InventorySlot;

typedef struct {
    int currentHp;
    int maxHp;
    int coins;
    int food;

    int baseStrength;
    int baseAgility;
    int baseWisdom;
    int baseCarisma;
    int baseResistance;

    int totalStrength;
    int totalAgility;
    int totalWisdom;
    int totalCarisma;
    int totalResistance;

    int staffBuffTurns;
    int staffBuffBonus;

    char equippedWeaponId[ID_LEN];
    char equippedArmorId[ID_LEN];
    char equippedShieldId[ID_LEN];

    InventorySlot inventory[MAX_UNIQUE_ITEMS];
    int uniqueItemCount;
} Player;

void InitItemCatalog(void);
const ItemDef* GetItemDef(const char *itemId);

void InitPlayer(Player *player);
void RecalculateStats(Player *player);
int GetWeaponDamage(const Player *player);
void TickStaffBuff(Player *player);

void AddItemById(Player *player, const char *itemId, int amount);
void RemoveItemAt(Player *player, int index, int amount);
bool UseOrEquipItemAt(Player *player, int index);
void UnequipSlot(Player *player, ItemType slotType);

#endif