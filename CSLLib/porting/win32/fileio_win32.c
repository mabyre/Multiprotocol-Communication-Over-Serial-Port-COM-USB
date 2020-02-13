/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: fileio_win32.c                                                     */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/* Description: porting layer for file access in WIN32 environment           */
/*****************************************************************************/
#ifdef WIN32
#ifndef __x86_64__

#include "..\..\csl.h"
#include "..\..\inc\generic.h"
#include "fileio_win32.h"
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
/******************************************************************************/
/* Name : cl_FlOpenWin32( cl8 *pFile, e_FileOptions eOpt, clvoid **ppFileId )  */
/* Description :                                                              */
/*       open a file in hosting environment: file shall in Read/Write mode	  */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pFileName: string containing the name of the file			  */
/*		e_FileOptions eOpts : options on opening the file in binary or text mode  */
/* ---------------                                                            */
/*  Out: clvoid **ppFileId : pointer on file handler							*/
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                             		*/
/*  ERROR,                    : Failure on execution                      		*/
/*  PARAMS_ERR,               :  Inconsistent parameters                  		*/
/******************************************************************************/
e_Result cl_FlOpenWin32( cl8 *pFileName, e_FileOptions eOpts, clvoid **ppFileId)
{
	/* check incoming parameters */
	if ( pFileName ==  CL_NULL )
		return ( CL_ERROR );

	if (ppFileId == CL_NULL)
		return ( CL_PARAMS_ERR);

	switch ( eOpts )
	{
		// in Windows, a new line is indicated by \r \n ("Carriage Return' and Line Feed')
	    // in Linux, a new line is indicated by  \n ("Carriage Return' and Line Feed')
		case ( CL_FILE_TEXT_MODE):
				_fmode = _O_TEXT;
				break;
		// for binary, remove the \r
		case ( CL_FILE_BINARY_MODE):
				_fmode = _O_BINARY;
				break;
		default:
			return ( CL_ERROR );
			break;
	}

	*ppFileId = fopen( pFileName, "rb+");

	if (*ppFileId != CL_NULL)
		return ( CL_OK );
	else
		return ( CL_ERROR );

}

