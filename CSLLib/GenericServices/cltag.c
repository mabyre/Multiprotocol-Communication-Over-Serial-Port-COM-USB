/********************************************/
/*  Name: cltag.c                */
/*											*/
/*  Created on: 5 nov. 2014						*/
/*      Author: fdespres						*/
/********************************************/
/* Description : */
/*											*/
/********************************************/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "..\csl.h"
#include "..\inc\generic.h"
#include "..\inc\clreaders.h"
#include "..\inc\clfile.h"


/*******************************************************************************************/
/* Name : e_Result cl_TagSetValue( clvoid *pFileId, , clu32 u32StartBoundary, clu32 u32StopBoundary */
/*								cl8 *pTagValue, cl8 *p8Value, clu32 u32ValueLen ) 	*/
/* Description :                                                                            */
/*        Find value after current tag in a file.											*/
/*		  on Success, the file pointer points to the end of the found value					*/
/*		  if current file pointer is pointing on '<', then it bypass it and look for an other */
/*		  '<'. 																				*/
/*			if no value is present after the tag, pointer is set to NULL but CL_OK			*/
/*			if the boundary is exceeded, then status is set to OK							*/
/*		 on CL_ERROR, the file pointer remains unchanged									*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : clvoid	*pFile			: File descriptor to parse 									*/
/*		clu32 u32StartBoundary		: Index if the file where we shall start to look at		*/
/*		clu32 u32StopBoundary		: Index in the file where we shall stop to look at		*/
/*		cl8 *pTagValue				: Tag to look for										*/
/*		clu8 *p8Value				: value 												*/
/*		clu32 u32ValueLen			: length of the tag found								*/
/*	Out : 																					*/
/* 	Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result cl_TagSetValue( clvoid *pFileId, clu32 u32StartBoundary, clu32 u32StopBoundary, cl8 **pp8Value, clu32 *pu32ValueLen )
{
	cl8 		cChar				=	'\0';
	clu32 		clu32InitialIndex 	= 	u32StartBoundary;
	clu32		u32DataIndex		= 	0;
	e_Result	status				=	CL_ERROR;
	clu8		bStartSign			= 	0;
	clu32		u32Index			=	0;
	t_clContext	*pCtxt 				= 	CL_NULL;
	clu32		u32Jndex			= 	0;
	cl8			*pValue				=	CL_NULL;

	// check global parameters
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

	// check incoming argument
	if ( pFileId == CL_NULL )
		return ( CL_ERROR );


	// check if we can access the file
	if ( CL_FAILED( status = cl_FlSetPos( pFileId, u32StartBoundary ) ) )
		return ( CL_ERROR );

	// check we are pointing on a tag and go to the end if ok
	for (;;)
	{
		DEBUG_PRINTF("TagSetValue:Thread: BEGIN");

		 if ( CL_FAILED( status = cl_FileGetChr( pFileId, &cChar ) ) )
			 break;

		if ( CL_FAILED( status = cl_FlGetPos( pFileId, &u32Index ) ) )
			break;

		if ( u32Index > u32StopBoundary)
			break;

		 // if we are on start of tag, indicate it
		if ( cChar == '<')
		{
			bStartSign	=	1;
			continue;
		}

		// if the data read is not a tag	=> exit
		if ( bStartSign ==	0	)
		{
			status = CL_ERROR;
			break;
		}

		// we are on a end tag => error, no value, exit
		if ( cChar == '/')
		{
			bStartSign 	= 	0;
			status 		= 	CL_ERROR;
			break;
		}

		// if end of tag => then continue to get the value
		if	( cChar == '>')
		{
			status = CL_OK;
			break;
		}
	}

	// if we found a valid tag, then retrieve the value
	if ( CL_SUCCESS( status ) )
	{
		status 	=	CL_ERROR;
		if ( CL_FAILED( status = cl_FlGetPos( pFileId, &u32DataIndex ) ) )
			return ( CL_ERROR );

		while ( CL_SUCCESS( status = cl_FileGetChr( pFileId, &cChar ) ) )	// parse current data. If not < or >, continue
		{

			if ( CL_FAILED( status = cl_FlGetPos( pFileId, &u32Index ) ) )
				break;

			// check if we exceeded the Boundary, if so return a NULL pointer
			if ( u32Index > u32StopBoundary)
			{
				status	=	CL_OK;
				*pp8Value = CL_NULL;
				*pu32ValueLen = 0;
				break;
			}
			// now check validity of data
			if ( !isalnum( cChar ) & ( cChar != ' ') & ( cChar != ':'))		// check we have some valid data
			{
				if ( u32Index == 0 )	// if we have no value exit with pointer set to CL_NULL
				{
					status	=	CL_OK;
					*pp8Value = CL_NULL;
					*pu32ValueLen = 0;
					break;
				}
				else	// we reached the end of the value => allocate memory and send it to upper layer
				{
					//if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnAllocMem( (clvoid **)&pValue, u32Index - u32DataIndex + 1) ) )
					if ( CL_FAILED( status = csl_malloc( (clvoid **)&pValue, u32Index - u32DataIndex + 1) ) )
						break;

					// reset the pointer to the beginning of data
					if ( CL_FAILED( status = cl_FlSetPos( pFileId, u32DataIndex ) ) )
						break;

					// copy the data
					for ( u32Jndex = 0; u32Jndex < (u32Index - u32DataIndex - 1); u32Jndex++)
					{
						if ( CL_FAILED( status  = cl_FileGetChr( pFileId, &cChar ) ) )
							break;

						*(*pp8Value + u32Jndex) = cChar ;
					}
					// add 0x0A and 0x0D at the end of the buffer to help for the display
					*(pValue + u32Index - u32DataIndex -1 ) = 0x0A;
					*(pValue + u32Index - u32DataIndex ) = 0x0D;

					// add the length
					*pu32ValueLen = u32Index - u32DataIndex - 1;

					// reset the pointer to the beginning of data
					if ( CL_FAILED( status = cl_FlSetPos( pFileId, u32DataIndex ) ) )
						break;

				}
				// exit
				break;
			}
			// if so, increment the number of elements
			u32Index++;
		}

	}

	// if FAILED reset the pointer to its initial position
	if ( CL_FAILED( status ) )
	{
		cl_FlSetPos( pFileId, clu32InitialIndex );
		return ( CL_ERROR );
	}
	else
		pp8Value = &pValue;

	return ( status );
}

/*******************************************************************************************/
/* Name : e_Result cl_TagGetValue( clvoid *pFileId, , clu32 u32StartBoundary, clu32 u32StopBoundary */
/*	cl8 **pp8Value, clu32 *pu32ValueLen, , clu32 *pu32ValueStartIndex, clu32 *pu32ValueStopIndex, */
/*	, clu8 bRemoveSpaceTrailer ) 															*/
/*																							*/
/* Description :                                                                            */
/*        Find value after current tag in a file.											*/
/*		  on Success, the file pointer points to the end of the found value					*/
/*		  if current file pointer is pointing on '<', then it bypass it and look for an other */
/*		  '<'. 																				*/
/*			if no value is present after the tag, pointer is set to NULL but CL_OK			*/
/*			if the boundary is exceeded, then status is set to OK							*/
/*		 on CL_ERROR, the file pointer remains unchanged									*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : clvoid	*pFile			: File descriptor to parse 									*/
/*		clu32 u32StartBoundary		: Index if the file where we shall start to look at		*/
/*		clu32 u32StopBoundary		: Index in the file where we shall stop to look at		*/
/*		clu32 *pu32ValueStartIndex	: start index of the value in file						*/
/*		clu32 *pu32ValueStopIndex	: stop index of the value in the file					*/
/*		clu8 bRemoveSpaceTrailer	: remove space before valid data in file				*/
/*	Out : **pp8Value				: buffer allocated which contains the tag including '<' and '>'	*/
/*		 pu32ValueLen				: length of the tag found									*/
/* 	Return value: e_Result                                                                  */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result cl_TagGetValue( clvoid *pFileId, clu32 u32StartBoundary, clu32 u32StopBoundary, cl8 **pp8Value, clu32 *pu32ValueLen, clu32 *pu32ValueStartIndex, clu32 *pu32ValueStopIndex, clu8 bRemoveSpaceTrailer )
{
	cl8 		cChar				=	'\0';
	clu32 		clu32InitialIndex 	= 	u32StartBoundary;
	clu32		u32DataIndex		= 	0;
	e_Result	status				=	CL_ERROR;
	clu8		bStartSign			= 	0;
	clu32		u32Index			=	0;
	t_clContext	*pCtxt 				= 	CL_NULL;
	clu32		u32Jndex			= 	0;
	cl8			*pValue				=	CL_NULL;
	clu8		bFoundValidChar		= 	CL_ERROR;

	// check global parameters
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

	// check incoming argument
	if ( pFileId == CL_NULL )
		return ( CL_ERROR );


	// check if we can access the file
	if ( CL_FAILED( status = cl_FlSetPos( pFileId, u32StartBoundary ) ) )
		return ( CL_ERROR );

	// check we are pointing on a tag and go to the end if ok
	for (;;)
	{
		DEBUG_PRINTF("TagGetValue:Thread: BEGIN");

		 if ( CL_FAILED( status = cl_FileGetChr( pFileId, &cChar ) ) )
			 break;

		if ( CL_FAILED( status = cl_FlGetPos( pFileId, &u32Index ) ) )
			break;

		if ( u32Index > u32StopBoundary)
			break;

		 // if we are on start of tag, indicate it
		if ( cChar == '<')
		{
			bStartSign	=	1;
			continue;
		}

		// if the data read is not a tag	=> exit
		if ( bStartSign ==	0	)
		{
			status = CL_ERROR;
			break;
		}

		// we are on a end tag => error, no value, exit
		if ( cChar == '/')
		{
			bStartSign 	= 	0;
			status 		= 	CL_ERROR;
			break;
		}

		// if end of tag => then continue to get the value
		if	( cChar == '>')
		{
			status = CL_OK;
			break;
		}

	}

	// if we found a valid tag, then retrieve the value
	if ( CL_SUCCESS( status ) )
	{
		status 	=	CL_ERROR;
		if ( CL_FAILED( status = cl_FlGetPos( pFileId, &u32DataIndex ) ) )
			return ( CL_ERROR );

		while ( CL_SUCCESS( status = cl_FileGetChr( pFileId, &cChar ) ) )	// parse current data. If not < or >, continue
		{

			if ( CL_FAILED( status = cl_FlGetPos( pFileId, &u32Index ) ) )
				break;

			// check if we exceeded the Boundary, if so return a NULL pointer
			if ( u32Index > (u32StopBoundary+1))
			{
				status	=	CL_OK;
				*pp8Value = CL_NULL;
				*pu32ValueLen = 0;
				break;
			}
			// now, check validity of data
            if ( ( !isalnum( cChar ) ) & ( cChar != ' ') & ( cChar != ':')& ( cChar !='_')& (cChar !='.'))		// check we have some valid data
			{
				if ( u32Index == 0 )	// if we have no value exit with pointer set to CL_NULL
				{
					status	=	CL_OK;
					*pp8Value = CL_NULL;
					*pu32ValueLen = 0;
					break;
				}
				else	// we reached the end of the value => allocate memory and send it to upper layer
				{
					if ( CL_FAILED( status = csl_malloc( (clvoid **)&pValue, u32Index - u32DataIndex + 1) ) )
						break;

					// reset the pointer to the beginning of data
					if ( CL_FAILED( status = cl_FlSetPos( pFileId, u32DataIndex ) ) )
						break;

					// copy the data
					for ( u32Jndex = 0; u32Jndex < (u32Index - u32DataIndex - 1); u32Jndex++)
					{
						if ( CL_FAILED( status  = cl_FileGetChr( pFileId, &cChar ) ) )
							break;

						*( pValue + u32Jndex) = cChar ;
					}
					// add 0x0A and 0x0D at the end of the buffer to help for the display
					*( pValue + u32Index - u32DataIndex -1 ) = 0x0A;
					*( pValue + u32Index - u32DataIndex ) = 0x0D;

					// add the length
					*pu32ValueLen = u32Index - u32DataIndex - 1;

					// reset the pointer to the beginning of data
					if ( CL_FAILED( status = cl_FlSetPos( pFileId, u32DataIndex ) ) )
						break;
				}
				// exit
				break;
			}
			// if so, increment the number of elements in case it is not a space and it is authorized
            if ( bRemoveSpaceTrailer == CL_OK)
            {
            	if ( cChar !=' ')
            		bFoundValidChar = CL_OK;
            	else
            	{
            		if ( bFoundValidChar == CL_ERROR )
            			u32DataIndex = u32Index;
            	}
            }
            u32Index++;
		}
	}

	// if FAILED reset the pointer to its initial position
	if ( CL_FAILED( status ) )
	{
		cl_FlSetPos( pFileId, clu32InitialIndex );
		return ( CL_ERROR );
	}
	else
	{
		// add pointer to data
		*pp8Value = (cl8 *)pValue;

		// Start Index of value in file
		*pu32ValueStartIndex = u32DataIndex;

		// Stop Index of value in File
		*pu32ValueStopIndex = u32Index;
	}
	return ( status );
}

