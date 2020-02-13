/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: comal_win32.c                                                      */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/* Description: porting layer for com port access in WIN32 environment       */
/*****************************************************************************/

/*--------------------------------------------------------------------------*\
 * C'est une adaptation assez pourrie de MTTTY
 * 22/05/2015
 * Suite à la mise en oeuvre de l'écoute sur la Thread Read, ça
 * ne fonctionne plus, je tente une implantation de ReportStatusEvent
 * je trouve encore une ou deux astuces pour que cela fonctionne
 * --------------------------------------------------------------------------*
 * 01/10/2015 - Suite à la lecture :
 * https://msdn.microsoft.com/en-us/library/ms810467.aspx
 * je pense que le code REAL_RFID créé a "side effects of SetCommMask"
 * BRY_01102015
 * BRY_02102015
\*--------------------------------------------------------------------------*/

#ifdef WIN32

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <string.h>

#include <windows.h>
#include <errno.h>

#elif defined (linux)

#error not defined for this platform

#else

#error not defined for this platform

#endif

#include "..\..\csl.h"
#include "comal_win32.h"
#include "..\..\inc\generic.h"
#include "..\..\inc\cltuple.h"
#include "..\..\inc\clhelpers.h"
#include "..\..\inc\clreaders.h"

/*--------------------------------------------------------------------------*/

#define NO_REAL_RFID

// BRY_07102015 - Affiche tels quels les octets
#define BUFFER_NETWORKS_TUPLES
#define BUFFER_MODE_RAW

/*--------------------------------------------------------------------------*/

e_Result cl_ReaderReadThreadTraitement( clvoid *pCallingReader, t_Tuple **ppTuple );

/*--------------------------------------------------------------------------*\
 * DEBUG PURPOSE
\*--------------------------------------------------------------------------*/

#define DEBUG_INTERNAL_THREAD_READER_AND_STATUS_PROC

#ifdef DEBUG_INTERNAL_THREAD_READER_AND_STATUS_PROC
#define TRACE_INTERNAL_THREAD_EVENT DEBUG_PRINTF1
#else
#define TRACE_INTERNAL_THREAD_EVENT
#endif

#define NO_TRACE_INTERNAL_THREAD_EVENT

/*--------------------------------------------------------------------------*\
 * Constant definitions
\*--------------------------------------------------------------------------*/

#define PURGE_FLAGS             PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR
#define EVENTFLAGS_DEFAULT      EV_BREAK | EV_CTS | EV_DSR | EV_ERR | EV_RING | EV_RLSD

#define STATUS_CHECK_TIMEOUT    500 //200 //500 // 100 //500

//#define NUM_READSTAT_HANDLES 	3 // no event : exist thread BRY_05102015
#define NUM_READSTAT_HANDLES 	4 // no event : exist thread
#define AMOUNT_TO_READ          512 // 128

/*
    TimeoutsDefault
        We need ReadIntervalTimeout here to cause the read operations
        that we do to actually timeout and become overlapped.
        Specifying 1 here causes ReadFile to return very quickly
        so that our reader thread will continue execution.
*/
//COMMTIMEOUTS g_TimeoutsDefault = { 0x01, 0, 0, 0, 0 };

//  typedef struct _COMMTIMEOUTS {
//    DWORD ReadIntervalTimeout;
//    DWORD ReadTotalTimeoutMultiplier;
//    DWORD ReadTotalTimeoutConstant;
//    DWORD WriteTotalTimeoutMultiplier;
//    DWORD WriteTotalTimeoutConstant;
//  } COMMTIMEOUTS,*LPCOMMTIMEOUTS;

COMMTIMEOUTS g_TimeoutsDefault = { 0x1, 0, 0, 0, 0 };

extern int errno;

/*--------------------------------------------------------------------------*/

void ErrorReporter(char *szMessage)
{
	DEBUG_PRINTF("ErrorReporter: Message: %s", szMessage);
	Sleep(10000); // une action utilisateur est requise
	 	 	 	   // lasser le temps à l'utilisateur de faire, par exemple,
	 	 	 	   // un reset du port COM
}

/*--------------------------------------------------------------------------*/

void ErrorHandler(char *szMessage)
{
    ErrorReporter(szMessage);
    ExitProcess(0);
}

/*--------------------------------------------------------------------------*/

static void UpdateStatus(t_Reader *ptReader, char * szText)
{
    char * szNewMsg;
    DWORD dwSize;
//    STATUS_MESSAGE * lpStatusMessage;
    static dwMessageCounter = 0;

    dwMessageCounter++;

    dwSize = strlen(szText) + 30;    // include NULL terminator and space for counter

    DEBUG_PRINTF("UpdateStatus: %s: %s", ptReader->tCOMParams.aucPortName, szText);

    SetEvent(ptReader->tCOMParams.handleStatusMessage);

    return ;
}

/*--------------------------------------------------------------------------*/

static void ReportStatusEvent(t_Reader *pReader, DWORD dwStatus)
{
    BOOL fRING, fRLSD, fRXCHAR, fRXFLAG, fTXEMPTY;
    BOOL fBREAK, fCTS, fDSR, fERR;
    char szMessage[100]; //char szMessage[70];

    HANDLE hPortCOM = (HANDLE)pReader->u64Handle;

    //
    // Get status event flags.
    //
    fCTS = EV_CTS & dwStatus;
    fDSR = EV_DSR & dwStatus;
    fERR = EV_ERR & dwStatus;
    fRING = EV_RING & dwStatus;
    fRLSD = EV_RLSD & dwStatus;
    fBREAK = EV_BREAK & dwStatus;
    fRXCHAR = EV_RXCHAR & dwStatus;
    fRXFLAG = EV_RXFLAG & dwStatus;
    fTXEMPTY = EV_TXEMPTY & dwStatus;

    /*
        Construct status message indicating the
        status event flags that are set.
    */
    strcpy(szMessage, "EVENT: ");
    strcat(szMessage, fCTS ? "CTS " : "");
    strcat(szMessage, fDSR ? "DSR " : "");
    strcat(szMessage, fERR ? "ERR " : "");
    strcat(szMessage, fRING ? "RING " : "");
    strcat(szMessage, fRLSD ? "RLSD " : "");
    strcat(szMessage, fBREAK ? "BREAK " : "");
    strcat(szMessage, fRXFLAG ? "RXFLAG " : "");
    strcat(szMessage, fRXCHAR ? "RXCHAR " : "");
    strcat(szMessage, fTXEMPTY ? "TXEMPTY " : "");

    /*
        If dwStatus == NULL, then no status event flags are set.
        This happens when the event flag is changed with SetCommMask.
    */
    if (dwStatus == 0x0000)
        strcat(szMessage, "NULL");

//    strcat(szMessage, "\r\n");

    //
    // Queue the status message for the status control
    //
    //UpdateStatus(szMessage);
    DEBUG_PRINTF( "ReportStatusEvent: COM %s", pReader->tCOMParams.aucPortName );
    DEBUG_PRINTF( "ReportStatusEvent: %s", szMessage );
    SetEvent(pReader->tCOMParams.handleStatusMessage);

    /*
        If an error flag is set in the event flag, then
        report the error with the status message
        If not, then just report the comm status.
    */
    if (fERR)
    {
//    	ReportCommError();
    	DEBUG_PRINTF("ReportStatusEvent: ReportCommError()");
    }

    /*
        Might as well check the modem status and comm status now since
        the event may have been caused by a change in line status.
        Line status is indicated by the CheckModemStatus function.
    */
//     CheckModemStatus( FALSE );
    DWORD dwNewModemStatus;
    if ( !GetCommModemStatus(hPortCOM, &dwNewModemStatus) )
    {
        ErrorReporter("GetCommModemStatus");
    }

    /*
        Since line status can affect sending/receiving when
        hardware flow-control is used, ReportComStat should
        be called to show comm status.  This is called only if no error
        was reported in the event flag.  If an error was reported, then
        ReportCommError was called above and CheckComStat was already called
        in that function.
    */
    if (!fERR)
    {
		//CheckComStat( FALSE );
		COMSTAT ComStatNew;
		DWORD dwErrors;

		DEBUG_PRINTF("ReportStatusEvent: ClearCommError()");

		if ( !ClearCommError(hPortCOM, &dwErrors, &ComStatNew) )
			ErrorReporter("ClearCommError");
    }

    return;
}

