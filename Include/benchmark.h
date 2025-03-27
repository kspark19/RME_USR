
#include"rme.h"
/* Word size */
#define RME_WORD_BIT                                RME_POW2(RME_WORD_ORDER)
#define RME_WORD_BYTE                               (RME_WORD_BIT>>3)
#define RME_WORD_BIT_O1                             (RME_WORD_BYTE)
#define RME_WORD_BIT_O2                             (RME_WORD_BYTE*2U)
#define RME_WORD_BIT_O3                             (RME_WORD_BYTE*3U)
#define RME_WORD_BIT_O4                             (RME_WORD_BYTE*4U)
#define RME_WORD_BIT_O5                             (RME_WORD_BYTE*5U)
#define RME_WORD_BIT_O6                             (RME_WORD_BYTE*6U)
#define RME_WORD_BIT_O7                             (RME_WORD_BYTE*7U)
#define RME_WORD_BIT_Q1                             RME_WORD_BIT_O2
#define RME_WORD_BIT_Q2                             RME_WORD_BIT_O4
#define RME_WORD_BIT_Q3                             RME_WORD_BIT_O6
#define RME_WORD_BIT_D1                             RME_WORD_BIT_Q2
/* Thread creation */
#define RME_THD_SVC(ATTR,IS_HYP,SVC)                (RME_FIELD(ATTR,7U)|RME_FIELD(IS_HYP,6U)|(SVC))
/* UART peripheral address */
#define RME_A7A_UART_CONTROL      					(*((volatile unsigned int*)(0xE0001000)))
#define RME_A7A_UART_MODE         					(*((volatile unsigned int*)(0xE0001004)))
#define RME_A7A_UART_BRGEN        					(*((volatile unsigned int*)(0xE0001018)))
#define RME_A7A_UART_STATUS       					(*((volatile unsigned int*)(0xE000102C)))
#define RME_A7A_UART_FIFO         					(*((volatile unsigned int*)(0xE0001030)))
#define RME_A7A_UART_BRDIV        					(*((volatile unsigned int*)(0xE0001034)))
#define RME_A7A_UART_STATUS_TXE   					(1U<<3)
//#define RME_DBG_S(STR)                              RME_Str_Print((const signed char*)(STR))
/* NULL */
#define RME_NULL 									0

#define RME_A7A_PUTCHAR(CHAR) \
do \
{ \
    while((RME_A7A_UART_STATUS&RME_A7A_UART_STATUS_TXE)==0); \
	RME_A7A_UART_FIFO=(CHAR); \
} \
while(0)

/* Define ********************************************************************/
/* Types */
typedef signed int                          s32;
typedef signed short                        s16;
typedef signed char                         s8;
typedef unsigned int                        u32;
typedef unsigned short                      u16;
typedef unsigned char                       u8;
typedef s32                                 tid_t;
typedef u32                       			ptr_t;
typedef s32                       			cnt_t;
typedef s32 								cid_t;
typedef s32                        			ret_t;

#define BENCHMARK_STACK_SIZE 4096
/* System service stub */
//#define RME_CAP_OP(OP,CAPID,ARG1,ARG2,ARG3) RME_Svc(((OP)<<(sizeof(ptr_t)*4)|(CAPID)),ARG1,ARG2,ARG3)
#define RME_PARAM_D_MASK                    (((ptr_t)(-1))>>(sizeof(ptr_t)*4))
#define RME_PARAM_Q_MASK                    (((ptr_t)(-1))>>(sizeof(ptr_t)*6))
#define RME_PARAM_O_MASK                    (((ptr_t)(-1))>>(sizeof(ptr_t)*7))
/* The parameter passing - not to be confused with kernel macros. These macros just place the parameters */
#define RME_PARAM_D1(X)                     (((X)&RME_PARAM_D_MASK)<<(sizeof(ptr_t)*4))
#define RME_PARAM_D0(X)                     ((X)&RME_PARAM_D_MASK)

