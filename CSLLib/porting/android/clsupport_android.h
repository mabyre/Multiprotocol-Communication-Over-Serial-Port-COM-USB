/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/

#ifndef CLSUPPORT_ANDROID_H_
#define CLSUPPORT_ANDROID_H_

#include "..\..\cltypes.h"
/******************************************************************************/
/* Name :  e_Result (cl_ResetAndroid_cb)(void)                                        */
/* Description : Reset completion received from underlayers                   */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : none                                                                 */
/* ---------------                                                            */
/*  Out:                                                                      */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                 */
/*  ERROR,                    : Failure on execution                          */
/*  MEM_ERR,                  :  Failure on memory management (failure,       */
/*                                  allocation ....)                          */
/*  TIMEOUT_ERR,              :  Overrun on timing                            */
/******************************************************************************/
e_Result cl_ResetAndroid_cb( clvoid );


/******************************************************************************/
/* Name :  e_Result cl_NetwSendDataDoneAndroid_cb(void)                        */
/* Description : Data completion callback sent by porting layer               */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : none                                                                 */
/* ---------------                                                            */
/*  Out:                                                                      */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                 */
/*  ERROR,                    : Failure on execution                          */
/*  MEM_ERR,                  :  Failure on memory management (failure,       */
/*                                  allocation ....)                          */
/*  PARAMS_ERR,               :  Inconsistent parameters                      */
/*  TIMEOUT_ERR,              :  Overrun on timing                            */
/******************************************************************************/
e_Result cl_NetwSendDataDoneAndroid_cb(void);


/******************************************************************************/
/* Name :  e_Result cl_NetwData2ReadAndroid_cb(t_Reader **ppReader, t_Tuple **ppTuple)  */
/* Description : Data received from the host                                  */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : none                                                                 */
/* ---------------                                                            */
/*  Out: t_Tuple **ppTuple: pointer on buffers' list containing data from a   */
/*                          reader                                            */
/*               if NULL, error                                               */
/*      t_Reader **ppReader: pointer on Reader where the buffers are coming   */
/*                            from                                            */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                 */
/*  ERROR,                    : Failure on execution                          */
/*  MEM_ERR,                  :  Failure on memory management (failure,       */
/*                                  allocation ....)                          */
/*  PARAMS_ERR,               :  Inconsistent parameters                      */
/*  TIMEOUT_ERR,              :  Overrun on timing                            */
/******************************************************************************/
e_Result cl_NetwData2ReadAndroid_cb( t_Reader **ppReader, t_Tuple **ppTuple );

/******************************************************************************/
/* Name :  e_Result (cl_SemaphoreCreateAndroid)( clu32 **pu32SemId );                  */
/* Description : create a semaphore (can be locked to 1 only)                 */
/*                protection of resources that could be accessed concurrently */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clu32 *pu32SemId : semaphore id.                                     */
/*          Shall point to NULL when parameter                                */
/*          is passed. Returned different from NULL on OK                     */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,        */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/******************************************************************************/
e_Result cl_SemaphoreCreateAndroid( clu32 **ppu32SemId );

/******************************************************************************/
/* Name :  e_Result (cl_SemaphoreWaitAndroid)( clu32 u32SemaphoreId, clu32 u32ms );   */
/* Description : wait for a semaphore until timeout to lock it                */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clu32 u32SemIdx : index of the semaphore to lock                     */
/*       clu32 u32ms: if 0, don't wait. If semaphore is free, lock it. If not */
/*                    exit on OK                                              */
/*                    if 0xFFFFFFFF, wait indefinitely                        */
/*                    others values: wait up to u32ms. If not locked after    */
/*                    this period, exit on TIMEOUT_ERROR                      */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,        */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/*  TIMEOUT_ERR,              :  Overrun on timing                             */
/******************************************************************************/
e_Result cl_SemaphoreWaitAndroid( clu32 u32SemIdx, clu32 u32ms);

/******************************************************************************/
/* Name :  e_Result (cl_SemaphoreReleaseAndroid)( clu32 u32SemaphoreId );             */
/* Description : release a semaphore already locked                           */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clu32 u32SemIdx : index of the semaphore to unlock                   */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*                          * OK                                              */
/*                          * ERROR: return if semaphore was not existing     */
/******************************************************************************/
e_Result cl_SemaphoreReleaseAndroid( clu32 u32SemIdx);


/******************************************************************************/
/* Name :  e_Result (cl_SemaphoreDestroyAndroid)( clu32 u32SemaphoreId );             */
/* Description : destroy a semaphore                                          */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clu32 u32SemIdx : index of the semaphore to unlock                   */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,         */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/******************************************************************************/
e_Result cl_SemaphoreDestroyAndroid( clu32 u32SemIdx);

