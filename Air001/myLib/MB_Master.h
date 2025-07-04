#ifndef _MB_Master_H
#define _MB_Master_H

#include "HardwareSerial.h"
#include "CRC_16.h"

#ifndef USART_PARITY_NONE
#define USART_PARITY_NONE            0x00000000U
#endif
#ifndef USART_PARITY_EVEN
#define USART_PARITY_EVEN            ((uint32_t)USART_CR1_PCE | USART_CR1_M)
#endif
#ifndef USART_PARITY_ODD
#define USART_PARITY_ODD             ((uint32_t)(USART_CR1_PCE | USART_CR1_PS | USART_CR1_M))
#endif

class MBMaster{
public:
    MBMaster(HardwareSerial* Serialx,uint32_t baud, uint16_t eParity);
    MBMaster(HardwareSerial* Serialx,uint32_t baud, uint16_t eParity,int8_t rs485_pin,uint8_t level);
    void Init();     //modbus主机初始化
    int8_t ReqReadCoils(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout); //读线圈 
    int8_t ReqWriteCoil(uint8_t MB_id, uint16_t addr,  uint8_t _data, uint16_t timeout); //写线圈
    int8_t ReqWriteCoil_S(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout); //写多个线圈
    int8_t ReqReadDiscreteInputs(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout); //读离散量输入
    int8_t ReqReadInputRegister(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout); //读输入寄存器
    int8_t ReqWriteHoldingRegister(uint8_t MB_id, uint16_t addr, uint16_t  _data, uint16_t timeout); //写单个保持寄存器
    int8_t ReqWriteHoldingRegister_S(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout); //写多个保持寄存器
    int8_t ReqReadHoldingRegister(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout); //读保持寄存器
    int8_t ReqReadWriteHoldingRegister_S(uint8_t MB_id, uint16_t raddr, uint16_t ruslen,uint16_t waddr, uint16_t wuslen,uint8_t *databuf, uint16_t timeout); //读写保持寄存器
private:
    uint8_t mb_m_rec_temp[32]; //保存主站接收到的数据
    uint8_t mb_m_rec_len = 0;         //接收长度

    USART_TypeDef* _USARTx;
    HardwareSerial* _Serialx;
    int8_t _rs485_pin = -1;
    uint8_t _level;
    uint32_t _baud;
    uint16_t _eParity;

    void SerialInit(); //串口初始化
    void SerialPutByte(uint8_t ucByte); //串口发送字节

    void RecDisable();  //关闭串口接收
    void RecEnble();    //使能串口接收
    void Send(uint8_t *p, uint16_t len);
};

MBMaster::MBMaster(HardwareSerial* Serialx,uint32_t baud, uint16_t eParity)
{
    _Serialx = Serialx;
    if(_Serialx==&Serial1){
        _USARTx = USART1;
    }else if(_Serialx==&Serial2){
        _USARTx = USART2;
    }
    _baud = baud;
    _eParity = eParity;
}

