/*--------------------------------------------------------------------------*\
 * Copyright (c) 1998-2015 SoDevLog. Written by Bruno Raby.
 *--------------------------------------------------------------------------*
 * TWriteThread.c
\*--------------------------------------------------------------------------*/

#include "cMenu.h"
#include "pmInterface.h"
#include "pmEnv.h"
#include "pmTrace.h"
#include "cInput.h"
#include "clreaders.h"
#include "csl.h"
#include "csl_component.h"
#include "cl_readers.h"

/*--------------------------------------------------------------------------*/

#define WAITING_READ_COMPLETE_TIMEOUT 4000 // 2000

/*--------------------------------------------------------------------------*/

static bool IsClosed = false; // indicate CSL have been closed by user

/*--------------------------------------------------------------------------*/

extern char FullExeFileNameWithDirectory[1024];

/*--------------------------------------------------------------------------*\
 * GLOBAL VARIABLES
\*--------------------------------------------------------------------------*/

static char g_friendlyNames[2][64] = { "00 00 00 00", "00 00 00 00" }; // reader's list
static int g_readerIndex = 0;

static t_Reader g_ReaderFilter;

/*---------------------------------------------------------------------------*/

bool IsXDigit(char *aString);

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
    // Petit test memoire, va cracher si test n'est pas libéré correctement
    //
    g_tHalFunc.fnAllocMem( (clvoid **)&test, 10, 0 , 0 ); // alloc 10 char
    g_tHalFunc.fnFreeMemSafely( ( void *)&test);

    g_tHalFunc.fnAllocMem( (clvoid **)&test, 10, 0 , 0 ); // alloc 10 char

    test[0] = 'A';
    test[9] = 'A';

    g_tHalFunc.fnFreeMemSafely( &test);

    //-----------------------------

    g_tCSLDefaultReader.tSync.u32Retries = 2;

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
		while ( pReader )
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

/*---------------------------------------------------------------------------*/

