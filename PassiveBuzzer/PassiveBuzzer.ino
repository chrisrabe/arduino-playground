#include "pitches.h"

const int PIN = 8;
const int BPM = 126;
const int S = (60000 / BPM) / 4;   // one sixteenth note in ms (~119 ms)
const int R = 0;                   // rest

// Play a note (or rest if freq is 0) for a number of sixteenths
void note(int freq, int sixteenths) {
  int ms = sixteenths * S;
  if (freq > 0) tone(PIN, freq, ms * 0.85);
  delay(ms);
  noTone(PIN);
}

// Fake "kick drum": a quick downward pitch sweep, then wait out the beat
void kick() {
  for (int f = 160; f > 50; f -= 22) {
    tone(PIN, f);
    delay(8);
  }
  noTone(PIN);
  delay(4 * S - 40);
}

// Folk-style arpeggio: plays a-b-c-b for one bar (16 sixteenths)
void arp(int a, int b, int c) {
  int p[4] = {a, b, c, b};
  for (int i = 0; i < 16; i++) note(p[i % 4], 1);
}

void intro() {
  arp(NOTE_E4, NOTE_G4, NOTE_B4);    // Em
  arp(NOTE_C4, NOTE_E4, NOTE_G4);    // C
  arp(NOTE_G3, NOTE_B3, NOTE_D4);    // G
  arp(NOTE_D4, NOTE_FS4, NOTE_A4);   // D
}

void buildUp() {
  // 4 bars of kick, like the pulse that starts under the build
  for (int i = 0; i < 8; i++) kick();

  // one bar of rising sixteenths
  int rise[16] = {
    NOTE_E4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5,
    NOTE_FS5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_E6, NOTE_FS6
  };
  for (int i = 0; i < 16; i++) note(rise[i], 1);

  note(R, 4);   // silence for one beat before the drop
}

void hook() {
  // Em
  note(NOTE_B5,3); note(NOTE_B5,3); note(NOTE_A5,2);
  note(NOTE_G5,3); note(NOTE_E5,3); note(R,2);
  // C
  note(NOTE_C6,3); note(NOTE_C6,3); note(NOTE_B5,2);
  note(NOTE_A5,4); note(NOTE_G5,4);
  // G
  note(NOTE_D6,3); note(NOTE_D6,3); note(NOTE_B5,2);
  note(NOTE_G5,3); note(NOTE_B5,3); note(R,2);
  // D
  note(NOTE_A5,3); note(NOTE_A5,3); note(NOTE_FS5,2);
  note(NOTE_D5,4); note(NOTE_E5,2); note(NOTE_FS5,2);
}

void finish() {
  note(NOTE_G5, 12);
  note(R, 4);
}

void setup() {}

void loop() {
  intro();
  buildUp();
  hook();
  hook();
  finish();
  delay(2000);
}