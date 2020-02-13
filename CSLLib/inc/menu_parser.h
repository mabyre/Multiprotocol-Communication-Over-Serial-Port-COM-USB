/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
#ifndef MENU_PARSER_H_
#define MENU_PARSER_H_

#include "..\csl.h"
/*******************************************************************************************/
/* Name : e_Result cl_MenuBuild( clu8 *pFile ) 												*/
/* Description :                                                                            */
/*        Build and execute a menu from a menu file descriptor 								*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : clu8 *pFile			: File descriptor to parse 									*/
/* 	Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result cl_MenuBuild( clu8 *pFile );


/*******************************************************************************************/
/* Name : e_Result cl_MenuBuildItem( clu8 *pFileId,											*/
/* 							clu8 *pu8CrtLevel, clu32 u32CrtLevelLen,						*/
/* 							clu8 **pu8NextLevel, clu32 *pu32NextLevelLen ) 					*/
/* Description :                                                                            */
/*        from a level, build the corresponding element										*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :clu8 *pFile			: File descriptor to parse 									*/
/*		clu8 *pu8CrtLevel		: string which describes current level						*/
/*		clu32 u32CrtLevel		: current level lenght descriptor							*/
/*	Out : 																					*/
/*		clu8 **pu8NextLevel		: Next Level string descriptor								*/
/*		clu32 *pu32NextLevelLen	: Length of the next level									*/
/* 	Return value: e_Result                                                                  */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result cl_MenuBuildItem( clu8 *pFileId, clu8 *pszu8CrtLevel, clu8 **ppu8NextLevel, clu32 *pu32NextLevelLen );

/***********************************************************************************************/
/* Name : e_Result cl_MenuGetBlock( clu8 *pFileId, cl8 *pszStartPattern, cl8 *pszStopPattern 	*/
/*			clu32 u32StartBoundary, clu32StopBoundary, clu32 *pu32StartIndex, clu32 *pu32StopIndex ) 	*/
/* Description :                                                                            	*/
/*        Find the block starting with pszStartPattern and pszStopPattern between StartBoundary */
/*		and StopBoundary. Returns the index of the block										*/
/*		 on CL_ERROR, the file pointer remains unchanged										*/
/************************************************************************************************/
/* Parameters:                                                                              	*/
/*  --------------                                                                          	*/
/*  In : clvoid	*pFileId			: File descriptor to parse 									*/
/*		cl8 *pszStartPattern		: string of first pattern to search							*/
/*		cl8 *pszStopPattern			: string of last pattern to search							*/
/*		clu32 u32StartBoundary		: Index if the file where we shall start to look at			*/
/*		clu32 u32StopBoundary		: Index in the file where we shall stop to look at			*/
/*	Out :																						*/
/*		clu32	*pu32StartIndex		: Start Index of the block pattern (first byte corresponding */
/*									to pszStartPattern in File 									*/
/*		clu32	*pu32StopIndex		: Stop Index of the block pattern (first byte corresponding */
/*									to pszStartPattern in File 									*/
/* 	Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result cl_MenuGetBlock( clu8 *pFileId, cl8 *pszStartPattern, cl8 *pszStopPattern, clu32 u32StartBoundary, clu32 u32StopBoundary, clu32 *pu32StartIndex, clu32 *pu32StopIndex );


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
e_Result cl_FindPreviousTag( clvoid *pFileId, cl8 **pp8Tag, clu32 *pu32TagLen );

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

/***********************************************************************************************/
/* Name : e_Result cl_MenuProcessAction( clu8	*pFileId, clu32 u32StartIndex, clu32 u32StopIndex  ) 	*/
/* Description :                                                                            	*/
/*        From this file position, pars <action> </action> and performs correct instructions */
/************************************************************************************************/
/* Parameters:                                                                              	*/
/*  --------------                                                                          	*/
/*  In : clvoid	*pFileId			: File descriptor to parse 									*/
/*	Out :																						*/
/* 	Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result cl_MenuProcessAction( clu8	*pFileId, clu32 u32StartIndex, clu32 u32StopIndex );

/*******************************************************************************************/
/* Name : e_Result cl_MenuPrintValue( clvoid *pFileId, cl8 *pszValue )						*/
/* Description :                                                                            */
/*        Print the value just after the tag where the file pointer is						*/
/*		  on error, do nothing 																*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : clvoid	*pFile			: File descriptor to parse 									*/
/*		clu8 *pszValue			: value to print out										*/
/* Out: none																				*/
/* 	Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result cl_MenuPrintValue( clvoid *pFileId, cl8 *pszValue );

#endif /* MENU_PARSER_H_ */
/**************************************************/
