/**
 * @file IRSendRev.h
 * @brief TW32F003的NEC红外收发库.
 * @version 0.1
 * @author hhdd
 * @copyright Copyright (c) 2021 TWen Technology Co., Ltd.
 * 
 */
#ifndef _IRSENDREV_H_
#define _IRSENDREV_H_

#include "TW32f0xx.h"
#include "wiring.h"
#include "TW32f0xx_PWM.h"

/******************** 红外发射采样时间宏定义, 用户不要随意修改  *******************/
#define     NEC_HDR_MARK	9000     //引导码高电平
#define     NEC_HDR_SPACE	4500     //引导码低电平
#define     NEC_BIT_MARK	560
#define     NEC_ONE_SPACE	1690
#define     NEC_ZERO_SPACE	560

/*******************************  红外接收宏定义  *********************************/
#define     RAWBUF          68
#define     D_LEN           0
#define     D_STARTH        1
#define     D_STARTL        2
#define     D_SHORT         3
#define     D_LONG          4
#define     D_DATALEN       5
#define     D_DATA          6

//红外接收状态
#define     STATE_IDLE      2
#define     STATE_MARK      3
#define     STATE_SPACE     4
#define     STATE_STOP      5
/******************** 红外接收采样时间宏定义, 用户不要随意修改  *******************/
#define     USECPERTICK     50
#define     _GAP            6000    // 最小发送间隔
#define     GAP_TICKS       (_GAP/USECPERTICK)

typedef struct {
    uint8_t recvpin;              //红外接收引脚
    uint8_t rcvstate;             //状态
    uint32_t timer;           // state timer, counts 50uS ticks.
    unsigned int rawbuf[RAWBUF];  //红外接收原始数据
    uint8_t rawlen;               //原始数据长度
} irparams_t;

class decode_results
{
public:
    volatile unsigned int *rawbuf; // Raw intervals in .5 us ticks
    uint8_t rawlen;           // Number of records in rawbuf.
};

class IRSendRev
{
//红外接收
private:
    volatile irparams_t irparams;
    decode_results results;
    uint8_t decode(decode_results *results);
public:
    void begin(uint8_t pin);    //红外接收初始化
    uint8_t recv_addr(void);    //红外接收地址
    uint8_t recv(void);     //红外接收数据
    uint8_t recv(uint8_t *revData); 
    uint8_t available();    //红外接收是否接收到数据
    void recv_callback();   //红外接收回调函数，50uS调用一次

//红外发送
private: 
    PWM_CHANNEL_Name ir_send_channel;
    uint8_t ir_send_pin;
    void mark(uint32_t usec);
    void space(uint32_t usec);
	void enableIROut(uint32_t hz);
public:
    void begin(PWM_CHANNEL_Name channel, uint8_t pin);  //红外发送初始化
    void sendRaw(uint32_t buf[], uint32_t len, uint32_t hz);  
    void send_nec(uint8_t address, uint8_t command);    //红外发送NEC码
};

/*************************************************   红外发射  *********************************************************/
void IRSendRev::begin(PWM_CHANNEL_Name channel, uint8_t pin)
{
    ir_send_channel = channel;
    ir_send_pin = pin;
}

/************************************************************************
 * @brief  红外发射使能.
 * @param  hz   PWM频率.
 * @return none
 **********************************************************************/
void IRSendRev::enableIROut(uint32_t hz) 
{
   PWM_Init(ir_send_channel, (PIN_Name)ir_send_pin, hz, 0); 
}

/************************************************************************
 * @brief  红外发射载波信号.
 * @param  time   时间.
 * @return none
 **********************************************************************/
void IRSendRev::mark(uint32_t time) 
{
    PWM_Duty_Updata(ir_send_channel, PWM_DUTY_MAX/3);
    delayMicroseconds(time);
}

/************************************************************************
 * @brief  红外发射空闲信号.
 * @param  time   时间.
 * @return none
 **********************************************************************/
void IRSendRev::space(uint32_t time) 
{
    PWM_Duty_Updata(ir_send_channel, 0);
    delayMicroseconds(time);
}

/************************************************************************
 * @brief  红外发射.
 * @param  buf   红外发送数组(buf[0]为发送空闲信号时间，buf[1]为发射载波信号时间，依次类推).
 * @param  len   发送的长度
 * @param  hz    载波频率
 * @return none
 **********************************************************************/
