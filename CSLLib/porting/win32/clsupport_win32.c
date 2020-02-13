/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: clsupport_win32.c                                                  */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/* Description: holding the functions used by CSL as wait/semaphore/thread   */
/*****************************************************************************/

#include "..\..\csl.h"
#include "..\..\clsupport.h"
#include "clsupport_win32.h"
#include "..\..\inc\generic.h"
#include "..\..\inc\clhelpers.h"

#include <windows.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>
#include <conio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include <DbgHelp.h>

/*--------------------------------------------------------------------------*/

#define NO_DEBUG_SEMAPHORE_WAIT

/*--------------------------------------------------------------------------*/


/******************************************************************************/
/* Name :  e_Result (cl_IOResetWin32_cb)(void)                                   */
/* Description : Reset completion received from underlayers                   */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  @param 	*pReader					( In ) Reader where the reset applies		*/
/*  @param	status						( In ) status of the reset				*/
/*  @return e_Result															*/
/*  Out:                                                                      */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                 */
/*  ERROR,                    : Failure on execution                          */
/*  MEM_ERR,                  :  Failure on memory management (failure,       */
/*                                  allocation ....)                          */
/*  TIMEOUT_ERR,              :  Overrun on timing                            */
/******************************************************************************/
e_Result cl_IOResetWin32_cb( clvoid *pReader, e_Result status )
{
	t_Reader 	*ptLocalReader = ( t_Reader *)pReader;

	DEBUG_PRINTF("cl_IOResetWin32_cb: BEGIN");

	if ( pReader == CL_NULL )
	{
		DEBUG_PRINTF("LOW LEVEL OS SUPPORT (IP/BT/COM) was reset, all readers which could have been connected are in an unknown state...!!!!! \n");
	}
	else
	{
		if ( ptLocalReader->tType & IP_READER_TYPE )
			DEBUG_PRINTF("connection to IP reader %s was reset  \n", ptLocalReader->tIPParams.aucIpAddr );

		if ( ptLocalReader->tType & COM_READER_TYPE )
			DEBUG_PRINTF("connection to COM reader %s was reset  \n", ptLocalReader->tCOMParams.aucPortName );

		if ( ptLocalReader->tType & BT_READER_TYPE )
			DEBUG_PRINTF("connection to BT reader %d was reset  \n", ptLocalReader->tBT.u8Unused );
	}
	return ( CL_OK );
}


/******************************************************************************/
/* Name :  e_Result cl_IOSendDataDoneWin32_cb( clvoid *pReader, clvoid *pTuple, e_Result status)  */
/* Description : Data completion callback sent by porting layer               */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  @param 	*pReader					( In ) Reader which issued the data		*/
/*  @param	*pTuple						( In ) Tuple send to network			*/
/*  @param	status						( In ) status of the send to network	*/
/*  @return e_Result															*/
/*  Out:                                                                      */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                 */
/*  ERROR,                    : Failure on execution                          */
/*  MEM_ERR,                  :  Failure on memory management (failure,       */
/*                                  allocation ....)                          */
/*  PARAMS_ERR,               :  Inconsistent parameters                      */
/*  TIMEOUT_ERR,              :  Overrun on timing                            */
/******************************************************************************/

/*
 * utilis�e par l'interface utilisateur
 */

e_Result cl_IOSendDataDoneWin32_cb( clvoid *pReader, clvoid *pTuple, e_Result status )
{
	e_Result 	eStatus = CL_ERROR;
	t_Reader 	*ptLocalReader = ( t_Reader *)pReader;
	t_Tuple 	*pLocalTuple = ( t_Tuple *)pTuple;

	int lgBuffer = 0;

	DEBUG_PRINTF1("cl_IOSendDataDoneWin32_cb: BEGIN");

	// Check parameters
	if ( ( pReader == CL_NULL ) | ( pTuple == CL_NULL ) )
	{
		if ( pReader == CL_NULL )
		{
			DEBUG_PRINTF("cl_IOSendDataDoneWin32_cb: pReader == CL_NULL: Error in parameters !");
		}
		if ( pTuple == CL_NULL )
		{
			DEBUG_PRINTF("cl_IOSendDataDoneWin32_cb: pTuple == CL_NULL: Error in parameters !");
		}
		return ( eStatus );
	}

	if ( ptLocalReader->tType & IP_READER_TYPE )
	{
		if ( CL_FAILED( status ) )
		{
			DEBUG_PRINTF("\n %s : ---> Data to Network Failed on reader %s with Tuple Len %d\n", pLocalTuple->cl8Time, ptLocalReader->tIPParams.aucIpAddr, pLocalTuple->ptBuf->ulLen );
		}
		else
		{
			DEBUG_PRINTF("\n %s : ---> To Reader %s Tuple Len %d\n", pLocalTuple->cl8Time, ptLocalReader->tIPParams.aucIpAddr, ((t_Tuple *)pTuple)->ptBuf->ulLen );
		}

	}

	if  ( ptLocalReader->tType & COM_READER_TYPE )
	{
		if ( CL_FAILED( status ) )
		{
			DEBUG_PRINTF("%s: ---> Data to Network Failed on reader %s with Tuple len %d\n", pLocalTuple->cl8Time, ptLocalReader->tCOMParams.aucPortName, pLocalTuple->ptBuf->ulLen );
		}
		else
		{
			if ( pLocalTuple->ptBuf->ulLen > 0 )
			{
//				char buffString[128]; //[SIZE_MAX_BUFFER];
//				strset(buffString, EOS);

				lgBuffer = pLocalTuple->ptBuf->ulLen * 3;
				char *buffString;
				csl_malloc((void **)&buffString, lgBuffer + 1);
				strnset(buffString, EOS, lgBuffer + 1);

				PrintBuffer(buffString, pLocalTuple->ptBuf->pData, pLocalTuple->ptBuf->ulLen, "%02X ");

				DEBUG_PRINTF("---> To_Reader [time:%s] [name:%s] [len: %d] %s", pLocalTuple->cl8Time, ptLocalReader->tCOMParams.aucPortName, ((t_Tuple *)pTuple)->ptBuf->ulLen, buffString );

				g_tCtxt.ptHalFuncs->fnFreeMemSafely( (void **)&buffString );
			}
		}
	}

	if  ( ptLocalReader->tType & BT_READER_TYPE )
	{
		if ( CL_FAILED( status ) )
		{
			DEBUG_PRINTF("%s: ---> Data to Network Failed on reader %x with Tuple Len %d\n", pLocalTuple->cl8Time, ptLocalReader->tBT.u8Unused, (clu8)pLocalTuple->ptBuf->ulLen );
		}
		else
		{
			DEBUG_PRINTF("%s: ---> To Reader %x Tuple Len %d\n", pLocalTuple->cl8Time, ptLocalReader->tBT.u8Unused, (clu8)((t_Tuple *)pTuple)->ptBuf->ulLen );
		}
	}

	DEBUG_PRINTF1("cl_IOSendDataDoneWin32_cb: END");
	return ( CL_OK );
}


