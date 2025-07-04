/**
 * @file CRC.h
 * @brief 根据生成多项式, CRC计算单元将输入32位数据,运算产生一个CRC结果
 *        使用CRC-32(以太网)多项式: 0x4C11DB7.
 * 
 * @version 0.1
 * @date 2023-01-03
 * 
 * @copyright Copyright (c) 2022 杭州天问五幺科技有限公司
 * 
 */
#ifndef _CRC_H_
#define _CRC_H_

#include "TW32f0xx.h"

/**
 * @brief 初始化CRC模块
 */
void CRC_begin()
{
    __HAL_RCC_CRC_CLK_ENABLE();
    CRC->CR |= CRC_CR_RESET;
    CRC->IDR &= ~CRC_IDR_IDR;

}

/**
 * @brief   计算CRC结果.
 *            该函数会以上一次计算的CRC值作为初始值开始计算.
 * @param   pBuffer     输入数组
 * @param   BufferLength    长度
 * @return  输出CRC值
 */
uint32_t CRC_accumulate(uint32_t pBuffer[], uint32_t BufferLength)
{
    uint32_t index; 
    uint32_t temp = 0U;
    for(index=0;index<BufferLength;index++)
    {
        CRC->DR = pBuffer[index];
    }
    temp = CRC->DR;
    return temp;
}

/**
 * @brief 计算CRC结果
 *          以pBuffer[0]作为初始值开始计算
 * @param   pBuffer     输入数组
 * @param   BufferLength    长度
 * @return  输出CRC值         
 */
uint32_t CRC_Calculate(uint32_t pBuffer[], uint32_t BufferLength)
{
    uint32_t index; 
    uint32_t temp = 0U;

    CRC->CR |= CRC_CR_RESET;    //复位CRC模块
    for (index = 0U; index < BufferLength; index++)
    {
        CRC->DR = pBuffer[index];
    }
    temp = CRC->DR;
    return temp;
}


#endif // !_




