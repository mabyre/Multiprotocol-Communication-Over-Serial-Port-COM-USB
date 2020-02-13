/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name:                                                                    */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/

/*--------------------------------------------------------------------------*\
 * We are managing to lists of Tuples : pReader->p_TplList2Send
 * and pReader->p_TplList2Read. This because at the and of the Write Cycle
 * We need to be able to recapitulate all what happend to send Tuples
 * to upper layer
 * BUG_26112015 - BUG "On ne visualise plus l'ACK"
\*--------------------------------------------------------------------------*/

#include "..\csl.h"
#include "..\inc\clthread.h"
#include "..\inc\generic.h"
#include "..\inc\cltuple.h"
#include "..\inc\clreaders.h"
#include "..\inc\otaservice.h"
#include "..\inc\clhelpers.h"
#include "pthread.h" // TODO : this is not portable
#include "processthreadsapi.h" // Exit process

#define TOTO
#define TATA
#define NOT_TUTU

#include <stdio.h>
#include <ctype.h>

#define _DEBUG_PRINTF

/*--------------------------------------------------------------------------*/

pthread_mutex_t g_MutexReadIncomingWriteRead = PTHREAD_MUTEX_INITIALIZER;

/*--------------------------------------------------------------------------*/

#define READ_INCOMING

#define DEBUG_READ_INCOMING
#ifdef DEBUG_READ_INCOMING
#define DEBUG_PRINTF_READ_INCOMING DEBUG_PRINTF
#else
#define NO_DEBUG_PRINTF_READ_INCOMING
#endif

/*--------------------------------------------------------------------------*/
/* To reuse this mode see SESSION_MODE_CANCELED
 */
#define USE_MUTEX

#ifdef USE_MUTEX

#define MUTEX_INCOMING_READ_LOCK(msg) 		DEBUG_PRINTF("xxxx_MUTEX %s: %s LOCK", pReader->tCOMParams.aucPortName, msg);\
											pthread_mutex_lock(&pReader->tSync.mutexReadIncomingWR);

#define MUTEX_INCOMING_READ_UNLOCK(msg) 	DEBUG_PRINTF("xxxx_MUTEX %s: %s UNLOCK", pReader->tCOMParams.aucPortName, msg);\
											pthread_mutex_unlock(&pReader->tSync.mutexReadIncomingWR);

#else
#define MUTEX_INCOMING_READ_LOCK
#define MUTEX_INCOMING_READ_UNLOCK
#endif

/*-------------------------------------------------------------------------*/

#define NO_USE_MUTEX_WRITE

#ifdef USE_MUTEX_WRITE
#define MUTEX_WRITE_LOCK(msg) 		DEBUG_PRINTF("xxxx_MUTEX_%s: %s_LOCK", pReader->tCOMParams.aucPortName, msg);\
									pthread_mutex_lock( &pReader->tSync.mutexWriteOnPortCom );

#define MUTEX_WRITE_UNLOCK(msg) 	DEBUG_PRINTF("xxxx_MUTEX_%s: %s_UNLOCK", pReader->tCOMParams.aucPortName, msg);\
									pthread_mutex_unlock( &pReader->tSync.mutexWriteOnPortCom );
#else
#define MUTEX_WRITE_LOCK
#define MUTEX_WRITE_UNLOCK
#endif

/*--------------------------------------------------------------------------*\
 * Helper function to send ACK "06"
\*--------------------------------------------------------------------------*/
static e_Result SendDataToReader( t_Reader *pReader, clu8 *aBufferToSend, clu32 aBufferSize  )
{
	t_clContext *pCtxt = CL_NULL;
    e_Result 	status = CL_ERROR;
    t_Buffer    *pBuffForNet = CL_NULL;
	t_Tuple     *pTuple2Send = CL_NULL;

	// get context to have access to function pointers for memory/thread management on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return CL_ERROR;

	if ( pReader->eState == STATE_CONNECT )
	{
		if ( aBufferSize %2 != 0 )
			return CL_ERROR;

		clu32 dataToSendLenght = aBufferSize/2;
		clu8 dataToSend[ dataToSendLenght ];
		ConvertASCIToHex(aBufferToSend, aBufferSize, dataToSend, &dataToSendLenght);

		// Allocate buffer for data to send
		csl_pmalloc( (clvoid **)&pBuffForNet, sizeof(t_Buffer) );
		csl_pmalloc( (clvoid **)&pBuffForNet->pData, dataToSendLenght);

		// Allocate tuple which holds the data
		if ( CL_FAILED( csl_pmalloc( (clvoid **)&pTuple2Send, sizeof(t_Tuple)))) return CL_ERROR;

		// Save data into the buffer
		memcpy( pBuffForNet->pData, dataToSend, dataToSendLenght );

		// initialize a tuple default flags with memory
		if ( CL_SUCCESS( cl_initTuple( pTuple2Send, pBuffForNet, &pBuffForNet->pData, dataToSendLenght ) ) )
		{
//			//
//			// Insert new Tuple at the end of p_TplList2Send
//			//
//			t_Tuple *tuple2Send = pReader->p_TplList2Send;
//			if ( tuple2Send != CL_NULL )
//			{
//				while ( tuple2Send->pNext != CL_NULL )
//				{
//					tuple2Send = tuple2Send->pNext;
//				};
//				tuple2Send->pNext = pTuple2Send;
//			}
//			else
//			{
//				pReader->p_TplList2Send = pTuple2Send;
//			}
//
//			//
//			// Set tuple2Send on the next to send
//			//
//			tuple2Send = pReader->p_TplList2Send;
//			while ( tuple2Send->pNext != CL_NULL )
//			{
//				tuple2Send = tuple2Send->pNext;
//			};

			status = pReader->tReaderHalFuncs.fnIOSendData( pReader, pTuple2Send->ptBuf );

			//
			// BUG_26112015 - Give Tuples to upper layer
			//
			pReader->tCallBacks.fnIOSendDataDone_cb( pReader, pTuple2Send, status );

			pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pBuffForNet->pData );
			pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pBuffForNet );
			pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2Send );
		}
	}

	return status;
}