MBMaster::MBMaster(HardwareSerial* Serialx,uint32_t baud, uint16_t eParity,int8_t rs485_pin,uint8_t level)
{
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

/************************************************************************
 * @brief  关闭串口接收.
 * @param  none.
 * @return none.
 **********************************************************************/
void MBMaster::RecDisable()
{
    _USARTx->CR1 &= ~(USART_CR1_RE|USART_CR1_RXNEIE); //接收禁止,RXNE中断禁止
    _USARTx->SR  &= ~(USART_SR_RXNE);    //RXNE标志清零
}

/************************************************************************
 * @brief  使能串口接收.
 * @param  none.
 * @return none.
 **********************************************************************/
void MBMaster::RecEnble()
{
    _USARTx->CR1 |= (uint32_t)(USART_CR1_RE|USART_CR1_TE|USART_CR1_RXNEIE);
}

/************************************************************************
 * @brief  modbus发送函数(内部调用).
 * @param  p    发送的数组.
 * @param  len  发送多少字节.
 * @return none.
 **********************************************************************/
void MBMaster::Send(uint8_t *p, uint16_t len)
{
    uint16_t crc16_temp = CRC16(p, len);

    if(_rs485_pin >= 0){
        digitalWrite((PIN_Name)_rs485_pin, _level);
    } 
    RecDisable();
    while (len--){
        SerialPutByte(*(p++));
    }
    SerialPutByte((uint8_t)crc16_temp);
    SerialPutByte((uint8_t)(crc16_temp >> 8));
    if(_rs485_pin >= 0){
        delay(10);      //等待发送完成，再切换485控制引脚
        digitalWrite((PIN_Name)_rs485_pin, !(_level));
    }
    RecEnble();
}

/************************************************************************
 * @brief  读线圈函数.
 * @param  MB_id        从站ID.
 * @param  addr         地址.
 * @param  uslen        长度.
 * @param  databuf      读回来的线圈值指针.
 * @param  timeout      超时时间(ms).
 * @return -1,接收数据错误;0,接收超时;1,成功.
 *     数据会以字节的形式返回,低地址线圈数据在最低位,不需要读的数据以0填充高位字节。
 *     例如读20地址的线圈,长度为11,读到的buf数据为0xCD,0x05
 *       要读11个线圈数据，所有数据会以2个字节返回,databuf指针指向的数组至少要2个字节。
 *       第一个字节为0xcd,转为二进制是1100 1101,20地址是这个数据的LSB,27地址是数据的MSB.
 *       在最后的数据字节中，将30-28地址输出表示为0x05,二进制是0000 0101。输出30是
 *       左侧第六个比特位置，输出28是这个字节的LSB,用零填充五个剩余高位比特。
 **********************************************************************/
int8_t MBMaster::ReqReadCoils(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout)
{
    uint8_t mb_m_command[6];    //保存主站要发送的数据
    uint16_t mb_m_j = 0;        //临时循环用变量
    uint16_t mb_m_crc_temp = 0; // 16位CRC临时变量

    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x01;//功能号
    mb_m_command[2] = (uint8_t)(addr >> 8);//地址高
    mb_m_command[3] = addr;//地址低
    mb_m_command[4] = (uint8_t)(uslen >> 8);//寄存器高
    mb_m_command[5] = uslen;//寄存器低
    Send(mb_m_command, 6);//发送
    
    (uslen % 8) ? (uslen = uslen / 8 + 1) : (uslen = uslen / 8);
    mb_m_j = 0;
    while(mb_m_j < timeout) //判断超时
    {
        delay(1);
        mb_m_j++;
        if(this->_Serialx->available() > 0){  //接收数据
            String str = this->_Serialx->readString();  //读取一帧数据
            //RecDisable();
            mb_m_rec_len = str.length();
            for(int i=0;i<mb_m_rec_len;i++)
            {
                mb_m_rec_temp[i] = str[i];
            }
            str.~String();
            mb_m_crc_temp = CRC16(mb_m_rec_temp, mb_m_rec_len - 2); //CRC校验
            if ((mb_m_rec_temp[mb_m_rec_len - 1] == (mb_m_crc_temp >> 8)) &&            \
                    (mb_m_rec_temp[mb_m_rec_len - 2] == (uint8_t)mb_m_crc_temp))   //CRC校验成功     
            {
                if(mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x01){  //返回正确数据
                    for (uint8_t j = 0; j < uslen; j++){
                        databuf[j] = mb_m_rec_temp[3 + j];
                    }
                    return 1;// 成功
                }
            }
            return -1;      //数据不正确
        }
    }
    return 0;//超时
}

/************************************************************************
 * @brief  写线圈函数.
 * @param  MB_id        从站ID.
 * @param  addr         地址.
 * @param  _data        线圈值(0或者1).
 * @param  timeout      超时时间(ms).
 * @return -1,接收数据错误;0,接收超时;1,成功.
 ************************************************************************/
int8_t MBMaster::ReqWriteCoil(uint8_t MB_id, uint16_t addr,  uint8_t _data, uint16_t timeout)
{
    uint8_t mb_m_command[6];    //保存主站要发送的数据
    uint16_t mb_m_j = 0;        //临时循环用变量
    uint16_t mb_m_crc_temp = 0; // 16位CRC临时变量

    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x05;//功能号
    mb_m_command[2] = (uint8_t)(addr >> 8);//地址高
    mb_m_command[3] = addr;//地址低
    mb_m_command[4] = (_data?0xff:0x00);
    mb_m_command[5] = 0x00;
    Send(mb_m_command, 6);//发送

    mb_m_j = 0;
    while(mb_m_j < timeout)//判断超时
    {
        delay(1);
        mb_m_j++;
        if(this->_Serialx->available() > 0){  //接收数据
            String str = this->_Serialx->readString();  //读取一帧数据
            //RecDisable();
            mb_m_rec_len = str.length();
            for(int i=0;i<mb_m_rec_len;i++)
            {
                mb_m_rec_temp[i] = str[i];
            }
            str.~String();
            mb_m_crc_temp = CRC16(mb_m_rec_temp, mb_m_rec_len - 2); //CRC校验
            if ((mb_m_rec_temp[mb_m_rec_len - 1] == (mb_m_crc_temp >> 8)) && (mb_m_rec_temp[mb_m_rec_len - 2] == (uint8_t)mb_m_crc_temp))
            {
                if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x05)//返回正确数据
                {
                    return 1;   //正确
                }
            }
            return -1;  //不正确数据
        }
    }
    return 0;
}

