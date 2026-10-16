/* PROJECT 8 - HERITAGE LIGHT HARP  (11.B Group 1)
   Hand over LDR 1 = choose the note. Hand over LDR 2 = change the colour.
   SCALE button: Western major <-> maqam Hijaz feel.  OCTAVE button: x1 / x2. */
const int L1 = A0, L2 = A1;             // LDR dividers
const int BUZZ = 8;                     // PASSIVE buzzer (tone)
const int R = 5, G = 6, B = 9;          // RGB on PWM pins (NOT 3 or 11: tone() uses that timer)
const int SW_SCALE = 2, SW_OCT = 4;     // buttons (other leg to GND)

int major[8] = {262, 294, 330, 349, 392, 440, 494, 523};   // C major
int hijaz[8] = {262, 277, 330, 349, 392, 415, 494, 523};   // maqam Hijaz feel

// ---- CALIBRATE with your own readings (Serial Monitor) ----
const int BRIGHT = 800;                 // LDR in full light
const int COVERED = 120;                // hand close over the LDR
const int SILENCE_LEVEL = 80;           // completely covered = silence
const bool RGB_COMMON_ANODE = false;    // true = longest leg to 5V

bool useHijaz = false;
int  octave = 1, lastNote = 0;
int  lastScale = HIGH, lastOct = HIGH;

// True ONCE at the moment a button goes from up (HIGH) to pressed (LOW).
// Holding the button down does NOT repeat it.
bool justPressed(int pin, int &lastState) {
  int now = digitalRead(pin);
  bool pressed = (lastState == HIGH && now == LOW);
  lastState = now;
  if (pressed) delay(25);               // ignore contact bounce
  return pressed;
}

int readAvg(int pin) {                  // average 5 readings (lights flicker at 50 Hz)
  long s = 0;
  for (int i = 0; i < 5; i++) s += analogRead(pin);
  return s / 5;
}

void pwm(int pin, int v) {
  v = constrain(v, 0, 255);
  analogWrite(pin, RGB_COMMON_ANODE ? 255 - v : v);
}

void setup() {
  Serial.begin(9600);
  pinMode(SW_SCALE, INPUT_PULLUP);
  pinMode(SW_OCT, INPUT_PULLUP);
  pinMode(R, OUTPUT); pinMode(G, OUTPUT); pinMode(B, OUTPUT);
}

void loop() {
  if (justPressed(SW_SCALE, lastScale)) { useHijaz = !useHijaz; Serial.println(useHijaz ? "HIJAZ" : "MAJOR"); }
  if (justPressed(SW_OCT, lastOct))     { octave = (octave == 1) ? 2 : 1; }

  int a = readAvg(L1);
  int b = readAvg(L2);

  int note = 0;
  if (a >= SILENCE_LEVEL) {
    int idx = constrain(map(a, COVERED, BRIGHT, 0, 7), 0, 7);
    note = (useHijaz ? hijaz[idx] : major[idx]) * octave;
  }
  if (note != lastNote) {               // only change the sound when the note changes
    if (note == 0) noTone(BUZZ); else tone(BUZZ, note);
    lastNote = note;
  }

  int hue = constrain(map(b, COVERED, BRIGHT, 0, 255), 0, 255);
  pwm(R, hue);
  pwm(G, 255 - hue);
  pwm(B, abs(128 - hue) * 2);

  Serial.print(a); Serial.print(","); Serial.println(b);
  delay(30);
}