void IRSendRev::sendRaw(uint32_t buf[], uint32_t len, uint32_t hz)
{
    enableIROut(hz);
    for(uint32_t i = 0; i < len; i++)
    {
        if(i & 1){
            space(buf[i]);
        }else{
            mark(buf[i]);
        }
    }
    space(0);
}

/************************************************************************
 * @brief  红外发射NEC码.
 * @param  address   发送的地址
 * @param  command   命令
 * @return none
 **********************************************************************/
void IRSendRev::send_nec(uint8_t address, uint8_t command)
{
    uint8_t i;
    uint8_t inverse_address = ~address;
    uint8_t inverse_command = ~command;

    // 1. 载波配置
    enableIROut(38000);
    // 2. 发送引导码
    mark(NEC_HDR_MARK);
    space(NEC_HDR_SPACE);
    // 3. 发送地址
    for (i=0;i<8;i++)
    {
        if (address & 0x01) 
        {
            mark(NEC_BIT_MARK);
            space(NEC_ONE_SPACE);
        } 
        else 
        {
            mark(NEC_BIT_MARK);
            space(NEC_ZERO_SPACE);
        }
        address >>= 1;
    }
    // 4. 发送地址反码
    for (i=0;i<8;i++)
    {
        if (inverse_address & 0x01) 
        {
            mark(NEC_BIT_MARK);
            space(NEC_ONE_SPACE);
        } 
        else 
        {
            mark(NEC_BIT_MARK);
            space(NEC_ZERO_SPACE);
        }
        inverse_address >>= 1;
    }
    // 5. 发送命令
    for (i=0;i<8;i++)
    {
        if (command & 0x01) 
        {
            mark(NEC_BIT_MARK);
            space(NEC_ONE_SPACE);
        } 
        else 
        {
            mark(NEC_BIT_MARK);
            space(NEC_ZERO_SPACE);
        }
        command >>= 1;
    }
    // 6. 发送命令反码
    for (i=0;i<8;i++)
    {
        if ((inverse_command) & 0x01) 
        {
            mark(NEC_BIT_MARK);
            space(NEC_ONE_SPACE);
        } 
        else 
        {
            mark(NEC_BIT_MARK);
            space(NEC_ZERO_SPACE);
        }
        inverse_command >>= 1;
    }
    mark(NEC_BIT_MARK);
    space(1);
}

/*************************************************   红外接收  *********************************************************/
/************************************************************************
 * @brief  红外接收初始化.
 * @param  pin:引脚.
 * @return none
 **********************************************************************/
void IRSendRev::begin(uint8_t pin)
{
    irparams.recvpin = pin;
    irparams.rcvstate = STATE_IDLE;
    irparams.rawlen = 0;  
    pinMode((PIN_Name)pin, GPIO_Mode_IN_FLOATING);  
}

/************************************************************************
 * @brief  红外接收数据解码.
 * @param  results  接收到的红外码数据指针.
 * @return 0,失败; 1,成功
 **********************************************************************/
uint8_t IRSendRev::decode(decode_results *results) {
    results->rawbuf = irparams.rawbuf;
    results->rawlen = irparams.rawlen;
    if (irparams.rcvstate != STATE_STOP) {
        return 0;
    }
    irparams.rcvstate = STATE_IDLE;
    irparams.rawlen = 0;
    return 1;
}

/************************************************************************
 * @brief  红外接收获取地址.
 * @param  none.
 * @return 红外接收到的地址
 **********************************************************************/
uint8_t IRSendRev::recv_addr(void)
{
    uint8_t revData[10];
    recv(revData);
    return revData[6];
}

/************************************************************************
 * @brief  红外接收获取数据.
 * @param  none.
 * @return 红外接收到的数据
 **********************************************************************/
uint8_t IRSendRev::recv(void)
{
    uint8_t revData[10];
    recv(revData);
    return revData[8];
}

/************************************************************************
 * @brief  红外接收处理函数（内部处理函数）.
 * @param  revData   返回红外接收到的所有数据指针.
 * @return none
 **********************************************************************/
