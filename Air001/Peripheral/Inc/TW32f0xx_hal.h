/**
  ******************************************************************************
  * @file    TW32f0xx_hal.h
  * @author  MCU Application Team
  * @brief   This file contains all the functions prototypes for the HAL
  *          module driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) Twen Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * This software is owned and published by:
  * Twen Semiconductor Co., Ltd.
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  *
  *  1. Redistributions of source code must retain the above copyright notice,
  *     this list of conditions and the following disclaimer.
  *  2. Redistributions in binary form must reproduce the above copyright notice,
  *     this list of conditions and the following disclaimer in the documentation
  *     and/or other materials provided with the distribution.
  *  3. Neither the name of the copyright holder nor the names of its contributors
  *     may be used to endorse or promote products derived from this software
  *     without specific prior written permission.
  *
  *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
  *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  *  POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TW32F0xx_HAL_H
#define __TW32F0xx_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TW32f0xx_hal_conf.h"

/** @addtogroup TW32F0xx_HAL_Driver
  * @{
  */

/** @addtogroup HAL
  * @{
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup HAL_Exported_Constants HAL Exported Constants
  * @{
  */

/** @defgroup HAL_TICK_FREQ Tick Frequency
  * @{
  */
typedef enum
{
  HAL_TICK_FREQ_10HZ         = 100U,
  HAL_TICK_FREQ_100HZ        = 10U,
  HAL_TICK_FREQ_1KHZ         = 1U,
  HAL_TICK_FREQ_DEFAULT      = HAL_TICK_FREQ_1KHZ
} HAL_TickFreqTypeDef;
/**
  * @}
  */
/* Exported types ------------------------------------------------------------*/
extern uint32_t uwTickPrio;
extern uint32_t uwTickFreq;
/** @defgroup SYSCFG_Exported_Constants SYSCFG Exported Constants
  * @{
  */

/** @defgroup SYSCFG_BootMode Boot Mode
  * @{
  */
#define SYSCFG_BOOT_MAINFLASH          0x00000000U                      /*!< Main Flash memory mapped at 0x0000 0000   */
#define SYSCFG_BOOT_SYSTEMFLASH        SYSCFG_CFGR1_MEM_MODE_0          /*!< System Flash memory mapped at 0x0000 0000 */
#define SYSCFG_BOOT_SRAM               (SYSCFG_CFGR1_MEM_MODE_1 | SYSCFG_CFGR1_MEM_MODE_0)  /*!< Embedded SRAM mapped at 0x0000 0000 */

/**
  * @}
  */
/* Exported macro ------------------------------------------------------------*/
/** @defgroup HAL_Exported_Macros HAL Exported Macros
  * @{
  */

/** @defgroup DBGMCU_Freeze_Unfreeze Freeze Unfreeze Peripherals in Debug mode
  * @brief   Freeze/Unfreeze Peripherals in Debug mode
  * Note:
  *       Debug registers DBGMCU_IDCODE and DBGMCU_CR are accessible only in
  *       debug mode (not accessible by the user software in normal mode).
  *       Refer to errata sheet of these devices for more details.
  * @{
  */