/******************************************************************************/
/* Name : cl_FlReadLnWin32( cl8 *pFile, clu8 *pu8Data, clu32 u32Len) 		 */
/* Description :                                                              */
/*       read byte buffers from File and stop when having LF character		*/
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : cl8 *pFile : File to read		                                  */
/* ---------------                                                            */
/*  Out : clu8 **ppu8Data : pointer where to store read data ( memory is allocated   */
/* by the caller)     														  */
/*  clu32	*pu32Len	   : read bytes in the file until new line is reached*/
/* Return value: e_Result                                                     */
/*  OK                        :  Result is OK                             	  */
/*  ERROR,                    : Failure on execution                      	  */
/*  MEM_ERR,                  :  Failure on memory management (failure,    	  */
/*                                  allocation ....)                          */
/*  PARAMS_ERR,               :  Inconsistent parameters                      */
/******************************************************************************/
e_Result cl_FlReadLnWin32( cl8 *pFile, clu8 **ppu8Data, clu32 *pu32Len)
{
	e_Result 	status 			= CL_ERROR;
	clu8 		*pReadBuf		= CL_NULL;
	clu8		*pAllocatedBuf	= CL_NULL;
	t_clContext *pCtxt 			= CL_NULL;
	clu32		clu32CmdLen		= 0;
	clu8		aucBuf[256];

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	// check incoming arguments
	if ( pFile == CL_NULL )
		return ( CL_ERROR );

	if ( ppu8Data == CL_NULL )
		return ( CL_ERROR );

	if ( pu32Len == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL )
		return ( CL_ERROR );

	for ( ;; )
	{
		memset( aucBuf, 0, 256);

		// read the u32 characters and save it in allocated buffer
		pReadBuf = ( clu8 *)fgets( (cl8 *)( aucBuf ), 256, ( FILE *)pFile );

		// if number of bytes read is different from expected number of character then error
		if ( pReadBuf !=  aucBuf )
		{
			DEBUG_PRINTF("cl_FlReadWin32 error\n");
			status = CL_ERROR;
			break;
		}

		// get length of command
		clu32CmdLen = strlen( (cl8 *)pReadBuf );

		// allocate memory to save this command
		if ( CL_FAILED( status = csl_malloc( (void **)&pAllocatedBuf, clu32CmdLen ) ) )
			break;

		// check if allocation was successful
		if ( pAllocatedBuf == CL_NULL)
			break;

		// update status
		status = CL_OK;

		break;
	}

	// treat exit case
	if ( CL_FAILED( status ) )	// if anything failed
	{
		pCtxt->ptHalFuncs->fnFreeMem( pAllocatedBuf ); // free any memory
		*ppu8Data = CL_NULL;							// indicate no valid data to upper layer
		*pu32Len = 0;									// indicate no valid data to upper layer
	}
	else	// normal case
	{

		// copy the data in the allocated buffer
		memcpy( pAllocatedBuf, pReadBuf, clu32CmdLen );

		// pass the information to the upper layers
		*ppu8Data = pAllocatedBuf;
		*pu32Len = clu32CmdLen;
	}

	// whichever happens, test if we reached end of File
	if ( feof( (FILE *)pFile ) != 0 )
		status = CL_EOF_ERR;


	return ( status );
}
/******************************************************************************/
/* Name : cl_FlWriteWin32( cl8 *pFile, clu8 *pu8Data, clu32 u32Len) 			  */
/* Description :                                                              */
/*       Write file in append mode 							                   */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : cl8 *pFile : File to write		                                  */
/*       tDevice *ptDevicesList : list of Clover devices linked to Reader     */
/*                                  passed in arguments                       */
/* ---------------                                                            */
/*  Out: none																  */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                             */
/*  ERROR,                    : Failure on execution                      */
/*  MEM_ERR,                  :  Failure on memory management (failure,   */
/*                                  allocation ....)                      */
/*  PARAMS_ERR,               :  Inconsistent parameters                  */
/******************************************************************************/
e_Result cl_FlWriteWin32( cl8 *pFile, clu8 *pu8Data, clu32 u32Len )
{
	e_Result status = CL_ERROR;
	clu32 clu32WriteLen = 0;

	// check incoming arguments
	if ( pFile == CL_NULL )
		return ( CL_ERROR );

	if ( pu8Data == CL_NULL )
		return ( CL_ERROR );

	if ( u32Len == 0 )
		return ( CL_ERROR );

	// read the u32 characters and save it in allocated buffer
	clu32WriteLen = fwrite( pu8Data, 1, u32Len, (FILE *)pFile );

	// if number of bytes read is different from expected number of character then error
	if ( clu32WriteLen != u32Len )
	{
		DEBUG_PRINTF("cl_FlWriteWin32 error: expected %d, effectively written %d", (cl32)u32Len, ( cl32 )clu32WriteLen  );
		status = CL_ERROR;
	}
	else
		status = CL_OK;

	// whichever happens, test if we reached end of File
	if ( feof( (FILE *)pFile ) != 0 )
		status = CL_EOF_ERR;



	return ( status );
}

/******************************************************************************/
/* Name : cl_FlCloseWin32( cl8 *pFile) 			  */
/* Description :                                                              */
/*       set a Clover devices list registered to one reader                   */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pFile : File to close		                                  */
/* ---------------                                                            */
/*  Out: none																  */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                             		*/
/*  ERROR,                    : Failure on execution                      		*/
/******************************************************************************/
e_Result cl_FlCloseWin32( cl8 *pFile )
{
	if ( pFile == CL_NULL )
		return ( CL_ERROR );

	// close the file
	fclose( ( FILE *)pFile );

	return ( CL_OK );
}
/******************************************************************************/
/* Name : cl_PrintCompleteWin32( clvoid ) 			  							*/
/* Description :                                                              	*/
/*       add a completion after print (typically fflush to put data out on console*/
/********************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : none							                                  	*/
/* ---------------                                                            	*/
/*  Out: none															   		*/
/* Return value: e_Result                                                      	*/
/*  OK                        :  Result is OK                             		*/
/*  ERROR,                    : Failure on execution                      		*/
/******************************************************************************/
e_Result cl_PrintCompleteWin32( clvoid )
{
	fflush( stdout );

	return ( CL_OK);
}

