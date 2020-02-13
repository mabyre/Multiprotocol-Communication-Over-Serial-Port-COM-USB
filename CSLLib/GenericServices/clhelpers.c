/********************************************/
/*  Name: clextras.c                */
/*											*/
/*  Created on: 9 oct. 2014						*/
/*      Author: fdespres						*/
/********************************************/
/* Description : */
/*											*/
/********************************************/

#include <stdio.h>
#include <string.h>

#include "..\csl.h"
#include "..\inc\generic.h"
#include "..\inc\cltuple.h"
#include "..\inc\clreaders.h"
#include "..\inc\clhelpers.h"

#include <windows.h> // TODO : BRY : not good to include windows here !

#define DEBUG_PRINT_TUPLE

/*--------------------------------------------------------------------------*/

#define SIZE_MAX_BUFFER 512

/*--------------------------------------------------------------------------*/

static void _printBuffer(char *aString, char *aFormatString, ...)
{
    va_list theArgs;
    char 	theBuffer[SIZE_MAX_BUFFER];

    memset(theBuffer, 0, SIZE_MAX_BUFFER);
    va_start(theArgs, aFormatString);
    _vsnprintf(theBuffer, SIZE_MAX_BUFFER - 1, aFormatString, theArgs);
    strcat(aString, theBuffer);
}

/*--------------------------------------------------------------------------*\
 * Print aBuffer bytes by bytes in aString
 */
void PrintBuffer(char *aString, unsigned char *aBuffer, int aLenght, char *aFormatString)
{
	int i = 0;
	for ( i = 0; i < aLenght ; i++ )
	{
		_printBuffer( aString, aFormatString, aBuffer[i] );
	}
}

/*--------------------------------------------------------------------------*/

static void reader_ResetParsingParams( t_FrameParsingParameters *parsingParameters )
{
	t_clContext *pCtxt = CL_NULL;
	e_Result 	status = CL_ERROR;

	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return CL_ERROR;

	parsingParameters->StartOfTuple  = 0;
	parsingParameters->EndOfTuple    = 0;
	parsingParameters->MiddleOfTuple = 0;
	parsingParameters->MiddleIndex   = 0;
	parsingParameters->StartIndex    = 0;
	parsingParameters->EndIndex      = 0;
	parsingParameters->FrameCRC		 = 0x0000;

	pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&parsingParameters->pDataStart );
	pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&parsingParameters->pDataEnd );
}