/************************************************************************
 * @brief  写多个线圈函数.
 * @param  MB_id        从站ID.
 * @param  addr         起始地址.
 * @param  uslen        线圈长度.
 * @param  databuf      数据指针.
 * @param  timeout      超时时间(ms).
 * @return -1,接收数据错误;0,接收超时;1,成功.
 *      和读线圈一致,8个线圈值组成一个字节数据,低地址线圈放在字节低位。
 *      最后数据字节中的未使用比特用零填充。
 ************************************************************************/
int8_t MBMaster::ReqWriteCoil_S(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout)
{
    uint8_t mb_m_command[32];    //保存主站要发送的数据
    uint16_t mb_m_j = 0;        //临时循环用变量
    uint16_t mb_m_crc_temp = 0; // 16位CRC临时变量

    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x0F;//功能号
    mb_m_command[2] = (uint8_t)(addr >> 8);//地址高
    mb_m_command[3] = addr;//地址低
    mb_m_command[4] = (uint8_t)(uslen >> 8);//寄存器高
    mb_m_command[5] = uslen;//寄存器低
    (uslen % 8) ? (uslen = uslen / 8 + 1) : (uslen = uslen / 8);
    mb_m_command[6] = uslen;//字节数
    for(int j=0;j<uslen;j++){
        mb_m_command[7+j] = databuf[j];
    }
    
    Send(mb_m_command, 7+uslen);//发送
    mb_m_j = 0;
    while(mb_m_j < timeout)//判断超时
    {
        delay(1);
        mb_m_j++;
        if(this->_Serialx->available() > 0){  //接收数据
            String str = this->_Serialx->readString();  //读取一帧数据
            //RecDisable();
            mb_m_rec_len = str.length();
            for(int i=0;i<mb_m_rec_len;i++)
            {
                mb_m_rec_temp[i] = str[i];
            }
            str.~String();
            mb_m_crc_temp = CRC16(mb_m_rec_temp, mb_m_rec_len - 2); //CRC校验
            if ((mb_m_rec_temp[mb_m_rec_len - 1] == (mb_m_crc_temp >> 8)) && (mb_m_rec_temp[mb_m_rec_len - 2] == (uint8_t)mb_m_crc_temp))
            {
                if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x0F)//返回正确数据
                {
                    return 1;   //正确
                } 
            }
            return -1;  //不正确数据
        }
    }
    return 0;   //超时
}

