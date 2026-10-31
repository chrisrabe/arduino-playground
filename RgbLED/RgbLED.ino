#define RED 3
#define GREEN 5
#define BLUE 6

const int colors[][3] = {
  {255, 0, 0},     // red
  {0, 255, 0},     // green
  {0, 0, 255},     // blue
  {255, 120, 0},   // orange
  {128, 0, 255},   // purple
  {0, 255, 255}    // cyan
};
const int numColors = sizeof(colors) / sizeof(colors[0]);
int currentColor = 0;

void setColor(int r, int g, int b) {
  analogWrite(RED, r);
  analogWrite(GREEN, g);
  analogWrite(BLUE, b);
}

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  setColor(200, 0, 0);
}


void loop() {
  setColor(colors[currentColor][0],
           colors[currentColor][1],
           colors[currentColor][2]);

  currentColor = (currentColor + 1) % numColors; 
  delay(500); 

}
