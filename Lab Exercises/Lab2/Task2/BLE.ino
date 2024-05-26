#include <ArduinoBLE.h>

// LED pin definitions
const int redLEDPin = 2;
const int yellowLEDPin = 3;
const int greenLEDPin = 4;

// BLE service and characteristic
BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEByteCharacteristic redLEDCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic yellowLEDCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic greenLEDCharacteristic("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial);

  // Initialize LED pins
  pinMode(redLEDPin, OUTPUT);
  pinMode(yellowLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);

  // Begin BLE initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  // Set up BLE service
  BLE.setLocalName("LED Control");
  BLE.setAdvertisedService(ledService);

  // Add characteristics to service
  ledService.addCharacteristic(redLEDCharacteristic);
  ledService.addCharacteristic(yellowLEDCharacteristic);
  ledService.addCharacteristic(greenLEDCharacteristic);

  // Add service
  BLE.addService(ledService);

  // Set initial values for characteristics
  redLEDCharacteristic.writeValue(0);
  yellowLEDCharacteristic.writeValue(0);
  greenLEDCharacteristic.writeValue(0);

  // Start advertising
  BLE.advertise();

  Serial.println("BLE LED Control is now active");
}

void loop() {
  // Listen for BLE devices
  BLEDevice central = BLE.central();

  // If a central is connected to the peripheral
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    // While the central is connected
    while (central.connected()) {
      // Check if the characteristics were written to
      if (redLEDCharacteristic.written()) {
        int redValue = redLEDCharacteristic.value();
        digitalWrite(redLEDPin, redValue);
        Serial.print("Red LED: ");
        Serial.println(redValue);
      }
      if (yellowLEDCharacteristic.written()) {
        int yellowValue = yellowLEDCharacteristic.value();
        digitalWrite(yellowLEDPin, yellowValue);
        Serial.print("Yellow LED: ");
        Serial.println(yellowValue);
      }
      if (greenLEDCharacteristic.written()) {
        int greenValue = greenLEDCharacteristic.value();
        digitalWrite(greenLEDPin, greenValue);
        Serial.print("Green LED: ");
        Serial.println(greenValue);
      }
    }

    // When the central disconnects
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
