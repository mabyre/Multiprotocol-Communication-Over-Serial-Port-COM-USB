/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
#include "..\csl.h"
#ifndef CLTAG_H_
#define CLTAG_H_

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
e_Result cl_TagGetValue( clvoid *pFileId, clu32 u32StartBoundary, clu32 u32StopBoundary, cl8 **pp8Value, clu32 *pu32ValueLen, clu32 *pu32ValueStartIndex, clu32 *pu32ValueStopIndex, clu8 bRemoveSpaceTrailer );

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
e_Result cl_FindNextTag( clvoid *pFileId, cl8 **pp8Tag, clu32 *pu32TagLen );



#endif /* CLTAG_H_ */
/**************************************************/
