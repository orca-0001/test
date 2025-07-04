#ifndef _HT1621_H
#define _HT1621_H

#include "TWEN32F0xx.h"

#define HT1621_BIAS   0x52     //0b1000 0101 0010  1/2duty 4com 

#define HT1621_SYSDIS 0X00	    //0b1000 0000 0000   关系统振荡器和LCD偏压发生器 
#define HT1621_SYSEN  0X02     //0b1000 0000 0010  打开系统振荡器 

#define HT1621_LCDOFF 0X04     //0b1000 0000 0100  关LCD偏压 
#define HT1621_LCDON  0X06     //0b1000 0000 0110  打开LCD偏压 

#define HT1621_XTAL   0x28     //0b1000 0010 1000  外部接时钟 
#define HT1621_RC256  0X30     //0b1000 0011 0000  内部时钟 

#define HT1621_TONEON  0X12    //0b1000 0001 0010  打开声音输出 
#define HT1621_TONEOFF 0X10    //0b1000 0001 0000  关闭声音输出 

#define HT1621_WDTDIS  0X0A    //0b1000 0000 1010  禁止看门狗 
#define HT1621_TIMEREN 0X0C    //0b1000 0000 1100  允许时基输出 
#define HT1621_TEST    0XC0    //0b1000 0000 1100  允许时基输出

#define HT1621_SER_COUNT 32

class HT1621{
public:
    HT1621(uint8_t cspin, int8_t rdpin, uint8_t wrpin, uint8_t datapin);
    void begin();
    // void clear();
    void write_data(uint8_t reg, uint8_t dat);
    uint8_t read_data(uint8_t reg);
    void write_all_data(uint8_t reg, uint8_t *dat, uint8_t cnt);
    void read_all_data(uint8_t reg, uint8_t *dat, uint8_t cnt);
    void set_buzzer_frequency(uint8_t fre);
    void buzzer_control(uint8_t runflag);
private:
    uint8_t _cspin;
    uint8_t _wrpin;
    uint8_t _datapin;
    uint8_t _rdpin;
    void write_byte(uint8_t dat, uint8_t cnt);
    void write_cmd(uint8_t cmd);
    void write_cmd2(uint8_t cmd);
};

/**
 * @brief 引脚分配 
 */
HT1621::HT1621(uint8_t cspin, int8_t rdpin, uint8_t wrpin, uint8_t datapin)
{
    _cspin = cspin;
    _wrpin = wrpin;
    _datapin = datapin;
    _rdpin = rdpin;
}

/**
 * @brief 从高位开始写入数据.
 * @param  dat  数据.
 * @param  cnt  传送数据位数（1-8位数据）.
 */
void HT1621::write_byte(uint8_t dat, uint8_t cnt)
{
    uint8_t i;
    for(i=0;i<cnt;i++)
    {
        if(dat&0x80){
            digitalWrite((PIN_Name)_datapin, 1);
        }else{
            digitalWrite((PIN_Name)_datapin, 0);
        }
        digitalWrite((PIN_Name)_wrpin, 0);
        delayMicroseconds(10);
        digitalWrite((PIN_Name)_wrpin, 1);
        dat <<= 1;
    }
}

/**
 * @brief 写入命令.
 * @param  cmd  命令
 */
void HT1621::write_cmd(uint8_t cmd)
{
    digitalWrite((PIN_Name)_cspin, 0);
    delayMicroseconds(5);
    write_byte(0x80,4);  //写入命令标志100
    write_byte(cmd, 8); //写入命令数据
    digitalWrite((PIN_Name)_cspin, 1);
    delayMicroseconds(5);
}

/**
 * @brief HT1621时基及测试命令.
 * @param  cmd  命令
 */
void HT1621::write_cmd2(uint8_t cmd)
{
    digitalWrite((PIN_Name)_cspin, 0);
    delayMicroseconds(5);
    write_byte(0xa0, 4);    //写入命令标志101
    write_byte(cmd, 8);  //写入命令数据
    digitalWrite((PIN_Name)_cspin, 1);
    delayMicroseconds(5);
}

/**
 * @brief HT1621写模式（命令代码101）.
 * @param  reg  写入地址.
 * @param  dat  写入数据(低四位有效).
 */
void HT1621::write_data(uint8_t reg, uint8_t dat)
{
    digitalWrite((PIN_Name)_cspin, 0);
    write_byte(0xa0,3); //写入数据标志101
    write_byte(reg<<2,6); //写入地址
    write_byte((dat<<4),4); //写入数据
    digitalWrite((PIN_Name)_cspin, 1);
}

/**
 * @brief  HT1621读模式（命令代码110）.
 * @param  reg  读地址.
 */
