section .text
  global _start

_start:
  ;; save cpu state
  	push rax
  	push rdi
  	push rsi
  	push rdx

  ;; write msg to stdout
  	mov rax,1                     ; [1] - sys_write
  	mov rdi,1                     ; 0 = stdin / 1 = stdout / 2 = stderr
  	lea rsi,[rel msg]             ; pointer(mem address) to msg (*char[])
  	mov rdx, msg_end - msg        ; msg size
  	syscall                       ; calls the function stored in rax

  ;; Uncypher
	mov rax, 0x1111111111111111
	mov rcx, 0x2222222222222222
	lea r9, [rel key]

.loop 	mov dl, [r9]
	xor byte [rax], dl
	add rax, 0x1
	sub rcx, 0x1
	add r9, 0x1
	cmp byte [r9], 0
	jne .cmp
	lea r9, [rel key]

.cmp	cmp rcx, 0
	jnz .loop
	jmp .out
  ;; restore cpu state

.out  	pop rdx
  	pop rsi
  	pop rdi
  	pop rax

  	;; jump to _main
  	mov rax, 0x1111111111111111   ; address changed during injection
  	jmp rax

align 8
  msg     db 'Time to undo THE XOR!',0xa,0
  msg_end db 0x0
  key	  db 'ABCDEFGH',0
  key_end db 0x0
