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
#ifndef CLTUPLE_H_
#define CLTUPLE_H_

#include "..\clsupport.h"
/*******************************************************************************************/
/* Name : e_Result cl_initTupleSetDefaultFields( e_ConnectionType eCtType, e_Crypted eCrypt,*/
/* 												clu32 u32Repeat, clu32 u32Period, 			*/
/*												clu32 u32MaxTimeout, clu8 *pFileName )      */
/* Description :                                                                            */
/*        return function pointers on the CSL functions provided by the customer            */
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*		e_ConnectionType       tCnct;            // Connection type (local/Framework 		*/
/* access....)																				*/
/*		e_Crypted              e_CryptedLink;     // encryption request on the buffer to	*/
/* 													send/receive							*/
/*		clu32                 ulRepeat;         // send the same buffer several times-0 means*/
/*												// : do not sent-1 is default.				*/
/*		clu32                 ulPeriod;         // Period to wait before sending a new buffer */
/*							. If 0, there is no waiting period between each send of data	*/
/*		clu32                 ulMaxTimeout;     // Max Period allowed before declaring that */
/*							the data sent was not sent and returning an Error to upper layer */
/*		clu8                  *pFileName;       // Save the buffer to file if pFileName is not NULL */
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*******************************************************************************************/
e_Result cl_initTupleSetDefaultFields( e_ConnectionType eCtType, e_Crypted eCrypt, clu32 u32Repeat, clu32 u32Period, clu32 u32MaxTimeout, clu8 *pFileName );

/*******************************************************************************************/
/* Name : e_Result cl_initTupleSetAllFields( e_ConnectionType eCtType, e_Crypted eCrypt,*/
/* 												clu32 u32Repeat, clu32 u32Period, 			*/
/*												clu32 u32MaxTimeout, clu8 *pFileName )      */
/* Description :                                                                            */
/*        return function pointers on the CSL functions provided by the customer            */
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*		t_Tuple *pTuple            					// tuple to initialize 					*/
/*		clu8 *pu8Data							     // buffer of data to be saved in pTuple	*/
/*		clu32	u32Len								// length of data to save				*/
/*		e_ConnectionType       tCnct;            // Connection type (local/Framework 		*/
/* access....)																				*/
/*		e_Crypted              e_CryptedLink;     // encryption request on the buffer to	*/
/* 													send/receive							*/
/*		clu32                 ulRepeat;         // send the same buffer several times-0 means*/
/*												// : do not sent-1 is default.				*/
/*		clu32                 ulPeriod;         // Period to wait before sending a new buffer */
/*							. If 0, there is no waiting period between each send of data	*/
/*		clu32                 ulMaxTimeout;     // Max Period allowed before declaring that */
/*							the data sent was not sent and returning an Error to upper layer */
/*		clu8                  *pFileName;       // Save the buffer to file if pFileName is not NULL */
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*	CL_PARAMS_ERR				: tuple is NULL or buffer is NULL							**/
/*******************************************************************************************/
e_Result cl_initTupleSetAllFields( t_Tuple *pTuple,  clu8 *pu8Data, clu32 u32Len, e_ConnectionType eCtType, e_Crypted eCrypt, clu32 u32Repeat, \
		clu32 u32Period, clu32 u32MaxTimeout, clu8 *pFileName );


#endif /* CLTUPLE_H_ */
/**************************************************/