/******************************************************************************/
/* Name :  e_Result cl_IOData2ReadWin32_cb(void *pReader, clvoid *pTuple, e_Result status)  */
/* Description : Data received from the host                                  */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : none                                                                 */
/* ---------------                                                            */
/*  @param 	*pReader					( In ) Reader which issued the data		*/
/*  @param	*pTuple						( In ) Tuple from network			*/
/*  @param	status						( In ) status of the recv from network	*/
/* 	@return value: e_Result                                                      */
/*  OK                        :  Result is OK                                 */
/*  ERROR,                    : Failure on execution                          */
/*  MEM_ERR,                  :  Failure on memory management (failure,       */
/*                                  allocation ....)                          */
/*  PARAMS_ERR,               :  Inconsistent parameters                      */
/*  TIMEOUT_ERR,              :  Overrun on timing                            */
/******************************************************************************/
e_Result cl_IOData2ReadWin32_cb( clvoid *pReader, clvoid *pTuple, e_Result status )
{
	e_Result 	eStatus = CL_ERROR;
	t_Reader 	*ptLocalReader = ( t_Reader *)pReader;
	t_Tuple 	*pLocalTuple = ( t_Tuple *)pTuple;

	DEBUG_PRINTF1("cl_IOData2ReadWin32_cb: BEGIN");

	if (( pReader == CL_NULL ) | ( pTuple == CL_NULL ))
	{
		DEBUG_PRINTF("Error in parameters for cl_IOData2ReadWin32_cb");
		return ( eStatus );
	}

	if ( ptLocalReader->tType & IP_READER_TYPE )
	{
		if ( CL_FAILED( status ) )
		{
			DEBUG_PRINTF("%s:-> From IP Reader Failed on reader %s with %d", pLocalTuple->cl8Time, ptLocalReader->tIPParams.aucIpAddr, pLocalTuple->ptBuf->ulLen );
		}
		else
		{
			DEBUG_PRINTF("%s:<- From IP Reader %s Tuple.lg:%d", pLocalTuple->cl8Time, ptLocalReader->tIPParams.aucIpAddr, ((t_Tuple *)pTuple)->ptBuf->ulLen );
		}

	}

	if  ( ptLocalReader->tType & BT_READER_TYPE )
	{
		if ( CL_FAILED( status ) )
		{
			DEBUG_PRINTF("%s:-> From BT Reader Failed on reader %s with %d", pLocalTuple->cl8Time, ptLocalReader->tCOMParams.aucPortName, pLocalTuple->ptBuf->ulLen );
		}
		else
		{
			DEBUG_PRINTF("%s:<- From BT Reader %s Tuple %d", pLocalTuple->cl8Time, ptLocalReader->tCOMParams.aucPortName, ((t_Tuple *)pTuple)->ptBuf->ulLen );
		}
	}

	if  ( ptLocalReader->tType & COM_READER_TYPE )
	{
		if ( CL_FAILED( status ) )
		{
			DEBUG_PRINTF("CL_FAILED( status )");
			DEBUG_PRINTF("<--- From_Reader [port:%s] [time:%s] Failed %d", pLocalTuple->cl8Time, ptLocalReader->tCOMParams.aucPortName, (clu8)(pLocalTuple->ptBuf->ulLen) );
		}
		else
		{
			if ( pLocalTuple->ptBuf == CL_NULL )
			{
				DEBUG_PRINTF("Error : pLocalTuple->ptBuf == CL_NULL");
				return CL_MEM_ERR;
			}

			// display received data
			if ( pLocalTuple->ptBuf->ulLen > 0 )
			{
				char buffString[SIZE_MAX_BUFFER];
//				char buffString[128];
				strset(buffString, EOS);

				PrintBuffer(buffString, pLocalTuple->ptBuf->pData, pLocalTuple->ptBuf->ulLen, "%02X ");

				DEBUG_PRINTF("<--- From_Reader [port:%s] [time:%s] [len:%d] %s", pLocalTuple->cl8Time, ptLocalReader->tCOMParams.aucPortName, pLocalTuple->ptBuf->ulLen, buffString);

//				g_tCtxt.ptHalFuncs->fnFreeMem(buffString);
			}
			else
			{
				DEBUG_PRINTF("cl_IOData2ReadWin32_cb:Probleme with ulLen is:%d", pLocalTuple->ptBuf->ulLen);
			}
		}
	}

	DEBUG_PRINTF1("cl_IOData2ReadWin32_cb: END");
	return CL_OK;
}
/******************************************************************************/
/* Name :  e_Result cl_IOStateWin32_cb( clvoid *pCtxt, clvoid *ptReader			*/
/*				, clvoid *ptDevice, e_Result status )  							*/
/* Description : Update State of Framework/Device/Reader                       	*/
/*			if pointer is not CL_NULL, then it indicates the originator of the 	*/
/*			update																*/
/*                                                                            	*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : none                                                                 	*/
/* ---------------                                                            	*/
/*  @param 	*pCtxt					( In ) framework pointer					*/
/*  @param	*ptReader				( In ) reader 								*/
/*  @param	*ptDevice				( In ) Pointer to Device					*/
/*  @param	status						( In ) status of the recv from network	*/
/* 	@return value: e_Result                                                     */
/*  OK                        :  Result is OK                                 	*/
/*  ERROR,                    : Failure on execution                          	*/
/*  MEM_ERR,                  :  Failure on memory management (failure,       	*/
/*                                  allocation ....)                          	*/
/*  PARAMS_ERR,               :  Inconsistent parameters                      	*/
/*  TIMEOUT_ERR,              :  Overrun on timing                            	*/
/******************************************************************************/
e_Result cl_IOStateWin32_cb( clvoid *pCtxt, clvoid *ptReader, clvoid *ptDevice, e_Result status )
{
	e_Result 	eStatus = CL_ERROR;
	t_Reader 	*pReader = ( t_Reader *)ptReader;
	t_Device 	*pDevice = ( t_Device *)ptDevice;
	cl8			*pDesc	=	CL_NULL;

	if ( ( !pCtxt ) & ( !pReader ) & ( !pDevice ) )
		return ( CL_ERROR );

	// check if the state change is coming from the framework
//	DEBUG_PRINTF("cl_IOStateWin32_cb: State update from Framework %x", status);

	// check if the state change is coming for a reader
	if ( pReader)
	{
		if (pReader->tType == COM_READER_TYPE )
			pDesc = pReader->tCOMParams.aucPortName;

		if ( pReader->tType == IP_READER_TYPE )
			pDesc = pReader->aucLabel;

		switch ( pReader->eState )
		{
			case ( STATE_INIT ):			/// indicates a reader is added to a list
                DEBUG_PRINTF("cl_IOStateWin32_cb: %s STATE INIT", pDesc );
				break;
			case ( STATE_CONNECT ):		/// asks to connect a reader to its IOs layer, indicates that a reader was successfully connected to its IOs layer
                DEBUG_PRINTF("cl_IOStateWin32_cb: %s: STATE CONNECT", pDesc );
				break;
			case ( STATE_DISCONNECT ):	/// asks to disconnect a reader from its IOs layer, indicates that a reader was successfully disconnected from its IOs layer
                DEBUG_PRINTF("cl_IOStateWin32_cb: %s: STATE DISCONNECT", pDesc );
				break;
			case ( STATE_ERROR ):		/// indicates that a reader got an error from its underlaying IOs layer
                DEBUG_PRINTF("cl_IOStateWin32_cb: %s: STATE ERROR", pDesc );
				break;
			case ( STATE_DISCOVER ):
				DEBUG_PRINTF("cl_IOStateWin32_cb: %s: STATE DISCOVER", pDesc );
				break;
			default:
                DEBUG_PRINTF("cl_IOStateWin32_cb: %s: ERROR : Unknown State Reader !?", pDesc );
				break;

		}
	}

	// check if the state change is coming from a device
	if ( pDevice )
	{
		switch ( pDevice->eState )
		{
		case ( STATE_INIT ):			/// indicates a reader is added to a list
			DEBUG_PRINTF("State update from Device %s STATE_INIT \n", pDevice->aucLabel );
			break;
		case ( STATE_CONNECT ):		/// asks to connect a reader to its IOs layer, indicates that a reader was successfully connected to its IOs layer
			DEBUG_PRINTF("State update from Device %s: STATE CONNECT", pDevice->aucLabel );
			break;
		case ( STATE_DISCONNECT ):	/// asks to disconnect a reader from its IOs layer, indicates that a reader was successfully disconnected from its IOs layer
			DEBUG_PRINTF("State update from Device %x", pDevice->aucLabel );
			break;
		case ( STATE_ERROR ):		/// indicates that a reader got an error from its underlaying IOs layer
			DEBUG_PRINTF("State update from Device %x", pDevice->aucLabel );
			break;
		default:
			DEBUG_PRINTF("Unknown State update from Device %s", pDevice->aucLabel );
			break;
		}
	}

	return ( status );

}

