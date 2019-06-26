/********************************************************  
memory map 
  Start Address	Size	CM4 view Devices	
	0x0400_0000	32KB	CM4 TCM / cache	
	0x0400_8000	64KB	CM4 TCM	
	0x0401_8000	32KB	Reserved	
	0x0402_0000	896KB	Reserved	
	0x0410_0000	1MB	Boot ROM CM4
	0x0420_0000	384KB	SYSRAM
	0x0430_0000	8KB	Retention SRAM
	0x0440_0000	1MB	WIFI ROM
	0x0800_0000	128MB	SFC0
 	0x1000_0000	128MB	SW virtual memory management
	0x1800_0000	128MB	SW virtual memory management
*********************************************************/
 
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
 
/* Memory Spaces Definitions: 4M FLASH,4M PSRAM*/
MEMORY
{
    ROM_HEAD_1(rx)        : ORIGIN = 0x08000000, LENGTH = 4K
    ROM_HEAD_2(rx)        : ORIGIN = 0x08001000, LENGTH = 4K
    ROM_BL(rx)            : ORIGIN = 0x08002000, LENGTH = 64K
    ROM_RTOS(rx)          : ORIGIN = 0x08012000, LENGTH = 3864K
    ROM_N9(rx)            : ORIGIN = 0x083D8000, LENGTH = 96K
    ROM_NVDM_RESERVED(rx) : ORIGIN = 0x083F0000, LENGTH = 64K  
    TCM (rwx)             : ORIGIN = 0x04008000, LENGTH = 64K
    SYSRAM (rwx)          : ORIGIN = 0x04200000, LENGTH = 384K
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
    .text :
    {
        _text_start = .;
        Image$$TEXT$$Base = .;
        
        KEEP(*(.reset_handler))
        KEEP(*(.init))
        KEEP(*(.fini))
        
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        
         . = ALIGN(4);
      __boson_data_lmastart = .;
        KEEP(*(.boson_data))
        . = ALIGN(4);
        Image$$TEXT$$Limit = .;
        _text_end = .;
        
    } > SYSRAM  AT> SYSRAM
   
   
    . = ALIGN(4);
    _tcm_text_load = LOADADDR(.tcm);

    .tcm :
    {
        
        _tcm_text_start = .;
        Image$$TCM$$RO$$Base = .;
        Image$$VECTOR$$TABLE$$Base = .;
         KEEP(*(.isr_vector))
        *(.tcm_code)
        *(.tcm_rodata)
        Image$$TCM$$RO$$Limit = .;
        Image$$TCM$$RW$$Base = .;
        
        *(.tcm_rwdata)
        . = ALIGN(4);
        Image$$TCM$$RW$$Limit = .;
        _tcm_text_end = .;
    }> TCM  AT> TCM
    . = ALIGN(4);
    .tcm_bss (NOLOAD) :
    {
        _tcm_zi_start = .;
        Image$$TCM$$ZI$$Base = .;
        
        *(.tcm_zidata)
        
        _tcm_zi_end = .;
        Image$$TCM$$ZI$$Limit = .;
        
        
    }> TCM AT> TCM  
    
    /* use to check if the stack exceeds the total TCM size*/
    .stack :
    {
        . = ALIGN(4);
        
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _stack_size;
        . = ALIGN(4);
        
    } > TCM

     . = ALIGN(32);
    _sysram_code_load = LOADADDR(.sysram_text);
    
    .sysram_text :
    {
        Image$$SYSRAM_TEXT$$Base = .;
       __sysram_code_start__ = .;
        _sysram_code_start = .;
        

        *(.sysram_code)
        *(.sysram_rodata)
        
        
        Image$$SYSRAM_TEXT$$Limit = .;
         __sysram_code_end__ = .;
        _sysram_code_end = .;
    } > SYSRAM   AT> SYSRAM
  
    /* 2-2.cacheable sysram RW data*/   
    . = ALIGN(32);
    
     _cached_sysram_data_load = LOADADDR(.cached_sysram_data);
     .cached_sysram_data . :
    {       
        __cached_sysram_data_start__ = .;
        _cached_sysram_data_start = .;
        Image$$CACHED_SYSRAM_DATA$$RW$$Base = .;

        *(.data)
        *(.data*)
        
        . = ALIGN(4);
        /* preinit data */
        PROVIDE (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE (__init_array_end = .);
        
        . = ALIGN(4);
        /* finit data */
        PROVIDE (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE (__fini_array_end = .);

        _cached_sysram_data_end = .;
        __cached_sysram_data_end__ = .;
       
       
       Image$$CACHED_SYSRAM_DATA$$RW$$Limit = .;
    
        
    } > SYSRAM  AT> SYSRAM
    
    /* 2-3.cacheable sysram ZI data*/
    
    . = ALIGN(32);
    .cached_sysram_bss (NOLOAD): 
    {
        _cached_sysram_bss_start = .;
        __cached_sysram_bss_start__ = .;
        Image$$CACHED_SYSRAM_DATA$$ZI$$Base = .;
        
        *(.bss)
        *(.bss*)
        *(COMMON)
        
        . = ALIGN(4);
        _cached_sysram_bss_end = .;
        __cached_sysram_bss_end__ = .;
        Image$$CACHED_SYSRAM_DATA$$ZI$$Limit = .;
        
    } > SYSRAM  AT> SYSRAM
    
     /* 2-4.noncacheable sysram RW data*/

       . = ALIGN(4);
       _noncached_sysram_rw_load = LOADADDR(.noncached_sysram_data);
      . -= ORIGIN(VSYSRAM);
       _sysram_noncached_start_addr = .;
    .noncached_sysram_data :
    {
        . = ALIGN(4);
        Image$$NONCACHED_SYSRAM_DATA$$Base = .;
        _noncached_sysram_rw_start = .;
        __noncached_sysram_rw_start__ = .;
        *(.noncached_sysram_rwdata)
        
        
        Image$$NONCACHED_SYSRAM_DATA$$Limit = .;
        _noncached_sysram_rw_end = .; 
        __noncached_sysram_rw_end__ = .;
    } > SYSRAM  AT> SYSRAM
    
    /* 2-5.noncacheable sysram ZI data*/ 
    
    . = ALIGN(4);
    .noncached_sysram_bss (NOLOAD) :
    {                
        Image$$NONCACHED_SYSRAM_ZI$$Base = .;
        _noncached_sysram_zi_start = .;
        __noncached_sysram_zi_start__ = .;
        *(.noncached_sysram_zidata)
        
        
        Image$$NONCACHED_SYSRAM_ZI$$Limit = .;
        _noncached_sysram_zi_end = .;
       __noncached_sysram_zi_end__ = .;
    } > SYSRAM  AT> SYSRAM
    
    ASSERT(_noncached_sysram_zi_end <= (ORIGIN(SYSRAM) + LENGTH(SYSRAM)),"SYSRAM expired")
  
  
    Image$$STACK$$ZI$$Base = _stack_end - _stack_size;
    Image$$STACK$$ZI$$Limit = _stack_end;
    
    /*extern linker symbol for region init */
    Image$$FLASH_CODE$$ZI$$Base = ORIGIN(ROM_HEAD_1);
    Image$$FLASH_CODE$$ZI$$Limit = ORIGIN(ROM_HEAD_1) + LENGTH(ROM_HEAD_1) +LENGTH(ROM_HEAD_2) +LENGTH(ROM_BL) + LENGTH(ROM_RTOS);
    Image$$FLASH_CODE$$ZI$$Length = LENGTH(ROM_HEAD_1) +LENGTH(ROM_HEAD_2) +LENGTH(ROM_BL) + LENGTH(ROM_RTOS);
    Load$$TCM$$Base = LOADADDR(.tcm);
    Load$$SYSRAM_TEXT$$Base = LOADADDR(.sysram_text);
    Load$$CACHED_SYSRAM_DATA$$Base = LOADADDR(.cached_sysram_data);
    Load$$NONCACHED_SYSRAM_DATA$$Base = LOADADDR(.noncached_sysram_data);
    
    /* provide nvdm start and length for NVDM management */
    Image$$NVDM$$ZI$$Base = ORIGIN(ROM_NVDM_RESERVED);
    Image$$NVDM$$ZI$$Length = LENGTH(ROM_NVDM_RESERVED);
    
    
}