/************************************************************************
 * @brief  读离散量输入函数.
 * @param  MB_id        从站ID.
 * @param  addr         地址.
 * @param  uslen        长度.
 * @param  databuf      读回来的读离散量输入值指针.
 * @param  timeout      超时时间(ms).
 * @return -1,接收数据错误;0,接收超时;1,成功.
 *     数据会以字节的形式返回,低地址数据在最低位,不需要读的数据以0填充高位字节。
 *     例如读20地址的离散量输入,长度为11,读到的buf数据为0xCD,0x05
 *       要读11个离散量数据，所有数据会以2个字节返回,databuf指针指向的数组至少要2个字节。
 *       第一个字节为0xcd,转为二进制是1100 1101,20地址是这个数据的LSB,27地址是数据的MSB.
 *       在最后的数据字节中，将30-28地址输出表示为0x05,二进制是0000 0101。输出30是
 *       左侧第六个比特位置，输出28是这个字节的LSB,用零填充五个剩余高位比特。
 **********************************************************************/
int8_t MBMaster::ReqReadDiscreteInputs(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout)
{
    uint8_t mb_m_command[6];    //保存主站要发送的数据
    uint16_t mb_m_j = 0;        //临时循环用变量
    uint16_t mb_m_crc_temp = 0; // 16位CRC临时变量

    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x02;//功能号
    mb_m_command[2] = (uint8_t)(addr >> 8);//地址高
    mb_m_command[3] = addr;//地址低
    mb_m_command[4] = (uint8_t)(uslen >> 8);//寄存器数量高
    mb_m_command[5] = uslen;//寄存器数量低
    Send(mb_m_command, 6);//发送

    (uslen % 8) ? (uslen = uslen / 8 + 1) : (uslen = uslen / 8);

    mb_m_j = 0;
    while(mb_m_j < timeout)//判断超时
    {
        delay(1);
        mb_m_j++;
        if(this->_Serialx->available() > 0){  //接收数据
            String str = this->_Serialx->readString();  //读取一帧数据
            //RecDisable();
            mb_m_rec_len = str.length();
            for(int i=0;i<mb_m_rec_len;i++)
            {
                mb_m_rec_temp[i] = str[i];
            }
            str.~String();
            mb_m_crc_temp = CRC16(mb_m_rec_temp, mb_m_rec_len - 2); //CRC校验
            if ((mb_m_rec_temp[mb_m_rec_len - 1] == (mb_m_crc_temp >> 8)) && (mb_m_rec_temp[mb_m_rec_len - 2] == (uint8_t)mb_m_crc_temp))
            {
                if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x02)//返回正确数据
                {
                    for (int j = 0; j < uslen; j++){
                        databuf[j] = mb_m_rec_temp[3 + j];
                    }
                    return 1;
                }
            }
            return -1;          //不正确类型
        }
    }
    return 0;
}

/************************************************************************
 * @brief  读输入寄存器函数.
 * @param  MB_id        从站ID.
 * @param  addr         地址.
 * @param  uslen        要读的寄存器数量.
 * @param  databuf      读回来的数据指针.
 * @param  timeout      超时时间(ms).
 * @return -1,接收数据错误;0,接收超时;1,成功.
 *      在Modbus中一个寄存器为2个字节，返回的数据高字节在前，低字节在后。
 ************************************************************************/
