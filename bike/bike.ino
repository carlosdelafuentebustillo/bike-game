#include <ArduinoBLE.h>

BLEService bikeService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service
// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic hrCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead);
BLEByteCharacteristic speedCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead);
BLEByteCharacteristic resistanceCharacteristic("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);



int odometer = 15;    // Analog pin 1 digital pin 15 first jack
int led = LED_BUILTIN;
const int AVG = 4;    // Number of reads necessary to count as a pulse
int counter = 0; unsigned long startTime; unsigned long runTime; unsigned long duration;
int led_delay = 0;
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(odometer, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("BIKE");
  BLE.setAdvertisedService(bikeService);

  // add the characteristic to the service
  bikeService.addCharacteristic(hrCharacteristic);
  bikeService.addCharacteristic(speedCharacteristic);
  bikeService.addCharacteristic(resistanceCharacteristic);

  // add service
  BLE.addService(bikeService);
  // set the initial value for the characeristic:
  hrCharacteristic.writeValue(0);
  speedCharacteristic.writeValue(0);
  resistanceCharacteristic.writeValue(0);
  // start advertising
  BLE.advertise();
  Serial.println("BLE Bike Peripheral");
}

// the loop routine runs over and over again forever:
void loop() {
  BLEDevice central = BLE.central();
  if (central) {
    Serial.print(F("Connected to central: "));
    Serial.println(central.address());
    // while the central is still connected to peripheral:
    while (central.connected()) {
      if(led_delay == 1000){digitalWrite(led, LOW);}
      else{led_delay++;}
      // read the input pin:
      int odometer_state = digitalRead(odometer);
      if (!digitalRead(odometer)){counter++;}   // if pulse read increase counter
      else{counter = 0;}
      if(counter==AVG){
        digitalWrite(led, HIGH);
        led_delay = 0;
        runTime = millis();
        duration = runTime - startTime;
        startTime = millis();
        if(duration < 2550){    
          Serial.println(duration);
          speedCharacteristic.writeValue(duration/10); // Send cents of a second
        }
      }
    }
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }    
}
