/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: script_engine.h                                                    */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/* Description: interpretation of scripting language                         */
/*****************************************************************************/

#ifndef SCRIPT_ENGINE_H_
#define SCRIPT_ENGINE_H_
#include "..\csl.h"
/// typedef enum st_ScriptData
typedef struct st_ScriptData
{
	clu32	clu32Len;	// lenght of arguments
	clu8	*pData	;	// pointer on buffer containing the arguments
	struct st_ScriptData *pNext;
}t_ScriptData;

/// typedef enum cle_ScriptCmd
typedef enum cle_ScriptCmd
{
	READER_IP_PORT,
	REPEAT_START,
	REPEAT_STOP,
	CMD,
	DELAY,
}e_ScriptCmd;

/// typedef cle_EngineType
typedef enum cle_EngineType
{
	CMD_FILE_ENGINE,
	MENU_FILE_ENGINE,
}e_EngineType;


/// typedef struct st_CmdStruct
typedef struct st_CmdStruct
{
	e_ScriptCmd		eCmd; 			/// catch cmd
	cl8				*CmdString;		/// cmd as a string
	clu32			argsNb;			/// args number for this cmd when calling underlaying function
	e_Result 		(*fnCmdFunction)( t_ScriptData *p_ScriptData, clu32 clu32ArgsNb);
}eCmdStruct;

//***********************************************************
//	Name: script_ProcessFile( cl8 *pFile)
//  Description: get script from file for execution
//***********************************************************
// Arguments:
// 	In :	( cl8 *pFile)	: file name of the file to open and
//			which contains the instructions to execute
//	Out :
// 	Return Values:
//				CL_OK
//				CL_BAD_PARAM : unexpected end of file/wrong arguments....
//				CL_ERROR	 : Error on execution from underlayers
//***********************************************************
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
e_Result script_ProcessFile( cl8 *pFile, e_EngineType eEngine );

e_Result cl_BuildScript( clu8 u8Data );
e_Result cl_ScriptRegisterReader( t_ScriptData *p_ScriptData, clu32 clu32ArgsNb );
e_Result cl_ScriptRepeatStart( t_ScriptData *p_ScriptData, clu32 clu32ArgsNb );
e_Result cl_ScriptRepeatStop( t_ScriptData *p_ScriptData, clu32 clu32ArgsNb );
e_Result cl_ScriptCmd( t_ScriptData *p_ScriptData, clu32 clu32ArgsNb );
e_Result cl_ScriptDelay( t_ScriptData *p_ScriptData, clu32 clu32ArgsNb );
e_Result cl_GetScriptCommand( clu8 *pScript, clu32 ulLen);


#ifdef _SCRIPT_ENGINE_C_C
eCmdStruct aCmdDef[]=
{
		{
				READER_IP_PORT,
				"READER_IP_PORT",
				2,
				cl_ScriptRegisterReader
		},
		{
				REPEAT_START,
				"REPEAT_START",
				1,
				cl_ScriptRepeatStart
		},
		{
				REPEAT_STOP,
				"REPEAT_STOP",
				1,
				cl_ScriptRepeatStop
		},
		{
				CMD,
				"CMD",
				1,
				cl_ScriptCmd
		},
		{
				DELAY,
				"DELAY",
				1,
				cl_ScriptDelay
		}

};

#else
	extern eCmdStruct aCmdDef[];
#endif // _SCRIPT_ENGINE_C_C


#endif /* SCRIPT_ENGINE_H_ */
/**************************************************/
