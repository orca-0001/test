/**
 * @file bmp280.h
 * @brief bmp280驱动库
 * @version 0.1
 * @date 2023-01-03
 * 
 * @copyright Copyright (c) 2022 杭州天问五幺科技有限公司
 * 
 */
#ifndef _BMP280_H
#define _BMP280_H

// #define BMP280_USE_HARDIIC      //是否使用硬件iic, 使用硬件iic时引脚配置无效
#if defined(BMP280_USE_HARDIIC)
#include "TW32f0xx_HARDIIC.h"
#else
#include "TW32f0xx_SOFTIIC.h"
#endif

#define BMP280_ADDRESS          (0x77) /**< The default I2C address for the sensor. */
#define BMP280_ADDRESS_ALT      (0x76) /**< Alternative I2C address for the sensor. */
#define BMP280_CHIPID           (0x58) /**< Default chip ID. */

/*!
 * Registers available on the sensor.
 */
enum {
    BMP280_REGISTER_DIG_T1 = 0x88,
    BMP280_REGISTER_DIG_T2 = 0x8A,
    BMP280_REGISTER_DIG_T3 = 0x8C,
    BMP280_REGISTER_DIG_P1 = 0x8E,
    BMP280_REGISTER_DIG_P2 = 0x90,
    BMP280_REGISTER_DIG_P3 = 0x92,
    BMP280_REGISTER_DIG_P4 = 0x94,
    BMP280_REGISTER_DIG_P5 = 0x96,
    BMP280_REGISTER_DIG_P6 = 0x98,
    BMP280_REGISTER_DIG_P7 = 0x9A,
    BMP280_REGISTER_DIG_P8 = 0x9C,
    BMP280_REGISTER_DIG_P9 = 0x9E,
    BMP280_REGISTER_CHIPID = 0xD0,
    BMP280_REGISTER_VERSION = 0xD1,
    BMP280_REGISTER_SOFTRESET = 0xE0,
    BMP280_REGISTER_CAL26 = 0xE1, /**< R calibration = 0xE1-0xF0 */
    BMP280_REGISTER_STATUS = 0xF3,
    BMP280_REGISTER_CONTROL = 0xF4,
    BMP280_REGISTER_CONFIG = 0xF5,
    BMP280_REGISTER_PRESSUREDATA = 0xF7,
    BMP280_REGISTER_TEMPDATA = 0xFA,
};

/*!
 *  Struct to hold calibration data.
 */
typedef struct {
    uint16_t dig_T1; /**< dig_T1 cal register. */
    int16_t  dig_T2;  /**<  dig_T2 cal register. */
    int16_t  dig_T3;  /**< dig_T3 cal register. */

    uint16_t dig_P1; /**< dig_P1 cal register. */
    int16_t  dig_P2;  /**< dig_P2 cal register. */
    int16_t  dig_P3;  /**< dig_P3 cal register. */
    int16_t  dig_P4;  /**< dig_P4 cal register. */
    int16_t  dig_P5;  /**< dig_P5 cal register. */
    int16_t  dig_P6;  /**< dig_P6 cal register. */
    int16_t  dig_P7;  /**< dig_P7 cal register. */
    int16_t  dig_P8;  /**< dig_P8 cal register. */
    int16_t  dig_P9;  /**< dig_P9 cal register. */
} bmp280_calib_data;

/** Oversampling rate for the sensor. */
enum sensor_sampling {
    /** No over-sampling. */
    SAMPLING_NONE = 0x00,
    /** 1x over-sampling. */
    SAMPLING_X1 = 0x01,
    /** 2x over-sampling. */
    SAMPLING_X2 = 0x02,
    /** 4x over-sampling. */
    SAMPLING_X4 = 0x03,
    /** 8x over-sampling. */
    SAMPLING_X8 = 0x04,
    /** 16x over-sampling. */
    SAMPLING_X16 = 0x05
};

/** Operating mode for the sensor. */
enum sensor_mode {
    /** Sleep mode. */
    MODE_SLEEP = 0x00,
    /** Forced mode. */
    MODE_FORCED = 0x01,
    /** Normal mode. */
    MODE_NORMAL = 0x03,
    /** Software reset. */
    MODE_SOFT_RESET_CODE = 0xB6
};

/** Filtering level for sensor data. */
enum sensor_filter {
    /** No filtering. */
    FILTER_OFF = 0x00,
    /** 2x filtering. */
    FILTER_X2 = 0x01,
    /** 4x filtering. */
    FILTER_X4 = 0x02,
    /** 8x filtering. */
    FILTER_X8 = 0x03,
    /** 16x filtering. */
    FILTER_X16 = 0x04
};