/*******************************************************************************************/
/* Name : e_Result cl_FindNextTag( clvoid *pFileId, cl8 **pp8Tag, clu32 *pu32TagLen ) 		*/
/* Description :                                                                            */
/*        Find next tag in a file.															*/
/*		  on Success, the file pointer points to the found Tag								*/
/*		  if current file pointer is pointing on '<', then it bypass it and look for an other */
/*		  '<'. 																				*/
/*		 on CL_ERROR, the file pointer remains unchanged									*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : clvoid	*pFile			: File descriptor to parse 									*/
/*	Out : **pp8Tag				: buffer allocated which contains the tag including '<' and '>'	*/
/*		 pu32TagLen				: length of the tag found									*/
/* 	Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result cl_FindNextTag( clvoid *pFileId, cl8 **pp8Tag, clu32 *pu32TagLen )
{
	clu8 	bFirstChar 			= CL_ERROR;
	clu8 	bStartOfTag 		= CL_ERROR;
	cl8 	cChar;
	clu32	clu32InitialIndex	= 0;
	clu32	clu32Index			= 0;
	clu32	u32Jndex			= 0;
	clu32	u32Pos				= 0;
	e_Result	status			= CL_ERROR;
	t_clContext	*pCtxt 			= 	CL_NULL;


	// check global parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )

	if ( pp8Tag == CL_NULL )
		return ( CL_ERROR );

	if ( pFileId == CL_NULL )
		return ( CL_ERROR );

	// check if we can access the file
	if ( CL_FAILED( status = cl_FlGetPos( pFileId, &clu32InitialIndex ) ) )
		return ( CL_ERROR );

	if ( CL_FAILED( status = cl_FlSetPos( pFileId, clu32InitialIndex + 1) ) )
		return ( CL_ERROR );

	// avoid the first tag if we are pointing on it... catch the next one
	while ( CL_SUCCESS( status = cl_FileGetChr( pFileId, &cChar ) ) )
	{

		if ( cChar == '<')	// if start signaling
		{
			if ( bFirstChar == CL_OK ) // if the first char is the start of a tag  then skip it to get next tag
			{
				bFirstChar = CL_ERROR;		// remove first char if a start of tag. Find the next tag
				continue;
			}
			bStartOfTag = CL_OK;
		}
		else
			bFirstChar = CL_ERROR;		// use to remove the first char as we are still pointing to former one


		// start to save the tag
		if ( bStartOfTag == CL_OK )
		{
			clu32Index++;

			if ( cChar == '>') // we have finished.... The tag is found! We shall rewind the file pointer to the beginnning of the new found tag
			{
				bStartOfTag = CL_ERROR;

				// get current tag position in File.
				if ( CL_FAILED( status = cl_FlGetPos( pFileId, &u32Pos ) ) )
					break;

				// rewind the pointer to the new tag position
				if ( CL_FAILED( status = cl_FlSetPos( pFileId, u32Pos - (clu32)clu32Index ) ) )
					break;

				// now, allocate memory for the tag
				if ( CL_FAILED( status = csl_malloc( (clvoid **)pp8Tag, clu32Index ) ))
					break;

				if ( pp8Tag == CL_NULL )
					break;

				if ( *pp8Tag == CL_NULL )
					break;

				// copy the data
				for ( u32Jndex = 0; u32Jndex < clu32Index; u32Jndex++)
				{
					if ( CL_FAILED( status  = cl_FileGetChr( pFileId, &cChar ) ) )
						break;

					*(*pp8Tag + u32Jndex) = cChar ;
				}

				if ( u32Jndex != clu32Index)
					break;

				// save len
				*pu32TagLen = clu32Index;

				// get current tag position in File.
				if ( CL_FAILED( status = cl_FlGetPos( pFileId, &u32Pos ) ) )
					break;

				// rewind the pointer to the new tag position
				if ( CL_FAILED( status = cl_FlSetPos( pFileId, u32Pos - (clu32)clu32Index ) ) )
					break;

				status = CL_OK;

				break;
			}
		}
	}

	if ( CL_FAILED( status ) )
	{
		// free any allocated memory
		pCtxt->ptHalFuncs->fnFreeMem( pp8Tag );

		*pp8Tag = CL_NULL;

		// rewind the pointer to the new tag position
		cl_FlSetPos( pFileId, clu32InitialIndex );

	}
	return ( status );
}

/*******************************************************************************************/
/* Name : e_Result cl_FindPreviousTag( clvoid *pFileId, cl8 **pp8Tag, clu32 *pu32TagLen ) 	*/
/* Description :                                                                            */
/*        Find previous tag in a file.														*/
/*		  on Success, the file pointer points to the found Tag								*/
/*		  if current file pointer is pointing on '<', then it bypass it and look for an other */
/*		  '<'. 																				*/
/*		 on CL_ERROR, the file pointer remains unchanged									*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : clvoid	*pFile			: File descriptor to parse 									*/
/*	Out : **pp8Tag				: buffer allocated which contains the tag including '<' and '>'	*/
/*		 pu32TagLen				: length of the tag found									*/
/* 	Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result cl_FindPreviousTag( clvoid *pFileId, cl8 **pp8Tag, clu32 *pu32TagLen )
{
	clu8 	bFirstChar 			=	CL_ERROR;
	clu8 	bStartOfTag 		= 	CL_ERROR;
	cl8 	cChar;
	clu32	clu32Index			= 	0;
	clu32 	u32Jndex			= 	0;
	clu32	u32Pos				= 	0;
	clu32	clu32InitialIndex	=	0;
	e_Result	status			= 	CL_ERROR;
	t_clContext	*pCtxt 			= 	CL_NULL;


	// check global parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )

	if ( pp8Tag == CL_NULL )
		return ( CL_ERROR );

	if ( pFileId == CL_NULL )
		return ( CL_ERROR );


	// check if we can access the file
	if ( CL_FAILED( status = cl_FlGetPos( pFileId, &clu32InitialIndex ) ) )
		return ( CL_ERROR );


	// avoid the first tag if we are pointing on it... catch the next one
	do
	{

		// get current file position
		if ( CL_FAILED( status = cl_FlGetPos( pFileId, &u32Pos ) ) )
			break;

		// rewind the pointer to previous char
		if ( bFirstChar == CL_OK )
		{
			if ( CL_FAILED( status = cl_FlSetPos( pFileId, u32Pos - 1) ) )
				break;
		}
		else
			if ( CL_FAILED( status = cl_FlSetPos( pFileId, u32Pos - 2) ) )
				break;

		// get the char
		if ( CL_FAILED( status  = cl_FileGetChr( pFileId, &cChar ) ) )
			break;

		if ( cChar == '>')	// if start signaling
		{
			if ( bFirstChar == CL_OK ) // if the first char is the start of a tag  then skip it to get next tag
			{
				bFirstChar = CL_ERROR;		// remove first char if a start of tag. Find the next tag
				continue;
			}
			bStartOfTag = CL_OK;
		}
		else
			bFirstChar = CL_ERROR;		// use to remove the first char as we are still pointing to former one


		// start to save the tag
		if ( bStartOfTag == CL_OK )
		{
			clu32Index++;				// increment number of chars
			if ( cChar == '<') // we have finished.... The tag is found! We shall rewind the file pointer to the beginnning of the new found tag
			{
				bStartOfTag = CL_ERROR;
				// get current tag position in File.
				if ( CL_FAILED( status = cl_FlGetPos( pFileId, &u32Pos ) ) )
					break;

				// rewind the pointer to the new tag position
				if ( CL_FAILED( status = cl_FlSetPos( pFileId, u32Pos - 1 ) ) )
					break;

				// now, allocate memory for the tag
				if ( CL_FAILED( status = csl_malloc( (clvoid **)pp8Tag, clu32Index ) ))
					break;

				if ( pp8Tag == CL_NULL )
					break;

				if ( *pp8Tag == CL_NULL )
					break;

				// copy the data in allocated memory
				for ( u32Jndex = 0; u32Jndex < clu32Index; u32Jndex++)
				{
					if ( CL_FAILED( status  = cl_FileGetChr( pFileId, &cChar ) ) )
						break;

					*(*pp8Tag + u32Jndex) = cChar ;
				}

				if ( u32Jndex != clu32Index)
					break;

				// save len
				*pu32TagLen = clu32Index;

				status = CL_OK;

				break;
			}
		}
	}while ( 1 );

	if ( CL_FAILED( status ) )
	{
		// free any allocated memory
		pCtxt->ptHalFuncs->fnFreeMem( pp8Tag );

		*pp8Tag = CL_NULL;

		// rewind the pointer to the new tag position
		cl_FlSetPos( pFileId, clu32InitialIndex );
	}
	return ( status );
}




