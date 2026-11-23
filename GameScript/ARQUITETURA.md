# Documentação Técnica e Arquitetura - Marcus Quest

Este documento detalha a estrutura de diretórios, as decisões de projeto e a lógica de implementação do código desenvolvido em **linguagem C**. A utilização da biblioteca gráfica **Raylib** foi adotada conforme requisito especificado pelo professor da disciplina.

---

## 1. Arquitetura de Diretórios

O projeto foi dividido em módulos independentes para evitar o acoplamento entre a lógica de dados, o controle de fluxo e a renderização gráfica. Essa separação facilita a manutenção, a depuração de erros e a escalabilidade do sistema:

```text
MarcusQuest/
 ├── .vscode/                      # Configurações do ambiente de desenvolvimento
 ├── GameScript/
 │    ├── GameData/                # Camada de dados, entidades e regras de negócio
 │    │    ├── Database/           # Bases de dados externas estruturadas (.txt)
 │    │    │    ├── Characters/
 │    │    │    │    ├── player_init.txt
 │    │    │    │    └── sad_mage.txt
 │    │    │    └── Items/
 │    │    │         └── items.txt
 │    │    ├── Storyline/          # Roteiro e fluxo narrativo externo (.txt)
 │    │    │    ├── Chapter1/
 │    │    │    │    ├── C1-1.txt
 │    │    │    │    ├── C1-2.txt
 │    │    │    │    ├── C1-2C.txt
 │    │    │    │    ├── C1-2C2.txt
 │    │    │    │    ├── C1-2M.txt
 │    │    │    │    ├── C1-2MC.txt
 │    │    │    │    ├── C1-3.txt
 │    │    │    │    ├── C1-4.txt
 │    │    │    │    └── CH1.txt
 │    │    │    └── Prologue/
 │    │    │         ├── P1.txt
 │    │    │         ├── P2.txt
 │    │    │         ├── P3-CLASS.txt
 │    │    │         └── P4.txt
 │    │    ├── Combat.c
 │    │    ├── Player.c
 │    │    └── Story.c
 │    ├── GameScreens/             # Camada de apresentação (UI e entrada do usuário)
 │    │    ├── CreatorsScreen.c
 │    │    ├── MenuScreen.c
 │    │    └── PlayScreen.c
 │    ├── ScreenManadgement/       # Camada de controle de estados e cabeçalhos (.h)
 │    │    ├── combat.h
 │    │    ├── crtscr.h
 │    │    ├── menuscr.h
 │    │    ├── player.h
 │    │    ├── playscr.h
 │    │    ├── ScreenManager.c
 │    │    ├── scrmgr.h
 │    │    └── story.h
 │    ├── ARQUITETURA.md           # Notas auxiliares de arquitetura
 │    └── Main.c                   # Ponto de entrada e gerenciamento do loop principal
 ├── resources/                    # Recursos externos (fontes, imagens, áudios)
 ├── main.code-workspace           # Workspace do Visual Studio Code
 ├── Makefile                      # Automação de build utilizando wildcards
 ├── Makefile.Android              # Configuração auxiliar de compilação
 └── README.md                     # Documentação técnica do projeto
```

### 1.1. Organização por Camadas
- **Raiz (`MarcusQuest/`):** Centraliza a automação de compilação (`Makefile`, `Makefile.Android`), configurações do workspace (`.vscode/`, `main.code-workspace`), ativos visuais/sonoros (`resources/`), o código-fonte em `GameScript/` e a documentação (`README.md`).
- **Ponto de Entrada (`GameScript/Main.c`):** Inicializa a janela da Raylib e mantém o loop principal rodando a 60 FPS, delegando o processamento de entrada e a renderização ao gerenciador de telas.
- **Camada de Apresentação (`GameScript/GameScreens/`):** Reúne os módulos visuais (`MenuScreen.c`, `PlayScreen.c`, `CreatorsScreen.c`), responsáveis exclusivamente por desenhar a interface gráfica e capturar interações do jogador.
- **Camada de Controle e Interfaces (`GameScript/ScreenManadgement/`):** Contém a máquina de estados global (`ScreenManager.c`) e todos os arquivos de cabeçalho (`.h`), desacoplando as assinaturas de funções e estruturas de dados entre os módulos.
- **Camada de Lógica e Dados (`GameScript/GameData/`):** Concentra as regras de negócio em C (`Player.c`, `Story.c`, `Combat.c`) e os arquivos externos `.txt` divididos entre `Database/` (configuração de personagens, inimigos e itens) e `Storyline/` (cenas e ramificações da história).

---

## 2. Decisões Técnicas e Lógica do Código

