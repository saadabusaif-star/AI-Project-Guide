/* PROJECT 16 - SOLAR PANEL ANGLE FINDER  (11.A Group 1)
   Tilt the "panel" (LDR on card) slowly. Score 0-9 = how much light it gets.
   A beep marks every new best; GREEN = at the best angle, RED = below it. */
const int LDR = A0, RESETBTN = A1;      // RESET: A1 used as a digital input
const int GRN = 9, REDL = 10;           // LEDs (each through 220 ohm)
const int BUZZ = 11;                    // ACTIVE buzzer

// ---- CALIBRATE with your own lamp ----
const int DARK   = 80;                  // LDR in shade
const int BRIGHT = 900;                 // LDR facing the lamp

int best = 0, lastBtn = HIGH;
unsigned long beepAt = 0, lastPrint = 0;

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

int readAvg() {                         // average 20 readings (lights flicker)
  long s = 0;
  for (int i = 0; i < 20; i++) s += analogRead(LDR);
  return s / 20;
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) pinMode(SEG[i], OUTPUT);
  pinMode(GRN, OUTPUT); pinMode(REDL, OUTPUT); pinMode(BUZZ, OUTPUT);
  pinMode(RESETBTN, INPUT_PULLUP);
}

void loop() {
  int raw = readAvg();
  int score = constrain(map(raw, DARK, BRIGHT, 0, 9), 0, 9);

  if (justPressed(RESETBTN, lastBtn)) { best = 0; Serial.println("RESET - search again"); }
  if (score > best) { best = score; beepAt = millis(); }   // new record
  digitalWrite(BUZZ, (millis() - beepAt < 80 && beepAt != 0) ? HIGH : LOW);

  show(score);
  digitalWrite(GRN,  best > 0 && score >= best);
  digitalWrite(REDL, best > 0 && score <  best);

  if (millis() - lastPrint >= 200) {
    lastPrint = millis();
    Serial.print("raw="); Serial.print(raw);
    Serial.print("  score="); Serial.print(score);
    Serial.print("  best="); Serial.println(best);
  }
}
