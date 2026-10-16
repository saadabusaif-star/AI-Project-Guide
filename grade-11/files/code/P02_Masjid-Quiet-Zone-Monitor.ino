/* PROJECT 2 - MASJID QUIET ZONE MONITOR  (11.C Group 4)
   Microphone loudness -> score 0-9 on the 7-segment + traffic-light LEDs.
   Only noise that stays RED for 5 seconds gives one gentle reminder tone. */
const int MIC = A0;                     // sound module AO (analog) pin
const int GRN = 9, YEL = 10, RED = 11;  // LEDs (each through 220 ohm)
const int BUZZ = 12;                    // PASSIVE buzzer

// ---- CALIBRATE: read the Serial Monitor in a silent room and while shouting ----
const int QUIET_PP = 5;                 // peak-to-peak in a silent room
const int LOUD_PP  = 400;               // peak-to-peak when very loud

unsigned long loudSince = 0;

// ---------- 7-SEGMENT DISPLAY (pins a,b,c,d,e,f,g) ----------
const int SEG[7] = {2, 3, 4, 5, 6, 7, 8};
const bool SEG_COMMON_ANODE = false;   // true = common pin goes to 5V instead of GND
const byte DIGIT[10][7] = {
  {1,1,1,1,1,1,0}, {0,1,1,0,0,0,0}, {1,1,0,1,1,0,1}, {1,1,1,1,0,0,1},
  {0,1,1,0,0,1,1}, {1,0,1,1,0,1,1}, {1,0,1,1,1,1,1}, {1,1,1,0,0,0,0},
  {1,1,1,1,1,1,1}, {1,1,1,1,0,1,1} };

void show(int n) {                      // show one number 0-9
  n = constrain(n, 0, 9);
  for (int i = 0; i < 7; i++) {
    bool on = DIGIT[n][i];
    digitalWrite(SEG[i], SEG_COMMON_ANODE ? !on : on);
  }
}

int loudness() {                        // peak-to-peak over 50 ms
  unsigned long start = millis();
  int hi = 0, lo = 1023;
  while (millis() - start < 50) {
    int v = analogRead(MIC);
    if (v > hi) hi = v;
    if (v < lo) lo = v;
  }
  return hi - lo;
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) pinMode(SEG[i], OUTPUT);
  pinMode(GRN, OUTPUT); pinMode(YEL, OUTPUT); pinMode(RED, OUTPUT);
  pinMode(BUZZ, OUTPUT);
}

void loop() {
  int pp = loudness();
  int score = constrain(map(pp, QUIET_PP, LOUD_PP, 0, 9), 0, 9);
  show(score);

  digitalWrite(GRN, score <= 3);
  digitalWrite(YEL, score >= 4 && score <= 6);
  digitalWrite(RED, score >= 7);

  if (score >= 7) {
    if (loudSince == 0) loudSince = millis();            // start the 5 s clock
    if (millis() - loudSince > 5000) {                   // loud for 5 s
      tone(BUZZ, 1200, 300);
      loudSince = millis();                              // wait another 5 s
    }
  } else {
    loudSince = 0;                                       // one cough resets it
  }

  Serial.print("pp="); Serial.print(pp);
  Serial.print("  score="); Serial.println(score);
  delay(80);
}
