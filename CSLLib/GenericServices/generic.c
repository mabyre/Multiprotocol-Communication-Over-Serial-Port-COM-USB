/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name:                                                                    */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
#define GENERIC_C_
#include <stdio.h>
#include <string.h>
#include "..\csl.h"
#include "..\inc\generic.h"
#include "..\inc\clreaders.h"
#include "..\inc\cldevices.h"
#include "..\inc\cltuple.h"
#include "..\inc\otaservice.h"

clu32 g_PrintCounter = 0;
/********************************************************************************************/
/* Name :  e_Result clInit( 																*/
/* 							e_StackSupport eStackSupport                       				*/
/*							t_clSys_HalFunctions *pSysContext                               */
/*                          t_clSys_CallBackFunctions *pCallBacks,							*/
/*							t_clSys_CallBackFunctions *ptCallbacksDiscover					*/
/*							t_Reader* pReader												*/
/*							t_clReader_HalFuncs	*p_IPReaderDefaultHAL,						*/
/*							t_clReader_HalFuncs	*p_IPReaderDiscoverDefaultHAL,				*/
/* 							t_clReader_HalFuncs	*p_COMReaderDefaultHAL, 					*/
/* 							t_clReader_HalFuncs	*p_COMReaderDiscoverDefaultHAL, 			*/
/* 							t_clReader_HalFuncs	*p_BTReaderDefaultHAL,						*/
/* 							t_clReader_HalFuncs	*p_BTReaderDiscoverDefaultHAL,				*/
/* 							t_MenuFileDef *ptMenuFileDef )									*/
/* Description : Initialize the library with function pointers and callbacks                */
/*                                                                                          */
/********************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : clSys_HalFunctions_s *pSysContext: function pointers for library                   */
/*      if NULL, no errors are returned but empty functions are implemented                 */
/*      per default                                                                         */
/*      the library initialize locks/semaphores/network access in this call                 */
/*        CALSSys_CallBackFunctions_s *pCallBacks: function pointers for                    */
/*                        callbacks signaled by the library                                 */
/*		t_MenuFileDef *ptMenuFileDef : file to store persistent parameters					*/
/*		e_StackSupport eStackSupport : specifies if IP/COM/BT support is required			*/
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*  MEM_ERR,                  :  Failure on memory management (failure,                     */
/*                                  allocation ....)                                        */
/*  PARAMS_ERR,               :  Inconsistent parameters                                    */
/********************************************************************************************/
e_Result clInit( e_StackSupport eStackSupport, t_clSys_HalFunctions *pSysContext, t_clSys_CallBackFunctions *pCallBacks, t_clSys_CallBackFunctions *ptCallbacksDiscover, t_Reader* pReader, t_clReader_HalFuncs *p_IPReaderDefaultHAL, t_clReader_HalFuncs *p_IPReaderDiscoverDefaultHAL, t_clReader_HalFuncs	*p_COMReaderDefaultHAL, t_clReader_HalFuncs	*p_COMReaderDiscoverDefaultHAL, t_clReader_HalFuncs	*p_BTReaderDefaultHAL, t_clReader_HalFuncs	*p_BTReaderDiscoverDefaultHAL, const t_MenuFileDef *ptMenuFileDef )
{
	t_clContext *pCtxt = CL_NULL;
	e_Result status = CL_ERROR;
    clvoid *tThreadRegistrationReader = CL_NULL;
	clvoid *tThreadCryptoId = CL_NULL;

	if ( ( pSysContext == CL_NULL ) | ( pCallBacks == CL_NULL ) | ( ptMenuFileDef == CL_NULL ) | ( pReader == CL_NULL ) )
		return ( CL_PARAMS_ERR );

	if ( (!(eStackSupport & IP_STACK_SUPPORT)) & (!(eStackSupport & COM_STACK_SUPPORT)) & (!(eStackSupport & BT_STACK_SUPPORT)) )
		return ( CL_PARAMS_ERR );

	g_tCtxt.eStackSupport			=	eStackSupport;
	g_tCtxt.ptHalFuncs 				= 	( t_clSys_HalFunctions * ) 			pSysContext;
	g_tCtxt.ptCallbacks 			=	( t_clSys_CallBackFunctions * ) 	pCallBacks;
	g_tCtxt.ptCallbacksDiscover		=	( t_clSys_CallBackFunctions * ) 	ptCallbacksDiscover;
	g_tCtxt.ptMenuFileDef			= 	( t_MenuFileDef *)					ptMenuFileDef ;
	g_tCtxt.p_COMReaderHal 			= 	( t_clReader_HalFuncs *) 			p_COMReaderDefaultHAL;
	g_tCtxt.p_IPReaderHal 			= 	( t_clReader_HalFuncs *)	 		p_IPReaderDefaultHAL;
	g_tCtxt.p_BTReaderHal 			= 	( t_clReader_HalFuncs *) 			p_BTReaderDefaultHAL;
	g_tCtxt.p_COMReaderDiscoverHal 	= 	( t_clReader_HalFuncs *) 			p_COMReaderDiscoverDefaultHAL;
	g_tCtxt.p_IPReaderDiscoverHal 	= 	( t_clReader_HalFuncs *)	 		p_IPReaderDiscoverDefaultHAL;
	g_tCtxt.p_BTReaderDiscoverHal 	= 	( t_clReader_HalFuncs *) 			p_BTReaderDiscoverDefaultHAL;
	g_tCtxt.ptCslReader				=	( t_Reader *)						pReader;

	// for crypto/OTA: this is internal resource exposed for debugging only. Not exposed when initializing the framework
	g_tCtxt.tCryptoParams.ptEncryptionKeyReader	= 	CL_NULL;
	g_tCtxt.tCryptoParams.ptTargetReader		=	CL_NULL;
	g_tCtxt.tDebugParams.u64NanoSeconds			=	0;
	g_tCtxt.tDebugParams.u64Seconds				=	0;

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED( cl_GetContext( &pCtxt )) )
	{
		return ( CL_ERROR );
	}
	else
	{
		DEBUG_PRINTF("clInit:cl_GetContext:CL_OK");
	}

	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptCallbacks == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnCreateThread == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnDestroyThread == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnSemaphoreCreate == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnSemaphoreDestroy == CL_NULL )
		return ( CL_ERROR );

	// check if necessary function for IP Stack support are present if needed
	if ( pCtxt->eStackSupport & IP_STACK_SUPPORT )
	{
		DEBUG_PRINTF("IP_STACK_SUPPORT");
		if ( pCtxt->ptHalFuncs->fnIPStackSupportLoad == CL_NULL )
			return ( CL_ERROR );

		if ( pCtxt->ptHalFuncs->fnIPStackSupportUnload == CL_NULL )
			return ( CL_ERROR );
	}

	if ( pCtxt->eStackSupport & COM_STACK_SUPPORT )
	{
		DEBUG_PRINTF("COM_STACK_SUPPORT");
		if ( pCtxt->ptHalFuncs->fnCOMStackSupportLoad == CL_NULL )
			return ( CL_ERROR );

		if ( pCtxt->ptHalFuncs->fnCOMStackSupportUnload == CL_NULL )
			return ( CL_ERROR );
	}

	if ( pCtxt->eStackSupport & BT_STACK_SUPPORT )
	{
		DEBUG_PRINTF("BT_STACK_SUPPORT");
		if ( pCtxt->ptHalFuncs->fnBTStackSupportLoad == CL_NULL )
			return ( CL_ERROR );

		if ( pCtxt->ptHalFuncs->fnBTStackSupportUnload == CL_NULL )
			return ( CL_ERROR );
	}


	/*--------------------------------------------------------------------*/
	/* Completement d�bile, m�connaissance du RT                          */
	/*--------------------------------------------------------------------*/
	for (;;)
	{
		DEBUG_PRINTF("clInit:Thread: BEGIN");

		// if IP stack support is required
		if ( pCtxt->eStackSupport & IP_STACK_SUPPORT )
		{
			// load the support
			if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnIPStackSupportLoad(  ) ) )
				break;
		}

		// if BlueTooth support is required
		if ( pCtxt->eStackSupport & BT_STACK_SUPPORT )
		{
			// load the support
			if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnBTStackSupportLoad(  ) ) )
				break;
		}

		// if COM support is required
		if ( pCtxt->eStackSupport & COM_STACK_SUPPORT )
		{
			// load the support
			if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnCOMStackSupportLoad(  ) ) )
				break;
		}

		//******************************************************************************************
		// Initialize Framework helpers (tuple/reader/device)
		//******************************************************************************************

		// helper to initialize tuples with default fields for subsequent transfers
		cl_initTupleSetDefaultFields( TSP_LAYER_SET_LOCAL, NONE, 0, 0, 0, CL_NULL );

		// helper to initialize reader fields (name and port)
		cl_initReaderSetDefaultFields( COM_READER_TYPE, (clu8 *)"CLOVER_READER", strlen("CLOVER_READER"), (t_clIPReader_Params *)&pReader->tIPParams, (t_clCOMReader_Params*)&pReader->tCOMParams, (t_clBTReader_Params*)&pReader->tBT );

		// helper to initialize devices
		cl_initDeviceSetDefaultFields( NONE, 0xC0A8B2A3, (clu8 *)"CLOVER DEVICE", (clu32)sizeof("CLOVER DEVICE")  );

		// initialize the OTA service
		cl_initOTAService( );

		//******************************************************************************************
		// initialize 2 threads
		// 1. thread to register readers (1 thread for all CSL)
		// 2. thread to handle the access to the crypto library/resources (1 thread for all CSL)
		//******************************************************************************************

		// create signal for read thread
		status = pCtxt->ptHalFuncs->fnSemaphoreCreate( &pCtxt->g_RegReader.pSgl );
		if ( CL_FAILED(status ) )
			break;

		// start thread for reader registration
        status = pCtxt->ptHalFuncs->fnCreateThread( cl_ReaderRegistrationThread, SIZE_STACK_THREAD, CL_NULL, &pCtxt->g_RegReader.tThreadId );
		if ( CL_FAILED(status ) )
			break;

		// BRY_0804 pas besoin de la thread crypto pour l'instant
