/******************************************************************************
Filename    : benchmark.c
Author      : pry
Date        : 04/09/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The benchmark file for RME.
******************************************************************************/

/* Include *******************************************************************/
#include"sys.h"
ptr_t Cur_addr;
ptr_t start;
ptr_t end;
/* Need to export error codes, and size of each object, in words! */
/* End Include ***************************************************************/


/* Function:RME_Same_Prc_Thd_Switch_Test_Thd *********************************
Description : The thread for testing same-process thread switching performance.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Same_Prc_Thd_Switch_Test_Thd(ptr_t Param1)
{
    ret_t Retval;


    /* Now we switch back to the init thread, immediately */
    while(1)
    {
        /*Retval=RME_CAP_OP(RME_SVC_THD_SWT,0,
                          RME_BOOT_INIT_THD,
                          0,
                          0);*/
    	/*USR_DBG_S("\r\n hello! this is thread ");
    	    USR_DBG_I(Param1);*/
        Retval=RME_Thd_Swt(RME_BOOT_INIT_THD,0);
    }
}
/* End Function:RME_Same_Prc_Thd_Switch_Test_Thd ****************************/
/* Function:RME_Diff_Prc_Thd_Switch_Test_Thd *********************************
Description : The thread for testing same-process thread switching performance.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Diff_Prc_Thd_Switch_Test_Thd(ptr_t Param1)
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



void RME_Thd_Create(cid_t Cap_Prc,cid_t Cap_Thd,ptr_t Tid,ptr_t Prio,ptr_t Entry,ptr_t Raddr,cid_t Cap_Sig,ptr_t Time, ptr_t Stack)
{
	ret_t Retval;
	cnt_t Count;
	ptr_t Stack_Addr;
	ptr_t Temp;
	/*USR_DBG_S("\r\n-----create a thread , TID= ");
	USR_DBG_H(Tid);*/
	/* Initialize the thread's stack before entering it */
	Stack_Addr=(ptr_t)Stack;

	/* create a thread */
	Retval=RME_Thd_Crt(RME_BOOT_INIT_CPT,RME_BOOT_INIT_KOM,Cap_Thd,
					   Cap_Prc,RME_THD_PRIO_MAX,Raddr,0);
	/*USR_DBG_S("\r\ncreate a thread      retval= ");
	USR_DBG_I(Retval);*/

	/* Bind the thread to the processor */
	Retval=RME_Thd_Sched_Bind(Cap_Thd,RME_BOOT_INIT_THD,Cap_Sig,
							  Tid,Prio,RME_BOOT_HYPER_KOM_VADDR);
	/*USR_DBG_S("\r\nBind the thread to the processor retval= ");
	USR_DBG_I(Retval);*/
	/* Set the execution information */

	Retval=RME_Thd_Exec_Set(Cap_Thd,Entry,Stack_Addr,Tid);
	//RME_Stack[2000]=1234;
	/*USR_DBG_S("\r\nSet the execution information retval= ");
	USR_DBG_I(Retval);
	USR_DBG_S("\r\n");*/
	/* Delegate some timeslice to it */
	Retval=RME_Thd_Time_Xfer(Cap_Thd,RME_BOOT_INIT_THD,Time);
	/*USR_DBG_S("\r\nDelegate some timeslice to it retval= ");
	USR_DBG_I(Retval);*/
}

void RME_Same_Prc_Thd_Sig_Test_Thd_send(void) //low prio 4
{
	USR_DBG_S("\r\nEntering send thread!");


	while(1)
	{
		USR_DBG_S("\r\nSend!");
		start=get_time();
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
		end=get_time();
		USR_DBG_S("\r\nReceived!");
		USR_DBG_S("\r\n same prc sig send take time");
		USR_DBG_I((end-start)*2);
	}
}
void RME_Diff_Prc_Thd_Sig_Test_Thd_send(void) //low prio 4
{
	USR_DBG_S("\r\nEntering diff send thread!");

	while(1)
	{
		USR_DBG_S("\r\nSend!");
		start=get_time();
		RME_Sig_Snd(RME_BOOT_SIG_BENCH);
	}

}

void RME_Diff_Prc_Thd_Sig_Test_Thd_recv(void) // high prio 5
{
	USR_DBG_S("\r\nEntering diff receive thread!");

	while(1)
	{
		if(RME_Sig_Rcv(RME_BOOT_SIG_BENCH,RME_RCV_BS)!=1)
		{
			USR_DBG_S("\r\nReceived wrong number!");
			while(1);
		}
		end=get_time();
		USR_DBG_S("\r\nReceived!");
		USR_DBG_S("\r\n diff prc sig send take time");
		USR_DBG_I((end-start)*2);
	}
}



