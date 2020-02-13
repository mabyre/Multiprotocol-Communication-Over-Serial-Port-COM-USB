/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/

#ifndef OTASERVICE_H_
#define OTASERVICE_H_

#include "..\csl.h"
#include "..\clsupport.h"

// OTA: ENTER DOWNLOAD MODE Command offset
typedef enum cle_OTA_EnterDownOffsetDef
{
	CL_OTA_ENTER_DOWN_CMD_OFF			=	0,
	CL_OTA_ENTER_DOWN_SESSION_TYPE_OFF	=	1,
	CL_OTA_ENTER_DOWN_PASSWORD_OFF		=	2,
    CL_OTA_ENTER_DOWN_FRAG_DIM_OFF		=	10,
    CL_OTA_ENTER_DOWN_FW_NUMB_OFF		=	12,
    CL_OTA_ENTER_DOWN_START_FW_VERS_OFF	=	14,
    CL_OTA_ENTER_DOWN_STOP_FW_VERS_OFF	=	18,
    CL_OTA_ENTER_DOWN_NEW_FW_VERS_OFF	=	22,
    CL_OTA_ENTER_DOWN_TRANS_ID_OFF		=	26,
    CL_OTA_ENTER_DOWN_FW_SIZE_OFF		=	28,
    CL_OTA_ENTER_DOWN_GLOBAL_CRC32_OFF	=	32,
    CL_OTA_ENTER_DOWN_STATUS_OFF		=	1,
    CL_OTA_DOWN_FRAGMENT_SIZE			=	0x00BD,
}e_OTAEnterDownOffsDefs;

// OTA: ENTER DOWNLOAD MODE Session Type definitions
typedef enum cle_OTA_EnterDownSessionDef
{
	CL_OTA_ENTER_DOWN_SESSION_APP_FIRMWARE_FOR_ME						=	0x00,	/// Will download an applicative firmware that is dedicated for a local use
	CL_OTA_ENTER_DOWN_SESSION_RESCUE_FIRMWARE_FOR_ME					=	0x01,	/// Will download a rescue firmware that is dedicated for a local use
	CL_OTA_ENTER_DOWN_SESSION_BOOTLOADER_FIRMWARE_FOR_ME				=	0x02,	/// Will download a rescue firmware that is dedicated to a local using.
	CL_OTA_ENTER_DOWN_SESSION_DOWNLOAD_APP_FIRMWARE_TO_FORWARD			=	0x04,	/// Will download an pplicative firmware that could not been applied locally.
																					/// It will be only dedicated to forward the firmware.
	CL_OTA_ENTER_DOWN_SESSION_DOWNLOAD_RESCUE_FIRMWARE_TO_FORWARD		=	0x05,	/// Will download a rescue firmware that could not been applied locally.
																					/// It will be only dedicated to forward the firmware.
	CL_OTA_ENTER_DOWN_SESSION_DOWNLOAD_BOOTLOADER_FIRMWARE_TO_FORWARD	=	0x06,	/// Will download a bootloader firmware that could not been applied locally.
																					/// It will be only dedicated to forward the firmware.
	CL_OTA_ENTER_DOWN_SESSION_MY_APPLI_FIRMWARE_TO_FORWARD				=	0x08,	/// This session is useful to inform the node that its current applicative
																					/// firmware will be forwarded to other nodes that responds to criteria given
																					/// in the next parameters.
	CL_OTA_ENTER_DOWN_SESSION_MY_RESCUEFIRMWARE_TO_FORWARD				=	0x09,	/// This session is useful to inform the node that its current rescue firmware
																					/// will be forwarded to other nodes that responds to criteria given in the next
																					/// parameters.
	CL_OTA_ENTER_DOWN_SESSION_MY_BOOTLOADER_FIRMWARE_TO_FORWARD			=	0x0A,	/// This session is useful to inform the node that its current bootloader firmware
																					/// will be forwarded to other nodes that responds to criteria given in the next
																					/// parameters.
}e_OTAEnterDownSessionDefs;

