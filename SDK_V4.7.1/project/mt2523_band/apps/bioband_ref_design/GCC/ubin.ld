/********************************************************  
memory map 
  Start Address	Size	CM4 view Devices
  	0x0000_0000	64MB	EMI	
	0x0400_0000	32KB	CM4 TCM / cache	
	0x0400_8000	128KB	CM4 TCM	
	0x0402_8000	40KB	Reserved	
	0x0403_2000	824KB	Reserved
	0x0410_0000	1MB	Boot ROM CM4
	0x0420_0000	62MB	Reserved
	0x0800_0000	128MB	SFC0
 	0x1000_0000	128MB	SW virtual memory management
	0x1800_0000	128MB	SW virtual memory management
*********************************************************/
 
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
 
/* Memory Spaces Definitions */
MEMORY
{
    ROM_BL(rx)            : ORIGIN = 0x08000000, LENGTH = 64K
    ROM_RTOS(rx)          : ORIGIN = 0x08010000, LENGTH = 2304K
    ROM_UBIN(rx)          : ORIGIN = 0x08250000, LENGTH = 800K 
    ROM_FOTA_RESERVED(rx) : ORIGIN = 0x08318000, LENGTH = 800K
    ROM_NVDM_RESERVED(rx) : ORIGIN = 0x083E0000, LENGTH = 64K
    ROM_EPO_RESERVED(rx)  : ORIGIN = 0x083F0000, LENGTH = 64K
    RAM (rwx)             : ORIGIN = 0x00000000, LENGTH = 3696K    
    VRAM(rwx)             : ORIGIN = 0x10000000, LENGTH = 3696K   
    RAM_UBIN(rwx)         : ORIGIN = 0x1039C000, LENGTH = 400K
    TCM (rwx)             : ORIGIN = 0x04008000, LENGTH = 128K
}


SECTIONS
{
    . = ORIGIN(ROM_UBIN);
    
    .ubin_table :
    {
        _ubin_table_start = .;
        
        KEEP(*(.ubin_table)) /* ubin function table */
        
        _ubin_table_end = .;
        . = ALIGN(4);
    } > ROM_UBIN AT> ROM_UBIN
    
    . = ALIGN(4);
    
    .text  :
    {
        _ubin_text_start = .;
        *(.text*)
        *(.rodata*)
        
        _ubin_text_end = .;
        
    } > ROM_UBIN  AT> ROM_UBIN

    . = ALIGN(4);
    _ubin_data_load = LOADADDR(.data);
    
    .data :
    {
        _ubin_data_start = .;
        
        *(.data)
        *(.data*)
        
        _ubin_data_end = .;
        
    } > RAM_UBIN  AT> ROM_UBIN
    
    . = ALIGN(4);
    
    .bss :
    {
        _ubin_bss_start = .;
        *(.bss*)
        *(COMMON)
        
        _ubin_bss_end = .;
        
    } > RAM_UBIN  AT> ROM_UBIN
}