/* Description : The thread for testing same-process thread signal send and receive performance.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Same_Prc_Thd_Sig_Test_Thd_main_nouse(ptr_t TID)
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
	RME_Thd_Create(RME_BOOT_INIT_PRC,RME_BOOT_BENCH_RCV_THD,RME_TID_4,5,(ptr_t)RME_Same_Prc_Thd_Sig_Test_Thd_recv,
                   Cur_addr,RME_BOOT_SIG_BENCH,RME_THD_INF_TIME,(ptr_t)&RME_Stack3[2000]);
	Cur_addr+=512;
	RME_Thd_Create(RME_BOOT_INIT_PRC,RME_BOOT_BENCH_SEN_THD,RME_TID_3,4,(ptr_t)RME_Same_Prc_Thd_Sig_Test_Thd_send,
				   Cur_addr,RME_BOOT_SIG_BENCH,RME_THD_INF_TIME,(ptr_t)&RME_Stack2[2000]);
	Cur_addr+=512;
}


void RME_Diff_Prc_Thd_Sig_Testinit(void)
{
	ret_t Retval;
	//Cur_addr=RME_BOOT_BENCH_PGT_RADDR;
	/* create a signal end point */
	Retval=RME_Sig_Crt(RME_BOOT_INIT_CPT,RME_BOOT_SIG_BENCH);
	USR_DBG_S("\r\ncreate a signal end point      retval= ");
	USR_DBG_I(Retval);
	/* create signal test thread */
	//void RME_Thd_Create(cid_t Cap_Thd,ptr_t Tid,ptr_t Prc,ptr_t Entry,ptr_t Raddr,cid_t Cap_Sig,ptr_t Time)
	RME_Thd_Create(RME_BOOT_INIT_PRC,RME_BOOT_BENCH_RCV_THD,RME_TID_4,5,(ptr_t)RME_Diff_Prc_Thd_Sig_Test_Thd_recv,
                   Cur_addr,RME_BOOT_SIG_BENCH,RME_THD_INF_TIME,(ptr_t)&RME_Stack3[2000]);
	Cur_addr=RME_BOOT_BENCH_PGT_RADDR;

	    ptr_t Count;


		USR_DBG_S(" \r\nCur_addr= ");
	    USR_DBG_H(Cur_addr);
		Retval=RME_Pgt_Crt(RME_BOOT_INIT_CPT,RME_BOOT_INIT_KOM,RME_BOOT_BENCH_PGT,
				RME_BOOT_BENCH_PGT_RADDR,0x00000000U,RME_PGT_TOP,RME_PGT_SIZE_1M,RME_PGT_NUM_4K);

		USR_DBG_S("\r\ncreate rcv page table  retval= ");
		USR_DBG_I(Retval);

		// add page to benchmark pgt
		for(Count=0U;Count<0x400U;Count++)
		    {
			Retval=RME_Pgt_Add(RME_BOOT_BENCH_PGT,Count,RME_PGT_ALL_DYN,
											 RME_BOOT_INIT_PGT,
											 Count,
											 0);
			    /*USR_DBG_S("\r\nadd page to benchmark pgt  retval= ");
				USR_DBG_I(Retval);*/
		    }
			/* Device memory 1, 512MiB 0x40000000 -> 0x40000000 */
		    for(Count=0U;Count<0x200U;Count++)
		    {
		    	Retval=RME_Pgt_Add(RME_BOOT_BENCH_PGT,(Count+0x400U),
											 RME_PGT_READ|RME_PGT_WRITE,
											 RME_BOOT_INIT_PGT,
											 (Count+0x400U),
											 0);
				/*USR_DBG_S("\r\nadd page to benchmark pgt  retval= ");
							USR_DBG_I(Retval);*/
		    }

		    /* Device memory 2, 512MiB 0x60000000 -> 0xE0000000 */
		    for(Count=0U;Count<0x200U;Count++)
		    {
		    	Retval=RME_Pgt_Add(RME_BOOT_BENCH_PGT,(Count+0x600U),
		    								 RME_PGT_READ|RME_PGT_WRITE,
											 RME_BOOT_INIT_PGT,
											 (Count+0x600U),
											 0);
				/*USR_DBG_S("\r\nadd page to benchmark pgt  retval= ");
										USR_DBG_I(Retval);*/
		    }
		    USR_DBG_S("\r\nFirst section's first entry ");
		    USR_DBG_H(RME_A7A_REG(Cur_addr+RME_KOM_VA_BASE));
		    USR_DBG_S(" @ ");
		    USR_DBG_H(Cur_addr+RME_KOM_VA_BASE);

		    USR_DBG_S("\r\nFirst section's 0x080th entry ");
		    USR_DBG_H(RME_A7A_REG(Cur_addr+0x080*RME_WORD_BYTE+RME_KOM_VA_BASE));
		    USR_DBG_S(" @ ");
		    USR_DBG_H(Cur_addr+0x080*RME_WORD_BYTE+RME_KOM_VA_BASE);

		    USR_DBG_S("\r\nSecond section's first entry ");
		    USR_DBG_H(RME_A7A_REG(Cur_addr+0x400*RME_WORD_BYTE+RME_KOM_VA_BASE));
		    USR_DBG_S(" @ ");
		    USR_DBG_H(Cur_addr+0x400*RME_WORD_BYTE+RME_KOM_VA_BASE);

		    USR_DBG_S("\r\nThird section's first entry ");
		    USR_DBG_H(RME_A7A_REG(Cur_addr+0x600*RME_WORD_BYTE+RME_KOM_VA_BASE));
		    USR_DBG_S(" @ ");
		    USR_DBG_H(Cur_addr+0x600*RME_WORD_BYTE+RME_KOM_VA_BASE);

		    /*USR_DBG_S("\r\nKernel pgtbl's 80th entry ");
		    USR_DBG_H(RME_A7A_REG(((ptr_t)(&__RME_A7A_Kern_Pgt))+0x080*RME_WORD_BYTE));
		    USR_DBG_S(" @ ");
		    USR_DBG_H(((ptr_t)(&__RME_A7A_Kern_Pgt))+0x080*RME_WORD_BYTE);*/

		//创建新进程 create benchmark prc
		    Retval=RME_Prc_Crt(RME_BOOT_INIT_CPT,RME_BOOT_BENCH_PRC,RME_BOOT_INIT_CPT,
		    				   RME_BOOT_BENCH_PGT);
		    USR_DBG_S("\r\n create benchmark prc  retval= ");
		    						USR_DBG_I(Retval);

		    		//创建新线程 create new thread

	  /*RME_Thd_Create(RME_BOOT_BENCH_PRC,RME_BOOT_BENCH_THD,RME_TID_4,0,
		    		(ptr_t)RME_Same_Prc_Thd_Switch_Test_Thd,
					RME_BOOT_BENCH_KOM_FRONTIER,
					RME_CID_NULL,RME_THD_INF_TIME,
					(ptr_t)&RME_Stack3[2000]);*/
		    						Cur_addr=RME_BOOT_BENCH_KOM_FRONTIER+512;
		    						USR_DBG_S(" \r\nCur_addr= ");
		    							    USR_DBG_H(Cur_addr);
		 RME_Thd_Create(RME_BOOT_BENCH_PRC,RME_BOOT_BENCH_SEN_THD,RME_TID_3,4,(ptr_t)RME_Diff_Prc_Thd_Sig_Test_Thd_send,
	  				   Cur_addr,RME_BOOT_SIG_BENCH,RME_THD_INF_TIME,(ptr_t)&RME_Stack2[2000]);
	  	  	  	  	 USR_DBG_S("\r\n create send thd  retval= ");
	  	  	  	  	 USR_DBG_I(Retval);

		    		//切换到新线程 switch to new prc
		    	    Retval=RME_Thd_Swt(RME_BOOT_BENCH_SEN_THD,0);
	  	  	  	  	 while(1);

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
    //_RME_Tsc_Init();
    //for(Count=0;Count<10000;Count++)
    /*start=get_time();

    RME_Thd_Swt(RME_BOOT_BENCH_THD,0);
    end=get_time();
    USR_DBG_S("\r\n swt 1 times time= ");
    USR_DBG_I(end-start);*/


    start=get_time();
    for(Count=0;Count<10000;Count++)
    {
    	//start=get_time();

    	    RME_Thd_Swt(RME_BOOT_BENCH_THD,0);
    	    /*end=get_time();
    	    USR_DBG_S("\r\n swt 1 times time= ");
    	    USR_DBG_I(end-start);*/

    }
    
    end=get_time();
    USR_DBG_S("\r\n swt 10000 times time avg = ");
    USR_DBG_I((end-start)/10000);

    USR_DBG_S("\r\ntest done ");

    // test sig_snd/sig_rcv



    while(1);
}
/* End Function:RME_Same_Prc_Thd_Switch_Test ********************************/



