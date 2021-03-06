/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: clfile.h                                                           */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
#include "..\csl.h"
#ifndef CLFILE_H_
#define CLFILE_H_

/******************************************************************************/
/* Name : cl_FlOpen( cl8 *pFile, e_FileOptions eOpt ); 				  */
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
e_Result cl_FlOpen( cl8 *pFileName, e_FileOptions eOpts, clvoid **ppFileId);

/******************************************************************************/
/** @fn  cl_FlReadLn( cl8 *pFile, clu8 **ppu8Data, clu32 u32Len)
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
e_Result cl_FlReadLn( cl8 *pFile, clu8 **ppu8Data, clu32 *pu32Len);


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
e_Result cl_FlWrite( cl8 *pFile, clu8 *pu8Data, clu32 u32Len );

/******************************************************************************/
/* Name : cl_FlClose( cl8 *pFile) 			  */
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
e_Result cl_FlClose( cl8 *pFile );

/******************************************************************************/
/* Name : e_Result cl_FlGetPos( clu8 *pFile, clu32 *pu32Pos )				  	*/
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
e_Result cl_FlGetPos( clu8 *pFile, clu32 *pu32Pos );

/******************************************************************************/
/* Name : e_Result cl_FlSetPos( clu8 *pFile, clu32 u32Pos )				  	*/
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
e_Result cl_FlSetPos( clu8 *pFile, clu32 u32Pos );

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
e_Result cl_FileGetChr( clvoid *pFileId, cl8 *pcChar );



#endif /* CLFILE_H_ */
/**************************************************/