/******************************************************************************/
/* Name :  e_Result cl_DiscoverSendDataDoneWin32_cb( clvoid *pReader, clvoid *pTuple, e_Result status)  */
/* Description : Data completion callback sent by porting layer during discover	*/
/*                                                                            	*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  @param 	*pReader					( In ) Reader which issued the data		*/
/*  @param	*pTuple						( In ) Tuple send to network			*/
/*  @param	status						( In ) status of the send to network	*/
/*  @return e_Result															*/
/*  Out:                                                                      	*/
/* Return value: e_Result                                                      	*/
/*  OK                        :  Result is OK                                 	*/
/*  ERROR,                    : Failure on execution                          	*/
/*  MEM_ERR,                  :  Failure on memory management (failure,       	*/
/*                                  allocation ....)                          	*/
/*  PARAMS_ERR,               :  Inconsistent parameters                      	*/
/*  TIMEOUT_ERR,              :  Overrun on timing                            	*/
/******************************************************************************/
e_Result cl_DiscoverSendDataDoneWin32_cb( clvoid *pReader, clvoid *pTuple, e_Result status )
{
	e_Result 	eStatus = CL_ERROR;
	t_Reader 	*ptLocalReader = ( t_Reader *)pReader;
	t_Tuple 	*pLocalTuple = ( t_Tuple *)pTuple;

	DEBUG_PRINTF("cl_DiscoverSendDataDoneWin32_cb: BEGIN");

	if (( pReader == CL_NULL ) | ( pTuple == CL_NULL ))
	{
		DEBUG_PRINTF("Error in parameters for cl_NetwSendDataDoneWin32_cb \n");
		return ( eStatus );
	}

	if ( ptLocalReader->tType & IP_READER_TYPE )
	{
		if ( CL_FAILED( status ) )
		{
			DEBUG_PRINTF("%s : -> Data to Network Failed on reader %s with Tuple Len %d\n", pLocalTuple->cl8Time, ptLocalReader->tIPParams.aucIpAddr, pLocalTuple->ptBuf->ulLen );
		}
		else
		{
			DEBUG_PRINTF("%s : -> To Reader %s Tuple Len %d\n", pLocalTuple->cl8Time, ptLocalReader->tIPParams.aucIpAddr, ((t_Tuple *)pTuple)->ptBuf->ulLen );
		}

	}

	if  ( ptLocalReader->tType & COM_READER_TYPE )
	{
		if ( CL_FAILED( status ) )
		{
			DEBUG_PRINTF("%s : -> Data to Network Failed on reader %s withTuple Len %d\n", pLocalTuple->cl8Time, ptLocalReader->tCOMParams.aucPortName, pLocalTuple->ptBuf->ulLen );
		}
		else
		{
			DEBUG_PRINTF("%s : -> To Reader %s Tuple Len %d\n", pLocalTuple->cl8Time, ptLocalReader->tCOMParams.aucPortName, ((t_Tuple *)pTuple)->ptBuf->ulLen );
		}
	}

	if  ( ptLocalReader->tType & BT_READER_TYPE )
	{
		if ( CL_FAILED( status ) )
		{
				DEBUG_PRINTF("%s : -> Data to Network Failed on reader %x with Tuple Len %d\n", pLocalTuple->cl8Time, ptLocalReader->tBT.u8Unused, (clu8)pLocalTuple->ptBuf->ulLen );

		}
		else
		{
			DEBUG_PRINTF("%s : -> To Reader %x Tuple Len %d\n", pLocalTuple->cl8Time, ptLocalReader->tBT.u8Unused, (clu8)((t_Tuple *)pTuple)->ptBuf->ulLen );
		}
	}

	return ( CL_OK );
}
/******************************************************************************/
/* Name :  e_Result cl_DiscoverData2ReadWin32_cb(void *pReader, clvoid *pTuple, e_Result status)  */
/* Description : Data received from the host during Discover 					*/
/*                                                                            	*/
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : none                                                                 */
/* ---------------                                                            */
/*  @param 	*pReader					( In ) Reader which issued the data		*/
/*  @param	*pTuple						( In ) Tuple from network			*/
/*  @param	status						( In ) status of the recv from network	*/
/* 	@return value: e_Result                                                      */
/*  OK                        :  Result is OK                                 */
/*  ERROR,                    : Failure on execution                          */
/*  MEM_ERR,                  :  Failure on memory management (failure,       */
/*                                  allocation ....)                          */
/*  PARAMS_ERR,               :  Inconsistent parameters                      */
/*  TIMEOUT_ERR,              :  Overrun on timing                            */
/******************************************************************************/
e_Result cl_DiscoverData2ReadWin32_cb( clvoid *pReader, clvoid *pTuple, e_Result status )
{
	e_Result 	eStatus = CL_ERROR;
	t_Reader 	*ptLocalReader = ( t_Reader *)pReader;
	t_Tuple 	*pLocalTuple = ( t_Tuple *)pTuple;

	DEBUG_PRINTF("cl_DiscoverData2ReadWin32_cb");

	if (( pReader == CL_NULL ) | ( pTuple == CL_NULL ))
	{
		DEBUG_PRINTF("Error in parameters for cl_DiscoverData2ReadWin32_cb");
		return ( eStatus );
	}

	if ( ptLocalReader->tType & IP_READER_TYPE )
	{
		if ( CL_FAILED( status ) )
		{
				DEBUG_PRINTF("\n %s : -> From Reader Failed on reader %s with %d", pLocalTuple->cl8Time, ptLocalReader->tIPParams.aucIpAddr, pLocalTuple->ptBuf->ulLen );

		}
		else
		{
			DEBUG_PRINTF("%s : <- From Reader %s Tuple %d", pLocalTuple->cl8Time, ptLocalReader->tIPParams.aucIpAddr, ((t_Tuple *)pTuple)->ptBuf->ulLen );
			if ( pLocalTuple->ptBuf->ulLen == 16 )
            {
				memcpy( ptLocalReader->tCloverSense.au8RadioAddress, &pLocalTuple->ptBuf->pData[10], 6);
				// this is an asynchronous event... Re event state of the reader to update user data
				cl_ReaderSetState( ptLocalReader, ptLocalReader->eState );
            }
		}
	}

	if  ( ptLocalReader->tType & BT_READER_TYPE )
	{
		if ( CL_FAILED( status ) )
		{
			DEBUG_PRINTF("\n %s : -> From COM Reader Failed on reader %s with %d", pLocalTuple->cl8Time, ptLocalReader->tCOMParams.aucPortName, pLocalTuple->ptBuf->ulLen );
		}
		else
		{
			DEBUG_PRINTF("\n %s : <- From COM Reader %s Tuple %d", pLocalTuple->cl8Time, ptLocalReader->tCOMParams.aucPortName, ((t_Tuple *)pTuple)->ptBuf->ulLen );
		}
	}

	if  ( ptLocalReader->tType & COM_READER_TYPE )
	{
		if ( CL_FAILED( status ) )
		{
			DEBUG_PRINTF("%s : -> From COM Reader Failed on reader %x with %d", pLocalTuple->cl8Time, ptLocalReader->tBT.u8Unused, (clu8)(pLocalTuple->ptBuf->ulLen) );
		}
		else
		{
			DEBUG_PRINTF("%s : <- From Reader %x Tuple %d", pLocalTuple->cl8Time, ptLocalReader->tBT.u8Unused, (clu8)((t_Tuple *)pTuple)->ptBuf->ulLen );
		}
	}

	return ( CL_OK );
}
/******************************************************************************/
/* Name :  e_Result cl_ClbkDiscover_ReadDone(void *pReader, clvoid *pTuple, 	*/
/*							e_Result status)  									*/
/* Description : Data received from the host during an IP discover process		*/
/*                                                                            	*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : none                                                                 	*/
/* ---------------                                                            	*/
/*  @param 	*pReader					( In ) Reader which issued the data		*/
/*  @param	*pTuple						( In ) Tuple from network				*/
/*  @param	status						( In ) status of the recv from network	*/
/* 	@return value: e_Result                                                     */
/*  OK                        :  Result is OK                                 	*/
/*  ERROR,                    : Failure on execution                          	*/
/*  MEM_ERR,                  :  Failure on memory management (failure,       	*/
/*                                  allocation ....)                          	*/
/*  PARAMS_ERR,               :  Inconsistent parameters                      	*/
/*  TIMEOUT_ERR,              :  Overrun on timing                            	*/
/*******************************************************************************/
e_Result cl_ClbkDiscover_ReadDone( clvoid *ptReader, clvoid *ptTuple, e_Result eStatus )
{
	t_clContext *pCtxt 		= CL_NULL;
	e_Result 	status 		= CL_ERROR;
	t_Reader	*pReader 	= CL_NULL;
	t_Tuple		*pTuple 	= CL_NULL;
	t_Tuple		*pNextTuple	= CL_NULL;
	t_Reader	tLocalReader;
	t_Reader	*pReaderInList	= CL_NULL;
	cl8			aucTempName[16];
	clu8		u8Index = 0;
	cl32		cl32Temp	= 0;
	cl8			ac8IPDiscover[]= {0x01, 0x01, 0x01, 0x09, 0x10, 0x00};
	t_Tuple 	tTuple4Discover;
	clu8        *pDiscover              = CL_NULL;
	clu32 		u32TsfNb	= 0;

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );

	// check incoming parameters
	if (ptReader == CL_NULL )
		return ( status );

	if ( ptTuple == CL_NULL )
		return ( status );

	// get variables
	pReader 	= ( t_Reader *)ptReader;
	pTuple 		= ( t_Tuple *)ptTuple;

	// parse the incoming tuple to retreive the ip address
	if ( !pTuple->ptBuf )
		return ( CL_ERROR );

	if ( !pTuple->ptBuf->pData)
		return ( CL_ERROR );

	// parse incoming buffer which is 4 bytes long and contains the IP address of a detected reader
	switch ( pReader->tType )
	{
	  case (IP_READER_TYPE ):
	  {
		  if ( pTuple->ptBuf->ulLen >=  CSL_DISCOVER_PROTOCOL_LANTRONIX_IP_ADDR_LEN )
		  {
			 if ( CL_FAILED( cl_ReaderFillWithDefaultFields( &tLocalReader, IP_READER_TYPE ) ) )
				 return (CL_ERROR);

			// retreive the proper IP address
			memset( &tLocalReader.tIPParams.aucIpAddr[0], 0, sizeof( tLocalReader.tIPParams.aucIpAddr ));
			memset( aucTempName, 0, sizeof( aucTempName) );
			itoa( (clu32)(pTuple->ptBuf->pData[CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_0_OFFSET]), aucTempName, 10 );
			strcat( tLocalReader.tIPParams.aucIpAddr, aucTempName );
			strcat( tLocalReader.tIPParams.aucIpAddr, "." );
			memset( aucTempName, 0, sizeof( aucTempName) );
			itoa( (clu32)(pTuple->ptBuf->pData[CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_1_OFFSET]), aucTempName, 10 );
			strcat( tLocalReader.tIPParams.aucIpAddr, aucTempName );
			strcat( tLocalReader.tIPParams.aucIpAddr, "." );
			memset( aucTempName, 0, sizeof( aucTempName) );
			itoa( (clu32)(pTuple->ptBuf->pData[CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_2_OFFSET]), aucTempName, 10 );
			strcat( tLocalReader.tIPParams.aucIpAddr, aucTempName );
			strcat( tLocalReader.tIPParams.aucIpAddr, "." );
			memset( aucTempName, 0, sizeof( aucTempName) );
			itoa( (clu32)(pTuple->ptBuf->pData[CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_3_OFFSET]), aucTempName, 10 );
			strcat( tLocalReader.tIPParams.aucIpAddr, aucTempName );

			memset( tLocalReader.aucLabel, 0, sizeof( tLocalReader.aucLabel) );
			strcpy( (cl8*)tLocalReader.aucLabel, "IP_Reader" );
			strcat( tLocalReader.aucLabel, tLocalReader.tIPParams.aucIpAddr);

			tLocalReader.tIPParams.u32Port = IP_READER_DEFAULT_TCP_PORT;

			DEBUG_PRINTF("\nIP reader discovered %s\n", tLocalReader.aucLabel );
			status = cl_readerAddToList( &tLocalReader );
			// get reader in the list
			if ( CL_FAILED( cl_readerFindInList( &pReaderInList, &tLocalReader ) ) )
				break;

			memcpy( &pReaderInList->tReaderDiscoverHalFuncs, &pReaderInList->tReaderHalFuncs, sizeof( pReader->tReaderDiscoverHalFuncs ) );

			if ( pReaderInList )
			{
				// if already in DISCOVER.... do not reask to reopen port and so on
				if ( pReaderInList->eState != STATE_DISCOVER )
				{
					// set state for reader. If an error happens while trying to connect, we check the status afterwards
					// so not trying return code at this point is ok
					cl_ReaderSetState( pReaderInList, STATE_DISCOVER );
				}
				// at this stage, the reader shall be in DISCOVER mode if present. Otherwise, remove it from the list
				if ( pReaderInList->eState != STATE_DISCOVER )
				{
					if ( CL_FAILED( cl_ReaderRemoveFromList( pReaderInList ) ) )
						break;
				}

				// points to default frame for Clover discover
				pDiscover = &ac8IPDiscover[0];
				// initialize a tuple default flags with memory
				if ( CL_FAILED( cl_initTuple( &tTuple4Discover, CL_NULL, &pDiscover, sizeof( ac8IPDiscover ) ) ) )
					cl_ReaderRemoveFromList( pReaderInList );
				else
				{
					// add time flag if any
					// before sending to network, tag data with time stamp
					pCtxt->ptHalFuncs->fnGetTime( (&tTuple4Discover)->cl8Time, sizeof( tTuple4Discover.cl8Time) );

					// now a COM port is detected : check if it is a valid reader
					if ( CL_FAILED( cl_sendData( CL_NULL, pReaderInList, &tTuple4Discover, NON_BLOCKING, &u32TsfNb ) ) )
					{
						cl_ReaderRemoveFromList( pReaderInList );
					}
					else
					{
						DEBUG_PRINTF("Reader %s connected\n", pReaderInList->aucLabel );
						// wait 1s before checking a new reader popped in
						if ( pCtxt->ptHalFuncs->fnWaitMs != CL_NULL )
							pCtxt->ptHalFuncs->fnWaitMs( 5000 );
					}
				}
			}
		}
		break;
	  }
	  case ( COM_READER_TYPE ):
	  {
		  DEBUG_PRINTF("-> From reader %s with %d : %x", pReader->tCOMParams.aucPortName, pTuple->ptBuf->ulLen, pTuple->ptBuf->pData[0] );
		  if ( pTuple->ptBuf->ulLen == 9 )
		  {
			  if ( ( pTuple->ptBuf->pData[0] == 0x01) & ( pTuple->ptBuf->pData[1] == 0x07) & ( pTuple->ptBuf->pData[2] == 0x02) )
			  {
					memcpy( pReader->tCloverSense.au8RadioAddress, pTuple->ptBuf->pData + 3, 6 );
					DEBUG_PRINTF("-> From reader %s : Clover Sense address is ", pReader->tCOMParams.aucPortName);
					memset( aucTempName, 0, sizeof( aucTempName ) );
					for (u8Index = 0; u8Index < 6; u8Index++)
					{
						cl32Temp = ( (clu32) pReader->tCloverSense.au8RadioAddress[u8Index] ) & 0x000000FF;
						sprintf(aucTempName+u8Index*2, "%x",cl32Temp);
					}
					DEBUG_PRINTF("%s\n", aucTempName);

					// this is an asynchronous event... Re event state of the reader to update user data
					cl_ReaderSetState( pReader, pReader->eState );
			  }
		  }
		  break;
	  }
	  case (BT_READER_TYPE ):
	  default:
		DEBUG_PRINTF("\nUNKNOWN READER TYPE %s\n", pReader->tType );
	}

	return (status );
}

