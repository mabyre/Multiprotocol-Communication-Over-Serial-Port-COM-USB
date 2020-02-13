/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "..\csl.h"
#include "..\cltypes.h"
#include "..\clsupport.h"
#include "..\inc\menu_parser.h"
#include "..\inc\script_engine.h"
#include "..\inc\generic.h"
#include "..\inc\clfile.h"
#include "..\inc\cltag.h"
#include "..\inc\clreaders.h"

/********************************************************************************/
/************* Prototypes definition	*****************************************/
/********************************************************************************/
e_Result cl_PrintDebugMenu( clu8 *pFileDebugId, cl8 *pValue, clu32 u32ValueLen );
/********************************************************************************/
/********************************************************************************/
e_Result cl_PrintDebugMenu( clu8 *pFileDebugId, cl8 *pValue, clu32 u32ValueLen )
{
	e_Result status	=	CL_OK;

	DEBUG_PRINTF("%s", pValue);

	return ( status );
}

/*******************************************************************************************/
/* Name : e_Result cl_MenuBuild( clu8 *pFile ) 												*/
/* Description :                                                                            */
/*        Build and execute a menu from a menu file descriptor 								*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : clvoid *pFile			: File descriptor to parse 									*/
/* 	Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result cl_MenuBuild( clu8 *pFile )
{
	e_Result status 			= 	CL_ERROR;
	clu8 	*pu8Level 			= 	(clu8 *)"1";
	clu32	u32LevelLen			= 	(clu32) strlen( pu8Level );
	clu8	*pu8NextLevel 		= 	CL_NULL;
	clu32 	u32NextLevelLen 	= 	0;
	t_clContext	*pCtxt 			= 	CL_NULL;
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

	if ( pCtxt->ptHalFuncs->fnFileClose == CL_NULL )
		return ( CL_ERROR );

	if ( pFile == CL_NULL )	// file pointer was not initialized. Returns CL_PARAMS_ERR
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFileOpen( pFile, CL_FILE_BINARY_MODE, &pFileId) != CL_OK ) // file on opening file
		return ( CL_ERROR );

	// loop in the menu between the different items until we exit
	for(;;)
	{
		// go in the menu and display currrent level of menu
		if ( CL_FAILED( status = cl_MenuBuildItem( pFileId, pu8Level, &pu8NextLevel, &u32NextLevelLen ) ) )
			break;

		// get next element in the menu
		if ( (pu8NextLevel == CL_NULL ) & ( u32NextLevelLen == 0 ) )
			break;
		else
		{
			// update the menu with next level
			pu8Level = pu8NextLevel;
			u32LevelLen = u32NextLevelLen;
		}

	}while (pu8NextLevel  != CL_NULL );


	// the menu is finished => exit from program
	return (status );
}


/*******************************************************************************************/
/* Name : e_Result cl_MenuBuildItem( clu8 *pFileId,											*/
/* 							clu8 *pu8CrtLevel, clu32 u32CrtLevelLen,						*/
/* 							clu8 **pu8NextLevel, clu32 *pu32NextLevelLen ) 					*/
/* Description :                                                                            */
/*        from a level, build the corresponding element										*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : clvoid *pFile			: File descriptor to parse 									*/
/*		clu8 *pu8CrtLevel		: string which describes current level						*/
/*		clu32 u32CrtLevel		: current level lenght descriptor							*/
/*	Out : 																					*/
/*		clu8 **pu8NextLevel		: Next Level string descriptor								*/
/*		clu32 *pu32NextLevelLen	: Length of the next level									*/
/* 	Return value: e_Result                                                                  */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result cl_MenuBuildItem( clu8 *pFileId, clu8 *pszu8CrtLevel, clu8 **ppu8NextLevel, clu32 *pu32NextLevelLen )
{
	e_Result status 					= 	CL_ERROR;
	t_clContext	*pCtxt 					= 	CL_NULL;

	clu32 	u32StartBlockIndex 			= 	0;			// start index of the block
	clu32	u32StopBlockIndex			= 	0xFFFFFFFF;	// stop index of the block
	clu32	u32ActionStartBlockIndex	=	0;	// start index of the action block
	clu32 	u32ActionStopBlockIndex		=	0;	// stop index of the action block
	clu32	u32DisplayStartBlockIndex	=	0;	// start index of the display block
	clu32 	u32DisplayStopBlockIndex	=	0;	// stop index of the display block
	clu32	u32ChoiceStartBlockIndex	=	0;	// start index of the display block
	clu32 	u32ChoiceStopBlockIndex		=	0;	// stop index of the display block
	clu32	u32CrtStartIndex			=	0;	// current start index
	clu32	u32CrtStopIndex				=	0;	//
	clu32	u32StartValueIndex			=	0;	// start index of a value (to set/to get)
	clu32	u32StopValueIndex			=	0;	// stop index of a vallue (to set/to get)
	clu8	*pValue						=	CL_NULL;	// pointer on value field
	clu32	u32ValueLen					=	0;

	// check global parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )


	// first check incoming parameters
	if ( pFileId ==  CL_NULL )
		return ( CL_ERROR );

	if ( pszu8CrtLevel == CL_NULL )
		return ( CL_ERROR );

	if ( ppu8NextLevel == CL_NULL )
		return ( CL_ERROR );

	if ( pu32NextLevelLen == CL_NULL)
		return ( CL_ERROR );


	cl_FlGetPos( pFileId, &u32StartBlockIndex );
	// now proceed on treatment

	for ( ;; )	// find the level for entry
	{
		// find the file pointer where stands Current Menu Level
		if ( CL_FAILED(  cl_MenuGetBlock( pFileId, "<level>", "</level>", u32StartBlockIndex, 0xFFFFFFFF, &u32StartBlockIndex, &u32StopBlockIndex ) ) )
			return ( CL_ERROR );

		// get the value of level
		if ( CL_FAILED( status = cl_TagGetValue( pFileId, u32StartBlockIndex, u32StopBlockIndex, &pValue, &u32ValueLen, &u32StartValueIndex, &u32StopValueIndex, CL_OK ) ) )
			return ( CL_ERROR );

		// if we didn't found a value => skip to next block
		if ( pValue == CL_NULL )
		{
			if ( CL_FAILED( cl_FlSetPos( pFileId, u32StopBlockIndex ) ) )
			{
				pCtxt->ptHalFuncs->fnFreeMem( pValue );
				return ( CL_ERROR );
			}
			else
				continue;
		}
		// check if we have the same value
		if ( memcmp( pszu8CrtLevel, pValue, u32ValueLen ) )
			status = CL_ERROR;

		if ( CL_FAILED( pCtxt->ptHalFuncs->fnFreeMem( pValue ) ) )
			return ( CL_ERROR );

		if ( CL_FAILED( status ) )
			continue;

		// find the file pointer where stands Current Menu Level
		if ( CL_FAILED(  cl_MenuGetBlock( pFileId, "<title>", "</title>", u32StartBlockIndex, u32StopBlockIndex, &u32CrtStartIndex, &u32CrtStopIndex ) ) )
			return ( CL_ERROR );

		if  ( ( u32CrtStartIndex == 0 ) | ( u32CrtStopIndex == 0 ) )
			continue;

		// print the value
		status =  cl_MenuPrintValue( pFileId, "<title>" );

		cl_FlGetPos( pFileId, &u32CrtStartIndex );

		for ( ;; )		// treat <display> if any
		{
			// find the file pointer where stands Current Menu Level
			if ( CL_FAILED(  cl_MenuGetBlock( pFileId, "<display>", "</display>", u32CrtStopIndex, u32StopBlockIndex, &u32DisplayStartBlockIndex, &u32DisplayStopBlockIndex ) ) )
				return ( CL_ERROR );

			if (( u32DisplayStartBlockIndex == 0 ) | ( u32DisplayStopBlockIndex == 0 ))
				break;

			// find the file pointer where stands Current Menu Level
			if ( CL_FAILED(  cl_MenuGetBlock( pFileId, "<description>", "</description>", u32DisplayStartBlockIndex, u32DisplayStopBlockIndex, &u32StartBlockIndex, &u32StopBlockIndex ) ) )
				return ( CL_ERROR );
/*
			if (( u32DisplayStartBlockIndex == 0 ) | ( u32DisplayStopBlockIndex == 0 ))
				break;
*/

			cl_MenuPrintValue( pFileId, "<description>" );

			// now treat any inner <action>
			if ( CL_FAILED(  cl_MenuGetBlock( pFileId, "<action>", "</action>",u32StartBlockIndex, u32StopBlockIndex, &u32ActionStartBlockIndex, &u32ActionStopBlockIndex ) ) )
			{
				// skip current block
				cl_FlSetPos( pFileId, u32StartBlockIndex);
				// continue to parse the file
				continue;
			}
			else
			{
				// process the action field
				status = cl_MenuProcessAction( pFileId, u32ActionStartBlockIndex, u32ActionStopBlockIndex );
			}

			// skip current block
			cl_FlSetPos( pFileId, u32StopBlockIndex);
		}

		for ( ;; )		// treat <choice> if any
		{
			// find the file pointer where stands Current Menu Level
			if ( CL_FAILED(  cl_MenuGetBlock( pFileId, "<choice>", "</choice>", u32StartBlockIndex, u32StopBlockIndex, &u32ChoiceStartBlockIndex, &u32ChoiceStopBlockIndex ) ) )
				return ( CL_ERROR );

			if  ( ( u32ChoiceStartBlockIndex == 0 ) | ( u32ChoiceStopBlockIndex == 0 ) )
				continue;

			// find the file pointer where stands Current Menu Level
			if ( CL_FAILED(  cl_MenuGetBlock( pFileId, "<value>", "</value>", u32ChoiceStartBlockIndex, u32ChoiceStopBlockIndex, &u32CrtStartIndex, &u32CrtStopIndex ) ) )
				return ( CL_ERROR );

			if  ( ( u32CrtStartIndex != 0 ) & ( u32CrtStopIndex != 0 ) )
				cl_MenuPrintValue( pFileId, "<value>" );

			// find the file pointer where stands Current Menu Level
			if ( CL_FAILED(  cl_MenuGetBlock( pFileId, "<description>", "</description>", u32ChoiceStartBlockIndex, u32ChoiceStopBlockIndex , &u32CrtStartIndex, &u32CrtStopIndex ) ) )
				return ( CL_ERROR );

			if  ( ( u32CrtStartIndex != 0 ) & ( u32CrtStopIndex != 0 ) )				// print the Value
				cl_MenuPrintValue( pFileId, "<description>" );


			// now treat any inner <action>
			if ( CL_FAILED(  cl_MenuGetBlock( pFileId, "<action>", "</action>", u32ChoiceStartBlockIndex, u32ChoiceStopBlockIndex, &u32CrtStartIndex, &u32CrtStopIndex ) ) )
			{
				// skip current block
				cl_FlSetPos( pFileId, u32StopBlockIndex);
				// continue to parse the file
				continue;
			}
			else
			{
				// process the action field
				status = cl_MenuProcessAction( pFileId, u32CrtStartIndex, u32CrtStopIndex);
			}

			// skip current block
			cl_FlSetPos( pFileId, u32StopBlockIndex);
		}
	}
#ifdef FABRICE
	// print the choice list
	if ( CL_FAILED ( cl_MenuPrintChoice(  ) ) )
		return ( CL_ERROR );

	// by default, generate a getchar
	if ( CL_FAILED(  cl_MenuGetInput( pu8Choice, &u32ChoiceLen ) ) )
		return ( CL_ERROR );

	// reinitialize to the start of the block
	cl_FlSetPos( pFileId, u32StartIndex );

	//
	while ( u32ChoiceStartIndex)
	{
		// from this point, looks for the choice with corresponding value
		status = cl_FlGetBlock( pFileId, "<choice>", "</choice>", &u32StartBlockIndex, &u32StopBlockIndex );

		// set the file position to start of the block
		cl_FlSetPos( pFileId, u32StartBlockIndex );

		// else check if the value corresponds to the choice the user just did
		status = cl_FlGetBlock( pString, "<value>", "</value>", &u32ValueStartIndex, &u32ValueStopIndex );
		if ( u32ValueStopIndex == 0 )	// there is no value => continue to next choice in order to check if there is a match
			continue;

		cl_FlSetPos( pFileId, u32ValueStartIndex + strlen("<value>" ));	// put the position of the file reader to the value after <value>

		cl_FlGetPos( pFileId, &ulCrtPos );								// get current position

		cl_FlGetValue( pFileId, pu8Value, u32ValueLen );

		// if value is effectively non null, then save a value in the field
		if ( pu8Value != CL_NULL )
		{
			if ( u32ChoiceLen == u32ValueLen ) // then error !!!!
				cl_FlSetValue( pFileId, pu8Choice, u32ChoiceLen);
		}
	}
#endif
//	cl_MenuGetNextLevel( pFileId, pszNextLevel, &u32NextLevelLen );FlSetPos( u32NextLevelStartIndex );
	// memcpy the next level to caller in order to parse next menu element
//	pszString = memcpy( );
	return ( status );
}

