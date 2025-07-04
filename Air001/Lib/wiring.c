/**
 * @file wiring.c
 * @brief 基本硬件驱动函数定义
 * @version 0.1
 * @date 2023-01-03
 * 
 * @copyright Copyright (c) 2022 杭州天问五幺科技有限公司
 * 
 */
#include "TW32f0xx.h"
#include "wiring.h"
#include "string.h"

volatile unsigned long _systick_count = 0;

/**
 * @brief 获取系统运行时间（毫秒级）
 * @note  基于内核的SysTick
 * @return 运行时间（毫秒）
 */
uint32_t millis()
{
	unsigned long m;
    m = _systick_count;
	return m;
}

/**
 * @brief 获取系统运行时间（微秒级）
 * @note  基于内核的SysTick
 * @return 运行时间（微秒）
 */
unsigned long micros(void)
{
	volatile unsigned long lm,s,l;
	s  = SysTick->VAL;
	lm = _systick_count;
	l  = lm*1000 + (23999-s)/24;	
	return l;
}

/**
 * @brief 毫秒级延迟
 * @note  注意延时等待时，系统处于停滞状态，除了中断
 * @param ms 延时时间（毫秒）
 */
void delay(unsigned long ms)
{
	if (ms == 0)
	{
		ms = 1;
	}
	unsigned long start = millis();
	while (millis() - start <= (ms-1));
}

/**
 * @brief 微秒级延迟
 * @note  注意延时等待时，系统处于停滞状态，除了中断
 * @param ms 延时时间（微秒）
 */
void delayMicroseconds(uint32_t us)
{
    // unsigned long start = micros();
	// while (micros() - start <= (us-1));
	 volatile uint32_t temp=0;
	if(us<5){
        while(--us){
            for(temp=0;temp<1;temp++){;}
        }
	}else{
		us=(us*45)/120;
        while(--us){
			for(temp=0;temp<3;temp++){;}
		}
	}
}

/**
 * @brief 串口1初始化
 * @note  默认使用PA14(TX)/PA13(RX)，如需要使用其它引脚，请自己修改
 * @param baudrate 波特率
 */
void UART1_Init(uint32_t BAUD)
{
    __HAL_RCC_USART1_CLK_ENABLE();
    //将PA14设置为AF1, PA13设置为AF8.
    GPIOA->AFR[1] &= ~(0x0FF00000U);
    GPIOA->AFR[1] |=   0x01800000; 

    USART1->CR1 &=~USART_CR1_UE;    //失能USART1
    USART1->BRR = UART_BRR_SAMPLING16(HSI_VALUE, BAUD);
    USART1->CR3 = (uint32_t)0;
    USART1->CR2 = (uint32_t)0;
    USART1->CR1 |= (uint32_t)(USART_CR1_RE|USART_CR1_TE|USART_CR1_RXNEIE);
 
    USART1->CR1 |=  USART_CR1_UE;       //使能USART1

    /* 使能NVIC */
    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_EnableIRQ(USART1_IRQn); 
    
    //设置为复用功能模式
    GPIOA->MODER &= ~0x3C000000;
    GPIOA->MODER |=  0x28000000;
}

/**
 * @brief 串口1发送字符
 * @param c 字符
 */
void UART1_SendCh(unsigned char c)
{
    while((USART1->SR&USART_SR_TXE) != USART_SR_TXE); //等待发送缓冲为空
    USART1->DR = (c & (uint32_t)0x000000ff);
    while((USART1->SR&USART_SR_TC) == 0); //传送完成
}

/**
 * @brief 串口2初始化
 * @note  默认使用PA0(TX)/PA1(RX)，如需要使用其它引脚，请自己修改
 * @param baudrate 波特率
 */
void UART2_Init(uint32_t BAUD)
{
    __HAL_RCC_USART2_CLK_ENABLE();
    //将PA0,PA1设置为AF9.
    GPIOA->AFR[0] &= ~((uint32_t)0x000000FF);
    GPIOA->AFR[0] |= ((uint32_t)0x00000099); 

    USART2->CR1 &=~USART_CR1_UE;    //失能USART2
    USART2->BRR = UART_BRR_SAMPLING16(HSI_VALUE, BAUD);
    USART2->CR3 = (uint32_t)0;
    USART2->CR2 = (uint32_t)0;
    USART2->CR1 |= (uint32_t)(USART_CR1_RE|USART_CR1_TE|USART_CR1_RXNEIE);
 
    USART2->CR1 |=  USART_CR1_UE;       //使能USART2

    /* 使能NVIC */
    NVIC_SetPriority(USART2_IRQn, 0);
    NVIC_EnableIRQ(USART2_IRQn); 
    
    //设置为复用功能模式
    GPIOA->MODER &= ~0x0000000F;
    GPIOA->MODER |= 0x0000000A;
}