static void DoSetUpCommReader( void )
{
	e_Result    status = CL_ERROR;

	t_Reader *pReaderFromList = CL_NULL;

	pm_trace1("DoSetUpCommReader: BEGIN");
	cl_GetElapsedTimeHighPerformanceWin32();

	status = cl_InitReaderFromFileByFriendlyName( g_friendlyNames[g_readerIndex], &g_ReaderFilter );
	if ( status == CL_PARAMS_ERR )
	{
		pm_trace0("DoSetUpCommReader: Reader not foud in readerlist file !");
	}

    if ( CL_FAILED( status = cl_readerFindInList( &pReaderFromList, &g_ReaderFilter ) ) )
    {
    	pm_trace0("DoSetUpCommReader: Reader in reader list:NOT FOUND");
        return;
    }

    if ( pReaderFromList == CL_NULL )
    {
    	pm_trace0("DoSetUpCommReader: Pointer to Reader in reader list:NULL");
        return;
    }

    if ( pReaderFromList->eState != STATE_CONNECT )
	{
		status = cl_ReaderSetState( pReaderFromList, STATE_CONNECT);
		if ( CL_FAILED( status ) )
		{
			pm_trace0("DoSetUpCommReader: cl_ReaderSetState %s: -------> NOT CONNECTED", pReaderFromList->tCOMParams.aucPortName );
		}
		else
		{
			pm_trace0("DoSetUpCommReader: cl_ReaderSetState %s: -------> CONNECTED", pReaderFromList->tCOMParams.aucPortName );
		}

		if ( CL_SUCCESS( status ) )
		{
			status = cl_ReaderStartThreads( pReaderFromList );
			pm_trace0("DoSetUpCommReader: cl_ReaderSetState %s: -------> STARTED", pReaderFromList->tCOMParams.aucPortName );
		}

	}
    else
    {
        if ( ( pReaderFromList->eState != STATE_DISCONNECT ) & ( pReaderFromList->eState != STATE_DISCOVER ) )
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

/*---------------------------------------------------------------------------*/

static e_Result SendDataToReaderFindInList( clu8 *aBufferToSend, clu32 aBufferSize  )
{
	bool 		readerInStateConnected = false;
	t_Reader 	*pReaderFromList;
	t_clContext *pCtxt = CL_NULL;
    e_Result 	status = CL_ERROR;
    t_Buffer    *pBuffForNet = CL_NULL;

	t_Tuple     *pTuple2Send = CL_NULL;

	// get context to have access to function pointers for memory/thread management on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return CL_ERROR;

	// Is Reader already in the list of CSL readers list
	if ( CL_FAILED( cl_readerFindInList( &pReaderFromList, &g_ReaderFilter ) ) )
	{
		DEBUG_PRINTF("Reader not foud in CLS reader list !");
		return CL_ERROR;
	}

	// So we can check the Reader's state
	if ( pReaderFromList )
	{
		if ( pReaderFromList->eState == STATE_CONNECT )
		{
			DEBUG_PRINTF("Reader in state CONNECTED");
			readerInStateConnected = true;
		}
		else
		{
			DEBUG_PRINTF("Reader not CONNECTED");
		}
	}

	if (readerInStateConnected == true )
	{
		if ( aBufferSize %2 != 0 )
			return CL_ERROR;

		clu32 dataToSendLenght = aBufferSize/2;
		clu8 dataToSend[ dataToSendLenght ];
		ConvertASCIToHex(aBufferToSend, aBufferSize, dataToSend, &dataToSendLenght);

		// Allocate buffer for data to send
		csl_pmalloc( (clvoid **)&pBuffForNet, sizeof(t_Buffer) );
		csl_pmalloc( (clvoid **)&pBuffForNet->pData, dataToSendLenght);

		// allocate tuple which holds the data
		if ( CL_FAILED( csl_pmalloc( (clvoid **)&pTuple2Send, sizeof(t_Tuple)))) return CL_ERROR;

		// save data in buffer
		memcpy( pBuffForNet->pData, dataToSend, dataToSendLenght );

		// initialize a tuple default flags with memory
		if ( CL_SUCCESS( cl_initTuple( pTuple2Send, pBuffForNet, &pBuffForNet->pData, dataToSendLenght ) ) )
		{
			//
			// Direct Send Buffer to Serial Communication Port
			//
			//cl_COMAlSendDataWin32( pReaderFromList, pTuple2Send->ptBuf );
			status = pReaderFromList->tReaderHalFuncs.fnIOSendData( pReaderFromList, pTuple2Send->ptBuf );

			while ( pTuple2Send != NULL )
			{
				pReaderFromList->tCallBacks.fnIOSendDataDone_cb( pReaderFromList, pTuple2Send, status );

				t_Tuple *pTupleNext = (t_Tuple *)pTuple2Send->pNext;
				pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pBuffForNet->pData );
				pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pBuffForNet );
				pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2Send );
				pTuple2Send = pTupleNext;
			}
		}
	}

	return status;
}

/*---------------------------------------------------------------------------*/

static void DoSendACommand( void )
{
	e_Result 	status = CL_ERROR;

    bool result;
    char commandString[80];
    int  commandLenght = 0;

	clu8 dataToSendAsciiForACK[2] = "06";

    result = Input_String("Entrer une commande : ", commandString, "010101", pmfalse, pmfalse );

    commandLenght = strnlen( commandString, 80 );
    DEBUG_PRINTF("DoSendACommand: [%d octets]: %s", commandLenght, commandString);

	if ( IsXDigit(commandString) == false )
	{
		c_printf("Error : Entrez un buffer à envoyer.\n");
		return;
	}

	status = SendDataToReaderFindInList( commandString, commandLenght );
	if ( CL_FAILED( status ) )
	{
		c_printf("SendDataToReaderFindInList: FAILED\n");
		return;
	}
	else
	{
		pm_trace0("--->[%2d]:%s", commandLenght, commandString);
	}
}