/*--------------------------------------------------------------------------*/

static bool g_TimeOut = false;

/*--------------------------------------------------------------------------*\
 * BRY 16102015 - Implementation of retry protocol
\*--------------------------------------------------------------------------*/
e_Result clReaderWriteThread( clvoid *pCallingReader )
{
	t_clContext 	*pCtxt 		= CL_NULL;
	e_Result 		status 		= CL_ERROR;
	t_Reader 		*pReader 	= CL_NULL;
	e_State			 eState;
	clu8			cancel		      = 1; // bidon
	t_Tuple			*pTuple2Send      = CL_NULL;
	static int		nbRetry           = 0;

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return CL_ERROR;

	pReader = pCallingReader;

	DEBUG_PRINTF("clReaderWriteThread_%s: BEGIN", pReader->tCOMParams.aucPortName);

	do
	{
		pCtxt->ptHalFuncs->fnDestroyThreadAsked( &cancel );

		if ( pReader == CL_NULL )
		{
			DEBUG_PRINTF("clReaderWriteThread: READER_IS_NULL");
			continue; // return waiting for cancellation
		}

		//
		// Wait for a connected reader
		//
		cl_ReaderGetState( pReader, &eState );
		if ( eState != STATE_CONNECT )
		{
			DEBUG_PRINTF1("clReaderWriteThread_%s: READER_NOT_CONNECTED", pReader->tCOMParams.aucPortName);
			pCtxt->ptHalFuncs->fnWaitMs( 1000 );

			continue; // return waiting for cancellation
		}

		// TODO: implement sem_wait in CSL
//		status = pCtxt->ptHalFuncs->fnSemaphoreWait( pReader->tSync.pSgl4Write, 5000 );
		sem_wait( pReader->tSync.pSgl4Write );

		DEBUG_PRINTF("clReaderWriteThread_%s: WRITE", pReader->tCOMParams.aucPortName);

		//
		// Send tuple to IO PortCOM
		//
		// TODO: Find why UI layer can release pSgl4Write with p_TplList2Send == NULL
		if ( pReader->p_TplList2Send == CL_NULL )
		{
			DEBUG_PRINTF("clReaderWriteThread_%s: pReader->p_TplList2Send == CL_NULL: ERROR", pReader->tCOMParams.aucPortName);
			continue; // return waiting for another pSgl4Write
		}

		pTuple2Send = pReader->p_TplList2Send;

		MUTEX_WRITE_LOCK("clReaderWriteThread");
		status = pReader->tReaderHalFuncs.fnIOSendData( pReader, pTuple2Send->ptBuf );
		MUTEX_WRITE_UNLOCK("clReaderWriteThread");
		if ( status != CL_OK )
		{
			DEBUG_PRINTF("clReaderWriteThread_%s: fnIOSendData: ERROR", pReader->tCOMParams.aucPortName);
			continue; // return waiting for another pSgl4Write
		}

		//
		// Give Tuples to upper layer
		//
		pReader->tCallBacks.fnIOSendDataDone_cb( pReader, pTuple2Send, status );

		//
		// Wait for the ReadThread to say that ACK is received
		//
		status = pCtxt->ptHalFuncs->fnSemaphoreWait( pReader->tSync.pSgl4ReadComplete, 300 );
		if ( status == CL_TIMEOUT_ERR )
		{
			DEBUG_PRINTF("clReaderWriteThread_%s: TIME_OUT", pReader->tCOMParams.aucPortName);
			if ( nbRetry <= pReader->tSync.u32Retries )
			{
				nbRetry = nbRetry + 1;
				if ( nbRetry < 3 )
				{
					DEBUG_PRINTF("clReaderWriteThread : RETRY [%d]", nbRetry );

					// Release WriteThread itself
					pCtxt->ptHalFuncs->fnSemaphoreRelease( pReader->tSync.pSgl4Write );
				}
			}
		}
		else
		{
			DEBUG_PRINTF("clReaderWriteThread_%s: WRITE_OK", pReader->tCOMParams.aucPortName);
		}

		if ( nbRetry == 3 || status == CL_OK )
		{
			DEBUG_PRINTF("clReaderWriteThread_%s: WRITE_END", pReader->tCOMParams.aucPortName);
			nbRetry = 0;

			cl_ReaderFreeListOfTuples( &(pReader->p_TplList2Send) );

			//
			// Tell upper layer that write's cycle is finished
			//
			pCtxt->ptHalFuncs->fnSemaphoreRelease( pReader->tSync.pSgl4WriteComplete );
		}

	}
	while ( 1 );
}

