/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of HDUSB - http://www.haohaodada.com/

  Copyright (c) 2019 Blue
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h
#include <TW32f0xx.h>

#define NUM_DIGITAL_PINS            18
#define NUM_PWM_PINS                4
#define NUM_ANALOG_INPUTS           10

// #define digitalPinHasPWM(p)         ((p) == PA1 || (p) == PC0 || (p) == PC3 || (p) == PC4 || (p) == PC5 || (p) == PC7 || (p) == PD2 || (p) == PD3)
// #define LED_BUILTIN 17

typedef enum
{
    PA0 = 0,
    PA1,
    PA2,
    PA3,
    PA4,
    PA5,
    PA6,
    PA7,
    PA8,
    PA9,
    PA10,
    PA11,
    PA12,
    PA13,
    PA14,
    PA15,
    PB0,
    PB1,
    PB2,
    PB3,
    PB4,
    PB5,
    PB6,
    PB7,
    PB8,
    PB9,
    PB10,
    PB11,
    PB12,
    PB13,
    PB14,
    PB15,
    PF0,
    PF1,
    PF2,
    PF3,
    PF4
} PIN_Name;

static const uint32_t DIGITAL_PIN_PORT[37]={
  GPIOA_BASE,  //PA0
  GPIOA_BASE,  //PA1
  GPIOA_BASE,  //PA2
  GPIOA_BASE,  //PA3
  GPIOA_BASE,  //PA4
  GPIOA_BASE,  //PA5
  GPIOA_BASE,  //PA6
  GPIOA_BASE,  //PA7
  GPIOA_BASE,  //PA8
  GPIOA_BASE,  //PA9
  GPIOA_BASE,  //PA10
  GPIOA_BASE,  //PA11
  GPIOA_BASE,  //PA12
  GPIOA_BASE,  //PA13
  GPIOA_BASE,  //PA14
  GPIOA_BASE,  //PA15
  GPIOB_BASE,  //PB0
  GPIOB_BASE,  //PB1
  GPIOB_BASE,  //PB2
  GPIOB_BASE,  //PB3
  GPIOB_BASE,  //PB4
  GPIOB_BASE,  //PB5
  GPIOB_BASE,  //PB6
  GPIOB_BASE,  //PB7
  GPIOB_BASE,  //PB8
  GPIOB_BASE,  //PB9
  GPIOB_BASE,  //PB10
  GPIOB_BASE,  //PB11
  GPIOB_BASE,  //PB12
  GPIOB_BASE,  //PB13
  GPIOB_BASE,  //PB14
  GPIOB_BASE,  //PB15
  GPIOF_BASE,  //PF0
  GPIOF_BASE,  //PF1
  GPIOF_BASE,  //PF2
  GPIOF_BASE,  //PF3
  GPIOF_BASE,  //PF4
};

const uint32_t DIGITAL_PIN_NUM[37]={
  0, //PA0
  1, //PA1
  2, //PA2
  3, //PA3
  4, //PA4
  5, //PA5
  6, //PA6
  7, //PA7
  8, //PA8占位
  9, //PA9
  10, //PA10
  11, //PA11
  12, //PA12
  13, //PA13
  14, //PA14
  15, //PA15  
  0, //PB0
  1, //PB1
  2, //PB2
  3, //PB3
  4, //PB4
  5, //PB5
  6, //PB6
  7, //PB7
  8, //PB8占位
  9, //PB9
  10, //PB10
  11, //PB11
  12, //PB12
  13, //PB13
  14, //PB14
  15, //PB15 
  0, //PF0
  1, //PF1
  2, //PF2
  3, //PF3
  4, //PF4

};

const uint32_t ANALOG_PIN_NUM[NUM_ANALOG_INPUTS]={
  PA0,    //A0
  PA1,    //A1
  PA2,    //A2
  PA3,    //A3
  PA4,    //A4
  PA5,    //A5
  PA6,    //A6
  PA7,    //A7
  PB0,    //A6
  PB1,    //A7
};	

#endif