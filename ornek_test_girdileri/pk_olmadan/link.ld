OUTPUT_ARCH( "riscv" )
ENTRY(start)

SECTIONS
{
  . = 0x80000000;
  .text : { *(.text) }
  .text2 : { *(.text2) }
  . = ALIGN(0x1000);
  .data : { *(.data) }
  .bss : { *(.bss) }
  _end = .;
  . = ALIGN(0x1000);
  . = ALIGN(0x1000);
  _stack_end = .;
}