/******************************************************************************/
/* Name :  e_Result cl_COMAlResetWin32( clvoid *ptReader );                   */
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
e_Result cl_COMAlResetWin32(  clvoid *ptReader )
{
	return ( CL_OK );
}

/******************************************************************************/
/* Name :  e_Result cl_COMAlRegisterWin32( clvoid *ptReader );                */
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
e_Result cl_COMAlRegisterWin32(  clvoid *ptReader )
{
	return ( CL_OK );
}
/******************************************************************************/
/* Name :  e_Result cl_COMAlUnregisterWin32( clvoid *ptReader );                  */
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
e_Result cl_COMAlUnregisterWin32(  clvoid *ptReader )
{
	return ( CL_OK);
}

/******************************************************************************/
/* Name :  e_Result cl_COMAlSendDataWin32( clvoid *ptReader, t_Buffer *ptBuff);   */
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
e_Result cl_COMAlSendDataWin32( clvoid *ptReader, t_Buffer *ptBuff)
{
	t_clContext *pCtxt 		= CL_NULL;
	e_Result status 		= CL_NULL;
	clu32 	iReturnCode 	= 0;
    t_Tuple *ptTuple		= CL_NULL;
    t_Buffer *pLocalBuf		= CL_NULL;
    clu8	*pData			= CL_NULL;
    t_Reader *pReader		= (t_Reader *)ptReader;
	cl8		*pMsg			= CL_NULL;
	OVERLAPPED osWrite = {0};
	DWORD dwWritten;
	DWORD dwRes;
	BOOL fRes;

	DEBUG_PRINTF1("cl_COMAlSendDataWin32: BEGIN");

	// check incoming parameters
	if ( pReader == CL_NULL )
		return ( CL_PARAMS_ERR );

	if ( ptBuff == CL_NULL )
		return ( CL_PARAMS_ERR );


	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL)
		return ( CL_ERROR );

	for (;;)
	{
		DEBUG_PRINTF1("cl_COMAlSendDataWin32: for (;;) BEGIN");

		//----
		// perform a local copy of incoming buff to apply format change with serial support without corrupting incoming data (which is used subsequently in CSL)
		//----
		// allocate memory for structure t_Buffer
		//if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnAllocMem( ( clvoid **)&pLocalBuf, sizeof( t_Buffer ) ) ) )
		if ( CL_FAILED( status = csl_malloc( ( clvoid **)&pLocalBuf, sizeof( t_Buffer ) ) ) )
		{
			DEBUG_PRINTF("cl_COMAlSendDataWin32: csl_malloc(pLocalBuf) FAILED");
			return ( CL_ERROR );
		}

		if ( CL_FAILED( status = csl_malloc( ( clvoid **)&pData, ptBuff->ulLen ) ) )
		{
			pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&pLocalBuf );
			DEBUG_PRINTF("cl_COMAlSendDataWin32: csl_malloc(pData) FAILED");
			return ( CL_ERROR );
		}

		pLocalBuf->pData = pData;
		memcpy( pLocalBuf->pData, ptBuff->pData, ptBuff->ulLen);
		pLocalBuf->ulLen = ptBuff->ulLen;

		//DEBUG_PRINTF("\n 1. cl_COMAlSendDataWin32 %d \n", ptBuff->ulLen);

		//----
		// add the low level encapsulation if necessary
		//----
		if ( CL_FAILED( status = cl_HelperEncapsulateToSerial( pReader, pLocalBuf ) ) )
		{
			DEBUG_PRINTF("Failed on cl_HelperEncapsulateToSerial %s with Len %d\n ", pReader->tCOMParams.aucPortName, pLocalBuf->ulLen );
			break;
		}

		//DEBUG_PRINTF("\n 2. cl_COMAlSendDataWin32 %d \n", pLocalBuf->ulLen);

		// Do some basic settings
		DCB serialParams = { 0 };
		serialParams.DCBlength = sizeof(serialParams);

		if ( !GetCommState(pReader->u64Handle, &serialParams) )
		{
			DEBUG_PRINTF("cl_COMAlSendDataWin32: Error in GetCommState\n");
		}

		COMMTIMEOUTS readtimeout = { 0 };
		if ( !GetCommTimeouts( pReader->u64Handle, &readtimeout ) )
		{
			DEBUG_PRINTF("cl_COMAlSendDataWin32: GetCommTimeouts: ERROR");
		}

		// Create this write operation's OVERLAPPED structure's hEvent.
		osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, CL_NULL);
		if (osWrite.hEvent == CL_NULL)
		   // error creating overlapped event handle
		   break;

		//DEBUG_PRINTF("WRITE:  Reader handle is %x\n", pReader->u64Handle );
		// Issue write.
		if ( !WriteFile( pReader->u64Handle, (clvoid *)pLocalBuf->pData, pLocalBuf->ulLen, &dwWritten, &osWrite ) )
		{
		   if (GetLastError() != ERROR_IO_PENDING)
		   {
			  // WriteFile failed, but isn't delayed. Report error and abort.
			  status = CL_ERROR;
		   }
		   else
		   {
			  // Write is pending.
			  dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
			  switch(dwRes)
			  {
				 // OVERLAPPED structure's event has been signaled.
				 case WAIT_OBJECT_0:
				 {
					  if (!GetOverlappedResult(pReader->u64Handle, &osWrite, &dwWritten, FALSE))
							status = CL_ERROR;
					  else
					  {
						  // Write operation completed successfully.
						  status = CL_OK;
					  }
					  break;
				 }
				 default:
				 {
					  // An error has occurred in WaitForSingleObject.
					  // This usually indicates a problem with the
					 // OVERLAPPED structure's event handle.
					  status = CL_ERROR;
					  break;
				 }
			  }
		   }
		}
		else
		{
		   // WriteFile completed immediately.
		   status = CL_OK;
		}

		DEBUG_PRINTF1("cl_COMAlSendDataWin32: for (;;) END");
		break;
	}

	char *buffString;
	csl_malloc( (void **)&buffString, pLocalBuf->ulLen * 3 + 3 );
	strnset(buffString, EOS, pLocalBuf->ulLen * 3 + 3);
	//buffString[pLocalBuf->ulLen * 3 + 1] = EOS;
	PrintBuffer(buffString, pLocalBuf->pData, pLocalBuf->ulLen, "%02X ");
    // buffString==NULL si on ne met pas cl_GetElapsedTime à la fin !!! BRY_WHAOU
	DEBUG_PRINTF("------> cl_COMAlSendDataWin32: send %d oct : %s [etime:%d]", pLocalBuf->ulLen, buffString, cl_GetElapsedTime() );
	pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&buffString );

    if ( pLocalBuf )
    {
    	pData = pLocalBuf->pData;
    	pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pLocalBuf );
    }

    if ( pData )
    {
    	pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pData );
    }

    if ( osWrite.hEvent)
    {
    	// close handle to avoid leaks
    	CloseHandle ( osWrite.hEvent );
    }

	return ( status );
}

//
// https://msdn.microsoft.com/en-us/library/ms810467.aspx#serial_topic5
//

