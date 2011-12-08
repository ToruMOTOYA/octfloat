	.text
        .machine ppc64
	.text
	.align 2
	.p2align 4,,15
	.globl add256
add256:
	# output: 3 0, 8, 16, 24
	#            MSB      LSB
	ld 6,24(4)
	ld 7,24(5)
	addc 8,6,7
	std 8,24(3)
	ld 6,16(4)
	ld 7,16(5)
	adde 8,6,7
	std 8,16(3)
	ld 6,8(4)
	ld 7,8(5)
	adde 8,6,7
	std 8,8(3)
	ld 6,0(4)
	ld 7,0(5)
	adde 8,6,7
	std 8,0(3)
	blr
	.p2align 4,,15
	.globl mul256
mul256:
	# output: 3 0,  8,  16, 24, 32, 40, 48, 56
	#            MSB                        LSB
	# input: 4 0,  8,  16, 24
	#           6  7  8  9
	#        5 0,  8,  16, 24
	#           28 29 30 31
	stdu 1,-40(1)
	std 31,32(1)
	std 30,24(1)
	std 29,16(1)
	std 28,8(1)
	
	ld 6,0(4)
	ld 7,8(4)
	ld 8,16(4)
	ld 9,24(4)
	
	ld 28,0(5)
	ld 29,8(5)
	ld 30,16(5)
	ld 31,24(5)
	xor 0,0,0
	
	# 0
	mulld 10,9,31
	std 10,56(3)
	# 1
	mulhdu 10,9,31
	mulld 11,8,31
	addc 10,10,11
	adde 12,0,0
	mulld 11,9,30
	addc 10,10,11
	adde 12,12,0
	std 10,48(3)
	# 2
	mulhdu 10,8,31
	addc 10,10,12
	adde 12,0,0
	mulld 11,7,31
	addc 10,10,11
	adde 12,12,0
	mulhdu 11,9,30
	addc 10,10,11
	adde 12,12,0
	mulld 11,8,30
	addc 10,10,11
	adde 12,12,0
	mulld 11,9,29
	addc 10,10,11
	adde 12,12,0
	std 10,40(3)
	# 3
	mulhdu 10,7,31
	addc 10,10,12
	adde 12,0,0
	mulld 11,6,31
	addc 10,10,11
	adde 12,12,0
	mulhdu 11,30,8
	addc 10,10,11
	adde 12,12,0
	mulld 11,30,7
	addc 10,10,11
	adde 12,12,0
	mulhdu 11,29,9
	addc 10,10,11
	adde 12,12,0
	mulld 11,29,8
	addc 10,10,11
	adde 12,12,0
	mulld 11,28,9
	addc 10,10,11
	adde 12,12,0
	std 10,32(3)
	# 4
	mulhdu 10,6,31
	addc 10,10,12
	adde 12,0,0
	mulhdu 11,30,7
	addc 10,10,11
	adde 12,12,0
	mulld 11,30,6
	addc 10,10,11
	adde 12,12,0
	mulhdu 11,29,8
	addc 10,10,11
	adde 12,12,0
	mulld 11,29,7
	addc 10,10,11
	adde 12,12,0
	mulhdu 11,28,9
	addc 10,10,11
	adde 12,12,0
	mulld 10,28,8
	addc 10,10,11
	adde 12,12,0
	std 10,24(3)
	# 5
	mulhdu 10,6,30
	addc 10,10,12
	adde 12,0,0
	mulhdu 11,29,7
	addc 10,10,11
	adde 12,12,0
	mulld 11,29,6
	addc 10,10,11
	adde 12,12,0
	mulhdu 11,28,8
	addc 10,10,11
	adde 12,12,0
	mulld 11,28,7
	addc 10,10,11
	adde 12,12,0
	std 10,16(3)
	# 6
	mulhdu 10,29,6
	addc 10,10,12
	adde 12,0,0
	mulhdu 11,28,7
	addc 10,10,11
	adde 12,12,0
	mulld 11,28,6
	addc 10,10,11
	adde 12,12,0
	std 10,8(3)
	# 7
	mulhdu 10,28,6
	add 10,10,12
	std 10,0(3)
	
	ld 28,8(1)
	ld 29,16(1)
	ld 30,24(1)
	ld 31,32(1)
	addi 1,1,40
	blr
