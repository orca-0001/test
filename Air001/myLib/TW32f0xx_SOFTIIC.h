#ifndef _CH32V_SOFTIIC_H_
#define _CH32V_SOFTIIC_H_

#include <TW32f0xx.h>
#include "wiring.h"

#define KI2C_DELAY_USEC 1

const uint8_t  KI2C_READ = 1;        //读位
const uint8_t  KI2C_WRITE = 0;       //写位

//------------------------------------------------------------------------------
class KIICBase {
public:
    //读函数
    //last:应答控制位
    virtual uint8_t read(uint8_t last) = 0;
    //重新开始信号
    //addr:设备地址
    virtual bool restart(uint8_t addr) = 0;
    //起始信号
    //addr:设备地址
    virtual bool start(uint8_t addr) = 0;
    //停止信号
    virtual void stop(void) = 0;
    //写函数
    //last:要写的数据
    virtual bool write(uint8_t data) = 0;
};
//---------------------------------------------------------------------

class KSoftIIC : public KIICBase {
public:
    KSoftIIC();
    void begin(uint8_t sdapin, uint8_t sclpin); //SOFTIIC初始化
    uint8_t read(uint8_t last); //读函数
    bool restart(uint8_t addr); //重复起始信号
    bool start(uint8_t addr); //起始信号
    void stop(void);  //停止信号
    bool write(uint8_t b);  //写函数
private:
    //SoftI2cMaster() {}
    GPIO_TypeDef *SDA_GPIOx=NULL;
    GPIO_TypeDef *SCL_GPIOx=NULL;
    uint8_t sda_position;
    uint8_t scl_position;

    // uint8_t _sdapin;
    // uint8_t _sclpin;
};

//========================================================================
// 描述: 构造函数.
// 参数: none
// 返回: none.
//========================================================================
KSoftIIC::KSoftIIC()
{
}

//========================================================================
// 描述: 初始化函数.
// 参数: sdapin：SDA引脚; sclpin: scl引脚.
// 返回: none.
//========================================================================
void KSoftIIC::begin(uint8_t sdapin,uint8_t sclpin)
{
    SDA_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[sdapin];
    SCL_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[sclpin];
    sda_position = DIGITAL_PIN_NUM[sdapin];
    scl_position = DIGITAL_PIN_NUM[sclpin];

    //sda引脚配置
    SDA_GPIOx->MODER &= ~(0x3UL << (sda_position * 2u));
    SDA_GPIOx->MODER |= (0x01U << (sda_position * 2u));   //设置为输出模式
    // SDA_GPIOx->OSPEEDR &= ~(0x3UL << (sda_position * 2u));
    SDA_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (sda_position * 2u));   //设置引脚速度非常高
    SDA_GPIOx->OTYPER  &= ~(0x1UL << sda_position) ;    //复位为推挽输出
    // SDA_GPIOx->OTYPER  |= (((GPIO_MODE_OUTPUT_OD & 0x00000010u) >> 4u) << sda_position);   //设置引脚为开漏
    SDA_GPIOx->BSRR |= (0x1U << sda_position);  //sda拉高

    //scl引脚配置
    SCL_GPIOx->MODER &= ~(0x3UL << (scl_position * 2u));
    SCL_GPIOx->MODER |= ((GPIO_MODE_OUTPUT_OD & 0x00000003u) << (scl_position * 2u));   //设置为输出模式
    // SCL_GPIOx->OSPEEDR &= ~(0x3UL << (scl_position * 2u));
    SCL_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (scl_position * 2u));   //设置引脚非常高
    SCL_GPIOx->OTYPER  &= ~(0x1UL << scl_position) ;    //复位为推挽输出
    // SCL_GPIOx->OTYPER  |= (((GPIO_MODE_OUTPUT_OD & 0x00000010u) >> 4u) << scl_position);   //设置引脚为开漏
    SCL_GPIOx->BSRR |= (0x1U << scl_position);  //scl拉高

}

//========================================================================
// 描述: SOFTIIC发送起始信号.
// 参数: addr：设备地址.
// 返回: none.
//========================================================================
bool KSoftIIC::start(uint8_t addr)
{
    SDA_GPIOx->BRR |= (0x1U << sda_position);  //sda拉低
	delayMicroseconds(KI2C_DELAY_USEC);
    SCL_GPIOx->BRR |= (0x1U << scl_position);  //scl拉低
	delayMicroseconds(KI2C_DELAY_USEC);
	return write(addr);
}

//========================================================================
// 描述: SOFTIIC发送重复起始信号.
// 参数: none.
// 返回: none.
//========================================================================
bool KSoftIIC::restart(uint8_t addr)
{
    SDA_GPIOx->BSRR |= (0x1U << sda_position);  //sda拉高
    delayMicroseconds(KI2C_DELAY_USEC);
    SCL_GPIOx->BSRR |= (0x1U << scl_position);  //sda拉高
    delayMicroseconds(KI2C_DELAY_USEC);
	return start(addr);
}

