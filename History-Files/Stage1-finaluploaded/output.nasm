extern printf
 extern scanf
section .data
formatin : db "%d",0
formatout : db "%d",10,0
section .bss
lresult5: resw 1
la3: resw 1
lb3: resw 1
lc3: resw 1
ld3: resw 1
section .text
 GLOBAL main
main:
mov dx,10
mov dx,20
mov dx,30
mov ax,dx
movsx rax,ax
push rax
pop rax
add dx,ax
mov ax,dx
movsx rax,ax
push rax
mov dx,10
mov ax,dx
movsx rax,ax
push rax
mov ax,dx
movsx rax,ax
push rax
mov dx,8
mov ax,dx
movsx rax,ax
push rax
mov dx,2
mov ax,dx
movsx rax,ax
push rax
pop rax
neg dx
add dx,ax
pop rax
add dx,ax
pop rax
neg dx
add dx,ax
pop rax
add dx,ax
pop rax
neg dx
add dx,ax
mov rdi,formatout
mov al,0
call printf
mov ebx,0
mov eax,1
int 0x80