/*--------------------------------------------------------------------------*\
 * Desencapsuler les trames en provenance du SerialPort COM
 *--------------------------------------------------------------------------*
 * Allouer la memoire pour les nouveaux buffers
 * les ajouter dans la liste pReader->p_TplList2Read
 * BRY-10/10/2015
 *--------------------------------------------------------------------------*
 * j'aurais pu faire :
 * int lg = cl_ReaderGetSerialEncapsTrailerLen( pReader );
 * READER_SERIAL_PROTOCOL_TRAILER_LEN == 3
 * de toutes les façons quand ca va changer et il faudra
 * revoir tout ça dans un futur projet
\*--------------------------------------------------------------------------*/
e_Result cl_NetworkToTuples( t_Reader *pReader, clu8 *pData, cl32 cl32Len, t_Tuple **ppTuple )
{
	t_clContext *pCtxt 		  = CL_NULL;
	e_Result 	status 		  = CL_ERROR;
	t_Buffer	*pCrtBuff 	  = CL_NULL;
	clu8		*pCrtData	  = CL_NULL;
	cl32		cl32Index	  = 0;
    clu16		calculateCrc  = 0x0000;

	clu32 		startPayloadIndex = 0;
	clu32 		endPayloadIndex = 0;

	clu8 		*pDataToWork;
	clu32		u32NbOfTuples = 0;

	t_Tuple		*pTupleNew	= CL_NULL;
	t_Tuple		*pTupleHead	= CL_NULL;
	t_Tuple		*pTupleNext	= CL_NULL;

	// check arguments
	if ( pData == CL_NULL )
		return CL_ERROR;

	// by default : one frame in one buffer
	pDataToWork = pData;

    if ( pReader == CL_NULL  )
        return CL_ERROR;

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return CL_ERROR;

	pTupleHead = *ppTuple;

	// If we are computing second part of Middle's mode
	if ( pReader->tParsingParams.MiddleOfTuple == 1 )
	{
		pReader->tParsingParams.StartOfTuple = 4;
	}

	//
	// Parse the incoming buffer
	//
	for ( cl32Index = 0; cl32Index < cl32Len; cl32Index++ )
	{
		// Check first byte of a new tuple
		if ( pReader->tParsingParams.StartOfTuple == 0 )
		{
			// check CLOVER_SERIAL_PROTO_SYNC
			if ( pData[ cl32Index ] == CLOVER_SERIAL_PROTO_SYNC )
			{
				pReader->tParsingParams.StartOfTuple = 1;
				pReader->tParsingParams.StartIndex 	 = cl32Index;

				// we only receive "FF"
				if ( cl32Len == 1 )
				{
					// copy the beginning of the data
					csl_malloc( (void **)&pReader->tParsingParams.pDataStart, cl32Len );
					memcpy( pReader->tParsingParams.pDataStart, pData, cl32Len );
					pReader->tParsingParams.MiddleIndex = cl32Len;
				}
			}
			continue;
		}

		// Detected start of tuple
		if ( pReader->tParsingParams.StartOfTuple == 1 )
		{
			if ( pData[ cl32Index ] ==  CLOVER_SERIAL_PROTO_STX )
			{
				pReader->tParsingParams.StartOfTuple = 2;

				// we only receive "FF02" save it
				if ( cl32Len == 2 )
				{
					// copy the beginning of the data
					csl_malloc( (void **)&pReader->tParsingParams.pDataStart, cl32Len );
					memcpy( pReader->tParsingParams.pDataStart, pData, cl32Len );
					pReader->tParsingParams.MiddleIndex = 2;
				}
			}
			else
			{
				DEBUG_PRINTF("cl_NetworkToTuples: STX_ERROR");
				reader_ResetParsingParams( &(pReader->tParsingParams) );
			}
			continue;
		}

		// Get the length of payload from 3rd byte: payload +  2bytes CRC + 1byte ETX
		if ( pReader->tParsingParams.StartOfTuple == 2 )
		{
			pReader->tParsingParams.EndIndex = pReader->tParsingParams.StartIndex + pData[cl32Index] + 2;

			// we are steel in Middle mode
			if ( pReader->tParsingParams.MiddleIndex != 0 )
			{
				// copy the end of the data
				csl_malloc( (void **)&pReader->tParsingParams.pDataEnd, pReader->tParsingParams.MiddleIndex + cl32Len );
				memcpy( pReader->tParsingParams.pDataEnd, pReader->tParsingParams.pDataStart, pReader->tParsingParams.MiddleIndex );
				memcpy( &pReader->tParsingParams.pDataEnd[ pReader->tParsingParams.MiddleIndex ], pData, cl32Len );

				pReader->tParsingParams.StartOfTuple = 4;
				pReader->tParsingParams.MiddleOfTuple = 1;
				continue;
			}

			// there is no end of data in this buffer, let's go in Middle mode
			if ( pReader->tParsingParams.EndIndex + 1 > cl32Len )
			{
				// copy the beginning of the data
				csl_malloc( (void **)&pReader->tParsingParams.pDataStart, cl32Len );
				memcpy( pReader->tParsingParams.pDataStart, pData,cl32Len );

				pReader->tParsingParams.MiddleIndex = cl32Len;
				pReader->tParsingParams.MiddleOfTuple = 1;
			}

			pReader->tParsingParams.StartOfTuple = 3;
			continue;
		}

		if ( ( pReader->tParsingParams.StartOfTuple == 4 ) && ( pReader->tParsingParams.MiddleOfTuple == 1 ) )
		{
			if ( cl32Index == pReader->tParsingParams.StartIndex )
			{
				// copy the end of the data
				csl_malloc( (void **)&pReader->tParsingParams.pDataEnd, pReader->tParsingParams.MiddleIndex + cl32Len );
				memcpy( pReader->tParsingParams.pDataEnd, pReader->tParsingParams.pDataStart, pReader->tParsingParams.MiddleIndex );
				memcpy( &pReader->tParsingParams.pDataEnd[ pReader->tParsingParams.MiddleIndex ], pData, cl32Len );
			}

			// CRC LSB
			if ( cl32Index == ( pReader->tParsingParams.EndIndex - pReader->tParsingParams.MiddleIndex ) - 2 )
			{
				pReader->tParsingParams.FrameCRC |= (( (clu16)pData[cl32Index]) & 0x00FF );
				continue;
			}

			// CRC MSB
			if ( cl32Index == ( pReader->tParsingParams.EndIndex - pReader->tParsingParams.MiddleIndex ) - 1 )
			{
				pReader->tParsingParams.FrameCRC |= (( ((clu16)pData[cl32Index]) << 8) & 0xFF00 );
				continue;
			}

			// if not gone over whole buffer, continue
			if ( cl32Index < ( pReader->tParsingParams.EndIndex - pReader->tParsingParams.MiddleIndex ) )
				continue;
			else
				pReader->tParsingParams.EndOfTuple = 1;
		}

		if ( pReader->tParsingParams.StartOfTuple == 3 )
		{
			// CRC LSB
			if ( cl32Index == pReader->tParsingParams.EndIndex - 2 )
			{
				pReader->tParsingParams.FrameCRC |= (( (clu16)pData[cl32Index]) & 0x00FF );
				continue;
			}

			// CRC MSB
			if ( cl32Index == pReader->tParsingParams.EndIndex - 1 )
			{
				pReader->tParsingParams.FrameCRC |= (( ((clu16)pData[cl32Index]) << 8) & 0xFF00 );
				continue;
			}

			// if not gone over whole buffer, continue
			if ( cl32Index < pReader->tParsingParams.EndIndex )
			{
				continue;
			}
			else
			{
				pReader->tParsingParams.EndOfTuple = 1;
			}
		}

		// Prepare the tuple to be added to the list
		if ( pReader->tParsingParams.EndOfTuple == 1 )
		{
			if (  pReader->tParsingParams.MiddleOfTuple == 1 )
			{
				pDataToWork = pReader->tParsingParams.pDataEnd;
			}
			else
			{
				pDataToWork = pData;
			}

            if ( CL_FAILED( status = cl_CheckSerialFrameEncapsulation( pReader, pDataToWork ) ) )
			{
            	DEBUG_PRINTF("cl_NetworkToTuples: SERIAL_ENCAPSULATION_ERROR");
            	reader_ResetParsingParams( &(pReader->tParsingParams) );
            	break;
			}

            // verify CRC get in frame - 2 is because CRC calculate also on Length - 4 is equal to FF02CRC1CRC203 - 1
            cl_HelperSerialCrc( &pDataToWork[ pReader->tParsingParams.StartIndex + 2 ], pReader->tParsingParams.EndIndex - pReader->tParsingParams.StartIndex - 4, &calculateCrc );
            if ( calculateCrc != pReader->tParsingParams.FrameCRC )
            {
            	DEBUG_PRINTF("cl_NetworkToTuples: CRC_ERROR");
            	reader_ResetParsingParams( &(pReader->tParsingParams) );
            	break;
            }

            // remove octets for protocol
			startPayloadIndex = pReader->tParsingParams.StartIndex + 3;
			endPayloadIndex = pReader->tParsingParams.EndIndex - 3;

			// allocate memory for data
			status = csl_pmalloc( ( clvoid **)&pCrtData, ( endPayloadIndex - startPayloadIndex ) + 1 );
			if ( CL_FAILED( status ) )
            {
            	DEBUG_PRINTF("cl_NetworkToTuples: MALLOC_ERROR");
            	reader_ResetParsingParams( &(pReader->tParsingParams) );
            	break;
            }

			// allocate memory for structure t_Buffer
			status = csl_pmalloc( ( clvoid **)&pCrtBuff, sizeof( t_Buffer ) );
			if ( CL_FAILED( status ))
                break;

			// length
			pCrtBuff->ulLen = ( endPayloadIndex - startPayloadIndex ) + 1;

			// allocate memory space
			pCrtBuff->pData = pCrtData;

			// BRY_08102015
			memset( pCrtBuff->pData, 0, pCrtBuff->ulLen);

			// copy related data in
			memcpy( pCrtBuff->pData, &pDataToWork[ startPayloadIndex ], pCrtBuff->ulLen );

			status = csl_pmalloc( (clvoid **)&pTupleNew, sizeof( t_Tuple ) );
			if ( CL_FAILED( status ))
				break;

			// initialize the tuple
			if ( CL_FAILED( cl_initTuple( pTupleNew, pCrtBuff, &pCrtBuff->pData, pCrtBuff->ulLen ) ) )
			{
				break;
			}

			//
			// Chain new Tuple at the end of the List
			//
			if ( pTupleHead == CL_NULL )
			{
				pTupleHead = pTupleNew;
			}
			else
			{
				pTupleNext = pTupleHead;
				while ( pTupleNext->pNext != CL_NULL )
				{
					pTupleNext = pTupleNext->pNext;
				}
				pTupleNext->pNext = (t_Tuple *)pTupleNew;
			}

			u32NbOfTuples++;

			pReader->tParsingParams.StartOfTuple  = 0;
			pReader->tParsingParams.MiddleOfTuple = 0;
			pReader->tParsingParams.EndOfTuple    = 0;
			pReader->tParsingParams.MiddleIndex   = 0;
			pReader->tParsingParams.FrameCRC      = 0x0000;

			pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&pReader->tParsingParams.pDataStart );
			pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&pReader->tParsingParams.pDataEnd );
		}
	}

	*ppTuple = pTupleHead;

#ifdef DEBUG_PRINT_TUPLE

	DEBUG_PRINTF("cl_NetworkToTuples: Incoming_Tuples: %d", u32NbOfTuples);

	t_Tuple *pTupleList = *ppTuple;
	for (;;)
	{
		if ( pTupleList != CL_NULL )
		{
			char buffString[1024];
			strnset(buffString, EOS, 1024);
			PrintBuffer(buffString, (clu8*)pTupleList->ptBuf->pData, pTupleList->ptBuf->ulLen, "%02X ");

			DEBUG_PRINTF("<--- From_Network : [port:%s] [time:%s] [len:%d] %s", pReader->tCOMParams.aucPortName, pTupleList->cl8Time,  pTupleList->ptBuf->ulLen, buffString);

			pTupleList = pTupleList->pNext ;
		}
		else
			break;
	}
#endif

	return status;
}

