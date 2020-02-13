/*
 * script_engine.c
 *
 *  Created on: 23 juin 2014
 *      Author: fdespres
 */
#define _SCRIPT_ENGINE_C_C

#include <stdio.h>
#include "..\cltypes.h"
#include "..\csl.h"
#include "..\inc\script_engine.h"
#include "..\inc\generic.h"
#include "..\inc\clreaders.h"
#include "..\inc\cltuple.h"
#include "..\inc\menu_parser.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


//***********************************************************
//	Name: script_ProcessFile( cl8 *pFile , e_EngineType eEngine)
//  Description: get script from file for execution
//***********************************************************
// Arguments:
// 	In :	( cl8 *pFile)
//		e_EngineType eEngine	: CMD_FILE_ENGINE: start a CMD file
//								 MENU_FILE_ENGINE: start a menu file
//	Out :
// 	Return Values:
//				CL_OK
//				CL_BAD_PARAM : unexpected end of file/wrong arguments....
//				CL_ERROR	 : Error on execution from underlayers
//***********************************************************
e_Result script_ProcessFile( cl8 *pFile, e_EngineType eEngine )
{
	e_Result 	status 			= 	CL_ERROR;
	t_Tuple  	*pTuple 		= 	CL_NULL;
	t_Tuple		*pTupleReceived = 	CL_NULL;
	e_HciCmdType cmdType 		= 	HCI_UNKNOWN_CMD;
	clu8 		*pData  		= 	CL_NULL;
	clu32 		u32Len 			= 	0;
	t_clContext *pCtxt 			= 	CL_NULL;
	clvoid 		*pFileId 		=	CL_NULL;

	// check global parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFileOpen == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFileReadLn == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFileClose == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )
		return ( CL_ERROR );

	for(;;)
	{
		if ( pFile == CL_NULL )	// file pointer was not initialized. Returns CL_PARAMS_ERR
		{
			status = CL_PARAMS_ERR;
			break;
		}

		if ( pCtxt->ptHalFuncs->fnFileOpen( pFile, CL_FILE_BINARY_MODE, &pFileId) != CL_OK ) // file on opening file
		{
			status = CL_ERROR;
			break;
		}

		// parse file and retrieve instructions for scripting
		for ( ;; )
		{
			// read a new instruction  (allocation is performed by underlayer)
			// free shall be done here, in CSL
			if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnFileReadLn( pFileId, &pData, &u32Len ) ) )
				break;

			// if there is a data to send
			if ( pData == CL_NULL )
				continue;

			switch ( eEngine )
			{
				case ( CMD_FILE_ENGINE ):
				{
					// parse the command read from the file
					status = cl_GetScriptCommand( pData, u32Len );
					break;
				}
				case ( MENU_FILE_ENGINE ):
				{
					status = cl_MenuBuild( pCtxt->ptMenuFileDef->pMenuDescriptionFile );
					break;
				}
				default:
				{
					DEBUG_PRINTF("Invalid option\n");
					status = CL_ERROR;
				}
			}
			// free the memory buffer containing the command from the file
			pCtxt->ptHalFuncs->fnFreeMem( pData );

			if ( CL_FAILED( status ) )
				break;
		}
		if ( CL_FAILED( status ))
		{

			// close the file
			pCtxt->ptHalFuncs->fnFileClose( pFileId );

			DEBUG_PRINTF("script_ProcessInst FAILED %d\n", status );
		}
		break;
	}

	return ( status );
}

