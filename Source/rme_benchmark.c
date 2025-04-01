/******************************************************************************
Filename    : benchmark.c
Author      : pry
Date        : 04/09/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The benchmark file for RME.
******************************************************************************/

/* Include *******************************************************************/
#include"benchmark.h"

ptr_t Cur_addr;
ptr_t start;
ptr_t end;
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
 //   TIM_HandleTypeDef TIM2_Handle;
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

/* Function:RME_Same_Prc_Thd_Switch_Test_Thd *********************************
Description : The thread for testing same-process thread switching performance.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Same_Prc_Thd_Switch_Test_Thd(ptr_t Param1)
{
    ret_t Retval;

    /*USR_DBG_S("\r\n hello! this is thread ");
    USR_DBG_I(Param1);*/
    /* Now we switch back to the init thread, immediately */
    while(1)
    {
        /*Retval=RME_CAP_OP(RME_SVC_THD_SWT,0,
                          RME_BOOT_INIT_THD,
                          0,
                          0);*/
        Retval=RME_Thd_Swt(RME_BOOT_INIT_THD,0);
    }
}
/* End Function:RME_Same_Prc_Thd_Switch_Test_Thd ****************************/


void RME_Thd_Create(cid_t Cap_Thd,ptr_t Tid,ptr_t Prc,ptr_t Entry,ptr_t Raddr,cid_t Cap_Sig,ptr_t Time, ptr_t Stack)
{
	ret_t Retval;
	cnt_t Count;
	ptr_t Stack_Addr;
	ptr_t Temp;
	USR_DBG_S("\r\n-----create a thread , TID= ");
	USR_DBG_H(Tid);
	/* Initialize the thread's stack before entering it */
	Stack_Addr=(ptr_t)Stack;

	/* create a thread */
	Retval=RME_Thd_Crt(RME_BOOT_INIT_CPT,RME_BOOT_INIT_KOM,Cap_Thd,
					   RME_BOOT_INIT_PRC,RME_THD_PRIO_MAX,Raddr,0);
	USR_DBG_S("\r\ncreate a thread      retval= ");
	USR_DBG_I(Retval);

	/* Bind the thread to the processor */
	Retval=RME_Thd_Sched_Bind(Cap_Thd,RME_BOOT_INIT_THD,Cap_Sig,
							  Tid,Prc,RME_BOOT_HYPER_KOM_VADDR);
	USR_DBG_S("\r\nBind the thread to the processor retval= ");
	USR_DBG_I(Retval);
	/* Set the execution information */

	Retval=RME_Thd_Exec_Set(Cap_Thd,Entry,Stack_Addr,Tid);
	//RME_Stack[2000]=1234;
	USR_DBG_S("\r\nSet the execution information retval= ");
	USR_DBG_I(Retval);
	USR_DBG_S("\r\n");
	/* Delegate some timeslice to it */
	Retval=RME_Thd_Time_Xfer(Cap_Thd,RME_BOOT_INIT_THD,Time);
	USR_DBG_S("\r\nDelegate some timeslice to it retval= ");
	USR_DBG_I(Retval);
}

void RME_Same_Prc_Thd_Sig_Test_Thd_send(void) //low prio 4
{
	USR_DBG_S("\r\nEntering send thread!");


	while(1)
	{
		USR_DBG_S("\r\nSend!");
		RME_Sig_Snd(RME_BOOT_SIG_BENCH);
	}

}

void RME_Same_Prc_Thd_Sig_Test_Thd_recv(void) // high prio 5
{
	USR_DBG_S("\r\nEntering receive thread!");

	while(1)
	{
		if(RME_Sig_Rcv(RME_BOOT_SIG_BENCH,RME_RCV_BS)!=1)
		{
			USR_DBG_S("\r\nReceived wrong number!");
			while(1);
		}
		USR_DBG_S("\r\nReceived!");
	}
}




