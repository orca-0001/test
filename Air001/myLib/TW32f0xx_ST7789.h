/**
 * @file ST7789.h
 * @brief ST7789彩屏驱动头文件
 * @author LI (com)
 * @version 1.0
 * @date 2023-09-25
 * 
 * @copyright Copyright (c) 2023 haohaodada.com
 * 
 /**
  * 2024-4-8 修改旋转屏幕方向函数 ，更改偏移量 部分屏幕分辨率没有需要自己添加
 /**
  * 2024-4-19 修改初始化函数 ST7789支持两种初始化模式
 */
#ifndef __ST7789_H_
#define __ST7789_H_	

#include "twen_font.h"
#include <stdlib.h>
#include "TW32f0xx.h"
#include "twen_gfx.h"
#include "wiring.h"
#include "myLib/TW32f0xx_HARDSPI.h"

/************************************* 用户配置区 *****************************************/ 
#ifndef ST7789_RGB_ORDER			
#define	ST7789_RGB_ORDER			0	//RGB和BGR切换位
#endif

#ifndef ST7789_DATA_PORT_TYPE
#define ST7789_DATA_PORT_TYPE       0    //0：低8位 1：高8位 2 ：16位
#endif

#ifndef ST7789_USE_SPI_80_MODE	         //使用80并口或者SPI
#define ST7789_USE_SPI_80_MODE      0    //0:80并口   1：软件SPI   2:使用硬件SPI;
#endif

#ifndef ST7789_driver_command
#define ST7789_driver_command 0     //ST7789驱动 若屏幕颜色不对可以选择更改驱动(0/1)（1->ST7789   0->ST7789_2）
#endif

//LCD重要参数集
typedef struct
{
	uint16_t width;		//LCD 宽度
	uint16_t height;	//LCD 高度
	uint16_t id;			//LCD ID
	uint8_t  dir;			//彩屏显示方向。	
	/* 保存真正显示区域的地址信息 */
	uint8_t colstart ;
	uint8_t rowstart ; // Screen display area to CGRAM area coordinate offsets
}_st7789_lcd_dev;

//LCD参数
_st7789_lcd_dev  st7789_lcddev;	//管理LCD重要参数


//颜色
#define ST7789_WHITE         	 	0xFFFF
#define ST7789_BLACK         	 	0x0000	  
#define ST7789_BLUE             	0x001F  
#define ST7789_BRED             	0XF81F
#define ST7789_GBLUE            	0X07FF
#define ST7789_RED           	 	0xF800
#define ST7789_MAGENTA       	 	0xF81F
#define ST7789_GREEN         	 	0x07E0
#define ST7789_CYAN          	 	0x7FFF
#define ST7789_YELLOW        	 	0xFFE0
#define ST7789_BROWN            	0XBC40 //棕色
#define ST7789_BRRED            	0XFC07 //棕红色
#define ST7789_GRAY             	0X8430 //灰色
#define ST7789_DARKBLUE      	 	0X01CF	//深蓝色
#define ST7789_LIGHTBLUE      	 	0X7D7C	//浅蓝色  
#define ST7789_GRAYBLUE       	 	0X5458 //灰蓝色
#define ST7789_LIGHTGREEN     	 	0X841F //浅绿色
#define ST7789_LGRAY            	0XC618 //浅灰色(PANNEL),窗体背景色
#define ST7789_LGRAYBLUE        	0XA651 //浅灰蓝色(中间层颜色)
#define ST7789_LBBLUE           	0X2B12 //浅棕蓝色(选择条目的反色)



class ST7789 : public TWEN_GFX
{
public:
	//8位并口模式 
	ST7789(uint16_t w, uint16_t h,GPIO_TypeDef*  GPIO_DATA,int8_t rd_pin, int8_t wr_pin, int8_t rs_pin, 
									int8_t cs_pin, int8_t rst_pin);
	//软件SPI串口模式
	ST7789(uint16_t w, uint16_t h,int8_t scl_pin,int8_t sda_pin,int8_t rst_pin,int8_t dc_pin,int8_t cs_pin,int8_t blk_pin);
	
