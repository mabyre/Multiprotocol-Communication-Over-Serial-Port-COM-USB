/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: clhaldefs.h                                                        */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/*  Description: portable function pointer definition                        */
/*****************************************************************************/

#ifndef CLHALDEFS_H_
#define CLHALDEFS_H_

/*****************************************************************************/
/*                              PORTING API                                  */
/*****************************************************************************/


/*****************************************************************************/
/** @fn :  e_Result (*fnReset_cb)(void *pReader, e_Result status )
*
* @brief : Reset completion received from underlayers
*  @param 	*pReader					( In ) Reader where the reset applies
*  @param	status						( In ) status of the reset
*
* @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result (*fnReset_cb)( clvoid *pReader, e_Result status );


/*****************************************************************************/
/** @fn :  e_Result (*fnIOSendDataDone_cb)(void *pReader, clvoid *pTuple, e_Result status)
*
*   @brief : Data completion callback sent by porting layer
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
e_Result (*fnIOSendDataDone_cb)( clvoid *pReader, clvoid *pTuple, e_Result status );


/*****************************************************************************/
/** @fn  e_Result (*fnIOData2Read_cb)(t_Reader *pReader, t_Tuple *pTuple)
* @brief : Data received from the host
*
*  @param 	*pReader					( In ) Reader which issued the data
*  @param	*pTuple						( In ) Tuple from network
*  @param	status						( In ) status of the recv from network
*
* @return e_Result
*
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result (*fnIOData2Read_cb)( clvoid *pReader, clvoid *pTuple, e_Result status );

/*****************************************************************************/
/** @fn  e_Result (*fnIOState_cb)( clvoid *pCtxt, clvoid *pReader, clvoid *pDevice)
* @brief : status update from the framework, reader, or device.
* \n if one of the parameter is not CL_NULL, it identifies the caller
*
*  @param 	*pCtxt						( In ) framework
*  @param 	*pReader					( In ) Reader which issued the data
*  @param	*pTuple						( In ) Tuple from network
*  @param	status						( In ) status of the recv from network
*
* @return e_Result
*
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result (*fnIOState_cb) ( clvoid *pCtxt, clvoid *pReader, clvoid *pDevice, e_Result eStatus );

/*****************************************************************************/
/** @fn :  e_Result (*fnSemaphoreCreate)( clvoid **pSem );
* 	@brief : create a semaphore (can be locked to 1 only)
* \n               protection of resources that could be accessed concurrently
*
* @param *pSem 					( In ) semaphore id.
*  \n        					Shall point to NULL when parameter
*  \n        					is passed. Returned different from NULL on OK
*
* @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* ************************************************************************** */
e_Result (*fnSemaphoreCreate)( clvoid **ppSem );

/****************************************************************************/
/** @fn :  e_Result (*fnSemaphoreWait)( clvoid *pSem, clu32 u32ms );
* 	@brief : wait for a semaphore until timeout to lock it
*
*
* @param u32SemIdx 				( In ) index of the semaphore to lock
* @param u32ms					( In ) if 0, don't wait. If semaphore is free, lock it. If not
* \n                   			exit on OK
* \n                   			if 0xFFFFFFFF, wait indefinitely
* \n                   			others values: wait up to u32ms. If not locked after
* \n                   			this period, exit on TIMEOUT_ERROR
*
* @result e_Result
* \n  OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result (*fnSemaphoreWait)( clvoid *pSem, clu32 u32ms);

/*****************************************************************************/
/** @fn :  e_Result (*fnSemaphoreRelease)( clu32 u32SemaphoreId );
* 	@brief : release a semaphore already locked
*
* 	@param						( In ) pointer to Semaphore to free
*
* @return e_Result
* \n                          * OK
* \n                          * ERROR: return if semaphore was not existing
* ************************************************************************** */
  e_Result (*fnSemaphoreRelease)( clvoid *pSem);


/*--------------------------------------------------------------------------*/

e_Result (*fnMutexCreate)( clvoid **aAdrMutex );
e_Result (*fnMutexDestroy)( clvoid **aAdrMutex );

/*--------------------------------------------------------------------------*/

