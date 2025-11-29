#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SHT31.h"

Adafruit_SHT31 sht30 = Adafruit_SHT31();

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Time variables
int secondNow = 0, minuteNow = 0, hourNow = 0;
int dayNow = 1, monthNow = 1, yearNow = 2025;

// Number of days in each month
const int daysInMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

unsigned long previousMillis = 0;
const unsigned long interval = 1000; 

void setup() {
  Serial.begin(115200);
  Serial.println("OLED + SHT30 + Clock test...");

  Wire.begin(12, 13);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while (1);
  }
  display.setRotation(2);

  // Initialize SHT30
  if (!sht30.begin(0x44)) {
    Serial.println("SHT30 not found!");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);

  Serial.println("Waiting for time data from Python...");
}

void loop() {
  // --- Receive time data from Python (once) ---
  if (Serial.available() >= 6) {
    hourNow   = Serial.read();
    minuteNow = Serial.read();
    secondNow = Serial.read();
    dayNow    = Serial.read();
    monthNow  = Serial.read();
    yearNow   = Serial.read() + 2000; // Python sends only the last 2 digits
    Serial.println("Time data received from Python.");
  }

  // --- Increase time based on millis() ---
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    secondNow++;
    if (secondNow >= 60) { secondNow = 0; minuteNow++; }
    if (minuteNow >= 60) { minuteNow = 0; hourNow++; }
    if (hourNow >= 24) { hourNow = 0; dayNow++; }

    // Handle date increment
    int dim = daysInMonth[monthNow - 1];
    if (dayNow > dim) {
      dayNow = 1;
      monthNow++;
      if (monthNow > 12) {
        monthNow = 1;
        yearNow++;
      }
    }
  }

  // --- Read sensor data ---
  float temp = sht30.readTemperature();
  float hum  = sht30.readHumidity();

  display.clearDisplay();
  display.setCursor(0, 0);

  if (!isnan(temp) && !isnan(hum)) {
    display.print(temp, 1); display.println(" C");
    display.print(hum, 1); display.println(" %");
  } else {
    display.println("Cannot read SHT30!");
  }

  display.printf("%02d:%02d:%02d\n", hourNow, minuteNow, secondNow);
  display.printf("%02d/%02d/%04d\n", dayNow, monthNow, yearNow);

  display.display();

  delay(1000); // small delay to keep loop fast, millis() handles the clock timing
}