/**
 * @brief 串口2发送字符
 * @param c 字符
 */
void UART2_SendCh(unsigned char c)
{
    while((USART2->SR&USART_SR_TXE)!=USART_SR_TXE); //等待发送缓冲为空
    USART2->DR = (c & (uint32_t)0x000000ff);
    while((USART2->SR&USART_SR_TC)==0);   //传送完成
}

/**
 * @brief GPIO引脚初始化
 * @param GPIOx GPIO端口号
 *              GPIOA
 *              GPIOB
 *              GPIOF
 * @param GPIO_Pin GPIO引脚号(0~15)
 * @param mode 引脚模式
 *              GPIO_MODE_INPUT         浮空输入
 *              GPIO_MODE_OUTPUT_PP     通用推挽输出 
 *              GPIO_MODE_OUTPUT_OD     通用开漏输出
 *              GPIO_MODE_AF_PP         复用推挽输出
 *              GPIO_MODE_AF_OD         复用开漏输出
 *              GPIO_MODE_ANALOG        模拟输入
 * @param pull  上下拉设置
 *               GPIO_NOPULL            无上下拉
 *               GPIO_PULLUP            上拉
 *               GPIO_PULLDOWN          下拉
 * @param speed  引脚速度配置
 *               GPIO_SPEED_FREQ_LOW        非常低
 *               GPIO_SPEED_FREQ_MEDIUM     低速
 *               GPIO_SPEED_FREQ_HIGH       高速
 *               GPIO_SPEED_FREQ_VERY_HIGH  非常高
 */
void GPIO_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t mode, uint32_t pull, uint32_t speed)
{
    GPIOx->MODER &= ~(0x3UL << (GPIO_Pin * 2u));
    GPIOx->MODER |= ((mode & 0x00000003u) << (GPIO_Pin * 2u));

    if ((mode == GPIO_MODE_OUTPUT_PP) || (mode == GPIO_MODE_AF_PP) ||
        (mode == GPIO_MODE_OUTPUT_OD) || (mode == GPIO_MODE_AF_OD))
    {
        GPIOx->OSPEEDR &= ~(0x3UL << (GPIO_Pin * 2u));
        GPIOx->OSPEEDR |= (speed << (GPIO_Pin * 2u));   //设置引脚速度
        GPIOx->OTYPER  &= ~(0x1UL << GPIO_Pin) ;
        GPIOx->OTYPER  |= (((mode & 0x00000010u) >> 4u) << GPIO_Pin);   //设置引脚输出类型
    }
    GPIOx->PUPDR &= ~(0x3UL << (GPIO_Pin * 2u));
    GPIOx->PUPDR |= (pull << (GPIO_Pin * 2u));    //设置上下拉
}

/**
 * @brief GPIO端口初始化
 * @param GPIOx GPIO端口号
 *              GPIOA
 *              GPIOB
 *              GPIOF
 * @param mode 引脚模式
 *              GPIO_Mode_IN_FLOATING    浮空输入
 *              GPIO_Mode_AIN            模拟输入
 *              GPIO_Mode_Out_PP         通用推挽输出
 *              GPIO_Mode_Out_OD         通用开漏输出
 *              GPIO_Mode_AF_PP          复用推挽输出
 *              GPIO_Mode_AF_OD          复用开漏输出
 *              GPIO_Mode_IPD            下拉输入
 *              GPIO_Mode_IPU            上拉输入
 */
