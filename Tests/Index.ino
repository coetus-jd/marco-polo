#include "constants.h"
#include "enums.h"

#include <arduino-tvout/TVout.h>
#include <arduino-tvout/TVoutfonts/fontALL.h>

TVout tv;

void setup() {
    tv.begin(NTSC, 128, 96);
    tv.set_pixel()
    tv.select_font(font8x8);
    if (GameState::Working == 1) {
        printf("ok");
    }
    tv.print(65, 40, "Press any key");
}

void loop() {}