/*****************************************************************************/
/** @fn :  e_Result (*fnSemaphoreDestroy)( clu32 u32SemaphoreId );
* 	@brief : destroy a semaphore
*
*
* @param u32SemIdx 				: ( In ) index of the semaphore to unlock
*
* @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* ************************************************************************** */
e_Result (*fnSemaphoreDestroy)( clvoid *pSem);

/*****************************************************************************/
/** @fn :  e_Result (*fnDestroyThreadAsked)( clu8 *pu8CancelRequest );
* 	@brief : Signal that a semaphore destroy was requested
*
*
* @param pu8CancelRequest 		: ( Out ) indicates a destroy was requested
*
* @return e_Result
* \n OK                        :  Result is OK
* ************************************************************************** */
e_Result (*fnDestroyThreadAsked)( clu8 *pu8CancelRequest );


/*****************************************************************************/
/** @fn :  e_Result (*fnWaitMs)( clu32 u32ms );
* @brief : wait for u32ms before continuing
*
* @param u32ms 					( In ) number of ms to wait
*
* @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* \n TIMEOUT_ERR,              :  Overrun on timing
* \n TRANSFER_IN_PROCESS_ERR   :  A non blocking call is in progress. This
* \n                             transfer is rejected. The user application
* \n                             shall recall the API to ensure that this
* \n                             request is proceeded
* ************************************************************************** */
  e_Result (*fnWaitMs)( clu32 u32ms );


/*****************************************************************************/
/**  @fn   e_Result (*fnAllocMem)( t_Buffer **pptBuff, clu32 ulLen);
*
* 	@brief allocate memory and returns allocated pointed pointer to data
*
* @param ulLen					( In ) length of the buffer in bytes to be allocated by CSL
*
* @param **pptBuff 				( In / Out )pointer of a data buffer allocated in the
*  \n      						hosting system. This data is released using fnFreeMem
* @return e_Result
*  \n OK                        :  Result is OK
*  \n ERROR,                    : Failure on execution
*  \n MEM_ERR,                  :  Failure on memory management (failure,
*  \n                                 allocation ....)
*  \n PARAMS_ERR,               :  Inconsistent parameters
*  \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result (*fnAllocMem)( clvoid **pptBuff, clu32 ulLen);

/*****************************************************************************/
/** @fn      e_Result (*fnFreeMem)( t_Buffer *ptBuff);
* 	@brief free memory pointed by ptBuff
*
*
* @param	*ptBuff 			( In ) pointer of a data buffer to free
*  \n           if NULL or non-existing pointer to Buffer, return PARAMS_ERROR
*  \n           if allocation is failing, return MEM_ERROR
*
* @ return Return value: e_Result
*  \n OK                        :  Result is OK
*  \n ERROR,                    : Failure on execution
*  \n MEM_ERR,                  :  Failure on memory management (failure,
*  \n                                 allocation ....)
*  \n PARAMS_ERR,               :  Inconsistent parameters
*  \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result (*fnFreeMem)( clvoid *ptBuff );

/*--------------------------------------------------------------------------*/

e_Result (*fnFreeMemSafely)( clvoid **ptBuff );