//***********************************************************
/**	@name: cl_GetScriptCommand( clu8 *pScript, clu32 ulLen )
*  @brief: from a command line, call the correct API of CSL

* Arguments:
* 	In :
*	@param *pScript			: ( In) Pointer on script line
*	@param ulLen				: ( In )Len of script line
*
*	@return
*   \n				CL_OK
*	\n				CL_BAD_PARAM : unexpected end of file/wrong arguments....
*	\n				CL_ERROR	 : Error on execution from underlayers
*********************************************************** */
e_Result cl_GetScriptCommand( clu8 *pScript, clu32 ulLen)
{
	clu32 u32Index 		= 0;
	clu32 u32Jndex 		= 0;
	clu32 u32FirstIndex = 0;
	clu32 u32LastIndex 	= 0;
	clu8  u8IndexStart 	= 0;
	clu32 u32CmdFound 	= 0;
	clu32 u32CmdIndex 	= 0;
	t_clContext *pCtxt 	= CL_NULL;
	e_Result	status 	= CL_ERROR;
	t_ScriptData *p_ScriptData = CL_NULL;
	t_ScriptData *p_CrtScriptData = CL_NULL;


	// check parameters
	if ( pScript == CL_NULL )
		return ( CL_ERROR );

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
			return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )
		return ( CL_ERROR );


	// /* parse line of commands and retrieve the commands and its args */
	for ( u32Index = 0; u32Index < ulLen; u32Index++ )
	{
		// check if we have a comment. In this case, exit directly the line
		if ( (*(pScript + u32Index) ) == 0x2F )	// 0x2F is '/'
		{
			if ( u32CmdFound != 0)	// if we already found a command and we are here, it means that we have parsed a valid command from input file;
			{ // so check that we have a function to execute
				if ( aCmdDef[u32CmdIndex].fnCmdFunction != CL_NULL )
				{
					// now, let's execute it with its arguments and number of arguments
					status = aCmdDef[u32CmdIndex].fnCmdFunction( p_ScriptData, u32CmdFound );

					if ( CL_FAILED( status) )	// if failed, exit and release any data
					{
						DEBUG_PRINTF("Script failed on %s \n", p_ScriptData->pData );
						break;
					}
					// else continue and process next one
				}
				else
				{
					DEBUG_PRINTF("Nothing to execute for this command %s issued by the script file \n", p_ScriptData->pData );
					status = CL_ERROR;
				}
			}
			break;
		}
		// check if we have an ASCII character or '_' or '.'
		if ( ( isalnum( *(pScript + u32Index) ) ) | ((*(pScript + u32Index)) == '_' ) | ((*(pScript + u32Index)) == '.' ) )
		{
			if (u8IndexStart == 0)
			{
				u8IndexStart = 1;
				u32FirstIndex = u32Index;
			}

		}
		else	// this is not an ASCII character.
		{
			if ( u8IndexStart == 1)	// we have already found some character in the command. try to recognize the command
			{
				u32LastIndex = u32Index - 1; // save last position of a valid command

				if ( u32CmdFound == 0 )	// try to find the command
				{
					status = CL_ERROR;
					for ( u32Jndex = 0; u32Jndex < (sizeof( aCmdDef)/sizeof(eCmdStruct) ); u32Jndex++ )
					{
						// compare in the array of cmd accepted by the script engine if this command exists
						if ( memcmp( pScript + u32FirstIndex, aCmdDef[u32Jndex].CmdString, (u32LastIndex - u32FirstIndex + 1)  ) == 0 )
						{
							// if the command exists => reserve memory to store and prepare to catch its arguments
							status = CL_OK;

							// allocate memory for command
							if ( CL_FAILED( ( cls_pmalloc( (void **)&p_ScriptData, sizeof( t_ScriptData ) )) ) )
							{
								status = CL_ERROR;
								break;
							}

							// initialize pCrtScript with first element of the list
							p_ScriptData->pNext = CL_NULL;
							p_CrtScriptData = p_ScriptData;

							// allocate memory for command
							if ( CL_FAILED( ( cls_pmalloc( (void **)&p_CrtScriptData->pData, (u32LastIndex - u32FirstIndex + 1) )) ) )
							{
								status = CL_ERROR;
								break;
							}

							// then copy in it
							memcpy( p_CrtScriptData->pData, aCmdDef[u32Jndex].CmdString, (u32LastIndex - u32FirstIndex + 1) );
							p_CrtScriptData->clu32Len = (u32LastIndex - u32FirstIndex + 1);

							break;
						}
					}

					if ( CL_FAILED( status ) )
					{
						DEBUG_PRINTF("File parser is getting error. No valid Command detected on %s \n", pScript);
						break;
					}
					u32CmdFound = 1;
					u32CmdIndex = u32Jndex;
					u8IndexStart = 0;
					u32FirstIndex = 0;


					DEBUG_PRINTF("command is : %s\n", p_ScriptData->pData );
					continue;
				}

				if ( u32CmdFound >= 1)	// we have already found the command. Now get the arguments (maximum 5)
				{
					if ( u32CmdFound > aCmdDef[u32CmdIndex].argsNb )
					{
						DEBUG_PRINTF("Too many arguments specified in the file of script for this command. \n");
						DEBUG_PRINTF("For %s, it shall be %d arguments\n",  aCmdDef[u32CmdIndex].CmdString, (cl32) aCmdDef[u32CmdIndex].argsNb );
						status = CL_ERROR;
						break;
						// memcpy the arguments
					}
					else
					{

						// allocate memory for command
						if ( CL_FAILED( ( cls_pmalloc( (void **)&p_CrtScriptData->pNext, sizeof( t_ScriptData ) )) ) )
						{
							status = CL_ERROR;
							break;
						}
						// points to allocated memory
						p_CrtScriptData = p_CrtScriptData->pNext;
						p_CrtScriptData->pNext = CL_NULL;
						p_CrtScriptData->clu32Len = 0;

						// allocate memory for command
						if ( CL_FAILED( ( cls_pmalloc( (void **)&p_CrtScriptData->pData, (u32LastIndex - u32FirstIndex + 1) )) ) )
						{
							status = CL_ERROR;
							break;
						}


						// then copy in it
						memcpy( p_CrtScriptData->pData, pScript + u32FirstIndex, (u32LastIndex - u32FirstIndex + 1) );
						p_CrtScriptData->clu32Len = (u32LastIndex - u32FirstIndex + 1);


						// display arguments
						DEBUG_PRINTF("arg %d is %s\n", u32CmdFound, (cl8 *)p_CrtScriptData->pData);

						// reset state machine to catch next argument if any
						u32CmdFound++;	// increment arguments number
						u8IndexStart = 0;	// reset position counter on arguments
						u32FirstIndex = 0; // reset position counter on arguments
						continue;
					}
				}
			}
		}
	}

	return ( status );
}

