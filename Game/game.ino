#include <TVout.h>
#include <TVoutfonts/fontALL.h>
#include "constants.h"
#include "utils.h"
#include "logo.h"
#include "room_image.h"
#include "dark_scenary.h"
#include "marco_icon.h"
#include "polo_icon.h"

TVout tv;

// Game
int current_game_state = 0;
char game_name[] = "Neutron games";
bool show_sinalization_found = false;
int sinalization_found_time = 0;
bool force_loop_run = false;

int eyeX = 0;
int eyeY = 0;
int maxHeight = 0;
int maxWidth = 0;

// Marco polo
int marco_polo_timer = 0;
int marco_polo_used = MARCO_POLO_CHANCES;
bool showing_marco_polo = false;

// Timing
int timer = GAME_DURATION;
int one_second = 1000;
int milli_seconds = 0;

// Positions
int possible_positions[5][4] = {
  // Is enabled?    X  Y  Id
  { ENEMY_DISABLED, 0, 0, 0 },
  { ENEMY_DISABLED, 0, 0, 0 },
  { ENEMY_DISABLED, 0, 0, 0 },
  { ENEMY_DISABLED, 0, 0, 0 },
  { ENEMY_DISABLED, 0, 0, 0 },
};

int ALL_CONTROLS[5] = {
  RIGHT_MOVEMENT,
  LEFT_MOVEMENT,
  UP_MOVEMENT,
  DOWN_MOVEMENT,
  ACTION
};

// Collision controls
bool colliding = false;
int enemy_id_collided = 0;
int enemies_found = 0;

void setup() {
  randomSeed(analogRead(A0));

  configure_buttons();
  configure_libraries();
  configure_debugger();
  configure_screen();

  // Starts the eye in the center
  eyeX = (maxWidth / 2) - 5;
  eyeY = maxHeight - 20;

  generate_enemies_positions();

  start_screen();
}