//		// create signal for this thread
//		status = pCtxt->ptHalFuncs->fnSemaphoreCreate( &pCtxt->tCrypto.pSgl );
//		if ( CL_FAILED(status ) )
//			break;
//
//		// start thread for crypto/OTA resource
//		status = pCtxt->ptHalFuncs->fnCreateThread( cl_CryptoThread, 8496, CL_NULL, &pCtxt->tCrypto.tThreadId );
//		if ( CL_FAILED(status ) )
//			break;

		// create signal for callback synchronization with user application
		status = pCtxt->ptHalFuncs->fnSemaphoreCreate( &pCtxt->tClbCtrl.pSgl );
		if ( CL_FAILED(status ) )
			break;

		// free semaphore for callback usage at initialization
		if ( pCtxt->ptHalFuncs->fnSemaphoreRelease != CL_NULL )
			pCtxt->ptHalFuncs->fnSemaphoreRelease( pCtxt->tClbCtrl.pSgl );

		// no thread creation for user application callback synchro
		pCtxt->tClbCtrl.tThreadId	=	CL_NULL;

		//*******************************************************************************************
		// After these initialization, CSL waits for a reader to be registered (via a  signal)
		// in cl_ReaderRegistrationThread
		// Then, it adds the reader to the list of registered readers, and create 2 threads
		// per reader:
		//		1. one dedicated to write operations from the application to the reader
		//		2. one dedicated to read operations from the network to the application
		//*******************************************************************************************
		DEBUG_PRINTF("clInit:Thread:END");
		break;
	}

	if (CL_FAILED( status ) )
	{
		// destroy all threads that were created
		// destroy the thread for crypto access
		if ( pCtxt->tCrypto.tThreadId != CL_NULL )
		{
			pCtxt->ptHalFuncs->fnDestroyThread( pCtxt->tCrypto.tThreadId );
			pCtxt->tCrypto.tThreadId = CL_NULL;
		}

		// destroy the thread for reader registration
		if ( pCtxt->g_RegReader.tThreadId != CL_NULL )
		{
			pCtxt->ptHalFuncs->fnDestroyThread( pCtxt->g_RegReader.tThreadId );
			pCtxt->g_RegReader.tThreadId = CL_NULL;
		}

		// destroy all signals that were created
		// destroy the signal for crypto
		if ( pCtxt->tCrypto.pSgl != CL_NULL)
		{
			pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCtxt->tCrypto.pSgl );
			pCtxt->tCrypto.pSgl = CL_NULL;
		}

		// destroy the signal for reader registration
		if ( pCtxt->g_RegReader.pSgl != CL_NULL)
		{
			pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCtxt->g_RegReader.pSgl );
			pCtxt->g_RegReader.pSgl = CL_NULL;
		}

		// if IP stack support was required
		if ( pCtxt->eStackSupport & IP_STACK_SUPPORT )
		{
			// unload the support
			pCtxt->ptHalFuncs->fnIPStackSupportUnload(  );
		}

		// if IP stack support was required - BRY BT:BlueTooth
		if ( pCtxt->eStackSupport & BT_STACK_SUPPORT )
		{
			// unload the support
			pCtxt->ptHalFuncs->fnBTStackSupportUnload(  );
		}

		// if IP stack support was required
		if ( pCtxt->eStackSupport & COM_STACK_SUPPORT )
		{
			// unload the support
			pCtxt->ptHalFuncs->fnCOMStackSupportUnload(  );
		}

		DEBUG_PRINTF("clInit FAILED");
	}

    return ( status );
}