#define RME_PARAM_Q3(X)                     (((X)&RME_PARAM_Q_MASK)<<(sizeof(ptr_t)*6))
#define RME_PARAM_Q2(X)                     (((X)&RME_PARAM_Q_MASK)<<(sizeof(ptr_t)*4))
#define RME_PARAM_Q1(X)                     (((X)&RME_PARAM_Q_MASK)<<(sizeof(ptr_t)*2))
#define RME_PARAM_Q0(X)                     ((X)&RME_PARAM_Q_MASK)

#define RME_PARAM_O7(X)                     (((X)&RME_PARAM_O_MASK)<<(sizeof(ptr_t)*7))
#define RME_PARAM_O6(X)                     (((X)&RME_PARAM_O_MASK)<<(sizeof(ptr_t)*6))
#define RME_PARAM_O5(X)                     (((X)&RME_PARAM_O_MASK)<<(sizeof(ptr_t)*5))
#define RME_PARAM_O4(X)                     (((X)&RME_PARAM_O_MASK)<<(sizeof(ptr_t)*4))
#define RME_PARAM_O3(X)                     (((X)&RME_PARAM_O_MASK)<<(sizeof(ptr_t)*3))
#define RME_PARAM_O2(X)                     (((X)&RME_PARAM_O_MASK)<<(sizeof(ptr_t)*2))
#define RME_PARAM_O1(X)                     (((X)&RME_PARAM_O_MASK)<<(sizeof(ptr_t)*1))
#define RME_PARAM_O0(X)                     ((X)&RME_PARAM_O_MASK)


/* Bit mask */
#define RME_MASK_FULL                               (~((ptr_t)0U))
#define RME_MASK_WORD                               (~(RME_MASK_FULL<<RME_WORD_ORDER))
#define RME_MASK_WORD_O                             RME_MASK_END(RME_WORD_BIT_O1-1U)
#define RME_MASK_WORD_Q                             RME_MASK_END(RME_WORD_BIT_Q1-1U)
#define RME_MASK_WORD_D                             RME_MASK_END(RME_WORD_BIT_D1-1U)
/* Init thread infinite time marker */
#define RME_THD_INIT_TIME                           (RME_MASK_FULL>>1)
/* Other thread infinite time marker */
#define RME_THD_INF_TIME                            (RME_THD_INIT_TIME-1U)

/* Initial boot capabilities - This should be in accordnace with the kernel settings */
/* The capability table of the init process */
#define RME_BOOT_INIT_CPT                 		0
/* The top-level page table of the init process - always 4GB full range split into 8 pages */
#define RME_BOOT_INIT_PGT                 		1
/* The init process */
#define RME_BOOT_INIT_PRC                 		2
/* The init thread */
#define RME_BOOT_INIT_THD                 		3
/* The initial kernel function capability */
#define RME_BOOT_INIT_KFN          				4
/* The initial kernel memory capability */
#define RME_BOOT_INIT_KOM         				5
/* The initial timer endpoint */
#define RME_BOOT_INIT_TIMER      				6
/* The test objects */
#define RME_BOOT_BENCH_THD       				8
#define RME_BOOT_BENCH_PGT_TOP   				9
#define RME_BOOT_BENCH_PGT_SRAM  				10
#define RME_BOOT_BENCH_SEN_THD					14
#define RME_BOOT_BENCH_RCV_THD					15
#define RME_BOOT_BENCH_THD_MAIN					16
/* The top-level page table of the init process's SRAM */
#define RME_BOOT_INIT_PGT_SRAM    				11
/* the signal capability */
#define RME_BOOT_SIG_BENCH						13
/* Power of 2 */
#define RME_FIELD(VAL,POW)                      (((ptr_t)(VAL))<<(POW))
#define RME_POW2(POW)                           RME_FIELD(1U,POW)
/* The order of bits in one CPU machine word */
#define RME_WORD_ORDER            				(5U)
/* Word size */
#define RME_WORD_BIT                            RME_POW2(RME_WORD_ORDER)
/* Need to export the memory frontier! */
/* Need to export the flags as well ! */
/* Export the errno too */
#define RME_BOOT_BENCH_KOM_FRONTIER 			0x00009000U
/* The kernel-accessible virtual memory address,for this thread's register sets */
#define RME_BOOT_HYPER_KOM_VADDR                0x00000000U
//0x10005000
/* The stack safe size */
#define RME_STACK_SAFE_SIZE 					16
/* Doesn't want to receive the scheduler signal for the thread */
#define RME_CID_NULL   							0x8000
/* The maximum priority allowed for this thread. Once set, this cannot be changed. */
#define RME_THD_PRIO_MAX                        31
/*The thread ID*/
#define RME_TID_1                               1
#define RME_TID_2                               2
#define RME_TID_3                               3
#define RME_TID_4                               4
#define RME_TID_6                               6

