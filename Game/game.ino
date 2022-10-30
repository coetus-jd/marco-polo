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
// Para teste
int enemyX = 50;
int enemyY = 50;

// Timing
int timer = 120;
int milliSeconds = 0;

int ALL_CONTROLS[5] = { RIGHT_MOVEMENT, LEFT_MOVEMENT, UP_MOVEMENT, DOWN_MOVEMENT, ACTION };

String title = "Pique esconde";

bool collision = false;

void setup() {
  configure_buttons();
  configure_libraries();
  configure_debugger();

  maxWidth = tv.hres() - 10;
  maxHeight = tv.vres() - 3;

  eyeX = maxHeight / 2;
  eyeY = maxWidth / 2;

  start_screen();
}

void loop() {
  tv.delay_frame(1);
  tv.clear_screen();
  create_enemies();

  tv.draw_rect(0, 0, maxWidth, maxHeight, WHITE);
  show_debug_info();
  draw_eyes();
  collision_confirm(enemyX, enemyY, ENEMY_WIDTH, ENEMY_HEIGHT);

  time();
}


void run() {
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

  scenery();
  tv.delay_frame(75);

  // tv.bitmap((tv.hres() - pong_logo[0]) / 2 + 11, (tv.vres() - pong_logo[1]) / 2, pong_logo);

  // show_text(tv, SCREEN_WIDTH / 4, SCREEN_HEIGHT - 80, "Game name");
  // show_text(tv, SCREEN_WIDTH / 3, SCREEN_HEIGHT - 40, "Play");
  // show_text(tv, 10, SCREEN_HEIGHT - 10, "By Astha", font4x6);
  // show_text(tv, SCREEN_WIDTH - 50, SCREEN_HEIGHT - 10, "FATEC 2022", font4x6);
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
}

void draw_eyes() {
  // Left
  tv.draw_rect(eyeX, eyeY, 3, 3, WHITE);
  // Right
  tv.draw_rect(eyeX + 5, eyeY, 3, 3, WHITE);

  if (eyeX < SCREEN_WIDTH && digitalRead(RIGHT_MOVEMENT) == LOW) {
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

  if (eyeY < SCREEN_HEIGHT && digitalRead(DOWN_MOVEMENT) == LOW) {
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
  tv.print(2, 5, eyeX);
  tv.print(2, 15, eyeY);
}

void create_enemies() {
  tv.draw_rect(30, 40, ENEMY_WIDTH, ENEMY_HEIGHT, WHITE);
  // draw_enemy();
}

// void draw_enemy() {
//   tv.draw_rect(eyeX + 1, eyeY + 1, ENEMY_WIDTH, ENEMY_HEIGHT, WHITE);
// }

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
  tv.draw_rect(enemyX, enemyY, enemyWidth, enemyHeight, WHITE);

  if (
    (eyeX + EYE_WIDTH >= enemyX && eyeX <= enemyX + enemyWidth)
    && (eyeY + EYE_HEIGHT >= enemyY && eyeY <= enemyY + enemyHeight)
  )
  {
    //Serial.println("Colidindo");
    collision = true;
  }
  else
  {
    //Serial.println("Não Colidindo");
    collision = false;
  }
}

void time() {
  int seconds = millis() - milliSeconds;

  if (seconds >= 1000)
  {
    timer = timer - 1;
    Serial.println(timer);
    Serial.println(seconds);
    Serial.println(milliSeconds);
    milliSeconds = millis();
  }
}

void scenery() {
  tv.bitmap((tv.hres() - room_image[0]) - 10, (tv.vres() - room_image[1]) / 2, room_image);
}