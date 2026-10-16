/* PROJECT 18 - DIGITAL TASBIH COUNTER  (11.A Group 3)
   Each press = +1. Display shows the units; LEDs light at 10, 20 and 30.
   At 33 a two-note tone plays and it starts again.
   Hold the device tipped (tilt switch) for 1 second to clear.          */
const int BTN = 9;                      // count button (other leg to GND)
const int L10 = 10, L20 = 11, L30 = 12; // LEDs (each through 220 ohm)
const int BUZZ = 13;                    // PASSIVE buzzer (tone)
const int TILT = A0;                    // tilt switch, A0 used as digital input
const int TARGET = 33;

int count = 0, rounds = 0, lastBtn = HIGH;
unsigned long tiltSince = 0;

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
  pinMode(BTN, INPUT_PULLUP);
  pinMode(TILT, INPUT_PULLUP);
  pinMode(L10, OUTPUT); pinMode(L20, OUTPUT); pinMode(L30, OUTPUT);
  pinMode(BUZZ, OUTPUT);
}

void loop() {
  if (justPressed(BTN, lastBtn)) {      // count the EDGE of the press
    count++;
    tone(BUZZ, 2500, 15);               // tiny click so the user feels it counted
    Serial.print("count = "); Serial.println(count);
  }

  if (digitalRead(TILT) == LOW) { if (tiltSince == 0) tiltSince = millis(); }
  else tiltSince = 0;
  if (tiltSince != 0 && millis() - tiltSince > 1000 && count > 0) {
    count = 0; Serial.println("cleared");
  }

  show(count % 10);
  digitalWrite(L10, count >= 10);
  digitalWrite(L20, count >= 20);
  digitalWrite(L30, count >= 30);

  if (count >= TARGET) {
    tone(BUZZ, 1200, 180); delay(220);
    tone(BUZZ, 1600, 360); delay(400);
    rounds++; count = 0;
    Serial.print("33 reached - rounds completed = "); Serial.println(rounds);
  }
}
