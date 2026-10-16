/* PROJECT 7 - SHARING FRIDGE GUARD  (11.B Group 7)
   Thermistor measures temperature. RGB: BLUE too cold, GREEN safe, RED too warm.
   Out of range for 30 s -> buzzer until ACKNOWLEDGE is pressed.
   Also remembers the lowest and highest temperature since power-on.   */
const int TH = A0;                      // thermistor divider middle point
const int R = 9, G = 10, B = 11;        // RGB LED legs (each through 220 ohm)
const int BUZZ = 7;                     // ACTIVE buzzer
const int ACK = 4;                      // acknowledge button (other leg to GND)

// classroom demo band - a REAL food fridge uses 2.0 to 8.0
const float SAFE_LOW = 20.0, SAFE_HIGH = 26.0;
const unsigned long ALARM_AFTER = 30000;   // 30 seconds

unsigned long outSince = 0, lastPrint = 0;
bool acknowledged = false;
int  lastBtn = HIGH;
float tMin = 999, tMax = -999;

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
// ---------- RGB LED ----------
const bool RGB_COMMON_ANODE = false;   // true = longest leg goes to 5V instead of GND
void colour(bool r, bool g, bool b) {
  digitalWrite(R, RGB_COMMON_ANODE ? !r : r);
  digitalWrite(G, RGB_COMMON_ANODE ? !g : g);
  digitalWrite(B, RGB_COMMON_ANODE ? !b : b);
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
  pinMode(R, OUTPUT); pinMode(G, OUTPUT); pinMode(B, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  pinMode(ACK, INPUT_PULLUP);
}

void loop() {
  float t = tempC();
  if (t == -99) { colour(1, 0, 1); Serial.println("Check thermistor wiring"); delay(500); return; }
  if (t < tMin) tMin = t;
  if (t > tMax) tMax = t;

  bool cold = (t < SAFE_LOW), hot = (t > SAFE_HIGH);
  if (cold)     colour(0, 0, 1);
  else if (hot) colour(1, 0, 0);
  else          colour(0, 1, 0);

  if (justPressed(ACK, lastBtn)) acknowledged = true;

  bool alarm = false;
  if (cold || hot) {
    if (outSince == 0) outSince = millis();               // first moment outside
    alarm = (millis() - outSince > ALARM_AFTER) && !acknowledged;
  } else {
    outSince = 0; acknowledged = false;                   // back to safe: re-arm
  }
  digitalWrite(BUZZ, alarm ? HIGH : LOW);

  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();
    Serial.print("T="); Serial.print(t, 1);
    Serial.print(" C  min="); Serial.print(tMin, 1);
    Serial.print("  max="); Serial.println(tMax, 1);
  }
}
