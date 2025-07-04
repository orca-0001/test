#ifndef _PWM_H_
#define _PWM_H_

#include "TW32f0xx.h"
#include "wiring.h"

#ifndef PWM_DUTY_MAX
#define PWM_DUTY_MAX		1000
#endif

typedef enum
{
    TIM1_CH1 = 0,
    TIM1_CH2,
    TIM1_CH3,
    TIM1_CH4,
    TIM3_CH1,
    TIM3_CH2,
    TIM3_CH3,
    TIM3_CH4,   
    TIM14_CH1,  
    TIM16_CH1,
    TIM17_CH1,
} PWM_CHANNEL_Name;

static const uint32_t GET_PWM_TIM[11]={
    TIM1_BASE,  
    TIM1_BASE,  
    TIM1_BASE,  
    TIM1_BASE,   
    TIM3_BASE,  
    TIM3_BASE,  
    TIM3_BASE,  
    TIM3_BASE,
    TIM14_BASE,
    TIM16_BASE,  
    TIM17_BASE,
};

void PWM_GPIO_Init(PWM_CHANNEL_Name channel, PIN_Name pin);

/**
 * @brief PWM初始化函数
 * @param channel 通道
 *              详见PWM_CHANNEL_Name。
 * @param pin 引脚名
 *              PA0-PA15  
 *              PB0-PB15
 *              PF0-PF15
 * 
 * 引脚分布如下：
 *      TIM1_CH1    PA3(AF13) 
 *      TIM1_CH2    PA13(AF13)
 *      TIM1_CH3    PA0(AF13)       PB6(AF1)
 *      TIM1_CH4    PA1(AF13)
 *      TIM3_CH1    PA2(AF13)       PA6(AF1)  
 *      TIM3_CH2    PA5(AF13)       PA7(AF1)        PB5(AF1)
 *      TIM3_CH3    PA4(AF13)       PB0(AF1)
 *      TIM3_CH4    PB1(AF1) 
 *      TIM14_CH1   PA4(AF4)        PA7(AF4)        PB1(AF0)        PF0(AF2)       PF1(AF13)
 *      TIM16_CH1   PA6(AF5)
 *      TIM17_CH1   PA7(AF5) 
 * 注意同一组PWM频率一样，占空比可以不一样。  
 */
