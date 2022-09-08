#pragma once
#include <stdint.h>
union float2bytes
{
    float fv;
    uint8_t bv[4];
};

union long2bytes
{
    long lv;
    uint8_t bv[4];
};

union shot2bytes
{
    short sv;
    uint8_t bv[4];
};
void long2bits(long val, uint8_t *addr)
{
    addr[0] = val & 0x7f;
    addr[1] = (val >> 7) & 0x7f;
    addr[2] = (val >> 14) & 0x7f;
    addr[3] = (val >> 21) & 0x7f;
    addr[4] = (val >> 28) & 0x7f;
}

void ushort2bits(unsigned short val, uint8_t *addr)
{
    addr[0] = val & 0x7f;
    addr[1] = (val >> 7) & 0x7f;
}
void short2bits(short val, uint8_t *addr)
{
    addr[0] = val & 0x7f;
    addr[1] = (val >> 7) & 0x7f;
    addr[2] = (val >> 14) & 0x7f;
}

void float2bits(float val, uint8_t *addr)
{
    float2bytes f;
    f.fv = (val);
    long2bytes l;
    l.bv[0] = f.bv[0];
    l.bv[1] = f.bv[1];
    l.bv[2] = f.bv[2];
    l.bv[3] = f.bv[3];
    long2bits(l.lv, addr);
}