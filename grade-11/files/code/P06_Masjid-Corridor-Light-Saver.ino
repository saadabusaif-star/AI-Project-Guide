/* PROJECT 6 - MASJID CORRIDOR LIGHT SAVER  (11.B Group 6)
   Lamps (4 LEDs) switch on ONLY when it is dark AND a person passed in
   the last 10 seconds. The knob sets how dark counts as "dark".         */
const int LDR = A0, KNOB = A1;
const int IR = 2;                       // IR OUT (LOW = person)
const int LAMP[4] = {3, 4, 5, 6};       // 4 LEDs, each with its OWN 220 ohm
const unsigned long HOLD_MS = 10000;    // lamps stay on 10 s after last person

unsigned long lastSeen = 0;
bool everSeen = false;

void setup() {
  Serial.begin(9600);
  pinMode(IR, INPUT);
  for (int i = 0; i < 4; i++) pinMode(LAMP[i], OUTPUT);
}

void loop() {
  int light     = analogRead(LDR);                       // brighter = bigger
  int darkLevel = map(analogRead(KNOB), 0, 1023, 100, 900);

  bool isDark = (light < darkLevel);
  bool person = (digitalRead(IR) == LOW);
  if (person) { lastSeen = millis(); everSeen = true; }

  bool on = isDark && everSeen && (millis() - lastSeen < HOLD_MS);
  for (int i = 0; i < 4; i++) digitalWrite(LAMP[i], on);

  // three numbers -> open Tools > Serial Plotter to see the graph
  Serial.print(light);     Serial.print(",");
  Serial.print(darkLevel); Serial.print(",");
  Serial.println(on ? 900 : 0);
  delay(100);
}
