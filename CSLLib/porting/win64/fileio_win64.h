/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: fileio_win64.h                                                     */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/* Description: porting layer for file access in WIN64 environment           */
/*****************************************************************************/
#if define WIN64
#if defined(__x86_64__)


#ifndef FILEIO_WIN64_H_
#define FILEIO_WIN64_H_

#include "..\..\cltypes.h"

/******************************************************************************/
/* Name : cl_FlOpenWin64( clvoid *pFile, e_FileOptions eOpt ); */
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
e_Result cl_FlOpenWin64( clvoid *pFile, e_FileOptions eOpts);

/******************************************************************************/
/* Name : cl_FlReadWin64( clvoid *pFile, clu8 **ppu8Data, clu64 u64Len) 			  */
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
e_Result cl_FlReadWin64( clvoid *pFile, clu8 **pu8Data, clu64 u64Len);


/******************************************************************************/
/* Name : cl_FlWriteWin64( clvoid *pFile, clu8 *pu8Data, clu64 u64Len) 			  */
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
e_Result cl_FlWriteWin64( clvoid *pFile, clu8 *pu8Data, clu64 u64Len );

/******************************************************************************/
/* Name : cl_FlCloseWin64( clvoid *pFile) 			  */
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
e_Result cl_FlCloseWin64( clvoid *pFile );




#endif /* FILEIO_WIN64_H_ */
//**************************************************
#endif // 64sbits
#endif // WIN64
