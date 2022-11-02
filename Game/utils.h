#include <TVout.h>
#include <TVoutfonts/fontALL.h>

void show_text(TVout& tv, uint8_t x, uint8_t y, char text[], unsigned char type[] = font8x8) {
  tv.select_font(type);
  tv.print(x, y, text);
}

void show_text(TVout& tv, uint8_t x, uint8_t y, String text, unsigned char type[] = font8x8) {
  tv.select_font(type);

  char temp[text.length()];

  int i;
  for (i = 0; i < sizeof(temp); i++) {
    temp[i] = text[i];
  }

  tv.print(x, y, temp);
}

int x_position_to_center(String text, int maxWidth) {
  int text_lenght = text.length();
  int center_point = ((maxWidth / 2) - text_lenght);

  return (center_point - (center_point / 2));
}