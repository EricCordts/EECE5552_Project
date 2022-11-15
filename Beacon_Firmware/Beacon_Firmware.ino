// EECE5552 Beacon Firmware Program 
#include <ArduinoBLE.h>

//need mbed.h for clock functions
#include <mbed.h>

//define UUID
#define BLE_UUID_BEACON_SERVICE_DEVICE1           "2a675dfb-a1b0-4c11-9ad1-031a84594196"
#define BLE_UUID_BEACON_SERVICE_DEVICE2           "ae7a527a-64f7-11ed-9022-0242ac120002"
#define BLE_UUID_BEACON_SERVICE_DEVICE3           "b52a20c8-64f7-11ed-9022-0242ac120002"
#define BLE_UUID_BEACON_SERVICE_DEVICE4           "ba5948e4-64f7-11ed-9022-0242ac120002"

#define BLE_DEVICE_NAME                           "Arduino Nano 33 BLE"
#define BLE_LOCAL_NAME                            "Arduino 1 (Nano 33 BLE)"
#define BLE_LED_PIN                               LED_BUILTIN

//Measurement Characteristics
BLEService Beacon_Service(BLE_UUID_BEACON_SERVICE_DEVICE1);

//function declarations:
bool setupBleMode();
void bleTask();
void DateTimeWrittenHandler(BLEDevice central, BLECharacteristic bleCharacteristic);
void bleConnectHandler(BLEDevice central);
void bleDisconnectHandler(BLEDevice central);
void timeTask();
void initializeClock(void);

bool timeUpdated = false;

void setup() {
  Serial.begin(9600); 
  //while(!Serial);
  
  // Pin Outputs 
  pinMode(LED_BUILTIN, OUTPUT);

  // Check Initialization of BLE service
  if (!setupBleMode())
  {
    Serial.println(F("Failed to initialize BLE"));
    while (1);
  }
  else
  {
    Serial.println(F("BLE initialized. Waiting for client connection"));
  }
}

void loop() {
  Serial.println(F("In Loop"));
  bleTask();
}

bool setupBleMode()
{
  if (!BLE.begin())
  {
    return false;
  }

  Serial.println(F("Set device and local name"));
  //set device and local name 
  BLE.setDeviceName(BLE_DEVICE_NAME);
  BLE.setLocalName(BLE_LOCAL_NAME);

  Serial.println(F("Set service"));
  //set serivce 
  BLE.setAdvertisedService(Beacon_Service);
  BLE.addService(Beacon_Service);

  //set BLE event handlers; this is like switch-case in c++
  BLE.setEventHandler(BLEConnected, bleConnectHandler);
  BLE.setEventHandler(BLEDisconnected, bleDisconnectHandler);

  //start advertising
  Serial.println(F("Advertising"));
  BLE.advertise();
  return true;
}

void bleConnectHandler(BLEDevice central)
{
  digitalWrite(BLE_LED_PIN, HIGH);
  Serial.print(F("Connected to central: "));
  Serial.println(central.address());
}

void bleDisconnectHandler(BLEDevice central)
{
  digitalWrite (BLE_LED_PIN, LOW);
  Serial.print(F("Disconnected from central: "));
  Serial.println(central.address());
}

void bleTask()
{
}
