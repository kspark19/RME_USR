#include"benchmark.h"
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
              ptr_t Haddr - The kernel-accessible virtual memory address
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
/* Function:RME_Prc_Crt *******************************************************
Description : Create a process. A process is in fact a protection domain
              associated with a set of capabilities.
Input       : cid_t Cap_Cpt_Crt - The capability to the capability table to
                                      put this process capability in.
                                      2-Level.
              cid_t Cap_Prc - The capability slot that you want this newly
                                  created process capability to be in.
                                  1-Level.
              cid_t Cap_Cpt - The capability to the capability table to use
                                  for this process.
                                  2-Level.
              cid_t Cap_Pgt - The capability to the page table to use for
                                  this process.
                                  2-Level.
              ptr_t Raw_Pgt - Alternate user-supplied page table physical
                                  address.
Output      : None.
Return      : ret_t - If successful, 0; or an error code.
******************************************************************************/
ret_t RME_Prc_Crt(cid_t Cap_Cpt_Crt,
                      cid_t Cap_Prc,
                      cid_t Cap_Cpt,
                      cid_t Cap_Pgt)
{
    return RME_SVC(RME_SVC_PRC_CRT,
                   Cap_Cpt_Crt,
                   Cap_Prc,
                   Cap_Cpt,
                   Cap_Pgt);
}
/* End Function:RME_Prc_Crt **************************************************/

/* Function:RME_Prc_Del *******************************************************
Description : Delete a process.
Input       : cid_t Cap_Cpt - The capability to the capability table.
                                  2-Level.
              cid_t Cap_Prc - The capability to the process.
                                  1-Level.
Output      : None.
Return      : ret_t - If successful, 0; or an error code.
******************************************************************************/
ret_t RME_Prc_Del(cid_t Cap_Cpt,
                      cid_t Cap_Prc)
{
    return RME_SVC(RME_SVC_PRC_DEL,
                   Cap_Cpt,
                   Cap_Prc,
                   0U,
                   0U);
}
/* End Function:RME_Prc_Del **************************************************/

/* Function:RME_Prc_Cpt *******************************************************
Description : Change a process's capability table.
Input       : cid_t Cap_Prc - The capability to the process that have been
                                  created already.
                                  2-Level.
              cid_t Cap_Cpt - The capability to the capability table to use
                                  for this process.
                                  2-Level.
Output      : None.
Return      : ret_t - If successful, 0; or an error code.
******************************************************************************/
ret_t RME_Prc_Cpt(cid_t Cap_Prc,
                      cid_t Cap_Cpt)
{
    return RME_SVC(RME_SVC_PRC_CPT,
                   0U,
                   Cap_Prc,
                   Cap_Cpt,
                   0U);
}
/* End Function:RME_Prc_Cpt **************************************************/

/* Function:RME_Prc_Pgt *******************************************************
Description : Change a process's page table.
Input       : cid_t Cap_Prc - The capability to the process that have been
                                  created already.
                                  2-Level.
              cid_t Cap_Pgt - The capability to the page table to use for
                                  this process.
                                  2-Level.
              ptr_t Raw_Pgt - Alternate user-supplied page table physical
                                  address.
Output      : None.
Return      : ret_t - If successful, 0; or an error code.
******************************************************************************/
ret_t RME_Prc_Pgt(cid_t Cap_Prc,
                      cid_t Cap_Pgt)
{
    return RME_SVC(RME_SVC_PRC_PGT,
                   0U,
                   Cap_Prc,
                   Cap_Pgt,
                   0U);
}
/* End Function:RME_Prc_Pgt **************************************************/
/* Function:RME_Pgt_Crt *******************************************************
Description : Create a page table.
Input       : cid_t Cap_Cpt - The capability table that contains the newly
                                  created cap to pgtbl.
                                  2-Level.
              cid_t Cap_Kom - The kernel memory capability.
                                  2-Level.
              cid_t Cap_Pgt - The capability slot that you want this newly
                                  created page table capability to be in.
                                  1-Level.
              ptr_t Raddr - The relative virtual address to store the page
                                table kernel object.
              ptr_t Base - The virtual address to start mapping for this
                               page table. This address must be aligned to the
                               total size of the table.
              ptr_t Is_Top - Whether this page table is the top-level. If
                                 it is, we will map all the kernel page
                                 directories into this one.
              ptr_t Size_Order - The size order of the page table. The size
                                     refers to the size of each page in the
                                     page directory.
              ptr_t Num_Order - The number order of entries in the table.
Output      : None.
Return      : ret_t - If successful, 0; or an error code.
******************************************************************************/
ret_t RME_Pgt_Crt(cid_t Cap_Cpt,
                      cid_t Cap_Kom,
                      cid_t Cap_Pgt,
                      ptr_t Raddr,
                      ptr_t Base,
                      ptr_t Is_Top,
                      ptr_t Size_Order,
                      ptr_t Num_Order)
{

    return RME_SVC(RME_PGT_SVC(Num_Order,RME_SVC_PGT_CRT),
                   Cap_Cpt,
                   RME_PARAM_D1(Cap_Kom)|RME_PARAM_Q1(Cap_Pgt)|RME_PARAM_Q0(Size_Order),
                   Raddr,
                   Base|Is_Top);
}
/* End Function:RME_Pgt_Crt **************************************************/

