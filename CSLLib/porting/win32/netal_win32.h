/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/

#ifdef WIN32
//#ifndef(__x86_64__)

#ifndef NETAL_WIN32_H_
#define NETAL_WIN32_H_


/******************************************************************************/
/* Name :  e_Result cl_NetwResetWin32( clvoid *ptReader );                       */
/* Description : reset the network connection to a reader                     */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pReader : pointer to a reader where to reset the connection*/
/*                           if NULL, reset complete network connection       */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                 */
/*  ERROR,                    : Failure on execution                          */
/*  MEM_ERR,                  :  Failure on memory management (failure,       */
/*                                  allocation ....)                          */
/*  PARAMS_ERR,               :  Inconsistent parameters                      */
/*  TIMEOUT_ERR,              :  Overrun on timing                            */
/******************************************************************************/
e_Result cl_NetwResetWin32(  clvoid *ptReader );

/******************************************************************************/
/* Name :  e_Result cl_NetwRegisterWin32( clvoid *ptReader );                    */
/* Description : register a reader to the hosting os. It allows to implement  */
/*          a blocking call in the underneath layer on incoming packets from */
/*          the network and propagate a signal the library to the dedicated  */
/*          reader. Then the reader reads the data coming from the underneath*/
/*          layer                                                            */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *ptReader : pointer to a reader that this library deals with */
/*                           if NULL, returns PARAMS_ERR                      */
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
e_Result cl_NetwRegisterWin32(  clvoid *ptReader );

/******************************************************************************/
/* Name :  e_Result cl_NetwUnregisterWin32( clvoid *ptReader );                  */
/* Description : unregister a reader from the hosting os. It allows to remove */
/*          a blocking call in the underneath layer on incoming packets from */
/*          the network and not to propagate a signal the library to the     */
/*          reader.                                                          */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *ptReader : pointer to a reader that this library deals with */
/*                           if NULL, returns PARAMS_ERR                      */
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
e_Result cl_NetwUnregisterWin32(  clvoid *ptReader );


/******************************************************************************/
/* Name :  e_Result cl_NetwResetWin32( clvoid *ptReader );                       */
/* Description : reset the network connection to a reader                     */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *ptReader : pointer to a reader where to reset the connection*/
/*                           if NULL, reset complete network connection       */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,        */
/*                                  allocation ....)                           */
/*  TIMEOUT_ERR,              :  Overrun on timing                             */
/******************************************************************************/
e_Result cl_NetwResetWin32(  clvoid *ptReader );

/******************************************************************************/
/* Name :  e_Result cl_NetwSendDataWin32( clvoid *ptReader, t_Buffer *ptBuff);   */
/* Description : send data to a reader via the network API. This access be    */
/*                shall be provided by the customer and vary depending on     */
/*              target host system OS                                         */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *ptReader : pointer to there reader where to send the data   */
/*                           if NULL, returns PARAMS_ERR                      */
/*        t_Buffer *ptBuff : pointer to data buffer                           */
/*                           if NULL, returns PARAMS_ERR                      */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*                          * OK                                              */
/*                          * ERROR:   Failure on execution		*/
/*                          * MEM_ERROR : Memory Error (failure, allocation..)*/
/*                          * PARAMS_ERR : Inconsistent parameters            */
/*                          * TIMEOUT_ERR : Timeout when sending data over network */
/*                          * TRANSFER_IN_PROCESS_ERR: a transfer on this     */
/*                          reader is already ongoing                         */
/******************************************************************************/
e_Result cl_NetwSendDataWin32( clvoid *ptReader, t_Buffer *ptBuff);



/******************************************************************************/
/* Name :  e_Result cl_NetwGetDataWin32( clvoid *ptReader, , t_Tuple **pptTuple);  */
/* Description : read data from a reader via the network API. This access be    */
/*                shall be provided by the customer and vary depending on     */
/*              target host system OS                                         */
/*              Implements a call which is non blocking or not to allow polling */
/*              on different readers. Only one thread is passing from one reader*/
/*              to the others                                                 */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *ptReader : pointer to there reader where to send the data   */
/*                           if NULL, returns PARAMS_ERR                      */
/*        t_Tuple **pptTuple: pointer to tuple list. Allocation is provided  */
/*                            by underlayer. Allocation shall be done using   */
/*                            fnAllocMem to allow library to free it properly */
/*                           if NULL or points to NULL, returns PARAMS_ERR    */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*                          * OK                                              */
/*                          * ERROR:   Failure on execution						*/
/*                          * MEM_ERROR : Memory Error (failure, allocation..)*/
/*                          * PARAMS_ERR : Inconsistent parameters            */
/*                          * TIMEOUT_ERR : Timeout when sending data over network */
/*                          * TRANSFER_IN_PROCESS_ERR: a transfer on this     */
/*                          reader is already ongoing                         */
/******************************************************************************/
e_Result cl_NetwGetDataWin32(  clvoid *ptReader, t_Tuple **pptTuple  );


/******************************************************************************/
/* Name :  e_Result cl_NetwGetDNSTableWin32(  clvoid **pptReaderList );         */
/* Description : read the DNS table from the target host system OS            */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid **pptReaderList : pointer to the list of readers registered */
/*                           in the host system                               */
/*                if NULL, no Readers are present in the system               */
/*                it is called by the library after a successful fnNetwReset */
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
e_Result cl_NetwGetDNSTableWin32( clvoid **pptReaderList);