/******************************************************************************/
/* Name :  e_Result cl_COMAlGetStatus( clvoid *pReader);  */
/* Description : check that an event happened on the COM port (different from TX_EMPTY)	*/
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pReader : pointer to there reader where to send the data   */
/*                           if NULL, returns PARAMS_ERR                      */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*                          * OK : read operation can proceed                   */
/*                          * ERROR:   do not enter read operation	*/
/******************************************************************************/
e_Result cl_COMAlGetStatus( clvoid *ptReader )
{
	t_Reader *pReader = ptReader;
	OVERLAPPED osStatus = {0};
	DWORD 		dwOvRes;
	DWORD      	dwRes;
	DWORD      	dwCommEvent;
	DWORD      	dwStoredFlags;
	BOOL		fWaitingOnStat = FALSE;
	BOOL bWaitingOnStatusHandle;
	BOOL bExitFromThere = FALSE;
	e_Result	status = CL_ERROR;

/*   dwStoredFlags = EV_BREAK | EV_CTS | EV_DSR | EV_ERR | EV_RING |\
                  EV_RLSD | EV_RXCHAR | EV_RXFLAG | EV_TXEMPTY ;
   if (!SetCommMask(pReader->u64Handle , dwStoredFlags))
      // error setting communications mask; abort
      return ( CL_ERROR );
*/
   osStatus.hEvent = CreateEvent(NULL, TRUE, FALSE, CL_NULL);
   if (osStatus.hEvent == CL_NULL)
   {
	   DEBUG_PRINTF("cl_COMAlGetStatus: %s BEGIN: ERROR creating event", pReader->tCOMParams.aucPortName);
      // error creating event; abort
      return ( CL_ERROR );
   }

   DEBUG_PRINTF("cl_COMAlGetStatus: %s BEGIN", pReader->tCOMParams.aucPortName);

   while ( bExitFromThere == FALSE )
   {
      // Issue a status event check if one hasn't been issued already.
      if ( !fWaitingOnStat )
      {
         if ( !WaitCommEvent( pReader->u64Handle , &dwCommEvent, &osStatus) )
         {
            if ( GetLastError() == ERROR_IO_PENDING )
            {
                fWaitingOnStat = TRUE;
//               bWaitingOnStatusHandle = TRUE;
            }
            else
            {
				fWaitingOnStat = FALSE;
				bExitFromThere = TRUE;
				// error in WaitCommEvent; abort
				break;
            }
         }
         else
         {
            // WaitCommEvent returned immediately.
            // Deal with status event as appropriate.
        	 DEBUG_PRINTF("Serial Event %x", dwCommEvent);
        	 if ( dwCommEvent & EV_BREAK )
        	 {
        		 DEBUG_PRINTF("Serial Event: EV_BREAK;\n");
        	 }
        	 if ( dwCommEvent & EV_CTS )
        	 {
        		 DEBUG_PRINTF("Serial Event: EV_CTS;\n");
        	 }
        	 if ( dwCommEvent & EV_DSR )
        	 {
        		 DEBUG_PRINTF("Serial Event: EV_DSR;\n");
        	 }
        	 if ( dwCommEvent & EV_ERR )
        	 {
        		 DEBUG_PRINTF("Serial Event: EV_ERR;\n");
        	 }
        	 if ( dwCommEvent & EV_RING )
        	 {
        		 DEBUG_PRINTF("Serial Event: EV_RING;\n");
        	 }
        	 if ( dwCommEvent & EV_RLSD )
        	 {
        		 DEBUG_PRINTF("Serial Event: EV_RLSD;\n");
        	 }
        	 if ( dwCommEvent & EV_RXCHAR )
        	 {
        		 DEBUG_PRINTF("Serial Event: EV_RXCHAR;\n");
        	 }
        	 if ( dwCommEvent & EV_RXFLAG )
        	 {
        		 DEBUG_PRINTF("Serial Event: EV_RXFLAG;\n");
        	 }

        	 bExitFromThere = TRUE;
             status = CL_OK;

        	 break;
         }
      }

      // Check on overlapped operation.
      if ( fWaitingOnStat )
      {
         // Wait a little while for an event to occur.
         dwRes = WaitForSingleObject( osStatus.hEvent, STATUS_CHECK_TIMEOUT );
         switch( dwRes )
         {
        	 //
			 // status completed
			 //
             case WAIT_OBJECT_0:
             {
                 if ( !GetOverlappedResult( pReader->u64Handle, &osStatus, &dwOvRes, FALSE ) )
                 {
						// An error occurred in the overlapped operation;
						// call GetLastError to find out what it was
						// and abort if it is fatal.
						 DEBUG_PRINTF( "cl_COMAlGetStatus: GetOverlappedResult: FAILED" );
						 if ( GetLastError() == ERROR_OPERATION_ABORTED )
						 {
							 DEBUG_PRINTF( "cl_COMAlGetStatus: GetOverlappedResult: ERROR_OPERATION_ABORTED" );
//							 status = CL_OPERATION_ABORTED;
							 SetEvent( osStatus.hEvent ); // BRY_ : il faut sortir de là au plus vite
						 }
						 else
						 {
							 // ErrorInComm :
	//                		    ErrorReporter(szMessage);
	//                		    BreakDownCommPort();
	//                		    ExitProcess(0); oups !!!!

							 DEBUG_PRINTF( "cl_COMAlGetStatus: GetOverlappedResult: ErrorInComm !!!" );
						 }
                 }
                 else
                 {
                    // Status event is stored in the event flag
                    // specified in the original WaitCommEvent call.
                    // Deal with the status event as appropriate.
/*                	 DEBUG_PRINTF("Serial Event %x", dwCommEvent);

                	 if ( dwCommEvent & EV_BREAK )
                	 {
                		 DEBUG_PRINTF("Serial Event: EV_BREAK;\n");
                	 }
                	 if ( dwCommEvent & EV_CTS )
                	 {
                		 DEBUG_PRINTF("Serial Event: EV_CTS;\n");
                	 }
                	 if ( dwCommEvent & EV_DSR )
                	 {
                		 DEBUG_PRINTF("Serial Event: EV_DSR;\n");
                	 }
                	 if ( dwCommEvent & EV_ERR )
                	 {
                		 DEBUG_PRINTF("Serial Event: EV_ERR;\n");
                	 }
                	 if ( dwCommEvent & EV_RING )
                	 {
                		 DEBUG_PRINTF("Serial Event: EV_RING;\n");
                	 }
                	 if ( dwCommEvent & EV_RLSD )
                	 {
                		 DEBUG_PRINTF("Serial Event: EV_RLSD;\n");
                	 }
                	 if ( dwCommEvent & EV_RXCHAR )
                	 {
                		 DEBUG_PRINTF("Serial Event: EV_RXCHAR;\n");
                	 }
                	 if ( dwCommEvent & EV_RXFLAG )
                	 {
                		 DEBUG_PRINTF("Serial Event: EV_RXFLAG;\n");
                	 }
                    //ReportStatusEvent(dwCommEvent);
*/
//                	 ReportStatusEvent(pReader, dwCommEvent, statusMessageEvent);
					 // Set fWaitingOnStat flag to indicate that a new
					 // WaitCommEvent is to be issued.

                     status = CL_OK;
                 }

            	 fWaitingOnStat = FALSE;
            	 bExitFromThere = TRUE;

                 break;
             }

             case WAIT_TIMEOUT:
             {
                 // Operation isn't complete yet. fWaitingOnStatusHandle flag
                 // isn't changed since I'll loop back around and I don't want
                 // to issue another WaitCommEvent until the first one finishes.
                 //
                 // This is a good time to do some background work.

            	 fWaitingOnStat = TRUE;
            	 status = CL_TIMEOUT_ERR;

            	 bExitFromThere = TRUE; // BRY_2205

                //DoBackgroundWork();
                 break;
             }

             default:
             {
                 // Error in the WaitForSingleObject; abort
                 // This indicates a problem with the OVERLAPPED structure's
                 // event handle.

                //CloseHandle(osStatus.hEvent);
            	 fWaitingOnStat = FALSE;
            	 bExitFromThere = TRUE;
             }
         } // End of switch( dwRes )
      }// End of if ( fWaitingOnStat )
   }// while ( bExitFromThere == FALSE )

   CloseHandle( osStatus.hEvent );

//   DEBUG_PRINTF("cl_COMAlGetStatus: status: %d", status ); je n'étais pas sûr qu'en dessous ça fonctionne mais si
   // J'ai vu que l'on reste bloqué ici alors que le Redear à été supprimé ...
   if ( pReader != CL_NULL )
   {
	   DEBUG_PRINTF("cl_COMAlGetStatus: %s status: %s", pReader->tCOMParams.aucPortName, status == CL_TIMEOUT_ERR ? "TIMEOUT" : (status == CL_OK) ? "OK" : "ERROR" );
	   DEBUG_PRINTF("cl_COMAlGetStatus: %s END", pReader->tCOMParams.aucPortName);
   }
   else
   {
	   DEBUG_PRINTF("cl_COMAlGetStatus: pReader == CL_NULL: END");
   }

   return ( status );
/*   if ( fWaitingOnStat == TRUE )
	   return ( CL_OK );
   else
	   return ( CL_ERROR );*/
}