/*--------------------------------------------------------------------------*/

e_Result v0_000_cl_NetworkToTuples( t_Reader *pReader, clu8 *pData, cl32 cl32Len, t_Tuple **ppTuple )
{
	t_clContext *pCtxt 		= 	CL_NULL;
	e_Result 	status 		= 	CL_ERROR;
    e_Result    e_CheckStatus = CL_ERROR;
	t_Buffer	*pCrtBuff 	= 	CL_NULL;
	clu8		*pCrtData	= 	CL_NULL;
	cl32		cl32Index	= 	0;
	clu16		clu16Crc		= 0;

	clu8 		*pDataToWork;
	clu32		u32NbOfTuples	=	0;

	t_Tuple		*pTupleNew	= CL_NULL;
	t_Tuple		*pTupleHead	= CL_NULL;
	t_Tuple		*pTupleNext	= CL_NULL;

	// check arguments
	if ( pData == CL_NULL )
		return CL_ERROR;

	pDataToWork = pData;

    if ( pReader == CL_NULL  )
        return CL_ERROR;

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return CL_ERROR;

	pTupleHead = *ppTuple;

	// if we are computing Middle it's not start
	if ( pReader->tParsingParams.MiddleOfTuple == 1 )
	{
		pReader->tParsingParams.StartOfTuple = 2;
	}

	// parse the incoming buffer
	for ( cl32Index = 0; cl32Index < cl32Len; cl32Index++ )
	{
		// check first byte of a new tuple
		if ( pReader->tParsingParams.StartOfTuple == 0 )
		{
			// check CLOVER_SERIAL_PROTO_SYNC
			if ( pData[cl32Index] == CLOVER_SERIAL_PROTO_SYNC )
			{
				pReader->tParsingParams.StartOfTuple = 1;
				pReader->tParsingParams.StartIndex 	 = cl32Index;
				pReader->tParsingParams.EndOfTuple 	 = 0;
				pReader->tParsingParams.EndIndex     = 0;
				clu16Crc = 0;
			}
			continue;
		}

		// if we detected a start of tuple
		if ( pReader->tParsingParams.StartOfTuple == 1 )
		{
			// check if the second byte is of type STX
			if ( cl32Index == ( pReader->tParsingParams.StartIndex + 1) )
			{
				if ( pData[cl32Index] !=  CLOVER_SERIAL_PROTO_STX )	// reset the state machine
				{
					pReader->tParsingParams.StartOfTuple = 0;
					pReader->tParsingParams.StartIndex   = 0;
					pReader->tParsingParams.EndIndex	 = 0;
					pReader->tParsingParams.EndOfTuple	 = 0;
					clu16Crc = 0;
				}
				continue;
			}

			// get the length of payload from 3rd byte: payload +  2bytes CRC + 1byte ETX
			if ( cl32Index == ( pReader->tParsingParams.StartIndex + 2 ) )
			{
                pReader->tParsingParams.EndIndex = pData[cl32Index] + cl32Index;

                // is there the end of data
                if ( pReader->tParsingParams.EndIndex > cl32Len )
                {
                	// copy the beginning of the data
        			csl_malloc( (void **)&pReader->tParsingParams.pDataStart, cl32Len );
        			memcpy( pReader->tParsingParams.pDataStart, pData,cl32Len );

        			pReader->tParsingParams.MiddleIndex = cl32Len;
        			pReader->tParsingParams.MiddleOfTuple = 1;
                }
			}

			// CRC LSB
			if ( cl32Index == (pReader->tParsingParams.EndIndex - 2) )
			{
				clu16Crc |= (((clu16) pData[cl32Index])&0x00FF);
				continue;
			}

			// CRC MSB
			if ( cl32Index == ( pReader->tParsingParams.EndIndex - 1) )
			{
				clu16Crc |= ((((clu16) pData[cl32Index])>>8) &0xFF00);
				continue;
			}

			// if not gone over whole buffer, continue
			if ( cl32Index != pReader->tParsingParams.EndIndex )
				continue;
			else
				pReader->tParsingParams.EndOfTuple = 1;
		}

		if ( pReader->tParsingParams.MiddleOfTuple == 1 )
		{
			// if it's an end of the frame
			if ( ( cl32Index == 0 ) && ( pData[cl32Len - 1] ==  CLOVER_SERIAL_PROTO_ETX ) )
			{
            	// copy the end of the data
    			csl_malloc( (void **)&pReader->tParsingParams.pDataEnd, pReader->tParsingParams.MiddleIndex + cl32Len );
    			memcpy( pReader->tParsingParams.pDataEnd, pReader->tParsingParams.pDataStart, pReader->tParsingParams.MiddleIndex );
    			memcpy( &pReader->tParsingParams.pDataEnd[ pReader->tParsingParams.MiddleIndex ], pData, cl32Len );

    			pReader->tParsingParams.EndOfTuple = 1;
			}
			else
			{
				// otherwise we don't know what to do
				DEBUG_PRINTF("cl_NetworkToTuples: FRAME_REJECTED");

				reader_ResetParsingParams( &(pReader->tParsingParams) );
				return CL_ERROR;
			}

			// continue parsing for CRC
		}

		// at this stage, we prepare the tuple to be added to the list
		if ( pReader->tParsingParams.EndOfTuple == 1 )
		{
			// frame to short to be good - reject
			if ( pReader->tParsingParams.EndIndex < READER_SERIAL_PROTOCOL_TRAILER_LEN )
			{
				DEBUG_PRINTF("cl_NetworkToTuples: FRAME_REJECTED");

				reader_ResetParsingParams( &(pReader->tParsingParams) );
				return CL_ERROR;
			}

			if (  pReader->tParsingParams.MiddleOfTuple == 1 )
			{
				pDataToWork = pReader->tParsingParams.pDataEnd;
			}
			else
			{
				pDataToWork = pData;
			}

            if ( CL_FAILED( status =
            		cl_HelperCheckEncapsulateSerial(
            				pReader,
							&pDataToWork[ pReader->tParsingParams.StartIndex ],
							( pReader->tParsingParams.EndIndex - pReader->tParsingParams.StartIndex + 1 ),
							&pReader->tParsingParams.StartIndex,
							&pReader->tParsingParams.EndIndex, &e_CheckStatus) ) )
                break;

			// allocate memory for data + 2bytes to add CR / LF for DEBUG_PRINTF
			status = csl_pmalloc( ( clvoid **)&pCrtData, ( pReader->tParsingParams.EndIndex - pReader->tParsingParams.StartIndex + 1 + 3) );
			if ( CL_FAILED( status ))
                break;

			//----------------
			// allocate memory for structure t_Buffer
			status = csl_pmalloc( ( clvoid **)&pCrtBuff, sizeof( t_Buffer ) );
			if ( CL_FAILED( status ))
                break;

			// update length
			pCrtBuff->ulLen = ( pReader->tParsingParams.EndIndex - pReader->tParsingParams.StartIndex + 1 );

			// allocate memory space
			pCrtBuff->pData = pCrtData;

			// BRY_08102015
			memset( pCrtBuff->pData, 0, pCrtBuff->ulLen);

			// copy related data in
			memcpy( pCrtBuff->pData, &pDataToWork[pReader->tParsingParams.StartIndex], pCrtBuff->ulLen );

			status = csl_pmalloc( (clvoid **)&pTupleNew, sizeof( t_Tuple ) );
			if ( CL_FAILED( status ))
				break;

			// initialize the tuple
			if ( CL_FAILED( cl_initTuple( pTupleNew, pCrtBuff, &pCrtBuff->pData, pCrtBuff->ulLen ) ) )
			{
				break;
			}
			else
			{
				DEBUG_PRINTF1("cl_NetworkToTuples: cl_initTuple: pCrtBuff->ulLen: [%d octets]", pCrtBuff->ulLen);
			}

			//
			// Chain new Tuple at the end of the List
			//
			if ( pTupleHead == CL_NULL )
			{
				pTupleHead = pTupleNew;
			}
			else
			{
				pTupleNext = pTupleHead;
				while ( pTupleNext->pNext != CL_NULL )
				{
					pTupleNext = pTupleNext->pNext;
				}
				pTupleNext->pNext = (t_Tuple *)pTupleNew;
			}

			u32NbOfTuples++;
		}

		// this is a failing case. Junk is coming back from the network => trash the data until we have someting decent
		// or reset all counters as we have prepared the Tuple for upper layers
//		pReader->tParsingParams.StartOfTuple  = 0;
//		pReader->tParsingParams.EndOfTuple 	  = 0;
//		pReader->tParsingParams.MiddleOfTuple = 0;
//		pReader->tParsingParams.MiddleIndex	  = 0;
//		pReader->tParsingParams.FirstIndex    = 0;
//		pReader->tParsingParams.EndIndex      = 0;

		reader_ResetParsingParams( &(pReader->tParsingParams) );

		clu16Crc = 0;
	}

	*ppTuple = pTupleHead;

#ifdef DEBUG_PRINT_TUPLE

	DEBUG_PRINTF("cl_NetworkToTuples: Incoming_Tuples: %d", u32NbOfTuples);

	t_Tuple *pTupleList = *ppTuple;
	for (;;)
	{
		if ( pTupleList != CL_NULL )
		{
			char buffString[1024];
			strnset(buffString, EOS, 1024);
			PrintBuffer(buffString, (clu8*)pTupleList->ptBuf->pData, pTupleList->ptBuf->ulLen, "%02X ");

			DEBUG_PRINTF("<--- From_Network : [port:%s] [time:%s] [len:%d] %s", pReader->tCOMParams.aucPortName, pTupleList->cl8Time,  pTupleList->ptBuf->ulLen, buffString);

			pTupleList = pTupleList->pNext ;
		}
		else
			break;
	}
#endif

	return status;
}