void PWM_Init(PWM_CHANNEL_Name channel, PIN_Name pin, uint32_t freq, uint16_t duty)
{
    uint16_t arr,psc;
    TIM_TypeDef* TIMx = (TIM_TypeDef*) GET_PWM_TIM[channel];
    if(duty > PWM_DUTY_MAX)	
	{
		duty = PWM_DUTY_MAX;
	}
    uint16_t freq_div = ((HSE_VALUE / freq) >> 16);						
	uint16_t period_temp = (HSE_VALUE / freq / (freq_div+1));	
	uint16_t match_temp = (uint16_t)(period_temp*((float)duty/PWM_DUTY_MAX));	
	arr = period_temp;
	psc = freq_div;	

    PWM_GPIO_Init(channel, pin);

    //定时器计数设置
    if(TIMx == TIM1)
	{
        __HAL_RCC_TIM1_CLK_ENABLE();
        TIMx->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
        TIMx->CR1 |= TIM_COUNTERMODE_UP;    //设置为向上计数
	}else if(TIMx == TIM3)
	{
        __HAL_RCC_TIM3_CLK_ENABLE();
        TIMx->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
        TIMx->CR1 |= TIM_COUNTERMODE_UP;    //设置为向上计数
	}else if(TIMx == TIM14)
    {
        __HAL_RCC_TIM14_CLK_ENABLE();
    }else if(TIMx == TIM16)
    {
        __HAL_RCC_TIM16_CLK_ENABLE();
    }else if(TIMx == TIM17)
    {
        __HAL_RCC_TIM17_CLK_ENABLE();
    }
    TIMx->CR1 &= ~(TIM_CR1_CKD|TIM_CR1_ARPE);   ////时钟不分频,自动重装载寄存器没有缓冲
    if((TIMx==TIM1)||(TIMx==TIM16)||(TIMx==TIM17)){
        TIMx->RCR = 0;   //不重复计数
    }
    TIMx->PSC = psc;  	//预分频器设置
    TIMx->ARR = arr;  	//设定计数器自动重装值

    //PWM设置
    //ch1
    if(  (channel == TIM1_CH1) || (channel == TIM3_CH1)||      \
         (channel == TIM14_CH1) || (channel == TIM16_CH1) || (channel == TIM17_CH1) )
    {
        TIMx->CCER  &= ~TIM_CCER_CC1E;   //关闭OC1
        TIMx->CCMR1 &= ~(TIM_CCMR1_OC1M | TIM_CCMR1_CC1S);
        TIMx->CCMR1 |= TIM_OCMODE_PWM1;  //输出配置为PWM1模式
        TIMx->CCER  &= ~TIM_CCER_CC1P;
        TIMx->CCER  |= TIM_OCPOLARITY_HIGH;  //OC通道输出高电平有效
        if(TIMx == TIM1){
            TIMx->CCER &= ~TIM_CCER_CC1NP;
            TIMx->CCER |= TIM_OCNPOLARITY_HIGH;   //OCN通道输出高电平有效
            TIMx->CCER &= ~TIM_CCER_CC1NE;     //0C1N禁止输出       
        }
        if((TIMx == TIM1)||(TIMx == TIM16)||(TIMx == TIM17)){
            TIMx->CR2 &= ~(TIM_CR2_OIS1|TIM_CR2_OIS1N);
            TIMx->CR2 |= TIM_OCIDLESTATE_RESET;     //空闲状态OC1输出低电平
            TIMx->CR2 |= TIM_OCNIDLESTATE_RESET;    //空闲状态OC1N输出低电平
        }
        TIMx->CCR1 = match_temp;
        TIMx->CCER |= 0x0001U;
    }
    //ch2
    if( (channel == TIM1_CH2)||(channel == TIM3_CH2) )
    {
        TIMx->CCER &= ~TIM_CCER_CC2E;   //关闭OC2
        TIMx->CCMR1 &= ~(TIM_CCMR1_OC2M | TIM_CCMR1_CC2S);
        TIMx->CCMR1 |= (TIM_OCMODE_PWM1 << 8U);   //输出配置为PWM1模式
        TIMx->CCER &= ~TIM_CCER_CC2P;
        TIMx->CCER |= (TIM_OCPOLARITY_HIGH << 4U);  //OC通道输出高电平有效
        if(TIMx == TIM1){
            TIMx->CCER &= ~TIM_CCER_CC2NP;
            TIMx->CCER |= (TIM_OCNPOLARITY_HIGH << 4U);   //OCN通道输出高电平有效
            TIMx->CCER &= ~TIM_CCER_CC2NE;     //OC2N禁止输出   
        }
        if((TIMx == TIM1)||(TIMx == TIM16)||(TIMx == TIM17)){
            TIMx->CR2 &= ~(TIM_CR2_OIS2|TIM_CR2_OIS2N);
            TIMx->CR2 |= (TIM_OCIDLESTATE_RESET << 2U);     //空闲状态OC2输出低电平
            TIMx->CR2 |= (TIM_OCNIDLESTATE_RESET << 2U);    //空闲状态OC2N输出低电平
        }
        TIMx->CCR2 = match_temp;
        TIMx->CCER |= (0x0001U << 4);
    }
    //ch3
    if( (channel == TIM1_CH3)||(channel == TIM3_CH3) )
    {
        TIMx->CCER &= ~TIM_CCER_CC3E;   //关闭OC3
        TIMx->CCMR2 &= ~(TIM_CCMR2_OC3M | TIM_CCMR2_CC3S);
        TIMx->CCMR2 |= TIM_OCMODE_PWM1;   //输出配置为PWM1模式
        TIMx->CCER &= ~TIM_CCER_CC3P;
        TIMx->CCER |= (TIM_OCPOLARITY_HIGH << 8U);  //OC通道输出高电平有效
        if(TIMx == TIM1){
            TIMx->CCER &= ~TIM_CCER_CC3NP;
            TIMx->CCER |= (TIM_OCNPOLARITY_HIGH << 8U);   //OCN通道输出高电平有效
            TIMx->CCER &= ~TIM_CCER_CC3NE;     //OC3N禁止输出   
        }
        if((TIMx == TIM1)||(TIMx == TIM16)||(TIMx == TIM17)){
            TIMx->CR2 &= ~(TIM_CR2_OIS3|TIM_CR2_OIS3N);
            TIMx->CR2 |= (TIM_OCIDLESTATE_RESET << 4U);     //空闲状态OC3输出低电平
            TIMx->CR2 |= (TIM_OCNIDLESTATE_RESET << 4U);    //空闲状态OC3N输出低电平
        }
        TIMx->CCR3 = match_temp;
        TIMx->CCER |= (0x0001U << 8U);
    }
    //ch4
    if( (channel == TIM1_CH4)||(channel == TIM3_CH4) )
    {
        TIMx->CCER &= ~TIM_CCER_CC4E;   //关闭OC4
        TIMx->CCMR2 &= ~(TIM_CCMR2_OC4M | TIM_CCMR2_CC4S);
        TIMx->CCMR2 |= (TIM_OCMODE_PWM1 << 8U);  //输出配置为PWM1模式
        TIMx->CCER &= ~TIM_CCER_CC4P;
        TIMx->CCER |= (TIM_OCPOLARITY_HIGH << 12U);  //OC通道输出高电平有效
        if((TIMx == TIM1)||(TIMx == TIM16)||(TIMx == TIM17)){
            TIMx->CR2 &= ~TIM_CR2_OIS4;
            TIMx->CR2 |= (TIM_OCIDLESTATE_RESET << 6U);     //空闲状态OC4输出低电平
        }
        TIMx->CCR4 = match_temp;
        TIMx->CCER |= (0x0001U << 12U);
    }
    if((TIMx==TIM1)||(TIMx==TIM16)||(TIMx==TIM17)){
        TIMx->BDTR |= TIM_BDTR_MOE; //主输出使能
    }
    if((TIMx->SMCR & TIM_SMCR_SMS) != TIM_SLAVEMODE_TRIGGER)
    {
        TIMx->CR1 |= TIM_CR1_CEN;   //开启计数器
    }
}

