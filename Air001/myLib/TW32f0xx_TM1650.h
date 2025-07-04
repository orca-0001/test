#ifndef _TM1650_H_
#define _TM1650_H_

#include <TWEN32F0xx.h>
// #define TM1650_USE_HARDIIC      //是否使用硬件iic, 使用硬件iic时引脚配置无效
#if defined(TM1650_USE_HARDIIC)
#include "TW32f0xx_HARDIIC.h"
#define tm1650_iic    hardiic  
#else
#include "TW32f0xx_SOFTIIC.h"
#define tm1650_iic    softiic  
#endif

//  共阴
//                              0,    1,   2,   3,   4,  5,   6,   7,   8,   9,   A,   B,   C,   D,   E,   F,   -,  NONE                          
const uint8_t tm1650_tab[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x40,0x00};
//  共阳
// const uint8_t tm1650_tab[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,0xBF,0xFF};

class TM1650
{
public:
#if defined(TM1650_USE_HARDIIC)
    TM1650();
#else
    TM1650(uint8_t sda,uint8_t scl);
#endif
	void begin();
	void clear(void);
	void display_num(int16_t num);
	void display_num(int16_t num,bool dir);
	void display_num(uint8_t one, uint8_t two, uint8_t three, uint8_t four);
	void display_float(float f);
	void display_time(uint8_t hour,uint8_t min,uint8_t point);
	void displayABCDEF(const String &s, uint8_t bit);
	void display_bit(uint8_t num, uint8_t _bit,uint8_t point); //在bit位显示数num;point为1，小数点亮,否则为灭
	void clear_bit(uint8_t _bit);
private:
	uint8_t SCL_pin,SDA_pin;
	void write_data(unsigned char addr,unsigned char DATA);
};

/**
 * @brief 引脚分配.
 * @param  sda    SDA引脚(如果使用硬件iic则引脚固定,具体引脚见"HARDIIC.h"文件).
 * @param  scl    SCL引脚(如果使用硬件iic则引脚固定,具体引脚见"HARDIIC.h"文件).
 * @return: none.
 */
#if defined(TM1650_USE_HARDIIC)
TM1650::TM1650()
{
}
#else
TM1650::TM1650(uint8_t sda,uint8_t scl)
{
    SDA_pin = sda;
	SCL_pin = scl;
}
#endif

/**
 * @brief TM1650初始化.
 */
void TM1650::begin()
{
#if defined(TM1650_USE_HARDIIC)
    hardiic.begin(100000);
#else
	softiic.begin(SDA_pin,SCL_pin);
#endif
    write_data(0x48,0x31);     // 开显示、8段显示方式、3级亮度
}

/**
 * @brief 显示时间.
 * @param hour  小时
 * @param min   分钟
 * @param point 时钟点。1,显示;0,不显示.
 */
void TM1650::display_time(uint8_t hour,uint8_t min,uint8_t point)
{
    hour = hour > 99 ? 99 : hour;
    min = min > 99 ? 99 : min;
	write_data(0x68, tm1650_tab[hour/10]);
    if (point == 0x01)
    {
    	write_data(0x6A, tm1650_tab[hour%10]|0x80);
    }
    else
    {
    	write_data(0x6A, tm1650_tab[hour%10]&(~0X80));
    }
    write_data(0x6C, tm1650_tab[min/10]);
	write_data(0x6E, tm1650_tab[min%10]);
}

/**
 * @brief 每个数码管单独显示数字.
 * @param one   第一个数码管显示的数字
 * @param two   第二个数码管显示的数字
 * @param three 第三个数码管显示的数字
 * @param four  第四个数码管显示的数字
 */
void TM1650::display_num(uint8_t one, uint8_t two, uint8_t three, uint8_t four)
{
	write_data(0x68+6,tm1650_tab[one]);
	write_data(0x68+4,tm1650_tab[two]);
	write_data(0x68+2,tm1650_tab[three]);
	write_data(0x68,tm1650_tab[four]);
}

/**
 * @brief 显示整数.
 * @param num   要显示的整数
 */
void TM1650::display_num(int16_t num)
{
	int16_t temp=0;
	uint8_t i=0;
	if(num >= 10000){clear();return;}
	if(num <= -1000){clear();return;}

	if(num >= 0){
		for(i=4;i>=1;i--){
			temp = num % 10;
			num = num / 10;
			display_bit(temp,4-i,0);
			if(num == 0)
			{
				break;
			}
		}
	}else{
		num = -num;
		for(i=4;i>=1;i--){
			temp = num % 10;
			num = num / 10;
			display_bit(temp,4-i,0);
			if(num == 0){
				i--;
				display_bit(16,4-i,0);	//添加负号
				break;
			}
		}
	}
}

