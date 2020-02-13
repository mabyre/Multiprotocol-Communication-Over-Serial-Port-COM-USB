/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/

#ifndef CLSUPPORT__WIN32_H_
#define CLSUPPORT__WIN32_H_

#include "..\..\cltypes.h"
#include "..\..\clstructs.h"

/******************************************************************************/
/* Name :  e_Result (cl_IOResetWin32_cb)(void)                                   */
/* Description : Reset completion received from underlayers                   */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  @param 	*pReader					( In ) Reader where the reset applies		*/
/*  @param	status						( In ) status of the reset				*/
/*  @return e_Result															*/
/*  Out:                                                                      */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                 */
/*  ERROR,                    : Failure on execution                          */
/*  MEM_ERR,                  :  Failure on memory management (failure,       */
/*                                  allocation ....)                          */
/*  TIMEOUT_ERR,              :  Overrun on timing                            */
/******************************************************************************/
e_Result cl_IOResetWin32_cb( clvoid *pReader, e_Result status );

/******************************************************************************/
/* Name :  e_Result cl_IOSendDataDoneWin32_cb( clvoid *pReader, clvoid *pTuple, e_Result status)  */
/* Description : Data completion callback sent by porting layer               */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  @param 	*pReader					( In ) Reader which issued the data		*/
/*  @param	*pTuple						( In ) Tuple send to network			*/
/*  @param	status						( In ) status of the send to network	*/
/*  @return e_Result															*/
/*  Out:                                                                      */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                 */
/*  ERROR,                    : Failure on execution                          */
/*  MEM_ERR,                  :  Failure on memory management (failure,       */
/*                                  allocation ....)                          */
/*  PARAMS_ERR,               :  Inconsistent parameters                      */
/*  TIMEOUT_ERR,              :  Overrun on timing                            */
/******************************************************************************/
e_Result cl_IOSendDataDoneWin32_cb( clvoid *pReader, clvoid *pTuple, e_Result status );

/******************************************************************************/
/* Name :  e_Result cl_IOData2ReadWin32_cb(void *pReader, clvoid *pTuple, e_Result status)  */
/* Description : Data received from the host                                  */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : none                                                                 */
/* ---------------                                                            */
/*  @param 	*pReader					( In ) Reader which issued the data		*/
/*  @param	*pTuple						( In ) Tuple from network			*/
/*  @param	status						( In ) status of the recv from network	*/
/* 	@return value: e_Result                                                      */
/*  OK                        :  Result is OK                                 */
/*  ERROR,                    : Failure on execution                          */
/*  MEM_ERR,                  :  Failure on memory management (failure,       */
/*                                  allocation ....)                          */
/*  PARAMS_ERR,               :  Inconsistent parameters                      */
/*  TIMEOUT_ERR,              :  Overrun on timing                            */
/******************************************************************************/
e_Result cl_IOData2ReadWin32_cb( clvoid *pReader, clvoid *pTuple, e_Result status );

/********************************************************************************/
/* Name :  e_Result cl_IOStateWin32_cb( clvoid *pCtxt, clvoid *ptReader, 		*/
/*			clvoid *ptDevice, e_Result status)  								*/
/* Description : status changed from framework/reader/device				 	*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : none                                                                 	*/
/* ---------------                                                            	*/
/*  @param 	*pReader					( In ) Reader which issued the data		*/
/*  @param	*pTuple						( In ) Tuple from network				*/
/*  @param	status						( In ) status of the recv from network	*/
/* 	@return value: e_Result                                                     */
/*  OK                        :  Result is OK                                 	*/
/*  ERROR,                    : Failure on execution                          	*/
/*  MEM_ERR,                  :  Failure on memory management (failure,       	*/
/*                                  allocation ....)                          	*/
/*  PARAMS_ERR,               :  Inconsistent parameters                      	*/
/*  TIMEOUT_ERR,              :  Overrun on timing                            	*/
/******************************************************************************/
e_Result cl_IOStateWin32_cb( clvoid *pCtxt, clvoid *ptReader, clvoid *ptDevice, e_Result status );