/********************************************************************************/
/* Name :  e_Result NetwData2Read_cb_Reader1( clvoid **ppReader, clvoid **ppTuple ) */
/* Description : custom callback on data reception for Reader 1				     */
/*				a copy of data shall be done at t								*/
/*                                                                            	*/
/********************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : clvoid *pSem : semaphore to unlock                   					*/
/* ---------------                                                            	*/
/*  Out: none                                                                 	*/
/* Return value: e_Result                                                      	*/
/*                          * OK                                              	*/
/*                          * ERROR: return if semaphore was not existing     	*/
/********************************************************************************/
e_Result NetwData2Read_cb_ReaderScript( clvoid **ppReader, clvoid **ppTuple )
{
	t_Reader *pReader = *ppReader;
	t_Tuple *pTuple = *ppTuple;

	/* check incoming parameters */
	if ( pReader == CL_NULL )
		return ( CL_ERROR );

	if ( pTuple == CL_NULL )
		return ( CL_ERROR );

	/* parse tuples coming from underlayers */
	while ( pTuple != CL_NULL )
	{
		if ( pTuple->ptBuf == CL_NULL)
			break;

		if ( pTuple->ptBuf->pData == CL_NULL )
			break;

		DEBUG_PRINTF("the reader %s received %d ", pReader->aucLabel, pTuple->ptBuf->ulLen );

		pTuple = ( t_Tuple *)pTuple->pNext;
	}
	return ( CL_OK );
}

