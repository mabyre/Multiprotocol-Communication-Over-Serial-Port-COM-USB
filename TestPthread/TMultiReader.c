/*--------------------------------------------------------------------------*\
 * Copyright (c) 1998-2015 SoDevLog. Written by Bruno Raby.
 *--------------------------------------------------------------------------*
 * TMultiReader.c - Tester le Multireaders
\*--------------------------------------------------------------------------*/

/*
 * Instancier 2 readers et communiquer entre eux
 * avec la trame radio "CAFE"
 *
 */

#include "cMenu.h"
#include "pmInterface.h"
#include "pmEnv.h"
#include "pmTrace.h"
#include "cInput.h"
#include "clreaders.h"
#include "csl.h"
#include "csl_component.h"

/*--------------------------------------------------------------------------*/

#define WAITING_READ_COMPLETE_TIMEOUT 3000 // 4000

/*--------------------------------------------------------------------------*/

int stat_DoSend010101 = 0;
int stat_DoSendCMDThroughReaderToOther = 0;
int stat_DoSetRadioParamsToReaders = 0;
int stat_DoSendCAFEThroughReaderToOther = 0;
int stat_DoStressMonoReadWrite = 0;
int stat_DoStressReadWrite = 0;

/*--------------------------------------------------------------------------*/

static bool IsClosed = false; // indicate CSL have been closed by user

/*--------------------------------------------------------------------------*/

extern char FullExeFileNameWithDirectory[1024];

/*--------------------------------------------------------------------------*\
 * GLOBAL VARIABLES
\*--------------------------------------------------------------------------*/

static char g_friendlyNames[2][64] = { "00 00 00 00", "00 00 00 00" }; // reader's list
static int g_readerIndex = 0;
static int g_NbReaders = 0;

/*---------------------------------------------------------------------------*/

bool IsXDigit(char *aString)
{
	int i = 0;

	for (i = 0; i < strlen(aString); i++)
	{
		if ( isxdigit( aString[i] ) == false )
		{
			return false;
		}
	}

	return true;
}

/*---------------------------------------------------------------------------*/

static void DoInitGlobalContext(void)
{
    e_Result status = CL_ERROR;
    char *test;

    pm_trace0("DoInitGlobalContext: BEGIN");
    cl_GetElapsedTimeHighPerformanceWin32();

    //------------------------------
    // Settings of User's functions
    //------------------------------

    g_tHalFunc.fnTrace0 = (void *)pm_trace0; // (void *): to avoid warning message
    g_tHalFunc.fnTrace1 = (void *)pm_trace1; // (void *): to avoid warning message

    g_tHalFunc.fnAllocMem = csl_AllocDebug;
    g_tHalFunc.fnFreeMem = csl_FreeDebug;
    g_tHalFunc.fnFreeMemSafely = csl_FreeSafeDebug;

    //
    // Petit test memoire
    //
    g_tHalFunc.fnAllocMem( (clvoid **)&test, 10, 0 , 0 ); // alloc 10 char
    g_tHalFunc.fnFreeMemSafely( ( void *)&test);

    g_tHalFunc.fnAllocMem( (clvoid **)&test, 10, 0 , 0 ); // alloc 10 char

    test[0] = 'A';
    test[9] = 'A';

    g_tHalFunc.fnFreeMemSafely( &test);

    //-----------------------------

    g_tCSLDefaultReader.tSync.u32Retries = 2;

    //-----------------------------

    status = csl_InitGlobalContext
	(
		(e_StackSupport)( COM_STACK_SUPPORT ),
		(t_clSys_HalFunctions *)&g_tHalFunc,
		(t_clSys_CallBackFunctions *)&g_tCallbacks,
		(t_clSys_CallBackFunctions *)CL_NULL,
		&g_tCSLDefaultReader,
		&g_IPReaderDefaultHAL,
		CL_NULL,
		&g_COMReaderDefaultHAL,
		CL_NULL,
		CL_NULL,
		CL_NULL,
		(t_MenuFileDef *)&g_tMenuFileDef,
		FullExeFileNameWithDirectory
	);
    if ( CL_SUCCESS( status ) )
	{
		pm_trace0( "csl_InitGlobalContext: SUCCESS" );
	}
	else
	{
		pm_trace0( "csl_InitGlobalContext: FAILED" );
	}

    status = cl_InitReadersFromFile( g_friendlyNames );
	if ( CL_SUCCESS( status ) )
	{
		pm_trace0("DoInitGlobalContext: cl_InitReadersFromFile: SUCCESS");
	}
	else
	{
		pm_trace0("DoInitGlobalContext: cl_InitReadersFromFile: FAILED");
	}

	pm_trace0("DoInitGlobalContext: END [%d ms]", cl_GetElapsedTimeHighPerformanceWin32() );
}

