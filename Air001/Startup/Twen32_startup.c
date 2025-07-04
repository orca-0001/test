
#include "TW32f0xx.h"
__attribute__ ((section(".startup")))

// These are defined and created by the linker, locating them in memory
extern unsigned char _etext;
extern unsigned char _data;
extern unsigned char _edata;
extern unsigned char _bss;
extern unsigned char _ebss;
//*****************************************************************************
//
// Initialisation of global objects.
//
//*****************************************************************************
extern void __libc_init_array (void);

//*****************************************************************************
//
//  Copy data segment from FLASH to RAM and clear bss segment
//  No stach setup is necessary for Cortex-M3 
//
//*****************************************************************************
extern void (*__init_array_start []) (void) __attribute__((weak));
extern void (*__init_array_end []) (void) __attribute__((weak));
// Prototype the required startup functions
extern void main(void);

// The entry point of the application, prepare segments,
// initialize the cpu and execute main()
void boot_entry(void)
{
  register unsigned char *src, *dst;
  int count;
  int i;

  // Get physical data address and copy it to sram
  src = &_etext;
  dst = &_data;
  while(dst < &_edata) {
    *dst++ = *src++;
  }

  // Clear the bss segment
  dst = &_bss;
  while(dst < &_ebss) {
    *dst++ = 0;
  }

  count = __init_array_end - __init_array_start;
  for (i = 0; i < count; i++)
     __init_array_start[i] ();

  // Execute the code at the program entry point
  main();

  // Do nothing when returned from main, just keep looping
  while(1);
}
