/**
 * @file    ST7735S.h
 * @brief   TW32F003驱动st7735S的spi彩屏驱动程序。
 * @version 0.1
 * @date    2023-01-12
 * @author  hhdd
 * @copyright Copyright (c) 2021 TWen51 Technology Co., Ltd.
 */

#ifndef _ST7735S_H
#define _ST7735S_H

#include "twen_gfx.h"

// #define  ST7735S_USE_HARDSPI      //使用硬件SPI
#if defined(ST7735S_USE_HARDSPI)
#include "TW32f0xx_HARDSPI.h"
#else
#include "TW32f0xx_SOFTSPI.h"
#endif

#ifndef ST7735S_RGB_GBR_ORDER
#define ST7735S_RGB_GBR_ORDER    1
#endif

//颜色
#define TFT_LCD_WHITE         	 0xFFFF
#define TFT_LCD_BLACK         	 0x0000	  
#define TFT_LCD_BLUE             0x001F  
#define TFT_LCD_BRED             0XF81F
#define TFT_LCD_GBLUE            0X07FF
#define TFT_LCD_RED           	 0xF800
#define TFT_LCD_MAGENTA       	 0xF81F
#define TFT_LCD_GREEN         	 0x07E0
#define TFT_LCD_CYAN          	 0x7FFF
#define TFT_LCD_YELLOW        	 0xFFE0
#define TFT_LCD_BROWN            0XBC40 //棕色
#define TFT_LCD_BRRED            0XFC07 //棕红色
#define TFT_LCD_GRAY             0X8430 //灰色
#define TFT_LCD_DARKBLUE      	 0X01CF	//深蓝色
#define TFT_LCD_LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define TFT_LCD_GRAYBLUE       	 0X5458 //灰蓝色
#define TFT_LCD_LIGHTGREEN     	 0X841F //浅绿色
#define TFT_LCD_LGRAY            0XC618 //浅灰色(PANNEL),窗体背景色
#define TFT_LCD_LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define TFT_LCD_LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

#define USE_HORIZONTAL      0  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏

//LCD重要参数集
typedef struct
{
	uint16_t width;		//LCD 宽度
	uint16_t height;	//LCD 高度
	uint16_t id;			//LCD ID
	uint8_t  dir;			//彩屏显示方向。	
	/* 保存真正显示区域的地址信息 */
	uint16_t x_start_width; 	//x方向起始地址到x起始边界的距离
	uint16_t x_end_width; 	//x方向结束地址到x起始边界的距离
	uint16_t y_start_width;	//y方向起始地址到y起始边界的距离
	uint16_t y_end_width; 	//y方向结束地址到y起始边界的距离
}_lcd_dev_s;

class SPITFT : public TWEN_GFX
{
public:
#if defined(ST7735S_USE_HARDSPI)
	SPITFT(uint16_t w, uint16_t h, int8_t cs_pin, int8_t dc_pin, int8_t rst_pin, int8_t blk_pin=-1);
#else
	SPITFT(uint16_t w, uint16_t h, int8_t cs_pin, int8_t sck_pin, int8_t mosi_pin,  int8_t dc_pin, 
				int8_t rst_pin, int8_t blk_pin=-1);
#endif
	void init();
	void start_write();	//开始写入
	void end_write();	//结束写入
	void backlight(uint8_t bright);	//控制背光
	void set_direction(uint8_t direction);	//设置显示方向
	void set_windows(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1); //设置显示窗口
	void clear(uint16_t color);	//清屏
	void draw_point(int16_t x, int16_t y, uint16_t color); //画点
	void show_picture(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint8_t pic[]);	//显示图片

	void fill_rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color);//填充矩形
private:
    GPIO_TypeDef *SCK_GPIOx=NULL;
    GPIO_TypeDef *SI_GPIOx=NULL;
	GPIO_TypeDef *CS_GPIOx=NULL;
	GPIO_TypeDef *DC_GPIOx=NULL;
	GPIO_TypeDef *RST_GPIOx=NULL;
	GPIO_TypeDef *BLK_GPIOx=NULL;
    uint8_t sck_position;
    uint8_t si_position;
	uint8_t cs_position;
	uint8_t dc_position;
	uint8_t rst_position;
	uint8_t blk_position;

	_lcd_dev_s lcddev;	//管理LCD重要参数

	void write_8bit(uint8_t dat);	//写入8位数据
	void write_16bit(uint16_t Data);  //写入16位数据
	void write_cmd(uint8_t Reg);	//写命令
};

