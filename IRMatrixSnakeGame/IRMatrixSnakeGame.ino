#include "IRremote.h"
#include "LedControl.h"

int receiver = 3; // Signal pin of IR receiver
IRrecv irrecv(receiver);

LedControl lc = LedControl(12, 10, 11, 1);

enum Direction { UP, DOWN, LEFT, RIGHT };
Direction snakeDir = RIGHT;

const int8_t SIZE = 8;
const uint8_t MAX_LEN = SIZE * SIZE;  // 64

int8_t segX[MAX_LEN];  // segment 0 is the head
int8_t segY[MAX_LEN];
uint8_t snakeLen = 3;

int8_t foodX = 0;
int8_t foodY = 0;
bool foodOn = true;                    // food blinks so it stands out from the snake
unsigned long lastBlink = 0;
const unsigned long blinkInterval = 150;

unsigned long lastMove = 0;
const unsigned long moveInterval = 250;  // ms per step; lower = faster

// Logical coordinates: x goes right, y goes down.
// All hardware rotation is handled here and nowhere else.
void setPixel(int8_t x, int8_t y, bool on) {
  lc.setLed(0, x, SIZE - 1 - y, on);
}

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  Serial.begin(9600);
  irrecv.enableIRIn();

  randomSeed(analogRead(A0));  // floating pin gives a different seed each power-up
  resetGame();
}

void loop() {
  readIR();

  if (millis() - lastMove >= moveInterval) {
    lastMove = millis();
    stepSnake();
  }

  // Blink the food
  if (millis() - lastBlink >= blinkInterval) {
    lastBlink = millis();
    foodOn = !foodOn;
    setPixel(foodX, foodY, foodOn);
  }
}

void resetGame() {
  lc.clearDisplay(0);
  snakeDir = RIGHT;
  snakeLen = 3;

  // Head at (4,3), body trailing to the left
  for (uint8_t i = 0; i < snakeLen; i++) {
    segX[i] = 4 - i;
    segY[i] = 3;
    setPixel(segX[i], segY[i], true);
  }

  spawnFood();
  lastMove = millis();
}

bool isSnake(int8_t x, int8_t y) {
  for (uint8_t i = 0; i < snakeLen; i++) {
    if (segX[i] == x && segY[i] == y) return true;
  }
  return false;
}

// Picks a random cell that the snake is not on
void spawnFood() {
  uint8_t emptyCells = MAX_LEN - snakeLen;
  uint8_t target = random(emptyCells);  // which empty cell to use

  for (int8_t y = 0; y < SIZE; y++) {
    for (int8_t x = 0; x < SIZE; x++) {
      if (isSnake(x, y)) continue;
      if (target == 0) {
        foodX = x;
        foodY = y;
        foodOn = true;
        setPixel(foodX, foodY, true);
        return;
      }
      target--;
    }
  }
}

void stepSnake() {
  // Work out where the head is going
  int8_t newX = segX[0];
  int8_t newY = segY[0];
  switch (snakeDir) {
    case UP:    newY--; break;
    case DOWN:  newY++; break;
    case LEFT:  newX--; break;
    case RIGHT: newX++; break;
  }

  // Wrap around the edges
  if (newX < 0) newX = SIZE - 1;
  if (newX >= SIZE) newX = 0;
  if (newY < 0) newY = SIZE - 1;
  if (newY >= SIZE) newY = 0;

  bool eating = (newX == foodX && newY == foodY);

  // Collision check. If not eating, the tail moves away this step,
  // so the head is allowed to move into the tail's current cell.
  uint8_t checkCount = eating ? snakeLen : snakeLen - 1;
  for (uint8_t i = 0; i < checkCount; i++) {
    if (segX[i] == newX && segY[i] == newY) {
      gameOver();
      return;
    }
  }

  if (eating) {
    snakeLen++;  // the old tail position is kept as the new last segment
  } else {
    setPixel(segX[snakeLen - 1], segY[snakeLen - 1], false);  // erase tail
  }

  // Each segment moves into the position of the one in front of it
  for (uint8_t i = snakeLen - 1; i > 0; i--) {
    segX[i] = segX[i - 1];
    segY[i] = segY[i - 1];
  }

  segX[0] = newX;
  segY[0] = newY;
  setPixel(newX, newY, true);

  if (eating) {
    Serial.print("Length: ");
    Serial.println(snakeLen);

    if (snakeLen >= MAX_LEN) {
      gameWon();
    } else {
      spawnFood();
    }
  }
}

// Big X, flashed 3 times
void gameOver() {
  Serial.println("Game over");

  for (uint8_t flash = 0; flash < 3; flash++) {
    lc.clearDisplay(0);
    delay(300);
    for (int8_t i = 0; i < SIZE; i++) {
      setPixel(i, i, true);
      setPixel(i, SIZE - 1 - i, true);
    }
    delay(300);
  }

  lc.clearDisplay(0);
  delay(500);
  resetGame();
}

// Whole screen filled, flashed 3 times
void gameWon() {
  Serial.println("You win!");

  for (uint8_t flash = 0; flash < 3; flash++) {
    lc.clearDisplay(0);
    delay(300);
    for (int8_t y = 0; y < SIZE; y++) {
      for (int8_t x = 0; x < SIZE; x++) {
        setPixel(x, y, true);
      }
    }
    delay(300);
  }

  delay(500);
  resetGame();
}

void readIR() {
  if (!irrecv.decode()) return;

  // Ignore repeat codes (button held down)
  if (!irrecv.decodedIRData.flags) {
    bool changed = false;
    switch (irrecv.decodedIRData.decodedRawData) {
      case 0xE718FF00: changed = setDirection(UP);    break;  // 2
      case 0xAD52FF00: changed = setDirection(DOWN);  break;  // 8
      case 0xF708FF00: changed = setDirection(LEFT);  break;  // 4
      case 0xA55AFF00: changed = setDirection(RIGHT); break;  // 6
    }
    if (changed) {
      stepSnake();          // respond immediately
      lastMove = millis();  // restart the timer
    }
  }
  irrecv.resume();
}

const char* dirName(Direction d) {
  switch (d) {
    case UP:    return "UP";
    case DOWN:  return "DOWN";
    case LEFT:  return "LEFT";
    case RIGHT: return "RIGHT";
  }
  return "?";
}

bool setDirection(Direction newDir) {
  // Block 180-degree reversals (snake can't turn back into itself)
  if ((snakeDir == UP    && newDir == DOWN)  ||
      (snakeDir == DOWN  && newDir == UP)    ||
      (snakeDir == LEFT  && newDir == RIGHT) ||
      (snakeDir == RIGHT && newDir == LEFT)) {
    return false;
  }
  if (newDir == snakeDir) return false;  // no change

  snakeDir = newDir;
  Serial.print("Direction: ");
  Serial.println(dirName(snakeDir));
  return true;
}