/*--------------------------------------------------------------------------*\
 *
\*--------------------------------------------------------------------------*/
e_Result cl_ReaderAndStatusProc( clvoid *ptReader )
{
	OVERLAPPED 	osReader = {0};  // overlapped structure for read operations
    OVERLAPPED 	osStatus = {0};  // overlapped structure for status operations
    HANDLE     	hArray[NUM_READSTAT_HANDLES];
	DWORD      	dwStoredFlags 	= 0xFFFFFFFF;      // local copy of event flags
	DWORD      	dwCommEvent;     // result from WaitCommEvent
	DWORD      	dwOvRes;         // result from GetOverlappedResult
	DWORD 	   	dwRead;          // bytes actually read
	DWORD      	dwRes;           // result from WaitForSingleObject
	BOOL       	fWaitingOnRead 	= FALSE;
	BOOL       	fWaitingOnStat 	= FALSE;
	BOOL       	fThreadDone 	= FALSE;
	char 		lpBuf[AMOUNT_TO_READ];

	t_clContext *pCtxt 		= CL_NULL;
	e_Result 	status 		= CL_ERROR;
	t_Reader	*pReader	= ptReader;
	e_State		eState;

	DWORD dwErrors;
	COMSTAT ComStatNew;

	HANDLE hPortCOM = (HANDLE)pReader->u64Handle;

	/* check incoming parameters */
	if ( pReader == CL_NULL )
	{
		DEBUG_PRINTF("cl_COMAlGetDataWin32: pReader == CL_NULL");
		return ( CL_PARAMS_ERR );
	}

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED( status =  cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	//
	// Create two overlapped structures, one for read events
	// and another for status events
	//
	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osReader.hEvent == NULL)
	{
		DEBUG_PRINTF("cl_COMAlGetDataWin32: ERROR osReader.hEvent == NULL");
		return CL_ERROR_IN_COMM;
	}

	osStatus.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osStatus.hEvent == NULL)
	{
		DEBUG_PRINTF("cl_COMAlGetDataWin32: ERROR osStatus.hEvent == NULL");
		return CL_ERROR_IN_COMM;
	}

	//
	// Status message event
	//
	pReader->tCOMParams.handleStatusMessage = CreateEvent( NULL, FALSE, FALSE, NULL );
	if ( pReader->tCOMParams.handleStatusMessage == NULL )
	{
		DEBUG_PRINTF("CreateEvent (Status message event)");
		return CL_ERROR_IN_COMM;
	}

    //
    // We want to detect the following events:
    //   Read events (from ReadFile)
    //   Status events (from WaitCommEvent)
    //   Status message events (from our UpdateStatus)
    //   Thread exit events (from our shutdown functions) BRY_NO
    //
    hArray[0] = osReader.hEvent;
    hArray[1] = osStatus.hEvent;
    hArray[2] = pReader->tCOMParams.handleStatusMessage;
    hArray[3] = pReader->tCOMParams.handleThreadExitEvent;

    //
    // initial check, forces updates
    //
    //CheckModemStatus(TRUE);
    DWORD dwNewModemStatus;
    if ( !GetCommModemStatus(hPortCOM, &dwNewModemStatus) )
    	ErrorReporter("GetCommModemStatus");

    // CheckComStat(TRUE);
    if ( !ClearCommError(hPortCOM, &dwErrors, &ComStatNew) )
        ErrorReporter("ClearCommError");

    while ( !fThreadDone )
    {
		//
		// Wait for a connected reader
		//
		if ( pReader->eState != STATE_CONNECT )
		{
			DEBUG_PRINTF1("cl_ReaderAndStatusProc_%s: READER_NOT_CONNECTED", pReader->tCOMParams.aucPortName);
			pCtxt->ptHalFuncs->fnWaitMs( 1000 );

			continue; // return waiting
		}

		if ( !fWaitingOnRead )
		{
			if ( !ReadFile( hPortCOM, lpBuf, AMOUNT_TO_READ, &dwRead, &osReader ) )
			{
				DWORD err = GetLastError();
				if (err != ERROR_IO_PENDING )     // read not delayed?
				{
					DEBUG_PRINTF("cl_ReaderAndStatusProc: GetLastError() != ERROR_IO_PENDING: ERROR_IN_COMM: 0x%X", err );
					return CL_ERROR_IN_COMM;
				}

				fWaitingOnRead = TRUE;
			}
			else
			{
				// read completed immediately
				if ( dwRead )
				{
					// OutputABuffer
#ifdef BUFFER_MODE_RAW
					char *buffString;
					csl_malloc( (void **)&buffString, 2048 );
					strnset(buffString, EOS, 1024);
					PrintBuffer(buffString, (clu8*)lpBuf, dwRead, "%02X ");
					DEBUG_PRINTF("<--- Mode_Raw : [port:%s] [len:%d] %s", pReader->tCOMParams.aucPortName, dwRead, buffString);
					pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&buffString);
#endif
#ifdef BUFFER_NETWORKS_TUPLES
					DEBUG_PRINTF1("<------ cl_ReaderAndStatusProc: immediately_received_%s: [oct:%d] [etime:%d]", pReader->tCOMParams.aucPortName, (int)dwRead, cl_GetElapsedTime() );

					status = cl_NetworkToTuples( pReader, (clu8*)lpBuf, (cl32)dwRead, &(pReader->p_TplList2Read) );

//					status = cl_ReaderFreeListOfTuples2Read( &(pReader->p_TplList2Read) );

					status = cl_ReaderReadThreadTraitement( pReader, &(pReader->p_TplList2Read) );
#endif
				}
			}

			TRACE_INTERNAL_THREAD_EVENT("cl_ReaderAndStatusProc: fWaitingOnRead == %s", fWaitingOnRead == FALSE ? "FALSE" : "TRUE");
		}

        //
        // If status flags have changed, then reset comm mask.
        // This will cause a pending WaitCommEvent to complete
        // and the resultant event flag will be NULL.
        //
		if ( dwStoredFlags != (EVENTFLAGS_DEFAULT) )
		{
			dwStoredFlags = (EVENTFLAGS_DEFAULT);
			if ( SetCommMask( hPortCOM, dwStoredFlags) == 0 )
			{
				// error setting communications mask; abort
				DEBUG_PRINTF("cl_ReaderAndStatusProc: Setting Communications Mask: ERROR");
				return CL_ERROR_IN_COMM;
			}

			TRACE_INTERNAL_THREAD_EVENT("cl_ReaderAndStatusProc: SetCommMask(newStoredFlags)");
		}

		//
		// if no status check is outstanding, then issue another one
		//
		if ( !fWaitingOnStat )
		{
			if ( !WaitCommEvent( hPortCOM, &dwCommEvent, &osStatus) )
			{
				DWORD err = GetLastError();
				if ( err != ERROR_IO_PENDING )	  // Wait not delayed?
				{
					DEBUG_PRINTF("cl_ReaderAndStatusProc: WaitCommEvent Error: 0x%04X", err);
				}
				else
					fWaitingOnStat = TRUE;
			}
			else
			{
				// WaitCommEvent returned immediately
				DEBUG_PRINTF("cl_COMAlGetDataWin32: ReportStatusEvent: !fWaitingOnStat");
				ReportStatusEvent(pReader, dwCommEvent);
			}

			TRACE_INTERNAL_THREAD_EVENT("cl_ReaderAndStatusProc: fWaitingOnStat == %s", fWaitingOnStat == FALSE ? "FALSE" : "TRUE");
		}

		//
		// wait for pending operations to complete
		//
		if ( fWaitingOnStat && fWaitingOnRead )
		{
			dwRes = WaitForMultipleObjects( NUM_READSTAT_HANDLES, hArray, FALSE, STATUS_CHECK_TIMEOUT );
			switch( dwRes )
			{
				//
				// read completed
				//
				case WAIT_OBJECT_0:

					if ( !GetOverlappedResult( hPortCOM, &osReader, &dwRead, FALSE) )
					{
						if ( GetLastError() == ERROR_OPERATION_ABORTED )
						{
							DEBUG_PRINTF("cl_COMAlGetDataWin32: GetOverlappedResult: GetLastError() == ERROR_OPERATION_ABORTED");
							UpdateStatus(pReader, "Read Aborted");
						}
						else
						{
							DEBUG_PRINTF("cl_COMAlGetDataWin32: GetOverlappedResult: GetLastError() != ERROR_OPERATION_ABORTED");
							//ErrorInComm("GetOverlappedResult (in Reader)"); // exit processus !!!

							status = CL_ERROR_IN_COMM;
						}
					}
					else
					{
						status = CL_OK;

						// read completed successfully
						if ( dwRead )
						{
							// OutputABuffer
#ifdef BUFFER_MODE_RAW
							char *buffString;
							csl_malloc( (void **)&buffString, 2048 );
							strnset(buffString, EOS, 1024);
							PrintBuffer(buffString, (clu8*)lpBuf, dwRead, "%02X ");
							DEBUG_PRINTF("<--- Mode_Raw : [port:%s] [len:%d] %s", pReader->tCOMParams.aucPortName, dwRead, buffString);
							pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&buffString);
#endif
#ifdef BUFFER_NETWORKS_TUPLES
							DEBUG_PRINTF1("<------ cl_ReaderAndStatusProc: complete_received_%s: [oct:%d] [etime:%d]", pReader->tCOMParams.aucPortName, (int)dwRead, cl_GetElapsedTime()  );

							status = cl_NetworkToTuples( pReader, (clu8*)lpBuf, (cl32)dwRead, &(pReader->p_TplList2Read) );

//							status = cl_ReaderFreeListOfTuples2Read( &(pReader->p_TplList2Read) );

							status = cl_ReaderReadThreadTraitement( pReader, &(pReader->p_TplList2Read) );
#endif

						}
					}

					TRACE_INTERNAL_THREAD_EVENT("cl_ReaderAndStatusProc: read_completed");

					fWaitingOnRead = FALSE;
					break;

				//
				// status completed
				//
				case WAIT_OBJECT_0 + 1:

					status = CL_OK;

					if ( !GetOverlappedResult( hPortCOM, &osStatus, &dwOvRes, FALSE) )
					{
						if ( GetLastError() == ERROR_OPERATION_ABORTED )
						{
							UpdateStatus( pReader, "WaitCommEvent aborted");
						}
						else
						{
							DEBUG_PRINTF("cl_COMAlGetDataWin32: GetOverlappedResult: GetLastError() != ERROR_OPERATION_ABORTED");
							// ErrorInComm("GetOverlappedResult (in Reader)");
							status = CL_ERROR_IN_COMM;
						}
					}
					else
					{
						// status check completed successfully
						DEBUG_PRINTF("cl_COMAlGetDataWin32: ReportStatusEvent: WAIT_OBJECT_0 + 1");
						ReportStatusEvent(pReader, dwCommEvent);

						// BRY_01102015 - Tell the upper layer Reader gets a problem
	//					DEBUG_PRINTF("cl_COMAlGetDataWin32: pReader->eState = STATE_DISCONNECT");
	//					pReader->eState = STATE_DISCONNECT;

						// BRY_02102015 - j'aimerais plutot faire ça
//						status = CL_ERROR_IN_COMM;
//						if ( ( dwCommEvent & EV_RING ) == EV_RING )
//						{
//							DEBUG_PRINTF("ReportStatusEvent: EV_RING Reader: STATE_DISCONNECT");
//							pReader->eState = STATE_DISCONNECT;
//
//							// Nothing can be done here on the Reader like cl_ReaderRemoveFromList because
//							// it will be thread suicide
//						}
//						else
//						{
//							DEBUG_PRINTF("ReportStatusEvent: dwCommEvent & EV_RING != EV_RING: %d", dwCommEvent );
//						}
					}

					fWaitingOnStat = FALSE;
					TRACE_INTERNAL_THREAD_EVENT("cl_ReaderAndStatusProc: status_completed");

					break;

				//
				// status message event
				//
				case WAIT_OBJECT_0 + 2:

					TRACE_INTERNAL_THREAD_EVENT("cl_ReaderAndStatusProc: StatusMessage Event");
	//                  BRY_DONT  StatusMessage(); On ne peut pas faire ca ... C'est trop Win32 !!!

					status = CL_OK;

					break;

				//
				// thread exit event
				//
				case WAIT_OBJECT_0 + 3:

					fThreadDone = TRUE;
					DEBUG_PRINTF("ReaderAndStatusProc: EXIT_EVENT");

					break;

				case WAIT_TIMEOUT:

					//
					// if status checks are not allowed, then don't issue the
					// modem status check nor the com stat check
					//
	//				if (!NOSTATUS(TTYInfo)) {
	//					CheckModemStatus(FALSE);    // take this opportunity to do
	//					CheckComStat(FALSE);        //   a modem status check and
	//												//   a comm status check
	//				}

					status = CL_TIMEOUT_ERR;

					NO_TRACE_INTERNAL_THREAD_EVENT("cl_ReaderAndStatusProc: WAIT_TIMEOUT");



					break;

				case WAIT_FAILED :
					ErrorReporter("cl_ReaderAndStatusProc: FAILED");
					break;

				default:
					ErrorReporter("cl_ReaderAndStatusProc: DEFAULT ???");
					break;
			}
		}
    }

    //
    // close event handles
    //
    CloseHandle(osReader.hEvent);
    CloseHandle(osStatus.hEvent);

	return status;
}

