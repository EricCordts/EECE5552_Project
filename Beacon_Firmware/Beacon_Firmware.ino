// Capstone Arduino Program 

#include <ArduinoBLE.h>

//need mbed.h for clock functions
#include <mbed.h>


//define UUID
#define BLE_UUID_BEACON_SERVICE                   "2a675dfb-a1b0-4c11-9ad1-031a84594196" //"1e0f9d07-42fe-4b48-b405-38374e5f2d97"
//#define BLE_UUID_SENSOR_DATA                      "d80de551-8403-4bae-9f78-4d2af89ff17b"
#define BLE_DEVICE_NAME                           "Arduino Nano 33 BLE"
#define BLE_LOCAL_NAME                            "Arduino 1 (Nano 33 BLE)"
#define BLE_LED_PIN                               LED_BUILTIN

//Measurement Characteristics
BLEService Beacon_Service(BLE_UUID_BEACON_SERVICE);
//BLECharacteristic Dummy_data(BLE_UUID_SENSOR_DATA , BLERead, sizeof accel_gyro_data.bytes);

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
/*#define BLE_UPDATE_INTERVAL 10
  read_Accel_Gyro();
  
  static uint32_t previousMillis = 0;
  uint32_t currentMillis = millis();
  //Milliseconds Value
  //millisecondsCharacteristic.writeValue(currentMillis%1000);

  if (currentMillis - previousMillis >= BLE_UPDATE_INTERVAL)
  {
    previousMillis = currentMillis;
    BLE.poll();
  }
  
  if (timeUpdated)
  {
    timeUpdated = false;
    dateTimeCharacteristic.writeValue(dateTimeData.bytes, sizeof dateTimeData.bytes);
  }*/
}

