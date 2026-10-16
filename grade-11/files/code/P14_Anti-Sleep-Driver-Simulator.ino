/* PROJECT 14 - ANTI-SLEEP DRIVER SIMULATOR  (11.C Group 1)
   At random times the GREEN LED asks "are you awake?" - press ALERT fast.
   Slow (>1.5 s) or missed checks lower the alertness score (9 -> 0).
   Head nod (tilt switch) = score 0 at once. Score 3 or less = RED + alarm. */
const int ALERT = 9, TILT = 10;         // buttons / tilt switch (other leg to GND)
const int GRN = 11, REDL = 12;          // LEDs (each through 220 ohm)
const int BUZZ = 13;                    // PASSIVE buzzer (tone)
const int RESETBTN = A0;                // A0 used as a digital input

int  score = 9;
bool waiting = false;
int  lastAlert = HIGH, lastReset = HIGH;
unsigned long askedAt = 0, nextCheck = 0, lastBeep = 0, tiltSince = 0;

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

void scheduleNext() {
  waiting = false;
  digitalWrite(GRN, LOW);
  nextCheck = millis() + random(6000, 15000);  // next check in 6-15 s
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) pinMode(SEG[i], OUTPUT);
  pinMode(ALERT, INPUT_PULLUP);
  pinMode(TILT, INPUT_PULLUP);
  pinMode(RESETBTN, INPUT_PULLUP);
  pinMode(GRN, OUTPUT); pinMode(REDL, OUTPUT); pinMode(BUZZ, OUTPUT);
  randomSeed(analogRead(A1));           // A1 left EMPTY on purpose = random noise
  scheduleNext();
  Serial.println("Anti-Sleep Driver Simulator - session start");
}

void loop() {
  if (justPressed(RESETBTN, lastReset)) { score = 9; scheduleNext(); Serial.println("RESET"); }

  // head nod = tilt switch closed for at least 300 ms
  if (digitalRead(TILT) == LOW) { if (tiltSince == 0) tiltSince = millis(); }
  else tiltSince = 0;
  if (tiltSince != 0 && millis() - tiltSince > 300 && score > 0) {
    score = 0;
    Serial.println("HEAD NOD DETECTED");
  }

  if (!waiting && (long)(millis() - nextCheck) >= 0) {   // time for a check
    waiting = true; askedAt = millis();
    digitalWrite(GRN, HIGH);
  }

  bool pressed = justPressed(ALERT, lastAlert);          // a new press, not a held button
  if (waiting) {
    if (pressed) {
      unsigned long rt = millis() - askedAt;
      Serial.print("reaction = "); Serial.print(rt); Serial.println(" ms");
      if (rt > 1500 && score > 0) score--;               // slow = getting tired
      scheduleNext();
    } else if (millis() - askedAt > 3000) {              // missed completely
      Serial.println("MISSED CHECK");
      if (score > 0) score--;
      scheduleNext();
    }
  }

  show(score);
  digitalWrite(REDL, score <= 3);
  if (score <= 3 && millis() - lastBeep > 700) {         // wake-up alarm, no delay()
    tone(BUZZ, 850, 250);
    lastBeep = millis();
  }
}