void GPIOX_Init(GPIO_TypeDef* GPIOx, uint8_t mode)
{
    if((mode == GPIO_Mode_IN_FLOATING)||(mode == GPIO_Mode_IPD)||(mode == GPIO_Mode_IPU)){
        if(mode == GPIO_Mode_IPD){   //下拉
            GPIOx->PUPDR = 0xAAAAAAAA;
        }else if(mode == GPIO_Mode_IPU){
            GPIOx->PUPDR = 0x55555555;
        }else{
            GPIOx->PUPDR = 0x0UL;   //无上下拉
        }
        GPIOx->MODER = 0;   //设置为输入模式
        
    }else if(mode == GPIO_Mode_AIN){
        GPIOx->PUPDR = 0x0UL;   //无上下拉 
        GPIOx->MODER = 0xFFFFFFFF;   //模拟模式

    }else if((mode == GPIO_Mode_Out_PP)||(mode == GPIO_Mode_Out_OD)){
        GPIOx->PUPDR = 0x0UL;
        GPIOx->MODER = 0x55555555;   //通用输出模式
        GPIOx->OTYPER = 0x0UL;        //推挽输出
        if(mode == GPIO_Mode_Out_OD){
            GPIOx->OTYPER = 0xFFFFUL;     //开漏输出 
        }
        GPIOx->OSPEEDR = 0xFFFFFFFF;    //引脚设置为高速模式
    }else if((mode == GPIO_Mode_AF_PP)||(mode == GPIO_Mode_AF_OD)){
        GPIOx->PUPDR = 0x0UL;   //无上下拉
        GPIOx->MODER = 0xAAAAAAAA;
        GPIOx->OTYPER &= 0x0UL;        //推挽输出
        if(mode == GPIO_Mode_AF_OD){
            GPIOx->OTYPER = 0xFFFFUL;     //开漏输出 
        }
        GPIOx->OSPEEDR = 0xFFFFFFFF;    //引脚设置为高速模式
    }
}

/**
 * @brief GPIO引脚初始化
 * @note Arduino引脚初始化接口函数
 * @param pin 引脚名
 *              PA0 - PA15
 *              PB0 - PB15
 *              PF0 - PF15          
 * @param mode 引脚模式
 *              GPIO_Mode_IN_FLOATING          浮空输入
 *              GPIO_Mode_AIN         模拟输入
 *              GPIO_Mode_Out_PP      通用推挽输出
 *              GPIO_Mode_Out_OD      通用开漏输出
 *              GPIO_Mode_AF_PP       复用推挽输出
 *              GPIO_Mode_AF_OD       复用开漏输出
 *              GPIO_Mode_IPD         下拉输入    
 *              GPIO_Mode_IPU         上拉输入
 */
void pinMode(PIN_Name pin, GPIOMode_TypeDef mode)
{
    GPIO_TypeDef *GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[pin];
    uint32_t position = DIGITAL_PIN_NUM[pin];
    switch(mode){
        case GPIO_Mode_IN_FLOATING:
            GPIO_Init(GPIOx,position,GPIO_MODE_INPUT,GPIO_NOPULL,GPIO_SPEED_FREQ_VERY_HIGH);
            break;
        case GPIO_Mode_IPD:
            GPIO_Init(GPIOx,position,GPIO_MODE_INPUT,GPIO_PULLDOWN,GPIO_SPEED_FREQ_VERY_HIGH);
            break;
        case GPIO_Mode_IPU:
            GPIO_Init(GPIOx,position,GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_VERY_HIGH);
            break;
        case GPIO_Mode_Out_PP:
            GPIO_Init(GPIOx,position,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_VERY_HIGH);
            break;
        case GPIO_Mode_Out_OD:
            GPIO_Init(GPIOx,position,GPIO_MODE_OUTPUT_OD,GPIO_NOPULL,GPIO_SPEED_FREQ_VERY_HIGH);
            break;
        case GPIO_Mode_AF_PP:
            GPIO_Init(GPIOx,position,GPIO_MODE_AF_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_VERY_HIGH);
            break;
        case GPIO_Mode_AF_OD:
            GPIO_Init(GPIOx,position,GPIO_MODE_AF_OD,GPIO_NOPULL,GPIO_SPEED_FREQ_VERY_HIGH);
            break;
        case GPIO_Mode_AIN:
            GPIO_Init(GPIOx,position,GPIO_MODE_ANALOG,GPIO_NOPULL,GPIO_SPEED_FREQ_VERY_HIGH);
            break;
    }
}