/*--------------------------------------------------------------------------*/

e_Result cl_HelperNetworkToTuples( t_Reader *pReader, clu8 *pData, cl32 cl32Len, t_Tuple **ppTuple )
{
	t_clContext *pCtxt 		= 	CL_NULL;
	e_Result 	status 		= 	CL_ERROR;
    e_Result    e_CheckStatus = CL_ERROR;
	t_Buffer	*pCrtBuff 	= 	CL_NULL;
	clu16		clu16Crc		=	0;
	clu8		*pCrtData	= 	CL_NULL;
	cl32		cl32Index	= 	0;
	cl8			cl8EndOfTuple 	= 0;
	cl8			cl8StartOfTuple = 0;
	cl32		cl32FirstIndex 	= 0;
	cl32		cl32EndIndex	= 0;
	t_Tuple		*pTupleNew	= CL_NULL;
	t_Tuple		*pTupleHead	= CL_NULL;
	t_Tuple		*pTupleNext	= CL_NULL;

	clu32		u32NbOfTuples	=	0;

	// check arguments
	if ( pData == CL_NULL )
		return ( CL_ERROR );

    if ( pReader == CL_NULL  )
        return ( CL_ERROR );

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL)
		return ( CL_ERROR );

	pTupleHead = *ppTuple;

	/************************************************************************/
	/* parse the incoming buffer and prepare the tuple list for upper layer */
	/* the format of underlaying driver can concatenate several buffers together */
	/* valid data is always:	*/
	/*  - ASCII characters		*/
	/*  - then 0x00				*/
	/*  - then 0x0A				*/
	/*	- finally 0x0D			*/
	/**********************************************************************/
	for ( cl32Index = 0; cl32Index < cl32Len; cl32Index++ )
	{
		/* check first byte of a new tuple */
		if ( cl8StartOfTuple == 0 )
		{
			// check if specific character :
			// - serial sync CLOVER_SERIAL_PROTO_SYNC
			if ( pData[cl32Index] == CLOVER_SERIAL_PROTO_SYNC )
			{
				cl8StartOfTuple = 	1;
				cl32FirstIndex 	= 	cl32Index;
				cl8EndOfTuple 	= 	0;
				cl32EndIndex	= 	0;
				clu16Crc		=	0;
			}
			continue;
		}

		// if we detected a start of tuple
		if ( cl8StartOfTuple == 1 )
		{
			// check if the second byte is of type STX
			if ( cl32Index == ( cl32FirstIndex + 1) )
			{
				if ( pData[cl32Index] !=  CLOVER_SERIAL_PROTO_STX )	// reset the state machine
				{
					cl8StartOfTuple	= 0;
					cl32FirstIndex 	= 0;
					cl32EndIndex	= 0;
					cl8EndOfTuple	= 0;
					clu16Crc		= 0;
				}
				continue;
			}
			// get the length of payload from 3rd byte (length is all data after this field : aka payload +  2bytes CRC + 1byte ETX )
			if ( cl32Index == ( cl32FirstIndex + 2 ) )
                cl32EndIndex = pData[cl32Index] + cl32Index;

			// CRC LSB
			if ( cl32Index == (cl32EndIndex - 2) )
			{
				clu16Crc |= (((clu16) pData[cl32Index])&0x00FF);
				continue;
			}

			// CRC MSB
			if ( cl32Index == ( cl32Index - 1) )
			{
				clu16Crc |= ((((clu16) pData[cl32Index])>>8) &0xFF00);
				continue;
			}

			// if not gone over whole buffer, continue
			if ( cl32Index != cl32EndIndex )
				continue;
			else
				cl8EndOfTuple = 1;
		}

		/* at this stage, we prepare the tuple to be added to the list */
		if ( cl8EndOfTuple == 1 )
		{
            if ( CL_FAILED( status = cl_HelperCheckEncapsulateSerial( pReader, &pData[ cl32FirstIndex],  ( cl32EndIndex - cl32FirstIndex + 1 ), &cl32FirstIndex, &cl32EndIndex, &e_CheckStatus) ) )
                break;

			// allocate memory for data + 2bytes to add CR / LF for DEBUG_PRINTF
			status = csl_pmalloc( ( clvoid **)&pCrtData, ( cl32EndIndex - cl32FirstIndex + 1 + 3) );
			if ( CL_FAILED( status ))
                break;

			//----------------
			// allocate memory for structure t_Buffer
			status = csl_pmalloc( ( clvoid **)&pCrtBuff, sizeof( t_Buffer ) );
			if ( CL_FAILED( status ))
                break;

			// update length
			pCrtBuff->ulLen = ( cl32EndIndex - cl32FirstIndex + 1 );

			// allocate memory space
			pCrtBuff->pData = pCrtData;

			// initialize with 0
            memset( pCrtBuff->pData, 0, pCrtBuff->ulLen + 3);

			// add CR LF at the end of the buffer for PRINT
			pCrtBuff->pData[ pCrtBuff->ulLen ] = 0x0D;
			pCrtBuff->pData[ pCrtBuff->ulLen + 1 ] = 0x0A;
			pCrtBuff->pData[ pCrtBuff->ulLen + 2 ] = 0x00;

			// copy related data in
			memcpy( pCrtBuff->pData, &pData[cl32FirstIndex], pCrtBuff->ulLen );

			status = csl_pmalloc( (clvoid **)&pTupleNew, sizeof( t_Tuple ) );
			if ( CL_FAILED( status ))
				break;

			// initialize the tuple with default flags
			if ( CL_FAILED( cl_initTuple( pTupleNew, pCrtBuff, &pCrtBuff->pData, pCrtBuff->ulLen ) ) )
			{
				break;
			}
			else
			{
				DEBUG_PRINTF1("HelperNetworkToTuples: initTuple: [%d octets]", pCrtBuff->ulLen);
			}

			//
			// Chain new Tuple at the end of the List
			//
			if ( pTupleHead == CL_NULL )
			{
				pTupleHead = pTupleNew;
			}
			else
			{
				pTupleNext = pTupleHead;
				while ( pTupleNext->pNext != CL_NULL )
				{
					pTupleNext = pTupleNext->pNext;
				}
				pTupleNext->pNext = (t_Tuple *)pTupleNew;
			}

			u32NbOfTuples++;
		}

		// this is a failing case. Junk is coming back from the network => trash the data until we have someting decent or reset all counters as we have prepared the Tuple for upper layers
		cl8StartOfTuple = 	0;
		cl8EndOfTuple 	= 	0;
		cl32FirstIndex 	= 	0;
		cl32EndIndex 	= 	0;
		clu16Crc		=	0;
	}

	*ppTuple = pTupleHead;

