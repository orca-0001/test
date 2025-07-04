
// The GCC compiler defines the current architecture derived from the -mcpu argument.
// When target cpu is the cortex-m0, it automatically defines __ARM_ARCH_6M__
#ifndef __ARM_ARCH_6M__
  #error "The target ARM cpu must be Cortex-M0 compatible (-mcpu=cortex-m0)"
#endif

// Declare a weak alias macro as described in the GCC manual[1][2]
#define WEAK_ALIAS(f) __attribute__ ((weak, alias (#f)));
#define SECTION(s) __attribute__ ((section(s)))

/******************************************************************************
 * Forward undefined IRQ handlers to an infinite loop function. The Handlers
 * are weakly aliased which means that (re)definitions will overide these.
 *****************************************************************************/

void irq_undefined() {
  // Do nothing when occured interrupt is not defined, just keep looping
  while(1);
}


void WWDG_IRQHandler(void)       WEAK_ALIAS(irq_undefined);
void PVD_IRQHandler(void)      WEAK_ALIAS(irq_undefined);
void RTC_IRQHandler(void)    WEAK_ALIAS(irq_undefined);
void FLASH_IRQHandler(void)    WEAK_ALIAS(irq_undefined);
void RCC_CRS_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void EXTI0_1_IRQHandler(void)    WEAK_ALIAS(irq_undefined);
void EXTI2_3_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void EXTI4_15_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void DMA1_Channel1_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void DMA1_Channel2_3_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void DMA1_Channel4_5_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void ADC_COMP_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)   WEAK_ALIAS(irq_undefined);
void TIM1_CC_IRQHandler(void)    WEAK_ALIAS(irq_undefined);
void TIM2_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void TIM3_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void TIM14_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void TIM16_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void TIM17_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void I2C1_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void SPI1_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void SPI2_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void UART1_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void UART2_IRQHandler(void)      WEAK_ALIAS(irq_undefined);
void AES_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void CAN_IRQHandler(void)     WEAK_ALIAS(irq_undefined);
void USB_IRQHandler(void)      WEAK_ALIAS(irq_undefined);
 

/*****************************************************************************
 * Forward undefined fault handlers to an infinite loop function. The Handlers
 * are weakly aliased which means that (re)definitions will overide these.
 ****************************************************************************/

void fault_undefined() {
  // Do nothing when occured interrupt is not defined, just keep looping
  while(1);
}

void NMI_Handler(void)          WEAK_ALIAS(fault_undefined);
void HardFault_Handler(void)    WEAK_ALIAS(fault_undefined);
void SVC_Handler(void)          WEAK_ALIAS(fault_undefined);
void PendSV_Handler(void)       WEAK_ALIAS(fault_undefined);
void SysTick_Handler(void)      WEAK_ALIAS(fault_undefined);

/******************************************************************************
 * Forward undefined IRQ handlers to an infinite loop function. The Handlers
 * are weakly aliased which means that (re)definitions will overide these.
 *****************************************************************************/

// Prototype the entry values, which are handled by the linker script
extern void stack_entry;
extern void boot_entry(void);

// Defined irq vectors using simple c code following the description in a white 
// paper from ARM[3] and code example from Simonsson Fun Technologies[4].
// These vectors are placed at the memory location defined in the linker script
const void *vectors[] SECTION(".irq_vectors") =
{
  // Stack and program reset entry point
  (void*)&stack_entry,   // The initial stack pointer
  boot_entry,            // The reset handler
NMI_Handler,
HardFault_Handler,0,0,0,0,0,0,0,SVC_Handler,
0,0,
PendSV_Handler,
SysTick_Handler,

WWDG_IRQHandler, 
PVD_IRQHandler,
RTC_IRQHandler,
FLASH_IRQHandler,
RCC_CRS_IRQHandler,
EXTI0_1_IRQHandler,
EXTI2_3_IRQHandler,
EXTI4_15_IRQHandler,
0,
DMA1_Channel1_IRQHandler,
DMA1_Channel2_3_IRQHandler,
DMA1_Channel4_5_IRQHandler,
ADC_COMP_IRQHandler,
TIM1_BRK_UP_TRG_COM_IRQHandler,
TIM1_CC_IRQHandler,
TIM2_IRQHandler,
TIM3_IRQHandler,
0,
0,
TIM14_IRQHandler,
0,
TIM16_IRQHandler,
TIM17_IRQHandler,
I2C1_IRQHandler,
0,
SPI1_IRQHandler,
SPI2_IRQHandler,
UART1_IRQHandler,
UART2_IRQHandler,
AES_IRQHandler,
CAN_IRQHandler,
USB_IRQHandler, 
};

/******************************************************************************
 * References
 *  [1] http://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
 *  [2] http://gcc.gnu.org/onlinedocs/gcc/Variable-Attributes.html
 *  [3] http://www.arm.com/files/pdf/Cortex-M3_programming_for_ARM7_developers.pdf
 *  [4] http://fun-tech.se/stm32/OlimexBlinky/mini.php
 *****************************************************************************/

