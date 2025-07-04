/**
 * @file    GC9A01A.h
 * @brief   TW32F003驱动GC9A01的spi彩屏。
 * @version 0.0.1
 * @date    2023-04-04
 * @author  hhdd
 * @copyright Copyright (c) 2021 TWen51 Technology Co., Ltd.
 */
#ifndef _GC9A01A_H_
#define _GC9A01A_H_

#include "twen_gfx.h"

// #define  GC9A01A_USE_HARDSPI      //使用硬件SPI
#if defined(GC9A01A_USE_HARDSPI)
#include "TW32f0xx_HARDSPI.h"
#else
#include "TW32f0xx_SOFTSPI.h"
#endif

#ifndef GC9A01A_RGB_GBR_ORDER
#define GC9A01A_RGB_GBR_ORDER    1
#endif

#define USE_HORIZONTAL      0  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏

//保存gc9a01a彩屏重要参数
typedef struct
{
	uint16_t width;		//LCD 宽度
	uint16_t height;	//LCD 高度
	uint16_t id;			//LCD ID
	uint8_t  dir;			//彩屏显示方向。	
}_gc9a01a_dev_s;

class GC9A01A : public TWEN_GFX
{
public:
#if defined(GC9A01A_USE_HARDSPI)
	GC9A01A(uint16_t w, uint16_t h, int8_t cs, int8_t res, int8_t a0, int8_t led=-1);
#else
	GC9A01A(uint16_t w, uint16_t h, int8_t cs, int8_t res, int8_t a0,  int8_t sda, 
				int8_t sck, int8_t led=-1);
#endif
	void init();
	void start_write();	//开始写入
	void end_write();	//结束写入
	void backlight(uint8_t bright);	//控制背光
	void set_direction(uint8_t direction);	//设置显示方向
	void set_windows(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1); //设置显示窗口
	void clear(uint16_t color);	//清屏
	void draw_point(int16_t x, int16_t y, uint16_t color); //画点
	void show_picture(uint16_t x,uint16_t y,uint16_t width,uint16_t height,const uint8_t pic[]);	//显示图片

	void fill_rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color);//填充矩形

private:
    GPIO_TypeDef *CS_GPIOx=NULL;
    GPIO_TypeDef *RES_GPIOx=NULL;
    GPIO_TypeDef *A0_GPIOx=NULL;
    GPIO_TypeDef *SDA_GPIOx=NULL;
    GPIO_TypeDef *SCK_GPIOx=NULL;
	GPIO_TypeDef *LED_GPIOx=NULL;
    uint8_t cs_position;
    uint8_t res_position;
    uint8_t a0_position;
    uint8_t sda_position;
    uint8_t sck_position;
	uint8_t led_position;

	_gc9a01a_dev_s lcddev;	//管理LCD重要参数

	void write_8bit(uint8_t dat);	//写入8位数据
	void write_16bit(uint16_t Data);  //写入16位数据
	void write_cmd(uint8_t Reg);	//写命令
};

/**
 * @brief  初始化配置.
 * @param  w    屏幕宽度.
 * @param  h    屏幕高度.
 * @param  cs   CS引脚.
 * @param  res  RES引脚.
 * @param  a0   A0引脚.
 * @param  led  背光引脚.
 * @param  sck  SCK引脚(如果为硬件spi则引脚固定且不可配置，参考TW32f0xx_HARDSPI.h文件).
 * @param  sda  MOSI引脚(如果为硬件spi则引脚固定且不可配置，参考TW32f0xx_HARDSPI.h文件).
 * @return none.
 *     
 */
#if defined(GC9A01A_USE_HARDSPI)
GC9A01A::GC9A01A(uint16_t w, uint16_t h, int8_t cs, int8_t res, int8_t a0, int8_t led):TWEN_GFX(w, h)
{
	lcddev.width = w;
	lcddev.height = h;
    CS_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[cs];
    RES_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[res];
	A0_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[a0];
    cs_position = DIGITAL_PIN_NUM[cs];
    res_position = DIGITAL_PIN_NUM[res];
	a0_position = DIGITAL_PIN_NUM[a0];
	if(led != -1)
	{
		LED_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[led];
		led_position = DIGITAL_PIN_NUM[led];
	}
}
#else
GC9A01A::GC9A01A(uint16_t w, uint16_t h, int8_t cs, int8_t res, int8_t a0,  int8_t sda, 
				    int8_t sck, int8_t led):TWEN_GFX(w, h)
{
	lcddev.width = w;
	lcddev.height = h;
    CS_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[cs];
    RES_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[res];
	A0_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[a0];
	SDA_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[sda];
	SCK_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[sck];
    cs_position = DIGITAL_PIN_NUM[cs];
    res_position = DIGITAL_PIN_NUM[res];
	a0_position = DIGITAL_PIN_NUM[a0];
	sda_position = DIGITAL_PIN_NUM[sda];
	sck_position = DIGITAL_PIN_NUM[sck];
	if(led != -1)
	{
		LED_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[led];
		led_position = DIGITAL_PIN_NUM[led];
	}
}
#endif

