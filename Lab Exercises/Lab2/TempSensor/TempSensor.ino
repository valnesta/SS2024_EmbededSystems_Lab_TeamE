#include <SPI.h>
#include <WiFiNINA.h>
#include <DHT.h>

#define DHTPIN 7
//#define sensorPin 7

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

char ssid[] = "TP-Link_88EA";
char pass[] = "29175529";
int keyIndex = 0;

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  Serial.begin(9600);
//We try to connect to a network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  //we try to start the server service
  server.begin();

  Serial.print("SSID: ");                   //Print the ssid
  Serial.println(WiFi.SSID());              //Print what the ssid really is
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");             //Print the IP address
  Serial.println(ip);
}

void loop() {

  //int reading = analogRead(sensorPin);
  //float voltage = reading * 5.0;
  //voltage /= 1024.0;
  //float temperatureC = (voltage - 0.5) * 100 ;
  //float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;

  float humidity = dht.readHumidity();
  float temperatureC = dht.readTemperature();
  float temperatureF = dht.readTemperature(true);


  if (isnan(humidity) || isnan(temperatureC) || isnan(temperatureF)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

   // Compute heat index in Fahrenheit (the default)
  float heatIndexF = dht.computeHeatIndex(temperatureF, humidity);
  // Compute heat index in Celsius
  float heatIndexC = dht.computeHeatIndex(temperatureC, humidity, false);

  // Print the results to the serial monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.print(" °C ");
  Serial.print(temperatureF);
  Serial.print(" °F\t");
  Serial.print("Heat index: ");
  Serial.print(heatIndexC);
  Serial.print(" °C ");
  Serial.print(heatIndexF);
  Serial.println(" °F");












  WiFiClient client = server.available();   
  if (client) {                             
    Serial.println("new client");           
    String currentLine = "";                
    while (client.connected()) {            
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        if (c == '\n') {                    

          if (currentLine.length() == 0) {

            client.print("Temperature: ");
            client.println(temperatureF);

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    client.stop();
    Serial.println("client disonnected");
  }
}