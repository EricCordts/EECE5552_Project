// Anklet Arduino Program 

#include <ArduinoBLE.h>

#define BLE_UUID_BEACON_SERVICE_DEVICE1           "2a675dfb-a1b0-4c11-9ad1-031a84594196"
#define BLE_UUID_BEACON_SERVICE_DEVICE2           "ae7a527a-64f7-11ed-9022-0242ac120002"
//#define BLE_UUID_BEACON_SERVICE_DEVICE3           "b52a20c8-64f7-11ed-9022-0242ac120002"
//#define BLE_UUID_BEACON_SERVICE_DEVICE4           "ba5948e4-64f7-11ed-9022-0242ac120002"
 
#define BLE_DEVICE_NAME                           "Arduino Nano 33 BLE (Central)"
#define BLE_LOCAL_NAME                            "Arduino 1 (Nano 33 BLE) (Central)"
#define BLE_LED_PIN                               LED_BUILTIN

const int threshold_count = 100; 
const int comparison_count = 5;
const float threshold_factor = 0.6;
int nominalRssiValue1;
int threshold_1;
int threshold_2;
//int threshold_3;
//int threshold_4;

bool stepDetected;

// Anklet Beacons
BLEDevice anklet1;

//function declarations:
bool setupBleMode();
void bleTask();
void bleConnectHandler(BLEDevice central);
void bleDisconnectHandler(BLEDevice central);
void connectToDevice1();
void checkDevice1Rssi();
int averageRssiForComparison(int count);

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

  // Connect to Peripheral 1
  connectToDevice1();
  threshold_1 = threshold_calc(BLE_UUID_BEACON_SERVICE_DEVICE1, threshold_count);
  Serial.println(F("Threshold 1: "));
  Serial.println(threshold_1);
  //threshold_2 = threshold_calc(BLE_UUID_BEACON_SERVICE_DEVICE2, threshold_count);
  //Serial.println(F("Threshold 2: "));
  //Serial.println(threshold_2);

  stepDetected = false;
}

void loop() {
  checkDevice1Rssi();
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
  
  BLE.setEventHandler(BLEConnected, bleConnectHandler);
  BLE.setEventHandler(BLEDisconnected, bleDisconnectHandler);
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

int threshold_calc(String UUID, int count)
{
  nominalRssiValue1 = averageRssiForComparison(threshold_count);
  return threshold_factor * (nominalRssiValue1);
}

void connectToDevice1()
{	  
	Serial.println("- Discovering anklet 1");
  while(!anklet1.connected())
  {
    BLE.scanForUuid(BLE_UUID_BEACON_SERVICE_DEVICE1);
    anklet1 = BLE.available();
    if(anklet1)
    {
      Serial.println(anklet1.localName());
      Serial.println(anklet1.advertisedServiceUuid());
      anklet1.connect();
      BLE.stopScan();        
    }
  }
}

void checkDevice1Rssi()
{
  int rssiReading = averageRssiForComparison(comparison_count);
  if(rssiReading > threshold_1 && !stepDetected)
  {
    Serial.println("Step detected!");
    stepDetected = true;
  }
  else if(rssiReading < (nominalRssiValue1 + 5))
  {
    Serial.println(rssiReading);
    stepDetected = false;
  }
}

int averageRssiForComparison(int count)
{
  int i = 0;
  int sum = 0;
  while (i < count){
    if (anklet1.connected())
    {
      int rssiVal = anklet1.rssi();
      if(rssiVal != 127 && rssiVal != 0)
      {
        sum += anklet1.rssi();
        i += 1;
      }
    }
  }
  return sum/count;
}


