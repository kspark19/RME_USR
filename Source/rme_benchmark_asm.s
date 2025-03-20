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
				NOP
				MOV		R0,R1
				PUSH	{R0}
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

///* Function:RME_Inv **********************************************************
//Description : Do an invocation, and get the return value for that invocation as well.
//Input       : R4 - The system call number/other information.
//              R5 - The invocation capability.
//              R6 - The first argument for the invocation.
//              R7 - Argument 3.
//Output      : None.
//*****************************************************************************/
RME_Inv:
                PUSH       {R4-R7}  // Manual clobbering
                MOV        R4,R0    // Manually pass the parameters according to ARM calling convention
                MOV        R5,R1
                MOV        R6,R2
                MOV        R7,R3
                SVC        #0x00   
                MOV        R0,R4    // This is the return value
                POP        {R4-R7}  // Manual recovering
                BX         LR
                B          .        // Shouldn't reach here.
///* End Function:RME_Svc *****************************************************/

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
