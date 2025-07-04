/**
 * @file    hardiic.h
 * @brief   PA2(SDA), PA3(SCL).
 * @version 0.1
 * @date    2023-01-12
 * @author  hhdd
 * @copyright Copyright (c) 2021 TWen51 Technology Co., Ltd.
 */
#ifndef  _HARDIIC_H_
#define  _HARDIIC_H_

#include "TW32f0xx.h"
#include "TW32f0xx_hal_i2c.h"

class KHardIIC{
public:
    KHardIIC();
    void begin(uint32_t bound); //初始化
    uint8_t restart(uint8_t addr); //重复起始信号
    uint8_t start(uint8_t addr); //起始信号
    uint8_t write(uint8_t b);  //写函数
    uint8_t read(uint8_t last=0); //读函数
    void stop(void);  //停止信号
private:
    uint8_t _sdapin;
    uint8_t _sclpin;
};

/**
 * @brief 构造函数
 */
KHardIIC::KHardIIC()
{

}

/**
 * @brief 初始化函数
 * @param bound  硬件iic速率
 */
void KHardIIC::begin(uint32_t bound)
{
    uint32_t freqrange;
    uint32_t pclk1;

    __HAL_RCC_I2C_CLK_ENABLE();   //I2C时钟使能
    GPIOA->MODER &= ~0xF0U;
    GPIOA->MODER |= 0xA0U;      //复用
    GPIOA->OTYPER |= 0x0CU;     //开漏输出
    GPIOA->OSPEEDR |= 0xF0U;    //引脚速度设为非常快
    GPIOA->AFR[0] &= ~0xFF00U;
    GPIOA->AFR[0] |=  0xCC00U;  //设为AF12
    GPIOA->PUPDR &= ~0xF0U;  
    GPIOA->PUPDR &= ~0x50U;  //上拉

    __HAL_RCC_I2C_FORCE_RESET();
    __HAL_RCC_I2C_RELEASE_RESET();

    I2C->CR1 &= ~I2C_CR1_PE;
    pclk1 = HSI_VALUE;
    freqrange = (pclk1/1000000U);
    //配置I2C模块时钟频率
    MODIFY_REG(I2C->CR2, I2C_CR2_FREQ, freqrange);
    //配置I2C的最大上升时间
    MODIFY_REG(I2C->TRISE, I2C_TRISE_TRISE, I2C_RISE_TIME(freqrange, bound));
    //配置I2C速度
    MODIFY_REG(I2C->CCR, (I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR), I2C_SPEED(pclk1, bound, I2C_CCR_DUTY));
    //禁止广播呼叫,禁止时钟延长
    I2C->CR1= (I2C->CR1&(~(0x1U<<6))) | (0x1U<<7);
    //设置本机地址
    I2C->OAR1 = 0;
    I2C->CR1 |= I2C_CR1_PE;
}

/**
 * @brief 发送起始信号以及设备地址.
 * @param addr  地址(读地址或写地址).
 * @return 0,没有接收到应答; 1,接收到设备应答.
 */
uint8_t KHardIIC::start(uint8_t addr)
{
    int count = 0;
    while((I2C->SR2&0x2U) != 0x0U);       //等待总线不忙
    if ((I2C->CR1 & I2C_CR1_PE) != I2C_CR1_PE)
    {
        I2C->CR1 |= I2C_CR1_PE;
    }
    I2C->CR1 &= ~I2C_CR1_POS;
    I2C->CR1 |= I2C_CR1_ACK;    //使能Acknowledge
    I2C->CR1 |= I2C_CR1_START;  //生成一个起始信号
    while((I2C->SR1&I2C_SR1_SB) == 0);   //等待起始条件发送完成
    I2C->DR = (uint8_t)(addr & 0xFE);   //发送地址
    while((I2C->SR1&I2C_SR1_ADDR) == 0){   //等待地址发送完成
        count++;
        if(count >= 1000){
            I2C->CR1 |= I2C_CR1_STOP;
            return 0;
        }
    }
    //Clear ADDR flag
    __IO uint32_t tmpreg = 0x00U; 
    tmpreg = I2C->SR1; 
    tmpreg = I2C->SR2; 
    UNUSED(tmpreg);
    return 1;
}

/**
 * @brief 发送重复起始信号以及设备地址.
 * @param addr  设备地址.
 * @return 0,没有接收到应答; 1,接收到设备应答.
 */
uint8_t KHardIIC::restart(uint8_t addr)
{
    int count = 0;
    I2C->CR1 |= I2C_CR1_START;  //生成一个起始信号
    while((I2C->SR1&I2C_SR1_SB) == 0);
    I2C->DR = (uint8_t)(addr | 0x01);
    while((I2C->SR1&I2C_SR1_ADDR) == 0){
        count++;
        if((I2C->SR1&I2C_SR1_AF) == 1){
            I2C->CR1 |= I2C_CR1_STOP;
            I2C->SR1 = ~I2C_FLAG_AF;
        }
        if(count >= 1000){
            I2C->CR1 |= I2C_CR1_STOP;
            return 0;
        }
    }
    return 1;
}

/**
 * @brief 硬件IIC读函数.
 * @param last  是否应答
 *              0,应答; 1,非应答.
 * @return 读取到的数据.
 */
uint8_t KHardIIC::read(uint8_t last) 
{
    uint8_t dat = 0;
    (last==1)?(I2C->CR1 &= ~I2C_CR1_ACK):(I2C->CR1 |= I2C_CR1_ACK);
    __disable_irq();
    volatile uint32_t _tmpreg = 0x00U; 
    _tmpreg = I2C->SR1; 
    _tmpreg = I2C->SR2; 
    (void)_tmpreg; 
    __enable_irq();
    while( (I2C->SR1&I2C_SR1_RXNE) == 0){
        if(I2C->SR1&I2C_SR1_STOPF){
            I2C->SR1 &= ~I2C_SR1_STOPF;
        }
    }
    dat = (uint8_t)I2C->DR;
    return dat;
}

/**
 * @brief 硬件IIC写函数.
 * @param data  数据
 * @return 0,失败; 1,成功.
 */
uint8_t KHardIIC::write(uint8_t data) 
{
    while((I2C->SR1&I2C_SR1_TXE) == 0)
    {
        if((I2C->SR1&I2C_SR1_AF) == SET)
        {
            I2C->SR1 = ~(I2C_FLAG_AF& 0x0000FFFFU);    /* Clear NACKF Flag */
        }
    }
    I2C->DR = (uint8_t)data;
    while((I2C->SR1&I2C_SR1_TXE) == 0)
    {
        if((I2C->SR1&I2C_SR1_AF) == SET)
        {
            I2C->SR1 = ~(I2C_FLAG_AF& 0x0000FFFFU);    /* Clear NACKF Flag */
        }
    }
    return 1;
}

/**
 * @brief 产生停止信号.
 */
void KHardIIC::stop(void)
{
    I2C->CR1 |= I2C_CR1_STOP;
}

KHardIIC hardiic;

#endif




