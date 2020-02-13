/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: fileio_posix.h                                                     */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/* Description: porting layer for file access in POSIX                       */
/*****************************************************************************/
#if define LINUXPC

#ifndef FILEIO_POSIX_H_
#define FILEIO_POSIX_H_


#include "..\..\cltypes.h"

/******************************************************************************/
/* Name : cl_FlOpenPosix( clvoid *pFile, e_FileOptions eOpt ); */
/* Description :                                                              */
/*       set a Clover devices list registered to one reader                   */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : tReader *ptReader: Reader to address                                 */
/*       tDevice *ptDevicesList : list of Clover devices linked to Reader     */
/*                                  passed in arguments                       */
/* ---------------                                                            */
/*  Out: none
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                             */
/*  ERROR,                    : Failure on execution                      */
/*  MEM_ERR,                  :  Failure on memory management (failure,   */
/*                                  allocation ....)                      */
/*  PARAMS_ERR,               :  Inconsistent parameters                  */
/******************************************************************************/
e_Result cl_FlOpenPosix( clvoid *pFile, e_FileOptions eOpts);

/******************************************************************************/
/* Name : cl_FlReadPosix( clvoid *pFile, clu8 **ppu8Data, clu32 u32Len) 			  */
/* Description :                                                              */
/*       read byte buffers from File							                   */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pFile : File to read		                                  */
/* ---------------                                                            */
/*  Out : clu8 **pu8Data : pointer where to store read data ( memory is allocated   */
/* by the caller)     														  */
/* Return value: e_Result                                                     */
/*  OK                        :  Result is OK                             	  */
/*  ERROR,                    : Failure on execution                      	  */
/*  MEM_ERR,                  :  Failure on memory management (failure,    	  */
/*                                  allocation ....)                          */
/*  PARAMS_ERR,               :  Inconsistent parameters                      */
/******************************************************************************/
e_Result cl_FlReadPosix( clvoid *pFile, clu8 **pu8Data, clu32 u32Len);


/******************************************************************************/
/* Name : cl_FlWritePosix( clvoid *pFile, clu8 *pu8Data, clu32 u32Len) 			  */
/* Description :                                                              */
/*       Write file in append mode 							                   */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pFile : File to write		                                  */
/*       tDevice *ptDevicesList : list of Clover devices linked to Reader     */
/*                                  passed in arguments                       */
/* ---------------                                                            */
/*  Out: none
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                             */
/*  ERROR,                    : Failure on execution                      */
/*  MEM_ERR,                  :  Failure on memory management (failure,   */
/*                                  allocation ....)                      */
/*  PARAMS_ERR,               :  Inconsistent parameters                  */
/******************************************************************************/
e_Result cl_FlWritePosix( clvoid *pFile, clu8 *pu8Data, clu32 u32Len );

/******************************************************************************/
/* Name : cl_FlClosePosix( clvoid *pFile) 			  */
/* Description :                                                              */
/*       set a Clover devices list registered to one reader                   */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : clvoid *pFile : File to close		                                  */
/* ---------------                                                            */
/*  Out: none
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                             */
/*  ERROR,                    : Failure on execution                      */
/******************************************************************************/
e_Result cl_FlClosePosix( clvoid *pFile );

/******************************************************************************/
/* Name : cl_PrintCompletePosix( clvoid ) 			  							*/
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
e_Result cl_PrintCompletePosix( clvoid )


#endif /* FILEIO_POSIX_H_ */


#endif // LINUXPC