/***********************************************************************
 * @brief  初始化配置.
 * @param  w    屏幕宽度.
 * @param  h    屏幕高度.
 * @param  cs_pin     CS引脚.
 * @param  dc_pin     DC引脚.
 * @param  rst_pin    RST引脚.
 * @param  blk_pin    背光引脚.
 * @param  sck_pin    SCK引脚(如果为硬件spi则引脚固定且不可配置，参考TW32f0xx_HARDSPI.h文件).
 * @param  mosi_pin   MOSI引脚(如果为硬件spi则引脚固定且不可配置，参考TW32f0xx_HARDSPI.h文件).
 * @return none.
 *     目前支持80*160, 128*128, 128*160分辨率的彩屏。
 **********************************************************************/
#if defined(ST7735S_USE_HARDSPI)
SPITFT::SPITFT(uint16_t w, uint16_t h, int8_t cs_pin, int8_t dc_pin, int8_t rst_pin, int8_t blk_pin):TWEN_GFX(w, h)
{
	lcddev.width = w;
	lcddev.height = h;
    CS_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[cs_pin];
    DC_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[dc_pin];
	RST_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[rst_pin];
    cs_position = DIGITAL_PIN_NUM[cs_pin];
    dc_position = DIGITAL_PIN_NUM[dc_pin];
	rst_position = DIGITAL_PIN_NUM[rst_pin];
	if(blk_pin != -1)
	{
		BLK_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[blk_pin];
		blk_position = DIGITAL_PIN_NUM[blk_pin];
	}
}
#else
SPITFT::SPITFT(uint16_t w, uint16_t h, int8_t cs_pin, int8_t sck_pin, int8_t mosi_pin, int8_t dc_pin, 
					int8_t rst_pin, int8_t blk_pin):TWEN_GFX(w, h)
{

	lcddev.width = w;
	lcddev.height = h;
    CS_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[cs_pin];
    DC_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[dc_pin];
	RST_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[rst_pin];
	SI_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[mosi_pin];
	SCK_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[sck_pin];
    cs_position = DIGITAL_PIN_NUM[cs_pin];
    dc_position = DIGITAL_PIN_NUM[dc_pin];
	rst_position = DIGITAL_PIN_NUM[rst_pin];
	si_position = DIGITAL_PIN_NUM[mosi_pin];
	sck_position = DIGITAL_PIN_NUM[sck_pin];
	if(blk_pin != -1)
	{
		BLK_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[blk_pin];
		blk_position = DIGITAL_PIN_NUM[blk_pin];
	}
}
#endif

//========================================================================
// 描述: LCD开始写入数据.
// 参数: none.
// 返回: none.
//========================================================================
void SPITFT::start_write()
{
	CS_GPIOx->BRR  |= (0x1U << cs_position);  //CS拉低
}

//========================================================================
// 描述: LCD结束写入数据.
// 参数: none.
// 返回: none.
//========================================================================
void SPITFT::end_write()
{
	CS_GPIOx->BSRR |= (0x1U << cs_position);  //CS拉高
}

//========================================================================
// 描述: LCD写入8位数据.
// 参数: VAL:写入的字节数据.
// 返回: none.
//========================================================================
void SPITFT::write_8bit(uint8_t dat)
{
#if defined(ST7735S_USE_HARDSPI)
	hardspi.write_data(dat);
#else
	for (int bit = 0; bit < 8; bit++) 
	{
		if (dat & 0x80){
			SI_GPIOx->BSRR |= (0x1U << si_position);  //MOSI拉高
		}else{
			SI_GPIOx->BRR |= (0x1U << si_position);  //MOSI拉低
		}
		SCK_GPIOx->BSRR |= (0x1U << sck_position);  //SCK拉高	
		dat <<= 1;
		SCK_GPIOx->BRR |= (0x1U << sck_position);  //SCK拉低
	}
#endif
}

