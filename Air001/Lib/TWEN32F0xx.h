#ifndef TW32F003_h
#define TW32F003_h

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <TW32f0xx.h>
// #include "WMath.h"
#include "WString.h"
#include "itoa.h"

#ifdef __cplusplus
extern "C"{
#endif
#include "wiring.h"
#include "pins_arduino.h"

#define SystemCoreClock     24000000UL

#define clockCyclesPerMicrosecond() ( SystemCoreClock / 8000000 ) //p_us
#define clockCyclesPerMillisecond() ( SystemCoreClock / 8000 )    //p_ms
#define clockCyclesToMicroseconds(a) ( (a) * clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMillisecond() )

#define HIGH 0x1
#define LOW  0x0

#define INPUT        0
#define OUTPUT       1

#define true 0x1
#define false 0x0

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

#define SERIAL  0x0
#define DISPLAY 0x1

#define LSBFIRST 0
#define MSBFIRST 1

#define CHANGE 1
#define FALLING 2
#define RISING 3

#define INTERNAL 3
#define DEFAULT 1
#define EXTERNAL 0

#define uchar unsigned char

// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

typedef unsigned int word;

#define bit(b) (1UL << (b))

typedef uint8_t boolean;
typedef uint8_t byte;

// int atexit(void (*func)()) __attribute__((weak));

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
#include "WCharacter.h"
#include "WString.h"

#define word(...) makeWord(__VA_ARGS__)

#endif

#endif

