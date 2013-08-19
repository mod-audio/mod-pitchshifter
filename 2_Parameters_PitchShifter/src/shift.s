	.file	"shift.cpp"
	.text
.Ltext0:
	.section	.text._ZNSt7complexIdEC2Edd,"axG",@progbits,_ZNSt7complexIdEC5Edd,comdat
	.align 2
	.weak	_ZNSt7complexIdEC2Edd
	.type	_ZNSt7complexIdEC2Edd, @function
_ZNSt7complexIdEC2Edd:
.LFB1228:
	.file 1 "/usr/include/c++/4.7/complex"
	.loc 1 1195 0
	.cfi_startproc
	pushq	%rbp
.LCFI0:
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
.LCFI1:
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movsd	%xmm0, -16(%rbp)
	movsd	%xmm1, -24(%rbp)
.LBB2:
	.loc 1 1200 0
	movq	-8(%rbp), %rdx
	movq	-16(%rbp), %rax
	movq	%rax, (%rdx)
	.loc 1 1201 0
	movq	-8(%rbp), %rdx
	movq	-24(%rbp), %rax
	movq	%rax, 8(%rdx)
.LBE2:
	.loc 1 1202 0
	popq	%rbp
.LCFI2:
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1228:
	.size	_ZNSt7complexIdEC2Edd, .-_ZNSt7complexIdEC2Edd
	.weak	_ZNSt7complexIdEC1Edd
	.set	_ZNSt7complexIdEC1Edd,_ZNSt7complexIdEC2Edd
	.section	.text._ZNSt7complexIdE4realEv,"axG",@progbits,_ZNSt7complexIdE4realEv,comdat
	.align 2
	.weak	_ZNSt7complexIdE4realEv
	.type	_ZNSt7complexIdE4realEv, @function
_ZNSt7complexIdE4realEv:
.LFB1233:
	.loc 1 1220 0
	.cfi_startproc
	pushq	%rbp
.LCFI3:
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
.LCFI4:
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	.loc 1 1220 0
	movq	-8(%rbp), %rax
	popq	%rbp
.LCFI5:
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1233:
	.size	_ZNSt7complexIdE4realEv, .-_ZNSt7complexIdE4realEv
	.section	.text._ZNSt7complexIdEaSEd,"axG",@progbits,_ZNSt7complexIdEaSEd,comdat
	.align 2
	.weak	_ZNSt7complexIdEaSEd
	.type	_ZNSt7complexIdEaSEd, @function
_ZNSt7complexIdEaSEd:
.LFB1239:
	.loc 1 1241 0
	.cfi_startproc
	pushq	%rbp
.LCFI6:
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
.LCFI7:
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movsd	%xmm0, -16(%rbp)
	.loc 1 1243 0
	movq	-16(%rbp), %rcx
	movl	$0, %edx
	movq	-8(%rbp), %rax
	movq	%rcx, (%rax)
	movq	%rdx, 8(%rax)
	.loc 1 1244 0
	movq	-8(%rbp), %rax
	.loc 1 1245 0
	popq	%rbp
.LCFI8:
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1239:
	.size	_ZNSt7complexIdEaSEd, .-_ZNSt7complexIdEaSEd
	.text
	.globl	_Z5shiftiiPiPdS0_PSt7complexIdES0_PS0_S0_S3_S3_S0_S3_S3_S0_S0_S0_iP11fftw_plan_sS6_
	.type	_Z5shiftiiPiPdS0_PSt7complexIdES0_PS0_S0_S3_S3_S0_S3_S3_S0_S0_S0_iP11fftw_plan_sS6_, @function
_Z5shiftiiPiPdS0_PSt7complexIdES0_PS0_S0_S3_S3_S0_S3_S3_S0_S0_S0_iP11fftw_plan_sS6_:
.LFB1288:
	.file 2 "shift.cpp"
	.loc 2 10 0
	.cfi_startproc
	.cfi_personality 0x3,__gxx_personality_v0
	.cfi_lsda 0x3,.LLSDA1288
	pushq	%rbp
.LCFI9:
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
.LCFI10:
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$232, %rsp
	.cfi_offset 3, -24
	movl	%edi, -180(%rbp)
	movl	%esi, -184(%rbp)
	movq	%rdx, -192(%rbp)
	movq	%rcx, -200(%rbp)
	movq	%r8, -208(%rbp)
	movq	%r9, -216(%rbp)
.LBB3:
	.loc 2 19 0
	movl	-180(%rbp), %eax
	movl	%eax, -20(%rbp)
.LBB4:
	.loc 2 20 0
	movl	$1, -24(%rbp)
	jmp	.L7