/*---------------------------------------------------------------------------*/

static void DoDisplayReadersList( void )
{
	e_Result  status = CL_ERROR;
	t_Reader *pReader = CL_NULL;
	t_Tuple  *pTuple = CL_NULL;

	pm_trace0("DoDisplayReadersList: BEGIN");

	status = cl_getReader_List( &pReader);
	if ( CL_SUCCESS( status ) )
	{
		while ( pReader)
		{
			// display Reader's parameters
			pm_trace0( "Friendly name: %s", pReader->aucLabel );
			pm_trace0( "- reader port name: %s", pReader->tCOMParams.aucPortName );
			pm_trace0( "- reader state: %s", pReader->eState == STATE_CONNECT ? "Connected" : "Not connected" );
			pm_trace0( "- reader handle: 0x%x", pReader->u64Handle );

			int idTuple = 1;
			pTuple = pReader->p_TplList2Send;
			while ( pTuple != CL_NULL )
			{
				pm_trace0( "- tuple %d: Time: %s", idTuple, pTuple->cl8Time );
				pm_trace0( "  buf len: %d", idTuple, pTuple->ptBuf->ulLen );

				idTuple = idTuple + 1;
				pTuple = pTuple->pNext;
			}

			pReader = pReader->pNext;
		}
	}
	pm_trace0("DoDisplayReadersList: END");
}

/*---------------------------------------------------------------------------*/

static void DoChangeReader( void )
{
	bool result;
	int readerIndex = 0;

	c_printf("Reader actuel : [%d] %s\n", g_readerIndex, g_friendlyNames[g_readerIndex]);

	result = Input_UInt16("Choisir le reader (0 ou 1)", &readerIndex, 0, pmfalse, pmfalse );

	if( result )
	{
		if ( readerIndex >= 0 && readerIndex <= 1 )
		{
			g_readerIndex = readerIndex;
			c_printf("Nouveau reader : [%d] %s\n", g_readerIndex, g_friendlyNames[g_readerIndex]);
		}
		else
		{
			c_printf("Erreur dans le choix du reader, le reader est toujours %d\n", g_readerIndex);
		}
	}
}

/*---------------------------------------------------------------------------*\
 * If it is in state != STATE_CONNECT => Connect else => DISCONNECT
 */
static void DoChangeStateReader( void )
{
	e_Result    status = CL_ERROR;

	t_Reader tReaderFilter;
	t_Reader *pReaderFromList = CL_NULL;

	pm_trace1("DoChangeStateReader: BEGIN");
	cl_GetElapsedTimeHighPerformanceWin32();

	status = cl_InitReaderFromFileByFriendlyName( g_friendlyNames[g_readerIndex], &tReaderFilter );
	if ( status == CL_PARAMS_ERR )
	{
		pm_trace0("Reader not foud in readerlist file !");
	}

    if ( CL_FAILED( status = cl_readerFindInList( &pReaderFromList, &tReaderFilter ) ) )
    {
    	pm_trace0("Reader in reader list:NOT FOUND");
        return;
    }

    if ( pReaderFromList == CL_NULL )
    {
    	pm_trace0("Pointer to Reader in reader list:NULL");
        return;
    }

    if ( pReaderFromList->eState != STATE_CONNECT )
	{
    	// change reader state
		status = cl_ReaderSetState( pReaderFromList, STATE_CONNECT);
		if ( CL_FAILED( status ) )
		{
			pm_trace0("DoChangeStateReader: cl_ReaderSetState %s: -------> NOT CONNECTED", pReaderFromList->tCOMParams.aucPortName );
		}
		else
		{
			pm_trace0("DoChangeStateReader: cl_ReaderSetState %s: -------> CONNECTED", pReaderFromList->tCOMParams.aucPortName );
		}

		if ( CL_SUCCESS( status ) )
		{
			status = cl_ReaderStartThreads( pReaderFromList );
			pm_trace0("DoChangeStateReader: cl_ReaderSetState %s: -------> STARTED", pReaderFromList->tCOMParams.aucPortName );
		}
	}
    else
    {
        if ((pReaderFromList->eState != STATE_DISCONNECT ) & ( pReaderFromList->eState != STATE_DISCOVER ))
        {
            // disconnect from lower layer
        	status = cl_ReaderSetState( pReaderFromList, STATE_DISCONNECT );
			if ( CL_FAILED( status ) )
			{
				pm_trace0("DoChangeStateReader: cl_ReaderSetState: STATE_DISCONNECT: ERROR");
			}
        }
    }

    pm_trace0("DoChangeStateReader: END [%d ms]", cl_GetElapsedTimeHighPerformanceWin32());
}

