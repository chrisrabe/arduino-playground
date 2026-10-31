#define LED_PIN 5
#define BUTTON_A_PIN 9
#define BUTTON_B_PIN 8

bool isActive = false;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 30; // ms

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);
}

void loop() {
  bool reading = digitalRead(BUTTON_A_PIN);

  if(reading != lastButtonState) {
    unsigned long now = millis();
    if(now - lastDebounceTime > debounceDelay) {
      lastDebounceTime = now;
      if(reading == LOW) {
        isActive = !isActive;
        digitalWrite(LED_PIN, isActive ? HIGH : LOW);
      }
    }
    lastButtonState = reading;
  }
}
