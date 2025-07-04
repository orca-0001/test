/**
 * @file pcf8563.h
 * @brief pcf8563时钟芯片驱动库
 * @version 0.2
 * @date 2024-6-15
 * 
 * -> 修复初始化时间清零问题
 * 
 * @copyright Copyright (c) 2022 杭州天问五幺科技有限公司
 * 
 */

#ifndef  _PCF8563_H
#define  _PCF8563_H

#define PCF8563_ADDR_R         0xa3     //读地址
#define PCF8563_ADDR_W         0xa2     //写地址

/* 寄存器地址 */
#define PCF8563_STAT1_ADDR     0x0
#define PCF8563_STAT2_ADDR     0x01
#define PCF8563_SEC_ADDR       0x02
#define PCF8563_MIN_ADDR       0x03
#define PCF8563_HR_ADDR        0x04
#define PCF8563_DAY_ADDR       0x05
#define PCF8563_WEEKDAY_ADDR   0x06
#define PCF8563_MONTH_ADDR     0x07
#define PCF8563_YEAR_ADDR      0x08
#define PCF8563_ALRM_MIN_ADDR  0x09
#define PCF8563_SQW_ADDR       0x0D
#define PCF8563_TIMER1_ADDR    0x0E
#define PCF8563_TIMER2_ADDR    0x0F

#define PCF8563_ALARM          0x80     /* 0,使能报警功能;1,不使能报警功能 */
#define PCF8563_ALARM_AIE      0x02     /* 0,报警中断无效;1,报警中断有效 */
#define PCF8563_ALARM_AF       0x08     /* 当报警发生时,AF位被置一 */

#define PCF8563_NO_ALARM       99
#define PCF8563_TIMER_TIE      0x01     /* 0,定时器中断无效;1,定时器中断有效 */
#define PCF8563_TIMER_TF       0x04     /* 定时器倒计数结束被置1 */
#define PCF8563_TIMER_TI_TP    0x10     /* 0,当TF有效时INT有效;1,INT脉冲有效 */
#define PCF8563_TIMER_TD10     0x03     /* 定时器时钟频率选择位 */
#define PCF8563_TIMER_TE       0x80     /* 0,定时器无效;1,定时器有效 */

/* 倒计数定时器时钟频率选择表 */
#define PCF8563_TMR_4096HZ      0x00
#define PCF8563_TMR_64Hz        0x01
#define PCF8563_TMR_1Hz         0x02
#define PCF8563_TMR_1MIN        0x03

#define PCF8563_CENTURY_MASK    0x80
#define PCF8563_VLSEC_MASK      0x80

/* date format flags */
#define PCF8563_DATE_WORLD      0x01
#define PCF8563_DATE_ASIA       0x02
#define PCF8563_DATE_US         0x04
#define PCF8563_DATE_CZ         0x05

/* time format flags */
#define PCF8563_TIME_HMS        0x01
#define PCF8563_TIME_HM         0x02
#define PCF8563_TIME_HMS_12     0x03
#define PCF8563_TIME_HM_12      0x04

/* 方波输出频率选择表 */
#define PCF8563_SQW_DISABLE     0x00
#define PCF8563_SQW_32KHZ       0x80
#define PCF8563_SQW_1024HZ      0x81
#define PCF8563_SQW_32HZ        0x82
#define PCF8563_SQW_1HZ         0x83

// #define  PCF8563_USE_HARDIIC      //使用硬件I2C
#if defined(PCF8563_USE_HARDIIC)
#include "TW32f0xx_HARDIIC.h"
#define  PCF8563_IICX       (hardiic)
#else
#include "TW32f0xx_SOFTIIC.h"
#define  PCF8563_IICX       (softiic)
#endif

class PCF8563 {
public:
#if defined(PCF8563_USE_HARDIIC)
    PCF8563();
#else
    PCF8563(uint8_t sdapin,uint8_t sclpin);
#endif
    void zeroClock();  /* Zero date/time, alarm / timer, default clkout */
    void clearStatus(); /* set both status bytes to zero */
    uint8_t readStatus2();
    void clearVoltLow(void); /* Only clearing is possible */