/********************************************************************************/
/** @fn  e_Result cl_ScriptRegisterReader( t_ScriptData *p_ScriptData, clu32 clu32ArgsNb ) */
/*  @brief from script command, catch arguments and call API to:				*/
/*		- add the reader to the list of readers if needed						*/
/*		- initiate the connection to it											*/
/*                                                                            	*/
/********************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : t_ScriptData *p_ScriptData : pointer on buffer of data from script	in char	*/
/*				( first arg: command )											*/
/*				( second arg :  arg1 of command )								*/
/*				( third arg : arg2 of command )......							*/
/*				.......															*/
/* 		clu32 clu32ArgsNb: length of element of contains in p_ScriptData list	*/
/* ---------------                                                            	*/
/*  Out: none                                                                 	*/
/* Return value: e_Result                                                      	*/
/*                          * OK                                              	*/
/*                          * ERROR: params was wrong or reader registration failed    	*/
/********************************************************************************/
e_Result cl_ScriptRegisterReader( t_ScriptData *p_ScriptData, clu32 clu32ArgsNb )
{
	t_clContext *pCtxt = CL_NULL;
	e_Result status = CL_ERROR;
	t_Reader *pFoundReader = CL_NULL;

	// assign a local reader with its default flag
	t_Reader tReader;
/*	t_Reader tReader = {
			IP_READER_TYPE|LANTRONIX_READER_TYPE,
			STATE_INIT,
			"Reader 192.168.0.110", 	// explicit name
			CL_NULL,					// handle to reader  (filled by software)
			{	// IP Settings
					"192.168.0.110", 	//		192.168.0.111
					0x00000080A3923646, //Mac Address
					IP_READER_DEFAULT_TCP_PORT,	// connection port: telnet
			},
			{	// COM settings
					"COM1",					// com port to connect to
					CL_COM_BYTESIZE_8BITS,	// Byte size for transfer
					CL_COM_STOPBITS_10BIT,	// 0 stop bit
					CL_COM_PARITYBIT_NONE, 	// Parity bit none
					CL_COM_BAUDRATE_115200,	// baud rate
					50,						// read intervall timeout
					50,						// read total timeout base
					10,						// read total timeout multiplier
					50,						// write total timeout base
					10,						// write total timeout mutliplier
			},
			{	// BT settings
					0,
			},
			{ // tRdrSynchro
					CL_NULL,	// Thread Id For Read Operations : auto filled when adding the reader to the list of readers
					CL_NULL,	// Thread Id For Write Operations : auto filled when adding the reader to the list of readers
					CL_NULL,	// Signal For Read Operations : auto filled when adding the reader to the list of readers
					CL_NULL,	// Signal For Write Operations : auto filled when adding the reader to the list of readers
			},
			CL_NULL,			// Tuple list to be sent and attached to this reader
			CL_NULL,			// Tuple list to be read and attached to this reader
			{
					CL_NULL,					// callback used on completion of reset of network stack
					CL_NULL,		// callback used when data was pushed out to the reader
					NetwData2Read_cb_ReaderScript,			// callback used when data is coming from reader
			},
			{
					CL_NULL,		// Use default from framework : register a reader from library to porting layers
					CL_NULL,		// Use default from framework : unregister a reader from library to porting layers
					CL_NULL,		// Use default from framework : reset connection to reader
					CL_NULL,		// Use default from framework : open connection
					CL_NULL,		// Use default from framework : send data
					CL_NULL,		// Use default from framework : get data
					CL_NULL,		// Use default from framework : close connection to reader
					CL_NULL,		// Use default from framework : get DNS table
					CL_NULL,		// Use default from framework : set ip range
			},
			CL_NULL			// Pointer on next reader : auto filled when adding the reader to the list of Readers
	};
*/
	// check incoming parameters
	if ( p_ScriptData == CL_NULL )
		return ( CL_ERROR );

	if ( p_ScriptData->pNext == CL_NULL )
		return ( CL_ERROR );

	if ( p_ScriptData->pNext->pData == CL_NULL )
		return ( CL_ERROR );

	if ( p_ScriptData->pNext->pNext == CL_NULL )
		return ( CL_ERROR );

	if ( p_ScriptData->pNext->pNext->pData == CL_NULL )
		return ( CL_ERROR );

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );


	// fill local reader with default parameters of readers
	if ( CL_FAILED( cl_ReaderFillWithDefaultFields(  &tReader, IP_READER_TYPE ) ) )
		return ( CL_ERROR );



	// from now, we fill the reader properties coming from the script
	// 1. first element is the command. Pass it, we checked before calling it is the right command
	p_ScriptData = p_ScriptData->pNext ;

	// 2. second element: assign ip address
	// clear buffer
	memset ( tReader.tIPParams.aucIpAddr, 0, sizeof( tReader.tIPParams.aucIpAddr ) );

	// compare length of arguments before applying it
	if ( p_ScriptData->clu32Len <= sizeof( tReader.tIPParams.aucIpAddr ) )
		memcpy( tReader.tIPParams.aucIpAddr, p_ScriptData->pData, p_ScriptData->clu32Len );
	else
		return ( CL_ERROR );

	// 3. third element  assign tcp port
	p_ScriptData = p_ScriptData->pNext ;

	// get TCP port
	tReader.tIPParams.u32Port = atoi( p_ScriptData->pData );

	// register one reader
	if ( CL_FAILED( status = cl_readerAddToList( &tReader ) ) )
	{
		DEBUG_PRINTF("Failed to add the reader to the list\n");
	}

	return ( status );
}