//========================================================================
// 描述: LCD写入16位数据.
// 参数: Data:写入的数据.
// 返回: none.
//========================================================================
void SPITFT::write_16bit(uint16_t Data)
{
#if defined(ST7735S_USE_HARDSPI)
	hardspi.write_data((uint8_t)(Data>>8));
	hardspi.write_data((uint8_t)(Data));
#else
	for (int bit = 0; bit < 16; bit++) 
	{
		if (Data & 0x8000){
			SI_GPIOx->BSRR |= (0x1U << si_position);  //MOSI拉高
		}else{
			SI_GPIOx->BRR |= (0x1U << si_position);  //MOSI拉低
		}
		SCK_GPIOx->BSRR |= (0x1U << sck_position);  //SCK拉高	
		Data <<= 1;
		SCK_GPIOx->BRR |= (0x1U << sck_position);  //SCK拉低
	}
#endif
}

//========================================================================
// 描述: LCD写入命令.
// 参数: Reg:写入的命令.
// 返回: none.
//========================================================================
void SPITFT::write_cmd(uint8_t Reg)	 
{	
	DC_GPIOx->BRR |= (0x1U << dc_position);  //DC拉低
	write_8bit(Reg);
    DC_GPIOx->BSRR |= (0x1U << dc_position);  //DC拉高
}

//========================================================================
// 描述: LCD设置窗口.
// 参数: x0,y0:起始坐标; x1,y1:终点坐标.
// 返回: none.
//========================================================================
void SPITFT::set_windows(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	if(lcddev.dir == 0){
		write_cmd(0x2A);
		write_16bit(x0+lcddev.x_start_width);
		write_16bit(x1+lcddev.x_start_width);
		write_cmd(0x2B);
		write_16bit(y0+lcddev.y_start_width);
		write_16bit(y1+lcddev.y_start_width);	
	}else if(lcddev.dir == 1){
		write_cmd(0x2A);
		write_16bit(x0+lcddev.y_start_width);
		write_16bit(x1+lcddev.y_start_width);
		write_cmd(0x2B);
		write_16bit(y0+lcddev.x_end_width);
		write_16bit(y1+lcddev.x_end_width);	
	}else if(lcddev.dir == 2){
		write_cmd(0x2A);
		write_16bit(x0+lcddev.x_end_width);
		write_16bit(x1+lcddev.x_end_width);
		write_cmd(0x2B);
		write_16bit(y0+lcddev.y_end_width);
		write_16bit(y1+lcddev.y_end_width);		
	}else if(lcddev.dir == 3){
		write_cmd(0x2A);
		write_16bit(x0+lcddev.y_end_width);
		write_16bit(x1+lcddev.y_end_width);
		write_cmd(0x2B);
		write_16bit(y0+lcddev.x_start_width);
		write_16bit(y1+lcddev.x_start_width);	
	}else{
		return;
	}	
	write_cmd(0x2C);						
}

//========================================================================
// 描述: LCD初始化.
// 参数: none.
// 返回: none.
//========================================================================
void SPITFT::init()
{
	if((lcddev.width == 80)&&(lcddev.height == 160)){  //132*162
		lcddev.x_start_width = 26;
		lcddev.x_end_width = 26;
		lcddev.y_start_width = 1;
		lcddev.y_end_width = 1;		
	}else if((lcddev.width == 128)&&(lcddev.height == 128)){  //128*160
		lcddev.x_start_width = 0;
		lcddev.x_end_width = 0;
		lcddev.y_start_width = 0;
		lcddev.y_end_width = 32;
	}else if((lcddev.width == 128)&&(lcddev.height == 160)){  //128*160
		lcddev.x_start_width = 0;
		lcddev.x_end_width = 0;
		lcddev.y_start_width = 0;
		lcddev.y_end_width = 0;
	}else{
		//Add your own  screen 
	}	
#if defined(ST7735S_USE_HARDSPI)
	hardspi.begin();
#else
	//SCK配置为通用推挽输出模式
    SCK_GPIOx->MODER &= ~(0x3UL << (sck_position * 2u));
    SCK_GPIOx->MODER |= (0x01U << (sck_position * 2u));   //设置为输出模式
    SCK_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (sck_position * 2u));   //设置引脚速度非常高
    SCK_GPIOx->OTYPER  &= ~(0x1UL << sck_position) ;    //复位为推挽输出
	//MOSI配置为通用推挽输出模式
    SI_GPIOx->MODER &= ~(0x3UL << (si_position * 2u));
    SI_GPIOx->MODER |= (0x01U << (si_position * 2u));   //设置为输出模式
    SI_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (si_position * 2u));   //设置引脚速度非常高
    SI_GPIOx->OTYPER  &= ~(0x1UL << si_position) ;    //复位为推挽输出
