#include <FastLED.h>

#define LED_PIN    6
#define NUM_LEDS   64
#define MATRIX_W   8
#define MATRIX_H   8

#define INITIAL_LEN   4
#define MAX_LEN       48
#define MOVE_MS       100
#define BLINK_MS      300

CRGB leds[NUM_LEDS];

inline int XY(int x, int y) { return y * MATRIX_W + x; }

inline int8_t wrapX(int v) { return (int8_t)(((v % 8) + 8) & 7); }
inline int8_t wrapY(int v) { return (int8_t)(((v % 8) + 8) & 7); }

int8_t  snakeX[MAX_LEN];
int8_t  snakeY[MAX_LEN];
uint8_t snakeLen;
int8_t  dirX, dirY;

int8_t   foodX, foodY;
bool     foodBlinkOn;
uint32_t foodBlinkMs;

uint32_t lastMoveMs;
uint32_t pauseUntilMs;


bool isSnakeCell(int8_t x, int8_t y, bool skipTail = false) {
  int lim = skipTail ? (int)snakeLen - 1 : (int)snakeLen;
  for (int i = 0; i < lim; i++)
    if (snakeX[i] == x && snakeY[i] == y) return true;
  return false;
}

int wrapDist(int8_t x1, int8_t y1, int8_t x2, int8_t y2) {
  int dx = abs((int)x1 - (int)x2);
  int dy = abs((int)y1 - (int)y2);
  if (dx > 4) dx = 8 - dx;
  if (dy > 4) dy = 8 - dy;
  return dx + dy;
}

void spawnFood() {
  int8_t  fx, fy;
  uint8_t tries = 0;
  do {
    fx = (int8_t)random(MATRIX_W);
    fy = (int8_t)random(MATRIX_H);
    if (++tries > 200) break;
  } while (isSnakeCell(fx, fy));

  foodX        = fx;
  foodY        = fy;
  pauseUntilMs = millis() + (uint32_t)random(1000, 2001);
  foodBlinkOn  = true;
  foodBlinkMs  = millis();
}

bool isTrapped() {
  const int8_t DX[] = { 1, -1,  0,  0 };
  const int8_t DY[] = { 0,  0,  1, -1 };
  int8_t hx = snakeX[0], hy = snakeY[0];
  for (int i = 0; i < 4; i++) {
    int8_t nx = wrapX(hx + DX[i]);
    int8_t ny = wrapY(hy + DY[i]);
    if (!isSnakeCell(nx, ny, true)) return false;
  }
  return true;
}

void chooseDir() {
  const int8_t DX[] = { 1, -1,  0,  0 };
  const int8_t DY[] = { 0,  0,  1, -1 };

  int8_t hx = snakeX[0], hy = snakeY[0];
  int    bestScore = 32000;
  int8_t bestDx = dirX, bestDy = dirY;
  bool   found  = false;

  uint8_t order[4] = { 0, 1, 2, 3 };
  if (random(2)) { order[2] = 3; order[3] = 2; }

  for (int ii = 0; ii < 4; ii++) {
    uint8_t i  = order[ii];
    int8_t  nx = wrapX(hx + DX[i]);
    int8_t  ny = wrapY(hy + DY[i]);

    if (isSnakeCell(nx, ny, true)) continue;

    int score = wrapDist(nx, ny, foodX, foodY);

    if (!found || score < bestScore) {
      bestScore = score;
      bestDx    = DX[i];
      bestDy    = DY[i];
      found     = true;
    }
  }

  dirX = bestDx;
  dirY = bestDy;
}

void initSnake() {
  snakeLen = INITIAL_LEN;
  dirX = 1; dirY = 0;

  snakeX[0] = 4;  snakeY[0] = 3;
  snakeX[1] = 3;  snakeY[1] = 3;
  snakeX[2] = 2;  snakeY[2] = 3;
  snakeX[3] = 1;  snakeY[3] = 3;
}

void spiralAnimation() {
  int8_t  sx[64], sy[64];
  uint8_t idx = 0;
  int top = 0, bot = 7, lft = 0, rgt = 7;

  while (top <= bot && lft <= rgt) {
    for (int x = lft; x <= rgt; x++) { sx[idx] = x; sy[idx] = top; idx++; }
    top++;
    for (int y = top; y <= bot; y++) { sx[idx] = rgt; sy[idx] = y; idx++; }
    rgt--;
    if (top <= bot) {
      for (int x = rgt; x >= lft; x--) { sx[idx] = x; sy[idx] = bot; idx++; }
      bot--;
    }
    if (lft <= rgt) {
      for (int y = bot; y >= top; y--) { sx[idx] = lft; sy[idx] = y; idx++; }
      lft++;
    }
  }

  FastLED.clear();
  FastLED.show();
  delay(150);

  uint8_t hue = 0;
  for (uint8_t i = 0; i < 64; i++) {
    leds[XY(sx[i], sy[i])] = CHSV(hue, 230, 200);
    hue += 4;
    FastLED.show();
    delay(25);
  }

  delay(400);

  for (uint8_t f = 0; f < 25; f++) {
    fadeToBlackBy(leds, NUM_LEDS, 20);
    FastLED.show();
    delay(25);
  }

  FastLED.clear();
  FastLED.show();
  delay(200);
}

void drawSnake() {
  uint8_t denom = (snakeLen > 1) ? (snakeLen - 1) : 1;

  for (int i = 0; i < snakeLen; i++) {
    CRGB c;
    if (i == 0) {
      c = CRGB(0, 255, 50);
    } else {
      uint8_t hue = 96  + (uint8_t)((uint16_t)i * 96  / denom);
      uint8_t bri = 230 - (uint8_t)((uint16_t)i * 155 / denom);
      c = CHSV(hue, 255, bri);
    }
    leds[XY(snakeX[i], snakeY[i])] = c;
  }
}

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(55);
  randomSeed(analogRead(A0));

  initSnake();
  spawnFood();
  lastMoveMs   = millis();
  pauseUntilMs = millis();
}

void loop() {
  uint32_t now = millis();

  if (now - foodBlinkMs >= BLINK_MS) {
    foodBlinkOn = !foodBlinkOn;
    foodBlinkMs = now;
  }

  if (now >= pauseUntilMs && now - lastMoveMs >= MOVE_MS) {
    lastMoveMs = now;

    if (isTrapped()) {
      spiralAnimation();
      initSnake();
      spawnFood();
      lastMoveMs = millis();
      return;
    }

    chooseDir();

    int8_t nhx = wrapX(snakeX[0] + dirX);
    int8_t nhy = wrapY(snakeY[0] + dirY);

    bool eating = (nhx == foodX && nhy == foodY);

    if (eating && snakeLen < MAX_LEN) snakeLen++;
    for (int i = snakeLen - 1; i > 0; i--) {
      snakeX[i] = snakeX[i - 1];
      snakeY[i] = snakeY[i - 1];
    }
    snakeX[0] = nhx;
    snakeY[0] = nhy;

    if (eating) {
      spawnFood();
    }

    if (snakeLen >= MAX_LEN) {
      spiralAnimation();
      initSnake();
      spawnFood();
      lastMoveMs = millis();
      return;
    }
  }

  FastLED.clear();
  drawSnake();
  if (foodBlinkOn) {
    leds[XY(foodX, foodY)] = CRGB(255, 0, 0);
  }
  FastLED.show();

  delay(16);
}
