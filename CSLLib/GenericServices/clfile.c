/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: clfile.c                                                           */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
#include "..\inc\clfile.h"
#include "..\inc\generic.h"

/********************************************************************************/
/* Name : cl_FlOpen( cl8 *pFile, e_FileOptions eOpt ); 				  			*/
/* Description :                                                              	*/
/*       open a file in hosting environment: file shall in Read/Write mode	  	*/
/********************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : clvoid *pFileName: string containing the name of the file			  	*/
/*		e_FileOptions eOpts : options on opening the file in binary or text mode  */
/* ---------------                                                            	*/
/*  Out: clvoid **ppFileId : pointer on file descriptor							*/
/* Return value: e_Result                                                      	*/
/*  OK                        :  Result is OK                             		*/
/*  ERROR,                    : Failure on execution                      		*/
/*  PARAMS_ERR,               :  Inconsistent parameters                  		*/
/********************************************************************************/
e_Result cl_FlOpen( cl8 *pFileName, e_FileOptions eOpts, clvoid **ppFileId)
{
	t_clContext *pCtxt 			= CL_NULL;
	e_Result	status			= CL_ERROR;

	// check global parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	// call function
	status = pCtxt->ptHalFuncs->fnFileOpen( pFileName, CL_FILE_BINARY_MODE, ppFileId );

	return ( status );
}

/******************************************************************************/
/** @fn  cl_FlReadLn( cl8 *pFileId, clu8 **ppu8Data, clu32 u32Len)
* @brief
* \n      read byte buffers from File. The byte buffer is stopped on LF character
* \n		 Buffer allocation is done by this API

* 	@param *pFileId				( In ) File descriptor

*	@param **ppu8Data 			( Out ) pointer where to store read data ( memory is allocated
* \n							by the caller)
*   @param *pu32Len				( Out ) point to number of bytes effectively read
* @return e_Result
*  \n OK                        :  Result is OK
*  \n ERROR,                    : Failure on execution
*  \n MEM_ERR,                  :  Failure on memory management (failure,
*  \n                                 allocation ....)
****************************************************************************/
e_Result cl_FlReadLn( cl8 *pFileId, clu8 **ppu8Data, clu32 *pu32Len)
{
	t_clContext *pCtxt 			= CL_NULL;
	e_Result	status			= CL_ERROR;

	// check global parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	// call function
	status = pCtxt->ptHalFuncs->fnFileReadLn( pFileId, ppu8Data, pu32Len );

	return ( status );

}


/******************************************************************************/
/* Name : cl_FlWrite( cl8 *pFileId, clu8 *pu8Data, clu32 u32Len) 			  */
/* Description :                                                              */
/*       Write file in append mode 							                   */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pFileId : File Descriptor									 */
/*       tDevice *ptDevicesList : list of Clover devices linked to Reader     */
/*                                  passed in arguments                       */
/* ---------------                                                            */
/*  Out: none																	*/
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                             */
/*  ERROR,                    : Failure on execution                      */
/*  MEM_ERR,                  :  Failure on memory management (failure,   */
/*                                  allocation ....)                      */
/*  PARAMS_ERR,               :  Inconsistent parameters                  */
/******************************************************************************/
e_Result cl_FlWrite( cl8 *pFileId, clu8 *pu8Data, clu32 u32Len )
{
	t_clContext *pCtxt 			= CL_NULL;
		e_Result	status			= CL_ERROR;

		// check global parameters
		if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
			return ( CL_ERROR );

		if ( pCtxt == CL_NULL )
			return ( CL_ERROR );

		if ( pCtxt->ptHalFuncs == CL_NULL )
			return ( CL_ERROR );

		// call function
		status = pCtxt->ptHalFuncs->fnFileWrite( pFileId, pu8Data, u32Len );

		return ( status );

}

/******************************************************************************/
/* Name : cl_FlClose( cl8 *pFileId) 			  */
/* Description :                                                              */
/*       set a Clover devices list registered to one reader                   */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : cl8 *pFileId : File descriptor to close		                      */
/* ---------------                                                            */
/*  Out: none																	*/
/* Return value: e_Result                                                      	*/
/*  OK                        :  Result is OK                             		*/
/*  ERROR,                    : Failure on execution                      		*/
/******************************************************************************/
e_Result cl_FlClose( cl8 *pFileId )
{
	t_clContext *pCtxt 			= CL_NULL;
		e_Result	status			= CL_ERROR;

		// check global parameters
		if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
			return ( CL_ERROR );

		if ( pCtxt == CL_NULL )
			return ( CL_ERROR );

		if ( pCtxt->ptHalFuncs == CL_NULL )
			return ( CL_ERROR );

		// call function
		status = pCtxt->ptHalFuncs->fnFileClose( pFileId );

		return ( status );

}

/******************************************************************************/
/* Name : e_Result cl_FlGetPos( cl8 *pFileId, clu32 *pu32Pos )				  	*/
/* Description :                                                              	*/
/*       set the file position pointer to this position						  	*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : cl8 *pFileId 			: File Descriptor								*/
/* ---------------                                                            	*/
/*  Out: 																  		*/
/*       clu32 *pu32Pos			: current position pointer in the file to set	*/
/* Return value: e_Result                                                     	*/
/*  OK                        :  Result is OK                             		*/
/*  ERROR,                    : Failure on execution                      		*/
/*  MEM_ERR,                  :  Failure on memory management (failure,   		*/
/*                                  allocation ....)                      		*/
/*  PARAMS_ERR,               :  Inconsistent parameters                  		*/
/******************************************************************************/
e_Result cl_FlGetPos( clu8 *pFileId, clu32 *pu32Pos )
{
	t_clContext *pCtxt 			= CL_NULL;
	e_Result	status			= CL_ERROR;

	// check global parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	// call function
	status = pCtxt->ptHalFuncs->fnFileGetPos( (cl8 *)pFileId, pu32Pos );

	return ( status );
}

/******************************************************************************/
/* Name : e_Result cl_FlSetPosWin32( cl8 *pFileId, clu32 u32Pos )				  	*/
/* Description :                                                              	*/
/*       set the file position pointer to this position						  	*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : cl8 *pFileId 			: File 		                                  	*/
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
e_Result cl_FlSetPos( clu8 *pFile, clu32 u32Pos )
{
	t_clContext *pCtxt 			= CL_NULL;
	e_Result	status			= CL_ERROR;

	// check global parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	// call function
	status = pCtxt->ptHalFuncs->fnFileSetPos( (cl8 *)pFile, u32Pos );

	return ( status );
}


/******************************************************************************/
/* Name : e_Result cl_FileGetChr( clvoid *pFileId, cl8 *pcChar )				  */
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
e_Result cl_FileGetChr( clvoid *pFileId, cl8 *pcChar )
{
	t_clContext *pCtxt 			= CL_NULL;
	e_Result	status			= CL_ERROR;

	// check global parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	// call function
	status = pCtxt->ptHalFuncs->fnFileGetChar( pFileId, pcChar );

	return ( status );

}