/**
 * @brief 设置GPIO引脚电平状态
 * @note Arduino引脚设置接口函数
 * @param pin 引脚名
 *              PA0 - PA15
 *              PB0 - PB15
 *              PF0 - PF15
 * @return 电平状态
 *          HIGH:高
 *          LOW：低
 */
void digitalWrite(PIN_Name pin,uint8_t state)
{
    if (state != GPIO_PIN_RESET)
    {
        ((GPIO_TypeDef*)DIGITAL_PIN_PORT[pin])->BSRR |= ((uint32_t)0x1 << DIGITAL_PIN_NUM[pin]);
    }
    else
    {
        ((GPIO_TypeDef*)DIGITAL_PIN_PORT[pin])->BRR |= ((uint32_t)0x1 << DIGITAL_PIN_NUM[pin]);
    }
}

/**
 * @brief 获取GPIO引脚电平状态
 * @note  Arduino引脚状态读取接口函数
 * @param pin 引脚名
 *              PA0 - PA15
 *              PB0 - PB15
 *              PF0 - PF15
 * @return 电平状态
 *          HIGH:高
 *          LOW：低
 */
uint8_t digitalRead(PIN_Name pin)
{
    if (((((GPIO_TypeDef*)DIGITAL_PIN_PORT[pin])->IDR) & (0x01UL<<DIGITAL_PIN_NUM[pin])) != 0x00u)
    {
       return GPIO_PIN_SET;
    }
    return GPIO_PIN_RESET;
}

/**
 * @brief 配置引脚复用功能
 * @note  
 * @param pin 引脚名
 *              PA0 - PA15
 *              PB0 - PB15
 *              PF0 - PF15
 * @param Alternate 复用功能
 *                      GPIO_AF0
 *                      GPIO_AF1 
 *                      GPIO_AF2 
 *                      GPIO_AF3
 *                      GPIO_AF4 
 *                      GPIO_AF5
 *                      GPIO_AF6 
 *                      GPIO_AF7
 *                      GPIO_AF8
 *                      GPIO_AF9
 *                      GPIO_AF10
 *                      GPIO_AF11
 *                      GPIO_AF12
 *                      GPIO_AF13
 *                      GPIO_AF14
 *                      GPIO_AF15
 */
void GPIO_PinRemapConfig(PIN_Name pin, uint32_t Alternate)
{
    GPIO_TypeDef *GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[pin];
    uint32_t position = DIGITAL_PIN_NUM[pin];
    GPIOx->AFR[position >> 3u] &= ~(0xFu << ((position & 0x07u) * 4u));
    GPIOx->AFR[position >> 3u] |= (Alternate << ((position & 0x07u) * 4u));
}

/**
 * @brief ADC初始化
 * @note 采用单次转换模式、单通道模式
 * @param adcn adc通道选择
 *              ADC_PA0,
 *	            ADC_PA1,
 *	            ADC_PA2, 
 *	            ADC_PA3,
 *	            ADC_PA4,
 *              ADC_PA5,
 *	            ADC_PA6,
 *	            ADC_PA7,
 *              ADC_PB0,
 *              ADC_PB1
 * @param sample 采样周期 
 *              ADC_SAMPLETIME_3CYCLES_5
 *              ADC_SAMPLETIME_5CYCLES_5
 *              ADC_SAMPLETIME_7CYCLES_5
 *              ADC_SAMPLETIME_13CYCLES_5
 *              ADC_SAMPLETIME_28CYCLES_5
 *              ADC_SAMPLETIME_41CYCLES_5
 *              ADC_SAMPLETIME_71CYCLES_5
 *              ADC_SAMPLETIME_239CYCLES_5
 * @param resolution 采样位数 
 *              ADC_RESOLUTION_12B
 *              ADC_RESOLUTION_10B
 *              ADC_RESOLUTION_8B
 *              ADC_RESOLUTION_6B
 */
