/*
  ESP32 SHTC3 Temperature & Humidity Sensor with Sensesiot Platform
  Using Adafruit SHTC3 Library - Send to Sensesiot only
  Data will be pulled by n8n via MQTT for AI processing
  Created: June 2025
*/

#include <Sensesiot.h>
#include <Wire.h>
#include <Adafruit_SHTC3.h>
#include <Adafruit_Sensor.h>

// Configuration
const char key[] = "qwCOowlTnOYNSLVmZVMW";
const char wifissid[] = "Maker Hub_2.4G";
const char wifipw[] = "Makerhub69";

// Initialize
SensesiotClient sensesProtocol(key);
Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();

// Variables
float temperature = 0.0;
float humidity = 0.0;
unsigned long lastReading = 0;
const unsigned long readingInterval = 3000; // 2 seconds

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 SHTC3 Sensesiot Starting...");
  
  // Initialize I2C
  Wire.begin();
  
  // Initialize SHTC3 sensor
  if (!shtc3.begin()) {
    Serial.println("SHTC3 sensor initialization failed!");
    Serial.println("Check wiring and I2C address");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("SHTC3 sensor initialized successfully");
  
  // Connect to Sensesiot
  sensesProtocol.begin(wifissid, wifipw);
  sensesProtocol.waitUntilReady();
  Serial.println("Connected to Sensesiot Platform");
  
  Serial.println("=== Device Information ===");
  Serial.println("Device Key: " + String(key));
  Serial.println("Data will be available for n8n via MQTT:");
  Serial.println("  Temperature: data/" + String(key) + "/1");
  Serial.println("  Humidity: data/" + String(key) + "/2");
  Serial.println("=============================");
}

void loop() {
  // Check connection
  if (!sensesProtocol.ready()) {
    Serial.println("Disconnected from Sensesiot");
    sensesProtocol.begin(wifissid, wifipw);
    sensesProtocol.waitUntilReady();
    Serial.println("Reconnected to Sensesiot");
  }
  
  sensesProtocol.loop();
  
  // Read sensor data
  if (millis() - lastReading >= readingInterval) {
    if (readSHTC3Data()) {
      // Send data to Sensesiot
      sensesProtocol.setData(1, temperature);  // Channel 1: Temperature
      sensesProtocol.setData(2, humidity);     // Channel 2: Humidity
      
      // Display readings
      Serial.println("=== Sensor Reading ===");
      Serial.println("Temperature: " + String(temperature, 2) + " °C");
      Serial.println("Humidity: " + String(humidity, 2) + " %");
      Serial.println("Data sent to Sensesiot");
      Serial.println("Available for n8n via MQTT");
      Serial.println();
      
      lastReading = millis();
    } else {
      Serial.println("Failed to read SHTC3 sensor!");
    }
  }
  
  delay(100); // Small delay for stability
}

bool readSHTC3Data() {
  sensors_event_t humidity_event, temp_event;
  
  if (shtc3.getEvent(&humidity_event, &temp_event)) {
    temperature = temp_event.temperature;
    humidity = humidity_event.relative_humidity;
    return true;
  } else {
    Serial.println("SHTC3 read failed!");
    return false;
  }
}

// Optional: Function to check sensor status
void printSensorInfo() {
  Serial.println("=== SHTC3 Sensor Info ===");
  Serial.println("Sensor: Adafruit SHTC3");
  Serial.println("Status: Connected");
  Serial.println("========================");
}