/*

*/

#ifndef Wiring_h
#define Wiring_h

#ifdef __cplusplus
extern "C"{
#endif

#include "TW32f0xx.h"
#include "pins_arduino.h"

#define HIGH 0x1
#define LOW  0x0

#define INPUT        0
#define OUTPUT       1

/* Configuration Mode enumeration */
typedef enum
{
    GPIO_Mode_AIN = 0x0,				//模拟输入
    GPIO_Mode_IN_FLOATING = 0x04,		//浮空输入
    GPIO_Mode_IPD = 0x28,				//下拉输入
    GPIO_Mode_IPU = 0x48,				//上拉输入
    GPIO_Mode_Out_OD = 0x14,			//通用开漏输出
    GPIO_Mode_Out_PP = 0x10,			//通用推免输出
    GPIO_Mode_AF_OD = 0x1C,				//复用开漏输出
    GPIO_Mode_AF_PP = 0x18				//复用推免输出
} GPIOMode_TypeDef;

#define GPIO_AF0           ((uint8_t)0x00)
#define GPIO_AF1           ((uint8_t)0x01)
#define GPIO_AF2           ((uint8_t)0x02)
#define GPIO_AF3           ((uint8_t)0x03)
#define GPIO_AF4           ((uint8_t)0x04)
#define GPIO_AF5           ((uint8_t)0x05)
#define GPIO_AF6           ((uint8_t)0x06)
#define GPIO_AF7           ((uint8_t)0x07)
#define GPIO_AF8           ((uint8_t)0x08)
#define GPIO_AF9           ((uint8_t)0x09)
#define GPIO_AF10          ((uint8_t)0x0A)
#define GPIO_AF11          ((uint8_t)0x0B)
#define GPIO_AF12          ((uint8_t)0x0C)
#define GPIO_AF13          ((uint8_t)0x0D)
#define GPIO_AF14          ((uint8_t)0x0E)
#define GPIO_AF15          ((uint8_t)0x0F)

typedef enum
{
	ADC_PA0 = 0,
	ADC_PA1,
	ADC_PA2, 
	ADC_PA3,
	ADC_PA4,
	ADC_PA5,
	ADC_PA6,
	ADC_PA7,
    ADC_PB0,
    ADC_PB1,
} ADC_Name;


void TW32F0xx_init(void);

void UART1_Init(uint32_t BAUD);
void UART1_SendCh(unsigned char c);
void UART2_Init(uint32_t BAUD);
void UART2_SendCh(unsigned char c);

void GPIO_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode, uint32_t pull, uint32_t speed);
void GPIOX_Init(GPIO_TypeDef* GPIOx, uint8_t mode);
void GPIO_PinRemapConfig(PIN_Name pin, uint32_t Alternate);
void pinMode(PIN_Name pin,GPIOMode_TypeDef mode);
void digitalWrite(PIN_Name pin,uint8_t state);
uint8_t digitalRead(PIN_Name pin);


uint32_t millis();
unsigned long micros(void);
void delayMicroseconds(uint32_t us);
void delay(unsigned long ms);

void ADC_Init(ADC_Name adcn, uint32_t sample, uint32_t resolution);
uint16_t ADC_Read(ADC_Name adcn);

#ifdef __cplusplus
} // extern "C"
#endif


#endif
