#ifndef _KZ_GPIO_H
#define _KZ_GPIO_H

#include "ch32v003fun.h"
#include "ch32v003_GPIO_branchless.h"

enum
{
    A1 = 0x01,  A2,

    C0 = 0x20,  C1 ,    C2 ,    C3 ,    C4 ,    C5 ,    C6 ,    C7 ,
    D0 = 0x30,  D1 ,    D2 ,    D3 ,    D4 ,    D5 ,    D6 ,    D7 ,

    PIN_NULL = 0xFF
};

#define KZ_GPIO_HIGH(GPIOv) GPIO_digitalWrite(GPIOv, high)
#define KZ_GPIO_LOW(GPIOv) GPIO_digitalWrite(GPIOv, low);
#define KZ_GPIO_SET_OUTPUT(GPIOv) GPIO_pinMode(GPIOv,GPIO_pinMode_O_pushPull,GPIO_Speed_50MHz)
#define KZ_GPIO_SET_INPUT(GPIOv) GPIO_pinMode(GPIOv,GPIO_pinMode_I_pullDown,GPIO_Speed_50MHz)
#define KZ_GPIO_READ(GPIOv) GPIO_digitalRead(GPIOv)

#define KZ_ENABLE_GPIO_PORT_ 	GPIO_port_enable(GPIO_port_C);\
GPIO_port_enable(GPIO_port_D);\
GPIO_port_enable(GPIO_port_A);

#endif