	//硬件SPI串口模式
	ST7789(uint16_t w, uint16_t h,int8_t cs_pin,int8_t dc_pin,int8_t rst_pin,int8_t blk_pin);

	void init();

 	void start_write();
	void end_write();
	void set_windows(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	void clear(uint16_t color);
	void set_direction(uint8_t direction);
	void set_direction(uint8_t direction,uint16_t OFFSET_1,uint16_t OFFSET_2);
	void draw_point(int16_t x, int16_t y, uint16_t color); //画点
	void fill_rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color);
    void show_picture(int16_t x,int16_t y,int16_t length,int16_t width,const int8_t pic[]);

private:
    #if (ST7789_USE_SPI_80_MODE==2)
	HARDSPI hardspi;
	#endif
	void write_8bit(uint8_t HVAL);
	void write_cmd(uint8_t Reg);
	void write_16bit(uint16_t Data);
	GPIO_TypeDef* DATA_GPIOx = NULL;
	uint8_t dc_pin;
	uint8_t sda_pin;
	uint8_t scl_pin;
	uint8_t blk_pin;
	uint8_t rd_pin;
	uint8_t wr_pin;
	uint8_t cs_pin;
	uint8_t rs_pin;
	uint8_t rst_pin;
 	#if (ST7789_USE_SPI_80_MODE==0)
    inline void st7789_w_data(uint16_t dat)		
	{
    #if ST7789_DATA_PORT_TYPE==0     //低8位
        DATA_GPIOx->ODR = dat | (DATA_GPIOx->ODR &~((uint32_t)(0xff)));
    #elif  ST7789_DATA_PORT_TYPE==1      //高8位
        DATA_GPIOx->ODR = (dat<<8) | (DATA_GPIOx->ODR &~((uint32_t)(0xff<<8)));
    #else   //16位并口
        DATA_GPIOx->ODR = ((uint16_t)dat);
    #endif
    }
	#endif
};

/**
 * @brief Construct a new tw32f0xx st7789::tw32f0xx st7789 object
 * @param  w                屏幕宽度
 * @param  h                屏幕高度
 * @param  GPIO_DATA        数据端口
 * @param  rd_pin           RD引脚
 * @param  wr_pin           WR引脚
 * @param  cs_pin           CS引脚
 * @param  rs_pin           RS引脚
 * @param  rst_pin          RST引脚    
 */
ST7789::ST7789(uint16_t w, uint16_t h,GPIO_TypeDef*  GPIO_DATA,int8_t rd_pin, int8_t wr_pin, int8_t rs_pin, 
									int8_t cs_pin, int8_t rst_pin):TWEN_GFX(w, h)
{
	st7789_lcddev.width = w;
	st7789_lcddev.height = h;
	DATA_GPIOx = GPIO_DATA;
	this->rd_pin = rd_pin;
	this->wr_pin = wr_pin;
	this->cs_pin = cs_pin;
	this->rs_pin = rs_pin;
	this->rst_pin = rst_pin;

}
/**
 * @brief Construct a new ST7789::ST7789 object
 * @param  w                屏幕宽度
 * @param  h                屏幕高度
 * @param  cs_pin           片选
 * @param  dc_pin           数据/命令
 * @param  rst_pin          复位
 * @param  sda_pin          数据MOSI
 * @param  scl_pin          时钟SLK
 * @param  blk_pin          背光
 */
ST7789::ST7789(uint16_t w, uint16_t h,int8_t scl_pin,int8_t sda_pin,int8_t rst_pin,int8_t dc_pin,int8_t cs_pin,int8_t blk_pin):TWEN_GFX(w, h)
{
	st7789_lcddev.width = w;
	st7789_lcddev.height = h;
	this->cs_pin = cs_pin;
	this->dc_pin = dc_pin;
	this->rst_pin = rst_pin;
	this->sda_pin = sda_pin;
	this->scl_pin = scl_pin;
	this->blk_pin = blk_pin;
}
/**
 * @brief Construct a new ST7789::ST7789 object
 * @param  w                屏幕宽度
 * @param  h                屏幕高度
 * @param  cs_pin           片选
 * @param  dc_pin           数据/命令
 * @param  rst_pin          复位
 * @param  blk_pin          背光
 */