int8_t MBMaster::ReqReadInputRegister(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout)
{
    uint8_t mb_m_command[6];    //保存主站要发送的数据
    uint16_t mb_m_j = 0;        //临时循环用变量
    uint16_t mb_m_crc_temp = 0; // 16位CRC临时变量
    uint8_t mb_m_rec_en = 0;          //接收到数据并CRC检验有效

    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x04;//功能号
    mb_m_command[2] = (uint8_t)(addr >> 8);//地址高
    mb_m_command[3] = addr;//地址低
    mb_m_command[4] = (uint8_t)(uslen >> 8);//寄存器高
    mb_m_command[5] = uslen;//寄存器低
    Send(mb_m_command, 6);//发送

    mb_m_j = 0;
    while(mb_m_j < timeout)//判断超时
    {
        delay(1);
        mb_m_j++;
        if(this->_Serialx->available() > 0){  //接收数据
            String str = this->_Serialx->readString();  //读取一帧数据
            //RecDisable();
            mb_m_rec_len = str.length();
            for(int i=0;i<mb_m_rec_len;i++)
            {
                mb_m_rec_temp[i] = str[i];
            }
            str.~String();
            mb_m_crc_temp = CRC16(mb_m_rec_temp, mb_m_rec_len - 2); //CRC校验
            if ((mb_m_rec_temp[mb_m_rec_len - 1] == (mb_m_crc_temp >> 8)) && (mb_m_rec_temp[mb_m_rec_len - 2] == (uint8_t)mb_m_crc_temp))
            {
                if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x04)//返回正确数据
                {
                    for (int j = 0; j < uslen*2; j++){
                        databuf[j] = mb_m_rec_temp[3 + j];
                    }
                    return 1;       
                }             
            }
            return -1;      //不正确数据
        }
    }
    return 0;
}

/************************************************************************
 * @brief  写单个寄存器函数.
 * @param  MB_id        从站ID.
 * @param  addr         地址.
 * @param  _data        要写入的寄存器值.
 * @param  timeout      超时时间(ms).
 * @return -1,接收数据错误;0,接收超时;1,成功.
 ************************************************************************/
int8_t MBMaster::ReqWriteHoldingRegister(uint8_t MB_id, uint16_t addr, uint16_t  _data, uint16_t timeout)
{
    uint8_t mb_m_command[6];    //保存主站要发送的数据
    uint16_t mb_m_j = 0;        //临时循环用变量
    uint16_t mb_m_crc_temp = 0; // 16位CRC临时变量

    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x06;//功能号
    mb_m_command[2] = (uint8_t)(addr >> 8);//地址高
    mb_m_command[3] = addr;//地址低
    mb_m_command[4] = (uint8_t)(_data >> 8);//字节高
    mb_m_command[5] = _data;//字节低

    Send(mb_m_command, 6);//发送

    mb_m_j = 0;
    while(mb_m_j < timeout)//判断超时
    {
        delay(1);
        mb_m_j++;
        if(this->_Serialx->available() > 0){  //接收数据
            String str = this->_Serialx->readString();  //读取一帧数据
            //RecDisable();
            mb_m_rec_len = str.length();
            for(int i=0;i<mb_m_rec_len;i++)
            {
                mb_m_rec_temp[i] = str[i];
            }
            str.~String();
            mb_m_crc_temp = CRC16(mb_m_rec_temp, mb_m_rec_len - 2); //CRC校验
            if ((mb_m_rec_temp[mb_m_rec_len - 1] == (mb_m_crc_temp >> 8)) && (mb_m_rec_temp[mb_m_rec_len - 2] == (uint8_t)mb_m_crc_temp))
            {
                if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x06)//返回正确数据
                {
                    return 1;
                }
            }
            return -1;//不正确数据
        }
    }
    return 0;
}

/************************************************************************
 * @brief  写多个寄存器函数.
 * @param  MB_id        从站ID.
 * @param  addr         地址.
 * @param  uslen        寄存器数量.
 * @param  databuf      要写入的数据指针.
 * @param  timeout      超时时间(ms).
 * @return -1,接收数据错误;0,接收超时;1,成功.
 *      在Modbus中一个寄存器为2个字节，返回的数据高字节在前，低字节在后。
 ************************************************************************/
