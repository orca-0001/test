/**
 * @file Ultrasonic.h
 * @brief 支持SR04等三线、四线超声波.
 * @version 0.1
 * @date 2023-01-03
 * 
 * @copyright Copyright (c) 2022 杭州天问五幺科技有限公司
 * 
 */
#ifndef _ULTRASONIC_H_
#define _ULTRASONIC_H_

#include <TWEN32F0xx.h>

class Ultrasonic
{
public:
	Ultrasonic(uint8_t pin);//三线超声波，收发同一个引脚
    Ultrasonic(uint8_t trig, uint8_t echo);//四线超声波，收发不同引脚
	uint16_t distance(uint16_t MAXcm=400);
private:
	uint8_t _Ultrasonic_pin,_Ultrasonic_pin2;
	long measure(unsigned long timeout);
    volatile bool _measureFlag;
    volatile long _lastEnterTime;
    volatile float _measureValue;
};

Ultrasonic::Ultrasonic(uint8_t pin)
{
    _Ultrasonic_pin = pin;
    _Ultrasonic_pin2 = pin;
    _lastEnterTime = millis();
    _measureFlag = true;
    _measureValue = 0;
}

Ultrasonic::Ultrasonic(uint8_t trig, uint8_t echo)
{
    _Ultrasonic_pin = trig;
    _Ultrasonic_pin2 = echo;
    _lastEnterTime = millis();
    _measureFlag = true;
    _measureValue = 0;
}

long Ultrasonic::measure(unsigned long timeout)
{
    long duration;
    if(millis() - _lastEnterTime > 23)
    {
        _measureFlag = true; 
    }
    if(_measureFlag == true)
    {
        _lastEnterTime = millis();
        _measureFlag = false;
        pinMode((PIN_Name)_Ultrasonic_pin, GPIO_Mode_Out_PP);
        digitalWrite((PIN_Name)_Ultrasonic_pin,LOW);
        delayMicroseconds(2);
        digitalWrite((PIN_Name)_Ultrasonic_pin,HIGH);
        delayMicroseconds(10);
        digitalWrite((PIN_Name)_Ultrasonic_pin,LOW);
        pinMode((PIN_Name)_Ultrasonic_pin2, GPIO_Mode_IN_FLOATING);
        //获取高电平时间宽度
        unsigned long start = micros();
        while (digitalRead((PIN_Name)_Ultrasonic_pin2) != HIGH) {
            if ((micros() - start) >= timeout) {
                return 0;
            }
        }
        start = micros();
        while (digitalRead((PIN_Name)_Ultrasonic_pin2) == HIGH) {
            if ((micros() - start) >= timeout) {
                return 0;
            }
        }
        duration = micros() - start;
        _measureValue = duration;
    }
    else
    {
        duration = _measureValue;
    }
    return(duration);
}

uint16_t Ultrasonic::distance(uint16_t MAXcm)
{
    long distance = measure(MAXcm * 55 + 200);
    if(distance == 0)
    {
        distance = MAXcm * 58;
    }
    return( distance / 58);
}

#endif