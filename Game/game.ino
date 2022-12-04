// Importa a biblioteca TVOut e sua biblioteca para fontes
#include <TVout.h>
#include <TVoutfonts/fontALL.h>
// Valores contantes utilizados por todo o jogo
#include "constants.h"

// Classe de utilidade, com abstrações de tarefas comuns
#include "utils.h"

// Imagem da logo do projeto
#include "logo.h"

// Imagem da sala do jogo quando as luzes estão acesas
#include "room_image.h"

// Imagem da sala do jogo quando as luzes estão apagadas
// Não utilizado pois estava ficando muito pesado no jogo, foi recriado a imagem
// manualmente na função (draw_game_delimiters)
// #include "dark_scenary.h"

// Imagem com um balão de "Marco"
#include "marco_icon.h"

// Imagem com um balão de "Polo"
#include "polo_icon.h"

// Objeto da biblioteca TVOut
TVout tv;

// Variáveis relacionadas ao game no geral
int current_game_state = 0;
char game_name[] = "Neutron games";
// Controla por quanto tempo a sinalização de que o jogador achou um inimigo está sendo exibida
int signalization_found_time = 0;
// Indica se deve ou não exibir a sinalização de que o jogador achou um inimigo
bool show_signalization_found = false;

// Posição em X do player
int eyeX = 0;
// Posição em Y do player
int eyeY = 0;
// Altura jogável
int maxHeight = 0;
// Largura jogável
int maxWidth = 0;

// Lista com todos os pinos dos controles (botões) do jogo
int ALL_CONTROLS[5] = {
  RIGHT_MOVEMENT,
  LEFT_MOVEMENT,
  UP_MOVEMENT,
  DOWN_MOVEMENT,
  ACTION
};

// Variáveis relacionadas ao marco polo
// Contabilizador de quanto tempo o jogador está pressionando o botão de marco polo
int marco_polo_timer = 0;
// Quantos marcos polos já foram usados
int marco_polo_used = MARCO_POLO_CHANCES;
// Indica se as imagens de marco e polo estão sendo exibidas
bool showing_marco_polo = false;

// Variáveis relacionadas a duração do game
int timer = GAME_DURATION;
// Quanto vale 1 segundo no jogo
int one_second = 333;
// Guarda o total de tempo jogado
int milli_seconds = 0;

// Matriz com todas as posições dos inimigos (posições geradas
// aleatoriamente posteriormente na função generate_enemies_positions())
int possible_positions[5][4] = {
  // Está habilitado?    X  Y  ID
  { ENEMY_DISABLED,      0, 0, 0 },
  { ENEMY_DISABLED,      0, 0, 0 },
  { ENEMY_DISABLED,      0, 0, 0 },
  { ENEMY_DISABLED,      0, 0, 0 },
  { ENEMY_DISABLED,      0, 0, 0 },
};

// Controles de colisão
// Indica se o player está colidindo com algum inimigo
bool colliding = false;
// Id do inimigo colidido
int enemy_id_collided = 0;
// Contabilizar do total de inimigos encontrados
int enemies_found = 0;

// Inicializa o jogo
void setup() {
  // Alimenta o randomizador do Arduino para tentar ao máximo gerar números diferentes
  randomSeed(analogRead(A0));
  
  configure_buttons();
  configure_libraries();
  configure_debugger();
  configure_screen();

  // Coloca o olho iniciando sempre ao centro
  eyeX = (maxWidth / 2) - 4;
  eyeY = maxHeight - 20;

  generate_enemies_positions();

  start_screen();
}

// Método que será chamado a cada frame no jogo
// OBS: Não usar o Serial.print dentro do loop, por alguma razão alguns comportamentos estranhos
// acontecem dentro do Wowki
void loop() {
  // Caso o jogador tenha vencido o jogo é necessário somente exibir a tela de vencer
  if (current_game_state == WIN) {
    win_screen();
    return;
  }

  // Caso o jogador tenha vencido o jogo é necessário somente exibir a tela de perder
  if (current_game_state == LOST) {
    game_over_screen();
    return;
  }

  // No começo de cada frame a tela é sempre limpada
  tv.delay_frame(1);
  tv.clear_screen();

  draw_game_delimiters();

  draw_all_enemies();
  draw_eyes();

  show_debug_info();

  verify_if_has_collided_with_enemy();

  control_game_time();
  activate_marco_polo();

  if (show_signalization_found) {
    show_signalization();
  }
}

