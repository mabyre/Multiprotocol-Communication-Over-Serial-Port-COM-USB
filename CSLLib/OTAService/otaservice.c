/********************************************/
/*  Name: otaservice.c                */
/*											*/
/*  Created on: 6 janv. 2015						*/
/*      Author: fdespres						*/
/********************************************/
/* Description : */
/*											*/
/********************************************/
#define OTASERVICE_C_

#include "..\csl.h"
#include "..\inc\otaservice.h"

/*******************************************************************************************/
/** @fn : clvoid cl_initOTAService(  )
* @brief :
* \n       	initialize internal variables of OTA service
* @param **ppOta	: ( In-Out ) stores pointer on OTA object
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* *************************************************************************************** */
clvoid cl_initOTAService( )
{
	g_tOtaInternal.pTrailerData[ 0 ] = 0x01;
	g_tOtaInternal.pTrailerData[ 1 ] = 0x09;
	g_tOtaInternal.pTrailerData[ 2 ] = 0x09;
	g_tOtaInternal.pEndData			=	CL_NULL;
	g_tOtaInternal.u32CurrentPacket	=	0;
	g_tOtaInternal.u8CurrentCommand	=	0;
	g_tOtaInternal.u32TrailerLen	=	3;
	g_tOtaInternal.u32EndLen		=	0;
	g_tOtaInternal.u32BinaryLen		=	0;
	g_tOtaInternal.pBinaryData		=	CL_NULL;
	g_tOtaInternal.eEncReaderState	=	STATE_DEFAULT;
	g_tOtaInternal.eTargReaderState	=	STATE_DEFAULT;
	g_tOtaInternal.pTargReader		=	CL_NULL;
	g_tOtaInternal.pEncReader		=	CL_NULL;
}
/*******************************************************************************************/
/** @fn : e_Result cl_getOTAService( t_OtaInternal **ppOta )
* @brief :
* \n       	return a pointer on OTA global variable
* @param **ppOta	: ( In-Out ) stores pointer on OTA object
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* *************************************************************************************** */
e_Result cl_getOTAService( t_OtaInternal **ppOta )
{
	if ( ppOta == CL_NULL )
		return ( CL_ERROR );

	*ppOta = &g_tOtaInternal;

	return ( CL_OK);
}
/*******************************************************************************************/
/** @fn : e_Result cl_startOTA( t_Reader *pEncryptionKeyReader, t_Reader *pTargetReader);
* @brief :
* \n       	start the OTA process with parameters set in the file defined in cl_Init
* \n       	block the readers passed in parameters in emission/reception while are in OTA_MODE
* \n 		until the end of the proce
* @param *pEncryptionKeyReader	: ( In ) designate the Key which will perform the encryption
* 								of the file
* @param *pTargetReader			: ( In ) reader used to transmit the encrypted frames to the
* 								end device via the CLOVER-NET architecture
*  @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* \n TIMEOUT_ERR,              :  Overrun on timing
* *************************************************************************************** */
e_Result cl_startOTA( t_Reader *ptEncryptionKeyReader, t_Reader *ptTargetReader )
{
	e_Result status 			= 	CL_ERROR;
	t_clContext	*pCtxt 			= 	CL_NULL;
	t_Reader *pEncReader 		= 	CL_NULL;
	t_Reader *pTargReader 		= 	CL_NULL;
	e_State	eEncState			= 	STATE_DEFAULT;
	e_State	eTargState			=	STATE_DEFAULT;

	// check global parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->tCrypto.pSgl == CL_NULL)
		return ( CL_ERROR );

	if ( ptEncryptionKeyReader == CL_NULL )
		return ( CL_ERROR );

	if ( ptTargetReader == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnSemaphoreWait == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnSemaphoreRelease == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnWaitMs == CL_NULL )
		return ( CL_ERROR );

	if ( CL_FAILED( cl_readerFindInList( &pEncReader, ptEncryptionKeyReader ) ) )
		return ( CL_ERROR );

	if ( pEncReader == CL_NULL )
		return ( CL_ERROR );

	if ( CL_FAILED( cl_readerFindInList( &pTargReader, ptTargetReader ) ) )
		return ( CL_ERROR );

	// wait for available readers before starting an OTA process
	do
	{
		if ( CL_FAILED( cl_ReaderGetState( pEncReader, &eEncState ) ) )
			return ( CL_ERROR );

		if ( CL_FAILED( cl_ReaderGetState( pTargReader, &eTargState ) ) )
			return ( CL_ERROR );
	}while ( ( eEncState == STATE_OTA) | ( eTargState == STATE_OTA) );

	// wait for OTA task to be available
	status = pCtxt->ptHalFuncs->fnSemaphoreWait( pCtxt->tCrypto.pSgl,  1 );

	// save encryption dongle in charge of encrypting the data
	pCtxt->tCryptoParams.ptEncryptionKeyReader = ptEncryptionKeyReader;

	// save target reader in charge of transmitting the data to CLOVER-NET
	pCtxt->tCryptoParams.ptTargetReader =	ptTargetReader;

	// unblock OTA serviceby signaling that a data is present
	pCtxt->ptHalFuncs->fnSemaphoreRelease( pCtxt->tCrypto.pSgl );

	// wait for OTA thread to get the request and do necessary initialization
	//pCtxt->ptHalFuncs->fnWaitMs( 1000 );


	return ( CL_OK );
}

