#include <ArduinoBLE.h>

#define BLE_UUID_CHAR "315f50e2-55c9-4b10-8b46-6c66957b4d98"           //Characteristic UUID
#define BLE_UUID_ANKLET_SAMPLE "1e0f9d07-42fe-4b48-b405-38374e5f2d97"  //Device UUID

BLEService Arduino_measurements(BLE_UUID_ANKLET_SAMPLE);
BLECharCharacteristic step_sample(BLE_UUID_CHAR, BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);
  //while (!Serial)
    //;
  // put your setup code here, to run once:
  if (!BLE.begin()) {
    Serial.println("* Starting Bluetooth® Low Energy module failed!");
    while (1)
      ;
  }

  BLE.setLocalName("Nano 33 BLE (Central)");
  BLE.setAdvertisedService(Arduino_measurements);
  Arduino_measurements.addCharacteristic(step_sample);
  BLE.addService(Arduino_measurements);
  step_sample.writeValue(0);
  BLE.advertise();

  Serial.println("Arduino Nano 33 BLE Sense (Central Device)");
  Serial.println(" ");
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEDevice central = BLE.central();
  Serial.println("- Discovering central device...");
  delay(500);

  if (central) {
    Serial.println("* Connected to central device!");
    Serial.print("* Device MAC address: ");
    Serial.println(central.address());
    Serial.println(" ");

    while (central.connected()) {
      if (step_sample.written()) {
        byte current_step = step_sample.value();
        if (current_step & (1<<0)) {
          Serial.println("Red");
          Serial.println(" ");
        }

        if (current_step & (1<<1)) {
          Serial.println("Blue");
          Serial.println(" ");
        }

        if (current_step & (1<<2)) {
          Serial.println("Green");
          Serial.println(" ");
        }

        if (current_step & (1<<3)) {
          Serial.println("Yellow");
          Serial.println(" ");
        }

        if (current_step & (1<<5)) {
          Serial.println("Done with calculating threshold values");
          Serial.println(" ");
        }

        if (current_step & (1<<6)) {
          Serial.println("Back to middle");
          Serial.println(" ");
        }
      }
    }

    Serial.println("* Disconnected to central device!");
  }
}

void connectToPeripheral() {
  BLEDevice peripheral;

  Serial.println("- Discovering peripheral device...");

  do {
    BLE.scanForUuid(BLE_UUID_ANKLET_SAMPLE);
    peripheral = BLE.available();
  } while (!peripheral);

  if (peripheral) {
    Serial.println("* Peripheral device found!");
    Serial.print("* Device MAC address: ");
    Serial.println(peripheral.address());
    Serial.print("* Device name: ");
    Serial.println(peripheral.localName());
    Serial.print("* Advertised service UUID: ");
    Serial.println(peripheral.advertisedServiceUuid());
    Serial.println(" ");
    BLE.stopScan();
    step_detection(peripheral);
  }
}

void step_detection(BLEDevice peripheral) {
  Serial.println("- Connecting to peripheral device...");

  if (peripheral.connect()) {
    Serial.println("* Connected to peripheral device!");
    Serial.println(" ");
  } else {
    Serial.println("* Connection to peripheral device failed!");
    Serial.println(" ");
    return;
  }

  Serial.println("- Discovering peripheral device attributes...");
  if (peripheral.discoverAttributes()) {
    Serial.println("* Peripheral device attributes discovered!");
    Serial.println(" ");
  } else {
    Serial.println("* Peripheral device attributes discovery failed!");
    Serial.println(" ");
    peripheral.disconnect();
    return;
  }

  BLECharacteristic step_Characteristic = peripheral.characteristic(BLE_UUID_CHAR);

  if (!step_Characteristic) {
    Serial.println("* Peripheral device does not have step characteristic!");
    peripheral.disconnect();
    return;
  } else {
    step_Characteristic.subscribe();
  }

  while (peripheral.connected()) {
    // check if the value of the simple key characteristic has been updated
    if (step_Characteristic.valueUpdated()) {
      byte current_step;
      step_Characteristic.readValue(current_step);

      if (current_step & 0x01) {
        Serial.println("Red");
        Serial.println(" ");
      }

      if (current_step & 0x02) {
        Serial.println("Blue");
        Serial.println(" ");
      }

      if (current_step & 0x03) {
        Serial.println("Green");
        Serial.println(" ");
      }

      if (current_step & 0x04) {
        Serial.println("Yellow");
        Serial.println(" ");
      }

      if (current_step & 0x05) {
        Serial.println("Done with calculating threshold values");
        Serial.println(" ");
      }
    }
    //current_step = (char *)step_Characteristic.value();
  }
  Serial.println("- Peripheral device disconnected!");
}