/*---------------------------------------------------------------------------*\
 * if aReader != NULL send to aReader otherwise send to Reader Selected
 */
static void SendDataToReader( char *aReader, char *aFrame )
{
	bool 		readerConnected = false;
	t_Reader 	theReader;
	t_Reader 	*pReaderFromList;
	t_clContext *pCtxt = CL_NULL;
    e_Result 	status = CL_ERROR;
    t_Buffer    *pBuffForNet = CL_NULL;
	t_Tuple     *pTuple2Send = CL_NULL;

	e_SERIAL_RetryProtocolDef eSynchroStatus = CL_SERIAL_PROT_W_END_DONE;
	bool waiting_exchange = true;

	if ( aReader == CL_NULL )
	{
		status = cl_InitReaderFromFileByFriendlyName( g_friendlyNames[g_readerIndex], &theReader );
		if ( status == CL_PARAMS_ERR )
		{
			pm_trace0("Reader not foud in readerlist file !");
		}

		// Is Reader already in the list of CSL readers list
		if ( CL_FAILED( cl_readerFindInList( &pReaderFromList, &theReader ) ) )
		{
			pm_trace0("Reader not foud in CLS reader list !");

			// Si le reader n'est pas dans la liste on ne pourra pas
			// lui envoyer de commandes, on return
			return;
		}
	}
	else
	{
		status = cl_readerFindInListByFriendlyName( &pReaderFromList, aReader );
		if ( status == CL_PARAMS_ERR )
		{
			pm_trace0("SendDataToReader: Reader not foud in readerlist file !");
		}
	}

	if ( pReaderFromList )
	{
		if ( pReaderFromList->eState == STATE_CONNECT )
		{
			pm_trace1("SendDataToReader: Reader CONNECTED");
			readerConnected = true;
		}
		else
		{
			pm_trace0("SendDataToReader: Reader NOT CONNECTED");
		}
	}

	if ( readerConnected == true )
	{
		size_t lg = strlen( aFrame );

		pm_trace1("SendDataToReader: cmd %d", lg);

		if ( lg <= 0 || lg % 2 != 0 )
		{
			pm_trace0("SendDataToReader: Error");
			return;
		}

		size_t lg2 = lg / 2;

		clu8 dataToSendAscii[ lg ];
		clu8 dataToSend[ lg2 ];
		clu32 dataToSendLenght = lg2 ;
		clu8  *pData = dataToSend;

		memcpy( dataToSendAscii, aFrame, lg );

		ConvertStringToHexa(dataToSendAscii, strlen(dataToSendAscii), dataToSend);

		if ( !pData ) return;

		// get context to have access to function pointers for memory/thread management on platform
		if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
			return;

		// check context
		if ( pCtxt->ptHalFuncs == CL_NULL ) return;
		if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL ) return;

		// Allocate buffer for data to send
		csl_pmalloc( (clvoid **)&pBuffForNet, sizeof(t_Buffer) );
		csl_pmalloc( (clvoid **)&pBuffForNet->pData, dataToSendLenght);

		// allocate tuple which holds the data
		if ( CL_FAILED( csl_pmalloc( (clvoid **) &pTuple2Send, sizeof(t_Tuple)))) return;

		// save data in buffer
		memcpy( pBuffForNet->pData, pData, dataToSendLenght );

		//
		// Initialize a tuple default flags with memory
		//
		if ( CL_SUCCESS( cl_initTuple( pTuple2Send, pBuffForNet, &pBuffForNet->pData, dataToSendLenght ) ) )
		{
			stat_DoStressReadWrite += 1;

			pReaderFromList->p_TplList2Send = pTuple2Send;
			pCtxt->ptHalFuncs->fnSemaphoreRelease( pReaderFromList->tSync.pSgl4Write );

			DEBUG_PRINTF("SendDataToReader: ------------------------------------> [%d] Waiting_for_Write_To_Completed ...", stat_DoStressReadWrite );

			status = pCtxt->ptHalFuncs->fnSemaphoreWait( pReaderFromList->tSync.pSgl4WriteComplete, WAITING_READ_COMPLETE_TIMEOUT );
			if ( status == CL_OK )
			{
				pm_trace0("SendDataToReader: Write_Complete: OK");
			}
			if ( status == CL_TIMEOUT_ERR )
			{
				pm_trace0("SendDataToReader: Write_Complete: TIMEOUT");
			}

			DEBUG_PRINTF("SendDataToReader: ------------------------------------> [%d] New_Write_Can_Beging ...", stat_DoStressReadWrite );

		}
		else
		{
			DEBUG_PRINTF("SendDataToReader: ---------> ERROR_INIT_TUPLE");
		}
	}
}

