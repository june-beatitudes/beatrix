.equ BEA_SYSCALL_YIELD, 5

.globl bea_do_syscall

.thumb_func
bea_do_syscall:
    @ Preserve the registers that aren't automatically preserved for us
    PUSH {R4-R11}
    @ Load the preserved PC and extract the syscall reason
    MRS R0, PSP
    LDR R1, [R0, 24]
    SUB R1, 2
    LDRH R1, [R1]
    UBFX R1, R1, 0, 8
    @ Handle YIELD
    CMP R1, BEA_SYSCALL_YIELD
    BNE _bds_exit
    LDR R0, =0xE000ED04
    LDR R2, =(1<<28)
    STR R2, [R0]
_bds_exit:
    @ Exit
    POP {R4-R11}
    BX LR