    void getDateTime();     /* get date and time vals to local vars */
    void setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t weekday, 
                            uint8_t hour,uint8_t minute, uint8_t sec);
    void getAlarm();  // same as getDateTime
    bool alarmEnabled();  // true if alarm interrupt is enabled
    bool alarmActive();   // true if alarm is active (going off)
    void enableAlarm(); /* activate alarm flag and interrupt */
    /* set alarm vals, 99=ignore */
    void setAlarm(uint8_t min, uint8_t hour, uint8_t day, uint8_t weekday);
    void clearAlarm(); /* clear alarm flag and interrupt */
    void resetAlarm();  /* clear alarm flag but leave interrupt unchanged */

    bool timerEnabled();  // true if timer and interrupt is enabled
    bool timerActive();   // true if timer is active (going off)
    void enableTimer(void); // activate timer flag and interrupt
    void setTimer(uint8_t value, uint8_t frequency, bool is_pulsed);  // set value & frequency
    void clearTimer(void); // clear timer flag, and interrupt, leave value unchanged
    void resetTimer(void); // same as clearTimer() but leave interrupt unchanged */

    void setSquareWave(uint8_t frequency);
    void clearSquareWave();

    int leapDaysBetween(uint8_t year_start, uint8_t year_end) const; //返回2个年份之间的闰日数
    bool isLeapYear(int year) const;  //如果是闰年返回True
    uint8_t daysInMonth(uint16_t year, uint8_t month) const;  //返回某年某月的天数
    uint8_t daysInYear(uint16_t year, uint8_t month, uint8_t day) const;  //返回从某一天算起的那年剩余天数
    uint8_t whatWeekday(int year, uint8_t month, uint8_t day) const; //返回某天是星期几（0-6）

    bool getVoltLow();
    uint8_t getSecond(); //获取秒
    uint8_t getMinute(); //获取分钟
    uint8_t getHour(); //获取小时
    uint8_t getDay();  //获取日期
    uint8_t getMonth(); //获取月
    uint16_t getYear(); //获取年
    bool getCentury(); //获取世纪，0代表20xx , 1代表19xx
    uint8_t getWeekday();  //获取周
    uint8_t getStatus1();
    uint8_t getStatus2();

    uint8_t getAlarmMinute();
    uint8_t getAlarmHour();
    uint8_t getAlarmDay();
    uint8_t getAlarmWeekday();

    uint8_t getTimerControl();
    uint8_t getTimerValue();

    void begin();   //初始化
    void setTime(uint8_t hour, uint8_t minute, uint8_t sec);  //设置时间
    void getTime();  //获取时间
    void setDate(uint16_t year, uint8_t month, uint8_t day, uint8_t weekday); //设置日期
    void getDate();  //获取日期

private:
    /* methods */
    uint8_t decToBcd(uint8_t value);
    uint8_t bcdToDec(uint8_t value);
    /* time variables */
    uint8_t hour;
    uint8_t hour12;
    bool am;
    uint8_t minute;
    bool volt_low; 
    uint8_t sec;
    uint8_t day;
    uint8_t weekday;
    uint8_t month;
    uint16_t year;
    /* alarm */
    uint8_t alarm_hour;
    uint8_t alarm_minute;
    uint8_t alarm_weekday;
    uint8_t alarm_day;
    /* CLKOUT */
    uint8_t squareWave;
    /* timer */
    uint8_t timer_control;
    uint8_t timer_value;
    /* support */
    uint8_t status1;
    uint8_t status2;
    bool century;

    char strOut[9];
    char strDate[11];

    uint8_t _sdapin;
    uint8_t _sclpin;
};

/**
 * @brief  PCF8563引脚脚位选择.
 * @param  sdapin     SDA引脚.
 * @param  sclpin     SCL引脚.
 * @return none.
 *          如果为使用硬件iic则引脚固定无法修改，详见hardiic.h
 */
#if defined(PCF8563_USE_HARDIIC)
PCF8563::PCF8563()
{

}
#else
PCF8563::PCF8563(uint8_t sdapin,uint8_t sclpin)
{
    _sdapin = sdapin;
    _sclpin = sclpin;
}
#endif