/*--------------------------------------------------------------------------*\
 * Initilisation du "Context" sans tout le bordel li� au threading ...
\*--------------------------------------------------------------------------*/
e_Result csl_InitGlobalContext
(
	e_StackSupport eStackSupport,
	t_clSys_HalFunctions *pSysContext,
	t_clSys_CallBackFunctions *pCallBacks,
	t_clSys_CallBackFunctions *ptCallbacksDiscover,
	t_Reader* pReader,
	t_clReader_HalFuncs *p_IPReaderDefaultHAL,
	t_clReader_HalFuncs *p_IPReaderDiscoverDefaultHAL,
	t_clReader_HalFuncs	*p_COMReaderDefaultHAL,
	t_clReader_HalFuncs	*p_COMReaderDiscoverDefaultHAL,
	t_clReader_HalFuncs	*p_BTReaderDefaultHAL,
	t_clReader_HalFuncs	*p_BTReaderDiscoverDefaultHAL,
	const t_MenuFileDef *ptMenuFileDef,
	cl8 *p_FullPath
)
{
	t_clContext *pCtxt = CL_NULL;

	if ( ( pSysContext == CL_NULL ) | ( pCallBacks == CL_NULL ) | ( ptMenuFileDef == CL_NULL ) | ( pReader == CL_NULL ) )
		return ( CL_PARAMS_ERR );

	if ( (!(eStackSupport & IP_STACK_SUPPORT)) & (!(eStackSupport & COM_STACK_SUPPORT)) & (!(eStackSupport & BT_STACK_SUPPORT)) )
		return ( CL_PARAMS_ERR );

	g_tCtxt.eStackSupport			=	eStackSupport;
	g_tCtxt.ptHalFuncs 				= 	( t_clSys_HalFunctions * ) 			pSysContext;
	g_tCtxt.ptCallbacks 			=	( t_clSys_CallBackFunctions * ) 	pCallBacks;
	g_tCtxt.ptCallbacksDiscover		=	( t_clSys_CallBackFunctions * ) 	ptCallbacksDiscover;
	g_tCtxt.ptMenuFileDef			= 	( t_MenuFileDef *)					ptMenuFileDef ;
	g_tCtxt.p_COMReaderHal 			= 	( t_clReader_HalFuncs *) 			p_COMReaderDefaultHAL;
	g_tCtxt.p_IPReaderHal 			= 	( t_clReader_HalFuncs *)	 		p_IPReaderDefaultHAL;
	g_tCtxt.p_BTReaderHal 			= 	( t_clReader_HalFuncs *) 			p_BTReaderDefaultHAL;
	g_tCtxt.p_COMReaderDiscoverHal 	= 	( t_clReader_HalFuncs *) 			p_COMReaderDiscoverDefaultHAL;
	g_tCtxt.p_IPReaderDiscoverHal 	= 	( t_clReader_HalFuncs *)	 		p_IPReaderDiscoverDefaultHAL;
	g_tCtxt.p_BTReaderDiscoverHal 	= 	( t_clReader_HalFuncs *) 			p_BTReaderDiscoverDefaultHAL;
	g_tCtxt.ptCslReader				=	( t_Reader *)						pReader;

	DEBUG_PRINTF("csl_InitGlobalContext: BEGIN");

	// for crypto/OTA: this is internal resource exposed for debugging only. Not exposed when initializing the framework
	g_tCtxt.tCryptoParams.ptEncryptionKeyReader	= 	CL_NULL;
	g_tCtxt.tCryptoParams.ptTargetReader		=	CL_NULL;
	g_tCtxt.tDebugParams.u64NanoSeconds			=	0;
	g_tCtxt.tDebugParams.u64Seconds				=	0;

	// Other parameters
	if ( p_FullPath != CL_NULL )
	{
		strcpy( g_tCtxt.tConfigParams.FullPathName, p_FullPath );
	}

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED( cl_GetContext( &pCtxt )) )
	{
		DEBUG_PRINTF("csl_InitGlobalContext: cl_GetContext: CL_ERROR");
		return CL_ERROR;
	}
	else
	{
		DEBUG_PRINTF("csl_InitGlobalContext: cl_GetContext: CL_OK");
	}

	if ( pCtxt == CL_NULL)
		return CL_ERROR;

	if ( pCtxt->ptCallbacks == CL_NULL )
		return CL_ERROR;

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return CL_ERROR;

	if ( pCtxt->ptHalFuncs->fnCreateThread == CL_NULL )
		return CL_ERROR;

	if ( pCtxt->ptHalFuncs->fnDestroyThread == CL_NULL )
		return CL_ERROR;

	if ( pCtxt->ptHalFuncs->fnSemaphoreCreate == CL_NULL )
		return CL_ERROR;

	if ( pCtxt->ptHalFuncs->fnSemaphoreDestroy == CL_NULL )
		return CL_ERROR;

	// check if necessary function for IP Stack support are present if needed
	if ( pCtxt->eStackSupport & IP_STACK_SUPPORT )
	{
		DEBUG_PRINTF("IP_STACK_SUPPORT");
		if ( pCtxt->ptHalFuncs->fnIPStackSupportLoad == CL_NULL )
			return CL_ERROR;

		if ( pCtxt->ptHalFuncs->fnIPStackSupportUnload == CL_NULL )
			return CL_ERROR;
	}

	if ( pCtxt->eStackSupport & COM_STACK_SUPPORT )
	{
		DEBUG_PRINTF("COM_STACK_SUPPORT");
		if ( pCtxt->ptHalFuncs->fnCOMStackSupportLoad == CL_NULL )
			return CL_ERROR;

		if ( pCtxt->ptHalFuncs->fnCOMStackSupportUnload == CL_NULL )
			return CL_ERROR;
	}

	if ( pCtxt->eStackSupport & BT_STACK_SUPPORT )
	{
		DEBUG_PRINTF("BT_STACK_SUPPORT");
		if ( pCtxt->ptHalFuncs->fnBTStackSupportLoad == CL_NULL )
			return CL_ERROR;

		if ( pCtxt->ptHalFuncs->fnBTStackSupportUnload == CL_NULL )
			return CL_ERROR;
	}

	DEBUG_PRINTF("csl_InitGlobalContext: END");
	return CL_OK;
}

