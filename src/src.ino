#include "cyberpi.h"
#include "mbot2.h"
#define PADDLE_MODE 1
#define BOAT_MODE 0
#define MODE PADDLE_MODE
#if MODE == BOAT_MODE
#include "ble_client.h"
#elif MODE == PADDLE_MODE
#include "ble_server.h"
float xx[128];
int push_idx = 0;
int pop_idx = 0;
#endif
CyberPi cyber;
int values[2] = {0, 0};
int lo[7] = {48, 50, 52, 53, 55, 57, 59};
int mo[7] = {60, 62, 64, 65, 67, 69, 71};
int ho[7] = {72, 74, 76, 77, 79, 81, 83};

void onBLEReceived(uint16_t connId, uint8_t *res, int len)
{
// Serial.printf("%d %d\n", connId, len);
#if MODE == BOAT_MODE
    char str[65];
    int i;
    for (i = 0; i < len; i++)
    {
        str[i] = res[i];
    }
    str[len] = 0;
    values[connId] = (int)strtof(str, NULL);
#endif
    // cyber.clean_lcd();
}
void onBLEConnected(const char *name)
{
    cyber.clean_lcd();
    cyber.set_bitmap(5, 7, cyber.create_text(L"CyPaddle 已连接", 0x6Fa6, 16));
    cyber.render_lcd();
}
void setup()
{
    Serial.begin(115200);
    cyber.begin();
    cyber.clean_lcd();
#if MODE == BOAT_MODE
    cyber.set_bitmap(10, 7, cyber.create_text(L"启动中...", 0x6Fa6, 16));
    cyber.render_lcd();
    mbot2_init();
    ble_client_init(onBLEConnected, onBLEReceived);
    cyber.clean_lcd();
    cyber.set_bitmap(10, 7, cyber.create_text(L"CyBoat 已上线", 0x6Fa6, 16));
    cyber.render_lcd();
#elif MODE == PADDLE_MODE
    cyber.set_bitmap(5, 7, cyber.create_text(L"启动中...", 0x6Fa6, 16));
    cyber.render_lcd();
    ble_server_init("CyBoat P1", onBLEConnected, onBLEReceived);
    cyber.set_bitmap(5, 7, cyber.create_text(L"CyPaddle 已上线", 0x6Fa6, 16));
    cyber.render_lcd();
#endif
    // cyboat - client
    // cypaddle - server
}
void loop()
{
// for (int i = 1; i < 11; i++)
// {
//     mbot2_run(i+2, i+2);
//     delay(100);
// }
// for (int i = 0; i < 10; i++)
// {
//     mbot2_run(12 - i, 12 - i);
//     delay(100);
// }
#if MODE == PADDLE_MODE
    cyber.read_gyro();
    // Serial.printf("%.2f,%.2f,%.2f,%.2f,%.2f\n", cyber.get_roll(), cyber.get_pitch(), cyber.get_acc_x() / 80, cyber.get_acc_y() / 80, cyber.get_acc_z() / 80);
    // xx[push_idx] = cyber.get_acc_z();
    // push_idx++;
    // push_idx = push_idx % 128;
    // cyber.clean_lcd();
    // for (int i = 0; i < 128; i++)
    // {
    //     pop_idx = push_idx + 1;
    //     int idx = pop_idx + i;
    //     idx %= 128;
    //     int current = (int8_t)(xx[idx] / 64) + 64;
    //     cyber.set_lcd_pixel(i, current, 0xffff);
    // }
    int z = (int)(cyber.get_acc_z() / 80);
    char *str1 = (char *)malloc(16);
    sprintf(str1, "z: %d\0", z);
    cyber.set_bitmap(5, 37, cyber.create_text(str1, 0xA6a6, 16));
    free(str1);
    cyber.render_lcd();

    char *str2 = (char *)malloc(16);
    sprintf(str2, "%d\n\0", z);
    ble_server_send(str2);
    free(str2);
    ble_server_run();
#elif MODE == BOAT_MODE
    for (int i = 0; i < 2; i++)
    {
        char *str = (char *)malloc(64);
        sprintf(str, "%d:%d\0", i, values[i]);
        cyber.set_bitmap(16, 32 + 32 * i, cyber.create_text(str, 0xffff, 16));
        free(str);
        cyber.render_lcd();
    }
    ble_client_run();
#endif
    delay(25);
}