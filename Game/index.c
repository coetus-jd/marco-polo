#include <stdio.h>
#include <stdlib.h>
#include "../Library/arduino-tvout/TVout.h"

enum State { Working = 1, Failed = 0 };

struct Image
{
  char Source[10];
} Image;


int main() {
  char test[10] = { 'A' };

  enum State currentStatus = Working;
  struct Image myImage;

  // myImage = (Image)malloc(sizeof(Image));

  printf("Current state: %d", myImage.Source[0]);
  return 0;
}