/* Function:RME_Pgt_Del *******************************************************
Description : Delete a page table.
Input       : cid_t Cap_Cpt - The capability to the captbl that may contain
                                  the cap to new captbl.
                                  2-Level.
              cid_t Cap_Pgt - The capability slot that you want this newly
                                  created page table capability to be in.
                                  1-Level.
Output      : None.
Return      : ret_t - If successful, 0; or an error code.
******************************************************************************/
ret_t RME_Pgt_Del(cid_t Cap_Cpt,
                      cid_t Cap_Pgt)
{
    return RME_SVC(RME_SVC_PGT_DEL,
                   Cap_Cpt,
                   Cap_Pgt,
                   0U,
                   0U);
}
/* End Function:RME_Pgt_Del **************************************************/

/* Function:RME_Pgt_Add *******************************************************
Description : Delegate a page from one page table to another. This is the only
              way to add pages to new page tables after the system boots.
Input       : cid_t Cap_Pgt_Dst - The capability to the destination page
                                      directory.
                                      2-Level.
              ptr_t Pos_Dst - The position to delegate to in the
                                  destination page directory.
              ptr_t Flag_Dst - The page access permission for the
                                   destination page.
              cid_t Cap_Pgt_Src - The capability to the source page
                                      directory.
                                      2-Level.
              ptr_t Pos_Dst - The position to delegate from in the source
                                  page directory.
              ptr_t Index - The index of the physical address frame to
                                delegate. For example, if the destination
                                directory's page size is 1/4 of that of the
                                source directory, index=0 will delegate the
                                first 1/4, index=1 will delegate the second
                                1/4, index=2 will delegate the third 1/4, and
                                index=3 will delegate the last 1/4.
                                All other index values are illegal.
Output      : None.
Return      : ret_t - If successful, 0; or an error code.
******************************************************************************/
ret_t RME_Pgt_Add(cid_t Cap_Pgt_Dst,
                      ptr_t Pos_Dst,
                      ptr_t Flag_Dst,
                      cid_t Cap_Pgt_Src,
                      ptr_t Pos_Src,
                      ptr_t Index)
{
    return RME_SVC(RME_SVC_PGT_ADD,
                   RME_PARAM_D0(Flag_Dst),
                   RME_PARAM_D1(Cap_Pgt_Dst)|RME_PARAM_D0(Pos_Dst),
                   RME_PARAM_D1(Cap_Pgt_Src)|RME_PARAM_D0(Pos_Src),
                   Index);
}
/* End Function:RME_Pgt_Add **************************************************/

ret_t RME_Inv_Crt(cid_t Cap_Cpt,
                      cid_t Cap_Kom,
                      cid_t Cap_Inv,
                      cid_t Cap_Prc,
                      ptr_t Raddr)
{
    return RME_SVC(RME_SVC_INV_CRT,
                   Cap_Cpt,
                   RME_PARAM_D1(Cap_Kom)|RME_PARAM_D0(Cap_Inv),
                   Cap_Prc,
                   Raddr);
}

ret_t RME_Inv_Set(cid_t Cap_Inv,
                      ptr_t Entry,
                      ptr_t Stack,
                      ptr_t Is_Exc_Ret)
{
    return RME_SVC(RME_SVC_INV_SET,
                   0U,
                   RME_PARAM_D1(Is_Exc_Ret)|RME_PARAM_D0(Cap_Inv),
                   Entry,
                   Stack);
}

ret_t RME_Inv_Act(cid_t Cap_Inv,
				  ptr_t Param,
				  ptr_t* Retval);
/*{
	return RME_SVC(RME_SVC_INV_ACT,
					   0U,
					   (cid_t)Cap_Inv,
					   Param,
	                   0U);
}*/
ret_t RME_Inv_Ret(ptr_t Retval);
/*{
	return RME_SVC(RME_SVC_INV_RET,
						   0U,
						   Retval,
						   0U,
		                   0U);
}*/
