#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 4         // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT22 // DHT 22 (AM2302)


DHT_Unified dht(DHTPIN, DHTTYPE);

// Pinout for Segments
//               0  1  2   3  4  5  6  7
//               a  b  c   d  e  f  g  dp
int mySeven[] = {9, 7, 11, 5, 6, 10, 8, A0};

// Common Cathode Pins
const int display[] = {3, 12, 13, 2};

// LED On/Off states for each digit (Could add A-F later)
                    // 0  1  2  3  4  5  6  7
                    // a  b  c  d  e  f  g  dp
int layout[11][10] = {{1, 1, 1, 1, 1, 1, 0, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1, 0}, // 2
  {1, 1, 1, 1, 0, 0, 1, 0}, // 3
  {0, 1, 1, 0, 0, 1, 1, 0}, // 4
  {1, 0, 1, 1, 0, 1, 1, 0}, // 5
  {1, 0, 1, 1, 1, 1, 1, 0}, // 6
  {1, 1, 1, 0, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1, 0}, // 8
  {1, 1, 1, 1, 0, 1, 1, 0}, // 9
  {0, 0, 0, 0, 0, 0, 0, 0}, // 10 (clear)
};



// Maybe better as bytes? Shouldn't go over 9
byte dig[] = { 0, 0, 0, 0};

// Delay (ms) Between Switching between each cathode (for multiplexing)
const int dtime = 1;

// Counter to avoid updating sensor too frequently
// Dependant on dtime/cycle count
unsigned int updateCount = 0;
unsigned int updateWhen = 500;

void setup()
{

  for (byte i = 0; i <= 3; i++) {
    pinMode(display[i], OUTPUT);
    digitalWrite(display[i], HIGH);
  }

  for (int i = 5; i <= 13; i++) {
    pinMode(i, OUTPUT);
  }
  pinMode(A0, OUTPUT);

  Serial.begin(9600);
  // Initialize Temp Sensor.
  dht.begin();
}

void loop()
{
  sensorUpdate();
  for (byte i = 0; i <= 3; i++) {
    writeDigit(dig[i]);
    digitalWrite(display[i], LOW);
    delay(dtime);
    digitalWrite(display[i], HIGH);
  }
}

void writeDigit (int digit) {
  for (byte i = 0; i <= 6; i = i + 1) {
    if (layout[digit][i] == 1) {
      digitalWrite(mySeven[i], HIGH);
    } else {
      digitalWrite(mySeven[i], LOW);
    }
  }
}

void dpoint (bool on) {
  if (on == true) {
    digitalWrite(mySeven[7], HIGH);
  } else {
    digitalWrite(mySeven[7], LOW);
  }
}


void sensorUpdate () // Is there any way to make this faster to avoid flicker?
{
  if (updateCount < updateWhen) {
    updateCount++;
    return;
  }
  
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    int tempF = round((event.temperature * 1.8) + 32);
    dig[0] = tempF % 10;
    dig[1] = (tempF / 10) % 10;
    Serial.println(event.temperature);
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    int humdity = round(event.relative_humidity);
    dig[2] = (humdity % 10);
    dig[3] = (humdity / 10) % 10;
  }
  updateCount = 0; // Reset counter after update
}
