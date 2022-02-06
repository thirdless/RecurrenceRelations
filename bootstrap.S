.globl _start
.globl main

.equ SYSCALL_ADDR, 0x02010000

# NOTE: this will exit the whole simulation, i.e. stop all harts
.macro SYS_EXIT, exit_code
li   a7, 93
li   a0, \exit_code
li   t0, SYSCALL_ADDR
csrr a6, mhartid
sw   a6, 0(t0)
.endm

trap_handler:
	li   t0, SYSCALL_ADDR
	csrr t1, mhartid
	sw   t1, 0(t0)
	csrr t0, mepc
	addi t0, t0, 4
	csrw mepc, t0
	mret

# NOTE: each core will start here with execution
_start:

# initialize global pointer (see crt0.S of the RISC-V newlib C-library port)
.option push
.option norelax
1:auipc gp, %pcrel_hi(__global_pointer$)
  addi  gp, gp, %pcrel_lo(1b)
.option pop

csrr a0, mhartid   # return a core specific number 0 or 1
li t0, 0
beq a0, t0, core0

li t0, 1
beq a0, t0, core1

li t0, 2
beq a0, t0, core2

li t0, 3
beq a0, t0, core3

li t0, 4
beq a0, t0, core4

li t0, 5
beq a0, t0, core5

li t0, 6
beq a0, t0, core6

li t0, 7
beq a0, t0, core7

li t0, 8
beq a0, t0, core8

li t0, 9
beq a0, t0, core9

li t0, 10
beq a0, t0, core10

li t0, 11
beq a0, t0, core11

li t0, 12
beq a0, t0, core12

li t0, 13
beq a0, t0, core13

li t0, 14
beq a0, t0, core14

li t0, 15
beq a0, t0, core15

core0:
la sp, stack0_end  # code executed only by core0
j end

core1:
la sp, stack1_end  # code executed only by core1
j end

core2:
la sp, stack2_end  # code executed only by core1
j end

core3:
la sp, stack3_end  # code executed only by core1
j end

core4:
la sp, stack4_end  # code executed only by core1
j end

core5:
la sp, stack5_end  # code executed only by core1
j end

core6:
la sp, stack6_end  # code executed only by core1
j end

core7:
la sp, stack7_end  # code executed only by core1
j end

core8:
la sp, stack8_end  # code executed only by core1
j end

core9:
la sp, stack9_end  # code executed only by core1
j end

core10:
la sp, stack10_end  # code executed only by core1
j end

core11:
la sp, stack11_end  # code executed only by core1
j end

core12:
la sp, stack12_end  # code executed only by core1
j end

core13:
la sp, stack13_end  # code executed only by core1
j end

core14:
la sp, stack14_end  # code executed only by core1
j end

core15:
la sp, stack15_end  # code executed only by core1
j end

end:

la t0, trap_handler
csrw mtvec, t0

csrr a0, mhartid 
jal main

# wait until all cores have finished
la t0, exit_counter
li t1, 1
li t2, 15

amoadd.w a0, t1, 0(t0)

1:
blt a0, t2, 1b

# call exit (SYS_EXIT=93) with exit code 0 (argument in a0)
SYS_EXIT 0

.align 8
stack0_begin:
.zero 32768
stack0_end:

.align 8
stack1_begin:
.zero 32768
stack1_end:

.align 8
stack2_begin:
.zero 32768
stack2_end:

.align 8
stack3_begin:
.zero 32768
stack3_end:

.align 8
stack4_begin:
.zero 32768
stack4_end:

.align 8
stack5_begin:
.zero 32768
stack5_end:

.align 8
stack6_begin:
.zero 32768
stack6_end:

.align 8
stack7_begin:
.zero 32768
stack7_end:

.align 8
stack8_begin:
.zero 32768
stack8_end:

.align 8
stack9_begin:
.zero 32768
stack9_end:

.align 8
stack10_begin:
.zero 32768
stack10_end:

.align 8
stack11_begin:
.zero 32768
stack11_end:

.align 8
stack12_begin:
.zero 32768
stack12_end:

.align 8
stack13_begin:
.zero 32768
stack13_end:

.align 8
stack14_begin:
.zero 32768
stack14_end:

.align 8
stack15_begin:
.zero 32768
stack15_end:

exit_counter:
.word 0
