#define BLYNK_TEMPLATE_ID "TMPL4mPhgtRSv"
#define BLYNK_TEMPLATE_NAME "IoT Plant Monitoring"

#include <WiFiNINA.h>
#include <BlynkSimpleWiFiNINA.h>
#include <Wire.h>
#include "DFRobot_PH.h"
#include <EEPROM.h>

// Moisture 
int moistureData = A0;
int sensorValue = 0;
int percent = 0;

#define PH_data A1
float voltagePH, phValue, temperature = 25;
DFRobot_PH ph;

float voltagePH_4 = 1967.77; // voltage at pH 4.0
float voltagePH_7 = 1459.96; // voltage at pH 7.0
float m = (4.0 - 7.0) / (voltagePH_4 - voltagePH_7); // Slope
float b = 7.0 - m * voltagePH_7; // Intercept

// WiFi credentials
char ssid[] = "TP-Link_88EA"; 
char pass[] = "29175529";     

// Cloud credentials
char auth[] = "GrDoMrKxwAj2hc-FzF-amXeZcJtQ20QK"; 

void setup() {
  
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }

  // connecting to WiFi network
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // Failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  // Initialize cloud
  Blynk.begin(auth, ssid, pass);
  Serial.println("Blynk is connected");

  ph.begin();
  Serial.println("pH Sensor is ready.");
}

void loop() {
  
  sensorValue = analogRead(moistureData);
  percent = map(sensorValue, 0, 1023, 0, 100); // Map the sensor value to a percentage (assuming 0-1023 range)

 
  Serial.print("Moisture Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(", Moisture Percent: ");
  Serial.println(percent);

  // Sending moisture value to cloud
  Blynk.virtualWrite(V3, percent); //  virtual pin for the moisture sensor

  // Read PH sensor value
  static unsigned long timepoint = millis();
  if (millis() - timepoint > 5000U) {
    timepoint = millis();
    voltagePH = analogRead(PH_data) / 1024.0 * 5000; // reading pH voltage
    Serial.print("Voltage: ");
    Serial.println(voltagePH);

    phValue = m * voltagePH + b;

    if (isnan(phValue)) {
      Serial.println("Error: pH calculation resulted in NaN.");
    } else {
      Serial.print("PH: ");
      Serial.println(phValue, 1);

      // Sending pH value to cloud
      Blynk.virtualWrite(V2, phValue); //virtual pin for the pH sensor
    }
  }

  // cloud
  Blynk.run();

  // Wait before reading again
  delay(1000);
}

void calibration() {
  Serial.println("Enter Calibration Mode");
  while (1) {
    if (Serial.available() > 0) {
      char cal_cmd = Serial.read();
      if (cal_cmd == '4' || cal_cmd == '7' || cal_cmd == '1') {
        float voltage = analogRead(PH_data) * 5.0 / 1024.0; // analog reading to voltage in V
        int ph_val = (cal_cmd == '1') ? 10 : cal_cmd - '0'; // Convert '4', '7', '1' to 4, 7, 10
        ph.calibration(voltage, temperature); 
        Serial.print("Calibrated at pH ");
        Serial.println(ph_val);
      } else if (cal_cmd == 'e') {
        Serial.println("Exit Calibration Mode");
        break;
      }
    }
  }
}