#endif

	//RST配置为通用推挽输出模式
    RST_GPIOx->MODER &= ~(0x3UL << (rst_position * 2u));
    RST_GPIOx->MODER |= (0x01U << (rst_position * 2u));   //设置为输出模式
    RST_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (rst_position * 2u));   //设置引脚速度非常高
    RST_GPIOx->OTYPER  &= ~(0x1UL << rst_position) ;    //复位为推挽输出
	//CS配置为通用推挽输出模式
    CS_GPIOx->MODER &= ~(0x3UL << (cs_position * 2u));
    CS_GPIOx->MODER |= (0x01U << (cs_position * 2u));   //设置为输出模式
    CS_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (cs_position * 2u));   //设置引脚速度非常高
    CS_GPIOx->OTYPER  &= ~(0x1UL << cs_position) ;    //复位为推挽输出
	//DC配置为通用推挽输出模式
    DC_GPIOx->MODER &= ~(0x3UL << (dc_position * 2u));
    DC_GPIOx->MODER |= (0x01U << (dc_position * 2u));   //设置为输出模式
    DC_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (dc_position * 2u));   //设置引脚速度非常高
    DC_GPIOx->OTYPER  &= ~(0x1UL << dc_position) ;    //复位为推挽输出

	RST_GPIOx->BSRR |= (0x1U << rst_position);  //RST拉高
	delay(50);
	RST_GPIOx->BRR |= (0x1U << rst_position);  //RST拉低
	delay(100);
	RST_GPIOx->BSRR |= (0x1U << rst_position);  //RST拉高
	delay(50);

	start_write();
    write_cmd(0x11); //Exit Sleep
    delay(50);	
    write_cmd(0xB1);
    write_8bit(0x02);
    write_8bit(0x35);
    write_8bit(0x36);

    write_cmd(0xB2);
    write_8bit(0x02);
    write_8bit(0x35);
    write_8bit(0x36);

    write_cmd(0xB3);
    write_8bit(0x02);
    write_8bit(0x35);
    write_8bit(0x36);
    write_8bit(0x02);
    write_8bit(0x35);
    write_8bit(0x36);
		
    //------------------------------------End ST7735S Frame Rate-----------------------------------------//
    write_cmd(0xB4); //Dot inversion
    write_8bit(0x03); //03

    write_cmd(0xC0);
    write_8bit(0xa2);
    write_8bit(0x02);
    write_8bit(0x84);

    write_cmd(0xC1);
    write_8bit(0XC5);

    write_cmd(0xC2);
    write_8bit(0x0D);
    write_8bit(0x00);

    write_cmd(0xC3);
    write_8bit(0x8D);
    write_8bit(0x2A);

    write_cmd(0xC4);
    write_8bit(0x8D);
    write_8bit(0xEE);
    //---------------------------------End ST7735S Power Sequence-------------------------------------//
    write_cmd(0xC5); //VCOM
    write_8bit(0x03);
	
	write_cmd(0x3a); //Set Color Format
	write_8bit(0x05);
    //------------------------------------ST7735S Gamma Sequence-----------------------------------------//
    write_cmd(0xE0);
    write_8bit(0x12);
    write_8bit(0x1C);
    write_8bit(0x10);
    write_8bit(0x18);
    write_8bit(0x33);
    write_8bit(0x2c);
    write_8bit(0x25);
    write_8bit(0x28);
    write_8bit(0x28);
    write_8bit(0x27);
    write_8bit(0x2f);
    write_8bit(0x3C);
    write_8bit(0x00);
    write_8bit(0x03);
    write_8bit(0x03);
    write_8bit(0x10);

    write_cmd(0xE1);
    write_8bit(0x12);
    write_8bit(0x1c);
    write_8bit(0x10);
    write_8bit(0x18);
    write_8bit(0x2d);
    write_8bit(0x28);
    write_8bit(0x23);
    write_8bit(0x28);
    write_8bit(0x28);
    write_8bit(0x26);
    write_8bit(0x2f);
    write_8bit(0x3B);
    write_8bit(0x00);
    write_8bit(0x03);
    write_8bit(0x03);
    write_8bit(0x10);

	// write_cmd(0x30);             
	// write_8bit(0x00);             
	// write_8bit(0x00);             
	// write_8bit(0x00);             
	// write_8bit(0x80);

	if((lcddev.width==80)&&(lcddev.height==160))
	{
		write_cmd(0x21); //Display Inversion On
	}	 
	write_cmd(0x12);
	write_cmd(0x29); // Display On
	set_direction(USE_HORIZONTAL);
}

