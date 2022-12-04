// Classe de utilidade, com abstrações de tarefas comuns
#include <TVout.h>
#include <TVoutfonts/fontALL.h>

// Mostra um terminado texto na tela, em uma determinada posição X e Y com um
// determinado tamanho (por padrão 8x8)
void show_text(TVout& tv, uint8_t x, uint8_t y, char text[], unsigned char type[] = font8x8) {
  tv.select_font(type);
  tv.print(x, y, text);
}