/*---------------------------------------------------------------------------*/

static void DoSend010101ToReader( void )
{
	stat_DoSend010101 += 1;

	SendDataToReader( CL_NULL, "010101" );
}

/*---------------------------------------------------------------------------*/

static void DoSendCmdToReader( void )
{
	bool result;
    char commandString[80];
    int  commandLenght = 0;

	result = Input_String("Entrer une commande : ", commandString, "010101", pmfalse, pmfalse );

	commandLenght = strnlen( commandString, 80 );
	DEBUG_PRINTF("DoSendACommand: [%d octets]: %s", commandLenght, commandString);

	if ( IsXDigit( commandString ) == false )
	{
		c_printf("Error : Entrez un buffer à envoyer.\n");
		return;
	}

	SendDataToReader( g_friendlyNames[ g_readerIndex ], commandString );
}

/*---------------------------------------------------------------------------*\
 *
 * 01040201010d00XXXXXXXX0000000000000000
 *
 * XXXXXXXX étant l'adresse de destination donc l'adresse du master (l'autre)
 *
 * g_friendlyNames[0] : Serial;03 0E 0D 67;COM3;115200;000000000000;
 */
static void DoSendCMDThroughReaderToOther( void )
{
	bool 		readerConnected = false;
	t_Reader 	theReader;
	t_Reader 	*pReaderFromList;
	t_clContext *pCtxt = CL_NULL;
    e_Result 	status = CL_ERROR;
    t_Buffer    *pBuffForNet = CL_NULL;
	t_Tuple     *pTuple2Send = CL_NULL;

	e_SERIAL_RetryProtocolDef eSynchroStatus = CL_SERIAL_PROT_W_END_DONE;
	bool waiting_exchange = true;

	// Take other reader
	int otherReaderIndex = g_readerIndex == 0 ? 1 : 0;

	PMLITE_BEGIN_PROC(DoSendDataToReader);

	// Get context to have access to function pointers for memory/thread management on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return;

	status = cl_InitReaderFromFileByFriendlyName( g_friendlyNames[ g_readerIndex ], &theReader );
	if ( status == CL_PARAMS_ERR )
	{
		pm_trace0("Reader not foud in readerlist file !");
	}

	// Is Reader already in the list of CSL readers list
	if ( CL_FAILED( cl_readerFindInList( &pReaderFromList, &theReader ) ) )
	{
		pm_trace0("Reader not foud in CLS reader list !");

		// Si le reader n'est pas dans la liste on ne pourra pas
		// lui envoyer de commandes, on return
		return;
	}

	if ( pReaderFromList )
	{
		if ( pReaderFromList->eState == STATE_CONNECT )
		{
			pm_trace0("Reader in state CONNECTED");
			readerConnected = true;
		}
		else
		{
			pm_trace0("Reader not CONNECTED");
		}
	}

	if ( readerConnected == true )
	{
		size_t lenDataToSend = 19; // octets

		int i = 0, j = 0;

		//const char dataToSendAscii[] = "01040201010D030E0D500000000000000000";
		//const char dataToSendAscii[] = "01040201010D00030E0D500000000000000000"; // erreur corrigé 04/06/2015
		const unsigned char constDataToSendAscii[] = "01040201010D00XXXXXXXX0000000000000000"; // erreur corrigé 04/06/2015

		unsigned char dataToSendAscii[lenDataToSend * 2 + 1];
		char readerName[9];

		//
		// Replace XXXXXXXX by the friendly name of the other reader
		//
		for ( i = 0; i < lenDataToSend * 2; i++ )
		{
			dataToSendAscii[i] = constDataToSendAscii[i];
		}

		for ( i = 0; i < 11 ; i++ )
		{
			readerName[i] = g_friendlyNames[ otherReaderIndex ][i];
		}

		// Je n'ai pas trouvé l'algo ...
//		for ( i = 14, j = 0 ; i < 22; i++ )
//		{
//			DEBUG_PRINTF("DoSendCMDThroughReaderToOther: i:%d j:%d", i, j );
//
//			dataToSendAscii[i] = readerName[j];
//
//			j++;
//			if ( j % 2 == 0)
//				j = j + 1;
//		}

		dataToSendAscii[14] = readerName[0];
		dataToSendAscii[15] = readerName[1];
		dataToSendAscii[16] = readerName[3];
		dataToSendAscii[17] = readerName[4];
		dataToSendAscii[18] = readerName[6];
		dataToSendAscii[19] = readerName[7];
		dataToSendAscii[20] = readerName[9];
		dataToSendAscii[21] = readerName[10];

		dataToSendAscii[lenDataToSend * 2] = EOS;

		DEBUG_PRINTF("DoSendCMDThroughReaderToOther: dataToSendAscii: %s", dataToSendAscii);

		clu8 dataToSend[lenDataToSend];
		clu8 *pData = dataToSend;

		ConvertStringToHexa(dataToSendAscii, strlen(dataToSendAscii), dataToSend);

		// check input parameters
		if ( pCtxt->ptHalFuncs == CL_NULL ) return;

		if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL ) return;

		// Allocate buffer for data to send
		csl_pmalloc( (clvoid **)&pBuffForNet, sizeof(t_Buffer) );
		csl_pmalloc( (clvoid **)&pBuffForNet->pData, lenDataToSend);

		// allocate tuple which holds the data
		if ( CL_FAILED( csl_pmalloc( (clvoid **) &pTuple2Send, sizeof(t_Tuple)))) return;

		// save data in buffer
		memcpy( pBuffForNet->pData, pData, sizeof(dataToSend) );

		// initialize a tuple default flags with memory
		if ( CL_SUCCESS( cl_initTuple( pTuple2Send, pBuffForNet, &pBuffForNet->pData, sizeof(dataToSend) ) ) )
		{
			stat_DoSendCMDThroughReaderToOther += 1;

			pReaderFromList->p_TplList2Send = pTuple2Send;
			pCtxt->ptHalFuncs->fnSemaphoreRelease( pReaderFromList->tSync.pSgl4Write );


			DEBUG_PRINTF("DoSendDataToReader: ---------> Waiting_for_Write_To_Completed ...");

			status = pCtxt->ptHalFuncs->fnSemaphoreWait( pReaderFromList->tSync.pSgl4WriteComplete, WAITING_READ_COMPLETE_TIMEOUT );
			if ( status == CL_OK )
			{
				pm_trace0("DoSendDataToReader: Write_Complete: OK");
			}
			if ( status == CL_TIMEOUT_ERR )
			{
				pm_trace0("DoSendDataToReader: Write_Complete: TIMEOUT");
			}

			DEBUG_PRINTF("DoSendDataToReader: ---------> New_Write_Can_Beging ...");
		}
	}
}

