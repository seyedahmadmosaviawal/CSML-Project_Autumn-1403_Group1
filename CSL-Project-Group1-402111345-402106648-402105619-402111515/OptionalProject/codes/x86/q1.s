global asm_main
extern printf
extern scanf
extern malloc
extern clock
section .data
    input_format: db "%lld",0
    output_format: db "%lld ", 0
    time: db "%d",10,0
    next_line: db "",10, 0
    matrix_sizes: dq 0
    input_matrix1: dq 0
    input_matrix2:dq 0
    output_matrix: dq 0

section .text
asm_main:
    call get_input

    call clock
    push rax
    mov rcx,1000
    loop_:
    push rcx
    ; push arguments of the function to stack
    mov rax,[matrix_sizes]
    push rax ;push n
    mov rax,[input_matrix1]
    push rax ;push address of first matrix
    mov rax,[input_matrix2]
    push rax ;push address of second matrix 
    mov rax,[output_matrix]
    push rax ;push output matrix address
    call multiply_matrix 
    pop rcx
        loop loop_

    call clock
    pop r11
    sub rax , r11
    mov rsi , rax
    ; ; cvtss2sd xmm0, xmm0 
    mov rdi,time
    ; ; movd xmm0,eax
    ; ; movd xmm1,ebx
    ; ; cvtss2sd xmm0, xmm0
    ; ; cvtss2sd xmm1, xmm1
    ; ; mov rsi, time
    ; ; subss xmm0, xmm1
     sub rsp,8 ;fix rsp for C functions
     call printf
     mov rdi,next_line
     call printf
     add rsp,8

    call print_output
    jmp end_program

multiply_matrix:
    push rbp
    mov rbp,rsp
    ; get the arguments from the stack
    add rbp,16
    mov r10,[rbp] ;outputMatrix
    add rbp,8
    mov r12,[rbp] ;input_matrix2
    add rbp,8
    mov r11,[rbp] ;input_matrix1

    add rbp,8
    mov r14,[rbp] ;n

    mov rcx,r14 ;n
    mov r8,r14;n
    shl r8,3 ; n * 8 for itterating the column


    calculate:
        push rcx
        push r11
        push r12
        mov rcx,r14 ;number of elements in each row (n)
        calculate_row:
            push rcx
            push r12
            push r11
            mov rcx,r14

            xor rbx,rbx

            calculate_one:
                mov rax,[r11]
                mov rdx,[r12]
                imul rax,rdx
                add rbx,rax
                add r11,8 ; next element in row (first matrix)
                add r12,r8 ; next element in column (second matrix)
                loop calculate_one

            mov [r10],rbx
            add r10,8 ;next element in output matrix
            pop r11
            pop r12
            add r12,8 ;next column in second matrix
            pop rcx
            loop calculate_row
        
        pop r12
        pop r11
        add r11,r8 ;next row in first matrix
        pop rcx
        loop calculate

    pop rbp
    ret 32 


get_input:
    mov rdi,input_format
    lea rsi,[matrix_sizes] ;n
    call scanf


    ; Allocate memory for the matrices
    mov rax,[matrix_sizes] ; n
    imul rax ;n*n
    shl rax,3  ; quad word
    mov rdi,rax
    call malloc
    mov [input_matrix1],rax

    mov rax,[matrix_sizes] ; n
    imul rax ;n*n
    shl rax,3 ; quad word
    mov rdi,rax
    call malloc
    mov [input_matrix2],rax


    mov rax,[matrix_sizes] ; n
    imul rax ;n*n
    shl rax,3 ; quad word
    mov rdi,rax
    call malloc
    mov [output_matrix],rax

    mov rax,[matrix_sizes] ; n
    mov rcx,[matrix_sizes]
    imul rax ; n*n
    mov rsi,[input_matrix1]
    first_matrix:
        mov rdi,input_format
        push rsi
        push rcx ;scanf changes registers, so we push rcx and rsi before calling it
        call scanf
        pop rcx
        pop rsi
        add rsi,8 ;next element of matrix
        loop first_matrix

    mov rax,[matrix_sizes]
    mov rcx,[matrix_sizes]
    imul rax ; n * n
    mov rsi,[input_matrix2]
    second_matrix:
        mov rdi,input_format
        push rsi
        push rcx
        call scanf
        pop rcx
        pop rsi
        add rsi,8 ;next element of matrix
        loop second_matrix

    ret

print_output:
    mov rcx,[matrix_sizes] ;n (number of rows in output matrix)
    mov r15,[output_matrix]
    print_line:
        push rcx
        mov rcx,[matrix_sizes] ;n (number of columns in output matrix)

        print_element:
            push rcx
            mov rdi,output_format
            mov rsi,[r15]
            call printf
            add r15,8
            add rsi,8
            pop rcx
            loop print_element

        mov rdi,next_line

        sub rsp,8 ;fix rsp for C functions
        call printf
        add rsp,8 ;restore rsp to correct value
        
        pop rcx
        loop print_line
    
    ret

end_program:
    ret