e_Result V0_0_clReaderWriteThread( clvoid *pCallingReader )
{
	t_clContext 	*pCtxt 		= CL_NULL;
	e_Result 		status 		= CL_ERROR;
	t_Reader 		*pReader 	= CL_NULL;
	e_State			 eState;
	clu8			cancel		      = 1; // bidon
	t_Tuple			*pLocalTupleList  = CL_NULL;
	t_Tuple			*pTuple2Send      = CL_NULL;
	int				nbRetry           = 1;

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return CL_ERROR;

	pReader = pCallingReader;

	DEBUG_PRINTF("clReaderWriteThread_%s: BEGIN", pReader->tCOMParams.aucPortName);

	do
	{
		pCtxt->ptHalFuncs->fnDestroyThreadAsked( &cancel );

		if ( pReader == CL_NULL )
		{
			DEBUG_PRINTF("clReaderWriteThread: READER_IS_NULL");
			continue; // return waiting for cancellation
		}

		//
		// Wait for a connected reader
		//
		cl_ReaderGetState( pReader, &eState );
		if ( eState != STATE_CONNECT )
		{
			DEBUG_PRINTF1("clReaderWriteThread_%s: READER_NOT_CONNECTED", pReader->tCOMParams.aucPortName);
			pCtxt->ptHalFuncs->fnWaitMs( 1000 );

			continue; // return waiting for cancellation
		}

		// TODO: implement sem_wait in CSL
//		status = pCtxt->ptHalFuncs->fnSemaphoreWait( pReader->tSync.pSgl4Write, 5000 );
		sem_wait( pReader->tSync.pSgl4Write );

		DEBUG_PRINTF("clReaderWriteThread_%s: WRITE", pReader->tCOMParams.aucPortName);

		//
		// Send tuple to IO PortCOM
		//
		// TODO: Find why UI layer can release pSgl4Write with p_TplList2Send == NULL
		if ( pReader->p_TplList2Send == CL_NULL )
		{
			continue; // return waiting for another pSgl4Write
		}

		pLocalTupleList	= pReader->p_TplList2Send;

		MUTEX_WRITE_LOCK("clReaderWriteThread");
		status = pReader->tReaderHalFuncs.fnIOSendData( pReader, pLocalTupleList->ptBuf );
		MUTEX_WRITE_UNLOCK("clReaderWriteThread");

		//
		// Give Tuples to upper layer
		//
		pTuple2Send = pReader->p_TplList2Send;
		pReader->tCallBacks.fnIOSendDataDone_cb( pReader, pTuple2Send, status );

		//
		// Wait for the ReadThread to say that ACK is received
		//
		status = pCtxt->ptHalFuncs->fnSemaphoreWait( pReader->tSync.pSgl4ReadComplete, 300 );
		if ( status == CL_TIMEOUT_ERR )
		{
			DEBUG_PRINTF("clReaderWriteThread_%s: TIME_OUT", pReader->tCOMParams.aucPortName);
			if ( nbRetry <= pReader->tSync.u32Retries )
			{
				DEBUG_PRINTF("clReaderWriteThread : RETRY [%d]", nbRetry);

				// Release WriteThread itself
				pCtxt->ptHalFuncs->fnSemaphoreRelease( pReader->tSync.pSgl4Write );

				nbRetry = nbRetry + 1;
			}

		}
		else
		{
			DEBUG_PRINTF("clReaderWriteThread_%s: WRITE_OK", pReader->tCOMParams.aucPortName);
		}

		if ( nbRetry == 4 || status == CL_OK )
		{
			DEBUG_PRINTF("clReaderWriteThread_%s: WRITE_END", pReader->tCOMParams.aucPortName);
			nbRetry = 1;

			cl_ReaderFreeListOfTuples( &(pReader->p_TplList2Send) );

			//
			// Tell upper layer that write's cycle is finished
			//
			pCtxt->ptHalFuncs->fnSemaphoreRelease( pReader->tSync.pSgl4WriteComplete );
		}

	}
	while ( 1 );
}

e_Result OLD_OLD_clReaderWriteThread( clvoid *pCallingReader )
{
	t_clContext 	*pCtxt 		= CL_NULL;
	e_Result 		status 		= CL_ERROR;
	t_Reader 		*pReader 	= CL_NULL;
	e_State			 eState;
	clu8			cancel		= 1; // bidon
	t_Tuple			*pLocalTupleList	= CL_NULL;
	t_Tuple			*pTuple2Send = CL_NULL;

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return CL_ERROR;

	pReader = pCallingReader;

	DEBUG_PRINTF("clReaderWriteThread: DoBEGIN");

	do
	{
		pCtxt->ptHalFuncs->fnDestroyThreadAsked( &cancel );

		if ( pReader == CL_NULL )
		{
			DEBUG_PRINTF("clReaderWriteThread: READER_IS_NULL");
			continue; // return waiting for cancellation
		}

		//
		// Wait for a connected reader
		//
		cl_ReaderGetState( pReader, &eState );
		if ( eState != STATE_CONNECT )
		{
			DEBUG_PRINTF1("clReaderWriteThread_%s: READER_NOT_CONNECTED", pReader->tCOMParams.aucPortName);
			pCtxt->ptHalFuncs->fnWaitMs( 1000 );

			continue; // return waiting for cancellation
		}

//		status = pCtxt->ptHalFuncs->fnSemaphoreWait( pReader->tSync.pSgl4Write, 5000 );
		sem_wait( pReader->tSync.pSgl4Write ); // TODO: implement sem_wait in CSL

		DEBUG_PRINTF("clReaderWriteThread_%s: WRITE", pReader->tCOMParams.aucPortName);

		//
		// Send tuple to IO PortCOM
		//
		// TODO: Find why UI layer can release pSgl4Write with p_TplList2Send == NULL
		if ( pReader->p_TplList2Send == CL_NULL )
		{
			continue; // return waiting for another pSgl4Write
		}
		pLocalTupleList	= pReader->p_TplList2Send;
		status = pReader->tReaderHalFuncs.fnIOSendData( pReader, pLocalTupleList->ptBuf );

		//
		// Give Tuples to upper layer and free memory
		//
		pTuple2Send = pReader->p_TplList2Send;
		while ( pTuple2Send != NULL )
		{
			pReader->tCallBacks.fnIOSendDataDone_cb( pReader, pTuple2Send, status );

			t_Tuple *pTupleNext = (t_Tuple *)pTuple2Send->pNext;
			pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2Send->ptBuf->pData );
			pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2Send->ptBuf );
			pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2Send );
			pTuple2Send = pTupleNext;
		}
		pReader->p_TplList2Send = CL_NULL;

		//
		// Tell to the ReadThread that it can Read Tuples
		//
		pCtxt->ptHalFuncs->fnSemaphoreRelease(pReader->tSync.pSgl4Read);

		//
		// Wait for the ReadThread to End
		//
		status = pCtxt->ptHalFuncs->fnSemaphoreWait( pReader->tSync.pSgl4ReadComplete, 3000 );
		if ( status == CL_TIMEOUT_ERR )
		{
			DEBUG_PRINTF("clReaderWriteThread_%s: WRITE_COMPLETE: ERROR", pReader->tCOMParams.aucPortName);
		}
		else
		{
			DEBUG_PRINTF("clReaderWriteThread_%s: WRITE_COMPLETE: OK", pReader->tCOMParams.aucPortName);
		}

		//
		// Tell upper layer that cycle is finished
		//
		pCtxt->ptHalFuncs->fnSemaphoreRelease( pReader->tSync.pSgl4WriteComplete );
	}
	while ( 1 );
}

