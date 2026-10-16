/* PROJECT 4 - WUDU WATER GUARDIAN  (11.B Group 4)
   IR sensor sees hands under the tap and times the water with millis().
   Litres = flow rate x time, shown on the 7-segment. Past the limit the
   LED turns RED and beeps while water runs. Button = reset for next person. */
const int GRN = 9, REDL = 10;           // LEDs (each through 220 ohm)
const int BUZZ = 11;                    // ACTIVE buzzer
const int IR = 12;                      // IR module OUT (LOW = hands present)
const int RESETBTN = A0;                // A0 used as a normal digital input

// ---- MEASURE YOUR OWN TAP: seconds to fill a 1-litre bottle -> 1 / seconds ----
const float FLOW_LPS = 0.10;            // litres per second (0.10 = 6 L per minute)
const float LIMIT_L  = 3.0;             // litres allowed for one wudu

float litres = 0;
bool  flowing = false;
int   lastBtn = HIGH;
unsigned long lastTick = 0, lastBeep = 0, lastPrint = 0;

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
  pinMode(GRN, OUTPUT); pinMode(REDL, OUTPUT); pinMode(BUZZ, OUTPUT);
  pinMode(IR, INPUT);
  pinMode(RESETBTN, INPUT_PULLUP);
}

void loop() {
  bool hands = (digitalRead(IR) == LOW);

  if (hands) {
    if (!flowing) { flowing = true; lastTick = millis(); }
    unsigned long now = millis();
    litres += FLOW_LPS * (now - lastTick) / 1000.0;      // add this slice of time
    lastTick = now;
  } else {
    flowing = false;
  }

  if (justPressed(RESETBTN, lastBtn)) { litres = 0; Serial.println("RESET - next person"); }

  show((int)litres);                                     // whole litres 0-9
  bool over = (litres > LIMIT_L);
  digitalWrite(REDL, over);
  digitalWrite(GRN, !over);

  // beep 100 ms every second, only while over the limit AND water is running
  if (over && hands && millis() - lastBeep >= 1000) lastBeep = millis();
  digitalWrite(BUZZ, (over && hands && millis() - lastBeep < 100) ? HIGH : LOW);

  if (millis() - lastPrint >= 500) {
    lastPrint = millis();
    Serial.print("litres = "); Serial.println(litres, 2);
  }
}
