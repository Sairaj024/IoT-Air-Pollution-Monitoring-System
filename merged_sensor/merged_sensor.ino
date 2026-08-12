/*
 * Integrated Sensor System
 * Sensors: MQ135 (Gas), DHT11 (Temp/Hum), GPS (TinyGPS++), PM2.5 (Dust)
 * Buzzer: Sounds when PM2.5 levels are high.
 */

#include "DHT.h"
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// --- PIN DEFINITIONS ---
#define MQ135_PIN A0
#define DHTPIN D1       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
#define PIN_PM25 D5     // PM2.5 sensor pin (Changed to D5 as requested)
#define PIN_PM1 D6      // PM1 sensor pin (Changed to D6 as requested)
#define GPS_RX D7       // Connect GPS TX to D7
#define GPS_TX D8       // Connect GPS RX to D8
#define BUZZER_PIN D2   // Buzzer connected to D2 (GPIO 4)

// --- CONSTANTS & CONFIG ---
const uint32_t GPSBaud = 9600;
const unsigned long sampleTime = 5000; // PM sample time (5 seconds)
const float PM25_HIGH_THRESHOLD = 2.5; // Alert when PM2.5 > 2.50 mg/m3
const int GAS_HIGH_THRESHOLD = 300;     // Alert when MQ135 > 300

// --- WIFI CONFIG (Connect to your Hotspot) ---
const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";

// --- SENSOR OBJECTS ---
DHT dht(DHTPIN, DHTTYPE);
TinyGPSPlus gps;
SoftwareSerial ss(GPS_RX, GPS_TX);
ESP8266WebServer server(80);

// --- GLOBAL VARIABLES ---
unsigned long pmStartTime = 0;
float lowPM = 0;
unsigned long lastDHTRead = 0;
const unsigned long dhtInterval = 2000; // Read DHT every 2 seconds

// --- LATEST SENSOR DATA ---
float currentTemp = 0;
float currentHum = 0;
float currentPM25 = 0;
int currentGas = 0;
double currentLat = 0;
double currentLng = 0;
int currentAQI = 0;

// --- HELPER FUNCTIONS (Updated per request) ---
float calc_low_ratio(float lowPulse) {
  return (lowPulse / sampleTime) * 100.0f;
}

float calc_c_ugm3(float lowPulse) {
  float r = calc_low_ratio(lowPulse);
  // Linear mapping for DSM501A/PPD42NS sensors
  // Ratio * 5.0 gives a more realistic ug/m3 for indoor/outdoor environments
  float c_ugm3 = r * 5.0f; 
  if (c_ugm3 < 0) c_ugm3 = 0;
  return c_ugm3;
}

float calc_c_pcs283ml(float lowPulse) {
  float r = calc_low_ratio(lowPulse);
  float c_pcs283ml = 625.0f * r;
  if (c_pcs283ml > 12500.0f) c_pcs283ml = 12500.0f; // Limit to max
  return c_pcs283ml;
}

// EPA AQI Calculation for PM2.5 (24hr standard)
int calculateAQI(float pm25) {
  if (pm25 < 0) return 0;
  if (pm25 <= 12.0) return (50.0/12.0) * pm25;
  if (pm25 <= 35.4) return ((100.0-51.0)/(35.4-12.1)) * (pm25-12.1) + 51;
  if (pm25 <= 55.4) return ((150.0-101.0)/(55.4-35.5)) * (pm25-35.5) + 101;
  if (pm25 <= 150.4) return ((200.0-151.0)/(150.4-55.5)) * (pm25-55.5) + 151;
  if (pm25 <= 250.4) return ((300.0-201.0)/(250.4-150.5)) * (pm25-150.5) + 201;
  if (pm25 <= 350.4) return ((400.0-301.0)/(350.4-250.5)) * (pm25-250.5) + 301;
  return ((500.0-401.0)/(500.4-350.5)) * (pm25-350.5) + 401;
}