/*****************************************************************************/
/** @fn :  e_Result cl_OTAProgress_cb( clu32 eState, clvoid *pReader, clu32 u32Progress );
* 	@brief : OTA service update from underlayer
*
*
*  @param	eState						( In ) Command sent to lower layer
*  @param 	*pReader					( In ) Reader which issued the data
*  @param	u32Progress					( In ) progress on 100% per 5% step for display in the OTA process
*  @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result cl_OTAProgress_cb ( clu32 eState, clvoid *pReader, clu32 u32Progress )
{
	e_Result status = CL_ERROR;


	return ( status );
}

/*****************************************************************************/
/** @fn :  e_Result cl_OTASendDataDone_cb( clvoid *pReader, clvoid *pTuple, e_Result status );
* 	@brief : OTA send data to network notification
*
*
*  @param 	*pReader					( In ) Reader which issued the data
*  @param	*pTuple						( In ) Tuple send to network
*  @param	status						( In ) status of the send to network
*  @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result cl_OTASendDataDone_cb( clvoid *pReader, clvoid *pTuple, e_Result status )
{
	e_Result eStatus = CL_ERROR;


	return ( eStatus );
}

/*****************************************************************************/
/** @fn :  (*fnOTAData2Read_cb) ( clvoid *ppReader, clvoid *ppTuple, e_Result eStatus );
* 	@brief : OTA data has been received from underlayer
*
*
*  @param 	*pReader					( In ) Reader which issued the data
*  @param	*pTuple						( In ) Tuple send to network
*  @param	status						( In ) status of the send to network
*  @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* \n TIMEOUT_ERR,              :  Overrun on timing
* ************************************************************************** */
e_Result cl_OTAData2Read_cb( clvoid *ppReader, clvoid *ppTuple, e_Result status )
{
	e_Result 		eStatus 	= 	CL_ERROR;
    t_OtaInternal 	*pOta		=	CL_NULL;
	t_clContext 	*pCtxt 		= 	CL_NULL;
	t_Tuple 		*pTuple		=	CL_NULL;
	t_Tuple			*pTuple2Send=	CL_NULL;
	t_Reader		*pReader	=	CL_NULL;
	t_Reader		*pReader2Send=	CL_NULL;
	clu8			a8OTACloverService[]	=	{0x01, 0x09, 0x09 };
	clu8			a8OTABuff[256 + 64 ];	// !!!!! 64 is totally arbitrary!!! this is extra memory space to hold trailer/additionnal data to get access to OTA service in CLOVER NET
	clu8			au8TransId[2];
	t_Buffer		tLocalBuf;
	clu32			u32OTACopyLen;			// Len to copy in OTA buff
	clu16			u16Crc	=	0;			// crc for frame
	clu32			u32DataLen2Send	=	0;
	clu32			u32TsfNb		=	0;
	clu8			*pData2Send		=	CL_NULL;
	clu8			*pValue			=	CL_NULL;
	clu32			u32ValueLen		=	0;
	clu32			u32TempLen		=	0;
	clu32			i				=	0;
	clu32			j				=	0;
	clu32			u32MissFragId	=	0;		// missing fragment
	e_Result		bFound			=	CL_ERROR;
	clu16			u16TransactionId	=	0;
    // get pointer on OTA service
    if ( CL_FAILED( cl_getOTAService( &pOta ) ) )
    	return ( CL_ERROR );

    if ( pOta == CL_NULL )
    	return ( CL_ERROR );

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	// check global parameters
	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );


	if ( ppTuple == CL_NULL)
		return ( CL_ERROR );

	if ( ppReader == CL_NULL )
		return ( CL_ERROR );

	// get reader pointer
	pReader = ( t_Reader *)(* (( t_Reader **)ppReader));

	// get Tuple pointer
	pTuple = ( t_Tuple *)(*(( t_Tuple **)ppTuple));
	if ( !pTuple )
		return ( CL_ERROR );

	// check this data is coming from OTA service. If not, it was pushed to user land.... don't care, it can be auto-generated answer from devices in the network. Only
	// the final application can decide what to do with that
	if ( !pTuple->ptBuf)
		return ( CL_ERROR );

	// check if len is sufficient including the command from the OTA service
	if ( pTuple->ptBuf->ulLen < sizeof( a8OTACloverService ) )
		return ( CL_ERROR );

	// compare the incoming data. if the header is not the correct one-> exit
	if ( !memcmp( a8OTACloverService, pTuple->ptBuf->pData , sizeof( a8OTACloverService ) ) )
	{
		return ( CL_OK);
	}

	tLocalBuf.pData = 	&pTuple->ptBuf->pData[pOta->u32TrailerLen];
	tLocalBuf.ulLen	=	pTuple->ptBuf->ulLen - pOta->u32TrailerLen - pOta->u32EndLen;

	if ( tLocalBuf.ulLen == 0)
	{
		DEBUG_PRINTF("OTA : ENTER DOWNLOAD MODE: ERROR Unknown command\n");
		return ( CL_ERROR );
	}

	// reset memory to set the command
	memset( a8OTABuff, 0, sizeof( a8OTABuff) );

	// copy header/trailer to access ota service
	memcpy( a8OTABuff, pOta->pTrailerData, pOta->u32TrailerLen );

	if ( pOta->u8CurrentCommand != tLocalBuf.pData[  CL_OTA_GENERIC_CMD_OFFSET ] )
	{
		DEBUG_PRINTF("ERROR in OTA STATE Machine : Expected %x != Received %x", pOta->u8CurrentCommand, tLocalBuf.pData[  CL_OTA_GENERIC_CMD_OFFSET ]);
	}
	// depending on incoming command, makes change in the OTA service state machine
	switch ( tLocalBuf.pData[  CL_OTA_GENERIC_CMD_OFFSET ] )
	{
		case ( CL_OTA_TIMEOUT_EVENT ):
		{
			DEBUG_PRINTF("OTA Service Error: TIMEOUT ERROR \n");
			pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
			status = CL_ERROR;

			break;
		}
		case ( CL_OTA_MW_FW_UPGRADE_INIT ):
		{
			DEBUG_PRINTF("OTA OTA_MW_FW_UPGRADE_INIT\n");
			pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
			status = CL_ERROR;

			break;
		}
		case ( CL_OTA_ENTER_DOWNLOAD_MODE ):
		{
			DEBUG_PRINTF("CL_OTA_ENTER_DOWN_STATUS_OFF = %x", tLocalBuf.pData[CL_OTA_ENTER_DOWN_STATUS_OFF]);
			// check the return code and prepare next command
			switch ( tLocalBuf.pData[CL_OTA_ENTER_DOWN_STATUS_OFF] )
			{
				case (	CL_OTA_ENTER_DOWN_STATUS_SUCCESS	): /// succeeded
				{
					DEBUG_PRINTF("OTA ENTER DOWNLOAD MODE: SUCCESS %d\n", cl_GetElapsedTime());

					// compute data length to copy
					if ( ( pOta->u32BinaryLen - pOta->u32BinaryIndex + 1) > CL_OTA_DOWN_FRAGMENT_SIZE )
						u32OTACopyLen	=	CL_OTA_DOWN_FRAGMENT_SIZE;
					else
						u32OTACopyLen	=	pOta->u32BinaryLen - pOta->u32BinaryIndex + 1;
					// if not 0 and continue previous session was not required, then prepare command DOWNLOAD_FRAGMENT
					// else go to missing fragment

					if (( u32OTACopyLen )|(pOta->bOTAContinuePrvSession == CL_OK))
					{
						// Command id
						a8OTABuff[ CL_OTA_DOWNLOAD_FRAGMENT_CMD_OFF + pOta->u32TrailerLen ]	= CL_OTA_DOWNLOAD_FRAGMENT;

						// frame_id
						a8OTABuff[ CL_OTA_DOWNLOAD_FRAGMENT_FRAME_ID_OFF + pOta->u32TrailerLen ]	= (clu8)((pOta->u32CurrentPacket>>8)&0x000000FF);
						a8OTABuff[ CL_OTA_DOWNLOAD_FRAGMENT_FRAME_ID_OFF + 1 + pOta->u32TrailerLen ]	= (clu8)(pOta->u32CurrentPacket&0x000000FF);
						pOta->u32CurrentPacket++;

						// copy data
						memcpy( &a8OTABuff[CL_OTA_DOWNLOAD_FRAGMENT_DATA_BYTES_OFF + pOta->u32TrailerLen], &pOta->pBinaryData[pOta->u32BinaryIndex], u32OTACopyLen );

						// get transaction ID
						if ( CL_FAILED ( status = cl_GetParams( (clu8*)"ota_transaction_id", strlen("ota_transaction_id"), &pValue, &u32ValueLen, CL_OK ) ) )
							break;
						if ( u32ValueLen>4)
							break;
						cl_HelperConvertASCIItoHex( pValue, u32ValueLen, au8TransId, &u32TempLen );
						if ( u32TempLen != 2)
						{
							pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
							status = CL_ERROR;
							break;
						}
						cl_FreeMem( pValue );

						// compute transaction id
						u16TransactionId =	((((clu16)au8TransId[0])<<8)&0xFF00);
						u16TransactionId |=	((((clu16)au8TransId[1]))&0x00FF);

						//	compute crc 16 xor transaction id
						cl_HelperOTAFrameCrc( &a8OTABuff[CL_OTA_DOWNLOAD_FRAGMENT_DATA_BYTES_OFF + pOta->u32TrailerLen], u32OTACopyLen, u16TransactionId, &u16Crc );

						// copy transaction id xor fra
						a8OTABuff[CL_OTA_DOWNLOAD_FRAGMENT_CRC_XOR_TRID_OFF + pOta->u32TrailerLen] = (clu8)(u16Crc >>8 );
						a8OTABuff[CL_OTA_DOWNLOAD_FRAGMENT_CRC_XOR_TRID_OFF + 1 + pOta->u32TrailerLen] = (clu8)u16Crc;

						// len to send to encryption dongle
						u32DataLen2Send	=	pOta->u32TrailerLen + u32OTACopyLen + pOta->u32EndLen + CL_OTA_DOWNLOAD_FRAGMENT_DATA_BYTES_OFF;
						// reader to send this command to
						pReader2Send	=	pOta->pEncReader;

						// increment index of data to send
						pOta->u32BinaryIndex += u32DataLen2Send;
					}
					else
					{
						// continue previous session indicator reset as we entered the GET MISSING FRAME state
						pOta->bOTAContinuePrvSession = CL_ERROR;


						// Prepare the command now

						// Command id
						a8OTABuff[ CL_OTA_GET_MISSING_FRAGMENT_CMD_OFF + pOta->u32TrailerLen ]	= CL_OTA_GET_MISSING_FRAGMENT;

						// transaction id
						a8OTABuff[ CL_OTA_GET_MISSING_FRAGMENT_TRANSACTION_ID_OFF + pOta->u32TrailerLen ]	= pOta->u32CurrentPacket;

						// Transaction Id
						if ( CL_FAILED ( status = cl_GetParams( (clu8*)"ota_transaction_id", strlen("ota_transaction_id"), &pValue, &u32ValueLen, CL_OK ) ) )
							break;
						cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &a8OTABuff[ CL_OTA_GET_MISSING_FRAGMENT_TRANSACTION_ID_OFF + pOta->u32TrailerLen ], &u32TempLen );
						if ( u32TempLen != 2)
						{
							pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
							status = CL_ERROR;
							break;
						}
						cl_FreeMem( pValue );

						// len to send to encryption dongle
						u32DataLen2Send	=	pOta->u32TrailerLen + (CL_OTA_GET_MISSING_FRAGMENT_TRANSACTION_ID_OFF + 1) + pOta->u32EndLen + 1;
						// reader to send this command to
						pReader2Send	=	pOta->pEncReader;
					}
					// if we reached last packet sent, then update index of next command to received
					if ( pOta->u32BinaryIndex == (pOta->u32BinaryLen - 1)	)
						pOta->u8NextCommand	=	CL_OTA_GET_MISSING_FRAGMENT;
					else
						pOta->u8CurrentCommand =	CL_OTA_DOWNLOAD_FRAGMENT;

					// from now, allocate memory for tuple/buffer to send command

					status = CL_OK;
					break;
				}
				case (	CL_OTA_ENTER_DOWN_STATUS_GENERIC_ERROR	):	/// (bad frame length and others inconsistencies)
				{
					DEBUG_PRINTF("OTA Service Error: ENTER DOWNLOAD MODE GENERIC ERROR \n");
					pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
					status = CL_ERROR;
					break;
				}
				case ( 	CL_OTA_ENTER_DOWN_STATUS_PASS_ERROR ):	/// (init password is different to the one received here)
				{
					DEBUG_PRINTF("OTA Service Error: ENTER DOWNLOAD MODE PASSWORD ERROR!\n");
					pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
					status = CL_ERROR;
					break;
				}
				case ( CL_OTA_ENTER_DOWN_STATUS_FRAG_SIZE_ERROR ):	/// (init fragment size was different)
				{
					DEBUG_PRINTF("OTA Service Error: ENTER DOWNLOAD MODE FRAGMENT SIZE ERROR\n");
					pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
					status = CL_ERROR;
					break;
				}
				case ( CL_OTA_ENTER_DOWN_STATUS_NO_VALID_DWLD_SES ): /// (the download session number isn’t supported)
				{
					DEBUG_PRINTF("OTA Service Error: ENTER DOWNLOAD MODE NO VALID DOWNLOAD SESSION\n");
					pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
					status = CL_ERROR;
					break;
				}
				case ( CL_OTA_ENTER_DOWN_STATUS_FW_NUM_ERROR ):		/// (bad current firmware number)
				{
					DEBUG_PRINTF("OTA Service Error: ENTER DOWNLOAD MODE FIRMWARE NUMBER ERROR\n");
					pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
					status = CL_ERROR;
					break;
				}
				case (CL_OTA_ENTER_DOWN_STATUS_FW_RANGE_VERSION_ERROR ):	/// (product not concerned by the upgrade regarding its current version value)
				{
					DEBUG_PRINTF("OTA Service Error: ENTER DOWNLOAD FIRMWARE RANGE VERSION ERROR\n");
					pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
					status = CL_ERROR;
					break;
				}
				case ( CL_OTA_ENTER_DOWN_STATUS_NEW_FW_VERSION_ERROR ): /// (the new firmware version value isn’t highest in comparison to the current version value)
				{
					DEBUG_PRINTF("OTA Service Error: ENTER DOWNLOAD MODE NEW FIRMWARE VERSION ERROR\n");
					pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
					status = CL_ERROR;
					break;
				}
				default:
				{
					DEBUG_PRINTF("OTA : ENTER DOWNLOAD MODE: ERROR Unknown command\n");
					pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
					status = CL_ERROR;
					break;
				}
			}

			break;
		}
		case ( CL_OTA_FORMAT_EXTERNAL_MEMORY_WITH_NODE_CURRENT_FW ):
		{
			DEBUG_PRINTF("OTA ERROR: Received unexpected !!!! CL_OTA_FORMAT_EXTERNAL_MEMORY_WITH_NODE_CURRENT_FW\n");
			pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
			status = CL_ERROR;
			break;
		}
		case ( CL_OTA_DOWNLOAD_FRAGMENT ):
		{
			// continue previous session indicator reset as we entered the GET MISSING FRAME state
			pOta->bOTAContinuePrvSession = CL_ERROR;

			// check the return code
			switch ( tLocalBuf.pData[CL_OTA_DOWNLOAD_FRAGMENT_STATUS_OFF] )
			{
				case (	CL_OTA_DOWNLOAD_FRAGMENT_CMD_SUCCESS	): /// succeeded
				{
					DEBUG_PRINTF("OTA DOWNLOAD FRAGMENT SUCCESS\n");
					u32OTACopyLen	=	0;

					// ****** RETRY*******
					// some fragments were missed in previous session of OTA_DOWNLOAD_FRAG => retry
					if ( pOta->bOTAMissFrag	==	CL_ERROR	)
					{	// look for missed fragment
						bFound = CL_ERROR;
						//check if any missing flags
						for ( i=0; i < sizeof( pOta->aOTAMissingFrag); i++ )
						{
							if ( pOta->aOTAMissingFrag[i] != 0)
							{
								for ( j=0; j<8; j++)
								{
									// check bit 7 of the first byte is fragment 1
									if ( !((pOta->aOTAMissingFrag[i]>>(7-j))&0x01) )
										continue;
									else
									{
										// reset the flag to send
										pOta->aOTAMissingFrag[i] = pOta->aOTAMissingFrag[i] & ( ~(0x01<<(7-j)) );
										// indicates we found a missing fragment
										bFound = CL_OK;
										// compute fragment value
										u32MissFragId	=	i*8+j*8 + 1;
										break;
									}
								}
							}
						}
						if ( CL_SUCCESS( bFound ) )
						{
							// modify index for copy in case of DOWN_FRAGMENT
							pOta->u32BinaryIndex	=	(u32MissFragId - 1)*CL_OTA_DOWN_FRAGMENT_SIZE;
							pOta->u32CurrentPacket	=	u32MissFragId;

							// compute data length to copy
							if ( ( pOta->u32BinaryLen - pOta->u32BinaryIndex + 1) > CL_OTA_DOWN_FRAGMENT_SIZE )
								u32OTACopyLen	=	CL_OTA_DOWN_FRAGMENT_SIZE;
							else
								u32OTACopyLen	=	pOta->u32BinaryLen - pOta->u32BinaryIndex + 1;
						}

					}
					// ******* NORMAL SEND ********
					// if we have some data to copy already, it missed that we are sending missing fragment.... following code
					//	shall not be executed as it corresponds to normal DOWNLOAD FRAGMENT process
					if ( u32OTACopyLen == 0)
					{
						// compute data length to copy
						if ( ( pOta->u32BinaryLen - pOta->u32BinaryIndex + 1) > CL_OTA_DOWN_FRAGMENT_SIZE )
							u32OTACopyLen	=	CL_OTA_DOWN_FRAGMENT_SIZE;
						else
							u32OTACopyLen	=	pOta->u32BinaryLen - pOta->u32BinaryIndex + 1;
					}

					// at this stage,
					// if not 0, then prepare command DOWNLOAD_FRAGMENT
					// else go to missing fragment

					if ( u32OTACopyLen )
					{
						// Command id
						a8OTABuff[ CL_OTA_DOWNLOAD_FRAGMENT_CMD_OFF + pOta->u32TrailerLen ]	= CL_OTA_DOWNLOAD_FRAGMENT;

						// frame_id
						a8OTABuff[ CL_OTA_DOWNLOAD_FRAGMENT_FRAME_ID_OFF + pOta->u32TrailerLen ]	= (clu8)((pOta->u32CurrentPacket>>8)&0x000000FF);
						a8OTABuff[ CL_OTA_DOWNLOAD_FRAGMENT_FRAME_ID_OFF + 1 + pOta->u32TrailerLen ]	= (clu8)(pOta->u32CurrentPacket&0x000000FF);
						pOta->u32CurrentPacket++;

						// copy data
						memcpy( &a8OTABuff[CL_OTA_DOWNLOAD_FRAGMENT_DATA_BYTES_OFF + pOta->u32TrailerLen], &pOta->pBinaryData[pOta->u32BinaryIndex], u32OTACopyLen );

						// get transaction ID
						if ( CL_FAILED ( status = cl_GetParams( (clu8*)"ota_transaction_id", strlen("ota_transaction_id"), &pValue, &u32ValueLen, CL_OK ) ) )
							break;
						if ( u32ValueLen>4)
							break;
						cl_HelperConvertASCIItoHex( pValue, u32ValueLen, au8TransId, &u32TempLen );
						if ( u32TempLen != 2)
						{
							pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
							status = CL_ERROR;
							break;
						}
						cl_FreeMem( pValue );

						// compute transaction id
						u16TransactionId =	((((clu16)au8TransId[0])<<8)&0xFF00);
						u16TransactionId |=	((((clu16)au8TransId[1]))&0x00FF);

						//	compute crc 16 xor transaction id
						cl_HelperOTAFrameCrc( &a8OTABuff[CL_OTA_DOWNLOAD_FRAGMENT_DATA_BYTES_OFF + pOta->u32TrailerLen], u32OTACopyLen, u16TransactionId, &u16Crc );

						// copy transaction id xor fra
						a8OTABuff[CL_OTA_DOWNLOAD_FRAGMENT_CRC_XOR_TRID_OFF + pOta->u32TrailerLen] = (clu8)(u16Crc >>8 );
						a8OTABuff[CL_OTA_DOWNLOAD_FRAGMENT_CRC_XOR_TRID_OFF + 1 + pOta->u32TrailerLen] = (clu8)u16Crc;

						// len to send to encryption dongle
						u32DataLen2Send	=	pOta->u32TrailerLen + u32OTACopyLen + pOta->u32EndLen ;
						// reader to send this command to
						pReader2Send	=	pOta->pEncReader;

						// increment index of data to send
						pOta->u32BinaryIndex += u32DataLen2Send;
					}
					else
					{
						// Command id
						a8OTABuff[ CL_OTA_GET_MISSING_FRAGMENT_CMD_OFF + pOta->u32TrailerLen ]	= CL_OTA_GET_MISSING_FRAGMENT;

						// transaction id
						a8OTABuff[ CL_OTA_GET_MISSING_FRAGMENT_TRANSACTION_ID_OFF + pOta->u32TrailerLen ]	= pOta->u32CurrentPacket;

						// Transaction Id
						if ( CL_FAILED ( status = cl_GetParams( (clu8*)"ota_transaction_id", strlen("ota_transaction_id"), &pValue, &u32ValueLen, CL_OK ) ) )
							break;
						cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &a8OTABuff[ CL_OTA_GET_MISSING_FRAGMENT_TRANSACTION_ID_OFF + pOta->u32TrailerLen ], &u32TempLen );
						if ( u32TempLen != 2)
						{
							pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
							status = CL_ERROR;
							break;
						}
						cl_FreeMem( pValue );

						// len to send to encryption dongle
						u32DataLen2Send	=	pOta->u32TrailerLen + (CL_OTA_GET_MISSING_FRAGMENT_TRANSACTION_ID_OFF + 1) + pOta->u32EndLen + 1;
						// reader to send this command to
						pReader2Send	=	pOta->pEncReader;
					}

					// if we reached last packet sent, then update index of next command to received
					if ( pOta->u32BinaryIndex == (pOta->u32BinaryLen - 1)	)
						pOta->u8NextCommand	=	CL_OTA_GET_MISSING_FRAGMENT;
					else
						pOta->u8CurrentCommand =	CL_OTA_DOWNLOAD_FRAGMENT;

					// from now, allocate memory for tuple/buffer to send command

					status = CL_OK;
					break;
				}
				case (	CL_OTA_ENTER_DOWN_STATUS_GENERIC_ERROR	):	/// (bad frame length and others inconsistencies)
				{
					DEBUG_PRINTF("OTA Service Error: ENTER DOWNLOAD MODE GENERIC ERROR \n");
					pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
					status = CL_ERROR;
					break;
				}
				case ( 	CL_OTA_ENTER_DOWN_STATUS_PASS_ERROR ):	/// (init password is different to the one received here)
				{
					DEBUG_PRINTF("OTA Service Error: ENTER DOWNLOAD MODE PASSWORD ERROR!\n");
					pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
					status = CL_ERROR;
					break;
				}
				case ( CL_OTA_ENTER_DOWN_STATUS_FRAG_SIZE_ERROR ):	/// (init fragment size was different)
				{
					DEBUG_PRINTF("OTA Service Error: ENTER DOWNLOAD MODE FRAGMENT SIZE ERROR\n");
					pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
					status = CL_ERROR;
					break;
				}
				case ( CL_OTA_ENTER_DOWN_STATUS_NO_VALID_DWLD_SES ): /// (the download session number isn’t supported)
				{
					DEBUG_PRINTF("OTA Service Error: ENTER DOWNLOAD MODE NO VALID DOWNLOAD SESSION\n");
					pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
					status = CL_ERROR;
					break;
				}
				case ( CL_OTA_ENTER_DOWN_STATUS_FW_NUM_ERROR ):		/// (bad current firmware number)
				{
					DEBUG_PRINTF("OTA Service Error: ENTER DOWNLOAD MODE FIRMWARE NUMBER ERROR\n");
					pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
					status = CL_ERROR;
					break;
				}
				case (CL_OTA_ENTER_DOWN_STATUS_FW_RANGE_VERSION_ERROR ):	/// (product not concerned by the upgrade regarding its current version value)
				{
					DEBUG_PRINTF("OTA Service Error: ENTER DOWNLOAD FIRMWARE RANGE VERSION ERROR\n");
					pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
					status = CL_ERROR;
					break;
				}
				case ( CL_OTA_ENTER_DOWN_STATUS_NEW_FW_VERSION_ERROR ): /// (the new firmware version value isn t highest in comparison to the current version value)
				{
					DEBUG_PRINTF("OTA Service Error: ENTER DOWNLOAD MODE NEW FIRMWARE VERSION ERROR\n");
					pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
					status = CL_ERROR;
					break;
				}
				default:
				{
					DEBUG_PRINTF("OTA : ENTER DOWNLOAD MODE: ERROR Unknown command\n");
					pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
					status = CL_ERROR;
					break;
				}
			}

			break;
		}
		case ( CL_OTA_GET_MISSING_FRAGMENT ):
		{
			// check the return code

			DEBUG_PRINTF("OTA GET MISSING FRAGMENT status is %x\n", tLocalBuf.pData[CL_OTA_GENERIC_STATUS_OFFSET]);
			if ( ( tLocalBuf.pData[CL_OTA_GENERIC_STATUS_OFFSET] & 0x01) == 0x01 )
			{
				DEBUG_PRINTF("OTA GET MISSING FRAGMENT: Session opened by Application Flag\n");
			}
			if ( ( tLocalBuf.pData[CL_OTA_GENERIC_STATUS_OFFSET] & 0x02) == 0x02 )
			{
				DEBUG_PRINTF("OTA GET MISSING FRAGMENT: Session opened by Radio Frequency Flag\n");
			}
			if ( ( tLocalBuf.pData[CL_OTA_GENERIC_STATUS_OFFSET] & 0x04) == 0x04 )
			{
				DEBUG_PRINTF("OTA GET MISSING FRAGMENT: Session firmware not dedicated to this node flag\n");
			}
			if ( ( tLocalBuf.pData[CL_OTA_GENERIC_STATUS_OFFSET] & 0x08) == 0x08 )
			{
				DEBUG_PRINTF("OTA GET MISSING FRAGMENT: Session my_firmware_to_forward flag\n");
			}
			if ( ( tLocalBuf.pData[CL_OTA_GENERIC_STATUS_OFFSET] & 0x10) == 0x10 )
			{
				DEBUG_PRINTF("OTA GET MISSING FRAGMENT: Session Forward-firwmare_launched_by_RF Flag\n");
			}

			if ( ( tLocalBuf.pData[CL_OTA_GENERIC_STATUS_OFFSET] & 0xE0) != 0x0 )
			{
				DEBUG_PRINTF("OTA GET MISSING FRAGMENT:Forwarding fragment session number of tries %d\n", (( tLocalBuf.pData[CL_OTA_GENERIC_STATUS_OFFSET] & 0xE0)>>5));
			}

			if ( (tLocalBuf.ulLen - ( CL_OTA_GET_MISSING_FRAGMENT_FRAGMENT_FLAGS_OFF + 1)) != ( sizeof(pOta->aOTAMissingFrag) ) )
			{
				DEBUG_PRINTF("OTA GET MISSING FRAGMENT: Error on received Len. Expected %d != %d Received\n", sizeof(pOta->aOTAMissingFrag) , (tLocalBuf.ulLen - ( CL_OTA_GET_MISSING_FRAGMENT_FRAGMENT_FLAGS_OFF + 1)));
				pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
				status = CL_ERROR;
				break;
			}
			else	// now...
			//	1. save missing fragments flags..
			//	2. if any : send the first missing fragment
			//   	if none : issue the FLASH_EXT_FIRMWARE
			{
				// 1. save missing fragment flags
				memcpy( pOta->aOTAMissingFrag, tLocalBuf.pData, sizeof( pOta->aOTAMissingFrag) );

				bFound = CL_ERROR;
				//check if any missing flags
				for ( i=0; i < sizeof( pOta->aOTAMissingFrag); i++ )
				{
					if ( pOta->aOTAMissingFrag[i] != 0)
					{
						for ( j=0; j<8; j++)
						{
							// check bit 7 of the first byte is fragment 1
							if ( !((pOta->aOTAMissingFrag[i]>>(7-j))&0x01) )
								continue;
							else
							{
								// reset the flag to send
								pOta->aOTAMissingFrag[i] = pOta->aOTAMissingFrag[i] & ( ~(0x01<<(7-j)) );
								// indicates we found a missing fragment
								bFound = CL_OK;
								// compute fragment value
								u32MissFragId	=	i*8+j*8 + 1;
								break;
							}
						}
					}
				}

				// generate either a flash firmware or reenter download fragment
				if ( CL_SUCCESS( bFound) )
				{
					if ( pOta->u8OTAMissFragRetry != 0 )
					{
						// modify index for copy in case of DOWN_FRAGMENT
						pOta->u32BinaryIndex	=	(u32MissFragId - 1)*CL_OTA_DOWN_FRAGMENT_SIZE;
						pOta->u32CurrentPacket	=	u32MissFragId;

						// compute data length to copy
						if ( ( pOta->u32BinaryLen - pOta->u32BinaryIndex + 1) > CL_OTA_DOWN_FRAGMENT_SIZE )
							u32OTACopyLen	=	CL_OTA_DOWN_FRAGMENT_SIZE;
						else
							u32OTACopyLen	=	pOta->u32BinaryLen - pOta->u32BinaryIndex + 1;
						// if not 0, then prepare command DOWNLOAD_FRAGMENT
						// else go to missing fragment

						if ( u32OTACopyLen )
						{
							// Command id
							a8OTABuff[ CL_OTA_DOWNLOAD_FRAGMENT_CMD_OFF + pOta->u32TrailerLen ]	= CL_OTA_DOWNLOAD_FRAGMENT;

							// frame_id
							a8OTABuff[ CL_OTA_DOWNLOAD_FRAGMENT_FRAME_ID_OFF + pOta->u32TrailerLen ]	= (clu8)((pOta->u32CurrentPacket>>8)&0x000000FF);
							a8OTABuff[ CL_OTA_DOWNLOAD_FRAGMENT_FRAME_ID_OFF + 1 + pOta->u32TrailerLen ]	= (clu8)(pOta->u32CurrentPacket&0x000000FF);
							pOta->u32CurrentPacket++;

							// copy data
							memcpy( &a8OTABuff[CL_OTA_DOWNLOAD_FRAGMENT_DATA_BYTES_OFF + pOta->u32TrailerLen], &pOta->pBinaryData[pOta->u32BinaryIndex], u32OTACopyLen );

							// get transaction ID
							if ( CL_FAILED ( status = cl_GetParams( (clu8*)"ota_transaction_id", strlen("ota_transaction_id"), &pValue, &u32ValueLen, CL_OK ) ) )
								break;
							if ( u32ValueLen>4)
								break;
							cl_HelperConvertASCIItoHex( pValue, u32ValueLen, au8TransId, &u32TempLen );
							if ( u32TempLen != 2)
							{
								pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
								status = CL_ERROR;
								break;
							}
							cl_FreeMem( pValue );

							// compute transaction id
							u16TransactionId =	((((clu16)au8TransId[0])<<8)&0xFF00);
							u16TransactionId |=	((((clu16)au8TransId[1]))&0x00FF);

							//	compute crc 16 xor transaction id
							cl_HelperOTAFrameCrc( &a8OTABuff[CL_OTA_DOWNLOAD_FRAGMENT_DATA_BYTES_OFF + pOta->u32TrailerLen], u32OTACopyLen, u16TransactionId, &u16Crc );

							// copy transaction id xor fra
							a8OTABuff[CL_OTA_DOWNLOAD_FRAGMENT_CRC_XOR_TRID_OFF + pOta->u32TrailerLen] = (clu8)(u16Crc >>8 );
							a8OTABuff[CL_OTA_DOWNLOAD_FRAGMENT_CRC_XOR_TRID_OFF + 1 + pOta->u32TrailerLen] = (clu8)u16Crc;

							// len to send to encryption dongle
							u32DataLen2Send	=	pOta->u32TrailerLen + u32OTACopyLen + pOta->u32EndLen ;
							// reader to send this command to
							pReader2Send	=	pOta->pEncReader;

							// increment index of data to send
							pOta->u32BinaryIndex += u32DataLen2Send;

							pOta->bOTAMissFrag		=	CL_ERROR;						// some fragments were missed => retry
							pOta->u8OTAMissFragRetry--;									// decrement the number of retry
						}
					}
					else	// Abort OTA and signal to upper layer
					{
						pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
						status = CL_ERROR;
						break;
					}
				}
				else	// generate a FLASH_EXTERNAL_FW command if required or CL_OTA_FORWARD_EXTERNAL_FW
				{

					// FLASH_EXTERNAL_FW command
					if ( ( pOta->u8FirmwareDestination	==	CL_OTA_ENTER_DOWN_SESSION_APP_FIRMWARE_FOR_ME ) | ( pOta->u8FirmwareDestination	==	CL_OTA_ENTER_DOWN_SESSION_RESCUE_FIRMWARE_FOR_ME ) | ( pOta->u8FirmwareDestination	==	CL_OTA_ENTER_DOWN_SESSION_BOOTLOADER_FIRMWARE_FOR_ME ) )
					{
						// Command id
						a8OTABuff[ CL_OTA_GENERIC_CMD_OFFSET + pOta->u32TrailerLen ]	= CL_OTA_FLASH_EXTERNAL_FW;
						// len to send to encryption dongle
						u32DataLen2Send	=	pOta->u32TrailerLen + 1 + pOta->u32EndLen ;
						// reader to send this command to
						pReader2Send	=	pOta->pEncReader;
					}
					else	// CL_OTA_FORWARD_EXTERNAL_FW command
					{
						//**** Command id
						a8OTABuff[ CL_OTA_FORWARD_EXT_FW_CMD_OFF + pOta->u32TrailerLen ]	= CL_OTA_FORWARD_EXTERNAL_FW;

						//*** radio mode
						if ( CL_FAILED ( status = cl_GetParams( (clu8*)"to_forward_to_rf_mode", strlen("to_forward_to_rf_mode"), &pValue, &u32ValueLen, CL_OK ) ) )
							break;

			            cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &a8OTABuff[CL_OTA_FORWARD_EXT_FW_RADIO_MODE_OFF + pOta->u32TrailerLen ], &u32TempLen );
						cl_FreeMem( pValue );
			            if ( u32TempLen != 1)
						{
			            	pValue = CL_NULL;
							status = CL_ERROR;
			                break;
						}

						//*** radio channel
						if ( CL_FAILED ( status = cl_GetParams( (clu8*)"tx_channel_to_forward_to", strlen("tx_channel_to_forward_to"), &pValue, &u32ValueLen, CL_OK ) ) )
							break;

			            cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &a8OTABuff[CL_OTA_FORWARD_EXT_FW_RADIO_CHANNEL_OFF + pOta->u32TrailerLen ], &u32TempLen );
						cl_FreeMem( pValue );
			            if ( u32TempLen != 1)
						{
			            	pValue = CL_NULL;
							status = CL_ERROR;
			                break;
						}

			            //**** rf com type
						if ( CL_FAILED ( status = cl_GetParams( (clu8*)"ota_forward_addressing_type", strlen("ota_forward_addressing_type"), &pValue, &u32ValueLen, CL_OK ) ) )
							break;

			            cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &a8OTABuff[CL_OTA_FORWARD_EXT_FW_RF_COM_TYPE_OFF + pOta->u32TrailerLen ], &u32TempLen );
						cl_FreeMem( pValue );
			            if ( u32TempLen != 1)
						{
			            	pValue = CL_NULL;
							status = CL_ERROR;
			                break;
						}

			            //**** destination address
						if ( CL_FAILED ( status = cl_GetParams( (clu8*)"destination_address_to_forward_to", strlen("destination_address_to_forward_to"), &pValue, &u32ValueLen, CL_OK ) ) )
							break;

			            cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &a8OTABuff[CL_OTA_FORWARD_EXT_FW_DEST_ADDR_OFF + pOta->u32TrailerLen ], &u32TempLen );
						cl_FreeMem( pValue );
			            if ( u32TempLen != 6)
						{
			            	pValue = CL_NULL;
							status = CL_ERROR;
			                break;
						}

			            //**** ota password
						if ( CL_FAILED ( status = cl_GetParams( (clu8*)"ota_password", strlen("ota_password"), &pValue, &u32ValueLen, CL_OK ) ) )
							break;

			            cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &a8OTABuff[CL_OTA_FORWARD_EXT_FW_PASSWORD_OFF + pOta->u32TrailerLen ], &u32TempLen );
						cl_FreeMem( pValue );
			            if ( u32TempLen != 8)
						{
			            	pValue = CL_NULL;
							status = CL_ERROR;
			                break;
						}

			            //**** start firmware version
						if ( CL_FAILED ( status = cl_GetParams( (clu8*)"device_current_version_start_range", strlen("device_current_version_start_range"), &pValue, &u32ValueLen, CL_OK ) ) )
							break;

			            cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &a8OTABuff[CL_OTA_FORWARD_EXT_FW_START_FW_VERS_OFF + pOta->u32TrailerLen ], &u32TempLen );
						cl_FreeMem( pValue );
			            if ( u32TempLen != 4)
						{
			            	pValue = CL_NULL;
							status = CL_ERROR;
			                break;
						}

			            //**** stop firmware version
						if ( CL_FAILED ( status = cl_GetParams( (clu8*)"device_current_version_end_range", strlen("device_current_version_end_range"), &pValue, &u32ValueLen, CL_OK ) ) )
							break;

			            cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &a8OTABuff[CL_OTA_FORWARD_EXT_FW_STOP_FW_VERS_OFF + pOta->u32TrailerLen ], &u32TempLen );
						cl_FreeMem( pValue );
			            if ( u32TempLen != 4)
						{
			            	pValue = CL_NULL;
							status = CL_ERROR;
			                break;
						}

			            //**** transaction id
						if ( CL_FAILED ( status = cl_GetParams( (clu8*)"ota_transaction_id", strlen("ota_transaction_id"), &pValue, &u32ValueLen, CL_OK ) ) )
							break;

			            cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &a8OTABuff[CL_OTA_FORWARD_EXT_FW_TRANS_ID_OFF + pOta->u32TrailerLen ], &u32TempLen );
						cl_FreeMem( pValue );
			            if ( u32TempLen != 4)
						{
			            	pValue = CL_NULL;
							status = CL_ERROR;
			                break;
						}

						// len to send to encryption dongle
						u32DataLen2Send	=	pOta->u32TrailerLen + 28 + pOta->u32EndLen ;
						// reader to send this command to
						pReader2Send	=	pOta->pEncReader;

					}
					status = CL_OK;
				}


			}
			break;
		}
		case ( CL_OTA_FLASH_EXTERNAL_FW):
		{
			if ( ( tLocalBuf.pData[CL_OTA_GENERIC_STATUS_OFFSET] ) == CL_OTA_FLASH_EXT_FW_SUCCESS )
			{
				// lock callback access before pushing to user land
				if ( CL_SUCCESS( pCtxt->ptHalFuncs->fnSemaphoreWait( pCtxt->tClbCtrl.pSgl, 100  ) ) )
				{
					pCtxt->ptCallbacks->fnOTAProgress_cb( CL_OTA_FLASH_EXTERNAL_FW, pOta->pTargReader, 100 );
					pCtxt->ptHalFuncs->fnSemaphoreRelease( pCtxt->tClbCtrl.pSgl );
				}
				else
				{
					DEBUG_PRINTF("Failed to lock callback sync semaphore 2 \n");
				}

				pOta->u8CurrentCommand	=	CL_OTA_FLASH_EXTERNAL_FW;
			}
			else
			{
				pOta->u8CurrentCommand	=	CL_OTA_UNKNOWN_COMMAND;
				// lock callback access before pushing to user land
				if ( CL_SUCCESS( pCtxt->ptHalFuncs->fnSemaphoreWait( pCtxt->tClbCtrl.pSgl, 100  ) ) )
				{
					pCtxt->ptCallbacks->fnOTAProgress_cb( CL_OTA_FLASH_EXTERNAL_FW, pOta->pTargReader, 99 );
					pCtxt->ptHalFuncs->fnSemaphoreRelease( pCtxt->tClbCtrl.pSgl );
				}
				else
				{
					DEBUG_PRINTF("Failed to lock callback sync semaphore 3\n");
				}
			}
			// exit from this
			status = CL_ERROR;
			break;
		}
		case ( CL_OTA_FORWARD_EXTERNAL_FW ):
		{
			status = CL_OK;
			break;
		}
		case ( CL_OTA_REBOOT_ON_RESCUE_FW):
		{
			status = CL_OK;
			break;
		}
		case ( CL_OTA_GET_FW_NUMBERS_AND_VERSIONS ):
		{
			status = CL_OK;
			break;
		}
		case ( CL_OTA_UNKNOWN_COMMAND):
		default:
		{
			status = CL_ERROR;
			break;
		}
	}

	// from now, allocate tuple/buffer/memroy to send to device
	if ( CL_SUCCESS( status ) )
	{
		do
		{
			// first allocate tuple
			//----------------
			// allocate memory for Tuple
			status = csl_malloc( ( clvoid **)&pTuple2Send, sizeof( t_Tuple ) );
			if ( CL_FAILED( status ))
				break;

			if ( !pTuple2Send )
				break;

			// allocate memory for data to send
			status = csl_malloc( ( clvoid **)&pData2Send, u32DataLen2Send );
			if ( CL_FAILED( status ))
				break;

			if ( !pData2Send )
				break;

			// copy data
			memcpy( pData2Send, a8OTABuff, u32DataLen2Send );

			// initialize a tuple default flags with memory
			if ( CL_FAILED( status = cl_initTuple( pTuple2Send, CL_NULL, &pData2Send, u32DataLen2Send) ) )
				break;

			// add time flag if any
			// before sending to network, tag data with time stamp
			pCtxt->ptHalFuncs->fnGetTime( pTuple2Send->cl8Time, sizeof( pTuple2Send->cl8Time) );

			// specify that this tuple is reserved for OTA needs
			pTuple2Send->tOptions.tCnct = TSP_LAYER_REQ_OTA_SVC;
			if ( CL_FAILED( cl_sendData( CL_NULL, pReader2Send, pTuple2Send, NON_BLOCKING, &u32TsfNb ) ) )
				break;

			// signal a progress in the OTA process with 5% progress
			if ( pCtxt->ptCallbacks->fnOTAProgress_cb )
			{
				pOta->u8OtaProgress += 5;
				// lock callback access before pushing to user land
				if ( CL_SUCCESS( pCtxt->ptHalFuncs->fnSemaphoreWait( pCtxt->tClbCtrl.pSgl, 100  ) ) )
				{
					pCtxt->ptCallbacks->fnOTAProgress_cb( CL_OTA_ENTER_DOWNLOAD_MODE, pReader2Send, pOta->u8OtaProgress );
					pCtxt->ptHalFuncs->fnSemaphoreRelease( pCtxt->tClbCtrl.pSgl );
				}
				else
				{
					DEBUG_PRINTF("Failed to lock callback sync semaphore 4\n");
				}

			}
			break;
		}while ( 1 );
	}

	if ( CL_FAILED( status ) )
	{

		if (pOta)
		{
			// free memory of file
			cl_FreeMem( pOta->pBinaryData );
			pOta->pBinaryData	=	CL_NULL;
			pOta->u32BinaryLen	=	0;

			// restore readers state
//			if ( pOta->pEncReader)
//				cl_ReaderSetState( pOta->pEncReader, pOta->eEncReaderState );
//			pOta->pEncReader	=	CL_NULL;

//			if ( pOta->pTargReader )
//				cl_ReaderSetState( pOta->pTargReader, pOta->eTargReaderState );
//			pOta->pTargReader	=	CL_NULL;

//			if ( pOta->pTargReader)
//				pOta->pTargReader	=	CL_NULL;

//			if ( pOta->pEncReader)
//				pOta->pEncReader	=	CL_NULL;

			pOta->u32CurrentPacket	=	1;							// this is used for Frame_Id
			pOta->u32BinaryIndex	=	0;							// this is the current index in the binary. for now, nothing is sent... so 0
			pOta->bOTAMissFrag		=	CL_OK;						// reinitialize values.
			pOta->pEncReader		=	CL_NULL;
			pOta->pTargReader		=	CL_NULL;
			pOta->u8OTAMissFragRetry	=	CL_OTA_MISSING_FRAGS_MAX_RETRYS;

			if ( pOta->u8CurrentCommand	==	CL_OTA_UNKNOWN_COMMAND )	// if an error happened we signal it with 0xFF progress bar indicator
			{
				// lock callback access before pushing to user land
				if ( CL_SUCCESS( pCtxt->ptHalFuncs->fnSemaphoreWait( pCtxt->tClbCtrl.pSgl, 100  ) ) )
				{
					// if we have found missed fragment and we are already exceeded the retries.... => signal error on completion of OTA
					pCtxt->ptCallbacks->fnOTAProgress_cb( CL_OTA_GET_MISSING_FRAGMENT, pReader2Send, 0xFF );
					pCtxt->ptHalFuncs->fnSemaphoreRelease( pCtxt->tClbCtrl.pSgl );
				}
				else
				{
					DEBUG_PRINTF("Failed to lock callback sync semaphore 5\n");
				}
			}
		}
		//
	}

	return ( eStatus );
}

