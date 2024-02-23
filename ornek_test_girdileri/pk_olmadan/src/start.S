
.global _start
.type   _start, @function

_start:
  # stack pointer'i ayarla
  # main'i cagir
  nop
  nop
  nop
  la sp, _stack_end
  li t1, 0x6000 
  csrrs x0, mstatus, t1
  fsw f0, 4(sp)
  call main

infinity_loop:
  j infinity_loop
    