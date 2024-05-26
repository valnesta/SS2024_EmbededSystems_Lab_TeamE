int moistPin = A0;  
int sensorValue = 0;  
int percent = 0;


  void setup() {
  Serial.begin(9600);
}

void loop() {
  
  sensorValue = analogRead(moistPin);

  percent = map(sensorValue, 0, 1023, 0, 100); // Map the sensor value to a percentage (assuming 0-1023 range)
  printValuesToSerial();
  delay(5000);


}
void printValuesToSerial() {
  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(", Moisture Percent: ");
  Serial.println(percent);
}




#include <SPI.h>
#include <WiFiNINA.h>
#include "tempAccess.h"
#include <DHT.h>

#define DHTPIN 7       // My data pin
#define DHTTYPE DHT11   // ensor type DHT 

char ssid[] = "Galaxy A71 17C1";
char pass[] = "ifmm7540";
int keyIndex = 0;

DHT dht(DHTPIN, DHTTYPE);  // Initializing DHT sensor for DHT11

int status = WL_IDLE_STATUS;
WiFiServer server(80);

// I declared the printWiFiStatus function
void printWiFiStatus();

void setup() 
{
  Serial.begin(9600);
  while (!Serial) 
  {
    ;
  }

  Serial.println("Measuring Temperature and Humidity??");

  if (WiFi.status() == WL_NO_MODULE) 
  {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  Serial.print("Current firmware version: ");
  Serial.println(fv);
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) 
  {
    Serial.println("Please upgrade to the latest firmware");
  }

  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) 
  {
    Serial.println("Creating access point failed");
    while (true);
  }

  delay(10000);

  server.begin();

  dht.begin();  //We Initialized the DHT sensor

  printWiFiStatus(); // Calling printWiFiStatus function
}

void loop() 
{
  // Read data from DHT sensor directly instead
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (status != WiFi.status()) 
  {
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) 
    {
      Serial.println("Device connected to AP");
    } 
    else 
    {
      Serial.println("Device disconnected from AP");
    }
  }
  
  WiFiClient client = server.available();

  if (client) 
  {
    Serial.println("New client");           
    String currentLine = "";                
    while (client.connected()) 
    {            
      delayMicroseconds(10);                
      if (client.available()) 
      {             
        char c = client.read();            
        Serial.write(c);                    
        if (c == '\n') 
        {                   
          if (currentLine.length() == 0) 
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // This gives Improved HTML response with proper formatting
            client.println("<html><head><title>Temperature and Humidity</title></head><body>");
            client.println("<h1>Temperature and Humidity</h1>");
            client.print("<p>Humidity (%): ");
            client.print(h, 2);
            client.println("</p>");
            client.print("<p>Temperature (C): ");
            client.print(t, 2);
            client.println("</p>");
            client.println("</body></html>");

            client.println();
            break;
          }
          else 
          {
            currentLine = "";
          }
        }
        else if (c != '\r') 
        {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }
}

// Definition of printWiFiStatus function
void printWiFiStatus()
{
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  Serial.print("Hoola! Lets go to the browser for details: ");
  Serial.println(ip);
}


