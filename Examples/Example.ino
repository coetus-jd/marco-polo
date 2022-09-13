// Alexandre Souza

#include "../Library/arduino-tvout/TVout.h"
#include "../Library/arduino-tvout/TVoutfonts/fontALL.h"

#define direita 3
#define esquerda 4

#define disparo 5

// CONSTANTES ARMAZENA AS FREQUÊNCIAS DAS NOTAS
const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784;
const int gSH = 830;
const int aH = 880;

int largura = 120;
int altura = 96;

int lMaxima, aMaxima;

int pontos = 1;

bool inicioJogo;

TVout tv;
TVout tv2;

class Posicao {
public:
  int posX;
  int posY;
};

class Nave : public Posicao {
public:
  void begin(int x, int y) {
    posX = x;
    posY = y;
  }
  void desenhaNave() {
    tv.draw_rect(posX, posY, 10, 10, WHITE, WHITE);
    tv.draw_rect(posX + 3, posY - 5, 4, 4, WHITE, WHITE);
    tv.draw_rect(posX - 5, posY + 5, 20, 3, WHITE, WHITE);
  }
  void somNave() {
    tv.tone(100, 200);
    tv.tone(50, 200);
    tv.tone(100, 200);
  }
  void moverNaveDireita() {
    // GARANTE QUE A NAVE NÃO SAIA DO CAMPO E A MOVIMENTA PARA A DIREITA
    if (posX < lMaxima - 16) {
      posX += 2;
      somNave();
    }
  }
  // GARANTE QUE A NAVE NÃO SAIA DO CAMPO E A MOVIMENTA PARA A ESQUERDA
  void moverNaveEsquerda() {
    if (posX > 5) {
      posX -= 2;
      somNave();
    }
  }
};
Nave nave;

class Missil : public Posicao {
public:
  boolean estado;
  void desenhaMissil() {
    // VERIFICA SE O MISSIL ESTA ATIVO E SE ELE STA DENTRO DO CAMPO
    if (this->estado && this->posY > 3) {
      // EMITE UM SOM SOMENTE NO COMESSO DO DISPARO
      if (this->posY == nave.posY) {
        tv.tone(100, 50);
        delay(20);
        tv.tone(200, 50);
        delay(20);
        tv.tone(300, 50);
        delay(20);
        tv.tone(400, 50);
      }
      this->posY -= 3;
      tv.draw_rect(posX, posY, 2, 4, WHITE, WHITE);
    } else {
      this->estado = 0;
      this->posX = nave.posX + 4;
      this->posY = nave.posY;
    }
  }
};
Missil missil;

class Asteroid : public Posicao {
public:
  boolean estado;
  int posicaoInicial;
  void desenhaAsteroid() {
    if (this->estado && this->posY < aMaxima) {
      tv.draw_rect(posX, posY, 8, 14, WHITE, WHITE);
      tv.draw_rect(posX - 2, posY + 3, 12, 8, WHITE, WHITE);
      this->posY += 1;
    } else {
      this->posicaoInicial = random(6, lMaxima - 13);
      this->posX = posicaoInicial;
      this->posY = 2;
      this->estado = 1;
    }

    // VERIFICA SE O MISSIL ATINGIU O ASTEROID
    if (missil.posY < this->posY + 3 && missil.posY > this->posY - 3 &&
        missil.posX > this->posX - 7 && missil.posX < this->posX + 10 &&
        missil.estado == 1) {

      tv.tone(50, 300);
      this->estado = 0;
      missil.estado = 0;
      pontos += 1;
      if (pontos == 50) {
        tv.tone(100, 300);
        delay(300);
        tv.tone(500, 200);
        delay(200);
        tv.tone(100, 200);
        delay(200);
        tv.tone(500, 200);
        delay(200);
        tv.tone(100, 70);
        delay(70);
        tv.tone(500, 70);
        pontos = 1;
      }
    }
  }
};
Asteroid asteroid;

// DESENHA BORDA - LIMITE DO CAMPO
void desenhaCampo() {
  tv.draw_rect(0, 0, lMaxima, aMaxima, WHITE);
  tv.draw_rect(200, 50 - pontos, 2, pontos, WHITE);
}

// COMEÇO DAS FUNÇÕES RESPONSÁVEIS PELA MÚSICA TEMA
void firstSection() {
  beep(a, 500);
  beep(a, 500);
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);

  delay(500);

  beep(eH, 500);
  beep(eH, 500);
  beep(eH, 500);
  beep(fH, 350);
  beep(cH, 150);
  beep(gS, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);

  delay(500);
}

void secondSection() {
  beep(aH, 500);
  beep(a, 300);
  beep(a, 150);
  beep(aH, 500);
  beep(gSH, 325);
  beep(gH, 175);
  beep(fSH, 125);
  beep(fH, 125);
  beep(fSH, 250);

  delay(325);

  beep(aS, 250);
  beep(dSH, 500);
  beep(dH, 325);
  beep(cSH, 175);
  beep(cH, 125);
  beep(b, 125);
  beep(cH, 250);

  delay(350);
}
// REPRODUZ AS NOTAS
void beep(int note, int duration) {
  tv.tone(note, duration);
  delay(duration);
}
// FIM DAS FUNÇÕES RESPONSÁVEIS PELA MÚSICA TEMA

void setup() {
  pinMode(direita, INPUT_PULLUP);
  pinMode(esquerda, INPUT_PULLUP);
  pinMode(disparo, INPUT_PULLUP);

  tv.begin(NTSC, 120, 96);
  lMaxima = tv.hres() - 10;
  aMaxima = tv.vres() - 3;

  // POSICIONA A NAVE NO CENTRO INFERIOR DO CAMPO
  nave.begin(95, 205);

  // DEFINE ONDE O ASTEROID IRÁ APARECER
  randomSeed(analogRead(0));
  inicioJogo = 1;
}

void loop() {
  tv.clear_screen();
  desenhaCampo();
  nave.desenhaNave();
  missil.desenhaMissil();
  if (inicioJogo) {
    // TOCA MÚSICA TEMA
    // Play first section
    firstSection();

    // Play second section
    secondSection();

    // Variant 1
    beep(f, 250);
    beep(gS, 500);
    beep(f, 350);
    beep(a, 125);
    beep(cH, 500);
    beep(a, 375);
    beep(cH, 125);
    beep(eH, 650);

    delay(500);

    // Repeat second section
    secondSection();

    // Variant 2
    beep(f, 250);
    beep(gS, 500);
    beep(f, 375);
    beep(cH, 125);
    beep(a, 500);
    beep(f, 375);
    beep(cH, 125);
    beep(a, 650);

    delay(650);
    inicioJogo = 0;
  }

  asteroid.desenhaAsteroid();
  if (digitalRead(direita) == LOW)
    nave.moverNaveDireita();
  if (digitalRead(esquerda) == LOW)
    nave.moverNaveEsquerda();
  if (digitalRead(disparo) == LOW)
    missil.estado = 1;
  delay(25);
}