/*--------------------------------------------------------------------------*/

e_Result cl_ReaderReadThreadTraitement( clvoid *pCallingReader, t_Tuple **ppTuple )
{
	t_clContext 	*pCtxt           = CL_NULL;
	e_Result 		 status_cl       = CL_ERROR;
	t_Reader        *pReader		 = CL_NULL;
	t_Tuple			*pTuple2Read     = CL_NULL;
    clu32 			 nbTupleIncoming = 0;
    clu8 			 bufferACK[2]    = "06";

	if ( CL_FAILED( status_cl = cl_GetContext( &pCtxt ) ) )
		return CL_ERROR;

	pReader = pCallingReader;

	DEBUG_PRINTF("cl_ReaderReadThreadTraitement_%s: BEGIN", pReader->tCOMParams.aucPortName);

	pTuple2Read = pReader->p_TplList2Read;
	while ( pTuple2Read != NULL )
	{
		// Count how many tuples are received
		nbTupleIncoming += 1;

		//
		// Give Tuples to upper layer
		//
		pReader->tCallBacks.fnIOData2Read_cb( pReader, pTuple2Read, CL_OK );

		//
		// Ctrl have a length of 1 do not SendACK
		// otherwize send ACK
		//
		if ( pTuple2Read->ptBuf->ulLen > 1 )
		{
			DEBUG_PRINTF("cl_ReaderReadThreadTraitement_%s: INCOMING_FRAME", pReader->tCOMParams.aucPortName );

			SendDataToReader( pReader, bufferACK, 2 );
		}

		// Receiving an ACK
		if ( ( pTuple2Read->ptBuf->ulLen == 1 ) && ( pTuple2Read->ptBuf->pData[0] ==  0x06 ) )
		{
			DEBUG_PRINTF("cl_ReaderReadThreadTraitement_%s: ACK_RECEIVED", pReader->tCOMParams.aucPortName );

			pCtxt->ptHalFuncs->fnSemaphoreRelease( pReader->tSync.pSgl4ReadComplete );
		}

		pTuple2Read = pTuple2Read->pNext;
	}

	DEBUG_PRINTF("cl_ReaderReadThreadTraitement_%s: Incoming [%d]", pReader->tCOMParams.aucPortName, nbTupleIncoming);

	cl_ReaderFreeListOfTuples( ppTuple );

	DEBUG_PRINTF("cl_ReaderReadThreadTraitement_%s: END", pReader->tCOMParams.aucPortName);
}

/*--------------------------------------------------------------------------*/

e_Result TEST_cl_ReaderReadThreadTraitement( clvoid *pCallingReader, t_Tuple **ppTuple )
{
	t_clContext 	*pCtxt 				= CL_NULL;
	e_Result 		 status_cl 			= CL_ERROR;
	t_Reader        *pReader			= CL_NULL;
	t_Tuple			*pTuple2Read = CL_NULL;
    clu32 			 nbTupleIncoming  = 0;
    clu8 			 bufferACK[2] = "06";

	if ( CL_FAILED( status_cl = cl_GetContext( &pCtxt ) ) )
		return CL_ERROR;

	pReader = pCallingReader;

	DEBUG_PRINTF("cl_ReaderReadThreadTraitement_%s: BEGIN", pReader->tCOMParams.aucPortName);

	cl_ReaderFreeListOfTuples( ppTuple );

	DEBUG_PRINTF("cl_ReaderReadThreadTraitement_%s: END", pReader->tCOMParams.aucPortName);
}

/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*\
 * Write Thread in mode Session
