#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ScreenManadgement/player.h"

#define ITEMS_FILE_PATH "GameScript/GameData/Database/Items/items.txt"
#define PLAYER_INIT_PATH "GameScript/GameData/Database/Characters/player_init.txt"
#define FIRST_IDX 0
#define MAX_LINE_LEN 256
#define EQUIP_SLOTS_COUNT 3
#define UNARMED_DAMAGE 1
#define STAFF_BUFF_DURATION 2

static ItemDef itemCatalog[MAX_CATALOG_ITEMS];
static int catalogCount = 0;

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

static ItemType ParseItemType(const char *str) {
    if (strcmp(str, "WEAPON") == 0) return ITEM_WEAPON;
    if (strcmp(str, "ARMOR") == 0) return ITEM_ARMOR;
    if (strcmp(str, "SHIELD") == 0) return ITEM_SHIELD;
    if (strcmp(str, "CONSUMABLE") == 0) return ITEM_CONSUMABLE;
    return ITEM_MISC;
}

static StatBonusType ParseStatBonus(const char *str) {
    if (strcmp(str, "RESISTANCE") == 0) return STAT_RESISTANCE;
    if (strcmp(str, "BUFF_STAFF") == 0) return STAT_BUFF_STAFF;
    if (strcmp(str, "DAMAGE") == 0) return STAT_DAMAGE;
    return STAT_NONE;
}

void InitItemCatalog(void) {
    FILE *file = fopen(ITEMS_FILE_PATH, "r");
    catalogCount = 0;
    if (file == NULL) return;

    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), file) != NULL && catalogCount < MAX_CATALOG_ITEMS) {
        TrimNewline(line);
        if (strlen(line) == 0) continue;

        ItemDef *def = &itemCatalog[catalogCount];
        def->id[FIRST_IDX] = '\0';
        def->name[FIRST_IDX] = '\0';
        def->type = ITEM_MISC;
        def->statBonus = STAT_NONE;
        def->bonusValue = 0;
        def->description[FIRST_IDX] = '\0';

        char *token = strtok(line, "|");
        while (token != NULL) {
            char *clean = TrimSpaces(token);
            if (strncmp(clean, "ID:", 3) == 0) {
                strncpy(def->id, TrimSpaces(clean + 3), ID_LEN - 1);
                def->id[ID_LEN - 1] = '\0';
            } else if (strncmp(clean, "NAME:", 5) == 0) {
                strncpy(def->name, TrimSpaces(clean + 5), NAME_LEN - 1);
                def->name[NAME_LEN - 1] = '\0';
            } else if (strncmp(clean, "TYPE:", 5) == 0) {
                def->type = ParseItemType(TrimSpaces(clean + 5));
            } else if (strncmp(clean, "STAT:", 5) == 0) {
                def->statBonus = ParseStatBonus(TrimSpaces(clean + 5));
            } else if (strncmp(clean, "VAL:", 4) == 0) {
                def->bonusValue = atoi(TrimSpaces(clean + 4));
            } else if (strncmp(clean, "DESC:", 5) == 0) {
                strncpy(def->description, TrimSpaces(clean + 5), DESC_LEN - 1);
                def->description[DESC_LEN - 1] = '\0';
            }
            token = strtok(NULL, "|");
        }

        if (def->id[FIRST_IDX] != '\0') {
            catalogCount++;
        }
    }
    fclose(file);
}

const ItemDef* GetItemDef(const char *itemId) {
    if (itemId == NULL || strcmp(itemId, "none") == 0) return NULL;
    for (int i = 0; i < catalogCount; i++) {
        if (strcmp(itemCatalog[i].id, itemId) == 0) {
            return &itemCatalog[i];
        }
    }
    return NULL;
}

void RecalculateStats(Player *player) {
    player->totalStrength = player->baseStrength;
    player->totalAgility = player->baseAgility;
    player->totalWisdom = player->baseWisdom;
    player->totalCarisma = player->baseCarisma;
    player->totalResistance = player->baseResistance;

    const char *equippedIds[EQUIP_SLOTS_COUNT] = {
        player->equippedWeaponId,
        player->equippedArmorId,
        player->equippedShieldId
    };

    for (int i = 0; i < EQUIP_SLOTS_COUNT; i++) {
        const ItemDef *def = GetItemDef(equippedIds[i]);
        if (def != NULL && def->statBonus == STAT_RESISTANCE) {
            player->totalResistance += def->bonusValue;
        }
    }
}

int GetWeaponDamage(const Player *player) {
    const ItemDef *weapon = GetItemDef(player->equippedWeaponId);
    if (weapon == NULL) return UNARMED_DAMAGE;

    int dmg = (weapon->statBonus == STAT_DAMAGE) ? weapon->bonusValue : UNARMED_DAMAGE;
    if (strcmp(player->equippedWeaponId, "cajado") == 0 && player->staffBuffTurns > 0) {
        dmg += player->staffBuffBonus;
    }
    return dmg;
}

void TickStaffBuff(Player *player) {
    if (player->staffBuffTurns > 0) {
        player->staffBuffTurns--;
        if (player->staffBuffTurns == 0) {
            player->staffBuffBonus = 0;
        }
    }
}