/**
 * @brief  彩屏开始写入数据.
 */
void GC9A01A::start_write()
{
	CS_GPIOx->BRR  |= (0x1U << cs_position);  //CS拉低
}


/**
 * @brief  彩屏结束写入数据.
 */
void GC9A01A::end_write()
{
	CS_GPIOx->BSRR |= (0x1U << cs_position);  //CS拉高
}

/**
 * @brief  彩屏写入8位数据.
 * @param  dat	写入的字节数据
 */
void GC9A01A::write_8bit(uint8_t dat)
{
#if defined(GC9A01A_USE_HARDSPI)
	hardspi.write_data(dat);
#else
	for (int bit = 0; bit < 8; bit++) 
	{
		if (dat & 0x80){
			SDA_GPIOx->BSRR |= (0x1U << sda_position);  //MOSI拉高
		}else{
			SDA_GPIOx->BRR |= (0x1U << sda_position);  //MOSI拉低
		}
		SCK_GPIOx->BSRR |= (0x1U << sck_position);  //SCK拉高	
		dat <<= 1;
		SCK_GPIOx->BRR |= (0x1U << sck_position);  //SCK拉低
	}
#endif
}


/**
 * @brief  彩屏写入16位数据.
 * @param  Data	写入的数据
 */
void GC9A01A::write_16bit(uint16_t Data)
{
#if defined(GC9A01A_USE_HARDSPI)
	hardspi.write_data((uint8_t)(Data>>8));
	hardspi.write_data((uint8_t)(Data));
#else
	for (int bit = 0; bit < 16; bit++) 
	{
		if (Data & 0x8000){
			SDA_GPIOx->BSRR |= (0x1U << sda_position);  //MOSI拉高
		}else{
			SDA_GPIOx->BRR |= (0x1U << sda_position);  //MOSI拉低
		}
		SCK_GPIOx->BSRR |= (0x1U << sck_position);  //SCK拉高	
		Data <<= 1;
		SCK_GPIOx->BRR |= (0x1U << sck_position);  //SCK拉低
	}
#endif
}

/**
 * @brief  彩屏写入命令.
 * @param  Reg	写入的命令.
 */
void GC9A01A::write_cmd(uint8_t Reg)	 
{	
	A0_GPIOx->BRR |= (0x1U << a0_position);  //DC拉低
	write_8bit(Reg);
    A0_GPIOx->BSRR |= (0x1U << a0_position);  //DC拉高
}

/**
 * @brief  彩屏初始化.
 */
void GC9A01A::init()
{
#if defined(GC9A01A_USE_HARDSPI)
	hardspi.begin();
#else
	//SCK配置为通用推挽输出模式
    SCK_GPIOx->MODER &= ~(0x3UL << (sck_position * 2u));
    SCK_GPIOx->MODER |= (0x01U << (sck_position * 2u));   //设置为输出模式
    SCK_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (sck_position * 2u));   //设置引脚速度非常高
    SCK_GPIOx->OTYPER  &= ~(0x1UL << sck_position) ;    //复位为推挽输出
	//MOSI配置为通用推挽输出模式
    SDA_GPIOx->MODER &= ~(0x3UL << (sda_position * 2u));
    SDA_GPIOx->MODER |= (0x01U << (sda_position * 2u));   //设置为输出模式
    SDA_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (sda_position * 2u));   //设置引脚速度非常高
    SDA_GPIOx->OTYPER  &= ~(0x1UL << sda_position) ;    //复位为推挽输出
