/**
 * @file  WS2812.h
 * @brief RGB单总线模块驱动的头文件
 * @version 0.1
 * @author blue
 * @copyright Copyright (c) 2021 TWen Technology Co., Ltd.
 * 
 */

#ifndef _WS2812_H_
#define _WS2812_H_

#include "TW32f0xx.h"
#include "wiring.h"
#include <stdlib.h>

typedef union {
	struct
	{
		int g;
		int r;
		int b;
	}simple;
	int grb[3];
}color_t;

typedef struct _rgb_strip_t{
	GPIO_TypeDef *GPIOx;
	uint32_t gpio_pinx;
 	uint8_t position; 
 	int num_leds;
 	color_t *leds;
}rgb_strip_t;

class WS2812
{
public:
	WS2812( uint8_t pin);
	void pixel_init(uint16_t num_leds);	//RGB初始化
	uint8_t pixel_set_color(uint16_t index, uint8_t red, uint8_t green, uint8_t blue);	//RGB设置颜色
	uint8_t pixel_set_color(uint16_t index, uint32_t c);	//RGB设置颜色
    void pixel_set_all_color(uint8_t red, uint8_t green, uint8_t blue);	//设置所有RGB颜色
	void pixel_show();	//RGB更新显示
	uint8_t pixel_set_color_and_show(uint16_t index, uint8_t red, uint8_t green, uint8_t blue);	//RGB设置颜色并显示
	void pixel_clear();	//RGB清除
	void rainbow(uint8_t wait);//彩虹循环呼吸效果
	void rainbowCycle(uint8_t wait);//流光溢彩效果
	void setBrightness(uint8_t b);//设置亮度值

	void sendOne();
	void sendZero();
private:
	void sendRGB(uint8_t red,uint8_t green,uint8_t blue);
	void rstRGB();
	uint32_t color(uint8_t r, uint8_t g, uint8_t b) {
   		return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
 	}
	uint32_t wheel(uint8_t WheelPos);

	rgb_strip_t strip;
};
	uint8_t _brightness;

/**
 * @brief 引脚配置
 * @param pin   引脚
 *              PA0 - PA15
 *              PB0 - PB15
 *              PF0 - PF4
 */
//========================================================================
// 描述: 引脚初始化.
// 参数: pin:引脚.
// 返回: none.
//========================================================================
WS2812::WS2812(uint8_t pin)
{
	strip.position  = DIGITAL_PIN_NUM[pin];
	strip.GPIOx     = (GPIO_TypeDef*)DIGITAL_PIN_PORT[pin];
	strip.gpio_pinx = (0x1U << strip.position);
	_brightness     = 50;
}

/**
 * @brief RGB发送1码
 */
void WS2812::sendOne()
{
	//高电平850ns
	(strip.GPIOx)->BSRR |= strip.gpio_pinx;			//(0x1U<<strip.position);
	__asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n  \
					 nop\n				   					   \
				    ");
	//低电平400ns
	(strip.GPIOx)->BRR |= strip.gpio_pinx;		//(0x1U<<strip.position);
}

/**
 * @brief RGB发送0码
 */
void WS2812::sendZero()
{
	//高电平400ns
	(strip.GPIOx)->BSRR |= strip.gpio_pinx;				//(0x1U<<strip.position);
	__asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\n");
	//低电平850ns
	(strip.GPIOx)->BRR |= strip.gpio_pinx;		//(0x1U<<strip.position);
}

/**
 * @brief RGB发送数据
 * @param red	红色值
 * @param green	绿色值
 * @param blue	蓝色值		
 */
void WS2812::sendRGB(uint8_t red,uint8_t green,uint8_t blue)
{
    __disable_irq();
	for(uint8_t i=0;i<8;i++)
	{
		if(green&0x80){
			sendOne();
		}else{
			sendZero();
		}
		 green<<=1;
	}
	for(uint8_t i=0;i<8;i++)
	{
		if(red&0x80){
			sendOne();
		}else{
			sendZero();
		}
		 red<<=1;
	}
	for(uint8_t i=0;i<8;i++)
	{
		if(blue&0x80){
			sendOne();
		}else{
			sendZero();
		}
		 blue<<=1;
	}
	__enable_irq();
}

/**
 * @brief RGB引脚模式设置	
 */
void WS2812::rstRGB()
{
	GPIO_Init(strip.GPIOx,strip.position,GPIO_MODE_OUTPUT_PP, GPIO_NOPULL,GPIO_SPEED_FREQ_VERY_HIGH);
	(strip.GPIOx)->BRR |= (0x1U << strip.position);
}

/**
 * @brief RGB初始化	
 * @param num_leds RGB个数
 */
void WS2812::pixel_init(uint16_t num_leds)
{	
	strip.leds = (color_t*) malloc((sizeof(color_t)*num_leds));
	strip.num_leds = num_leds;
	rstRGB(); 
	for (int i = 0; i < num_leds; i++)
	{	
		strip.leds[i].simple.g = 0;
		strip.leds[i].simple.r = 0;
		strip.leds[i].simple.b = 0;
	}
}

