/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
*
* @file cltypes.h
* @brief all data types used in CSL definitions and descriptions
*  Created on: 24 juin 2014
*  @author: fdespres
									*/

#ifndef CLTYPES_H_
#define CLTYPES_H_


/******************************/
/* definition of main types */
/*******************************/
/** @def cl8
 * @brief 8 bit signed */
#define cl8    	char

/** @def clu8
 * @brief 8 bit unsigned */
#define clu8   	unsigned char

/** @def cl16
 * @brief 16 bit signed */
#define cl16   	short

/** @def clu16
 * @brief 16 bit unsigned */
#define clu16  	unsigned short

/** @def cl32
 * @brief 32 bit signed */
#define cl32   	long

/** @def clu32
 * @brief 32 bit unsigned */
#define clu32  	unsigned long

/** @def cl64
 * @brief 64 bit signed */
#define cl64   	long long

/** @def clu64
 * @brief 64 bit unsigned */
#define clu64  	unsigned long long

/** @def clvoid
 * @brief generic pointer */
#define	clvoid	void

/** @def CL_NULL
 * @brief NULL in Clover system */
#define CL_NULL 0

/** @def clbool
 * @brief boolen in Clover system */
//#define clbool 	bool               	/// boolean BRY_31/30/2015 not used !!!!

/* BRY_31/03/2015 */
/*----------------*/

#ifndef QT_COMPILATION

/* Definition of boolean type */
#define bool  unsigned char
#define true  (1==1)
#define false (1==0)

#endif

/* String definition */
#define STRING_LENGTH ((size_t)1024)

/* End Of String */
#define EOS '\x0'

#define SIZE_MAX_BUFFER 512

/****************************************************************************/
/*  Generic result from API                                                 */
/****************************************************************************/

/** @enum e_Result
 @brief 	error code for different functions
 @param  	CL_OK = 0					  	Result is OK
 @param 	CL_ERROR,                    	Failure on execution
 @param 	CL_MEM_ERR,                    	Failure on memory management (failure, allocation ....)
 @param 	CL_PARAMS_ERR,               	Inconsistent parameters
 @param 	CL_TIMEOUT_ERR,              	Overrun on timing
 @param 	CL_TRANSFER_IN_PROCESS_ERR   	A non blocking call is in progress. This transfer is rejected. The user application shall recall the API to ensure that this request is proceeded
 */
typedef enum cle_Rslt
{
  CL_OK = 0,                   	///  Result is OK
  CL_ERROR,                    	///  Failure on execution
  CL_MEM_ERR,                  	///  Failure on memory management (failure, allocation ....)
  CL_PARAMS_ERR,               	///  Inconsistent parameters
  CL_TIMEOUT_ERR,              	///  Overrun on timing
  CL_TRANSFER_IN_PROCESS_ERR,   ///  A non blocking call is in progress. This transfer is rejected. The user application shall recall the API to ensure that this request is proceeded
  CL_ERROR_IN_COMM,				/// BRY : COM Layer says "error in comm"
  CL_EOF_ERR					/// reach end of file
}e_Result;

/** @enum e_Crypted */
typedef enum cle_Crptd
{
  DEFAULT_ENCRYPTION = 0,	/// defaut value is used (aka if not set for a device via cl_DeviceSetEncryptionLevel API, then there will be non encryption applied for this device).
                          	/// if cl_DeviceSetEncryptionLevel was called and specified one value of e_Crypted, then subsequent transfer to the end-device will be performed
                          	/// using this encryption unless explicitely set
  NONE,               		/// No encryption
  CRYPTO_APPLI,       		/// Encryption at application level,
  CRYPTO_PHY,         		/// Encryption at Physical level
}e_Crypted;

/** @enum e_CallType */
typedef enum cle_CllTp
{
  BLOCKING,           		/// call to the API is blocking. Until the completion of the call is not acheived, the call to the API is not returning
  NON_BLOCKING,        		/// call to the API is non blocking. The completion is signaled via a callback
}e_CallType;

/** @enum e_OptionsKey */
typedef enum cle_PtnsK
{
  PROD_KEY,					/// production key
  NEW_KEY					/// new key
}e_OptionsKey;


/** @enum e_ConnectionType */
typedef enum cle_CnnctnTp
{
  TSP_LAYER_SET_LOCAL                 = 0x01,      /// Set configuration in reader
  TSP_LAYER_SET_DEVICE                      ,      /// Send data to the device (framework)
  TSP_LAYER_ANSW_DEVICE                     ,      /// Get data from device (framework)
  TSP_LAYER_REQ_CLV_STACK                   ,      /// Request to Clover Stack
  TSP_LAYER_ANSW_CLV_STACK                  ,       /// Answer from Clover Stack
  TSP_LAYER_REQ_LOCAL_SVC                   ,       /// Request local service
  TSP_LAYER_ANSW_LOCAL_SVC                  ,       /// Answer from local service
  TSP_LAYER_ANSW_ERR_LOCAL_SVC              ,       /// Error from local service
  TSP_LAYER_REQ_LOCAL_MW                    ,       /// Request to Local Middleware
  TSP_LAYER_ANSW_LOCAL_MW                   ,       /// Answer from Local Middleware
  TSP_LAYER_REQ_OTA_SVC						,		/// send frame for OTA service
  TSP_LAYER_ANSW_OTA_SVC					,		/// get frame from OTA service
  TSP_LAYER_ERR_MSG                   = 0xFF,        /// Error Message
}e_ConnectionType;