uint8_t IRSendRev::recv(uint8_t *revData)
{
    uint32_t count       = results.rawlen;
    uint32_t nshort      = 0;
    uint32_t nlong       = 0;
    uint32_t count_data  = 0;

    //全部数据包括帧间隔一位，起始脉冲2位，结束脉冲1位，2个电平组成一位数据
    count_data = (count-4)/16;      //获取数据字节数

    for(uint32_t i = 0; i < 10; i++)
    {
        nshort += results.rawbuf[3+2*i];
    }
    nshort /= 10;       //获取低电平时间平均值

    uint32_t i = 0;
    uint32_t j = 0;
    while(1)            
    {
        if((uint32_t)results.rawbuf[4+2*i] > (1.5*nshort))
        {
            nlong += results.rawbuf[4+2*i];
            j++;
        }
        i++;
        if(j==10)break;
        if((4+2*i)>(count-10))break;
    }
    nlong /= j;

    for(i = 0; i<count_data; i++)
    {
        revData[i+D_DATA] = 0x00;
        for(j = 0; j<8; j++)
        {
            //通过高电平时间判断数据是0还是1
            if((uint32_t)results.rawbuf[4 + 16*i + j*2] > (uint32_t)(1.5*nshort))
            {
                revData[i+D_DATA] |= (0x01<< j);
            }
            else
            {
                revData[i+D_DATA] &= ~(0x01<<j);
            }
        }
    }
    revData[D_LEN]      = count_data+5;
    revData[D_STARTH]   = results.rawbuf[1];
    revData[D_STARTL]   = results.rawbuf[2];
    revData[D_SHORT]    = nshort;
    revData[D_LONG]     = nlong;
    revData[D_DATALEN]  = count_data;

    irparams.rcvstate = STATE_IDLE; //准备接收下一个数据
    irparams.rawlen = 0;

    return revData[D_LEN]+1;
}

/************************************************************************
 * @brief  红外是否接收到数据.
 * @param  none.
 * @return  0,没有接收到数据;其他值:数据长度.
 ************************************************************************/
uint8_t IRSendRev::available()
{
    if(decode(&results))
    {
        uint32_t count  = results.rawlen;
        if(count < 20 || (count -4)%8 != 0)
        {
            irparams.rcvstate = STATE_IDLE; //准备接收下一个数据
            irparams.rawlen = 0;
            return 0;
        }
        return (uint8_t)((count-4)/16+6);
    }
    else 
    {
        return 0;
    }
}

/************************************************************************
 * @brief  红外接收中断回调函数(50us回调一次).
 * @param  none.
 * @return none.
 ************************************************************************/
void IRSendRev::recv_callback()
{
    uint8_t irdata;
    irdata = digitalRead((PIN_Name)irparams.recvpin);
    irparams.timer++;
    if (irparams.rawlen > RAWBUF) {    //如果数据长度溢出
        irparams.rcvstate = STATE_STOP;
        irparams.timer = 0;
    }
    switch(irparams.rcvstate) {
        case STATE_IDLE: //空闲态
            if (irdata == LOW) {        
                if (irparams.timer < GAP_TICKS) {   //接收到起始脉冲时间小于最小帧间隔
                    irparams.timer = 0;
                }else{                              //接收到起始脉冲
                    irparams.rawlen = 0;
                    irparams.rawbuf[irparams.rawlen++] = irparams.timer; //帧间隔时间
                    irparams.timer = 0;
                    irparams.rcvstate = STATE_MARK;
                }
            }
        break;

        case STATE_MARK: // timing MARK
            if(irdata == HIGH) 
            {
                irparams.rawbuf[irparams.rawlen++] = irparams.timer;
                irparams.timer = 0;
                irparams.rcvstate = STATE_SPACE;
            }
        break;

        case STATE_SPACE: // timing SPACE
            if (irdata == LOW) { // SPACE just ended, record it
                irparams.rawbuf[irparams.rawlen++] = irparams.timer;
                irparams.timer = 0;
                irparams.rcvstate = STATE_MARK;
            }else { // SPACE
                if (irparams.timer > GAP_TICKS) {
                    irparams.rcvstate = STATE_STOP;
                } 
            }
        break;

        case STATE_STOP: // waiting, measuring gap
            if (irdata == LOW) { // reset gap timer
                irparams.timer = 0;
            }
        break;
    }
}


#endif
