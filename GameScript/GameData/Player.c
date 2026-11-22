#include <string.h>
#include "../ScreenManadgement/player.h"

void InitPlayer(Player *player) {
    player->currentHp = 20;
    player->maxHp = 20;
    player->coins = 0;
    player->food = 0;

    player->strength = 0;
    player->agility = 0;
    player->wisdom = 0;
    player->charisma = 0;
    player->resistance = 0;

    player->uniqueItemCount = 0;
}

void AddItem(Player *player, const char *name, const char *desc, int amount, ItemEffectType effect, int effectValue) {
    // 1. Verifica se o item ja existe para apenas somar a quantidade (limite 10)
    for (int i = 0; i < player->uniqueItemCount; i++) {
        if (strcmp(player->inventory[i].name, name) == 0) {
            player->inventory[i].quantity += amount;
            if (player->inventory[i].quantity > MAX_ITEM_STACK) {
                player->inventory[i].quantity = MAX_ITEM_STACK;
            }
            return;
        }
    }

    // 2. Se for um item novo e houver espaco nos 15 slots
    if (player->uniqueItemCount < MAX_UNIQUE_ITEMS) {
        int idx = player->uniqueItemCount;
        strncpy(player->inventory[idx].name, name, sizeof(player->inventory[idx].name) - 1);
        player->inventory[idx].name[sizeof(player->inventory[idx].name) - 1] = '\0';

        if (desc != NULL) {
            strncpy(player->inventory[idx].description, desc, sizeof(player->inventory[idx].description) - 1);
            player->inventory[idx].description[sizeof(player->inventory[idx].description) - 1] = '\0';
        } else {
            player->inventory[idx].description[0] = '\0';
        }

        player->inventory[idx].quantity = (amount > MAX_ITEM_STACK) ? MAX_ITEM_STACK : amount;
        player->inventory[idx].effect = effect;
        player->inventory[idx].effectValue = effectValue;
        player->uniqueItemCount++;
    }
}

void RemoveItem(Player *player, int index, int amount) {
    if (index < 0 || index >= player->uniqueItemCount) return;

    player->inventory[index].quantity -= amount;

    // Se zerar a quantidade, remove o slot e reorganiza a lista
    if (player->inventory[index].quantity <= 0) {
        for (int i = index; i < player->uniqueItemCount - 1; i++) {
            player->inventory[i] = player->inventory[i + 1];
        }
        player->uniqueItemCount--;
    }
}

void UseItem(Player *player, int index) {
    if (index < 0 || index >= player->uniqueItemCount) return;

    InventoryItem *item = &player->inventory[index];

    // Executa a acao correspondente do item
    if (item->effect == EFFECT_ADD_RESISTANCE) {
        player->resistance += item->effectValue;
        RemoveItem(player, index, 1);
    } 
    else if (item->effect == EFFECT_BUFF_STAFF) {
        // Consome o pergaminho ao usar
        RemoveItem(player, index, 1);
    }
}