/** @enum e_FileOptions */
typedef enum cle_FileOptions
{
	CL_FILE_TEXT_MODE,			/// in Windows, a new line is indicated by "Carriage Return' and Line Feed'
								/// in Linux, a new line is indicated by  "Line Feed"
	CL_FILE_BINARY_MODE,		/// for binary, remove the Carriage Return
}e_FileOptions;

/** @enum e_ActionScript */
typedef enum cle_ActionScript
{
  	ACTION_SCRIPT_SET_VALUE,	/// in a script, declare that the variable shall be updated after entering a value
  	ACTION_SCRIPT_GET_VALUE,	/// in a script, get the value
  	ACTION_SCRIPT_NONE,
  	ACTION_SCRIPT_UNKNOWN,
}e_ActionScript;

/** def CL_FAILED( x ) */
#define CL_FAILED( x ) (( (x)==CL_OK ) ? 0: 1)

/** def CL_SUCCESS( x ) */
#define CL_SUCCESS( x ) (( (x)==CL_OK ) ? 1: 0)

/** @enum e_ReaderType */
typedef enum cle_ReaderType
{
	IP_READER_TYPE		=	0x01,		/// IP reader
	COM_READER_TYPE 	=	0x02,		/// Serial reader accessed over COM interface
	BT_READER_TYPE		=	0x04,		/// Bluetooth reader (should be accessed via COM interface)
	READER_TYPE_MASK	=	0x0F,		/// enable to mask the reader type for OS IO support without being bothered by specific readers brands/versions....
	LANTRONIX_READER_TYPE = 0x80,		/// reader type of type LANTRONIX
	UNKNOWN_READER_TYPE = 0xFF,			/// unknown reader type
}e_ReaderType;

/** @enum e_HciCmdType */
typedef enum cle_HciCommandType
{
	HCI_INITIALIZATION_CMD,		/// initialization commands in HCI mode 	(check documentation)
	HCI_HIGH_LEVEL_CMD,			/// High level commands					( check documentation )
	HCI_SPONTANEOUS_CMD,		/// Spontaneous commands					( commands issued by end device )
	HCI_PAIRING_CMD,			/// pairing command to register a device to a reader
	HCI_UNKNOWN_CMD,			/// unknown cmd to detect error
}e_HciCmdType;

typedef enum cle_StackSupport
{
	IP_STACK_SUPPORT	= 	0x01, 	/// indicates that IP readers using TCP/IP support from host OS while be used
	COM_STACK_SUPPORT	=	0x02,	/// indicates that COM readers using COM support from host OS while be used
	BT_STACK_SUPPORT	=	0x04,	/// indicates that Bluetooth readers using Bluetooth support from host OS while be used
}e_StackSupport;

typedef enum cle_ReaderState
{
	STATE_DEFAULT, 				/// default state of framework, reader, device before init/connect/disconnect/error/ok
	STATE_INIT, 				/// indicates the framework is initializing, a reader is added to a list, a device is added to the list
	STATE_CONNECT,				/// asks to connect a reader/device to its IOs layer, indicates that a reader/device was successfully connected to its IOs layer
	STATE_DISCONNECT,			/// asks to disconnect a reader/device from its IOs layer, indicates that a reader/device was successfully disconnected from its IOs layer
	STATE_DISCOVER,				/// use the discover mechanism by creating UDP socket instead of TCP. Then broadcast on 255.255.255.255 to get list of IP readers connected
	STATE_ERROR,				/// indicates that the framework/reader/device got an error from its underlaying IOs layer
	STATE_OTA,					/// indicates that the framework/reader is in OTA mode
	STATE_OK,					/// indicates the framework correctly initialized
}e_State;


typedef enum cle_COM_BitsDef
{
	CL_COM_BYTESIZE_5BITS,
	CL_COM_BYTESIZE_6BITS,
	CL_COM_BYTESIZE_7BITS,
	CL_COM_BYTESIZE_8BITS,
	CL_COM_STOPBITS_10BIT,
	CL_COM_STOPBITS_15BIT,
	CL_COM_STOPBITS_20BIT,
	CL_COM_PARITYBIT_NONE,
	CL_COM_PARITYBIT_EVEN,
	CL_COM_PARITYBIT_ODD,
	CL_COM_PARITYBIT_MARK,
	CL_COM_PARITYBIT_SPACE,
	CL_COM_BAUDRATE_4800,
	CL_COM_BAUDRATE_9600,
	CL_COM_BAUDRATE_19200,
	CL_COM_BAUDRATE_38400,
	CL_COM_BAUDRATE_57600,
	CL_COM_BAUDRATE_115200,
}e_COMBitDefs;