//========================================================================
// 描述: RGB显示.
// 参数: none.
// 返回: none.
//========================================================================
void WS2812::pixel_show()
{
	rstRGB(); 
	delay(1);
	for (int i = 0; i < strip.num_leds; i++)
	{
		sendRGB(strip.leds[i].simple.r,strip.leds[i].simple.g,strip.leds[i].simple.b);
	}
}

//========================================================================
// 描述: RGB写入颜色值.
// 参数: index:第几个RGB; red,green,blue:RGB颜色值.
// 返回: 成功 1；失败 0.
//========================================================================
uint8_t WS2812::pixel_set_color(uint16_t index, uint8_t red, uint8_t green, uint8_t blue)
{
	if(_brightness) 
	{ 
		red = (red * _brightness) >> 8;
		green = (green * _brightness) >> 8;
		blue = (blue * _brightness) >> 8;
	}
	if (index < strip.num_leds)
	{
		strip.leds[index].simple.r = red;
		strip.leds[index].simple.g = green;
		strip.leds[index].simple.b = blue;
	}
	else{
		return 1;
	}
	return 0;
}

//========================================================================
// 描述: RGB写入颜色值.
// 参数: index:第几个RGB; 32位WRGB值.
// 返回: 成功 1；失败 0.
//========================================================================
uint8_t WS2812::pixel_set_color(uint16_t index, uint32_t c)
{
	uint8_t r = (uint8_t)(c >> 16);
  	uint8_t g = (uint8_t)(c >>  8);
  	uint8_t b = (uint8_t)c;

	if(_brightness) 
	{ 
		r = (r * _brightness) >> 8;
		g = (g * _brightness) >> 8;
		b = (b * _brightness) >> 8;
	}
	if (index < strip.num_leds)
	{
		strip.leds[index].simple.r = r;
		strip.leds[index].simple.g = g;
		strip.leds[index].simple.b = b;
	}
	else{
		return 1;
	}
	return 0;
}

//========================================================================
// 描述: 全部RGB写入颜色值.
// 参数: red,green,blue:RGB颜色值.
// 返回: none.
//========================================================================
void WS2812::pixel_set_all_color(uint8_t red, uint8_t green, uint8_t blue)
{ 
	if(_brightness) 
	{ 
		red = (red * _brightness) >> 8;
		green = (green * _brightness) >> 8;
		blue = (blue * _brightness) >> 8;
	}
    for (int i = 0; i < strip.num_leds; i++)
	{
		strip.leds[i].simple.g = green;
		strip.leds[i].simple.r = red;
		strip.leds[i].simple.b = blue;
	}
}

//========================================================================
// 描述: RGB设置颜色并显示.
// 参数: index:第几个RGB;red,green,blue:RGB颜色值.
// 返回: none.
//========================================================================
uint8_t WS2812::pixel_set_color_and_show(uint16_t index, uint8_t red, uint8_t green, uint8_t blue )
{
	if(_brightness) 
	{ 
		red = (red * _brightness) >> 8;
		green = (green * _brightness) >> 8;
		blue = (blue * _brightness) >> 8;
	}
	
	if (index < strip.num_leds)
	{
		strip.leds[index].simple.r = red;
		strip.leds[index].simple.g = green;
		strip.leds[index].simple.b = blue;
		pixel_show();
	}else{
		return 1;
	}
	return 0;
}

//========================================================================
// 描述: 关闭RGB.
// 参数: none
// 返回: none.
//========================================================================
void WS2812::pixel_clear()
{
	for (int i = 0; i < strip.num_leds; i++)
	{
		strip.leds[i].simple.g = 0;
		strip.leds[i].simple.r = 0;
		strip.leds[i].simple.b = 0;
	}
	pixel_show();
}

//========================================================================
// 描述: 颜色转换.
// 参数: 0-255颜色值
// 返回: 32位WRGB值.
//========================================================================
uint32_t WS2812::wheel(uint8_t WheelPos)
{
	WheelPos = 255 - WheelPos;
  	if(WheelPos < 85) {
  	  return color(255 - WheelPos * 3, 0, WheelPos * 3);
  	}
  	if(WheelPos < 170) {
  	  WheelPos -= 85;
  	  return color(0, WheelPos * 3, 255 - WheelPos * 3);
  	}
  	WheelPos -= 170;
  	return color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
 
//========================================================================
// 描述: 彩虹循环呼吸效果.
// 参数: 间隔时间
// 返回: none.
//========================================================================
void WS2812::rainbow(uint8_t wait) {
    uint16_t i, j;

    for(j=0; j<256; j++) {
        for(i=0; i<strip.num_leds; i++) {
            pixel_set_color(i, wheel((i+j) & 255));
        }
        pixel_show();
        delay(wait);
    }
}

//========================================================================
// 描述: 流光溢彩效果.
// 参数: 间隔时间
// 返回: none.
//========================================================================
void WS2812::rainbowCycle(uint8_t wait) {
    uint16_t i, j;

    for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
        for(i=0; i< strip.num_leds; i++) {
            pixel_set_color(i, wheel(((i * 256 / strip.num_leds) + j) & 255));
        }
        pixel_show();
        delay(wait);
    }
}

//========================================================================
// 描述: 设置亮度.
// 参数: 亮度值
// 返回: none.
//========================================================================
void WS2812::setBrightness(uint8_t b){
    _brightness = b;
}


#endif  //ws2812.h