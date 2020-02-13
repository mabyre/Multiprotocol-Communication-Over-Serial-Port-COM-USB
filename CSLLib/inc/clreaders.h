/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: cltuple.h                                                          */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/* Description: Tuple helpers                                                */
/*****************************************************************************/

#ifndef CLREADERS_H_
#define CLREADERS_H_

#include 	"..\csl.h"
#include 	"..\inc\generic.h"

#ifdef WIN32
	#include <stdio.h>
	#include <string.h>
#define cl_openfile fopen
#else
#define cl_openfile ((void*)0)
#endif

/*--------------------------------------------------------------------------*/

void ConvertASCIToHex( unsigned char *pStringBuf, unsigned inLen, unsigned char *pOutData, unsigned long *pu32OutLen );

void ConvertASCIToHexa( unsigned char *pStringBuf, unsigned inLen, unsigned char *pOutData );

unsigned char* ConvertStringToHexa( const char *in, size_t len, unsigned char *out );

void ConvertByteToAscii( unsigned char *pData, unsigned long u32Len, char *pDataInString );

/*--------------------------------------------------------------------------*/

/*******************************************************************************************/
/* Name : e_Result cl_initReaderSetDefaultFields( 	e_StackSupport eType, 					*/
/*													clu8 *pucReaderName, 					*/
/*													clu32 u32ReaderNameLen, 				*/
/*													t_clIPReader_Params *pIPReaderParams, 	*/
/*													t_clCOMReader_Params *pCOMReaderParams, */
/*													t_clBTReader_Params *pBTReaderParams	*/
/* Description :                                                                            */
/*        init reader structure with default elements							            */
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*		e_StackSupport eType, 			: IP /Bluetooth / COM serial						*/
/*		clu8*        pucReaderName;     : name of the reader								*/
/*		clu32        u32ReaderNameLen;  : length of the reader								*/
/*		t_clIPReader_Params *pIPReaderParams :	IP reader parameters 						*/
/*		t_clCOMReader_Params *pCOMReaderParams:  COM reader parameters						*/
/*		t_clBTReader_Params *pBTReaderParams: 	Bluetooth reader parameters					*/
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*******************************************************************************************/
e_Result cl_initReaderSetDefaultFields( e_StackSupport eType, clu8 *pucReaderName, clu32 u32ReaderNameLen, t_clIPReader_Params *pIPReaderParams, t_clCOMReader_Params *pCOMReaderParams, t_clBTReader_Params *pBTReaderParams );

/*--------------------------------------------------------------------------*/

e_Result cl_InitReadersFromFile(char **friendlyNames);

/**************************************************************************/
/* Name : e_Result cl_ReaderRemoveFromList( t_Reader *ppReader)             */
/* Description :                                                         	 */
/*        Remove a reader from the list										*/
/**************************************************************************/
/* Parameters:                                                            	*/
/*  --------------                                                        	*/
/*  In: 	t_Reader *pReader: Remove the reader from the list. NULL not 	*/
/*			allowed															*/
/* ---------------                                                        	*/
/*  Out: 																	 */
/* Return value: e_Result                                                 	*/
/*  CL_OK                        :  Result is OK                             */
/*  CL_ERROR,                    : Failure on execution or list of readers 	*/
/*												is empty                      */
/*  CL_MEM_ERR,                  :  Failure on memory management (failure,   */
/*                                  allocation ....)                      	*/
/*  CL_PARAMS_ERR,               :  Inconsistent parameters                  */
/**************************************************************************/
e_Result cl_ReaderRemoveFromList( t_Reader *pReader );

/*--------------------------------------------------------------------------*/

e_Result cl_ReaderFreeListOfTuples( t_Tuple **ppTuple );

