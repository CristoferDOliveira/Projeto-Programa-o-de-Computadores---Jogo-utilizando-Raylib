#ifndef STORY_H
#define STORY_H

#include "player.h"

#define MAX_CHOICES 4
#define MAX_CHOICE_TEXT 40
#define MAX_TARGET_LEN 64
#define MAX_STORY_TEXT 512

typedef struct {
    char text[MAX_CHOICE_TEXT];
    char nextTarget[MAX_TARGET_LEN]; // Arquivo (ex: "Chapter1/C2.txt") ou secao (ex: "#MAGE")
    int classRewardId;               // 0 = nenhum, 1 = Mago, 2 = Guerreiro, 3 = Arqueiro
} StoryChoice;

typedef struct {
    char text[MAX_STORY_TEXT];
    StoryChoice choices[MAX_CHOICES];
    int choiceCount;
} StoryNode;

void InitStory(void);
const StoryNode* GetCurrentStoryNode(void);
void SelectStoryChoice(int choiceIndex, Player *player);

#endif