/*--------------------------------------------------------------------------*\
 * Initilisation des "Threads"
\*--------------------------------------------------------------------------*/
e_Result csl_InitThreads( void )
{
	t_clContext *pCtxt = CL_NULL;
	e_Result status = CL_ERROR;

	DEBUG_PRINTF("csl_InitThreads: BEGIN");

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED( status = cl_GetContext( &pCtxt )) )
	{
		return status;
	}
	else
	{
		DEBUG_PRINTF("csl_InitThreads: cl_GetContext: CL_OK");
	}

	// if IP stack support is required
	if ( pCtxt->eStackSupport & IP_STACK_SUPPORT )
	{
		// load the support
		if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnIPStackSupportLoad(  ) ) )
			return status;
	}

	// if BlueTooth support is required
	if ( pCtxt->eStackSupport & BT_STACK_SUPPORT )
	{
		// load the support
		if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnBTStackSupportLoad(  ) ) )
			return status;
	}

	// if COM support is required
	if ( pCtxt->eStackSupport & COM_STACK_SUPPORT )
	{
		// load the support
		if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnCOMStackSupportLoad(  ) ) )
			return status;
	}

	//******************************************************************************************
	// Initialize Framework helpers (tuple/reader/device)
	//******************************************************************************************

	// helper to initialize tuples with default fields for subsequent transfers
	cl_initTupleSetDefaultFields( TSP_LAYER_SET_LOCAL, NONE, 0, 0, 0, CL_NULL );

	// helper to initialize reader fields (name and port)
	cl_initReaderSetDefaultFields( COM_READER_TYPE, (clu8 *)"CLOVER_READER", strlen("CLOVER_READER"), (t_clIPReader_Params *)&pCtxt->ptCslReader->tIPParams,
			(t_clCOMReader_Params*)&pCtxt->ptCslReader->tCOMParams, (t_clBTReader_Params*)&pCtxt->ptCslReader->tBT );

	// helper to initialize devices
	cl_initDeviceSetDefaultFields( NONE, 0xC0A8B2A3, (clu8 *)"CLOVER DEVICE", (clu32)sizeof("CLOVER DEVICE")  );

	// initialize the OTA service
	cl_initOTAService( );

	//******************************************************************************************
	// initialize 2 threads
	// 1. thread to register readers (1 thread for all CSL)
	// 2. thread to handle the access to the crypto library/resources (1 thread for all CSL)
	//******************************************************************************************

	// create signal for read thread
	// BRY_1604 pas besoin de thread registration pour l'instant
