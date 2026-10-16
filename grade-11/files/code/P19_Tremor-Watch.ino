/* PROJECT 19 - TREMOR WATCH  (11.A Group 4)
   Counts shakes of the tilt switch in the last 5 seconds (sliding window).
   Alarm only when BOTH agree: 5+ shakes AND a rumble on the microphone.  */
const int TILT = 9;                     // tilt / vibration switch (other leg to GND)
const int GRN = 10, REDL = 11;          // LEDs (each through 220 ohm)
const int BUZZ = 12;                    // ACTIVE buzzer
const int MIC = A0, RESETBTN = A1;      // sound AO; RESET on A1 as digital input

const unsigned long WINDOW_MS = 5000;   // the sliding window
const int SHAKE_LIMIT  = 5;             // shakes needed inside the window
const int RUMBLE_LIMIT = 60;            // CALIBRATE: microphone peak-to-peak

const int MAX_EVENTS = 24;
unsigned long events[MAX_EVENTS];
int nEvents = 0, lastTilt = HIGH, lastBtn = HIGH;
unsigned long lastShake = 0, lastBeep = 0, lastPrint = 0;

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

int loudness() {                        // peak-to-peak over 30 ms
  unsigned long start = millis();
  int hi = 0, lo = 1023;
  while (millis() - start < 30) {
    int v = analogRead(MIC);
    if (v > hi) hi = v;
    if (v < lo) lo = v;
  }
  return hi - lo;
}

void addEvent() { if (nEvents < MAX_EVENTS) events[nEvents++] = millis(); }

int countInWindow() {                   // forget shakes older than 5 s
  int keep = 0;
  for (int i = 0; i < nEvents; i++)
    if (millis() - events[i] <= WINDOW_MS) events[keep++] = events[i];
  nEvents = keep;
  return nEvents;
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) pinMode(SEG[i], OUTPUT);
  pinMode(TILT, INPUT_PULLUP);
  pinMode(RESETBTN, INPUT_PULLUP);
  pinMode(GRN, OUTPUT); pinMode(REDL, OUTPUT); pinMode(BUZZ, OUTPUT);
}

void loop() {
  int now = digitalRead(TILT);
  if (lastTilt == HIGH && now == LOW && millis() - lastShake > 60) {   // one shake = one edge
    addEvent();
    lastShake = millis();
  }
  lastTilt = now;

  if (justPressed(RESETBTN, lastBtn)) { nEvents = 0; Serial.println("RESET"); }

  int shakes = countInWindow();
  int rumble = loudness();
  show(shakes);

  bool alarm = (shakes >= SHAKE_LIMIT && rumble > RUMBLE_LIMIT);   // BOTH must agree
  digitalWrite(GRN, !alarm);
  digitalWrite(REDL, alarm);
  if (alarm && millis() - lastBeep >= 400) lastBeep = millis();
  digitalWrite(BUZZ, (alarm && millis() - lastBeep < 120) ? HIGH : LOW);

  if (millis() - lastPrint >= 250) {
    lastPrint = millis();
    Serial.print("shakes="); Serial.print(shakes);
    Serial.print("  rumble="); Serial.println(rumble);
  }
}