// Configura todos os botões e componentes utilizados no jogo
void configure_buttons() {
  pinMode(RIGHT_MOVEMENT, INPUT_PULLUP);
  pinMode(LEFT_MOVEMENT, INPUT_PULLUP);
  pinMode(UP_MOVEMENT, INPUT_PULLUP);
  pinMode(DOWN_MOVEMENT, INPUT_PULLUP);
  pinMode(ACTION, INPUT_PULLUP);
  pinMode(SOUND, OUTPUT);
}

// Configura todas as bibliotecas utilizadas
void configure_libraries() {
  tv.begin(NTSC, SCREEN_WIDTH, SCREEN_HEIGHT);
}

// Configura o tamanho da tela jogável
void configure_screen() {
  maxWidth = tv.hres() - 10;
  maxHeight = tv.vres() - 3;
}

// Configura o debugger do Arduino
void configure_debugger() {
  Serial.begin(9650);
}

// Desenha a imagem do quarto com a luz apagada
void draw_game_delimiters() {
  // Método antigo usando a imagem:
  // tv.bitmap((tv.hres() - dark_scenary[0]) - 10, (tv.vres() - dark_scenary[1]) / 2, dark_scenary);

  // Desenha um quadrado branco por volta da tela
  tv.draw_rect(0, 0, maxWidth, maxHeight, WHITE);

  // Desenha o quadrado por volta de toda a tela jogável
  tv.draw_rect(
    RIGHT_WALL_WIDTH,
    UP_WALL_HEIGHT,
    maxWidth - 12,
    maxHeight - 24,
    WHITE
  );

  // Linha no canto superior esquerdo
  tv.draw_line(1, 1, RIGHT_WALL_WIDTH, UP_WALL_HEIGHT, WHITE);
  // Linha no canto inferior esquerdo
  tv.draw_line(
    1,
    maxHeight - 1,
    LEFT_WALL_WIDTH - 1,
    maxHeight - DOWN_WALL_HEIGHT - 2, WHITE
  );

  // Linha no canto superior direito
  tv.draw_line(maxWidth, 0, maxWidth - RIGHT_WALL_WIDTH, UP_WALL_HEIGHT, WHITE);
  // Linha no canto inferior direito
  tv.draw_line(
    maxWidth - 1,
    maxHeight - 1,
    maxWidth - RIGHT_WALL_WIDTH,
    maxHeight - DOWN_WALL_HEIGHT - 2,
    WHITE
  );

  // Desenha a porta
  tv.draw_line(
    (maxWidth / 2) - 10,
    maxHeight - 1,
    (maxWidth / 2) + 10,
    maxHeight - 1,
    BLACK
  );
  tv.draw_line(
    (maxWidth / 2) - 10,
    maxHeight - 1,
    (maxWidth / 2) - 8,
    maxHeight - DOWN_WALL_HEIGHT - 1,
    WHITE
  );
  tv.draw_line(
    (maxWidth / 2) + 10,
    maxHeight - 1,
    (maxWidth / 2) + 8,
    maxHeight - DOWN_WALL_HEIGHT - 1,
    WHITE
  );
}

// Exibe a tela de início
void start_screen() {
  current_game_state = IN_START_SCREEN;

  bool pressed = false;

  // Limpa a tela
  tv.delay_frame(1);
  tv.clear_screen();

  tv.draw_rect(0, 0, maxWidth, maxHeight, WHITE);

  tv.bitmap(8, 10, logo);
  show_text(tv, 10, maxHeight - 20, "BY ASTHA", font4x6);
  show_text(tv, maxWidth - 40, maxHeight - 20, "FATEC AM", font4x6);

  // Delay para o jogador não pressionar os botões muito rápido e acabar nem
  // vendo a tela de start
  delay(2000);

  // Enquanto o jogador não pressionar nenhum botão o jogo não irá prosseguir
  do {
    pressed = has_pressed_any_button();
  } while (!pressed);

  // Limpa a tela
  tv.delay_frame(1);
  tv.clear_screen();

  current_game_state = PLAYING;
  scenery();
}