/* End Define ****************************************************************/



/* Private Variables *********************************************************/
/* The stack of the threads - enough for 4 threads */
ptr_t RME_Stack1[2048];
ptr_t RME_Stack2[2048];
ptr_t RME_Stack3[2048];
ptr_t RME_Stack4[2048];
ptr_t RME_Stack5[2048];
u16 Time[10000];
s8 RME_Bench_Buf[1024];
/* End Private Variables *****************************************************/

/* Function Prototypes *******************************************************/
extern ret_t RME_Svc(ptr_t Svc_Capid,ptr_t Param1, ptr_t Param2, ptr_t Param3);
/* System service stub */
#define RME_SVC(SVC,CID,ARG1,ARG2,ARG3)             RME_Svc(RME_FIELD(SVC,RME_WORD_BIT_D1)|((ptr_t)(CID)),((ptr_t)(ARG1)),((ptr_t)(ARG2)),((ptr_t)(ARG3)));
extern cnt_t RME_Sprint_Uint(s8* Buffer,u32 Arg_Int);
extern void RME_Thd_Stub(void);
extern void RME_Inv_Stub(void);
ptr_t _RME_Stack_Init(ptr_t Stack, ptr_t Stub, ptr_t Param1, ptr_t Param2, ptr_t Param3, ptr_t Param4);
void RME_Benchmark(void);
void RME_Same_Prc_Thd_Switch_Test_Thd(ptr_t Param1);
void RME_Same_Prc_Thd_Switch_Test(void);
/* End Function Prototypes ***************************************************/

/* Function:__USR_Putchar *****************************************************
Description : Output a character to console. In Cortex-M, under most circumstances,
              we should use the ITM for such outputs.
Input       : char Char - The character to print.
Output      : None.
Return      : rme_ptr_t - Always 0.
******************************************************************************/
ptr_t __USR_Putchar(char Char)
{
    RME_A7A_PUTCHAR(Char);
    return 0;
}
/* End Function __USR_Putchar ***************************************************/


/* Function:USR_DBG_I *****************************************************
Description : Print a signed integer on the debugging console. This integer is
              printed as decimal with sign.
Input       : rme_cnt_t Int - The integer to print.
Output      : None.
Return      : rme_cnt_t - The length of the string printed.
******************************************************************************/
cnt_t USR_DBG_I(cnt_t Int)
{
    cnt_t Num;
    cnt_t Abs;
    cnt_t Iter;
    cnt_t Count;
    cnt_t Div;

    /* Exit on zero */
    if(Int==0)
    {


        __USR_Putchar('0');
        return 1;
    }
    else
    {

        /* No action required */
    }


    /* Correct all negatives into positives */
    if(Int<0)
    {


        __USR_Putchar('-');
        Abs=-Int;
        Num=1;
    }
    else
    {


        Abs=Int;
        Num=0;
    }

    /* How many digits are there? */
    Count=0;
    Div=1;
    Iter=Abs;
    while(1U)
    {
        Iter/=10;
        Count++;
        if(Iter!=0)
        {


            Div*=10;
        }
        else
        {


            break;
        }
    }
    Num+=Count;

    /* Print the integer */
    Iter=Abs;

    while(Count>0)
    {
        Count--;
        __USR_Putchar((s8)(Iter/Div)+'0');
        Iter=Iter%Div;
        Div/=10;
    }

    return Num;
}
/* End Function:USR_DBG_I ************************************************/