/******************************************************************************/
/* Name :  e_Result cl_DiscoverData2ReadWin32_cb(void *pReader, clvoid *pTuple, e_Result status)  */
/* Description : Data received from the host during Discover 					*/
/*                                                                            	*/
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : none                                                                 */
/* ---------------                                                            */
/*  @param 	*pReader					( In ) Reader which issued the data		*/
/*  @param	*pTuple						( In ) Tuple from network			*/
/*  @param	status						( In ) status of the recv from network	*/
/* 	@return value: e_Result                                                      */
/*  OK                        :  Result is OK                                 */
/*  ERROR,                    : Failure on execution                          */
/*  MEM_ERR,                  :  Failure on memory management (failure,       */
/*                                  allocation ....)                          */
/*  PARAMS_ERR,               :  Inconsistent parameters                      */
/*  TIMEOUT_ERR,              :  Overrun on timing                            */
/******************************************************************************/
e_Result cl_DiscoverData2ReadWin32_cb( clvoid *pReader, clvoid *pTuple, e_Result status );

/******************************************************************************/
/* Name :  e_Result cl_DiscoverSendDataDoneWin32_cb( clvoid *pReader, clvoid *pTuple, e_Result status)  */
/* Description : Data completion callback sent by porting layer during discover	*/
/*                                                                            	*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  @param 	*pReader					( In ) Reader which issued the data		*/
/*  @param	*pTuple						( In ) Tuple send to network			*/
/*  @param	status						( In ) status of the send to network	*/
/*  @return e_Result															*/
/*  Out:                                                                      	*/
/* Return value: e_Result                                                      	*/
/*  OK                        :  Result is OK                                 	*/
/*  ERROR,                    : Failure on execution                          	*/
/*  MEM_ERR,                  :  Failure on memory management (failure,       	*/
/*                                  allocation ....)                          	*/
/*  PARAMS_ERR,               :  Inconsistent parameters                      	*/
/*  TIMEOUT_ERR,              :  Overrun on timing                            	*/
/******************************************************************************/
e_Result cl_DiscoverSendDataDoneWin32_cb( clvoid *pReader, clvoid *pTuple, e_Result status );
/******************************************************************************/
/* Name :  e_Result cl_ClbkDiscover_ReadDone(void *pReader, clvoid *pTuple, 	*/
/*							e_Result status)  									*/
/* Description : Data received from the host during an IP discover process		*/
/*                                                                            	*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : none                                                                 	*/
/* ---------------                                                            	*/
/*  @param 	*pReader					( In ) Reader which issued the data		*/
/*  @param	*pTuple						( In ) Tuple from network				*/
/*  @param	status						( In ) status of the recv from network	*/
/* 	@return value: e_Result                                                     */
/*  OK                        :  Result is OK                                 	*/
/*  ERROR,                    : Failure on execution                          	*/
/*  MEM_ERR,                  :  Failure on memory management (failure,       	*/
/*                                  allocation ....)                          	*/
/*  PARAMS_ERR,               :  Inconsistent parameters                      	*/
/*  TIMEOUT_ERR,              :  Overrun on timing                            	*/
/*******************************************************************************/
  e_Result cl_ClbkDiscover_ReadDone( clvoid *ptReader, clvoid *ptTuple, e_Result eStatus );
/******************************************************************************/
/* Name :  e_Result (cl_SemaphoreCreateWin32)( clvoid **ppu32SemId );                  */
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
e_Result cl_SemaphoreCreateWin32( clvoid **ppSemaphore );

/******************************************************************************/
/* Name :  e_Result (cl_SemaphoreWaitWin32)( clvoid *pSem, clu32 u32ms );   */
/* Description : wait for a semaphore until timeout to lock it                */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pSem : opaque pointer on semaphore                     */
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
e_Result cl_SemaphoreWaitWin32( clvoid *pSem, clu32 u32ms);

