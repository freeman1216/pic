MEMORY {
  SLOT_MEM(rwx) : ORIGIN = 0x00000000, LENGTH = 8k
}

SECTIONS{
	.text : ALIGN(8)
	{
		KEEP(*(.header))
		*(.text)
		. = ALIGN(4);
	} > SLOT_MEM
}