/*---------------------------------------------------------------------------*/

static void DoSetRadioParamsToReaders( void )
{
	bool 		readerConnected = false;
	t_Reader 	theReader;
	t_Reader 	*pReaderFromList;
	t_clContext *pCtxt = CL_NULL;
    e_Result 	status = CL_ERROR;
    t_Buffer    *pBuffForNet = CL_NULL;
	t_Tuple     *pTuple2Send = CL_NULL;

	int reader_index = 0;

	PMLITE_BEGIN_PROC(DoSendDataToReader);

	// Get context to have access to function pointers for memory/thread management on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return;

	for ( reader_index = 0; reader_index < 2; reader_index++ )
	{
		status = cl_InitReaderFromFileByFriendlyName(g_friendlyNames[ reader_index ], &theReader);
		if ( status == CL_PARAMS_ERR )
		{
			pm_trace0("Reader not foud in readerlist file !");
		}

		// Is Reader already in the list of CSL readers list
		if ( CL_FAILED( cl_readerFindInList( &pReaderFromList, &theReader ) ) )
		{
			pm_trace0("Reader not foud in CLS reader list !");

			// Si le reader n'est pas dans la liste on ne pourra pas
			// lui envoyer de commandes, on return
			return;
		}

		if ( pReaderFromList )
		{
			if ( pReaderFromList->eState == STATE_CONNECT )
			{
				pm_trace0("Reader in state CONNECTED");
				readerConnected = true;
			}
			else
			{
				pm_trace0("Reader not CONNECTED");
			}
		}

		if ( readerConnected == true )
		{
			// Take the other reader
			int otherReaderIndex = reader_index == 0 ? 1 : 0;

			size_t lenDataToSend = 27; // octets
			int i = 0, j = 0;

			const unsigned char constDataToSendAscii[] = "010402030602003211020028010D00XXXXXXXX0000000000000000";

			unsigned char dataToSendAscii[lenDataToSend * 2 + 1];
			char readerName[9];

			//
			// Replace XXXXXXXX by the friendly name of the other reader
			//
			for ( i = 0; i < lenDataToSend * 2; i++ )
			{
				dataToSendAscii[i] = constDataToSendAscii[i];
			}

			for ( i = 0; i < 11 ; i++ )
			{
				readerName[i] = g_friendlyNames[ otherReaderIndex ][i];
			}

			// Je n'ai pas trouvé l'algo ...
			dataToSendAscii[30] = readerName[0];
			dataToSendAscii[31] = readerName[1];
			dataToSendAscii[32] = readerName[3];
			dataToSendAscii[33] = readerName[4];
			dataToSendAscii[34] = readerName[6];
			dataToSendAscii[35] = readerName[7];
			dataToSendAscii[36] = readerName[9];
			dataToSendAscii[37] = readerName[10];

			dataToSendAscii[lenDataToSend * 2] = EOS;

			DEBUG_PRINTF("DoSetRadioParamsToReaders: send to %s : dataToSendAscii: %s", pReaderFromList->tCOMParams.aucPortName, dataToSendAscii );

			clu8 dataToSend[lenDataToSend];
			clu8 *pData = dataToSend;

			ConvertStringToHexa(dataToSendAscii, strlen(dataToSendAscii), dataToSend);

			// check input parameters
			if ( pCtxt->ptHalFuncs == CL_NULL ) return;

			if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL ) return;

			// Allocate buffer for data to send
			csl_pmalloc( (clvoid **)&pBuffForNet, sizeof(t_Buffer) );
			csl_pmalloc( (clvoid **)&pBuffForNet->pData, lenDataToSend);

			// allocate tuple which holds the data
			if ( CL_FAILED( csl_pmalloc( (clvoid **) &pTuple2Send, sizeof(t_Tuple)))) return;

			// save data in buffer
			memcpy( pBuffForNet->pData, pData, sizeof(dataToSend) );

			// initialize a tuple default flags with memory
			if ( CL_SUCCESS( cl_initTuple( pTuple2Send, pBuffForNet, &pBuffForNet->pData, sizeof(dataToSend) ) ) )
			{
				stat_DoSetRadioParamsToReaders += 1;

				pReaderFromList->p_TplList2Send = pTuple2Send;
				pCtxt->ptHalFuncs->fnSemaphoreRelease( pReaderFromList->tSync.pSgl4Write );
			}

			DEBUG_PRINTF("DoSetRadioParamsToReaders_%d: ---------> Waiting_for_Write_To_Completed ...", reader_index );

			status = pCtxt->ptHalFuncs->fnSemaphoreWait( pReaderFromList->tSync.pSgl4WriteComplete, WAITING_READ_COMPLETE_TIMEOUT );
			if ( status == CL_OK )
			{
				pm_trace0("DoSendDataToReader: Write_Complete: OK");
			}
			if ( status == CL_TIMEOUT_ERR )
			{
				pm_trace0("DoSendDataToReader: Write_Complete: TIMEOUT");
			}

			DEBUG_PRINTF("DoSendDataToReader: ---------> New_Write_Can_Beging ...");
		}
	} // for reader_index
}

