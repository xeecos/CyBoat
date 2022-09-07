#pragma once
#include <stdint.h>
typedef enum
{
    BLE_OK = 0,
    BLE_ERR,
    BLE_NAME_GET,
    BLE_NAME_SET,
    BLE_WIFI_SET,
    BLE_IP_GET
} BLE_METHOD;
void ble_server_init(const char* name,void (*)(uint8_t*,int));
void ble_server_send(int status, const char* msg);
void ble_server_send(char *msg);
void ble_server_send_ok();
void ble_server_parse();
bool ble_server_connected();
void ble_server_run();