e_Result cl_COMAlGetDataWin32( clvoid *ptReader, t_Tuple **ppTuple )
{

	return CL_ERROR;
}

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
/*  In : clvoid *pReader : pointer to there reader where to send the data   */
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
e_Result ANOTHER_SHIT_THAT_STINKS_By_FD_cl_COMAlGetDataWin32( clvoid *ptReader, t_Tuple **pptTuple )
{
	OVERLAPPED 	osReader 	= {0};
	t_clContext *pCtxt 		= CL_NULL;
	e_Result 	status 		= CL_ERROR;
	cl64		cl64Len		= 0 ;
	clu8 		*pData		= CL_NULL;
	cl8			*pMsg		= CL_NULL;
	t_Reader	*pReader	= ptReader;
	DWORD 		dwRead;
	BOOL 		fWaitingOnRead = FALSE;
	DWORD 		dwRes 		= 0;

	COMSTAT comStat;
	DWORD   dwErrors;

	DWORD      dwStoredFlags = 0xFFFFFFFF;      // local copy of event flags

#define READ_TIMEOUT        500     // 500 milliseconds BRY_1804
#define SERIAL_BUF_SIZE		512 	// 1024

	/* check incoming parameters */
	if ( pReader == CL_NULL )
	{
		DEBUG_PRINTF("cl_COMAlGetDataWin32: pReader == CL_NULL");
		return ( CL_PARAMS_ERR );
	}

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL)
		return ( CL_ERROR );

	// TODO : BRY 18052015 ras le bol de ce code a la con
	status = cl_COMAlGetStatus( ptReader );
	if ( status == CL_TIMEOUT_ERR || status == CL_ERROR )
	{
		// Do not allocate if we are in Error or in Timeout
		//return status; just don't care !!! BRY_30042015
	}

	// allocate memory to receive data coming from underlayers
	//status = pCtxt->ptHalFuncs->fnAllocMem( ( clvoid **)&pData, SERIAL_BUF_SIZE );	// max size is doubled as we can have short buffers concatenated together.
	status = csl_malloc( ( clvoid **)&pData, SERIAL_BUF_SIZE );	// max size is doubled as we can have short buffers concatenated together.
	if ( CL_FAILED( status ))
    {
        DEBUG_PRINTF("cl_COMAlGetDataWin32: ERROR in fnAllocMem\n");
		return ( CL_MEM_ERR );
    }
    if ( pData == CL_NULL )
    {
        DEBUG_PRINTF("cl_COMAlGetDataWin32: ERROR pData == CL_NULL\n");
        return ( CL_MEM_ERR );
    }
	// initialize with 0
	memset( pData, 0, SERIAL_BUF_SIZE );


	// get some basic settings
	DCB serialParams = { 0 };
	serialParams.DCBlength = sizeof(serialParams);

	if ( !GetCommState(pReader->u64Handle, &serialParams) )
	{
		DEBUG_PRINTF("cl_COMAlGetDataWin32: ERROR in GetCommState\n");
	}

	COMMTIMEOUTS readtimeout = { 0 };
	if ( !GetCommTimeouts( pReader->u64Handle, &readtimeout ) )
	{
		DEBUG_PRINTF("cl_COMAlGetDataWin32: ERROR in GetCommTimeouts\n");
	}

	// Create the overlapped event. Must be closed before exiting
	// to avoid a handle leak.
	osReader.hEvent = CreateEvent( CL_NULL, TRUE, FALSE, CL_NULL);
    if ( osReader.hEvent == CL_NULL ) // Error creating overlapped event; abort.
	{
        DEBUG_PRINTF("cl_COMAlGetDataWin32: ERROR osReader.hEvent == NULL \n");
		status = CL_ERROR;
		return status;
	}

    if ( !fWaitingOnRead )
    {
		if ( !ReadFile(pReader->u64Handle, pData, SERIAL_BUF_SIZE, &dwRead, &osReader ) )
		{
			if ( GetLastError() != ERROR_IO_PENDING )     // read not delayed?
			{
				DEBUG_PRINTF("cl_COMAlGetDataWin32: GetLastError() == ERROR_IO_PENDING");
				status = CL_ERROR;

				fWaitingOnRead = TRUE;
			}
			else
			{
				dwRes = WaitForSingleObject( osReader.hEvent, READ_TIMEOUT );
				switch( dwRes )
				{
					// Read completed.
					case WAIT_OBJECT_0:
					{
					  if ( !GetOverlappedResult((HANDLE)pReader->u64Handle, &osReader, &dwRead, FALSE ) )
					  {
							status = CL_ERROR;
							// Error in communications; report it.
							DEBUG_PRINTF("cl_COMAlGetDataWin32: ERROR in communication");
					  }
					  else
					  {
							DEBUG_PRINTF("<------ cl_COMAlGetDataWin32: Received_%s: %x [etime:%d]", pReader->tCOMParams.aucPortName, (int)dwRead, cl_GetElapsedTime()  );

							// Read completed successfully.
							// parse the incoming buffer and prepare the tuple list for upper layer
							// the format of underlaying driver can concatenate several buffers together
							status = cl_HelperNetworkToTuples( pReader, pData, (cl32)dwRead, pptTuple );
					  }

					  break;

					  fWaitingOnRead = FALSE;
					}

					case WAIT_TIMEOUT:
					{
						// Operation isn't complete yet. fWaitingOnRead flag isn't
						// changed since I'll loop back around, and I don't want
						// to issue another read until the first one finishes.
						//
						// This is a good time to do some background work.
						DEBUG_PRINTF("cl_COMAlGetDataWin32: COM_TIMEOUT: %s", pReader->tCOMParams.aucPortName );
						// status = CL_OK; BRY_2204
						// Comme cl_HelperNetworkToTuples delègue le free en retournant la TupleList dans pptTuple
						// si on est en timeout, il ne faut rien faire donc pour l'appelant c'est sympa de tester
						// status. Je n'ai pas voulu prendre le risque d'ajouter une valeur dans l'enum e_Result
						// je me sers donc de CL_TIMEOUT_ERR mais ce n'est pas une ERREUR ...
						status = CL_TIMEOUT_ERR;
						break;
					}

					default:
					{
						status = CL_ERROR;
						// Error in the WaitForSingleObject; abort.
						// This indicates a problem with the OVERLAPPED structure's
						// event handle.
						break;
					}
				}
			}
		}
		else
		{
			// read completed immediately
			DEBUG_PRINTF("<------ cl_COMAlGetDataWin32: COM_received completed: %x [etime:%d]", (int)dwRead, cl_GetElapsedTime() );

			// parse the incoming buffer and prepare the tuple list for upper layer
			// the format of underlaying driver can concatenate several buffers together
			status = cl_HelperNetworkToTuples( pReader, pData, (cl32)dwRead, pptTuple );
		}
    }

	if ( CL_FAILED( status ) )
	{
		//
		// Error ou Timeout c'est la merde la comm tombe et c'est la fin des opérations ...
		// Il faut reseter le port tout le reste n'y fera rien
		//

		DEBUG_PRINTF("cl_COMAlGetDataWin32: ERROR ReadFile failed with : %x", (clu32)dwRes ); // 102L is for TIME_OUT
//		if ( FlushFileBuffers(pReader->u64Handle) )
//		{
//			DEBUG_PRINTF("cl_COMAlGetDataWin32: FlushFileBuffers : OK");
//		}
//		else
//		{
//			DEBUG_PRINTF("cl_COMAlGetDataWin32: FlushFileBuffers : NOT OK");
//		}

//		if ( ClearCommError(pReader->u64Handle, &dwErrors, &comStat) != 0 )
//		{
//			DEBUG_PRINTF("cl_COMAlGetDataWin32: ClearCommError : OK");
//		}
//		else
//		{
//			DEBUG_PRINTF("cl_COMAlGetDataWin32: ClearCommError : NOT OK");
//		}
//
//		if ( PurgeComm(pReader->u64Handle, PURGE_FLAGS) != 0 )
//		{
//			DEBUG_PRINTF("cl_COMAlGetDataWin32: PurgeComm : OK");
//		}
//		else
//		{
//			DEBUG_PRINTF("cl_COMAlGetDataWin32: PurgeComm : NOT OK");
//		}
	}

	// free unused memory as helper created tuple list and this buffer won't be used in upper layers
	if ( pData != CL_NULL )
	{
		pCtxt->ptHalFuncs->fnFreeMemSafely( ( clvoid **)&pData ); // BRY_FREE safe
	}

    // close handle to avoid leaks
    CloseHandle ( osReader.hEvent );

	// exit
	return ( status );
}

