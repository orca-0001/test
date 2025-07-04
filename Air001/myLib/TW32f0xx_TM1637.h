/**
 * @file TM1637.h
 * @brief TM1637数码管驱动库。
 *        TM1637通信方式和IIC相似,不过TM1637是低位先传输(LSB),和IIC正好相反.
 * @version 0.1
 * @date 2023-01-03
 * 
 * @copyright Copyright (c) 2022 杭州天问五幺科技有限公司
 * 
 */

#ifndef __TM1637_H_
#define __TM1637_H_

#include <TWEN32F0xx.h>

/*  共阴
 *                              0,    1,   2,   3,   4,  5,   6,   7,   8,   9,   A,   B,   C,   D,   E,   F,   -,  NONE   */                             
const uint8_t nixietube_tab[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x40,0x00};
//  共阳
// const uint8_t nixietube_tab[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,0xBF,0xFF};

class TM1637{
public:
    TM1637(uint8_t sdapin, uint8_t sclpin);
    void begin(uint8_t brightness = 4);
    void clear();
    void display_num(int32_t num);
    void display_bit(uint8_t num, uint8_t digits,uint8_t point);
    void display_time(uint8_t m, uint8_t s,uint8_t point);

    void write_com(uint8_t cmd);
    void write(uint8_t reg, uint8_t value);
    void write(uint8_t reg, uint8_t* value, uint8_t len);

    uint8_t read_key();
private:
    uint8_t _sdapin, _sclpin;
    void start();
    void stop();
    uint8_t waitack();
    void sendbyte(uint8_t dat);
};

/**
 * @brief 引脚分配
 */
TM1637::TM1637(uint8_t sdapin, uint8_t sclpin)
{
    _sdapin = sdapin;
    _sclpin = sclpin;
}

/**
 * @brief  发送start信号
 */
void TM1637::start()
{
    digitalWrite((PIN_Name)_sclpin, 1);
    digitalWrite((PIN_Name)_sdapin, 1);
    delayMicroseconds(2);
    digitalWrite((PIN_Name)_sdapin, 0);
}

/**
 * @brief  发送stop信号
 */
void TM1637::stop()
{
    digitalWrite((PIN_Name)_sclpin, 0);
    delayMicroseconds(4);
    digitalWrite((PIN_Name)_sdapin, 0);
    delayMicroseconds(4);
    digitalWrite((PIN_Name)_sclpin, 1);
    delayMicroseconds(4);
    digitalWrite((PIN_Name)_sdapin, 1);
}

/**
 * @brief  等待ack信号
 * @return 0,接收到应答信号; 1,没有接收到应答信号.
 */
uint8_t TM1637::waitack()
{
    uint8_t ucErrTime=0;
    pinMode((PIN_Name)_sdapin, GPIO_Mode_IPU);
    digitalWrite((PIN_Name)_sdapin, 1);
    delayMicroseconds(1);
    digitalWrite((PIN_Name)_sclpin, 1);
    delayMicroseconds(1);
    while( digitalRead((PIN_Name)_sdapin) == 1 ){
        ucErrTime++;
        if(ucErrTime > 250)
        {
            stop();
            pinMode((PIN_Name)_sdapin, GPIO_Mode_Out_PP);
            return 1;
        }
    }
    digitalWrite((PIN_Name)_sclpin, 0);
    pinMode((PIN_Name)_sdapin, GPIO_Mode_Out_PP);
    return 0;
}

/**
 * @brief  写一个字节（LSB,TM1637先传输低位数据，和IIC协议刚好相反）
 * @param  dat  数据.
 */
void TM1637::sendbyte(uint8_t dat)
{
    uint8_t i;
    digitalWrite((PIN_Name)_sclpin, 0);
    for(i=0; i<8; i++){
        digitalWrite((PIN_Name)_sdapin, dat&0x01);
        dat>>= 1;
        delayMicroseconds(2);
        digitalWrite((PIN_Name)_sclpin, 1);
        delayMicroseconds(2);
        digitalWrite((PIN_Name)_sclpin, 0);
    }
}

/**
 * @brief  发送命令字
 * @param  cmd  命令.
 */
void TM1637::write_com(uint8_t cmd)
{
    start();
    sendbyte(cmd);
    waitack();
    stop();
}

/**
 * @brief  指定地址写入单个数据
 * @param  reg    寄存器地址.
 * @param  value  数据.
 */
