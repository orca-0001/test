#ifndef _DHTXX_H_
#define _DHTXX_H_

#include <TWEN32F0xx.h>

/* 判断高低电平的临界值 */
#define DHTxx_COUNT         30
/* 数据长度 */
#define DHTxx_MAXTIMINGS    85

#define DHT11               11
#define DHT22               22
#define DHT21               21
#define AM2301              21

/*
 *  DHT11串口读取的counter值：
 *    5-56-59-
 *    36-14-36-14-35-47-35-15-35-14-35-48-35-47-35-14-
 *    34-14-36-14-35-14-35-15-35-14-35-14-36-14-35-15-
 *    35-15-35-14-35-15-35-14-35-46-35-48-35-47-35-47-
 *    35-15-35-14-35-14-36-14-35-14-35-15-35-14-35-14-
 *    36-14-35-14-35-48-35-47-35-14-35-48-35-14-35-46-
 *    37-0-
 */

class DHTxx
{
public:
    DHTxx(uint8_t port);
    DHTxx(uint8_t port, uint8_t type, uint8_t count=DHTxx_COUNT);
    int8_t readTemperature(bool S=false);
    int8_t readHumidity(void);
    int convertCtoF(int c);

private:
    uint8_t data[6];
    uint8_t _pin, _type, _count;
    bool read(void);
    unsigned long _lastreadtime;
    bool firstreading;
};

DHTxx::DHTxx(uint8_t port)
{
    _pin = port;
    _type = DHT11;
    _count = DHTxx_COUNT;
    firstreading = true;
    pinMode((PIN_Name)_pin, GPIO_Mode_IN_FLOATING);
    digitalWrite((PIN_Name)_pin, HIGH);
    _lastreadtime = 0;
}

DHTxx::DHTxx(uint8_t port, uint8_t type, uint8_t count)
{
    _pin = port;
    _type = type;
    _count = count;
    firstreading = true;
    pinMode((PIN_Name)_pin, GPIO_Mode_IN_FLOATING);
    digitalWrite((PIN_Name)_pin, HIGH);
    _lastreadtime = 0;
}

/************************************************************************
  * @brief  读取温度.
  * @param  S  0:摄氏度; 1:华氏度.
  * @return 温度值(-20~60℃)
  **********************************************************************/
int8_t DHTxx::readTemperature(bool S) 
{
    uint16_t f;
    if (read()) 
    {
        switch (_type) {
            case DHT11:
                f = data[2];
                if(data[3]&0x80){
                    f = -f;
                }
                break; 
            case DHT22:
            case DHT21:
                f = (((uint16_t)(data[2] & 0x7F)<<8)|data[3]) / 10;
                if (data[2] & 0x80){
                    f = -f;
                }
        }
        if(S){
            f = convertCtoF(f);
        }
        return f;
    }
    return 0;
}

int DHTxx::convertCtoF(int c) 
{
    return c * 9 / 5 + 32;
}

/************************************************************************
  * @brief  读取湿度.
  * @param  none.
  * @return 湿度值(5 ~ 95%RH), 失败返回-1.
  **********************************************************************/
int8_t DHTxx::readHumidity(void) 
{
    uint16_t f;
    if (read()) 
    {
        switch (_type) {
            case DHT11:
                f = data[0];
                break;
            case DHT21:
            case DHT22:
                f = (((uint16_t)data[0]<<8) | data[1]) / 10;
                break;
        }
        return f;
    }
    return (-1);
}

bool DHTxx::read(void) 
{
    uint8_t laststate = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0, i;
    unsigned long currenttime;
    
    // pull the pin high and wait 250 milliseconds
    digitalWrite((PIN_Name)_pin, HIGH);
    delayMicroseconds(40);

    currenttime = millis();
    if (currenttime < _lastreadtime) 
    {
        // ie there was a rollover
        _lastreadtime = 0;
    }
    if (!firstreading && ((currenttime - _lastreadtime) < 2000)) 
    {
        return true; // return last correct measurement
    }
    firstreading = false;
    _lastreadtime = millis();
    data[0] = data[1] = data[2] = data[3] = data[4] = 0;
    
    // now pull it low for ~20 milliseconds
    pinMode((PIN_Name)_pin, GPIO_Mode_Out_PP);
    digitalWrite((PIN_Name)_pin, LOW);
    delay(20);          //延时20ms
    digitalWrite((PIN_Name)_pin, HIGH);
    pinMode((PIN_Name)_pin, GPIO_Mode_IN_FLOATING);

    // read in timings
    for ( i=0; i< DHTxx_MAXTIMINGS; i++) 
    {
        counter = 0;
        while (digitalRead((PIN_Name)_pin) == laststate) 
        {
            counter++;
            //延时1us,速率慢的单片机,这里可以不需要
            if (counter == 255) 
            {
                break;
            }
        }
        laststate = digitalRead((PIN_Name)_pin);
        if (counter == 255) break;
        // 忽略最前面的3个数据
        if ((i >= 4) && (i%2 == 0)) 
        {
            // shove each bit into the storage bytes
            data[j/8] <<= 1;
            if (counter > _count)
                data[j/8] |= 1;
            j++;
        }
    }
    // check we read 40 bits and that the checksum matches
    if((j >= 40)&&( data[4] == (uint8_t)(data[0] + data[1] + data[2] + data[3]))){
        return true;
    }
    return false;
}

#endif