ST7789::ST7789(uint16_t w, uint16_t h,int8_t rst_pin,int8_t dc_pin,int8_t cs_pin,int8_t blk_pin):TWEN_GFX(w, h)
{
	st7789_lcddev.width = w;
	st7789_lcddev.height = h;
	this->cs_pin = cs_pin;
	this->dc_pin = dc_pin;
	this->rst_pin = rst_pin;
	this->blk_pin = blk_pin;
}
/**
 * @brief 初始化屏幕
 */
void ST7789::init()
{

	//设置成推挽输出
	//端口初始化

    #if (ST7789_USE_SPI_80_MODE==0)  //80并口
		#if (ST7789_DATA_PORT_TYPE==0)     //低8位
			DATA_GPIOx->PUPDR = 0x0UL;      //无上拉
			DATA_GPIOx->MODER = 0x5555 | (DATA_GPIOx->MODER &~((uint32_t)(0xffff)));
			DATA_GPIOx->OTYPER &= 0xffff0000UL;        //推挽输出
			DATA_GPIOx->OSPEEDR = 0xFFFFFFFF;    //引脚设置为高速模式
		#elif  (ST7789_DATA_PORT_TYPE==1)      //高8位
			DATA_GPIOx->PUPDR = 0x0UL;      //无上拉
			DATA_GPIOx->MODER = (0x5555<<8) | (DATA_GPIOx->MODER &~((uint32_t)(0xffff<<8)));
			DATA_GPIOx->OTYPER &= 0x0000ffffUL;        //推挽输出
			DATA_GPIOx->OSPEEDR = 0xFFFFFFFF;    //引脚设置为高速模式
		#else   //16位并口
			DATA_GPIOx->PUPDR = 0x0UL;      //无上拉
			DATA_GPIOx->MODER = 0x55555555 ;
			DATA_GPIOx->OTYPER &= 0x0;        //推挽输出
			DATA_GPIOx->OSPEEDR = 0xFFFFFFFF;    //引脚设置为高速模式
		#endif
		pinMode((PIN_Name)rd_pin,GPIO_Mode_Out_PP);
		pinMode((PIN_Name)wr_pin,GPIO_Mode_Out_PP);
		pinMode((PIN_Name)cs_pin,GPIO_Mode_Out_PP);
		pinMode((PIN_Name)rs_pin,GPIO_Mode_Out_PP);
		pinMode((PIN_Name)rst_pin,GPIO_Mode_Out_PP);
	#elif (ST7789_USE_SPI_80_MODE==1)  //软件SPI    
		pinMode((PIN_Name)sda_pin, GPIO_Mode_Out_PP);
	    pinMode((PIN_Name)scl_pin, GPIO_Mode_Out_PP);
		pinMode((PIN_Name)cs_pin,GPIO_Mode_Out_PP);
		pinMode((PIN_Name)dc_pin,GPIO_Mode_Out_PP);
		pinMode((PIN_Name)rst_pin,GPIO_Mode_Out_PP);
		pinMode((PIN_Name)blk_pin,GPIO_Mode_Out_PP);
		digitalWrite((PIN_Name)sda_pin,1);
		digitalWrite((PIN_Name)scl_pin,1);
		digitalWrite((PIN_Name)cs_pin,1);
		digitalWrite((PIN_Name)dc_pin,1);
		digitalWrite((PIN_Name)rst_pin,1);
		digitalWrite((PIN_Name)blk_pin,1);
	#elif  (ST7789_USE_SPI_80_MODE==2)//硬件SPI  
		hardspi.begin();
		hardspi.set_speed(0);
		hardspi.set_mode(1,1);

		pinMode((PIN_Name)cs_pin,GPIO_Mode_Out_PP);
		pinMode((PIN_Name)dc_pin,GPIO_Mode_Out_PP);
		pinMode((PIN_Name)rst_pin,GPIO_Mode_Out_PP);
		pinMode((PIN_Name)blk_pin,GPIO_Mode_Out_PP);
		digitalWrite((PIN_Name)cs_pin,1);
		digitalWrite((PIN_Name)dc_pin,1);
		digitalWrite((PIN_Name)rst_pin,1);
		digitalWrite((PIN_Name)blk_pin,1);
	#endif


	//初始化之前复位
	digitalWrite((PIN_Name)rst_pin,0);
	delay(100);
	digitalWrite((PIN_Name)rst_pin,1);
	delay(100);

	#if (ST7789_USE_SPI_80_MODE==0)  //80并口
	write_cmd(0x11);   //sleep out
	delay(120); 
	//----------------display and color format setting------------------//
    write_cmd(0x36);
    write_8bit(0x00);
    write_cmd(0x3a);
    write_8bit(0x05);
	//----------------------ST7789 Frame rate setting--------------------//
    write_cmd(0xb2);
    write_8bit(0x0c);
    write_8bit(0x0c);
    write_8bit(0x00);
    write_8bit(0x33);
    write_8bit(0x33);
    write_cmd(0xb7);
    write_8bit(0x35);
    //----------------------ST7789 Power setting--------------------------//
    write_cmd(0xbb);
    write_8bit(0x1c);
    write_cmd(0xc0);
    write_8bit(0x2c);
    write_cmd(0xc2);
    write_8bit(0x01);
    write_cmd(0xc3);
    write_8bit(0x0b);
    write_cmd(0xc4);
    write_8bit(0x20);
    write_cmd(0xc6);
    write_8bit(0x0f);
    write_cmd(0xd0);
    write_8bit(0xa4);
    write_8bit(0xa1);
    //-----------------------ST7789 gamma setting--------------------------//
    write_cmd(0xe0);
    write_8bit(0xd0);
    write_8bit(0x00);
    write_8bit(0x03);
    write_8bit(0x09);
    write_8bit(0x13);
    write_8bit(0x1c);
    write_8bit(0x3a);
    write_8bit(0x55);
    write_8bit(0x48);
    write_8bit(0x18);
    write_8bit(0x12);
    write_8bit(0x0e);
    write_8bit(0x19);
    write_8bit(0x1e);

    write_cmd(0xe1);
    write_8bit(0xd0);
    write_8bit(0x00);
    write_8bit(0x03);
    write_8bit(0x09);
    write_8bit(0x05);
    write_8bit(0x25);
    write_8bit(0x3a);
    write_8bit(0x55);
    write_8bit(0x50);
    write_8bit(0x3d);
    write_8bit(0x1c);
    write_8bit(0x1d);
    write_8bit(0x1d);
    write_8bit(0x1e);
    write_cmd(0x29);
	#else      //SPI
	#if (ST7789_driver_command == 1)
	write_cmd(0x11);//sleep out
	delay(120);

	write_cmd(0x13);    // Normal display mode on

	write_cmd(0x36);
	write_8bit(0x0);

	write_cmd(0xB6);
	write_8bit(0x0A);
	write_8bit(0x82);	

	write_cmd(0xB0);
	write_8bit(0x00);
	write_8bit(0xE0); // 5 to 6 bit conversion: r0 = r5, b0 = b5

	write_cmd(0x3a);
	write_8bit(0x55);
	delay(10);

	write_cmd(0xb2);
	write_8bit(0x0c);
	write_8bit(0x0c);
	write_8bit(0x00);
	write_8bit(0x33);
	write_8bit(0x33);

	write_cmd(0xb7);      // Voltages: VGH / VGL
	write_8bit(0x35);

	write_cmd(0xbb);
	write_8bit(0x28);		// JLX240 display datasheet

	write_cmd(0xc0);
	write_8bit(0x0C);

	write_cmd(0xc2);
	write_8bit(0x01);
	write_8bit(0xFF);

	write_cmd(0xc3);       // voltage VRHS
	write_8bit(0x10);

	write_cmd(0xc4);
	write_8bit(0x20);

	write_cmd(0xc6);
	write_8bit(0x0f);

	write_cmd(0xd0);
	write_8bit(0xa4);
	write_8bit(0xa1);

	//--------------------------------ST7789V gamma setting---------------------------------------//
	write_cmd(0xe0);
	write_8bit(0xd0);
	write_8bit(0x00);
	write_8bit(0x02);
	write_8bit(0x07);
	write_8bit(0x0a);
	write_8bit(0x28);
	write_8bit(0x32);
	write_8bit(0x44);
	write_8bit(0x42);
	write_8bit(0x06);
	write_8bit(0x0e);
	write_8bit(0x12);
	write_8bit(0x14);
	write_8bit(0x17);

	write_cmd(0xe1);
	write_8bit(0xd0);
	write_8bit(0x00);
	write_8bit(0x02);
	write_8bit(0x07);
	write_8bit(0x0a);
	write_8bit(0x28);
	write_8bit(0x31);
	write_8bit(0x54);
	write_8bit(0x47);
	write_8bit(0x0e);
	write_8bit(0x1c);
	write_8bit(0x17);
	write_8bit(0x1b);
	write_8bit(0x1e);

	write_cmd(0x21);

	write_cmd(0x2a);    // Column address set
	write_8bit(0x00);
	write_8bit(0x00);
	write_8bit(0x00);
	write_8bit(0xEF);    // 239

	write_cmd(0x2b);    // Row address set
	write_8bit(0x00);
	write_8bit(0x00);
	write_8bit(0x01);
	write_8bit(0x3F);    // 319

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	delay(120);
	write_cmd(0x29);    //Display on
	delay(120);

	#else
	write_cmd(0x11);
	delay(255);

	write_cmd(0x3A);
	write_8bit(0x55);   
	delay(10);

	write_cmd(0x36);
	write_8bit(0x0);   

	write_cmd(0x2A);
	write_8bit(0x0);   
	write_8bit(0x0); 
	write_8bit(0x0); 
	write_8bit(0xF0); 

	write_cmd(0x2B);
	write_8bit(0x0);   
	write_8bit(0x0); 
	write_8bit(0x0); 
	write_8bit(0xF0); 

	write_cmd(0x21);
	delay(10);

	write_cmd(0x13);
	delay(10);

	write_cmd(0x29);
	delay(255);
	#endif

	#endif

	set_text_color(ST7789_RED,ST7789_WHITE);
	set_bg_mode(0);  //不叠加方式显示
	set_direction(0);//设置LCD显示方向 
}

