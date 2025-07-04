#ifndef _DS18B20_H_
#define _DS18B20_H_

#include <TWEN32F0xx.h>

class DS18B20
{
public:
    DS18B20(uint8_t  pin);
	float read(); 

private:
    uint8_t _pin;
    uint8_t reset();
    uint8_t read_one_char();
	void write_one_char(uint8_t dat); 
};

/**
  * @brief  引脚分配.
  * @param  pin 引脚.
  */
DS18B20::DS18B20(uint8_t  pin)
{
    _pin = pin;
}

/**
  * @brief   复位模块.
  */
uint8_t DS18B20::reset(void)  
{ 
    uint8_t r;
    uint8_t retries = 125;
	
    // wait until the wire is high... just in case
    pinMode((PIN_Name)_pin,GPIO_Mode_Out_PP);
	digitalWrite((PIN_Name)_pin,1);
	delayMicroseconds(8);
    do {
        if ( retries-- == 0) return 0;
        delayMicroseconds(2);
    } while( !digitalRead((PIN_Name)_pin));
    
    digitalWrite((PIN_Name)_pin,0);   // pull low for 500uS
	delayMicroseconds(500);
	digitalWrite((PIN_Name)_pin,1); 
	delayMicroseconds(15);
    pinMode((PIN_Name)_pin, GPIO_Mode_IN_FLOATING);
    delayMicroseconds(50);
    r = !digitalRead((PIN_Name)_pin);
	delayMicroseconds(500);
    return r;
} 

/**
  * @brief   读一个字节数据.
  * @return  读到的数据    
  */
unsigned char DS18B20::read_one_char(void) 
{ 
    unsigned char i=0; 
    unsigned char dat = 0; 
    for (i=8;i>0;i--) 
    { 
        dat>>=1;
        pinMode((PIN_Name)_pin,GPIO_Mode_Out_PP); //保证两次读操作时间间隔1us以上
        digitalWrite((PIN_Name)_pin,0); // 给脉冲信号 
        digitalWrite((PIN_Name)_pin,0); //保证主机拉低总线1μs以上 
        digitalWrite((PIN_Name)_pin,1); 
        pinMode((PIN_Name)_pin, GPIO_Mode_IN_FLOATING);
        delayMicroseconds(5);
        if(digitalRead((PIN_Name)_pin))     //主机对总线采样的数 判断 
        {
            dat|=0x80; 
        }
        delayMicroseconds(50);//等待读操作结束 
    } 
    return(dat); 
} 

/**
  * @brief   写一个字节数据.
  * @param   dat  数据    
  */
void DS18B20::write_one_char(unsigned char dat) 
{ 
    unsigned char i=0; 
    pinMode((PIN_Name)_pin,GPIO_Mode_Out_PP);
    for (i=8; i>0; i--) 
    { 
        delayMicroseconds(1);//保证两次写操作时间间隔1us以上 
        digitalWrite((PIN_Name)_pin,0); 
        digitalWrite((PIN_Name)_pin,0); //保证主机拉低总线1μs以上 

        digitalWrite((PIN_Name)_pin,(dat&0x01));  //向总线写入数据 
        delayMicroseconds(60);//写时间间隙必须最少持续60us 
        digitalWrite((PIN_Name)_pin,1); 
        dat>>=1;   
    } 
} 

/**
  * @brief   读取ds18b20当前温度.
  * @return  温度数据    
  */
float DS18B20::read() 
{ 
    uint8_t  low_byte,high_byte;
    int16_t  TReading, SignBit,Tc_100;

    //中断可能会打断读操作，根据需要，这里可以添加关闭中断函数

    reset();
    write_one_char(0xCC);     // 跳过读序号列号的操作 
    write_one_char(0x44);  // 启动温度转换 
    delayMicroseconds(600);
    reset(); 
    write_one_char(0xCC);  //跳过读序号列号的操作 
    write_one_char(0xBE);  //读取温度寄存器等（共可读9个寄存器） 前两个就是温度 
    delayMicroseconds(600); 
    low_byte = read_one_char();     //读取温度值低位 
    high_byte = read_one_char();    //读取温度值高位 

    TReading = (high_byte << 8) + low_byte;
	SignBit = TReading & 0x8000;	// test most sig bit

	if (SignBit)			// negative
	{
		TReading = (TReading ^ 0xffff) + 1;	// 2's comp
		TReading=0-TReading;

	}
	Tc_100 = (6 * TReading) + TReading / 4;
		
	//中断可能会打断读操作，根据需要，这里可以添加恢复中断函数
		
    return ((float)Tc_100/100.0);	
} 

#endif