// --- WEB SERVER HANDLERS ---
void handleData() {
  String json = "{";
  json += "\"temp\":" + String(currentTemp) + ",";
  json += "\"hum\":" + String(currentHum) + ",";
  json += "\"pm25\":" + String(currentPM25) + ",";
  json += "\"aqi\":" + String(currentAQI) + ",";
  json += "\"gas\":" + String(currentGas) + ",";
  json += "\"lat\":" + String(currentLat, 6) + ",";
  json += "\"lng\":" + String(currentLng, 6);
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  // Common Serial Monitor
  Serial.begin(115200);
  delay(100);
  
  // GPS Serial
  ss.begin(GPSBaud);
  
  // DHT Init
  dht.begin();
  
  // PM Sensor Pins
  pinMode(PIN_PM25, INPUT);
  pinMode(PIN_PM1, INPUT);
  
  // Buzzer Pin & Startup Test
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH); // Test Beep ON
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);  // Test Beep OFF

  // WiFi Connection Setup
  WiFi.begin(ssid, password);
  Serial.print(F("Connecting to WiFi..."));
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F("\nWiFi Connected!"));
  Serial.print(F("IP Address: ")); Serial.println(WiFi.localIP());

  // Web Server Routes
  server.on("/data", handleData);
  server.begin();
  Serial.println(F("Web server started at /data"));

  Serial.println(F("\n--- Integrated Sensor System Started ---"));
  Serial.println(F("Warming up PM sensor..."));
  pmStartTime = millis();
}

void loop() {
  yield(); // Keep ESP8266 background tasks running

  // 1. GPS READING (Polling SoftwareSerial)
  while (ss.available() > 0) {
    gps.encode(ss.read());
    if (gps.location.isUpdated()) {
      currentLat = gps.location.lat();
      currentLng = gps.location.lng();
      Serial.print(F("GPS >> Lat: ")); Serial.print(currentLat, 6);
      Serial.print(F(", Lng: ")); Serial.print(currentLng, 6);
      Serial.print(F(", Sats: ")); Serial.println(gps.satellites.value());
    }
  }

  server.handleClient(); // Handle web requests

  // 2. PM SENSOR READING (Non-blocking)
  static unsigned long lastPMSample = 0;
  if (micros() - lastPMSample >= 1000) { // Sample every 1ms without using delay()
    lastPMSample = micros();
    if (digitalRead(PIN_PM25) == LOW) {
      lowPM += 1; // Accumulate milliseconds of LOW pulse
    }
  }

  // DEBUG: Check if we are receiving ANY data from GPS
  static unsigned long lastGPSCheck = 0;
  if (millis() - lastGPSCheck > 5000) {
      lastGPSCheck = millis();
      if (ss.available() == 0 && gps.charsProcessed() == 0) {
          Serial.println(F("GPS >> Warning: No data received from GPS module. Check wiring!"));
      } else {
          Serial.print(F("GPS >> Data received: ")); Serial.print(gps.charsProcessed()); Serial.println(F(" chars"));
      }
  }

  // 3. PERIODIC TASKS (PM, MQ135, DHT)
  unsigned long currentMillis = millis();

  // Check if PM sample time (5s) is complete
  if (currentMillis - pmStartTime >= sampleTime) {
    // Calculate calibrated ug/m3 directly
    currentPM25 = calc_c_ugm3(lowPM);
    
    currentAQI = calculateAQI(currentPM25);
    
    Serial.println(F("\n------ AIR QUALITY ------"));
    Serial.print(F("PM2.5: ")); Serial.print(currentPM25); Serial.println(F(" ug/m3"));
    Serial.print(F("AQI Index: ")); Serial.println(currentAQI);
    Serial.print(F("PM Ratio: ")); Serial.print(calc_low_ratio(lowPM)); Serial.println(F("%"));
    Serial.print(F("Particles (pcs/283ml): ")); Serial.println(calc_c_pcs283ml(lowPM));
    
    // Buzzer Control (Trigger on AQI > 100 OR High Gas)
    if (currentAQI > 100 || currentGas > GAS_HIGH_THRESHOLD) {
      digitalWrite(BUZZER_PIN, HIGH);
      Serial.println(F("!!! WARNING: HIGH POLLUTION DETECTED (Buzzer ON) !!!"));
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }

    // MQ135 Reading
    currentGas = analogRead(MQ135_PIN);
    Serial.print(F("Gas (MQ135): ")); Serial.println(currentGas);
    Serial.println(F("------------------------\n"));

    lowPM = 0;
    pmStartTime = currentMillis;
  }

  // DHT Reading (every 2 seconds)
  if (currentMillis - lastDHTRead >= dhtInterval) {
    lastDHTRead = currentMillis;
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (!isnan(h) && !isnan(t)) {
      currentTemp = t;
      currentHum = h;
      Serial.print(F("DHT >> Humidity: ")); Serial.print(h);
      Serial.print(F("%, Temp: ")); Serial.print(t); Serial.println(F("°C"));
    } else {
      Serial.println(F("DHT >> Sensor error!"));
    }
  }

  // GPS Wiring Check
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    static bool gpsErrorOnce = false;
    if (!gpsErrorOnce) {
      Serial.println(F("GPS >> No GPS detected. Check D7/D8."));
      gpsErrorOnce = true;
    }
  }
}