void ADC_Init(ADC_Name adcn, uint32_t sample, uint32_t resolution)
{
    __HAL_RCC_ADC_FORCE_RESET();
    __HAL_RCC_ADC_RELEASE_RESET();
	__HAL_RCC_ADC_CLK_ENABLE();
 
    pinMode(ANALOG_PIN_NUM[adcn], GPIO_MODE_ANALOG);
    if((ADC1->CR&ADC_CR_ADEN) == 0){    //ADC没有使能才能校准
        ADC1->CR |= ADC_CR_ADCAL;   //ADC校准启动
        while((ADC1->CR&ADC_CR_ADCAL) != 0); //等待校准完成
    }
    MODIFY_REG(ADC1->CFGR1, ADC_CFGR1_RESSEL, resolution);  //设置ADC位数
    MODIFY_REG(ADC1->CFGR2, ADC_CFGR2_CKMODE, ADC_CLOCK_SYNC_PCLK_DIV1);    //ADC时钟设置为PCLK
    ADC1->CFGR1 &= ~(ADC_CFGR1_DISCEN|ADC_CFGR1_CONT|ADC_CFGR1_OVRMOD|ADC_CFGR1_EXTSEL|ADC_CFGR1_EXTEN|ADC_CFGR1_ALIGN|ADC_CFGR1_SCANDIR);
	ADC1->CFGR1 |= (0x1UL<<16)|(0x01UL<<14)|(0x0UL<<13)|(0x0UL<<12)|(0x0UL<<5)|(0x1UL<<2);
    
    ADC1->SMPR &= ~(ADC_SMPR_SMP);
    ADC1->SMPR |= (sample & ADC_SMPR_SMP); //设置采样周期

    ADC1->IER = 0;  //不使能中断
}

/**
 * @brief 读ADC值
 * @param adcn adc通道选择
 *              ADC_PA0,
 *	            ADC_PA1,
 *	            ADC_PA2, 
 *	            ADC_PA3,
 *	            ADC_PA4,
 *              ADC_PA5,
 *	            ADC_PA6,
 *	            ADC_PA7,
 *              ADC_PB0,
 *              ADC_PB1
 * @return ADC值
 */
uint16_t ADC_Read(ADC_Name adcn)
{
    if((ADC1->CR & ADC_CR_ADSTART) == RESET){     //没有正在进行的ADC转换
        ADC1->CHSELR = 0x0UL;
        ADC1->CHSELR |= ADC_CHSELR_CHANNEL(adcn);   //通道选择
        ADC1->CR  |= ADC_CR_ADEN;  //ADC使能到开启转换，时间间隔应大于8个时钟
        __asm volatile("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
        ADC1->ISR = (ADC_FLAG_EOC | ADC_FLAG_EOS | ADC_FLAG_OVR);   //清状态
        ADC1->CR |= ADC_CR_ADSTART;     //启动一次转换
        while( (ADC1->ISR & (ADC_FLAG_EOC | ADC_FLAG_EOS)) == 0);
        ADC1->ISR |= (ADC_FLAG_EOC | ADC_FLAG_EOS);   //清状态
        return ADC1->DR;
    }   
    return 0;
}

/**
 * @brief 系统初始化
 */
void TW32F0xx_init()
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    RCC->ICSCR = (RCC->ICSCR & 0xFFFF0000) |(0x4<<13) | ((*(uint32_t *)(0x1FFF0F10)) & 0x1FFF);
	while((RCC->CR&(0x01<<10))!=(0x01<<10));
    memcpy((void*)0x20000000, (void*)0x8000800,0xc0);
    SCB->VTOR = SRAM_BASE;
    __enable_irq();
	SysTick->LOAD = 23999;
	SysTick->VAL  = (0x00);
	SysTick->CTRL = 0x07;
    NVIC_SetPriority(SysTick_IRQn, 0);
    NVIC_EnableIRQ(SysTick_IRQn);
}

/**
 * @brief   引脚翻转
 * @param   pin 引脚名
 *               PA0 - PA15
 *               PB0 - PB15
 *               PF0 - PF15
 */
void digitalToggle(uint8_t pin)
{	
    GPIO_TypeDef* GPIOx = (GPIO_TypeDef*)DIGITAL_PIN_PORT[pin];
	uint16_t GPIO_Pin = DIGITAL_PIN_NUM[pin];
	pinMode(pin, GPIO_Mode_Out_PP);
	if (GPIOx->ODR & (1<<GPIO_Pin))
	{
		GPIOx->BRR |= (1<<GPIO_Pin);
	}
	else
	{
		GPIOx->BSRR |= (1<<GPIO_Pin);
	}
}

/**
 * @brief   滴答定时器中断函数
 */
void SysTick_Handler(void)
{
	_systick_count++;
}