/**
 * @brief 显示整数.
 * @param num   要显示的整数
 * @param dir   0,数据左对齐，1,数据右对齐.
 *              此处的左右会根据具体硬件连接会不同。
 */
void TM1650::display_num(int16_t num,bool dir)
{
	int16_t temp=0;
	uint8_t i=0;
	if(num >= 10000){clear();return;}
	if(num <= -1000){clear();return;}

	if(num >= 0){
		for(i=4;i>=1;i--){
			temp = num % 10;
			num = num / 10;
			if(dir){
				display_bit(temp,4-i,0);
			}else{
				display_bit(temp,i+1,0);
			}
			if(num == 0)
			{
				break;
			}
		}
	}else{
		num = -num;
		for(i=4;i>=1;i--){
			temp = num % 10;
			num = num / 10;
			if(dir){
				display_bit(temp,4-i,0);
			}else{
				display_bit(temp,i+1,0);
			}
			if(num == 0){
				i--;
				if(dir){
					display_bit(16,4-i,0);
				}else{
					display_bit(16,i+1,0);
				}
				break;
			}
		}
	}
}

/**
 * @brief 显示小数.
 * @param f  要显示的小数.
 */
void TM1650::display_float(float f)
{
	uint8_t a[4];
	uint8_t b[4];
	uint8_t int_bit=0,float_bit=0;
	uint16_t int_part=0,float_part=0;
	uint8_t i=0;

	if(f >= 10000){clear();return;}
	if(f <= -1000){clear();return;}

	clear();
	if(f == 0)
	{
		display_num(0);
	}
	else if(f > 0)
	{
		int_part = (int16_t)f/1;
		//获取整数位位数
		a[0] = int_part%10;if(a[0] != 0)int_bit = 1;//最低位
		a[1] = int_part%100/10;if(a[1] != 0)int_bit = 2;
		a[2] = int_part%1000/100;if(a[2] != 0)int_bit = 3;
		a[3] = int_part/1000;if(a[3] != 0)int_bit = 4;

		//获取小数位位数
		float_part = (uint32_t)(f*10000)%10000;
		b[0] = float_part/1000;if(b[0] != 0)float_bit = 1;
		b[1] = float_part%1000/100;if(b[1] != 0)float_bit = 2;
		b[2] = float_part%100/10;if(b[2] != 0)float_bit = 3;
		b[3] = float_part%10;if(b[3] != 0)float_bit = 4;
		
		if(int_bit == 4)	//4位全为整数位
		{
			display_num(int_part);
		}else if(int_bit == 3)	//整数位为3位
		{
			if(b[0] == 0)	//整数有3位,小数0位有效
			{
				display_num(int_part);
			}else
			{
				display_bit(a[2],3,0);
				display_bit(a[1],2,0);
				display_bit(a[0],1,1);
				display_bit(b[0],0,0);
			}
		}else if(int_bit == 2)//整数有2位
		{
			if(b[1] == 0)
			{
				if(b[0] == 0)	//整数有2位,小数为0
				{
					display_num(int_part);
				}else			//整数有2位,小数后1位有效
				{
					display_bit(a[1],2,0);
					display_bit(a[0],1,1);
					display_bit(b[0],0,0);
				}
			}else	//小数点后二位不为零
			{
				display_bit(a[1],3,0);
				display_bit(a[0],2,1);
				display_bit(b[0],1,0);
				display_bit(b[1],0,0);
			}
		}else if(int_bit == 1)//整数只有1位
		{
			if(b[2] == 0)
			{
				if(b[1] == 0)
				{
					if(b[0] == 0)	//小数部分全为0
					{
						display_num(int_part);
					}else          //小数后一位不为0
					{
						display_bit(a[0],1,0);
						display_bit(b[0],0,0);
					}
				}else
				{
					display_bit(a[0],2,1);
					display_bit(b[0],1,0);
					display_bit(b[1],0,0);
				}
			}else		//小数部分全不为0
			{
				display_bit(a[0],3,1);
				display_bit(b[0],2,0);
				display_bit(b[1],1,0);
				display_bit(b[2],0,0);
			}
		}else if(int_bit == 0)
		{
			if(float_bit > 3)float_bit=3;
			for(i=0; i<float_bit; i++){
				display_bit(b[(float_bit-1)-i],i,0);
			}
			display_bit(0,float_bit,1);
		}
	}else{
		f = -f;
		int_part = (int16_t)f/1;
		//获取整数位位数
		a[0] = int_part%10;if(a[0] != 0)int_bit = 1;//最低位
		a[1] = int_part%100/10;if(a[1] != 0)int_bit = 2;
		a[2] = int_part%1000/100;if(a[2] != 0)int_bit = 3;

		//获取小数位位数
		float_part = (uint32_t)(f*10000)%10000;
		b[0] = float_part/1000;if(b[0] != 0)float_bit = 1;
		b[1] = float_part%1000/100;if(b[1] != 0)float_bit = 2;
		b[2] = float_part%100/10;if(b[2] != 0)float_bit = 3;
		b[3] = float_part%10;if(b[3] != 0)float_bit = 4;
		
		if(int_bit == 3)	//整数位为3位
		{
			display_num(-int_part);
		}else if(int_bit == 2)	//整数位为2位
		{
			if(b[0] == 0)	//小数位为0
			{
				display_num(-int_part);
			}else{
				display_bit(16,3,0);	//显示"-"号
				display_bit(a[1],2,0);
				display_bit(a[0],1,1);
				display_bit(b[0],0,0);
			}
		}else if(int_bit == 1)  //整数有1位
		{
			if(b[1] == 0)
			{
				if(b[0] == 0)	//小数位全为0
				{
					display_num(-int_part);
				}else           //
				{
					display_bit(16,2,0);	//显示"-"号
					display_bit(a[0],1,1);
					display_bit(b[0],0,0);
				}
			}else		//小数后二位不为零
			{
				display_bit(16,3,0);	//显示"-"号
				display_bit(a[0],2,1);
				display_bit(b[0],1,0);
				display_bit(b[1],0,0);
			}
		}else if(int_bit == 0)		//整数位为0
		{
			if(float_bit > 2)float_bit=2;
			for(i=0; i<float_bit; i++){
				display_bit(b[(float_bit-1)-i],i,0);
			}
			display_bit(0,float_bit,1);
			display_bit(16,float_bit+1,0);
		}
	}
}

