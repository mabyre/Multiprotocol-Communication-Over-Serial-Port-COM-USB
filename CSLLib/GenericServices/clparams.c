/********************************************/
/*  Name: clparams.c                		*/
/*											*/
/*  Created on: 28 oct. 2014				*/
/*      Author: fdespres					*/
/********************************************/
/* Description : entry point for application layer to set parameters in CSL : */
/*											*/
/********************************************/
/*	In :									*/
/********************************************/
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "..\cltypes.h"
#include "..\csl.h"
#include "..\inc\clfile.h"
#include "..\inc\generic.h"
#include "..\inc\cltag.h"
/*******************************************************************************************/
/* Name : e_Result cl_GetParams( clu8 	*pParams, clu32 u32ParamsLen, clu8	*pValue			*/
/* , clu32 u32ValueLen )																	*/
/* Description :                                                                            */
/*        from an application, get access to a variable and retreive its values				*/
/*		Caution:!!!! this design is for one application only. No support of multi-thread	*/
/*				or multi-application is possible!!!!! 										*/
/*		Additional semaphore must be set in place outside CSL								*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : 																					*/
/*			clu8 	*pParams	: string containing the variable to write					*/
/*			clu32	u32ParamsLen: Length of Parameter to get value from						*/
/*			clu8	u8RemoveTrailerSpace : remove any space before data 					*/
/*	Out :																					*/
/*			clu8	**ppValue	: value returned from file read								*/
/*			clu32	*pu32ValueLen : length of read value									*/
/*	Out :	none 																			*/
/* 	Return value: e_Result                                                                  */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result cl_GetParams( clu8 *pParams, clu32 u32ParamsLen, clu8	**ppValue, clu32 *pu32ValueLen, clu8 clbRemoveTrailerSpace)
{
	e_Result	status 				= 	CL_ERROR;
	clvoid 		*pVarFileId			=	CL_NULL ;
	clu32		u32StartBlockIndex	=	0;
	clu32		u32StopBlockIndex 	= 	0;
	clu32		u32CrtStartIndex	=	0;
	clu32		u32CrtStopIndex 	= 	0;
	clu32		u32FileStartIndex	=	0;
	clu32		u32FileStopIndex 	= 	0;
	clu32		u32StartVarIndex	=	0;
	clu32		u32StopVarIndex		=	0;
	clu32		u32ValueLen			=	0;			// length of the value field
	clu32		u32StartValueIndex	=	0;
	clu32		u32StopValueIndex	=	0;
	clu32		u32Index			=	0;
    clu8		*pu8ReadValue		=	CL_NULL;
	t_clContext *pCtxt 			= CL_NULL;

	// check incoming param
	if ( pParams == CL_NULL )
		return ( CL_ERROR );

	if ( pu32ValueLen == CL_NULL )
		return ( CL_ERROR );

	if ( ppValue == CL_NULL )
		return ( CL_ERROR );

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL )
		return ( CL_MEM_ERR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )
		return ( CL_ERROR );


	do
	{
		// access to variables available
		if ( pCtxt->ptMenuFileDef != CL_NULL )
		{
			// open file with variable
			if ( CL_FAILED( status = cl_FlOpen( pCtxt->ptMenuFileDef->pMenuVarValues, CL_FILE_BINARY_MODE, &pVarFileId ) ) )
				break;

			u32CrtStartIndex	=	u32FileStartIndex;
			u32CrtStopIndex		=	u32FileStopIndex;

			// find params
			for ( ;; )		// treat <var> if any till the end of the file
			{
				// find the first <var> in the file
				if ( CL_FAILED(  cl_MenuGetBlock( pVarFileId, "<var>", "</var>", u32CrtStartIndex, 0xFFFF, &u32StartBlockIndex, &u32StopBlockIndex ) ) )
					break;

				// check if we have a block
				if (( u32StartBlockIndex == 0 ) | ( u32StopBlockIndex == 0 ))
					break;

				// prepare to skip current block
				u32CrtStartIndex = u32CrtStopIndex;

				// now find the name of the variable
				if ( CL_FAILED(  cl_MenuGetBlock( pVarFileId, "<name>", "</name>", u32StartBlockIndex, u32StopBlockIndex, &u32StartVarIndex, &u32StopVarIndex ) ) )
					break;

				if ( ( u32StartVarIndex == 0 ) | ( u32StartVarIndex == 0 ) )
					break;

				// get next tag value
                if ( CL_FAILED( status = cl_TagGetValue( pVarFileId, u32StartVarIndex, u32StopVarIndex, &pu8ReadValue, &u32ValueLen, &u32StartValueIndex, &u32StopValueIndex, CL_OK ) ) )
					break;

				// update index to find next block if failed to find correct variable name in this search
				u32CrtStartIndex = u32StopBlockIndex;

				// len of variable read
				*pu32ValueLen = (u32StopValueIndex - u32StartValueIndex + 1) ;

				// if we didn't found a value => skip to next block
				if ( pu8ReadValue == CL_NULL )
				{
					if ( CL_FAILED( status = cl_FlSetPos( pVarFileId, u32StopBlockIndex ) ) )
					{
						pCtxt->ptHalFuncs->fnFreeMem( pu8ReadValue );
						break;
					}
					else
						continue;
				}
                //DEBUG_PRINTF("%s\n", pu8ReadValue);
				// check if the found variable has the correct length
				if ( u32ParamsLen != u32ValueLen )
				{
					if ( CL_FAILED( pCtxt->ptHalFuncs->fnFreeMem( pu8ReadValue ) ) )
						break;

					continue;
				}

				// check if we are on the correct variable
				if ( memcmp( pu8ReadValue, pParams, u32ParamsLen ) )
				{
					// free memory
					if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnFreeMem( pu8ReadValue ) ) )
						break;

					continue;
				}
				else
				{
					if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnFreeMem( pu8ReadValue ) ) )
						break;

					//*******************
					// now get the value!
					// now find the name of the variable
					if ( CL_FAILED(  cl_MenuGetBlock( pVarFileId, "<value>", "</value>", u32StartBlockIndex, u32StopBlockIndex, &u32StartVarIndex, &u32StopVarIndex ) ) )
						break;

					if ( ( u32StartVarIndex == 0 ) | ( u32StartVarIndex == 0 ) )
						break;

					// get next tag value
					if ( CL_FAILED( status = cl_TagGetValue( pVarFileId, u32StartVarIndex, u32StopVarIndex, &pu8ReadValue, &u32ValueLen, &u32StartValueIndex, &u32StopValueIndex, CL_OK ) ) )
						break;

					// if we didn't found a value => skip to next block
					if ( pu8ReadValue == CL_NULL )
					{
						if ( CL_FAILED( cl_FlSetPos( pVarFileId, u32StopBlockIndex ) ) )
						{
							pCtxt->ptHalFuncs->fnFreeMem( pu8ReadValue );
							break;
						}
					}
					// map the buffer containing the value
					*ppValue 		= 	pu8ReadValue ;
					// return length
					*pu32ValueLen	=	u32ValueLen ;
	                //DEBUG_PRINTF("%s\n", pu8ReadValue);

					status = CL_OK;
					break;
				}


				// if the variable is not the good, skip to next variable
				if ( CL_FAILED( status ) )
					continue;
			}
		}
		break;

	}while ( 1 );

	// close variable file
	if ( pVarFileId != CL_NULL )
		cl_FlClose( pVarFileId );

	// release memory if needed
	if ( CL_FAILED( status ) )
	{
		if (*ppValue != CL_NULL)
			pCtxt->ptHalFuncs->fnFreeMem( pu8ReadValue );
	}
	return ( status );
}
/*******************************************************************************************/
/* Name : e_Result cl_SetParams( clu8 	*pParams, clu32 u32ParamsLen, clu8	**ppValue		*/
/* 												, clu32 *pu32ValueLen	)					*/
/* Description :                                                                            */
/*        from an application, get access to a variable and retreive its values				*/
/*		Caution:!!!! this design is for one application only. No support of multi-thread	*/
/*				or multi-application is possible!!!!! 										*/
/*		Additional semaphore must be set in place outside CSL								*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : 																					*/
/*			clu8 	*pParams	: string containing the variable to write					*/
/*			clu32	u32ParamsLen: Length of Parameter to get value from						*/
/*			clu8	*ppValue	: value to set in file										*/
/*			clu32	u32ValueLen : length of value to write									*/

