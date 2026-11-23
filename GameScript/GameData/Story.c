#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ScreenManadgement/story.h"

#define STORY_BASE_PATH "GameScript/GameData/Storyline/"
#define FIRST_IDX 0
#define MAX_PATH_LEN 256
#define MAX_LINE_LEN 600
#define MAX_HEADER_LEN 64

static StoryNode currentNode;
static char currentFilePath[MAX_PATH_LEN] = "Prologue/P1.txt";

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

static void ParseNewlines(char *dest, const char *src, int maxLen) {
    int j = 0;
    for (int i = 0; src[i] != '\0' && j < maxLen - 1; i++) {
        if (src[i] == '\\' && src[i + 1] == 'n') {
            dest[j++] = '\n';
            i++;
        } else {
            dest[j++] = src[i];
        }
    }
    dest[j] = '\0';
}

static void LoadSceneSection(const char *relativePath, const char *sectionName) {
    char fullPath[MAX_PATH_LEN];
    snprintf(fullPath, sizeof(fullPath), "%s%s", STORY_BASE_PATH, relativePath);

    FILE *file = fopen(fullPath, "r");
    currentNode.choiceCount = 0;
    currentNode.text[FIRST_IDX] = '\0';

    if (file == NULL) {
        snprintf(currentNode.text, sizeof(currentNode.text),
                 "Fim da demo ou arquivo nao encontrado:\n%s\n\nCrie esse arquivo na pasta Storyline para continuar!", fullPath);
        strcpy(currentNode.choices[FIRST_IDX].text, "Voltar ao inicio");
        strcpy(currentNode.choices[FIRST_IDX].nextTarget, "Prologue/P1.txt");
        currentNode.choices[FIRST_IDX].classRewardId = 0;
        currentNode.choiceCount = 1;
        return;
    }

    char line[MAX_LINE_LEN];
    bool isReadingTargetSection = false;
    bool fileHasSections = false;

    while (fgets(line, sizeof(line), file) != NULL) {
        TrimNewline(line);
        int lineLen = (int)strlen(line);
        if (lineLen == 0) continue;

        if (line[FIRST_IDX] == '[' && line[lineLen - 1] == ']') {
            fileHasSections = true;
            char header[MAX_HEADER_LEN];
            int headerLen = lineLen - 2;
            if (headerLen > 0 && headerLen < (int)sizeof(header)) {
                strncpy(header, line + 1, headerLen);
                header[headerLen] = '\0';

                if (strcmp(header, sectionName) == 0) {
                    isReadingTargetSection = true;
                } else if (isReadingTargetSection) {
                    break;
                }
            }
            continue;
        }

        if (!fileHasSections) {
            isReadingTargetSection = true;
        }

        if (!isReadingTargetSection) continue;

        if (strncmp(line, "TEXT: ", 6) == 0) {
            ParseNewlines(currentNode.text, line + 6, sizeof(currentNode.text));
        }
        else if (strncmp(line, "CHOICE: ", 8) == 0 && currentNode.choiceCount < MAX_CHOICES) {
            int cIdx = currentNode.choiceCount;
            char *data = line + 8;
            char *btnText = strtok(data, "|");
            char *target = strtok(NULL, "|");
            char *rewardStr = strtok(NULL, "|");

            if (btnText != NULL && target != NULL) {
                btnText = TrimSpaces(btnText);
                target = TrimSpaces(target);

                strncpy(currentNode.choices[cIdx].text, btnText, sizeof(currentNode.choices[cIdx].text) - 1);
                currentNode.choices[cIdx].text[sizeof(currentNode.choices[cIdx].text) - 1] = '\0';

                strncpy(currentNode.choices[cIdx].nextTarget, target, sizeof(currentNode.choices[cIdx].nextTarget) - 1);
                currentNode.choices[cIdx].nextTarget[sizeof(currentNode.choices[cIdx].nextTarget) - 1] = '\0';

                currentNode.choices[cIdx].classRewardId = (rewardStr != NULL) ? atoi(TrimSpaces(rewardStr)) : 0;
                currentNode.choiceCount++;
            }
        }
    }

    fclose(file);
}

void InitStory(void) {
    strcpy(currentFilePath, "Prologue/P1.txt");
    LoadSceneSection(currentFilePath, "MAIN");
}

const StoryNode* GetCurrentStoryNode(void) {
    return &currentNode;
}

static void ApplyClassRewards(int classId, Player *player) {
    if (classId == 1) {
        AddItemById(player, "cajado", 1);
        AddItemById(player, "pergaminho_fort", 1);
        AddItemById(player, "armadura_pano", 1);
        player->food += 5;
        player->coins += 10;
    }
    else if (classId == 2) {
        AddItemById(player, "espada", 1);
        AddItemById(player, "escudo_madeira", 1);
        AddItemById(player, "armadura_pano", 1);
        player->food += 5;
        player->coins += 10;
    }
    else if (classId == 3) {
        AddItemById(player, "arco", 1);
        AddItemById(player, "flechas", 10);
        AddItemById(player, "armadura_pano", 1);
        player->food += 5;
        player->coins += 10;
    }
}

void NavigateToTarget(const char *target, Player *player) {
    if (target == NULL || target[FIRST_IDX] == '\0') return;

    if (target[FIRST_IDX] == '#') {
        LoadSceneSection(currentFilePath, target + 1);
    } else {
        if (strcmp(target, "Prologue/P1.txt") == 0 && strcmp(currentFilePath, "Prologue/P1.txt") != 0) {
            InitPlayer(player);
        }
        strncpy(currentFilePath, target, MAX_PATH_LEN - 1);
        currentFilePath[MAX_PATH_LEN - 1] = '\0';
        LoadSceneSection(currentFilePath, "MAIN");
    }
}

void SelectStoryChoice(int choiceIndex, Player *player) {
    if (choiceIndex < 0 || choiceIndex >= currentNode.choiceCount) return;

    StoryChoice chosen = currentNode.choices[choiceIndex];

    if (chosen.classRewardId > 0) {
        ApplyClassRewards(chosen.classRewardId, player);
    }

    NavigateToTarget(chosen.nextTarget, player);
}