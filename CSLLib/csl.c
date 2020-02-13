/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: csl.c                                                              */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/*  Description: top layer APIs for CSL                                      */
/*****************************************************************************/
#define CSL_C

#include "csl.h"
#include "cltypes.h"
#include "clsupport.h"
#include "inc\clreaders.h"
#include "inc\cldevices.h"

/********************************************************************************************/
/* Name : cl_sendData(  t_Reader *pReader, t_Tuple *pTuple, e_CallType TsfType, clu32 *pTsfNb ) */
/* Description :                                                                            */
/*        Application wants to send data list to End device via a specified Reader or in 	*/
/*			autonomous way (if pReader is NULL, this API tries to do it on its own)			*/
/*        This function prepares all related buffers to be sent to End Device and           */
/*        encapsulates all calls to Reader/Service/Middleware/encryption to ease            */
/*        customer development                                                              */
/*        it uses the function pointer fnSendDataToReader to send the data to the reader    */
/*        a new call to this API will release memory allocated for previous call            */
/********************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :t_Device *pDevice		: end device to address. Cannot be CL_NULL					*/
/* 		tReader *pReader       : Reader to address. Can be CL_NULL							*/
/*       tTuple *pTuple         : list of buffers to send. Cannot be CL_NULL				 */
/*       e_CallType TsfType      : specifies if the call is blocking/non-blocking           */
/* ---------------                                                                          */
/*  Out: clu32 *pTsfNb          : returns by the library. The transfer number is provided   */
/*                                by the library for callbacks treatment on completion of   */
/*                                transfer to know when the data was effectively issued     */
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*  MEM_ERR,                  :  Failure on memory management (failure,                     */
/*                                  allocation ....)                                        */
/*  PARAMS_ERR,               :  Inconsistent parameters                                    */
/*  TIMEOUT_ERR,              :  Overrun on timing                                          */
/* TRANSFER_IN_PROCESS_ERR   :  A non blocking call is in progress. This                    */
/*                              transfer is rejected. The user application                  */
/*                              shall recall the API to ensure that this                    */
/*                              request is proceeded                                        */
/*******************************************************************************************/
e_Result cl_sendData( t_Device *pDevice, t_Reader *pReader, t_Tuple *pTuple, e_CallType TsfType, clu32 *pTsfNb )
{
	t_Reader *pReaderRegistered = CL_NULL;
	t_Device *pDeviceInList = CL_NULL;
	t_clContext *pCtxt 			= CL_NULL;
	e_Result status 			= CL_ERROR;
	clu32 u32Index 				= 0;
	clu8 *pData					= CL_NULL;
	clu8 *pTrailerData			= CL_NULL;
    t_Tuple	**ppCrtTuple			= CL_NULL;
    t_Tuple	*pCrtTuple			=	CL_NULL;

	//****************************
	// check incoming parameters
	//****************************
	// if a device is specified but the reader is not => CSL shall find the route looking at its list of readers
	//
	// if a reader is specified but the device is not => CSL shall ensure that the data is not for the END DEVICE
	//
	// if the reader AND the end device is specified => the call superseeds the list device/readers and the application pushes its directly to the device via the reader designated as
	// an argument.

	if ( ( pDevice == CL_NULL) & ( pReader == CL_NULL ) )
		return ( CL_PARAMS_ERR);

	if ( pTuple == CL_NULL )
		return ( CL_PARAMS_ERR );

	if ( pTsfNb == CL_NULL )
		return ( CL_PARAMS_ERR );

	// check global parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnSemaphoreWait == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnSemaphoreRelease == CL_NULL )
		return ( CL_ERROR );

	DEBUG_PRINTF("zzzzzzzzzzzzzzzz Send Data zzzzzzzzzzzzzzzz");
	// now, find the couple reader/device if not specified by the application
	if ( pDevice != CL_NULL )
	{
		if ( pReader == CL_NULL )
		{
			if ( CL_FAILED( cl_DeviceFindInList( &pDeviceInList, pDevice ) ) )
				return ( CL_PARAMS_ERR ); // there is no reader that can route this Tuple to the device!

			if ( pDeviceInList == CL_NULL )
				return ( CL_ERROR );

			if ( pDeviceInList->ptReader == CL_NULL )
				return ( CL_ERROR ); // there is no reader that can route this Tuple to the device!
			else
				pReaderRegistered = pDeviceInList->ptReader;
		}
		else
		{
			// check if we have opened the socket to this reader
			if ( CL_FAILED( cl_readerFindInList( &pReaderRegistered, pReader ) ) )
				return ( CL_PARAMS_ERR );			// this reader is not registered => exit

			if ( pReaderRegistered == CL_NULL )
				return ( CL_PARAMS_ERR ); // there is no reader that can route this Tuple to the device!
		}
	}
	else	// check that the transfer is not for a end-device
	{
		if ( pTuple->tOptions.tCnct == TSP_LAYER_SET_DEVICE )
			return ( CL_PARAMS_ERR );

		// check if we have opened the socket to this reader
		if ( CL_FAILED( cl_readerFindInList( &pReaderRegistered, pReader ) ) )
			return ( CL_PARAMS_ERR );			// this reader is not registered => exit

		if ( pReaderRegistered == CL_NULL )
			return ( CL_PARAMS_ERR ); // there is no reader that can route this Tuple to the device!

	}

	// at this stage pReaderRegistered CANNOT be NULL
	if ( pReaderRegistered == CL_NULL )
		return ( CL_PARAMS_ERR );

	do
	{
		// prepare data to be sent using a dedicated thread
		// signals to thread that a message is to be retrieved and send to lower layer
		DEBUG_PRINTF("cl_sendData: fnSemaphoreWait(pSgl4Write)");
		if ( CL_FAILED( pCtxt->ptHalFuncs->fnSemaphoreWait( pReaderRegistered->tSync.pSgl4Write, 0xFFFFFFFF) ) )
				break;

		// check if we are in OTA state. if it is the case, reject incoming packets not from OTA service
		if ( pReaderRegistered->eState == STATE_OTA )
		{
			if ( pTuple->tOptions.tCnct == TSP_LAYER_REQ_OTA_SVC)
			{
#ifdef TYTY
				// assign data to be sent by dedicated thread
				ppCrtTuple = &pReaderRegistered->p_TplList2Send;

				if ( *ppCrtTuple == CL_NULL )
					pReaderRegistered->p_TplList2Send = pTuple;
				else
				{
                    while ( ((*ppCrtTuple)->pNext ) != CL_NULL )
					{
						ppCrtTuple = (t_Tuple **)(& ((*ppCrtTuple)->pNext));
					}
					(*ppCrtTuple)->pNext = pTuple;
				}
#else
				pCrtTuple = pReaderRegistered->p_TplList2Send;
				if ( pCrtTuple == CL_NULL )
					pReaderRegistered->p_TplList2Send = pTuple;
				else
				{
					while ( pCrtTuple->pNext != CL_NULL )
					{
						pCrtTuple = pCrtTuple->pNext;
					};
					pCrtTuple->pNext = pTuple;
				}
#endif
			}
			else
			{
				// TO RELEASE MEMORY at user level
				status = CL_ERROR;
			}
		}
		else
		{
#ifdef TYTY

			// assign data to be sent by dedicated thread
			ppCrtTuple = &pReaderRegistered->p_TplList2Send;

			if ( *ppCrtTuple == CL_NULL )
				pReaderRegistered->p_TplList2Send = pTuple;
			else
			{
                while ( ((*ppCrtTuple)->pNext ) != CL_NULL )
				{
					ppCrtTuple = (t_Tuple **)(& ((*ppCrtTuple)->pNext));
				}
				(*ppCrtTuple)->pNext = pTuple;
			}
#else
				pCrtTuple = pReaderRegistered->p_TplList2Send;
				if ( pCrtTuple == CL_NULL )
					pReaderRegistered->p_TplList2Send = pTuple;
				else
				{
					while ( pCrtTuple->pNext != CL_NULL )
					{
						pCrtTuple = pCrtTuple->pNext;
					};
					pCrtTuple->pNext = pTuple;
				}
#endif

		}
		// now release the semaphore to enable the thread to send the data to the reader
		if ( CL_FAILED( pCtxt->ptHalFuncs->fnSemaphoreRelease( pReaderRegistered->tSync.pSgl4Write ) ) )
				break;

		if ( CL_FAILED(status) )
			break;

		// if blocking call is specified, then block on callbacks of reception
		if ( TsfType == BLOCKING )
		{
			// block the semaphore on the reader if a blocking call was specified. It will be unblocked when the transfer to the reader will be issued
			while( (  status = pCtxt->ptHalFuncs->fnSemaphoreWait( pReaderRegistered->tSync.pSgl4WriteComplete, 1  ) ) == CL_TIMEOUT_ERR );

			// at this stage, we have blocked the semaphore. Try to relock the semaphore. As long as it is not released by the lower layer, the send command was
			// not properly done
			while( (  status = pCtxt->ptHalFuncs->fnSemaphoreWait( pReaderRegistered->tSync.pSgl4WriteComplete, 1  ) ) == CL_TIMEOUT_ERR )
			{
				pCtxt->ptHalFuncs->fnWaitMs( 100 );
			}

			// now we can release it and pass to next call
			status = pCtxt->ptHalFuncs->fnSemaphoreRelease( pReaderRegistered->tSync.pSgl4WriteComplete );
		}

	   break;
	}
	while ( 1 );

	// error handler
	if ( CL_FAILED( status ) )
	{
		pCtxt->ptHalFuncs->fnSemaphoreDestroy( pReaderRegistered->tSync.pSgl4Write );
	}

	return CL_OK;
}


/*******************************************************************************************/
/* Name : cl_getData( t_Device **ppDevice, t_Reader **ppReader, t_Tuple **ppTuple, e_CallType TsfType, clu32 *pTrfNb ) */
/* Description :                                                                            */
/*        get Data from a reader (service data as an example) or end device                               */
/*        it is a blocking call of e_CallType is set to BLOCKING                             */
/*        if ppTuple is returned with NULL point no data is pushed back from underlayers    */
/*        in the library the callback fnNetwData2Read_cb shall have return                  */
/*        first before calling this API if NON_BLOCKING was specified                       */
/*        All data shall be released from memory after usage                                */
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : tReader **ppReader       : Reader to address                                       */
/*                                if NULL pointer is passed, this call waits for any data   */
/*                                coming on any reader                                      */
/*      e_CallType TsfType       : BLOCKING/NON BLOCKING call                               */
/* ---------------                                                                          */
/*  Out: t_Tuple **ppTuple      : pointer on read data from the reader                      */
/*                                if points to NULL, no data is coming from the underlayers */
/*		t_Device **ppDevice		: provide Device information where the data is coming from  */
/*       tReader **ppReader     : Provide Reader information where the data is coming from  */
/*                                if NULL, no data is sent back                             */
/*  clu32 *pTsfNb               : returns by the library. The transfer number is provided   */
/*                                by the library for callbacks treatment on completion of   */
/*                                transfer to know when the data was effectively issued     */
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*  MEM_ERR,                  :  Failure on memory management (failure,                     */
/*                                  allocation ....)                                        */
/*  PARAMS_ERR,               :  Inconsistent parameters                                    */
/*  TIMEOUT_ERR,              :  Overrun on timing                                          */
/*******************************************************************************************/
e_Result cl_getData( t_Device **ppDevice, t_Reader **ppReader, e_CallType TsfType, t_Tuple **ppTuple, clu32 *pTrfNb )
{
	t_Reader *pReaderRegistered = CL_NULL;
	t_Device *pDeviceInList = CL_NULL;
	t_clContext *pCtxt 			= CL_NULL;
	e_Result status 			= CL_ERROR;

	//****************************
	// check incoming parameters
	//****************************
	if ( ( ppDevice == CL_NULL) | ( ppReader == CL_NULL ) | ( ppTuple == CL_NULL ) | ( pTrfNb == CL_NULL ))
		return ( CL_PARAMS_ERR);

	// check global parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnSemaphoreWait == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnSemaphoreRelease == CL_NULL )
		return ( CL_ERROR );

	// now, find

	// now, find the couple reader/device if not specified by the application
	if ( *ppDevice != CL_NULL )
	{
		if ( *ppReader == CL_NULL )
		{
			if ( CL_FAILED( cl_DeviceFindInList( &pDeviceInList, *ppDevice ) ) )
				return ( CL_PARAMS_ERR ); // there is no reader that can route this Tuple to the device!

			if ( pDeviceInList == CL_NULL )
				return ( CL_ERROR );

			if ( pDeviceInList->ptReader == CL_NULL )
				return ( CL_ERROR ); // there is no reader that can route this Tuple to the device!
			else
				pReaderRegistered = pDeviceInList->ptReader;

		}
		else
		{
			// check if we have opened the socket to this reader
			if ( CL_FAILED( cl_readerFindInList( &pReaderRegistered, *ppReader ) ) )
				return ( CL_PARAMS_ERR );			// this reader is not registered => exit

			if ( pReaderRegistered == CL_NULL )
				return ( CL_PARAMS_ERR ); // there is no reader that can route this Tuple to the device!
		}
	}
	else	// check that the transfer is not for a end-device
	{
/*		if ( (*ppTuple)->tOptions.tCnct == TSP_LAYER_SET_DEVICE )
			return ( CL_PARAMS_ERR );
*/
		// check if we have opened the socket to this reader
		if ( CL_FAILED( cl_readerFindInList( &pReaderRegistered, *ppReader ) ) )
			return ( CL_PARAMS_ERR );			// this reader is not registered => exit

		if ( pReaderRegistered == CL_NULL )
			return ( CL_PARAMS_ERR ); // there is no reader that can route this Tuple to the device!

	}

	// at this stage pReaderRegistered CANNOT be NULL
	if ( pReaderRegistered == CL_NULL )
		return ( CL_PARAMS_ERR );


	do
	{
		// get data to be sent using a dedicated thread
		if ( CL_FAILED( pCtxt->ptHalFuncs->fnSemaphoreWait( pReaderRegistered->tSync.pSgl4Read, 0xFFFFFFFF) ) )
				break;
		// get tuple list
		*ppTuple = pReaderRegistered->p_TplList2Read;

		// release reader from its hold list
		pReaderRegistered->p_TplList2Read = CL_NULL;

		// signal we are ready for new data
		pCtxt->ptHalFuncs->fnSemaphoreRelease( pReaderRegistered->tSync.pSgl4Read );

        if (*ppTuple != CL_NULL )   // we have data coming from underlayer
            break;
	}
	while ( 1 );

	// error handler
	if ( CL_FAILED( status ) )
	{
		pCtxt->ptHalFuncs->fnSemaphoreDestroy( pReaderRegistered->tSync.pSgl4Write );

	}

	return ( CL_OK );}

/*******************************************************************************************/
/* Name : cl_getDataCompleted( t_Tuple *pTuple )                                            */
/* Description :                                                                            */
/*        library free the memory of buffers used from previous read calls                  */
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : t_Tuple *pTuple        : free memory of buffers after having used data read from   */
/*                                network                                                   */
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*  MEM_ERR,                  :  Failure on memory management (failure,                     */
/*                                  allocation ....)                                        */
/*  PARAMS_ERR,               :  Inconsistent parameters                                    */
/*  TIMEOUT_ERR,              :  Overrun on timing                                          */
/* TRANSFER_IN_PROCESS_ERR   :  A non blocking call is in progress. This                    */
/*                              transfer is rejected. The user application                  */
/*                              shall recall the API to ensure that this                    */
/*                              request is proceeded                                        */
/*******************************************************************************************/
e_Result cl_getDataCompleted( t_Tuple *pTuple )
{

	return ( CL_OK );
}


/********************************************************************************************/
/* Name : e_Result cl_DeviceSetEncryptionLevel( t_Device *pDevice, e_OptionsKey eKeyType  ); */
/* Description :                                                                            */
/*        define default encryption level for a device                                      */
/*        if in the t_Tuple, encryption set is not set (DEFAULT)							*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------         {                                                                 */
/*  In : t_Device *pDevice: designate the End Device where to set default encryption level  */
/*        e_Crypted eDefaultCryptValue:                                                     */
/*              if NONE: no encryption is set between end-user application and end-device   */
/*              if CRYPTO_APPLI : only Application layer of radio datagrams is encrypted      */
/*              if CRYPTO_PHY : PHY layer and APPLICATION Layer at radio levels are encrypted */
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*  MEM_ERR,                  :  Failure on memory management (failure,                     */
/*                                  allocation ....)                                        */
/*  PARAMS_ERR,               :  Inconsistent parameters                                    */
/*******************************************************************************************/
e_Result cl_DeviceSetEncryptionLevel( t_Device *pDevice, e_Crypted eDefaultCryptValue )
{
	return ( CL_OK );
}
/******************************************************************************/
/* Name :      e_Result cl_PrintOutCompletion(  clvoid );		        	  */
/* Description : printf completion to fflush as an example			          */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : none									                              */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/******************************************************************************/
e_Result cl_PrintOutCompletion( clvoid )
{
	t_clContext *pCtxt 		= CL_NULL;
	e_Result status 		= CL_ERROR;

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnPrintComplete != CL_NULL )
		pCtxt->ptHalFuncs->fnPrintComplete( );

	return ( CL_OK );

}
