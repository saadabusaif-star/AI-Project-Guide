/* PROJECT 15 - REACTION TIME CHAMPION  (11.C Group 2)
   RED = wait... after a random 2-6 s the GREEN LED says GO.
   First player to press wins (display shows 1 or 2). Pressing early = FALSE START. */
const int BTN_A = 9, BTN_B = 10;        // player buttons (other leg to GND)
const int GRN = 11, REDL = 12;          // LEDs (each through 220 ohm)
const int BUZZ = 13;                    // PASSIVE buzzer (tone)

int bestMs = 9999;                      // CHALLENGE: best time of the day

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

bool anyPressed() { return digitalRead(BTN_A) == LOW || digitalRead(BTN_B) == LOW; }

void falseStart() {
  show(0);
  digitalWrite(GRN, LOW); digitalWrite(REDL, HIGH);
  tone(BUZZ, 200, 600);
  Serial.println("FALSE START");
  delay(1500);
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) pinMode(SEG[i], OUTPUT);
  pinMode(BTN_A, INPUT_PULLUP); pinMode(BTN_B, INPUT_PULLUP);
  pinMode(GRN, OUTPUT); pinMode(REDL, OUTPUT); pinMode(BUZZ, OUTPUT);
  randomSeed(analogRead(A0));           // A0 left EMPTY on purpose
  Serial.println("Reaction Time Champion - ready");
}

void loop() {
  show(0);
  digitalWrite(REDL, HIGH); digitalWrite(GRN, LOW);

  while (anyPressed()) { }              // wait until both buttons are released
  delay(500);

  unsigned long wait = random(2000, 6000);
  unsigned long t0 = millis();
  while (millis() - t0 < wait) {
    if (anyPressed()) { falseStart(); return; }
  }

  digitalWrite(REDL, LOW); digitalWrite(GRN, HIGH);      // GO!
  unsigned long start = millis();

  int winner = 0;
  while (winner == 0) {
    byte a = digitalRead(BTN_A), b = digitalRead(BTN_B); // read both together
    if (a == LOW && b == LOW) winner = (random(2) == 0) ? 1 : 2;   // exact tie: fair coin
    else if (a == LOW) winner = 1;
    else if (b == LOW) winner = 2;
    if (millis() - start > 5000) { Serial.println("Nobody pressed"); return; }
  }
  unsigned long rt = millis() - start;
  if ((int)rt < bestMs) bestMs = rt;

  show(winner);
  tone(BUZZ, 1500, 150); delay(180); tone(BUZZ, 2200, 250);

  Serial.print("Winner: Player "); Serial.print(winner);
  Serial.print("   reaction = "); Serial.print(rt);
  Serial.print(" ms   best today = "); Serial.print(bestMs); Serial.println(" ms");
  delay(3000);
}