int8_t MBMaster::ReqWriteHoldingRegister_S(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout)
{
    uint8_t mb_m_command[32];    //保存主站要发送的数据
    uint16_t mb_m_j = 0;        //临时循环用变量
    uint16_t mb_m_crc_temp = 0; // 16位CRC临时变量

    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x10;//功能号
    mb_m_command[2] = (uint8_t)(addr >> 8);//地址高
    mb_m_command[3] = addr;//地址低
    mb_m_command[4] = (uint8_t)(uslen >> 8);//寄存器高
    mb_m_command[5] = uslen;//寄存器低
    mb_m_command[6] = uslen*2;//
    for(int j=0;j<uslen*2;j++){
        mb_m_command[7+j]=databuf[j];
    }
    Send(mb_m_command, 7+uslen*2);//发送

    mb_m_j = 0;
    while(mb_m_j < timeout)//判断超时
    {
        delay(1);
        mb_m_j++;
        if(this->_Serialx->available() > 0){  //接收数据
            String str = this->_Serialx->readString();  //读取一帧数据
            //RecDisable();
            mb_m_rec_len = str.length();
            for(int i=0;i<mb_m_rec_len;i++)
            {
                mb_m_rec_temp[i] = str[i];
            }
            str.~String();
            mb_m_crc_temp = CRC16(mb_m_rec_temp, mb_m_rec_len - 2); //CRC校验
            if ((mb_m_rec_temp[mb_m_rec_len - 1] == (mb_m_crc_temp >> 8)) && (mb_m_rec_temp[mb_m_rec_len - 2] == (uint8_t)mb_m_crc_temp))
            {
                if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x10)//返回正确数据
                {
                    return 1;
                }
            }
            return -1;      //不正确数据
        }
    }
    return 0;
}

/************************************************************************
 * @brief  读保持寄存器函数.
 * @param  MB_id        从站ID.
 * @param  addr         地址.
 * @param  uslen        要读的寄存器数量.
 * @param  databuf      要写入的数据指针.
 * @param  timeout      超时时间(ms).
 * @return -1,接收数据错误;0,接收超时;1,成功.
 *      在Modbus中一个寄存器为2个字节，返回的数据高字节在前，低字节在后。
 ************************************************************************/
int8_t MBMaster::ReqReadHoldingRegister(uint8_t MB_id, uint16_t addr, uint16_t uslen, uint8_t *databuf, uint16_t timeout)
{
    uint8_t mb_m_command[6];    //保存主站要发送的数据
    uint16_t mb_m_j = 0;        //临时循环用变量
    uint16_t mb_m_crc_temp = 0; // 16位CRC临时变量

    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x03;//功能号
    mb_m_command[2] = (uint8_t)(addr >> 8);//地址高
    mb_m_command[3] = addr;//地址低
    mb_m_command[4] = (uint8_t)(uslen >> 8);//数据长度高
    mb_m_command[5] = uslen;//数据长度低
    mb_m_j = 0;
    Send(mb_m_command, 6);//发送

    while(mb_m_j < timeout)//判断是否超时
    {
        delay(1);
        mb_m_j++;
        if(this->_Serialx->available() > 0){  //接收数据
            String str = this->_Serialx->readString();  //读取一帧数据
            //RecDisable();
            mb_m_rec_len = str.length();
            for(int i=0;i<mb_m_rec_len;i++)
            {
                mb_m_rec_temp[i] = str[i];
            }
            str.~String();
            mb_m_crc_temp = CRC16(mb_m_rec_temp, mb_m_rec_len - 2); //CRC校验
            if ((mb_m_rec_temp[mb_m_rec_len - 1] == (mb_m_crc_temp >> 8)) && (mb_m_rec_temp[mb_m_rec_len - 2] == (uint8_t)mb_m_crc_temp))
            {
                if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x03)//返回正确数据
                {
                    for (int j = 0; j < uslen*2; j++){
                        databuf[j] = mb_m_rec_temp[3 + j];
                    }
                    return 1;
                }
            }
            return -1;//不正确数据
        }
    }
    return 0;
}

/************************************************************************
 * @brief  读写多个保持寄存器函数.
 * @param  MB_id        从站ID.
 * @param  raddr        读数据起始地址.
 * @param  ruslen       要读的寄存器数量.
 * @param  waddr        写数据起始地址.
 * @param  wuslen       要写的数据长度.
 * @param  databuf      要读写的数据指针.
 * @param  timeout      超时时间(ms).
 * @return -1,接收数据错误;0,接收超时;1,成功.
 *      在Modbus中一个寄存器为2个字节，返回的数据高字节在前，低字节在后。
 ************************************************************************/
