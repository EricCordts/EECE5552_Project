// Anklet Arduino Program 

#include <ArduinoBLE.h>

#define BLE_UUID_BEACON_SERVICE_DEVICE1           "2a675dfb-a1b0-4c11-9ad1-031a84594196"
#define BLE_UUID_BEACON_SERVICE_DEVICE2           "ae7a527a-64f7-11ed-9022-0242ac120002"
#define BLE_UUID_BEACON_SERVICE_DEVICE3           "b52a20c8-64f7-11ed-9022-0242ac120002"
 
#define BLE_DEVICE_NAME                           "Arduino Nano 33 BLE (Central)"
#define BLE_LOCAL_NAME                            "Arduino 1 (Nano 33 BLE) (Central)"
#define BLE_LED_PIN                               LED_BUILTIN

const int threshold_count = 100; 
const int comparison_count = 5;
const float threshold_factor = 0.75;
const float nominal_factor = 0.88;

int nominalDeviceRssi1;
int nominalDeviceRssi2;

int threshold1;
int threshold2;

bool stepDetected;
int deviceStepDetected;

// main controller
BLEDevice mainController;

//function declarations:
bool setupBleMode();
void bleTask();
void bleConnectHandler(BLEDevice central);
void bleDisconnectHandler(BLEDevice central);

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

  /*
  connectToDevice(BLE_UUID_BEACON_SERVICE_DEVICE2, beacon2);
  threshold_2 = threshold_calc(threshold_count, beacon2, nominalDeviceRssi2);
  Serial.println(F("Threshold 2: "));
  Serial.println(threshold_2);
  

  // Connect to Peripheral 1 and calculate the threshold
  connectToDevice(BLE_UUID_BEACON_SERVICE_DEVICE1, beacon1);
  threshold_1 = threshold_calc(threshold_count, beacon1, nominalDeviceRssi1);
  Serial.println(F("Threshold 1: "));
  Serial.println(threshold_1);
  */

  nominalDeviceRssi1 = getDeviceRssi(10, BLE_UUID_BEACON_SERVICE_DEVICE1);
  threshold1 = nominalDeviceRssi1 * threshold_factor;
  Serial.print("Device 1 threshold:");
  Serial.println(threshold1);
  nominalDeviceRssi2 = getDeviceRssi(10, BLE_UUID_BEACON_SERVICE_DEVICE2);
  threshold2 = nominalDeviceRssi2 * threshold_factor;
  Serial.print("Device 2 threshold:");
  Serial.println(threshold2);
  stepDetected = false;
  deviceStepDetected = 0;
}

void loop() {
  int device1Reading = -1000;
  int device2Reading = -1000;
  if((!stepDetected) || (stepDetected && deviceStepDetected == 1))
  {
    device1Reading = getDeviceRssi(1, BLE_UUID_BEACON_SERVICE_DEVICE1);
    Serial.print("Device 1 reading:");
    Serial.println(device1Reading);
  }
  if((!stepDetected) || (stepDetected && deviceStepDetected == 2))
  {  
    device2Reading = getDeviceRssi(1, BLE_UUID_BEACON_SERVICE_DEVICE2);
    Serial.print("Device 2 reading:");
    Serial.println(device2Reading);
  }
  
  if((device1Reading > threshold1) && !stepDetected)
  {
    stepDetected = true;
    deviceStepDetected = 1;    
    Serial.println("Step detected Device 1");
  }
  else if((device2Reading > threshold2) && !stepDetected)
  {
    stepDetected = true;
    deviceStepDetected = 2;
    Serial.println("Step detected Device 2");
  }

  // if a step has been detected, 
  // check that the original device has gone back to 
  // its nominal RSSI value.
  if(stepDetected)
  {
    if(deviceStepDetected == 1)
    {
      if(device1Reading < (nominalDeviceRssi1 * nominal_factor))
      {
        // reset stepDetected and deviceStepDetected to default values
        stepDetected = false;
        deviceStepDetected = 0;
        Serial.println("Step cleared");
      }
    }
    else if(deviceStepDetected = 2)
    {
      if(device2Reading < (nominalDeviceRssi2 * nominal_factor))
      {
        // reset stepDetected and deviceStepDetected to default values
        stepDetected = false;
        deviceStepDetected = 0;
        Serial.println("Step cleared");
      }
    }
  }
  
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

int threshold_calc(int count, BLEDevice& device, int& nominalDeviceRssi)
{
  nominalDeviceRssi = averageRssiForComparison(threshold_count, device);
  return threshold_factor * (nominalDeviceRssi);
}

void connectToDevice(String uuid, BLEDevice& device)
{
  Serial.print("Scanning for UUID: ");
  Serial.println(uuid);
  while(!device.connected())
  {
    BLE.scanForUuid(uuid);
    device = BLE.available();
    if(device)
    {
      Serial.println(device.localName());
      Serial.println(device.advertisedServiceUuid());
      device.connect();
      BLE.stopScan();        
    }
  }
}

/*
void checkBeacon1Rssi(BLEDevice& device)
{
  int rssiReading = averageRssiForComparison(comparison_count, device);
  if(rssiReading > threshold_1 && !stepDetectedDevice1 && !stepDetectedDevice2)
  {
    Serial.println("Step detected!");
    Serial.print("Beacon 1: ");
    Serial.println(rssiReading);
    stepDetectedDevice1 = true;
  }
  else if(rssiReading < (nominalDeviceRssi1 + 5))
  {
    Serial.println(rssiReading);
    stepDetectedDevice1 = false;
  }
}
*/
/*
void checkBeacon2Rssi(BLEDevice& device)
{
  int rssiReading = averageRssiForComparison(comparison_count, device);
  if(rssiReading > threshold_2 && !stepDetectedDevice1 && !stepDetectedDevice2)
  {
    Serial.println("Step detected!");
    Serial.print("Beacon 2: ");
    Serial.println(rssiReading);
    stepDetectedDevice2 = true;
  }
  else if(rssiReading < (nominalDeviceRssi2 + 5))
  {
    Serial.println(rssiReading);
    stepDetectedDevice2 = false;
  }
}*/

int averageRssiForComparison(int count, BLEDevice& device)
{
  int i = 0;
  int sum = 0;
  while (i < count){
    if (device.connected())
    {
      int rssiVal = device.rssi();
      if(rssiVal != 127 && rssiVal != 0)
      {
        //Serial.println(rssiVal);
        sum += device.rssi();
        i += 1;
      }
    }
  }
  return sum/count;
}

int getDeviceRssi(int checkCount, String uuid)
{
  int i = 0;
  int sum = 0;
  BLEDevice device;
  while(!device)
  {
    BLE.scanForUuid(uuid);
    device = BLE.available();
    if(device)
    {
      //Serial.println(device.localName());
      //Serial.println(device.advertisedServiceUuid());
      BLE.stopScan();

      while (i < checkCount)
      {
        int rssiVal = device.rssi();
        if(rssiVal != 127 && rssiVal != 0)
        {
          sum += device.rssi();
          i += 1;
        }
      }
    }
  }
  return sum/checkCount;
}