/* Function:RME_Diff_Prc_Thd_Switch_Test *************************************
Description : The same-process thread switch test code.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Diff_Prc_Thd_Switch_Test_noused(void)
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

/* Function:RME_Diff_Prc_Switch_Test *************************************
Description : The same-process switch test code.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Diff_Prc_Switch_Test(void)
{
	// create benchmark page table
	ret_t Retval;
	ptr_t Count;
	Cur_addr=RME_BOOT_BENCH_PGT_RADDR;

	/*USR_DBG_S(" \r\nCur_addr= ");
    USR_DBG_H(Cur_addr);*/
	Retval=RME_Pgt_Crt(RME_BOOT_INIT_CPT,RME_BOOT_INIT_KOM,RME_BOOT_BENCH_PGT,
			    	   Cur_addr,0x00000000U,RME_PGT_TOP,RME_PGT_SIZE_1M,RME_PGT_NUM_4K);

	/*USR_DBG_S("\r\ncreate benchmark page table  retval= ");
	USR_DBG_I(Retval);*/

	// add page to benchmark pgt
	for(Count=0U;Count<0x400U;Count++)
	    {
		Retval=RME_Pgt_Add(RME_BOOT_BENCH_PGT,Count,RME_PGT_ALL_DYN,
										 RME_BOOT_INIT_PGT,
										 Count,
										 0);
		    /*USR_DBG_S("\r\nadd page to benchmark pgt  retval= ");
			USR_DBG_I(Retval);*/
	    }
		/* Device memory 1, 512MiB 0x40000000 -> 0x40000000 */
	    for(Count=0U;Count<0x200U;Count++)
	    {
	    	Retval=RME_Pgt_Add(RME_BOOT_BENCH_PGT,(Count+0x400U),
										 RME_PGT_READ|RME_PGT_WRITE,
										 RME_BOOT_INIT_PGT,
										 (Count+0x400U),
										 0);
			/*USR_DBG_S("\r\nadd page to benchmark pgt  retval= ");
						USR_DBG_I(Retval);*/
	    }

	    /* Device memory 2, 512MiB 0x60000000 -> 0xE0000000 */
	    for(Count=0U;Count<0x200U;Count++)
	    {
	    	Retval=RME_Pgt_Add(RME_BOOT_BENCH_PGT,(Count+0x600U),
	    								 RME_PGT_READ|RME_PGT_WRITE,
										 RME_BOOT_INIT_PGT,
										 (Count+0x600U),
										 0);
			/*USR_DBG_S("\r\nadd page to benchmark pgt  retval= ");
									USR_DBG_I(Retval);*/
	    }
	    /*USR_DBG_S("\r\nFirst section's first entry ");
	    USR_DBG_H(RME_A7A_REG(Cur_addr+RME_KOM_VA_BASE));
	    USR_DBG_S(" @ ");
	    USR_DBG_H(Cur_addr+RME_KOM_VA_BASE);

	    USR_DBG_S("\r\nFirst section's 0x080th entry ");
	    USR_DBG_H(RME_A7A_REG(Cur_addr+0x080*RME_WORD_BYTE+RME_KOM_VA_BASE));
	    USR_DBG_S(" @ ");
	    USR_DBG_H(Cur_addr+0x080*RME_WORD_BYTE+RME_KOM_VA_BASE);

	    USR_DBG_S("\r\nSecond section's first entry ");
	    USR_DBG_H(RME_A7A_REG(Cur_addr+0x400*RME_WORD_BYTE+RME_KOM_VA_BASE));
	    USR_DBG_S(" @ ");
	    USR_DBG_H(Cur_addr+0x400*RME_WORD_BYTE+RME_KOM_VA_BASE);

	    USR_DBG_S("\r\nThird section's first entry ");
	    USR_DBG_H(RME_A7A_REG(Cur_addr+0x600*RME_WORD_BYTE+RME_KOM_VA_BASE));
	    USR_DBG_S(" @ ");
	    USR_DBG_H(Cur_addr+0x600*RME_WORD_BYTE+RME_KOM_VA_BASE);

	    /*USR_DBG_S("\r\nKernel pgtbl's 80th entry ");
	    USR_DBG_H(RME_A7A_REG(((ptr_t)(&__RME_A7A_Kern_Pgt))+0x080*RME_WORD_BYTE));
	    USR_DBG_S(" @ ");
	    USR_DBG_H(((ptr_t)(&__RME_A7A_Kern_Pgt))+0x080*RME_WORD_BYTE);*/

	    //创建新进程 create benchmark prc
	    Retval=RME_Prc_Crt(RME_BOOT_INIT_CPT,RME_BOOT_BENCH_PRC,RME_BOOT_INIT_CPT,
	    				   RME_BOOT_BENCH_PGT);
	    /*USR_DBG_S("\r\n create benchmark prc  retval= ");
	    						USR_DBG_I(Retval);*/

	    		//创建新线程 create new thread

	    		RME_Thd_Create(RME_BOOT_BENCH_PRC,RME_BOOT_BENCH_THD,RME_TID_4,0,
	    		(ptr_t)RME_Diff_Prc_Thd_Switch_Test_Thd,
				RME_BOOT_BENCH_KOM_FRONTIER,
				RME_CID_NULL,RME_THD_INF_TIME,
				(ptr_t)&RME_Stack3[2000]);


	    		//切换到新线程 switch to new prc

	    		start=get_time();
	    		for(Count=0;Count<10000;Count++)
	    		    {
	    				//start=get_time();
	    				Retval=RME_Thd_Swt(RME_BOOT_BENCH_THD,0);
	    				/*end=get_time();
	    				USR_DBG_S("\r\n switch to new prc time= ");
	    				USR_DBG_I(end-start);
	    				/*USR_DBG_S("\r\n swt benchmark prc  retval= ");
	    				USR_DBG_I(Retval);*/
	    		    }
	    	    //calculate time
	    	    end=get_time();
	    	    USR_DBG_S("\r\n switch to new prc 10000 time  = ");
	    	    	    	    USR_DBG_I(end-start);
	    	    USR_DBG_S("\r\n switch to new prc 10000 time avg = ");
	    	    USR_DBG_I((end-start)/10000);

	    	    /*USR_DBG_S("\r\n switch to new prc  retval= ");
	    	    USR_DBG_I(Retval);*/
	    	    USR_DBG_S("\r\nTest done!");
	    	    while(1);
}

