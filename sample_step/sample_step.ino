#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

#define BLE_DEVICE_NAME                         "Anklet Sample"
#define BLE_LOCAL_NAME                          "Anklet Sample (Nano 33 BLE)"
#define BLE_UUID_CHAR                           "315f50e2-55c9-4b10-8b46-6c66957b4d98"
#define BLE_UUID_ANKLET_SAMPLE                  "2a675dfb-a1b0-4c11-9ad1-031a84594196" //"1e0f9d07-42fe-4b48-b405-38374e5f2d97"

BLEService Arduino_measurements(BLE_UUID_ANKLET_SAMPLE);
BLECharCharacteristic step_sample(BLE_UUID_CHAR, BLERead|BLENotify);
bool setupBleMode();
void bleTask();
void bleConnectHandler(BLEDevice central);
void bleDisconnectHandler(BLEDevice central);

void setup() {
  Serial.begin(9600); 
  Serial.println(F("started program"));
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
  bleTask();
}

bool setupBleMode()
{
  if (!BLE.begin())
  {
    return false;
  }

  //set device and local name 
  BLE.setDeviceName(BLE_DEVICE_NAME);
  BLE.setLocalName(BLE_LOCAL_NAME);

  //set serivce 
  BLE.setAdvertisedService(Arduino_measurements);

  //add characteristics and service
  //Timestamp
  /*
  Arduino_timestamp.addCharacteristic(dateTimeCharacteristic);
  Arduino_timestamp.addCharacteristic(millisecondsCharacteristic);
  BLE.addService(Arduino_timestamp);
  */
  //Sensor Data
  Arduino_measurements.addCharacteristic(step_sample);
  BLE.addService(Arduino_measurements);


  // set the initial value for the characeristics
  //dateTimeCharacteristic.writeValue(dateTimeData.bytes, sizeof dateTimeData.bytes);  

  //set BLE event handlers; this is like switch-case in c++
  BLE.setEventHandler(BLEConnected, bleConnectHandler);
  BLE.setEventHandler(BLEDisconnected, bleDisconnectHandler);

  //set service and characteristic specific event handlers
  //dateTimeCharacteristic.setEventHandler(BLEWritten, DateTimeWrittenHandler);

  //start advertising
  BLE.advertise();
  return true;
}

void bleConnectHandler(BLEDevice central)
{
  Serial.print(F("Connected to central: "));
  Serial.println(central.address());
}

void bleDisconnectHandler(BLEDevice central)
{
  Serial.print(F("Disconnected from central: "));
  Serial.println(central.address());
}


void bleTask()
{
  step_sample.writeValue(1);
  Serial.print(F(step_sample.value()));
  BLE.poll();
  delay(500);
  step_sample.writeValue(2);
  Serial.print(F(step_sample.value()));
  BLE.poll();
  delay(500);
  step_sample.writeValue(3);
  Serial.print(F(step_sample.value()));
  BLE.poll();
  delay(500);
  step_sample.writeValue(4);
  Serial.print(F(step_sample.value()));
  BLE.poll();
  delay(500);
}