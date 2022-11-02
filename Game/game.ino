#include <TVout.h>
#include <TVoutfonts/fontALL.h>
#include "constants.h"
#include "utils.h"
#include "logo.h"
#include "room_image.h"

// using namespace std;

TVout tv;

// Game
int current_game_state = IN_START_SCREEN;

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
int possible_positions[5][4] = {
  // Is enabled?   X   Y   Id
  { ENEMY_ENABLED, 15, 15, 01 },
  { ENEMY_ENABLED, 75, 60, 02 },
  { ENEMY_ENABLED, 20, 60, 03 },
  { ENEMY_ENABLED, 80, 80, 04 },
  { ENEMY_ENABLED, 15, 80, 05 },
};

int ALL_CONTROLS[5] = { RIGHT_MOVEMENT, LEFT_MOVEMENT, UP_MOVEMENT, DOWN_MOVEMENT, ACTION };

String title = "Pique esconde";

// Collision controls
bool colliding = false;
int enemy_id_collided = 0;
int enemies_found = 0;

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

// OBS: don't use Serial.print inside a loop, for some reason its breaks
void loop() {
  tv.delay_frame(1);
  tv.clear_screen();

  if (current_game_state == WIN)
  {
    win_screen();
    return;
  }

  if (current_game_state == LOST)
  {
    game_over_screen();
    return;
  }

  tv.draw_rect(0, 0, maxWidth, maxHeight, WHITE);

  show_debug_info();
  draw_all_enemies();
  draw_eyes();
  verify_if_has_collided_with_enemy();
  activate_marco_polo();

  // control_game_time();
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

void configure_debugger() {
  Serial.begin(9650);
}

void start_screen() {
  current_game_state = IN_START_SCREEN;

  bool pressed = false;
  int center_point = 0;

  has_pressed_any_button();

  do {
    center_point = x_position_to_center(title, maxWidth);

    show_text(tv, center_point, maxHeight - 10, "Pique esconde", font4x6);

    pressed = has_pressed_any_button();

    tv.delay_frame(1);
    tv.clear_screen();
  } while (!pressed);

  current_game_state = PLAYING;

  scenery();
}

void game_over_screen() {
  bool pressed = false;
  int center_point = 0;
  int center_point_desc = 0;

  do {
    center_point = x_position_to_center("You lose", maxWidth);
    show_text(tv, center_point, maxHeight - 55, "You lose");

    pressed = has_pressed_any_button();

    tv.delay_frame(1);
    tv.clear_screen();
  } while (!pressed);
}

void win_screen() {
  bool pressed = false;

  int win_center_point = 0;
  int press_center_point = 0;
  int play_center_point = 0;

  String win_text = "You win!";
  String press_text = "Press any key";
  String play_text = "to play again";

  do  {
    win_center_point = x_position_to_center(win_text, maxWidth);
    press_center_point = x_position_to_center(press_text, maxWidth);
    play_center_point = x_position_to_center(play_text, maxWidth);

    show_text(tv, win_center_point, maxHeight - 65, "You win!");

    show_text(tv, press_center_point, maxHeight - 45, "Press any key", font4x6);
    show_text(tv, play_center_point, maxHeight - 35, "to play again", font4x6);

    pressed = has_pressed_any_button();

    tv.delay_frame(1);
    tv.clear_screen();
  } while (!pressed);

  reset_game();
  scenery();
}

void activate_marco_polo() {
  // If is showing the indicator and already used all marco polos, we need to await and continue showing
  // the indicators
  if (marco_polo_used == 0 && !showing_marco_polo)
    return;

  if (showing_marco_polo)
    draw_marco_polo_indicators();

  if (digitalRead(ACTION) == LOW)
    marco_polo_timer++;

  if (digitalRead(ACTION) == HIGH) {
    reset_marco_polo();
    return;
  }

  // Controls for how long the marco polo indicators (?, !) will be displayed
  if (showing_marco_polo && ((marco_polo_timer - TIME_MARCO_POLO) >= MARCO_POLO_SHOWING_TIME)) {
    // Serial.println("Reset marco polo");
    reset_marco_polo();
    return;
  }

  if (!showing_marco_polo && marco_polo_timer >= TIME_MARCO_POLO)
    marco_polo();
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

void show_debug_info() {
  tv.select_font(font4x6);

  // Show the current X value of the player
  tv.print(2, 5, eyeX);
  // Show the current Y value of the player
  tv.print(2, 15, eyeY);

  // Show the number of marco polo used
  tv.print(maxWidth - 10, 5, marco_polo_used);

  // Show the number of marco polo used
  tv.print(maxWidth - 10, maxHeight - 10, enemies_found);
}

void draw_all_enemies() {
  for (auto &positions : possible_positions)
  {
    int enabled = positions[0];

    if (enabled == ENEMY_DISABLED)
      continue;

    int x = positions[1];
    int y = positions[2];
    int id = positions[3];
    
    // In the game the enemies will not be displayed
    // draw_enemy(x, y);
  }
}

// void draw_enemy(int x, int y) {
//   tv.draw_rect(x, y, ENEMY_WIDTH, ENEMY_HEIGHT, WHITE);
// }

bool has_pressed_any_button() {
  int controls_length = (sizeof(ALL_CONTROLS) / sizeof(ALL_CONTROLS[0]));

  Serial.println(controls_length);

  bool pressed = false;

  for (int index = 0; index < controls_length; index++) {
    if (digitalRead(ALL_CONTROLS[index]) == LOW) {
      Serial.println("pressionou!");
      pressed = true;
    }

    Serial.println("não pressionou!");
  }

  Serial.println(pressed);

  return pressed;
}

void verify_if_has_collided_with_enemy() {
  for (auto &positions : possible_positions)
  {
    bool disable = positions[0] == ENEMY_DISABLED;

    if (disable) continue;

    int x = positions[1];
    int y = positions[2];
    int id = positions[3];

    collision_confirm(x, y, id);

    if (colliding) break;
  }

  if (!colliding)
    return;

  if (digitalRead(ACTION) != LOW)
    return;

  // if (enemy_id_collided <= 0)
  //   Serial.println("Id invalid!");

  disable_enemy(enemy_id_collided);
  enemies_found++;

  bool find_all = found_all_enemies();

  if (find_all)
    current_game_state = WIN;
}

void collision_confirm(int enemy_x, int enemy_y, int enemy_id) {
  colliding = (eyeX + EYE_WIDTH >= enemy_x && eyeX <= enemy_x + ENEMY_WIDTH)
              && (eyeY + EYE_HEIGHT >= enemy_y && eyeY <= enemy_y + ENEMY_HEIGHT);

  if (colliding)
  {
    enemy_id_collided = enemy_id;
    return;
  }

  enemy_id_collided = 0;
}

void control_game_time() {
  int seconds = millis() - milliSeconds;

  if (seconds >= GAME_DURATION)
  {
    timer = timer - 1;
    // Serial.println(timer);
    // Serial.println(seconds);
    // Serial.println(milliSeconds);
    milliSeconds = millis();
  }

  // current_game_state = LOST;
}

void scenery() {
  tv.bitmap((tv.hres() - room_image[0]) - 10, (tv.vres() - room_image[1]) / 2, room_image);
  tv.delay_frame(75);
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
    bool disable = positions[0] == ENEMY_DISABLED;

    if (disable) continue;

    int x = positions[1];
    int y = positions[2];

    show_text(tv, x, y - 6, "!", font4x6);
  }
}

void reset_marco_polo() {
  marco_polo_timer = 0;
  showing_marco_polo = false;
}

void disable_enemy(int enemy_id_to_disable) {
  for (auto &positions : possible_positions)
  {
    int id = positions[3];

    if (id == enemy_id_to_disable)
    {
      positions[0] = ENEMY_DISABLED;
      break;
    }
  }
}

bool found_all_enemies() {
  int internal_enemies_found_quantity = 0;
  int quantity_of_enemies = sizeof(possible_positions) / sizeof(possible_positions[0]);

  for (auto &positions : possible_positions)
  {
    if (positions[0] == ENEMY_DISABLED)
      internal_enemies_found_quantity++;
  }

  bool found_all = internal_enemies_found_quantity == quantity_of_enemies;

  return found_all;
}

void reset_game() {
  current_game_state = PLAYING;

  eyeY = maxWidth / 2;
  eyeX = maxHeight / 2;

  reset_marco_polo();
  marco_polo_used = MARCO_POLO_CHANCES;

  timer = 120;
  milliSeconds = 0;

  // Positions
  for (auto &positions : possible_positions)
  {
    positions[0] = ENEMY_ENABLED;
  }

  colliding = false;
  enemy_id_collided = 0;
  enemies_found = 0;
}