//========================================================================
// 描述: SOFTIIC发送停止信号.
// 参数: none.
// 返回: none.
//========================================================================
void KSoftIIC::stop()
{
    SDA_GPIOx->BRR |= (0x1U << sda_position);  //sda拉低
    delayMicroseconds(KI2C_DELAY_USEC);
    SCL_GPIOx->BSRR |= (0x1U << scl_position);  //scl拉高
    delayMicroseconds(KI2C_DELAY_USEC);
    SDA_GPIOx->BSRR |= (0x1U << sda_position);  //sda拉高
    delayMicroseconds(KI2C_DELAY_USEC);
}

//========================================================================
// 描述: SOFTIIC读函数.
// 参数: last:0表示应答,1表示非应答.
// 返回: 读取到的数据.
//========================================================================
uint8_t KSoftIIC::read(uint8_t last) {
    uint8_t b = 0;
    SDA_GPIOx->BSRR |= (0x1U << sda_position);  //sda拉高
    SDA_GPIOx->MODER &= ~(0x3UL << (sda_position * 2u));    //SDA设置为输入模式
    SDA_GPIOx->PUPDR &= ~(0x3UL << (sda_position * 2u));
    SDA_GPIOx->PUPDR |= (GPIO_PULLUP << (sda_position * 2u));    //SDA设置为上拉
    for (uint8_t i = 0; i < 8; i++) {
        b <<= 1;
        delayMicroseconds(KI2C_DELAY_USEC);
        SCL_GPIOx->BSRR |= (0x1U << scl_position);  //scl拉高
        delayMicroseconds(KI2C_DELAY_USEC);
        if((SDA_GPIOx->IDR&(0x01<<sda_position)) != 0x0U){
            b |= 1;
        }
        SCL_GPIOx->BRR |= (0x1U << scl_position);  //scl拉低
        delayMicroseconds(KI2C_DELAY_USEC);
    }
    SDA_GPIOx->MODER &= ~(0x3UL << (sda_position * 2u));
    SDA_GPIOx->MODER |= (0x01U << (sda_position * 2u));   //设置为输出模式
    SDA_GPIOx->OTYPER  &= ~(0x1UL << sda_position) ;    //设置为推挽输出
    // SDA_GPIOx->OTYPER  |= (((GPIO_MODE_OUTPUT_OD & 0x00000010u) >> 4u) << sda_position);   //设置引脚为开漏
    last?(SDA_GPIOx->BSRR |= (0x1U << sda_position)):(SDA_GPIOx->BRR |= (0x1U << sda_position));
    SCL_GPIOx->BSRR |= (0x1U << scl_position);  //scl拉高
    delayMicroseconds(KI2C_DELAY_USEC);
    SCL_GPIOx->BRR |= (0x1U << scl_position);  //scl拉低
    delayMicroseconds(KI2C_DELAY_USEC);
    SDA_GPIOx->BRR |= (0x1U << sda_position);   //sda拉低
    return b;
}

//========================================================================
// 描述: 写函数.
// 参数: data：数据.
// 返回: 0,没有接收到应答;1,接收到应答.
//========================================================================
bool KSoftIIC::write(uint8_t data) 
{
    uint8_t rtn = 0;
    for (uint8_t m = 0X80; m != 0; m >>= 1) {
        if(m&data){
            SDA_GPIOx->BSRR |= (0x1U << sda_position);   //sda拉高
        }else{
            SDA_GPIOx->BRR |= (0x1U << sda_position);   //sda拉低
        }
        SCL_GPIOx->BSRR |= (0x1U << scl_position);  //scl拉高
        //delayMicroseconds(KI2C_DELAY_USEC);
        SCL_GPIOx->BRR |= (0x1U << scl_position);  //scl拉低
        //delayMicroseconds(KI2C_DELAY_USEC);
    }
    // get Ack or Nak
    SDA_GPIOx->MODER &= ~(0x3UL << (sda_position * 2u));    //SDA设置为输入模式
    SDA_GPIOx->PUPDR &= ~(0x3UL << (sda_position * 2u));
    SDA_GPIOx->PUPDR |= (GPIO_PULLUP << (sda_position * 2u));    //SDA设置为上拉
    delayMicroseconds(KI2C_DELAY_USEC);
    SCL_GPIOx->BSRR |= (0x1U << scl_position);  //scl拉高
    delayMicroseconds(KI2C_DELAY_USEC);

    if((SDA_GPIOx->IDR& (0x01UL<<sda_position)) != 0x00u)
    {
        rtn = 1;
    }else{
        rtn = 0;
    }
    SCL_GPIOx->BRR |= (0x1U << scl_position);  //scl拉低
    delayMicroseconds(KI2C_DELAY_USEC);
    SDA_GPIOx->MODER &= ~(0x3UL << (sda_position * 2u));
    SDA_GPIOx->MODER |= (0x01U << (sda_position * 2u));   //设置为输出模式
    SDA_GPIOx->OTYPER  &= ~(0x1UL << sda_position) ;    //设置为推挽输出
    // SDA_GPIOx->OTYPER  |= (((GPIO_MODE_OUTPUT_OD & 0x00000010u) >> 4u) << sda_position);   //设置引脚为开漏
    SDA_GPIOx->BRR |= (0x1U << sda_position);  //sda拉低
    return rtn == 0;
}

KSoftIIC softiic;

#endif  // softiic.h