/* Peripherals on APB1 */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup HAL_Exported_Macros HAL Exported Macros
  * @{
  */

/** @defgroup HAL_Freeze_Unfreeze_Peripherals HAL Freeze Unfreeze Peripherals
  * @brief  Freeze/Unfreeze Peripherals in Debug mode
  * @{
  */



#if defined(DBGMCU_APB_FZ1_DBG_RTC_STOP)
#define __HAL_DBGMCU_FREEZE_RTC()            (DBGMCU->APBFZ1 |= (DBGMCU_APB_FZ1_DBG_RTC_STOP))
#define __HAL_DBGMCU_UNFREEZE_RTC()          (DBGMCU->APBFZ1 &= ~(DBGMCU_APB_FZ1_DBG_RTC_STOP))
#endif /* DBGMCU_APB_FZ_DBG_RTC_STOP */

#if defined(DBGMCU_APB_FZ1_DBG_I2C_SMBUS_TIMEOUT)
#define __HAL_DBGMCU_FREEZE_I2C_TIMEOUT()   (DBGMCU->APBFZ1 |= (DBGMCU_APB_FZ1_DBG_I2C_SMBUS_TIMEOUT))
#define __HAL_DBGMCU_UNFREEZE_I2C_TIMEOUT() (DBGMCU->APBFZ1 &= ~(DBGMCU_APB_FZ1_DBG_I2C_SMBUS_TIMEOUT))
#endif /* DBGMCU_APB_FZ_DBG_I2C_SMBUS_TIMEOUT */

#if defined(DBGMCU_APB_FZ1_DBG_IWDG_STOP)
#define __HAL_DBGMCU_FREEZE_IWDG()           (DBGMCU->APBFZ1 |= (DBGMCU_APB_FZ1_DBG_IWDG_STOP))
#define __HAL_DBGMCU_UNFREEZE_IWDG()         (DBGMCU->APBFZ1 &= ~(DBGMCU_APB_FZ1_DBG_IWDG_STOP))
#endif /* DBGMCU_APB_FZ_DBG_IWDG_STOP */

#if defined(DBGMCU_APB_FZ1_DBG_WWDG_STOP)
#define __HAL_DBGMCU_FREEZE_WWDG()           (DBGMCU->APBFZ1 |= (DBGMCU_APB_FZ1_DBG_WWDG_STOP))
#define __HAL_DBGMCU_UNFREEZE_WWDG()         (DBGMCU->APBFZ1 &= ~(DBGMCU_APB_FZ1_DBG_WWDG_STOP))
#endif /* DBGMCU_APB_FZ_DBG_WWDG_STOP */

#if defined(DBGMCU_APB_FZ1_DBG_TIM2_STOP)
#define __HAL_DBGMCU_FREEZE_TIM2()           (DBGMCU->APBFZ1 |= (DBGMCU_APB_FZ1_DBG_TIM2_STOP))
#define __HAL_DBGMCU_UNFREEZE_TIM2()         (DBGMCU->APBFZ1 &= ~(DBGMCU_APB_FZ1_DBG_TIM2_STOP))
#endif /* DBGMCU_APB_FZ_DBG_TIM2_STOP */

#if defined(DBGMCU_APB_FZ1_DBG_TIM3_STOP)
#define __HAL_DBGMCU_FREEZE_TIM3()           (DBGMCU->APBFZ1 |= (DBGMCU_APB_FZ1_DBG_TIM3_STOP))
#define __HAL_DBGMCU_UNFREEZE_TIM3()         (DBGMCU->APBFZ1 &= ~(DBGMCU_APB_FZ1_DBG_TIM3_STOP))
#endif /* DBGMCU_APB_FZ_DBG_TIM3_STOP */

#if defined(DBGMCU_APB_FZ1_DBG_TIM6_STOP)
#define __HAL_DBGMCU_FREEZE_TIM6()           (DBGMCU->APBFZ1 |= (DBGMCU_APB_FZ1_DBG_TIM6_STOP))
#define __HAL_DBGMCU_UNFREEZE_TIM6()         (DBGMCU->APBFZ1 &= ~(DBGMCU_APB_FZ1_DBG_TIM6_STOP))
#endif /* DBGMCU_APB_FZ_DBG_TIM6_STOP */

#if defined(DBGMCU_APB_FZ1_DBG_TIM7_STOP)
#define __HAL_DBGMCU_FREEZE_TIM7()           (DBGMCU->APBFZ1 |= (DBGMCU_APB_FZ1_DBG_TIM7_STOP))
#define __HAL_DBGMCU_UNFREEZE_TIM7()         (DBGMCU->APBFZ1 &= ~(DBGMCU_APB_FZ1_DBG_TIM7_STOP))
#endif /* DBGMCU_APB_FZ_DBG_TIM7_STOP */

#if defined(DBGMCU_APB_FZ1_DBG_TIM14_STOP)
#define __HAL_DBGMCU_FREEZE_TIM14()          (DBGMCU->APBFZ1 |= (DBGMCU_APB_FZ1_DBG_TIM14_STOP))
#define __HAL_DBGMCU_UNFREEZE_TIM14()        (DBGMCU->APBFZ1 &= ~(DBGMCU_APB_FZ1_DBG_TIM14_STOP))
#endif /* DBGMCU_APB_FZ_DBG_TIM14_STOP */

#if defined(DBGMCU_APB_FZ2_DBG_TIM1_STOP)
#define __HAL_DBGMCU_FREEZE_TIM1()           (DBGMCU->APBFZ2 |= (DBGMCU_APB_FZ2_DBG_TIM1_STOP))
#define __HAL_DBGMCU_UNFREEZE_TIM1()         (DBGMCU->APBFZ2 &= ~(DBGMCU_APB_FZ2_DBG_TIM1_STOP))
#endif /* DBGMCU_APB_FZ_DBG_TIM1_STOP */

#if defined(DBGMCU_APB_FZ2_DBG_TIM15_STOP)
#define __HAL_DBGMCU_FREEZE_TIM15()          (DBGMCU->APBFZ2 |= (DBGMCU_APB_FZ2_DBG_TIM15_STOP))
#define __HAL_DBGMCU_UNFREEZE_TIM15()        (DBGMCU->APBFZ2 &= ~(DBGMCU_APB_FZ2_DBG_TIM15_STOP))
#endif /* DBGMCU_APB_FZ2_DBG_TIM15_STOP */

#if defined(DBGMCU_APB_FZ2_DBG_TIM16_STOP)
#define __HAL_DBGMCU_FREEZE_TIM16()          (DBGMCU->APBFZ2 |= (DBGMCU_APB_FZ2_DBG_TIM16_STOP))
#define __HAL_DBGMCU_UNFREEZE_TIM16()        (DBGMCU->APBFZ2 &= ~(DBGMCU_APB_FZ2_DBG_TIM16_STOP))
#endif /* DBGMCU_APB_FZ_DBG_TIM16_STOP */

#if defined(DBGMCU_APB_FZ2_DBG_TIM17_STOP)
#define __HAL_DBGMCU_FREEZE_TIM17()          (DBGMCU->APBFZ2 |= (DBGMCU_APB_FZ2_DBG_TIM17_STOP))
#define __HAL_DBGMCU_UNFREEZE_TIM17()        (DBGMCU->APBFZ2 &= ~(DBGMCU_APB_FZ2_DBG_TIM17_STOP))
#endif /* DBGMCU_APB_FZ_DBG_TIM17_STOP */
/**
  * @}
  */

/** @defgroup HAL_Private_Macros HAL Private Macros
  * @{
  */
#define IS_TICKFREQ(FREQ) (((FREQ) == HAL_TICK_FREQ_10HZ)  || \
                           ((FREQ) == HAL_TICK_FREQ_100HZ) || \
                           ((FREQ) == HAL_TICK_FREQ_1KHZ))


