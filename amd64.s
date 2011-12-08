	.text
	.p2align 4,,15
	.globl add256
	.type	add256, @function
add256:
	// little-endian x86-64 implementation of 256-bit add
	// Arguments are register-passed. Returns carry
	// output : rdi ll, lh, hl, hh
	//              0   8   16  24 
	// input 1: rsi
	// input 2: rdx
	movq	0(%rsi),%rax
	movq	8(%rsi),%rcx
	movq	16(%rsi),%r8
	movq	24(%rsi),%r9
	xor	%rsi,%rsi
	add	0(%rdx),%rax
	adc	8(%rdx),%rcx
	adc	16(%rdx),%r8
	adc	24(%rdx),%r9
	adc	$0,%rsi
	movq	%rsi,%rax
	movq	%rax, 0(%rdi)
	movq	%rcx, 8(%rdi)
	movq	%r8, 16(%rdi)
	movq	%r9, 24(%rdi)
	ret
	.p2align 4,,15
	.globl mul256
	.type	mul256, @function
mul256:
	// little-endian x86-64 implementation of 256-bit mul
	// Arguments are register-passed, 
	// output : rdi lll, llh, lhl, lhh, hll, hlh, hhl, hhh
	//              0    8    16   24   32   40   48   56
	// input 1: (rsi) r15  r14  r13  r12  LSB
	// input 2: (rdx) r11  rbp  rsp  r10  LSB
	push	%rbx
	push	%rbp
	push	%r10
	push	%r12
	push	%r13
	push	%r14
	push	%r15

	movq	%rsp,%xmm1
	movq	(%rsi),%r12
	movq	8(%rsi),%r13
	movq	16(%rsi),%r14
	movq	24(%rsi),%r15
	movq	(%rdx),%r10
	movq	8(%rdx),%rsp
	movq	16(%rdx),%rbp
	movq	24(%rdx),%r11
	
	// 0
	movq	%r10,%rax
	mulq	%r12
	movq	%rax,(%rdi)
	// 1
	movq	%rdx,%rcx
	movq	%r10,%rax
	mulq	%r13
	xor	%ebx,%ebx
	add	%rax,%rcx
	adc	$0,%rbx
	movq	%rdx,%r8
	movq	%rsp,%rax
	mulq	%r12
	add	%rax,%rcx
	adc	$0,%rbx
	movq	%rcx,8(%rdi)
	// 2 rbx: result rcx: carry
	xor	%ecx,%ecx
	add	%r8,%rbx
	adc	$0,%rcx
	add	%rdx,%rbx
	adc	$0,%rcx
	movq	%r10,%rax
	mulq	%r14
	add	%rax,%rbx
	adc	$0,%rcx
	movq	%rdx,%r8
	movq	%rsp,%rax
	mulq	%r13
	add	%rax,%rbx
	adc	$0,%rcx
	movq	%rdx,%r9
	movq	%rbp,%rax
	mulq	%r12
	add	%rax,%rbx
	adc	$0,%rcx
	movq	%rbx,16(%rdi)
	// 3 rbx: carry rcx: result
	xor	%ebx,%ebx
	add	%r8,%rcx
	adc	$0,%rbx
	add	%r9,%rcx
	adc	$0,%rbx
	add	%rdx,%rcx
	adc	$0,%rbx
	movq	%r10,%rax
	// r10 liveness ends here
	// r15 is first used here
	mulq	%r15
	add	%rax,%rcx
	adc	$0,%rbx
	movq	%rdx,%r8
	movq	%rsp,%rax
	mulq	%r14
	add	%rax,%rcx
	adc	$0,%rbx
	movq	%rdx,%r9
	movq	%rbp,%rax
	mulq	%r13
	add	%rax,%rcx
	adc	$0,%rbx
	movq	%rdx,%r10
	// r11 is first used here
	movq	%r11,%rax
	mulq	%r12
	add	%rax,%rcx
	adc	$0,%rbx
	movq	%rcx,24(%rdi)
	// 4 rbx: result rcx: carry
	xor	%ecx,%ecx
	add	%r8,%rbx
	adc	$0,%rcx
	add	%r9,%rbx
	adc	$0,%rcx
	add	%r10,%rbx
	adc	$0,%rcx
	add	%rdx,%rbx
	adc	$0,%rcx
	movq	%rsp,%rax
	// rsp liveness ends here
	mulq	%r15
	add	%rax,%rbx
	adc	$0,%rcx
	movq	%rdx,%r8
	movq	%rbp,%rax
	mulq	%r14
	add	%rax,%rbx
	adc	$0,%rcx
	movq	%rdx,%r9
	movq	%r11,%rax
	mulq	%r13
	add	%rax,%rbx
	adc	$0,%rcx
	movq	%rbx,32(%rdi)
	// 5 rbx: carry rcx: result
	xor	%ebx,%ebx
	add	%r8,%rcx
	adc	$0,%rbx
	add	%r9,%rcx
	adc	$0,%rbx
	add	%rdx,%rcx
	adc	$0,%rbx
	movq	%rbp,%rax
	mulq	%r15
	add	%rax,%rcx
	adc	$0,%rbx
	movq	%rdx,%r8
	movq	%r11,%rax
	mulq	%r14
	add	%rax,%rcx
	adc	$0,%rbx
	movq	%rcx,40(%rdi)
	// 6 rbx: result rcx: carry
	xor	%ecx,%ecx
	add	%r8,%rbx
	adc	$0,%rcx
	add	%rdx,%rbx
	adc	$0,%rcx
	movq	%r11,%rax
	mulq	%r15
	add	%rax,%rbx
	adc	$0,%rcx
	movq	%rbx,48(%rdi)
	// 7 rcx: result
	add	%rdx,%rcx
	movq	%rcx,56(%rdi)
	
	movq	%xmm1,%rsp
	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%r10
	pop	%rbp
	pop	%rbx
	
	ret