/* Function:RME_Same_Prc_Thd_Switch_Test_Thd *********************************
Description : The thread for testing same-process thread signal send and receive performance.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Same_Prc_Thd_Sig_Test_Thd_main(ptr_t TID)
{
	USR_DBG_S("\r\n");
	USR_DBG_S("\r\n/****************");
	USR_DBG_S("begin RME_Same_Prc_Thd_Sig_Test_Thd_main");
	USR_DBG_S("****************/");
	USR_DBG_S("\r\nHello this is thread main");
	ret_t Retval;


	/* send the begin signal */
	RME_Sig_Snd(RME_BOOT_SIG_BENCH);
	RME_Thd_Swt(RME_BOOT_BENCH_SEN_THD,0);

	while(1);
}
/* Function:RME_Same_Prc_Thd_Switch_Testinit *************************************
Description : The same-process thread signal send and receive test code.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Same_Prc_Thd_Sig_Testinit(void)
{
	ret_t Retval;

	/* create a signal end point */
	Retval=RME_Sig_Crt(RME_BOOT_INIT_CPT,RME_BOOT_SIG_BENCH);
	USR_DBG_S("\r\ncreate a signal end point      retval= ");
	USR_DBG_I(Retval);
	/* create signal test thread */
	//void RME_Thd_Create(cid_t Cap_Thd,ptr_t Tid,ptr_t Prc,ptr_t Entry,ptr_t Raddr,cid_t Cap_Sig,ptr_t Time)
	RME_Thd_Create(RME_BOOT_BENCH_RCV_THD,RME_TID_4,5,(ptr_t)RME_Same_Prc_Thd_Sig_Test_Thd_recv,
                   Cur_addr,RME_BOOT_SIG_BENCH,RME_THD_INF_TIME,(ptr_t)&RME_Stack3[2000]);
	Cur_addr+=512;
	RME_Thd_Create(RME_BOOT_BENCH_SEN_THD,RME_TID_3,4,(ptr_t)RME_Same_Prc_Thd_Sig_Test_Thd_send,
				   Cur_addr,RME_BOOT_SIG_BENCH,RME_THD_INF_TIME,(ptr_t)&RME_Stack2[2000]);
	Cur_addr+=512;
}