// OTA: ENTER DOWNLOAD MODE return codes
typedef enum cle_OTA_EnterDownReturnCodeDef
{
	CL_OTA_ENTER_DOWN_STATUS_SUCCESS						=	0x00,	/// succeeded
	CL_OTA_ENTER_DOWN_STATUS_GENERIC_ERROR					=	0xFF,	/// (bad frame length and others inconsistencies)
	CL_OTA_ENTER_DOWN_STATUS_PASS_ERROR						=	0xFE,	/// (init password is different to the one received here)
	CL_OTA_ENTER_DOWN_STATUS_FRAG_SIZE_ERROR				=	0xFD,	/// (init fragment size was different)
	CL_OTA_ENTER_DOWN_STATUS_NO_VALID_DWLD_SES				=	0xFC,	/// (the download session number isn�t supported)
	CL_OTA_ENTER_DOWN_STATUS_FW_NUM_ERROR					=	0xFB,	/// (bad current firmware number)
	CL_OTA_ENTER_DOWN_STATUS_FW_RANGE_VERSION_ERROR			=	0xFA,	/// (product not concerned by the upgrade regarding its current version value)
	CL_OTA_ENTER_DOWN_STATUS_NEW_FW_VERSION_ERROR			=	0xF9,	/// (the new firmware version value isn�t highest in comparison to the current version value)
}e_OTA_EnterDownReturnCodeDef;



// OTA: DOWNLOAD_FRAGMENT definitions
typedef enum cle_OTA_DownloadFragmentsOffsetDef
{
	CL_OTA_DOWNLOAD_FRAGMENT_CMD_OFF						=	0,
	CL_OTA_DOWNLOAD_FRAGMENT_FRAME_ID_OFF					=	1,
	CL_OTA_DOWNLOAD_FRAGMENT_CRC_XOR_TRID_OFF				=	3,
	CL_OTA_DOWNLOAD_FRAGMENT_DATA_BYTES_OFF					=	5,
	CL_OTA_DOWNLOAD_FRAGMENT_STATUS_OFF						=	1,
}e_OTADownloadFragmOffsDefs;

// OTA: DOWNLOAD_FRAGMENT return CODE
typedef enum cle_OTA_DownloadFragmentsReturnCodeDef
{
	CL_OTA_DOWNLOAD_FRAGMENT_CMD_SUCCESS					=	0x00,
	CL_OTA_DOWNLOAD_FRAGMENT_CMD_ERROR						=	0xFF,
}e_OTA_DownloadFragmentsReturnCodeDef;

// internal structure used for OTA service
struct st_OtaInternal
{
	clu8		pTrailerData[3];		/// encapsulation to access OTA service in CLOVER-STACK: start of command/buffer
	clu8		*pEndData;				/// end buffer appended to data sent to device
	clu32		u32TrailerLen;			/// length of data before OTA data to access CLOVER-NET service
	clu32		u32EndLen;				/// length of data after OTA data to access CLOVER-NET OTA service
	clu8		u8CurrentCommand;		/// current command executed in the state machine
	clu8		u8NextCommand;			/// next command to be executed in the state machine
	clu32		u32CurrentPacket;		///	identifier of packet sent
	clu32		u32BinaryIndex;			///	index in the binary to send
	clu8		*pBinaryData;			///	pointer on the binary data to be sent to the device
	clu32		u32BinaryLen;			/// length of binary to send
	e_State		eEncReaderState;		/// encryption reader state before calling OTA
	e_State		eTargReaderState;		/// target reader state before calling OTA
	t_Reader	*pEncReader;			/// encryption reader
	t_Reader	*pTargReader;			/// target reader
	clu8		u8OtaProgress;			/// progress in the OTA process for display
	clu8		aOTAMissingFrag[100];	/// contains flags on missing fragments
	clu8		bOTAMissFrag;			/// if this is set to CL_ERROR => we have missing fragment to send.... so proceed
	clu8		u8OTAMissFragRetry;		///
	e_Result	bOTAContinuePrvSession;	/// if CL_OK, continue previous session is required, if not, we need to proceed on sending every packets
	clu8		u8FirmwareDestination;	/// destination of the firmware: local or sent to remote
};

