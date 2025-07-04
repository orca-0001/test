#ifndef _MB_Slave_H
#define _MB_Slave_H

#include "HardwareSerial.h"
#include "CRC_16.h"

#ifndef S_COIL_NCOILS
#define S_COIL_NCOILS   40 //线圈数量
#endif

#ifndef S_DISCRETE_INPUT_NDISCRETES
#define S_DISCRETE_INPUT_NDISCRETES   16 //离散量输入数量
#endif

#ifndef S_REG_INPUT_NREGS
#define S_REG_INPUT_NREGS 16 //输入寄存器数量
#endif

#ifndef S_REG_HOLDING_NREGS
#define S_REG_HOLDING_NREGS 16 //保持寄存器数量
#endif

#ifndef  S_COIL_START
#define  S_COIL_START 0  //线圈初始地址
#endif 

#ifndef S_DISCRETE_INPUT_START
#define S_DISCRETE_INPUT_START 0 //离散量初始地址
#endif 


#ifndef  S_REG_INPUT_START
#define  S_REG_INPUT_START 0  //输入寄存器初始地址
#endif 

#ifndef  S_REG_HOLDING_START
#define  S_REG_HOLDING_START 0   //保持寄存器初始地址
#endif 

#ifndef USART_PARITY_NONE
#define USART_PARITY_NONE            0x00000000U
#endif
#ifndef USART_PARITY_EVEN
#define USART_PARITY_EVEN            ((uint32_t)USART_CR1_PCE | USART_CR1_M)
#endif
#ifndef USART_PARITY_ODD
#define USART_PARITY_ODD             ((uint32_t)(USART_CR1_PCE | USART_CR1_PS | USART_CR1_M))
#endif

class MBSlave{
public:
    MBSlave(uint16_t id,HardwareSerial* Serialx,uint32_t baud, uint16_t eParity);
    MBSlave(uint16_t id,HardwareSerial* Serialx,uint32_t baud, uint16_t eParity,int8_t rs485_pin,uint8_t level);
    void Init(); //modbus从机初始化
    void Poll(); //modbus从机轮询函数
    int8_t GetCoil(uint16_t addr); //获取线圈值
    int8_t SetCoil(uint16_t addr,uint8_t vol); //设置线圈值
    int8_t GetDisc(uint16_t addr); //获取离散量输入值
    int8_t SetDisc(uint16_t addr,uint8_t vol); //设置离散量输入值
    uint16_t GetInputReg(uint16_t addr); //获取输入寄存器值
    int8_t SetInputReg(uint16_t addr,uint16_t vol); //设置输入寄存器值
    uint16_t GetHoldReg(uint16_t addr); //获取保持寄存器值
    int8_t SetHoldReg(uint16_t addr,uint16_t vol); //设置保持寄存器值

    uint16_t usSRegInBuf[S_REG_INPUT_NREGS]; //输入寄存器缓存
    uint16_t usSRegHoldBuf[S_REG_HOLDING_NREGS];   //保持寄存器缓存
    uint8_t ucSCoilBuf[(S_COIL_NCOILS % 8) ? ((S_COIL_NCOILS / 8) + 1) : (S_COIL_NCOILS / 8)];//线圈缓存
    uint8_t ucSDiscInBuf[(S_DISCRETE_INPUT_NDISCRETES % 8) ? ((S_DISCRETE_INPUT_NDISCRETES / 8) + 1) : (S_DISCRETE_INPUT_NDISCRETES / 8)];//从机离散量

private:
    uint8_t ModBus_data[32];      //接收数组
    uint8_t ModBus_Send_data[32]; //发送数组
    uint16_t mb_s_send_len;           //发送长度
    uint8_t mb_s_rec_len = 0;         //接收长度

    USART_TypeDef* _USARTx;
    HardwareSerial* _Serialx;
    int8_t _rs485_pin = -1;
    uint8_t _level;
    uint32_t _baud;
    uint16_t _eParity;
    uint16_t _id; //从站ID

    void SerialPutByte(uint8_t ucByte); //串口发送字节
    void Send(uint8_t *p, uint16_t len);
    void func_01();
    void func_02();
    void func_03();
    void func_04();
    void func_05();
    void func_06();
    void func_15();
    void func_16();
};

MBSlave::MBSlave(uint16_t id,HardwareSerial* Serialx,uint32_t baud, uint16_t eParity)
{
    _id = id;
    _Serialx = Serialx;
    if(_Serialx==&Serial1){
        _USARTx = USART1;
    }else if(_Serialx==&Serial2){
        _USARTx = USART2;
    }
    _baud = baud;
    _eParity = eParity;
}