\*--------------------------------------------------------------------------*/
e_Result SESSION_MODE_CANCELED_clReaderWriteThread( clvoid *pCallingReader )
{
	t_clContext 	*pCtxt 		= CL_NULL;
	e_Result 		status 		= CL_ERROR;
	t_Reader 		*pReader 	= CL_NULL;
	clu8			cancel		= 1; // bidon
	t_Tuple			*pLocalTupleList	= CL_NULL;
	t_Tuple			*pTuple2Send = CL_NULL;
	t_Tuple			*pTuple2Read = CL_NULL;
	e_SERIAL_RetryProtocolDef eSynchroStatus = CL_SERIAL_PROT_DEFAULT;

	clu8 			bufferACK[2] = "06";

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return CL_ERROR;

	pReader = pCallingReader;

	DEBUG_PRINTF("clReaderWriteThread: DoBEGIN");

	do
	{
		pCtxt->ptHalFuncs->fnDestroyThreadAsked( &cancel );

		status = pCtxt->ptHalFuncs->fnSemaphoreWait( pReader->tSync.pSgl4Write, 5000 );
		if ( status == CL_TIMEOUT_ERR )
		{
			DEBUG_PRINTF("clReaderWriteThread: %s Waiting ...", pReader->aucLabel);
			continue;
		}

		cl_ReaderGetSyncStatus("W", pReader, &eSynchroStatus );
		switch ( eSynchroStatus )
		{
			case CL_SERIAL_PROT_DEFAULT : // new data to send => initialize state machine

				if ( g_TimeOut == false )
				{
					MUTEX_INCOMING_READ_LOCK("clReaderWriteThread");

					DEBUG_PRINTF("clReaderWriteThread: IO_SEND_DATA");

					cl_ReaderSetSyncStatus( "W", pReader, CL_SERIAL_PROT_W_WAIT_FOR_ACK );

					pLocalTupleList	= pReader->p_TplList2Send;
					status = pReader->tReaderHalFuncs.fnIOSendData( pReader, pLocalTupleList->ptBuf );

					pCtxt->ptHalFuncs->fnSemaphoreRelease(pReader->tSync.pSgl4Read);
				}
				else
				{
					DEBUG_PRINTF("clReaderWriteThread: WRITE_TIMEOUT");

					g_TimeOut = false; // Reset Timeout given by ReadThread

					//
					// Because free could not have beeing done at the end of
					// write cycle
					//
					pTuple2Send = pReader->p_TplList2Send;
					while ( pTuple2Send != NULL )
					{
						t_Tuple *pTupleNext = (t_Tuple *)pTuple2Send->pNext;
						pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&pTuple2Send->ptBuf->pData );
						pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&pTuple2Send->ptBuf );
						pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&pTuple2Send );
						pTuple2Send = pTupleNext;
					}

					MUTEX_INCOMING_READ_UNLOCK("clReaderWriteThread");
				}

				break;

			case CL_SERIAL_PROT_R_RECEIVED_ACK : // 0x06 was received from read thread

				cl_ReaderSetSyncStatus( "W", pReader, CL_SERIAL_PROT_W_WAIT_FOR_ANSW );
				pCtxt->ptHalFuncs->fnSemaphoreRelease(pReader->tSync.pSgl4Read);

				break;

			case CL_SERIAL_PROT_R_WRITE_ACK_REQUIRED :

					SendDataToReader(pReader, bufferACK, 2);

					cl_ReaderSetSyncStatus( "W", pReader, CL_SERIAL_PROT_R_END_GRANTED );
					pCtxt->ptHalFuncs->fnSemaphoreRelease(pReader->tSync.pSgl4Read);

				break;

			case CL_SERIAL_PROT_W_END_DONE : // Machine state cycle is finished, do the cleaning

				if ( pReader->p_TplList2Send == CL_NULL )
				{
					DEBUG_PRINTF("clReaderWriteThread: p_TplList2Send == NULL");
				}
				else
				{
					//
					// Give Tuples to upper layer, free all memory allocated by Write Cycle
					//
					pTuple2Send = pReader->p_TplList2Send;
					while ( pTuple2Send != NULL )
					{
						pReader->tCallBacks.fnIOSendDataDone_cb( pReader, pTuple2Send, status );

						t_Tuple *pTupleNext = (t_Tuple *)pTuple2Send->pNext;
						pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2Send->ptBuf->pData );
						pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2Send->ptBuf );
						pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2Send );
						pTuple2Send = pTupleNext;
					}
					pReader->p_TplList2Send = CL_NULL;

					pTuple2Read = pReader->p_TplList2Read;
					while ( pTuple2Read != NULL )
					{
						pReader->tCallBacks.fnIOData2Read_cb( pReader, pTuple2Read, status );

						t_Tuple *pTupleNext = (t_Tuple *)pTuple2Read->pNext;
						pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2Read->ptBuf->pData );
						pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2Read->ptBuf );
						pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2Read );
						pTuple2Read = pTupleNext;
					}
					pReader->p_TplList2Read = CL_NULL;
				}

				//
				// Reset ReadThread
				//
				cl_ReaderSetSyncStatus("W", pReader, CL_SERIAL_PROT_DEFAULT );
				pCtxt->ptHalFuncs->fnSemaphoreRelease( pReader->tSync.pSgl4Read );

				//
				// Tell upper layer than a new write can begin
				//
				DEBUG_PRINTF("clReaderWriteThread: fnSemaphoreRelease( pReader_tSync_pSgl4WriteComplete )");
				pCtxt->ptHalFuncs->fnSemaphoreRelease( pReader->tSync.pSgl4WriteComplete );

				MUTEX_INCOMING_READ_UNLOCK("clReaderWriteThread");

				break;

			default :

				cl_ReaderSetSyncStatus("W", pReader, CL_SERIAL_PROT_DEFAULT );
				MUTEX_INCOMING_READ_UNLOCK("clReaderWriteThread");

				break;
		}
	}
	while ( 1 );
}

/*--------------------------------------------------------------------------*\
 * Problem : the case "ack is not received" is not manage
\*--------------------------------------------------------------------------*/