/*****************************************************************************/
/** @fn     e_Result (*fnCreateThread)( clvoid *pnFnThread,
*	\n 											clu32 ulStackSize
*	\n 											clvoid *pParams
 * 	\n 									clvoid **pp_tThreadId);
* @brief : create a thread for the library. Typically, there is 3 threads
*    \n - 1. one for reader registration and management
*    \n - 2. one for reading
*    \n - 3. one for writing
* \n
* \n **************************************************************************
*
* @param  *pnFnThread 			( In ) function pointer holding the code of the thread
* @param   ulStackSize 			( In )  size in bytes allocated for the thread
* @param   *pParams 			( In ) parameter passed to the function called when opening
* 		\n 								the thread
* @param   **pp_tThreadId 		( In ) pointer on the pointer of the Thread Id struct
*	\n 							(allocated by the fnCreateThread allocation using mem_alloc)
*
*  @param  **pp_tThreadId 		( Out )Thread if returned by hosting OS
* \n                  			if NULL, no thread was created
* @return e_Result
*  \n OK                        :  Result is OK
*  \n ERROR,                    : Failure on execution
*  \n MEM_ERR,                  :  Failure on memory management (failure,
*  \n                                 allocation ....)
*  \n PARAMS_ERR,               :  Inconsistent parameters
*  \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result (*fnCreateThread)( clvoid *pnFnThreadFunc, clu32 ulStackSize, clvoid *pParams, clvoid **pp_tThreadId);

/*****************************************************************************/
/** @fn :      e_Result (*fnDestroyThread)( clvoid *p_tThreadId );
*  @brief : destroy thread for the library identified by its id
*
* @params *pThreadId		( In ) opaque pointer to structure holding thread id to destroy
*
* @return e_Result
*  \n OK                        :  Result is OK
*  \n ERROR,                    : Failure on execution
*  \n MEM_ERR,                  :  Failure on memory management (failure,
*  \n                                 allocation ....)
*  \n PARAMS_ERR,               :  Inconsistent parameters
*  \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result (*fnDestroyThread)( clvoid *p_tThreadId );


/*****************************************************************************/
/** @fn      e_Result (*fnSetIPRange)( t_ReaderRange *pRange );
* @brief Application set the IP range and TCP port where to find the data
*
* **************************************************************************
* @param *pRange 			( In)  list of IP range and TCP port to scan and find the readers
*
* @return e_Result
*  \n OK                        :  Result is OK
*  \n ERROR,                    : Failure on execution
*  \n MEM_ERR,                  :  Failure on memory management (failure,
*  \n                                 allocation ....)
*  \n PARAMS_ERR,               :  Inconsistent parameters
* ************************************************************************** */
e_Result (*fnSetIPRange)( t_ReaderRange *pRange );

/*****************************************************************************/
/** @fn 		(*fnFileOpen)(cl8 *pFileName, e_FileOptions eOpts, clvoid **ppFileId)
* @brief  	open a file in hosting environment: file shall in Read/Write mode
*
*
* @param 	*pFileName				( In ) string containing the name of the file
* @param	eOpts					( In ) options on opening the file in binary or text mode
*
* @param 	**ppFileId 				( Out ) pointer on file handler
* @return  	e_Result
*  \n OK                        :  Result is OK
*  \n ERROR,                    : Failure on execution
*  \n PARAMS_ERR,               :  Inconsistent parameters
* ************************************************************************** */
e_Result (*fnFileOpen)(cl8 *pFileName, e_FileOptions eOpts, clvoid **ppFileId);

/*****************************************************************************/
/** @fn 	e_Result (*fnFileReadLn)( clvoid *pFile, clu8 **ppu8Data, clu32 *pu32Len)
* 	@brief 	read byte buffer from File. The read is stopped on LF
*	\n		allocation of buffer is performed by this API. CSL frees it automatically
* **************************************************************************
* @param 	*pFile 				( In ) File to read
*
* @param	**ppu8Data 			( Out )Pointer where to store data ( memory is allocated
* \n							by the caller)
* @param	*pu32Len			( Out ) read bytes before reaching end of the line ( reaching LF )
* @return e_Result
*  \n OK                        :  Result is OK
*  \n ERROR,                    : Failure on execution
*  \n MEM_ERR,                  :  Failure on memory management (failure,
*  \n                                 allocation ....)
*  \n PARAMS_ERR,               :  Inconsistent parameters
* ************************************************************************** */
e_Result (*fnFileReadLn)( clvoid *pFile, clu8 **ppu8Data, clu32 *pu32Len);

/*****************************************************************************/
/** @fn  	e_Result (*fnFileWrite)( cl8 *pFile, clu8 *pu8Data, clu32 u32Len )
* 	@brief  set a Clover devices list registered to one reader
* @param *pFile 				( In ) File to write
* @param *ptDevicesList 		( In ) List of Clover devices linked to Reader
*  \n                                 passed in arguments
*
* @return e_Result
*  \n OK                        :  Result is OK
*  \n ERROR,                    : Failure on execution
*  \n MEM_ERR,                  :  Failure on memory management (failure,
*  \n                                 allocation ....)
*  \n PARAMS_ERR,               :  Inconsistent parameters
* \n ************************************************************************** */
e_Result (*fnFileWrite)( cl8 *pFile, clu8 *pu8Data, clu32 u32Len );