/******************************************************************************/
/* Name :  e_Result (cl_SemaphoreCreateWin32)( clvoid **ppSemaphore );        */
/* Description : create a semaphore (can be locked to 1 only)                 */
/*                protection of resources that could be accessed concurrently */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clu32 *pu32SemId : semaphore id.                                     */
/*          Shall point to NULL when parameter                                */
/*          is passed. Returned different from NULL on OK                     */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,        */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/******************************************************************************/
e_Result cl_SemaphoreCreateWin32( clvoid **ppSemaphore )
{
	t_clContext *pCtxt 		= CL_NULL;
	sem_t *pSem;

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED( cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if (pCtxt->ptHalFuncs->fnAllocMem == CL_NULL)
		return ( CL_ERROR );

//	if ( CL_FAILED( pCtxt->ptHalFuncs->fnAllocMem( (clvoid **)&pSem, sizeof( sem_t ) ) ) )
//		return ( CL_ERROR );

	if ( CL_FAILED( csl_malloc( (clvoid **)&pSem, sizeof( sem_t ) ) ) )
		return ( CL_ERROR );

    if ( pSem == CL_NULL )
         return ( CL_ERROR);

    // int sem_init(sem_t *sem, int pshared, unsigned int value);
	// The sem_init() function is used to initialise the unnamed semaphore referred to by sem.
    // The value of the initialised semaphore is value. Following a successful call to sem_init(),
    // the semaphore may be used in subsequent calls to sem_wait(), sem_trywait(), sem_post(), and sem_destroy().
    // This semaphore remains usable until the semaphore is destroyed.
	// If the pshared argument has a non-zero value, then the semaphore is shared between processes; in this case,
    // any process that can access the semaphore sem can use sem for performing sem_wait(), sem_trywait(), sem_post(), and sem_destroy() operations.
	// Only sem itself may be used for performing synchronisation.
    // The result of referring to copies of sem in calls to sem_wait(), sem_trywait(), sem_post(), and sem_destroy(), is undefined.
	// If the pshared argument is zero, then the semaphore is shared between threads of the process; any thread in this process
    // can use sem for performing sem_wait(), sem_trywait(), sem_post(), and sem_destroy() operations.
    // The use of the semaphore by threads other than those created in the same process is undefined.
	// Attempting to initialise an already initialised semaphore results in undefined behaviour.

	// aka: semaphore is used between thread. Its initial value is 0, i.e that it will block if already locked an other thread until it releases it.
	if ( sem_init(  pSem,  0, 0 ) == - 1)
		return CL_ERROR;
	else
	{
		*ppSemaphore = pSem;
		return CL_OK;
	}
}

/******************************************************************************/
/* Name :  e_Result (cl_SemaphoreWaitWin32)( clvoid *pSem, clu32 u32ms );   */
/* Description : wait for a semaphore until timeout to lock it                */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pSem : opaque pointer on semaphore                     */
/*       clu32 u32ms: if 0, don't wait. If semaphore is free, lock it. If not */
/*                    exit on OK                                              */
/*                    if 0xFFFFFFFF, wait indefinitely                        */
/*                    others values: wait up to u32ms. If not locked after    */
/*                    this period, exit on TIMEOUT_ERROR                      */
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

#ifdef DEBUG_SEMAPHORE_WAIT
#define _DEBUG_PRINTF DEBUG_PRINTF
#else
#define _DEBUG_PRINTF(...) ((void )0)
#endif

e_Result cl_SemaphoreWaitWin32( clvoid *pSem, clu32 u32ms )
{
	e_Result status = CL_ERROR;
	long time_ms = u32ms;
	struct timespec abstime;

	long delta_time_secondes = time_ms / 1000L;
	long delta_time_nano_secondes = ( time_ms % 1000L ) * 1000000L ;

	_DEBUG_PRINTF("cl_SemaphoreWaitWin32: time_ms: %ld", time_ms);
	_DEBUG_PRINTF("cl_SemaphoreWaitWin32: delta_time_secondes: %ld", delta_time_secondes);
	_DEBUG_PRINTF("cl_SemaphoreWaitWin32: delta_time_nano_secondes: %ld", delta_time_nano_secondes);

	if ( pSem == CL_NULL )
	{
		DEBUG_PRINTF("cl_SemaphoreWaitWin32: pSem == CL_NULL");
		return CL_PARAMS_ERR;
	}

	if ( clock_gettime(CLOCK_REALTIME, &abstime) == -1 )
	{
		perror("clock_gettime");
		DEBUG_PRINTF("cl_SemaphoreWaitWin32: clock_gettime: ERROR");
		return CL_ERROR;
	}

	//
	// Start Timer
	//
	abstime.tv_sec += delta_time_secondes;
	abstime.tv_nsec += delta_time_nano_secondes;

	//
	// Now wait for semaphore during a certain period of time before failing
	//
	if ( sem_timedwait( pSem, &abstime ) == -1 )
	{
		switch ( errno )
		{
			case EINVAL:
				_DEBUG_PRINTF("The process or thread would have blocked, and the abstime parameter specified a nanoseconds field value less than zero or greater than or equal to 1000 million.");
				break;
			case ETIMEDOUT:
				_DEBUG_PRINTF("The semaphore could not be locked before the specified timeout expired.");
//				g_tCtxt.ptHalFuncs->fnStackTrace(); // BRY_StackTrace
				status = CL_TIMEOUT_ERR;
				break;
			default:
				DEBUG_PRINTF("errno is %x", errno );
				break;
		}

		return status;
	}
	else
		return CL_OK;
}


/******************************************************************************/
/* Name :  e_Result (cl_SemaphoreReleaseWin32)( clvoid *pSem );             */
/* Description : release a semaphore already locked                           */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pSem : semaphore to unlock                   					*/
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*                          * OK                                              */
/*                          * ERROR: return if semaphore was not existing     */
/******************************************************************************/
e_Result cl_SemaphoreReleaseWin32( clvoid *pSem )
{
	if ( sem_post( pSem ) == - 1 )
	{
		DEBUG_PRINTF("Release Semaphore Error!!!\n");
		return ( CL_ERROR );
	}
	else
		return ( CL_OK );
}



/******************************************************************************/
/* Name :  e_Result (cl_SemaphoreDestroyWin32)( clvoid *pSem );             */
/* Description : destroy a semaphore                                          */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In :  clvoid *pSem : semaphore to destroy					                   */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,         */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/******************************************************************************/
e_Result cl_SemaphoreDestroyWin32( clvoid *pSem )
{
	e_Result status = CL_ERROR;

	// release the semaphore
	status = sem_destroy( pSem );
	if ( status != 0 )
		status = CL_ERROR;

	// release related memory
	t_clContext *pCtxt = CL_NULL;

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED( cl_GetContext( &pCtxt )) )
		return CL_ERROR;

	if ( pCtxt == CL_NULL )
		return CL_ERROR;

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return CL_ERROR;

	if (pCtxt->ptHalFuncs->fnFreeMemSafely == CL_NULL)
		return CL_ERROR;

	if ( CL_FAILED( pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pSem ) ) )
		return CL_ERROR;

	return status;
}

