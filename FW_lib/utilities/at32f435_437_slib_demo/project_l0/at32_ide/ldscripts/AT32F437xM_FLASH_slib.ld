/*
*****************************************************************************
**
**  File        : AT32F437xM_FLASH.ld
**
**  Abstract    : Linker script for AT32F437xM Device with
**                4096KByte FLASH, 384KByte RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : Artery Tek AT32
**
**  Environment : Arm gcc toolchain
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x20060000;    /* end of RAM */

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
FLASH_1 (rx)       : ORIGIN = 0x08000000, LENGTH = 120K
SLIB_READ_ONLY (x) : ORIGIN = 0x0801E000, LENGTH = 4K
SLIB_INST (r)      : ORIGIN = 0x0801F000, LENGTH = 4K
FLASH_2 (rx)       : ORIGIN = 0x08020000, LENGTH = 3904K
SLIB_RAM (xrw)     : ORIGIN = 0x20000000, LENGTH = 4K  /* used for SLIB code */
RAM (xrw)          : ORIGIN = 0x20001000, LENGTH = 380K
}

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH_1

  .slib_inst :
  {
    . = ALIGN(4);
    *fir_filter.o (.text .text*);
    *arm_fir_f32.o (.text .text*);
    *arm_fir_init_f32.o (.text .text*);
    . = ALIGN(4);
  } > SLIB_INST  
 
  .slib_read_only :        /* SLIB_READ_ONLY arae */
  {
    . = ALIGN(4);
    *fir_coefficient.o (.rodata .rodata*);
    . = ALIGN(4);
  } > SLIB_READ_ONLY 
  
  .slib_ram :          /* Used for SLIB */
  {
    . = ALIGN(4);  
    *fir_filter.o (.data .data*);
    *fir_filter.o (.bss .bss*);   
    . = ALIGN(4);         
  } > SLIB_RAM

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(EXCLUDE_FILE (*arm_fir_f32.o *arm_fir_init_f32.o *fir_filter.o) .text .text*)    
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH_1

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    *(EXCLUDE_FILE (*arm_fir_f32.o *arm_fir_init_f32.o *fir_filter.o) .rodata .rodata*)        
    . = ALIGN(4);
  } >FLASH_1

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH_2
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH_2

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH_2
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH_2
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH_2

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM AT> FLASH_1

  .data_2 : 
  {
    . = ALIGN(4);
    _sdata_2 = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM AT> FLASH_2  

  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
