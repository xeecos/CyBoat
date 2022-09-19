#pragma once

void mbot2_init();
void mbot2_run(float left, float right);
void mbot2_servo_set(int angle1,int angle2,int angle3,int angle4);
void mbot2_servo_set(int idx,int angle);
void mbot2_dc_motor(float power1,float power2);