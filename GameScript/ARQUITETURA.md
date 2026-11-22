# Documentação Técnica e Arquitetura - Marcus Quest

Este documento detalha a estrutura de diretórios, as decisões de projeto e a lógica de implementação do código desenvolvido em **linguagem C**. A utilização da biblioteca gráfica **Raylib** foi adotada conforme requisito especificado pelo professor da disciplina.

---

## 1. Arquitetura de Diretórios

O projeto foi dividido em módulos independentes para evitar o acoplamento entre a lógica de dados, o controle de fluxo e a renderização gráfica. Essa separação facilita a manutenção, a depuração de erros e a escalabilidade do sistema:

```text
MarcusQuest/
 ├── Makefile                      # Automação de build utilizando wildcards
 └── GameScript/
      ├── Main.c                   # Ponto de entrada e gerenciamento do loop principal
      ├── GameScreens/             # Camada de apresentação (UI e entrada do usuário)
      │    ├── MenuScreen.c
      │    ├── PlayScreen.c
      │    └── CreatorsScreen.c
      ├── ScreenManadgement/       # Camada de controle de estados e cabeçalhos (.h)
      │    ├── scrmgr.h
      │    ├── ScreenManager.c
      │    ├── menuscr.h
      │    ├── playscr.h
      │    ├── crtscr.h
      │    ├── player.h
      │    └── story.h
      └── GameData/                # Camada de dados e regras de negócio
           ├── Player.c
           ├── Story.c
           └── Storyline/          # Base de dados externa da narrativa (.txt)
                ├── Chapter1/
                │    ├── C1.txt
                │    ├── C2.txt
                │    └── C3-CLASS.txt
                └── Chapter2/
```

---

## 2. Decisões Técnicas e Lógica do Código

### 2.1. Máquina de Estados Finitos (`ScreenManager`)
* **O que foi usado:** Um tipo enumerado (`typedef enum`) contendo os estados `SCREEN_MENU`, `SCREEN_PLAY` e `SCREEN_CREATORS`, controlado por estruturas `switch-case` no `ScreenManager.c`.
* **Por que foi utilizado:** Em vez de concentrar múltiplas condicionais (`if/else`) dentro do `Main.c`, a máquina de estados isola o ciclo de vida de cada tela. O `Main.c` apenas inicializa a janela e executa `UpdateCurrentScreen()` e `DrawCurrentScreen()` a 60 FPS, delegando ao gerenciador a responsabilidade de saber qual módulo deve processar cliques e desenhar na tela naquele instante.

### 2.2. Encapsulamento com Modificador `static`
* **O que foi usado:** Variáveis globais restritas ao escopo de arquivo (como `static Player player;` e `static StoryNode currentNode;`).
* **Por que foi utilizado:** Em C puro, variáveis declaradas fora de funções têm visibilidade global em todo o programa por padrão. O uso de `static` garante o encapsulamento, impedindo que uma tela acesse ou sobrescreva acidentalmente as coordenadas de botões ou estados internos de outra tela.

### 2.3. Separação entre Lógica (`Update`) e Renderização (`Draw`)
* **O que foi usado:** Divisão das funções de cada tela em três etapas: `Init` (cálculo único de posições), `Update` (detecção de colisão do mouse e alteração de estado) e `Draw` (desenho dos elementos).
* **Por que foi utilizado:** Calcular larguras de texto (`MeasureText`) e verificar cliques dentro do mesmo bloco de desenho prejudica a organização e a performance. Ao pré-calcular as caixas de colisão (`Rectangle`) no `Init`, o loop principal apenas valida `CheckCollisionPointRec` a cada frame.

### 2.4. Leitura Dinâmica de Arquivos Externos (`Story.c` + `.txt`)
* **O que foi usado:** Manipulação nativa de arquivos e strings da biblioteca padrão do C (`fopen`, `fgets`, `strtok`, `strncmp`) para interpretar arquivos `.txt` organizados em capítulos.
* **Por que foi utilizado:**
  1. **Desacoplamento de dados:** Manter textos longos e ramificações de história diretamente no código-fonte poluía os arquivos `.c` e exigia recompilação a cada alteração textual.
  2. **Zero dependências externas:** Como C não possui leitor nativo de JSON, a criação de um formato próprio baseado em marcadores (`TEXT:`, `CHOICE:` e seções `[NOME]`) permitiu leitura sob demanda apenas da cena atual, economizando memória.
  3. **Navegação híbrida:** O interpretador diferencia destinos internos (prefixo `#`, como `#MAGE`, que alterna seções dentro do mesmo arquivo) de destinos externos (caminhos relativos como `Chapter1/C2.txt`, que carregam um novo arquivo).

### 2.5. Estruturas de Dados e Gestão de Inventário (`Player.c`)
* **O que foi usado:** `struct` aninhadas (`Player` contendo um vetor estático `InventoryItem inventory[15]`) e algoritmos de busca linear e deslocamento de vetor (*shift*).
* **Por que foi utilizado:**
  * **Alocação estática previsível:** Como o inventário possui limite rígido de 15 itens únicos e empilhamento máximo de 10 unidades por item, o uso de vetores de tamanho fixo evita vazamentos de memória (*memory leaks*) comuns na alocação dinâmica.
  * **Agrupamento e reorganização:** Ao adicionar um item (`AddItem`), o sistema percorre os slots ativos com `strcmp`; se o item já existir, apenas incrementa a quantidade sem consumir um novo slot. Ao descartar ou usar a última unidade (`RemoveItem`), os elementos subsequentes são deslocados uma posição para trás para não deixar lacunas vazias na interface.
  * **Renderização condicional:** O botão de inspeção `[S]` é renderizado apenas se o primeiro caractere da descrição do item for diferente do terminador nulo (`description[0] != '\0'`), evitando botões inativos na tela.

### 2.6. Quebra de Linha Automática (`DrawTextWrapped`)
* **O que foi usado:** Algoritmo de tokenização de palavras (`strtok`) combinado com medição de largura em pixels (`MeasureText`).
* **Por que foi utilizado:** A função `DrawText` da Raylib não realiza quebra de linha automática quando o texto ultrapassa a largura da janela. A função auxiliar acumula palavras até atingir o limite máximo em pixels definido para a caixa de texto, inserindo quebras de linha dinamicamente.

### 2.7. Automação de Build (`Makefile`)
* **O que foi usado:** Função `$(wildcard ...)` atribuída imediatamente (`:=`) à variável `OBJS` no `Makefile`.
* **Por que foi utilizado:** Permite que o compilador (`gcc`) localize e compile automaticamente qualquer novo arquivo `.c` adicionado às subpastas do projeto, eliminando a necessidade de atualizar manualmente as listas de compilação a cada novo módulo criado.

---

## 3. Créditos e Ferramentas de Apoio
* **Desenvolvimento e Arquitetura:** Cristofer Dvorecky Oliveira (RA: 26020823)
* **Apoio Técnico:** Inteligência Artificial **Google Gemini** utilizada como ferramenta de consulta e auxílio na estruturação da arquitetura de troca de telas (`ScreenManager`) e na integração da leitura de arquivos `.txt` com os scripts em C.