//========================================================================
// 描述: 更新PWM占空比.
// 参数: 定时器通道、占空比（0-100）%
// 返回: none.
//========================================================================
void PWM_Duty_Updata (PWM_CHANNEL_Name channel, uint16_t duty)
{
    uint16_t arr,psc;
    TIM_TypeDef* TIMx = (TIM_TypeDef*) GET_PWM_TIM[channel];
	if(duty > PWM_DUTY_MAX)	
	{
		duty = PWM_DUTY_MAX;
	}
	uint16_t period_temp = TIMx->ARR;											// 获取自动重装载值
	uint16_t match_temp = (uint16_t)(period_temp*((float)duty/PWM_DUTY_MAX));	// 计算占空比
    //PWM设置
    //ch1
    if(  (channel == TIM1_CH1) || (channel == TIM3_CH1)||      \
         (channel == TIM14_CH1) || (channel == TIM16_CH1) || (channel == TIM17_CH1) )
    {
        TIMx->CCER  &= ~TIM_CCER_CC1E;   //关闭OC1
        TIMx->CCR1 = match_temp;
        TIMx->CCER |= 0x0001U;      //开启OC1
    }
    //ch2
    if( (channel == TIM1_CH2)||(channel == TIM3_CH2) )
    {
        TIMx->CCER &= ~TIM_CCER_CC2E;   //关闭OC2
        TIMx->CCR2 = match_temp;
        TIMx->CCER |= (0x0001U << 4);   //开启OC2
    }
    //ch3
    if( (channel == TIM1_CH3)||(channel == TIM3_CH3) )
    {
        TIMx->CCER &= ~TIM_CCER_CC3E;   //关闭OC3
        TIMx->CCR3 = match_temp;
        TIMx->CCER |= (0x0001U << 8U);  //开启OC3
    }
    //ch4
    if( (channel == TIM1_CH4)||(channel == TIM3_CH4) )
    {
        TIMx->CCER &= ~TIM_CCER_CC4E;   //关闭OC4
        TIMx->CCR4 = match_temp;
        TIMx->CCER |= (0x0001U << 12U); //开启OC4
    }
}

