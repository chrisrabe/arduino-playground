#include <Keypad.h>
#include <LiquidCrystal.h>

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
};

byte rowPins[ROWS] = {6,5,4,3};
byte colPins[COLS] = {A0,A1,A2,A3};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
LiquidCrystal lcd(7,8,9,10,11,12);

const byte MAX_LEN = 16;       // LCD is 16 characters wide
char entry[MAX_LEN + 1] = "";  // +1 for the null terminator
byte entryLen = 0;

void showEntry() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Entered:");
  lcd.setCursor(0, 1);
  lcd.print(entry);
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  showEntry();
}

void loop() {
  char key = customKeypad.getKey();

  if (key) {
    Serial.println(key);

    if (key == '*') {
      // backspace
      if (entryLen > 0) {
        entry[--entryLen] = '\0';
      }
    }
    else if (key == '#') {
      // clear everything
      entryLen = 0;
      entry[0] = '\0';
    }
    else {
      // any other key: 0-9, A-D
      if (entryLen < MAX_LEN) {
        entry[entryLen++] = key;
        entry[entryLen] = '\0';
      }
    }

    showEntry();
  }
}