/******************************************************************************
Filename    : benchmark.c
Author      : pry
Date        : 04/09/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The benchmark file for RME.
******************************************************************************/

/* Include *******************************************************************/
#include"benchmark.h"
/* Need to export error codes, and size of each object, in words! */
/* End Include ***************************************************************/

/* Function:_RME_Tsc_Init *****************************************************
Description : The initialization of timestamp counter. 19 secs before overflowing.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void _RME_Tsc_Init(void)
{
//    TIM_HandleTypeDef TIM2_Handle;
//    
//    /* Initialize timer 2 to run at the same speed as the CPU */
//    TIM2_Handle.Instance=TIM2;
//    TIM2_Handle.Init.Prescaler=0;
//    TIM2_Handle.Init.CounterMode=TIM_COUNTERMODE_UP;
//    TIM2_Handle.Init.Period=(ptr_t)(-1);
//    TIM2_Handle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
//    HAL_TIM_Base_Init(&TIM2_Handle);
//    __HAL_RCC_TIM2_CLK_ENABLE();
//    __HAL_TIM_ENABLE(&TIM2_Handle);
}
/* End Function:_RME_Tsc_Init ************************************************/

/* Function:_RME_Stack_Init ***************************************************
Description : The thread's stack initializer, initializes the thread's stack.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
ptr_t _RME_Stack_Init(ptr_t Stack, ptr_t Stub, ptr_t Param1, ptr_t Param2, ptr_t Param3, ptr_t Param4)
{
    struct RME_CMX_Ret_Stack* Stack_Ptr;
    
    Stack_Ptr=(struct RME_CMX_Ret_Stack*)(Stack-RME_STACK_SAFE_SIZE-sizeof(struct RME_CMX_Ret_Stack));
    Stack_Ptr->R0=Param1;
    Stack_Ptr->R1=Param2;
    Stack_Ptr->R2=Param3;
    Stack_Ptr->R3=Param4;
    Stack_Ptr->R12=0;
    Stack_Ptr->LR=0;
    Stack_Ptr->PC=Stub;
    /* Initialize the xPSR to avoid a transition to ARM state */
    Stack_Ptr->XPSR=0x01000200;
    
    return (ptr_t)Stack_Ptr;
}
/* End Function:_RME_Stack_Init **********************************************/

/* Function:RME_Same_Prc_Thd_Switch_Test_Thd *********************************
Description : The thread for testing same-process thread switching performance.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Same_Prc_Thd_Switch_Test_Thd(ptr_t Param1, ptr_t Param2, ptr_t Param3, ptr_t Param4)
{
    ret_t Retval;
    /* Now we switch back to the init thread, immediately */
    while(1)
    {
        Retval=RME_CAP_OP(RME_SVC_THD_SWT,0,
                          RME_BOOT_INIT_THD,
                          0,
                          0);
    }
}
/* End Function:RME_Same_Prc_Thd_Switch_Test_Thd ****************************/

