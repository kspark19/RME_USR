    .section .text
    .syntax divided
    .code 32
    .align 4

    .global _start
_start:
    MOV     R12, #0
    LDR     R11, =msg
printchar:
    LDRB    R10, [R11, R12]
    CMP     R10, #0
    BEQ     finish

printwait:
    LDR     R9, =0x6000102C
    LDR     R9, [R9]
    TST     R9, #0x08
    BEQ     printwait


    LDR     R9, =0x60001030
    STR     R10, [R9]

    ADD     R12, R12, #1
    B       printchar

finish:
    B       finish

msg:
    .string "hello world"