// OBS: don't use Serial.print inside a loop, for some reason its breaks
void loop() {
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

  // if (!has_pressed_any_button() && force_loop_run)
  //   return;

  tv.delay_frame(1);
  tv.clear_screen();

  draw_game_delimiters();

  draw_all_enemies();
  draw_eyes();
  
  show_debug_info();

  verify_if_has_collided_with_enemy();

  control_game_time();
  activate_marco_polo();

  if (show_sinalization_found) {
    show_sinalization();
  }

  force_loop_run = true;
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

void draw_game_delimiters() {
  // tv.bitmap((tv.hres() - dark_scenary[0]) - 10, (tv.vres() - dark_scenary[1]) / 2, dark_scenary);
  tv.draw_rect(0, 0, maxWidth, maxHeight, WHITE);

  tv.draw_rect(RIGHT_WALL_WIDTH, UP_WALL_HEIGHT, maxWidth - 12, maxHeight - 24, WHITE);

  // Line in the upper left
  tv.draw_line(1, 1, RIGHT_WALL_WIDTH, UP_WALL_HEIGHT, WHITE);
  // Line in the lower left
  tv.draw_line(1, maxHeight - 1, LEFT_WALL_WIDTH - 1, maxHeight - DOWN_WALL_HEIGHT - 2, WHITE);
  

  // Line in the upper right
  tv.draw_line(maxWidth, 0, maxWidth - RIGHT_WALL_WIDTH, UP_WALL_HEIGHT, WHITE);
  // Line in the lower right
  tv.draw_line(maxWidth - 1, maxHeight - 1, maxWidth - RIGHT_WALL_WIDTH, maxHeight - DOWN_WALL_HEIGHT - 2, WHITE);


  // Door
  tv.draw_line((maxWidth / 2) - 10, maxHeight - 1, (maxWidth / 2) + 10, maxHeight - 1, BLACK);
  tv.draw_line((maxWidth / 2) - 10, maxHeight - 1, (maxWidth / 2) - 8, maxHeight - DOWN_WALL_HEIGHT - 1, WHITE);
  tv.draw_line((maxWidth / 2) + 10, maxHeight - 1, (maxWidth / 2) + 8, maxHeight - DOWN_WALL_HEIGHT - 1, WHITE);
  // tv.draw_line((maxWidth / 2) - 10, maxHeight - 1, (maxWidth / 2) + 10, maxHeight - 1, BLACK);
}

void start_screen() {
  current_game_state = IN_START_SCREEN;

  bool pressed = false;
  int center_point = 0;

  tv.draw_rect(0, 0, maxWidth, maxHeight, WHITE);

  center_point = x_position_to_center(game_name, maxWidth);

  show_text(tv, 30, 30, game_name, font4x6);
  tv.bitmap(8, 10, logo);
  show_text(tv, 10, maxHeight - 20, "By Astha", font4x6);
  show_text(tv, maxWidth - 40, maxHeight - 20, "FATEC AM", font4x6);

  do {
    pressed = has_pressed_any_button();
  } while (!pressed);

  tv.delay_frame(1);
  tv.clear_screen();

  current_game_state = PLAYING;

  scenery();
}

void game_over_screen() {
  bool pressed = false;
  char you_lose[] = "You lost";

  draw_game_delimiters();

  show_text(tv, 20, 10, you_lose);
  show_text(tv, 40, 35, ":)");
  show_text(tv, 25, maxHeight - 35, "Press any key", font4x6);
  show_text(tv, 25, maxHeight - 25, "to play again", font4x6);

  delay(1000);

  do {
    pressed = has_pressed_any_button();
  } while (!pressed);

  tv.delay_frame(1);
  tv.clear_screen();

  reset_game();
  scenery();
}

void win_screen() {
  bool pressed = false;

  char win_text[] = "You win!";
  char press_text[] = "Press any key";
  char play_text[] = "to play again";

  draw_game_delimiters();

  show_text(tv, 20, 10, win_text);
  show_text(tv, 40, 35, ":(");
  show_text(tv, 25, maxHeight - 35, "Press any key", font4x6);
  show_text(tv, 25, maxHeight - 25, "to play again", font4x6);

  delay(1000);

  do {
    pressed = has_pressed_any_button();
  } while (!pressed);

  tv.delay_frame(1);
  tv.clear_screen();

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
  tv.draw_rect(eyeX, eyeY, 3, 4, WHITE);
  // Right
  tv.draw_rect(eyeX + 5, eyeY, 3, 4, WHITE);

  if (eyeX < (maxWidth - EYE_WIDTH - 8) && digitalRead(RIGHT_MOVEMENT) == LOW) {
    // Left iris
    tv.draw_rect(eyeX + 1, eyeY + 1, 1, 2, WHITE);
    tv.draw_rect(eyeX + 2, eyeY + 1, 1, 2, BLACK);

    // Right iris
    tv.draw_rect(eyeX + 6, eyeY + 1, 1, 2, WHITE);
    tv.draw_rect(eyeX + 7, eyeY + 1, 1, 2, BLACK);
    eyeX++;
  }

  if (eyeX > 8 && digitalRead(LEFT_MOVEMENT) == LOW) {
    // Left iris
    tv.draw_rect(eyeX + 1, eyeY + 1, 1, 2, WHITE);
    tv.draw_rect(eyeX, eyeY + 1, 1, 2, BLACK);

    // Right iris
    tv.draw_rect(eyeX + 6, eyeY + 1, 1, 2, WHITE);
    tv.draw_rect(eyeX + 5, eyeY + 1, 1, 2, BLACK);
    eyeX--;
  }

  if (eyeY > 18 && digitalRead(UP_MOVEMENT) == LOW) {
    // Left iris
    tv.draw_rect(eyeX + 1, eyeY + 1, 1, 2, WHITE);
    tv.draw_rect(eyeX + 1, eyeY, 1, 2, BLACK);

    // Right iris
    tv.draw_rect(eyeX + 6, eyeY + 1, 1, 2, WHITE);
    tv.draw_rect(eyeX + 6, eyeY, 1, 2, BLACK);
    eyeY--;
  }

  if (eyeY < (maxHeight - EYE_HEIGHT - 10) && digitalRead(DOWN_MOVEMENT) == LOW) {
    // Left iris
    tv.draw_rect(eyeX + 1, eyeY + 1, 1, 2, WHITE);
    tv.draw_rect(eyeX + 1, eyeY + 2, 1, 2, BLACK);

    // Right iris
    tv.draw_rect(eyeX + 6, eyeY + 1, 1, 2, WHITE);
    tv.draw_rect(eyeX + 6, eyeY + 2, 1, 2, BLACK);
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
  tv.print(maxWidth - 12, 5, marco_polo_used);

  // Show the number of marco polo used
  tv.print(maxWidth - 12, maxHeight - 15, enemies_found);
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

    // In the real game the enemies will not be displayed
    // draw_enemy(x, y);
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

void verify_if_has_collided_with_enemy() {
  if (digitalRead(ACTION) != LOW)
    return;

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

  // if (enemy_id_collided <= 0)
  //   Serial.println("Id invalid!");

  show_sinalization_found = true;

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
  int seconds = millis() - milli_seconds;
  tv.print(50, 5, timer);

  if (seconds >= one_second)
  {
    timer -= 1;
    milli_seconds = millis();
  }

  if (timer == 0)
  {
    current_game_state = LOST;
  }
}

void scenery() {
  tv.bitmap((tv.hres() - room_image[0]) - 10, (tv.vres() - room_image[1]) / 2, room_image);
  tv.delay_frame(125);
  tv.clear_screen();
}

void marco_polo() {
  marco_polo_used--;
  showing_marco_polo = true;
  draw_marco_polo_indicators();
}

void draw_marco_polo_indicators() {
  // show_text(tv, eyeX + 3, eyeY - 6, "?", font4x6);
  tv.bitmap(eyeX - 4, eyeY - 15, marco_icon);

  for (auto &positions : possible_positions)
  {
    bool disable = positions[0] == ENEMY_DISABLED;

    if (disable) continue;

    int x = positions[1];
    int y = positions[2];

    // show_text(tv, x, y - 2, "!", font4x6);
    tv.bitmap(x - 5, y, polo_icon);
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

  timer = GAME_DURATION;
  milli_seconds = 0;

  generate_enemies_positions();

  colliding = false;
  enemy_id_collided = 0;
  enemies_found = 0;

  show_sinalization_found = false;
  sinalization_found_time = 0;
}

int get_random_x_coordinate() {
  // randomSeed(analogRead(A0));
  return random(20, maxWidth - 20);
}

int get_random_y_coordinate() {
  // randomSeed(analogRead(A0));
  return random(20, maxHeight - 20);
}

void generate_enemies_positions() {
  int previous_id = 0;

  for (auto &positions : possible_positions)
  {
    previous_id++;

    positions[0] = ENEMY_ENABLED;
    positions[1] = get_random_x_coordinate();
    positions[2] = get_random_y_coordinate();
    positions[3] = previous_id;
  }
}

void show_sinalization() {
  sinalization_found_time++;

  if (sinalization_found_time >= DURATION_TO_SHOW_FOUND_SINALIZATION) {
    show_sinalization_found = false;
    sinalization_found_time = 0;
    return;
  }

  show_text(tv, eyeX + 2, eyeY - 10, ":)", font4x6);
}