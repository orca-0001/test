#ifndef __EXTI_H_
#define __EXTI_H_

#include "TW32f0xx.h"
#include "pins_arduino.h"
#include "wiring.h"

#define  EXTI_Trigger_Rising                    (GPIO_MODE_IT_RISING)   /*!< 上升沿中断      */
#define  EXTI_Trigger_Falling                   (GPIO_MODE_IT_FALLING)   /*!< 下降沿中断      */
#define  EXTI_Trigger_Rising_Falling            (GPIO_MODE_IT_RISING_FALLING)   /*!< 双边沿中断      */

typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr exti_fun[19] = {NULL};

/**
 * @brief 引脚中断初始化函数
 * @param pin 引脚名
 *              PA0 - PA15
 *              PB0 - PB15
 *              PF0 - PF15
 * @param interrupt_type 中断类型
 *              GPIO_MODE_IT_RISING     上升沿中断    
 *              GPIO_MODE_IT_FALLING    下降沿中断
 *              GPIO_MODE_IT_RISING_FALLING     双边沿中断
 */
void Pin_attachInterrupt(PIN_Name pin, uint32_t interrupt_type, void (*userFunc)(void))
{
    uint32_t position = DIGITAL_PIN_NUM[pin];
    GPIO_TypeDef* GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[pin];
    if(interrupt_type == GPIO_MODE_IT_RISING){         //上升沿中断
        GPIO_Init(GPIOx,position,GPIO_MODE_INPUT,GPIO_PULLDOWN,GPIO_SPEED_FREQ_VERY_HIGH);
    }else if(interrupt_type == GPIO_MODE_IT_FALLING){  //下降沿中断
        GPIO_Init(GPIOx,position,GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_VERY_HIGH);
    }else{
        GPIO_Init(GPIOx,position,GPIO_MODE_INPUT,GPIO_NOPULL,GPIO_SPEED_FREQ_VERY_HIGH);
    }
    EXTI->EXTICR[position >> 2u] &= ~(0x0FuL << (8u * (position & 0x03u)));
    EXTI->EXTICR[position >> 2u] |= (GPIO_GET_INDEX(GPIOx) << (8u * (position & 0x03u))); 

    EXTI->IMR |= (0x1UL << position);   //中断唤醒未屏蔽
    EXTI->EMR &= ~(0x1UL << position);  //事件唤醒屏蔽

    EXTI->RTSR &= ~(0x1UL << position);
    if ((interrupt_type & 0x00100000u) == 0x00100000u)
    {
        EXTI->RTSR |= (0x1UL << position);      //上升沿触发使能
    }
    EXTI->FTSR &= ~(0x1UL << position);
    if ((interrupt_type & 0x00200000u) == 0x00200000u)
    {
        EXTI->FTSR |= (0x1UL << position);      //下降沿触发使能
    }

    if(position < 2){
        NVIC_SetPriority(EXTI0_1_IRQn, 0);  //配置中断优先级
        NVIC_EnableIRQ(EXTI0_1_IRQn);   //使能EXTI中断
    }else if(position < 4){
        NVIC_SetPriority(EXTI2_3_IRQn, 0);
        NVIC_EnableIRQ(EXTI2_3_IRQn); 
    }else{
        NVIC_SetPriority(EXTI4_15_IRQn, 0);
        NVIC_EnableIRQ(EXTI4_15_IRQn);
    }
    exti_fun[position] = userFunc;
}

//========================================================================
// 描述: 引脚中断处理函数.
// 参数: none.
// 返回: none.
//========================================================================
#ifdef __cplusplus
extern "C" {
#endif

void EXTI0_1_IRQHandler(void)
{
    for(int i=0;i<2;i++){
        if((EXTI->PR & (0x0001u<<i)) != 0){
            EXTI->PR |= (0x0001u<<i);
            exti_fun[i]();
        }
    }    
}

void EXTI2_3_IRQHandler(void)
{
    for(int i=0;i<2;i++){
        if((EXTI->PR & (0x0004u<<i)) != 0){
            EXTI->PR |= (0x0004u<<i);
            exti_fun[i+2]();
        }
    }    
}

void EXTI4_15_IRQHandler(void)
{
    for(int i=0;i<12;i++){
        if((EXTI->PR & (0x0010u<<i)) != 0){
            EXTI->PR |= (0x0010u<<i);
            exti_fun[i+4]();
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif