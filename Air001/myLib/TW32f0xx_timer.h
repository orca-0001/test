/*
TW32f0xx定时器功能驱动
    一个由16位被可编程分频器驱动的自动装载高级定时器TIM1
    四个16位通用定时器TIM3, TIM14, TIM16/TIM17

使用范例：
void myfun(void)
{
    
}
TIM_Init(TIM2,5000,myfun);
*/
#ifndef _TIMER_H_
#define _TIMER_H_ 

#include "TW32f0xx.h"
#include "pins_arduino.h"
#include "wiring.h"

typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr tim_fun[5] = { NULL };

/**
 * @brief   TIM初始化
 * @param   TIMx 定时器
 *                TIM1
 *                TIM3
 *                TIM14
 *                TIM16
 *                TIM17
 * @param   period 周期(us)
 *              范围: 1 - 6553500us
 * @param   userFunc 回调函数
 */
void TIM_attachInterrupt(TIM_TypeDef* TIMx, uint32_t period, void (*userFunc)(void))
{
    if((TIMx==TIM1)||(TIMx==TIM3)){
        TIMx->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
        TIMx->CR1 |= TIM_COUNTERMODE_UP;    //设置为向上计数
    }
    TIMx->CR1 &= ~(TIM_CR1_CKD|TIM_CR1_ARPE);   ////时钟不分频,自动重装载寄存器没有缓冲
    if((TIMx==TIM1)||(TIMx==TIM16)||(TIMx==TIM17)){
        TIMx->RCR = 0;   //不重复计数
    }

    //1.使能时钟，记录回调函数
    if(TIMx == TIM1)
	{
        tim_fun[0] = userFunc;
        __HAL_RCC_TIM1_CLK_ENABLE();
        NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 3);  //设置中断优先级
        NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);     //中断使能
	}
	else if(TIMx == TIM3)
	{
        tim_fun[1] = userFunc;
        __HAL_RCC_TIM3_CLK_ENABLE();
        NVIC_SetPriority(TIM3_IRQn, 3);  //设置中断优先级
        NVIC_EnableIRQ(TIM3_IRQn);     //中断使能
	}
    else if(TIMx == TIM14)
    {
        tim_fun[2] = userFunc;
        __HAL_RCC_TIM14_CLK_ENABLE();
        NVIC_SetPriority(TIM14_IRQn, 3);  //设置中断优先级
        NVIC_EnableIRQ(TIM14_IRQn);     //中断使能
    }
    else if(TIMx == TIM16)
    {
        tim_fun[3] = userFunc;
        __HAL_RCC_TIM16_CLK_ENABLE();
        NVIC_SetPriority(TIM16_IRQn, 3);  //设置中断优先级
        NVIC_EnableIRQ(TIM16_IRQn);     //中断使能
    }
    else if(TIMx == TIM17)
    {
        tim_fun[4] = userFunc;
        __HAL_RCC_TIM17_CLK_ENABLE();
        NVIC_SetPriority(TIM17_IRQn, 3);  //设置中断优先级
        NVIC_EnableIRQ(TIM17_IRQn);     //中断使能
    }

    if (period > 50000) 
    {
        TIMx->PSC =  2399;  	//预分频器设置
        TIMx->ARR =  period / 100-1;  	//设定计数器自动重装值
    }else 
    {
        TIMx->PSC = 23;   	//预分频器设置
        TIMx->ARR = period-1;  	//设定计数器自动重装值
    }
    TIMx->EGR = TIM_EGR_UG;     //产生更新事件
    TIMx->DIER |= TIM_DIER_UIE;     //允许更新中断

    // if((TIMx->SMCR & TIM_SMCR_SMS) != TIM_SLAVEMODE_TRIGGER)
    // {
    //     TIMx->CR1 |= TIM_CR1_CEN;   //开启计数器
    // }
}

/**
 * @brief   更新周期
 * @param   TIMx 定时器
 *                TIM1
 *                TIM3
 *                TIM14
 *                TIM16/TIM17
 * @param   period 周期
 *              范围: 1 - 6553500us
 */
