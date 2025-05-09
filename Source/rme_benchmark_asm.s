///*****************************************************************************
//Filename    : benchmark_asm.s
//Author      : pry
//Date        : 19/04/2017
//Description : The Cortex-M user-level assembly scheduling support of the RME RTOS.
//*****************************************************************************/
                
///* Header *******************************************************************/
                //The align is "(2^3)/8=1(Byte)." In fact it does not take effect.
    .section .text
    .syntax divided
    .code 32
    .align 4
///* End Header ***************************************************************/

///* .global  *******************************************************************/
                //User entry stub
                .global           RME_Entry
                .global           RME_Deadloop
                //System call gate
                .global           RME_Svc
                //User level stub for thread creation
                .global           RME_Thd_Stub
                //User level stub for synchronous invocation
                .global           RME_Inv_Stub
                //Shut the semihosting up
                .global           __user_setup_stackheap
                .global			  RME_Inv_Act
                .global			  RME_Inv_Ret
///* End .global  ***************************************************************/

///* Import *******************************************************************/
                //The ARM C library entrance. This will do all the dirty init jobs for us.
                .global          RME_Benchmark
                //The C routine for
///* End Import ***************************************************************/

///* Function:RME_Entry ********************************************************
//Description : The entry of the process.
//Input       : None.
//Output      : None.
//*****************************************************************************/
RME_Entry:
                 LDR     R0, =RME_Benchmark
                 BX      R0
///* End Function:RME_Entry ***************************************************/

RME_Deadloop:
				NOP
				NOP
    /* Print a hex number in LR, R12 used as counter print r0 ********************************************/
    MOV 				LR,SP
    MOV					R12,#32     /* 32-bits */
nextdigit:
    SUB					R12,R12,#0x04
    LSR					R11,LR,R12
	AND					R11,R11,#0x0F
	CMP					R11,#0x09
	BGE					bigger
	ADD					R11,R11,#0x30 /* add '0' */
	B					printwait
bigger:
	ADD					R11,R11,#(0x41-10) /* add 'A' */
printwait:
    LDR                 R10,=0x6000102C
    LDR					R10,[R10]
    TST					R10,#0x08
    BEQ					printwait
    LDR                 R10,=0x60001030
    STR                 R11,[R10]
finish:
	CMP					R12,#0x00
	BNE					nextdigit
    /* Print a hex number in LR, R12 used as counter ********************************************/


				B   .

///* Function:RME_Thd_Stub *****************************************************
//Description : The user level stub for thread creation.
//Input       : R4 - The entry address.
//              R5 - The stack address that we are using now.
//Output      : None.
//*****************************************************************************/
RME_Thd_Stub:
                BLX      R4                 // Branch to the actual entry address
                //B        RME_Thd_Finish     // Jump to exiting code, should never return.
                B        .                  // Capture faults.
///* End Function:RME_Thd_Stub ************************************************/

///* Function:RME_Inv_Stub *****************************************************
//Description : The user level stub for synchronous invocation.
//Input       : R4 - The entry address.
//              R5 - The stack address that we are using now.
//Output      : None.
//*****************************************************************************/
RME_Inv_Stub:
                BLX      R4                 // Branch to the actual entry address
                //BX       RME_Inv_Finish     // Jump to exiting code, should never return.
                B        .                  // Capture faults.
///* End Function:RME_Inv_Stub ************************************************/

///* Function:RME_Svc **********************************************************
//Description : Trigger a system call.
//Input       : R0 - The system call number/other information.
//              R1 - Argument 1.
//              R2 - Argument 2.
//              R3 - Argument 3.
//Output      : None.
//*****************************************************************************/
RME_Svc:
                // Manually pass the parameters according to ARM calling convention
                //a7a put parameters in r0-r3,so we do nothing
                SVC        #0x00   
                BX         LR
                B          .        // Shouldn't reach here.
///* End Function:RME_Svc *****************************************************/

/* Function:RVM_Inv_Act *******************************************************
Description : Activate an invocation. If the return value is not desired, pass
              0 into R2. This is a default implementation that saves all general
              purpose registers and doesn't save FPU context. If you need a faster
              version, consider inline functions; if you need to save FPU contexts,
              please DIY.
Input       : R0 - rvm_cid_t Cap_Inv - The capability slot to the invocation stub. 2-Level.
              R1 - rvm_ptr_t Param - The parameter for the call.
Output      : R2 - rvm_ptr_t* Retval - The return value from the call.
Return      : R0 - rvm_ptr_t - The return value of the system call itself.
******************************************************************************/
RME_Inv_Act:
    PUSH                {LR}            	/* Save registers */
    PUSH                {R2,R4-R12}       	/* Save registers */

    MOV                 R2,R1               /* Parameter */
    MOV                 R1,R0
    MOV                 R0,#0x10000         /* RVM_SVC_INV_ACT */

    SVC                 #0x00               /* System call */

    POP                 {R2,R4-R12}         /* Restore registers */

    CMP               	R2,#0x00              /* Invocation return value*/
    STRNE				R1,[R2]

	POP					{PC}
/* End Function:RVM_Inv_Act **************************************************/

/* Function:RVM_Inv_Ret *******************************************************
Description : Manually return from an invocation, and set the return value to
              the old register set. This function does not need a capability
              table to work, and never returns.
Input       : R0 - The returning result from the invocation.
Output      : None.
Return      : None.
******************************************************************************/
RME_Inv_Ret:
	MOV                 R1,R0               /* Set return value */
    MOV                 R0,#0x00            /* RVM_SVC_INV_RET */

    SVC                 #0x00               /* System call */

    BX                  LR					/* Return failed if we reach here */
/* End Function:RVM_Inv_Ret **************************************************/


///* Function:__user_setup_stackheap *******************************************
//Description : We place the function here to shut the SEMIHOSTING up.
//Input       : None.
//Output      : None.
//*****************************************************************************/
__user_setup_stackheap:
                MOV      R0,R1     // Stack as where we came in, and definitely no heap
                MOV      R2,R1
                BX       LR
                B        .         // Capture faults
///* End Function:__user_setup_stackheap **************************************/

///* End Of File **************************************************************/

///* Copyright (C) Evo-Devo Instrum. All rights reserved **********************/