// Exibe a tela de perdeu
void game_over_screen() {
  bool pressed = false;
  char you_lose[] = "You lost";

  // Limpa a tela
  tv.delay_frame(1);
  tv.clear_screen();

  tv.draw_rect(0, 0, maxWidth, maxHeight, WHITE);

  show_text(tv, 20, 10, you_lose);
  show_text(tv, 40, 35, ":)");
  show_text(tv, 20, maxHeight - 35, "Press any button", font4x6);
  show_text(tv, 25, maxHeight - 25, "to play again", font4x6);

  // Som de derrota
  // play_sound(200, 40);

  // Delay para o jogador não pressionar os botões muito rápido e acabar nem
  // vendo a tela de perdeu
  delay(2000);

  // Enquanto o jogador não pressionar nenhum botão o jogo não irá prosseguir
  do {
    pressed = has_pressed_any_button();
  } while (!pressed);

  // Limpa a tela
  tv.delay_frame(1);
  tv.clear_screen();

  reset_game();
  start_screen();
}

// Exibe a tela de venceu
void win_screen() {
  bool pressed = false;
  
  // Limpa a tela
  tv.delay_frame(1);
  tv.clear_screen();

  tv.draw_rect(0, 0, maxWidth, maxHeight, WHITE);

  show_text(tv, 20, 10, "You win!");
  show_text(tv, 40, 35, ":(");
  show_text(tv, 20, maxHeight - 35, "Press any button", font4x6);
  show_text(tv, 25, maxHeight - 25, "to play again", font4x6);

  // Som de vitória
  // play_sound(150, 30);

  // Delay para o jogador não pressionar os botões muito rápido e acabar nem
  // vendo a tela de venceu
  delay(2000);

  // Enquanto o jogador não pressionar nenhum botão o jogo não irá prosseguir
  do {
    pressed = has_pressed_any_button();
  } while (!pressed);

  // Limpa a tela
  tv.delay_frame(1);
  tv.clear_screen();

  reset_game();
  start_screen();
}

// Controla o marco polo
void activate_marco_polo() {
  // Se estiver sendo mostrado os indicadores e ainda o jogador já usou todos os marcos
  // polos, precisamos esperar terminar de exibir os indicadores
  if (marco_polo_used == 0 && !showing_marco_polo)
    return;

  // Se estiver mostrando o marco polo, devemos chamar a função que desenha os elementos
  if (showing_marco_polo)
    draw_marco_polo_indicators();

  // Enquanto o botão de marco polo estiver pressionado adicionamos +1 ao timer
  if (digitalRead(ACTION) == LOW)
    marco_polo_timer++;

  // Assim que o botão de marco polo for soltado devemos parar de exibir os indicadores de marco polo
  if (digitalRead(ACTION) == HIGH) {
    reset_marco_polo();
    return;
  }

  // Controla por quanto tempo os indicadores de marco polo serão exibidos
  if (
    showing_marco_polo &&
    ((marco_polo_timer - TIME_MARCO_POLO) >= MARCO_POLO_SHOWING_TIME)
  ) {
    reset_marco_polo();
    return;
  }

  // Caso os marcos polos não estejam sendo exibidos e o jogador já pressionou
  // o botão de ação por tempo suficiente, exibimos os indicadores de marco polo
  if (!showing_marco_polo && marco_polo_timer >= TIME_MARCO_POLO)
    marco_polo();
}

