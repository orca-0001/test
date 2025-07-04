/**
 * @file at24cxx.h
 * @brief at24cxx驱动库
 * @version 0.1
 * @date 2023-01-03
 * 
 * @copyright Copyright (c) 2022 杭州天问五幺科技有限公司
 * 
 */
#ifndef _AT24CXX_H_
#define _AT24CXX_H_ 

// #define  AT24CXX_USE_HARDIIC      //使用硬件I2C
#if defined(AT24CXX_USE_HARDIIC)
#include "TW32f0xx_HARDIIC.h"
#define  AT24CXX_IICX       (hardiic)
#else
#include "TW32f0xx_SOFTIIC.h"
#define  AT24CXX_IICX       (softiic)
#endif

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767

class AT24CXX
{
public:
	AT24CXX(void);
#if defined(AT24CXX_USE_HARDIIC)
    void begin(uint32_t type,uint8_t slave_addr=0x50);
#else
    void begin(uint8_t sda, uint8_t scl, uint32_t type,uint8_t slave_addr=0x50);
#endif
    void write_one_byte(uint16_t addr, uint8_t dat);
    uint8_t read_one_byte(uint16_t addr);
    void write(uint16_t addr, uint8_t *pbuf, uint16_t len);
    void read(uint16_t addr, uint8_t *pbuf, uint16_t len);

private:
    uint8_t _slave_addr;
    uint32_t _type;
};

AT24CXX::AT24CXX(void)
{

}


/***********************************************************************
 * @brief  24cxx初始化.
 * @param  sda     SDA引脚.
 * @param  scl     SCL引脚.
 * @param  type    设备类型.
 * @param  slave_addr  设备地址.
 * @return none.
 **********************************************************************/
#if defined(AT24CXX_USE_HARDIIC)
void AT24CXX::begin(uint32_t type,uint8_t slave_addr)
{
    _slave_addr = slave_addr<<1;
    _type = type;
    AT24CXX_IICX.begin(80000);
}	
#else
void AT24CXX::begin(uint8_t sda, uint8_t scl,uint32_t type,uint8_t slave_addr)
{
    _slave_addr = slave_addr<<1;
    _type = type;
    AT24CXX_IICX.begin(sda,scl);
}	
#endif

/***********************************************************************
 * @brief  24cxx在指定地址写入一个数据(在写入完成后需要等待一段时间来保证写入完成).
 * @param  addr  要写入的地址.
 * @param  dat   要写入的数据.
 * @return none.
 **********************************************************************/
void AT24CXX::write_one_byte(uint16_t addr, uint8_t dat)
{
    if(_type > AT24C16)
    {
        AT24CXX_IICX.start(_slave_addr);     //开始信号
        AT24CXX_IICX.write(addr>>8);      //发送高地址
    }
    else
    {
        AT24CXX_IICX.start((uint8_t)(_slave_addr+((addr/256)<<1)));     //发送器件地址,写数据
    }
    
    AT24CXX_IICX.write((uint8_t)addr);      //发送低地址
    AT24CXX_IICX.write(dat);      //发送数据
    AT24CXX_IICX.stop();     //发送停止命令
}

/***********************************************************************
 * @brief  24cxx在指定地址读出一个数据.
 * @param  addr  要读的地址.
 * @return 读出的数据.
 **********************************************************************/
uint8_t AT24CXX::read_one_byte(uint16_t addr)
{
    uint8_t temp;
    if(_type > AT24C16)
    {
        AT24CXX_IICX.start(_slave_addr);      //发送写命令
        AT24CXX_IICX.write(addr>>8);      //发送高地址
    }
    else
    {
        AT24CXX_IICX.start((uint8_t)(_slave_addr+((addr/256)<<1)));     //发送器件地址,写数据
    }

    AT24CXX_IICX.write((uint8_t)addr);      //发送低地址
    if(AT24CXX_IICX.restart(_slave_addr|0x01))     //重复开始信号
    {
        temp = AT24CXX_IICX.read(1);
    }else{
        temp = 0;
    }
    AT24CXX_IICX.stop();     //发送停止命令
    return temp;
}

/***********************************************************************
 * @brief  24cxx在指定地址写入指定个数的数据.
 * @param  addr  要写的起始地址.
 * @param  pbuf  写数据指针.
 * @param  len   数据长度.
 * @return none.
 **********************************************************************/
void AT24CXX::write(uint16_t addr, uint8_t *pbuf, uint16_t len)
{
    uint8_t i;
    for(i=0;i<len;i++)
    {
        write_one_byte(addr, *pbuf);
        delay(5);
        addr++;
        pbuf++;
    }
}

/***********************************************************************
 * @brief  24cxx在指定地址读出指定个数的数据.
 * @param  addr  要读的起始地址.
 * @param  pbuf  读数据指针.
 * @param  len   数据长度.
 * @return none.
 **********************************************************************/
void AT24CXX::read(uint16_t addr, uint8_t *pbuf, uint16_t len)
{
    uint8_t i;
    for(i=0;i<len;i++)
    {
        *pbuf = read_one_byte(addr);
        addr++;
        pbuf++;
    }
}

#endif