#ifdef DEBUG_PRINT_TUPLE

	DEBUG_PRINTF("HelperNetworkToTuples: Incoming_Tuples: %d", u32NbOfTuples);

	t_Tuple *pTupleList = *ppTuple;
	for (;;)
	{
		if ( pTupleList != CL_NULL )
		{
			char buffString[1024];
			strnset(buffString, EOS, 1024);
			PrintBuffer(buffString, (clu8*)pTupleList->ptBuf->pData, pTupleList->ptBuf->ulLen, "%02X ");

			DEBUG_PRINTF("<--- From_Network : [port:%s] [time:%s] [len:%d] %s", pReader->tCOMParams.aucPortName, pTupleList->cl8Time,  pTupleList->ptBuf->ulLen, buffString);

			pTupleList = pTupleList->pNext ;
		}
		else
			break;
	}
#endif

	return ( status );
}

/*
 * VERYOLD_STINKS_SHIT by Mister FD
 */
e_Result VERYOLD_STINKS_SHIT__cl_HelperNetworkToTuples( t_Reader *pReader, clu8 *pData, cl32 cl32Len, t_Tuple **ppTuple )
{
	t_clContext *pCtxt 		= 	CL_NULL;
	e_Result 	status 		= 	CL_ERROR;
    e_Result    e_CheckStatus = CL_ERROR;
	t_Buffer	*pCrtBuff 	= 	CL_NULL;
	clu16		clu16Crc		=	0;
	clu8		*pCrtData	= 	CL_NULL;
	cl32		cl32Index	= 	0;
	cl8			cl8EndOfTuple 	= 0;
	cl8			cl8StartOfTuple = 0;
	cl32		cl32FirstIndex 	= 0;
	cl32		cl32EndIndex	= 0;
	t_Tuple		*pCrtTuple	= CL_NULL;
	t_Tuple		*pTupleListEntry	= CL_NULL;
	t_Tuple		*pTupleListTrailer	= CL_NULL;

	clu32		u32NbOfTuples	=	0;

	// check arguments
	if ( pData == CL_NULL )
		return ( CL_ERROR );

    if ( pReader == CL_NULL  )
        return ( CL_ERROR );

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL)
		return ( CL_ERROR );


	/************************************************************************/
	/* parse the incoming buffer and prepare the tuple list for upper layer */
	/* the format of underlaying driver can concatenate several buffers together */
	/* valid data is always:	*/
	/*  - ASCII characters		*/
	/*  - then 0x00				*/
	/*  - then 0x0A				*/
	/*	- finally 0x0D			*/
	/**********************************************************************/
	for ( cl32Index = 0; cl32Index < cl32Len; cl32Index++ )
	{
		/* check first byte of a new tuple */
		if ( cl8StartOfTuple == 0 )
		{
			// check if specific character :
			// - serial sync CLOVER_SERIAL_PROTO_SYNC
			if ( pData[cl32Index] == CLOVER_SERIAL_PROTO_SYNC )
			{
				cl8StartOfTuple = 	1;
				cl32FirstIndex 	= 	cl32Index;
				cl8EndOfTuple 	= 	0;
				cl32EndIndex	= 	0;
				clu16Crc		=	0;
			}
			continue;
		}

		// if we detected a start of tuple
		if ( cl8StartOfTuple == 1 )
		{
			// check if the second byte is of type STX
			if ( cl32Index == ( cl32FirstIndex + 1) )
			{
				if ( pData[cl32Index] !=  CLOVER_SERIAL_PROTO_STX )	// reset the state machine
				{
					cl8StartOfTuple	= 0;
					cl32FirstIndex 	= 0;
					cl32EndIndex	= 0;
					cl8EndOfTuple	= 0;
					clu16Crc		= 0;
				}
				continue;
			}
			// get the length of payload from 3rd byte (length is all data after this field : aka payload +  2bytes CRC + 1byte ETX )
			if ( cl32Index == ( cl32FirstIndex + 2 ) )
                cl32EndIndex = pData[cl32Index] + cl32Index;

			// CRC LSB
			if ( cl32Index == (cl32EndIndex - 2) )
			{
				clu16Crc |= (((clu16) pData[cl32Index])&0x00FF);
				continue;
			}

			// CRC MSB
			if ( cl32Index == ( cl32Index - 1) )
			{
				clu16Crc |= ((((clu16) pData[cl32Index])>>8) &0xFF00);
				continue;
			}

			// if not gone over whole buffer, continue
			if ( cl32Index != cl32EndIndex )
				continue;
			else
				cl8EndOfTuple = 1;
		}

		/* at this stage, we prepare the tuple to be added to the list */
		if ( cl8EndOfTuple == 1 )
		{
            if ( CL_FAILED( status = cl_HelperCheckEncapsulateSerial( pReader, &pData[ cl32FirstIndex],  ( cl32EndIndex - cl32FirstIndex + 1 ), &cl32FirstIndex, &cl32EndIndex, &e_CheckStatus) ) )
                break;

			// allocate memory for data + 2bytes to add CR / LF for DEBUG_PRINTF
			status = csl_pmalloc( ( clvoid **)&pCrtData, ( cl32EndIndex - cl32FirstIndex + 1 + 3) );
			if ( CL_FAILED( status ))
                break;

			//----------------
			// allocate memory for structure t_Buffer
			status = csl_pmalloc( ( clvoid **)&pCrtBuff, sizeof( t_Buffer ) );
			if ( CL_FAILED( status ))
                break;

			// update length
			pCrtBuff->ulLen = ( cl32EndIndex - cl32FirstIndex + 1 );

			// allocate memory space
			pCrtBuff->pData = pCrtData;

			// initialize with 0
            memset( pCrtBuff->pData, 0, pCrtBuff->ulLen + 3);

			// add CR LF at the end of the buffer for PRINT
			pCrtBuff->pData[ pCrtBuff->ulLen ] = 0x0D;
			pCrtBuff->pData[ pCrtBuff->ulLen + 1 ] = 0x0A;
			pCrtBuff->pData[ pCrtBuff->ulLen + 2 ] = 0x00;

			// copy related data in
			memcpy( pCrtBuff->pData, &pData[cl32FirstIndex], pCrtBuff->ulLen );

			//----------------
			// allocate memory for Tuple
			status = csl_pmalloc( ( clvoid **)&pCrtTuple, sizeof( t_Tuple ) );
			if ( CL_FAILED( status ))
                break;

			// initialize the tuple with default flags
			if ( CL_FAILED( cl_initTuple( pCrtTuple, pCrtBuff, &pCrtBuff->pData, pCrtBuff->ulLen ) ) )
			{
				break;
			}
			else
			{
				DEBUG_PRINTF("HelperNetworkToTuples: cl_initTuple: pCrtBuff->ulLen: [%d octets]", pCrtBuff->ulLen);
			}

			// add time flag if any
			// before sending to network, tag data with time stamp
			pCtxt->ptHalFuncs->fnGetTime( pCrtTuple->cl8Time, sizeof( pCrtTuple->cl8Time) );

			//-----------------
			// now affect the tuple to the (local tuple list)
			if ( pTupleListEntry == CL_NULL )	// initialize the list if empty
			{
				pTupleListEntry = pCrtTuple;
				pTupleListTrailer = pTupleListEntry;
				pTupleListEntry->pNext = CL_NULL;
			}
			else	// add current element to the list and displace trailer pointer accordingly
			{
				pTupleListTrailer->pNext = (t_Tuple *) pCrtTuple;
				pTupleListTrailer = pTupleListTrailer->pNext;
				pTupleListTrailer->pNext = CL_NULL;
			}
			u32NbOfTuples++;
		}

		// this is a failing case. Junk is coming back from the network => trash the data until we have someting decent or reset all counters as we have prepared the Tuple for upper layers
		cl8StartOfTuple = 	0;
		cl8EndOfTuple 	= 	0;
		cl32FirstIndex 	= 	0;
		cl32EndIndex 	= 	0;
		clu16Crc		=	0;
	}

	// save current list of type from network to upper layer
	*ppTuple = pTupleListEntry;