/* Function:USR_DBG_H *****************************************************
Description : Print a unsigned integer on the debugging console. This integer is
              printed as hexadecimal.
Input       : rme_ptr_t Uint - The unsigned integer to print.
Output      : None.
Return      : rme_cnt_t - The length of the string printed.
******************************************************************************/
cnt_t USR_DBG_H(ptr_t Uint)
{
    ptr_t Iter;
    ptr_t Count;
    ptr_t Num;

    /* Exit on zero */
    if(Uint==0U)
    {
        __USR_Putchar('0');
        return 1;
    }
    else
    {
        /* No action required */
    }

    /* Filter out all the zeroes */
    Count=0U;
    Iter=Uint;
    while((Iter>>(RME_WORD_BIT-4U))==0U)
    {
        Iter<<=4;
        Count++;
    }

    /* Count is the number of pts to print */
    Count=RME_POW2(RME_WORD_ORDER-2U)-Count;
    Num=Count;
    while(Count>0U)
    {
        Count--;
        Iter=(Uint>>(Count<<2U))&0x0FU;
        if(Iter<10U)
        {

            __USR_Putchar((s8)Iter+'0');
        }
        else
        {

            __USR_Putchar((s8)Iter+'A'-10);
        }
    }

    return (cnt_t)Num;
}
/* End Function:USR_DBG_H ************************************************/
cnt_t USR_DBG_S(const s8* String)
{
    ptr_t Count;

    for(Count=0U;Count<255;Count++)
    {
        if(String[Count]==(s8)'\0')
        {

            break;
        }
        else
        {
            /* No action required */
        }

        __USR_Putchar(String[Count]);
    }

    return (cnt_t)Count;
}



/* Function:RME_Thd_Crt *******************************************************
Description : Create a thread. A thread is the minimal kernel-level execution
              unit.
Input       : RME_cid_t Cap_Cpt - The capability to the capability table.
                                  2-Level.
              RME_cid_t Cap_Kom - The kernel memory capability.
                                  2-Level.
              RME_cid_t Cap_Thd - The capability slot that you want this newly
                                  created thread capability to be in.
                                  1-Level.
              RME_cid_t Cap_Prc - The capability to the process that it is in.
                                  2-Level.
              RME_ptr_t Prio_Max - The maximum priority allowed for this
                                   thread. Once set, this cannot be changed.
              RME_ptr_t Raddr - The relative virtual address to store the
                                thread kernel object.
              RME_ptr_t Attr - The context attributes.
Output      : None.
Return      : RME_ret_t - If successful, the Thread ID; or an error code.
******************************************************************************/
ret_t RME_Thd_Crt(cid_t Cap_Cpt,
                      cid_t Cap_Kom,
                      cid_t Cap_Thd,
                      cid_t Cap_Prc,
                      ptr_t Prio_Max,
                      ptr_t Raddr,
                      ptr_t Attr)
{
    return RME_SVC(RME_THD_SVC(Attr,0U,RME_SVC_THD_CRT),
                   Cap_Cpt,
                   RME_PARAM_D1(Cap_Kom)|RME_PARAM_D0(Cap_Thd),
                   RME_PARAM_D1(Cap_Prc)|RME_PARAM_D0(Prio_Max),
                   Raddr);
}
/* End Function:RME_Thd_Crt **************************************************/

/* Function:RME_Thd_Exec_Set **************************************************
Description : Set a thread's entry point and stack. The registers will be
              initialized with these contents.
Input       : RME_cid_t Cap_Thd - The capability to the thread.
                                  2-Level.
              void* Entry - The entry address of the thread.
              void* Stack - The stack address to use for execution.
              void* Param - The parameter to pass to the thread.
Output      : None.
Return      : RME_ret_t - If successful, 0; or an error code.
******************************************************************************/
ret_t RME_Thd_Exec_Set(cid_t Cap_Thd,
                           ptr_t Entry,
                           ptr_t Stack,
                           ptr_t Param)
{
    return RME_SVC(RME_SVC_THD_EXEC_SET,
                   Cap_Thd,
                   (ptr_t)Entry,
                   (ptr_t)Stack,
                   (ptr_t)Param);
}
/* End Function:RME_Thd_Exec_Set *********************************************/