MBSlave::MBSlave(uint16_t id,HardwareSerial* Serialx,uint32_t baud, uint16_t eParity,int8_t rs485_pin,uint8_t level)
{
    _id = id;
    _Serialx = Serialx;
    if(_Serialx==&Serial1){
        _USARTx = USART1;
    }else if(_Serialx==&Serial2){
        _USARTx = USART2;
    }
    _baud = baud;
    _eParity = eParity;
    _rs485_pin = rs485_pin;
    _level = level;
}

/**
 * @brief  获取当前线圈值.
 * @param  addr     地址.
 * @return 线圈值（不在范围内返回-1）..
 */
int8_t MBSlave::GetCoil(uint16_t addr)
{
    if ((addr < S_COIL_START) || (addr > (S_COIL_START + S_COIL_NCOILS)))
    {
        return -1;
    }

    if (ucSCoilBuf[(addr - S_COIL_START) / 8] & (1 << ((addr - S_COIL_START) % 8)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  设置线圈值.
 * @param  addr     地址.
 * @param  vol      值（0/1）.
 * @return 成功返回1.
 */
int8_t MBSlave::SetCoil(uint16_t addr,uint8_t vol)
{
    if ((addr < S_COIL_START) || (addr > (S_COIL_START + S_COIL_NCOILS)))
    {
        return -1;
    }
    if(vol)
    {
        ucSCoilBuf[(addr - S_COIL_START) / 8]|=(1 << ((addr - S_COIL_START) % 8));

    }else{
        ucSCoilBuf[(addr - S_COIL_START) / 8]&=~(1 << ((addr - S_COIL_START) % 8));

    }
    return 1;
}

/**
 * @brief  获取离散量输入的值.
 * @param  addr     地址.
 * @return 成功返回1.
 */
int8_t MBSlave::GetDisc(uint16_t addr)
{
    if ((addr < S_DISCRETE_INPUT_START) || (addr > (S_DISCRETE_INPUT_START + S_DISCRETE_INPUT_NDISCRETES)))
    {
        return -1;
    }

    if (ucSDiscInBuf[(addr - S_DISCRETE_INPUT_START) / 8] & (1 << ((addr - S_DISCRETE_INPUT_START) % 8)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  设置离散量输入的值.
 * @param  addr     地址.
 * @param  vol      值（0/1）
 * @return 成功返回1.
 */
int8_t MBSlave::SetDisc(uint16_t addr,uint8_t vol)
{
    if ((addr < S_DISCRETE_INPUT_START) || (addr > (S_DISCRETE_INPUT_START + S_DISCRETE_INPUT_NDISCRETES)))
    {
        return -1;
    }
    if(vol)
    {
        ucSDiscInBuf[(addr - S_DISCRETE_INPUT_START)/8]|=(1 << ((addr - S_DISCRETE_INPUT_START) % 8));

    }else{
        ucSDiscInBuf[(addr - S_DISCRETE_INPUT_START) / 8]&=~(1 << ((addr - S_DISCRETE_INPUT_START) % 8));

    }
    return 1;
}

/**
 * @brief  获取输入寄存器的值.
 * @param  addr     地址.
 * @return 成功返回1.
 */
uint16_t MBSlave::GetInputReg(uint16_t addr)
{
    if ((addr < S_REG_INPUT_START) || (addr > (S_REG_INPUT_START + S_REG_INPUT_NREGS)))
    {
        return 0;
    }
    return usSRegInBuf[addr - S_REG_INPUT_START];
}

/**
 * @brief  设置输入寄存器的值.
 * @param  addr     地址.
 * @param  vol      值（0/1）
 * @return 成功返回1.
 */
int8_t MBSlave::SetInputReg(uint16_t addr,uint16_t vol)
{
    if ((addr < S_REG_INPUT_START) || (addr > (S_REG_INPUT_START + S_REG_INPUT_NREGS)))
    {
        return -1;
    }
    usSRegInBuf[addr - S_REG_INPUT_START] = vol;
    return 1;
}

/**
 * @brief  获取保持寄存器的值.
 * @param  addr     地址.
 * @return 值.
 */
uint16_t MBSlave::GetHoldReg(uint16_t addr)
{
    if ((addr < S_REG_HOLDING_START) || (addr > (S_REG_HOLDING_START + S_REG_HOLDING_NREGS)))
    {
        return 0;
    }
    return usSRegHoldBuf[addr - S_REG_HOLDING_START];
}

/**
 * @brief  设置保持寄存器的值.
 * @param  addr     地址.
 * @param  vol      值
 * @return 成功返回1.
 */
int8_t MBSlave::SetHoldReg(uint16_t addr,uint16_t vol)
{
    if ((addr < S_REG_HOLDING_START) || (addr > (S_REG_HOLDING_START + S_REG_HOLDING_NREGS)))
    {
        return -1;
    }
    usSRegHoldBuf[addr - S_REG_HOLDING_START] = vol;
    return 1;
}

/**
 * @brief  modbus发送函数.
 * @param  p        发送的数组.
 * @param  len      长度
 */
void MBSlave::Send(uint8_t *p, uint16_t len)
{
    uint16_t crc16_temp = CRC16(p, len);
    if(_rs485_pin >= 0){
        digitalWrite((PIN_Name)_rs485_pin, _level);
    } 
    while (len--){
        SerialPutByte(*(p++));
    }
    SerialPutByte((uint8_t)crc16_temp);
    SerialPutByte((uint8_t)(crc16_temp >> 8));
    if(_rs485_pin >= 0){
        delay(5);//等待发送完成，再切换485控制引脚
        digitalWrite((PIN_Name)_rs485_pin, !(_level));
    }
}

/**
 * @brief  modbus初始化函数.
 */
void MBSlave::Init()
{
    uint8_t timeout;
    if(_rs485_pin >= 0)
    {
        pinMode((PIN_Name)_rs485_pin, GPIO_Mode_Out_PP);
    }
    _USARTx->CR1 &= ~((0x3U<<9)|(0x1U<<12));
    if(_eParity != 0){
        _USARTx->CR1 |= (uint32_t)_eParity;    //设置校验位
    }
    this->_Serialx->begin( _baud );
    timeout = 200000 / _baud;
    if(timeout <= 5){
        timeout = 5;
    }
    this->_Serialx->setTimeout(timeout);
}

/**
 * @brief  modbus读线圈（01功能码）处理函数（内部调用）.
 */
void MBSlave::func_01()
{
    uint8_t k;
    uint16_t slen, sadder;
    slen = ModBus_data[mb_s_rec_len - 4] * 256 + ModBus_data[mb_s_rec_len - 3];
    if (slen % 8)
    {
        mb_s_send_len = slen / 8 + 1;
    }
    else
    {
        mb_s_send_len = (slen / 8);
    }

    sadder = ModBus_data[2] * 256 + ModBus_data[3];

    if (sadder < S_COIL_START)
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x81;
        ModBus_Send_data[2] = 0x02;
        Send(ModBus_Send_data, 3);
    }
    else if ((sadder + slen) > (S_COIL_START + S_COIL_NCOILS))
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x81;
        ModBus_Send_data[2] = 0x03;
        Send(ModBus_Send_data, 3);
    }
    else
    {

        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x01;
        ModBus_Send_data[2] = mb_s_send_len;
        for (k = 0; k < mb_s_send_len; k++)
        {
            ModBus_Send_data[k + 3] = ucSCoilBuf[(sadder - S_COIL_START) / 8 + k];
        }
        Send(ModBus_Send_data, mb_s_send_len + 3);
    }
}

/**
 * @brief modbus读离散量输入（02功能码）处理函数（内部调用）.
 */
void MBSlave::func_02()
{

    uint8_t k;
    uint16_t slen, sadder;
    slen = ModBus_data[mb_s_rec_len - 4] * 256 + ModBus_data[mb_s_rec_len - 3];
    if (slen % 8)
    {
        mb_s_send_len = slen / 8 + 1;
    }
    else
    {
        mb_s_send_len = (slen / 8);
    }

    sadder = ModBus_data[2] * 256 + ModBus_data[3];

    if (sadder < S_DISCRETE_INPUT_START) //地址错误
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x82;
        ModBus_Send_data[2] = 0x02;
        Send(ModBus_Send_data, 3);
    }
    else if ((sadder + slen) > (S_DISCRETE_INPUT_START + S_DISCRETE_INPUT_NDISCRETES)) //数据越界
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x82;
        ModBus_Send_data[2] = 0x03;
        Send(ModBus_Send_data, 3);
    }
    else
    {

        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x02;
        ModBus_Send_data[2] = mb_s_send_len;
        for (k = 0; k < mb_s_send_len; k++)
        {
            ModBus_Send_data[k + 3] = ucSDiscInBuf[(sadder - S_DISCRETE_INPUT_START) / 8 + (sadder - S_DISCRETE_INPUT_START) % 8 + k];
        }
        Send(ModBus_Send_data, mb_s_send_len + 3);
    }
}

/**
 * @brief modbus读保持寄存器（03功能码）处理函数（内部调用）.
 */
void MBSlave::func_03()
{
    uint8_t k;
    uint16_t sadder;
    mb_s_send_len = ModBus_data[mb_s_rec_len - 3] * 2;
    sadder = ModBus_data[2] * 256 + ModBus_data[3];
    if (sadder < S_REG_HOLDING_START)
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x83;
        ModBus_Send_data[2] = 0x02;
        Send(ModBus_Send_data, 3);
    }
    else if ((sadder + mb_s_send_len) > (S_REG_HOLDING_START + (S_REG_HOLDING_NREGS * 2)))
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x83;
        ModBus_Send_data[2] = 0x03;
        Send(ModBus_Send_data, 3);
    }
    else
    {

        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x03;
        ModBus_Send_data[2] = mb_s_send_len;
        for (k = 0; k < (mb_s_send_len / 2); k++)
        {

            ModBus_Send_data[2 * k + 4] = (uint8_t)usSRegHoldBuf[(sadder - S_REG_HOLDING_START) + k];
            ModBus_Send_data[2 * k + 3] = usSRegHoldBuf[(sadder - S_REG_HOLDING_START) + k] >> 8;
        }
        Send(ModBus_Send_data, mb_s_send_len + 3);
    }
}

