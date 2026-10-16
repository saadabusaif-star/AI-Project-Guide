/* PROJECT 21 - WELL WATER SALINITY TESTER  (11.A Group 6)
   The soil probe is used as a conductivity cell: salty water conducts better.
   Salinity 0 (fresh) to 9 (your saltiest cup) on the display. The knob sets
   what the crop can tolerate: saltier than the knob = RED + beep.       */
const int PROBE = A0, KNOB = A1;
const int GRN = 9, REDL = 10;           // LEDs (each through 220 ohm)
const int BUZZ = 11;                    // ACTIVE buzzer

// ---- CALIBRATE from your own cups (Serial Monitor) ----
// With most kit modules the number goes DOWN as water gets saltier.
// Just type YOUR two numbers: map() works in either direction.
const int CLEAN_READING = 750;          // probe in tap / bottled water
const int SALTY_READING = 200;          // probe in your saltiest cup

unsigned long lastBeep = 0;

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

int readProbe() {
  long s = 0;
  for (int i = 0; i < 10; i++) { s += analogRead(PROBE); delay(2); }
  return s / 10;
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) pinMode(SEG[i], OUTPUT);
  pinMode(GRN, OUTPUT); pinMode(REDL, OUTPUT); pinMode(BUZZ, OUTPUT);
}

void loop() {
  int raw = readProbe();
  int salinity  = constrain(map(raw, CLEAN_READING, SALTY_READING, 0, 9), 0, 9);
  int tolerance = map(analogRead(KNOB), 0, 1023, 0, 9);
  show(salinity);

  bool tooSalty = (salinity > tolerance);
  digitalWrite(REDL, tooSalty);
  digitalWrite(GRN, !tooSalty);
  if (tooSalty && millis() - lastBeep >= 1000) lastBeep = millis();
  digitalWrite(BUZZ, (tooSalty && millis() - lastBeep < 150) ? HIGH : LOW);

  Serial.print("raw="); Serial.print(raw);
  Serial.print("  salinity="); Serial.print(salinity);
  Serial.print("  crop limit="); Serial.println(tolerance);
  delay(100);
}
