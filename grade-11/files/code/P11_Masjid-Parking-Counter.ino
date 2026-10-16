/* PROJECT 11 - MASJID PARKING COUNTER  (11.C Group 6)
   Every car that breaks the IR beam = one space less. The 7-segment shows
   free spaces (9 to 0). FULL = RED LED + beep. RESET = car park empty.  */
const int GRN = 9, REDL = 10;           // LEDs (each through 220 ohm)
const int BUZZ = 11;                    // ACTIVE buzzer
const int IR = 12;                      // IR OUT (LOW = car in the beam)
const int RESETBTN = A0;                // A0 used as a digital input
const int CAPACITY = 9;

int freeSlots = CAPACITY;
int lastIR = HIGH, lastBtn = HIGH;
unsigned long lastCar = 0, lastBeep = 0;

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
  int now = digitalRead(IR);
  // count the EDGE (the moment a car arrives), not the state; 300 ms debounce
  if (lastIR == HIGH && now == LOW && millis() - lastCar > 300) {
    lastCar = millis();
    if (freeSlots > 0) freeSlots--;
    Serial.print("Car in. Free spaces = "); Serial.println(freeSlots);
  }
  lastIR = now;

  if (justPressed(RESETBTN, lastBtn)) { freeSlots = CAPACITY; Serial.println("RESET"); }

  show(freeSlots);
  bool full = (freeSlots == 0);
  digitalWrite(GRN, !full);
  digitalWrite(REDL, full);

  if (full && millis() - lastBeep >= 2000) lastBeep = millis();
  digitalWrite(BUZZ, (full && millis() - lastBeep < 100) ? HIGH : LOW);
}