// Desenha os olhos (player)
void draw_eyes() {
  // Desenha o olho esquerdo
  tv.draw_rect(eyeX, eyeY, 3, 4, WHITE);
  // Desenha o olho direito
  tv.draw_rect(eyeX + 5, eyeY, 3, 4, WHITE);

  // Se acaso o jogador estiver se movendo para a direita, devemos colocar a iris
  // do olho para a direita
  if (eyeX < (maxWidth - EYE_WIDTH - 8) && digitalRead(RIGHT_MOVEMENT) == LOW) {
    // Iris esquerda
    tv.draw_rect(eyeX + 1, eyeY + 1, 1, 2, WHITE);
    tv.draw_rect(eyeX + 2, eyeY + 1, 1, 2, BLACK);

    // Iris direita
    tv.draw_rect(eyeX + 6, eyeY + 1, 1, 2, WHITE);
    tv.draw_rect(eyeX + 7, eyeY + 1, 1, 2, BLACK);
    eyeX++;
  }

  // Se acaso o jogador estiver se movendo para a esquerda, devemos colocar a iris
  // do olho para a esquerda
  if (eyeX > 8 && digitalRead(LEFT_MOVEMENT) == LOW) {
    // Iris esquerda
    tv.draw_rect(eyeX + 1, eyeY + 1, 1, 2, WHITE);
    tv.draw_rect(eyeX, eyeY + 1, 1, 2, BLACK);

    // Iris direita
    tv.draw_rect(eyeX + 6, eyeY + 1, 1, 2, WHITE);
    tv.draw_rect(eyeX + 5, eyeY + 1, 1, 2, BLACK);
    eyeX--;
  }

  // Se acaso o jogador estiver se movendo para cima, devemos colocar a iris
  // do olho para a cima
  if (eyeY > 18 && digitalRead(UP_MOVEMENT) == LOW) {
    // Iris esquerda
    tv.draw_rect(eyeX + 1, eyeY + 1, 1, 2, WHITE);
    tv.draw_rect(eyeX + 1, eyeY, 1, 2, BLACK);

    // Iris direita
    tv.draw_rect(eyeX + 6, eyeY + 1, 1, 2, WHITE);
    tv.draw_rect(eyeX + 6, eyeY, 1, 2, BLACK);
    eyeY--;
  }

  // Se acaso o jogador estiver se movendo para baixo, devemos colocar a iris
  // do olho para a baixo
  if (eyeY < (maxHeight - EYE_HEIGHT - 10) && digitalRead(DOWN_MOVEMENT) == LOW) {
    // Iris esquerda
    tv.draw_rect(eyeX + 1, eyeY + 1, 1, 2, WHITE);
    tv.draw_rect(eyeX + 1, eyeY + 2, 1, 2, BLACK);

    // Iris direita
    tv.draw_rect(eyeX + 6, eyeY + 1, 1, 2, WHITE);
    tv.draw_rect(eyeX + 6, eyeY + 2, 1, 2, BLACK);
    eyeY++;
  }
}

// Mostra informações de depuração na tela
void show_debug_info() {
  tv.select_font(font4x6);

  // Mostra a atual posição em X do player
  // tv.print(2, 5, eyeX);
  // Mostra a atual posição em Y do player
  // tv.print(2, 15, eyeY);

  // Mostra o número de marco polos usados no canto superior direito
  tv.print(maxWidth - 12, 5, marco_polo_used);

  // Mostra o número de inimigos encontrados no canto inferior direito
  tv.print(maxWidth - 12, maxHeight - 15, enemies_found);
}

// Desenha todos os inimigos na tela
void draw_all_enemies() {
  // Para cada posição gerada aleatoriamente desenhamos um inimigo
  for (auto &positions : possible_positions) {
    int enabled = positions[0];

    int x = positions[1];
    int y = positions[2];

    // Caso o inimigo esteja desabilitado nem prosseguimos, apenas exibimos um olho
    // para o jogador saber que ele já passou por ali
    if (enabled == ENEMY_DISABLED)
    {
      draw_enemy(x, y);
      continue;
    }
   
    int id = positions[3];
  }
}

// Desenha o inimigo quando encontrado
void draw_enemy(int x, int y) {
  // Olho esquerdo
  tv.draw_rect(x, y, 3, 3, WHITE);
  // Olho direito
  tv.draw_rect(x + 5, y, 3, 3, WHITE);
}