/**************************************************************************/
/* Name : e_Result cl_ReaderGetSerialEncapsLen( t_Reader *pReader);       */
/* Description :                                                          */
/*        this API return the length of the encapsulation to add to each buffer */
/*		  coming from userland before sending it on the network to a reader*/
/*		  This reader can get a trailer to add to the buffer due to its   */
/*		  specificity/setup												  */
/**************************************************************************/
/* Parameters:                                                            */
/*  --------------                                                        */
/*  In: t_Reader *pReader: chained list of readers in the system 	  */
/*                           	can be NULL                               */
/* ---------------                                                        */
/*  Out: none                                                             */
/* Return value: Length of the trailer. NULL if reader type is unknown	  */
/**************************************************************************/
clu32 cl_ReaderGetSerialEncapsLen( t_Reader *pReader );
/**************************************************************************/
/* Name : e_Result cl_ReaderGetSerialEncapsLen( t_Reader *pReader);       */
/* Description :                                                          */
/*        this API return the length of the trailer to add to each buffer */
/*		  coming from userland before sending it on the network to a reader*/
/*		  This reader can get a trailer to add to the buffer due to its   */
/*		  specificity/setup												  */
/**************************************************************************/
/* Parameters:                                                            */
/*  --------------                                                        */
/*  In: t_Reader *pReader: chained list of readers in the system 	  */
/*                           	can be NULL                               */
/* ---------------                                                        */
/*  Out: none                                                             */
/* Return value: Length of the trailer. NULL if reader type is unknown	  */
/**************************************************************************/
clu32 cl_ReaderGetSerialEncapsTrailerLen( t_Reader *pReader );
/**************************************************************************/
/* Name : e_Result cl_ReaderGetSerialEncapsFinishLen( t_Reader *pReader);       */
/* Description :                                                          */
/*        this API return the length of the finish datato add to each buffer */
/*		  coming from userland before sending it on the network to a reader*/
/*		  This reader can get a trailer to add to the buffer due to its   */
/*		  specificity/setup												  */
/**************************************************************************/
/* Parameters:                                                            */
/*  --------------                                                        */
/*  In: t_Reader *pReader: chained list of readers in the system 	  */
/*                           	can be NULL                               */
/* ---------------                                                        */
/*  Out: none                                                             */
/* Return value: Length of the trailer. NULL if reader type is unknown	  */
/**************************************************************************/
clu32 cl_ReaderGetSerialEncapsFinishLen( t_Reader *pReader );
/**************************************************************************/
/** @fn e_Result cl_ReaderSetLastAccessed( t_Reader *pReader )
* @brief
* \n       this API set the pointer of the reader which is	 accessed
* \n		  It can be used for scripting as an example to be able to chain
* \n		  on a reader, several commands, without respecifying it

*  @param *pReader			( In ) pointer on the reader
* \n                           	can be NULL
* @return CL_OK
************************************************************************* */
e_Result cl_ReaderSetLastAccessed( t_Reader *pReader );

/****************************************************************************/
/** @fn : e_Result cl_ReaderSetLastAccessed( t_Reader **ppReader )
* @brief
* \n       this API set the pointer of the reader which is	 accessed
* \n		  It can be used for scripting as an example to be able to chain
* \n		  on a reader, several commands, without respecifying it

*
* @param				( In/Out ) **ppReader: pointer on the reader last accessed
* \n                           	cannot be NULL
* @return				CL_OK
*	\n					CL_ERROR: Wrong parameter
************************************************************************* */
e_Result cl_ReaderGetLastAccessed( t_Reader **ppReader );

/*****************************************************************************/
/* Name : e_Result cl_ReaderRegistrationThread( t_Reader *ppReader)           */
/* Description :                                                         	 */
/*        thread which handles registration/unregistration of readers in CSL */
/*****************************************************************************/
/* Parameters:                                                            	*/
/*  --------------                                                        	*/
/*  In: 	t_Reader *pReader: Remove the reader from the list. NULL not 	*/
/*			allowed															*/
/* ---------------                                                        	*/
/*  Out: 																	*/
/* Return value: e_Result                                                 	*/
/*  CL_OK                        :  Result is OK                            */
/*  CL_ERROR,                    : Failure on execution or list of readers 	*/
/*												is empty                    */
/*  CL_MEM_ERR,                  :  Failure on memory management (failure,  */
/*                                  allocation ....)                      	*/
/*  CL_PARAMS_ERR,               :  Inconsistent parameters                 */
/**************************************************************************/
e_Result cl_ReaderRegistrationThread( void);

/*******************************************************************************************/
/* Name : e_Result cl_ReaderSetSyncStatus(  t_Reader *pReader, e_SERIAL_RetryProtocolDef eSyncStatus ) 		*/
/* Description :                                                                            */
/*        set synchronization state between read/write threads								*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*			*pReader	:	pointer on reader												*/
/*			eSyncStatus	:	synchonization status											*/
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  CL_ERROR                  :  Error on call    											*/
/*******************************************************************************************/
e_Result cl_ReaderSetSyncStatus(char *head,  t_Reader *pReader, e_SERIAL_RetryProtocolDef eSyncStatus );

/*******************************************************************************************/
/* Name : e_Result cl_ReaderGetSyncStatus(  t_Reader *pReader, e_SERIAL_RetryProtocolDef *p_eSyncStatus ) 		*/
/* Description :                                                                            */
/*        sget synchronization state between read/write threads								*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*			*pReader	:	pointer on reader												*/
/* ---------------                                                                          */
/*  Out:                                                                                	*/
/*			*p_eSyncStatus	:	synchonization status										*/
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  CL_ERROR                  :  Error on call    											*/
/*******************************************************************************************/
e_Result cl_ReaderGetSyncStatus(char *head, t_Reader *pReader, e_SERIAL_RetryProtocolDef *p_eSyncStatus );

#endif /* CLREADERS_H_ */
/**************************************************/
