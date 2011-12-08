	.text
        .machine ppc64
	.text
	.align 2
	.p2align 4,,15
	.globl _add256
_add256:
	// output: r3 0, 8, 16, 24
	//            MSB      LSB
	ld r6,24(r4)
	ld r7,24(r5)
	addc r8,r6,r7
	std r8,24(r3)
	ld r6,16(r4)
	ld r7,16(r5)
	adde r8,r6,r7
	std r8,16(r3)
	ld r6,8(r4)
	ld r7,8(r5)
	adde r8,r6,r7
	std r8,8(r3)
	ld r6,0(r4)
	ld r7,0(r5)
	adde r8,r6,r7
	std r8,0(r3)
	blr
	.p2align 4,,15
	.globl _mul256
_mul256:
	// output: r3 0,  8,  16, 24, 32, 40, 48, 56
	//            MSB                        LSB
	// input: r4 0,  8,  16, 24
	//           r6  r7  r8  r9
	//        r5 0,  8,  16, 24
	//           r28 r29 r30 r31
	stdu r1,-40(r1)
	std r31,32(r1)
	std r30,24(r1)
	std r29,16(r1)
	std r28,8(r1)
	
	ld r6,0(r4)
	ld r7,8(r4)
	ld r8,16(r4)
	ld r9,24(r4)
	
	ld r28,0(r5)
	ld r29,8(r5)
	ld r30,16(r5)
	ld r31,24(r5)
	xor r0,r0,r0
	
	// 0
	mulld r10,r9,r31
	std r10,56(r3)
	// 1
	mulhdu r10,r9,r31
	mulld r11,r8,r31
	addc r10,r10,r11
	adde r12,r0,r0
	mulld r11,r9,r30
	addc r10,r10,r11
	adde r12,r12,r0
	std r10,48(r3)
	// 2
	mulhdu r10,r8,r31
	addc r10,r10,r12
	adde r12,r0,r0
	mulld r11,r7,r31
	addc r10,r10,r11
	adde r12,r12,r0
	mulhdu r11,r9,r30
	addc r10,r10,r11
	adde r12,r12,r0
	mulld r11,r8,r30
	addc r10,r10,r11
	adde r12,r12,r0
	mulld r11,r9,r29
	addc r10,r10,r11
	adde r12,r12,r0
	std r10,40(r3)
	// 3
	mulhdu r10,r7,r31
	addc r10,r10,r12
	adde r12,r0,r0
	mulld r11,r6,r31
	addc r10,r10,r11
	adde r12,r12,r0
	mulhdu r11,r30,r8
	addc r10,r10,r11
	adde r12,r12,r0
	mulld r11,r30,r7
	addc r10,r10,r11
	adde r12,r12,r0
	mulhdu r11,r29,r9
	addc r10,r10,r11
	adde r12,r12,r0
	mulld r11,r29,r8
	addc r10,r10,r11
	adde r12,r12,r0
	mulld r11,r28,r9
	addc r10,r10,r11
	adde r12,r12,r0
	std r10,32(r3)
	// 4
	mulhdu r10,r6,r31
	addc r10,r10,r12
	adde r12,r0,r0
	mulhdu r11,r30,r7
	addc r10,r10,r11
	adde r12,r12,r0
	mulld r11,r30,r6
	addc r10,r10,r11
	adde r12,r12,r0
	mulhdu r11,r29,r8
	addc r10,r10,r11
	adde r12,r12,r0
	mulld r11,r29,r7
	addc r10,r10,r11
	adde r12,r12,r0
	mulhdu r11,r28,r9
	addc r10,r10,r11
	adde r12,r12,r0
	mulld r10,r28,r8
	addc r10,r10,r11
	adde r12,r12,r0
	std r10,24(r3)
	// 5
	mulhdu r10,r6,r30
	addc r10,r10,r12
	adde r12,r0,r0
	mulhdu r11,r29,r7
	addc r10,r10,r11
	adde r12,r12,r0
	mulld r11,r29,r6
	addc r10,r10,r11
	adde r12,r12,r0
	mulhdu r11,r28,r8
	addc r10,r10,r11
	adde r12,r12,r0
	mulld r11,r28,r7
	addc r10,r10,r11
	adde r12,r12,r0
	std r10,16(r3)
	// 6
	mulhdu r10,r29,r6
	addc r10,r10,r12
	adde r12,r0,r0
	mulhdu r11,r28,r7
	addc r10,r10,r11
	adde r12,r12,r0
	mulld r11,r28,r6
	addc r10,r10,r11
	adde r12,r12,r0
	std r10,8(r3)
	// 7
	mulhdu r10,r28,r6
	add r10,r10,r12
	std r10,0(r3)
	
	ld r28,8(r1)
	ld r29,16(r1)
	ld r30,24(r1)
	ld r31,32(r1)
	addi r1,r1,40
	blr