/* Function:RME_Same_Prc_Thd_Switch_Test *************************************
Description : The same-process thread switch test code.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Same_Prc_Thd_Switch_Test(void)
{
    /* Intra-process thread switching time */
    ret_t Retval;
    cnt_t Count;
    ptr_t Stack_Addr;
    ptr_t Temp;
    /* Initialize the thread's stack before entering it */
    Stack_Addr=_RME_Stack_Init((ptr_t)(&RME_Stack[2047]),
                               (ptr_t)RME_Thd_Stub,
                               1, 2, 3, 4);

    USR_DBG_S("\r\n(ptr_t)(&RME_Stack[2047])= ");
        USR_DBG_H((ptr_t)(&RME_Stack[2047]));
    USR_DBG_S("\r\nInitialize the thread's stack Stack_Addr= ");
    USR_DBG_H(Stack_Addr);

    /* There are still many bugs in the kernel. Need a white-box test to guarantee
     * that it is free of bugs. Find a scheme to do that */
    Retval=RME_CAP_OP(RME_SVC_THD_CRT,RME_BOOT_INIT_CPT,
                      RME_PARAM_D1(RME_BOOT_INIT_KOM)|RME_PARAM_D0(RME_BOOT_BENCH_THD),
                      RME_PARAM_D1(RME_BOOT_INIT_PRC)|RME_PARAM_D0(31),
                      RME_BOOT_BENCH_KOM_FRONTIER);
    USR_DBG_S("\r\ncreate a thread      retval= ");
    USR_DBG_I(Retval);

    /* Bind the thread to the processor */
    Retval=RME_CAP_OP(RME_SVC_THD_SCHED_BIND,RME_BOOT_BENCH_THD,
    		          RME_PARAM_D1(RME_BOOT_INIT_THD)|RME_PARAM_D0(RME_CID_NULL),
					  RME_PARAM_D1(11)|RME_PARAM_D0(RME_BOOT_INIT_PRC),
                      0);
    USR_DBG_S("\r\nBind the thread to the processor retval= ");
    USR_DBG_I(Retval);
    
    /* Set the execution information */
    Retval=RME_CAP_OP(RME_SVC_THD_EXEC_SET,0,
                      RME_BOOT_BENCH_THD,
                      (ptr_t)RME_Same_Prc_Thd_Switch_Test_Thd,
                      Stack_Addr);

    USR_DBG_S("\r\nSet the execution information retval= ");
    USR_DBG_I(Retval);

    /* Delegate some timeslice to it */
    Retval=RME_CAP_OP(RME_SVC_THD_TIME_XFER,0,
                      RME_BOOT_BENCH_THD,
                      RME_BOOT_INIT_THD,
                      10000000);
    
    /* Try to switch to that thread - should fail */
    Retval=RME_CAP_OP(RME_SVC_THD_SWT,0,
                      RME_BOOT_BENCH_THD,
                      0,
                      0);
    USR_DBG_S("\r\nTry to switch to that thread - should fail  retval= ");
    USR_DBG_I(Retval);
    /* Test result: intra-process ctxsw 358cycles/1.657us, frt w/mpu 163cycles/0.754us,
    * composite 324. opted max:323
    * all:33.0
    * empty: 4.09 - 0.409us, most time spent on internals
    * w/selections: 7.15 - maybe no need to check frozen cap from the proc.
    * w/checkings:10.926 - 317us.
    * total:16.57
    * 16.2us now, after cleaning up two bad things
    * 14.7us after CPUID optimizations. The quiescence hardly worked.
    * no cache - 3 times slower, mainly due to the flash. ART does not really help.
    * Performance cannot be further optimized anymore without compiler intrinsics.
    * Something terribly wrong with systick. 38 second wrapwround
    * This configuration, CPU works at 216MHz, correct, but the 
    * The TSC is always 8 cycles between reads.
    */
    _RME_Tsc_Init();
    //for(Count=0;Count<10000;Count++)
    for(Count=0;Count<10;Count++)
    {
        //Temp=RME_TSC();
        Retval=RME_CAP_OP(RME_SVC_THD_SWT,0,
                          RME_BOOT_BENCH_THD,
                          0,
                          0);
        //Temp=RME_TSC()-Temp;
       // Time[Count]=Temp-8;
        USR_DBG_S("\r\nTry to switch to that thread  retval= ");
        USR_DBG_I(Retval);
    }
    
    while(1);
}
/* End Function:RME_Same_Prc_Thd_Switch_Test ********************************/

/* Function:RME_Diff_Prc_Thd_Switch_Test_Thd *********************************
Description : The thread for testing same-process thread switching performance.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Diff_Prc_Thd_Switch_Test_Thd(ptr_t Param1, ptr_t Param2, ptr_t Param3, ptr_t Param4)
{
    ret_t Retval;
    /* Now we switch back to the init thread, immediately */
    while(1)
    {
        Retval=RME_CAP_OP(RME_SVC_THD_SWT,0,
                          RME_BOOT_INIT_THD,
                          0,
                          0);
    }
}
/* End Function:RME_Diff_Prc_Thd_Switch_Test_Thd ****************************/

/* Function:RME_Diff_Prc_Thd_Switch_Test *************************************
Description : The same-process thread switch test code.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Diff_Prc_Thd_Switch_Test(void)
{
    /* Intra-process thread switching time */
    ret_t Retval;
    cnt_t Count;
    ptr_t Stack_Addr;
    ptr_t Temp;
    ptr_t Frontier;
    
    Frontier=RME_BOOT_BENCH_KOM_FRONTIER;
    /* Initialize the thread's stack before entering it */
    Stack_Addr=_RME_Stack_Init((ptr_t)(&RME_Stack[2047]),
                               (ptr_t)RME_Thd_Stub,
                               1, 2, 3, 4);
    
    /* Create the page table for the whole address space range */
    Retval=RME_CAP_OP(RME_SVC_PGT_CRT,RME_BOOT_INIT_CPT,
                      RME_PARAM_D1(RME_BOOT_INIT_KOM)|RME_PARAM_Q1(RME_BOOT_BENCH_PGT_TOP)|
                      RME_PARAM_O1(29)|RME_PARAM_O0(3),
                      Frontier,
                      1);