//========================================================================
// 描述: 更新PWM频率.
// 参数: 定时器通道、频率、占空比（0-100）%
// 返回: none.
//========================================================================
void PWM_Frequency_Updata (PWM_CHANNEL_Name channel, uint32_t freq, uint16_t duty)
{
    TIM_TypeDef* TIMx = (TIM_TypeDef*) GET_PWM_TIM[channel];
    if(duty > PWM_DUTY_MAX)	
	{
		duty = PWM_DUTY_MAX;
	}
    uint16_t freq_div = ((HSE_VALUE / freq) >> 16);						
	uint16_t period_temp = (HSE_VALUE / freq / (freq_div+1));	
	uint16_t match_temp = (uint16_t)(period_temp*((float)duty/PWM_DUTY_MAX));	
	
    // TIMx->CR1 &= ~TIM_CR1_CEN;   //关闭计数器

    TIMx->PSC = freq_div;  	//预分频器设置
    TIMx->ARR = period_temp;  	//设定计数器自动重装值

    //PWM设置
    //ch1
    if(  (channel == TIM1_CH1) || (channel == TIM3_CH1)||      \
         (channel == TIM14_CH1) || (channel == TIM16_CH1) || (channel == TIM17_CH1) )
    {
        TIMx->CCER  &= ~TIM_CCER_CC1E;   //关闭OC1
        TIMx->CCR1 = match_temp;
        TIMx->CCER |= 0x0001U;
    }
    //ch2
    if( (channel == TIM1_CH2)||(channel == TIM3_CH2) )
    {
        TIMx->CCER &= ~TIM_CCER_CC2E;   //关闭OC2
        TIMx->CCR2 = match_temp;
        TIMx->CCER |= (0x0001U << 4);
    }
    //ch3
    if( (channel == TIM1_CH3)||(channel == TIM3_CH3) )
    {
        TIMx->CCER &= ~TIM_CCER_CC3E;   //关闭OC3
        TIMx->CCR3 = match_temp;
        TIMx->CCER |= (0x0001U << 8U);
    }
    //ch4
    if( (channel == TIM1_CH4)||(channel == TIM3_CH4) )
    {
        TIMx->CCER &= ~TIM_CCER_CC4E;   //关闭OC4
        TIMx->CCR4 = match_temp;
        TIMx->CCER |= (0x0001U << 12U);
    }
    if((TIMx==TIM1)||(TIMx==TIM16)||(TIMx==TIM17)){
        TIMx->BDTR |= TIM_BDTR_MOE; //主输出使能
    }
    if((TIMx->SMCR & TIM_SMCR_SMS) != TIM_SLAVEMODE_TRIGGER)
    {
        TIMx->CR1 |= TIM_CR1_CEN;   //开启计数器
    }
}


/**
 * @brief PWM引脚初始化函数
 * @param channel 通道
 *              详见PWM_CHANNEL_Name。
 * @param pin 引脚名
 *              PA0-PA15  
 *              PB0-PB15
 *              PF0-PF15
 */