int8_t MBMaster::ReqReadWriteHoldingRegister_S(uint8_t MB_id, uint16_t raddr, uint16_t ruslen,uint16_t waddr, uint16_t wuslen,uint8_t *databuf, uint16_t timeout)
{
    uint8_t mb_m_command[32];    //保存主站要发送的数据
    uint16_t mb_m_j = 0;        //临时循环用变量
    uint16_t mb_m_crc_temp = 0; // 16位CRC临时变量

    mb_m_command[0] = MB_id;//站号
    mb_m_command[1] = 0x17;//功能号
    mb_m_command[2] = (uint8_t)(raddr >> 8);//读起始地址高字节
    mb_m_command[3] = raddr;//读起始地址低字节
    mb_m_command[4] = (uint8_t)(ruslen >> 8);//读的数量高字节
    mb_m_command[5] = ruslen;//读的数量低字节
    mb_m_command[6] = (uint8_t)(waddr >> 8);//写起始地址高字节
    mb_m_command[7] = waddr;//写起始地址低字节
    mb_m_command[8] = (uint8_t)(wuslen >> 8);//写的数量高字节
    mb_m_command[9] = wuslen;//写的数量低字节
    mb_m_command[10] = wuslen*2;//
    
    for(int j=0;j<wuslen*2;j++){
        mb_m_command[11+j]=databuf[j];
    }
    mb_m_j = 0;
    Send(mb_m_command, 11+wuslen*2);//发送

    while(mb_m_j < timeout)//判断超时
    {
        delay(1);
        mb_m_j++;
        if(this->_Serialx->available() > 0){  //接收数据
            String str = this->_Serialx->readString();  //读取一帧数据
            //RecDisable();
            mb_m_rec_len = str.length();
            for(int i=0;i<mb_m_rec_len;i++)
            {
                mb_m_rec_temp[i] = str[i];
            }
            str.~String();
            mb_m_crc_temp = CRC16(mb_m_rec_temp, mb_m_rec_len - 2); //CRC校验
            if ((mb_m_rec_temp[mb_m_rec_len - 1] == (mb_m_crc_temp >> 8)) && (mb_m_rec_temp[mb_m_rec_len - 2] == (uint8_t)mb_m_crc_temp))
            {
                if (mb_m_rec_temp[0] == MB_id && mb_m_rec_temp[1] == 0x17)//返回正确数据
                {
                    for (int j = 0; j < ruslen*2; j++){
                        databuf[j] = mb_m_rec_temp[3 + j];
                    }
                    return 1;
                }
            }
            return -1;//不正确数据
        }
    }
    return 0;
}

/************************************************************************
 * @brief  主站初始化函数.
 * @param  none
 * @return none
 ************************************************************************/
void MBMaster::Init()
{
    uint8_t timeout = 5;
    if(_rs485_pin >= 0){
        pinMode((PIN_Name)_rs485_pin, GPIO_Mode_Out_PP);
    }
    _USARTx->CR1 &= ~((0x3U << 9)|(0x1U << 12));
    if(_eParity != 0){
        _USARTx->CR1 |= (uint32_t)_eParity;    //设置校验位
    }
    this->_Serialx->begin( _baud );
    timeout = 200000 / _baud;
    if(timeout <= 5){
        timeout = 5;
    }
    this->_Serialx->setTimeout(timeout);
    RecDisable();
}

/************************************************************************
 * @brief  串口写1字节函数（内部调用）.
 * @param  none
 * @return none
 ************************************************************************/
void MBMaster::SerialPutByte(uint8_t ucByte)
{
    while((_USARTx->SR&USART_SR_TXE) != USART_SR_TXE); //等待发送缓冲为空
    _USARTx->DR = (ucByte & (uint32_t)0x000000ff);
    while((_USARTx->SR&USART_SR_TC) == 0); //传送完成
}

#endif