void TIM_Duty_Updata(TIM_TypeDef* TIMx, uint32_t period)
{
    //1.设置定时周期
    if (period > 50000) 
    {
        TIMx->PSC =  2399;  	//预分频器设置
        TIMx->ARR =  period / 100-1;  	//设定计数器自动重装值
    }else 
    {
        TIMx->PSC = 23;   	//预分频器设置
        TIMx->ARR = period-1;  	//设定计数器自动重装值
    }
}

/**
 * @brief   停止定时器计数
 * @param   TIMx 定时器
 *                TIM1
 *                TIM3
 *                TIM14
 *                TIM16/TIM17
 */
void TIM_Disable(TIM_TypeDef* TIMx)
{
    if ((TIMx->CCER & TIM_CCER_CCxE_MASK) == 0UL) 
    { 
        if((TIMx->CCER & TIM_CCER_CCxNE_MASK) == 0UL) 
        { 
            TIMx->CR1 &= ~(TIM_CR1_CEN); 
        }
    }
}

/**
 * @brief   定时器开始计数
 * @param   TIMx 定时器
 *                TIM1
 *                TIM3
 *                TIM14
 *                TIM16/TIM17
 */
void TIM_Enable(TIM_TypeDef* TIMx)
{
    if((TIMx->SMCR & TIM_SMCR_SMS) != TIM_SLAVEMODE_TRIGGER)
    {
        TIMx->CR1 |= TIM_CR1_CEN;   //开启计数器
    }
}

/**
 * @brief   定时器解除占用
 * @param   TIMx 定时器
 *                TIM1
 *                TIM3
 *                TIM14
 *                TIM16/TIM17
 */
void TIM_detachInterrupt(TIM_TypeDef* TIMx)
{
    if(TIMx == TIM1)
	{
        tim_fun[0] = NULL;
        __HAL_RCC_TIM1_CLK_DISABLE();
	}
	else if(TIMx == TIM3)
	{
        tim_fun[1] = NULL;
        __HAL_RCC_TIM3_CLK_DISABLE();
	}
    else if(TIMx == TIM14)
    {
        tim_fun[2] = NULL;
        __HAL_RCC_TIM14_CLK_DISABLE();
    }
    else if(TIMx == TIM16)
    {
        tim_fun[3] = NULL;
        __HAL_RCC_TIM16_CLK_DISABLE();
    }
    else if(TIMx == TIM17)
    {
        tim_fun[4] = NULL;
        __HAL_RCC_TIM17_CLK_DISABLE();
    }
}

/**
 * @brief   定时器中断回调函数
 */
#ifdef __cplusplus
extern "C" {
#endif

void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
    if((TIM1->SR &0x1UL << 0U) == 0x1UL){
        TIM1->SR &= ~0x1UL; //清中断
        if(tim_fun[0]!=NULL)tim_fun[0]();
    }
}

void TIM3_IRQHandler(void)
{
    if((TIM3->SR &0x1UL << 0U) == 0x1UL){
        TIM3->SR &= ~0x1UL; //清中断
        if(tim_fun[1]!=NULL)tim_fun[1]();
    }
}

void TIM14_IRQHandler(void)
{
    if((TIM14->SR &0x1UL << 0U) == 0x1UL){
        TIM14->SR &= ~0x1UL; //清中断
        if(tim_fun[2]!=NULL)tim_fun[2]();
    }
}

void TIM16_IRQHandler(void)
{
    if((TIM16->SR &0x1UL << 0U) == 0x1UL){
        TIM16->SR &= ~0x1UL; //清中断
        if(tim_fun[3]!=NULL)tim_fun[3]();
    }
}

void TIM17_IRQHandler(void)
{
    if((TIM17->SR &0x1UL << 0U) == 0x1UL){
        TIM17->SR &= ~0x1UL; //清中断
        if(tim_fun[4]!=NULL)tim_fun[4]();
    }
}

#ifdef __cplusplus
}
#endif

#endif