void PWM_GPIO_Init(PWM_CHANNEL_Name channel, PIN_Name pin)
{
    GPIO_TypeDef *GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[pin];
    uint32_t position = DIGITAL_PIN_NUM[pin];

    GPIO_Init(GPIOx,position,GPIO_MODE_AF_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_VERY_HIGH);

    if(channel == TIM1_CH1){
        switch (pin){
            case PA3:
                GPIOx->AFR[0] &= ~(0xFu << 12);
                GPIOx->AFR[0] |= (GPIO_AF13 << 12);              
                break;
            default:
                break;
        }
    }else if(channel == TIM1_CH2){
        switch (pin){
            case PA13:
                GPIOx->AFR[1] &= ~(0xFu << 20);
                GPIOx->AFR[1] |= (GPIO_AF13 << 20);              
                break;
            default:
                break;
        }
    }else if(channel == TIM1_CH3){
        switch (pin){
            case PA0:
                GPIOx->AFR[0] &= ~0xFu;
                GPIOx->AFR[0] |= GPIO_AF13;              
                break;
            case PB6:
                GPIOx->AFR[0] &= ~(0xFu << 24);
                GPIOx->AFR[0] |= (GPIO_AF1 << 24);              
                break;
            default:
                break;
        }
    }else if(channel == TIM1_CH4){
        switch (pin){
            case PA1:
                GPIOx->AFR[0] &= ~(0xFu << 4);
                GPIOx->AFR[0] |= (GPIO_AF13 << 4);              
                break;
            default:
                break;
        }
    }else if(channel == TIM3_CH1){
        switch (pin){
            case PA2:
                GPIOx->AFR[0] &= ~(0xFu << 8);
                GPIOx->AFR[0] |= (GPIO_AF13 << 8);              
                break;
            case PA6:
                GPIOx->AFR[0] &= ~(0xFu << 24);
                GPIOx->AFR[0] |= (GPIO_AF1 << 24);              
                break;
            default:
                break;
        }
    }else if(channel == TIM3_CH2){
        switch (pin){
            case PA5:
                GPIOx->AFR[0] &= ~(0xFu << 20);
                GPIOx->AFR[0] |= (GPIO_AF13 << 20);              
                break;
            case PA7:
                GPIOx->AFR[0] &= ~(0xFu << 28);
                GPIOx->AFR[0] |= (GPIO_AF1 << 28);              
                break;
            case PB5:
                GPIOx->AFR[0] &= ~(0xFu << 20);
                GPIOx->AFR[0] |= (GPIO_AF1 << 20);              
                break;
            default:
                break;
        }
    }else if(channel == TIM3_CH3){
        switch (pin){
            case PA4:
                GPIOx->AFR[0] &= ~(0xFu << 16);
                GPIOx->AFR[0] |= (GPIO_AF13 << 16);              
                break;
            case PB0:
                GPIOx->AFR[0] &= ~0xFu;
                GPIOx->AFR[0] |= GPIO_AF1;              
                break;
            default:
                break;
        }
    }else if(channel == TIM3_CH4){
        switch (pin){
            case PB1:
                GPIOx->AFR[0] &= ~(0xFu << 4);
                GPIOx->AFR[0] |= (GPIO_AF1 << 4);              
                break;
            default:
                break;
        }
    }else if(channel == TIM14_CH1){
        switch (pin){
            case PA4:
                GPIOx->AFR[0] &= ~(0xFu << 16);
                GPIOx->AFR[0] |= (GPIO_AF4 << 16);              
                break;
            case PA7:
                GPIOx->AFR[0] &= ~(0xFu << 28);
                GPIOx->AFR[0] |= (GPIO_AF4 << 28);              
                break;
            case PB1:
                GPIOx->AFR[0] &= ~(0xFu << 4);
                GPIOx->AFR[0] |= (GPIO_AF0 << 4);              
                break;
            case PF0:
                GPIOx->AFR[0] &= ~0xFu;
                GPIOx->AFR[0] |= GPIO_AF2;              
                break;
            case PF1:
                GPIOx->AFR[0] &= ~(0xFu << 4);
                GPIOx->AFR[0] |= (GPIO_AF13 << 4);              
                break;
            default:
                break;
        }
    }else if(channel == TIM16_CH1){
        switch (pin){
            case PA6:
                GPIOx->AFR[0] &= ~(0xFu << 24);
                GPIOx->AFR[0] |= (GPIO_AF5 << 24);              
                break;
            default:
                break;
        }
    }else if(channel == TIM17_CH1){
        switch (pin){
            case PA7:
                GPIOx->AFR[0] &= ~(0xFu << 28);
                GPIOx->AFR[0] |= (GPIO_AF5 << 28);              
                break;
            default:
                break;
        }
    }else{
        return;
    }
}



#endif