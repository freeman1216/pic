MEMORY {
  SLOT_MEM(rwx) : ORIGIN = 0x00000000, LENGTH = 8k
}

SECTIONS{
	.text : ALIGN(4)
	{
		KEEP(*(.header))
		*(.text)
		. = ALIGN(4);
	} > SLOT_MEM
	
	.got : ALIGN(4)
	{
		__got = .;
		*(.got*)
		. = ALIGN(4);
		__got_end = .;
	} > SLOT_MEM
	
	.data : ALIGN(4)
    {   
        __data = .;
        *(.data)
        . = ALIGN(4);
        __edata = .;
    } > SLOT_MEM
    
    .bss : ALIGN(4)
    {
        __bss = .;
        *(.bss)
        __ebss = .;
    } > SLOT_MEM 
    
}
