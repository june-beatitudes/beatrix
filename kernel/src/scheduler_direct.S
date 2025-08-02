.equ _systick_ctrl_addr, 0xE000E010
.equ _systick_load_addr, 0xE000E014
.equ _systick_val_addr, 0xE000E018
.equ _systick_disable, 0b100
.equ _systick_enable, 0b111
.equ _icsr, 0xE000ED04

.equ _stack_load_bytes, 136 @ Includes R0-12, PC, LR, XPSR, S0-15, FSCP, and spacer 

.globl bea_do_scheduler

.thumb_func
bea_do_scheduler:
    @ Disable SysTick while we work
    MOV R0, _systick_disable
    LDR R1, =_systick_ctrl_addr
    STR R0, [R1]
    LDR R0, =(1<<25)
    LDR R1, =_icsr
    STR R0, [R1]
    @ Save the old PSP
    MRS R0, PSP
    MOV R4, LR
    BL _bea_save_psp
    MOV LR, R4
    @ Load R4-R11 onto the old stack
    STMDB R0!, {R4-R11}
    @ Keep around the old PSP for cleanup
    MOV R8, R0
    @ Persist the old state and switch over the current task index
    @ Old SP is loaded from R0 and new SP is stored in R0
    MOV R4, LR
    BL _bea_do_ctx_switch
    MOV LR, R4
    @ Clean up the old PSP's stack
    ADD R8, _stack_load_bytes
    MSR PSP, R8
    @ Allocate space on the new PSP
    SUB R0, _stack_load_bytes
    @ Fill in the new state
    @ R0 is unchanged
    MOV R4, LR
    BL _bea_fill_in_state
    MOV LR, R4
    @ Load R4-R11 from the new stack
    LDMIA R0!, {R4-R11}
    @ Store R0 back into the official PSP
    MSR PSP, R0
    @ Get the priority of the new task
    @ Priority is stored in R0
    MOV R3, LR
    BL _bea_get_priority
    MOV LR, R3
    @ Fill that in...
    LDR R1, =_systick_load_addr
    STR R0, [R1]
    LDR R0, 0
    LDR R1, =_systick_val_addr
    STR R0, [R1]
    @ Re-enable SysTick
    MOV R0, _systick_enable
    LDR R1, =_systick_ctrl_addr
    STR R0, [R1]
    @ Return from handler mode
    BX LR