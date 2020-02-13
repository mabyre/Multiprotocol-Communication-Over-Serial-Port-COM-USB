/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: fileio_win32.h                                                     */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/* Description: porting layer for file access in WIN32 environment           */
/*****************************************************************************/

#ifdef WIN32
#ifndef __x86_64__


#ifndef FILEIO_WIN32_H_
#define FILEIO_WIN32_H_

#include "..\..\cltypes.h"

/******************************************************************************/
/* Name : cl_FlOpenWin32( cl8 *pFile, e_FileOptions eOpt ); 				  */
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
e_Result cl_FlOpenWin32( cl8 *pFileName, e_FileOptions eOpts, clvoid **ppFileId);

/******************************************************************************/
/** @fn  cl_FlReadLnWin32( cl8 *pFile, clu8 **ppu8Data, clu32 u32Len)
* @brief
* \n      read byte buffers from File. The byte buffer is stopped on LF character
* \n		 Buffer allocation is done by this API

* 	@param *pFile 				( In ) File to read

*	@param **ppu8Data 			( Out ) pointer where to store read data ( memory is allocated
* \n							by the caller)
*   @param *pu32Len				( Out ) point to number of bytes effectively read
* @return e_Result
*  \n OK                        :  Result is OK
*  \n ERROR,                    : Failure on execution
*  \n MEM_ERR,                  :  Failure on memory management (failure,
*  \n                                 allocation ....)
****************************************************************************/
e_Result cl_FlReadLnWin32( cl8 *pFile, clu8 **ppu8Data, clu32 *pu32Len);


/******************************************************************************/
/* Name : cl_FlWriteWin32( cl8 *pFile, clu8 *pu8Data, clu32 u32Len) 			  */
/* Description :                                                              */
/*       Write file in append mode 							                   */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pFile : File to write		                                  */
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
e_Result cl_FlWriteWin32( cl8 *pFile, clu8 *pu8Data, clu32 u32Len );

/******************************************************************************/
/* Name : cl_FlCloseWin32( cl8 *pFile) 			  */
/* Description :                                                              */
/*       set a Clover devices list registered to one reader                   */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : cl8 *pFile : File to close		                                  */
/* ---------------                                                            */
/*  Out: none																	*/
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                             */
/*  ERROR,                    : Failure on execution                      */
/******************************************************************************/
e_Result cl_FlCloseWin32( cl8 *pFile );

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
e_Result cl_PrintCompleteWin32( clvoid );


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
e_Result cl_FlGetPosWin32( cl8 *pFile, clu32 *pu32Pos );

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
e_Result cl_FlSetPosWin32( cl8 *pFile, clu32 u32Pos );

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
e_Result cl_FileGetChrWin32( clvoid *pFileId, cl8 *pcChar );

#endif /* FILEIO_WIN32_H_ */
//**************************************************
#endif // 32 bits
#endif // WIN32