/**
 * @brief  返回开始年(包括)和结束年(不包括)之间的闰日(指2月29)数.
 * @param  year_start     开始年.
 * @param  year_end       结束年.
 * @return none.
 */
inline int PCF8563::leapDaysBetween(uint8_t year_start,uint8_t year_end) const 
{
    // Credit: Victor Haydin via stackoverflow.com
    int span_start = year_start;
    int span_end = year_end - 1;  // less year_end
    // Subtract leap-years before span_start, from leap-years before span_end
    return ((span_end / 4) - (span_end / 100) + (span_end / 400)) -
           ((span_start / 4) - (span_start / 100) + (span_start / 400));
}

/**
 * @brief  判断是否是闰年.
 * @param  year    年份.
 * @return true,闰年;false,平年.
 */
inline bool PCF8563::isLeapYear(int year) const
{
    if ((year % 4) != 0){
        return false;
    }else if ((year % 100) != 0){
        return true;
    }else if ((year % 400) != 0){
        return false;
    }else{
        return true;
    }
}

/**
 * @brief  返回某年某月所包含的天数.
 * @param  year    年份.
 * @param  month   月.
 * @return 天数.
 */
inline uint8_t PCF8563::daysInMonth(uint16_t year,uint8_t month) const
{
    const int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t dim = days[month - 1];
    if (month == 2 && isLeapYear(year))
        dim += 1;
    return dim;
}

/**
 * @brief  返回该年还剩余的天数.
 * @param  year    年.
 * @param  month   月.
 * @param  day     日.
 * @return 天数.
 */