//	status = pCtxt->ptHalFuncs->fnSemaphoreCreate( &pCtxt->g_RegReader.pSgl );
//	if ( CL_FAILED(status ) )
//		return status;
//
//	// start thread for reader registration
//	status = pCtxt->ptHalFuncs->fnCreateThread( cl_ReaderRegistrationThread, SIZE_STACK_THREAD, CL_NULL, &pCtxt->g_RegReader.tThreadId );
//	if ( CL_FAILED(status ) )
//		return status;

	// BRY_0804 pas besoin de la thread crypto pour l'instant
//		// create signal for this thread
//		status = pCtxt->ptHalFuncs->fnSemaphoreCreate( &pCtxt->tCrypto.pSgl );
//		if ( CL_FAILED(status ) )
//			break;
//
//		// start thread for crypto/OTA resource
//		status = pCtxt->ptHalFuncs->fnCreateThread( cl_CryptoThread, 8496, CL_NULL, &pCtxt->tCrypto.tThreadId );
//		if ( CL_FAILED(status ) )
//			break;

	// create signal for callback synchronization with user application
	status = pCtxt->ptHalFuncs->fnSemaphoreCreate( &pCtxt->tClbCtrl.pSgl );
	if ( CL_FAILED(status ) )
		return status;

	// free semaphore for callback usage at initialization
	if ( pCtxt->ptHalFuncs->fnSemaphoreRelease != CL_NULL )
		pCtxt->ptHalFuncs->fnSemaphoreRelease( pCtxt->tClbCtrl.pSgl );

	// no thread creation for user application callback synchro
	pCtxt->tClbCtrl.tThreadId	=	CL_NULL;

	//*******************************************************************************************
	// After these initialization, CSL waits for a reader to be registered (via a  signal)
	// in cl_ReaderRegistrationThread
	// Then, it adds the reader to the list of registered readers, and create 2 threads
	// per reader:
	//		1. one dedicated to write operations from the application to the reader
	//		2. one dedicated to read operations from the network to the application
	//*******************************************************************************************
	DEBUG_PRINTF("csl_InitThreads: END");
    return status;
}