/** Standby duration in ms */
enum standby_duration {
    /** 1 ms standby. */
    STANDBY_MS_1 = 0x00,
    /** 62.5 ms standby. */
    STANDBY_MS_63 = 0x01,
    /** 125 ms standby. */
    STANDBY_MS_125 = 0x02,
    /** 250 ms standby. */
    STANDBY_MS_250 = 0x03,
    /** 500 ms standby. */
    STANDBY_MS_500 = 0x04,
    /** 1000 ms standby. */
    STANDBY_MS_1000 = 0x05,
    /** 2000 ms standby. */
    STANDBY_MS_2000 = 0x06,
    /** 4000 ms standby. */
    STANDBY_MS_4000 = 0x07
};


class BMP280{
public:
    BMP280();
    ~BMP280(void);
#if defined(BMP280_USE_HARDIIC)
    bool begin(uint8_t addr = BMP280_ADDRESS_ALT, uint8_t chipid = BMP280_CHIPID);
#else
    bool begin(uint8_t sdapin, uint8_t sclpin, uint8_t addr = BMP280_ADDRESS_ALT, uint8_t chipid = BMP280_CHIPID);    
#endif
    void reset(void);
    uint8_t getStatus(void);
    uint8_t sensorID(void);

    int readTemperature();
    int readPressure(void);
    float readAltitude(float seaLevelhPa = 1013.25);
    float seaLevelForAltitude(float altitude, float atmospheric);
    float waterBoilingPoint(float pressure);

    // void takeForcedMeasurement();
    void setSampling(sensor_mode mode = MODE_NORMAL,
                    sensor_sampling tempSampling = SAMPLING_X16,
                    sensor_sampling pressSampling = SAMPLING_X16,
                    sensor_filter filter = FILTER_OFF,
                    standby_duration duration = STANDBY_MS_1);

private:
    /** Encapsulates the config register */
    struct config {
        /** Initialize to power-on-reset state */
        config() : t_sb(STANDBY_MS_1), filter(FILTER_OFF), none(0), spi3w_en(0) {}
        /** Inactive duration (standby time) in normal mode */
        unsigned int t_sb : 3;
        /** Filter settings */
        unsigned int filter : 3;
        /** Unused - don't set */
        unsigned int none : 1;
        /** Enables 3-wire SPI */
        unsigned int spi3w_en : 1;
        /** Used to retrieve the assembled config register's byte value. */
        unsigned int get() { return (t_sb << 5) | (filter << 2) | spi3w_en; }
    };

    /** Encapsulates trhe ctrl_meas register */
    struct ctrl_meas {
        /** Initialize to power-on-reset state */
        ctrl_meas()
            : osrs_t(SAMPLING_NONE), osrs_p(SAMPLING_NONE), mode(MODE_SLEEP) {}
        /** Temperature oversampling. */
        unsigned int osrs_t : 3;
        /** Pressure oversampling. */
        unsigned int osrs_p : 3;
        /** Device mode */
        unsigned int mode : 2;
        /** Used to retrieve the assembled ctrl_meas register's byte value. */
        unsigned int get() { return (osrs_t << 5) | (osrs_p << 2) | mode; }
    };

    void readCoefficients(void);
    uint8_t spixfer(uint8_t x);
    void write8(uint8_t reg, uint8_t value);
    uint8_t read8(uint8_t reg);
    uint16_t read16(uint8_t reg);
    uint32_t read24(uint8_t reg);
    int16_t readS16(uint8_t reg);
    uint16_t read16_LE(uint8_t reg);
    int16_t readS16_LE(uint8_t reg);

    uint8_t _i2caddr;

    int32_t _sensorID;
    int32_t t_fine;

    bmp280_calib_data _bmp280_calib;
    config _configReg;
    ctrl_meas _measReg;

    uint8_t _sda_pin;
    uint8_t _scl_pin;
};

BMP280::BMP280() 
{
    
}

BMP280::~BMP280(void)
{

}

/***********************************************************************
 * @brief  初始化.
 * @param  addr    设备地址.
 * @param  chipid  设备ID.
 * @param  sda_pin    SDA引脚(如果使用硬件iic则不需要).
 * @param  scl_pin    SCL引脚(如果使用硬件iic则不需要).
 * @return none.
 **********************************************************************/
#if defined(BMP280_USE_HARDIIC)
bool BMP280::begin(uint8_t addr, uint8_t chipid) 
{
    _i2caddr = addr<<1;
    hardiic.begin(100000);

    if (read8(BMP280_REGISTER_CHIPID) != chipid)
    {
        return false;
    }
    
    readCoefficients();
    // write8(BMP280_REGISTER_CONTROL, 0x3F); /* needed? */
    setSampling();
    delay(100);
    return true;
}
#else
bool BMP280::begin(uint8_t sdapin, uint8_t sclpin, uint8_t addr = BMP280_ADDRESS_ALT, uint8_t chipid = BMP280_CHIPID)
{
    _i2caddr = addr<<1;
    _sda_pin = sdapin;
    _scl_pin = sclpin;
    softiic.begin(_sda_pin, _scl_pin);
    if (read8(BMP280_REGISTER_CHIPID) != chipid)
    {
        return false;
    }
    readCoefficients();
    // write8(BMP280_REGISTER_CONTROL, 0x3F); /* needed? */
    setSampling();
    delay(100);
    return true;
}
#endif