//========================================================================
// 描述: LCD背光控制.
// 参数: 0：关闭背光；其他值：打开背光.
// 返回: none.
//========================================================================
void SPITFT::backlight(uint8_t bright)
{
    BLK_GPIOx->MODER &= ~(0x3UL << (blk_position * 2u));
    BLK_GPIOx->MODER |= (0x01U << (blk_position * 2u));   //设置为输出模式
    BLK_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (blk_position * 2u));   //设置引脚速度非常高
    BLK_GPIOx->OTYPER  &= ~(0x1UL << blk_position) ;    //复位为推挽输出
	if(bright == 0)
	{
		BLK_GPIOx->BRR |= (0x1U << blk_position);  //BLK拉低
	}else{
		BLK_GPIOx->BSRR |= (0x1U << blk_position);  //BLK拉高
	}
}

//========================================================================
// 描述: 设置显示方向.
// 参数: direction:0~3;
// 返回: none.
//========================================================================
void SPITFT::set_direction(uint8_t direction)
{
	lcddev.dir = direction;
	start_write();
	switch (lcddev.dir) {
		case 0:
			lcddev.width = lcddev.width;
			lcddev.height = lcddev.height;
			/* BIT7:MY; BIT6:MX; BIT5:MV(行列交换); BIT4:ML; BIT3:0,RGB,1,BGR; BIT2:MH */
			write_cmd(0x36);
			write_8bit((0 << 7)|(0 << 6)|(0 << 5)|(0 << 4)|(ST7735S_RGB_GBR_ORDER <<3));			
			break;
		case 1:
			lcddev.width = lcddev.height;
			lcddev.height = lcddev.width;
			write_cmd(0x36);
			write_8bit((0 << 7)|(1<<6)|(1 << 5)|(0 << 4)|(ST7735S_RGB_GBR_ORDER << 3));
			break;
		case 2:
			lcddev.width = lcddev.width;
			lcddev.height = lcddev.height;
			write_cmd(0x36);
			write_8bit((1 << 7)|(1 << 6)|(0 << 5)|(0 << 4)|(ST7735S_RGB_GBR_ORDER << 3));
			break;
		case 3:
			lcddev.width = lcddev.height;
			lcddev.height = lcddev.width;
			write_cmd(0x36);
			write_8bit((1 << 7)|(0 << 6)|(1 << 5)|(0 << 4)|(ST7735S_RGB_GBR_ORDER << 3));
			break;
		default:
			break;
	}
	TWEN_GFX::set_direction(lcddev.dir);
	end_write();
}


//========================================================================
// 描述: LCD清屏.
// 参数: color:清屏的颜色.
// 返回: none.
//========================================================================
void SPITFT::clear(uint16_t color)
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

//========================================================================
// 描述: LCD画点.
// 参数: x,y: 坐标.
// 返回: none.
//========================================================================
void SPITFT::draw_point(int16_t x, int16_t y, uint16_t color)
{
	start_write();
	set_windows(x, y, x, y);//设置光标位置 
	write_16bit(color);
	end_write();
}

//========================================================================
// 描述: LCD填充矩形.
// 参数: x0，y0:起始坐标; x1,y1:终止坐标; color:颜色
// 返回: none.
//========================================================================
void SPITFT::fill_rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color)
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

//========================================================================
// 描述: LCD显示图片.
// 参数: none.
// 返回: none.
//========================================================================
void SPITFT::show_picture(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint8_t pic[])
{
	uint16_t i,j;
	uint32_t k=0;
	start_write();
	set_windows(x,y,x+length-1,y+width-1);
	for(i=0;i<length;i++)
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

#endif  //st7735s.h