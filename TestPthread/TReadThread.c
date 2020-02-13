/*--------------------------------------------------------------------------*\
 * Copyright (c) 1998-2015 SoDevLog. Written by Bruno Raby.
 *--------------------------------------------------------------------------*
 * TReadThread.c - Realiser un test ou on ne fait que recevoir
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

static bool IsClosed = false; // indicate CSL have been closed by user

/*--------------------------------------------------------------------------*/

extern char FullExeFileNameWithDirectory[1024];

/*--------------------------------------------------------------------------*\
 * GLOBAL VARIABLES
\*--------------------------------------------------------------------------*/

static char g_friendlyNames[2][64] = { "00 00 00 00", "00 00 00 00" }; // reader's list
static int g_readerIndex = 0;

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
			pm_trace0( "- reader friendly name: %s", pReader->aucLabel );
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

	t_Reader tReaderFilter;
	t_Reader *pReaderFromList = CL_NULL;

	pm_trace1("DoSetUpCommReader: BEGIN");
	cl_GetElapsedTimeHighPerformanceWin32();

	status = cl_InitReaderFromFileByFriendlyName( g_friendlyNames[g_readerIndex], &tReaderFilter );
	if ( status == CL_PARAMS_ERR )
	{
		pm_trace0("DoSetUpCommReader: Reader not foud in readerlist file !");
	}

    if ( CL_FAILED( status = cl_readerFindInList( &pReaderFromList, &tReaderFilter ) ) )
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

PMMENU_BEGIN(TReadThread, "Read Thread")
	PMMENU_ITEM_EX(1, "Init CSL", 					DoInit)
    PMMENU_ITEM_EX(2, "Close CSL", 					DoClose_ByUser)
//	PMMENU_ITEM_EX(3, "SetupComm Reader", 			DoSetUpCommReader)
//    PMMENU_ITEM_EX(2, "Change Reader", 			DoChangeReader)
    PMMENU_ITEM_EX(4, "Display Reader's List", 		DoDisplayReadersList)
PMMENU_END()

/*---------------------------------------------------------------------------*/

PMMENU_HANDLER(TReadThread)
{
    PMMENU_CHOICE_BEGIN()
		PMMENU_CASE_INIT(DoInit);
        PMMENU_CASE_CLOSE(DoClose);
	PMMENU_CHOICE_END()
}