e_Result SESSION_MODE_CANCELED_clReaderReadThread( clvoid *pCallingReader )
{
	t_clContext 	*pCtxt 				= CL_NULL;
	e_Result 		 status_cl 			= CL_ERROR;
	e_Result 		 status_read 		= CL_ERROR;
	e_Result 		 status_incoming 	= CL_ERROR;
	t_Reader 		*pReader = CL_NULL;
	e_State			 eState;
	clu8			 cancel	=	1;
	e_SERIAL_RetryProtocolDef eSynchroStatus	=	CL_SERIAL_PROT_DEFAULT;
	t_Tuple			*pTuple2Read = CL_NULL;
    clu32 			nbTupleReceived  = 0;
	t_Tuple			*pTuple2ReadIncoming = CL_NULL;
    clu32 			nbTupleIncoming  = 0;

    clu8 			bufferACK[2] = "06";

	if ( CL_FAILED( status_cl = cl_GetContext( &pCtxt ) ) )
		return CL_ERROR;

	pReader = pCallingReader;

	DEBUG_PRINTF("clReaderReadThread: DoBEGIN");

	do
	{
		pCtxt->ptHalFuncs->fnDestroyThreadAsked( &cancel );

		if ( pReader == CL_NULL )
		{
			DEBUG_PRINTF("clReaderReadThread: Destroyed_Reader_IS_NULL");
			continue; // return waiting for cancellation
		}

		//
		// Wait for a connected reader
		// TODO!!!! do not enter read/write status if not connected. Put semaphore in place!!!
		//
		cl_ReaderGetState( pReader, &eState );
		if ( eState != STATE_CONNECT )
		{
			DEBUG_PRINTF("clReaderReadThread_%s Reader: NOT CONNECTED", pReader->tCOMParams.aucPortName);
			pCtxt->ptHalFuncs->fnWaitMs( 1000 );

			continue; // return waiting for cancellation
		}

		//
		// Wait for signal to read otherwise read incoming
		//
		status_incoming = pCtxt->ptHalFuncs->fnSemaphoreWait( pReader->tSync.pSgl4Read, 300 ); // 350 // 500 // 1000 / 5000 if nothing to be read
		if ( status_incoming == CL_TIMEOUT_ERR )
		{

#ifdef READ_INCOMING

			//
			// Do not ReadIncoming, if state is not CL_SERIAL_PROT_DEFAULT
			//
			cl_ReaderGetSyncStatus("ReadIncoming", pReader, &eSynchroStatus );
			if ( eSynchroStatus != CL_SERIAL_PROT_DEFAULT )
			{
				continue;
			}

			MUTEX_INCOMING_READ_LOCK("ReadIncoming");

			nbTupleIncoming = 0;

			//
			// There is no Write Request, so lets try to read something incoming
			//
			DEBUG_PRINTF_READ_INCOMING("clReaderReadThread_%s Read_Incoming", pReader->tCOMParams.aucPortName);

			status_incoming = pReader->tReaderHalFuncs.fnIOGetData( pReader,  &pReader->p_TplList2Read );
			if ( status_incoming == CL_OK )
			{
				DEBUG_PRINTF("clReaderReadThread_%s: INCOMING_OK: %X", pReader->tCOMParams.aucPortName, pReader->p_TplList2Read);

				pTuple2ReadIncoming = pReader->p_TplList2Read;
				while ( pTuple2ReadIncoming != NULL )
				{
					// Count how many tuples are received
					nbTupleIncoming += 1;
					pTuple2ReadIncoming = pTuple2ReadIncoming->pNext;
				}

				// Send an ack to a received frame
				if ( nbTupleIncoming >= 1 )
				{
					DEBUG_PRINTF("clReaderReadThread_%s: INCOMING_SEND_ACK", pReader->tCOMParams.aucPortName);
					SendDataToReader( pReader, bufferACK, 2 );
				}

				DEBUG_PRINTF_READ_INCOMING("clReaderReadThread_%s: IOGetData: Incoming [%d]", pReader->tCOMParams.aucPortName, nbTupleIncoming);

				//
				// Give Tuples to upper layer
				//
				pTuple2ReadIncoming = pReader->p_TplList2Read;
				while ( pTuple2ReadIncoming != NULL )
				{
					pReader->tCallBacks.fnIOData2Read_cb( pReader, pTuple2ReadIncoming, status_incoming );

					t_Tuple *pTupleNext = (t_Tuple *)pTuple2ReadIncoming->pNext;
					pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2ReadIncoming->ptBuf->pData );
					pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2ReadIncoming->ptBuf );
					pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2ReadIncoming );
					pTuple2ReadIncoming = pTupleNext;
				}
				pReader->p_TplList2Read = CL_NULL;

				//
				// Free the Ack
				//
				t_Tuple *pTuple2Send = pReader->p_TplList2Send;
				while ( pTuple2Send != NULL )
				{
					pReader->tCallBacks.fnIOSendDataDone_cb( pReader, pTuple2Send, status_incoming );

					t_Tuple *pTupleNext = (t_Tuple *)pTuple2Send->pNext;
					pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&pTuple2Send->ptBuf->pData );
					pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&pTuple2Send->ptBuf );
					pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&pTuple2Send );
					pTuple2Send = pTupleNext;
				}
				pReader->p_TplList2Send = CL_NULL;
			}

			if ( status_incoming == CL_TIMEOUT_ERR )
			{
				DEBUG_PRINTF("clReaderReadThread_%s: INCOMING_TIMEOUT", pReader->tCOMParams.aucPortName);

//				pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pReader->p_TplList2Read->ptBuf->pData );
//				pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pReader->p_TplList2Read->ptBuf );
//				pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pReader->p_TplList2Read );
			}

			if ( status_incoming == CL_MEM_ERR )
			{
				DEBUG_PRINTF("clReaderReadThread_%s: INCOMING_MEMORY_ERROR", pReader->tCOMParams.aucPortName);
			}

			if ( status_incoming == CL_ERROR_IN_COMM )
			{
				DEBUG_PRINTF("clReaderReadThread_%s: INCOMING_ERROR_IN_COMM", pReader->tCOMParams.aucPortName);
			}

			MUTEX_INCOMING_READ_UNLOCK("ReadIncoming");