/*******************************************************************************************/
/* Name : e_Result csl_Close( clvoid );                                                          */
/* Description :                                                                            */
/*        close the library and performs memory release/network closure...                  */
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : none                                                                               */
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*  MEM_ERR,                  :  Failure on memory management (failure,                     */
/*                                  allocation ....)                                        */
/*******************************************************************************************/
e_Result csl_Close( clvoid )
{
	t_clContext *pCtxt = CL_NULL;
	t_Reader *pReader = CL_NULL;
	t_Reader *pNextReader = CL_NULL;
	e_Result status = CL_ERROR;

	DEBUG_PRINTF("csl_Close: BEGIN");

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED( cl_GetContext( &pCtxt )) )
		return CL_ERROR;


	// destroy all threads that were created
	if (pCtxt->ptHalFuncs->fnDestroyThread != CL_NULL )
	{
		// destroy the thread for crypto access
		if ( pCtxt->tCrypto.tThreadId != CL_NULL )
		{
			pCtxt->ptHalFuncs->fnDestroyThread( pCtxt->tCrypto.tThreadId );
			pCtxt->tCrypto.tThreadId = CL_NULL;
		}

		// destroy the thread for reader registration
		if ( pCtxt->g_RegReader.tThreadId != CL_NULL )
		{
			DEBUG_PRINTF("csl_Close: fnDestroyThread: g_RegReader.tThreadId (RegistrationReader Thread)");
			pCtxt->ptHalFuncs->fnDestroyThread( pCtxt->g_RegReader.tThreadId );
			pCtxt->g_RegReader.tThreadId = CL_NULL;
		}
	}

	// destroy all signals that were created
	if ( pCtxt->ptHalFuncs->fnSemaphoreDestroy != CL_NULL)
	{
		// destroy the signal for crypto
		if ( pCtxt->tCrypto.pSgl != CL_NULL)
		{
			pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCtxt->tCrypto.pSgl );
			pCtxt->tCrypto.pSgl = CL_NULL;
		}

		// destroy the signal for reader registration
		if ( pCtxt->g_RegReader.pSgl != CL_NULL)
		{
			pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCtxt->g_RegReader.pSgl );
			pCtxt->g_RegReader.pSgl = CL_NULL;
		}

		// destroy signal for callback synchronization with user application
		if ( pCtxt->tClbCtrl.pSgl != CL_NULL)
		{
			pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCtxt->tClbCtrl.pSgl );
			pCtxt->tClbCtrl.pSgl = CL_NULL;
		}

	}

	// destroy all devices that were created


	// destroy all readers that were created
	if ( CL_SUCCESS( cl_GetReaderListEntry( &pReader ) ) )
	{
		while ( pReader != CL_NULL )
		{
			DEBUG_PRINTF("csl_Close: ReaderRemoveFromList: %s", pReader->aucLabel);

			pNextReader = ( t_Reader *) pReader->pNext;

			status = cl_ReaderRemoveFromList( pReader );
			if ( status != CL_OK )
			{
				DEBUG_PRINTF("csl_Close: ReaderRemoveFromList: ERROR");
			}
			else
			{
				DEBUG_PRINTF("csl_Close: ReaderRemoveFromList: OK");
			}

			pReader = pNextReader;
		}
	}

	//
	// stop the low layers
	//
	if ( pCtxt->eStackSupport & BT_STACK_SUPPORT )
	{
		DEBUG_PRINTF("csl_Close: BT_STACK_SUPPORT Unload");

		if (pCtxt->ptHalFuncs->fnBTStackSupportUnload )
			pCtxt->ptHalFuncs->fnBTStackSupportUnload( );
	}

	if ( pCtxt->eStackSupport & COM_STACK_SUPPORT )
	{
		DEBUG_PRINTF("csl_Close: COM_STACK_SUPPORT Unload");

		if ( pCtxt->ptHalFuncs->fnCOMStackSupportUnload )
			pCtxt->ptHalFuncs->fnCOMStackSupportUnload();
	}

	if ( pCtxt->eStackSupport & IP_STACK_SUPPORT )
	{
		DEBUG_PRINTF("csl_Close: IP_STACK_SUPPORT Unload");

		if ( pCtxt->ptHalFuncs->fnIPStackSupportUnload )
			pCtxt->ptHalFuncs->fnIPStackSupportUnload();
	}

	DEBUG_PRINTF("csl_Close: END");

	return CL_OK;
}

