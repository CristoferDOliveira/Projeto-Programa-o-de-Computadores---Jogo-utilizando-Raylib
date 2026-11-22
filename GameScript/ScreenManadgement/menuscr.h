#ifndef MENUSCR_H
#define MENUSCR_H

#include "scrmgr.h"

// Inicializa as posições dos textos e botões do menu
void InitMenuScreen(int screenWidth);

// Atualiza a lógica (cliques e mouse) e retorna a próxima tela
GameScreen UpdateMenuScreen(void);

// Desenha o menu na tela
void DrawMenuScreen(void);

#endif
