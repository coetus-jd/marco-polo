#include <TVout.h>
#include <TVoutfonts/fontALL.h>
#include "constants.h"
#include "utils.h"
#include "logo.h"
#include "room_image.h"

using namespace std;

TVout tv;

int eyeX = 0;
int eyeY = 0;
int maxHeight = 0;
int maxWidth = 0;

// Marco polo
int marco_polo_timer = 0;
int marco_polo_used = MARCO_POLO_CHANCES;
bool showing_marco_polo = false;

// Timing
int timer = 120;
int milliSeconds = 0;

// Positions
int possible_positions[5][2] = {
  // X   Y
  { 15, 15 },
  { 50, 75 },
  { 20, 60 },
  { 60, 80 },
  { 15, 80 },
};

int ALL_CONTROLS[5] = { RIGHT_MOVEMENT, LEFT_MOVEMENT, UP_MOVEMENT, DOWN_MOVEMENT, ACTION };

String title = "Pique esconde";

bool colliding = false;

void setup() {
  configure_buttons();
  configure_libraries();
  configure_debugger();
  configure_screen();

  // Starts the eye in the center
  eyeX = maxHeight / 2;
  eyeY = maxWidth / 2;

  start_screen();
}

void loop() {
  tv.delay_frame(1);
  tv.clear_screen();

  tv.draw_rect(0, 0, maxWidth, maxHeight, WHITE);
  
  show_debug_info();
  draw_all_enemies();
  draw_eyes();
  // collision_confirm(enemyX, enemyY, ENEMY_WIDTH, ENEMY_HEIGHT);
  activate_marco_polo();

  time();
}

void start_screen() {
  bool pressed = false;
  int center_point = 0;

  while (!pressed) {
    center_point = text_size(title, maxWidth);

    show_text(tv, center_point, maxHeight - 10, title, font4x6);

    pressed = has_pressed_any_button();

    tv.delay_frame(1);
    tv.clear_screen();
  }

  run();

  // tv.bitmap((tv.hres() - pong_logo[0]) / 2 + 11, (tv.vres() - pong_logo[1]) / 2, pong_logo);

  // show_text(tv, SCREEN_WIDTH / 4, SCREEN_HEIGHT - 80, "Game name");
  // show_text(tv, SCREEN_WIDTH / 3, SCREEN_HEIGHT - 40, "Play");
  // show_text(tv, 10, SCREEN_HEIGHT - 10, "By Astha", font4x6);
  // show_text(tv, SCREEN_WIDTH - 50, SCREEN_HEIGHT - 10, "FATEC 2022", font4x6);
}

void run() {
  // scenery();
  // tv.delay_frame(75);
  // create_enemies();
}

void configure_buttons() {
  pinMode(RIGHT_MOVEMENT, INPUT_PULLUP);
  pinMode(LEFT_MOVEMENT, INPUT_PULLUP);
  pinMode(UP_MOVEMENT, INPUT_PULLUP);
  pinMode(DOWN_MOVEMENT, INPUT_PULLUP);
  pinMode(ACTION, INPUT_PULLUP);
}