/******************************************************************************/
/* Name :  e_Result (cl_SemaphoreReleaseWin32)( clu32 u32SemaphoreId );             */
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
e_Result cl_SemaphoreReleaseWin32( clvoid *pSem);


/******************************************************************************/
/* Name :  e_Result (cl_SemaphoreDestroyWin32)( clvoid *pSem );             */
/* Description : destroy a semaphore                                          */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pSem : opaque pointer on Semaphore							  */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,         */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/******************************************************************************/
e_Result cl_SemaphoreDestroyWin32( clvoid *pSem );

/*--------------------------------------------------------------------------*/

e_Result cl_MutexDestroyWin32( clvoid **aAdrMutex );
e_Result cl_MutexCreateWin32( clvoid **aAdrMutex );

/*--------------------------------------------------------------------------*/

/******************************************************************************/
/* Name :  e_Result (cl_WaitMsWin32)( clu32 u32ms );                                */
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
  e_Result cl_WaitMsWin32( clu32 u32ms );

/******************************************************************************/
/*!!!!FD Name :      e_Result (cl_AllocMemWin32)( clvoid **pptBuff, clu32 ulLen); 	 */
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
e_Result cl_AllocMemWin32( clvoid **pptBuff, clu32 ulLen);


/******************************************************************************/
/* Name :      e_Result (cl_FreeMemWin32)( t_Buffer *ptBuff);                       */
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
e_Result cl_FreeMemWin32( clvoid *ptBuff );

/*--------------------------------------------------------------------------*/

e_Result cl_FreeMemSafelyWin32( clvoid **ptBuff );

/******************************************************************************/
/* Name :      e_Result (cl_CreateThreadWin32)( clvoid *pnFnThread,				*/
/*												clu32 ulStackSize				*/
/*												clvoid *pParams, 				*/
/*												clvoid *p_tThreadId);           */
/* Description : create a thread for the library. Typically, there is 3 threads */
/*    - 1. one for reader registration and management                         	*/
/*    - 2. one for crypto                                                    	*/
/*    - 3. 2 per readers: one for writing/one for reading						*/
/*                                                                            	*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : clvoid *pnFnThread : function pointer holding the code of the thread 	*/
/*		clu32 ulStackSize :  size in bytes allocated for the thread			  	*/
/*		clvoid *pParams		: parameters passed to the created thread			*/
/* ---------------                                                            	*/
/*  Out: 	                                                                 	*/
/*		clvoid **pp_tThreadId	: pointer on a pthread_t struct holding the thread id */
/*                  if NULL, no thread was created                            	*/
/* Return value: e_Result                                                      	*/
/*  OK                        :  Result is OK                                  	*/
/*  ERROR,                    : Failure on execution                           	*/
/*  MEM_ERR,                  :  Failure on memory management (failure,        	*/
/*                                  allocation ....)                           	*/
/*  PARAMS_ERR,               :  Inconsistent parameters                       	*/
/*  TIMEOUT_ERR,              :  Overrun on timing                             	*/
/******************************************************************************/
e_Result cl_CreateThreadWin32( clvoid *pnFnThreadFunc, clu32 ulStackSize, clvoid *pParams, clvoid **pp_tThreadId);
/******************************************************************************/
/* Name :      e_Result (*fnDestroyThread)( clvoid *pThreadId );               */
/* Description : destroy thread for the library identified by its id          */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pThreadId: Thread id to destroy                              */
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
e_Result (cl_DestroyThreadWin32)( clvoid *pThreadId );