/*--------------------------------------------------------------------------*/

e_Result cl_MutexCreateWin32( clvoid **aAdrMutex )
{
	e_Result status = CL_ERROR;

	if ( pthread_mutex_init( aAdrMutex,  0 ) == 0 )
	{
		status = CL_OK;
	}

	return status;
}

/*--------------------------------------------------------------------------*/

e_Result cl_MutexDestroyWin32( clvoid **aAdrMutex )
{
	e_Result status = CL_ERROR;

	if ( pthread_mutex_init( aAdrMutex,  0 ) == 0 )
	{
		status = CL_OK;
	}

	return status;
}

/******************************************************************************/
/* Name :  e_Result (cl_WaitMsWin32)( clu32 u32ms );                                */
/* Description : wait for u32ms before continuing                             */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In :clu32 u32ms : number of ms to wait                                    */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,        */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/*  TIMEOUT_ERR,              :  Overrun on timing                             */
/* TRANSFER_IN_PROCESS_ERR   :  A non blocking call is in progress. This       */
/*                              transfer is rejected. The user application     */
/*                              shall recall the API to ensure that this       */
/*                              request is proceeded                           */
/******************************************************************************/
e_Result cl_WaitMsWin32( clu32 u32ms )
{
	if (u32ms < ( 0xFFFFFFFF/1000) )
	{
		Sleep( u32ms  ); // wait for u32ms
		return ( CL_OK );
	}
	else
		return ( CL_ERROR );
}