void RME_Inv_test_fuction(int param)//迁移调用函数
{
	//USR_DBG_S("\r\n inv fuction!");
		//同步迁移调用返回
		RME_Inv_Ret(param);

}

void RME_Deadloop(void);

void RME_Diff_Inv_test(void)
{

	ret_t Retval;
	ptr_t Count;
	Cur_addr=RME_BOOT_BENCH_PGT_RADDR;
	//创建添加页表
	Retval=RME_Pgt_Crt(RME_BOOT_INIT_CPT,RME_BOOT_INIT_KOM,RME_BOOT_BENCH_PGT,
				       Cur_addr,0x00000000U,RME_PGT_TOP,RME_PGT_SIZE_1M,RME_PGT_NUM_4K);

	USR_DBG_S("\r\ncreate benchmark page table  retval= ");
	USR_DBG_I(Retval);

	// add page to benchmark pgt
	for(Count=0U;Count<0x400U;Count++)
	{
			Retval=RME_Pgt_Add(RME_BOOT_BENCH_PGT,Count,RME_PGT_ALL_DYN,
											 RME_BOOT_INIT_PGT,
											 Count,
											 0);
			    /*USR_DBG_S("\r\nadd page to benchmark pgt  retval= ");
				USR_DBG_I(Retval);*/
	}
			/* Device memory 1, 512MiB 0x40000000 -> 0x40000000 */
	for(Count=0U;Count<0x200U;Count++)
	{
		    	Retval=RME_Pgt_Add(RME_BOOT_BENCH_PGT,(Count+0x400U),
											 RME_PGT_READ|RME_PGT_WRITE,
											 RME_BOOT_INIT_PGT,
											 (Count+0x400U),
											 0);
				/*USR_DBG_S("\r\nadd page to benchmark pgt  retval= ");
							USR_DBG_I(Retval);*/
	}

		    /* Device memory 2, 512MiB 0x60000000 -> 0xE0000000 */
	for(Count=0U;Count<0x200U;Count++)
	{
		    	Retval=RME_Pgt_Add(RME_BOOT_BENCH_PGT,(Count+0x600U),
		    								 RME_PGT_READ|RME_PGT_WRITE,
											 RME_BOOT_INIT_PGT,
											 (Count+0x600U),
											 0);
				/*USR_DBG_S("\r\nadd page to benchmark pgt  retval= ");
										USR_DBG_I(Retval);*/
	}

	/*USR_DBG_S("\r\nFirst section's first entry ");
	USR_DBG_H(RME_A7A_REG(Cur_addr+RME_KOM_VA_BASE));
	USR_DBG_S(" @ ");
	USR_DBG_H(Cur_addr+RME_KOM_VA_BASE);

	USR_DBG_S("\r\nFirst section's 0x080th entry ");
	USR_DBG_H(RME_A7A_REG(Cur_addr+0x080*RME_WORD_BYTE+RME_KOM_VA_BASE));
	USR_DBG_S(" @ ");
	USR_DBG_H(Cur_addr+0x080*RME_WORD_BYTE+RME_KOM_VA_BASE);

	USR_DBG_S("\r\nSecond section's first entry ");
	USR_DBG_H(RME_A7A_REG(Cur_addr+0x400*RME_WORD_BYTE+RME_KOM_VA_BASE));
	USR_DBG_S(" @ ");
	USR_DBG_H(Cur_addr+0x400*RME_WORD_BYTE+RME_KOM_VA_BASE);

	USR_DBG_S("\r\nThird section's first entry ");
	USR_DBG_H(RME_A7A_REG(Cur_addr+0x600*RME_WORD_BYTE+RME_KOM_VA_BASE));
	USR_DBG_S(" @ ");
	USR_DBG_H(Cur_addr+0x600*RME_WORD_BYTE+RME_KOM_VA_BASE);

	/*USR_DBG_S("\r\nKernel pgtbl's 80th entry ");
	USR_DBG_H(RME_A7A_REG(((ptr_t)(&__RME_A7A_Kern_Pgt))+0x080*RME_WORD_BYTE));
	USR_DBG_S(" @ ");
	USR_DBG_H(((ptr_t)(&__RME_A7A_Kern_Pgt))+0x080*RME_WORD_BYTE);*/

	//创建测试进程
	Retval=RME_Prc_Crt(RME_BOOT_INIT_CPT,RME_BOOT_BENCH_PRC,RME_BOOT_INIT_CPT,
		    			RME_BOOT_BENCH_PGT);
	USR_DBG_S("\r\n create benchmark prc  retval= ");
	USR_DBG_I(Retval);


	//创建同步迁移调用
	//0x00009000U
	Retval=RME_Inv_Crt(RME_BOOT_INIT_CPT,
			           RME_BOOT_INIT_KOM,
					   RME_BOOT_BENCH_INV,
					   RME_BOOT_BENCH_PRC,
					   Cur_addr+0x10000);
	USR_DBG_S("\r\n create benchmark thd  retval= ");
	USR_DBG_I(Retval);
	//设置同步迁移调用
	Retval=RME_Inv_Set(RME_BOOT_BENCH_INV,
					    (ptr_t)&RME_Inv_test_fuction,
						(ptr_t)&RME_Stack3[2000],
						0);
	USR_DBG_S("\r\n create benchmark INV  retval= ");
	USR_DBG_I(Retval);
	start=get_time();
	for(int Count=0;Count<10000;Count++)
	{
		//迁移调用
		Retval=RME_Inv_Act(RME_BOOT_BENCH_INV,0,0);
		/*USR_DBG_S("\r\n  RME_Inv_Act  retval= ");
		USR_DBG_I(Retval);*/
	}
	end=get_time();
	USR_DBG_S("\r\n inv 10000 time  = ");
	USR_DBG_I(end-start);
	USR_DBG_S("\r\n inv 10000 time avg = ");
	USR_DBG_I((end-start)/10000);
	while(1);
}

/* Function:RME_Benchmark *****************************************************
Description : The benchmark entry, also the init thread.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RME_Benchmark(void)
{
	Cur_addr=RME_BOOT_BENCH_KOM_FRONTIER;
    //USR_DBG_S("\r\nhello this is User level!");

    //RME_Same_Prc_Thd_Sig_Testinit();
    RME_Diff_Prc_Thd_Sig_Testinit();
    //RME_Diff_Inv_test();
    //RME_Same_Prc_Thd_Switch_Test();
    //RME_Diff_Prc_Switch_Test();
    //USR_DBG_S("\r\nTest done!");
    while(1);
}
/* End Function:RME_Benchmark ************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
