/* PROJECT 20 - HEARING RANGE TESTER  (11.A Group 5)
   Turn the knob slowly upward: the tone rises from 200 Hz to 12 000 Hz.
   Press HEARD while you can still hear it. Display = band 0-9.
   GREEN = inside your best heard band, RED = above it.
   SAFETY: keep the buzzer at arm's length - never against the ear.     */
const int POT = A0;                     // frequency knob (middle leg)
const int HEARD = 9;                    // button (other leg to GND)
const int GRN = 10, REDL = 11;          // LEDs (each through 220 ohm)
const int BUZZ = 13;                    // PASSIVE buzzer (tone)

const long F_LOW  = 200;                // Hz at the bottom of the knob
const long F_HIGH = 12000;              // Hz at the top of the knob

int bestBand = -1, lastBtn = HIGH;
long lastFreq = 0;
unsigned long lastPrint = 0;

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
// True ONCE at the moment a button goes from up (HIGH) to pressed (LOW).
// Holding the button down does NOT repeat it.
bool justPressed(int pin, int &lastState) {
  int now = digitalRead(pin);
  bool pressed = (lastState == HIGH && now == LOW);
  lastState = now;
  if (pressed) delay(25);               // ignore contact bounce
  return pressed;
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) pinMode(SEG[i], OUTPUT);
  pinMode(HEARD, INPUT_PULLUP);
  pinMode(GRN, OUTPUT); pinMode(REDL, OUTPUT);
  pinMode(BUZZ, OUTPUT);
}

void loop() {
  long sum = 0;
  for (int i = 0; i < 8; i++) sum += analogRead(POT);   // smooth the knob
  int knob = sum / 8;
  long freq = map(knob, 0, 1023, F_LOW, F_HIGH);
  int  band = map(knob, 0, 1023, 0, 9);

  if (abs(freq - lastFreq) > 20) { tone(BUZZ, freq); lastFreq = freq; }   // only retune on change
  show(band);

  if (justPressed(HEARD, lastBtn)) {
    if (band > bestBand) bestBand = band;
    Serial.print("HEARD at band "); Serial.print(band);
    Serial.print("  ("); Serial.print(freq); Serial.println(" Hz)");
  }

  digitalWrite(GRN,  bestBand >= 0 && band <= bestBand);
  digitalWrite(REDL, bestBand >= 0 && band >  bestBand);

  if (millis() - lastPrint >= 300) {
    lastPrint = millis();
    Serial.print("freq="); Serial.print(freq);
    Serial.print("  band="); Serial.print(band);
    Serial.print("  best="); Serial.println(bestBand);
  }
}