//
// OLD OLD OLD
//

//e_Result OLD_cl_COMAlGetDataWin32(  clvoid *ptReader, t_Tuple **pptTuple )
//{
//	t_clContext *pCtxt 		= CL_NULL;
//	e_Result 	status 		= CL_ERROR;
//	cl64		cl64Len		= 0 ;
//	clu8 		*pData		= CL_NULL;
//	cl8			*pMsg			= CL_NULL;
//	t_Reader	*pReader	= ptReader;
//	DWORD 		dwRead;
//	BOOL 		fWaitingOnRead = FALSE;
//	OVERLAPPED 	osReader = {0};
//	DWORD dwRes;
//
//#define READ_TIMEOUT      500      // milliseconds
//#define SERIAL_BUF_SIZE		1024
//
//	/* check incoming parameters */
//	if ( pReader == CL_NULL )
//		return ( CL_PARAMS_ERR );
//
//	// get context to have access to function pointers for memory/thread managment on platform
//	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
//		return ( CL_ERROR );
//
//	if ( pCtxt == CL_NULL)
//		return ( CL_ERROR );
//
//	if ( pCtxt->ptHalFuncs == CL_NULL )
//		return ( CL_ERROR );
//
//	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL)
//		return ( CL_ERROR );
//
//	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL)
//		return ( CL_ERROR );
//
//	if ( CL_FAILED( cl_COMAlGetStatus( ptReader ) ) )
//	{
//		if ( pptTuple != CL_NULL)
//		{
//			*pptTuple = CL_NULL;
//		}
//		return ( CL_OK );
//	}
//
//	// allocate memory to receive data coming from underlayers
//	//status = pCtxt->ptHalFuncs->fnAllocMem( ( clvoid **)&pData, SERIAL_BUF_SIZE );	// max size is doubled as we can have short buffers concatenated together.
//	status = csl_malloc( ( clvoid **)&pData, SERIAL_BUF_SIZE );	// max size is doubled as we can have short buffers concatenated together.
//	if ( CL_FAILED( status ))
//    {
//        DEBUG_PRINTF("cl_COMAlGetDataWin32: ERROR in fnAllocMem\n");
//		return ( CL_MEM_ERR );
//    }
//    if ( pData == CL_NULL )
//    {
//        DEBUG_PRINTF("cl_COMAlGetDataWin32: ERROR pData == CL_NULL\n");
//        return ( CL_MEM_ERR );
//    }
//	// initialize with 0
//	memset( pData, 0, SERIAL_BUF_SIZE );
//
//
//	// get some basic settings
//	DCB serialParams = { 0 };
//	serialParams.DCBlength = sizeof(serialParams);
//
//	if ( !GetCommState(pReader->u64Handle, &serialParams) )
//	{
//		DEBUG_PRINTF("cl_COMAlGetDataWin32: ERROR in GetCommState\n");
//	}
//
//	COMMTIMEOUTS readtimeout = { 0 };
//	if ( !GetCommTimeouts( pReader->u64Handle, &readtimeout ) )
//	{
//		DEBUG_PRINTF("cl_COMAlGetDataWin32: ERROR in GetCommTimeouts\n");
//	}
//
//
//	// Create the overlapped event. Must be closed before exiting
//	// to avoid a handle leak.
//	osReader.hEvent = CreateEvent( CL_NULL, TRUE, FALSE, CL_NULL);
//
//    if ( osReader.hEvent == CL_NULL )// Error creating overlapped event; abort.
//	{
//        DEBUG_PRINTF("cl_COMAlGetDataWin32: ERROR osReader.hEvent == NULL \n");
//		status = CL_ERROR;
//	}
//    else
//    {
//        if (!fWaitingOnRead)
//        {
//    //		DEBUG_PRINTF("READ:  Reader handle is %x\n", pReader->u64Handle );
//           // Issue read operation.
//           if (!ReadFile( (HANDLE) pReader->u64Handle, pData, SERIAL_BUF_SIZE, &dwRead, &osReader))
//           {
//              if (GetLastError() != ERROR_IO_PENDING)     // read not delayed?
//                 // Error in communications; report it.
//                  status = CL_ERROR;
//              else
//              {
//                   dwRes = WaitForSingleObject(osReader.hEvent, READ_TIMEOUT);
//                   switch(dwRes)
//                   {
//                      // Read completed.
//                      case WAIT_OBJECT_0:
//                      {
//                          if ( !GetOverlappedResult((HANDLE)pReader->u64Handle, &osReader, &dwRead, FALSE ) )
//                          {
//                              status = CL_ERROR;
//                             // Error in communications; report it.
//                              DEBUG_PRINTF("cl_COMAlGetDataWin32: ERROR in communication");
//                          }
//                          else
//                          {
//								DEBUG_PRINTF("<------ cl_COMAlGetDataWin32: COM_Received: %x [etime:%d]", (int)dwRead, cl_GetElapsedTime()  );
//								// Read completed successfully.
//								// parse the incoming buffer and prepare the tuple list for upper layer
//								// the format of underlaying driver can concatenate several buffers together
//								status = cl_HelperNetworkToTuples( pReader, pData, (cl32)dwRead, pptTuple );
//
//								//  Reset flag so that another opertion can be issued.
//								fWaitingOnRead = FALSE;
//
//                          }
//                          break;
//                      }
//                      case WAIT_TIMEOUT:
//                      {
//                          // Operation isn't complete yet. fWaitingOnRead flag isn't
//                          // changed since I'll loop back around, and I don't want
//                          // to issue another read until the first one finishes.
//                          //
//                          // This is a good time to do some background work.
//                          DEBUG_PRINTF("cl_COMAlGetDataWin32: COM %s Timeout", pReader->tCOMParams.aucPortName );
//                          status = CL_OK;
//                          break;
//                      }
//                      default:
//                      {
//                          status = CL_ERROR;
//                          // Error in the WaitForSingleObject; abort.
//                          // This indicates a problem with the OVERLAPPED structure's
//                          // event handle.
//                          break;
//                      }
//                   }
//              }
//           }
//           else
//           {
//				DEBUG_PRINTF("<------ cl_COMAlGetDataWin32: COM_received completed: %x [etime:%d]", (int)dwRead, cl_GetElapsedTime() );
//				// read completed immediately
//
//				// parse the incoming buffer and prepare the tuple list for upper layer
//				// the format of underlaying driver can concatenate several buffers together
//				status = cl_HelperNetworkToTuples( pReader, pData, (cl32)dwRead, pptTuple );
//            }
//        }
//    }
//
//	if ( CL_FAILED( status ) )
//	{
//		DEBUG_PRINTF("cl_COMAlGetDataWin32: ERROR ReadFile failed with : %x \n", (clu32)dwRes );
//	}
//
//	// free unused memory as helper created tuple list and this buffer won't be used in upper layers
//	if ( pData != CL_NULL )
//		pCtxt->ptHalFuncs->fnFreeMem( (clvoid *)pData );
//
//    // close handle to avoid leaks
//    CloseHandle ( osReader.hEvent );
//
//	// exit
//	return ( status );
//}
//



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
e_Result cl_COMAlOpenConnectionWin32( clvoid *ptReader )
{
	t_clContext *pCtxt 		= CL_NULL;
	e_Result status 		= CL_ERROR;
	t_Reader *pReader		= ptReader;
	LPCSTR 	portname;
	DWORD  	accessdirection = ( GENERIC_READ | GENERIC_WRITE );
//	DWORD 	dwShareMode		= ( FILE_SHARE_READ | FILE_SHARE_WRITE );
	HANDLE 	hSerial 		= INVALID_HANDLE_VALUE;		// handle of the open com port
	cl8		*pMsg			= CL_NULL;
	cl8		clLocalCOMName[64];
	cl8		Trailer[]		= { 0x5C, 0x5C, 0x2E, 0x5C };	// content of Trailer to get COM access "\\.\" + portname
    DWORD   dwStoredFlags;

	// check params
	if ( pReader == CL_NULL )
		return ( CL_ERROR );

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	DEBUG_PRINTF("cl_COMAlOpenConnectionWin32: BEGIN");

	// check args
	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL)
		return ( CL_ERROR );

	if ( pReader == CL_NULL )
		return ( CL_ERROR );

	// assign portname
	memset( clLocalCOMName, 0, sizeof( clLocalCOMName ) );
	memcpy( clLocalCOMName, &Trailer[0], sizeof( Trailer) );
	memcpy( &clLocalCOMName[sizeof( Trailer )], pReader->tCOMParams.aucPortName, strlen( pReader->tCOMParams.aucPortName ) );

	portname = (LPCSTR)clLocalCOMName;

	// access to com port
	hSerial = CreateFile(
			portname,
			accessdirection,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			NULL );

	if ( hSerial == INVALID_HANDLE_VALUE )
	{
		errno = GetLastError();

		//if ( CL_SUCCESS( csl_pmalloc( (clvoid *)&pMsg, 256 ) ) )

/*		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,                 // It´s a system error
		                     NULL,                                      // No string to be formatted needed
		                     errno,                               // Hey Windows: Please explain this error!
		                     MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),  // Do it in the standard language
		                     pMsg,              			// Put the message here
		                     255,             // Number of bytes to store the message
		                     NULL);

		DEBUG_PRINTF("Com %s failed to open with %s \n", pReader->tCOMParams.aucPortName, pMsg );
*/
		// deallocate memory
		//pCtxt->ptHalFuncs->fnFreeMem( pMsg );

		DEBUG_PRINTF("cl_COMAlOpenConnectionWin32: Serial Port %s: errno: %s: ERROR", pReader->tCOMParams.aucPortName, strerror( errno ));
		status = CL_ERROR;
	}
	else
	{
		pReader->u64Handle = hSerial;
		DEBUG_PRINTF("cl_COMAlOpenConnectionWin32: Reader handle: 0x%x: OPEN", hSerial);

		DCB serialParams = { 0 };
		serialParams.DCBlength = sizeof(serialParams);

		if ( !GetCommState(pReader->u64Handle, &serialParams) )
		{
			DEBUG_PRINTF("cl_COMAlOpenConnectionWin32: Error in GetCommState");
		}

        switch ( pReader->tCOMParams.eBaudRate )
		{
			case CL_COM_BAUDRATE_4800: serialParams.BaudRate = CBR_4800;break;
			case CL_COM_BAUDRATE_9600: serialParams.BaudRate = CBR_9600;break;
			case CL_COM_BAUDRATE_19200: serialParams.BaudRate = CBR_19200;break;
			case CL_COM_BAUDRATE_38400: serialParams.BaudRate = CBR_38400;break;
			case CL_COM_BAUDRATE_57600: serialParams.BaudRate = CBR_57600;break;
			case CL_COM_BAUDRATE_115200: serialParams.BaudRate = CBR_115200;break;
			default:serialParams.BaudRate = 115200;break;

        }
		switch ( pReader->tCOMParams.eByteSize )
		{
			case CL_COM_BYTESIZE_5BITS : serialParams.ByteSize = DATABITS_5; break;
			case CL_COM_BYTESIZE_6BITS : serialParams.ByteSize = DATABITS_6; break;
			case CL_COM_BYTESIZE_7BITS : serialParams.ByteSize = DATABITS_7; break;
			case CL_COM_BYTESIZE_8BITS : serialParams.ByteSize = DATABITS_8; break;
			default:serialParams.ByteSize = DATABITS_8 ;break;
		}

		switch ( pReader->tCOMParams.eStopBits )
		{
			case CL_COM_STOPBITS_10BIT: serialParams.StopBits = ONESTOPBIT; break;
			case CL_COM_STOPBITS_15BIT: serialParams.StopBits = ONE5STOPBITS; break;
			case CL_COM_STOPBITS_20BIT: serialParams.StopBits = TWOSTOPBITS; break;
			default:serialParams.StopBits = STOPBITS_10; break;
		}

		switch ( pReader->tCOMParams.eParityBits )
		{
			case  CL_COM_PARITYBIT_NONE: serialParams.Parity = NOPARITY; break;
			case  CL_COM_PARITYBIT_ODD: serialParams.Parity = ODDPARITY; break;
			case  CL_COM_PARITYBIT_EVEN: serialParams.Parity = EVENPARITY; break;
			case  CL_COM_PARITYBIT_MARK: serialParams.Parity = MARKPARITY; break;
			case  CL_COM_PARITYBIT_SPACE: serialParams.Parity = SPACEPARITY; break;
			default:serialParams.Parity = PARITY_NONE; break;
		}

		// disable RTS on serial
		serialParams.fRtsControl	= RTS_CONTROL_DISABLE;
		serialParams.fDtrControl	= DTR_CONTROL_DISABLE;

		serialParams.fAbortOnError = TRUE;

		// BRY_16062015
		serialParams.fParity = TRUE;
		serialParams.XonLim = 0;
		serialParams.XoffLim = 0;

		if ( !SetCommState(pReader->u64Handle, &serialParams) )
		{
			errno = GetLastError();

			if ( CL_SUCCESS( csl_pmalloc( (clvoid *)&pMsg, 256 )))

			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,                 // It�s a system error
			                     NULL,                                      // No string to be formatted needed
			                     errno,                               // Hey Windows: Please explain this error!
			                     MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),  // Do it in the standard language
			                     pMsg,              			// Put the message here
			                     255,             // Number of bytes to store the message
			                     NULL);

			DEBUG_PRINTF("cl_COMAlOpenConnectionWin32: SetCommState: %s failed to open with %s \n", pReader->tCOMParams.aucPortName, pMsg );

			// deallocate memory
			pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&pMsg );
			status = CL_ERROR;
		}

		// BRY_30042015
