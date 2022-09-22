#include <TVout.h>
#include <TVoutfonts/fontALL.h>

void show_text(TVout tv, uint8_t x, uint8_t y, char text[], unsigned char type[] = font8x8) {
  tv.select_font(type);
  tv.print(x, y, text);
}