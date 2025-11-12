#include <ESP8266WiFi.h>
#include <ThingSpeak.h>  // Include ThingSpeak Library

#define buzzer D2

const char *thingSpeakApiKey = "4PZ2WDKN7NW1YHSS";
const int channelNumber = 2397467;
const char *ssid = "OnePlus";
const char *password = "12356789";

WiFiClient client;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  ThingSpeak.begin(client); // Initialize ThingSpeak
  
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
}

void loop() {
  int sensorValue = analogRead(A0);
  
  // Convert sensor value to PPM (example mapping)
  float ammoniaPPM = map(sensorValue, 0, 1023, 10, 300);
  float benzenePPM = map(sensorValue, 0, 1023, 10, 1000);
  float co2PPM = map(sensorValue, 0, 1023, 350, 10000);

  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);

  Serial.print(" | Ammonia (NH3) PPM: ");
  Serial.print(ammoniaPPM);

  Serial.print(" | Benzene (C6H6) PPM: ");
  Serial.print(benzenePPM);

  Serial.print(" | CO2 (Carbon Dioxide) PPM: ");
  Serial.print(co2PPM);

  // Set the fields properly
  ThingSpeak.setField(1, ammoniaPPM);  // Field 1: Ammonia
  ThingSpeak.setField(2, benzenePPM);  // Field 2: Benzene
  ThingSpeak.setField(3, co2PPM);      // Field 3: CO2

  // Air Quality checking
  if (ammoniaPPM > 250 || benzenePPM > 700 || co2PPM > 7000) {
    Serial.println(" | Air Quality: BAD ");
    digitalWrite(buzzer, HIGH);
  } else {
    Serial.println(" | Air Quality: GOOD ");
    digitalWrite(buzzer, LOW);
  }

  // Write the data to ThingSpeak
  int response = ThingSpeak.writeFields(channelNumber, thingSpeakApiKey);

  if (response == 200) {
    Serial.println("ThingSpeak update successful!");
  } else {
    Serial.println("Error updating ThingSpeak. HTTP error code " + String(response));
  }

  delay(15000); // ThingSpeak minimum delay is 15 seconds between updates
}