/**
 * @brief modbus读输入寄存器（04功能码）处理函数（内部调用）.
 */
void MBSlave::func_04()
{
    uint8_t k;
    uint16_t sadder;
    mb_s_send_len = ModBus_data[mb_s_rec_len - 3] * 2;
    sadder = ModBus_data[2] * 256 + ModBus_data[3];
    if (sadder < S_REG_HOLDING_START)
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x84;
        ModBus_Send_data[2] = 0x02;
        Send(ModBus_Send_data, 3);
    }
    else if ((sadder + mb_s_send_len) > (S_REG_INPUT_START + (S_REG_INPUT_NREGS * 2)))
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x84;
        ModBus_Send_data[2] = 0x03;
        Send(ModBus_Send_data, 3);
    }
    else
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x04;
        ModBus_Send_data[2] = mb_s_send_len;
        for (k = 0; k < (mb_s_send_len / 2); k++)
        {

            ModBus_Send_data[2 * k + 4] = (uint8_t)usSRegInBuf[(sadder - S_REG_INPUT_START) + k];
            ModBus_Send_data[2 * k + 3] = usSRegInBuf[(sadder - S_REG_INPUT_START) + k] >> 8;
        }
        Send(ModBus_Send_data, mb_s_send_len + 3);
    }
}

