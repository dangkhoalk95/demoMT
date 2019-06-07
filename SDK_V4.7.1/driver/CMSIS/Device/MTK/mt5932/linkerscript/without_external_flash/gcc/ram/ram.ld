/********************************************************
memory map
  Start Address  Size  CM4 view Devices
  0x0000_0000    64MB  EMI
  0x0400_0000    32KB  CM4 TCM / cache
  0x0400_8000    64KB  CM4 TCM
  0x0401_8000    32KB  Reserved
  0x0402_0000   896KB  Reserved
  0x0410_0000     1MB  Boot ROM CM4
  0x0420_0000   384KB  SYSRAM
  0x0430_0000     8KB  Retention SRAM
  0x0440_0000     1MB  WIFI ROM
  0x0800_0000   128MB  SFC0
  0x1000_0000   128MB  SW virtual memory management
  0x1800_0000   128MB  SW virtual memory management
*********************************************************/
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

/* Memory Spaces Definitions: 384K SYSRAM, 96K TCM */
MEMORY
{
    TCM (rwx)             : ORIGIN = 0x04000000, LENGTH = 96K
    SYSRAM (rwx)          : ORIGIN = 0x04200000, LENGTH = 384K
    SYSRAM_ALIAS (rwx)    : ORIGIN = 0x04200000, LENGTH = 384K
    BOSON_DATA (arw)      : ORIGIN = 0x04259c00, LENGTH = 0x000019D6 /*  6k , __iot_data_end = 0x0425b5d6 */
    BOSON_BSS (arw)       : ORIGIN = 0x0425b5d8, LENGTH = 0x00004530 /* 17k , __rom_bss_end__  = 0x0425fcc8 */
    VSYSRAM (rwx)         : ORIGIN = 0x14200000, LENGTH = 384K
}

 /* Highest address of the stack */
_stack_end = ORIGIN(TCM) + LENGTH(TCM);    /* end of TCM */

/* Generate a link error if stack don't fit into TCM */
_stack_size = 0x1000; /* required amount of stack 4KB*/

/* stack start */
_stack_start = _stack_end - _stack_size;


/* Entry Point */
ENTRY(Reset_Handler)