/*---------------------------------------------------------------------------*/

static void DoSendACommandNTimesWithDelay( void )
{
	e_Result 	status = CL_ERROR;

    bool result;
    char commandString[80];
    int  commandLenght = 0;
    pmuint16  repeatTimes = 0;
    pmuint16  delayTime = 0;
    int i = 0;

	clu8 dataToSendAsciiForACK[2] = "06";

	c_printf("Command, Repeat, Delay\n");

	/*
	 * Command
	 */
    result = Input_String("Enter a buffer", commandString, "010101", true, pmfalse );
    if (result == false)
    {
		c_printf("Command : Canceled.\n");
		return;
    }

    commandLenght = strnlen( commandString, 80 );
    DEBUG_PRINTF("DoSendACommand: [result: %d] [%d octets]: %s", result, commandLenght, commandString);
	if ( IsXDigit(commandString) == false )
	{
		c_printf("Error : The buffer is not hexadecimal.\n");
		return;
	}

	/*
	 * RepeatTimes
	 */
	result = Input_UInt16("Enter repeat times", &repeatTimes, true, 1, true );
	if ( result == false )
	{
		c_printf("RepeatTimes : Canceled.\n");
		return;
	}

	/*
	 * delayTime
	 */
	if ( Input_UInt16("Enter delay time (ms)", &delayTime, true, 500, true ) == false )
	{
		c_printf("DelayTime : Canceled.\n");
		return;
	}

	/*
	 *
	 */
	for ( i = 0; i < repeatTimes; i++ )
	{
		pm_trace0("Command: [%3d]", i + 1);

		status = SendDataToReaderFindInList( commandString, commandLenght );
		if ( CL_FAILED( status ) )
		{
			c_printf("SendDataToReaderFindInList: FAILED\n");
			return;
		}
		else
		{
			pm_trace0("--->[%2d]:%s", commandLenght, commandString);
		}

		Sleep( delayTime );
	}
}

/*---------------------------------------------------------------------------*/

static void DoSendACommandUsingWriteThread( void )
{
	bool result;
	e_Result status = CL_ERROR;
	t_clContext *pCtxt = CL_NULL;
	t_Reader 	*pReaderFromList;

    char commandString[80];
    int  commandLenght = 0;
    bool isReaderConnected = false;

	t_Buffer    *pBuffForNet = CL_NULL;
	t_Tuple     *pTuple2Send = CL_NULL;

    result = Input_String("Entrer une commande : ", commandString, "010101", pmfalse, pmfalse );

    commandLenght = strnlen( commandString, 80 );
    DEBUG_PRINTF("DoSendACommand: [%d octets]: %s", commandLenght, commandString);

	if ( IsXDigit(commandString) == false )
	{
		c_printf("Error : Entrez un buffer à envoyer.\n");
		return;
	}

	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return CL_ERROR;

	if ( CL_FAILED( cl_readerFindInList( &pReaderFromList, &g_ReaderFilter ) ) )
	{
		DEBUG_PRINTF("Reader not foud in CLS reader list !");
		return CL_ERROR;
	}

	// So we can check the Reader's state
	if ( pReaderFromList )
	{
		if ( pReaderFromList->eState == STATE_CONNECT )
		{
			DEBUG_PRINTF("Reader in state CONNECTED");
			isReaderConnected = true;
		}
		else
		{
			DEBUG_PRINTF("Reader not CONNECTED");
		}
	}

	if ( isReaderConnected == true )
	{
		if ( commandLenght %2 != 0 )
			return CL_ERROR;

		clu32 dataToSendLenght = commandLenght/2;
		clu8 dataToSend[ dataToSendLenght ];
		ConvertASCIToHex( commandString, commandLenght, dataToSend, &dataToSendLenght );

		// Allocate buffer for data to send
		csl_pmalloc( (clvoid **)&pBuffForNet, sizeof(t_Buffer) );
		csl_pmalloc( (clvoid **)&pBuffForNet->pData, dataToSendLenght);

		// Allocate tuple which holds the data
		if ( CL_FAILED( csl_pmalloc( (clvoid **)&pTuple2Send, sizeof(t_Tuple)))) return CL_ERROR;

		// Save data in buffer
		memcpy( pBuffForNet->pData, dataToSend, dataToSendLenght );

		// initialize a tuple default flags with memory
		if ( CL_SUCCESS( cl_initTuple( pTuple2Send, pBuffForNet, &pBuffForNet->pData, dataToSendLenght ) ) )
		{
			//
			// Send using clReaderWriteThread
			//
			pReaderFromList->p_TplList2Send = pTuple2Send;
			pCtxt->ptHalFuncs->fnSemaphoreRelease( pReaderFromList->tSync.pSgl4Write );

			DEBUG_PRINTF("DoSendACommandUsingWriteThread: ---------> Waiting_for_Write_To_Completed ...");

			status = pCtxt->ptHalFuncs->fnSemaphoreWait( pReaderFromList->tSync.pSgl4WriteComplete, WAITING_READ_COMPLETE_TIMEOUT );
			if ( status == CL_OK )
			{
				pm_trace0("DoSendACommandUsingWriteThread: Write_Complete: OK");
			}
			if ( status == CL_TIMEOUT_ERR )
			{
				pm_trace0("DoSendACommandUsingWriteThread: Write_Complete: TIMEOUT");
			}

			DEBUG_PRINTF("DoSendACommandUsingWriteThread: ---------> New_Write_Can_Beging ...");
		}
	}
}

