#pragma once
#include <stdint.h>
void ble_client_init(void (*onReceived)(uint8_t *, int));
void ble_client_connect(const char*name);
void ble_client_send(const char *msg, int len);
void ble_client_run();