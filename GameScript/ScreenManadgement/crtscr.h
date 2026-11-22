#ifndef CRTSCR_H
#define CRTSCR_H

#include "scrmgr.h"

// Inicializa as posições dos textos e do botão voltar
void InitCreatorsScreen(int screenWidth);

// Atualiza a lógica (clique no botão voltar) e retorna a próxima tela
GameScreen UpdateCreatorsScreen(void);

// Desenha a tela de criadores
void DrawCreatorsScreen(void);

#endif