/*******************************************************************************************/
/* Name : e_Result cl_GetContext( t_clContext **ppCtxt )                                      */
/* Description :                                                                            */
/*        return function pointers on the CSL functions provided by the customer            */
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : t_CslContext **pCtxt                                                                               */
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*  MEM_ERR,                  :  Failure on memory management (failure,                     */
/*                                  allocation ....)                                        */
/*******************************************************************************************/
e_Result cl_GetContext( t_clContext **ppCtxt )
{
	if ( *ppCtxt != CL_NULL )
		return ( CL_PARAMS_ERR );

	*ppCtxt = &g_tCtxt;	// pointer on context

	if ( g_tCtxt.ptHalFuncs == CL_NULL )
	{
		return CL_ERROR;
	}

	return CL_OK;
}

/*******************************************************************************************/
/* Name : e_Result cl_SetReaderListEntry( t_Reader *pReader )                                */
/* Description :                                                                            */
/*        return readers list entry            												*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : t_Reader *pReader                                                                               */
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*  MEM_ERR,                  :  Failure on memory management (failure,                     */
/*                                  allocation ....)                                        */
/*******************************************************************************************/
e_Result cl_SetReaderListEntry( t_Reader *pReader )
{
	g_ptReader = pReader;	// pointer on reader entrance

	return ( CL_OK );
}

/*******************************************************************************************/
/* Name : e_Result cl_GetReaderListEntry( t_Reader *pReader )                                */
/* Description :                                                                            */
/*        return readers list entry            												*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : t_Reader *pReader                                                                               */
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*  MEM_ERR,                  :  Failure on memory management (failure,                     */
/*                                  allocation ....)                                        */
/*******************************************************************************************/
e_Result cl_GetReaderListEntry( t_Reader **ppReader )
{
	if ( g_ptReader == CL_NULL )
	{
		DEBUG_PRINTF("cl_GetReaderListEntry: g_ptReader == CL_NULL");
	}

	*ppReader = (t_Reader *)( g_ptReader );	// pointer on reader list entrance

	return CL_OK;
}


/*******************************************************************************************/
/* Name : e_Result cl_GetDeviceListEntry( t_Device **ppDevice )                             */
/* Description :                                                                            */
/*        return get Head of Devices' list            										*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : t_Device **ppDevice: pointer to first device in the list							 */
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*  MEM_ERR,                  :  Failure on memory management (failure,                     */
/*                                  allocation ....)                                        */
/*******************************************************************************************/
e_Result cl_GetDeviceListEntry( t_Device **ppDevice )
{

	*ppDevice = (t_Device *)g_ptDefaultDevice;	// pointer on device list entrance

	return ( CL_OK );
}


/*******************************************************************************************/
/* Name : e_Result cl_FreeMem( clvoid *pData )					                             */
/* Description :                                                                            */
/*        free memory from user land	            										*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : clvoid *pData : pointer to buffer to free											 */
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*  MEM_ERR,                  :  Failure on memory management (failure,                     */
/*                                  allocation ....)                                        */
/*******************************************************************************************/
e_Result cl_FreeMem( clvoid *pData )
{
	t_clContext *pCtxt = CL_NULL;

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED( cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptCallbacks == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )
		return ( CL_ERROR );

	return ( pCtxt->ptHalFuncs->fnFreeMem( pData ) );
}

/******************************************************************************/
/* Name :      e_Result cl_GetElapsedTime(  )           					*/
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
cl64 cl_GetElapsedTime( )
{
	t_clContext *pCtxt = CL_NULL;

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED( cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );

	return ( pCtxt->ptHalFuncs->fn_GetElapsedTime() );
}
