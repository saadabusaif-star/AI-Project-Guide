/* PROJECT 3 - DATE PALM WATER SAVER  (11.B Group 3)
   Soil probe -> dryness 0 (soaking) to 9 (bone dry). The knob sets when
   to water. Dry past the knob = RED + beep each second, else GREEN.      */
const int SOIL = A0, KNOB = A1;
const int GRN = 9, REDL = 10;           // LEDs (each through 220 ohm)
const int BUZZ = 11;                    // ACTIVE buzzer

// ---- CALIBRATE: put YOUR two readings from the Serial Monitor here ----
const int WET_READING = 280;            // probe in a glass of water
const int DRY_READING = 680;            // probe in dry soil / in the air

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

int readSoil() {                        // average 10 readings = steadier number
  long sum = 0;
  for (int i = 0; i < 10; i++) { sum += analogRead(SOIL); delay(2); }
  return sum / 10;
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) pinMode(SEG[i], OUTPUT);
  pinMode(GRN, OUTPUT); pinMode(REDL, OUTPUT); pinMode(BUZZ, OUTPUT);
}

void loop() {
  int raw       = readSoil();
  int dryness   = constrain(map(raw, WET_READING, DRY_READING, 0, 9), 0, 9);
  int threshold = map(analogRead(KNOB), 0, 1023, 0, 9);
  show(dryness);

  bool needWater = (dryness >= threshold);
  digitalWrite(REDL, needWater);
  digitalWrite(GRN, !needWater);

  if (needWater && millis() - lastBeep >= 1000) lastBeep = millis();
  digitalWrite(BUZZ, (needWater && millis() - lastBeep < 150) ? HIGH : LOW);

  Serial.print("raw="); Serial.print(raw);
  Serial.print("  dryness="); Serial.print(dryness);
  Serial.print("  knob="); Serial.println(threshold);
  delay(50);
}