/** @brief  SYSCFG Break Cortex-M0+ Lockup lock.
  *         Enables and locks the connection of Cortex-M0+ LOCKUP (Hardfault) output to TIM1/15/16/17 Break input
  * @note   The selected configuration is locked and can be unlocked only by system reset.
  */
#define __HAL_SYSCFG_BREAK_LOCKUP_LOCK()        SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_LOCKUP_LOCK)
/** @brief  SYSCFG Break PVD lock.
  *         Enables and locks the PVD connection with Timer1/15/16/17 Break input, as well as the PVDE and PLS[2:0] in the PWR_CR register
  * @note   The selected configuration is locked and can be unlocked only by system reset
  */
#define __HAL_SYSCFG_BREAK_PVD_LOCK()           SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_PVD_LOCK)
/** @brief  SYSCFG COMP1 AS TIMx break.
  *         COMP1 as Timer1Break input
  * @note   The selected configuration is locked and can be unlocked only by system reset
  */
#define __HAL_SYSCFG_COMP1_BREAK_TIM1()           SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP1_BRK_TIM1)
#define __HAL_SYSCFG_COMP2_BREAK_TIM1()           SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP2_BRK_TIM1)
#define __HAL_SYSCFG_COMP1_BREAK_TIM16()           SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP1_BRK_TIM16)
#define __HAL_SYSCFG_COMP2_BREAK_TIM16()           SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP2_BRK_TIM16)
#define __HAL_SYSCFG_COMP1_BREAK_TIM17()           SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP1_BRK_TIM17)
#define __HAL_SYSCFG_COMP2_BREAK_TIM17()           SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP2_BRK_TIM17)

/**
  * @}
  */
/** @brief  Fast mode Plus driving capability on a specific GPIO
  */
