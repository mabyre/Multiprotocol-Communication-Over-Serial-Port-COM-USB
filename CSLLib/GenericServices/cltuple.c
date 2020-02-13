/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: cltuple.c                                                          */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/* Description: Tuple helpers                                                */
/*****************************************************************************/

#include "..\csl.h"
#include "..\cltypes.h"
#include "..\inc\generic.h"
#include <string.h>
t_Tuple g_tDefaultTuple = {
		CL_NULL,
		{
			TSP_LAYER_SET_LOCAL,
			NONE,
			0,
			0,
			0,
			CL_NULL
		},
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		},
		CL_NULL
};

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
e_Result cl_initTupleSetDefaultFields( e_ConnectionType eCtType, e_Crypted eCrypt, clu32 u32Repeat, clu32 u32Period, clu32 u32MaxTimeout, clu8 *pFileName  )
{
	// initialize type of connection
	g_tDefaultTuple.tOptions.tCnct = eCtType;

	// type of encryption to apply on the data
	g_tDefaultTuple.tOptions.e_CryptedLink = eCrypt;

	// number of repeat for this command
	g_tDefaultTuple.tOptions.ulRepeat = u32Repeat;

	// Period to wait before sending a new buffer. If 0, there is no waiting period between each send of data
	g_tDefaultTuple.tOptions.ulPeriod = u32Period;

	// Period to wait before sending a new buffer. If 0, there is no waiting period between each send of data
	g_tDefaultTuple.tOptions.ulMaxTimeout = u32MaxTimeout;

	// Period to wait before sending a new buffer. If 0, there is no waiting period between each send of data
	g_tDefaultTuple.tOptions.pFileName = pFileName;

	return ( CL_OK );

}


/*******************************************************************************************/
/* Name : e_Result cl_initTuple(  t_Tuple *pTuple,  clu8 *pu8Data, clu32 u32Len )			*/
/* Description :                                                                            */
/*        initialize a tuple using the default flags and a buffer of data (memory is		*/
/*		allocated by the caller if necessary												*/
/* BRY : fucking stupid function
 * imagine you call this function with CL_NULL has pBuff parameter
 * you will make a memory leak each time
 * thank's to FD
 */
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*		t_Tuple *pTuple            					// tuple to initialize 					*/
/*		t_Buffer *pBuff								// if CL_NULL, allocate t_Buffer struct */
/*													// otherwise buffer structure to insert in tuple */
/*		clu8 *pu8Data							     // buffer of data to be saved in pTuple	*/
/*		clu32	u32Len								// length of data to save				*/
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*******************************************************************************************/
e_Result cl_initTuple( t_Tuple *pTuple, t_Buffer *pBuff, clu8 **ppu8Data, clu32 u32Len )
{
	// variables definition
	t_clContext *pCtxt 		= CL_NULL;
	e_Result status 		= CL_ERROR;

	// check incoming parameters
	if ( pTuple == CL_NULL )
		return ( CL_PARAMS_ERR );

	if ( ppu8Data == CL_NULL )
		return ( CL_MEM_ERR );

	if ( *ppu8Data == CL_NULL )
		return ( CL_MEM_ERR );

	// check global params
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL )
		return ( CL_MEM_ERR );

// BRY_28092015 encore les conneries de FD
//	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )
//		return ( CL_ERROR );

	// initialize time with 0
	memset( pTuple->cl8Time, 0, sizeof( pTuple->cl8Time ) );

	// copy default parameters for tuple transmission in current type
	pTuple->tOptions.e_CryptedLink = g_tDefaultTuple.tOptions.e_CryptedLink ;

	// copy type of connection
	pTuple->tOptions.tCnct = g_tDefaultTuple.tOptions.tCnct;

	// type of encryption to apply on the data
	pTuple->tOptions.e_CryptedLink = g_tDefaultTuple.tOptions.e_CryptedLink;

	// number of repeat for this command
	pTuple->tOptions.ulRepeat = g_tDefaultTuple.tOptions.ulRepeat;

	// Period to wait before sending a new buffer. If 0, there is no waiting period between each send of data
	pTuple->tOptions.ulPeriod = g_tDefaultTuple.tOptions.ulPeriod;

	// Period to wait before sending a new buffer. If 0, there is no waiting period between each send of data
	pTuple->tOptions.ulMaxTimeout = g_tDefaultTuple.tOptions.ulMaxTimeout ;

	// Period to wait before sending a new buffer. If 0, there is no waiting period between each send of data
	pTuple->tOptions.pFileName = g_tDefaultTuple.tOptions.pFileName ;

	// pointer list shall be NULL
	pTuple->pNext = CL_NULL;

	// allocate memory from hosting environment for buffer struct if not done already
	if ( pBuff == CL_NULL )
	{
		//status = pCtxt->ptHalFuncs->fnAllocMem( (clvoid **)&pTuple->ptBuf, sizeof( t_Buffer ) );
		status = csl_malloc( (clvoid **)&pTuple->ptBuf, sizeof( t_Buffer ) );
		// if FAILED, exit
		if (( CL_FAILED (status) ) | ( pTuple->ptBuf == CL_NULL))
			return ( CL_MEM_ERR );
	}
	else	// initialize pBuff with already allocated structure
		pTuple->ptBuf = pBuff;

	// points tuple to current data array
	pTuple->ptBuf->pData = *ppu8Data;

	// set length of pointed data
	pTuple->ptBuf->ulLen = u32Len;

	// initialize next Tuple to CL_NULL
	pTuple->pNext = CL_NULL;

	// add time flag if any
	// before sending to network, tag data with time stamp
	pCtxt->ptHalFuncs->fnGetTime( pTuple->cl8Time, sizeof( pTuple->cl8Time) );

	DEBUG_PRINTF1("cl_initTuple: at time [%s]", pTuple->cl8Time);

	return ( CL_OK );
}

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
		clu32 u32Period, clu32 u32MaxTimeout, clu8 *pFileName )
{
	// check incoming parameters
	if ( pTuple == CL_NULL )
		return ( CL_PARAMS_ERR );


	if ( pu8Data == CL_NULL )
		return ( CL_PARAMS_ERR );

	// copy type of connection
	pTuple->tOptions.tCnct = eCtType;

	// type of encryption to apply on the data
	pTuple->tOptions.e_CryptedLink = g_tDefaultTuple.tOptions.e_CryptedLink;

	// number of repeat for this command
	pTuple->tOptions.ulRepeat = u32Repeat;

	// Period to wait before sending a new buffer. If 0, there is no waiting period between each send of data
	pTuple->tOptions.ulPeriod = u32Period;

	// Period to wait before sending a new buffer. If 0, there is no waiting period between each send of data
	pTuple->tOptions.ulMaxTimeout = u32MaxTimeout ;

	// Period to wait before sending a new buffer. If 0, there is no waiting period between each send of data
	pTuple->tOptions.pFileName = pFileName ;

	// points tuple to current data array
	pTuple->ptBuf->pData = pu8Data;

	// set length of pointed data
	pTuple->ptBuf->ulLen = u32Len;

	// initialize File Name to save data
	pTuple->tOptions.pFileName = pFileName;

	// pointer list shall be NULL
	pTuple->pNext = CL_NULL;

	return ( CL_OK );

}