/*****************************************************************************/
/** @fn e_Result (*fnFileClose)( cl8 *pFile)
*
* @brief set a Clover devices list registered to one reader
*
* @param *pFile 				( In ) File to close
*
* @return e_Result
* \n  OK                        :  Result is OK
* \n  ERROR,                    : Failure on execution
* ************************************************************************** */
e_Result (*fnFileClose)( cl8 *pFile);


/*****************************************************************************/
/** @fn e_Result (*fnFileSetPos)( cl8 *pFile, clu32 u32Pos)
*
* @brief set the file position to the given offset
*
* @param *pFile					( In ) File
* @param u32Pos					( In ) Position
* @return e_Result
* \n  OK                        :  Result is OK
* \n  ERROR,                    : Failure on execution
* ************************************************************************** */
e_Result (*fnFileSetPos)( cl8 *pFile, clu32 u32Pos );

/*****************************************************************************/
/** @fn e_Result (*fnFileGetPos)( cl8 *pFile, clu32 *pu32Pos)
*
* @brief get the file position
*
* @param *pFile					( In ) File
* @param *pu32Pos				( Out ) Current pointer position
* @return e_Result
* \n  OK                        :  Result is OK
* \n  ERROR,                    : Failure on execution
* ************************************************************************** */
e_Result (*fnFileGetPos)( cl8 *pFile, clu32 *pu32Pos );

/*****************************************************************************/
/** @fn e_Result (*fnIPStackSupportLoad)( clvoid)
*
* @brief load IP stack support on target OS
*
* @param *pFile					( In ) File
* @param *pu32Pos				( Out ) Current pointer position
* @return e_Result
* \n  OK                        :  Result is OK
* \n  ERROR,                    : Failure on execution
* ************************************************************************** */
e_Result (*fnIPStackSupportLoad)( clvoid );

/*****************************************************************************/
/** @fn e_Result (*fnIPStackSupportUnload)( clvoid)
*
* @brief unload IP stack support on target OS
*
* @param *pFile					( In ) File
* @param *pu32Pos				( Out ) Current pointer position
* @return e_Result
* \n  OK                        :  Result is OK
* \n  ERROR,                    : Failure on execution
* ************************************************************************** */
e_Result (*fnIPStackSupportUnload)( clvoid );

/*****************************************************************************/
/** @fn e_Result (*fnCOMStackSupportLoad)( clvoid)
*
* @brief load COM (serial) stack support on target OS
*
* @param *pFile					( In ) File
* @param *pu32Pos				( Out ) Current pointer position
* @return e_Result
* \n  OK                        :  Result is OK
* \n  ERROR,                    : Failure on execution
* ************************************************************************** */
e_Result (*fnCOMStackSupportLoad)( clvoid );

/*****************************************************************************/
/** @fn e_Result (*fnCOMStackSupportUnload)( clvoid)
*
* @brief unload COM (serial) stack support on target OS
*
* @param *pFile					( In ) File
* @param *pu32Pos				( Out ) Current pointer position
* @return e_Result
* \n  OK                        :  Result is OK
* \n  ERROR,                    : Failure on execution
* ************************************************************************** */
e_Result (*fnCOMStackSupportUnload)( clvoid );

/*****************************************************************************/
/** @fn e_Result (*fnBTStackSupportLoad)( clvoid)
*
* @brief load COM (serial) stack support on target OS
*
* @param *pFile					( In ) File
* @param *pu32Pos				( Out ) Current pointer position
* @return e_Result
* \n  OK                        :  Result is OK
* \n  ERROR,                    : Failure on execution
* ************************************************************************** */
e_Result (*fnBTStackSupportLoad)( clvoid );

/*****************************************************************************/
/** @fn e_Result (*fnCOMStackSupportUnload)( clvoid)
*
* @brief unload COM (serial) stack support on target OS
*
* @param *pFile					( In ) File
* @param *pu32Pos				( Out ) Current pointer position
* @return e_Result
* \n  OK                        :  Result is OK
* \n  ERROR,                    : Failure on execution
* ************************************************************************** */
e_Result (*fnBTStackSupportUnload)( clvoid );