#define SYSCFG_FASTMODEPLUS_PB6        SYSCFG_CFGR1_I2C_PB6_FMP  /*!< Enable Fast mode Plus on PB6 */
#define SYSCFG_FASTMODEPLUS_PB7        SYSCFG_CFGR1_I2C_PB7_FMP  /*!< Enable Fast mode Plus on PB7 */
#define SYSCFG_FASTMODEPLUS_PB8        SYSCFG_CFGR1_I2C_PB8_FMP  /*!< Enable Fast mode Plus on PB8 */
#define SYSCFG_FASTMODEPLUS_PB9        SYSCFG_CFGR1_I2C_PB9_FMP  /*!< Enable Fast mode Plus on PB9 */
#define SYSCFG_FASTMODEPLUS_PA9        SYSCFG_CFGR1_I2C_PA9_FMP  /*!< Enable Fast mode Plus on PA9 */
#define SYSCFG_FASTMODEPLUS_PA10       SYSCFG_CFGR1_I2C_PA10_FMP /*!< Enable Fast mode Plus on PA10 */

/**
 * @}
 */

/** @defgroup SYSCFG_FastModePlus_I2Cx Fast mode Plus driving capability activation for I2Cx
  * @{
  */

/** @brief  Fast mode Plus driving capability on a specific GPIO
  */
#define SYSCFG_FASTMODEPLUS_I2C1       SYSCFG_CFGR1_I2C1_FMP /*!< Enable Fast mode Plus on I2C1 */
/* Exported functions --------------------------------------------------------*/
/** @addtogroup HAL_Exported_Functions
  * @{
  */
/** @addtogroup HAL_Exported_Functions_Group1
  * @{
  */
/** @brief  Fast-mode Plus driving capability enable/disable macros
  * @param __FASTMODEPLUS__ This parameter can be a value of @ref SYSCFG_FastModePlus_GPIO
  */
#define __HAL_SYSCFG_FASTMODEPLUS_ENABLE(__FASTMODEPLUS__)  do {assert_param(IS_SYSCFG_FASTMODEPLUS((__FASTMODEPLUS__)));\
                                                                SET_BIT(SYSCFG->CFGR1, (__FASTMODEPLUS__));\
                                                               }while(0U)

#define __HAL_SYSCFG_FASTMODEPLUS_DISABLE(__FASTMODEPLUS__) do {assert_param(IS_SYSCFG_FASTMODEPLUS((__FASTMODEPLUS__)));\
                                                                CLEAR_BIT(SYSCFG->CFGR1, (__FASTMODEPLUS__));\
                                                               }while(0U)

/* Initialization and de-initialization functions  ******************************/
HAL_StatusTypeDef HAL_Init(void);
HAL_StatusTypeDef HAL_DeInit(void);
void HAL_MspInit(void);
void HAL_MspDeInit(void);
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority);
/**
  * @}
  */

/** @addtogroup HAL_Exported_Functions_Group2
  * @{
  */
/* Peripheral Control functions  ************************************************/
void HAL_IncTick(void);
void HAL_Delay(uint32_t Delay);
uint32_t HAL_GetTick(void);
uint32_t HAL_GetTickPrio(void);
HAL_StatusTypeDef HAL_SetTickFreq(uint32_t Freq);
uint32_t HAL_GetTickFreq(void);
void HAL_SuspendTick(void);
void HAL_ResumeTick(void);
uint32_t HAL_GetHalVersion(void);
uint32_t HAL_GetREVID(void);
uint32_t HAL_GetDEVID(void);
uint32_t HAL_GetUIDw0(void);
uint32_t HAL_GetUIDw1(void);
uint32_t HAL_GetUIDw2(void);
void HAL_DBGMCU_EnableDBGSleepMode(void);
void HAL_DBGMCU_DisableDBGSleepMode(void);
void HAL_DBGMCU_EnableDBGStopMode(void);
void HAL_DBGMCU_DisableDBGStopMode(void);
void HAL_SYSCFG_DMA_Req(uint32_t Requset);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup HAL_Private_Variables HAL Private Variables
  * @{
  */
/**
  * @}
  */
/* Private constants ---------------------------------------------------------*/
/** @defgroup HAL_Private_Constants HAL Private Constants
  * @{
  */
/**
  * @}
  */
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __TW32F0xx_HAL_H */

/************************ (C) COPYRIGHT Twen *****END OF FILE****/