//    Frontier+=;
//    /* Create the page table for the SRAM range */
//    Retval=RME_CAP_OP(RME_SVC_PGT_CRT,RME_BOOT_INIT_CPT,
//                      RME_PARAM_D1(RME_BOOT_INIT_KOM)|RME_PARAM_Q1(RME_BOOT_BENCH_PGT_SRAM)|
//                      RME_PARAM_O1(16)|RME_PARAM_O0(3),
//                      Frontier,
//                      0x20000001);
//    Frontier+=;
//    /* Map the pages into the top-level and the second-level */
//    RME_CAP_OP(RME_SVC_PGT_ADD,0,
//               RME_PARAM_Q1(RME_BOOT_BENCH_PGT_TOP)|0,
//               RME_PARAM_D1(RME_BOOT_INIT_PGT)|0,
//               |0)
                      
                  
    
    
    
    Retval=RME_CAP_OP(RME_SVC_THD_CRT,RME_BOOT_INIT_CPT,
                      RME_PARAM_D1(RME_BOOT_INIT_KOM)|RME_PARAM_D0(RME_BOOT_BENCH_THD),
                      RME_PARAM_D1(RME_BOOT_INIT_PRC)|RME_PARAM_D0(31),
                      RME_BOOT_BENCH_KOM_FRONTIER);
    
    /* Bind the thread to the processor */
    Retval=RME_CAP_OP(RME_SVC_THD_SCHED_BIND,0,
                      RME_BOOT_BENCH_THD,
                      RME_BOOT_INIT_THD,
                      0);
    
    /* Set the execution information */
    Retval=RME_CAP_OP(RME_SVC_THD_EXEC_SET,0,
                      RME_BOOT_BENCH_THD,
                      (ptr_t)RME_Same_Prc_Thd_Switch_Test_Thd,
                      Stack_Addr);
                      
    /* Delegate some timeslice to it */
    Retval=RME_CAP_OP(RME_SVC_THD_TIME_XFER,0,
                      RME_BOOT_BENCH_THD,
                      RME_BOOT_INIT_THD,
                      10000000);
    
    /* Try to switch to that thread - should fail */
    Retval=RME_CAP_OP(RME_SVC_THD_SWT,0,
                      RME_BOOT_BENCH_THD,
                      0,
                      0);
    /* Test result: intra-process ctxsw 358cycles/1.657us, frt w/mpu 163cycles/0.754us,
    * composite 324. opted max:323
    * all:33.0
    * empty: 4.09 - 0.409us, most time spent on internals
    * w/selections: 7.15 - maybe no need to check frozen cap from the proc.
    * w/checkings:10.926 - 317us.
    * total:16.57
    * 16.2us now, after cleaning up two bad things
    * 14.7us after CPUID optimizations. The quiescence hardly worked.
    * no cache - 3 times slower, mainly due to the flash. ART does not really help.
    * Performance cannot be further optimized anymore without compiler intrinsics.
    * Something terribly wrong with systick. 38 second wrapwround
    * This configuration, CPU works at 216MHz, correct, but the 
    * The TSC is always 8 cycles between reads.
    */
    _RME_Tsc_Init();
    for(Count=0;Count<10000;Count++)
    {
        //Temp=RME_TSC();
        Retval=RME_CAP_OP(RME_SVC_THD_SWT,0,
                          RME_BOOT_BENCH_THD,
                          0,
                          0);
        //Temp=RME_TSC()-Temp;
        //Time[Count]=Temp-8;
    }
    
    while(1);
}
/* End Function:RME_Diff_Prc_Thd_Switch_Test ********************************/

/* Function:RME_Benchmark *****************************************************
Description : The benchmark entry, also the init thread.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Benchmark(void)
{
    USR_DBG_S("\r\nbegin test ");
    USR_DBG_H(0x111111);
    RME_Same_Prc_Thd_Switch_Test();
}
/* End Function:RME_Benchmark ************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