//		// Set timeouts
//		COMMTIMEOUTS timeout = { 0 };
//		timeout.ReadIntervalTimeout = 50;
//		timeout.ReadTotalTimeoutConstant = 50;
//		timeout.ReadTotalTimeoutMultiplier = 10;
//		timeout.WriteTotalTimeoutConstant = 50;
//		timeout.WriteTotalTimeoutMultiplier = 10;
////		timeout.ReadTotalTimeoutConstant = 50;
////		timeout.ReadTotalTimeoutMultiplier = 50;
////		timeout.WriteTotalTimeoutConstant = 50;
////		timeout.WriteTotalTimeoutMultiplier = 10;

		// BRY_11052015
//		// instance an object of COMMTIMEOUTS.
//		COMMTIMEOUTS comTimeOut;
//		// Specify time-out between charactor for receiving.
//		comTimeOut.ReadIntervalTimeout = 3;
//		// Specify value that is multiplied
//		// by the requested number of bytes to be read.
//		comTimeOut.ReadTotalTimeoutMultiplier = 3;
//		// Specify value is added to the product of the
//		// ReadTotalTimeoutMultiplier member
//		comTimeOut.ReadTotalTimeoutConstant = 2;
//		// Specify value that is multiplied
//		// by the requested number of bytes to be sent.
//		comTimeOut.WriteTotalTimeoutMultiplier = 3;
//		// Specify value is added to the product of the
//		// WriteTotalTimeoutMultiplier member
//		comTimeOut.WriteTotalTimeoutConstant = 2;

		// BRY_11052015
		// it is really nuts, silly, stupid to play with
		// COMMTIMEOUTS without hardware specifications
		// so set COMMTIMEOUTS to the default ! Please ;)
		// ...

		if ( !SetCommTimeouts(pReader->u64Handle, &g_TimeoutsDefault) )
		{
			errno = GetLastError();

			if ( CL_SUCCESS( csl_pmalloc( (clvoid *)&pMsg, 256 )))

			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,                 // It�s a system error
								 NULL,                                      // No string to be formatted needed
								 errno,                               // Hey Windows: Please explain this error!
								 MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),  // Do it in the standard language
								 pMsg,              			// Put the message here
								 255,             // Number of bytes to store the message
								 NULL);

			DEBUG_PRINTF("SetCommTimeouts %s failed to open with %s \n", pReader->tCOMParams.aucPortName, pMsg );

			// deallocate memory
			pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&pMsg );
			status = CL_ERROR;
		}

		// Juste verify we can Get Timetout ... BRY
		COMMTIMEOUTS readtimeout = { 0 };
		GetCommTimeouts( pReader->u64Handle, &readtimeout );

