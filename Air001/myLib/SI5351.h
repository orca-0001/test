#ifndef SI5351_h
#define SI5351_h

#include "TW32f0xx_SOFTIIC.h"

//Si5351寄存器声明

#define PLL_SRC 0x00
#define XTAL_CL 0x80

#define OUTPUT_ENABLE_CONTROL  3

#define SI_CLK0_CONTROL	16			// Register definitions
#define SI_CLK1_CONTROL	17
#define SI_CLK2_CONTROL	18
#define SI_CLK3_CONTROL	19
#define SI_CLK4_CONTROL	20
#define SI_CLK5_CONTROL	21
#define SI_CLK6_CONTROL	22
#define SI_CLK7_CONTROL	23


#define SI_SYNTH_MS_0		42
#define SI_SYNTH_MS_1		50
#define SI_SYNTH_MS_2		58
#define SPREAD_SPECTRUM_PARAMETER 149

#define MULTISYNTH0_PARAMETERS_3  44
#define MULTISYNTH1_PARAMETERS_3  52
#define MULTISYNTH2_PARAMETERS_3  60

#define SI_PLL_RESET		177

#define INTERNAL_LOAD_CAPACITANCE   183

#define SI5351_CRYSTAL_LOAD_6PF     (1 << 6)
#define SI5351_CRYSTAL_LOAD_8PF     (2 << 6)
#define SI5351_CRYSTAL_LOAD_10PF    (3 << 6)


#define XTAL_FREQ	25000000			// Crystal frequency

typedef enum {
  SI_PLL_A = 0,
  SI_PLL_B 
} si5351PLL_t;

typedef enum {
  SI5351_MULTISYNTH_DIV_4 = 4,
  SI5351_MULTISYNTH_DIV_6 = 6,
  SI5351_MULTISYNTH_DIV_8 = 8
} si5351MultisynthDiv_t;


typedef enum {
  SI5351_R_DIV_1 = 0,
  SI5351_R_DIV_2 = 1,
  SI5351_R_DIV_4 = 2,
  SI5351_R_DIV_8 = 3,
  SI5351_R_DIV_16 = 4,
  SI5351_R_DIV_32 = 5,
  SI5351_R_DIV_64 = 6,
  SI5351_R_DIV_128 = 7,
} si5351RDiv_t;

class SI5351
{
public:
    SI5351(uint8_t sda_pin,uint8_t scl_pin);
    void begin(uint8_t _addr=0xc0);  //初始化
    void enableSpreadSpectrum(bool enabled) ;//使能扩频
    uint32_t setupPLL(si5351PLL_t pll, uint32_t a, uint32_t b, uint32_t c);
    void setupPLLInt(si5351PLL_t pll, uint8_t a);
    void setupMultisynth(uint8_t channel,si5351PLL_t pll, uint32_t a, uint32_t b, uint32_t c);
    void setupMultisynthInt(uint8_t channel,si5351PLL_t pll, si5351MultisynthDiv_t div);
    void SetFrequency(uint32_t frequency , uint8_t Chanal );
    void setupRdiv(uint8_t output,si5351RDiv_t  div);
    void enableOut(bool enabled);
private:
    KSoftIIC IIC;
	uint8_t _SI5351_ADDR;
    uint8_t _sda_pin;
    uint8_t _scl_pin;
    uint8_t lastRdivValue[3];
    void Si5351_SendREG(uint8_t REG_Address,uint8_t REG_data);
    uint8_t Si5351_ReadREG(uint8_t REG_Address);
    void Si5351_SendREG_Len(uint8_t REG_Address,uint8_t *REG_data,uint8_t LEN);
};

SI5351::SI5351(uint8_t sda_pin,uint8_t scl_pin)
{
    _sda_pin = sda_pin;
	_scl_pin = scl_pin;
}
void SI5351::begin(uint8_t _addr=0xc0)
{
    _SI5351_ADDR = (_addr);
    IIC.begin(_sda_pin, _scl_pin);
    for (uint8_t i = 0; i < 3; i++) 
    {
        lastRdivValue[i] = 0;
    }
    delay(100);
    Si5351_SendREG(OUTPUT_ENABLE_CONTROL,0xFF);// Disable all outputs
    Si5351_SendREG(SI_CLK0_CONTROL,0x80);//Power down all output drivers
    Si5351_SendREG(SI_CLK1_CONTROL,0x80);
    Si5351_SendREG(SI_CLK2_CONTROL,0x80);
    Si5351_SendREG(SI_CLK3_CONTROL,0x80);
    Si5351_SendREG(SI_CLK4_CONTROL,0x80);
    Si5351_SendREG(SI_CLK5_CONTROL,0x80);
    Si5351_SendREG(SI_CLK6_CONTROL,0x80);
    Si5351_SendREG(SI_CLK7_CONTROL,0x80);

    Si5351_SendREG(INTERNAL_LOAD_CAPACITANCE,SI5351_CRYSTAL_LOAD_10PF);

    enableSpreadSpectrum(false);//Disable spread spectrum output
}
void SI5351::Si5351_SendREG(uint8_t REG_Address,uint8_t REG_data)
{
    IIC.start(_SI5351_ADDR);
    IIC.write(REG_Address);
    IIC.write(REG_data);
    IIC.stop();
}
void SI5351::Si5351_SendREG_Len(uint8_t REG_Address,uint8_t *REG_data,uint8_t LEN)
{
    IIC.start(_SI5351_ADDR);
    IIC.write(REG_Address);
    for (uint8_t i = 0; i < LEN; i++)
    {
        IIC.write(REG_data[i]);
    }
    IIC.stop();
}
uint8_t SI5351::Si5351_ReadREG(uint8_t REG_Address)
{
    uint8_t rec;
    IIC.start(_SI5351_ADDR);
    IIC.write(REG_Address);
    IIC.restart(_SI5351_ADDR|0x01);
    rec = IIC.read(1);
    IIC.stop();
    return rec;
}
/**
 * @brief  使能扩频
 * @param  enabled         trur/false
 */