/* Function:RME_Thd_Time_Xfer *************************************************
Description : Transfer time from one thread to another. This can only be called
              from the core that the thread is on, and the the two threads
              involved in the time transfer must be on the same core.
              If the time transfered is more than or equal to what the source
              have, the source will be out of time or blocked. If the source is
              both out of time and blocked, we do not send the notification;
              Instead, we send the notification when the receive endpoint
              actually receive something.
              It is possible to transfer time to threads have a lower priority,
              and it is also possible to transfer time to threads that have a
              higher priority. In the latter case, and if the source is
              currently running, a preemption will occur.
Input       : RME_cid_t Cap_Thd_Dst - The destination thread.
                                      2-Level.
              RME_cid_t Cap_Thd_Src - The source thread.
                                      2-Level.
              RME_ptr_t Time - The time to transfer, in slices. A slice is the
                               minimal amount of time transfered in the system
                               usually on the order of 100us or 1ms.
                               Use RME_THD_INIT_TIME for revoking transfer.
                               Use RME_THD_INF_TIME for infinite trasnfer.
Output      : None.
Return      : RME_ret_t - If successful, the destination time amount; or an
                          error code.
******************************************************************************/
ret_t RME_Thd_Time_Xfer(cid_t Cap_Thd_Dst,
                        cid_t Cap_Thd_Src,
                        ptr_t Time)
{
    return RME_SVC(RME_SVC_THD_TIME_XFER,
                   0U,
                   Cap_Thd_Dst,
                   Cap_Thd_Src,
                   Time);
}
/* End Function:RME_Thd_Time_Xfer ********************************************/


/* Function:RME_Thd_Sched_Bind ************************************************
Description : Set a thread's priority level, and its scheduler thread. When
              there are any state changes on this thread, a notification will
              be sent to its scheduler thread. If the state of the thread
              changes for multiple times, then only the most recent state will
              be reflected in the scheduler's receive queue.
              The scheduler and the threads that it schedule must be on the
              same core. When a thread wants to go from one core to another,
              its notification to the scheduler must all be processed, and it
              must have no scheduler notifications in itself.
              This must be called on the same core with the Cap_Thd_Sched, and
              the Cap_Thd itself must be free.
              It is impossible to set a thread's priority beyond its maximum
              priority.
Input       : RME_cid_t Cap_Thd - The capability to the thread.
                                  2-Level.
              RME_cid_t Cap_Thd_Sched - The scheduler thread.
                                        2-Level.
              RME_cid_t Cap_Sig - The signal endpoint for scheduler
                                  notifications. This signal endpoint will be
                                  sent to whenever this thread has a fault, or
                                  timeouts. This is purely optional; if it is
                                  not needed, pass in RME_CID_NULL.
              RME_tid_t TID - The thread ID. This is user-supplied, and the
                              kernel will not check whether there are two
                              threads that have the same TID.
              RME_ptr_t Prio - The priority level, higher is more critical.
              rme_ptr_t Haddr - The kernel-accessible virtual memory address
                                for this thread's register sets, only used by
                                hypervisor-managed threads. For other threads,
                                please pass in NULL instead.

Output      : None.
Return      : RME_ret_t - If successful, 0; or an error code.
******************************************************************************/
ret_t RME_Thd_Sched_Bind(cid_t Cap_Thd,
                         cid_t Cap_Thd_Sched,
                         cid_t Cap_Sig,
                         tid_t TID,
                         ptr_t Prio,
                         ptr_t Haddr)
{
    return RME_SVC(RME_SVC_THD_SCHED_BIND,
                   Cap_Thd,
                   RME_PARAM_D1(Cap_Thd_Sched)|RME_PARAM_D0(Cap_Sig),
                   RME_PARAM_D1(TID)|RME_PARAM_D0(Prio),
                   Haddr);
}
/* End Function:RME_Thd_Sched_Bind *******************************************/