/***********************************************************************************************/
/* Name : e_Result cl_MenuProcessAction( clu8	*pFileId, clu32 u32StartIndex, clu32 u32StopIndex  ) 	*/
/* Description :                                                                            	*/
/*        From this file position, pars <action> </action> and performs correct instructions */
/************************************************************************************************/
/* Parameters:                                                                              	*/
/*  --------------                                                                          	*/
/*  In : clvoid	*pFileId			: File descriptor to parse 									*/
/*	Out :																						*/
/* 	Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result cl_MenuProcessAction( clu8	*pFileId, clu32 u32StartIndex, clu32 u32StopIndex )
{
	e_Result status	= CL_ERROR;
	e_ActionScript eActScript = ACTION_SCRIPT_UNKNOWN;
	cl8	*pValue		=	CL_NULL;
	clu32	u32ValueLen	=	0;
	clu32	u32Pos		=	0;
	clu32	u32StartVar	=	0;
	clu32	u32StopVar	=	0;
	clu32	u32StartValueIndex	=	0;
	clu32	u32StopValueIndex	=	0;
	t_clContext	*pCtxt 			= 	CL_NULL;

	// check global parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )
		return ( CL_ERROR );

	if ( CL_FAILED( cl_FlGetPos( pFileId, &u32Pos)) )
		return ( CL_ERROR );

	if ( CL_FAILED( cl_FlSetPos( pFileId, u32StartIndex) ) )
	{
		cl_FlSetPos( pFileId, u32Pos );
		return ( CL_ERROR );
	}

	//
	for ( ;; )
	{
		// get the value of action
		if ( CL_FAILED( status = cl_TagGetValue( pFileId, u32StartIndex, u32StopIndex, &pValue, &u32ValueLen, &u32StartValueIndex, &u32StopValueIndex, CL_OK ) ) )
			break;

		// if we didn't found a tag, get it to classify next steps
		if ( pValue == CL_NULL )
		{
			if ( CL_FAILED( cl_FlSetPos( pFileId, u32StopIndex ) ) )
			{
				pCtxt->ptHalFuncs->fnFreeMem( pValue );
				break;
			}
			else
				continue;
		}
		// check if we have the same value
		if ( !memcmp( "none", pValue, u32ValueLen ) )
			eActScript = ACTION_SCRIPT_NONE;

		// check if we have the same value
		if ( !memcmp( "set_value", pValue, u32ValueLen ) )
			eActScript = ACTION_SCRIPT_SET_VALUE;

		// check if we have the same value
		if ( !memcmp( "get_value", pValue, u32ValueLen ) )
			eActScript = ACTION_SCRIPT_GET_VALUE;

		if ( CL_FAILED( pCtxt->ptHalFuncs->fnFreeMem( pValue ) ) )
			break;

		// find the file pointer where stands Current Menu Level
		if ( CL_FAILED(  cl_MenuGetBlock( pFileId, "<var>", "</var>", u32StartIndex, u32StartIndex, &u32StartVar, &u32StopVar ) ) )
			break;

		// if there is a var specified
/*
		if ( ( u32StartVar != 0 ) & ( u32StopVar != 0 ) )
		{
			cl_FlScriptVarGet( );
		}
		else
			break;
*/

		break;
	}


	if ( CL_FAILED( status ) )
	{
		pCtxt->ptHalFuncs->fnFreeMem( pValue );


	}

	return ( status );
}

