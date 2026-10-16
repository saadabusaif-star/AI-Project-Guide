/* PROJECT 10 - HOT CAR CHILD ALARM  (11.C Group 5)
   Alarm ONLY when the seat is occupied AND the car is too hot.
   RGB: GREEN empty, YELLOW occupied & OK, flashing RED + buzzer = danger,
   BLUE = system disarmed (ARM button toggles).                         */
#include <DHT.h>
#define DHTPIN  3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int SEAT = 2;                     // tilt switch as seat sensor (LOW = occupied)
const int BUZZ = 4;                     // ACTIVE buzzer
const int ARM  = 5;                     // ARM / DISARM button
const int R = 9, G = 10, B = 11;        // RGB LED (each through 220 ohm)
const float TEMP_LIMIT = 32.0;          // 32 for the class demo, about 30 in a real car

bool armed = true;
int  lastBtn = HIGH;
float t = NAN;
unsigned long lastRead = 0;

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
  pinMode(SEAT, INPUT_PULLUP);
  pinMode(ARM, INPUT_PULLUP);
  pinMode(BUZZ, OUTPUT);
  pinMode(R, OUTPUT); pinMode(G, OUTPUT); pinMode(B, OUTPUT);
}

void loop() {
  if (justPressed(ARM, lastBtn)) { armed = !armed; Serial.println(armed ? "ARMED" : "DISARMED"); }

  if (millis() - lastRead >= 2000) {    // DHT11 every 2 s
    lastRead = millis();
    float v = dht.readTemperature();
    if (!isnan(v)) t = v;
    Serial.print("occupied="); Serial.print(digitalRead(SEAT) == LOW);
    Serial.print("  T="); Serial.println(t);
  }

  if (!armed) { colour(0, 0, 1); digitalWrite(BUZZ, LOW); return; }

  bool occupied = (digitalRead(SEAT) == LOW);
  bool danger = occupied && !isnan(t) && t > TEMP_LIMIT;   // the AND is the whole idea

  if (danger) {
    bool flashOn = (millis() / 300) % 2;                    // flash every 300 ms
    colour(flashOn, 0, 0);
    digitalWrite(BUZZ, flashOn);
  } else {
    digitalWrite(BUZZ, LOW);
    if (occupied) colour(1, 1, 0);      // yellow
    else          colour(0, 1, 0);      // green
  }
}
