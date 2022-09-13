#pragma once
#include <stdint.h>
void ble_client_init(void (*onConnected)(const char *), void (*onReceived)(uint16_t connId, uint8_t *, int));
void ble_client_send(const char *msg, int len);
void ble_client_run();