/**
 * @brief  彩屏开始写入数据.
 */
void ST7789::start_write()
{
	digitalWrite((PIN_Name)cs_pin,0);  //CS拉低
}

/**
 * @brief  彩屏结束写入数据.
 */
void ST7789::end_write()
{
	digitalWrite((PIN_Name)cs_pin,1);  //CS拉高
}


/**
 * @brief LCD写入一个字节数据.
 * @param  HVAL  写入的字节数据.
 */
void ST7789::write_8bit(uint8_t HVAL)
{
	#if (ST7789_USE_SPI_80_MODE==0)  //80并口
	digitalWrite((PIN_Name)rs_pin,1);
	digitalWrite((PIN_Name)rd_pin,1);
	st7789_w_data(HVAL);
	digitalWrite((PIN_Name)cs_pin,0);
	digitalWrite((PIN_Name)wr_pin,0);

	digitalWrite((PIN_Name)wr_pin,1);
	digitalWrite((PIN_Name)cs_pin,1);
	#elif (ST7789_USE_SPI_80_MODE==1)  
	digitalWrite((PIN_Name)cs_pin,0);
	for (uint8_t bit = 0; bit < 8; bit++) 
	{
		digitalWrite((PIN_Name)scl_pin,0);
		if (HVAL & 0x80)
			digitalWrite((PIN_Name)sda_pin,1);
		else
			digitalWrite((PIN_Name)sda_pin,0);
		digitalWrite((PIN_Name)scl_pin,1);
		HVAL <<= 1;
	}     
	digitalWrite((PIN_Name)cs_pin,1);     
	#elif (ST7789_USE_SPI_80_MODE==2)  
	digitalWrite((PIN_Name)cs_pin,0);
	hardspi.write_data(HVAL);
	digitalWrite((PIN_Name)cs_pin,1); 
	#endif
}