/******************************************************************************/
/* Name : e_Result fnFileGetChar( clvoid *pFileId, cl8 *pcChar )				  */
/* Description :                                                              	*/
/*       get a char from a file	and increment the file pointer by 1				*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : cl8 *pFileId 			: File descriptor								*/
/* ---------------                                                            	*/
/*  Out: cl8 *pcChar			: char read from the file						*/
/* Return value: e_Result                                                     	*/
/*  OK                        :  Result is OK                             		*/
/*  ERROR,                    : Failure on execution                      		*/
/*  MEM_ERR,                  :  Failure on memory management (failure,   		*/
/*                                  allocation ....)                      		*/
/*  PARAMS_ERR,               :  Inconsistent parameters                  		*/
/******************************************************************************/
e_Result (*fnFileGetChar)( clvoid *pFileId, cl8 *pcChar );

/*****************************************************************************/
/** @fn fnPrintComplete( clvoid )
 *
* 	@brief  perform a completion function for print operations. Typically fflush
*
* @return  e_Result
* \n  OK                        :  Result is OK
* \n  ERROR,                    : Failure on execution
* ************************************************************************** */
e_Result (*fnPrintComplete)( clvoid );


/*****************************************************************************/
/** @fn  e_Result *fnGetTime( cl8 **ppTime, clu32 ulLen )
* 	@brief returns formatted string with time from epoch
*
*
* @param 	**ppTime				( In )	80bytes long buffer to contain formatted string
* @param	ulLen 					( In )Length of the buffer passed to low-layer.
* 	\n								If <80, call is rejected
*
* @return e_Result
* \n  OK                        :  Result is OK
* \n  ERROR,                    : Failure on execution
* \n  MEM_ERR,                  :  Failure on memory management (failure,
* \n                                  allocation ....)
* \n  PARAMS_ERR,               :  Inconsistent parameters
* ************************************************************************** */
e_Result (*fnGetTime)( cl8 **ppTime, clu8 ulLen );

/*****************************************************************************/
/** @fn :  e_Result (*fnIORegister)( clvoid *pReader );
* @brief : register a reader to the hosting os. It allows to implement
* \n         a blocking call in the underneath layer on incoming packets from
* \n         the network and propagate a signal the library to the dedicated
* \n         reader. Then the reader reads the data coming from the underneath
* \n         layer
* \n
* \n
*
* @param *pReader 				(In ) Pointer to a reader that this library deals with
*  \n                         	if NULL, returns PARAMS_ERR
* @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* \n TIMEOUT_ERR,              :  Overrun on timing
************************************************************************** */
e_Result (*fnIORegister)(  clvoid *pReader );

/*****************************************************************************/
/** @fn  e_Result (*fnIOUnregister)( clvoid *pReader );
* @brief unregister a reader from the hosting os. It allows to remove
* \n         a blocking call in the underneath layer on incoming packets from
* \n         the network and not to propagate a signal the library to the
* \n         reader.
* \n
* \n **************************************************************************
* @param *pReader 				: ( In ) pointer to a reader that this library deals with
* \n                          	if NULL, returns PARAMS_ERR
*
* @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result (*fnIOUnregister)(  clvoid *pReader );


/*****************************************************************************/
/** @fn  e_Result (*fnIOReset)( clvoid *pReader );
* @brief reset the network connection to a reader
*
* param	*pReader				( In ) pointer to a reader where to reset the connection
*  \n                         if NULL, reset complete network connection
* \n
*
* @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result (*fnIOReset)(  clvoid *pReader );

/*****************************************************************************/
/** @fn :  e_Result (*fnIOSendData)( clvoid *pReader, t_Buffer *ptBuff);
* @brief : send data to a reader via the network API. This access be
* \n               shall be provided by the customer and vary depending on
* \n             target host system OS
* \n
*
* @param *pReader 			 ( In )pointer to there reader where to send the data
*  \n                         if NULL, returns PARAMS_ERR
* @param *ptBuff 			 ( In ) pointer to data buffer
*  \n                         if NULL, returns PARAMS_ERR
*
*
* @return e_Result
*  \n                        * OK
*  \n                        * ERROR:   Failure on execution
*  \n                        * MEM_ERROR : Memory Error (failure, allocation..)
*  \n                        * PARAMS_ERR : Inconsistent parameters
*  \n                        * TIMEOUT_ERR : Timeout when sending data over network
*  \n                         * TRANSFER_IN_PROCESS_ERR: a transfer on this
*  \n                         reader is already ongoing
* ************************************************************************** */
e_Result (*fnIOSendData)( clvoid *pReader, t_Buffer *ptBuff);