#ifdef DEBUG_PRINT
	DEBUG_PRINTF("HelperNetworkToTuples: Number_of_Tuples_Received: %d", u32NbOfTuples);
	t_Tuple *pTupleList = (t_Tuple *)pTupleListEntry;
	for (;;)
	{
		if ( pTupleList != CL_NULL )
		{
			DEBUG_PRINTF("<--- From_Network %s", pTupleList->ptBuf->pData );
			pTupleList = pTupleList->pNext ;
		}
		else
			break;
	}
#endif

	return ( status );
}


/*******************************************************************************************/
/* Name : e_Result cl_HelperEncapsulateToSerial( t_Reader *pReader, t_Buffer *ptBuff ) 		*/
/* Description :                                                                            */
/* add trailer/len/payload/crc/end of transmission to buffer to Tuple in order to send		*/
/* to readers. A check on reader type is performed to know if this is necessary				*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*		t_Reader 			*pReader:			// reader where the buffer shall be sent	*/
/*		t_Buffer 			*ptBuff :        	// buffer where to add the trailer/end		*/
/* ---------------                                                                          */
/*  Out: t_Buffer 			*ptBuff :        	// buffer where to add the trailer/end 		*/
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*	MEM_ERR					  : problem occured while getting memory from lower layer		*/
/*	ERROR					  : generic error												*/
/*******************************************************************************************/
e_Result cl_HelperEncapsulateToSerial( t_Reader *pReader, t_Buffer *ptBuff )
{
	t_clContext *pCtxt 		= 	CL_NULL;
	e_Result status 		= 	CL_NULL;
	clu8 	*pData 			= 	CL_NULL;
	clu32	u32Len			= 	0;
	clu16	u16Crc			=	0;

    // check incoming parameters
    if ( ptBuff == CL_NULL )
        return ( CL_PARAMS_ERR );

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL)
		return ( CL_ERROR );

	// get length of data to add for encapsulation
	if ( ( u32Len = cl_ReaderGetSerialEncapsLen( pReader) ) == 0 )
		return ( CL_OK );

	u32Len += ptBuff->ulLen;

	// Allocate memory for copying the tuple and adding trailer/end of telnet protocol
	if ( CL_FAILED( csl_pmalloc( (void **)&pData, u32Len ) ) )
		return ( CL_ERROR );

	if ( pData == CL_NULL )
		return ( CL_ERROR );

	do
	{
		// copy content from one buffer to the other
		memcpy( pData + cl_ReaderGetSerialEncapsTrailerLen( pReader ), ptBuff->pData , ptBuff->ulLen );

		// add trailer data
		pData[0] 			=	CLOVER_SERIAL_PROTO_SYNC;
		pData[1]			=	CLOVER_SERIAL_PROTO_STX;
		pData[2]			=	ptBuff->ulLen + cl_ReaderGetSerialEncapsFinishLen( pReader );
		pData[ u32Len - 1]	=	CLOVER_SERIAL_PROTO_ETX;

		if ( CL_FAILED( cl_HelperSerialCrc( &pData[2], (ptBuff->ulLen + 1) , &u16Crc ) ) )
				break;

		// add CRC
		pData[ u32Len - 3 ]	=	(clu8) u16Crc;
		pData[ u32Len - 2 ]	=	(clu8) (u16Crc>>8);

		// release memory of passed buffer
		if ( CL_FAILED( pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&ptBuff->pData ) )) // BRY_FREE Safe
			break;

		// assign new buffer to Tuple
		ptBuff->pData = pData;
		ptBuff->ulLen = u32Len;
		break;
	}
	while (1);

	if ( CL_FAILED( status) )
	{
		if ( pData )
		{
			pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&pData ); // BRY_FREE safe
		}
	}
	return ( status );
}

/*******************************************************************************************/
/* Name : e_Result cl_HelperCheckEncapsulateSerial(  t_Reader *pReader, clu8 *pData			*/
/*	clu32 u32Len, clu8 *pStartPayloadIndex, clu8 *pStopPayloadIndex,  e_Result *pe_checkStatus) */
/* Description :                                                                            */
/* check that:																				*/
/* 			encapsulated data from serial are correct										*/
/*			returns indexes of payload														*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*		t_Reader 			*pReader:			// reader where the buffer shall be sent	*/
/*		clu8 				*pData	: 			// buffer of data from reader				*/
/*		clu32 				u32Len	:			// length of buffer from reader				*/
/* ---------------                                                                          */
/*  Out: 		 																			*/
/*		clu8 				*pStartPayloadIndex: // Index of start of payload in buffer		*/
/*		clu8 				*pStopPayloadIndex : // Index of stop of payload in buffer		*/
/*		e_Result 			*pe_checkStatus		: // if format is ok=> CL_OK				*/
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*	MEM_ERR					  : problem occured while getting memory from lower layer		*/
/*	ERROR					  : generic error												*/
/*******************************************************************************************/
e_Result cl_CheckSerialFrameEncapsulation( t_Reader *pReader, clu8 *pData )
{
    e_Result status	= CL_OK;

    // check incoming parameters
    if ( pData == CL_NULL )
        return CL_PARAMS_ERR;

    // check protocol
    if
	(
		  ( pData[ pReader->tParsingParams.StartIndex ] !=	CLOVER_SERIAL_PROTO_SYNC )
		| ( pData[ pReader->tParsingParams.StartIndex + 1 ]	!=	CLOVER_SERIAL_PROTO_STX )
		| ( pData[ pReader->tParsingParams.StartIndex + 2 ] != ( pReader->tParsingParams.EndIndex - pReader->tParsingParams.StartIndex + 1 ) - cl_ReaderGetSerialEncapsFinishLen( pReader ) )
		| ( pData[ pReader->tParsingParams.EndIndex ] != CLOVER_SERIAL_PROTO_ETX)
	)
        return CL_PARAMS_ERR;

   return status;
}