/*---------------------------------------------------------------------------*/

static void DoStressMonoReaderReadWrite( void )
{
	bool 		reader1Connected = false;
	t_Reader 	theReader1;
	t_Reader 	*pReaderFromList1;
	t_clContext *pCtxt = CL_NULL;
    e_Result 	status = CL_ERROR;
    t_Buffer    *pBuffForNet = CL_NULL;
	t_Tuple     *pTuple2Send = CL_NULL;

	pmuint16  repeatTimes = 0;
	bool result;

	// get context to have access to function pointers for memory/thread management on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return;

	result = Input_UInt16("Enter repeat times", &repeatTimes, true, 20, true );
	if ( result == false )
	{
		c_printf("RepeatTimes : Canceled.\n");
		return;
	}

	//
	//------------------------------------ Reader1
	//
	status = cl_InitReaderFromFileByFriendlyName( g_friendlyNames[ g_readerIndex ], &theReader1 );
	if ( status == CL_PARAMS_ERR )
	{
		pm_trace0("Reader1 not foud in readerlist file !");
	}

	// Is Reader already in the list of CSL readers list
	if ( CL_FAILED( cl_readerFindInList( &pReaderFromList1, &theReader1 ) ) )
	{
		pm_trace0("Reader not foud in CLS reader list !");

		// Si le reader n'est pas dans la liste on ne pourra pas
		// lui envoyer de commandes, on return
		return;
	}

	if ( pReaderFromList1 )
	{
		if ( pReaderFromList1->eState == STATE_CONNECT )
		{
			pm_trace0("DoSendDataToReader: Reader1 CONNECTED");
			reader1Connected = true;
		}
		else
		{
			pm_trace0("DoSendDataToReader: Reader1 NOT CONNECTED");
		}
	}

	//--------------------------------------------------------------------

	if ( reader1Connected == true )
	{
		clu8 dataToSendAscii[6] = "010101";
		clu8 dataToSend[3];
		clu32 dataToSendLenght = 3;
		clu8    *pData = dataToSend;

		ConvertASCIToHex( dataToSendAscii, 6, dataToSend, &dataToSendLenght );

		int i = 0;
		for ( i = 1; i <= repeatTimes; i++ )
		{
			stat_DoStressMonoReadWrite += 1;
			DEBUG_PRINTF("-------------------------------------------------------> DoStressReadWrite [%d]", stat_DoStressMonoReadWrite );

			// Allocate buffer for data to send
			csl_pmalloc( (clvoid **)&pBuffForNet, sizeof(t_Buffer) );
			csl_pmalloc( (clvoid **)&pBuffForNet->pData, dataToSendLenght);

			// Allocate tuple which holds the data
			if ( CL_FAILED( csl_pmalloc( (clvoid **) &pTuple2Send, sizeof(t_Tuple)))) return;

			// Save data in buffer
			memcpy( pBuffForNet->pData, pData, sizeof( dataToSend ) );

			//
			// Initialize a tuple default flags with memory
			//
			if ( CL_SUCCESS( cl_initTuple( pTuple2Send, pBuffForNet, &pBuffForNet->pData, sizeof( dataToSend ) ) ) )
			{
				pReaderFromList1->p_TplList2Send = pTuple2Send;
				pCtxt->ptHalFuncs->fnSemaphoreRelease( pReaderFromList1->tSync.pSgl4Write );

				DEBUG_PRINTF("DoStressReadWrite: ---------> Waiting_for_Write_To_Completed [%d]", i );

				status = pCtxt->ptHalFuncs->fnSemaphoreWait( pReaderFromList1->tSync.pSgl4WriteComplete, WAITING_READ_COMPLETE_TIMEOUT );
				if ( status == CL_OK )
				{
					pm_trace0("DoStressReadWrite1: Write_Complete: OK");
				}
				if ( status == CL_TIMEOUT_ERR )
				{
					pm_trace0("DoStressReadWrite: Write_Complete: TIMEOUT");
				}

				DEBUG_PRINTF("DoStressReadWrite: ---------> New_Write_Can_Beging [%d]", i );
			}
		}
	}
}