/******************************************************************************/
/* Name :      e_Result (*cl_SetIPRangeWin32)( t_ReaderRange *p_Range);       */
/* Description : set the IP Range where to find the readers			          */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In :  t_ReaderRange *p_Range: Range of IP address to scan           		*/
/* ---------------                                                            	*/
/*  Out: none                                                                 	*/
/* Return value: e_Result                                                      	*/
/*  OK                        :  Result is OK                                  	*/
/*  ERROR,                    : Failure on execution                           	*/
/*  MEM_ERR,                  :  Failure on memory management (failure,        	*/
/*                                  allocation ....)                           	*/
/*  PARAMS_ERR,               :  Inconsistent parameters                       	*/
/*  TIMEOUT_ERR,              :  Overrun on timing                             	*/
/********************************************************************************/
e_Result (cl_SetIPRangeWin32) ( t_ReaderRange *p_Range );
/******************************************************************************/
/** @fn e_Result cl_GetTimeWin32( cl8 *pTime, clu32 ulLen )
* @brief returns formatted string with time from epoch
*
******************************************************************************
* @param 	**ppTime			( In )	80bytes long buffer to contain formatted string
* @param	ulLen				( In ) length of allocated buffer to receive the time information
* @return e_Result
* \n OK                        :  Result is OK
* \n  ERROR,                    : Failure on execution
* \n  MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n  PARAMS_ERR,               :  Inconsistent parameters
***************************************************************************** */
e_Result cl_GetTimeWin32( cl8 *pTime, clu32 ulLen );

/*****************************************************************************/
/** @fn :  e_Result (*fnDestroyThreadAsked)( clu8 *pu8CancelRequest );
* 	@brief : Signal that a thread destroy was requested
*
*
* @param pu8CancelRequest 		: ( Out ) indicates a destroy was requested
*
* @return e_Result
* \n OK                        :  Result is OK
* ************************************************************************** */
e_Result cl_DestroyThreadRequestedWin32( clu8 *pu8CancelRequest );

/*****************************************************************************/
/** @fn :  e_Result cl_OTAProgress_cb( clu32 eState, clvoid *pReader, clu32 u32Progress );
* 	@brief : OTA service update from underlayer
*
*
*  @param	eState						( In ) Command sent to lower layer
*  @param 	*pReader					( In ) Reader which issued the data
*  @param	u32Progress					( In ) progress on 100% per 5% step for display in the OTA process
*  @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result cl_OTAProgress_cb ( clu32 eState, clvoid *pReader, clu32 u32Progress );

/*****************************************************************************/
/** @fn :  e_Result cl_OTASendDataDone_cb( clvoid *pReader, clvoid *pTuple, e_Result status );
* 	@brief : OTA send data to network notification
*
*
*  @param 	*pReader					( In ) Reader which issued the data
*  @param	*pTuple						( In ) Tuple send to network
*  @param	status						( In ) status of the send to network
*  @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result cl_OTASendDataDone_cb( clvoid *pReader, clvoid *pTuple, e_Result status );
/*****************************************************************************/
/** @fn :  (*fnOTAData2Read_cb) ( clvoid *ppReader, clvoid *ppTuple, e_Result eStatus );
* 	@brief : OTA data has been received from underlayer
*
*
*  @param 	*pReader					( In ) Reader which issued the data
*  @param	*pTuple						( In ) Tuple send to network
*  @param	status						( In ) status of the send to network
*  @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result cl_OTAData2Read_cb( clvoid *ppReader, clvoid *ppTuple, e_Result eStatus );
/******************************************************************************/
/* Name :      e_Result cl_GetElapsedTimeWin32(  )           					*/
/* Description : returns time elapsed between each call, 0 if ERROR				*/
/*                                                                            	*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In :  none																	*/
/* ---------------                                                            	*/
/*  Out: none                                                                 	*/
/* Return value: time in millisecond                                          	*/
/******************************************************************************/
clu64 cl_GetElapsedTimeWin32( );

/*--------------------------------------------------------------------------*/

cl64 cl_GetElapsedTimeHighPerformanceWin32();

void cl_Trace( char *aFormatString, ... );

void cl_StackTrace( void );

#endif /* CLSUPPORT_H_ */
/**************************************************/