void SI5351::enableSpreadSpectrum(bool enabled) 
{
    uint8_t temp;
    temp = Si5351_ReadREG(SPREAD_SPECTRUM_PARAMETER);
    if (enabled) 
    {
        temp |= 0x80;
    } else
    {
        temp &= ~0x80;
    }   
    Si5351_SendREG(SPREAD_SPECTRUM_PARAMETER,temp);
}
/**
 * @brief 输出使能
 * @param  enabled          
 */
void SI5351::enableOut(bool enabled)
{
    if (enabled) 
    {
        Si5351_SendREG(OUTPUT_ENABLE_CONTROL,0x0);
    } else
    {
        Si5351_SendREG(OUTPUT_ENABLE_CONTROL,0xff);
    }       
}
/**
 * @brief 设置PPL时钟 返回0失败    a+b/c的范围 （15 + 0/1048575, 90)
 * @param  pll               锁相环A/B
 * @param  a                My Param doc
 * @param  b                My Param doc
 * @param  c                My Param doc
 * @return uint32_t 
 */
uint32_t SI5351::setupPLL(si5351PLL_t pll, uint32_t a, uint32_t b, uint32_t c)
{
    uint32_t P1;					// PLL config register P1
    uint32_t P2;					// PLL config register P2
    uint32_t P3;					// PLL config register P3

    float PLL_FER;
    PLL_FER =  (float)(a + ((float)b / (float)c));
    
    if(PLL_FER<15 || PLL_FER>90)
    {
        return 0;
    }
    if(b==0)/* Integer mode */
    {
        P1 = 128 * a - 512;
        P2 = b;
        P3 = c;        
    }
    else
    {
        P1 =(uint32_t)(128 * a + floor(128 * ((float)b / (float)c)) - 512);
        P2 = (uint32_t)(128 * b - c * floor(128 * ((float)b / (float)c)));
        P3 = c;
    }
    uint8_t baseaddr = (pll == SI_PLL_A ? 26 : 34);

    Si5351_SendREG(baseaddr, (P3 & 0x0000FF00) >> 8);
    Si5351_SendREG(baseaddr + 1, (P3 & 0x000000FF));
    Si5351_SendREG(baseaddr + 2, (P1 & 0x00030000) >> 16);
    Si5351_SendREG(baseaddr + 3, (P1 & 0x0000FF00) >> 8);
    Si5351_SendREG(baseaddr + 4, (P1 & 0x000000FF));
    Si5351_SendREG(baseaddr + 5,((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
    Si5351_SendREG(baseaddr + 6, (P2 & 0x0000FF00) >> 8);
    Si5351_SendREG(baseaddr + 7, (P2 & 0x000000FF));

    Si5351_SendREG(SI_PLL_RESET, (1 << 7) | (1 << 5));//Reset both PLLs

    PLL_FER = XTAL_FREQ*(a + ((float)b / (float)c));
    return PLL_FER;
}
/**
 * @brief Setup PLLA to integer only mode @ 900MHz (must be 600..900MHz) 
 * @param  pll           SI_SYNTH_PLL_A/SI_SYNTH_PLL_B 使用PLLA或PLLB
 * @param  a             倍数（24 and 36）
 */

void SI5351::setupPLLInt(si5351PLL_t pll, uint8_t a)
{
    setupPLL(pll,a,0,1);
}
//
/**
 * @brief 设置多synth  M(x)的范围（8 + 1/1048575, 2048)
 * @param  channel          通道（0-1-2）
 * @param  pll              锁相环A/B
 * @param  a                整数
 * @param  b                分子
 * @param  c                分母
 */
void SI5351::setupMultisynth(uint8_t channel,si5351PLL_t pll, uint32_t a, uint32_t b, uint32_t c)
{
    uint32_t P1;																					// Synth config register P1
    uint32_t P2;																					// Synth config register P2
    uint32_t P3;	
    uint8_t baseaddr = 0;																	// Synth config register P3
  /* Set the main PLL config registers */

    float mx;
    mx =  (float)(a + ((float)b / (float)c));
    if(mx<8 || mx>2048)
    {
        return ;
    }
    /* Fractional mode */
    P1 = (uint32_t)(128 * a + floor(128 * ((float)b / (float)c)) - 512);
    P2 = (uint32_t)(128 * b - c * floor(128 * ((float)b / (float)c)));
    P3 = c;


    switch (channel) 
    {
    case 0:
        baseaddr = SI_SYNTH_MS_0;
    break;
    case 1:
        baseaddr = SI_SYNTH_MS_1;
    break;
    case 2:
        baseaddr = SI_SYNTH_MS_2;
    break;
    default:
        break;
    }
    uint8_t sendBuffer[9];
    sendBuffer[0] = baseaddr;
    sendBuffer[1] = (P3 & 0xFF00) >> 8;
    sendBuffer[2] = P3 & 0xFF;
    sendBuffer[3] = ((P1 & 0x30000) >> 16) | lastRdivValue[channel];
    sendBuffer[4] = (P1 & 0xFF00) >> 8;
    sendBuffer[5] = P1 & 0xFF;             
    sendBuffer[6] = ((P3 & 0xF0000) >> 12) | ((P2 & 0xF0000) >> 16);
    sendBuffer[7] = (P2 & 0xFF00) >> 8;
    sendBuffer[8] = (P2 & 0xFF);

    Si5351_SendREG_Len(baseaddr,&sendBuffer[1],8);
    uint8_t clkControlReg = 0x0F; 

    if (pll == SI_PLL_B)
    clkControlReg |= (1 << 5); /* Uses PLLB */
    if (b == 0)
    clkControlReg |= (1 << 6); /* Integer mode */    

    switch (channel) {
    case 0:
        Si5351_SendREG(SI_CLK0_CONTROL,clkControlReg);
        break;
    case 1:
        Si5351_SendREG(SI_CLK1_CONTROL,clkControlReg);
        break;
    case 2:
        Si5351_SendREG(SI_CLK2_CONTROL,clkControlReg);
        break;
    }
}
/**
 * @brief 
 * @param  channel          通道（0-1-2）
 * @param  div              分频系数
 */
void SI5351::setupRdiv(uint8_t channel,si5351RDiv_t  div)
{
    uint8_t Rreg, regval;
    uint8_t divider = div;

    switch (channel) 
    {
    case 0:
        Rreg = MULTISYNTH0_PARAMETERS_3;
    break;
    case 1:
        Rreg = MULTISYNTH1_PARAMETERS_3;
    break;
    case 2:
        Rreg = MULTISYNTH2_PARAMETERS_3;
    break;
    default:
        break;
    }

    regval = Si5351_ReadREG(Rreg);

    regval &= 0x0F;

    divider &= 0x07;
    divider <<= 4;
    regval |= divider;
    lastRdivValue[channel] = divider;
    return Si5351_SendREG(Rreg, regval);
}
/**
 * @brief 
 * @param  channel          通道（0-1-2）
 * @param  pll              锁相环A/B
 * @param  div              分频系数
 */
void SI5351::setupMultisynthInt(uint8_t channel,si5351PLL_t pll, si5351MultisynthDiv_t div)
{
    setupMultisynth(channel,pll,div,0,1);
}

/**
 * @brief 
 * @param  frequency        频率 单位（HZ）
 * @param  Chanal           通道（0-1-2）
 */
void SI5351::SetFrequency(uint32_t frequency , uint8_t Chanal )
{
    float f;
    uint32_t mult;
    uint32_t num;
    uint32_t denom;
	
	Si5351_SendREG(PLL_SRC,0x00);
	Si5351_SendREG(XTAL_CL,0x80);
    
    if(frequency>=1000000)
    {
        mult = 900000000 / frequency;	//锁相环频率:900 mhz
        if((900000000 % frequency) == 0)
        {
            num = 0;  
            denom = 1;
        }
        else
        {
            f = ((float)900000000 / frequency) - (float)mult;
            num = 1048575 * f / 1;
            denom = 1048575;
        }
        setupPLL(SI_PLL_B,36,0,1);//pll =  900MHZ
        setupMultisynth(Chanal,SI_PLL_B, mult, num, denom);
        setupRdiv(Chanal, SI5351_R_DIV_1);
        enableOut(true);
    }
    else
    {
        mult = 12500000 / frequency;	//锁相环频率:400mhz
        if((12500000 % frequency) == 0)
        {
            num = 0;  
            denom = 1;           
        }
        else
        {
            f = ((float)12500000 / frequency) - (float)mult;
            num = 1048575 * f / 1;
            denom = 1048575;            
        }
        setupPLL(SI_PLL_A,16,0,1);//pll =  400MHZ
        setupMultisynth(Chanal,SI_PLL_A, mult, num, denom);
        setupRdiv(Chanal, SI5351_R_DIV_32);
        enableOut(true);
    }
}

#endif