/*---------------------------------------------------------------------------*/

static void DoStressReadWrite( void )
{
	bool result;
	pmuint16  repeatTimes = 0;

	result = Input_UInt16("Enter repeat times", &repeatTimes, true, 1, true );
	if ( result == false )
	{
		c_printf("RepeatTimes : Canceled.\n");
		return;
	}

	int i = 0;
	for ( i = 0; i < repeatTimes; i++ )
	{
		SendDataToReader( g_friendlyNames[0], "010602" );
		SendDataToReader( g_friendlyNames[1], "010101" );

		SendDataToReader( g_friendlyNames[0], "010101" );
		SendDataToReader( g_friendlyNames[1], "010602" );

		c_printf("Interation: [%d]\n", i);
	}

	pm_trace0( "DoStressReadWrite : %d", stat_DoStressReadWrite );
}

/*---------------------------------------------------------------------------*/

static void DoSendCAFEThroughReaderToOther( void )
{
	stat_DoSendCAFEThroughReaderToOther += 1;

	SendDataToReader( CL_NULL, "010201CAFE" ); // 2500 on renvoit CAFE avant la fin de transmission 01 03 02
}

/*---------------------------------------------------------------------------*/

static void DoStressCAFEFADA( void )
{
	int i = 0;
	bool result;
	pmuint16  repeatTimes = 0;

	result = Input_UInt16("Enter repeat times", &repeatTimes, true, 10, true );
	if ( result == false )
	{
		c_printf("RepeatTimes : Canceled.\n");
		return;
	}

	c_printf("DoStressCAFEFADA: Start\n");

	for ( i = 0; i < repeatTimes; i++ )
	{
		stat_DoSendCAFEThroughReaderToOther += 1;
		SendDataToReader( CL_NULL, "010201CAFEFADA" );

		c_printf("DoStressCAFEFADA: %d\n", i);
	}

	c_printf("DoStressCAFEFADA: End\n");
}

/*---------------------------------------------------------------------------*/

static int ReadersCount()
{
	e_Result status;
	t_Reader *pReader = CL_NULL;
	int nbReaders = 0;

	if ( CL_FAILED( status = cl_GetReaderListEntry( &pReader ) ) )
	{
		pm_trace0("ReadersCount: NO_READER");
		return 0;
	}

	while ( pReader != CL_NULL )
	{
		nbReaders += 1;
		pReader = (t_Reader *)pReader->pNext;
	}

	return nbReaders;
}