#endif

			continue; // return to waiting for pSgl4Read
		}

		cl_ReaderGetSyncStatus( "R", pReader, &eSynchroStatus );
		switch ( eSynchroStatus )
		{
			case CL_SERIAL_PROT_DEFAULT : // Initiale state machine

				nbTupleReceived = 0;

				DEBUG_PRINTF("clReaderReadThread: %s Waiting_For_New_Read ...", pReader->aucLabel);

				break;

			case CL_SERIAL_PROT_W_WAIT_FOR_ACK : // WriteThread is waiting for ReadThread to receive 0x06

				status_read = pReader->tReaderHalFuncs.fnIOGetData( pReader, &pReader->p_TplList2Read );
				if ( status_read == CL_OK )
				{
					DEBUG_PRINTF("clReaderReadThread_%s: IO_GET_DATA_OK", pReader->tCOMParams.aucPortName);

					pTuple2Read = pReader->p_TplList2Read;
					while ( pTuple2Read != NULL )
					{
						// Count how many tuples are received
						nbTupleReceived += 1;
						pTuple2Read = pTuple2Read->pNext;
					}

					g_TimeOut = false;

					cl_ReaderSetSyncStatus( "R", pReader, CL_SERIAL_PROT_R_RECEIVED_ACK );
				}

				if ( status_read == CL_TIMEOUT_ERR )
				{
					DEBUG_PRINTF("clReaderReadThread_%s: IO_GET_DATA_TIMEOUT", pReader->tCOMParams.aucPortName);

					g_TimeOut = true;

					cl_ReaderSetSyncStatus( "R", pReader, CL_SERIAL_PROT_DEFAULT );
				}

				pCtxt->ptHalFuncs->fnSemaphoreRelease(pReader->tSync.pSgl4Write);

				break;

			case CL_SERIAL_PROT_W_WAIT_FOR_ANSW :

				DEBUG_PRINTF("clReaderReadThread_%s: nbTupleReceived: %d", pReader->tCOMParams.aucPortName, nbTupleReceived );

				if ( nbTupleReceived == 0 )
				{
					DEBUG_PRINTF("clReaderReadThread_%s: NOT_RECEIVING: ERROR", pReader->tCOMParams.aucPortName);
					status_read = pCtxt->ptHalFuncs->fnSemaphoreWait( pReader->tSync.pSgl4ReadComplete, 3000 );
					if ( status_read == CL_TIMEOUT_ERR )
					{
						DEBUG_PRINTF("clReaderReadThread: NOT_RECEIVING: TIMEOUT");

						cl_ReaderSetSyncStatus( "R", pReader, CL_SERIAL_PROT_W_END_DONE );
						pCtxt->ptHalFuncs->fnSemaphoreRelease(pReader->tSync.pSgl4Write);
					}
				}

				if ( nbTupleReceived == 1 ) // We received ack now try to receive the answer
				{
					status_read = pReader->tReaderHalFuncs.fnIOGetData( pReader, &pReader->p_TplList2Read );
					if ( status_read == CL_OK )
					{
						DEBUG_PRINTF("clReaderReadThread_%s: IOGetData_OK", pReader->tCOMParams.aucPortName);

						pTuple2Read = pReader->p_TplList2Read;
						while ( pTuple2Read != NULL )
						{
							// Count how many tuples are received
							nbTupleReceived += 1;
							pTuple2Read = pTuple2Read->pNext;
						}

						g_TimeOut = false;

						cl_ReaderSetSyncStatus( "R", pReader, CL_SERIAL_PROT_W_WAIT_FOR_ANSW );
						pCtxt->ptHalFuncs->fnSemaphoreRelease(pReader->tSync.pSgl4Read); // auto_wake_up
					}

					if ( status_read == CL_TIMEOUT_ERR )
					{
						DEBUG_PRINTF("clReaderReadThread_%s: NOT_COMPLETE: ERROR", pReader->tCOMParams.aucPortName);

						cl_ReaderSetSyncStatus( "R", pReader, CL_SERIAL_PROT_R_END_GRANTED );
						pCtxt->ptHalFuncs->fnSemaphoreRelease(pReader->tSync.pSgl4Read); // auto_wake_up
					}
				}

				if ( nbTupleReceived > 1 ) // We received ack and answers
				{
					//
					// Ask WriteThread to Write ACK
					//
					cl_ReaderSetSyncStatus("R", pReader, CL_SERIAL_PROT_R_WRITE_ACK_REQUIRED );
					pCtxt->ptHalFuncs->fnSemaphoreRelease(pReader->tSync.pSgl4Write);
				}

				break;

			case CL_SERIAL_PROT_R_END_GRANTED : // Machine state cycle is finished, ask WriteThread to do the cleaning

				cl_ReaderSetSyncStatus( "R", pReader, CL_SERIAL_PROT_W_END_DONE );
				pCtxt->ptHalFuncs->fnSemaphoreRelease(pReader->tSync.pSgl4Write);

				break;

			default : // What to do ? Return in state default !

				DEBUG_PRINTF("clReaderReadThread_%s: ERROR: cl_ReaderSetSyncStatus: CL_SERIAL_PROT_DEFAULT", pReader->tCOMParams.aucPortName);
				cl_ReaderSetSyncStatus( "R", pReader, CL_SERIAL_PROT_DEFAULT );

				break;
		}

	}
	while ( 1 );
}