/*---------------------------------------------------------------------------*/

static void DoSendACommandUsingWriteThreadNTimes( void )
{
	bool result;
	e_Result status = CL_ERROR;
	t_clContext *pCtxt = CL_NULL;
	t_Reader 	*pReaderFromList;

    char commandString[80];
    int  commandLenght = 0;
    bool isReaderConnected = false;

	t_Buffer    *pBuffForNet = CL_NULL;
	t_Tuple     *pTuple2Send = CL_NULL;

	pmuint16  repeatTimes = 0;
	int i = 0;

	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return CL_ERROR;

	/*
	 * Command
	 */
    result = Input_String("Entrer une commande : ", commandString, "010101", pmfalse, pmfalse );

    commandLenght = strnlen( commandString, 80 );
    DEBUG_PRINTF("DoSendACommand: [%d octets]: %s", commandLenght, commandString);

	if ( IsXDigit(commandString) == false )
	{
		c_printf("Error : Entrez un buffer à envoyer.\n");
		return;
	}

	/*
	 * RepeatTimes
	 */
	result = Input_UInt16("Enter repeat times", &repeatTimes, true, 1, true );
	if ( result == false )
	{
		c_printf("RepeatTimes : Canceled.\n");
		return;
	}

	if ( CL_FAILED( cl_readerFindInList( &pReaderFromList, &g_ReaderFilter ) ) )
	{
		DEBUG_PRINTF("Reader not foud in CLS reader list !");
		return CL_ERROR;
	}

	// So we can check the Reader's state
	if ( pReaderFromList )
	{
		if ( pReaderFromList->eState == STATE_CONNECT )
		{
			DEBUG_PRINTF("Reader in state CONNECTED");
			isReaderConnected = true;
		}
		else
		{
			DEBUG_PRINTF("Reader not CONNECTED");
		}
	}

	if ( isReaderConnected == true )
	{
		if ( commandLenght %2 != 0 )
			return CL_ERROR;

		clu32 dataToSendLenght = commandLenght/2;
		clu8 dataToSend[ dataToSendLenght ];
		ConvertASCIToHex( commandString, commandLenght, dataToSend, &dataToSendLenght );

		/*
		 * Repeat N Times
		 */
		for ( i = 0; i < repeatTimes; i++ )
		{
			pm_trace0("Command: [%3d]", i + 1);

			// Allocate buffer for data to send
			csl_pmalloc( (clvoid **)&pBuffForNet, sizeof(t_Buffer) );
			csl_pmalloc( (clvoid **)&pBuffForNet->pData, dataToSendLenght);

			// allocate tuple which holds the data
			if ( CL_FAILED( csl_pmalloc( (clvoid **)&pTuple2Send, sizeof(t_Tuple)))) return CL_ERROR;

			// save data in buffer
			memcpy( pBuffForNet->pData, dataToSend, dataToSendLenght );

			// initialize a tuple default flags with memory
			if ( CL_SUCCESS( cl_initTuple( pTuple2Send, pBuffForNet, &pBuffForNet->pData, dataToSendLenght ) ) )
			{
				//
				// Send using clReaderWriteThread
				//
				pReaderFromList->p_TplList2Send = pTuple2Send;
				pCtxt->ptHalFuncs->fnSemaphoreRelease( pReaderFromList->tSync.pSgl4Write );

				DEBUG_PRINTF("DoSendACommandUsingWriteThread: ---------> Waiting_for_Write_To_Completed ...");

				status = pCtxt->ptHalFuncs->fnSemaphoreWait( pReaderFromList->tSync.pSgl4WriteComplete, WAITING_READ_COMPLETE_TIMEOUT );
				if ( status == CL_OK )
				{
					pm_trace0("DoSendACommandUsingWriteThread: Write_Complete: OK");
				}
				if ( status == CL_TIMEOUT_ERR )
				{
					pm_trace0("DoSendACommandUsingWriteThread: Write_Complete: TIMEOUT");
				}

				DEBUG_PRINTF("DoSendACommandUsingWriteThread: ---------> New_Write_Can_Beging ...");
			}
		}
	}
}