/***********************************************************************************************/
/* Name : e_Result cl_MenuGetBlock( clu8 *pFileId, cl8 *pszStartPattern, cl8 *pszStopPattern 	*/
/*			clu32 u32StartBoundary, clu32StopBoundary, clu32 *pu32StartIndex, clu32 *pu32StopIndex ) 	*/
/* Description :                                                                            	*/
/*        Find the block starting with pszStartPattern and pszStopPattern between StartBoundary */
/*		and StopBoundary. Returns the index of the block										*/
/*		 on CL_ERROR, the file pointer remains unchanged										*/
/************************************************************************************************/
/* Parameters:                                                                              	*/
/*  --------------                                                                          	*/
/*  In : clvoid	*pFileId			: File descriptor to parse 									*/
/*		cl8 *pszStartPattern		: string of first pattern to search							*/
/*		cl8 *pszStopPattern			: string of last pattern to search							*/
/*		clu32 u32StartBoundary		: Index if the file where we shall start to look at			*/
/*		clu32 u32StopBoundary		: Index in the file where we shall stop to look at			*/
/*	Out :																						*/
/*		clu32	*pu32StartIndex		: Start Index of the block pattern (first byte corresponding */
/*									to pszStartPattern in File 									*/
/*		clu32	*pu32StopIndex		: Stop Index of the block pattern (first byte corresponding */
/*									to pszStartPattern in File 									*/
/* 	Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result cl_MenuGetBlock( clu8 *pFileId, cl8 *pszStartPattern, cl8 *pszStopPattern, clu32 u32StartBoundary, clu32 u32StopBoundary, clu32 *pu32StartIndex, clu32 *pu32StopIndex )
{
	e_Result 	status 				= 	CL_ERROR;
	cl8			*pTag 				=	CL_NULL;
	clu32		u32TagLen			= 	0;
	t_clContext	*pCtxt 				= 	CL_NULL;
	clu32		u32Pos				=	0;
	clu32 		u32StartValueIndex	= 	0;		// start index of the value to get/set
	clu32 		u32StopValueIndex	= 	0;		// stop index of the value to get/set
	cl8			*pDescTest			= "<description>";
	clu32		u32ValueLen			=	0;
	cl8			*pValue				=	CL_NULL;
	// check global parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )
		return ( CL_ERROR );

	// check incoming parameters
	if ( pFileId == CL_NULL )
		return ( CL_ERROR );

	if ( pszStartPattern == CL_NULL )
		return ( CL_ERROR );

	if (pszStopPattern == CL_NULL )
		return ( CL_ERROR );

	if ( pu32StartIndex == CL_NULL )
		return ( CL_ERROR );

	if ( pu32StopIndex == CL_NULL )
		return ( CL_ERROR );

	if ( CL_FAILED( cl_FlSetPos( pFileId, u32StartBoundary) ) )
		return ( CL_ERROR );


	// get the start tag.
	while ( CL_SUCCESS( status = cl_FindNextTag( pFileId, &pTag, &u32TagLen ) )  )
	{
		// check if we can access the file
		if ( CL_FAILED( status = cl_FlGetPos( pFileId, pu32StartIndex ) ) )
			break;

		// check if we have exceeded the block
		if ( *pu32StartIndex > u32StopBoundary )
		{
			status = CL_ERROR;
			break;
		}

		if ( !memcmp( pTag, pDescTest, 10))
		{
			if ( CL_FAILED( cl_FlGetPos( pFileId, &u32Pos)) )
				return ( CL_ERROR );

			if ( CL_FAILED( cl_FlSetPos( pFileId, u32Pos - 1) ) )
			{
				cl_FlSetPos( pFileId, u32Pos );
				return ( CL_ERROR );
			}

			// get the value of level
			if ( CL_FAILED( status = cl_TagGetValue( pFileId, u32Pos, 0xFFFFFFFF, &pValue, &u32ValueLen, &u32StartValueIndex, &u32StopValueIndex, CL_OK ) ) )
				return ( CL_ERROR );

			DEBUG_PRINTF("pValue = %s",pValue);
		}
		// check if we found the searched pattern
		if ( memcmp( pTag, pszStartPattern, u32TagLen ) == 0 ) // we found the tag
		{
			// release the tag memory
			if ( CL_FAILED( pCtxt->ptHalFuncs->fnFreeMem( pTag ) ) )
					break;

			break;
		}
		else
			pCtxt->ptHalFuncs->fnFreeMem( pTag );

	}

	// free memory of tag in case of error
	if ( CL_FAILED( status ) )
	{
		// release the tag memory
		if ( CL_FAILED( pCtxt->ptHalFuncs->fnFreeMem( pTag ) ) )
				return ( CL_ERROR );
	}

	// get the end tag.
	while ( CL_SUCCESS( status = cl_FindNextTag( pFileId, &pTag, &u32TagLen ) )  )
	{

		// check if we can access the file
		if ( CL_FAILED( status = cl_FlGetPos( pFileId, pu32StopIndex ) ) )
			break;


		// check if we have exceeded the block
		if ( *pu32StopIndex > u32StopBoundary )
		{
			*pu32StartIndex 	= 	0;
			*pu32StopIndex	= 	0;

			status = CL_OK;
			break;
		}

		if ( memcmp( pTag, pszStopPattern, u32TagLen ) == 0 ) // we found the tag
		{
			// release the tag memory
			if ( CL_FAILED( pCtxt->ptHalFuncs->fnFreeMem( pTag ) ) )
					break;

			// check if we can access the file
			if ( CL_FAILED( status = cl_FlGetPos( pFileId, pu32StopIndex ) ) )
			{
				*pu32StartIndex 	= 	0;
				*pu32StopIndex	= 	0;
			}

			if ( CL_FAILED( status = cl_FlSetPos( pFileId, (*pu32StartIndex) ) ) )
			{
				*pu32StartIndex 	= 	0;
				*pu32StopIndex	= 	0;
			}
			break;
		}
		else
		{
			// release the tag memory
			if ( CL_FAILED( pCtxt->ptHalFuncs->fnFreeMem( pTag ) ) )
					break;
		}
	}

	if ( CL_FAILED( status ) )
	{
		if ( pTag != CL_NULL )
		{
			// release the tag memory
			pCtxt->ptHalFuncs->fnFreeMem( pTag );
		}
	}

	return ( status );
}
/*******************************************************************************************/
/* Name : e_Result cl_MenuPrintValue( clvoid *pFileId, cl8 *pszValue )						*/
/* Description :                                                                            */
/*        Print the value just after the tag where the file pointer is						*/
/*		  on error, do nothing 																*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : clvoid	*pFile			: File descriptor to parse 									*/
/*		cl8 *pszValue			: value to print out										*/
/* Out: none																				*/
/* 	Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result cl_MenuPrintValue( clvoid *pFileId, cl8 *pszValue )
{
	e_Result	status				= 	CL_ERROR;
	cl8			*pValue				=	CL_NULL;
	clu32		u32ValueLen			=	0;
	clu32		u32Pos				=	0;
	clu32		u32StartValueIndex	=	0;		// start index of the value found
	clu32		u32StopValueIndex	=	0;		// stop index of the value found
	t_clContext	*pCtxt 				= 	CL_NULL;

	// check global parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )
		return ( CL_ERROR );


	if ( CL_FAILED( cl_FlGetPos( pFileId, &u32Pos)) )
		return ( CL_ERROR );

	if ( CL_FAILED( cl_FlSetPos( pFileId, u32Pos - 1) ) )
	{
		cl_FlSetPos( pFileId, u32Pos );
		return ( CL_ERROR );
	}

	// get the value of level
	if ( CL_FAILED( status = cl_TagGetValue( pFileId, u32Pos, 0xFFFFFFFF, &pValue, &u32ValueLen, &u32StartValueIndex, &u32StopValueIndex, CL_OK ) ) )
		return ( CL_ERROR );

	// Write to output
	cl_PrintDebugMenu( pFileId, pValue, u32ValueLen );

	// release memory
	if ( CL_FAILED( pCtxt->ptHalFuncs->fnFreeMem( pValue ) ) )
		return ( CL_ERROR );


#ifdef FABRICE
	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )

	if ( CL_FAILED( cl_FlGetPos( pFileId, &u32Pos )) )
		return ( CL_ERROR );

	// get the value of level
	if ( CL_FAILED( status = cl_TagGetValue( pFileId, u32Pos, 0xFFFFFFFF, &pValue, &u32ValueLen, CL_OK ) ) )
		return ( CL_ERROR );

	// Write to output
	cl_PrintDebugMenu( pFileId, pValue, u32ValueLen );

	// release memory
	if ( CL_FAILED( pCtxt->ptHalFuncs->fnFreeMem( pValue ) ) )
		return ( CL_ERROR );

#endif
	return( status );
}