/***********************************************************************
 * @brief  设置设备的采样配置。
 * @param  mode    传感器的工作模式.
 * @param  tempSampling   温度读数的采样方案.
 * @param  pressSampling  压力读数的采样方案.
 * @param  filter    要应用的过滤模式(如果有).
 * @param  duration  采样时间.
 * @return none.
 **********************************************************************/
void BMP280::setSampling(sensor_mode mode,
                         sensor_sampling tempSampling,
                         sensor_sampling pressSampling,
                         sensor_filter filter,
                         standby_duration duration) 
{
    _measReg.mode = mode; 
    _measReg.osrs_t = tempSampling;
    _measReg.osrs_p = pressSampling;

    _configReg.filter = filter;
    _configReg.t_sb = duration;

    write8(BMP280_REGISTER_CONFIG, _configReg.get());
    write8(BMP280_REGISTER_CONTROL, _measReg.get());
}

/***********************************************************************
 * @brief  往寄存器写入1个字节数据。
 * @param  reg    寄存器地址.
 * @param  value  数据.
 * @return none.
 **********************************************************************/
void BMP280::write8(uint8_t reg, uint8_t value) 
{
#if defined(BMP280_USE_HARDIIC)
    hardiic.start(_i2caddr);
    hardiic.write(reg);
    hardiic.write(value);
    hardiic.stop();
#else
    softiic.start(_i2caddr);
    softiic.write(reg);
    softiic.write(value);
    softiic.stop();
#endif
}

/***********************************************************************
 * @brief  从寄存器读1个字节数据。
 * @param  reg    寄存器地址.
 * @return 读出的数据.
 **********************************************************************/
uint8_t BMP280::read8(uint8_t reg) 
{
    uint8_t dat;
#if defined(BMP280_USE_HARDIIC)
    hardiic.start(_i2caddr);
    hardiic.write(reg);
    hardiic.restart(_i2caddr | 0x01);
    dat = hardiic.read(1);
    hardiic.stop();
#else
    softiic.start(_i2caddr);
    softiic.write(reg);
    softiic.restart(_i2caddr | 0x01);
    dat = softiic.read(1);
    softiic.stop();
#endif
    return dat;
}

/***********************************************************************
 * @brief  从寄存器读2个字节数据。
 * @param  reg    寄存器起始地址.
 * @return 读出的数据.
 **********************************************************************/
uint16_t BMP280::read16(uint8_t reg) 
{
    uint8_t buffer[2];
    buffer[0] = read8(reg);
    buffer[1] = read8(reg+1);   
    return (uint16_t)(buffer[0]) << 8 | (uint16_t)(buffer[1]);
}

uint16_t BMP280::read16_LE(uint8_t reg) 
{
  uint16_t temp = read16(reg);
  return (temp >> 8) | (temp << 8);
}

int16_t BMP280::readS16(uint8_t reg) 
{ 
    return (int16_t)read16(reg); 
}

int16_t BMP280::readS16_LE(uint8_t reg) 
{
  return (int16_t)read16_LE(reg);
}

/***********************************************************************
 * @brief  从寄存器读3个字节数据。
 * @param  reg    寄存器起始地址.
 * @return 读出的数据.
 **********************************************************************/
uint32_t BMP280::read24(uint8_t reg) 
{
    uint8_t buffer[3];

    buffer[0] = read8(reg);
    buffer[1] = read8(reg+1);
    buffer[2] = read8(reg+2);
    return (uint32_t)(buffer[0]) << 16 | (uint32_t)(buffer[1]) << 8 |
           (uint32_t)(buffer[2]);
}

/***********************************************************************
 * @brief  读取BMP280出厂时的集合系数
 * @param  none.
 * @return none.
 **********************************************************************/
void BMP280::readCoefficients() {
    _bmp280_calib.dig_T1 = read16_LE(BMP280_REGISTER_DIG_T1);
    _bmp280_calib.dig_T2 = readS16_LE(BMP280_REGISTER_DIG_T2);
    _bmp280_calib.dig_T3 = readS16_LE(BMP280_REGISTER_DIG_T3);

    _bmp280_calib.dig_P1 = read16_LE(BMP280_REGISTER_DIG_P1);
    _bmp280_calib.dig_P2 = readS16_LE(BMP280_REGISTER_DIG_P2);
    _bmp280_calib.dig_P3 = readS16_LE(BMP280_REGISTER_DIG_P3);
    _bmp280_calib.dig_P4 = readS16_LE(BMP280_REGISTER_DIG_P4);
    _bmp280_calib.dig_P5 = readS16_LE(BMP280_REGISTER_DIG_P5);
    _bmp280_calib.dig_P6 = readS16_LE(BMP280_REGISTER_DIG_P6);
    _bmp280_calib.dig_P7 = readS16_LE(BMP280_REGISTER_DIG_P7);
    _bmp280_calib.dig_P8 = readS16_LE(BMP280_REGISTER_DIG_P8);
    _bmp280_calib.dig_P9 = readS16_LE(BMP280_REGISTER_DIG_P9);
}