void InitPlayer(Player *player) {
    if (catalogCount == 0) {
        InitItemCatalog();
    }

    player->maxHp = 20;
    player->currentHp = 20;
    player->coins = 0;
    player->food = 0;
    player->baseStrength = 0;
    player->baseAgility = 0;
    player->baseWisdom = 0;
    player->baseCarisma = 0;
    player->baseResistance = 0;
    player->staffBuffTurns = 0;
    player->staffBuffBonus = 0;
    strcpy(player->equippedWeaponId, "none");
    strcpy(player->equippedArmorId, "none");
    strcpy(player->equippedShieldId, "none");
    player->uniqueItemCount = 0;

    FILE *file = fopen(PLAYER_INIT_PATH, "r");
    if (file != NULL) {
        char line[MAX_LINE_LEN];
        while (fgets(line, sizeof(line), file) != NULL) {
            TrimNewline(line);
            if (strncmp(line, "HP:", 3) == 0) {
                player->maxHp = atoi(TrimSpaces(line + 3));
                player->currentHp = player->maxHp;
            } else if (strncmp(line, "COINS:", 6) == 0) {
                player->coins = atoi(TrimSpaces(line + 6));
            } else if (strncmp(line, "FOOD:", 5) == 0) {
                player->food = atoi(TrimSpaces(line + 5));
            } else if (strncmp(line, "STRENGTH:", 9) == 0) {
                player->baseStrength = atoi(TrimSpaces(line + 9));
            } else if (strncmp(line, "AGILITY:", 8) == 0) {
                player->baseAgility = atoi(TrimSpaces(line + 8));
            } else if (strncmp(line, "WISDOM:", 7) == 0) {
                player->baseWisdom = atoi(TrimSpaces(line + 7));
            } else if (strncmp(line, "CHARISMA:", 9) == 0) {
                player->baseCarisma = atoi(TrimSpaces(line + 9));
            } else if (strncmp(line, "RESISTANCE:", 11) == 0) {
                player->baseResistance = atoi(TrimSpaces(line + 11));
            } else if (strncmp(line, "EQUIP_WEAPON:", 13) == 0) {
                strncpy(player->equippedWeaponId, TrimSpaces(line + 13), ID_LEN - 1);
            } else if (strncmp(line, "EQUIP_ARMOR:", 12) == 0) {
                strncpy(player->equippedArmorId, TrimSpaces(line + 12), ID_LEN - 1);
            } else if (strncmp(line, "EQUIP_SHIELD:", 13) == 0) {
                strncpy(player->equippedShieldId, TrimSpaces(line + 13), ID_LEN - 1);
            }
        }
        fclose(file);
    }

    RecalculateStats(player);
}

void AddItemById(Player *player, const char *itemId, int amount) {
    if (GetItemDef(itemId) == NULL) return;

    for (int i = 0; i < player->uniqueItemCount; i++) {
        if (strcmp(player->inventory[i].itemId, itemId) == 0) {
            player->inventory[i].quantity += amount;
            if (player->inventory[i].quantity > MAX_ITEM_STACK) {
                player->inventory[i].quantity = MAX_ITEM_STACK;
            }
            return;
        }
    }

    if (player->uniqueItemCount < MAX_UNIQUE_ITEMS) {
        int idx = player->uniqueItemCount;
        strncpy(player->inventory[idx].itemId, itemId, ID_LEN - 1);
        player->inventory[idx].itemId[ID_LEN - 1] = '\0';
        player->inventory[idx].quantity = (amount > MAX_ITEM_STACK) ? MAX_ITEM_STACK : amount;
        player->uniqueItemCount++;
    }
}

void RemoveItemAt(Player *player, int index, int amount) {
    if (index < 0 || index >= player->uniqueItemCount) return;

    player->inventory[index].quantity -= amount;
    if (player->inventory[index].quantity <= 0) {
        for (int i = index; i < player->uniqueItemCount - 1; i++) {
            player->inventory[i] = player->inventory[i + 1];
        }
        player->uniqueItemCount--;
    }
}

void UnequipSlot(Player *player, ItemType slotType) {
    char *targetSlot = NULL;
    if (slotType == ITEM_WEAPON) targetSlot = player->equippedWeaponId;
    else if (slotType == ITEM_ARMOR) targetSlot = player->equippedArmorId;
    else if (slotType == ITEM_SHIELD) targetSlot = player->equippedShieldId;

    if (targetSlot != NULL && strcmp(targetSlot, "none") != 0) {
        if (player->uniqueItemCount < MAX_UNIQUE_ITEMS) {
            AddItemById(player, targetSlot, 1);
            strcpy(targetSlot, "none");
            RecalculateStats(player);
        }
    }
}

bool UseOrEquipItemAt(Player *player, int index) {
    if (index < 0 || index >= player->uniqueItemCount) return false;

    char selectedId[ID_LEN];
    strcpy(selectedId, player->inventory[index].itemId);

    const ItemDef *def = GetItemDef(selectedId);
    if (def == NULL) return false;

    if (def->type == ITEM_WEAPON || def->type == ITEM_ARMOR || def->type == ITEM_SHIELD) {
        char *targetSlot = NULL;
        if (def->type == ITEM_WEAPON) targetSlot = player->equippedWeaponId;
        else if (def->type == ITEM_ARMOR) targetSlot = player->equippedArmorId;
        else if (def->type == ITEM_SHIELD) targetSlot = player->equippedShieldId;

        char previousEquipped[ID_LEN];
        strcpy(previousEquipped, targetSlot);

        RemoveItemAt(player, index, 1);
        strcpy(targetSlot, selectedId);

        if (strcmp(previousEquipped, "none") != 0) {
            AddItemById(player, previousEquipped, 1);
        }

        RecalculateStats(player);
        return true;
    }
    else if (def->type == ITEM_CONSUMABLE) {
        if (def->statBonus == STAT_BUFF_STAFF) {
            player->staffBuffBonus = def->bonusValue;
            player->staffBuffTurns = STAFF_BUFF_DURATION;
        }
        RemoveItemAt(player, index, 1);
        return true;
    }

    return false;
}