/* Default linker script, for normal executables */
OUTPUT_FORMAT("elf32-ft32")
OUTPUT_ARCH(ft32)
SEARCH_DIR("/opt/ft32/ft32-elf/lib");
/* Allow the command line to override the memory region sizes.  */
__PMSIZE = DEFINED(__PMSIZE)  ? __PMSIZE : DEFINED(__FT930__)? 128K : 256K;
__RAMSIZE = DEFINED(__RAMSIZE) ? __RAMSIZE : DEFINED(__FT930__)? 32K : 64K;
/* Allocate space reserved for bootloader */
__PROGMAX = __PMSIZE - 4K;
MEMORY
{
  flash     (rx)     : ORIGIN = 0,         LENGTH = __PROGMAX
  bootflash (rx)     : ORIGIN = __PROGMAX, LENGTH = 4K
  ram       (rw!x)   : ORIGIN = 0x800000,  LENGTH = __RAMSIZE
  peripherals (rw!x) : ORIGIN = 0x810000,  LENGTH = 0x1000
  ehci      (rw!x)   : ORIGIN = 0x811000,  LENGTH = 0x2000
}

SECTIONS
{
  .text :
  {
    *(.crt0*)
    *(.text*)
    *(.strings)
    *(._pm*)
    *(.init)
    *(.fini)
     _etext = . ;
    . = ALIGN(4);
    	
  }  > flash
  
  .tors :
  {
    ___ctors = . ;
    *(.ctors)
    ___ctors_end = . ;
    ___dtors = . ;
    *(.dtors)
    ___dtors_end = . ;
    . = ALIGN(4);
  } > ram
  
  .data	  : AT (ADDR (.text) + SIZEOF (.text))
  {
  _sdata = .;
    *(.data)
    *(.rodata)
    *(.rodata*)
     _edata = . ;
  }  > ram
  


  .configs LOADADDR(.text) + SIZEOF(.text) + SIZEOF (.data) : AT (ADDR (.text) + SIZEOF (.text) + SIZEOF(.data))
  {
  
  	FILL(0xFF); /* Pad unused regions with 0xFF */
  	
  	/* If there's no SDBL/D2XX/LOGGER we don't update the location counter */
	. += (__sdbl_partition_sizeof ?  
			(__D2XX_partition_sizeof ? 
				(__dlog_partition_sizeof ? 
					LENGTH(flash)-0x4000 : /* All optional partitions used */ 
					LENGTH(flash)-0x3000   /* SDBL and D2XX partitions used */
				) : 
				(__dlog_partition_sizeof ? 
					LENGTH(flash)-0x3000 : /* DLOG and SDBL partitions used */
					LENGTH(flash)-0x2000   /* SDBL partition used */
				)
			) : 
				(__D2XX_partition_sizeof ?
					(__dlog_partition_sizeof ?  
						LENGTH(flash)-0x2000 : /* DLOG and D2XX partitions used */
						LENGTH(flash)-0x1000   /* D2XX partition used */
					) : 
					(__dlog_partition_sizeof ? 
						LENGTH(flash)-0x1000 : /* DLOG partition used */
						SIZEOF(.text) + SIZEOF (.data) /* No partitions used */
					)
				)
			) - SIZEOF(.text) - SIZEOF (.data);
							
	*(._flash_dlog_partition)
	*(._flash_d2xx_config)
	*(._flash_sdbl_partition)
    . = ALIGN(4);
	
  }  > flash

  .bss  SIZEOF(.data) + ADDR(.data) (NOLOAD) :
  {
     _bss_start = . ;
    *(.bss)
    *(COMMON)
     _end = . ;
  }  > ram
   __data_load_start = LOADADDR(.data);
   __data_load_end = __data_load_start + SIZEOF(.data);
   
  /* Peripheral Register Blocks from 0x810000 to 0x810FFF */
  .peripheralsBlock (NOLOAD):
  {
    KEEP(*(SORT_NONE(.peripheralsSection)));
  } > peripherals
  /* EHCI RAM - Accessed via aligned 32 bit reads and writes only. */
  .ehciBlock (NOLOAD):
  {
    KEEP(*(.ehciSection))
  } > ehci

  .stab 0 (NOLOAD) :
  {
    *(.stab)
  }
  .stabstr 0 (NOLOAD) :
  {
    *(.stabstr)
  }
}