/********************************************************************************/
/** @fn  e_Result cl_ScriptRegisterReader( t_ScriptData *p_ScriptData, clu32 clu32ArgsNb ) */
/*  @brief from script command, catch arguments and call API to:				*/
/*		- add the reader to the list of readers if needed						*/
/*		- initiate the connection to it											*/
/*                                                                            	*/
/********************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : t_ScriptData *p_ScriptData : pointer on buffer of data from script	in char	*/
/*				( first arg: command )											*/
/*				( second arg :  arg1 of command )								*/
/*				( third arg : arg2 of command )......							*/
/*				.......															*/
/* 		clu32 clu32ArgsNb: length of element of contains in p_ScriptData list	*/
/* ---------------                                                            	*/
/*  Out: none                                                                 	*/
/* Return value: e_Result                                                      	*/
/*                          * OK                                              	*/
/*                          * ERROR: params was wrong or reader registration failed    	*/
/********************************************************************************/
e_Result cl_ScriptRepeatStart( t_ScriptData *p_ScriptData, clu32 clu32ArgsNb )
{
	e_Result status = CL_ERROR;


	return ( status );
}

/********************************************************************************/
/** @fn  e_Result cl_ScriptRegisterReader( t_ScriptData *p_ScriptData, clu32 clu32ArgsNb ) */
/*  @brief from script command, catch arguments and call API to:				*/
/*		- add the reader to the list of readers if needed						*/
/*		- initiate the connection to it											*/
/*                                                                            	*/
/********************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : t_ScriptData *p_ScriptData : pointer on buffer of data from script	in char	*/
/*				( first arg: command )											*/
/*				( second arg :  arg1 of command )								*/
/*				( third arg : arg2 of command )......							*/
/*				.......															*/
/* 		clu32 clu32ArgsNb: length of element of contains in p_ScriptData list	*/
/* ---------------                                                            	*/
/*  Out: none                                                                 	*/
/* Return value: e_Result                                                      	*/
/*                          * OK                                              	*/
/*                          * ERROR: params was wrong or reader registration failed    	*/
/********************************************************************************/
e_Result cl_ScriptRepeatStop( t_ScriptData *p_ScriptData, clu32 clu32ArgsNb )
{
	e_Result status = CL_ERROR;


	return ( status );
}

