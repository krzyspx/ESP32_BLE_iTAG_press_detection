/**
   OK
  iTAG service - press button detection by notify
  http://100-x-arduino.blogspot.com/
Arduino IDE 1.8.5 
   //  pServerAddress = new BLEAddress("ff:ff:c2:0e:e0:e5"); //iTAG green
  // pServerAddress = new BLEAddress("fc:58:fa:05:a4:62"); //iTAG black
  //pServerAddress = new BLEAddress("fc:58:fa:04:cb:03"); //iTAG pink
*/

#include "BLEDevice.h"

static BLEUUID serviceUUID("0000ffe0-0000-1000-8000-00805f9b34fb"); // The remote service we wish to connect to.
static BLEUUID charUUID("0000ffe1-0000-1000-8000-00805f9b34fb"); // The characteristic of the remote service we are interested in.

static BLEAddress *pServerAddress;
static boolean doConnect = false;
static boolean connected = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;

bool deviceBleConnected = false;
bool serverfound = false;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {

    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.println(); Serial.print("BLE Advertised Device found: ");      Serial.println(advertisedDevice.toString().c_str());

      advertisedDevice.getScan()->stop();

      pServerAddress = new BLEAddress(advertisedDevice.getAddress());
      serverfound = true;
 //     Serial.print("Found our device!  ");
    } // onResult
}; // MyAdvertisedDeviceCallbacks

void scanBLEservers() {
  serverfound = false;
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(1);
}

class MyClientCallbacks: public BLEClientCallbacks {
    void onConnect(BLEClient *pClient) {
      deviceBleConnected = true;                                                  // set ble connected flag
      Serial.println("connected to my server");
    };

    void onDisconnect(BLEClient *pClient) {
      pClient->disconnect();
      deviceBleConnected = false;                                                 // clear ble connected flag
      Serial.println("disconnected from my server");
      connected = false;
      serverfound = false;
    }
};

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  Serial.println(); Serial.print("Notify from  iTAG");  // Serial.print(" length ");  Serial.println(length);

}

bool connectToServer(BLEAddress pAddress) {

  BLEClient*  pClient  = BLEDevice::createClient();  Serial.println(" Created client");

  pClient->setClientCallbacks(new MyClientCallbacks()); Serial.println(" Set Callbacks");

  pClient->connect(pAddress); // Serial.println(" Connected to server");

  int ifconn = pClient->isConnected();
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID); 
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");    return false;
  }
  Serial.println("Found service " + String(pRemoteService->toString().c_str())); //  Serial.println(pRemoteService->toString().c_str());

  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");    return false;
  }
  Serial.println("Found characteristic" + String(pRemoteCharacteristic->toString().c_str()));

  std::string value = pRemoteCharacteristic->readValue();  // Read the value of the characteristic.
  Serial.print("Read characteristic value: ");  Serial.println(value.c_str());


  const uint8_t bothOff[]        = {0x0, 0x0};
  const uint8_t notificationOn[] = {0x1, 0x0};
  const uint8_t indicationOn[]   = {0x2, 0x0};
  const uint8_t bothOn[]         = {0x3, 0x0};

  pRemoteCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true); //notification ON
  Serial.println("Nitification ON ");
  pRemoteCharacteristic->registerForNotify(notifyCallback); // Callback function when notification
  Serial.println("Notification Callback ");
}

void setup() {
  Serial.begin(115200);

  BLEDevice::init(""); Serial.println("Starting Arduino BLE Client application...");


} // End of setup.


void loop() {
  if (serverfound) {
    if (connected == false) {
      if (connectToServer(*pServerAddress)) {
        connected = true; Serial.println("Server UP");
      } else   {
        Serial.println("Server DOwN");
        deviceBleConnected = false;
      }
    }
  } else {
    scanBLEservers();
  }

  if (deviceBleConnected) {
    Serial.print("+");
    String newValue = "T";
    pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());     // Set the characteristic's value to be the array of bytes that is actually a string.
  }

  Serial.print("-");

  delay(1000);
} // End of loop
