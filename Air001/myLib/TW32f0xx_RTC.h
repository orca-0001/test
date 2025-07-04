/**
 * @file RTC.h
 * @brief TW32F003内部RTC模块驱动库
 * @version 0.1
 * @date 2023-01-03
 * 
 * @copyright Copyright (c) 2022 杭州天问五幺科技有限公司
 * 
 */
#ifndef _RTC_H_
#define _RTC_H_

#include "TW32f0xx.h"
#include "TW32f003x8.h"

uint8_t const mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};

class INTERNAL_RTC{
public:
    INTERNAL_RTC();
    void begin();   //初始化
    void setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
    void SetAlarm(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
    void getTime();  //获取时间

    uint8_t whatWeekday(uint32_t nYear, uint8_t nMonth, uint8_t nDay);
    uint8_t isLeapYear(int year);

    /* time variables */
    uint16_t _year;
    uint8_t  _month;
    uint8_t  _day;
    uint8_t  _hour;
    uint8_t  _minute;
    uint8_t  _sec;
    uint8_t  _weekday;
    
    /* alarm */
    uint8_t alarm_hour;
    uint8_t alarm_minute;
    uint8_t alarm_weekday;
    uint8_t alarm_day;
};   

INTERNAL_RTC::INTERNAL_RTC()
{

}

/**
 * @brief RTC模块初始化
 */
void INTERNAL_RTC::begin()
{
    uint32_t prescaler = 0U;

    //HAL_RCC_OscConfig
    if( (RCC->CFGR&(0x7U<<3)) == RCC_CFGR_SWS_LSI)    //如果LSI作为系统时钟
    {
        return;
    }
    RCC->CSR |= RCC_CSR_LSION;         //LSI OSC使能
    while ((RCC->CSR&RCC_CSR_LSIRDY) == 0U);   //等待LSI稳定

    //HAL_RCCEx_PeriphCLKConfig
    uint32_t tickstart = 0U, temp_reg = 0U;
    uint8_t  pwrclkchanged = RESET;

    if((RCC->APBENR1&RCC_APBENR1_PWREN) == 0){
        __IO uint32_t tmpreg; 
        RCC->APBENR1 |= RCC_APBENR1_PWREN;
        tmpreg = (RCC->APBENR1 & RCC_APBENR1_PWREN);
        UNUSED(tmpreg);  
        pwrclkchanged = SET;   
    }

    if ((PWR->CR1&PWR_CR1_DBP) == 0)        
    {
        PWR->CR1 |= PWR_CR1_DBP;    //允许访问RTC
        while((PWR->CR1&PWR_CR1_DBP) == 0);
    }

    temp_reg = (RCC->BDCR & RCC_BDCR_RTCSEL);
    if ((temp_reg != 0x00000000U) && (temp_reg != (RCC_RTCCLKSOURCE_LSI & RCC_BDCR_RTCSEL)))
    {
        temp_reg = (RCC->BDCR & ~(RCC_BDCR_RTCSEL));
        RCC->BDCR |= RCC_BDCR_BDRST;    //RTC domain软复位
        RCC->BDCR &= ~ RCC_BDCR_BDRST;
        RCC->BDCR = temp_reg;
    }
    RCC->BDCR &= ~RCC_BDCR_RTCSEL;
    RCC->BDCR |= RCC_RTCCLKSOURCE_LSI;      //RTC时钟源设置为LSI
    if (pwrclkchanged == SET)
    {
        RCC->APBENR1 &= ~RCC_APBENR1_PWREN;
    }
    __IO uint32_t tmpreg; 
    RCC->APBENR1 |= RCC_APBENR1_RTCAPBEN;
    tmpreg = (RCC->APBENR1&RCC_APBENR1_RTCAPBEN);
    UNUSED(tmpreg);

    RCC->BDCR |= RCC_BDCR_RTCEN;    //RTC时钟使能

    RTC->CRL &= ~RTC_CRL_RSF;
    while( (RTC->CRL&RTC_CRL_RSF)== 0x0U);  //等待寄存器同步完成

    while ((RTC->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET); //等待RTC写操作完成
    RTC->CRL |= RTC_CRL_CNF;   //进入配置模式
    RTC->CRL &= ~(RTC_CRL_OWF | RTC_CRL_ALRF | RTC_CRL_SECF);
    RTC->BKP_RTCCR &= ~(BKP_RTCCR_CCO|BKP_RTCCR_ASOE|BKP_RTCCR_ASOS);
    RTC->BKP_RTCCR |= 0;
    prescaler = LSI_VALUE - 1U;
    MODIFY_REG(RTC->PRLH, RTC_PRLH_PRL, (prescaler >> 16U));
    MODIFY_REG(RTC->PRLL, RTC_PRLL_PRL, (prescaler & RTC_PRLL_PRL));
    RTC->CRL &= ~RTC_CRL_CNF;   //退出配置模式,开始更新RTC寄存器
    while ((RTC->CRL & RTC_CRL_RTOFF) == RTC_CRL_RTOFF);
    while ((RTC->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET); //等待RTC写操作完成

    // NVIC_SetPriority(RTC_IRQn, 0);
    // NVIC_EnableIRQ(RTC_IRQn);
    // RTC->CRH |= RTC_CRH_OWIE;       //允许溢出中断
    // RTC->CRH |= RTC_CRH_ALRIE;      //允许闹钟中断
    // RTC->CRH |= RTC_CRH_SECIE;      //允许秒中断
}

/**
 * @brief RTC设置时间
 * @param year     年
 * @param month    月
 * @param day      日
 * @param hour     时
 * @param minute   分
 * @param sec      秒
 */
void INTERNAL_RTC::setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    uint32_t t;
    uint32_t seccount = 0;
    if((year < 1970)||(year > 2099))return;

    /* 获取1970年1月1日0时0分0秒到现在的秒数 */
    for(t=1970;t<year;t++){
		if(isLeapYear(t))seccount += 31622400;
		else seccount+=31536000;
    }
    month -= 1;
	for(t=0;t<month;t++)
	{
		seccount+=(uint32_t)mon_table[t]*86400;
		if(isLeapYear(year)&&t==1)seccount+=86400;
	}
	seccount += (uint32_t)(day-1)*86400;
	seccount += (uint32_t)hour*3600;
    seccount += (uint32_t)minute*60;
	seccount += second;

    while ((RTC->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET); //等待RTC写操作完成
    RTC->CRL |= RTC_CRL_CNF;   //进入配置模式
    RTC->CNTH = (seccount >> 16U);
    RTC->CNTL = (seccount & RTC_CNTL_RTC_CNT);
    RTC->CRL &= ~RTC_CRL_CNF;   //退出配置模式
    while ((RTC->CRL & RTC_CRL_RTOFF) == RTC_CRL_RTOFF);
    while ((RTC->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET); //等待RTC写操作完成
}


/**
 * @brief RTC获取时间
 */
void INTERNAL_RTC::getTime()
{
	static uint16_t daycnt=0;
	uint32_t counter_time=0;
    uint16_t high1 = 0U, high2 = 0U, low = 0U;
    uint32_t temp=0,temp1 = 0;

    if((RTC->CRL&RTC_CRL_OWF)!= RESET)return;
    high1 = (RTC->CNTH & RTC_CNTH_RTC_CNT);
    low   = (RTC->CNTL & RTC_CNTH_RTC_CNT);
    high2 = (RTC->CNTH & RTC_CNTH_RTC_CNT);
    if(high1 != high2){
        counter_time = ( ((uint32_t)high2 << 16U) | (RTC->CNTL&RTC_CNTL_RTC_CNT) );
    }else{
        counter_time = ( ((uint32_t) high1 << 16U) | low );
    }
    temp = counter_time/86400;
    if(daycnt != temp)
    {
        daycnt = temp;
        temp1 = 1970;
		while(temp >= 365)
		{
			if(isLeapYear(temp1))
			{
				if(temp>=366)temp-=366;
				else {temp1++;break;}
			}
			else temp-=365;
			temp1++;
		}
        this->_year = temp1;
        temp1 = 0;
		while(temp>=28)
		{
			if(isLeapYear(this->_year)&&temp1==1)
			{
				if(temp>=29)temp-=29;
				else break;
			}
			else
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];
				else break;
			}
			temp1++;
		}
        this->_month = temp1 + 1;
        this->_day   = temp + 1;
    }
	temp           = counter_time%86400;
	this->_hour    = temp / 3600;
	this->_minute  = (temp % 3600) / 60;
	this->_sec     = (temp % 3600) % 60;
	this->_weekday = whatWeekday(this->_year, this->_month, this->_day);
}

/**
 * @brief RTC设置闹钟
 * @param hour     时
 * @param minute   分
 * @param sec      秒
 */
void INTERNAL_RTC::SetAlarm(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    uint16_t t = 0;
    uint32_t seccount = 0;
    if(year < 1970||year > 2099)return;
	for(t=1970;t<year;t++)
	{
		if(isLeapYear(t))seccount+=31622400;
		else seccount+=31536000;
	}
	month -= 1;
	for(t=0;t<month;t++)
	{
		seccount+=(uint32_t)mon_table[t]*86400;
		if(isLeapYear(year)&&t==1)seccount+=86400;
	}
	seccount += (uint32_t)(day-1)*86400;
	seccount += (uint32_t)hour*3600;
    seccount += (uint32_t)minute*60;
	seccount += second;
   /* 将闹钟时间写入寄存器 */
    while ((RTC->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET); //等待RTC写操作完成
    RTC->CRL |= RTC_CRL_CNF;   //进入配置模式
    RTC->ALRH = (seccount >> 16U);
    RTC->ALRL = (seccount & RTC_CNTL_RTC_CNT);
    RTC->CRL &= ~RTC_CRL_CNF;   //退出配置模式
    while ((RTC->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET); //等待RTC写操作完成

    NVIC_SetPriority(RTC_IRQn, 0);
    NVIC_EnableIRQ(RTC_IRQn);

    RTC->CRL = ~RTC_CRL_ALRF;   //清闹钟标志
    RTC->CRH |= RTC_CRH_ALRIE;  //闹钟中断允许
}

/**
 * @brief 通过年月日获取星期
 * @param nYear  年
 * @param nMonth 月
 * @param nDay   日
 */
uint8_t INTERNAL_RTC::whatWeekday(uint32_t nYear, uint8_t nMonth, uint8_t nDay)
{
    uint32_t weekday = 0U;
    if (nMonth < 3U)
    {
        /*D = { [(23 x month)/9] + day + 4 + year + [(year-1)/4] - [(year-1)/100] + [(year-1)/400] } mod 7*/
        weekday = (((23U * nMonth) / 9U) + nDay + 4U + nYear + ((nYear - 1U) / 4U) - ((nYear - 1U) / 100U) + ((nYear - 1U) / 400U)) % 7U;
    }
    else
    {
        /*D = { [(23 x month)/9] + day + 4 + year + [year/4] - [year/100] + [year/400] - 2 } mod 7*/
        weekday = (((23U * nMonth) / 9U) + nDay + 4U + nYear + (nYear / 4U) - (nYear / 100U) + (nYear / 400U) - 2U) % 7U;
    }
    return (uint8_t)weekday;
}

/**
 * @brief 返回该年份是否是闰年
 * @param year  年份
 */
uint8_t INTERNAL_RTC::isLeapYear(int year)
{
    if ((year % 4) != 0){
        return 0;
    }
    if ((year % 100) != 0){
        return 1;
    } 
    if ((year % 400) != 0){
        return 0;
    }else{
        return 1;
    } 
}
#ifdef SETAlarm
void alarm_callback(void);
#endif
/**
 * @brief 中断回调函数
 */
#ifdef __cplusplus
extern "C" {
#endif
void RTC_IRQHandler(void)
{
    if( (RTC->CRH& RTC_CRH_ALRIE) != RESET)
    {
        if( (RTC->CRL& RTC_CRL_ALRF) != RESET)
        {
            #ifdef SETAlarm
            alarm_callback();
            #endif
            RTC->CRL = ~RTC_CRL_ALRF;
        }
    }
}
#ifdef __cplusplus
}
#endif

INTERNAL_RTC   rtc;

#endif // !