.L8:
	.loc 2 22 0 discriminator 2
	movl	-24(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,4), %rdx
	movq	-192(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	addl	%eax, -20(%rbp)
	.loc 2 20 0 discriminator 2
	addl	$1, -24(%rbp)
.L7:
	.loc 2 20 0 is_stmt 0 discriminator 1
	movl	104(%rbp), %eax
	subl	$1, %eax
	cmpl	-24(%rbp), %eax
	setge	%al
	testb	%al, %al
	jne	.L8
.LBE4:
	.loc 2 24 0 is_stmt 1
	movl	$0, -28(%rbp)
	.loc 2 26 0
	leaq	-176(%rbp), %rax
	xorpd	%xmm1, %xmm1
	xorpd	%xmm0, %xmm0
	movq	%rax, %rdi
	call	_ZNSt7complexIdEC1Edd
	.loc 2 43 0
	leaq	-160(%rbp), %rax
	movsd	.LC1(%rip), %xmm1
	xorpd	%xmm0, %xmm0
	movq	%rax, %rdi
	call	_ZNSt7complexIdEC1Edd
	movq	-160(%rbp), %rax
	movq	%rax, -176(%rbp)
	movq	-152(%rbp), %rax
	movq	%rax, -168(%rbp)
.LBB5:
	.loc 2 44 0
	movl	$1, -32(%rbp)
	jmp	.L9
.L10:
	.loc 2 46 0 discriminator 2
	movl	-32(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	88(%rbp), %rax
	addq	%rax, %rdx
	movl	$0, %eax
	movq	%rax, (%rdx)
	.loc 2 44 0 discriminator 2
	addl	$1, -32(%rbp)
.L9:
	.loc 2 44 0 is_stmt 0 discriminator 1
	movl	-32(%rbp), %eax
	cmpl	-20(%rbp), %eax
	setle	%al
	testb	%al, %al
	jne	.L10
.LBE5:
	.loc 2 49 0 is_stmt 1
	movl	-180(%rbp), %eax
	movl	-20(%rbp), %edx
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	88(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -72(%rbp)
.LBB6:
	.loc 2 54 0
	movl	$1, -36(%rbp)
	jmp	.L11
.L12:
	.loc 2 56 0 discriminator 2
	movl	-36(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	-200(%rbp), %rax
	addq	%rdx, %rax
	movsd	(%rax), %xmm1
	movl	-36(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	-208(%rbp), %rax
	addq	%rdx, %rax
	movsd	(%rax), %xmm0
	movapd	%xmm1, %xmm2
	mulsd	%xmm0, %xmm2
	movsd	%xmm2, -224(%rbp)
	cvtsi2sd	-180(%rbp), %xmm1
	cvtsi2sd	-184(%rbp), %xmm0
	addsd	%xmm0, %xmm0
	movapd	%xmm1, %xmm3
	divsd	%xmm0, %xmm3
	movapd	%xmm3, %xmm0
	call	sqrt
	movsd	-224(%rbp), %xmm4
	divsd	%xmm0, %xmm4
	movapd	%xmm4, %xmm0
	movl	-36(%rbp), %eax
	cltq
	subq	$1, %rax
	movq	%rax, %rdx
	salq	$4, %rdx
	movq	72(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, %rdi
	call	_ZNSt7complexIdEaSEd
	.loc 2 54 0 discriminator 2
	addl	$1, -36(%rbp)
.L11:
	.loc 2 54 0 is_stmt 0 discriminator 1
	movl	-36(%rbp), %eax
	cmpl	-180(%rbp), %eax
	setle	%al
	testb	%al, %al
	jne	.L12
.LBE6:
	.loc 2 60 0 is_stmt 1
	movq	112(%rbp), %rax
	movq	%rax, %rdi
.LEHB0:
	call	fftw_execute
.LBB7:
	.loc 2 65 0
	movl	$1, -40(%rbp)
	jmp	.L13
.L14:
	.loc 2 68 0 discriminator 2
	cvtsi2sd	-184(%rbp), %xmm0
	movsd	.LC2(%rip), %xmm1
	mulsd	%xmm1, %xmm0
	cvtsi2sd	-180(%rbp), %xmm1
	movapd	%xmm0, %xmm2
	divsd	%xmm1, %xmm2
	movapd	%xmm2, %xmm1
	cvtsi2sd	-40(%rbp), %xmm0
	movsd	.LC1(%rip), %xmm2
	subsd	%xmm2, %xmm0
	mulsd	%xmm1, %xmm0
	movsd	-80(%rbp), %xmm1
	movapd	%xmm1, %xmm3
	subsd	%xmm0, %xmm3
	movapd	%xmm3, %xmm0
	movsd	%xmm0, -88(%rbp)
	.loc 2 69 0 discriminator 2
	movsd	-88(%rbp), %xmm1
	movsd	.LC3(%rip), %xmm0
	addsd	%xmm1, %xmm0
	movsd	.LC2(%rip), %xmm1
	divsd	%xmm1, %xmm0
	movsd	%xmm0, -96(%rbp)
	.loc 2 70 0 discriminator 2
	movq	-96(%rbp), %rax
	movq	%rax, -240(%rbp)
	movsd	-240(%rbp), %xmm0
	call	floor
	movsd	%xmm0, -240(%rbp)
	movq	-240(%rbp), %rax
	movq	%rax, -96(%rbp)
	.loc 2 71 0 discriminator 2
	movsd	-96(%rbp), %xmm1
	movsd	.LC2(%rip), %xmm0
	mulsd	%xmm1, %xmm0
	movsd	-88(%rbp), %xmm1
	movapd	%xmm1, %xmm4
	subsd	%xmm0, %xmm4
	movapd	%xmm4, %xmm0
	movsd	%xmm0, -104(%rbp)
	.loc 2 72 0 discriminator 2
	cvtsi2sd	-180(%rbp), %xmm0
	movsd	.LC2(%rip), %xmm1
	divsd	%xmm0, %xmm1
	cvtsi2sd	-40(%rbp), %xmm0
	movsd	.LC1(%rip), %xmm2
	subsd	%xmm2, %xmm0
	mulsd	%xmm0, %xmm1
	cvtsi2sd	-184(%rbp), %xmm0
	movsd	-104(%rbp), %xmm2
	movapd	%xmm2, %xmm3
	divsd	%xmm0, %xmm3
	movapd	%xmm3, %xmm0
	addsd	%xmm1, %xmm0
	movsd	%xmm0, -112(%rbp)
	.loc 2 73 0 discriminator 2
	movl	-40(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	80(%rbp), %rax
	addq	%rax, %rdx
	movl	-40(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rcx
	movq	16(%rbp), %rax
	addq	%rcx, %rax
	movsd	(%rax), %xmm1
	movl	104(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,4), %rcx
	movq	-192(%rbp), %rax
	addq	%rcx, %rax
	movl	(%rax), %eax
	cvtsi2sd	%eax, %xmm0
	mulsd	-112(%rbp), %xmm0
	addsd	%xmm1, %xmm0
	movsd	%xmm0, (%rdx)
	.loc 2 75 0 discriminator 2
	movl	-40(%rbp), %eax
	cltq
	subq	$1, %rax
	movq	%rax, %rdx
	salq	$4, %rdx
	movq	-216(%rbp), %rax
	addq	%rax, %rdx
	movl	-40(%rbp), %eax
	cltq
	subq	$1, %rax
	movq	%rax, %rcx
	salq	$4, %rcx
	movq	40(%rbp), %rax
	addq	%rax, %rcx
	movq	(%rcx), %rax
	movq	%rax, (%rdx)
	movq	8(%rcx), %rax
	movq	%rax, 8(%rdx)
	.loc 2 76 0 discriminator 2
	movl	-40(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	16(%rbp), %rax
	addq	%rax, %rdx
	movl	-40(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rcx
	movq	80(%rbp), %rax
	addq	%rcx, %rax
	movq	(%rax), %rax
	movq	%rax, (%rdx)
	.loc 2 65 0 discriminator 2
	addl	$1, -40(%rbp)
.L13:
	.loc 2 65 0 is_stmt 0 discriminator 1
	movl	-40(%rbp), %eax
	cmpl	-180(%rbp), %eax
	setle	%al
	testb	%al, %al
	jne	.L14
.LBE7:
	.loc 2 81 0 is_stmt 1
	movq	120(%rbp), %rax
	movq	%rax, %rdi
	call	fftw_execute
.LEHE0:
.LBB8:
	.loc 2 82 0
	movl	$1, -44(%rbp)
	jmp	.L15
.L18:
.LBB9:
	.loc 2 84 0
	movl	-44(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	56(%rbp), %rax
	leaq	(%rdx,%rax), %rbx
	movl	-44(%rbp), %eax
	cltq
	subq	$1, %rax
	movq	%rax, %rdx
	salq	$4, %rdx
	movq	64(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, %rdi
	call	_ZSt4realIdERT_RSt7complexIS0_E
	movsd	(%rax), %xmm1
	cvtsi2sd	-180(%rbp), %xmm0
	movapd	%xmm1, %xmm4
	divsd	%xmm0, %xmm4
	movapd	%xmm4, %xmm0
	movsd	%xmm0, (%rbx)
	.loc 2 85 0
	movl	-44(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	56(%rbp), %rax
	leaq	(%rdx,%rax), %rbx
	movl	-44(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	56(%rbp), %rax
	addq	%rdx, %rax
	movsd	(%rax), %xmm1
	movl	-44(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	-208(%rbp), %rax
	addq	%rdx, %rax
	movsd	(%rax), %xmm0
	movapd	%xmm1, %xmm2
	mulsd	%xmm0, %xmm2
	movsd	%xmm2, -232(%rbp)
	cvtsi2sd	-180(%rbp), %xmm1
	movl	104(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,4), %rdx
	movq	-192(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	cvtsi2sd	%eax, %xmm0
	addsd	%xmm0, %xmm0
	movapd	%xmm1, %xmm3
	divsd	%xmm0, %xmm3
	movapd	%xmm3, %xmm0
	call	sqrt
	movsd	-232(%rbp), %xmm4
	divsd	%xmm0, %xmm4
	movapd	%xmm4, %xmm0
	movsd	%xmm0, (%rbx)
.LBB10:
	.loc 2 86 0
	movl	$1, -48(%rbp)
	jmp	.L16
.L17:
	.loc 2 88 0 discriminator 2
	movl	-44(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	24(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	-48(%rbp), %edx
	movslq	%edx, %rdx
	subq	$1, %rdx
	salq	$3, %rdx
	addq	%rax, %rdx
	movl	-44(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rcx
	movq	24(%rbp), %rax
	addq	%rcx, %rax
	movq	(%rax), %rax
	movl	-48(%rbp), %ecx
	movslq	%ecx, %rcx
	salq	$3, %rcx
	addq	%rcx, %rax
	movq	(%rax), %rax
	movq	%rax, (%rdx)
	.loc 2 86 0 discriminator 2
	addl	$1, -48(%rbp)
.L16:
	.loc 2 86 0 is_stmt 0 discriminator 1
	movl	104(%rbp), %eax
	subl	$1, %eax
	cmpl	-48(%rbp), %eax
	setge	%al
	testb	%al, %al
	jne	.L17
.LBE10:
	.loc 2 90 0 is_stmt 1
	movl	-44(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	24(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	104(%rbp), %edx
	movslq	%edx, %rdx
	subq	$1, %rdx
	salq	$3, %rdx
	addq	%rax, %rdx
	movl	-44(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rcx
	movq	56(%rbp), %rax
	addq	%rcx, %rax
	movq	(%rax), %rax
	movq	%rax, (%rdx)
.LBE9:
	.loc 2 82 0
	addl	$1, -44(%rbp)
.L15:
	.loc 2 82 0 is_stmt 0 discriminator 1
	movl	-44(%rbp), %eax
	cmpl	-180(%rbp), %eax
	setle	%al
	testb	%al, %al
	jne	.L18
.LBE8:
.LBB11:
	.loc 2 93 0 is_stmt 1
	movl	$1, -52(%rbp)
	jmp	.L19
.L22:
.LBB12:
.LBB13:
	.loc 2 95 0
	movl	$1, -56(%rbp)
	jmp	.L20
.L21:
	.loc 2 97 0 discriminator 2
	movl	-28(%rbp), %eax
	movl	-56(%rbp), %edx
	addl	%edx, %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	88(%rbp), %rax
	addq	%rdx, %rax
	movl	-28(%rbp), %edx
	movl	-56(%rbp), %ecx
	addl	%ecx, %edx
	movslq	%edx, %rdx
	subq	$1, %rdx
	leaq	0(,%rdx,8), %rcx
	movq	88(%rbp), %rdx
	addq	%rcx, %rdx
	movsd	(%rdx), %xmm1
	movl	-56(%rbp), %edx
	movslq	%edx, %rdx
	subq	$1, %rdx
	leaq	0(,%rdx,8), %rcx
	movq	24(%rbp), %rdx
	addq	%rcx, %rdx
	movq	(%rdx), %rdx
	movl	-52(%rbp), %ecx
	movslq	%ecx, %rcx
	subq	$1, %rcx
	salq	$3, %rcx
	addq	%rcx, %rdx
	movsd	(%rdx), %xmm0
	addsd	%xmm1, %xmm0
	movsd	%xmm0, (%rax)
	.loc 2 95 0 discriminator 2
	addl	$1, -56(%rbp)
.L20:
	.loc 2 95 0 is_stmt 0 discriminator 1
	movl	-56(%rbp), %eax
	cmpl	-180(%rbp), %eax
	setle	%al
	testb	%al, %al
	jne	.L21
.LBE13:
	.loc 2 99 0 is_stmt 1
	movl	-52(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,4), %rdx
	movq	-192(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	addl	%eax, -28(%rbp)
.LBE12:
	.loc 2 93 0
	addl	$1, -52(%rbp)
.L19:
	.loc 2 93 0 is_stmt 0 discriminator 1
	movl	-52(%rbp), %eax
	cmpl	104(%rbp), %eax
	setle	%al
	testb	%al, %al
	jne	.L22
.LBE11:
	.loc 2 103 0 is_stmt 1
	movl	104(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,4), %rdx
	movq	-192(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	cvtsi2sd	%eax, %xmm0
	cvtsi2sd	-184(%rbp), %xmm1
	divsd	%xmm1, %xmm0
	movsd	%xmm0, -120(%rbp)
.LBB14:
	.loc 2 105 0
	movl	$1, -60(%rbp)
	jmp	.L23
.L24:
	.loc 2 107 0 discriminator 2
	cvtsi2sd	-60(%rbp), %xmm0
	movsd	.LC1(%rip), %xmm1
	subsd	%xmm1, %xmm0
	mulsd	-120(%rbp), %xmm0
	movsd	.LC1(%rip), %xmm1
	addsd	%xmm1, %xmm0
	movsd	%xmm0, -128(%rbp)
	.loc 2 108 0 discriminator 2
	movq	-128(%rbp), %rax
	movq	%rax, -240(%rbp)
	movsd	-240(%rbp), %xmm0
	call	floor
	cvttsd2si	%xmm0, %eax
	movl	%eax, -132(%rbp)
	.loc 2 109 0 discriminator 2
	movq	-128(%rbp), %rax
	movq	%rax, -240(%rbp)
	movsd	-240(%rbp), %xmm0
	call	ceil
	cvttsd2si	%xmm0, %eax
	movl	%eax, -136(%rbp)
	.loc 2 110 0 discriminator 2
	movl	-60(%rbp), %eax
	cltq
	subq	$1, %rax
	leaq	0(,%rax,8), %rdx
	movq	32(%rbp), %rax
	addq	%rdx, %rax
	movl	-132(%rbp), %edx
	movslq	%edx, %rdx
	subq	$1, %rdx
	leaq	0(,%rdx,8), %rcx
	movq	-72(%rbp), %rdx
	addq	%rcx, %rdx
	movsd	(%rdx), %xmm3
	movl	-136(%rbp), %edx
	movslq	%edx, %rdx
	subq	$1, %rdx
	leaq	0(,%rdx,8), %rcx
	movq	-72(%rbp), %rdx
	addq	%rcx, %rdx
	movsd	(%rdx), %xmm0
	movl	-132(%rbp), %edx
	movslq	%edx, %rdx
	subq	$1, %rdx
	leaq	0(,%rdx,8), %rcx
	movq	-72(%rbp), %rdx
	addq	%rcx, %rdx
	movsd	(%rdx), %xmm1
	movapd	%xmm0, %xmm2
	subsd	%xmm1, %xmm2
	movapd	%xmm2, %xmm1
	cvtsi2sd	-132(%rbp), %xmm0
	movsd	-128(%rbp), %xmm2
	movapd	%xmm2, %xmm4
	subsd	%xmm0, %xmm4
	movapd	%xmm4, %xmm0
	mulsd	%xmm1, %xmm0
	addsd	%xmm3, %xmm0
	movsd	%xmm0, (%rax)
	.loc 2 105 0 discriminator 2
	addl	$1, -60(%rbp)
.L23:
	.loc 2 105 0 is_stmt 0 discriminator 1
	movl	-60(%rbp), %eax
	cmpl	-184(%rbp), %eax
	setle	%al
	testb	%al, %al
	jne	.L24
.LBE14:
.LBE3:
	.loc 2 117 0 is_stmt 1
	jmp	.L27
.L26:
	movq	%rax, %rdi
.LEHB1:
	call	_Unwind_Resume
.LEHE1:
.L27:
	addq	$232, %rsp
	popq	%rbx
	popq	%rbp
.LCFI11:
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1288:
	.globl	__gxx_personality_v0
	.section	.gcc_except_table,"a",@progbits
.LLSDA1288:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE1288-.LLSDACSB1288
.LLSDACSB1288:
	.uleb128 .LEHB0-.LFB1288
	.uleb128 .LEHE0-.LEHB0
	.uleb128 .L26-.LFB1288
	.uleb128 0
	.uleb128 .LEHB1-.LFB1288
	.uleb128 .LEHE1-.LEHB1
	.uleb128 0
	.uleb128 0
.LLSDACSE1288:
	.text
	.size	_Z5shiftiiPiPdS0_PSt7complexIdES0_PS0_S0_S3_S3_S0_S3_S3_S0_S0_S0_iP11fftw_plan_sS6_, .-_Z5shiftiiPiPdS0_PSt7complexIdES0_PS0_S0_S3_S3_S0_S3_S3_S0_S0_S0_iP11fftw_plan_sS6_
	.section	.text._ZSt4realIdERT_RSt7complexIS0_E,"axG",@progbits,_ZSt4realIdERT_RSt7complexIS0_E,comdat
	.weak	_ZSt4realIdERT_RSt7complexIS0_E
	.type	_ZSt4realIdERT_RSt7complexIS0_E, @function
_ZSt4realIdERT_RSt7complexIS0_E:
.LFB1289:
	.loc 1 545 0
	.cfi_startproc
	pushq	%rbp
.LCFI12:
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
.LCFI13:
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	.loc 1 546 0
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	_ZNSt7complexIdE4realEv
	leave
.LCFI14:
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1289:
	.size	_ZSt4realIdERT_RSt7complexIS0_E, .-_ZSt4realIdERT_RSt7complexIS0_E
	.section	.rodata
	.align 8
.LC1:
	.long	0
	.long	1072693248
	.align 8
.LC2:
	.long	1413754136
	.long	1075388923
	.align 8
.LC3:
	.long	1413754136
	.long	1074340347
	.text
.Letext0:
	.file 3 "/usr/lib/gcc/x86_64-linux-gnu/4.7/include/stddef.h"
	.file 4 "/usr/include/x86_64-linux-gnu/bits/types.h"
	.file 5 "/usr/include/libio.h"
	.file 6 "/usr/include/stdio.h"
	.file 7 "/usr/include/wchar.h"
	.file 8 "<built-in>"
	.file 9 "/usr/local/include/fftw3.h"
	.file 10 "/usr/include/c++/4.7/cwchar"
	.file 11 "/usr/include/c++/4.7/x86_64-linux-gnu/bits/c++config.h"
	.file 12 "/usr/include/c++/4.7/clocale"
	.file 13 "/usr/include/c++/4.7/cwctype"
	.file 14 "/usr/include/c++/4.7/ext/new_allocator.h"
	.file 15 "/usr/include/c++/4.7/ext/numeric_traits.h"
	.file 16 "/usr/include/time.h"
	.file 17 "/usr/include/c++/4.7/bits/cpp_type_traits.h"
	.file 18 "/usr/include/c++/4.7/debug/debug.h"
	.file 19 "/usr/include/locale.h"
	.file 20 "/usr/include/wctype.h"
	.file 21 "shift.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x1e2b
	.value	0x2
	.long	.Ldebug_abbrev0
	.byte	0x8
	.uleb128 0x1
	.long	.LASF262
	.byte	0x4
	.long	.LASF263
	.long	.LASF264
	.long	.Ldebug_ranges0+0
	.quad	0
	.quad	0
	.long	.Ldebug_line0
	.uleb128 0x2
	.long	.LASF6
	.byte	0x3
	.byte	0xd5
	.long	0x3c
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF0
	.uleb128 0x3
	.byte	0x1
	.byte	0x8
	.long	.LASF1
	.uleb128 0x3
	.byte	0x2
	.byte	0x7
	.long	.LASF2
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.long	.LASF3
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF4
	.uleb128 0x3
	.byte	0x2
	.byte	0x5
	.long	.LASF5
	.uleb128 0x2
	.long	.LASF7
	.byte	0x4
	.byte	0x29
	.long	0x71
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.long	.LASF8
	.uleb128 0x2
	.long	.LASF9
	.byte	0x4
	.byte	0x8d
	.long	0x78
	.uleb128 0x2
	.long	.LASF10
	.byte	0x4
	.byte	0x8e
	.long	0x78
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF11
	.uleb128 0x5
	.byte	0x8
	.uleb128 0x6
	.byte	0x8
	.long	0xa4
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF12
	.uleb128 0x7
	.long	.LASF48
	.byte	0xd8
	.byte	0x5
	.value	0x10f
	.long	0x278
	.uleb128 0x8
	.long	.LASF13
	.byte	0x5
	.value	0x110
	.long	0x71
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x8
	.long	.LASF14
	.byte	0x5
	.value	0x115
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x8
	.long	.LASF15
	.byte	0x5
	.value	0x116
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x8
	.long	.LASF16
	.byte	0x5
	.value	0x117
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0x8
	.long	.LASF17
	.byte	0x5
	.value	0x118
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0x8
	.long	.LASF18
	.byte	0x5
	.value	0x119
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x28
	.uleb128 0x8
	.long	.LASF19
	.byte	0x5
	.value	0x11a
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x30
	.uleb128 0x8
	.long	.LASF20
	.byte	0x5
	.value	0x11b
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x38
	.uleb128 0x8
	.long	.LASF21
	.byte	0x5
	.value	0x11c
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x40
	.uleb128 0x8
	.long	.LASF22
	.byte	0x5
	.value	0x11e
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x48
	.uleb128 0x8
	.long	.LASF23
	.byte	0x5
	.value	0x11f
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x50
	.uleb128 0x8
	.long	.LASF24
	.byte	0x5
	.value	0x120
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x58
	.uleb128 0x8
	.long	.LASF25
	.byte	0x5
	.value	0x122
	.long	0x375
	.byte	0x2
	.byte	0x23
	.uleb128 0x60
	.uleb128 0x8
	.long	.LASF26
	.byte	0x5
	.value	0x124
	.long	0x37b
	.byte	0x2
	.byte	0x23
	.uleb128 0x68
	.uleb128 0x8
	.long	.LASF27
	.byte	0x5
	.value	0x126
	.long	0x71
	.byte	0x2
	.byte	0x23
	.uleb128 0x70
	.uleb128 0x8
	.long	.LASF28
	.byte	0x5
	.value	0x12a
	.long	0x71
	.byte	0x2
	.byte	0x23
	.uleb128 0x74
	.uleb128 0x8
	.long	.LASF29
	.byte	0x5
	.value	0x12c
	.long	0x7f
	.byte	0x2
	.byte	0x23
	.uleb128 0x78
	.uleb128 0x8
	.long	.LASF30
	.byte	0x5
	.value	0x130
	.long	0x4a
	.byte	0x3
	.byte	0x23
	.uleb128 0x80
	.uleb128 0x8
	.long	.LASF31
	.byte	0x5
	.value	0x131
	.long	0x58
	.byte	0x3
	.byte	0x23
	.uleb128 0x82
	.uleb128 0x8
	.long	.LASF32
	.byte	0x5
	.value	0x132
	.long	0x381
	.byte	0x3
	.byte	0x23
	.uleb128 0x83
	.uleb128 0x8
	.long	.LASF33
	.byte	0x5
	.value	0x136
	.long	0x391
	.byte	0x3
	.byte	0x23
	.uleb128 0x88
	.uleb128 0x8
	.long	.LASF34
	.byte	0x5
	.value	0x13f
	.long	0x8a
	.byte	0x3
	.byte	0x23
	.uleb128 0x90
	.uleb128 0x8
	.long	.LASF35
	.byte	0x5
	.value	0x148
	.long	0x9c
	.byte	0x3
	.byte	0x23
	.uleb128 0x98
	.uleb128 0x8
	.long	.LASF36
	.byte	0x5
	.value	0x149
	.long	0x9c
	.byte	0x3
	.byte	0x23
	.uleb128 0xa0
	.uleb128 0x8
	.long	.LASF37
	.byte	0x5
	.value	0x14a
	.long	0x9c
	.byte	0x3
	.byte	0x23
	.uleb128 0xa8
	.uleb128 0x8
	.long	.LASF38
	.byte	0x5
	.value	0x14b
	.long	0x9c
	.byte	0x3
	.byte	0x23
	.uleb128 0xb0
	.uleb128 0x8
	.long	.LASF39
	.byte	0x5
	.value	0x14c
	.long	0x31
	.byte	0x3
	.byte	0x23
	.uleb128 0xb8
	.uleb128 0x8
	.long	.LASF40
	.byte	0x5
	.value	0x14e
	.long	0x71
	.byte	0x3
	.byte	0x23
	.uleb128 0xc0
	.uleb128 0x8
	.long	.LASF41
	.byte	0x5
	.value	0x150
	.long	0x397
	.byte	0x3
	.byte	0x23
	.uleb128 0xc4
	.byte	0
	.uleb128 0x2
	.long	.LASF42
	.byte	0x6
	.byte	0x41
	.long	0xab
	.uleb128 0x9
	.byte	0x8
	.byte	0x7
	.byte	0x54
	.long	.LASF265
	.long	0x2cb
	.uleb128 0xa
	.byte	0x4
	.byte	0x7
	.byte	0x57
	.long	0x2ae
	.uleb128 0xb
	.long	.LASF43
	.byte	0x7
	.byte	0x59
	.long	0x51
	.uleb128 0xb
	.long	.LASF44
	.byte	0x7
	.byte	0x5d
	.long	0x2cb
	.byte	0
	.uleb128 0xc
	.long	.LASF45
	.byte	0x7
	.byte	0x55
	.long	0x71
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xc
	.long	.LASF46
	.byte	0x7
	.byte	0x5e
	.long	0x28f
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.byte	0
	.uleb128 0xd
	.long	0xa4
	.long	0x2db
	.uleb128 0xe
	.long	0x95
	.byte	0x3
	.byte	0
	.uleb128 0x2
	.long	.LASF47
	.byte	0x7
	.byte	0x5f
	.long	0x283
	.uleb128 0xf
	.long	.LASF49
	.byte	0x18
	.byte	0x8
	.byte	0
	.long	0x32b
	.uleb128 0xc
	.long	.LASF50
	.byte	0x8
	.byte	0
	.long	0x51
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xc
	.long	.LASF51
	.byte	0x8
	.byte	0
	.long	0x51
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0xc
	.long	.LASF52
	.byte	0x8
	.byte	0
	.long	0x9c
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0xc
	.long	.LASF53
	.byte	0x8
	.byte	0
	.long	0x9c
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.byte	0
	.uleb128 0x10
	.long	.LASF266
	.byte	0x5
	.byte	0xb4
	.uleb128 0xf
	.long	.LASF54
	.byte	0x18
	.byte	0x5
	.byte	0xba
	.long	0x369
	.uleb128 0xc
	.long	.LASF55
	.byte	0x5
	.byte	0xbb
	.long	0x369
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xc
	.long	.LASF56
	.byte	0x5
	.byte	0xbc
	.long	0x36f
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0xc
	.long	.LASF57
	.byte	0x5
	.byte	0xc0
	.long	0x71
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x332
	.uleb128 0x6
	.byte	0x8
	.long	0xab
	.uleb128 0x6
	.byte	0x8
	.long	0x332
	.uleb128 0x6
	.byte	0x8
	.long	0xab
	.uleb128 0xd
	.long	0xa4
	.long	0x391
	.uleb128 0xe
	.long	0x95
	.byte	0
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x32b
	.uleb128 0xd
	.long	0xa4
	.long	0x3a7
	.uleb128 0xe
	.long	0x95
	.byte	0x13
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x3ad
	.uleb128 0x11
	.long	0xa4
	.uleb128 0x3
	.byte	0x8
	.byte	0x4
	.long	.LASF58
	.uleb128 0x12
	.long	.LASF59
	.byte	0x9
	.value	0x161
	.long	0x3c5
	.uleb128 0x6
	.byte	0x8
	.long	0x3cb
	.uleb128 0x13
	.long	.LASF267
	.byte	0x1
	.uleb128 0x3
	.byte	0x4
	.byte	0x4
	.long	.LASF60
	.uleb128 0x3
	.byte	0x10
	.byte	0x4
	.long	.LASF61
	.uleb128 0x3
	.byte	0x10
	.byte	0x4
	.long	.LASF62
	.uleb128 0x14
	.string	"std"
	.byte	0x8
	.byte	0
	.long	0xd2f
	.uleb128 0x15
	.byte	0xa
	.byte	0x42
	.long	0xe79
	.uleb128 0x15
	.byte	0xa
	.byte	0x8d
	.long	0xe6d
	.uleb128 0x15
	.byte	0xa
	.byte	0x8f
	.long	0xe89
	.uleb128 0x15
	.byte	0xa
	.byte	0x90
	.long	0xea1
	.uleb128 0x15
	.byte	0xa
	.byte	0x91
	.long	0xebf
	.uleb128 0x15
	.byte	0xa
	.byte	0x92
	.long	0xeee
	.uleb128 0x15
	.byte	0xa
	.byte	0x93
	.long	0xf0b
	.uleb128 0x15
	.byte	0xa
	.byte	0x94
	.long	0xf33
	.uleb128 0x15
	.byte	0xa
	.byte	0x95
	.long	0xf50
	.uleb128 0x15
	.byte	0xa
	.byte	0x96
	.long	0xf6e
	.uleb128 0x15
	.byte	0xa
	.byte	0x97
	.long	0xf8c
	.uleb128 0x15
	.byte	0xa
	.byte	0x98
	.long	0xfa4
	.uleb128 0x15
	.byte	0xa
	.byte	0x99
	.long	0xfb2
	.uleb128 0x15
	.byte	0xa
	.byte	0x9a
	.long	0xfda
	.uleb128 0x15
	.byte	0xa
	.byte	0x9b
	.long	0x1001
	.uleb128 0x15
	.byte	0xa
	.byte	0x9c
	.long	0x1024
	.uleb128 0x15
	.byte	0xa
	.byte	0x9d
	.long	0x1051
	.uleb128 0x15
	.byte	0xa
	.byte	0x9e
	.long	0x106e
	.uleb128 0x15
	.byte	0xa
	.byte	0xa0
	.long	0x1086
	.uleb128 0x15
	.byte	0xa
	.byte	0xa2
	.long	0x10a9
	.uleb128 0x15
	.byte	0xa
	.byte	0xa3
	.long	0x10c7
	.uleb128 0x15
	.byte	0xa
	.byte	0xa4
	.long	0x10e4
	.uleb128 0x15
	.byte	0xa
	.byte	0xa6
	.long	0x110c
	.uleb128 0x15
	.byte	0xa
	.byte	0xa9
	.long	0x112e
	.uleb128 0x15
	.byte	0xa
	.byte	0xac
	.long	0x1155
	.uleb128 0x15
	.byte	0xa
	.byte	0xae
	.long	0x1177
	.uleb128 0x15
	.byte	0xa
	.byte	0xb0
	.long	0x1194
	.uleb128 0x15
	.byte	0xa
	.byte	0xb2
	.long	0x11b1
	.uleb128 0x15
	.byte	0xa
	.byte	0xb3
	.long	0x11d3
	.uleb128 0x15
	.byte	0xa
	.byte	0xb4
	.long	0x11ef
	.uleb128 0x15
	.byte	0xa
	.byte	0xb5
	.long	0x120b
	.uleb128 0x15
	.byte	0xa
	.byte	0xb6
	.long	0x1227
	.uleb128 0x15
	.byte	0xa
	.byte	0xb7
	.long	0x1243
	.uleb128 0x15
	.byte	0xa
	.byte	0xb8
	.long	0x125f
	.uleb128 0x15
	.byte	0xa
	.byte	0xb9
	.long	0x1337
	.uleb128 0x15
	.byte	0xa
	.byte	0xba
	.long	0x134f
	.uleb128 0x15
	.byte	0xa
	.byte	0xbb
	.long	0x1370
	.uleb128 0x15
	.byte	0xa
	.byte	0xbc
	.long	0x1391
	.uleb128 0x15
	.byte	0xa
	.byte	0xbd
	.long	0x13b2
	.uleb128 0x15
	.byte	0xa
	.byte	0xbe
	.long	0x13df
	.uleb128 0x15
	.byte	0xa
	.byte	0xbf
	.long	0x13fb
	.uleb128 0x15
	.byte	0xa
	.byte	0xc1
	.long	0x141e
	.uleb128 0x15
	.byte	0xa
	.byte	0xc3
	.long	0x143b
	.uleb128 0x15
	.byte	0xa
	.byte	0xc4
	.long	0x145d
	.uleb128 0x15
	.byte	0xa
	.byte	0xc5
	.long	0x147f
	.uleb128 0x15
	.byte	0xa
	.byte	0xc6
	.long	0x14a1
	.uleb128 0x15
	.byte	0xa
	.byte	0xc7
	.long	0x14c2
	.uleb128 0x15
	.byte	0xa
	.byte	0xc8
	.long	0x14da
	.uleb128 0x15
	.byte	0xa
	.byte	0xc9
	.long	0x14fc
	.uleb128 0x15
	.byte	0xa
	.byte	0xca
	.long	0x151e
	.uleb128 0x15
	.byte	0xa
	.byte	0xcb
	.long	0x1540
	.uleb128 0x15
	.byte	0xa
	.byte	0xcc
	.long	0x1562
	.uleb128 0x15
	.byte	0xa
	.byte	0xcd
	.long	0x157b
	.uleb128 0x15
	.byte	0xa
	.byte	0xce
	.long	0x1594
	.uleb128 0x15
	.byte	0xa
	.byte	0xcf
	.long	0x15b4
	.uleb128 0x15
	.byte	0xa
	.byte	0xd0
	.long	0x15d5
	.uleb128 0x15
	.byte	0xa
	.byte	0xd1
	.long	0x15f5
	.uleb128 0x15
	.byte	0xa
	.byte	0xd2
	.long	0x1616
	.uleb128 0x16
	.byte	0xa
	.value	0x10a
	.long	0x163c
	.uleb128 0x16
	.byte	0xa
	.value	0x10b
	.long	0x1659
	.uleb128 0x16
	.byte	0xa
	.value	0x10c
	.long	0x167b
	.uleb128 0x17
	.long	.LASF268
	.byte	0x12
	.byte	0x31
	.uleb128 0x2
	.long	.LASF6
	.byte	0xb
	.byte	0xad
	.long	0x3c
	.uleb128 0x15
	.byte	0xc
	.byte	0x37
	.long	0x16b7
	.uleb128 0x15
	.byte	0xc
	.byte	0x38
	.long	0x1814
	.uleb128 0x15
	.byte	0xc
	.byte	0x39
	.long	0x1830
	.uleb128 0x2
	.long	.LASF63
	.byte	0xb
	.byte	0xae
	.long	0x78
	.uleb128 0x15
	.byte	0xd
	.byte	0x54
	.long	0x184e
	.uleb128 0x15
	.byte	0xd
	.byte	0x55
	.long	0x1843
	.uleb128 0x15
	.byte	0xd
	.byte	0x56
	.long	0xe6d
	.uleb128 0x15
	.byte	0xd
	.byte	0x5e
	.long	0x1864
	.uleb128 0x15
	.byte	0xd
	.byte	0x67
	.long	0x1880
	.uleb128 0x15
	.byte	0xd
	.byte	0x6a
	.long	0x189c
	.uleb128 0x15
	.byte	0xd
	.byte	0x6b
	.long	0x18b3
	.uleb128 0x7
	.long	.LASF64
	.byte	0x8
	.byte	0x1
	.value	0x411
	.long	0x857
	.uleb128 0x12
	.long	.LASF65
	.byte	0x1
	.value	0x414
	.long	0x18d4
	.uleb128 0x18
	.long	.LASF87
	.byte	0x1
	.value	0x49e
	.long	0x60f
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0x3
	.uleb128 0x19
	.byte	0x1
	.long	.LASF66
	.byte	0x1
	.value	0x416
	.byte	0x1
	.long	0x63d
	.long	0x649
	.uleb128 0x1a
	.long	0x18db
	.byte	0x1
	.uleb128 0x1b
	.long	0x60f
	.byte	0
	.uleb128 0x19
	.byte	0x1
	.long	.LASF66
	.byte	0x1
	.value	0x418
	.byte	0x1
	.long	0x65b
	.long	0x66c
	.uleb128 0x1a
	.long	0x18db
	.byte	0x1
	.uleb128 0x1b
	.long	0x3d1
	.uleb128 0x1b
	.long	0x3d1
	.byte	0
	.uleb128 0x1c
	.byte	0x1
	.long	.LASF66
	.byte	0x1
	.value	0x422
	.byte	0x1
	.byte	0x1
	.long	0x67f
	.long	0x68b
	.uleb128 0x1a
	.long	0x18db
	.byte	0x1
	.uleb128 0x1b
	.long	0x18e1
	.byte	0
	.uleb128 0x1c
	.byte	0x1
	.long	.LASF66
	.byte	0x1
	.value	0x423
	.byte	0x1
	.byte	0x1
	.long	0x69e
	.long	0x6aa
	.uleb128 0x1a
	.long	0x18db
	.byte	0x1
	.uleb128 0x1b
	.long	0x18e7
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF67
	.byte	0x1
	.value	0x42f
	.long	.LASF68
	.long	0x18ed
	.byte	0x1
	.long	0x6c4
	.long	0x6cb
	.uleb128 0x1a
	.long	0x18db
	.byte	0x1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF67
	.byte	0x1
	.value	0x432
	.long	.LASF69
	.long	0x18f3
	.byte	0x1
	.long	0x6e5
	.long	0x6ec
	.uleb128 0x1a
	.long	0x18fe
	.byte	0x1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF70
	.byte	0x1
	.value	0x435
	.long	.LASF71
	.long	0x18ed
	.byte	0x1
	.long	0x706
	.long	0x70d
	.uleb128 0x1a
	.long	0x18db
	.byte	0x1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF70
	.byte	0x1
	.value	0x438
	.long	.LASF72
	.long	0x18f3
	.byte	0x1
	.long	0x727
	.long	0x72e
	.uleb128 0x1a
	.long	0x18fe
	.byte	0x1
	.byte	0
	.uleb128 0x1e
	.byte	0x1
	.long	.LASF67
	.byte	0x1
	.value	0x43e
	.long	.LASF73
	.byte	0x1
	.long	0x744
	.long	0x750
	.uleb128 0x1a
	.long	0x18db
	.byte	0x1
	.uleb128 0x1b
	.long	0x3d1
	.byte	0
	.uleb128 0x1e
	.byte	0x1
	.long	.LASF70
	.byte	0x1
	.value	0x441
	.long	.LASF74
	.byte	0x1
	.long	0x766
	.long	0x772
	.uleb128 0x1a
	.long	0x18db
	.byte	0x1
	.uleb128 0x1b
	.long	0x3d1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF75
	.byte	0x1
	.value	0x444
	.long	.LASF76
	.long	0x1904
	.byte	0x1
	.long	0x78c
	.long	0x798
	.uleb128 0x1a
	.long	0x18db
	.byte	0x1
	.uleb128 0x1b
	.long	0x3d1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF77
	.byte	0x1
	.value	0x44b
	.long	.LASF78
	.long	0x1904
	.byte	0x1
	.long	0x7b2
	.long	0x7be
	.uleb128 0x1a
	.long	0x18db
	.byte	0x1
	.uleb128 0x1b
	.long	0x3d1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF79
	.byte	0x1
	.value	0x452
	.long	.LASF80
	.long	0x1904
	.byte	0x1
	.long	0x7d8
	.long	0x7e4
	.uleb128 0x1a
	.long	0x18db
	.byte	0x1
	.uleb128 0x1b
	.long	0x3d1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF81
	.byte	0x1
	.value	0x459
	.long	.LASF82
	.long	0x1904
	.byte	0x1
	.long	0x7fe
	.long	0x80a
	.uleb128 0x1a
	.long	0x18db
	.byte	0x1
	.uleb128 0x1b
	.long	0x3d1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF83
	.byte	0x1
	.value	0x460
	.long	.LASF84
	.long	0x1904
	.byte	0x1
	.long	0x824
	.long	0x830
	.uleb128 0x1a
	.long	0x18db
	.byte	0x1
	.uleb128 0x1b
	.long	0x3d1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF85
	.byte	0x1
	.value	0x49b
	.long	.LASF86
	.long	0x851
	.byte	0x1
	.long	0x84a
	.long	0x851
	.uleb128 0x1a
	.long	0x18fe
	.byte	0x1
	.byte	0
	.uleb128 0x11
	.long	0x60f
	.byte	0
	.uleb128 0x11
	.long	0x85c
	.uleb128 0x1f
	.long	.LASF100
	.byte	0x10
	.byte	0x1
	.value	0x4a4
	.long	0xab0
	.uleb128 0x12
	.long	.LASF65
	.byte	0x1
	.value	0x4a7
	.long	0x190a
	.uleb128 0x18
	.long	.LASF87
	.byte	0x1
	.value	0x532
	.long	0x869
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0x3
	.uleb128 0x19
	.byte	0x1
	.long	.LASF66
	.byte	0x1
	.value	0x4a9
	.byte	0x1
	.long	0x897
	.long	0x8a3
	.uleb128 0x1a
	.long	0x1911
	.byte	0x1
	.uleb128 0x1b
	.long	0x869
	.byte	0
	.uleb128 0x19
	.byte	0x1
	.long	.LASF66
	.byte	0x1
	.value	0x4ab
	.byte	0x1
	.long	0x8b5
	.long	0x8c6
	.uleb128 0x1a
	.long	0x1911
	.byte	0x1
	.uleb128 0x1b
	.long	0x3b2
	.uleb128 0x1b
	.long	0x3b2
	.byte	0
	.uleb128 0x19
	.byte	0x1
	.long	.LASF66
	.byte	0x1
	.value	0x4b5
	.byte	0x1
	.long	0x8d8
	.long	0x8e4
	.uleb128 0x1a
	.long	0x1911
	.byte	0x1
	.uleb128 0x1b
	.long	0x1917
	.byte	0
	.uleb128 0x1c
	.byte	0x1
	.long	.LASF66
	.byte	0x1
	.value	0x4b8
	.byte	0x1
	.byte	0x1
	.long	0x8f7
	.long	0x903
	.uleb128 0x1a
	.long	0x1911
	.byte	0x1
	.uleb128 0x1b
	.long	0x18e7
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF67
	.byte	0x1
	.value	0x4c4
	.long	.LASF88
	.long	0x191d
	.byte	0x1
	.long	0x91d
	.long	0x924
	.uleb128 0x1a
	.long	0x1911
	.byte	0x1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF67
	.byte	0x1
	.value	0x4c7
	.long	.LASF89
	.long	0x1923
	.byte	0x1
	.long	0x93e
	.long	0x945
	.uleb128 0x1a
	.long	0x192e
	.byte	0x1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF70
	.byte	0x1
	.value	0x4ca
	.long	.LASF90
	.long	0x191d
	.byte	0x1
	.long	0x95f
	.long	0x966
	.uleb128 0x1a
	.long	0x1911
	.byte	0x1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF70
	.byte	0x1
	.value	0x4cd
	.long	.LASF91
	.long	0x1923
	.byte	0x1
	.long	0x980
	.long	0x987
	.uleb128 0x1a
	.long	0x192e
	.byte	0x1
	.byte	0
	.uleb128 0x1e
	.byte	0x1
	.long	.LASF67
	.byte	0x1
	.value	0x4d3
	.long	.LASF92
	.byte	0x1
	.long	0x99d
	.long	0x9a9
	.uleb128 0x1a
	.long	0x1911
	.byte	0x1
	.uleb128 0x1b
	.long	0x3b2
	.byte	0
	.uleb128 0x1e
	.byte	0x1
	.long	.LASF70
	.byte	0x1
	.value	0x4d6
	.long	.LASF93
	.byte	0x1
	.long	0x9bf
	.long	0x9cb
	.uleb128 0x1a
	.long	0x1911
	.byte	0x1
	.uleb128 0x1b
	.long	0x3b2
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF75
	.byte	0x1
	.value	0x4d9
	.long	.LASF94
	.long	0x1934
	.byte	0x1
	.long	0x9e5
	.long	0x9f1
	.uleb128 0x1a
	.long	0x1911
	.byte	0x1
	.uleb128 0x1b
	.long	0x3b2
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF77
	.byte	0x1
	.value	0x4e0
	.long	.LASF95
	.long	0x1934
	.byte	0x1
	.long	0xa0b
	.long	0xa17
	.uleb128 0x1a
	.long	0x1911
	.byte	0x1
	.uleb128 0x1b
	.long	0x3b2
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF79
	.byte	0x1
	.value	0x4e7
	.long	.LASF96
	.long	0x1934
	.byte	0x1
	.long	0xa31
	.long	0xa3d
	.uleb128 0x1a
	.long	0x1911
	.byte	0x1
	.uleb128 0x1b
	.long	0x3b2
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF81
	.byte	0x1
	.value	0x4ee
	.long	.LASF97
	.long	0x1934
	.byte	0x1
	.long	0xa57
	.long	0xa63
	.uleb128 0x1a
	.long	0x1911
	.byte	0x1
	.uleb128 0x1b
	.long	0x3b2
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF83
	.byte	0x1
	.value	0x4f5
	.long	.LASF98
	.long	0x1934
	.byte	0x1
	.long	0xa7d
	.long	0xa89
	.uleb128 0x1a
	.long	0x1911
	.byte	0x1
	.uleb128 0x1b
	.long	0x3b2
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF85
	.byte	0x1
	.value	0x52f
	.long	.LASF99
	.long	0xaaa
	.byte	0x1
	.long	0xaa3
	.long	0xaaa
	.uleb128 0x1a
	.long	0x192e
	.byte	0x1
	.byte	0
	.uleb128 0x11
	.long	0x869
	.byte	0
	.uleb128 0x11
	.long	0xab5
	.uleb128 0x1f
	.long	.LASF101
	.byte	0x20
	.byte	0x1
	.value	0x538
	.long	0xd08
	.uleb128 0x12
	.long	.LASF65
	.byte	0x1
	.value	0x53b
	.long	0x193a
	.uleb128 0x18
	.long	.LASF87
	.byte	0x1
	.value	0x5c8
	.long	0xac2
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0x3
	.uleb128 0x19
	.byte	0x1
	.long	.LASF66
	.byte	0x1
	.value	0x53d
	.byte	0x1
	.long	0xaf0
	.long	0xafc
	.uleb128 0x1a
	.long	0x1941
	.byte	0x1
	.uleb128 0x1b
	.long	0xac2
	.byte	0
	.uleb128 0x19
	.byte	0x1
	.long	.LASF66
	.byte	0x1
	.value	0x53f
	.byte	0x1
	.long	0xb0e
	.long	0xb1f
	.uleb128 0x1a
	.long	0x1941
	.byte	0x1
	.uleb128 0x1b
	.long	0x3d8
	.uleb128 0x1b
	.long	0x3d8
	.byte	0
	.uleb128 0x19
	.byte	0x1
	.long	.LASF66
	.byte	0x1
	.value	0x54a
	.byte	0x1
	.long	0xb31
	.long	0xb3d
	.uleb128 0x1a
	.long	0x1941
	.byte	0x1
	.uleb128 0x1b
	.long	0x1917
	.byte	0
	.uleb128 0x19
	.byte	0x1
	.long	.LASF66
	.byte	0x1
	.value	0x54d
	.byte	0x1
	.long	0xb4f
	.long	0xb5b
	.uleb128 0x1a
	.long	0x1941
	.byte	0x1
	.uleb128 0x1b
	.long	0x18e1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF67
	.byte	0x1
	.value	0x55a
	.long	.LASF102
	.long	0x1947
	.byte	0x1
	.long	0xb75
	.long	0xb7c
	.uleb128 0x1a
	.long	0x1941
	.byte	0x1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF67
	.byte	0x1
	.value	0x55d
	.long	.LASF103
	.long	0x194d
	.byte	0x1
	.long	0xb96
	.long	0xb9d
	.uleb128 0x1a
	.long	0x1958
	.byte	0x1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF70
	.byte	0x1
	.value	0x560
	.long	.LASF104
	.long	0x1947
	.byte	0x1
	.long	0xbb7
	.long	0xbbe
	.uleb128 0x1a
	.long	0x1941
	.byte	0x1
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF70
	.byte	0x1
	.value	0x563
	.long	.LASF105
	.long	0x194d
	.byte	0x1
	.long	0xbd8
	.long	0xbdf
	.uleb128 0x1a
	.long	0x1958
	.byte	0x1
	.byte	0
	.uleb128 0x1e
	.byte	0x1
	.long	.LASF67
	.byte	0x1
	.value	0x569
	.long	.LASF106
	.byte	0x1
	.long	0xbf5
	.long	0xc01
	.uleb128 0x1a
	.long	0x1941
	.byte	0x1
	.uleb128 0x1b
	.long	0x3d8
	.byte	0
	.uleb128 0x1e
	.byte	0x1
	.long	.LASF70
	.byte	0x1
	.value	0x56c
	.long	.LASF107
	.byte	0x1
	.long	0xc17
	.long	0xc23
	.uleb128 0x1a
	.long	0x1941
	.byte	0x1
	.uleb128 0x1b
	.long	0x3d8
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF75
	.byte	0x1
	.value	0x56f
	.long	.LASF108
	.long	0x195e
	.byte	0x1
	.long	0xc3d
	.long	0xc49
	.uleb128 0x1a
	.long	0x1941
	.byte	0x1
	.uleb128 0x1b
	.long	0x3d8
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF77
	.byte	0x1
	.value	0x576
	.long	.LASF109
	.long	0x195e
	.byte	0x1
	.long	0xc63
	.long	0xc6f
	.uleb128 0x1a
	.long	0x1941
	.byte	0x1
	.uleb128 0x1b
	.long	0x3d8
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF79
	.byte	0x1
	.value	0x57d
	.long	.LASF110
	.long	0x195e
	.byte	0x1
	.long	0xc89
	.long	0xc95
	.uleb128 0x1a
	.long	0x1941
	.byte	0x1
	.uleb128 0x1b
	.long	0x3d8
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF81
	.byte	0x1
	.value	0x584
	.long	.LASF111
	.long	0x195e
	.byte	0x1
	.long	0xcaf
	.long	0xcbb
	.uleb128 0x1a
	.long	0x1941
	.byte	0x1
	.uleb128 0x1b
	.long	0x3d8
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF83
	.byte	0x1
	.value	0x58b
	.long	.LASF112
	.long	0x195e
	.byte	0x1
	.long	0xcd5
	.long	0xce1
	.uleb128 0x1a
	.long	0x1941
	.byte	0x1
	.uleb128 0x1b
	.long	0x3d8
	.byte	0
	.uleb128 0x1d
	.byte	0x1
	.long	.LASF85
	.byte	0x1
	.value	0x5c5
	.long	.LASF113
	.long	0xd02
	.byte	0x1
	.long	0xcfb
	.long	0xd02
	.uleb128 0x1a
	.long	0x1958
	.byte	0x1
	.byte	0
	.uleb128 0x11
	.long	0xac2
	.byte	0
	.uleb128 0x11
	.long	0x602
	.uleb128 0x20
	.byte	0x1
	.long	.LASF114
	.byte	0x1
	.value	0x221
	.long	.LASF269
	.long	0x191d
	.byte	0x1
	.uleb128 0x21
	.string	"_Tp"
	.long	0x3b2
	.uleb128 0x1b
	.long	0x1d82
	.byte	0
	.byte	0
	.uleb128 0x22
	.long	.LASF115
	.byte	0x11
	.byte	0x46
	.long	0xe5f
	.uleb128 0x15
	.byte	0xa
	.byte	0xfa
	.long	0x163c
	.uleb128 0x16
	.byte	0xa
	.value	0x103
	.long	0x1659
	.uleb128 0x16
	.byte	0xa
	.value	0x104
	.long	0x167b
	.uleb128 0x15
	.byte	0xe
	.byte	0x2a
	.long	0x5a6
	.uleb128 0x15
	.byte	0xe
	.byte	0x2b
	.long	0x5c6
	.uleb128 0xf
	.long	.LASF116
	.byte	0x1
	.byte	0xf
	.byte	0x37
	.long	0xd98
	.uleb128 0x23
	.long	.LASF117
	.byte	0xf
	.byte	0x3a
	.long	0xe84
	.byte	0x1
	.byte	0x1
	.uleb128 0x23
	.long	.LASF118
	.byte	0xf
	.byte	0x3b
	.long	0xe84
	.byte	0x1
	.byte	0x1
	.uleb128 0x24
	.long	.LASF119
	.long	0x71
	.uleb128 0x24
	.long	.LASF119
	.long	0x71
	.byte	0
	.uleb128 0xf
	.long	.LASF120
	.byte	0x1
	.byte	0xf
	.byte	0x37
	.long	0xdc4
	.uleb128 0x23
	.long	.LASF121
	.byte	0xf
	.byte	0x40
	.long	0xe84
	.byte	0x1
	.byte	0x1
	.uleb128 0x24
	.long	.LASF119
	.long	0x3c
	.uleb128 0x24
	.long	.LASF119
	.long	0x3c
	.byte	0
	.uleb128 0xf
	.long	.LASF122
	.byte	0x1
	.byte	0xf
	.byte	0x37
	.long	0xdf0
	.uleb128 0x23
	.long	.LASF118
	.byte	0xf
	.byte	0x3b
	.long	0x3ad
	.byte	0x1
	.byte	0x1
	.uleb128 0x24
	.long	.LASF119
	.long	0xa4
	.uleb128 0x24
	.long	.LASF119
	.long	0xa4
	.byte	0
	.uleb128 0xf
	.long	.LASF123
	.byte	0x1
	.byte	0xf
	.byte	0x37
	.long	0xe29
	.uleb128 0x23
	.long	.LASF117
	.byte	0xf
	.byte	0x3a
	.long	0x18ca
	.byte	0x1
	.byte	0x1
	.uleb128 0x23
	.long	.LASF118
	.byte	0xf
	.byte	0x3b
	.long	0x18ca
	.byte	0x1
	.byte	0x1
	.uleb128 0x24
	.long	.LASF119
	.long	0x5f
	.uleb128 0x24
	.long	.LASF119
	.long	0x5f
	.byte	0
	.uleb128 0x25
	.long	.LASF270
	.byte	0x1
	.byte	0xf
	.byte	0x37
	.uleb128 0x23
	.long	.LASF117
	.byte	0xf
	.byte	0x3a
	.long	0x18cf
	.byte	0x1
	.byte	0x1
	.uleb128 0x23
	.long	.LASF118
	.byte	0xf
	.byte	0x3b
	.long	0x18cf
	.byte	0x1
	.byte	0x1
	.uleb128 0x24
	.long	.LASF119
	.long	0x78
	.uleb128 0x24
	.long	.LASF119
	.long	0x78
	.byte	0
	.byte	0
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF124
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.long	.LASF125
	.uleb128 0x12
	.long	.LASF126
	.byte	0x3
	.value	0x162
	.long	0x51
	.uleb128 0x2
	.long	.LASF127
	.byte	0x7
	.byte	0x6a
	.long	0x2db
	.uleb128 0x11
	.long	0x71
	.uleb128 0x26
	.byte	0x1
	.long	.LASF128
	.byte	0x7
	.value	0x15f
	.long	0xe6d
	.byte	0x1
	.long	0xea1
	.uleb128 0x1b
	.long	0x71
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF129
	.byte	0x7
	.value	0x2e7
	.long	0xe6d
	.byte	0x1
	.long	0xeb9
	.uleb128 0x1b
	.long	0xeb9
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x278
	.uleb128 0x26
	.byte	0x1
	.long	.LASF130
	.byte	0x7
	.value	0x304
	.long	0xee1
	.byte	0x1
	.long	0xee1
	.uleb128 0x1b
	.long	0xee1
	.uleb128 0x1b
	.long	0x71
	.uleb128 0x1b
	.long	0xeb9
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0xee7
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.long	.LASF131
	.uleb128 0x26
	.byte	0x1
	.long	.LASF132
	.byte	0x7
	.value	0x2f5
	.long	0xe6d
	.byte	0x1
	.long	0xf0b
	.uleb128 0x1b
	.long	0xee7
	.uleb128 0x1b
	.long	0xeb9
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF133
	.byte	0x7
	.value	0x30b
	.long	0x71
	.byte	0x1
	.long	0xf28
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0xeb9
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0xf2e
	.uleb128 0x11
	.long	0xee7
	.uleb128 0x26
	.byte	0x1
	.long	.LASF134
	.byte	0x7
	.value	0x249
	.long	0x71
	.byte	0x1
	.long	0xf50
	.uleb128 0x1b
	.long	0xeb9
	.uleb128 0x1b
	.long	0x71
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF135
	.byte	0x7
	.value	0x250
	.long	0x71
	.byte	0x1
	.long	0xf6e
	.uleb128 0x1b
	.long	0xeb9
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x27
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF136
	.byte	0x7
	.value	0x279
	.long	0x71
	.byte	0x1
	.long	0xf8c
	.uleb128 0x1b
	.long	0xeb9
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x27
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF137
	.byte	0x7
	.value	0x2e8
	.long	0xe6d
	.byte	0x1
	.long	0xfa4
	.uleb128 0x1b
	.long	0xeb9
	.byte	0
	.uleb128 0x28
	.byte	0x1
	.long	.LASF226
	.byte	0x7
	.value	0x2ee
	.long	0xe6d
	.byte	0x1
	.uleb128 0x26
	.byte	0x1
	.long	.LASF138
	.byte	0x7
	.value	0x176
	.long	0x31
	.byte	0x1
	.long	0xfd4
	.uleb128 0x1b
	.long	0x3a7
	.uleb128 0x1b
	.long	0x31
	.uleb128 0x1b
	.long	0xfd4
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0xe79
	.uleb128 0x26
	.byte	0x1
	.long	.LASF139
	.byte	0x7
	.value	0x16b
	.long	0x31
	.byte	0x1
	.long	0x1001
	.uleb128 0x1b
	.long	0xee1
	.uleb128 0x1b
	.long	0x3a7
	.uleb128 0x1b
	.long	0x31
	.uleb128 0x1b
	.long	0xfd4
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF140
	.byte	0x7
	.value	0x167
	.long	0x71
	.byte	0x1
	.long	0x1019
	.uleb128 0x1b
	.long	0x1019
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x101f
	.uleb128 0x11
	.long	0xe79
	.uleb128 0x26
	.byte	0x1
	.long	.LASF141
	.byte	0x7
	.value	0x196
	.long	0x31
	.byte	0x1
	.long	0x104b
	.uleb128 0x1b
	.long	0xee1
	.uleb128 0x1b
	.long	0x104b
	.uleb128 0x1b
	.long	0x31
	.uleb128 0x1b
	.long	0xfd4
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x3a7
	.uleb128 0x26
	.byte	0x1
	.long	.LASF142
	.byte	0x7
	.value	0x2f6
	.long	0xe6d
	.byte	0x1
	.long	0x106e
	.uleb128 0x1b
	.long	0xee7
	.uleb128 0x1b
	.long	0xeb9
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF143
	.byte	0x7
	.value	0x2fc
	.long	0xe6d
	.byte	0x1
	.long	0x1086
	.uleb128 0x1b
	.long	0xee7
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF144
	.byte	0x7
	.value	0x25a
	.long	0x71
	.byte	0x1
	.long	0x10a9
	.uleb128 0x1b
	.long	0xee1
	.uleb128 0x1b
	.long	0x31
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x27
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF145
	.byte	0x7
	.value	0x283
	.long	0x71
	.byte	0x1
	.long	0x10c7
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x27
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF146
	.byte	0x7
	.value	0x313
	.long	0xe6d
	.byte	0x1
	.long	0x10e4
	.uleb128 0x1b
	.long	0xe6d
	.uleb128 0x1b
	.long	0xeb9
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF147
	.byte	0x7
	.value	0x262
	.long	0x71
	.byte	0x1
	.long	0x1106
	.uleb128 0x1b
	.long	0xeb9
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x1106
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x2e6
	.uleb128 0x26
	.byte	0x1
	.long	.LASF148
	.byte	0x7
	.value	0x2af
	.long	0x71
	.byte	0x1
	.long	0x112e
	.uleb128 0x1b
	.long	0xeb9
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x1106
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF149
	.byte	0x7
	.value	0x26f
	.long	0x71
	.byte	0x1
	.long	0x1155
	.uleb128 0x1b
	.long	0xee1
	.uleb128 0x1b
	.long	0x31
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x1106
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF150
	.byte	0x7
	.value	0x2bb
	.long	0x71
	.byte	0x1
	.long	0x1177
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x1106
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF151
	.byte	0x7
	.value	0x26a
	.long	0x71
	.byte	0x1
	.long	0x1194
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x1106
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF152
	.byte	0x7
	.value	0x2b7
	.long	0x71
	.byte	0x1
	.long	0x11b1
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x1106
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF153
	.byte	0x7
	.value	0x170
	.long	0x31
	.byte	0x1
	.long	0x11d3
	.uleb128 0x1b
	.long	0x9e
	.uleb128 0x1b
	.long	0xee7
	.uleb128 0x1b
	.long	0xfd4
	.byte	0
	.uleb128 0x29
	.byte	0x1
	.long	.LASF154
	.byte	0x7
	.byte	0x98
	.long	0xee1
	.byte	0x1
	.long	0x11ef
	.uleb128 0x1b
	.long	0xee1
	.uleb128 0x1b
	.long	0xf28
	.byte	0
	.uleb128 0x29
	.byte	0x1
	.long	.LASF155
	.byte	0x7
	.byte	0xa0
	.long	0x71
	.byte	0x1
	.long	0x120b
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0xf28
	.byte	0
	.uleb128 0x29
	.byte	0x1
	.long	.LASF156
	.byte	0x7
	.byte	0xbd
	.long	0x71
	.byte	0x1
	.long	0x1227
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0xf28
	.byte	0
	.uleb128 0x29
	.byte	0x1
	.long	.LASF157
	.byte	0x7
	.byte	0x90
	.long	0xee1
	.byte	0x1
	.long	0x1243
	.uleb128 0x1b
	.long	0xee1
	.uleb128 0x1b
	.long	0xf28
	.byte	0
	.uleb128 0x29
	.byte	0x1
	.long	.LASF158
	.byte	0x7
	.byte	0xf9
	.long	0x31
	.byte	0x1
	.long	0x125f
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0xf28
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF159
	.byte	0x7
	.value	0x355
	.long	0x31
	.byte	0x1
	.long	0x1286
	.uleb128 0x1b
	.long	0xee1
	.uleb128 0x1b
	.long	0x31
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x1286
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x128c
	.uleb128 0x11
	.long	0x1291
	.uleb128 0x2a
	.string	"tm"
	.byte	0x38
	.byte	0x10
	.byte	0x85
	.long	0x1337
	.uleb128 0xc
	.long	.LASF160
	.byte	0x10
	.byte	0x87
	.long	0x71
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xc
	.long	.LASF161
	.byte	0x10
	.byte	0x88
	.long	0x71
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0xc
	.long	.LASF162
	.byte	0x10
	.byte	0x89
	.long	0x71
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0xc
	.long	.LASF163
	.byte	0x10
	.byte	0x8a
	.long	0x71
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0xc
	.long	.LASF164
	.byte	0x10
	.byte	0x8b
	.long	0x71
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0xc
	.long	.LASF165
	.byte	0x10
	.byte	0x8c
	.long	0x71
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0xc
	.long	.LASF166
	.byte	0x10
	.byte	0x8d
	.long	0x71
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0xc
	.long	.LASF167
	.byte	0x10
	.byte	0x8e
	.long	0x71
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0xc
	.long	.LASF168
	.byte	0x10
	.byte	0x8f
	.long	0x71
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0xc
	.long	.LASF169
	.byte	0x10
	.byte	0x92
	.long	0x78
	.byte	0x2
	.byte	0x23
	.uleb128 0x28
	.uleb128 0xc
	.long	.LASF170
	.byte	0x10
	.byte	0x93
	.long	0x3a7
	.byte	0x2
	.byte	0x23
	.uleb128 0x30
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF171
	.byte	0x7
	.value	0x11c
	.long	0x31
	.byte	0x1
	.long	0x134f
	.uleb128 0x1b
	.long	0xf28
	.byte	0
	.uleb128 0x29
	.byte	0x1
	.long	.LASF172
	.byte	0x7
	.byte	0x9b
	.long	0xee1
	.byte	0x1
	.long	0x1370
	.uleb128 0x1b
	.long	0xee1
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x31
	.byte	0
	.uleb128 0x29
	.byte	0x1
	.long	.LASF173
	.byte	0x7
	.byte	0xa3
	.long	0x71
	.byte	0x1
	.long	0x1391
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x31
	.byte	0
	.uleb128 0x29
	.byte	0x1
	.long	.LASF174
	.byte	0x7
	.byte	0x93
	.long	0xee1
	.byte	0x1
	.long	0x13b2
	.uleb128 0x1b
	.long	0xee1
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x31
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF175
	.byte	0x7
	.value	0x19c
	.long	0x31
	.byte	0x1
	.long	0x13d9
	.uleb128 0x1b
	.long	0x9e
	.uleb128 0x1b
	.long	0x13d9
	.uleb128 0x1b
	.long	0x31
	.uleb128 0x1b
	.long	0xfd4
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0xf28
	.uleb128 0x29
	.byte	0x1
	.long	.LASF176
	.byte	0x7
	.byte	0xfd
	.long	0x31
	.byte	0x1
	.long	0x13fb
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0xf28
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF177
	.byte	0x7
	.value	0x1c0
	.long	0x3b2
	.byte	0x1
	.long	0x1418
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x1418
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0xee1
	.uleb128 0x26
	.byte	0x1
	.long	.LASF178
	.byte	0x7
	.value	0x1c7
	.long	0x3d1
	.byte	0x1
	.long	0x143b
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x1418
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF179
	.byte	0x7
	.value	0x117
	.long	0xee1
	.byte	0x1
	.long	0x145d
	.uleb128 0x1b
	.long	0xee1
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x1418
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF180
	.byte	0x7
	.value	0x1d2
	.long	0x78
	.byte	0x1
	.long	0x147f
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x1418
	.uleb128 0x1b
	.long	0x71
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF181
	.byte	0x7
	.value	0x1d7
	.long	0x3c
	.byte	0x1
	.long	0x14a1
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x1418
	.uleb128 0x1b
	.long	0x71
	.byte	0
	.uleb128 0x29
	.byte	0x1
	.long	.LASF182
	.byte	0x7
	.byte	0xc1
	.long	0x31
	.byte	0x1
	.long	0x14c2
	.uleb128 0x1b
	.long	0xee1
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x31
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF183
	.byte	0x7
	.value	0x163
	.long	0x71
	.byte	0x1
	.long	0x14da
	.uleb128 0x1b
	.long	0xe6d
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF184
	.byte	0x7
	.value	0x142
	.long	0x71
	.byte	0x1
	.long	0x14fc
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x31
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF185
	.byte	0x7
	.value	0x147
	.long	0xee1
	.byte	0x1
	.long	0x151e
	.uleb128 0x1b
	.long	0xee1
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x31
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF186
	.byte	0x7
	.value	0x14c
	.long	0xee1
	.byte	0x1
	.long	0x1540
	.uleb128 0x1b
	.long	0xee1
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x31
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF187
	.byte	0x7
	.value	0x150
	.long	0xee1
	.byte	0x1
	.long	0x1562
	.uleb128 0x1b
	.long	0xee1
	.uleb128 0x1b
	.long	0xee7
	.uleb128 0x1b
	.long	0x31
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF188
	.byte	0x7
	.value	0x257
	.long	0x71
	.byte	0x1
	.long	0x157b
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x27
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF189
	.byte	0x7
	.value	0x280
	.long	0x71
	.byte	0x1
	.long	0x1594
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x27
	.byte	0
	.uleb128 0x2b
	.byte	0x1
	.long	.LASF190
	.byte	0x7
	.byte	0xdd
	.long	.LASF190
	.long	0xf28
	.byte	0x1
	.long	0x15b4
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0xee7
	.byte	0
	.uleb128 0x2c
	.byte	0x1
	.long	.LASF191
	.byte	0x7
	.value	0x103
	.long	.LASF191
	.long	0xf28
	.byte	0x1
	.long	0x15d5
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0xf28
	.byte	0
	.uleb128 0x2b
	.byte	0x1
	.long	.LASF192
	.byte	0x7
	.byte	0xe7
	.long	.LASF192
	.long	0xf28
	.byte	0x1
	.long	0x15f5
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0xee7
	.byte	0
	.uleb128 0x2c
	.byte	0x1
	.long	.LASF193
	.byte	0x7
	.value	0x10e
	.long	.LASF193
	.long	0xf28
	.byte	0x1
	.long	0x1616
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0xf28
	.byte	0
	.uleb128 0x2c
	.byte	0x1
	.long	.LASF194
	.byte	0x7
	.value	0x139
	.long	.LASF194
	.long	0xf28
	.byte	0x1
	.long	0x163c
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0xee7
	.uleb128 0x1b
	.long	0x31
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF195
	.byte	0x7
	.value	0x1c9
	.long	0x3d8
	.byte	0x1
	.long	0x1659
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x1418
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF196
	.byte	0x7
	.value	0x1e1
	.long	0xe66
	.byte	0x1
	.long	0x167b
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x1418
	.uleb128 0x1b
	.long	0x71
	.byte	0
	.uleb128 0x26
	.byte	0x1
	.long	.LASF197
	.byte	0x7
	.value	0x1e8
	.long	0xe5f
	.byte	0x1
	.long	0x169d
	.uleb128 0x1b
	.long	0xf28
	.uleb128 0x1b
	.long	0x1418
	.uleb128 0x1b
	.long	0x71
	.byte	0
	.uleb128 0x22
	.long	.LASF198
	.byte	0x12
	.byte	0x38
	.long	0x16b0
	.uleb128 0x2d
	.byte	0x12
	.byte	0x39
	.long	0x59f
	.byte	0
	.uleb128 0x3
	.byte	0x1
	.byte	0x2
	.long	.LASF199
	.uleb128 0xf
	.long	.LASF200
	.byte	0x60
	.byte	0x13
	.byte	0x36
	.long	0x1814
	.uleb128 0xc
	.long	.LASF201
	.byte	0x13
	.byte	0x3a
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xc
	.long	.LASF202
	.byte	0x13
	.byte	0x3b
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0xc
	.long	.LASF203
	.byte	0x13
	.byte	0x41
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0xc
	.long	.LASF204
	.byte	0x13
	.byte	0x47
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0xc
	.long	.LASF205
	.byte	0x13
	.byte	0x48
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0xc
	.long	.LASF206
	.byte	0x13
	.byte	0x49
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x28
	.uleb128 0xc
	.long	.LASF207
	.byte	0x13
	.byte	0x4a
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x30
	.uleb128 0xc
	.long	.LASF208
	.byte	0x13
	.byte	0x4b
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x38
	.uleb128 0xc
	.long	.LASF209
	.byte	0x13
	.byte	0x4c
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x40
	.uleb128 0xc
	.long	.LASF210
	.byte	0x13
	.byte	0x4d
	.long	0x9e
	.byte	0x2
	.byte	0x23
	.uleb128 0x48
	.uleb128 0xc
	.long	.LASF211
	.byte	0x13
	.byte	0x4e
	.long	0xa4
	.byte	0x2
	.byte	0x23
	.uleb128 0x50
	.uleb128 0xc
	.long	.LASF212
	.byte	0x13
	.byte	0x4f
	.long	0xa4
	.byte	0x2
	.byte	0x23
	.uleb128 0x51
	.uleb128 0xc
	.long	.LASF213
	.byte	0x13
	.byte	0x51
	.long	0xa4
	.byte	0x2
	.byte	0x23
	.uleb128 0x52
	.uleb128 0xc
	.long	.LASF214
	.byte	0x13
	.byte	0x53
	.long	0xa4
	.byte	0x2
	.byte	0x23
	.uleb128 0x53
	.uleb128 0xc
	.long	.LASF215
	.byte	0x13
	.byte	0x55
	.long	0xa4
	.byte	0x2
	.byte	0x23
	.uleb128 0x54
	.uleb128 0xc
	.long	.LASF216
	.byte	0x13
	.byte	0x57
	.long	0xa4
	.byte	0x2
	.byte	0x23
	.uleb128 0x55
	.uleb128 0xc
	.long	.LASF217
	.byte	0x13
	.byte	0x5e
	.long	0xa4
	.byte	0x2
	.byte	0x23
	.uleb128 0x56
	.uleb128 0xc
	.long	.LASF218
	.byte	0x13
	.byte	0x5f
	.long	0xa4
	.byte	0x2
	.byte	0x23
	.uleb128 0x57
	.uleb128 0xc
	.long	.LASF219
	.byte	0x13
	.byte	0x62
	.long	0xa4
	.byte	0x2
	.byte	0x23
	.uleb128 0x58
	.uleb128 0xc
	.long	.LASF220
	.byte	0x13
	.byte	0x64
	.long	0xa4
	.byte	0x2
	.byte	0x23
	.uleb128 0x59
	.uleb128 0xc
	.long	.LASF221
	.byte	0x13
	.byte	0x66
	.long	0xa4
	.byte	0x2
	.byte	0x23
	.uleb128 0x5a
	.uleb128 0xc
	.long	.LASF222
	.byte	0x13
	.byte	0x68
	.long	0xa4
	.byte	0x2
	.byte	0x23
	.uleb128 0x5b
	.uleb128 0xc
	.long	.LASF223
	.byte	0x13
	.byte	0x6f
	.long	0xa4
	.byte	0x2
	.byte	0x23
	.uleb128 0x5c
	.uleb128 0xc
	.long	.LASF224
	.byte	0x13
	.byte	0x70
	.long	0xa4
	.byte	0x2
	.byte	0x23
	.uleb128 0x5d
	.byte	0
	.uleb128 0x29
	.byte	0x1
	.long	.LASF225
	.byte	0x13
	.byte	0x7d
	.long	0x9e
	.byte	0x1
	.long	0x1830
	.uleb128 0x1b
	.long	0x71
	.uleb128 0x1b
	.long	0x3a7
	.byte	0
	.uleb128 0x2e
	.byte	0x1
	.long	.LASF227
	.byte	0x13
	.byte	0x80
	.long	0x183d
	.byte	0x1
	.uleb128 0x6
	.byte	0x8
	.long	0x16b7
	.uleb128 0x2
	.long	.LASF228
	.byte	0x14
	.byte	0x35
	.long	0x3c
	.uleb128 0x2
	.long	.LASF229
	.byte	0x14
	.byte	0xbb
	.long	0x1859
	.uleb128 0x6
	.byte	0x8
	.long	0x185f
	.uleb128 0x11
	.long	0x66
	.uleb128 0x29
	.byte	0x1
	.long	.LASF230
	.byte	0x14
	.byte	0xb0
	.long	0x71
	.byte	0x1
	.long	0x1880
	.uleb128 0x1b
	.long	0xe6d
	.uleb128 0x1b
	.long	0x1843
	.byte	0
	.uleb128 0x29
	.byte	0x1
	.long	.LASF231
	.byte	0x14
	.byte	0xde
	.long	0xe6d
	.byte	0x1
	.long	0x189c
	.uleb128 0x1b
	.long	0xe6d
	.uleb128 0x1b
	.long	0x184e
	.byte	0
	.uleb128 0x29
	.byte	0x1
	.long	.LASF232
	.byte	0x14
	.byte	0xdb
	.long	0x184e
	.byte	0x1
	.long	0x18b3
	.uleb128 0x1b
	.long	0x3a7
	.byte	0
	.uleb128 0x29
	.byte	0x1
	.long	.LASF233
	.byte	0x14
	.byte	0xac
	.long	0x1843
	.byte	0x1
	.long	0x18ca
	.uleb128 0x1b
	.long	0x3a7
	.byte	0
	.uleb128 0x11
	.long	0x5f
	.uleb128 0x11
	.long	0x78
	.uleb128 0x3
	.byte	0x8
	.byte	0x3
	.long	.LASF234
	.uleb128 0x6
	.byte	0x8
	.long	0x602
	.uleb128 0x2f
	.byte	0x8
	.long	0x857
	.uleb128 0x2f
	.byte	0x8
	.long	0xab0
	.uleb128 0x2f
	.byte	0x8
	.long	0x3d1
	.uleb128 0x2f
	.byte	0x8
	.long	0x18f9
	.uleb128 0x11
	.long	0x3d1
	.uleb128 0x6
	.byte	0x8
	.long	0xd08
	.uleb128 0x2f
	.byte	0x8
	.long	0x602
	.uleb128 0x3
	.byte	0x10
	.byte	0x3
	.long	.LASF235
	.uleb128 0x6
	.byte	0x8
	.long	0x85c
	.uleb128 0x2f
	.byte	0x8
	.long	0xd08
	.uleb128 0x2f
	.byte	0x8
	.long	0x3b2
	.uleb128 0x2f
	.byte	0x8
	.long	0x1929
	.uleb128 0x11
	.long	0x3b2
	.uleb128 0x6
	.byte	0x8
	.long	0x857
	.uleb128 0x2f
	.byte	0x8
	.long	0x85c
	.uleb128 0x3
	.byte	0x20
	.byte	0x3
	.long	.LASF236
	.uleb128 0x6
	.byte	0x8
	.long	0xab5
	.uleb128 0x2f
	.byte	0x8
	.long	0x3d8
	.uleb128 0x2f
	.byte	0x8
	.long	0x1953
	.uleb128 0x11
	.long	0x3d8
	.uleb128 0x6
	.byte	0x8
	.long	0xab0
	.uleb128 0x2f
	.byte	0x8
	.long	0xab5
	.uleb128 0x2d
	.byte	0x15
	.byte	0x4
	.long	0x3e6
	.uleb128 0x30
	.long	0x8a3
	.byte	0x2
	.long	0x1979
	.long	0x199c
	.uleb128 0x31
	.long	.LASF237
	.long	0x199c
	.byte	0x1
	.uleb128 0x32
	.string	"__r"
	.byte	0x1
	.value	0x4ab
	.long	0x3b2
	.uleb128 0x32
	.string	"__i"
	.byte	0x1
	.value	0x4ab
	.long	0x3b2
	.byte	0
	.uleb128 0x11
	.long	0x1911
	.uleb128 0x33
	.long	0x196b
	.long	.LASF271
	.quad	.LFB1228
	.quad	.LFE1228
	.long	.LLST0
	.long	0x19c7
	.byte	0x1
	.long	0x19e0
	.uleb128 0x34
	.long	0x1979
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x34
	.long	0x1983
	.byte	0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x34
	.long	0x198f
	.byte	0x2
	.byte	0x91
	.sleb128 -40
	.byte	0
	.uleb128 0x35
	.long	0x903
	.quad	.LFB1233
	.quad	.LFE1233
	.long	.LLST1
	.long	0x1a02
	.byte	0x1
	.long	0x1a10
	.uleb128 0x36
	.long	.LASF237
	.long	0x199c
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.uleb128 0x35
	.long	0x9cb
	.quad	.LFB1239
	.quad	.LFE1239
	.long	.LLST2
	.long	0x1a32
	.byte	0x1
	.long	0x1a4f
	.uleb128 0x36
	.long	.LASF237
	.long	0x199c
	.byte	0x1
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x37
	.string	"__d"
	.byte	0x1
	.value	0x4d9
	.long	0x3b2
	.byte	0x2
	.byte	0x91
	.sleb128 -32
	.byte	0
	.uleb128 0x38
	.byte	0x1
	.long	.LASF272
	.byte	0x2
	.byte	0x9
	.long	.LASF273
	.quad	.LFB1288
	.quad	.LFE1288
	.long	.LLST3
	.byte	0x1
	.long	0x1d70
	.uleb128 0x39
	.string	"N"
	.byte	0x2
	.byte	0x9
	.long	0x71
	.byte	0x3
	.byte	0x91
	.sleb128 -196
	.uleb128 0x3a
	.long	.LASF238
	.byte	0x2
	.byte	0x9
	.long	0x71
	.byte	0x3
	.byte	0x91
	.sleb128 -200
	.uleb128 0x3a
	.long	.LASF239
	.byte	0x2
	.byte	0x9
	.long	0x1d70
	.byte	0x3
	.byte	0x91
	.sleb128 -208
	.uleb128 0x3a
	.long	.LASF240
	.byte	0x2
	.byte	0x9
	.long	0x1d76
	.byte	0x3
	.byte	0x91
	.sleb128 -216
	.uleb128 0x39
	.string	"w"
	.byte	0x2
	.byte	0x9
	.long	0x1d76
	.byte	0x3
	.byte	0x91
	.sleb128 -224
	.uleb128 0x3a
	.long	.LASF241
	.byte	0x2
	.byte	0x9
	.long	0x1911
	.byte	0x3
	.byte	0x91
	.sleb128 -232
	.uleb128 0x3a
	.long	.LASF242
	.byte	0x2
	.byte	0x9
	.long	0x1d76
	.byte	0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x39
	.string	"Q"
	.byte	0x2
	.byte	0x9
	.long	0x1d7c
	.byte	0x2
	.byte	0x91
	.sleb128 8
	.uleb128 0x3a
	.long	.LASF243
	.byte	0x2
	.byte	0x9
	.long	0x1d76
	.byte	0x2
	.byte	0x91
	.sleb128 16
	.uleb128 0x39
	.string	"Xa"
	.byte	0x2
	.byte	0x9
	.long	0x1911
	.byte	0x2
	.byte	0x91
	.sleb128 24
	.uleb128 0x39
	.string	"Xs"
	.byte	0x2
	.byte	0x9
	.long	0x1911
	.byte	0x2
	.byte	0x91
	.sleb128 32
	.uleb128 0x39
	.string	"q"
	.byte	0x2
	.byte	0x9
	.long	0x1d76
	.byte	0x2
	.byte	0x91
	.sleb128 40
	.uleb128 0x3a
	.long	.LASF244
	.byte	0x2
	.byte	0x9
	.long	0x1911
	.byte	0x2
	.byte	0x91
	.sleb128 48
	.uleb128 0x3a
	.long	.LASF245
	.byte	0x2
	.byte	0x9
	.long	0x1911
	.byte	0x2
	.byte	0x91
	.sleb128 56
	.uleb128 0x39
	.string	"Phi"
	.byte	0x2
	.byte	0x9
	.long	0x1d76
	.byte	0x3
	.byte	0x91
	.sleb128 64
	.uleb128 0x3a
	.long	.LASF246
	.byte	0x2
	.byte	0x9
	.long	0x1d76
	.byte	0x3
	.byte	0x91
	.sleb128 72
	.uleb128 0x3a
	.long	.LASF247
	.byte	0x2
	.byte	0x9
	.long	0x1d76
	.byte	0x3
	.byte	0x91
	.sleb128 80
	.uleb128 0x3a
	.long	.LASF248
	.byte	0x2
	.byte	0x9
	.long	0x71
	.byte	0x3
	.byte	0x91
	.sleb128 88
	.uleb128 0x39
	.string	"p"
	.byte	0x2
	.byte	0x9
	.long	0x3b9
	.byte	0x3
	.byte	0x91
	.sleb128 96
	.uleb128 0x39
	.string	"p2"
	.byte	0x2
	.byte	0x9
	.long	0x3b9
	.byte	0x3
	.byte	0x91
	.sleb128 104
	.uleb128 0x3b
	.quad	.LBB3
	.quad	.LBE3
	.uleb128 0x3c
	.string	"AUX"
	.byte	0x2
	.byte	0xc
	.long	0x3b2
	.byte	0x3
	.byte	0x91
	.sleb128 -112
	.uleb128 0x3d
	.long	.LASF249
	.byte	0x2
	.byte	0xd
	.long	0x3b2
	.byte	0x3
	.byte	0x91
	.sleb128 -96
	.uleb128 0x3d
	.long	.LASF250
	.byte	0x2
	.byte	0xe
	.long	0x3b2
	.byte	0x3
	.byte	0x91
	.sleb128 -104
	.uleb128 0x3d
	.long	.LASF251
	.byte	0x2
	.byte	0xf
	.long	0x3b2
	.byte	0x3
	.byte	0x91
	.sleb128 -120
	.uleb128 0x3d
	.long	.LASF252
	.byte	0x2
	.byte	0x10
	.long	0x3b2
	.byte	0x3
	.byte	0x91
	.sleb128 -128
	.uleb128 0x3c
	.string	"L"
	.byte	0x2
	.byte	0x12
	.long	0x71
	.byte	0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x3d
	.long	.LASF253
	.byte	0x2
	.byte	0x18
	.long	0x71
	.byte	0x2
	.byte	0x91
	.sleb128 -44
	.uleb128 0x3c
	.string	"r"
	.byte	0x2
	.byte	0x19
	.long	0x3b2
	.byte	0x3
	.byte	0x91
	.sleb128 -136
	.uleb128 0x3c
	.string	"j"
	.byte	0x2
	.byte	0x1a
	.long	0x85c
	.byte	0x3
	.byte	0x91
	.sleb128 -192
	.uleb128 0x3c
	.string	"n1"
	.byte	0x2
	.byte	0x1b
	.long	0x71
	.byte	0x3
	.byte	0x91
	.sleb128 -148
	.uleb128 0x3c
	.string	"n2"
	.byte	0x2
	.byte	0x1c
	.long	0x71
	.byte	0x3
	.byte	0x91
	.sleb128 -152
	.uleb128 0x3c
	.string	"n3"
	.byte	0x2
	.byte	0x1d
	.long	0x3b2
	.byte	0x3
	.byte	0x91
	.sleb128 -144
	.uleb128 0x3e
	.quad	.LBB4
	.quad	.LBE4
	.long	0x1c67
	.uleb128 0x3c
	.string	"i"
	.byte	0x2
	.byte	0x14
	.long	0x71
	.byte	0x2
	.byte	0x91
	.sleb128 -40
	.byte	0
	.uleb128 0x3e
	.quad	.LBB5
	.quad	.LBE5
	.long	0x1c89
	.uleb128 0x3c
	.string	"i"
	.byte	0x2
	.byte	0x2c
	.long	0x71
	.byte	0x2
	.byte	0x91
	.sleb128 -48
	.byte	0
	.uleb128 0x3e
	.quad	.LBB6
	.quad	.LBE6
	.long	0x1cab
	.uleb128 0x3c
	.string	"i"
	.byte	0x2
	.byte	0x36
	.long	0x71
	.byte	0x2
	.byte	0x91
	.sleb128 -52
	.byte	0
	.uleb128 0x3e
	.quad	.LBB7
	.quad	.LBE7
	.long	0x1ccd
	.uleb128 0x3c
	.string	"i"
	.byte	0x2
	.byte	0x41
	.long	0x71
	.byte	0x2
	.byte	0x91
	.sleb128 -56
	.byte	0
	.uleb128 0x3e
	.quad	.LBB8
	.quad	.LBE8
	.long	0x1d0d
	.uleb128 0x3c
	.string	"i"
	.byte	0x2
	.byte	0x52
	.long	0x71
	.byte	0x2
	.byte	0x91
	.sleb128 -60
	.uleb128 0x3b
	.quad	.LBB10
	.quad	.LBE10
	.uleb128 0x3c
	.string	"j"
	.byte	0x2
	.byte	0x56
	.long	0x71
	.byte	0x2
	.byte	0x91
	.sleb128 -64
	.byte	0
	.byte	0
	.uleb128 0x3e
	.quad	.LBB11
	.quad	.LBE11
	.long	0x1d4f
	.uleb128 0x3c
	.string	"k"
	.byte	0x2
	.byte	0x5d
	.long	0x71
	.byte	0x3
	.byte	0x91
	.sleb128 -68
	.uleb128 0x3b
	.quad	.LBB13
	.quad	.LBE13
	.uleb128 0x3c
	.string	"i"
	.byte	0x2
	.byte	0x5f
	.long	0x71
	.byte	0x3
	.byte	0x91
	.sleb128 -72
	.byte	0
	.byte	0
	.uleb128 0x3b
	.quad	.LBB14
	.quad	.LBE14
	.uleb128 0x3c
	.string	"n"
	.byte	0x2
	.byte	0x69
	.long	0x71
	.byte	0x3
	.byte	0x91
	.sleb128 -76
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x6
	.byte	0x8
	.long	0x71
	.uleb128 0x6
	.byte	0x8
	.long	0x3b2
	.uleb128 0x6
	.byte	0x8
	.long	0x1d76
	.uleb128 0x2f
	.byte	0x8
	.long	0x85c
	.uleb128 0x3f
	.long	0xd0d
	.quad	.LFB1289
	.quad	.LFE1289
	.long	.LLST4
	.byte	0x1
	.long	0x1dbf
	.uleb128 0x21
	.string	"_Tp"
	.long	0x3b2
	.uleb128 0x37
	.string	"__z"
	.byte	0x1
	.value	0x221
	.long	0x1dbf
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.uleb128 0x11
	.long	0x1d82
	.uleb128 0x40
	.long	0xd6b
	.long	.LASF254
	.sleb128 -2147483648
	.uleb128 0x41
	.long	0xd78
	.long	.LASF255
	.long	0x7fffffff
	.uleb128 0x42
	.long	0xda4
	.long	.LASF256
	.byte	0x40
	.uleb128 0x42
	.long	0xdd0
	.long	.LASF257
	.byte	0x7f
	.uleb128 0x40
	.long	0xdfc
	.long	.LASF258
	.sleb128 -32768
	.uleb128 0x43
	.long	0xe09
	.long	.LASF259
	.value	0x7fff
	.uleb128 0x40
	.long	0xe31
	.long	.LASF260
	.sleb128 -9223372036854775808
	.uleb128 0x44
	.long	0xe3e
	.long	.LASF261
	.quad	0x7fffffffffffffff
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x55
	.uleb128 0x6
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x17
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x13
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x39
	.byte	0x1
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x8
	.byte	0
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x18
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x8
	.byte	0
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x18
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x39
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.uleb128 0x32
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x34
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1c
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x63
	.uleb128 0xc
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1d
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1f
	.uleb128 0x2
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x20
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x21
	.uleb128 0x2f
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x22
	.uleb128 0x39
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x23
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x24
	.uleb128 0x2f
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x25
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x26
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x27
	.uleb128 0x18
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x28
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x29
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2a
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2b
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2c
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2d
	.uleb128 0x3a
	.byte	0
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x18
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2e
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x2f
	.uleb128 0x10
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x30
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x47
	.uleb128 0x13
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x31
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x34
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x32
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x33
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x2117
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x34
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x35
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x47
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x64
	.uleb128 0x13
	.uleb128 0x2117
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x36
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x34
	.uleb128 0xc
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x37
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x38
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x2116
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x39
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x3a
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x3b
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.byte	0
	.byte	0
	.uleb128 0x3c
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x3d
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3f
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x47
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x2116
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x40
	.uleb128 0x34
	.byte	0
	.uleb128 0x47
	.uleb128 0x13
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xd
	.byte	0
	.byte	0
	.uleb128 0x41
	.uleb128 0x34
	.byte	0
	.uleb128 0x47
	.uleb128 0x13
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x42
	.uleb128 0x34
	.byte	0
	.uleb128 0x47
	.uleb128 0x13
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x43
	.uleb128 0x34
	.byte	0
	.uleb128 0x47
	.uleb128 0x13
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0x5
	.byte	0
	.byte	0
	.uleb128 0x44
	.uleb128 0x34
	.byte	0
	.uleb128 0x47
	.uleb128 0x13
	.uleb128 0x2007
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0x7
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
.LLST0:
	.quad	.LFB1228
	.quad	.LCFI0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI0
	.quad	.LCFI1
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI1
	.quad	.LCFI2
	.value	0x2
	.byte	0x76
	.sleb128 16
	.quad	.LCFI2
	.quad	.LFE1228
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	0
	.quad	0
.LLST1:
	.quad	.LFB1233
	.quad	.LCFI3
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI3
	.quad	.LCFI4
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI4
	.quad	.LCFI5
	.value	0x2
	.byte	0x76
	.sleb128 16
	.quad	.LCFI5
	.quad	.LFE1233
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	0
	.quad	0
.LLST2:
	.quad	.LFB1239
	.quad	.LCFI6
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI6
	.quad	.LCFI7
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI7
	.quad	.LCFI8
	.value	0x2
	.byte	0x76
	.sleb128 16
	.quad	.LCFI8
	.quad	.LFE1239
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	0
	.quad	0
.LLST3:
	.quad	.LFB1288
	.quad	.LCFI9
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI9
	.quad	.LCFI10
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI10
	.quad	.LCFI11
	.value	0x2
	.byte	0x76
	.sleb128 16
	.quad	.LCFI11
	.quad	.LFE1288
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	0
	.quad	0
.LLST4:
	.quad	.LFB1289
	.quad	.LCFI12
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI12
	.quad	.LCFI13
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI13
	.quad	.LCFI14
	.value	0x2
	.byte	0x76
	.sleb128 16
	.quad	.LCFI14
	.quad	.LFE1289
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	0
	.quad	0
	.section	.debug_aranges,"",@progbits
	.long	0x6c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x8
	.byte	0
	.value	0
	.value	0
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.quad	.LFB1228
	.quad	.LFE1228-.LFB1228
	.quad	.LFB1233
	.quad	.LFE1233-.LFB1233
	.quad	.LFB1239
	.quad	.LFE1239-.LFB1239
	.quad	.LFB1289
	.quad	.LFE1289-.LFB1289
	.quad	0
	.quad	0
	.section	.debug_ranges,"",@progbits
.Ldebug_ranges0:
	.quad	.Ltext0
	.quad	.Letext0
	.quad	.LFB1228
	.quad	.LFE1228
	.quad	.LFB1233
	.quad	.LFE1233
	.quad	.LFB1239
	.quad	.LFE1239
	.quad	.LFB1289
	.quad	.LFE1289
	.quad	0
	.quad	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF269:
	.string	"_ZSt4realIdERT_RSt7complexIS0_E"
.LASF6:
	.string	"size_t"
.LASF11:
	.string	"sizetype"
.LASF162:
	.string	"tm_hour"
.LASF46:
	.string	"__value"
.LASF116:
	.string	"__numeric_traits_integer<int>"
.LASF255:
	.string	"_ZN9__gnu_cxx24__numeric_traits_integerIiE5__maxE"
.LASF101:
	.string	"complex<long double>"
.LASF109:
	.string	"_ZNSt7complexIeEpLEe"
.LASF99:
	.string	"_ZNKSt7complexIdE5__repEv"
.LASF24:
	.string	"_IO_save_end"
.LASF118:
	.string	"__max"
.LASF158:
	.string	"wcscspn"
.LASF227:
	.string	"localeconv"
.LASF98:
	.string	"_ZNSt7complexIdEdVEd"
.LASF212:
	.string	"frac_digits"
.LASF17:
	.string	"_IO_write_base"
.LASF33:
	.string	"_lock"
.LASF204:
	.string	"int_curr_symbol"
.LASF190:
	.string	"wcschr"
.LASF258:
	.string	"_ZN9__gnu_cxx24__numeric_traits_integerIsE5__minE"
.LASF215:
	.string	"n_cs_precedes"
.LASF22:
	.string	"_IO_save_base"
.LASF139:
	.string	"mbrtowc"
.LASF249:
	.string	"d_phi"
.LASF95:
	.string	"_ZNSt7complexIdEpLEd"
.LASF182:
	.string	"wcsxfrm"
.LASF211:
	.string	"int_frac_digits"
.LASF26:
	.string	"_chain"
.LASF156:
	.string	"wcscoll"
.LASF69:
	.string	"_ZNKSt7complexIfE4realEv"
.LASF30:
	.string	"_cur_column"
.LASF209:
	.string	"positive_sign"
.LASF92:
	.string	"_ZNSt7complexIdE4realEd"
.LASF43:
	.string	"__wch"
.LASF88:
	.string	"_ZNSt7complexIdE4realEv"
.LASF105:
	.string	"_ZNKSt7complexIeE4imagEv"
.LASF246:
	.string	"ysaida"
.LASF66:
	.string	"complex"
.LASF82:
	.string	"_ZNSt7complexIfEmLEf"
.LASF192:
	.string	"wcsrchr"
.LASF206:
	.string	"mon_decimal_point"
.LASF8:
	.string	"long int"
.LASF168:
	.string	"tm_isdst"
.LASF122:
	.string	"__numeric_traits_integer<char>"
.LASF151:
	.string	"vwprintf"
.LASF59:
	.string	"fftw_plan"
.LASF264:
	.string	"/home/andre/mod-pitchshifter/2_Parameters_PitchShifter/src"
.LASF54:
	.string	"_IO_marker"
.LASF221:
	.string	"int_n_cs_precedes"
.LASF231:
	.string	"towctrans"
.LASF127:
	.string	"mbstate_t"
.LASF85:
	.string	"__rep"
.LASF119:
	.string	"_Value"
.LASF167:
	.string	"tm_yday"
.LASF4:
	.string	"signed char"
.LASF48:
	.string	"_IO_FILE"
.LASF228:
	.string	"wctype_t"
.LASF75:
	.string	"operator="
.LASF247:
	.string	"ysaida2"
.LASF129:
	.string	"fgetwc"
.LASF226:
	.string	"getwchar"
.LASF130:
	.string	"fgetws"
.LASF1:
	.string	"unsigned char"
.LASF216:
	.string	"n_sep_by_space"
.LASF194:
	.string	"wmemchr"
.LASF259:
	.string	"_ZN9__gnu_cxx24__numeric_traits_integerIsE5__maxE"
.LASF155:
	.string	"wcscmp"
.LASF144:
	.string	"swprintf"
.LASF191:
	.string	"wcspbrk"
.LASF12:
	.string	"char"
.LASF242:
	.string	"PhiPrevious"
.LASF233:
	.string	"wctype"
.LASF173:
	.string	"wcsncmp"
.LASF266:
	.string	"_IO_lock_t"
.LASF224:
	.string	"int_n_sign_posn"
.LASF218:
	.string	"n_sign_posn"
.LASF186:
	.string	"wmemmove"
.LASF70:
	.string	"imag"
.LASF117:
	.string	"__min"
.LASF128:
	.string	"btowc"
.LASF14:
	.string	"_IO_read_ptr"
.LASF189:
	.string	"wscanf"
.LASF207:
	.string	"mon_thousands_sep"
.LASF146:
	.string	"ungetwc"
.LASF51:
	.string	"fp_offset"
.LASF63:
	.string	"ptrdiff_t"
.LASF254:
	.string	"_ZN9__gnu_cxx24__numeric_traits_integerIiE5__minE"
.LASF229:
	.string	"wctrans_t"
.LASF138:
	.string	"mbrlen"
.LASF57:
	.string	"_pos"
.LASF210:
	.string	"negative_sign"
.LASF219:
	.string	"int_p_cs_precedes"
.LASF135:
	.string	"fwprintf"
.LASF25:
	.string	"_markers"
.LASF197:
	.string	"wcstoull"
.LASF106:
	.string	"_ZNSt7complexIeE4realEe"
.LASF163:
	.string	"tm_mday"
.LASF65:
	.string	"_ComplexT"
.LASF102:
	.string	"_ZNSt7complexIeE4realEv"
.LASF72:
	.string	"_ZNKSt7complexIfE4imagEv"
.LASF149:
	.string	"vswprintf"
.LASF93:
	.string	"_ZNSt7complexIdE4imagEd"
.LASF187:
	.string	"wmemset"
.LASF90:
	.string	"_ZNSt7complexIdE4imagEv"
.LASF134:
	.string	"fwide"
.LASF252:
	.string	"omega_true_sobre_fs"
.LASF160:
	.string	"tm_sec"
.LASF267:
	.string	"fftw_plan_s"
.LASF34:
	.string	"_offset"
.LASF273:
	.string	"_Z5shiftiiPiPdS0_PSt7complexIdES0_PS0_S0_S3_S3_S0_S3_S3_S0_S0_S0_iP11fftw_plan_sS6_"
.LASF174:
	.string	"wcsncpy"
.LASF143:
	.string	"putwchar"
.LASF184:
	.string	"wmemcmp"
.LASF0:
	.string	"long unsigned int"
.LASF28:
	.string	"_flags2"
.LASF198:
	.string	"__gnu_debug"
.LASF272:
	.string	"shift"
.LASF16:
	.string	"_IO_read_base"
.LASF100:
	.string	"complex<double>"
.LASF147:
	.string	"vfwprintf"
.LASF263:
	.string	"shift.cpp"
.LASF237:
	.string	"this"
.LASF41:
	.string	"_unused2"
.LASF214:
	.string	"p_sep_by_space"
.LASF62:
	.string	"__float128"
.LASF29:
	.string	"_old_offset"
.LASF222:
	.string	"int_n_sep_by_space"
.LASF76:
	.string	"_ZNSt7complexIfEaSEf"
.LASF125:
	.string	"long long int"
.LASF240:
	.string	"frames"
.LASF47:
	.string	"__mbstate_t"
.LASF185:
	.string	"wmemcpy"
.LASF235:
	.string	"complex double"
.LASF164:
	.string	"tm_mon"
.LASF58:
	.string	"double"
.LASF19:
	.string	"_IO_write_end"
.LASF111:
	.string	"_ZNSt7complexIeEmLEe"
.LASF251:
	.string	"d_phi_wrapped"
.LASF183:
	.string	"wctob"
.LASF253:
	.string	"Hops"
.LASF50:
	.string	"gp_offset"
.LASF80:
	.string	"_ZNSt7complexIfEmIEf"
.LASF256:
	.string	"_ZN9__gnu_cxx24__numeric_traits_integerImE8__digitsE"
.LASF60:
	.string	"float"
.LASF161:
	.string	"tm_min"
.LASF20:
	.string	"_IO_buf_base"
.LASF67:
	.string	"real"
.LASF3:
	.string	"unsigned int"
.LASF73:
	.string	"_ZNSt7complexIfE4realEf"
.LASF262:
	.string	"GNU C++ 4.7.2"
.LASF176:
	.string	"wcsspn"
.LASF68:
	.string	"_ZNSt7complexIfE4realEv"
.LASF217:
	.string	"p_sign_posn"
.LASF64:
	.string	"complex<float>"
.LASF35:
	.string	"__pad1"
.LASF36:
	.string	"__pad2"
.LASF37:
	.string	"__pad3"
.LASF38:
	.string	"__pad4"
.LASF39:
	.string	"__pad5"
.LASF56:
	.string	"_sbuf"
.LASF97:
	.string	"_ZNSt7complexIdEmLEd"
.LASF244:
	.string	"qaux"
.LASF104:
	.string	"_ZNSt7complexIeE4imagEv"
.LASF52:
	.string	"overflow_arg_area"
.LASF13:
	.string	"_flags"
.LASF241:
	.string	"XaPrevious"
.LASF84:
	.string	"_ZNSt7complexIfEdVEf"
.LASF40:
	.string	"_mode"
.LASF201:
	.string	"decimal_point"
.LASF45:
	.string	"__count"
.LASF115:
	.string	"__gnu_cxx"
.LASF199:
	.string	"bool"
.LASF61:
	.string	"long double"
.LASF142:
	.string	"putwc"
.LASF78:
	.string	"_ZNSt7complexIfEpLEf"
.LASF270:
	.string	"__numeric_traits_integer<long int>"
.LASF114:
	.string	"real<double>"
.LASF89:
	.string	"_ZNKSt7complexIdE4realEv"
.LASF44:
	.string	"__wchb"
.LASF86:
	.string	"_ZNKSt7complexIfE5__repEv"
.LASF236:
	.string	"complex long double"
.LASF238:
	.string	"hopa"
.LASF124:
	.string	"long long unsigned int"
.LASF87:
	.string	"_M_value"
.LASF53:
	.string	"reg_save_area"
.LASF195:
	.string	"wcstold"
.LASF220:
	.string	"int_p_sep_by_space"
.LASF239:
	.string	"hops"
.LASF196:
	.string	"wcstoll"
.LASF9:
	.string	"__off_t"
.LASF193:
	.string	"wcsstr"
.LASF157:
	.string	"wcscpy"
.LASF257:
	.string	"_ZN9__gnu_cxx24__numeric_traits_integerIcE5__maxE"
.LASF175:
	.string	"wcsrtombs"
.LASF166:
	.string	"tm_wday"
.LASF140:
	.string	"mbsinit"
.LASF145:
	.string	"swscanf"
.LASF121:
	.string	"__digits"
.LASF177:
	.string	"wcstod"
.LASF178:
	.string	"wcstof"
.LASF179:
	.string	"wcstok"
.LASF180:
	.string	"wcstol"
.LASF42:
	.string	"__FILE"
.LASF243:
	.string	"yshift"
.LASF23:
	.string	"_IO_backup_base"
.LASF225:
	.string	"setlocale"
.LASF32:
	.string	"_shortbuf"
.LASF245:
	.string	"framesaux"
.LASF250:
	.string	"d_phi_prime"
.LASF136:
	.string	"fwscanf"
.LASF126:
	.string	"wint_t"
.LASF55:
	.string	"_next"
.LASF10:
	.string	"__off64_t"
.LASF232:
	.string	"wctrans"
.LASF202:
	.string	"thousands_sep"
.LASF21:
	.string	"_IO_buf_end"
.LASF171:
	.string	"wcslen"
.LASF74:
	.string	"_ZNSt7complexIfE4imagEf"
.LASF81:
	.string	"operator*="
.LASF107:
	.string	"_ZNSt7complexIeE4imagEe"
.LASF268:
	.string	"__debug"
.LASF71:
	.string	"_ZNSt7complexIfE4imagEv"
.LASF169:
	.string	"tm_gmtoff"
.LASF205:
	.string	"currency_symbol"
.LASF5:
	.string	"short int"
.LASF159:
	.string	"wcsftime"
.LASF77:
	.string	"operator+="
.LASF31:
	.string	"_vtable_offset"
.LASF208:
	.string	"mon_grouping"
.LASF234:
	.string	"complex float"
.LASF154:
	.string	"wcscat"
.LASF265:
	.string	"11__mbstate_t"
.LASF113:
	.string	"_ZNKSt7complexIeE5__repEv"
.LASF223:
	.string	"int_p_sign_posn"
.LASF170:
	.string	"tm_zone"
.LASF108:
	.string	"_ZNSt7complexIeEaSEe"
.LASF152:
	.string	"vwscanf"
.LASF153:
	.string	"wcrtomb"
.LASF200:
	.string	"lconv"
.LASF15:
	.string	"_IO_read_end"
.LASF172:
	.string	"wcsncat"
.LASF103:
	.string	"_ZNKSt7complexIeE4realEv"
.LASF123:
	.string	"__numeric_traits_integer<short int>"
.LASF132:
	.string	"fputwc"
.LASF79:
	.string	"operator-="
.LASF27:
	.string	"_fileno"
.LASF91:
	.string	"_ZNKSt7complexIdE4imagEv"
.LASF133:
	.string	"fputws"
.LASF150:
	.string	"vswscanf"
.LASF141:
	.string	"mbsrtowcs"
.LASF110:
	.string	"_ZNSt7complexIeEmIEe"
.LASF213:
	.string	"p_cs_precedes"
.LASF120:
	.string	"__numeric_traits_integer<long unsigned int>"
.LASF165:
	.string	"tm_year"
.LASF2:
	.string	"short unsigned int"
.LASF94:
	.string	"_ZNSt7complexIdEaSEd"
.LASF148:
	.string	"vfwscanf"
.LASF18:
	.string	"_IO_write_ptr"
.LASF7:
	.string	"__int32_t"
.LASF271:
	.string	"_ZNSt7complexIdEC2Edd"
.LASF137:
	.string	"getwc"
.LASF230:
	.string	"iswctype"
.LASF203:
	.string	"grouping"
.LASF83:
	.string	"operator/="
.LASF188:
	.string	"wprintf"
.LASF261:
	.string	"_ZN9__gnu_cxx24__numeric_traits_integerIlE5__maxE"
.LASF248:
	.string	"Qcolumn"
.LASF260:
	.string	"_ZN9__gnu_cxx24__numeric_traits_integerIlE5__minE"
.LASF96:
	.string	"_ZNSt7complexIdEmIEd"
.LASF131:
	.string	"wchar_t"
.LASF49:
	.string	"typedef __va_list_tag __va_list_tag"
.LASF112:
	.string	"_ZNSt7complexIeEdVEe"
.LASF181:
	.string	"wcstoul"
	.ident	"GCC: (Debian 4.7.2-5) 4.7.2"
	.section	.note.GNU-stack,"",@progbits
