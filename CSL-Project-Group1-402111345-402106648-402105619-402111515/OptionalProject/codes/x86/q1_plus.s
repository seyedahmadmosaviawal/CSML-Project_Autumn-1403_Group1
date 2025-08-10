global asm_main
extern printf
extern scanf
extern malloc
extern clock

section .data
    input_format db "%u",0
    output_format: db "%u ", 0
    time: db "%d",10,0
    next_line: db "",10, 0
    matrix_sizes: dq 0
    input_matrix1: dq 0
    input_matrix2: dq 0
    transpose_matrix2: dq 0
    output_matrix: dq 0
    start: dd 0
    end: dd 0

section .text
asm_main:

    call get_input

     call clock
     push rax
     mov rcx,1000
     loop_:
        push rcx
        mov rax,[matrix_sizes]
        push rax ;push n
        mov rax,[transpose_matrix2]
        push rax ;push transpose matrix address
        mov rax,[input_matrix2]
        push rax ;push address of second matrix
        call build_transpose

        mov rax,[matrix_sizes]
        push rax ;push n
        mov rax,[input_matrix1]
        push rax ;push address of first matrix
        mov rax,[transpose_matrix2]
        push rax ;push address of second matrix 
        mov rax,[output_matrix]
        push rax ;push output matrix address
        call multiply_matrix_vector
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



build_transpose:
    push rbp
    mov rbp,rsp
    add rbp,16
    mov r10,[rbp] ;input_matrix2
    add rbp,8
    mov r11,[rbp] ;transpose_matrix
    add rbp,8
    mov r12,[rbp] ;n

    mov r9,r12
    shl r9,2 ;scaled index for going to the next row
    mov rcx,r12
    build_collumns:
        push rcx
        push r10
        mov rcx,r12
        put_elements:
            mov eax,[r10]
            mov [r11],eax
            add r11,4
            add r10,r9
            loop put_elements
        pop r10
        add r10,4
        pop rcx
        loop build_collumns
    
    pop rbp
    ret 24

multiply_matrix_vector:
    push rbp
    mov rbp,rsp
    add rbp,16
    mov r10,[rbp] ;outputMatrix
    add rbp,8
    mov r12,[rbp] ;input_matrix2
    add rbp,8
    mov r11,[rbp] ;input_matrix1
    add rbp,8
    mov r13,[rbp] ;n

    mov rcx,r13 ;number of rows of output martix
    

    mov r8,r13
    shl r8,2 ;mov in collumn
    mov r15,r13
    shr r13,2 ;vector multiply so we need 1/4 multiply
    
    calculate:
        push rcx
        push r11
        push r12
        mov rcx,r15
        calculate_row:
            push rcx
            mov rcx,r13
            xor rdx,rdx
            push r11
            calculate_element:
                movdqu xmm0,[r11]
                movdqu xmm1,[r12]
                pmulld xmm0,xmm1
                phaddd xmm0, xmm0
                phaddd xmm0, xmm0
                movd ebx,xmm0
                add edx,ebx
                add r11,16
                add r12,16
                loop calculate_element
            mov [r10],edx
            add r10,4
            pop r11
            pop rcx
            loop calculate_row
        pop r12
        pop r11
        add r11,r8
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
    imul rax
    shl rax,2  ;word
    mov rdi,rax
    call malloc
    mov [input_matrix1],rax


    mov rax,[matrix_sizes] ; n
    imul rax
    shl rax,2 ;word
    mov rdi,rax
    call malloc
    mov [input_matrix2],rax

    mov rax,[matrix_sizes] ; n
    imul rax
    shl rax,2 ;word
    mov rdi,rax
    call malloc
    mov [transpose_matrix2],rax


    mov rax,[matrix_sizes] ; n
    imul rax
    shl rax,2 ; quad word
    mov rdi,rax
    call malloc
    mov [output_matrix],rax

    mov rax,[matrix_sizes]
    mov rcx,[matrix_sizes]
    imul rax ; n * n
    mov rsi,[input_matrix1]
    first_matrix:
        mov rdi,input_format
        push rsi
        push rcx ;scanf changes registers, so we push rcx and rsi before calling it
        call scanf
        pop rcx
        pop rsi
        add rsi,4 ;next element of matrix
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
        add rsi,4 ;next element of matrix
        loop second_matrix

    ret


print_output:
    mov rcx,[matrix_sizes] ;n (number of rows in output matrix)
    mov r15,[output_matrix]
    print_line:
        push rcx
        mov rcx,[matrix_sizes] ; n (number of columns in output matrix)

        print_element:
            push rcx
            mov rdi,output_format
            xor rsi,rsi
            mov esi,[r15]
            call printf
            add r15,4
            add rsi,4
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