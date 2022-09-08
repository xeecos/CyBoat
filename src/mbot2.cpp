#include "mbot2.h"
#include <HardwareSerial.h>
#include "utils.h"
HardwareSerial uart(2);

void sendCMD(uint8_t *cmd, int len)
{
    uart.write(0xf0);
    int checksum = 0;
    for (int i = 0; i < len; i++)
    {
        uart.write(cmd[i]);
        Serial.printf("hex:%d, %x\n", i, cmd[i]);
        checksum += cmd[i];
    }
    uart.write(checksum & 0x7f);
    uart.write(0xf7);
}
void uart2Handler(void *params)
{
    while (1)
    {
        if (uart.available())
        {
            char c = uart.read();
        }
        delay(500);
    }
}
void mbot2_init()
{
    uart.begin(921600, SERIAL_8N1, 21, 15);
    uint8_t cmd[4] = {0xff, 0x10, 0x0, 0xf};
    sendCMD(cmd, 4);
    xTaskCreatePinnedToCore(uart2Handler, "_on_uart", 4096, NULL, 10, NULL, 1);
}
void mbot2_run(float left, float right)
{
    uint8_t cmd[25] = {0x1, 0x67, 0x07, 0x05, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float2bits(-left, &cmd[4]);
    float2bits(-right, &cmd[9]);
    long2bits(0,&cmd[14]);
    short2bits(1,&cmd[19]);
    short2bits(1,&cmd[22]);
    sendCMD(cmd, 25);
}
void mbot2_servo_set(int idx, int angle)
{
}