/**************************************************************************/
/* Name : e_Result clReaderWriteThread( clvoid *pCallingReader );             			*/
/* Description :                                                        */
/*        Thread which handles write operation from application to low-layers	*/
/*		  on failure, the thread auto-destroy itself					*/
/**************************************************************************/
/* Parameters:                                                            */
/*  --------------                                                        */
/*  In: 	void *pCallingReader : pointer on reader struct this thread is 	*/
/*				associated to											  */
/* ---------------                                                        */
/*  Out: none                                                             */
/* Return value: e_Result                                                 */
/*  CL_OK                        :  Result is OK                          */
/**************************************************************************/


/********************************************************************************/
/* Name : e_Result clReaderReadThread( clvoid );             					*/
/* Description :                                                        		*/
/*        Thread which handles read operation from application to low-layers	*/
/*		  on failure, the thread auto-destroy itself							*/
/**************************************************************************/
/* Parameters:                                                            */
/*  --------------                                                        */
/*  In: 	none			    										*/
/* ---------------                                                        */
/*  Out: none                                                             */
/* Return value: e_Result                                                 */
/*  CL_OK                        :  Result is OK                          */
/**************************************************************************/
/********************************************************************************/
/* Name : e_Result clCryptoThread( clvoid );             					*/
/* Description :                                                        		*/
/*        Thread which handles access to crypto resource (one per CSL shared )	*/
/*		among readers 															*/
/*		  on failure, the thread auto-destroy itself							*/
/**************************************************************************/
/* Parameters:                                                            */
/*  --------------                                                        */
/*  In: 	none			    										*/
/* ---------------                                                        */
/*  Out: none                                                             */
/* Return value: e_Result                                                 */
/*  CL_OK                        :  Result is OK                          */
/**************************************************************************/
e_Result clCryptoThread( clvoid )
{

	t_clContext *pCtxt = CL_NULL;
	e_Result status = CL_ERROR;
//	pthread_mutex_t crypto_mutex = PTHREAD_MUTEX_INITIALIZER;
//	pthread_cond_t crypto_got_request = PTHREAD_COND_INITIALIZER;

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );

	/* */
	for ( ;; )
	{

#if 0
		pCtxt->ptHalFuncs->fnSemaphoreWait( );

		/* first, lock crypto mutex */
		int rc = pthread_mutex_lock( &crypto_mutex );
		if (rc) { /* an error has occurred */
//		    perror("pthread_mutex_lock");
		    pthread_exit( CL_NULL );
		}
		// mutex is now locked - wait on condition variable (data to treat)  */
		/* During the execution of pthread_cond_wait, the mutex is unlocked. */
		rc = pthread_cond_wait( &crypto_got_request, &crypto_mutex);
		if (rc == 0) { /* we were awakened due to the cond. variable being signaled */
		               /* The mutex is now locked again by pthread_cond_wait()      */
		    /* do your stuff... */

		}
		// re-initialize condition for further use
		pthread_cond_init( &crypto_got_request, CL_NULL );
		// finally, unlock the mutex and loop again
		pthread_mutex_unlock( &crypto_mutex );
#endif

	}

	pCtxt->ptHalFuncs->fnDestroyThread( CL_NULL );

	return ( CL_OK );

}

/********************************************************************************/
/* Name : e_Result clRegisterThread( clvoid *fnThreadId, clu64 u64ThreadId ); 	*/
/* Description :                                                        		*/
/*        Register a thread in a thread list with its id to ease identification	*/
/*		and closure 															*/
/**************************************************************************/
/* Parameters:                                                            */
/*  --------------                                                        */
/*  In: 	none			    										*/
/* ---------------                                                        */
/*  Out: none                                                             */
/* Return value: e_Result                                                 */
/*  CL_OK                        :  Result is OK                          */
/**************************************************************************/
e_Result clRegisterThread( clvoid *fnThreadId, clu64 u64ThreadId )
{


	return ( CL_OK );
}

/********************************************************************************/
/* Name : e_Result clRegisteredThread(  clu64 u64ThreadId, clvoid *pu64ThreadId); */
/* Description :                                                        		*/
/*      Get the function pointer of the thread to allow its unregistration		*/
/*		  on failure, the thread auto-destroy itself							*/
/********************************************************************************/
/* Parameters:                                                            		*/
/*  --------------                                                        		*/
/*  In: 	none			    												*/
/* ---------------                                                        		*/
/*  Out: none                                                             		*/
/* Return value: e_Result                                                 		*/
/*  CL_OK                        :  Result is OK                          		*/
/********************************************************************************/
e_Result clRegisteredThread( clu64 u64ThreadId, clvoid *pu64ThreadId )
{

	return ( CL_OK );
}

/********************************************************************************/
/* Name : e_Result clCryptoThread( clvoid );             					*/
/* Description :                                                        		*/
/*        Thread which handles access to crypto resource (one per CSL shared )	*/
/*		among readers 															*/
/*		  on failure, the thread auto-destroy itself							*/
/**************************************************************************/
/* Parameters:                                                            */
/*  --------------                                                        */
/*  In: 	none			    										*/
/* ---------------                                                        */
/*  Out: none                                                             */
/* Return value: e_Result                                                 */
/*  CL_OK                        :  Result is OK                          */
/**************************************************************************/
e_Result clUnregisterThread(  clvoid *fnThreadId, clu64 u64ThreadId )
{

	return ( CL_OK );
}
