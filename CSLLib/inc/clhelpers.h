/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
#include "..\csl.h"

#ifndef CLHELPERS_H_
#define CLHELPERS_H_

// Over IP/Bluetooth/USB, an specific algorithm is done to allow to unfragment the data coming from the network as they
// can be concatenated by the reader
// protocol is defined as follow:
//	| 1 byte 	| 1 byte	|	1 byte 		| n bytes ......	| 2 bytes	| 1 byte	|
//	| SYNC(0xFF)| STX(0x02)	| LEN(nbytes)	| PAYLOAD			| CRC16		| ETX(0x03	|
#define CLOVER_SERIAL_PROTO_SYNC	0xFF 	// SYNC character to detect start of frame
#define CLOVER_SERIAL_PROTO_STX		0x02	// STX (Start of Transmission) to detect start of frame
#define CLOVER_SERIAL_PROTO_ETX		0x03	// ETX (End of Transmission) to detect end of frame
#define CLOVER_SERIAL_PROTO_SYNC_OFFSET	0	// SYNC is first byte
#define CLOVER_SERIAL_PROTO_STX_OFFSET	1	// STX is second byte
#define CLOVER_SERIAL_PROTO_LEN_OFFSET	2	// LENgth offset



//--------------
#define	CLOVER_SERIAL_ACK	0x06			// the Clover board was not accessed on the serial
#define	CLOVER_SERIAL_NACK	0x15			// the Clover board didn't acked the command on the serial (unrecognized command)
#define	IP_DISCONNECTION_EVENT	0x16		// A disconnection happened on the IP layer => we will receive commands that were stacked in the reader

#define CSL_PROTOCOL_SERIAL_HEADER_INDEX	0
#define CSL_PROTOCOL_SERIAL_FIELD_LEN_INDEX	2
#define CSL_PROTOCOL_SERIAL_DATA_INDEX		3


//------------------- defines used for discover of IP
// once a discover token is sent over network, Lantronix answer with 00 00 00 F9, then 4bytes of IP address
#define CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_0	0x00 	// SYNC character 0
#define CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_1	0x00 	// SYNC character 1
#define CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_2	0x00 	// SYNC character 2
#define CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_3	0xF9 	// SYNC character 3
#define CSL_DISCOVER_PROTOCOL_LANTRONIX_MIN_LEN			8 		// SYNC frame is at least 8bytes
#define CSL_DISCOVER_PROTOCOL_LANTRONIX_IP_ADDR_LEN		4 		// IPV4 address

#define CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_0_OFFSET	0x00 	// SYNC character 0
#define CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_1_OFFSET	0x01 	// SYNC character 1
#define CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_2_OFFSET	0x02 	// SYNC character 2
#define CSL_DISCOVER_PROTOCOL_LANTRONIX_START_BYTE_3_OFFSET	0x03 	// SYNC character 3
#define CSL_DISCOVER_PROTOCOL_LANTRONIX_IP_ADDR_OFFSET		0x04 	// SYNC character 4

/*--------------------------------------------------------------------------*/

void PrintBuffer(char *aString, unsigned char *aBuffer, int aLenght, char *aFormatString);

/*--------------------------------------------------------------------------*/

/*******************************************************************************************/
/** @fn e_Result clInit( t_clSys_HalFunctions *pSysContext
 \n                          CALSSys_CallBackFunctions_s *pCallBacks)
* @brief Initialize the library with function pointers and callbacks
* \n
*****************************************************************************************
*  @param *pSysContext 			: (In) function pointers for library
*      \n if NULL, no errors are returned but empty functions are implemented
*      \n per default
*      \n the library initialize locks/semaphores/network access in this call
*   @param *pCallBacks 			: (In) function pointers for
*      \n                  			callbacks signaled by the library
* @return e_Result
*  \n OK                        :  Result is OK
*  \n ERROR,                    : Failure on execution
*  \n MEM_ERR,                  :  Failure on memory management (failure,
*  \n                                allocation ....)
*  \n PARAMS_ERR,               :  Inconsistent parameters
******************************************************************************************* */


/*******************************************************************************************/
/* Name : e_Result cl_HelperNetworkToTuples( t_Reader *pReader,  clu8 *pData, cl32 cl32Len  */
/* , t_Tuple **pptTuple                                                                     */
/* Description :                                                                            */
/* parse the incoming buffer and prepare the tuple list for upper layer 					*/
/* the format of underlaying driver can concatenate several buffers together 				*/
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
e_Result cl_HelperNetworkToTuples( t_Reader *pReader, clu8 *pData, cl32 cl32Len, t_Tuple **ppTuple );

/*--------------------------------------------------------------------------*/

e_Result cl_NetworkToTuples( t_Reader *pReader, clu8 *pData, cl32 cl32Len, t_Tuple **ppTuple );

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
e_Result cl_HelperEncapsulateToSerial(  t_Reader *pReader, t_Buffer *ptBuff );

/*******************************************************************************************/
/* Name : e_Result cl_HelperSerialCrc( clu8 *pData, clu32 u32Len, clu16 *pu16Crc ) 			*/
/* Description :                                                                            */
/* compute CRC 16 on the data buffer and returns it											*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*		clu8 					*pData : buffer to parse 									*/
/*		clu32 					u32Len	: Len to parse										*/
/* ---------------                                                                          */
/*  Out: clu16				*pu16Crc 			// pointer on CRC							*/
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*	ERROR					  : generic error												*/
/*******************************************************************************************/
e_Result cl_HelperSerialCrc( clu8 *pData, clu32 u32Len, clu16 *pu16Crc );

/*******************************************************************************************/
/* Name : e_Result cl_HelperCheckEncapsulateSerial(  t_Reader *pReader, clu8 *pData			*/
/*	clu32 u32Len, clu8 *pStartPayloadIndex, clu8 *pStopPayloadIndex,  e_Result *pe_checkStatus)*/
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
e_Result cl_HelperCheckEncapsulateSerial(  t_Reader *pReader, clu8 *pData, clu32 u32Len, cl32 *pStartPayloadIndex, cl32 *pStopPayloadIndex,  e_Result *pe_checkStatus);

e_Result cl_CheckSerialFrameEncapsulation( t_Reader *pReader, clu8 *pData );

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
e_Result cl_HelperDiscoverNetworkToTuples( t_Reader *pReader, clu8 *pData, cl32 cl32Len, t_Tuple **pptTuple );

/*******************************************************************************************/
/* Name : e_Result cl_HelperOTAFrameCrc( clu8 *pData, clu32 u32Len, clu16 u16TransactionId, clu16 *pu16Crc ) 	*/
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
e_Result cl_HelperOTAFrameCrc( clu8 *pData, clu32 u32Len, clu16 u16TransactionId, clu16 *pu16Crc );

#endif /* CLHELPERS_H_ */
/**************************************************/