/******************************************************************************/
/* Name :  e_Result (cl_NetwOpenConnectionWin32)( clvoid *ptReader );  		*/
/* Description : open a connection to a reader    								*/
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pReader : pointer to there reader to connect to with ip    */
/*                           address and port			                      */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                     */
/*                          * OK   : connection ok                            */
/*                          * PARAMS_ERR : Inconsistent parameters            */
/*                          * TIMEOUT_ERR : no answer from the reader at this address */
/*							i.e : no reader at all							*/
/******************************************************************************/
e_Result (cl_NetwOpenConnectionWin32)( clvoid *ptReader );
/******************************************************************************/
/* Name :  e_Result cl_NetwGetDataDiscoverWin32( clvoid *ptReader, t_Tuple **pptTuple);  */
/* Description : read data from a reader via the network API. This access be    */
/*                shall be provided by the customer and vary depending on     	*/
/*              target host system OS                                         	*/
/*              Implements a call which is non blocking or not to allow polling */
/*              on different readers. Only one thread is passing from one reader*/
/*              to the others                                                 	*/
/*				This API, for discover process, receive UDP data				*/
/*                                                                            	*/
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader *pReader : pointer to there reader where to send the data   */
/*                           if NULL, returns PARAMS_ERR                      */
/*        t_Tuple **pptTuple: pointer to tuple list. Allocation is provided  */
/*                            by underlayer. Allocation shall be done using   */
/*                            fnAllocMem to allow library to free it properly */
/*                           if NULL or points to NULL, returns PARAMS_ERR    */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*                          * OK                                              */
/*                          * ERROR:   Failure on execution						*/
/*                          * MEM_ERROR : Memory Error (failure, allocation..)*/
/*                          * PARAMS_ERR : Inconsistent parameters            */
/*                          * TIMEOUT_ERR : Timeout when sending data over network */
/*                          * TRANSFER_IN_PROCESS_ERR: a transfer on this     */
/*                          reader is already ongoing                         */
/******************************************************************************/
e_Result cl_NetwGetDataDiscoverWin32(  clvoid *ptReader, t_Tuple **pptTuple );
/*******************************************************************************/
/* Name :  e_Result (cl_NetwOpenConnectionDiscoverWin32)( clvoid *ptReader );  	*/
/* Description : open a connection to a reader to be used for discover of others readers */
/*	for IP readers, this function declares UDP sockets for discover instead of	*/
/*	standard TCP																*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : t_Reader *pReader : pointer to there reader to connect to with ip    	*/
/*                           address and port			                      	*/
/* ---------------                                                            	*/
/*  Out: none                                                                 	*/
/* Return value: e_Result                                                     	*/
/*                          * OK   : connection ok                            	*/
/*                          * PARAMS_ERR : Inconsistent parameters            	*/
/*                          * TIMEOUT_ERR : no answer from the reader at this address */
/*							i.e : no reader at all								*/
/******************************************************************************/
e_Result cl_NetwOpenConnectionDiscoverWin32( clvoid *ptReader );
/******************************************************************************/
/* Name :  e_Result cl_NetwSendDataDiscoverWin32( clvoid *ptReader, t_Buffer *ptBuff);   */
/* Description : send data to a reader via the network API. This access be    */
/*                shall be provided by the customer and vary depending on     */
/*              target host system OS                                         */
/*				This API, for discover process, send data over UDP socket	*/
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader *pReader : pointer to there reader where to send the data   */
/*                           if NULL, returns PARAMS_ERR                      */
/*        t_Buffer *ptBuff : pointer to data buffer                           */
/*                           if NULL, returns PARAMS_ERR                      */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*                          * OK                                              */
/*                          * ERROR:   Failure on execution					  */
/*                          * MEM_ERROR : Memory Error (failure, allocation..)*/
/*                          * PARAMS_ERR : Inconsistent parameters            */
/*                          * TIMEOUT_ERR : Timeout when sending data over network */
/*                          * TRANSFER_IN_PROCESS_ERR: a transfer on this     */
/*                          reader is already ongoing                         */
/******************************************************************************/
e_Result cl_NetwSendDataDiscoverWin32( clvoid *ptReader, t_Buffer *ptBuff);
/******************************************************************************/
/* Name :  e_Result (cl_NetwCloseConnectionWin32)( clvoid *ptReader );  		*/
/* Description : close a connection to a reader    								*/
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *ptReader : pointer to there reader to close connection    */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                     */
/*                          * OK   : connection ok                            */
/*                          * PARAMS_ERR : Inconsistent parameters            */
/******************************************************************************/
e_Result (cl_NetwCloseConnectionWin32)( clvoid *ptReader );

/*****************************************************************************/
/* e_Result cl_IPStackSupportLoadWin32( clvoid)								*/
/*																			*/
/* Description : load IP stack support on target OS							*/
/*****************************************************************************/
/* Return Value : 															*/
/*   OK                        :  Result is OK								*/
/*   ERROR,                    : Failure on execution						*/
/****************************************************************************/
e_Result (cl_IPStackSupportLoadWin32)( clvoid );

/*****************************************************************************/
/* e_Result cl_IPStackSupportUnloadWin32( clvoid)								*/
/*																			*/
/* Description : unload IP stack support on target OS						*/
/*****************************************************************************/
/* Return Value : 															*/
/*   OK                        :  Result is OK								*/
/*****************************************************************************/
e_Result cl_IPStackSupportUnloadWin32( clvoid );


#endif /* NETAL_WIN32_H_ */
//#endif // 32 bits
#endif // WIN32
/**************************************************/
