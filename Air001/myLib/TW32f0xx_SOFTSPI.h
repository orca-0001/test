#ifndef    _CH32V_SOFTSPI_H_
#define    _CH32V_SOFTSPI_H_ 

#include <TW32f0xx.h>
#include "wiring.h"

#define     SOFTSPI_CPHA 0
#define     SOFTSPI_CPOL 0

class SOFTSPI{
public:
    SOFTSPI();
    void begin(uint8_t sckpin, uint8_t mosipin, uint8_t misopin); //SOFTSPI初始化

    uint8_t wr_data(uint8_t data); //读/写函数
    void write_data(uint8_t data); //写函数
    uint8_t read_data(); //读函数

private:
    GPIO_TypeDef *SCK_GPIOx=NULL;
    GPIO_TypeDef *SI_GPIOx=NULL;
    GPIO_TypeDef *SO_GPIOx=NULL;
    uint8_t sck_position;
    uint8_t si_position;
    uint8_t so_position;
};

//========================================================================
// 描述: 构造函数.
// 参数: none
// 返回: none.
//========================================================================
SOFTSPI::SOFTSPI()
{}

//========================================================================
// 描述: 初始化函数.
// 参数: sckpin:sck引脚; mosipin: mosi引脚; misopin：miso引脚.
// 返回: none.
//========================================================================
void SOFTSPI::begin(uint8_t sckpin, uint8_t mosipin, uint8_t misopin)
{
    SCK_GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[sckpin];
    SI_GPIOx  = (GPIO_TypeDef*)DIGITAL_PIN_PORT[mosipin];
    SO_GPIOx  = (GPIO_TypeDef*)DIGITAL_PIN_PORT[misopin];

    sck_position = DIGITAL_PIN_NUM[sckpin];
    si_position  = DIGITAL_PIN_NUM[mosipin];
    so_position  = DIGITAL_PIN_NUM[misopin];
    //SCK引脚配置
    SCK_GPIOx->MODER &= ~(0x3UL << (sck_position * 2u));
    SCK_GPIOx->MODER |= (0x01U << (sck_position * 2u));   //设置为输出模式
    SCK_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (sck_position * 2u));   //设置引脚速度非常高
    SCK_GPIOx->OTYPER  &= ~(0x1UL << sck_position) ;    //复位为推挽输出
    SCK_GPIOx->BRR |= (0x1U << sck_position);  //SCK拉低
    //MOSI引脚配置
    SI_GPIOx->MODER &= ~(0x3UL << (si_position * 2u));
    SI_GPIOx->MODER |= (0x01U << (si_position * 2u));   //设置为输出模式
    SI_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (si_position * 2u));   //设置引脚速度非常高
    SI_GPIOx->OTYPER  &= ~(0x1UL << si_position) ;    //复位为推挽输出
    SI_GPIOx->BSRR |= (0x1U << si_position);  //MOSI拉高
    //MISO引脚配置
    SO_GPIOx->MODER &= ~(0x3UL << (so_position * 2u));  //设置为浮空输入模式
    SO_GPIOx->OSPEEDR |= (GPIO_SPEED_FREQ_VERY_HIGH << (so_position * 2u));   //设置引脚速度非常高
    SO_GPIOx->PUPDR &= ~(0x3UL << (so_position * 2u));
    SO_GPIOx->PUPDR |= (GPIO_PULLUP << (so_position * 2u));    //MISO设置为上拉
}

//========================================================================
// 描述: 读写函数.
// 参数: sckpin:sck引脚; mosipin: mosi引脚; misopin：miso引脚.
// 返回: 读到的数据.
//========================================================================
uint8_t SOFTSPI::wr_data(uint8_t data)
{
    uint8_t datavalue=0;
    uint8_t wr_data = data;
#if SOFTSPI_CPHA==0 && SOFTSPI_CPOL==0
    for(int i=0;i<8;i++){
        if(wr_data&0x80){
            SI_GPIOx->BSRR |= (0x1U << si_position);  //MOSI拉高
        }else{
            SI_GPIOx->BRR |= (0x1U << si_position);  //MOSI拉低
        }
        wr_data <<=1;
        SCK_GPIOx->BSRR |= (0x1U << sck_position);  //SCK拉高
        datavalue <<=1;
        if( (SO_GPIOx->IDR&(0x1U<<so_position)) != 0x0U ){
            datavalue |= 0x01;
        }
        SCK_GPIOx->BRR |= (0x1U << sck_position);  //SCK拉低
        //delay(1);
    }
    
#elif SOFTSPI_CPHA==0 && SOFTSPI_CPOL==1
    for(i=0;i<8;i++){
        if(wr_data&0x80){
            SI_GPIOx->BSRR |= (0x1U << si_position);  //MOSI拉高
        }else{
            SI_GPIOx->BRR |= (0x1U << si_position);  //MOSI拉低
        }
        wr_data <<=1;
        SCK_GPIOx->BRR |= (0x1U << sck_position);  //SCK拉低
        datavalue <<=1;
        if( (SO_GPIOx->IDR&(0x1U<<so_position)) != 0x0U ){
            datavalue |= 0x01;
        }
        SCK_GPIOx->BSRR |= (0x1U << sck_position);  //SCK拉高
        //delay(1);
    }  
    
#elif SOFTSPI_CPHA==1 && SOFTSPI_CPOL==0
    for(i=0;i<8;i++){
        SCK_GPIOx->BSRR |= (0x1U << sck_position);  //SCK拉高
        if(wr_data&0x80){
            SI_GPIOx->BSRR |= (0x1U << si_position);  //MOSI拉高
        }else{
            SI_GPIOx->BRR |= (0x1U << si_position);  //MOSI拉低
        }
        wr_data <<=1;
        SCK_GPIOx->BRR |= (0x1U << sck_position);  //SCK拉低
        datavalue <<=1;
        if( (SO_GPIOx->IDR&(0x1U<<so_position)) != 0x0U ){
            datavalue |= 0x01;
        }
        //delay(1);
    } 
    
#elif SOFTSPI_CPHA==1 && SOFTSPI_CPOL==1
    for(i=0;i<8;i++){
        SCK_GPIOx->BRR |= (0x1U << sck_position);  //SCK拉低
        if(wr_data&0x80){
            SI_GPIOx->BSRR |= (0x1U << si_position);  //MOSI拉高
        }else{
            SI_GPIOx->BRR |= (0x1U << si_position);  //MOSI拉低
        }
        wr_data <<=1;
        SCK_GPIOx->BSRR |= (0x1U << sck_position);  //SCK拉高
        datavalue <<=1;
        if( (SO_GPIOx->IDR&(0x1U<<so_position)) != 0x0U ){
            datavalue |= 0x01;
        }
        SCK_GPIOx->BSRR |= (0x1U << sck_position);  //SCK拉高
        // delay(1);
    } 
#endif
    return datavalue;
}

//========================================================================
// 描述: 写函数.
// 参数: data：数据.
// 返回: none.
//========================================================================
void SOFTSPI::write_data(uint8_t data)
{
    wr_data(data);
}

//========================================================================
// 描述: 读函数.
// 参数: none.
// 返回: 读到的数据.
//========================================================================
uint8_t SOFTSPI::read_data()
{
    return wr_data(0xff);
}

#endif  //softspi.h
