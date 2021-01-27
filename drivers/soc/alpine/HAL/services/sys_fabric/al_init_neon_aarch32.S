/*
 * Copyright 2017, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

#ifdef __arm__

.global al_neon_enable
.type al_neon_enable, %function
.func al_neon_enable
al_neon_enable:
	@ enabling NEON
	MRC p15, 0, r1, c1, c0, 2  @ Read Coprocessor Access Control Register
	ORR r1, #(0xff << 20)      @ enable cp10 and cp 11
	MCR p15, 0, r1, c1, c0, 2  @ Write Coprocessor Access Control Register
	dsb
	isb
	@ global neon + vfp enable
	MOV r1, #0x40000000
	.word 0xeee81a10     @ manual encoding : VMSR FPEXC, r1
	@ see this assembler bug:
	@ http://lists.gnu.org/archive/html/bug-binutils/2012-03/msg00096.html

	cmp r0, #0
	beq 1f
	@ Initialize registers
	mov  r1, #0
	vdup d0.32, r1
	vdup d1.32, r1
	vdup d2.32, r1
	vdup d3.32, r1
	vdup d4.32, r1
	vdup d5.32, r1
	vdup d6.32, r1
	vdup d7.32, r1
	vdup d8.32, r1
	vdup d9.32, r1
	vdup d10.32, r1
	vdup d11.32, r1
	vdup d12.32, r1
	vdup d13.32, r1
	vdup d14.32, r1
	vdup d15.32, r1
	vdup d16.32, r1
	vdup d17.32, r1
	vdup d18.32, r1
	vdup d19.32, r1
	vdup d20.32, r1
	vdup d21.32, r1
	vdup d22.32, r1
	vdup d23.32, r1
	vdup d24.32, r1
	vdup d25.32, r1
	vdup d26.32, r1
	vdup d27.32, r1
	vdup d28.32, r1
	vdup d29.32, r1
	vdup d30.32, r1
	vdup d31.32, r1
1:
	bx lr
.endfunc

#endif /* defined(__arm__) */
