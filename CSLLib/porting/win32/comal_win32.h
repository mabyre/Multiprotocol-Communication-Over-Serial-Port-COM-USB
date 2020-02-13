/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: comal_win32.h                                                      */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/* Description: porting layer for com port access in WIN32 environment       */
/*****************************************************************************/

#ifndef COMAL_WIN32_H_
#define COMAL_WIN32_H_

/******************************************************************************/
/* Name :  e_Result cl_COMAlResetWin32( clvoid *pReader );                   */
/* Description : reset the com connection to a reader                     		*/
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pReader : pointer to a reader where to reset the connection*/
/*                           if NULL, reset complete network connection       */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                     */
/*  OK                        :  Result is OK                                 */
/*  ERROR,                    : Failure on execution                          */
/*  MEM_ERR,                  :  Failure on memory management (failure,       */
/*                                  allocation ....)                          */
/*  PARAMS_ERR,               :  Inconsistent parameters                      */
/*  TIMEOUT_ERR,              :  Overrun on timing                            */
/******************************************************************************/
e_Result cl_COMAlResetWin32(  clvoid *pReader );

/******************************************************************************/
/* Name :  e_Result cl_COMAlRegisterWin32( clvoid *pReader );                */
/* Description : register a reader to the hosting os. It allows to implement  */
/*          a blocking call in the underneath layer on incoming packets from  */
/*          the com port and propagate a signal the library to the dedicated   */
/*          reader. Then the reader reads the data coming from the underneath */
/*          layer                                                             */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader *pReader : pointer to a reader that this library deals with */
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
e_Result cl_COMAlRegisterWin32(  clvoid *pReader );

/******************************************************************************/
/* Name :  e_Result cl_COMAlUnregisterWin32( clvoid *pReader );                  */
/* Description : unregister a reader from the hosting os. It allows to remove */
/*          a blocking call in the underneath layer on incoming packets from */
/*          the network and not to propagate a signal the library to the     */
/*          reader.                                                          */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pReader : pointer to a reader that this library deals with */
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
e_Result cl_COMAlUnregisterWin32(  clvoid *pReader );

/******************************************************************************/
/* Name :  e_Result cl_COMAlSendDataWin32( clvoid *pReader, t_Buffer *ptBuff);   */
/* Description : send data to a reader via the COM API. This access be    */
/*                shall be provided by the customer and vary depending on     */
/*              target host system OS                                         */
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
e_Result cl_COMAlSendDataWin32( clvoid *pReader, t_Buffer *ptBuff);

/******************************************************************************/
/* Name :  e_Result cl_COMAlGetDataWin32( clvoid *pReader, t_Tuple **pptTuple);  */
/* Description : read data from a reader via the COM API. This access be    	*/
/*                shall be provided by the customer and vary depending on     */
/*              target host system OS                                         */
/*              Implements a call which is non blocking or not to allow polling */
/*              on different readers. Only one thread is passing from one reader*/
/*              to the others                                                 */
/*                                                                            */
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
e_Result cl_COMAlGetDataWin32(  clvoid *pReader, t_Tuple **pptTuple );

// BRY_05102015
e_Result cl_ReaderAndStatusProc( clvoid *ptReader );

/******************************************************************************/
/* Name :  e_Result (cl_COMAlOpenConnectionWin32)( clvoid *pReader );  		*/
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
e_Result cl_COMAlOpenConnectionWin32( clvoid *pReader );

/******************************************************************************/
/* Name :  e_Result (cl_COMAlCloseConnectionWin32)( clvoid *pReader );  		*/
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
e_Result cl_COMAlCloseConnectionWin32( clvoid *pReader );

/*****************************************************************************/
/* e_Result cl_IPStackSupportLoadWin32( clvoid)								*/
/*																			*/
/* Description : load COM stack support on target OS							*/
/*****************************************************************************/
/* Return Value : 															*/
/*   OK                        :  Result is OK								*/
/*   ERROR,                    : Failure on execution						*/
/****************************************************************************/
e_Result cl_COMStackSupportLoadWin32( clvoid );

/*****************************************************************************/
/* e_Result cl_IPStackSupportUnloadWin32( clvoid)								*/
/*																			*/
/* Description : unload COM stack support on target OS						*/
/*****************************************************************************/
/* Return Value : 															*/
/*   OK                        :  Result is OK								*/
/*****************************************************************************/
e_Result cl_COMStackSupportUnloadWin32( clvoid );

#endif /* COMAL_WIN32_H_ */
/**************************************************/