/* Function:RME_Same_Prc_Thd_Switch_Test *************************************
Description : The same-process thread switch test code.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Same_Prc_Thd_Switch_Test(void)
{
	/*USR_DBG_S("\r\n/****************");
	USR_DBG_S("begin RME_Same_Prc_Thd_Switch_Test");
    /* Intra-process thread switching time */
    ret_t Retval;
    cnt_t Count;
    ptr_t Stack_Addr;
    ptr_t Temp;
    /* Initialize the thread's stack before entering it */
    Stack_Addr=(ptr_t)&RME_Stack1[2000];/* _RME_Stack_Init((ptr_t)(&RME_Stack[2047]),
                               (ptr_t)RME_Thd_Stub,
                               1, 2, 3, 4); */

    /*USR_DBG_S("\r\n(ptr_t)(&RME_Stack[2047])= ");
    USR_DBG_H((ptr_t)(&RME_Stack1[2047]));
    USR_DBG_S("\r\nInitialize the thread's stack Stack_Addr= ");
    USR_DBG_H(Stack_Addr);*/

    /* There are still many bugs in the kernel. Need a white-box test to guarantee
     * that it is free of bugs. Find a scheme to do that */
    /*Retval=RME_CAP_OP(RME_SVC_THD_CRT,RME_BOOT_INIT_CPT,
                      RME_PARAM_D1(RME_BOOT_INIT_KOM)|RME_PARAM_D0(RME_BOOT_BENCH_THD),
                      RME_PARAM_D1(RME_BOOT_INIT_PRC)|RME_PARAM_D0(31),
                      RME_BOOT_BENCH_KOM_FRONTIER);*/
    Retval=RME_Thd_Crt(RME_BOOT_INIT_CPT,RME_BOOT_INIT_KOM,RME_BOOT_BENCH_THD,
                       RME_BOOT_INIT_PRC,RME_THD_PRIO_MAX,RME_BOOT_BENCH_KOM_FRONTIER,0);
    //USR_DBG_S("\r\ncreate a thread      retval= ");
    //USR_DBG_I(Retval);

    /* Bind the thread to the processor */
    /*Retval=RME_CAP_OP(RME_SVC_THD_SCHED_BIND,RME_BOOT_BENCH_THD,
    		          RME_PARAM_D1(RME_BOOT_INIT_THD)|RME_PARAM_D0(RME_CID_NULL),
					  RME_PARAM_D1(RME_TID_2)|RME_PARAM_D0(RME_BOOT_INIT_PRC),
					  RME_BOOT_HYPER_KOM_VADDR);*/
    Retval=RME_Thd_Sched_Bind(RME_BOOT_BENCH_THD,RME_BOOT_INIT_THD,RME_CID_NULL,
                              RME_TID_2,0,RME_BOOT_HYPER_KOM_VADDR);
    //USR_DBG_S("\r\nBind the thread to the processor retval= ");
    //USR_DBG_I(Retval);
    extern void RME_Deadloop(void);
    /* Set the execution information */
    /*Retval=RME_CAP_OP(RME_SVC_THD_EXEC_SET,RME_BOOT_BENCH_THD,
	         	 	 (ptr_t)RME_Same_Prc_Thd_Switch_Test_Thd,
    		         //(ptr_t)RME_Deadloop,
					 Stack_Addr,RME_TID_2);*/
    Retval=RME_Thd_Exec_Set(RME_BOOT_BENCH_THD,(ptr_t)RME_Same_Prc_Thd_Switch_Test_Thd,Stack_Addr,RME_TID_2);
    //RME_Stack[2000]=1234;
    //USR_DBG_S("\r\nSet the execution information retval= ");
    //USR_DBG_I(Retval);
    //USR_DBG_S("\r\n");
    /* Delegate some timeslice to it */
    /*Retval=RME_CAP_OP(RME_SVC_THD_TIME_XFER,RME_BOOT_BENCH_THD,
                      RME_BOOT_BENCH_THD,
                      RME_BOOT_INIT_THD,
                      10000000);*/
    Retval=RME_Thd_Time_Xfer(RME_BOOT_BENCH_THD,RME_BOOT_INIT_THD,RME_THD_INF_TIME);

    //Retval=RME_Thd_Time_Xfer(RME_BOOT_BENCH_THD,RME_BOOT_INIT_THD,1000);
    //USR_DBG_S("\r\nDelegate some timeslice to it retval= ");
         //USR_DBG_I(Retval);

    Retval=RME_Thd_Swt(RME_BOOT_BENCH_THD,0);
    /*USR_DBG_S("\r\nTry to switch to that thread - should fail  retval= ");
    USR_DBG_I(Retval);*/
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
    //_RME_Tsc_Init();
    //for(Count=0;Count<10000;Count++)
    start=get_time();
    for(Count=0;Count<10000;Count++)
    {
         RME_Thd_Swt(RME_BOOT_BENCH_THD,0);
        //Temp=RME_TSC();
        /*Retval=RME_CAP_OP(RME_SVC_THD_SWT,0,
                          RME_BOOT_BENCH_THD,
                          0,
                          0);*/
        //Retval=RME_Thd_Swt(RME_BOOT_BENCH_THD,0);
        //Temp=RME_TSC()-Temp;
       // Time[Count]=Temp-8;
        //USR_DBG_S("\r\nTry to switch to that thread  retval= ");
        //USR_DBG_I(Retval);
    }
    
    end=get_time();
    USR_DBG_S("\r\n swt 10000 times time= ");
    USR_DBG_I((end-start)*2);
    USR_DBG_S("\r\n swt 1 times time= ");
    USR_DBG_I((end-start)/20000*2);
    USR_DBG_S("\r\ntest done ");

    // test sig_snd/sig_rcv



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
        /*Retval=RME_CAP_OP(RME_SVC_THD_SWT,0,
                          RME_BOOT_INIT_THD,
                          0,
                          0);*/
        Retval=RME_Thd_Swt(RME_BOOT_INIT_THD,0);
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
    Stack_Addr=_RME_Stack_Init((ptr_t)(&RME_Stack1[2047]),
                               (ptr_t)RME_Thd_Stub,
                               1, 2, 3, 4);
    
    /* Create the page table for the whole address space range */
    /*Retval=RME_CAP_OP(RME_SVC_PGT_CRT,RME_BOOT_INIT_CPT,
                      RME_PARAM_D1(RME_BOOT_INIT_KOM)|RME_PARAM_Q1(RME_BOOT_BENCH_PGT_TOP)|
                      RME_PARAM_O1(29)|RME_PARAM_O0(3),
                      Frontier,
                      1);*/