typedef enum cle_OTA_CommandsDef
{
	CL_OTA_TIMEOUT_EVENT			=	0x00,
	CL_OTA_MW_FW_UPGRADE_INIT		=	0x01,
	CL_OTA_ENTER_DOWNLOAD_MODE 		=	0x02,
	CL_OTA_FORMAT_EXTERNAL_MEMORY_WITH_NODE_CURRENT_FW	=	0x20,
	CL_OTA_DOWNLOAD_FRAGMENT		=	0x21,
	CL_OTA_GET_MISSING_FRAGMENT		=	0x22,
	CL_OTA_FLASH_EXTERNAL_FW		=	0x23,
	CL_OTA_FORWARD_EXTERNAL_FW		=	0x24,
	CL_OTA_REBOOT_ON_RESCUE_FW		=	0x25,
	CL_OTA_GET_FW_NUMBERS_AND_VERSIONS	=	0x26,
	CL_OTA_UNKNOWN_COMMAND			=	0xFF
}e_OTACmdDefs;


typedef enum cle_NET_TransmissionTypeDef
{
	CL_NET_TRANSMISSION_UNICAST		=	0x01,
	CL_NET_TRANSMISSION_MULTICAST	=	0x02,
	CL_NET_TRANSMISSION_BROADCAST	=	0x03,
}e_NET_TransmissionTypeDef;

typedef enum cle_SERIAL_RetryProtocolDef
{
	CL_SERIAL_PROT_DEFAULT			=	0x01,	// by default, the Write/Read thread synchro is in this state
	CL_SERIAL_PROT_W_WAIT_FOR_ACK	=	0x02,	// a write was issued by the write thread. Write is waiting for Read Thread to receive 0x06
	CL_SERIAL_PROT_R_RECEIVED_ACK	=	0x04,	// 0x06 was received from read thread. Depending on type of Command sent, either Wait for the answer or exit
	CL_SERIAL_PROT_R_RECEIVED_NACK	=	0x08,	// 0x15 was received from read thread. Reemit data to reader
	CL_SERIAL_PROT_W_WAIT_FOR_ANSW	=	0x10,	// Wait for Answer from the reader (timeout..)
	CL_SERIAL_PROT_R_WRITE_ACK_REQUIRED	=	0x20,// a correct answer was received from the reader, Write thread Shall acknowledge it with 0x06
	CL_SERIAL_PROT_W_END_REQUIRED	=	0x40, 	// write thread requires that the read thread acknowledge this
	CL_SERIAL_PROT_R_END_GRANTED	=	0x80, 	// acknowledge is done, we can process next tuple
	CL_SERIAL_PROT_W_END_DONE		=	0x88,	// end performed
	CL_SERIAL_PROT_UNKNOWN			= 	0xFF,	// no idea of what to do....
}
e_SERIAL_RetryProtocolDef;


// Serial protocol define: A serial reader and IP reader use the same low encapsulation to:
// 1. enhance security during transfer
// 2. allow sorting of packets if reader pushes several packets concatenated
#define READER_SERIAL_PROTOCOL_TRAILER_LEN				3	// Serial protocol: trailer len is 3: SYNC 	| 	STX 	| 	LEN
#define READER_SERIAL_PROTOCOL_FINISH_LEN				3	// Serial protocol finish len is 3: 	CRC 	| 	CRC 	| 	ETX
#define READER_SERIAL_PROTOCOL_LEN						( READER_SERIAL_PROTOCOL_TRAILER_LEN + READER_SERIAL_PROTOCOL_FINISH_LEN )
#define IP_READER_SERIAL_PROTOCOL_TRAILER_LEN			3	// serial protocol: trailer len is 3: SYNC 	| 	STX 	| 	LEN
#define IP_READER_SERIAL_PROTOCOL_FINISH_LEN			3	// serial protocol finish len is 3: 	CRC 	| 	CRC 	| 	ETX
#define IP_READER_SERIAL_PROTOCOL_LEN					( IP_READER_SERIAL_PROTOCOL_TRAILER_LEN + IP_READER_SERIAL_PROTOCOL_FINISH_LEN )

#define IP_READER_DEFAULT_TCP_PORT						3636

#define	CSL_SERIAL_MAX_REPEATS	3								// repeat 3 times max the last command before exiting on error
#define CSL_SERIAL_MAX_TIMEOUT	300								//	300 ms of timeout on serial read

#endif /* CLTYPES_H_ */
