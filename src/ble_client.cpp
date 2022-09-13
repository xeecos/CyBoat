#include "ble_client.h"
#include "config.h"
#include "BLEDevice.h"
//#include "BLEScan.h"
#include <Arduino.h>
// The remote service we wish to connect to.
static BLEUUID serviceUUID(SERVICE_UUID);
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID(CHARACTERISTIC_UUID_RX);
static BLEUUID notifyUUID(CHARACTERISTIC_UUID_TX);

static bool doConnect = false;
static bool connected = false;
static bool doScan = false;
// static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLEAdvertisedDevice *_enterDevices[2];
void (*_onReceived)(uint16_t connId, uint8_t *, int);
void (*_onConnected)(const char *);
struct ble_client
{
    uint16_t connId;
    const char *name;
    /* data */
};
int _idx = 0;
ble_client conns[2];
static void notifyCallback(
    BLERemoteCharacteristic *pBLERemoteCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{
    // Serial.print("Notify callback for characteristic ");
    // Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    // Serial.print(" of data length ");
    // Serial.println(length);
    // Serial.print("data: ");
    // Serial.println((char *)pData);
    _onReceived(pBLERemoteCharacteristic->getRemoteService()->getClient()->getConnId(), pData, length);
}

class MyClientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient *pclient)
    {
    }

    void onDisconnect(BLEClient *pclient)
    {
        connected = false;
        Serial.println("onDisconnect");
    }
};

bool connectToServer()
{
    for (int i = 0; i < 2; i++)
    {
        BLEAdvertisedDevice *myDevice = _enterDevices[i];
        Serial.print("Forming a connection to ");
        Serial.println(myDevice->getAddress().toString().c_str());

        BLEClient *pClient = BLEDevice::createClient();
        Serial.println(" - Created client");

        pClient->setClientCallbacks(new MyClientCallback());

        // Connect to the remove BLE Server.
        pClient->connect(myDevice); // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
        Serial.println(" - Connected to server");
        pClient->setMTU(517); // set client to request maximum MTU from server (default is 23 otherwise)

        // Obtain a reference to the service we are after in the remote BLE server.
        BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
        if (pRemoteService == nullptr)
        {
            Serial.print("Failed to find our service UUID: ");
            Serial.println(serviceUUID.toString().c_str());
            pClient->disconnect();
            return false;
        }
        Serial.println(" - Found our service");

        // Obtain a reference to the characteristic in the service of the remote BLE server.
        BLERemoteCharacteristic *pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
        BLERemoteCharacteristic *pRemoteNotifyCharacteristic = pRemoteService->getCharacteristic(notifyUUID);
        if (pRemoteCharacteristic == nullptr)
        {
            Serial.print("Failed to find our characteristic UUID: ");
            Serial.println(charUUID.toString().c_str());
            pClient->disconnect();
            return false;
        }
        Serial.println(" - Found our characteristic");

        // Read the value of the characteristic.
        if (pRemoteCharacteristic->canRead())
        {
            // std::string value = pRemoteCharacteristic->readValue();
            // Serial.print("The characteristic value was: ");
            // Serial.println(value.c_str());
        }

        if (pRemoteNotifyCharacteristic->canNotify())
        {
            // Serial.println("canNotify!");
            pRemoteNotifyCharacteristic->registerForNotify(notifyCallback);
        }

        conns[i].connId = pClient->getConnId();
        conns[i].name = myDevice->getName().c_str();
        _onConnected(myDevice->getName().c_str());
    }
    connected = true;
    return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    /**
     * Called for each advertising BLE server.
     */
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {

        // We have found a device, let us now see if it contains the service we are looking for.
        if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID))
        {

            Serial.print("BLE Advertised Device found: ");
            Serial.println(advertisedDevice.toString().c_str());
            _enterDevices[_idx] = new BLEAdvertisedDevice(advertisedDevice);
            _idx++;
            if (_idx == 2)
            {
                BLEDevice::getScan()->stop();
                doConnect = true;
                doScan = true;
            }
            //

        } // Found our server
    }     // onResult
};        // MyAdvertisedDeviceCallbacks

void ble_client_init(void (*onConnected)(const char *), void (*onReceived)(uint16_t connId, uint8_t *, int))
{
    _onReceived = onReceived;
    _onConnected = onConnected;
    BLEDevice::init("");

    // Retrieve a Scanner and set the callback we want to use to be informed when we
    // have detected a new device.  Specify that we want active scanning and start the
    // scan to run for 5 seconds.
    BLEScan *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5, true);
}
void ble_client_send(const char *msg, int len)
{
    if (connected)
    {
        // pRemoteCharacteristic->writeValue(msg, len);
    }
}
void ble_client_run()
{
    if (doConnect == true)
    {
        if (connectToServer())
        {
            Serial.println("We are now connected to the BLE Server.");
        }
        else
        {
            Serial.println("We have failed to connect to the server; there is nothin more we will do.");
        }
        doConnect = false;
    }

    // If we are connected to a peer BLE Server, update the characteristic each time we are reached
    // with the current time since boot.
    if (connected)
    {
        // String newValue = "time:" + String(millis() / 1000)+"\n";
        // Serial.println("Setting new characteristic value to \"" + newValue + "\"");

        // Set the characteristic's value to be the array of bytes that is actually a string.
        // pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
    }
    else if (doScan)
    {
        BLEDevice::getScan()->start(0);
        delay(1000);
    }
}