/******************************************************************************/
/* Name : e_Result cl_FlSetPosWin32( cl8 *pFile, clu32 u32Pos )				  	*/
/* Description :                                                              	*/
/*       set the file position pointer to this position						  	*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : cl8 *pFile 			: File 		                                  	*/
/*       clu32 u32Pos			:  position pointer in the file to set		  	*/
/* ---------------                                                            	*/
/*  Out: none																  	*/
/* Return value: e_Result                                                     	*/
/*  OK                        :  Result is OK                             		*/
/*  ERROR,                    : Failure on execution                      		*/
/*  MEM_ERR,                  :  Failure on memory management (failure,   		*/
/*                                  allocation ....)                      		*/
/*  PARAMS_ERR,               :  Inconsistent parameters                  		*/
/******************************************************************************/
e_Result cl_FlSetPosWin32( cl8 *pFile, clu32 u32Pos )
{
	e_Result status = CL_ERROR;
	fpos_t position =  (fpos_t) u32Pos;

	// check incoming arguments
	if ( pFile == CL_NULL )
		return ( CL_ERROR );


	if ( fsetpos( (FILE *)pFile, &position ) == 0 )
		status = CL_OK ;

	if ( feof( (FILE *)pFile ) != 0 )
		status = CL_EOF_ERR;

	return ( status );
}

/******************************************************************************/
/* Name : e_Result cl_FlGetPosWin32( cl8 *pFile, clu32 *pu32Pos )				  	*/
/* Description :                                                              	*/
/*       set the file position pointer to this position						  	*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : cl8 *pFile 			: File 		                                  	*/
/* ---------------                                                            	*/
/*  Out: 																  	*/
/*       clu32 *pu32Pos			: current position pointer in the file to set	*/
/* Return value: e_Result                                                     	*/
/*  OK                        :  Result is OK                             		*/
/*  ERROR,                    : Failure on execution                      		*/
/*  MEM_ERR,                  :  Failure on memory management (failure,   		*/
/*                                  allocation ....)                      		*/
/*  PARAMS_ERR,               :  Inconsistent parameters                  		*/
/******************************************************************************/
e_Result cl_FlGetPosWin32( cl8 *pFile, clu32 *pu32Pos )
{
	e_Result status = CL_ERROR;
	fpos_t position = 0;

	// check incoming arguments
	if ( pFile == CL_NULL )
		return ( CL_ERROR );

	if ( pu32Pos == CL_NULL )
		return ( CL_ERROR );

	/* get file position	 */
	if ( fgetpos( (FILE *)pFile, (fpos_t *)&position ) == 0 )
	{
		status = CL_OK ;
		*pu32Pos = (clu32) position;
	}

	if ( feof( (FILE *)pFile ) != 0 )
		status = CL_EOF_ERR;

	return ( status );
}

/******************************************************************************/
/* Name : e_Result cl_FileGetChrWin32( clvoid *pFileId, cl8 *pcChar )				  */
/* Description :                                                              	*/
/*       get a char from a file	and increment the file pointer by 1				*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : cl8 *pFileId 			: File descriptor								*/
/* ---------------                                                            	*/
/*  Out: cl8 *pcChar			: char read from the file						*/
/* Return value: e_Result                                                     	*/
/*  OK                        :  Result is OK                             		*/
/*  ERROR,                    : Failure on execution                      		*/
/*  MEM_ERR,                  :  Failure on memory management (failure,   		*/
/*                                  allocation ....)                      		*/
/*  PARAMS_ERR,               :  Inconsistent parameters                  		*/
/******************************************************************************/
e_Result cl_FileGetChrWin32( clvoid *pFileId, cl8 *pcChar )
{
	e_Result status = CL_ERROR;

	// check incoming arguments
	if ( pFileId == CL_NULL )
		return ( CL_ERROR );

	if ( pcChar == CL_NULL )
		return ( CL_ERROR );

	/* get file position	 */
	if ( (*pcChar = fgetc( (FILE *)pFileId )) != EOF )
		status = CL_OK ;

	if ( feof( (FILE *)pFileId ) != 0 )
		status = CL_EOF_ERR;

	return ( status );
}

#endif // 32 bits
#endif 	// WIN32