/* 	Return value: e_Result                                                                  */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result	cl_SetParams( clu8 	*pParams, clu32 u32ParamsLen, clu8	*pValue, clu32 u32ValueLen )
{
	e_Result	status 				= 	CL_ERROR;
	clvoid 		*pVarFileId			=	CL_NULL ;
	clu32		u32StartBlockIndex	=	0;
	clu32		u32StopBlockIndex 	= 	0;
	clu32		u32CrtStartIndex	=	0;
	clu32		u32CrtStopIndex 	= 	0;
	clu32		u32FileStartIndex	=	0;
	clu32		u32FileStopIndex 	= 	0;
	clu32		u32StartVarIndex	=	0;
	clu32		u32StopVarIndex		=	0;
	clu32		u32Len				=	0;			// length of the value field
	clu32		u32StartValueIndex	=	0;
	clu32		u32StopValueIndex	=	0;
	clu32		u32VarMaxLen			=	0;
	clu32		u32Index			=	0;
    clu8		*pu8ReadValue		=	CL_NULL;
    clu8		*pu8CastedVar		=	CL_NULL;
	t_clContext *pCtxt 				= 	CL_NULL;

	// check incoming param
	if ( pParams == CL_NULL )
		return ( CL_ERROR );

	if ( pValue == CL_NULL )
		return ( CL_ERROR );

	// check globals
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL )
		return ( CL_MEM_ERR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )
		return ( CL_ERROR );


	do
	{
		// access to variables available
		if ( pCtxt->ptMenuFileDef != CL_NULL )
		{
			// open file with variable
			if ( CL_FAILED( status = cl_FlOpen( pCtxt->ptMenuFileDef->pMenuVarValues, CL_FILE_BINARY_MODE, &pVarFileId ) ) )
				break;


			u32CrtStartIndex	=	u32FileStartIndex;
			u32CrtStopIndex		=	u32FileStopIndex;

			// find params
			for ( ;; )		// treat <var> if any till the end of the file
			{
				// find the first <var> in the file
				if ( CL_FAILED(  cl_MenuGetBlock( pVarFileId, "<var>", "</var>", u32CrtStartIndex, 0xFFFF, &u32StartBlockIndex, &u32StopBlockIndex ) ) )
					break;

				// check if we have a block
				if (( u32StartBlockIndex == 0 ) | ( u32StopBlockIndex == 0 ))
					break;

				// prepare to skip current block
				u32CrtStartIndex = u32CrtStopIndex;

				// now find the name of the variable
				if ( CL_FAILED(  cl_MenuGetBlock( pVarFileId, "<name>", "</name>", u32StartBlockIndex, u32StopBlockIndex, &u32StartVarIndex, &u32StopVarIndex ) ) )
					break;

				if ( ( u32StartVarIndex == 0 ) | ( u32StartVarIndex == 0 ) )
					break;

				// get next tag value
                if ( CL_FAILED( status = cl_TagGetValue( pVarFileId, u32StartVarIndex, u32StopVarIndex, &pu8ReadValue, &u32Len, &u32StartValueIndex, &u32StopValueIndex, CL_OK ) ) )
					break;

				// update index to find next block if failed to find correct variable name in this search
				u32CrtStartIndex = u32StopBlockIndex;

				// if we didn't found a value => skip to next block
				if ( pu8ReadValue == CL_NULL )
				{
					if ( CL_FAILED( status = cl_FlSetPos( pVarFileId, u32StopBlockIndex ) ) )
					{
						pCtxt->ptHalFuncs->fnFreeMem( pu8ReadValue );
						break;
					}
					else
						continue;
				}
                //DEBUG_PRINTF("%s\n", pu8ReadValue);
				// check if the found variable has the correct length
				if ( u32ParamsLen != u32Len )
				{
					if ( CL_FAILED( pCtxt->ptHalFuncs->fnFreeMem( pu8ReadValue ) ) )
						break;

					continue;
				}

				// check if we are on the correct variable
				if ( memcmp( pu8ReadValue, pParams, u32ParamsLen ) )
				{
					// free memory
					if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnFreeMem( pu8ReadValue ) ) )
						break;

					continue;
				}
				else
				{
					if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnFreeMem( pu8ReadValue ) ) )
						break;
					//*****************
					// now get the max length of the variable in file
					//*****************
					if ( CL_FAILED(  cl_MenuGetBlock( pVarFileId, "<value_max_len>", "</value_max_len>", u32StartBlockIndex, u32StopBlockIndex, &u32StartVarIndex, &u32StopVarIndex ) ) )
						break;

					if ( ( u32StartVarIndex == 0 ) | ( u32StartVarIndex == 0 ) )
						break;

					// get next tag value
					if ( CL_FAILED( status = cl_TagGetValue( pVarFileId, u32StartVarIndex, u32StopVarIndex, &pu8ReadValue, &u32Len, &u32StartValueIndex, &u32StopValueIndex, CL_OK ) ) )
						break;

					// if we didn't found a value => skip to next block
					if ( pu8ReadValue == CL_NULL )
					{
						if ( CL_FAILED( cl_FlSetPos( pVarFileId, u32StopBlockIndex ) ) )
						{
							pCtxt->ptHalFuncs->fnFreeMem( pu8ReadValue );
							break;
						}
					}

					// get var max Max
					if ( !( u32VarMaxLen = atoi( pu8ReadValue ) ) )
					{
						pCtxt->ptHalFuncs->fnFreeMem( pu8ReadValue );
						break;
					}

					//*******************
					// now get the value
					// now find the name of the variable
					if ( CL_FAILED(  cl_MenuGetBlock( pVarFileId, "<value>", "</value>", u32StartBlockIndex, u32StopBlockIndex, &u32StartVarIndex, &u32StopVarIndex ) ) )
						break;

					if ( ( u32StartVarIndex == 0 ) | ( u32StartVarIndex == 0 ) )
						break;

					// get next tag value
					if ( CL_FAILED( status = cl_TagGetValue( pVarFileId, u32StartVarIndex, u32StopVarIndex, &pu8ReadValue, &u32Len, &u32StartValueIndex, &u32StopValueIndex, CL_OK ) ) )
						break;

					// if we didn't found a value => skip to next block
					if ( pu8ReadValue == CL_NULL )
					{
						if ( CL_FAILED( cl_FlSetPos( pVarFileId, u32StopBlockIndex ) ) )
						{
							pCtxt->ptHalFuncs->fnFreeMem( pu8ReadValue );
							break;
						}
					}

					if ( CL_FAILED( pCtxt->ptHalFuncs->fnFreeMem( pu8ReadValue ) ) )
						break;


					// place pointer on value to change
					// but first check that we have enough space to write what we want to and correct u32StartValueIndex accordingly
                    if (( u32StopValueIndex - u32StartValueIndex - 1) != u32ValueLen)
                        u32StartValueIndex = u32StopValueIndex - u32VarMaxLen - 1;

					if ( CL_FAILED( status = cl_FlSetPos( pVarFileId, u32StartValueIndex ) ) )
						break;

					// at this stage, expand/cast variable if necessary to respect variable size declaration
					//if ( CL_FAILED( pCtxt->ptHalFuncs->fnAllocMem((clvoid **)&pu8CastedVar, u32VarMaxLen ) ))
					if ( CL_FAILED( csl_malloc((clvoid **)&pu8CastedVar, u32VarMaxLen ) ))
						break;

					// copy existing data and replace missing data with ' '
					for ( u32Index = 0; u32Index < u32VarMaxLen; u32Index++ )
					{
						if ( u32VarMaxLen > u32ValueLen )
						{
							if ( u32Index < (u32VarMaxLen - u32ValueLen) )
                                pu8CastedVar[ u32Index ] = ' ';
							else
								pu8CastedVar[ u32Index ] = pValue[ u32Index - ( u32VarMaxLen - u32ValueLen )];
						}
						else
							pu8CastedVar[ u32Index ] = pValue[ u32Index ];
					}

					// Write the value!!!!
					status = cl_FlWrite( pVarFileId, pu8CastedVar, u32VarMaxLen );

					pCtxt->ptHalFuncs->fnFreeMem( pu8CastedVar );
					break;
				}


				// if the variable is not the good, skip to next variable
				if ( CL_FAILED( status ) )
					continue;
			}
		}
		break;

	}while ( 1 );

	// close variable file
	if ( pVarFileId != CL_NULL )
		cl_FlClose( pVarFileId );

	return ( status );
}