/**
 * @brief modbus写单个线圈（05功能码）处理函数（内部调用）.
 */
void MBSlave::func_05()
{
    uint8_t k;
    uint16_t sadder = ModBus_data[2] * 256 + ModBus_data[3];
    if (sadder < S_COIL_START || sadder > (S_COIL_START + S_COIL_NCOILS))
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x85;
        ModBus_Send_data[2] = 0x02;
        Send(ModBus_Send_data, 3);
    }
    else
    {
        sadder = (sadder - S_COIL_START);
        if (ModBus_data[4])
        {
            ucSCoilBuf[sadder / 8] |= (1 << (sadder % 8));
        }
        else
        {
            ucSCoilBuf[sadder / 8] &= ~(1 << (sadder % 8));
        }

        for (k = 0; k < 8; k++)
            ModBus_Send_data[k] = ModBus_data[k];
        Send(ModBus_Send_data, 6);
    }
}

/**
 * @brief modbus写单个保持寄存器（06功能码）处理函数（内部调用）.
 */
void MBSlave::func_06()
{
    uint8_t k;
    uint16_t sadder = ModBus_data[2] * 256 + ModBus_data[3];
    if (sadder < S_REG_HOLDING_START || sadder > (S_REG_HOLDING_START + S_REG_HOLDING_NREGS))
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x86;
        ModBus_Send_data[2] = 0x02;
        Send(ModBus_Send_data, 3);
    }
    else
    {
        sadder = (sadder - S_REG_HOLDING_START);
        usSRegHoldBuf[sadder] = ModBus_data[4] * 256 + ModBus_data[5];

        for (k = 0; k < 8; k++)
            ModBus_Send_data[k] = ModBus_data[k];
        Send(ModBus_Send_data, 6);
    }
}