/**
 * @brief LCD写入二个字节数据.
 * @param  Data      
 */
void ST7789::write_16bit(uint16_t Data)
{
	#if (ST7789_USE_SPI_80_MODE==0)  //80并口
	digitalWrite((PIN_Name)rs_pin,1);
	digitalWrite((PIN_Name)rd_pin,1);
	st7789_w_data(Data>>8);
	digitalWrite((PIN_Name)cs_pin,0);
	digitalWrite((PIN_Name)wr_pin,0);

	digitalWrite((PIN_Name)wr_pin,1);	
	st7789_w_data(Data&0xff);
	digitalWrite((PIN_Name)cs_pin,0);
	digitalWrite((PIN_Name)wr_pin,0);

	digitalWrite((PIN_Name)wr_pin,1);
	digitalWrite((PIN_Name)cs_pin,1);
	#else
	write_8bit(Data>>8);
	write_8bit(Data);
	#endif
}

/**
 * @brief LCD写入一字节命令.
 * @param  Reg              My Param doc
 */
void ST7789::write_cmd(uint8_t Reg)	 
{	
	#if (ST7789_USE_SPI_80_MODE==0)  //80并口
	digitalWrite((PIN_Name)rs_pin,0);
	digitalWrite((PIN_Name)rd_pin,1);
	st7789_w_data(Reg);
	digitalWrite((PIN_Name)cs_pin,0);
    digitalWrite((PIN_Name)wr_pin,0);
	digitalWrite((PIN_Name)wr_pin,1);
	digitalWrite((PIN_Name)cs_pin,1);
	#else
	digitalWrite((PIN_Name)dc_pin,0);
	write_8bit(Reg);
	digitalWrite((PIN_Name)dc_pin,1);
	#endif
}
/**
 * @brief 设置窗口
 * @param  x0               起始x坐标
 * @param  y0               起始y坐标
 * @param  x1               结束x坐标
 * @param  y1               结束y坐标
 */