/*
 * VERYOLD_STINKS_SHIT_
 * Encore une grosse merde à Despres décidément ...
 *
 */
e_Result  cl_HelperCheckEncapsulateSerial(  t_Reader *pReader, clu8 *pData, clu32 u32Len, cl32 *pStartPayloadIndex, cl32 *pStopPayloadIndex,  e_Result *pe_checkStatus)
{
    e_Result status 		= 	CL_NULL;

    // check incoming parameters
    if ( pData == CL_NULL )
        return ( CL_PARAMS_ERR );

    if ( u32Len == 0)
        return ( CL_PARAMS_ERR );

    if ( pStartPayloadIndex == CL_NULL )
        return ( CL_PARAMS_ERR );

    if ( pStopPayloadIndex == CL_NULL )
        return ( CL_PARAMS_ERR );

    if ( pe_checkStatus == CL_NULL )
        return ( CL_PARAMS_ERR );

    *pe_checkStatus = CL_ERROR;
    // add trailer data
    if ( ( pData[0] !=	CLOVER_SERIAL_PROTO_SYNC ) | ( pData[1]	!=	CLOVER_SERIAL_PROTO_STX) | ( pData[2] != (u32Len - cl_ReaderGetSerialEncapsFinishLen( pReader )) ) | ( pData[ u32Len - 1] != CLOVER_SERIAL_PROTO_ETX) )
        return ( CL_PARAMS_ERR );

    *pStopPayloadIndex  = *pStartPayloadIndex + u32Len - 4;
    *pStartPayloadIndex += 3;


    *pe_checkStatus = CL_OK;

   return ( status );
}

/*******************************************************************************************/
/* Name : e_Result cl_HelperSerialCrc( t_Buffer *ptBuff, clu16 *pu16Crc ) 					*/
/* Description :                                                                            */
/* compute CRC 16 on the data buffer and returns it											*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*		t_Buffer 			*ptBuff :        	// buffer where to compute the crc			*/
/* ---------------                                                                          */
/*  Out: clu16				*pu16Crc 			// pointer on CRC							*/
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*	ERROR					  : generic error												*/
/*******************************************************************************************/
e_Result cl_HelperSerialCrc( clu8 *pData, clu32 u32Len, clu16 *pu16Crc )
{
	e_Result status 	= 	CL_OK;
	clu16 	u16Crc		= 	0;
	clu16	u16Poly		= 	0x8408;
	clu16	u16Carry	=	0;
	clu32	i, j;

	/* check incoming parameters */
	if ( pData == CL_NULL )
		return ( CL_ERROR );

	if ( pu16Crc == CL_NULL )
		return ( CL_ERROR );

	for ( i = 0; i < u32Len ; i++ )
	{
		u16Crc ^= ( (clu16)(pData[i] ) );	// xor existing incoming crc with new byte

		for ( j = 0; j < 8; j++ )				// perform CRC/Carry on bits
		{
			u16Carry =	(u16Crc & 1);
			u16Crc /=2;

			if ( u16Carry )
				u16Crc ^= u16Poly;
		}
	}

	*pu16Crc = u16Crc;

	return ( status );
}

/*******************************************************************************************/
/* Name : e_Result cl_HelperOTAFrameCrc( clu8 *pData, clu32 u32Len, clu16 u16FrameId, clu16 *pu16Crc ) 	*/
/* Description :                                                                            */
/* compute CRC 16 on the frame buffer and returns it											*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*			*pData	: pointer of frame buffer where to compute CRC							*/
/*			u32Len	: len of frame buffer to compute CRC for								*/
/*			u16TransactionId	: transaction Id of the exchange							*/
/* ---------------                                                                          */
/*  Out: clu16				*pu16Crc 			// pointer on CRC							*/
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*	ERROR					  : generic error												*/
/*******************************************************************************************/
e_Result cl_HelperOTAFrameCrc( clu8 *pData, clu32 u32Len, clu16 u16TransactionId, clu16 *pu16Crc )
{
	e_Result status 	= 	CL_OK;
	clu16 	u16Crc		= 	0;
	clu16	u16Poly		= 	0x8408;
	clu16	u16Carry	=	0;
	clu32	i, j;

	/* check incoming parameters */
	if ( pData == CL_NULL )
		return ( CL_ERROR );

	if ( pu16Crc == CL_NULL )
		return ( CL_ERROR );

	if ( CL_FAILED( cl_HelperSerialCrc( pData, u32Len, &u16Crc ) ) )
		return ( CL_ERROR );

	// Xor finally with frame Id
	u16Crc = u16TransactionId ^ u16Crc ;

	*pu16Crc = u16Crc;

	return ( status );
}