SECTIONS
{
    . = ORIGIN(SYSRAM);

    .text : ALIGN(4)
    {
        _text_start                         = .;
        Image$$TEXT$$Base                   = .;

        KEEP(*(.reset_handler))
        KEEP(*(.init))
        KEEP(*(.fini))
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);

        Image$$TEXT$$Limit                  = .;
        _text_end                           = .;

    } > SYSRAM_ALIAS  AT> SYSRAM

    . = ALIGN(4);

    _sysram_code_load = LOADADDR(.sysram_text);

    .sysram_text : ALIGN(4)
    {
        Image$$CACHED_SYSRAM_TEXT$$Base     = .;
        _sysram_code_start                  = .;




        Image$$CACHED_SYSRAM_TEXT$$Limit = .;
        _sysram_code_end = .;
    } > SYSRAM_ALIAS   AT> SYSRAM
  
    . = ALIGN(4);

    _cached_sysram_data_load = LOADADDR(.cached_sysram_data);
    .cached_sysram_data . :
    {
        _cached_sysram_data_start           = .;
        Image$$CACHED_SYSRAM_DATA$$RW$$Base = .;

        *(.data)
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE (__preinit_array_start = .);
        KEEP    (*(.preinit_array))
        PROVIDE (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE (__init_array_start = .);
        KEEP    (*(SORT(.init_array.*)))
        KEEP    (*(.init_array))
        PROVIDE (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE (__fini_array_start = .);
        KEEP    (*(SORT(.fini_array.*)))
        KEEP    (*(.fini_array))
        PROVIDE (__fini_array_end = .);

        _cached_sysram_data_end             = .;
        Image$$CACHED_SYSRAM_DATA$$RW$$Limit = .;

    } > SYSRAM_ALIAS  AT> SYSRAM

    . = ALIGN(4);
    .cached_sysram_bss :
    {
        _cached_sysram_bss_start            = .;
        Image$$CACHED_SYSRAM_DATA$$ZI$$Base = .;

        *(.bss)
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _cached_sysram_bss_end              = .;
        Image$$CACHED_SYSRAM_DATA$$ZI$$Limit = .;

    } > SYSRAM_ALIAS  AT> SYSRAM

    . = ALIGN(4);
    _noncached_sysram_rw_load = LOADADDR(.noncached_sysram_data);
    .noncached_sysram_data :
    {
        Image$$NONCACHED_SYSRAM_DATA$$Base = .;
        _noncached_sysram_rw_start = .;

        Image$$NONCACHED_SYSRAM_DATA$$Limit = .;
        _noncached_sysram_rw_end            = .;
    } > SYSRAM_ALIAS  AT> SYSRAM

    . = ALIGN(4);
    .noncached_sysram_bss (NOLOAD) :
    {
        Image$$NONCACHED_SYSRAM_ZI$$Base    = .;
        _noncached_sysram_zi_start          = .;

        Image$$NONCACHED_SYSRAM_ZI$$Limit   = .;
        _noncached_sysram_zi_end            = .;
     
    } > SYSRAM_ALIAS  AT> SYSRAM

       . = ALIGN(4);

    __boson_data_lmastart = LOADADDR(.boson);

    .boson : ALIGN(4)
    {
        KEEP(*(.boson_data))
    } > BOSON_DATA AT> BOSON_DATA

   . = ALIGN(4);

    _tcm_text_load = LOADADDR(.tcm);

    .tcm :
    {
        _tcm_text_start                     = .;
        Image$$TCM$$RO$$Base                = .;
        Image$$VECTOR$$TABLE$$Base          = .;

        KEEP(*(.isr_vector))
        *(.tcm_code)
        *(.tcm_rodata)

        Image$$TCM$$RO$$Limit               = .;
        Image$$TCM$$RW$$Base                = .;

        *(.tcm_rwdata)

        . = ALIGN(4);

        _tcm_text_end                       = .;
        Image$$TCM$$RW$$Limit               = .;
    } > TCM  AT> SYSRAM

    . = ALIGN(4);

    .tcm_bss (NOLOAD) :
    {
        _tcm_zi_start                       = .;
        Image$$TCM$$ZI$$Base                = .;

        *(.tcm_zidata)
         
        . = ALIGN(4);
        _tcm_zi_end                         = .;
        Image$$TCM$$ZI$$Limit               = .;
    } > TCM  AT> SYSRAM

    /* for checking if the stack exceeds the total TCM size */
 
    .stack (NOLOAD):
    {
        . = ALIGN(4);

        PROVIDE (  end = . );
        PROVIDE ( _end = . );

        . = . + _stack_size;
        . = ALIGN(4);
    } > TCM

      . = ALIGN(4);
      __code_slim_tcm_lmastart = LOADADDR(.code_slim_tcm);

    .code_slim_tcm : ALIGN(4)
    {
        __code_slim_tcm_start               = .;

        KEEP(*(.code_slim_tcm))

        __code_slim_tcm_end                 = .;
    } > TCM  AT> SYSRAM

    . = ALIGN(4);

    __n9_fw_lma_start = LOADADDR(.n9_fw);

    .n9_fw :
    {
        *(.n9fw*)
         _n9_fw_end = .;
    } > SYSRAM  AT> SYSRAM

   

    ASSERT(_n9_fw_end <= ORIGIN(BOSON_DATA), "N9 FW corrupt boson data")
    ASSERT(_cached_sysram_bss_end    <= ORIGIN(BOSON_DATA), "bss  corrupt boson data")

    Image$$STACK$$ZI$$Base  = _stack_end - _stack_size;
    Image$$STACK$$ZI$$Limit = _stack_end;
}