/******************************************************************************/
/*!!!!FD Name :      e_Result (cl_AllocMemWin32)( t_Buffer **pptBuff, clu32 ulLen); 	 */
/* Description : allocate memory and returns allocated pointed pointer to data*/
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In :																	  */
/*		clu32 ulLen: length of the buffer in bytes to be allocated by CSL	  */
/* ---------------                                                            */
/*  In/Out :t_Buffer **pptBuff : pointer of a data buffer allocated in the    */
/*        hosting system. This data is released using fnFreeMem               */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,        */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/*  TIMEOUT_ERR,              :  Overrun on timing                             */
/******************************************************************************/
clu32 g_u32StackIndex = 0;

e_Result cl_AllocMemWin32( clvoid **pptBuff, clu32 ulLen)
{
	if ( pptBuff == CL_NULL )
		return ( CL_PARAMS_ERR );

	if ( ulLen > 1024 )
	{
		DEBUG_PRINTF("!!!! ENORMOUS malloc %x %d\n", (clu32)ulLen );
	}

	*pptBuff = malloc( ulLen );

	if ( *pptBuff == CL_NULL )
	{
		DEBUG_PRINTF("cl_AllocMemWin32:malloc FAILED to alloc %d oct !!!!!", ulLen );
		return ( CL_MEM_ERR );
	}
	else
		return ( CL_OK );
}

/******************************************************************************/
/* Name :      e_Result (cl_FreeMemWin32)( t_Buffer *ptBuff);                 */
/* Description : free memory pointed by ptBuff                                */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In/Out :t_Buffer *ptBuff : pointer of a data buffer to free               */
/*            if NULL or non-existing pointer to Buffer, return CL_PARAMS_ERR */
/*			when exiting, ptBuff is set to NULL								  */
/*																			  */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  CL_PARAMS_ERR,               :  Inconsistent parameters                    */
/******************************************************************************/

