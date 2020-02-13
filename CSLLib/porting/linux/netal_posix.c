/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: netal_posix.c                                                      */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/

#include "..\..\csl.h"
#include "netal_posix.h"


#ifdef LINUXPC


/******************************************************************************/
/* Name :  e_Result cl_NetwResetLinux( t_Reader *pReader );                       */
/* Description : reset the network connection to a reader                     */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader *pReader : pointer to a reader where to reset the connection*/
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
e_Result cl_NetwResetLinux(  t_Reader *pReader )
{
	return ( CL_OK );
}

/******************************************************************************/
/* Name :  e_Result cl_NetwRegisterLinux( t_Reader *pReader );                    */
/* Description : register a reader to the hosting os. It allows to implement  */
/*          a blocking call in the underneath layer on incoming packets from */
/*          the network and propagate a signal the library to the dedicated  */
/*          reader. Then the reader reads the data coming from the underneath*/
/*          layer                                                            */
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
e_Result cl_NetwRegisterLinux(  t_Reader *pReader )
{
	return ( CL_OK );
}


/******************************************************************************/
/* Name :  e_Result cl_NetwUnregisterLinux( t_Reader *pReader );                  */
/* Description : unregister a reader from the hosting os. It allows to remove */
/*          a blocking call in the underneath layer on incoming packets from */
/*          the network and not to propagate a signal the library to the     */
/*          reader.                                                          */
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
e_Result cl_NetwUnregisterLinux(  t_Reader *pReader )
{
	return ( CL_OK );
}



/******************************************************************************/
/* Name :  e_Result cl_NetwResetLinux( t_Reader *pReader );                       */
/* Description : reset the network connection to a reader                     */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader *pReader : pointer to a reader where to reset the connection*/
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
e_Result cl_NetwResetLinux(  t_Reader *pReader )
{
	return ( CL_OK );
}


/******************************************************************************/
/* Name :  e_Result cl_NetwSendDataLinux( t_Reader *pReader, t_Buffer *ptBuff);   */
/* Description : send data to a reader via the network API. This access be    */
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
/*                          * ERROR:   Failure on execution						*/
/*                          * MEM_ERROR : Memory Error (failure, allocation..)*/
/*                          * PARAMS_ERR : Inconsistent parameters            */
/*                          * TIMEOUT_ERR : Timeout when sending data over network */
/*                          * TRANSFER_IN_PROCESS_ERR: a transfer on this     */
/*                          reader is already ongoing                         */
/******************************************************************************/
e_Result cl_NetwSendDataLinux( t_Reader *pReader, t_Buffer *ptBuff)
{
	return ( CL_OK );
}




/******************************************************************************/
/* Name :  e_Result cl_NetwGetDataLinux( t_Reader *pReader, t_Tuple **pptTuple);  */
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
e_Result cl_NetwGetDataLinux(  t_Reader *pReader, t_Tuple **pptTuple  )
{
	return ( CL_OK );
}



/******************************************************************************/
/* Name :  e_Result cl_NetwGetDNSTableLinux(  t_Reader **pptReaderList );         */
/* Description : read the DNS table from the target host system OS            */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader **pptReaderList : pointer to the list of readers registered */
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
e_Result cl_NetwGetDNSTableLinux( t_Reader **pptReaderList)
{
	return ( CL_OK );
}


#endif // LINUXPC
