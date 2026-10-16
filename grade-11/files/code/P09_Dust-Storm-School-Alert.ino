/* PROJECT 9 - DUST STORM SCHOOL ALERT  (11.B Group 2)
   Dust in the air blocks sunlight. The LDR compares today's light with a
   clear day: GREEN safe, YELLOW caution, RED = keep students indoors.
   The DHT11 adds temperature and humidity to the Serial Plotter.        */
#include <DHT.h>
#define DHTPIN  2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int LDR = A0;
const int SAFE = 3, CAUTION = 4, INDOORS = 5;   // LEDs (each through 220 ohm)

// ---- CALIBRATE: LDR reading on a CLEAR day at the same hour ----
const int CLEAR_DAY = 700;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(SAFE, OUTPUT); pinMode(CAUTION, OUTPUT); pinMode(INDOORS, OUTPUT);
  delay(1000);
}

void loop() {
  long sum = 0;
  for (int i = 0; i < 10; i++) { sum += analogRead(LDR); delay(5); }
  int light = sum / 10;
  float percent = 100.0 * light / CLEAR_DAY;   // % of a clear day's light

  digitalWrite(SAFE,    percent >  75);
  digitalWrite(CAUTION, percent <= 75 && percent > 45);
  digitalWrite(INDOORS, percent <= 45);

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) { t = 0; h = 0; }   // keep the plotter running

  // numbers only -> Tools > Serial Plotter draws three lines
  Serial.print(t); Serial.print(",");
  Serial.print(h); Serial.print(",");
  Serial.println(percent);
  delay(2000);                                  // DHT11: never faster than 2 s
}