e_Result cl_FreeMemWin32( clvoid *ptBuff )
{
	if (ptBuff == NULL )
		return ( CL_PARAMS_ERR );
    //DEBUG_PRINTF("free %x\n", (clu32)ptBuff);
	free( ptBuff );
	ptBuff = CL_NULL;

	return ( CL_OK );
}

/*--------------------------------------------------------------------------*/

e_Result cl_FreeMemSafelyWin32( clvoid **ptBuff )
{
	free( *ptBuff );
	*ptBuff = (void *)0;

	return ( CL_OK );
}

/******************************************************************************/
/* Name :      e_Result (cl_CreateThreadWin32)( clvoid *pnFnThread,				*/
/*												clu32 ulStackSize				*/
/*												clvoid *pParams, 				*/
/*												clvoid *p_tThreadId);           */
/* Description : create a thread for the library. Typically, there is 3 threads */
/*    - 1. one for reader registration and management                         	*/
/*    - 2. one for crypto                                                    	*/
/*    - 3. 2 per readers: one for writing/one for reading						*/
/*                                                                            	*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : clvoid *pnFnThread : function pointer holding the code of the thread 	*/
/*		clu32 ulStackSize :  size in bytes allocated for the thread			  	*/
/*		clvoid *pParams		: parameters passed to the created thread			*/
/* ---------------                                                            	*/
/*  Out: 	                                                                 	*/
/*		clvoid *p_tThreadId	: pointer on a pthread_t struct holding the thread id */
/*                  if NULL, no thread was created                            	*/
/* Return value: e_Result                                                      	*/
/*  OK                        :  Result is OK                                  	*/
/*  ERROR,                    : Failure on execution                           	*/
/*  MEM_ERR,                  :  Failure on memory management (failure,        	*/
/*                                  allocation ....)                           	*/
/*  PARAMS_ERR,               :  Inconsistent parameters                       	*/
/*  TIMEOUT_ERR,              :  Overrun on timing                             	*/
/******************************************************************************/

e_Result cl_CreateThreadWin32( clvoid *pnFnThreadFunc, clu32 ulStackSize, clvoid *pParams, clvoid **pp_tThreadId)
{
	t_clContext *pCtxt 		= CL_NULL;
    pthread_t	*pThread;

    // get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED( cl_GetContext( &pCtxt )) )
		return CL_ERROR;

	if ( pCtxt == CL_NULL )
		return CL_ERROR;

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return CL_ERROR;

	if (pCtxt->ptHalFuncs->fnAllocMem == CL_NULL)
		return CL_ERROR;

    if ( CL_FAILED( csl_malloc( (clvoid **) &pThread, sizeof( pthread_t ) ) ) )
		return CL_ERROR;

    if ( pthread_create( pThread, CL_NULL, pnFnThreadFunc, pParams) != 0 )
    {
		return CL_ERROR;
    }
	else
	{        
		*pp_tThreadId = (clvoid *)pThread;
		return CL_OK;
	}
}

/******************************************************************************/
/* Name :      e_Result (*fnDestroyThread)( clvoid *p_tThreadId  )            */
/* Description : destroy calling thread           				    		  */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pThreadId : opaque pointer to thread structure 			 */
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
e_Result cl_DestroyThreadWin32( clvoid *p_tThreadId  )
{
	t_clContext *pCtxt 		= CL_NULL;
    pthread_t 	*ptThId 	= (pthread_t *)p_tThreadId;
    int       	 result 	= 0;

    DEBUG_PRINTF("cl_DestroyThreadWin32: BEGIN");

	if ( CL_FAILED( cl_GetContext( &pCtxt )) )
		return CL_ERROR;

	if ( pCtxt == CL_NULL )
		return CL_ERROR;

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return CL_ERROR;

	if (pCtxt->ptHalFuncs->fnFreeMemSafely == CL_NULL)
		return CL_ERROR;

    if ( ptThId )
    {
        DEBUG_PRINTF( "cl_DestroyThreadWin32: Thread canceled is 0x%04X", *ptThId );

        result = pthread_cancel( *ptThId );
        if ( result != 0 )
        {
			switch ( result )
			{
				case ESRCH:
					DEBUG_PRINTF( "No thread matches the passed as argument.");
					return CL_ERROR;
					break;

				case EINVAL:
					DEBUG_PRINTF( "The thread has been detached or another thread is already waiting for the end of the same thread.");
					return CL_ERROR;
					break;

				case EDEADLK:
					DEBUG_PRINTF( "The thread passed as an argument is the calling thread.");
					return CL_ERROR;
					break;
			}
        }
        else
        {
        	DEBUG_PRINTF( "cl_DestroyThreadWin32: pthread_cancel: SUCCEEDED" );

        	if ( CL_FAILED( pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&p_tThreadId ) ) )
        	{
        		DEBUG_PRINTF( "cl_DestroyThreadWin32: fnFreeMem: CL_FAILED" );
        		return CL_ERROR;
        	}
        }
    }

    DEBUG_PRINTF("cl_DestroyThreadWin32: END");
	return CL_OK;
}
/*****************************************************************************/
/* * @fn :  e_Result (*fnDestroyThreadAsked)( clu8 *pu8CancelRequest );
* 	@brief : Signal that a thread destroy was requested
*
*
* @param pu8CancelRequest 		: ( Out ) indicates a destroy was requested
*
* @return e_Result
* \n OK                        :  Result is OK
* ************************************************************************** */
e_Result cl_DestroyThreadRequestedWin32( clu8 *pu8CancelRequest )
{
    if ( pu8CancelRequest == CL_NULL)
		return ( CL_ERROR );

	// in win32/64 all threads are by default cancellable and wait for cancellation point to effectively being cancelled and freed.
	// caller is waiting this call to return from a request to cancel issued by him
	pthread_testcancel();

	*pu8CancelRequest = 0;

	return ( CL_OK );
}


/********************************************************************************/
/* Name :      e_Result (*fnSetIPRange)( t_ReaderRange *pRange );              	*/
/* Description : Application set the IP range and UDP port where to find the data*/
/*                                                                            	*/
/********************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : t_ReaderRange *pRange : list of IP range and UDP port to scan and 		*/
/* find the readers 	  														*/
/* ---------------                                                            	*/
/*  Out: none                                                                 	*/
/* Return value: e_Result                                                      	*/
/*  OK                        :  Result is OK                                  	*/
/*  ERROR,                    : Failure on execution                           	*/
/*  MEM_ERR,                  :  Failure on memory management (failure,        	*/
/*                                  allocation ....)                           	*/
/*  PARAMS_ERR,               :  Inconsistent parameters                       	*/
/*********************************************************************************/
e_Result cl_SetIPRangeWin32( t_ReaderRange *pRange )
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

	// lock the reader registration thread before setting up the IP list
	if ( pCtxt->g_RegReader.tThreadId == CL_NULL )
		return ( CL_ERROR );


	// copy list of IP range/UDP port for further scan

	// signal change in the list


	// unlock the reader registration thread

	return ( CL_OK );
}