### 2.1. Máquina de Estados Finitos (`ScreenManager`)
- **O que foi usado:** Um tipo enumerado (`typedef enum`) contendo os estados `SCREEN_MENU`, `SCREEN_PLAY` e `SCREEN_CREATORS`, controlado por estruturas `switch-case` no `ScreenManager.c`.
- **Por que foi utilizado:** Em vez de concentrar múltiplas condicionais (`if/else`) dentro do `Main.c`, a máquina de estados isola o ciclo de vida de cada tela. O `Main.c` apenas inicializa a janela e executa `UpdateCurrentScreen()` e `DrawCurrentScreen()` a 60 FPS, delegando ao gerenciador a responsabilidade de saber qual módulo deve processar cliques e desenhar na tela naquele instante.

### 2.2. Encapsulamento com Modificador `static`
- **O que foi usado:** Variáveis globais restritas ao escopo de arquivo (como `static Player player;`, `static CombatState combat;` e `static StoryNode currentNode;`).
- **Por que foi utilizado:** Em C puro, variáveis declaradas fora de funções têm visibilidade global em todo o programa por padrão. O uso de `static` garante o encapsulamento, impedindo que uma tela acesse ou sobrescreva acidentalmente as coordenadas de botões ou estados internos de outra tela.

### 2.3. Separação entre Lógica (`Update`) e Renderização (`Draw`)
- **O que foi usado:** Divisão das funções de cada tela em três etapas: `Init` (cálculo único de posições e reinicialização limpa via `ResetPlayScreen` quando aplicável), `Update` (detecção de colisão do mouse e alteração de estado) e `Draw` (desenho dos elementos).
- **Por que foi utilizado:** Calcular larguras de texto (`MeasureText`) e verificar cliques dentro do mesmo bloco de desenho prejudica a organização e a performance. Ao pré-calcular as caixas de colisão (`Rectangle`) no `Init`, o loop principal apenas valida `CheckCollisionPointRec` a cada frame.

### 2.4. Bases de Dados Externas em `.txt` (`Database/` e `Storyline/`)
- **O que foi usado:** Manipulação nativa de arquivos e strings da biblioteca padrão do C (`fopen`, `fgets`, `strtok`, `strncmp`, `atoi`) para carregar dados estruturados a partir de arquivos `.txt`.
- **Por que foi utilizado:** Como C não possui leitor nativo de JSON ou XML, a adoção de um formato próprio baseado em chaves (`KEY: VALUE`) e seções (`[NOME]`) eliminou dependências externas e permitiu alterar conteúdo e balanceamento sem recompilar o código:
  1. **Personagens e Inimigos (`Database/Characters/player_init.txt` e `sad_mage.txt`):** Armazenam atributos iniciais do protagonista e os status de cada inimigo (`NAME`, `HP`, `DAMAGE`, `AGILITY`, `RESISTANCE`, `REWARD_COINS` e `REWARD_ITEM`). Isso permite criar novos oponentes ou ajustar dificuldades apenas editando arquivos de texto.
  2. **Catálogo de Itens (`Database/Items/items.txt`):** Centraliza a definição dos itens do jogo (identificadores, nomes, categorias, atributos/efeitos e descrições). Facilita a manutenção do inventário e o vínculo com recompensas de combate ou escolhas narrativas sem alterar estruturas fixas no código-fonte.
  3. **Narrativa e Gatilhos (`Storyline/Prologue/` e `Storyline/Chapter1/`):** Organiza o roteiro por capítulos e cenas (`P1.txt` a `P4.txt`, `CH1.txt`, `C1-1.txt` a `C1-4.txt`), cujo fluxo e ramificações tiveram o **Twine** como ferramenta de esboço inicial antes da conversão para os arquivos `.txt`. O interpretador carrega sob demanda o nó atual, diferencia saltos internos (prefixo `#`, como `#MAGE`), transições de arquivo (caminhos relativos), gatilhos de combate (`COMBAT:...`) e retorno ao menu (`#MENU` / `MENU`, tratado em `PlayScreen.c` com reset da partida para `SCREEN_MENU`).