#endif
	//RES配置为通用推挽输出模式
    RES_GPIOx->MODER &= ~(0x3UL << (res_position * 2u));
    RES_GPIOx->MODER |= (0x01U << (res_position * 2u));   //设置为输出模式
    RES_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (res_position * 2u));   //设置引脚速度非常高
    RES_GPIOx->OTYPER  &= ~(0x1UL << res_position);    //复位为推挽输出
	//CS配置为通用推挽输出模式
    CS_GPIOx->MODER &= ~(0x3UL << (cs_position * 2u));
    CS_GPIOx->MODER |= (0x01U << (cs_position * 2u));   //设置为输出模式
    CS_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (cs_position * 2u));   //设置引脚速度非常高
    CS_GPIOx->OTYPER  &= ~(0x1UL << cs_position) ;    //复位为推挽输出
	//A0配置为通用推挽输出模式
    A0_GPIOx->MODER &= ~(0x3UL << (a0_position * 2u));
    A0_GPIOx->MODER |= (0x01U << (a0_position * 2u));   //设置为输出模式
    A0_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (a0_position * 2u));   //设置引脚速度非常高
    A0_GPIOx->OTYPER  &= ~(0x1UL << a0_position) ;    //复位为推挽输出

	RES_GPIOx->BSRR |= (0x1U << res_position);  //RST拉高
	delay(50);
	RES_GPIOx->BRR |= (0x1U << res_position);  //RST拉低
	delay(100);
	RES_GPIOx->BSRR |= (0x1U << res_position);  //RST拉高
	delay(50);

	start_write();
    write_cmd(0xEF);
	write_cmd(0xEB);
	write_8bit(0x14); 
	
    write_cmd(0xFE);			 
	write_cmd(0xEF); 

	write_cmd(0xEB);	
	write_8bit(0x14); 

	write_cmd(0x84);			
	write_8bit(0x40); 

	write_cmd(0x85);			
	write_8bit(0xFF); 

	write_cmd(0x86);			
	write_8bit(0xFF); 

	write_cmd(0x87);			
	write_8bit(0xFF);

	write_cmd(0x88);			
	write_8bit(0x0A);

	write_cmd(0x89);			
	write_8bit(0x21); 

	write_cmd(0x8A);			
	write_8bit(0x00); 

	write_cmd(0x8B);			
	write_8bit(0x80); 

	write_cmd(0x8C);			
	write_8bit(0x01); 

	write_cmd(0x8D);			
	write_8bit(0x01); 

	write_cmd(0x8E);			
	write_8bit(0xFF); 

	write_cmd(0x8F);			
	write_8bit(0xFF); 


	write_cmd(0xB6);
	write_8bit(0x00);
	write_8bit(0x20);

	write_cmd(0x36);    //Set as vertical screen
	write_8bit(0x08);

	write_cmd(0x3A);			
	write_8bit(0x05); 


	write_cmd(0x90);			
	write_8bit(0x08);
	write_8bit(0x08);
	write_8bit(0x08);
	write_8bit(0x08); 

	write_cmd(0xBD);			
	write_8bit(0x06);
	
	write_cmd(0xBC);			
	write_8bit(0x00);	

	write_cmd(0xFF);			
	write_8bit(0x60);
	write_8bit(0x01);
	write_8bit(0x04);

	write_cmd(0xC3);			
	write_8bit(0x13);
	write_cmd(0xC4);			
	write_8bit(0x13);

	write_cmd(0xC9);			
	write_8bit(0x22);

	write_cmd(0xBE);			
	write_8bit(0x11); 

	write_cmd(0xE1);			
	write_8bit(0x10);
	write_8bit(0x0E);

	write_cmd(0xDF);			
	write_8bit(0x21);
	write_8bit(0x0c);
	write_8bit(0x02);

	write_cmd(0xF0);   
	write_8bit(0x45);
	write_8bit(0x09);
	write_8bit(0x08);
	write_8bit(0x08);
	write_8bit(0x26);
 	write_8bit(0x2A);

 	write_cmd(0xF1);    
 	write_8bit(0x43);
 	write_8bit(0x70);
 	write_8bit(0x72);
 	write_8bit(0x36);
 	write_8bit(0x37);  
 	write_8bit(0x6F);


 	write_cmd(0xF2);   
 	write_8bit(0x45);
 	write_8bit(0x09);
 	write_8bit(0x08);
 	write_8bit(0x08);
 	write_8bit(0x26);
 	write_8bit(0x2A);

 	write_cmd(0xF3);   
 	write_8bit(0x43);
 	write_8bit(0x70);
 	write_8bit(0x72);
 	write_8bit(0x36);
 	write_8bit(0x37); 
 	write_8bit(0x6F);

	write_cmd(0xED);	
	write_8bit(0x1B); 
	write_8bit(0x0B); 

	write_cmd(0xAE);			
	write_8bit(0x77);
	
	write_cmd(0xCD);			
	write_8bit(0x63);		


	write_cmd(0x70);			
	write_8bit(0x07);
	write_8bit(0x07);
	write_8bit(0x04);
	write_8bit(0x0E); 
	write_8bit(0x0F); 
	write_8bit(0x09);
	write_8bit(0x07);
	write_8bit(0x08);
	write_8bit(0x03);

	write_cmd(0xE8);			
	write_8bit(0x34);

	write_cmd(0x62);			
	write_8bit(0x18);
	write_8bit(0x0D);
	write_8bit(0x71);
	write_8bit(0xED);
	write_8bit(0x70); 
	write_8bit(0x70);
	write_8bit(0x18);
	write_8bit(0x0F);
	write_8bit(0x71);
	write_8bit(0xEF);
	write_8bit(0x70); 
	write_8bit(0x70);

	write_cmd(0x63);			
	write_8bit(0x18);
	write_8bit(0x11);
	write_8bit(0x71);
	write_8bit(0xF1);
	write_8bit(0x70); 
	write_8bit(0x70);
	write_8bit(0x18);
	write_8bit(0x13);
	write_8bit(0x71);
	write_8bit(0xF3);
	write_8bit(0x70); 
	write_8bit(0x70);

	write_cmd(0x64);			
	write_8bit(0x28);
	write_8bit(0x29);
	write_8bit(0xF1);
	write_8bit(0x01);
	write_8bit(0xF1);
	write_8bit(0x00);
	write_8bit(0x07);

	write_cmd(0x66);			
	write_8bit(0x3C);
	write_8bit(0x00);
	write_8bit(0xCD);
	write_8bit(0x67);
	write_8bit(0x45);
	write_8bit(0x45);
	write_8bit(0x10);
	write_8bit(0x00);
	write_8bit(0x00);
	write_8bit(0x00);

	write_cmd(0x67);			
	write_8bit(0x00);
	write_8bit(0x3C);
	write_8bit(0x00);
	write_8bit(0x00);
	write_8bit(0x00);
	write_8bit(0x01);
	write_8bit(0x54);
	write_8bit(0x10);
	write_8bit(0x32);
	write_8bit(0x98);

	write_cmd(0x74);			
	write_8bit(0x10);	
	write_8bit(0x85);	
	write_8bit(0x80);
	write_8bit(0x00); 
	write_8bit(0x00); 
	write_8bit(0x4E);
	write_8bit(0x00);					
	
    write_cmd(0x98);			
	write_8bit(0x3e);
	write_8bit(0x07);

	write_cmd(0x35);	
	write_cmd(0x21);

	write_cmd(0x11);
	delay(120);
	write_cmd(0x29);
	delay(20);
    end_write();
	set_direction(USE_HORIZONTAL);
}

