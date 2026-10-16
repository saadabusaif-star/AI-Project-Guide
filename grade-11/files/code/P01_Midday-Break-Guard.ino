/* PROJECT 1 - MIDDAY BREAK GUARD  (11.C Group 3)
   Measures temperature + humidity, works out how hot the air FEELS
   (Thom Discomfort Index) and shows GREEN / YELLOW / RED.
   On RED the buzzer beeps every second until MUTE is pressed.
   Libraries: "DHT sensor library" (Adafruit) + "Adafruit Unified Sensor"   */
#include <DHT.h>
#define DHTPIN  2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int R = 9, G = 10, B = 11;        // RGB LED legs (each through 220 ohm)
const int BUZZ = 7;                     // ACTIVE buzzer
const int BTN  = 4;                     // MUTE button (other leg to GND)

const float DI_CARE = 24.0;             // from here: YELLOW (take care)
const float DI_STOP = 28.0;             // from here: RED (stop outdoor work)

bool muted = false;
int  lastBtn = HIGH;
int  level = -1;                        // -1 none yet, 0 green, 1 yellow, 2 red
unsigned long lastRead = 0, lastBeep = 0;
unsigned long redSeconds = 0;           // CHALLENGE: time spent in red

// ---------- RGB LED ----------
const bool RGB_COMMON_ANODE = false;   // true = longest leg goes to 5V instead of GND
void colour(bool r, bool g, bool b) {
  digitalWrite(R, RGB_COMMON_ANODE ? !r : r);
  digitalWrite(G, RGB_COMMON_ANODE ? !g : g);
  digitalWrite(B, RGB_COMMON_ANODE ? !b : b);
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
  dht.begin();
  pinMode(R, OUTPUT); pinMode(G, OUTPUT); pinMode(B, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  pinMode(BTN, INPUT_PULLUP);
  colour(0, 0, 0);
  delay(1000);                          // DHT11 needs 1 s after power-up
}

void loop() {
  // 1) the mute button is checked EVERY loop, so a press is never missed
  if (justPressed(BTN, lastBtn)) {
    muted = !muted;
    Serial.println(muted ? "MUTE ON" : "MUTE OFF");
  }

  // 2) read the DHT11 only every 2 seconds (it cannot go faster)
  if (millis() - lastRead >= 2000) {
    lastRead = millis();
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (isnan(t) || isnan(h)) {
      Serial.println("DHT11 not responding - check DATA wire / 10k pull-up");
    } else {
      float di = t - 0.55 * (1 - h / 100.0) * (t - 14.5);
      if (di < DI_CARE)      level = 0;
      else if (di < DI_STOP) level = 1;
      else                   level = 2;
      if (level < 2) muted = false;     // safe again: next red will alarm
      if (level == 2) redSeconds += 2;

      if (level == 0) colour(0, 1, 0);  // green
      if (level == 1) colour(1, 1, 0);  // yellow = red + green
      if (level == 2) colour(1, 0, 0);  // red

      Serial.print("T="); Serial.print(t, 1);
      Serial.print(" C  H="); Serial.print(h, 0);
      Serial.print(" %  DI="); Serial.print(di, 1);
      Serial.print("  red minutes="); Serial.println(redSeconds / 60.0, 1);
    }
  }

  // 3) buzzer: 200 ms beep every second while RED and not muted (no delay!)
  if (level == 2 && !muted && millis() - lastBeep >= 1000) lastBeep = millis();
  digitalWrite(BUZZ, (level == 2 && !muted && millis() - lastBeep < 200) ? HIGH : LOW);
}