/*******************************************************************************************/
/* Name : e_Result cl_HelperDiscoverNetworkToTuples( t_Reader *pReader,  clu8 *pData, cl32 cl32Len  */
/* , t_Tuple **pptTuple                                                                     */
/* Description :                                                                            */
/* parse the incoming buffer and prepare the tuple list for upper layer 					*/
/* the format of underlaying driver can concatenate several buffers together 				*/
/* -------------------------																*/
/*	Note :																					*/
/* if a valid buffer was found and a set of tuples was created, memory linked to incoming   */
/* buffer is not freed!!! 																	*/
/*  .... as we don"t know what the caller can do with this memory							*/
/*																							*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*		clu8					*pData;			// buffer from underlayer			 		*/
/*		cl32                 cl32Len;         	// length of the buffer to treat			*/
/* ---------------                                                                          */
/*  Out: t_Tuple 			**pptTuple;			// linked list of Tuple 					*/
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*	MEM_ERR					  : problem occured while getting memory from lower layer		*/
/*	ERROR					  : generic error												*/
/*******************************************************************************************/
e_Result cl_HelperDiscoverNetworkToTuples( t_Reader *pReader, clu8 *pData, cl32 cl32Len, t_Tuple **pptTuple )
{
	t_clContext *pCtxt 		= 	CL_NULL;
	e_Result 	status 		= 	CL_ERROR;
    e_Result    e_CheckStatus = CL_ERROR;
	t_Buffer	*pCrtBuff 	= 	CL_NULL;
	clu16		clu16Crc		=	0;
	clu8		*pCrtData	= 	CL_NULL;
	cl32		cl32Index	= 	0;
	cl8			cl8EndOfTuple 	= 0;
	cl8			cl8StartOfTuple = 0;
	cl32		cl32FirstIndex 	= 0;
	cl32		cl32EndIndex	= 0;
	t_Tuple		*pCrtTuple	= CL_NULL;
	t_Tuple		*pTupleListEntry	= CL_NULL;
	t_Tuple		*pTupleListTrailer	= CL_NULL;

	// check arguments
	if ( pData == CL_NULL )
		return ( CL_ERROR );

    if ( pReader == CL_NULL  )
        return ( CL_ERROR );

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL)
		return ( CL_ERROR );


	/************************************************************************/
	/* parse the incoming buffer and prepare the tuple list for upper layer */
	/* the format of underlaying driver can concatenate several buffers together */
	/* valid data is always:	*/
	/*  - ASCII characters		*/
	/*  - then 0x00				*/
	/*  - then 0x0A				*/
	/*	- finally 0x0D			*/
	/**********************************************************************/
	if ( cl32Len < CSL_DISCOVER_PROTOCOL_LANTRONIX_MIN_LEN)
		return ( CL_OK);

	do
	{

		if ( ( pData[CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_0_OFFSET] == CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_0)
			& ( pData[CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_1_OFFSET] == CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_1)
			& ( pData[CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_2_OFFSET] == CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_2)
			& ( pData[CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_3_OFFSET] == CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_3) )
		{

			// allocate memory for data: CSL_DISCOVER_PROTOCOL_LANTRONIX_IP_ADDR_LEN
			status = csl_pmalloc( ( clvoid **)&pCrtData, CSL_DISCOVER_PROTOCOL_LANTRONIX_IP_ADDR_LEN );
			if ( CL_FAILED( status ))
				break;

			//----------------
			// allocate memory for structure t_Buffer
			status = csl_pmalloc( ( clvoid **)&pCrtBuff, sizeof( t_Buffer ) );
			if ( CL_FAILED( status ))
				break;

			// update length
			pCrtBuff->ulLen = CSL_DISCOVER_PROTOCOL_LANTRONIX_IP_ADDR_LEN;

			// allocate memory space
			pCrtBuff->pData = pCrtData;

			// copy related data in
			memcpy( pCrtBuff->pData, &pData[CSL_DISCOVER_PROTOCOL_LANTRONIX_IP_ADDR_OFFSET], pCrtBuff->ulLen );

			//----------------
			// allocate memory for Tuple
			status = csl_pmalloc( ( clvoid **)&pCrtTuple, sizeof( t_Tuple ) );
			if ( CL_FAILED( status ))
                break;

			// initialize the tuple with default flags
			if ( CL_FAILED( status = cl_initTuple( pCrtTuple, pCrtBuff, &pCrtBuff->pData, pCrtBuff->ulLen ) ) )
				break;

			// add time flag if any
			// before sending to network, tag data with time stamp
			pCtxt->ptHalFuncs->fnGetTime( pCrtTuple->cl8Time, sizeof( pCrtTuple->cl8Time) );

			// save current list of type from network to upper layer
			*pptTuple = pCrtTuple;

			if ( pTupleListEntry == CL_NULL )	// initialize the list if empty
			{
				pTupleListEntry = pCrtTuple;
				pTupleListTrailer = pTupleListEntry;
				pTupleListEntry->pNext = CL_NULL;
			}

		}
		else
			return ( CL_OK );
		break;
	}
	while (1);

#ifdef DEBUG_PRINT
	for (;;)
	{
		if ( pTupleListEntry != CL_NULL )
		{
			DEBUG_PRINTF("<- From Network %s\n", pTupleListEntry->ptBuf->pData );
			pTupleListEntry = pTupleListEntry->pNext ;
		}
		else
			break;
	}
#endif

	// -------------------------
	//	Note :
	// if a valid buffer was found and reshaped, memory linked to incoming buffer is not freed!!! .... as we don"t know what the caller can do with this memory
	///
	// -------------------------
	// free memory in case of error
	if ( CL_FAILED( status ) )
	{
		// release memory
		if ( pCrtData )
			pCtxt->ptHalFuncs->fnFreeMem( pCrtData );

		if ( pCrtBuff )
			pCtxt->ptHalFuncs->fnFreeMem( pCrtBuff );

		// empty the list of tuples if any and release memory
		if ( pTupleListEntry != CL_NULL )
		{
			pCrtTuple = pTupleListEntry ;

			while ( pCrtTuple )
			{
				pTupleListTrailer = pCrtTuple->pNext;
				pCtxt->ptHalFuncs->fnFreeMem( pCrtTuple->ptBuf->pData );
				pCtxt->ptHalFuncs->fnFreeMem( pCrtTuple->ptBuf );
				pCtxt->ptHalFuncs->fnFreeMem( pCrtTuple );
				pCrtTuple = CL_NULL;
				pCrtTuple = pTupleListTrailer;
			}
		}

	}

	return ( status );
}


/*******************************************************************************************/
/* Name : e_Result cl_HelperConvertASCIItoHex( clu8 *pASCIIData, clu32 u32ASCIILen, clu8 *pHexBuf, clu32 *pu32Len ) */
/* Description :                                                                            */
/* read ASCII data, convert it to ASCII and push content to output buffer 					*/
/*																							*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*		clu8					*pASCIIData;	// buffer with ASCII buffer	 				*/
/*		cl32                 	u32ASCIILen;    // length of the buffer to treat			*/
/* ---------------                                                                          */
/*  Out: 																					*/
/*		clu8					*pHexBuf;	// buffer with ASCII buffer	 				*/
/*		cl32                 	pu32Len;    // length of the buffer to treat			*/
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*	MEM_ERR					  : problem occured while getting memory from lower layer		*/
/*	ERROR					  : generic error												*/
/*******************************************************************************************/
e_Result cl_HelperConvertASCIItoHex( clu8 *pASCIIData, clu32 u32ASCIILen, clu8 *pHexBuf, clu32 *pu32Len )
{
	t_clContext *pCtxt 		= 	CL_NULL;
	e_Result 	status 		= 	CL_ERROR;
	clu32 		i			=	0;
	clu8		ucNibble	=	0;
	clu32		j			=	0;
	// check arguments
	if ( pASCIIData == CL_NULL )
		return ( CL_ERROR );

    if ( pHexBuf == CL_NULL  )
        return ( CL_ERROR );

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL)
		return ( CL_ERROR );

	if ( u32ASCIILen == 0)
		return ( CL_ERROR );

	/* */

    /* convert ASCII to byte */
    for ( i = 0; i < u32ASCIILen; i++ )
    {
    	j = i+ ( u32ASCIILen %2 );

        if (!(j%2))
        {
            if (( pASCIIData[i]>= 0x30 ) & ( pASCIIData[i] <= 0x39))
                ucNibble = (pASCIIData[i]- 0x30)<<4;
            else
            {
                if (( pASCIIData[i] >= 0x41 ) & ( pASCIIData[i] <0x47))
                    ucNibble = (pASCIIData[i]- 0x41 + 0x0A)<<4;
            }
        }
        else
        {
            if (( pASCIIData[i]>= 0x30 ) & ( pASCIIData[i] <= 0x39))
                ucNibble |= (pASCIIData[i]- 0x30)&0x0F;
            else
            {
                if (( pASCIIData[i] >= 0x41 ) & ( pASCIIData[i] <0x47))
                    ucNibble |= (pASCIIData[i]- 0x41 + 0x0A)&0x0F;
            }
            pHexBuf[i/2]= ucNibble;
        }
    }
    *pu32Len = (i / 2) + (u32ASCIILen % 2);

    return ( CL_OK );
}