typedef struct st_OtaInternal t_OtaInternal;


// OTA: GET_MISSING_FRAGMENT definitions
typedef enum cle_OTA_GetMissingFragmentsOffsetDef
{
	CL_OTA_GET_MISSING_FRAGMENT_CMD_OFF						=	0,
	CL_OTA_GET_MISSING_FRAGMENT_TRANSACTION_ID_OFF			=	1,
}e_OTAGetMissingFragmOffsDefs;

typedef enum cle_OTA_GetMissingFragmentsReturnOffDef
{
	CL_OTA_GET_MISSING_FRAGMENT_STATUS_OFF						=	0,
	CL_OTA_GET_MISSING_FRAGMENT_SESSION_STATUS_OFF				=	1,
	CL_OTA_GET_MISSING_FRAGMENT_FRAGMENT_FLAGS_OFF				=	2,
}e_OTA_GetMissingFragmentsReturnOffDef;

// OTA : 	FORWARD_EXTERNAL_FW
typedef enum cle_OTA_ForwardExternalFwOffsetDef
{
	CL_OTA_FORWARD_EXT_FW_CMD_OFF							=	0,
	CL_OTA_FORWARD_EXT_FW_RADIO_MODE_OFF					=	1,
	CL_OTA_FORWARD_EXT_FW_RADIO_CHANNEL_OFF					=	2,
	CL_OTA_FORWARD_EXT_FW_RF_COM_TYPE_OFF					=	3,
	CL_OTA_FORWARD_EXT_FW_DEST_ADDR_OFF						=	4,
	CL_OTA_FORWARD_EXT_FW_PASSWORD_OFF						=	10,
	CL_OTA_FORWARD_EXT_FW_START_FW_VERS_OFF					=	18,
	CL_OTA_FORWARD_EXT_FW_STOP_FW_VERS_OFF					=	22,
	CL_OTA_FORWARD_EXT_FW_TRANS_ID_OFF						=	26,
}e_OTAForwardExternalFwOffsDefs;

// OTA: FLASH_EXT_FW return CODE
typedef enum cle_OTA_FlashExtFwReturnCodeDef
{
	CL_OTA_FLASH_EXT_FW_SUCCESS								=	0x00,
	CL_OTA_FLASH_EXT_FW_ERROR								=	0xFF,
}e_OTA_FlashExtFwReturnCodeDef;

#define	CL_OTA_ENTER_DOWNLOAD_MODE_SIZE	36				// size of the command ENTER_DOWNLOAD_MODE

#define CL_OTA_GENERIC_CMD_OFFSET					0	// GENERIC COMMAND OFFSET
#define CL_OTA_GENERIC_STATUS_OFFSET				1	// Generic status byte offset
#define CL_OTA_MISSING_FRAGS_MAX_RETRYS				4	// retry CL_OTA_MISSING_FRAGS_MAX_RETRYS to send missing fragments
/*****************************************************************************/
/* Name : e_Result cl_CryptoThread( void )						           */
/* Description :                                                         	 */
/*        thread which handles OTA service in CSL */
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
e_Result cl_CryptoThread( clvoid );

/*******************************************************************************************/
/** @fn : clvoid cl_initOTAService(  )
* @brief :
* \n       	initialize internal variables of OTA service
* @param **ppOta	: ( In-Out ) stores pointer on OTA object
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* *************************************************************************************** */
clvoid cl_initOTAService( );
/*******************************************************************************************/
/** @fn : e_Result cl_getOTAService( t_OtaInternal **ppOta )
* @brief :
* \n       	return a pointer on OTA global variable
* @param **ppOta	: ( In-Out ) stores pointer on OTA object
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* *************************************************************************************** */
e_Result cl_getOTAService( t_OtaInternal **ppOta );

#ifdef OTASERVICE_C_
	t_OtaInternal g_tOtaInternal;
#else
	extern t_OtaInternal g_tOtaInternal;
#endif

#endif /* OTASERVICE_H_ */
/**************************************************/