/**
 * @brief  彩屏设置显示窗口.
 * @param  x0	窗口的起始x坐标.
 * @param  y0	窗口的起始y坐标. 
 * @param  x1	窗口的终点x坐标. 
 * @param  y1	窗口的终点y坐标. 
 */
void GC9A01A::set_windows(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	if(lcddev.dir == 0){
		write_cmd(0x2A);
		write_16bit(x0);
		write_16bit(x1);
		write_cmd(0x2B);
		write_16bit(y0);
		write_16bit(y1); //	
	}else if(lcddev.dir == 1){
		write_cmd(0x2A);
		write_16bit(x0);
		write_16bit(x1);
		write_cmd(0x2B);
		write_16bit(y0);
		write_16bit(y1);	
	}else if(lcddev.dir == 2){
		write_cmd(0x2A);
		write_16bit(x0);
		write_16bit(x1);
		write_cmd(0x2B);
		write_16bit(y0);
		write_16bit(y1);		
	}else if(lcddev.dir == 3){
		write_cmd(0x2A);
		write_16bit(x0);
		write_16bit(x1);
		write_cmd(0x2B);
		write_16bit(y0);
		write_16bit(y1);	
	}else{
		return;
	}	
	write_cmd(0x2C);	
}

/**
 * @brief  彩屏背光控制.
 * @param  bright	0:关闭背光;其他值:打开背光.	
 */