void ST7789::set_windows(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	// if(st7789_lcddev.dir == 0){
		write_cmd(0x2A);
		write_16bit(x0+st7789_lcddev.colstart);
		write_16bit(x1+st7789_lcddev.colstart);
		write_cmd(0x2B);
		write_16bit(y0+st7789_lcddev.rowstart);
		write_16bit(y1+st7789_lcddev.rowstart);	
	// }else if(st7789_lcddev.dir == 1){
	// 	write_cmd(0x2A);
	// 	write_16bit(x0+st7789_lcddev.y_start_width);
	// 	write_16bit(x1+st7789_lcddev.y_start_width);
	// 	write_cmd(0x2B);
	// 	write_16bit(y0+st7789_lcddev.x_end_width);
	// 	write_16bit(y1+st7789_lcddev.x_end_width);	
	// }else if(st7789_lcddev.dir == 2){
	// 	write_cmd(0x2A);
	// 	write_16bit(x0+st7789_lcddev.x_end_width);
	// 	write_16bit(x1+st7789_lcddev.x_end_width);
	// 	write_cmd(0x2B);
	// 	write_16bit(y0+st7789_lcddev.y_end_width);
	// 	write_16bit(y1+st7789_lcddev.y_end_width);		
	// }else if(st7789_lcddev.dir == 3){
	// 	write_cmd(0x2A);
	// 	write_16bit(x0+st7789_lcddev.y_end_width);
	// 	write_16bit(x1+st7789_lcddev.y_end_width);
	// 	write_cmd(0x2B);
	// 	write_16bit(y0+st7789_lcddev.x_start_width);
	// 	write_16bit(y1+st7789_lcddev.x_start_width);	
	// }else{
	// 	return;
	// }	                

	write_cmd(0x2c);	//开始写入GRAM						
}
/**
 * @brief 以color颜色清屏
 * @param  color            RGB值 颜色 （红色-0xf800）
 */