uint8_t HT1621::read_data(uint8_t reg)
{
    uint8_t i,dat=0;
    digitalWrite((PIN_Name)_cspin, 0);
    write_byte(0xc0,3); //写入数据标志110
    write_byte(reg<<2,6); //写入读地址
    delayMicroseconds(5);
    digitalWrite((PIN_Name)_rdpin, 1);
    pinMode((PIN_Name)_datapin, GPIO_Mode_IN_FLOATING);
    for(i=0;i<4;i++)
    {
        digitalWrite((PIN_Name)_rdpin, 0);
        delayMicroseconds(5);
        digitalWrite((PIN_Name)_rdpin, 1);
        if(digitalRead((PIN_Name)_datapin))
        {
            dat |= 0x08;
        }
        dat >>= 1;
    }
    pinMode((PIN_Name)_datapin, GPIO_Mode_Out_PP);
    digitalWrite((PIN_Name)_datapin, 1);
    digitalWrite((PIN_Name)_cspin, 1);
    return dat;
}

/**
 * @brief  HT1621连续写入函数.
 * @param  reg  写地址.
 * @param  dat  数据指针.
 * @param  cnt  需要写入的字节数.
 */
void HT1621::write_all_data(uint8_t reg, uint8_t *dat, uint8_t cnt)
{
    uint8_t i;
    digitalWrite((PIN_Name)_cspin, 0);
    write_byte(0xa0,3); //写入数据标志101
    write_byte(reg<<2,6); //写入地址
    for(i=0;i<cnt;i++){
       write_byte((*dat<<4),4); //写入数据
       dat++; 
    }
    digitalWrite((PIN_Name)_cspin, 1);
}

/**
 * @brief  HT1621连续读函数.
 * @param  reg  写入读初始地址，.
 * @param  dat  读入的数据指针，cnt.
 * @param  cnt  需要读的字节数.
 */
void HT1621::read_all_data(uint8_t reg, uint8_t *dat, uint8_t cnt)
{
    uint8_t i,j;
    digitalWrite((PIN_Name)_cspin, 0);
    write_byte(0xc0,3); //写入数据标志110
    write_byte(reg<<2,6); //写入读地址
    pinMode((PIN_Name)_datapin, GPIO_Mode_IN_FLOATING);
    for(j=0;j<cnt;j++){
        for(i=0;i<4;i++)
        {
            digitalWrite((PIN_Name)_rdpin, 0);
            delayMicroseconds(10);
            digitalWrite((PIN_Name)_rdpin, 1);
            if(digitalRead((PIN_Name)_datapin))
            {
                *dat |= 0x08;
            }
            *dat >>= 1;
        }
        dat++;
    }
    pinMode((PIN_Name)_datapin, GPIO_Mode_Out_PP);
    digitalWrite((PIN_Name)_datapin, 1);
    digitalWrite((PIN_Name)_cspin, 1);
}

/**
 * @brief  HT1621清屏.
 */
// void HT1621::clear()
// {
//     uint8_t i;
//     digitalWrite((PIN_Name)_cspin, 0);
//     write_byte(0xa0, 3);    //写入数据标志101 
//     write_byte(0,6);//写入地址数据 
//     for (i = 0; i < HT1621_SER_COUNT; i++) 
//     { 
//         write_byte(0xf0,4);//写入数据
//     } 
//     digitalWrite((PIN_Name)_cspin, 1);
// }

/**
 * @brief  HT1621控制蜂鸣器输出频率.
 * @param  fre  频率
 *              0,4KHz,1,2KHz.
 */
void HT1621::set_buzzer_frequency(uint8_t fre)
{
    if(fre == 0){
        write_cmd(0x80);
    }else{
        write_cmd(0xC0);
    }
}

/**
 * @brief  HT1621控制蜂鸣器输出.
 * @param  runflag  0,关闭蜂鸣器;其他值，开启蜂鸣器.
 */
void HT1621::buzzer_control(uint8_t runflag)
{
   (runflag == 0)?write_cmd(HT1621_TONEOFF):write_cmd(HT1621_TONEON);
}

/**
 * @brief  HT1621初始化.
 */
void HT1621::begin()
{
    pinMode((PIN_Name)_cspin, GPIO_Mode_Out_PP);
    pinMode((PIN_Name)_datapin, GPIO_Mode_Out_PP);
    pinMode((PIN_Name)_wrpin, GPIO_Mode_Out_PP);
    digitalWrite((PIN_Name)_cspin, 1);
    digitalWrite((PIN_Name)_datapin, 1);
    digitalWrite((PIN_Name)_wrpin, 1);
    
    if(_rdpin >= 0){
        pinMode((PIN_Name)_rdpin, GPIO_Mode_Out_PP);
        digitalWrite((PIN_Name)_rdpin, 1);
    }

    delay(200);
    write_cmd(HT1621_BIAS); 
    write_cmd(HT1621_RC256); //使用内部振荡器 
    //write_cmd(HT1621_SYSDIS); 
    write_cmd(HT1621_WDTDIS); 
    write_cmd(HT1621_SYSEN); 
    write_cmd(HT1621_LCDON); 
    write_cmd(HT1621_TIMEREN); 
    write_cmd2(HT1621_TEST);
}

#endif 