void TM1637::write(uint8_t reg, uint8_t value)
{
    write_com(0x44);    //固定地址
    start();
    sendbyte(reg|0xc0);
    waitack();
    sendbyte(value);
    waitack();
    stop();    
}

/**
 * @brief  TM1637写函数
 * @param  reg    寄存器地址.
 * @param  value  数据指针.
 * @param  len    数据长度
 */
void TM1637::write(uint8_t reg, uint8_t* value, uint8_t len)
{
    write_com(0x40);    //自动地址增加
    start();
    sendbyte(reg|0xc0);
    waitack();
    for(int i=0;i<len;i++)
    {
        sendbyte(value[i]);
        waitack();
    }
    stop();
}

/**
 * @brief  TM1637读按键
 * @return 被按下的按键
 */
uint8_t TM1637::read_key()
{
    uint8_t rekey,i;
    start();
    sendbyte(0x42);
    waitack();
    digitalWrite((PIN_Name)_sdapin, 1);
    pinMode((PIN_Name)_sdapin, GPIO_Mode_IPU);
    for(i=0; i<8;i++) {
        digitalWrite((PIN_Name)_sclpin, 0);
        rekey = rekey>>1;
        delayMicroseconds(30);
        digitalWrite((PIN_Name)_sclpin, 1);
        delayMicroseconds(2);
        if(digitalRead((PIN_Name)_sdapin) == 1){
            rekey= rekey | 0x80;
        }else{
            rekey= rekey | 0x00;
        }
        delayMicroseconds(30);
    }
    waitack();
    stop();
    pinMode((PIN_Name)_sdapin, GPIO_Mode_Out_PP);
    return rekey;
}

/**
 * @brief  TM1637初始化
 * @param  brightness   亮度(0-7)
 */
void TM1637::begin(uint8_t brightness)
{
    pinMode((PIN_Name)_sdapin, GPIO_Mode_Out_PP);
    digitalWrite((PIN_Name)_sdapin, 1);
    pinMode((PIN_Name)_sclpin, GPIO_Mode_Out_PP);
    digitalWrite((PIN_Name)_sclpin, 1);
    if(brightness<8){
        write_com(brightness+0x88); 
    }else{
        write_com(0x8b);   //1000 1011b
    } 
}

/**
 * @brief  TM1637清除
 */
void TM1637::clear()
{
    uint8_t i;
	for(i=0; i<6;i++)
	{
		write(i,0);  
	}
}

/**
 * @brief  TM1637显示数字
 * @param  num  要显示的数字
 */
void TM1637::display_num(int32_t num)
{
    int32_t temp;
    uint8_t i;
    if(num>=0)
    {
        for(i=3;i>=0;i--)
        {
            temp = num%10;
            num = num/10;
            write(i,nixietube_tab[temp]);
            if(num == 0)
            {
                break;
            }
        }
    }
    else//负数
    {
        num = -num;
        for(i=3;i>=0;i--)
        {
            temp=num%10;
            num=num/10;
            write(i,nixietube_tab[temp]);
            if(num == 0)
            {
                i--;
                write(i,nixietube_tab[16]);//添加负号
                break;
            }
        }
    }
}

/**
 * @brief  TM1637在指定位显示
 * @param  num    要显示的数字
 * @param  _bit   在哪一位显示
 * @param  point  1,时钟点亮，0,时钟点灭(第三位才有时钟点，其余为point没有作用)
 */
void TM1637::display_bit(uint8_t num, uint8_t _bit,uint8_t point)
{
	uint8_t temp;
	if(num > 15)return;
	if(_bit>3)return;
	temp=nixietube_tab[num];

	if(point==0){
		temp&= ~(0X80);
		write((uint8_t)(3-_bit),temp);	
	}else{
		temp|=0x80;
		write((uint8_t)(3-_bit),temp);
	}
}

/**
 * @brief  TM1637显示时间
 * @param  m    左边数码管值
 * @param  s    右边数码管值
 * @param  point  1,时钟点亮，0,时钟点灭(第三位才有时钟点，其余为point没有作用)
 */
void TM1637::display_time(uint8_t m, uint8_t s,uint8_t point)
{
	uint8_t m_h,m_l,s_h,s_l;
	if( (m>99) || (s>99))return;
	m_h = m/10;
	m_l = m%10;
	s_h = s/10;
	s_l = s%10;
	display_bit(m_h, 3, 0);
	display_bit(m_l, 2, point);
	display_bit(s_h, 1, 0);
	display_bit(s_l, 0, 0);
}



#endif
