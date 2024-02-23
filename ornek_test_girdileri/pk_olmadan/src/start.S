
.global _start
.type   _start, @function

_start:
  # stack pointer'i ayarla
  # main'i cagir
  nop
  nop
  nop
  # ofsett 15, value 1 (ebreakm). ofset 6 of width 3, value 1 (cause). ofset 0 of width 2, value 3 (prv). 
  # li t1, 0x8103
  # li t1, 0x8043
  # csrr a0, dcsr
  # clear the neccessary bits of the t2
  # csrrs x0, dcsr, t1
  # dret
  # ebreak
  # li t0, 0x12345678
  # slli t0, t0, 32
  # li t1, 0x9abcdef0
  # or t0, t0, t1
  # slli t0, t0, 64 # assembler message: improper shamt
  la sp, _stack_end
  li t1, 0x6000 
  csrrs x0, mstatus, t1
  fsw f0, 4(sp)
  call main

infinity_loop:
  j infinity_loop
    