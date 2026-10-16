/* PROJECT 13 - HEALTHY STUDY DESK GUARDIAN  (11.A Group 7)
   RED LED = face too close to the book/screen, YELLOW = room too dark,
   BLUE = room too warm. Too close for 10 s -> beep (SNOOZE button mutes). */
#include <DHT.h>
#define DHTPIN  3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int IR = 2, SNOOZE = 4;
const int L_CLOSE = 5, L_DARK = 6, L_WARM = 7;   // LEDs (each through 220 ohm)
const int BUZZ = 8;                              // ACTIVE buzzer
const int LDR = A0;

// ---- CALIBRATE in your own room ----
const int   DARK_ON  = 300;             // darker than this -> yellow ON
const int   DARK_OFF = 340;             // brighter than this -> yellow OFF (dead band)
const float WARM_LIMIT = 27.0;

unsigned long tooCloseSince = 0, lastRead = 0, lastBeep = 0, lastPrint = 0;
bool snoozed = false, tooDark = false, tooWarm = false;
int  lastBtn = HIGH;
float t = NAN;

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
  dht.begin();
  pinMode(IR, INPUT);
  pinMode(SNOOZE, INPUT_PULLUP);
  pinMode(L_CLOSE, OUTPUT); pinMode(L_DARK, OUTPUT); pinMode(L_WARM, OUTPUT);
  pinMode(BUZZ, OUTPUT);
}

void loop() {
  if (justPressed(SNOOZE, lastBtn)) { snoozed = !snoozed; Serial.println(snoozed ? "SNOOZE ON" : "SNOOZE OFF"); }

  bool tooClose = (digitalRead(IR) == LOW);
  int light = analogRead(LDR);
  if (light < DARK_ON)  tooDark = true;           // dead band stops flicker
  if (light > DARK_OFF) tooDark = false;

  if (millis() - lastRead >= 2000) {
    lastRead = millis();
    float v = dht.readTemperature();
    if (!isnan(v)) t = v;
    tooWarm = (!isnan(t) && t > WARM_LIMIT);
  }

  digitalWrite(L_CLOSE, tooClose);
  digitalWrite(L_DARK, tooDark);
  digitalWrite(L_WARM, tooWarm);

  if (tooClose) { if (tooCloseSince == 0) tooCloseSince = millis(); }
  else tooCloseSince = 0;

  bool nag = !snoozed && tooCloseSince != 0 && millis() - tooCloseSince > 10000;
  if (nag && millis() - lastBeep >= 1000) lastBeep = millis();
  digitalWrite(BUZZ, (nag && millis() - lastBeep < 150) ? HIGH : LOW);

  if (millis() - lastPrint >= 500) {
    lastPrint = millis();
    Serial.print("close="); Serial.print(tooClose);
    Serial.print("  light="); Serial.print(light);
    Serial.print("  T="); Serial.println(t);
  }
}
