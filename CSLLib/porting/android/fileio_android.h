/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/

#if defined __ANDROID__


#ifndef FILEIO_ANDROID_H_
#define FILEIO_ANDROID_H_


#include "..\..\cltypes.h"

/******************************************************************************/
/* Name : cl_FlOpenAndroid( clvoid *pFile, e_FileOptions eOpt ); */
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
e_Result cl_FlOpenAndroid( clvoid *pFile, e_FileOptions eOpts);

/******************************************************************************/
/* Name : cl_FlReadAndroid( clvoid *pFile, clu8 **ppu8Data, clu32 u32Len) 			  */
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
e_Result cl_FlReadAndroid( clvoid *pFile, clu8 **pu8Data, clu32 u32Len);


/******************************************************************************/
/* Name : cl_FlWriteAndroid( clvoid *pFile, clu8 *pu8Data, clu32 u32Len) 			  */
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
e_Result cl_FlWriteAndroid( clvoid *pFile, clu8 *pu8Data, clu32 u32Len );

/******************************************************************************/
/* Name : cl_FlCloseAndroid( clvoid *pFile) 			  */
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
e_Result cl_FlCloseAndroid( clvoid *pFile );

/******************************************************************************/
/* Name : cl_PrintCompleteAndroid( clvoid ) 			  							*/
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
e_Result cl_PrintCompleteAndroid( clvoid )



#endif /* FILEIO_ANDROID_H_ */


#endif // __ANDROID__
