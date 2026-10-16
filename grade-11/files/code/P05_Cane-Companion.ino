/* PROJECT 5 - CANE COMPANION  (11.B Group 5)
   IR sensor warns of an obstacle ahead (short high beeps).
   Tilt switch detects the cane lying on the floor for 1 s (long low tone).
   SILENCE button turns the sound off/on; the LED still works.          */
const int IR = 2;                       // IR OUT (LOW = obstacle)
const int TILT = 3;                     // tilt switch (LOW = cane fallen)
const int SILENCE = 4;                  // button (other leg to GND)
const int BUZZ = 8;                     // PASSIVE buzzer (tone)
const int LED = 9;                      // LED through 220 ohm

bool silent = false;
int  lastBtn = HIGH;
unsigned long tiltSince = 0, lastBeep = 0, lastPrint = 0;

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
  pinMode(IR, INPUT);
  pinMode(TILT, INPUT_PULLUP);
  pinMode(SILENCE, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
}

void loop() {
  if (justPressed(SILENCE, lastBtn)) {
    silent = !silent;
    Serial.println(silent ? "SILENT" : "SOUND ON");
  }

  bool obstacle = (digitalRead(IR) == LOW);

  // fallen only if the tilt switch stays closed for 1 second (no false alarms)
  if (digitalRead(TILT) == LOW) { if (tiltSince == 0) tiltSince = millis(); }
  else tiltSince = 0;
  bool fallen = (tiltSince != 0 && millis() - tiltSince > 1000);

  digitalWrite(LED, obstacle || fallen);

  if (silent) {
    noTone(BUZZ);
  } else if (fallen) {
    tone(BUZZ, 900);                    // continuous: person may be down
  } else if (obstacle) {
    if (millis() - lastBeep >= 250) { tone(BUZZ, 2000, 80); lastBeep = millis(); }
  } else {
    noTone(BUZZ);
  }

  if (millis() - lastPrint >= 300) {
    lastPrint = millis();
    Serial.print("obstacle="); Serial.print(obstacle);
    Serial.print("  fallen="); Serial.println(fallen);
  }
}