/**
 * @brief modbus写多个线圈（15功能码）处理函数（内部调用）.
 */
void MBSlave::func_15()
{
    uint8_t k;
    uint16_t slen, sadder;
    slen = ModBus_data[4] * 256 + ModBus_data[5];
    if (slen % 8)
    {
        mb_s_send_len = slen / 8 + 1;
    }
    else
    {
        mb_s_send_len = (slen / 8);
    }

    sadder = ModBus_data[2] * 256 + ModBus_data[3];

    if (sadder < S_COIL_START)
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x8F;
        ModBus_Send_data[2] = 0x02;
        Send(ModBus_Send_data, 3);
    }
    else if ((sadder + slen) > (S_COIL_START + S_COIL_NCOILS))
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x8F;
        ModBus_Send_data[2] = 0x03;
        Send(ModBus_Send_data, 3);
    }
    else
    {
        for (k = 0; k < ModBus_data[6]; k++)

            ucSCoilBuf[((sadder - S_COIL_START) / 8) + k] = ModBus_data[7 + k]; //只能以8的倍数为起始地址发送

        for (k = 0; k < 8; k++)
            ModBus_Send_data[k] = ModBus_data[k];
        Send(ModBus_Send_data, 6);
    }
}

/**
 * @brief modbus写多个保持寄存器（16功能码）处理函数（内部调用）.
 */
void MBSlave::func_16()
{
    uint8_t k;
    uint16_t sadder;
    mb_s_send_len = ModBus_data[6];
    sadder = ModBus_data[2] * 256 + ModBus_data[3];
    if (sadder < S_REG_HOLDING_START)
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x90;
        ModBus_Send_data[2] = 0x02;
        Send(ModBus_Send_data, 3);
    }
    else if ((sadder + mb_s_send_len) > (S_REG_HOLDING_START + (S_REG_HOLDING_NREGS * 2)))
    {
        ModBus_Send_data[0] = _id;
        ModBus_Send_data[1] = 0x90;
        ModBus_Send_data[2] = 0x03;
        Send(ModBus_Send_data, 3);
    }
    else
    {
        for (k = 0; k < 8; k++)
            ModBus_Send_data[k] = ModBus_data[k];
        Send(ModBus_Send_data, 6);
    }
    for (k = 0; k < (mb_s_send_len / 2); k++)
    {
        usSRegHoldBuf[sadder - S_REG_HOLDING_START + k] = ModBus_data[7 + 2 * k] * 256 + ModBus_data[8 + 2 * k];
    }
}

/**
 * @brief modbus轮询（在主函数中轮询）.
 */
void MBSlave::Poll()
{
    uint16_t mb_s_crc_temp = 0; // 16位CRC临时变量
    if(this->_Serialx->available() > 0){
        String str = this->_Serialx->readString();  //读取一帧数据
        mb_s_rec_len = str.length();
        for(int i=0;i<mb_s_rec_len;i++)
        {
            ModBus_data[i] = str[i];
        }
        str.~String();
        mb_s_crc_temp = CRC16(ModBus_data, mb_s_rec_len - 2); //CRC校验
        if ((ModBus_data[mb_s_rec_len - 1] == (mb_s_crc_temp >> 8)) &&          \
                (ModBus_data[mb_s_rec_len - 2] == (uint8_t)mb_s_crc_temp) &&    \
                    (ModBus_data[0] == _id))
        {
            if (ModBus_data[1] == 1)
            {
                func_01();
            }else if (ModBus_data[1] == 2)
            {
                func_02();
            }else if (ModBus_data[1] == 3)
            {
                func_03();
            }else if (ModBus_data[1] == 4)
            {
                func_04();
            }else if (ModBus_data[1] == 5)
            {
                func_05();
            }else if (ModBus_data[1] == 6)
            {
                func_06();
            }else if (ModBus_data[1] == 15)
            {
                func_15();
            }else if (ModBus_data[1] == 16)
            {
                func_16();
            } 
        }
    }
}

/**
 * @brief 串口写1字节函数（内部调用）.
 * @param ucByte    数据
 */
void MBSlave::SerialPutByte(uint8_t ucByte)
{
    this->_Serialx->write(ucByte);
}

#endif

