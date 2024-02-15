
.global _start
.type   _start, @function

_start:
  # stack pointer'i ayarla
  # main'i cagir
  nop
  nop
  nop
  la sp, _stack_end
  # call fptest
  call main
infinity_loop:
  j infinity_loop


.type fptest, @function

# fptest:
#   addi sp, sp, -8     # Adjust stack pointer for 8 bytes, making room for one 64-bit register
#   fsw f0, 4(sp)      # Save fs0 if it's going to be used or modified

#   li t0, 2            # Load immediate value 2 into t0
#   fcvt.s.w f0, t0     # Convert t0 from integer to single-precision float in f0
#   fsqrt.s f1, f0      # Square root of f0, result in f1

#   flw f0, 4(sp)      # Restore fs0
#   addi sp, sp, 8      # Adjust the stack pointer back
#   ret                 # Return from fptest