/****************************************************************************/
/** @fn  e_Result (*fnIOGetData)( clvoid *pReader, t_Tuple **pptTuple);
* @brief read data from a reader via the network API. This access be
* \n               shall be provided by the customer and vary depending on
* \n             target host system OS
* \n             Implements a call which is non blocking or not to allow polling
* \n             on different readers. Only one thread is passing from one reader
* \n             to the others
* \n
* \n **************************************************************************
* @param *pReader 				( In ) pointer to there reader where to send the data
*  \n                         	if NULL, returns PARAMS_ERR
* @param **pptTuple				( In ) pointer to tuple list. Allocation is provided
*  \n                          by underlayer. Allocation shall be done using
*  \n                          fnAllocMem to allow library to free it properly
*  \n                         if NULL or points to NULL, returns PARAMS_ERR
*
* @return e_Result
* \n                         * OK
* \n                         * ERROR:   Failure on execution
* \n                         * MEM_ERROR : Memory Error (failure, allocation..)
* \n                         * PARAMS_ERR : Inconsistent parameters
* \n                         * TIMEOUT_ERR : Timeout when sending data over network
* \n                         * TRANSFER_IN_PROCESS_ERR: a transfer on this
* \n                         reader is already ongoing
* ************************************************************************** */
e_Result (*fnIOGetData)(  clvoid *pReader, t_Tuple **pptTuple  );

/*****************************************************************************/
/** @fn e_Result (*fnIOOpenConnection)( clvoid *pReader );
* 	@brief open a connection to a reader
*
*
* @param *pReader			: ( In ) pointer to there reader to connect to with IP
* \n                          address and port
*
* @return e_Result
* \n                         * OK   : connection ok
* \n                         * PARAMS_ERR : Inconsistent parameters
* \n                         * TIMEOUT_ERR : no answer from the reader at this address
* \n						i.e : no reader at all
* ************************************************************************** */
e_Result (*fnIOOpenConnection)( clvoid *pReader );

/*****************************************************************************/
/** @fn :  e_Result (*fnNetwOpenConnection)( clvoid *pReader );
* @brief : close a connection to a reader
*
* param *pReader			( In ) pointer to there reader to close connection
*
* @return e_Result
* \n                         * OK   : connection ok
* \n                         * PARAMS_ERR : Inconsistent parameters
* ************************************************************************** */
e_Result (*fnIOCloseConnection)( clvoid *pReader );


/*****************************************************************************/
/** @fn :  e_Result (*fnNetwGetDNSTable)(  clvoid **pptReaderList );
* 	@brief : read the DNS table from the target host system OS
*
*
* @param **pptReaderList 		( In ) pointer to the list of readers registered
*  \n                         	in the host system
*  \n              				if NULL, no Readers are present in the system
*  \n               			it is called by the library after a successful fnNetwReset
*  *
* @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result (*fnIOGetDNSTable)( clvoid **pptReaderList);

/*****************************************************************************/
/** @fn :  e_Result (*fnOTAProgress_cb) ( clu32 eState, clvoid *pReader, clu32 u32Progress );
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
e_Result (*fnOTAProgress_cb) ( clu32 eState, clvoid *pReader, clu32 u32Progress );

/*****************************************************************************/
/** @fn :  e_Result (*fnOTASendDataDone_cb) ( clvoid *pReader, clvoid *pTuple, e_Result status );
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
e_Result (*fnOTASendDataDone_cb) ( clvoid *pReader, clvoid *pTuple, e_Result status );

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
e_Result (*fnOTAData2Read_cb) ( clvoid *ppReader, clvoid *ppTuple, e_Result eStatus );


void (*fnTrace)( char *aFormatString, ... );

/******************************************************************************/
/* Name :      e_Result (*fn_GetElapsedTime)(  )           						*/
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
clu64 (*fn_GetElapsedTime)();


#endif /* CLHALDEFS_H_ */
/**************************************************/