/* Function:RME_Thd_Swt *******************************************************
Description : Switch to another thread. The thread to switch to must have the
              same preemptive priority as this thread, and have time, and not
              blocked.
Input       : RME_cid_t Cap_Thd - The capability to the thread. If this is -1,
                                  the kernel will pickup whatever thread that
                                  has the highest priority and time to run.
                                  2-Level.
              RME_ptr_t Is_Yield - This is a flag to indicate whether this
                                   is a full yield. If it is, the kernel will
                                   discard all the time alloted on this
                                   thread.
Output      : None.
Return      : RME_ret_t - If successful, 0; or an error code.
******************************************************************************/
ret_t RME_Thd_Swt(cid_t Cap_Thd,
                  ptr_t Is_Yield)
{
    return RME_SVC(RME_SVC_THD_SWT,
                   0U,
                   Cap_Thd,
                   Is_Yield,
                   0U);
}
/* End Function:RME_Thd_Swt **************************************************/
/*asynchronous communication function*/
/* Function:RME_Sig_Crt *******************************************************
Description : Create a signal endpoint.
Input       : RME_cid_t Cap_Cpt - The capability to the capability table to use
                                  for this signal.
                                  2-Level.
              RME_cid_t Cap_Sig - The capability slot that you want this newly
                                  created signal capability to be in.
                                  1-Level.
Output      : None.
Return      : RME_ret_t - If successful, 0; or an error code.
******************************************************************************/
ret_t RME_Sig_Crt(cid_t Cap_Cpt,
                  cid_t Cap_Sig)
{
    return RME_SVC(RME_SVC_SIG_CRT,
                   Cap_Cpt,
                   Cap_Sig,
                   0U,
                   0U);
}
/* End Function:RME_Sig_Crt **************************************************/
/* Function:RME_Sig_Del *******************************************************
Description : Delete a signal endpoint.
Input       : RME_cid_t Cap_Cpt - The capability to the capability table to
                                  delete from.
                                  2-Level.
              RME_cid_t Cap_Sig - The capability to the signal.
                                  1-Level.
Output      : None.
Return      : RME_ret_t - If successful, 0; or an error code.
******************************************************************************/
ret_t RME_Sig_Del(cid_t Cap_Cpt,
                  cid_t Cap_Sig)
{
    return RME_SVC(RME_SVC_SIG_DEL,
                   Cap_Cpt,
                   Cap_Sig,
                   0U,
                   0U);
}
/* End Function:RME_Sig_Del **************************************************/
/* Function:RME_Sig_Snd ******************************************************
Description : Try to send to a signal endpoint. This system call can cause
              a potential context switch.
Input       : RME_cid_t Cap_Sig - The capability to the signal.
                                  2-Level.
Output      : None.
Return      : RME_ret_t - If successful, 0, or an error code.
******************************************************************************/
ret_t RME_Sig_Snd(cid_t Cap_Sig)
{
    return RME_SVC(RME_SVC_SIG_SND,
                   0U,
                   Cap_Sig,
                   0U,
                   0U);
}
/* End Function:RME_Sig_Snd **************************************************/
/* Function:RME_Sig_Rcv *******************************************************
Description : Try to receive from a signal endpoint. The rules for signal
              endpoint receive is:
              1.If a receive endpoint have many send endpoints, everyone can
                send to it, and sending to it will increase the count by 1.
              2.If some thread blocks on a receive endpoint, the wakeup is only
                possible from the same core that thread is on.
              3.It is not recommended to let 2 cores operate on the rcv
                endpoint simutaneously.
              This system call can potentially trigger a context switch.
Input       : RME_cid_t Cap_Sig - The capability to the signal.
                                  2-Level.
              RME_ptr_t Option - The receive option.
Output      : None.
Return      : RME_ret_t - If successful, a non-negative number containing the
                          number of signals received; or an error code.
******************************************************************************/
ret_t RME_Sig_Rcv(cid_t Cap_Sig,
                  ptr_t Option)
{
    return RME_SVC(RME_SVC_SIG_RCV,
                   0U,
                   Cap_Sig,
                   Option,
                   0U);
}
