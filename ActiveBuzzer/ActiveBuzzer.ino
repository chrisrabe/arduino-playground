const int buzzer = 12;

// ---------- helpers ----------
void beep(int onMs, int offMs) {
  digitalWrite(buzzer, HIGH);
  delay(onMs);
  digitalWrite(buzzer, LOW);
  delay(offMs);
}

// ---------- Morse code ----------
const char* MORSE[26] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
  "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
  "..-", "...-", ".--", "-..-", "-.--", "--.."
};

void playMorse(const char* text, int unit = 100) {
  for (const char* p = text; *p; p++) {
    char c = *p;
    if (c == ' ') { delay(unit * 7); continue; }   // word gap
    if (c >= 'a' && c <= 'z') c -= 32;
    if (c < 'A' || c > 'Z') continue;
    for (const char* s = MORSE[c - 'A']; *s; s++) {
      beep(*s == '.' ? unit : unit * 3, unit);     // dot / dash, then gap
    }
    delay(unit * 2);                               // letter gap (total 3 units)
  }
}

void message() {
  playMorse("SOS");
}

void setup() {
  pinMode(buzzer, OUTPUT);
}

void loop() {
  message();       delay(1500);
}