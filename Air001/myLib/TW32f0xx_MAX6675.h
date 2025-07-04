#ifndef _MAX6675_H 
#define _MAX6675_H

#include <TWEN32F0xx.h>

//#define  MAX6675_USE_HARDSPI      //使用硬件SPI
#if defined(MAX6675_USE_HARDSPI)
#include "TW32f0xx_HARDSPI.h"
#else
#include "TW32f0xx_SOFTSPI.h"
#endif

class MAX6675{
public:
#if defined(MAX6675_USE_HARDSPI)
    MAX6675(uint8_t cspin);
#else
    MAX6675(uint8_t sopin, uint8_t sckpin, uint8_t cspin);
#endif
    void begin();
    uint16_t read_reg();
    float read_temp();
private:
    uint8_t _sopin;
    uint8_t _sckpin;
    uint8_t _cspin;
};

/**
 * @brief 引脚分配
 * @note 
 *      如果使用硬件SPI,引脚请查看HARDSPI.h
 */
#if defined(MAX6675_USE_HARDSPI)
MAX6675::MAX6675(uint8_t cspin)
{
    _cspin = cspin;
}
#else
MAX6675::MAX6675(uint8_t sopin, uint8_t sckpin, uint8_t cspin)
{
    _sopin = sopin;
    _sckpin = sckpin;
    _cspin = cspin;
}
#endif

/**
 * @brief 初始化
 */
void MAX6675::begin()
{
    pinMode((PIN_Name)_cspin, GPIO_Mode_Out_PP);
    digitalWrite((PIN_Name)_cspin, 1);
#if defined(MAX6675_USE_HARDSPI)
    hardspi.begin();
#else
    pinMode((PIN_Name)_sckpin, GPIO_Mode_Out_PP);
    digitalWrite((PIN_Name)_sckpin, 1);
    pinMode((PIN_Name)_sopin, GPIO_Mode_IPU);
    digitalWrite((PIN_Name)_sopin, 1);
#endif
}

/**
 * @brief  读原始数据
 * @return 读出的16位数据
 */
uint16_t MAX6675::read_reg() 
{
    uint8_t i;
    uint16_t dat=0;
    digitalWrite((PIN_Name)_cspin, 0);
#if defined(MAX6675_USE_HARDSPI)
    hardspi.set_speed(2);
    dat = hardspi.read_data();
    dat = (dat<<8) | hardspi.read_data();
#else
    for(i=0;i<16;i++){
        digitalWrite((PIN_Name)_sckpin, 1);
        dat <<=1;
        if(digitalRead((PIN_Name)_sopin)){                  
            dat |= 0x01;
        }
        digitalWrite((PIN_Name)_sckpin, 0);
    }
#endif
    digitalWrite((PIN_Name)_cspin, 1);
    return dat;
}

/**
 * @brief  读温度
 * @return 读到的温度数据
 */
float MAX6675::read_temp()
{
    uint16_t t;
    t = read_reg();
    if(t&0x8006)      //D2位为1表示掉线，该位为0表示连接;D1和D15位为0
    {
        return 1024;
    }else{
        t = t<<1;
        t = t>>4;
        return (float)t/4.0; ////测得的温度单位是0.25，所以乘以0.25才能得到以度为单位的温度值
    }
}

#endif