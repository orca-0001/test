/*MEMORY {   flash(rx): ORIGIN = 0x8000800, LENGTH = 64K   sram(rwx): ORIGIN = 0x200000c0, LENGTH = 8K-0xc0 }*/
MEMORY {   flash(rx): ORIGIN = 0x8000800, LENGTH = 32K   sram(rwx): ORIGIN = 0x200000c0, LENGTH = 4k-0xc0 }
sram_top = ORIGIN(sram) + LENGTH(sram);
ENTRY(boot_entry)

SECTIONS
{
  .text :
  {
    KEEP(*(.irq_vectors))
    *(.text*)
    *(.rodata*)
*(.ARM.extab* .gnu.linkonce.armextab.*)
        *(.gcc_except_table)
        *(.eh_frame_hdr)
        *(.eh_frame)

        . = ALIGN(4);
        KEEP(*(.init))

        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;

        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;

        . = ALIGN(0x4);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))

        . = ALIGN(4);
        KEEP(*(.fini))

        . = ALIGN(4);
        __fini_array_start = .;
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        __fini_array_end = .;

        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))

  } > flash

  /*
   * More information about Special Section Indexes is available in the
   * free "ELF for the ARM Architecture" document from ARM Limited
   * http://simplemachines.it/doc/aaelf.pdf
   *
   */
  .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } > flash
  __exidx_start = .;
  .ARM.exidx : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) } > flash
  __exidx_end = .;

  _etext = .;
    
  .data : AT (__exidx_end)
  {
    _data = .;
    *(vtable)
    *(.data*)
    _edata = .;
  } > sram

  /* zero initialized data */
  .bss :
  {
    _bss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > sram
  
  end = .;

  /* For GDB compatibility we decrease the top with 16 bytes */
  stack_entry = sram_top - 128;
}