// Verifica se o jogador pressionou qualquer botão
bool has_pressed_any_button() {
  // Total do número de botões (no caso do jogo 5)
  int controls_length = (sizeof(ALL_CONTROLS) / sizeof(ALL_CONTROLS[0]));

  // Indica se foi pressionado algum botão
  bool pressed = false;

  // Para cada botão verificamos se ele foi pressionado ou não
  for (int index = 0; index < controls_length; index++) {
    if (digitalRead(ALL_CONTROLS[index]) == LOW) {
      pressed = true;
    }
  }

  return pressed;
}

// Verifica se o player colidiu com algum inimigo
void verify_if_has_collided_with_enemy() {
  // Caso o player não esteja tentando encontrar nenhum inimigo (pressionando o botão de ação)
  // nem precisamos verificar as colisões (economizando performance)
  if (digitalRead(ACTION) != LOW)
    return;

  for (auto &positions : possible_positions) {
    bool disable = positions[0] == ENEMY_DISABLED;

    if (disable)
      continue;

    int x = positions[1];
    int y = positions[2];
    int id = positions[3];

    collision_confirm(x, y, id);

    // Se colidiu com qualquer que seja o inimigo paramos o loop por completo
    if (colliding)
      break;
  }

  // Se o player não colidiu com nenhum inimigo nem prosseguimos
  if (!colliding)
    return;

  show_signalization_found = true;

  disable_enemy(enemy_id_collided);
  enemies_found++;

  // Som quando acha um inimigo
  // play_sound(100, 10);

  // Indica se todos os inimigos foram encontrados
  bool find_all = found_all_enemies();

  // Se todos os inimigos foram encontrados, quer dizer que o jogador ganhou
  if (find_all) {
    delay(500);
    current_game_state = WIN;
  }
}

// Verifica se um inimigo colidiu com o player ou vice-versa
void collision_confirm(int enemy_x, int enemy_y, int enemy_id) {
  colliding = (eyeX + EYE_WIDTH >= enemy_x && eyeX <= enemy_x + ENEMY_WIDTH) &&
    (eyeY + EYE_HEIGHT >= enemy_y && eyeY <= enemy_y + ENEMY_HEIGHT);

  // Se sim, guardamos o id de qual inimigo o player colidiu
  if (colliding) {
    enemy_id_collided = enemy_id;
    return;
  }

  enemy_id_collided = 0;
}

// Controla o tempo que o jogador tem para ganhar o jogo
// Passando esse tempo e o jogador não achando todos os inimigos, é considerado que o jogador perdeu
void control_game_time() {
  int seconds = millis() - milli_seconds;
  tv.print(50, 5, timer);

  // Caso tenha passado o tempo determinado, tiramos 1s do tempo de jogo
  if (seconds >= one_second) {
    timer -= 1;
    milli_seconds = millis();
  }

  // Se o timer chegou a 0 quer dizer que acabou o tempo e o jogador perdeu
  if (timer == 0) {
    current_game_state = LOST;
  }
}

// Desenha o quarto com as luzes acesas
void scenery() {
  tv.bitmap(
    (tv.hres() - room_image[0]) - 10,
    (tv.vres() - room_image[1]) / 2,
    room_image
  );

  bool pressed = false;

  delay(2000);

  // Enquanto o jogador não pressionar nenhum botão o jogo não irá prosseguir
  do {
    pressed = has_pressed_any_button();
  } while (!pressed);

  tv.clear_screen();
}

// Inicializa um novo marco polo
void marco_polo() {
  // Som do Marco Polo
  // play_sound(150, 20);

  // Marcamos que utilizamos 1 marco polo
  marco_polo_used--;
  showing_marco_polo = true;
  draw_marco_polo_indicators();
}

// Desenha na tela os indicadores de "Marco" encima do player e "Polo" em cada inimigo
void draw_marco_polo_indicators() {
  // show_text(tv, eyeX + 3, eyeY - 6, "?", font4x6);
  tv.bitmap(eyeX - 4, eyeY - 15, marco_icon);

  for (auto &positions : possible_positions) {
    bool disable = positions[0] == ENEMY_DISABLED;

    if (disable)
      continue;

    int x = positions[1];
    int y = positions[2];

    // show_text(tv, x, y - 2, "!", font4x6);
    tv.bitmap(x - 5, y, polo_icon);
  }
}