void ST7789::clear(uint16_t color)
{
	uint16_t i, j;
	set_windows(0, 0, st7789_lcddev.width - 1, st7789_lcddev.height - 1);
	for (i = 0; i < st7789_lcddev.width; i++)
	{
		for (j = 0; j < st7789_lcddev.height; j++)
		{
			write_16bit(color);
		}
	}
}
/**
 * @brief 设置屏幕显示方向
 * @param  direction        （0-3）
 */
void ST7789::set_direction(uint8_t direction)
{
	st7789_lcddev.dir = direction;
	uint16_t width = 0;
	uint16_t height = 0;
	width = st7789_lcddev.width;
	height = st7789_lcddev.height;
	switch (st7789_lcddev.dir) {
		case 0:
			st7789_lcddev.width = width;
			st7789_lcddev.height = height;
			if (width == 135)
			{
				st7789_lcddev.colstart = 52;
				st7789_lcddev.rowstart = 40;
			}
			else if(height == 280)
			{
				st7789_lcddev.colstart = 0;
				st7789_lcddev.rowstart = 20;
			}
			else if(width == 172)
			{
				st7789_lcddev.colstart = 34;
				st7789_lcddev.rowstart = 0;
			}
			else if(width == 170)
			{
				st7789_lcddev.colstart = 35;
				st7789_lcddev.rowstart = 0;
			}
			else  
			{
				st7789_lcddev.colstart = 0;
				st7789_lcddev.rowstart = 0;
			}
            write_cmd(0x36);
            /* BIT7:MY; BIT6:MX; BIT5:MV(行列交换); BIT4:ML; BIT3:0,RGB,1,BGR; BIT2:MH */
            write_8bit((0<<7)|(0<<6)|(0<<5)|(0<<4)|(ST7789_RGB_ORDER<<3));
			break;
		case 1:
			st7789_lcddev.width = height;
			st7789_lcddev.height = width;
			if (width == 135)
			{
				st7789_lcddev.colstart = 40;
				st7789_lcddev.rowstart = 53;
			}
			else if(height == 280)
			{
				st7789_lcddev.colstart = 20;
				st7789_lcddev.rowstart = 0;
			}
			else if(width == 172)
			{
				st7789_lcddev.colstart = 0;
				st7789_lcddev.rowstart = 34;
			}
			else if(width == 170)
			{
				st7789_lcddev.colstart = 0;
				st7789_lcddev.rowstart = 35;
			}
			else
			{
				st7789_lcddev.colstart = 0;
				st7789_lcddev.rowstart = 0;
			}
            write_cmd(0x36);
            write_8bit((0<<7)|(1<<6)|(1<<5)|(0<<4)|(ST7789_RGB_ORDER<<3));
			break;
		case 2:
			st7789_lcddev.width = width;
			st7789_lcddev.height = height;
			if (width == 135)
			{
				st7789_lcddev.colstart = 53;
				st7789_lcddev.rowstart = 40;
			}
			else if(height == 280)
			{
				st7789_lcddev.colstart = 0;
				st7789_lcddev.rowstart = 20;
			}
			else if(width == 172)
			{
				st7789_lcddev.colstart = 34;
				st7789_lcddev.rowstart = 0;
			}
			else if(width == 170)
			{
				st7789_lcddev.colstart = 35;
				st7789_lcddev.rowstart = 0;
			}
			else
			{
				st7789_lcddev.colstart = 0;
				st7789_lcddev.rowstart = 80;
			}
			write_cmd(0x36);
			write_8bit((1<<7)|(1<<6)|(0<<5)|(0<<4)|(ST7789_RGB_ORDER<<3));
			break;
		case 3:
			st7789_lcddev.width = height;
			st7789_lcddev.height = width;
			if (width == 135)
			{
				st7789_lcddev.colstart = 40;
				st7789_lcddev.rowstart = 52;
			}
			else if(height == 280)
			{
				st7789_lcddev.colstart = 20;
				st7789_lcddev.rowstart = 0;
			}
			else if(width == 172)
			{
				st7789_lcddev.colstart = 0;
				st7789_lcddev.rowstart = 34;
			}
			else if(width == 170)
			{
				st7789_lcddev.colstart = 0;
				st7789_lcddev.rowstart = 35;
			}
			else
			{
				st7789_lcddev.colstart = 80;
				st7789_lcddev.rowstart = 0;
			}
			write_cmd(0x36);
			write_8bit((1<<7)|(0<<6)|(1<<5)|(0<<4)|(ST7789_RGB_ORDER<<3));
			break;
		default:
			break;
	}	
}
/**
 * @brief 屏幕方向旋转
 * @param  direction        设置屏幕显示方向（0-3）
 * @param  OFFSET_1         偏移量
 * @param  OFFSET_2         偏移量
 * 
 */
