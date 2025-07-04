/**
 * @file HX711.h
 * @brief HX711是一款专为高精度称重传感器而设计的24位A/D转换器芯片。
 * @version 0.1
 * @date 2023-01-03
 * 
 * @copyright Copyright (c) 2022 杭州天问五幺科技有限公司
 * 
 */
#ifndef _HX711_H
#define _HX711_H

#include <TWEN32F0xx.h>

class HX711{
public:
    HX711(uint8_t sckpin, uint8_t dtpin);
    void begin();
    uint32_t read_count();  

private:
    uint8_t _sckpin;
    uint8_t _dtpin;
};

HX711::HX711(uint8_t sckpin, uint8_t dtpin)
{
    _sckpin = sckpin;
    _dtpin = dtpin;
}

//========================================================================
// 描述: 初始化.
// 参数: none.
// 返回: none.
//========================================================================
void HX711::begin()
{
    pinMode((PIN_Name)_sckpin, GPIO_Mode_Out_PP);
    digitalWrite((PIN_Name)_sckpin, 1);
    pinMode((PIN_Name)_dtpin, GPIO_Mode_IN_FLOATING);
    digitalWrite((PIN_Name)_dtpin, 1);
}

//========================================================================
// 描述: HX711读取原始数据.
// 参数: none.
// 返回: none.
//========================================================================
uint32_t HX711::read_count()
{
	uint32_t count = 0;
	uint8_t i=0;

	digitalWrite((PIN_Name)_sckpin, 0);	//ADSK=0; 使能AD（PD_SCK 置低）
	while(digitalRead((PIN_Name)_dtpin)); 	//AD转换未结束则等待，否则开始读取
	for (i=0;i<24;i++)
	{
		digitalWrite((PIN_Name)_sckpin, 1);	// ADSK=1; PD_SCK 置高（发送脉冲）
        count = count<<1;		//下降沿来时变量Count左移一位，右侧补零
        digitalWrite((PIN_Name)_sckpin, 0);	//ADSK=0; PD_SCK 置低
        if(digitalRead((PIN_Name)_dtpin))count++;
	}
	digitalWrite((PIN_Name)_sckpin, 1);		// ADSK=1;
	count=count^0x800000;		//第25个脉冲下降沿来时，转换数据
	digitalWrite((PIN_Name)_sckpin, 0);		//ADSK=0;
	return(count);
}

#endif 



