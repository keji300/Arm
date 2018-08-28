.text
.global _start
_start:
		b		reset
		ldr		pc,_undefined_instruction
		ldr		pc,_software_interrupt
		ldr		pc,_prefetch_abort
		ldr		pc,_data_abort
		ldr		pc,_not_used
		ldr		pc,=irq_handler
		ldr		pc,_fiq

_undefined_instruction: .word  _undefined_instruction
_software_interrupt:	.word  _software_interrupt
_prefetch_abort:		.word  _prefetch_abort
_data_abort:			.word  _data_abort
_not_used:				.word  _not_used
_irq:					.word  irq_handler
_fiq:					.word  _fiq


reset:
	ldr	r0,=0x40008000
	mcr	p15,0,r0,c12,c0,0		@ Vector Base Address Register
init_stack:
		ldr		r0,stacktop         /*get stack top pointer*/

	/********svc mode stack********/
		mov		sp,r0
		sub		r0,#128*4          /*512 byte  for irq mode of stack*/

	/****irq mode stack**/
		msr		cpsr,#0xd2
		mov		sp,r0
		sub		r0,#128*4          /*512 byte  for irq mode of stack*/
	/***fiq mode stack***/
		msr 	cpsr,#0xd1
		mov		sp,r0
		sub		r0,#0
	/***abort mode stack***/
		msr		cpsr,#0xd7
		mov		sp,r0
		sub		r0,#0
	/***undefine mode stack***/
		msr		cpsr,#0xdb
		mov		sp,r0
		sub		r0,#0
   /*** sys mode and usr mode stack ***/
		msr		cpsr,#0x10
		mov		sp,r0             /*1024 byte  for user mode of stack*/
		mov     r0,#0x12
		mov     r1,#0x10
		b		main

	.align	4

//r0 =  dest="he "  r1  buf"hello "
.global mystrcopy
mystrcopy:
	ldrb r2,[r1],#1
	strb r2,[r0],#1
	cmp r2,#0
	bne mystrcopy

	mov r0,#0x11
	mrs r0,cpsr
	mov pc,lr
	/****  swi_interrupt handler  ****/

	/****  irq_handler  ****/
irq_handler:

	sub  lr,lr,#4 //修正返回地址，3级流水线
	stmfd sp!,{r0-r12,lr}//保存现场
	.weak do_irq         //
	mrs r0,cpsr
	bl	do_irq
	ldmfd sp!,{r0-r12,pc}^//恢复现场

stacktop:    .word 		stack+4*512
.data

stack:	 .space  4*512