void GC9A01A::backlight(uint8_t bright)
{
    LED_GPIOx->MODER &= ~(0x3UL << (led_position * 2u));
    LED_GPIOx->MODER |= (0x01U << (led_position * 2u));   //设置为输出模式
    LED_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (led_position * 2u));   //设置引脚速度非常高
    LED_GPIOx->OTYPER  &= ~(0x1UL << led_position) ;    //复位为推挽输出
	if(bright == 0)
	{
		LED_GPIOx->BRR |= (0x1U << led_position);  //BLK拉低
	}else{
		LED_GPIOx->BSRR |= (0x1U << led_position);  //BLK拉高
	}
}

/**
 * @brief  设置显示方向.
 * @param  direction	0 - 3.
 * 			0 - 0度，
 * 			1 - 90度，
 * 			2 - 180度，
 * 			3 - 270度。	
 */
void GC9A01A::set_direction(uint8_t direction)
{
	lcddev.dir = direction;
	start_write();
	switch (lcddev.dir) {
		case 0:
			lcddev.width = lcddev.width;
			lcddev.height = lcddev.height;
			/* BIT7:MY; BIT6:MX; BIT5:MV(行列交换); BIT4:ML; BIT3:0,RGB,1,BGR; BIT2:MH */
			write_cmd(0x36);
			write_8bit((0 << 7)|(0 << 6)|(0 << 5)|(0 << 4)|(GC9A01A_RGB_GBR_ORDER <<3));			
			break;
		case 1:
			lcddev.width = lcddev.height;
			lcddev.height = lcddev.width;
			write_cmd(0x36);
			write_8bit((0 << 7)|(1<<6)|(1 << 5)|(0 << 4)|(GC9A01A_RGB_GBR_ORDER << 3));
			break;
		case 2:
			lcddev.width = lcddev.width;
			lcddev.height = lcddev.height;
			write_cmd(0x36);
			write_8bit((1 << 7)|(1 << 6)|(0 << 5)|(0 << 4)|(GC9A01A_RGB_GBR_ORDER << 3));
			break;
		case 3:
			lcddev.width = lcddev.height;
			lcddev.height = lcddev.width;
			write_cmd(0x36);
			write_8bit((1 << 7)|(0 << 6)|(1 << 5)|(0 << 4)|(GC9A01A_RGB_GBR_ORDER << 3));
			break;
		default:
			break;
	}
	TWEN_GFX::set_direction(lcddev.dir);
	end_write();
}

/**
 * @brief  彩屏清屏.
 * @param  color	清成的颜色.
 */
void GC9A01A::clear(uint16_t color)
{
	uint16_t i, j;
	start_write();
	set_windows(0, 0, lcddev.width - 1, lcddev.height - 1);
	for (i = 0; i < lcddev.width; i++)
	{
		for (j = 0; j < lcddev.height; j++)
		{
			write_16bit(color);
		}
	}
	end_write();
}

/**
 * @brief  彩屏画点.
 * @param  x	该点的x坐标.
 * @param  y	该点的y坐标.
 * @param  color	颜色值.
 */
void GC9A01A::draw_point(int16_t x, int16_t y, uint16_t color)
{
	start_write();
	set_windows(x, y, x, y);//设置光标位置 
	write_16bit(color);
	end_write();
}

/**
 * @brief  彩屏填充矩形.
 * @param  x0	矩形起点的x坐标.
 * @param  y0	矩形起点的y坐标.
 * @param  x1	矩形终点的x坐标.
 * @param  y1	矩形终点的y坐标.
 * @param  color	颜色值.
 */
void GC9A01A::fill_rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color)
{
	uint16_t i, j;
	uint16_t width = x1 - x0 + 1; 		//得到填充的宽度
	uint16_t height = y1 - y0 + 1;		//高度
	start_write();
	set_windows(x0, y0, x1, y1);//设置显示窗口
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
			write_16bit(color);
	}
	set_windows(0, 0, lcddev.width - 1, lcddev.height - 1);//恢复窗口设置为全屏
	end_write();
}

/**
 * @brief  彩屏显示图片.
 * @param  x	图片起始点的x坐标.
 * @param  y	图片起始点的y坐标.
 * @param  width	图片宽度.
 * @param  height	图片高度.
 * @param  pic[]	图片数组.
 */
void GC9A01A::show_picture(uint16_t x,uint16_t y,uint16_t width,uint16_t height,const uint8_t pic[])
{
	uint16_t i,j;
	uint32_t k=0;
	start_write();
	set_windows(x,y,x+width-1,y+height-1);
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			write_8bit(pic[k*2]);
			write_8bit(pic[k*2+1]);
			k++;
		}
	}	
	end_write();		
}

#endif // !_GC9A01A_H_