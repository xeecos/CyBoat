#include "cyberpi.h"

CyberPi cyber;
int lo[7] = {48,50,52,53,55,57,59};
int mo[7] = {60,62,64,65,67,69,71};
int ho[7] = {72,74,76,77,79,81,83};
void mic_recv(uint8_t* samples,int len) 
{
    cyber.clean_lcd();
    for(int i=0;i<len;i+=8)
    {
      int current = (int8_t)samples[i+1];
      if(current>-64&&current<64)
      {
        cyber.set_lcd_pixel(i/8,current+64,0xffff);
      }
    }
    cyber.render_lcd();
}

void setup()
{
    Serial.begin(115200);
    cyber.begin();
    // cyber.on_microphone_data(mic_recv);
    // for(int i=0;i<14;i++)
    // {
    //     cyber.set_instrument(i);
    //     int idx = 0;
    //     while(idx<7)
    //     {
    //         cyber.set_pitch(0,lo[idx],100);
    //         delay(600);
    //         idx++;
    //     }
    //     idx = 0;
    //     while(idx<7)
    //     {
    //         cyber.set_pitch(0,mo[idx],100);
    //         delay(600);
    //         idx++;
    //     }
    //     idx = 0;
    //     while(idx<7)
    //     {
    //         cyber.set_pitch(0,ho[idx],100);
    //         delay(600);
    //         idx++;
    //     }
    // }
}
float xx[128];
int push_idx = 0;
int pop_idx = 0;
void loop()
{    
    // Serial.printf("%.2f,%.2f,%.2f,%.2f,%.2f\n",cyber.get_roll(),cyber.get_pitch(),cyber.get_acc_x(),cyber.get_acc_y(),cyber.get_acc_z());
    xx[push_idx] = cyber.get_acc_z();
    push_idx++;
    push_idx = push_idx%128;
    cyber.clean_lcd();
    for(int i=0;i<128;i++)
    {
        pop_idx = push_idx+1;
        int idx = pop_idx+i;
        idx %= 128;
        int current = (int8_t)(xx[idx]/64)+64;
        cyber.set_lcd_pixel(i,current,0xffff);
    }
    cyber.render_lcd();
    delay(50);
}