// Capstone Arduino Program 

#include <ArduinoBLE.h>

//need mbed.h for clock functions
#include <mbed.h>

#define BLE_UUID_BEACON_SERVICE_DEVICE1           "2a675dfb-a1b0-4c11-9ad1-031a84594196"
#define BLE_UUID_BEACON_SERVICE_DEVICE2           "ae7a527a-64f7-11ed-9022-0242ac120002"
#define BLE_UUID_BEACON_SERVICE_DEVICE3           "b52a20c8-64f7-11ed-9022-0242ac120002"
#define BLE_UUID_BEACON_SERVICE_DEVICE4           "ba5948e4-64f7-11ed-9022-0242ac120002"
 
#define BLE_DEVICE_NAME                           "Arduino Nano 33 BLE (Central)"
#define BLE_LOCAL_NAME                            "Arduino 1 (Nano 33 BLE) (Central)"
#define BLE_LED_PIN                               LED_BUILTIN

int threshold_count = 10; 
int threshold_factor = 2/3;
int threshold_1;
int threshold_2;
int threshold_3;
int threshold_4;

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
  while(!Serial);
  
  // Pin Outputs 
  pinMode(LED_BUILTIN, OUTPUT);

  // Check Initialization of BLE service
  if (!setupBleCentral())
  {
    Serial.println(F("Failed to initialize BLE"));
    while (1);
  }
  else
  {
    Serial.println(F("BLE initialized. Waiting for client connection"));
  }
  threshold_1 = threshold_calc(BLE_UUID_BEACON_SERVICE_DEVICE1, threshold_count);
  Serial.println(F("Threshold 1: "));
  Serial.println(threshold_1);
  threshold_2 = threshold_calc(BLE_UUID_BEACON_SERVICE_DEVICE2, threshold_count);
  Serial.println(F("Threshold 2: "));
  Serial.println(threshold_2);
}

void loop() {
  
  bleTask();
}

bool setupBleCentral()
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

int threshold_calc(String UUID, int count){
  Serial.print(UUID);
  BLEDevice peripheral;
  int i = 0; 
  int sum = 0;
  while (i < count){
  BLE.scanForUuid(UUID);
  peripheral = BLE.available();
    if (peripheral){
      sum += peripheral.rssi();
      BLE.stopScan();
    }
    i += 1;
  }
  return (threshold_factor * sum)/count;
}

void bleTask()
{
  Serial.print(F("Entered BLE TASK"));
  BLEDevice peripheral;
	  
	//Serial.println("- Discovering peripheral device...");

  BLE.scanForUuid(BLE_UUID_BEACON_SERVICE_DEVICE1);
	peripheral = BLE.available();
	  
	if (peripheral) {
	  //Serial.println("* Peripheral device found!");
	  //Serial.print("* Device MAC address: ");
	  //Serial.println(peripheral.address());
	  //Serial.print("* Device name: ");
	  //Serial.println(peripheral.localName());
	  //Serial.print("* Advertised service UUID: ");
	  //Serial.println(peripheral.advertisedServiceUuid());
	  //Serial.println(" ");
    Serial.println(peripheral.rssi());
	  //Serial.println(" "); 
	  BLE.stopScan();
	}
 
}