/******************************************************************************/
/* Name :  e_Result (cl_WaitMsAndroid)( clu32 u32ms );                                */
/* Description : wait for u32ms before continuing                             */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In :clu32 u32ms : number of ms to wait                                    */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,        */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/*  TIMEOUT_ERR,              :  Overrun on timing                             */
/* TRANSFER_IN_PROCESS_ERR   :  A non blocking call is in progress. This       */
/*                              transfer is rejected. The user application     */
/*                              shall recall the API to ensure that this       */
/*                              request is proceeded                           */
/******************************************************************************/
  e_Result cl_WaitMsAndroid( clu32 u32ms );

/******************************************************************************/
/*!!!!FD Name :      e_Result (cl_AllocMemAndroid)( t_Buffer **pptBuff, clu32 ulLen); 	 */
/* Description : allocate memory and returns allocated pointed pointer to data*/
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In :																	  */
/*		clu32 ulLen: length of the buffer in bytes to be allocated by CSL	  */
/* ---------------                                                            */
/*  In/Out :t_Buffer **pptBuff : pointer of a data buffer allocated in the    */
/*        hosting system. This data is released using fnFreeMem               */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,        */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/*  TIMEOUT_ERR,              :  Overrun on timing                             */
/******************************************************************************/
e_Result cl_AllocMemAndroid( t_Buffer **pptBuff, clu32 ulLen);


/******************************************************************************/
/* Name :      e_Result (cl_FreeMemAndroid)( t_Buffer *ptBuff);                       */
/* Description : free memory pointed by ptBuff                                */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In :t_Buffer *ptBuff : pointer of a data buffer to free                   */
/*            if NULL or non-existing pointer to Buffer, return PARAMS_ERROR  */
/*            if allocation is failing, return MEM_ERROR                      */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,        */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/*  TIMEOUT_ERR,              :  Overrun on timing                             */
/******************************************************************************/
e_Result cl_FreeMemAndroid( t_Buffer *ptBuff );

/******************************************************************************/
/* !!! FD Name :      e_Result (cl_CreateThreadAndroid)( clvoid *pnFnThread,	*/
/*												clu32 ulStackSize				*/
/* 										clu32 *pu32ThreadId);               	*/
/* Description : create a thread for the library. Typically, there is 3 threads */
/*    - 1. one for reader registration and management                         */
/*    - 2. one for reading                                                    */
/*    - 3. one for writing														*/
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pnFnThread : function pointer holding the code of the thread */
/*		clu32 ulStackSize :  size in bytes allocated for the thread			  */
/* ---------------                                                            */
/*  Out: 	                                                                 */
/* 		clu32 *pu32ThreadId : Thread if returned by hosting OS               */
/*                  if NULL, no thread was created                            */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,        */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/*  TIMEOUT_ERR,              :  Overrun on timing                             */
/******************************************************************************/
e_Result (cl_CreateThreadAndroid)( clvoid *pnFnThread, clu32 ulStackSize, clvoid *pParams, clu32 *pu32ThreadId);
/******************************************************************************/
/* Name :      e_Result (*fnDestroyThread)( clvoid *pThreadId);               */
/* Description : destroy thread for the library identified by its id          */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pThreadId: Opaque pointer to thread id to destroy              */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,        */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/*  TIMEOUT_ERR,              :  Overrun on timing                             */
/******************************************************************************/
e_Result (cl_DestroyThreadAndroid)( clvoid *pThreadId );

/******************************************************************************/
/* Name :  e_Result (cl_NetwOpenConnectionWin32)( t_Reader *pReader );  		*/
/* Description : open a connection to a reader    								*/
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader *pReader : pointer to there reader to connect to with ip    */
/*                           address and port			                      */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                     */
/*                          * OK   : connection ok                            */
/*                          * PARAMS_ERR : Inconsistent parameters            */
/*                          * TIMEOUT_ERR : no answer from the reader at this address */
/*							i.e : no reader at all							*/
/******************************************************************************/
e_Result (cl_NetwOpenConnectionAndroid)( t_Reader *pReader );

/******************************************************************************/
/* Name :  e_Result (cl_NetwCloseConnectionWin32)( t_Reader *pReader );  		*/
/* Description : close a connection to a reader    								*/
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader *pReader : pointer to there reader to close connection    */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                     */
/*                          * OK   : connection ok                            */
/*                          * PARAMS_ERR : Inconsistent parameters            */
/******************************************************************************/
e_Result (cl_NetwCloseConnectionAndroid)( t_Reader *pReader );

/******************************************************************************/
/* Name :      e_Result cl_GetTimeAndroid( cl8 *pTime, clu8 ulLen )           */
/* Description : returns formatted string with time from epoch		 		  */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In :  cl8 **ppTime:	80bytes long buffer to contain formatted string	  	  */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,        */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/******************************************************************************/
e_Result cl_GetTimeAndroid( cl8 *pTime, clu8 ulLen );

#endif /* CLSUPPORT_H_ */
/**************************************************/