void configure_libraries() {
  tv.begin(NTSC, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void configure_screen() {
  maxWidth = tv.hres() - 10;
  maxHeight = tv.vres() - 3;
}

void activate_marco_polo() {
  // If is showing the indicator and already used all marco polos, we need to await and continue showing
  // the indicators 
  if (marco_polo_used == 0 && !showing_marco_polo) {
    Serial.println("Used all marco polos");
    return;
  }

  if (showing_marco_polo) {
    draw_marco_polo_indicators();
  }

  if (digitalRead(ACTION) == LOW) {
    marco_polo_timer++;
    Serial.println(marco_polo_timer);
  }

  if (digitalRead(ACTION) == HIGH) {
    reset_marco_polo();
    return;
  }

  // Controls how many the marco polo indicators (?, !) will be displayed
  if (showing_marco_polo && ((marco_polo_timer - TIME_MARCO_POLO) >= MARCO_POLO_SHOWING_TIME)) {
    // Serial.println("Reset marco polo");
    reset_marco_polo();
    return;
  }

  if (!showing_marco_polo && marco_polo_timer >= TIME_MARCO_POLO) {
    // Serial.println("Show marco polo");
    marco_polo();
  }
}

void draw_eyes() {
  // Left
  tv.draw_rect(eyeX, eyeY, 3, 3, WHITE);
  // Right
  tv.draw_rect(eyeX + 5, eyeY, 3, 3, WHITE);

  if (eyeX < (maxWidth - EYE_WIDTH - 1) && digitalRead(RIGHT_MOVEMENT) == LOW) {
    // Left iris
    tv.draw_rect(eyeX + 1, eyeY + 1, 1, 1, WHITE);
    tv.draw_rect(eyeX + 2, eyeY + 1, 1, 1, BLACK);

    // Right iris
    tv.draw_rect(eyeX + 6, eyeY + 1, 1, 1, WHITE);
    tv.draw_rect(eyeX + 7, eyeY + 1, 1, 1, BLACK);
    eyeX++;
  }

  if (eyeX > 1 && digitalRead(LEFT_MOVEMENT) == LOW) {
    // Left iris
    tv.draw_rect(eyeX + 1, eyeY + 1, 1, 1, WHITE);
    tv.draw_rect(eyeX, eyeY + 1, 1, 1, BLACK);

    // Right iris
    tv.draw_rect(eyeX + 6, eyeY + 1, 1, 1, WHITE);
    tv.draw_rect(eyeX + 5, eyeY + 1, 1, 1, BLACK);
    eyeX--;
  }

  if (eyeY > 1 && digitalRead(UP_MOVEMENT) == LOW) {
    // Left iris
    tv.draw_rect(eyeX + 1, eyeY + 1, 1, 1, WHITE);
    tv.draw_rect(eyeX + 1, eyeY, 1, 1, BLACK);

    // Right iris
    tv.draw_rect(eyeX + 6, eyeY + 1, 1, 1, WHITE);
    tv.draw_rect(eyeX + 6, eyeY, 1, 1, BLACK);
    eyeY--;
  }

  if (eyeY < (maxHeight - EYE_HEIGHT - 1) && digitalRead(DOWN_MOVEMENT) == LOW) {
    // Left iris
    tv.draw_rect(eyeX + 1, eyeY + 1, 1, 1, WHITE);
    tv.draw_rect(eyeX + 1, eyeY + 2, 1, 1, BLACK);

    // Right iris
    tv.draw_rect(eyeX + 6, eyeY + 1, 1, 1, WHITE);
    tv.draw_rect(eyeX + 6, eyeY + 2, 1, 1, BLACK);
    eyeY++;
  }
}

void configure_debugger() {
  Serial.begin(9650);
}

void show_debug_info() {
  tv.select_font(font4x6);

  // Show the current X value of the player
  tv.print(2, 5, eyeX);
  // Show the current Y value of the player
  tv.print(2, 15, eyeY);
}

void draw_all_enemies() {
  for (auto &positions : possible_positions)
  {
    int x = positions[0];
    int y = positions[1];

    draw_enemy(x, y);

    // for (auto &coordinates : positions)
    // {
    //   Serial.println(coordinates);
    // }
  }
}

void draw_enemy(int x, int y) {
  tv.draw_rect(x, y, ENEMY_WIDTH, ENEMY_HEIGHT, WHITE);
}

bool has_pressed_any_button() {
  int controls_length = (sizeof(ALL_CONTROLS) / sizeof(ALL_CONTROLS[0]));

  bool pressed = false;

  for (int index = 0; index < controls_length; index++) {
    if (digitalRead(ALL_CONTROLS[index]) == LOW) {
      pressed = true;
    }
  }

  return pressed;
}

void collision_confirm(int enemyX, int enemyY, int enemyWidth, int enemyHeight) {
  // tv.draw_rect(enemyX, enemyY, enemyWidth, enemyHeight, WHITE);

  colliding = (eyeX + EYE_WIDTH >= enemyX && eyeX <= enemyX + enemyWidth)
              && (eyeY + EYE_HEIGHT >= enemyY && eyeY <= enemyY + enemyHeight);
}

void time() {
  int seconds = millis() - milliSeconds;

  if (seconds >= GAME_DURATION)
  {
    timer = timer - 1;
    // Serial.println(timer);
    // Serial.println(seconds);
    // Serial.println(milliSeconds);
    milliSeconds = millis();
  }

  // game_over();
}

void scenery() {
  tv.bitmap((tv.hres() - room_image[0]) - 10, (tv.vres() - room_image[1]) / 2, room_image);
}

void game_over() {
  bool pressed = false;
  int center_point = 0;
  int center_point_desc = 0;

  while (!pressed) {
    center_point = text_size("Fim de Jogo", maxWidth, 8);
    show_text(tv, center_point, maxHeight - 55, "Fim de Jogo");

    pressed = has_pressed_any_button();

    tv.delay_frame(1);
    tv.clear_screen();
  }
}

void marco_polo() {
  marco_polo_used--;
  showing_marco_polo = true;
  draw_marco_polo_indicators();
}

void draw_marco_polo_indicators() {
  show_text(tv, eyeX + 3, eyeY - 6, "?", font4x6);

  for (auto &positions : possible_positions)
  {
    int x = positions[0];
    int y = positions[1];

    show_text(tv, x, y - 6, "!", font4x6);
  }
}

void reset_marco_polo() {
  marco_polo_timer = 0;
  showing_marco_polo = false;
}