MEMORY {
  SLOT_MEM(rwx) : ORIGIN = 0x00000000, LENGTH = 8k
}

PHDRS
{
  text PT_LOAD ;
}

SECTIONS{
	.header : ALIGN(4)
	{
		KEEP(*(.header))
	} > SLOT_MEM : text
	
	.text : ALIGN(4)
	{
		*(.text)
		*(.plt)
		. = ALIGN(4);
	} > SLOT_MEM
	
	.rel.dyn : ALIGN(4)
	{
		__rel = .;
		*(.rel*)
		*(.rela*)
		__erel = .;
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
    
	.dynsym : ALIGN(4)
	{
		__dynsym = .;
		*(.dynsym)
	}
	
	.dynstr : ALIGN(4)
	{
		__dynstr = .;
		*(.dynstr)
	}

	/DISCARD/ : { 
        *(.dynamic) *(.gnu.hash) 
        *(.ARM.attributes) *(.comment)
    }
}