/***********************************************************************
 * @brief  读取BMP280温度
 * @param  none.
 * @return none.
 **********************************************************************/
int BMP280::readTemperature() 
{
  int32_t var1, var2;
  int32_t adc_T = read24(BMP280_REGISTER_TEMPDATA);
  adc_T >>= 4;

  var1 = ((((adc_T >> 3) - ((int32_t)_bmp280_calib.dig_T1 << 1))) *     \
          ((int32_t)_bmp280_calib.dig_T2)) >> 11;

  var2 = (((((adc_T >> 4) - ((int32_t)_bmp280_calib.dig_T1)) *          \
            ((adc_T >> 4) - ((int32_t)_bmp280_calib.dig_T1))) >>12) *   \
            ((int32_t)_bmp280_calib.dig_T3)) >> 14;
  t_fine = var1 + var2;
  int T = (t_fine * 5 + 128) >> 8;
  return T / 100;
}

/***********************************************************************
 * @brief  读取BMP280气压,单位为Pa
 * @param  none.
 * @return none.
 **********************************************************************/
int BMP280::readPressure() 
{
    int64_t var1, var2, p;

    // Must be done first to get the t_fine variable set up
    readTemperature();

    int32_t adc_P = read24(BMP280_REGISTER_PRESSUREDATA);
    adc_P >>= 4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_bmp280_calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)_bmp280_calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)_bmp280_calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)_bmp280_calib.dig_P3) >> 8) +
            ((var1 * (int64_t)_bmp280_calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)_bmp280_calib.dig_P1) >> 33;

    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)_bmp280_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)_bmp280_calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)_bmp280_calib.dig_P7) << 4);
    return (int)p / 256;
}

/**************************************************************
 * 使用读到的气压值和提供的海平面hPa作为参考计算大约高度。
 * @param seaLevelhPa 目前海平面的hPa。
 * @return 海拔高度，单位为米
 **************************************************************/
float BMP280::readAltitude(float seaLevelhPa) 
{
    float altitude;

    float pressure = readPressure(); // in Si units for Pascal
    pressure /= 100;

    altitude = 44330 * (1.0 - pow(pressure / seaLevelhPa, 0.1903));

    return altitude;
}

/**************************************************************
 * @brief 计算指定高度的海平面气压(QFH)和气压(QFE)。
 * @param altitude     海拔高度，单位为m
 * @param atmospheric  大气压（hPa）
 * @return 近似气压(hPa)
 **************************************************************/
float BMP280::seaLevelForAltitude(float altitude, float atmospheric) 
{
    // Equation taken from BMP180 datasheet (page 17):
    // http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

    // Note that using the equation from wikipedia can give bad results
    // at high altitude.  See this thread for more information:
    // http://forums.adafruit.com/viewtopic.php?f=22&t=58064
    return atmospheric / pow(1.0 - (altitude / 44330.0), 5.255);
}

/**************************************************************
 * @brief 计算给定压力下水的沸点。
 * @param pressure 
 *         气压
 * @return 温度（℃）
 **************************************************************/
float BMP280::waterBoilingPoint(float pressure) {
    // Magnusformular for calculation of the boiling point of water at a given
    // pressure
    return (234.175 * log(pressure / 6.1078)) /
            (17.08085 - log(pressure / 6.1078));
}

/***********************************************************************
 * @brief  复位
 * @param  none.
 * @return none.
 **********************************************************************/
void BMP280::reset(void) 
{
    write8(BMP280_REGISTER_SOFTRESET, MODE_SOFT_RESET_CODE);
}

/***********************************************************************
 * @brief  返回传感器ID
 * @param  none.
 * @return 0x61(BME680), 0x60(BME280), 0x56,0x57,0x58(BMP280).
 **********************************************************************/
uint8_t BMP280::sensorID(void) 
{
    return read8(BMP280_REGISTER_CHIPID);
}

/***********************************************************************
 * @brief  从硬件状态寄存器获取最新的传感器事件。
 * @param  none.
 * @return 事件.
 **********************************************************************/
uint8_t BMP280::getStatus(void) 
{
    return read8(BMP280_REGISTER_STATUS);
}

#endif