/*---------------------------------------------------------------------------*/

static void DoInit( void )
{
	pm_trace0("DoInit: BEGIN");

	DoInitGlobalContext();
	c_printf("DoInitGlobalContext: OK\n");

	DoSetUpCommReader();
	c_printf("DoSetUpCommReader: OK\n");

	IsClosed = false;

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
		Sleep( 300 ); // let the user see "Closed" on console's display
	}
	else
	{
		c_printf("Closed by user\n");
		pm_trace0("Closed by user");
	}
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

PMMENU_BEGIN( TWriteThread, "Write Thread " )
	PMMENU_ITEM_EX(1, "Init CSL", 				DoInit)
    PMMENU_ITEM_EX(2, "Close CSL", 				DoClose_ByUser)
//	PMMENU_ITEM_EX(3, "SetupComm Reader", 		DoSetUpCommReader)
//    PMMENU_ITEM_EX(2, "Change Reader", 		DoChangeReader)
    PMMENU_ITEM_EX(4, "Display Reader's List", 	DoDisplayReadersList)
//	PMMENU_ITEM_SEPARATOR("--- Send Direct to IO --------------------------")
//    PMMENU_ITEM_EX(5, "Send Command direct to IO", DoSendACommand)
//	PMMENU_ITEM_EX(6, "Send Command direct to IO N Times with Delay", DoSendACommandNTimesWithDelay)
//	PMMENU_ITEM_SEPARATOR("--- Send Using Write Thread --------------------")
	PMMENU_ITEM_SEPARATOR("-----------------------")
    PMMENU_ITEM_EX(7, "Send Command ", DoSendACommandUsingWriteThread)
    PMMENU_ITEM_EX(8, "Send Command N Times", DoSendACommandUsingWriteThreadNTimes)
PMMENU_END()

/*---------------------------------------------------------------------------*/

PMMENU_HANDLER( TWriteThread )
{
    PMMENU_CHOICE_BEGIN()
		PMMENU_CASE_INIT(DoInit);
        PMMENU_CASE_CLOSE(DoClose);
	PMMENU_CHOICE_END()
}
