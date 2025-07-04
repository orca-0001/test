/**
 * @file hardspi.h
 * @brief 硬件spi库
 *        PA2(SPI1_SCK), PA0(SPI1_MISO), PA1(SPI1_MOSI)
 * @version 0.1
 * @date 2023-01-03
 * 
 * @copyright Copyright (c) 2022 杭州天问五幺科技有限公司
 * 
 */
#ifndef _HARDSPI_H_
#define _HARDSPI_H_

#include "TWEN32F0xx.h"

class HARDSPI {
public:
    void begin();   //初始化
    void end();
    void set_speed(uint16_t speed);
    void set_mode(uint8_t CPOL, uint8_t CPHL);

    uint8_t wr_data(uint8_t dat);
    void write_data(uint8_t dat);
    uint8_t read_data(void);
};

/**
 * @brief SPI初始化
 * @note 
 *    默认:
 *    引脚固定为PA2(SPI1_SCK), PA0(SPI1_MISO), PA1(SPI1_MOSI).
 *    主设备, 全双工, MSB, 4分频, 
 *    空闲时SCK保持低电平
 *    数据采样从第一个时钟边沿开始
 */
void HARDSPI::begin()
{
    __HAL_RCC_SPI1_CLK_ENABLE();

    //引脚配置
    GPIOA->MODER &= ~(0x3FU);   //复用功能模式
    GPIOA->MODER |= (0x2AU);
    GPIOA->OTYPER &= ~(0x7U);  //端口输出设为复位状态
    GPIOA->PUPDR &= ~(0x3FU);
    GPIOA->PUPDR |= (0x2U<<4);   //PA2(SCK)下拉
    GPIOA->OSPEEDR |= (0x3FU);   //端口速度设为非常高

    //复用为spi功能
    GPIOA->AFR[0] &= ~(0xFFFU);
    GPIOA->AFR[0] |= (0xAAAU);  //AF10

    /*  
     *  主设备, 全双工, MSB, 4分频, 
     *  空闲时SCK保持低电平
     *  数据采样从第一个时钟边沿开始
     */
    SPI1->CR1 &= ~(0x1UL << 6U);
    SPI1->CR1 = (0x1U << 2)|(0x1U << 3);
    SPI1->CR2 = (0x1UL<<2)|(0x1UL<<12);
}

/**
 * @brief 禁止SPI
 */
void HARDSPI::end()
{
    SPI1->CR1 &= ~SPI_CR1_SPE;      //禁止SPI
}

/**
 * @brief SPI设置速率
 * @param speed 速率0-7（2，4，8，16，32，64，128，256分频）
 */
void HARDSPI::set_speed(uint16_t speed)
{
    SPI1->CR1 &= ~SPI_CR1_SPE;      //禁止SPI
    SPI1->CR1 &= ~0x38U;
    SPI1->CR1 |= ((uint32_t)speed << 3);
    SPI1->CR1 |= SPI_CR1_SPE;      //使能SPI
}

/**
 * @brief SPI设置模式
 * @param CPOL  时钟极性(0:空闲时SCK为低电平; 1:空闲时SCK为高电平);
 * @param CPHL  时钟相位(0:数据采样从第一个时钟沿开始; 1:数据采样从第二个时钟沿开始).
 */
void HARDSPI::set_mode(uint8_t CPOL, uint8_t CPHL)
{
    SPI1->CR1 &= ~SPI_CR1_SPE;      //禁止SPI
    SPI1->CR1 &= ~0x3U;
    SPI1->CR1 |= (uint32_t)((CPOL<<1) | CPHL);
    SPI1->CR1 |= SPI_CR1_SPE;      //使能SPI
}

/**
 * @brief SPI读写一个字节数据
 * @param dat  字节数据
 * @return 读取到的字节数据
 */
uint8_t HARDSPI::wr_data(uint8_t dat)  //读写一个字节数据
{
    uint8_t rec = 0;
    __IO uint8_t  tmpreg8 = 0;
    if ((SPI1->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
    {
        SPI1->CR1 |= SPI_CR1_SPE;
    }
    *((__IO uint8_t *)&SPI1->DR) = (uint8_t)dat;
    while (1)
    {
        if ((SPI1->SR&SPI_SR_RXNE)==SPI_SR_RXNE)
        {
            (*(uint8_t *)&rec) = *(__IO uint8_t *)&SPI1->DR;
            break;
        }
    }
    while((SPI1->SR & SPI_SR_FTLVL) != 0x00000000U);  //等待发送FIFO为空
    while((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY);   //等待SPI不忙
    while ((SPI1->SR & SPI_SR_FRLVL) != 0x00000000U)  //当接收FIFO不为空
    {
        tmpreg8 = *((__IO uint8_t *)&SPI1->DR);
        UNUSED(tmpreg8);
    }
    return rec;
}

/**
 * @brief SPI写入一个字节数据
 * @param dat  字节数据
 */
void HARDSPI::write_data(uint8_t dat)
{
    volatile uint32_t tmpreg_ovr = 0x00U; 
    __IO uint8_t  tmpreg8 = 0;
    if ((SPI1->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
    {
        SPI1->CR1 |= SPI_CR1_SPE;
    }
    *((__IO uint8_t *)&SPI1->DR) = (uint8_t)dat;

    while((SPI1->SR & SPI_SR_FTLVL) != (0x00000000U));  //等待发送FIFO为空
    while((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY);   //等待SPI不忙
    while ((SPI1->SR & SPI_SR_FRLVL) != (0x00000000U))  //当接收FIFO不为空
    {
        tmpreg8 = *((__IO uint8_t *)&SPI1->DR);
        UNUSED(tmpreg8);
    }
    tmpreg_ovr = SPI1->DR; 
    tmpreg_ovr = SPI1->SR; 
    (void)tmpreg_ovr; 
}

/**
 * @brief SPI读一个字节数据
 * @return 读取到的数据
 */
uint8_t HARDSPI::read_data(void)
{
    return wr_data(0xff);
}

HARDSPI hardspi;

#endif // !_HARDSPI_H_