/*---------------------------------------------------------------------------*/

static void DoInit( void )
{


	pm_trace0("DoInit: BEGIN");

	DoInitGlobalContext();
	c_printf("DoInitGlobalContext: OK\n");

	g_NbReaders = ReadersCount();
	pm_trace0( "DoInitGlobalContext: ReadersCount: %d", g_NbReaders );

	// Connect first reader
	if ( g_NbReaders >= 1 )
	{
		g_readerIndex = 0;
		DoChangeStateReader();
	}

	// Connect second reader
	if ( g_NbReaders >= 2 )
	{
		g_readerIndex = 1;
		DoChangeStateReader();
	}

	// Reselect first reader
	g_readerIndex = 0;

	c_printf("DoChangeStateReader: OK\n");

//	DoSetRadioParamsToReaders();
//	c_printf("DoSetRadioParamsToReaders: OK\n");

	pm_trace0("DoInit: ------------------------------------------------> INIT END");
}

/*---------------------------------------------------------------------------*/

static void DoClose( void )
{
	e_Result status = CL_ERROR;

	if ( IsClosed == false )
	{
		pm_trace0("DoClose: BEGIN");

		c_printf("Waiting for closing ...");

		status = csl_Close();

		pm_trace0("DoClose: status: %s", status == 0 ? "OK" : "NOT OK!???");
		pm_trace0("DoClose: END");

		c_printf(" Closed\n");
		Sleep( 600 ); // let the user see "Closed" on console's display
	}
	else
	{
		c_printf("Closed by user\n");
		pm_trace0("Closed by user");
	}

	pm_trace0("---------> stat_DoSend010101: %d", stat_DoSend010101);
	pm_trace0("---------> stat_DoSendCMDThroughReaderToOther: %d", stat_DoSendCMDThroughReaderToOther);
	pm_trace0("---------> stat_DoSetRadioParamsToReaders: %d", stat_DoSetRadioParamsToReaders);
	pm_trace0("---------> stat_DoSendCAFEThroughReaderToOther: %d", stat_DoSendCAFEThroughReaderToOther);
	pm_trace0("---------> stat_DoStressMonoReadWrite: %d", stat_DoStressMonoReadWrite);
	pm_trace0("---------> stat_DoStressReadWrite: %d", stat_DoStressReadWrite);
}

/*---------------------------------------------------------------------------*/

static void DoClose_ByUser(void)
{
    e_Result status = CL_ERROR;

    pm_trace0("DoClose_ByUser: BEGIN");

    DoClose();

	IsClosed = true;
	pm_trace0("DoClose_ByUser: END");
}

/*--------------------------------------------------------------------------*/

PMMENU_BEGIN( TMultiReader, "Multi Reader")
//    PMMENU_ITEM_EX(1, "Init GlobalContext CSL for COM", 		DoInitGlobalContext)
    PMMENU_ITEM_EX(1, "Change Reader", 							DoChangeReader)
    PMMENU_ITEM_EX(2, "Connect/Disconnect Reader", 				DoChangeStateReader)
//    PMMENU_ITEM_EX(4, "Close CSL Manualy", 						DoClose_ByUser)
    PMMENU_ITEM_EX(3, "Display Reader's List", 					DoDisplayReadersList)
	PMMENU_ITEM_SEPARATOR("--- On selected reader ---------------")
	PMMENU_ITEM_EX(4, "Send firmeware version '010101'", 		DoSend010101ToReader)
    PMMENU_ITEM_EX(5, "Send a command", 						DoSendCmdToReader)
    PMMENU_ITEM_EX(6, "Send cmd through Reader to other",		DoSendCMDThroughReaderToOther)
	PMMENU_ITEM_EX(7, "Set RadioParams To Readers",				DoSetRadioParamsToReaders)
	PMMENU_ITEM_EX(8, "Send 'CAFE' through Reader to other", 	DoSendCAFEThroughReaderToOther)
	PMMENU_ITEM_SEPARATOR("--------------------------------------")
	PMMENU_ITEM_EX(11, "Stress Mono Reader ReadWrite", 			DoStressMonoReaderReadWrite)
	PMMENU_ITEM_EX(22, "Stress ReadWrite", 						DoStressReadWrite)
	PMMENU_ITEM_EX(33, "Stress 'CAFEFADA'", 					DoStressCAFEFADA)
PMMENU_END()

/*---------------------------------------------------------------------------*/

PMMENU_HANDLER( TMultiReader )
{
    PMMENU_CHOICE_BEGIN()
		PMMENU_CASE_INIT(DoInit);
        PMMENU_CASE_CLOSE(DoClose);
	PMMENU_CHOICE_END()
}
