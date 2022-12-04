// Valores contantes utilizados por todo o jogo

// Pino do botão para se movimentar para a direita
#define RIGHT_MOVEMENT 2
// Pino do botão para se movimentar para a esquerda
#define LEFT_MOVEMENT 3
// Pino do botão para se movimentar para cima
#define UP_MOVEMENT 4
// Pino do botão para se movimentar para baixo
#define DOWN_MOVEMENT 5
// Pino do botão para realizar o marco polo
#define ACTION 6

// Altura total da tela
#define SCREEN_HEIGHT 96
// Largura total da tela
#define SCREEN_WIDTH 120

// Altura do olho (player)
#define EYE_HEIGHT 4
// Largura do olho (player)
#define EYE_WIDTH 8

// Altura do inimigo
#define ENEMY_HEIGHT 16
// Largura do inimigo
#define ENEMY_WIDTH 12
// Valor que indica se um inimigo está desabilitado
#define ENEMY_DISABLED 0
// Valor que indica se um inimigo está habilitado
#define ENEMY_ENABLED 1

// Define por quanto tempo o jogador precisa segurar para ativar o marco polo
#define TIME_MARCO_POLO 35
// Quantos marco polo o jogador pode usar
#define MARCO_POLO_CHANCES 4
// Por quanto tempo os indicadores de marco polo irão aparecer na tela,
// caso o jogador mantenha o botão pressionado
#define MARCO_POLO_SHOWING_TIME 70

// Duração total do jogo em segundos
#define GAME_DURATION 20
// Por quanto tempo o indicador de que o player achou inimigo irá aparecer na tela
#define DURATION_TO_SHOW_FOUND_SIGNALIZATION 50

// Todos os estados em que o jogo pode estar
enum GAME_STATES {
  // Indica que o jogador está na tela de início
  IN_START_SCREEN = 1,
  // Indica que o jogador está jogando
  PLAYING,
  // Indica que o jogador perdeu
  LOST,
  // Indica que o jogador ganhou
  WIN
};

// Espessura da parede direita do quarto quando está apagado
#define RIGHT_WALL_WIDTH 6
// Espessura da parede esquerda do quarto quando está apagado
#define LEFT_WALL_WIDTH 6
// Altura da parede superior do quarto quando está apagado
#define UP_WALL_HEIGHT 16
// Altura da parede inferior do quarto quando está apagado
#define DOWN_WALL_HEIGHT 6

// Sons
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

#define SOUND 8

// Som que representa que foi encontrado um inimigo
// Código para ser acessado no jogo: 10
const int sound_found[] = {
  NOTE_B5, NOTE_C6, NOTE_E6
};

// Som que representa que o marco polo foi usado
// Código para ser acessado no jogo: 20
const int sound_call[] = {
  NOTE_B6, NOTE_E7, NOTE_B6, NOTE_E7, NOTE_B6
};

// Som que representa que o jogador venceu
// Código para ser acessado no jogo: 30
const int sound_win[] = {
  NOTE_FS5, NOTE_FS5, NOTE_D5, NOTE_B4, REST, NOTE_B4, REST, NOTE_E5,
  REST, NOTE_E5, REST, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5,
  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, REST, NOTE_D5, REST, NOTE_FS5,
  REST, NOTE_FS5, REST, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5,
  NOTE_FS5, NOTE_FS5, NOTE_D5, NOTE_B4, REST, NOTE_B4, REST, NOTE_E5,

  REST, NOTE_E5, REST, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5,
  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, REST, NOTE_D5, REST, NOTE_FS5,
  REST, NOTE_FS5, REST, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5,
  NOTE_FS5, NOTE_FS5, NOTE_D5, NOTE_B4, REST, NOTE_B4, REST, NOTE_E5,
  REST, NOTE_E5, REST, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5,

  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, REST, NOTE_D5, REST, NOTE_FS5,
  REST, NOTE_FS5, REST, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5
};

// Som que representa que o jogador perdeu
// Código para ser acessado no jogo: 40
const int sound_lose[] = {
  NOTE_B4, NOTE_G4, NOTE_D4, NOTE_CS4, NOTE_B4, NOTE_G4, NOTE_CS4,
  NOTE_A4, NOTE_FS4, NOTE_C4, NOTE_B3, NOTE_F4, NOTE_D4, NOTE_B3, NOTE_E4
};