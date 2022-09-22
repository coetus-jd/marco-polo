#include <TVout.h>
#include <TVoutfonts/fontALL.h>
#include "constants.h"
#include "utils.h"
#include "logo.h"

TVout tv;

int eyeX = 0;
int eyeY = 0;

void setup() {
  configure_buttons();
  configure_libraries();
  //start_screen();
  //delay(1000);
}

void loop() {
  tv.delay_frame(1);
  tv.clear_screen();
  draw_eyes();
  // put your main code here, to run repeatedly:
}


void run() {
}

void start_screen() {
  tv.bitmap((tv.hres() - pong_logo[0]) / 2 + 11, (tv.vres() - pong_logo[1]) / 2, pong_logo);

  //show_text(tv, SCREEN_WIDTH / 4, SCREEN_HEIGHT - 80, "Game name");
  //show_text(tv, SCREEN_WIDTH / 3, SCREEN_HEIGHT - 40, "Play");
  //show_text(tv, 10, SCREEN_HEIGHT - 10, "By Astha", font4x6);
  //show_text(tv, SCREEN_WIDTH - 50, SCREEN_HEIGHT - 10, "FATEC 2022", font4x6);
}

void configure_buttons() {
  pinMode(RIGHT_MOVEMENT, INPUT_PULLUP);
  pinMode(LEFT_MOVEMENT, INPUT_PULLUP);
  pinMode(UP_MOVEMENT, INPUT_PULLUP);
  pinMode(DOWN_MOVEMENT, INPUT_PULLUP);
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
    tv.set_pixel(eyeX + 1, eyeY + 1, WHITE);
    tv.set_pixel(eyeX + 2, eyeY + 1, BLACK);

    // Right iris
    tv.set_pixel(eyeX + 6, eyeY + 1, WHITE);
    tv.set_pixel(eyeX + 7, eyeY + 1, BLACK);
    eyeX++;
  }

  if (eyeX > 1 && digitalRead(LEFT_MOVEMENT) == LOW) {
    // Left iris
    tv.set_pixel(eyeX + 1, eyeY + 1, WHITE);
    tv.set_pixel(eyeX, eyeY + 1, BLACK);

    // Right iris
    tv.set_pixel(eyeX + 6, eyeY + 1, WHITE);
    tv.set_pixel(eyeX + 5, eyeY + 1, BLACK);
    eyeX--;
  }
  
  if (eyeY > 1 && digitalRead(UP_MOVEMENT) == LOW) {
    // Left iris
    tv.set_pixel(eyeX + 1, eyeY + 1, WHITE);
    tv.set_pixel(eyeX + 1, eyeY, BLACK);

    // Right iris
    tv.set_pixel(eyeX + 6, eyeY + 1, WHITE);
    tv.set_pixel(eyeX + 6, eyeY, BLACK);
    eyeY--;
  }

  if (eyeY < SCREEN_HEIGHT && digitalRead(DOWN_MOVEMENT) == LOW) {
    // Left iris
    tv.set_pixel(eyeX + 1, eyeY + 1, WHITE);
    tv.set_pixel(eyeX + 1, eyeY + 2, BLACK);

    // Right iris
    tv.set_pixel(eyeX + 6, eyeY + 1, WHITE);
    tv.set_pixel(eyeX + 6, eyeY + 2, BLACK);
    eyeY++;
  }
}