//    Frontier+=;
//    /* Create the page table for the SRAM range */
//    Retval=RME_CAP_OP(RME_SVC_PGT_CRT,RME_BOOT_INIT_CPT,
//                      RME_PARAM_D1(RME_BOOT_INIT_KOM)|RME_PARAM_Q1(RME_BOOT_BENCH_PGT_SRAM)|
//                      RME_PARAM_O1(16)|RME_PARAM_O0(3),
//                      Frontier,
//                      0x20000001);*/
//    Frontier+=;
//    /* Map the pages into the top-level and the second-level */
//    RME_CAP_OP(RME_SVC_PGT_ADD,0,
//               RME_PARAM_Q1(RME_BOOT_BENCH_PGT_TOP)|0,
//               RME_PARAM_D1(RME_BOOT_INIT_PGT)|0,
//               |0)*/
                      
                  
    
    
    
    /*Retval=RME_CAP_OP(RME_SVC_THD_CRT,RME_BOOT_INIT_CPT,
                      RME_PARAM_D1(RME_BOOT_INIT_KOM)|RME_PARAM_D0(RME_BOOT_BENCH_THD),
                      RME_PARAM_D1(RME_BOOT_INIT_PRC)|RME_PARAM_D0(31),
                      RME_BOOT_BENCH_KOM_FRONTIER);*/
    
    /* Bind the thread to the processor */
    /*Retval=RME_CAP_OP(RME_SVC_THD_SCHED_BIND,0,
                      RME_BOOT_BENCH_THD,
                      RME_BOOT_INIT_THD,
                      0);*/
    
    /* Set the execution information */
    /*Retval=RME_CAP_OP(RME_SVC_THD_EXEC_SET,0,
                      RME_BOOT_BENCH_THD,
                      (ptr_t)RME_Same_Prc_Thd_Switch_Test_Thd,
                      Stack_Addr);*/
                      
    /* Delegate some timeslice to it */
    /*Retval=RME_CAP_OP(RME_SVC_THD_TIME_XFER,0,
                      RME_BOOT_BENCH_THD,
                      RME_BOOT_INIT_THD,
                      10000000);*/
    
    /* Try to switch to that thread - should fail */
    /*Retval=RME_CAP_OP(RME_SVC_THD_SWT,0,
                      RME_BOOT_BENCH_THD,
                      0,
                      0);*/
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
        /*Retval=RME_CAP_OP(RME_SVC_THD_SWT,0,
                          RME_BOOT_BENCH_THD,
                          0,
                          0);*/
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
	Cur_addr=RME_BOOT_BENCH_KOM_FRONTIER;
    USR_DBG_S("\r\nTest begin!");

    //RME_Same_Prc_Thd_Sig_Testinit();
    RME_Same_Prc_Thd_Switch_Test();
    USR_DBG_S("\r\nTest done!");
    while(1);
}
/* End Function:RME_Benchmark ************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
