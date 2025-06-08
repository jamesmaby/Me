OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")

/* Internal Memory Map */
MEMORY
{
	FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 128K
	RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 16K
}

_eram = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
	.text :
	{
		KEEP(*(.isr_vector))
		*(.text*)

		KEEP(*(.init))
		KEEP(*(.fini))

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)

		/* .dtors */
		*crtbegin.o(.dtors)
		*crtbegin?.o(.dtors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
		*(SORT(.dtors.*))
		*(.dtors)

		*(.rodata*)

		KEEP(*(.eh_frame*))
	} > FLASH

	.console_cmd_list : {
		__start_console_cmd_list = .;
		KEEP(*(SORT(.console_cmd_list*)));
		__end_console_cmd_list = .;
	} > FLASH

	.cg_elt_list : {
		KEEP(*(SORT(.cg_elt_list*)));
	} > FLASH

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > FLASH

	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > FLASH

	. = ALIGN(4);

	__exidx_end = .;
	__etext = .;

	/* _sidata is used in startup code */
	_sidata = __etext;

	.data : AT (__etext)
	{
		__data_start__ = .;
		_sdata = __data_start__;

		*(vtable)
		*(.data*)

		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		/* init data */
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);

		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		KEEP(*(.jcr*))
		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;
		_edata = __data_end__;
	} > RAM

	.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		_sbss = __bss_start__;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
		_ebss = __bss_end__;
	} > RAM

	.heap (COPY):
	{
		__end__ = .;
		_end = __end__;
		end = __end__;
		*(.heap*)
		__HeapLimit = .;
	} > RAM

	.co_stack (NOLOAD):
	{
		. = ALIGN(8);
		*(.co_stack .co_stack.*)
	} > RAM

	.me_firm_version __etext + SIZEOF(.data) :
	{
		. = ALIGN(4);
		KEEP(*(.me_firm_version*))
	} > FLASH

	/* Set stack top to end of RAM, and stack limit move down by size of .co_stack */
	__StackTop = ORIGIN(RAM) + LENGTH(RAM);
	__StackLimit = __StackTop - SIZEOF(.co_stack);
	PROVIDE(__stack = __StackTop);

	/* Check if data + heap + stack exceeds RAM limit */
	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
}
