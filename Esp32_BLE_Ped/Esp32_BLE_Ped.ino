/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    
    Modified 1/2019 by Larry Bonnette to add walking quadreped code and
    Changed service and charateristic UUID to DF01, DF02, and DF03 to make it easier to play with.
    
12 DOF Quad code from:
*** 2018-09-30
*** Modified by V.Palleschi from the original by Salvatore Fancello,
*** http://www.progettiarduino.com
*** e-mail: salvatorefancello89@gmail.com
*** e-mail: vpalleschi@gmail.com
*** https://www.thingiverse.com/thing:3119066

   Bluetooth Setup:
   Create a BLE server that, once we receive a connection, will send periodic notifications back to the iPhone.
   The service advertises itself as: DF01
   Has a characteristic of: DF02 - used for receiving data with "WRITE" 
   Has a characteristic of: DF03 - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second. 
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN 180 //
#define SERVOMAX 420 //
#define SLOW 50 //
#define FAST 10 //

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
bool ledState = false;
int ledChanged = 2; 

char rcvdata;

const int LED = 2; // Could be different depending on the dev board. I used the DOIT ESP32 dev board.

//std::string rxValue; // Could also make this a global var to access it in loop()

#define SERVICE_UUID           "DF01" // UART service UUID
#define CHARACTERISTIC_UUID_RX "DF02"
#define CHARACTERISTIC_UUID_TX "DF03"
 
void servocontrol(int n, int a) // n= servo number a= angle
{
if (n > 5){
a = map(a, 180, 0, SERVOMIN, SERVOMAX);
pwm.setPWM(n, 0, a);} //was n-6
else {
a = map(a, 0, 180, SERVOMIN, SERVOMAX);
pwm.setPWM(n, 0, a);}
}

// n = leg number, an = coxa servo (sits on the body), f = femer servo (one servo out from the body), t = tibia servo (last servo out from the body)
// example: Leg #1  an = i2c channel 0, f = i2c channel 1, t = i2c channel 2
// moveleg converts each leg segment into a channel number, and the angle. It then sends it to servocontrol(servo number (I2C channel) , angle)

void moveleg(int n, int an, int f, int t, int w)
{
yield(); // This is here to allow the use of an ESP8266
if (t >=0) {servocontrol(3*(n-1)+2, t);}; // move the tibia first
delay(w);
if (f >=0) {servocontrol(3*(n-1)+1, f);}; // Then move the femer
delay(w);
if (an >=0) {servocontrol(3*(n-1), an);}; // and lastly move the coxa (body servo)
delay(w);
}

// n = leg number
void stp(int n, int a, int w)
{
yield();

 servocontrol(3*(n-1)+1, 140); 
 delay(w);
 servocontrol(3*(n-1)+2, 40);
 delay(w);
 servocontrol(3*(n-1), a);
 delay(w);
 servocontrol(3*(n-1)+2, 90);
 delay(w);
 servocontrol(3*(n-1)+1, 90);

}

// n = leg number, a = body servo (coxa) angle
void mov(int n, int a, int w)
{
moveleg(n,a,-1,-1, w);
}

void iniz()
{
moveleg(1,90,90,90, FAST);
moveleg(2,90,90,90, FAST);
moveleg(3,90,90,90, FAST);
moveleg(4,90,90,90, FAST);
}

void stand()
{
moveleg(1,-1,50,80, FAST);
moveleg(2,-1,50,80, FAST);
moveleg(3,-1,35,80, FAST);
moveleg(4,-1,50,80, FAST);
}

void squat()
{
moveleg(1,-1,130,100, FAST);
moveleg(2,-1,130,100, FAST);
moveleg(3,-1,130,100, FAST);
moveleg(4,-1,130,100, FAST);
}

void wave_r(){

for (int j=0; j< 3; j++){
moveleg(4,-1,160,0, SLOW);
for (int i=30; i <= 110; i++){
moveleg(4,i,-1,-1, FAST);
}
for (int i=110; i >= 20; i--){
moveleg(4,i,-1,-1, FAST);
}
}
moveleg(4,90,90,90, SLOW);
}

void wave_l(){
for (int j=0; j< 3; j++){
moveleg(1,-1,160,0, SLOW);
for (int i=30; i <= 110; i++){
moveleg(1,i,-1,-1, FAST);
}
for (int i=110; i >= 20; i--){
moveleg(1,i,-1,-1, FAST);
}
}
moveleg(1,90,90,90, SLOW);
}

