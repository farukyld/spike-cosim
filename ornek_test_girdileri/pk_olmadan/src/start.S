

.global start
.type   start, @function

nop
start:
  # stack pointer'i ayarla
  # main'i cagir
  # li t0, -1
  # csrrs x0, mhpmcounter3, t0 
  # vl, vtype, hip, fcsr, vstart, vxrm, vcsr, seed
  # illegal instruction
  # csrr t0, vl
  # csrr t0, vtype
  # csrr t0, hip
  # csrr t0, fcsr
  # csrr t0, vstart
  # csrr t0, vxrm
  # csrr t0, vcsr
  # csrr t0, seed

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
  fcvt.s.w f0, t1
  fsd f0, 8(sp)
  # la t1, 0x100
  # li t0, 0
  # sw t0, 0(t1)
  call main

.align 4

infinity_loop:
  j infinity_loop

.section .data

.align 14

var:
  .word 1234