//        dwStoredFlags = EV_BREAK | EV_CTS | EV_DSR | EV_ERR | EV_RING |\
//                       EV_RLSD | EV_RXCHAR | EV_RXFLAG ;

//        dwStoredFlags = EV_BREAK | EV_CTS | EV_DSR | EV_ERR | EV_RING |\
//                       EV_RLSD | EV_RXCHAR | EV_RXFLAG | EV_TXEMPTY ;

		// BRY_05102015
//        dwStoredFlags = EVENTFLAGS_DEFAULT;
//        if ( SetCommMask(pReader->u64Handle , dwStoredFlags) != 0)
//        {
//        	DEBUG_PRINTF("cl_COMAlOpenConnectionWin32: SetCommMask: 0x%X", dwStoredFlags);
//        }
//        else
//        {
//           // error setting communications mask; abort
//        	DEBUG_PRINTF("cl_COMAlOpenConnectionWin32: setting communications mask: ERROR");
//        	return ( CL_ERROR );
//        }

		status = CL_OK;

	}

	DEBUG_PRINTF("cl_COMAlOpenConnectionWin32: END");
	return ( status );
}

/******************************************************************************/
/* Name :  e_Result (cl_COMAlCloseConnectionWin32)( clvoid * *pReader );  		*/
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
e_Result cl_COMAlCloseConnectionWin32( clvoid *ptReader )
{
	BOOL result;

	t_Reader *pReader = (t_Reader *)ptReader;

	// check params
	if ( ptReader == CL_NULL )
		return CL_ERROR;

	if ( pReader->u64Handle == INVALID_HANDLE_VALUE )
	{
		DEBUG_PRINTF("cl_COMAlCloseConnectionWin32: INVALID_HANDLE_VALUE");
		return CL_ERROR;
	}

	result = CloseHandle( (void *)pReader->u64Handle );
	if ( result == FALSE )
	{
		DEBUG_PRINTF("cl_COMAlCloseConnectionWin32: CloseHandle: NOT CLOSED");
		return CL_ERROR;
	}
	else
	{
		DEBUG_PRINTF("cl_COMAlCloseConnectionWin32: CloseHandle: CLOSED");
	}

	return CL_OK;
}

/*****************************************************************************/
/* e_Result cl_IPStackSupportLoadWin32( clvoid)								*/
/*																			*/
/* Description : load COM stack support on target OS							*/
/*****************************************************************************/
/* Return Value : 															*/
/*   OK                        :  Result is OK								*/
/*   ERROR,                    : Failure on execution						*/
/****************************************************************************/
e_Result cl_COMStackSupportLoadWin32( clvoid )
{
	DEBUG_PRINTF("cl_COMStackSupportLoadWin32: CL_OK");
	return CL_OK;
}
/*****************************************************************************/
/* e_Result cl_IPStackSupportUnloadWin32( clvoid)								*/
/*																			*/
/* Description : unload COM stack support on target OS						*/
/*****************************************************************************/
/* Return Value : 															*/
/*   OK                        :  Result is OK								*/
/*****************************************************************************/
e_Result cl_COMStackSupportUnloadWin32( clvoid )
{
	return ( CL_OK );
}
