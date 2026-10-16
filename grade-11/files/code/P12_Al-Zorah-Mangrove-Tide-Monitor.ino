/* PROJECT 12 - AL ZORAH MANGROVE TIDE MONITOR  (11.C Group 7)
   Water level sensor -> RED low tide, YELLOW mid, GREEN high tide.
   Low tide OR the stake falling over (tilt switch) -> beep.
   The sensor is powered from D7 ONLY while reading = much less corrosion. */
const int SENSE = A0, POWER = 7;
const int RED = 2, YEL = 3, GRN = 4;    // LEDs (each through 220 ohm)
const int TILT = 5;                     // tilt switch on the stake
const int BUZZ = 6;                     // ACTIVE buzzer

// ---- CALIBRATE with your own cup of water ----
const int LOW_MARK = 150, HIGH_MARK = 400;

unsigned long tiltSince = 0, lastBeep = 0, lastRead = 0, lastPrint = 0;
int lvl = 0;

int readLevel() {
  digitalWrite(POWER, HIGH);            // switch the strip ON
  delay(20);
  int v = analogRead(SENSE);
  digitalWrite(POWER, LOW);             // and OFF again
  return v;
}

void setup() {
  Serial.begin(9600);
  pinMode(POWER, OUTPUT); digitalWrite(POWER, LOW);
  pinMode(RED, OUTPUT); pinMode(YEL, OUTPUT); pinMode(GRN, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  pinMode(TILT, INPUT_PULLUP);
}

void loop() {
  if (millis() - lastRead >= 400) { lastRead = millis(); lvl = readLevel(); }

  // stake counts as fallen only if the tilt switch stays closed for 300 ms
  if (digitalRead(TILT) == LOW) { if (tiltSince == 0) tiltSince = millis(); }
  else tiltSince = 0;
  bool fallen = (tiltSince != 0 && millis() - tiltSince > 300);

  digitalWrite(RED, lvl <  LOW_MARK);
  digitalWrite(YEL, lvl >= LOW_MARK && lvl < HIGH_MARK);
  digitalWrite(GRN, lvl >= HIGH_MARK);

  bool alarm = (lvl < LOW_MARK) || fallen;
  if (alarm && millis() - lastBeep >= 1000) lastBeep = millis();
  digitalWrite(BUZZ, (alarm && millis() - lastBeep < 120) ? HIGH : LOW);

  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();
    Serial.print("tide="); Serial.print(lvl);
    Serial.print("  stake fallen="); Serial.println(fallen);
  }
}