### 2.5. Estruturas de Dados e Gestão de Inventário (`Player.c`)
- **O que foi usado:** `struct` aninhadas (`Player` contendo um vetor estático `InventoryItem inventory[15]`) e algoritmos de busca linear e deslocamento de vetor (*shift*).
- **Por que foi utilizado:**
  - **Alocação estática previsível:** Como o inventário possui limite rígido de 15 itens únicos e empilhamento máximo de 10 unidades por item, o uso de vetores de tamanho fixo evita vazamentos de memória (*memory leaks*) comuns na alocação dinâmica.
  - **Agrupamento e reorganização:** Ao adicionar um item (`AddItem` / `AddItemById`), o sistema percorre os slots ativos com `strcmp`; se o item já existir, apenas incrementa a quantidade sem consumir um novo slot. Ao descartar ou usar a última unidade (`RemoveItem` / `RemoveItemAt`), os elementos subsequentes são deslocados uma posição para trás para não deixar lacunas vazias na interface.
  - **Controle de consumo e equipamentos:** Gerencia armas equipadas, buffs temporários (como o cajado), munição (consumo de flechas ao atacar com o arco) e renderização condicional do botão de inspeção `[S]` apenas quando há descrição válida (`description[0] != '\0'`).

### 2.6. Sistema de Combate por Turnos e QTE (`Combat.c` / `combat.h`)
- **O que foi usado:** Máquina de estados dedicada ao combate (`CombatPhase`: `TURN_PLAYER`, `TURN_ENEMY_QTE`, `COMBAT_VICTORY`, `COMBAT_DEFEAT`), temporizador baseado em tempo real (`GetFrameTime()`) e carregamento dinâmico de inimigos a partir de `GameScript/GameData/Database/Characters/`.
- **Por que foi utilizado:**
  1. **Controle rígido de fluxo e turnos:** A iniciativa compara a agilidade total do jogador (`totalAgility`) com a do inimigo para definir quem age primeiro. Durante o turno do jogador, ações como **Atacar**, **Itens** (limitado a 1 uso por turno via `itemUsedThisTurn`) e **Comida** (cura controlada até o limite `maxHp`) são isoladas para impedir comandos duplicados ou estados inconsistentes.
  2. **Mecânica ativa de defesa via QTE (`QTE_DURATION_SEC = 0.7f`):** No turno do inimigo (`TURN_ENEMY_QTE`), o sistema sorteia uma direção de esquiva (`DODGE_LEFT` ou `DODGE_RIGHT`) e inicia uma janela de reação rápida de **0,7 segundos**, decrementada continuamente por `GetFrameTime()` para manter a mesma velocidade independentemente da taxa de quadros. O motivo dessa escolha foi transformar a defesa em uma interação ativa de reflexo: acertar a direção reduz o dano recebido pela metade, enquanto errar ou deixar o tempo esgotar aplica o dano integral subtraído da resistência do jogador.
  3. **Consistência matemática e integração narrativa:** O cálculo de dano impõe um piso mínimo (`MIN_DAMAGE = 1`) para evitar combates travados em zero de dano. Ao detectar vitória ou derrota (`CheckCombatEnd`), o sistema concede automaticamente moedas e itens de recompensa definidos no `.txt` do inimigo e redireciona o fluxo para os nós narrativos configurados em `winTarget` ou `loseTarget`.

### 2.7. Quebra de Linha Automática (`DrawTextWrapped`)
- **O que foi usado:** Algoritmo de tokenização de palavras (`strtok`) combinado com medição de largura em pixels (`MeasureText`).
- **Por que foi utilizado:** A função `DrawText` da Raylib não realiza quebra de linha automática quando o texto ultrapassa a largura da janela. A função auxiliar acumula palavras até atingir o limite máximo em pixels definido para a caixa de texto, inserindo quebras de linha dinamicamente.

### 2.8. Automação de Build (`Makefile`)
- **O que foi usado:** Função `$(wildcard ...)` atribuída imediatamente (`:=`) à variável `OBJS` no `Makefile`.
- **Por que foi utilizado:** Permite que o compilador (`gcc`) localize e compile automaticamente qualquer novo arquivo `.c` adicionado às subpastas do projeto (como a inclusão de `Combat.c`), eliminando a necessidade de atualizar manualmente as listas de compilação a cada novo módulo criado.

---

## 3. Créditos e Ferramentas de Apoio
- **Desenvolvimento, Arquitetura e Criação da História:** Cristofer Dvorecky Oliveira (RA: 26020823)
- **Criação de Personagens e Itens e Revisão de Código:** Augusto do Amarante (RA: 26018423)
- **Esboço e Prototipação da História:** **Twine**, utilizado como ferramenta de apoio para esboçar o fluxo narrativo e as ramificações da história antes da implementação nos arquivos `.txt`.
- **Apoio Técnico:** Inteligência Artificial **Google Gemini** utilizada como ferramenta de consulta e auxílio na estruturação da arquitetura de troca de telas (`ScreenManager`), na integração da leitura de arquivos `.txt` com os scripts em C e na criação dos sistemas de **QTE**, **combate por turnos** e **gerenciamento de itens**.