void ST7789::set_direction(uint8_t direction,uint16_t OFFSET_1,uint16_t OFFSET_2)
{
	st7789_lcddev.dir = direction;
	uint16_t width = 0;
	uint16_t height = 0;
	width = st7789_lcddev.width;
	height = st7789_lcddev.height;
	
	switch (st7789_lcddev.dir) {
		case 0:
			st7789_lcddev.width = width;
			st7789_lcddev.height = height;

			st7789_lcddev.colstart = OFFSET_1;
			st7789_lcddev.rowstart = OFFSET_2;
			
            write_cmd(0x36);
            /* BIT7:MY; BIT6:MX; BIT5:MV(行列交换); BIT4:ML; BIT3:0,RGB,1,BGR; BIT2:MH */
            write_8bit((0<<7)|(0<<6)|(0<<5)|(0<<4)|(ST7789_RGB_ORDER<<3));
			break;
		case 1:
			st7789_lcddev.width = height;
			st7789_lcddev.height = width;

			st7789_lcddev.colstart = OFFSET_1;
			st7789_lcddev.rowstart = OFFSET_2;

            write_cmd(0x36);
            write_8bit((0<<7)|(1<<6)|(1<<5)|(0<<4)|(ST7789_RGB_ORDER<<3));
			break;
		case 2:
			st7789_lcddev.width = width;
			st7789_lcddev.height = height;
			st7789_lcddev.colstart = OFFSET_1;
			st7789_lcddev.rowstart = OFFSET_2;
			write_cmd(0x36);
			write_8bit((1<<7)|(1<<6)|(0<<5)|(0<<4)|(ST7789_RGB_ORDER<<3));
			break;
		case 3:
			st7789_lcddev.width = height;
			st7789_lcddev.height = width;
			st7789_lcddev.colstart = OFFSET_1;
			st7789_lcddev.rowstart = OFFSET_2;
			write_cmd(0x36);
			write_8bit((1<<7)|(0<<6)|(1<<5)|(0<<4)|(ST7789_RGB_ORDER<<3));
			break;
		default:
			break;
	}	
}
/**
 * @brief 在（x,y）坐标画一个color颜色的点
 * @param  x                X坐标
 * @param  y                Y坐标
 * @param  color            RGB值
 */
void ST7789::draw_point(int16_t x, int16_t y, uint16_t color)
{
	set_windows(x, y, x, y);//设置光标位置 
	write_16bit(color);
}     
/**
 * @brief 填充矩形
 * @param  x0               起始X坐标
 * @param  y0               起始Y坐标
 * @param  x1               结束X坐标
 * @param  y1               结束Y坐标
 * @param  color            RGB值
 */
void ST7789::fill_rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color)
{
	uint16_t i, j;
	uint16_t width = x1 - x0 + 1; 		//得到填充的宽度
	uint16_t height = y1 - y0 + 1;		//高度
	set_windows(x0, y0, x1, y1);//设置显示窗口
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
			write_16bit(color);
	}
	set_windows(0, 0, st7789_lcddev.width - 1, st7789_lcddev.height - 1);//恢复窗口设置为全屏
}
void ST7789::show_picture(int16_t x,int16_t y,int16_t length,int16_t width,const int8_t pic[])
{
	int16_t i,j;
	int32_t k=0;
	set_windows(x,y,x+length-1,y+width-1);
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			write_16bit(pic[k*2+1]);
			k++;
		}
	}	
}
#endif  //tftlcd.h





                 