/**
 * @brief 单独数码管显示数.
 * @param num  要显示的数(0-9).
 * @param bit  第几位要显示.
 * @param point 1显示小数点，0不显示.
 */
void TM1650::display_bit(uint8_t num, uint8_t bit,uint8_t point)
{
	if(num > 16)return;
	if(point==0){
		write_data(0x68+6-2*(bit),tm1650_tab[num]&(~0X80));
	}else{
		write_data(0x68+6-2*(bit),tm1650_tab[num]|0x80);
	}
}

/**
 * @brief 单独数码管显示ABCDEF.
 * @param s    要显示的字符.
 * @param bit  第几位要显示.
 */
void TM1650::displayABCDEF(const String &s, uint8_t bit)
{ 
    uint8_t i = 0;
    
    if(s.compareTo("a") == 0)
    {
        i = 1;
    }
    else if(s.compareTo("b") == 0)
    {
        i = 2;
    }
    else if(s.compareTo("c") == 0)
    {
        i = 3;
    }
    else if(s.compareTo("d") == 0)
    {
        i = 4;
    }
    else if(s.compareTo("e") == 0)
    {
        i = 5;
    }
    else if(s.compareTo("f") == 0)
    {
        i = 6;
    }
	switch(i)
	{
		case 1:
			write_data(0x68+2*(bit),tm1650_tab[10]);
		break;

		case 2:
			write_data(0x68+2*(bit),tm1650_tab[11]);
		break;

		case 3:
			write_data(0x68+2*(bit),tm1650_tab[12]);
		break;

		case 4:
			write_data(0x68+2*(bit-1),tm1650_tab[13]);
		break;

		case 5:
			write_data(0x68+2*(bit-1),tm1650_tab[14]);
		break;
        
		case 6:
			write_data(0x68+2*(bit),tm1650_tab[15]);
		break;
	}
}

/**
 * @brief 数码管清屏.
 */
void TM1650::clear(void)
{
	for(int a=0; a<4;a++)
	{
		write_data(0x68+a*2,0);
	}
}

/**
 * @brief 数码管指定位清除.
 * @param bit 要清除的位
 */
void TM1650::clear_bit(uint8_t bit)
{
	if(bit < 1)return;
	write_data(0x68+6-(bit - 1)*2,0);
}

/**
 * @brief TM1650写函数.
 * @param addr 寄存器地址
 * @param DATA 数据
 */
void TM1650::write_data(unsigned char addr,unsigned char DATA)		//指定地址写入数据
{
	tm1650_iic.start(addr);	
	tm1650_iic.write(DATA);	
	tm1650_iic.stop();
}

#endif