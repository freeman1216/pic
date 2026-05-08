MEMORY {
  ROM(rx) : ORIGIN = 0x08000000, LENGTH = 1024k
  RAM(rwx) : ORIGIN = 0x20000000, LENGTH = 640k
}

__estack     = ORIGIN(RAM) + LENGTH(RAM);       /* stack points to end of SRAM */

SECTIONS {
    .text : ALIGN(4)
    {   KEEP(*(.ivt))
        *(.text)
        . = ALIGN(4);
        __etext = .;
    } > ROM
	.slot : ALIGN(4)
	{
		__slot = .;
		*(.slot)
	}
    __rdata = LOADADDR(.data); /* data placed in rom */

    .data : ALIGN(4)
    {   
        __data = .;
        *(.data)
        . = ALIGN(4);
        __edata = .;
    } > RAM AT > ROM
    
    .bss : ALIGN(4)
    {
        __bss = .;
        *(.bss)
        . = ALIGN(32);
        __ebss = .;
    } > RAM 
    

    __rramfunc = LOADADDR(.ramfunc); /* data placed in rom */

    .ramfunc :ALIGN(4)
    {   
        __ramfunc = .;
        *(.ramfunc)
        *(.text.memmove)
        . = ALIGN(4);
        __eramfunc = .;
        
    } >RAM AT > ROM
   
    .init_array :ALIGN(4)
    {  
        __init_array = .;
       *(.init_array)
       . = ALIGN(4);
        __einit_array = .;
    } > ROM

    .rodata : ALIGN(4)
    {
        *(.rodata)
        . = ALIGN(4);
        __erodata = .;
    } > ROM

} 
