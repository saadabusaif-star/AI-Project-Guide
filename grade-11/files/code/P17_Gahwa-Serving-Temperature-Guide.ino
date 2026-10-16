/* PROJECT 17 - GAHWA SERVING TEMPERATURE GUIDE  (11.A Group 2)
   Thermistor (sealed in a thin plastic sleeve) in the cup.
   BLUE too cold, GREEN ready to serve, RED too hot. One chirp when ready.
   Display: 0 = 30 C ... 9 = 90 C.  SAFETY: warm water only, never boiling. */
const int TH = A0;
const int REDL = 9, GRN = 10, BLU = 11; // LEDs (each through 220 ohm)
const int BUZZ = 12;                    // ACTIVE buzzer

// ---- YOUR serving band - set it after testing with a real thermometer ----
const float READY_LOW  = 60.0;
const float READY_HIGH = 75.0;

bool chirped = false;
unsigned long chirpAt = 0;

// ---------- THERMISTOR -> degrees C (Beta equation) ----------
const float BETA = 3950.0, R0 = 10000.0, T0 = 298.15, RFIX = 10000.0;
float tempC() {
  long sum = 0;
  for (int i = 0; i < 10; i++) sum += analogRead(TH);   // average 10 readings
  float raw = sum / 10.0;
  if (raw < 1 || raw > 1022) return -99;                // wire loose / short
  float rTh = RFIX * ((1023.0 / raw) - 1.0);            // thermistor to 5V, 10k to GND
  float tK  = 1.0 / (1.0 / T0 + (1.0 / BETA) * log(rTh / R0));
  return tK - 273.15;
}
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

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) pinMode(SEG[i], OUTPUT);
  pinMode(REDL, OUTPUT); pinMode(GRN, OUTPUT); pinMode(BLU, OUTPUT);
  pinMode(BUZZ, OUTPUT);
}

void loop() {
  float t = tempC();
  if (t == -99) { Serial.println("Check thermistor wiring"); delay(500); return; }

  int score = constrain((int)((t - 30.0) / 60.0 * 9.0 + 0.5), 0, 9);
  show(score);

  bool ready = (t >= READY_LOW && t <= READY_HIGH);
  digitalWrite(BLU,  t < READY_LOW);
  digitalWrite(GRN,  ready);
  digitalWrite(REDL, t > READY_HIGH);

  if (ready && !chirped) { chirped = true; chirpAt = millis(); }   // once per entry
  if (!ready) chirped = false;
  digitalWrite(BUZZ, (chirped && millis() - chirpAt < 120) ? HIGH : LOW);

  Serial.print("T = "); Serial.print(t, 1);
  Serial.print(" C   score = "); Serial.println(score);
  delay(250);
}