inline uint8_t PCF8563::daysInYear( uint16_t year,uint8_t month,uint8_t day) const
{
    const int days[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    uint8_t total = days[month - 1] + day;
    if ((month > 2) && isLeapYear(year))
        total += 1;
    return total;
}

/**
 * @brief  返回该日是星期几.
 * @param  year    年.
 * @param  month   月.
 * @param  day     日.
 * @return 星期.
 */
inline uint8_t PCF8563::whatWeekday(int year, uint8_t month, uint8_t day) const
{
    // Credit: Tomohiko Sakamoto
    // http://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week
    year -= (month < 3);
    static int trans[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    return (year + year / 4 - year / 100 + year / 400 +
            trans[month - 1] + day) % 7;
}

/**
 * @brief  dec转bcd码.
 * @param  val    dec码.
 * @return bcd码.
 */
uint8_t PCF8563::decToBcd(uint8_t val)
{
    return ( (val/10*16) + (val%10) );
}

/**
 * @brief  bcd转dec码.
 * @param  val    bcd码.
 * @return dec码.
 */
uint8_t PCF8563::bcdToDec(uint8_t val)
{
    return ( (val/16*10) + (val%16) );
}

/**
 * @brief  寄存器值配置.
 */
void PCF8563::zeroClock()
{
    PCF8563_IICX.start(PCF8563_ADDR_W);    // Issue I2C start signal
    PCF8563_IICX.write((uint8_t)0x0);        // start address

    PCF8563_IICX.write((uint8_t)0x0);     //control/status1
    PCF8563_IICX.write((uint8_t)0x0);     //control/status2
    PCF8563_IICX.write((uint8_t)0x00);    //set seconds to 0 & VL to 0
    PCF8563_IICX.write((uint8_t)0x00);    //set minutes to 0
    PCF8563_IICX.write((uint8_t)0x00);    //set hour to 0
    PCF8563_IICX.write((uint8_t)0x01);    //set day to 1
    PCF8563_IICX.write((uint8_t)0x00);    //set weekday to 0
    PCF8563_IICX.write((uint8_t)0x81);    //set month to 1, century to 1900
    PCF8563_IICX.write((uint8_t)0x00);    //set year to 0
    PCF8563_IICX.write((uint8_t)0x80);    //minute alarm value reset to 00
    PCF8563_IICX.write((uint8_t)0x80);    //hour alarm value reset to 00
    PCF8563_IICX.write((uint8_t)0x80);    //day alarm value reset to 00
    PCF8563_IICX.write((uint8_t)0x80);    //weekday alarm value reset to 00
    PCF8563_IICX.write((uint8_t)PCF8563_SQW_32KHZ); //set SQW to default, see: setSquareWave
    PCF8563_IICX.write((uint8_t)0x0);     //timer off
    PCF8563_IICX.stop();
}

/**
 * @brief  清除状态.
 */
void PCF8563::clearStatus()
{
    PCF8563_IICX.start(PCF8563_ADDR_W);    // Issue I2C start signal
    PCF8563_IICX.write((uint8_t)0x0);
    PCF8563_IICX.write((uint8_t)0x0);                 //control/status1
    PCF8563_IICX.write((uint8_t)0x0);                 //control/status2
    PCF8563_IICX.stop();
}

/**
 * @brief  获取status2状态.
 * @return status状态
 */
uint8_t PCF8563::readStatus2()
{
    getDateTime();
    return getStatus2();
}

/**
 * @brief  清除VL状态.
 */
void PCF8563::clearVoltLow(void)
{
    getDateTime();
    // Only clearing is possible on device (I tried)
    setDateTime(getYear(), getMonth(), getDay(), getWeekday(), 
                  getHour(),getMinute(), getSecond());
}

/**
 * @brief  读取rtc数据.
 */
void PCF8563::getDateTime(void)
{
    uint8_t readBuffer[16] = {0};
    PCF8563_IICX.start(PCF8563_ADDR_W);
    PCF8563_IICX.write((uint8_t)PCF8563_STAT1_ADDR);
    PCF8563_IICX.restart(PCF8563_ADDR_R);
    for (uint8_t i=0; i < 15; i++){
        readBuffer[i] = PCF8563_IICX.read(0);
    }
    readBuffer[16] = PCF8563_IICX.read(1);
    PCF8563_IICX.stop();

    // status bytes
    status1 = readBuffer[0];
    status2 = readBuffer[1];

    // time bytes
    volt_low = readBuffer[2] & PCF8563_VLSEC_MASK;  //VL_Seconds
    sec = bcdToDec(readBuffer[2] & ~PCF8563_VLSEC_MASK);
    minute = bcdToDec(readBuffer[3] & 0x7f);
    hour = bcdToDec(readBuffer[4] & 0x3f);
    if(bcdToDec(readBuffer[4] & 0x3f) > 12)
    {
        hour12 = bcdToDec(readBuffer[4] & 0x3f) - 12;
        am = false;
    }
    else
    {
        hour12 = hour;
        am = true;
    }
    
    // date bytes
    day = bcdToDec(readBuffer[5] & 0x3f);
    weekday = bcdToDec(readBuffer[6] & 0x07);
    //get raw month data uint8_t and set month and century with it.
    month = readBuffer[7];
    if (month & PCF8563_CENTURY_MASK)
        century = true;
    else
        century = false;
    month = month & 0x1f;
    month = bcdToDec(month);
    if(century == false){
        year = bcdToDec(readBuffer[8]) + 2000;
    }else{
        year = bcdToDec(readBuffer[8]) + 1900;
    }
        
    // alarm bytes
    alarm_minute = readBuffer[9];
    if(0x80 & alarm_minute)
        alarm_minute = PCF8563_NO_ALARM;
    else
        alarm_minute = bcdToDec(alarm_minute & 0x7f);
    alarm_hour = readBuffer[10];
    if(0x80 & alarm_hour)
        alarm_hour = PCF8563_NO_ALARM;
    else
        alarm_hour = bcdToDec(alarm_hour & 0x3f);
    alarm_day = readBuffer[11];
    if(0x80 & alarm_day)
        alarm_day = PCF8563_NO_ALARM;
    else
        alarm_day = bcdToDec(alarm_day  & 0x3f);
    alarm_weekday = readBuffer[12];
    if(0x80 & alarm_weekday)
        alarm_weekday = PCF8563_NO_ALARM;
    else
        alarm_weekday = bcdToDec(alarm_weekday  & 0x07);

    // CLKOUT_control 0x03 = 0b00000011
    squareWave = readBuffer[13] & 0x03;

    // timer bytes
    timer_control = readBuffer[14] & 0x03;
    timer_value = readBuffer[15];  // current value != set value when running
}

/**
 * @brief  写rtc数据.
 * @param  year     年(2000-2099)
 * @param  month    月(1-12)
 * @param  day      日(1-31)
 * @param  weekday  星期(0-6)
 * @param  hour     时(0-23)
 * @param  minute   分(0-59)
 * @param  sec      秒(0-59)
 */
void PCF8563::setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t weekday, 
                            uint8_t hour,uint8_t minute, uint8_t sec)
{
    /* year val is 00 to 99, xx
        with the highest bit of month = century
        0=20xx
        1=19xx
    */
    month = decToBcd(month);
    if(year >= 2000){
        month &= ~PCF8563_CENTURY_MASK;
        year = year - 2000;
    }else{
        month |= PCF8563_CENTURY_MASK;
        year = year - 1900;
    }
    
    /* As per data sheet, have to set everything all in one operation */
    PCF8563_IICX.start(PCF8563_ADDR_W);    // Issue I2C start signal
    PCF8563_IICX.write(PCF8563_SEC_ADDR);       // send addr low uint8_t, req'd
    PCF8563_IICX.write(decToBcd(sec) &~PCF8563_VLSEC_MASK); //set sec, clear VL bit
    PCF8563_IICX.write(decToBcd(minute));    //set minutes
    PCF8563_IICX.write(decToBcd(hour));        //set hour
    PCF8563_IICX.write(decToBcd(day));            //set day
    PCF8563_IICX.write(decToBcd(weekday));    //set weekday
    PCF8563_IICX.write(month);                 //set month, century to 1
    PCF8563_IICX.write(decToBcd(year));        //set year to 99
    PCF8563_IICX.stop();
    // Keep values in-sync with device
    getDateTime();
}

/**
 * @brief  获取alarm.
 */
void PCF8563::getAlarm()
{
    getDateTime();
}

/**
 * @brief  获取报警中断是否使能.
 * @return true,使能;false,没有使能.
 */
bool PCF8563::alarmEnabled()
{
    return getStatus2() & PCF8563_ALARM_AIE;
}

/**
 * @brief  获取报警是否发生.
 * @return true,报警发生;false,报警没有发生.
 */
bool PCF8563::alarmActive()
{
    return getStatus2() & PCF8563_ALARM_AF;
}

/**
 * @brief  使能报警中断.
 */
void PCF8563::enableAlarm()
{
    getDateTime();  // operate on current values
    //set status2 AF val to zero
    status2 &= ~PCF8563_ALARM_AF;
    //set TF to 1 masks it from changing, as per data-sheet
    status2 |= PCF8563_TIMER_TF;
    //enable the interrupt
    status2 |= PCF8563_ALARM_AIE;

    //enable the interrupt
    PCF8563_IICX.start(PCF8563_ADDR_W);
    PCF8563_IICX.write((uint8_t)PCF8563_STAT2_ADDR);
    PCF8563_IICX.write((uint8_t)status2);
    PCF8563_IICX.stop();
}

/**
 * @brief 设置闹钟报警值(将其设为99则被忽略).
 * @param min     分   
 * @param hour    时
 * @param day     日
 * @param weekday 周
 */
void PCF8563::setAlarm(uint8_t min, uint8_t hour, uint8_t day, uint8_t weekday)
{
    getDateTime();  // operate on current values
    if ((min>=0)&&(min < 60)){
        min = decToBcd(min);
        min &= ~PCF8563_ALARM;
    } else {
        min = 0x0; min |= PCF8563_ALARM;
    }

    if ((hour>=0)&&(hour < 24)) {
        hour = decToBcd(hour);
        hour &= ~PCF8563_ALARM;
    } else {
        hour = 0x0; 
        hour |= PCF8563_ALARM;
    }

    if ((day>=1)&&(day < 32)) {
        day = decToBcd(day); 
        day &= ~PCF8563_ALARM;
    } else {
        day = 0x0; 
        day |= PCF8563_ALARM;
    }

    if ((weekday>=0)&&(weekday<6)) {
        weekday = decToBcd(weekday);
        weekday &= ~PCF8563_ALARM;
    } else {
        weekday = 0x0; 
        weekday |= PCF8563_ALARM;
    }

    alarm_hour = hour;
    alarm_minute = min;
    alarm_weekday = weekday;
    alarm_day = day;

    // First set alarm values, then enable
    PCF8563_IICX.start(PCF8563_ADDR_W);
    PCF8563_IICX.write((uint8_t)PCF8563_ALRM_MIN_ADDR);
    PCF8563_IICX.write((uint8_t)alarm_minute);
    PCF8563_IICX.write((uint8_t)alarm_hour);
    PCF8563_IICX.write((uint8_t)alarm_day);
    PCF8563_IICX.write((uint8_t)alarm_weekday);
    PCF8563_IICX.stop();

    PCF8563::enableAlarm();
}

/**
 * @brief 清除闹钟.
 */
void PCF8563::clearAlarm()
{
    //set status2 AF val to zero to reset alarm
    status2 &= ~PCF8563_ALARM_AF;
    //set TF to 1 masks it from changing, as per data-sheet
    status2 |= PCF8563_TIMER_TF;
    //turn off the interrupt
    status2 &= ~PCF8563_ALARM_AIE;

    PCF8563_IICX.start(PCF8563_ADDR_W);
    PCF8563_IICX.write((uint8_t)PCF8563_STAT2_ADDR);
    PCF8563_IICX.write((uint8_t)status2);
    PCF8563_IICX.stop();
}

/**
 * @brief 复位闹钟.
 */
void PCF8563::resetAlarm()
{
    //set status2 AF val to zero to reset alarm
    status2 &= ~PCF8563_ALARM_AF;
    //set TF to 1 masks it from changing, as per data-sheet
    status2 |= PCF8563_TIMER_TF;

    PCF8563_IICX.start(PCF8563_ADDR_W);
    PCF8563_IICX.write((uint8_t)PCF8563_STAT2_ADDR);
    PCF8563_IICX.write((uint8_t)status2);
    PCF8563_IICX.stop();
}

/**
 * @brief 获取定时器中断是否使能.
 * @return true,使能; false,没有使能.
 */
bool PCF8563::timerEnabled()
{
    if (getStatus2() & PCF8563_TIMER_TIE){
        if (timer_control & PCF8563_TIMER_TE){
            return true;
        }  
    }
    return false;
}

/**
 * @brief  获取定时器是否动作.
 * @return true,动作; false,没有动作.
 */
bool PCF8563::timerActive()
{
    return getStatus2() & PCF8563_TIMER_TF;
}

/**
 * @brief  使能定时器中断.
 */
void PCF8563::enableTimer(void)
{
    getDateTime();
    //set TE to 1
    timer_control |= PCF8563_TIMER_TE;
    //set status2 TF val to zero
    status2 &= ~PCF8563_TIMER_TF;
    //set AF to 1 masks it from changing, as per data-sheet
    status2 |= PCF8563_ALARM_AF;
    //enable the interrupt
    status2 |= PCF8563_TIMER_TIE;

    // Enable interrupt first, then enable timer
    PCF8563_IICX.start(PCF8563_ADDR_W);
    PCF8563_IICX.write((uint8_t)PCF8563_STAT2_ADDR);
    PCF8563_IICX.write((uint8_t)status2);
    PCF8563_IICX.stop();

    PCF8563_IICX.start(PCF8563_ADDR_W);
    PCF8563_IICX.write((uint8_t)PCF8563_TIMER1_ADDR);
    PCF8563_IICX.write((uint8_t)timer_control);  // Timer starts ticking now!
    PCF8563_IICX.stop();
}

/**
 * @brief  设置倒计数定时器.
 * @param  value   定时器倒计数数值(0-255)
 * @param  frequency    定时器时钟频率
 *             00B: 4096Hz
 *             01B: 64hz
 *             10B: 1Hz
 *             11B: 1/60Hz
 * @param  is_pulsed
 *              false:当TF有效时INT有效
 *              true :INT脉冲有效
 */
void PCF8563::setTimer(uint8_t value, uint8_t frequency, bool is_pulsed)
{
    getDateTime();
    if (is_pulsed){
        status2 |= is_pulsed << 4;
    }else{
        status2 &= ~(is_pulsed << 4);
    } 
    timer_value = value;
    // TE set to 1 in enableTimer(), leave 0 for now
    timer_control |= (frequency & PCF8563_TIMER_TD10); // use only last 2 bits

    PCF8563_IICX.start(PCF8563_ADDR_W);
    PCF8563_IICX.write((uint8_t)PCF8563_TIMER1_ADDR);
    PCF8563_IICX.write((uint8_t)timer_control);
    PCF8563_IICX.write((uint8_t)timer_value);
    PCF8563_IICX.stop();

    PCF8563_IICX.start(PCF8563_ADDR_W);
    PCF8563_IICX.write((uint8_t)PCF8563_STAT2_ADDR);
    PCF8563_IICX.write((uint8_t)status2);
    PCF8563_IICX.stop();

    enableTimer();
}

/**
 * @brief  清除倒计数定时器标志位和中断.
 */
void PCF8563::clearTimer(void)
{
    getDateTime();
    //set status2 TF val to zero
    status2 &= ~PCF8563_TIMER_TF;
    //set AF to 1 masks it from changing, as per data-sheet
    status2 |= PCF8563_ALARM_AF;
    //turn off the interrupt
    status2 &= ~PCF8563_TIMER_TIE;
    //turn off the timer
    timer_control = 0;

    // Stop timer first
    PCF8563_IICX.start(PCF8563_ADDR_W);
    PCF8563_IICX.write((uint8_t)PCF8563_TIMER1_ADDR);
    PCF8563_IICX.write((uint8_t)timer_control);
    PCF8563_IICX.stop();

    // clear flag and interrupt
    PCF8563_IICX.start(PCF8563_ADDR_W);
    PCF8563_IICX.write((uint8_t)PCF8563_STAT2_ADDR);
    PCF8563_IICX.write((uint8_t)status2);
    PCF8563_IICX.stop();
}

/**
 * @brief  复位倒计数定时器标志位和中断.
 */
void PCF8563::resetTimer(void)
{
    getDateTime();
    //set status2 TF val to zero to reset timer
    status2 &= ~PCF8563_TIMER_TF;
    //set AF to 1 masks it from changing, as per data-sheet
    status2 |= PCF8563_ALARM_AF;

    PCF8563_IICX.start(PCF8563_ADDR_W);
    PCF8563_IICX.write((uint8_t)PCF8563_STAT2_ADDR);
    PCF8563_IICX.write((uint8_t)status2);
    PCF8563_IICX.stop();
}

/**
 * @brief  设置方波引脚输出.
 * @param  frequency  
 *              00B:32.768kHz
 *              01B:1024Hz
 *              10B:32Hz
 *              11B:1Hz 
 */
void PCF8563::setSquareWave(uint8_t frequency)
{
    PCF8563_IICX.start(PCF8563_ADDR_W); 
    PCF8563_IICX.write((uint8_t)PCF8563_SQW_ADDR);
    PCF8563_IICX.write((uint8_t)frequency);
    PCF8563_IICX.stop();
}

/**
 * @brief  清除方波引脚输出.
 */
void PCF8563::clearSquareWave()
{
    PCF8563::setSquareWave(PCF8563_SQW_DISABLE);
}

/**
 * @brief  初始化.
 */
void PCF8563::begin()
{
#if defined(PCF8563_USE_HARDIIC)
    PCF8563_IICX.begin(80000);
#else
    PCF8563_IICX.begin(_sdapin, _sclpin);
#endif
    PCF8563_IICX.start(PCF8563_ADDR_W);  
    PCF8563_IICX.write((uint8_t)0x0);      //写0、1寄存器
    PCF8563_IICX.write((uint8_t)0x0);     //正常模式 RTC 源时钟运行
    PCF8563_IICX.write((uint8_t)0x0);    //禁用报警中断、定时器中断
    PCF8563_IICX.stop();

    PCF8563_IICX.start(PCF8563_ADDR_W);  
    PCF8563_IICX.write((uint8_t)0x09);      //写0X09\A\B\C寄存器
    PCF8563_IICX.write((uint8_t)0x80);    //分报警已禁用
    PCF8563_IICX.write((uint8_t)0x80);    //小时报警已禁用
    PCF8563_IICX.write((uint8_t)0x80);    //天数报警已禁用
    PCF8563_IICX.write((uint8_t)0x80);    //周天数报警已禁用
    PCF8563_IICX.write((uint8_t)0x0);     //CLKOUT 输出设置为高阻抗
    PCF8563_IICX.write((uint8_t)0x0);     //定时器已被禁用
    PCF8563_IICX.stop();
}

/**
 * @brief  设置时分秒时间.
 * @param  hour    时
 * @param  minute  分
 * @param  sec     秒
 */
void PCF8563::setTime(uint8_t hour, uint8_t minute, uint8_t sec)
{
    getDateTime();
    setDateTime(getYear(), getMonth(), getDay(), getWeekday(), hour, minute, sec);
}

/**
 * @brief  设置日期.
 * @param  year    年
 * @param  month   月
 * @param  day     日
 * @param  weekday     周
 */
void PCF8563::setDate(uint16_t year, uint8_t month, uint8_t day, uint8_t weekday)
{
    getDateTime();
    setDateTime(year, month, day, weekday, getHour(), getMinute(), getSecond());
}

/**
 * @brief  获取日期.
 */
void PCF8563::getDate()
{
    getDateTime();
}

/**
 * @brief  获取时间.
 */
void PCF8563::getTime()
{
    getDateTime();
}

/**
 * @brief  获取VL(掉电检测，当VDD低于Vlow，VL被置"1").
 * @return true,可能产生不准确的时钟/日历信息
 *         false,保证准确的时钟/日历数据
 */
bool PCF8563::getVoltLow(void)
{
    return volt_low;
}

/**
 * @brief  获取秒.
 */
uint8_t PCF8563::getSecond() {
    return sec;
}

/**
 * @brief  获取分.
 */
uint8_t PCF8563::getMinute() {
    return minute;
}

/**
 * @brief  获取时.
 */
uint8_t PCF8563::getHour() {
    return hour;
}

/**
 * @brief  获取闹钟的分.
 */
uint8_t PCF8563::getAlarmMinute() {
    return alarm_minute;
}

/**
 * @brief  获取闹钟的时.
 */
uint8_t PCF8563::getAlarmHour() {
    return alarm_hour;
}

/**
 * @brief  获取闹钟的日.
 */
uint8_t PCF8563::getAlarmDay() {
    return alarm_day;
}

/**
 * @brief  获取闹钟的周.
 */
uint8_t PCF8563::getAlarmWeekday() {
    return alarm_weekday;
}

/**
 * @brief  获取定时器时钟频率.
 */
uint8_t PCF8563::getTimerControl() {
    return timer_control;
}

/**
 * @brief  获取时间.
 */
uint8_t PCF8563::getTimerValue() {
    // Impossible to freeze this value, it could
    // be changing during read.  Multiple reads
    // required to check for consistency.
    uint8_t last_value;
    do {
        last_value = timer_value;
        getDateTime();
    } while (timer_value != last_value);
    return timer_value;
}

/**
 * @brief  获取日.
 */
uint8_t PCF8563::getDay() {
    return day;
}

/**
 * @brief  获取月.
 */
uint8_t PCF8563::getMonth() {
    return month;
}

/**
 * @brief  获取年.
 */
uint16_t PCF8563::getYear() {
    return year;
}

/**
 * @brief  获取世纪.
 */
bool PCF8563::getCentury() {
    return century;
}

/**
 * @brief  获取周.
 */
uint8_t PCF8563::getWeekday() {
    return weekday;
}

/**
 * @brief  获取状态寄存器1.
 */
uint8_t PCF8563::getStatus1() {
    return status1;
}

/**
 * @brief  获取状态寄存器2.
 */
uint8_t PCF8563::getStatus2() {
    return status2;
}


#endif //