// Volta o marco polo ao estado inicial
void reset_marco_polo() {
  marco_polo_timer = 0;
  showing_marco_polo = false;
}

// Desabilita um inimigo (o mesmo é encontrado)
void disable_enemy(int enemy_id_to_disable) {
  for (auto &positions : possible_positions) {
    int id = positions[3];

    if (id == enemy_id_to_disable) {
      positions[0] = ENEMY_DISABLED;
      break;
    }
  }
}

// Verifica se todos os inimigos foram encontrados
bool found_all_enemies() {
  int internal_enemies_found_quantity = 0;
  int quantity_of_enemies = sizeof(possible_positions) / sizeof(possible_positions[0]);

  for (auto &positions : possible_positions) {
    // Se um inimigo está desabilitado indica que ele foi encontrado
    if (positions[0] == ENEMY_DISABLED)
      internal_enemies_found_quantity++;
  }

  // Se o total de inimigos desabilitados é igual ao total de inimigos criados, então achamos todos
  bool found_all = internal_enemies_found_quantity == quantity_of_enemies;

  return found_all;
}

// Reseta o jogo por completo, voltando as variáveis ao seu estado original
void reset_game() {
  current_game_state = PLAYING;
  show_signalization_found = false;
  signalization_found_time = 0;

  eyeX = (maxWidth / 2) - 4;
  eyeY = maxHeight - 20;

  reset_marco_polo();
  marco_polo_used = MARCO_POLO_CHANCES;

  timer = GAME_DURATION;
  milli_seconds = 0;

  colliding = false;
  enemy_id_collided = 0;
  enemies_found = 0;

  generate_enemies_positions();
}

// Gera uma posição válida em X
int get_random_x_coordinate() {
  return random(20, maxWidth - 20);
}

// Gera uma posição válida em Y
int get_random_y_coordinate() {
  return random(20, maxHeight - 20);
}

// Gera randomicamente as posições dos inimigos em X e Y
void generate_enemies_positions() {
  int previous_id = 0;

  for (auto &positions : possible_positions) {
    // Gera um id numérico sequencial padrão (1, 2, 3, etc.)
    previous_id++;

    positions[0] = ENEMY_ENABLED;
    positions[1] = get_random_x_coordinate();
    positions[2] = get_random_y_coordinate();
    positions[3] = previous_id;
  }
}

// Mostra a sinalização de que o jogador achou um inimigo
void show_signalization() {
  signalization_found_time++;

  // Se a sinalização já foi exibida pelo tempo definido, devemos remover ela da tela
  if (signalization_found_time >= DURATION_TO_SHOW_FOUND_SIGNALIZATION) {
    show_signalization_found = false;
    signalization_found_time = 0;
    return;
  }

  show_text(tv, eyeX + 2, eyeY - 10, ":)", font4x6);
}

// Toca um som definido no arquivo de constants.h
// OBS: comentado pois ainda está causando bugs no jogo
// void play_sound(int time_delay, int which_sound) {
//   Serial.println("Chegou no ínicio");
//     if (which_sound == 10) //found
//     {
//       for (int i = 0; i < sizeof(sound_found); i++)

//       {
//         tone(SOUND, sound_found[i]);
//         delay(time_delay);
//       }
//     }
//     else if (which_sound == 20) //Marco Polo
//     {
//       for (int i = 0; i < sizeof(sound_call); i++)
//       {
//         tone(SOUND, sound_call[i]);
//         delay(time_delay);
//       }
//     }
//     else if (which_sound == 30) //Win
//     {
//       for (int i = 0; i < sizeof(sound_win); i++)
//       {
//         tone(SOUND, sound_win[i]);
//         delay(time_delay);
//       }
//     }
//     else if (which_sound == 40) //Lose
//     {
//       for (int i = 0; i < sizeof(sound_lose); i++)
//       {
//         tone(SOUND, sound_lose[i]);
//         delay(time_delay);
//       }

//     }

//     noTone(SOUND);
//     Serial.println("Chegou no fim");
// }