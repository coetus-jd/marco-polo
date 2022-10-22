#include <TVout.h>
#include <TVoutfonts/fontALL.h>

void show_text(TVout tv, uint8_t x, uint8_t y, char text[], unsigned char type[] = font8x8) {
  tv.select_font(type);
  tv.print(x, y, text);
}

void show_text(TVout tv, uint8_t x, uint8_t y, String text, unsigned char type[] = font8x8) {
  tv.select_font(type);

  char temp[text.length()];

  int i;
  for (i = 0; i < sizeof(temp); i++) {
    temp[i] = text[i];
  }

  tv.print(x, y, temp);
}

int text_size(String text, int maxWidth) {
  int text_lenght = text.length();
  int center_point = (maxWidth - text_lenght * 4) / 2;

  return center_point;
}