void skew_r(){
moveleg(1,20,-1,-1, FAST);
moveleg(2,20,-1,-1, FAST);
moveleg(3,160,-1,-1, FAST);
moveleg(4,160,-1,-1, FAST);
}
void skew_l(){
moveleg(1,160,-1,-1, FAST);
moveleg(2,160,-1,-1, FAST);
moveleg(3,20,-1,-1, FAST);
moveleg(4,20,-1,-1, FAST);
}
void courtsy(){
moveleg(1,-1,140,80, FAST);
moveleg(4,-1,140,80, FAST);
moveleg(2,-1,40,80, FAST);
moveleg(3,-1,40,80, FAST);

}
void prepare_jump(){
moveleg(1,-1,40,80, FAST);
moveleg(4,-1,40,80, FAST);
moveleg(2,-1,140,80, FAST);
moveleg(3,-1,140,80, FAST);
}

void rotate_r(){
yield();
Serial.println("In rot R Loop");
stp(1,160, SLOW);
stp(2,160, SLOW);
stp(3,20, SLOW);
stp(4,20, SLOW);
delay(50);
mov(4,90, FAST);
mov(3,90, FAST);
mov(2,90, FAST);
mov(1,90, FAST);
}

void rotate_l(){
yield();
Serial.println("In rot L Loop");
stp(1,20, SLOW);
stp(2,20, SLOW);
stp(3,160, SLOW);
stp(4,160, SLOW);
delay(50);
mov(1,90, FAST);
mov(2,90, FAST);
mov(3,90, FAST);
mov(4,90, FAST);
}

void forward(){
yield();
Serial.println("In Forward Loop");
mov(4,50, FAST);
mov(1,50, FAST);
mov(3,50, FAST);
mov(2,50, FAST);
stp(1,130, SLOW);
stp(4,130, SLOW);
stp(2,130, SLOW);
stp(3,130, SLOW);
delay(SLOW);
iniz();
}

void backward(){
yield();
Serial.println("In Backward Loop");
mov(2,130, FAST);
mov(3,130, FAST);
mov(1,130, FAST);
mov(4,130, FAST);
stp(3,50, SLOW);
stp(2,50, SLOW);
stp(4,50, SLOW);
stp(1,50, SLOW);
delay(SLOW);
iniz();
}
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    // On first connect send led status to iPhone
    if (ledState == false){
    pCharacteristic->setValue("X"); // Sending OFF message to iPhone
    pCharacteristic->notify(); // Send the value to the iPhone!
    }
    if (ledState == true){
    pCharacteristic->setValue("N"); // Sending On message to iPhone
    pCharacteristic->notify(); // Send the value to the iPhone!
    }
   };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");

        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
          rcvdata = rxValue[i]; // Place data comming from iPhone in global
        }
      }
    }
};


void setup() {

pwm.begin();
pwm1.begin();
pwm1.setPWMFreq(60); // Analog servos run at ~60 Hz updates
pwm.setPWMFreq(60); // Analog servos run at ~60 Hz updates

iniz();
  
  Serial.begin(115200);

  pinMode(LED, OUTPUT);

  // Create the BLE Device
  BLEDevice::init("ESP"); // Give it a name

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
                      
  pCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}




void loop() {
  if (deviceConnected) {

switch (rcvdata)
{
       case 'A': 
     {
        Serial.println("LOOP Case A");
        forward();
        break;
     }
       case 'B': 
     {
        Serial.println("LOOP Case B");
        backward();
        break;
     }
      case 'G': 
     {
        Serial.println("LOOP Case G");
        rotate_l();
        break;
     }
       case 'F': 
     {
        Serial.println("LOOP Case F");
        rotate_r();
        break;
     }
       case 'E': 
     {
        Serial.println("LOOP Case E");
        iniz();
        break;
     }
        case 's': 
     {
        Serial.println("LOOP Case s");
        stand();
        break;
     }
       case 'q': 
     {
        Serial.println("LOOP Case q");
        squat();
        break;
     }
       case 'l': 
     {
        Serial.println("LOOP Case l");
        wave_l();
        break;
     }
       case 'r': 
     {
        Serial.println("LOOP Case r");
        wave_r();
        break;
     }
  }

  }

}