/********************************************************************************/
/** @fn  e_Result cl_ScriptRegisterReader( t_ScriptData *p_ScriptData, clu32 clu32ArgsNb ) */
/*  @brief from script command, catch arguments and call API to:				*/
/*		- add the reader to the list of readers if needed						*/
/*		- initiate the connection to it											*/
/*                                                                            	*/
/********************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : t_ScriptData *p_ScriptData : pointer on buffer of data from script	in char	*/
/*				( first arg: command )											*/
/*				( second arg :  arg1 of command )								*/
/*				( third arg : arg2 of command )......							*/
/*				.......															*/
/* 		clu32 clu32ArgsNb: length of element of contains in p_ScriptData list	*/
/* ---------------                                                            	*/
/*  Out: none                                                                 	*/
/* Return value: e_Result                                                      	*/
/*                          * OK                                              	*/
/*                          * ERROR: params was wrong or reader registration failed    	*/
/********************************************************************************/
e_Result cl_ScriptCmd( t_ScriptData *p_Script, clu32 clu32ArgsNb )
{
	e_Result 	status = CL_ERROR;
	t_Reader	*pLastReader = CL_NULL;
	t_Tuple 	tTuple2Send;
	clu32		clu32TsfNb = 0;

	// check incoming parameters
	if ( p_Script == CL_NULL )
		return ( CL_ERROR );

	if ( p_Script->pNext == CL_NULL )
		return ( CL_ERROR );

	if ( p_Script->pNext->pData == CL_NULL )
		return ( CL_ERROR );

	if ( p_Script->pNext->pNext != CL_NULL )
		return ( CL_ERROR );


	// get last reader accessed. It allows to send the data to the correct reader
	if ( CL_FAILED( cl_ReaderGetLastAccessed( &pLastReader )) )
	{
		DEBUG_PRINTF("CMD cannot be executed as cl_ReaderGetLastAccessed returned an Error \n");
		return ( CL_ERROR );
	}

	// initialize a tuple default flags with memory
	if ( CL_FAILED( cl_initTuple( &tTuple2Send, CL_NULL, &p_Script->pNext->pData, p_Script->pNext->clu32Len ) ) )
		return ( CL_ERROR );

	// now send the data connected the last reader specified in the script
	if ( CL_FAILED ( cl_sendData( CL_NULL, pLastReader, &tTuple2Send, BLOCKING, &clu32TsfNb ) ) )
		return ( CL_ERROR );

	if ( CL_FAILED ( cl_sendData( CL_NULL, pLastReader, &tTuple2Send, BLOCKING, &clu32TsfNb ) ) )
		return ( CL_ERROR );

	return ( status );
}

/********************************************************************************/
/** @fn  e_Result cl_ScriptRegisterReader( t_ScriptData *p_ScriptData, clu32 clu32ArgsNb ) */
/*  @brief from script command, catch arguments and call API to:				*/
/*		- add the reader to the list of readers if needed						*/
/*		- initiate the connection to it											*/
/*                                                                            	*/
/********************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : t_ScriptData *p_ScriptData : pointer on buffer of data from script	in char	*/
/*				( first arg: command )											*/
/*				( second arg :  arg1 of command )								*/
/*				( third arg : arg2 of command )......							*/
/*				.......															*/
/* 		clu32 clu32ArgsNb: length of element of contains in p_ScriptData list	*/
/* ---------------                                                            	*/
/*  Out: none                                                                 	*/
/* Return value: e_Result                                                      	*/
/*                          * OK                                              	*/
/*                          * ERROR: params was wrong or reader registration failed    	*/
/********************************************************************************/
e_Result cl_ScriptDelay( t_ScriptData *p_Script, clu32 clu32ArgsNb )
{
	t_clContext *pCtxt = CL_NULL;
	e_Result 	status = CL_ERROR;
	t_Reader	*pLastReader = CL_NULL;
	clu32		clu32Period = 0;

	// check incoming parameters
	if ( p_Script == CL_NULL )
		return ( CL_ERROR );

	if ( p_Script->pNext == CL_NULL )
		return ( CL_ERROR );

	if ( p_Script->pNext->pData == CL_NULL )
		return ( CL_ERROR );

	if ( p_Script->pNext->pNext != CL_NULL )
		return ( CL_ERROR );

	// check global parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnWaitMs == CL_NULL )
		return ( CL_ERROR );

	// get last reader accessed. It allows to send the data to the correct reader
	if ( CL_FAILED( cl_ReaderGetLastAccessed( &pLastReader )) )
	{
		DEBUG_PRINTF("CMD cannot be executed as cl_ReaderGetLastAccessed returned an Error \n");
		return ( CL_ERROR );

	}

	// get period to wait
	clu32Period = atoi( p_Script->pNext->pData );

	// now wait
	status = pCtxt->ptHalFuncs->fnWaitMs( clu32Period );

	return ( status );
}

