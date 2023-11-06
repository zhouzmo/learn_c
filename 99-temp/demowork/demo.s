	.file	"demo01.c"
	.text
	.def	__main;	.scl	2;	.type	32;	.endef
	.section .rdata,"dr"
.LC0:
	.ascii "a:%d\11 b:%d\11 c:%d\11 d:%d\11 c:%d\11\0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	movl	$4160, %eax
	call	___chkstk_ms
	subq	%rax, %rsp
	.seh_stackalloc	4160
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	call	__main
	movl	$0, 4028(%rbp)
	movl	4012(%rbp), %eax
	movl	%eax, 4024(%rbp)
	movl	4028(%rbp), %eax
	movl	%eax, 4020(%rbp)
	movl	4028(%rbp), %eax
	movl	%eax, 4016(%rbp)
	movl	4016(%rbp), %r8d
	movl	4020(%rbp), %ecx
	movl	4028(%rbp), %edx
	movl	4024(%rbp), %eax
	movl	%eax, 40(%rsp)
	leaq	-80(%rbp), %rax
	movq	%rax, 32(%rsp)
	movl	%r8d, %r9d
	movl	%ecx, %r8d
	leaq	.LC0(%rip), %rcx
	call	printf
	movl	$0, %eax
	addq	$4160, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.ident	"GCC: (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0"
	.def	printf;	.scl	2;	.type	32;	.endef