/******************************************************************************/
/* Name :      e_Result cl_GetTimeWin32( cl8 *pTime, clu32 ulLen )           */
/* Description : returns formatted string with time from epoch		 		  */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In :  cl8 **ppTime:	80bytes long buffer to contain formatted string	  	  */
/*		  ulLen	: length of allocated buffer to receive the time information  */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,        */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/******************************************************************************/
e_Result cl_GetTimeWin32( cl8 *pTime, clu32 ulLen )
{
	time_t     now;
	struct tm  ts;
	cl8 buf[80];
	struct timespec real_time;
	char strMilliseconds[80];

#define MILLION 1000000L

	/* check incoming parameters */
	if ( pTime == CL_NULL )
		return ( CL_ERROR );

	if ( ulLen < 80 )
		return ( CL_ERROR );

	/* Get current time */
	time(&now);

	/* Format time, "ddd yyyy-mm-dd hh:mm:ss zzz" */ /* BRY : zzz ???? les milliseondes, ca m'�tonnerait !!! je suis oblig� de le faire */
	ts = *localtime(&now);
	strftime( buf, ulLen, "%a %Y-%m-%d %H:%M:%S", &ts);

	/* Get micro seconde in milliseconds, concat with time */
	clock_gettime(CLOCK_REALTIME,&real_time);
	sprintf(strMilliseconds, ":%03d", real_time.tv_nsec / MILLION);
	strcat(buf,strMilliseconds);

	/* perform local copy to return arguments */
	memcpy( (cl8 *) pTime, buf, ulLen );

	return ( CL_OK );
}

/******************************************************************************/
/* Name :      e_Result cl_GetElapsedTimeWin32(  )           					*/
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
clu64 cl_GetElapsedTimeWin32( clvoid )
{
	t_clContext *pCtxt 		= CL_NULL;
	e_Result status 		= CL_ERROR;
	struct timespec crt_time;
	clu64 	u64Seconds		=	0;
	clu64	u64NanoSeconds	=	0;

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

#define MILLION 1000000L

	clock_gettime(CLOCK_REALTIME,&crt_time);

	if (!(( crt_time.tv_sec == 0) & ( crt_time.tv_nsec==0)))
	{
		if ( crt_time.tv_nsec >= pCtxt->tDebugParams.u64NanoSeconds)
		{
            u64NanoSeconds = (clu64)(((clu64)(crt_time.tv_nsec - pCtxt->tDebugParams.u64NanoSeconds)) / MILLION);
            u64Seconds = (crt_time.tv_sec - pCtxt->tDebugParams.u64Seconds)/1000;
		}
		else
		{
            u64NanoSeconds = (clu64)(((clu64)(- crt_time.tv_nsec + pCtxt->tDebugParams.u64NanoSeconds)) / MILLION);
            u64Seconds = (crt_time.tv_sec - pCtxt->tDebugParams.u64Seconds + 1)/1000;
		}
	}
	pCtxt->tDebugParams.u64Seconds = crt_time.tv_sec;
	pCtxt->tDebugParams.u64NanoSeconds = crt_time.tv_nsec;

	return (u64Seconds + u64NanoSeconds);
}

/*--------------------------------------------------------------------------*\
 * Give the time since last call in millisecondes
\*--------------------------------------------------------------------------*/
cl64 cl_GetElapsedTimeHighPerformanceWin32()
{
	static int calculateFrequency = 1;

	static long double counterPeriode = 0;
	static LARGE_INTEGER counterStart;
	static LARGE_INTEGER frequency;

	LONGLONG duration;
	LARGE_INTEGER counterStop;

	// Calculate high performance counter's periode
	if ( calculateFrequency )
	{
		if ( QueryPerformanceFrequency( &frequency ) != 0 )
		{
			counterPeriode = 1/(double)frequency.QuadPart; /* T = 1/f */
		}
		else
		{
			counterPeriode = 0;
		}
		calculateFrequency = 0;
	}

	QueryPerformanceCounter( &counterStop );
	duration = counterStop.QuadPart - counterStart.QuadPart;
	counterStart = counterStop;

	return (cl64)((duration * counterPeriode) * 1000L);
}

/*--------------------------------------------------------------------------*/

void cl_Trace( char *aFormatString, ... )
{
	// TODO: something
}

/*--------------------------------------------------------------------------*/

void cl_StackTrace( void )
{
  HANDLE process = GetCurrentProcess();
  HANDLE thread = GetCurrentThread();

  CONTEXT context;
  memset(&context, 0, sizeof(CONTEXT));
  context.ContextFlags = CONTEXT_FULL;
  RtlCaptureContext(&context);

  SymInitialize(process, NULL, TRUE);

  DWORD image;
  STACKFRAME64 stackframe;
  ZeroMemory(&stackframe, sizeof(STACKFRAME64));

#ifdef _M_IX86
  image = IMAGE_FILE_MACHINE_I386;
  stackframe.AddrPC.Offset = context.Eip;
  stackframe.AddrPC.Mode = AddrModeFlat;
  stackframe.AddrFrame.Offset = context.Ebp;
  stackframe.AddrFrame.Mode = AddrModeFlat;
  stackframe.AddrStack.Offset = context.Esp;
  stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
  image = IMAGE_FILE_MACHINE_AMD64;
  stackframe.AddrPC.Offset = context.Rip;
  stackframe.AddrPC.Mode = AddrModeFlat;
  stackframe.AddrFrame.Offset = context.Rsp;
  stackframe.AddrFrame.Mode = AddrModeFlat;
  stackframe.AddrStack.Offset = context.Rsp;
  stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
  image = IMAGE_FILE_MACHINE_IA64;
  stackframe.AddrPC.Offset = context.StIIP;
  stackframe.AddrPC.Mode = AddrModeFlat;
  stackframe.AddrFrame.Offset = context.IntSp;
  stackframe.AddrFrame.Mode = AddrModeFlat;
  stackframe.AddrBStore.Offset = context.RsBSP;
  stackframe.AddrBStore.Mode = AddrModeFlat;
  stackframe.AddrStack.Offset = context.IntSp;
  stackframe.AddrStack.Mode = AddrModeFlat;
#endif

  size_t i = 0;

  for ( i = 0; i < 25; i++ )
  {
    BOOL result = StackWalk64(
      image, process, thread,
      &stackframe, &context, NULL,
      SymFunctionTableAccess64, SymGetModuleBase64, NULL);

    if (!result) { break; }

    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    DWORD64 displacement = 0;
    if (SymFromAddr(process, stackframe.AddrPC.Offset, &displacement, symbol))
    {
//      printf("[%i] %s\n", i, symbol->Name);
    	DEBUG_PRINTF("[%i] %s\n", i, symbol->Name);
    }
    else
    {
//      printf("[%i] ???\n", i);
    	DEBUG_PRINTF("[%i] ???\n", i);
    }

  }

  SymCleanup(process);
}