/*****************************************************************************/
/* Name : e_Result cl_CryptoThread( void )						           	*/
/* Description :                                                         	 */
/*        thread which handles OTA service in CSL 							*/
/*****************************************************************************/
/* Parameters:                                                            	*/
/*  --------------                                                        	*/
/*  Out: 																	*/
/* Return value: e_Result                                                 	*/
/*  CL_OK                        :  Result is OK                            */
/*  CL_ERROR,                    : Failure on execution or list of readers 	*/
/*												is empty                    */
/*  CL_MEM_ERR,                  :  Failure on memory management (failure,  */
/*                                  allocation ....)                      	*/
/*  CL_PARAMS_ERR,               :  Inconsistent parameters                 */
/**************************************************************************/
e_Result cl_CryptoThread( clvoid )
{
	t_clContext 	*pCtxt = CL_NULL;
	e_Result 		status = CL_ERROR;
	e_State			eState;
	clu8			u8DestroyThread	=	0;
	t_Reader 		*pEncReader 		= 	CL_NULL;
	t_Reader 		*pTargReader 		= 	CL_NULL;
	e_State			eEncReaderState 	= 	STATE_DEFAULT;
	e_State 		eTargReaderState	=	STATE_DEFAULT;
	t_Tuple 		*ptTupleEnterDownloadMode = CL_NULL;
	clu8			*pEnterDownloadMode	=	CL_NULL;
	clu8			*pValue				= 	CL_NULL;
	clu32			u32ValueLen			=	0;
	clvoid 			*pFileId 		=	CL_NULL;
    clu8 			*pu8Line 			= CL_NULL;
    clu32 			u32LineLen			=	0;
    clu32			u32FileLen			=	0;
    clu32			u32LocalLineLen		=	0;
    clu8			aucTempLen[3];
    clu8			*pData				=	CL_NULL;
    clu32 			u32MaxTotalLen		=	0;
    clu32			u32Index			=	0;
    clu32 			u32Crc				=	0;
    clu32			u32Carry			=	0;
    clu32			i					=	0;
    clu32			j					=	0;
    clu32			u32TsfNb			=	0;
    clu8			aTempArray[512];
    clu32			u32TempLen			=	0;
    clu8			u8TempValue			=	0;
    clu8			*pCmdBuf			= 	CL_NULL;
    clu32			u32TrailerLen		=	0;
    t_OtaInternal 	*pOta				=	CL_NULL;
    clu32			u32TempValue		=	0;
    clu32 			u32FragId			=	1;

    DEBUG_PRINTF("CryptoThread: BEGIN");

    // get pointer on OTA service
    if ( CL_FAILED( cl_getOTAService( &pOta ) ) )
    	return ( CL_ERROR );

    if ( pOta == CL_NULL )
    	return ( CL_ERROR );

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	// check global parameters
	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnSemaphoreWait == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnSemaphoreRelease == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnGetTime == CL_NULL )
		return ( CL_ERROR );

    if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL )
        return ( CL_ERROR );

	// at this stage of the thread, we are waiting from a message of the application to enter OTA service
	// on any error, we shall:
	//	- close the connection with the reader
	//  - remove it from the list of registered readers
	//  - signal it to upper layers
	do
	{
		if ( CL_FAILED(  pCtxt->ptHalFuncs->fnDestroyThreadAsked( &u8DestroyThread ) ) )
			break;

		if ( u8DestroyThread != 0 )	//if a destroy was asked .. then proceed :)
		{
			status = CL_ERROR;
			break;
		}

	    DEBUG_PRINTF("CryptoThread:WAIT");

	    // wait indefinitely for signal to this reader
		status = pCtxt->ptHalFuncs->fnSemaphoreWait( pCtxt->tCrypto.pSgl,  5000 );

		if (( status != CL_OK ) & (status != CL_TIMEOUT_ERR ))
			break;

		if ( CL_FAILED(  pCtxt->ptHalFuncs->fnDestroyThreadAsked( &u8DestroyThread ) ) )
			break;

		if ( u8DestroyThread != 0 )	//if a destroy was asked .. then proceed :)
		{
			status = CL_ERROR;
			break;
		}

		if ( status == CL_TIMEOUT_ERR )
			continue;

		do
		{
			if ( CL_FAILED(  pCtxt->ptHalFuncs->fnDestroyThreadAsked( &u8DestroyThread ) ) )
				break;

			if ( u8DestroyThread != 0 )	//if a destroy was asked .. then proceed :)
			{
				status = CL_ERROR;
				break;
			}

			// get params from application
			if ( ( pCtxt->tCryptoParams.ptEncryptionKeyReader == CL_NULL ) | ( pCtxt->tCryptoParams.ptTargetReader == CL_NULL ) )
				continue;

			if ( CL_FAILED( cl_readerFindInList( &pEncReader, pCtxt->tCryptoParams.ptEncryptionKeyReader ) ) )
				continue;

			if ( CL_FAILED( cl_readerFindInList( &pTargReader, pCtxt->tCryptoParams.ptTargetReader ) ) )
				continue;

			// release calling application
			if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnSemaphoreRelease( pCtxt->tCrypto.pSgl ) ))
				break;

			// remove retries
			pCtxt->tCryptoParams.ptEncryptionKeyReader 	=	CL_NULL;
			pCtxt->tCryptoParams.ptTargetReader			=	CL_NULL;

			// save previous state of reader for further restoration
			if ( CL_FAILED( cl_ReaderGetState( pEncReader, &eEncReaderState ) ) )
				break;

			if ( CL_FAILED( cl_ReaderGetState( pTargReader, &eTargReaderState ) ) )
				break;

			// change state of readers to prevent unwanted data from lower layer
			if ( CL_FAILED( cl_ReaderSetState( pEncReader, STATE_OTA ) ) )
				break;

			// change on the other reader if different from previous
			if ( pEncReader != pTargReader )
			{
				if ( CL_FAILED( cl_ReaderSetState( pTargReader, STATE_OTA ) ) )
					break;
			}

			// save previous state of readers to OTA service for further restoration
			pOta->eEncReaderState	=	eEncReaderState;
			pOta->eTargReaderState	=	eTargReaderState;
			pOta->pEncReader		=	pEncReader;
			pOta->pTargReader		=	pTargReader;
			pOta->u8CurrentCommand	=	CL_OTA_ENTER_DOWNLOAD_MODE;	//
			pOta->u8NextCommand		=	CL_OTA_ENTER_DOWNLOAD_MODE;	//
			pOta->u32CurrentPacket	=	1;							// this is used for Frame_Id
			pOta->u32BinaryIndex	=	0;							// this is the current index in the binary. for now, nothing is sent... so 0
			pOta->u8OtaProgress		=	0;
			pOta->bOTAMissFrag		=	CL_OK;						// nothing missed yet as nothing sent :)
			pOta->u8OTAMissFragRetry	= CL_OTA_MISSING_FRAGS_MAX_RETRYS;

			// allocate space to store the data to send to the reader
			if ( CL_FAILED( csl_malloc( &pEnterDownloadMode, CL_OTA_ENTER_DOWNLOAD_MODE_SIZE + pOta->u32TrailerLen + pOta->u32EndLen ) ) )
				break;

			if ( !pEnterDownloadMode )
				break;

			// copy trailer
			if ( pOta->pTrailerData )
				memcpy( pEnterDownloadMode, pOta->pTrailerData, pOta->u32TrailerLen );

            // copy end data
            if ( pOta->pEndData )
            	memcpy( pEnterDownloadMode[CL_OTA_ENTER_DOWNLOAD_MODE_SIZE + pOta->u32TrailerLen - 1], pOta->pEndData, pOta->u32EndLen);

			// fill enter download mode buffer
			// first command
			pEnterDownloadMode[CL_OTA_ENTER_DOWN_CMD_OFF + pOta->u32TrailerLen] = CL_OTA_ENTER_DOWNLOAD_MODE;

			//************************************
			// session type
			//************************************

			//-- session : B0-3
			if ( CL_FAILED ( status = cl_GetParams( (clu8*)"session_type", strlen("session_type"), &pValue, &u32ValueLen, CL_OK ) ) )
				break;

			cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &u8TempValue, &u32TempLen );
			cl_FreeMem( pValue );
			if ( u32TempLen != 1)
			{
				pValue = CL_NULL;
				status = CL_ERROR;
				break;
			}
			pEnterDownloadMode[CL_OTA_ENTER_DOWN_SESSION_TYPE_OFF + pOta->u32TrailerLen ] = u8TempValue & 0x0F;	// specify which session it is

			// save firmware destination for next use in the state machine (either issue a CL_OTA_FLASH_EXTERNAL_FW or CL_OTA_FORWARD_EXTERNAL_FW )
			pOta->u8FirmwareDestination	=	u8TempValue;

			// encrypted session or not??? B4
			if ( CL_FAILED ( status = cl_GetParams( (clu8*)"ota_activate_encryption", strlen("ota_activate_encryption"), &pValue, &u32ValueLen, CL_OK ) ) )
				break;

			cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &u8TempValue, &u32TempLen );
			cl_FreeMem( pValue );
			if ( u32TempLen != 1)
			{
				pValue = CL_NULL;
				status = CL_ERROR;
				break;
			}
			if ( u8TempValue != 0 )// non encrypted firmware session
				pEnterDownloadMode[CL_OTA_ENTER_DOWN_SESSION_TYPE_OFF + pOta->u32TrailerLen ] |= 0x10;	// encrypted firmware session


			//-- packing or not??? B5
			if ( CL_FAILED ( status = cl_GetParams( (clu8*)"ota_activate_packing", strlen("ota_activate_packing"), &pValue, &u32ValueLen, CL_OK ) ) )
				break;

			cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &u8TempValue, &u32TempLen );
			cl_FreeMem( pValue );
			if ( u32TempLen != 1)
			{
				pValue = CL_NULL;
				status = CL_ERROR;
				break;
			}
			if ( u8TempValue != 0 )	//unpacking required
				pEnterDownloadMode[CL_OTA_ENTER_DOWN_SESSION_TYPE_OFF + pOta->u32TrailerLen ] |= 0x20;	// unpacking required


			//-- allow downgrade or not??? B6
			if ( CL_FAILED ( status = cl_GetParams( (clu8*)"ota_allow_downgrade", strlen("ota_allow_downgrade"), &pValue, &u32ValueLen, CL_OK ) ) )
				break;

			cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &u8TempValue, &u32TempLen );
			cl_FreeMem( pValue );
			if ( u32TempLen != 1)
			{
				pValue = CL_NULL;
				status = CL_ERROR;
				break;
			}
			if ( u8TempValue != 0 )// downgrade allowed
				pEnterDownloadMode[CL_OTA_ENTER_DOWN_SESSION_TYPE_OFF + pOta->u32TrailerLen ] |= 0x40;	// downgrade allowed


			//-- continue previous session or not??? B7
			if ( CL_FAILED ( status = cl_GetParams( (clu8*)"ota_continue_previous_session", strlen("ota_continue_previous_session"), &pValue, &u32ValueLen, CL_OK ) ) )
				break;

			cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &u8TempValue, &u32TempLen );
			cl_FreeMem( pValue );
			if ( u32TempLen != 1)
			{
				pValue = CL_NULL;
				status = CL_ERROR;
				break;
			}
			if ( u8TempValue != 0 )// continue previous session required
			{
				pEnterDownloadMode[CL_OTA_ENTER_DOWN_SESSION_TYPE_OFF + pOta->u32TrailerLen ] |= 0x80;	// continue previous session required
				pOta->bOTAContinuePrvSession	=	CL_OK;
			}
			else
			{
				pOta->bOTAContinuePrvSession	=	CL_ERROR;
			}
			//***************
			// Password
			//***************
			if ( CL_FAILED ( status = cl_GetParams( (clu8*)"ota_password", strlen("ota_password"), &pValue, &u32ValueLen, CL_OK ) ) )
				break;

            cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &pEnterDownloadMode[CL_OTA_ENTER_DOWN_PASSWORD_OFF + pOta->u32TrailerLen ], &u32TempLen );
			cl_FreeMem( pValue );
            if ( u32TempLen != 8)
			{
            	pValue = CL_NULL;
				status = CL_ERROR;
                break;
			}

            //***************
			// Frag_dim	: FIXED SIZE
            //***************
			pEnterDownloadMode[CL_OTA_ENTER_DOWN_FRAG_DIM_OFF + pOta->u32TrailerLen] = ((clu8)(((clu16)CL_OTA_DOWN_FRAGMENT_SIZE) >>8));
			pEnterDownloadMode[CL_OTA_ENTER_DOWN_FRAG_DIM_OFF + 1 + pOta->u32TrailerLen] = ((clu8)(((clu16)CL_OTA_DOWN_FRAGMENT_SIZE)));

			//***************
			//firmware number
			//***************
			if ( CL_FAILED ( status = cl_GetParams( (clu8*)"target_device_firmware_number", strlen("target_device_firmware_number"), &pValue, &u32ValueLen, CL_OK ) ) )
				break;

			cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &pEnterDownloadMode[CL_OTA_ENTER_DOWN_FW_NUMB_OFF + pOta->u32TrailerLen], &u32TempLen );
			cl_FreeMem( pValue );
			if ( u32TempLen != 2)
			{
				pValue = CL_NULL;
				status = CL_ERROR;
				break;
			}

			//***************
			// start range current version
			//***************
			if ( CL_FAILED ( status = cl_GetParams( (clu8*)"device_current_version_start_range", strlen("device_current_version_start_range"), &pValue, &u32ValueLen, CL_OK ) ) )
				break;
			cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &pEnterDownloadMode[CL_OTA_ENTER_DOWN_START_FW_VERS_OFF + pOta->u32TrailerLen], &u32TempLen );
			cl_FreeMem( pValue );
			if ( u32TempLen != 4)
			{
				pValue = CL_NULL;
				status = CL_ERROR;
				break;
			}

			//***************
			// stop range current version
			//***************
			if ( CL_FAILED ( status = cl_GetParams( (clu8*)"device_current_version_end_range", strlen("device_current_version_end_range"), &pValue, &u32ValueLen, CL_OK ) ) )
				break;
			cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &pEnterDownloadMode[CL_OTA_ENTER_DOWN_STOP_FW_VERS_OFF + pOta->u32TrailerLen], &u32TempLen );
			cl_FreeMem( pValue );
			if ( u32TempLen != 4)
			{
				pValue = CL_NULL;
				status = CL_ERROR;
				break;
			}

			//***************
			// new firmware version
			//***************
			if ( CL_FAILED ( status = cl_GetParams( (clu8*)"new_firmware_version", strlen("new_firmware_version"), &pValue, &u32ValueLen, CL_OK ) ) )
				break;
			cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &pEnterDownloadMode[CL_OTA_ENTER_DOWN_NEW_FW_VERS_OFF + pOta->u32TrailerLen], &u32TempLen );
			cl_FreeMem( pValue );

			if ( u32TempLen != 4)
			{
				pValue = CL_NULL;
				status = CL_ERROR;
				break;
			}

			//***************
			// Transaction Id
			//***************
			if ( CL_FAILED ( status = cl_GetParams( (clu8*)"ota_transaction_id", strlen("ota_transaction_id"), &pValue, &u32ValueLen, CL_OK ) ) )
				break;
			cl_HelperConvertASCIItoHex( pValue, u32ValueLen, &pEnterDownloadMode[ CL_OTA_ENTER_DOWN_TRANS_ID_OFF + pOta->u32TrailerLen ], &u32TempLen );
			cl_FreeMem( pValue );

			if ( u32TempLen != 2)
			{
				pValue = CL_NULL;
				status = CL_ERROR;
				break;
			}

			//***************
			// firmware size
			//***************
			if ( CL_FAILED ( status = cl_GetParams( (clu8*)"encryption_file_name", strlen("encryption_file_name"), &pValue, &u32ValueLen, CL_OK ) ) )
				break;

			// save file name
            memset( aTempArray, 0, sizeof( aTempArray ) );
            memcpy( aTempArray, pValue, u32ValueLen);

			cl_FreeMem( pValue );

			if ( u32ValueLen == 0)
			{
				pValue = CL_NULL;
				status = CL_ERROR;
				break;
			}

			/* */
			if ( pCtxt->ptHalFuncs->fnFileOpen( aTempArray, CL_FILE_BINARY_MODE, &pFileId) != CL_OK ) // file on opening file
			{
				status = CL_ERROR;
				break;
			}
			// rewind file position to start of file
			pCtxt->ptHalFuncs->fnFileSetPos( pFileId, 0);

			// read length of file
			while ( pCtxt->ptHalFuncs->fnFileReadLn( pFileId, &pu8Line, &u32LineLen)  != CL_EOF_ERR )
			{
				// free memory;
				cl_FreeMem( pu8Line );
				pu8Line = CL_NULL;
			}
			// free memory
			if ( pu8Line )
				cl_FreeMem( pu8Line );

			if ( CL_FAILED ( status = pCtxt->ptHalFuncs->fnFileGetPos( pFileId, &u32MaxTotalLen ) ) )
			{
				if ( status != CL_EOF_ERR )
					break;
			}

			// allocate space to store the binary data
			if ( CL_FAILED( csl_malloc( &pData, u32MaxTotalLen ) ) )
				break;

			// rewind file position to start of file
			pCtxt->ptHalFuncs->fnFileSetPos( pFileId, 0);

			// set memory to zero
			memset( pData, 0, u32MaxTotalLen );

			// get length of data
			while ( CL_SUCCESS( pCtxt->ptHalFuncs->fnFileReadLn( pFileId, &pu8Line, &u32LineLen) ) )
			{
				// check if the line is a DATA record type (do not parse header or trailer blocks))
				if (!memcmp( pu8Line, "S3", strlen("S3")))
				{
                    // get line length
                    cl_HelperConvertASCIItoHex( &pu8Line[2], 2 ,  &aucTempLen[0], &u32LocalLineLen );	// perform global CRC32 cal
                    u32LineLen = aucTempLen[0] - 5; // 5 bytes are reserved for address, 1 byte for CRC in the line
                    u32LineLen *= 2; 	// *2 as we treat have nibble to convert

                    // get line data
					cl_HelperConvertASCIItoHex( &pu8Line[12], u32LineLen ,  &pData[u32Index], &u32LocalLineLen );	// perform global CRC32 cal
					u32Index += u32LocalLineLen;
					u32FileLen 		+= 	u32LocalLineLen;
				}
				cl_FreeMem( pu8Line );
				pu8Line = CL_NULL;
			};

			if ( pu8Line )
				cl_FreeMem( pu8Line );

			// all data sent to key are padded to 189 (max RF frame size)
			if ( u32FileLen % 189)
			{
				memset( &pData[u32Index], 0, 189- (u32FileLen % 189) );
				u32FileLen += 189- (u32FileLen % 189);
				u32Index = u32FileLen;
			}

			// save information of OTA file to transmit for further use and memory release
			pOta->pBinaryData 	=	pData;
			pOta->u32BinaryLen	=	u32FileLen;


			// rewind file position to start of file
			pCtxt->ptHalFuncs->fnFileSetPos( pFileId, 0);

			// copy length to buffer
			pEnterDownloadMode[ CL_OTA_ENTER_DOWN_FW_SIZE_OFF + pOta->u32TrailerLen]		=	( (clu8) ((u32FileLen>>24) & 0x000000FF) );
			pEnterDownloadMode[ CL_OTA_ENTER_DOWN_FW_SIZE_OFF + 1 + pOta->u32TrailerLen ]	=	( (clu8) ((u32FileLen>>16) & 0x000000FF) );
			pEnterDownloadMode[ CL_OTA_ENTER_DOWN_FW_SIZE_OFF + 2 + pOta->u32TrailerLen ]	=	( (clu8) ((u32FileLen>>8) & 0x000000FF) );
			pEnterDownloadMode[ CL_OTA_ENTER_DOWN_FW_SIZE_OFF + 3 + pOta->u32TrailerLen ]	=	( (clu8) ((u32FileLen) 	& 0x000000FF) );

			//***************
			// calculate global CRC on
			//***************
			u32FragId = 0;
			for ( i = 0; i < u32Index; i++ )
			{
				u32TempValue = pData[i];
				u32Crc ^= u32TempValue;

				for ( j = 0; j < 8; j++ )
				{
					u32Carry = (u32Crc & 1);
					u32Crc = (( u32Crc & 0xFFFFFFFE ) / 2) & 0x7FFFFFFF;
					if ( u32Carry != 0)
					{
						u32Crc = ( ( u32Crc ^ 0x04C11DB7) & 0xFFFFFFFF);
					}

				}
			}

			pEnterDownloadMode[ CL_OTA_ENTER_DOWN_GLOBAL_CRC32_OFF + pOta->u32TrailerLen]	=	( (clu8)((u32Crc >> 24) & 0x000000FF) );
			pEnterDownloadMode[ CL_OTA_ENTER_DOWN_GLOBAL_CRC32_OFF + 1 + pOta->u32TrailerLen]	=	( (clu8)((u32Crc >> 16) & 0x000000FF) );
			pEnterDownloadMode[ CL_OTA_ENTER_DOWN_GLOBAL_CRC32_OFF + 2 + pOta->u32TrailerLen]	=	( (clu8)((u32Crc >> 8) & 0x000000FF) );
			pEnterDownloadMode[ CL_OTA_ENTER_DOWN_GLOBAL_CRC32_OFF + 3 + pOta->u32TrailerLen]	=	( (clu8)((u32Crc ) & 0x000000FF) );

			//***************
			// Tuple send
			//***************
			//----------------
			// allocate memory for Tuple
			status = csl_pmalloc( ( clvoid **)&ptTupleEnterDownloadMode, sizeof( t_Tuple ) );
			if ( CL_FAILED( status ))
                break;

			if ( !ptTupleEnterDownloadMode )
				break;


			// initialize a tuple default flags with memory
			if ( CL_FAILED( status = cl_initTuple( ptTupleEnterDownloadMode, CL_NULL, &pEnterDownloadMode, CL_OTA_ENTER_DOWNLOAD_MODE_SIZE + pOta->u32TrailerLen) ) )
				break;

			// add time flag if any
			// before sending to network, tag data with time stamp
			pCtxt->ptHalFuncs->fnGetTime( ptTupleEnterDownloadMode->cl8Time, sizeof( ptTupleEnterDownloadMode->cl8Time) );


			// specify that this tuple is reserved for OTA needs
			ptTupleEnterDownloadMode->tOptions.tCnct = TSP_LAYER_REQ_OTA_SVC;
			if ( CL_FAILED( cl_sendData( CL_NULL, pEncReader, ptTupleEnterDownloadMode, NON_BLOCKING, &u32TsfNb ) ) )
				break;

			// signal a progress in the OTA process with 5% progress
			if ( pCtxt->ptCallbacks->fnOTAProgress_cb )
			{
				pOta->u8OtaProgress		+=	1;
				if ( CL_SUCCESS( pCtxt->ptHalFuncs->fnSemaphoreWait( pCtxt->tClbCtrl.pSgl, 100  ) ) )
				{
					pCtxt->ptCallbacks->fnOTAProgress_cb( CL_OTA_ENTER_DOWNLOAD_MODE, pEncReader, pOta->u8OtaProgress );
					pCtxt->ptHalFuncs->fnSemaphoreRelease( pCtxt->tClbCtrl.pSgl );
				}
				else
				{
					DEBUG_PRINTF("Failed to lock callback sync semaphore 6\n");
				}
			}

			break;

		}while ( 1 );

		/*
		// restore state to previous
		cl_ReaderSetState( pEncReader, eEncReaderState );

		// restore state to previous
		cl_ReaderSetState( pTargReader, eTargReaderState );
		 */
		// exit and clean
		if ( CL_FAILED( status ) )
		{
			if ( pValue )
			{
				cl_FreeMem( pValue );
				pValue	= 	CL_NULL;
			}

			if ( pFileId )
			{
				pCtxt->ptHalFuncs->fnFileClose( pFileId );
				pFileId	=	CL_NULL;
			}

			if ( pData )
			{
				cl_FreeMem( pData );
				pData = CL_NULL;
			}

			if ( pOta )
			{
				if ( pOta->pBinaryData )
					pOta->pBinaryData = CL_NULL;	// release of mem

				// set length of binary to NULL
				pOta->u32BinaryLen	=	0;
			}

			// signal an error to userland
			if ( CL_SUCCESS( pCtxt->ptHalFuncs->fnSemaphoreWait( pCtxt->tClbCtrl.pSgl, 100  ) ) )
			{
				pCtxt->ptCallbacks->fnOTAProgress_cb( CL_OTA_GET_MISSING_FRAGMENT, pEncReader, 0xFF );
				pCtxt->ptHalFuncs->fnSemaphoreRelease( pCtxt->tClbCtrl.pSgl );
			}
			else
			{
				DEBUG_PRINTF("Failed to lock callback sync semaphore 7\n");
			}


			// restore readers state
			if ( pEncReader)
				cl_ReaderSetState( pEncReader, pOta->eEncReaderState );

			if ( pTargReader )
				cl_ReaderSetState( pEncReader, pOta->eTargReaderState );

			if ( pOta->pTargReader)
				pOta->pTargReader	=	CL_NULL;

			if ( pOta->pEncReader)
				pOta->pEncReader	=	CL_NULL;

			pOta->u32CurrentPacket	=	1;							// this is used for Frame_Id
			pOta->u32BinaryIndex	=	0;							// this is the current index in the binary. for now, nothing is sent... so 0
		}

		//
		if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnSemaphoreRelease( pCtxt->tCrypto.pSgl ) ))
			break;
		// relieve thread if nothing is coming from upper layer
//        pCtxt->ptHalFuncs->fnWaitMs( 50000 );

	}
	while( 1 );

/*	if ( CL_FAILED( status ) )
	{
		DEBUG_PRINTF("Reader Write Thread closed due to pb\n");
		pCrtReader->tReaderHalFuncs.fnIOCloseConnection( pCrtReader );
		pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4Read );
	}


	status = clRegisteredThread( clReaderReadThread );
	pCtxt->ptHalFuncs->fnDestroyThread( CL_NULL );
*/
	return ( CL_OK );
}

