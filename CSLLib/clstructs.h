/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: clstructs.h                                                        */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/*  Description :  defines all structures used in the framework              */
/*****************************************************************************/
#include "cltypes.h"

#ifndef CLSTRUCT_H_
#define CLSTRUCT_H_

/*****************************************************************************/
/*  Structures and defines for Readers and Devices management                */
/*****************************************************************************/

/// typedef struct st_GenericSynchroSupport
typedef struct st_GenericSynchroSupport
{
	clvoid	*tThreadId;			/// create one thread or task per reader on read
	clvoid	*pSgl;				/// create a signal from upper layer to CSL to indicate that a data has to be sent

}t_GenSynchro;


/// typedef struct st_ReaderSynchroSupport
typedef struct st_ReaderSynchroSupport
{
	clvoid	*tThreadId4Read;		/// create one thread or task per reader on read
	clvoid	*tThreadId4Write;		/// create one thread or task per reader for write
	clvoid	*mutexReadIncomingWR;	// mutual exclusion between ReadIncoming and Write/Read - No more used: SESSION_MODE_CANCELED
	clvoid	*mutexWriteOnPortCom;   // mutual exclusion to write on PortCom
	clvoid 	*pSgl4Read;				/// create a signal from lower layer to CSL to indicate that a data is available for read
	clvoid	*pSgl4Write;			/// create a signal from upper layer to CSL to indicate that a data has to be sent
	clvoid  *pSgl4WriteComplete;	/// signal that an underlayer write was performed
	clvoid	*pSgl4ReadComplete;		/// signal that a successful read was completed by underlayer
	clu32	u32SerialTimeout;		/// general timeout value on low level exchange before exiting on error on low level command
	clu32	u32Retries;				/// hold retries count for serial low layer repeat protocol
	e_SERIAL_RetryProtocolDef eStatus;	/// hold status of the exchange between read and write threads. It used for retries/timeouts management
	e_CallType	eCallType;			/// specify BLOCKING read or asynchronous read via callbacks
}
t_RdrSynchro;

///------------------------------------------------------
/// IP/COM Range used to discover the readers on a network
///-----------------------------------------------------
/// typedef struct st_IPRange
typedef struct st_ReaderRange
{
	clu64	u64startRange;		/// first IP address for the TCP scan
	clu64	u64stopRange;		/// first IP address for the IP scan
	clu32	u32Port;			/// TCP port to scan
	clvoid	*pNext;
}t_ReaderRange;

///------------------------------------------------------
/// information about the radio card contained in the reader
///------------------------------------------------------
struct st_CloverSense
{
	clu8	au8RadioAddress[6];	///
	clvoid	*pNext;				/// pointer on next CloverSense on this reader
};

typedef struct st_CloverSense t_CloverSense;

///------------------------------------------------------
/// Options on current buffer (repeat/encrypt/relaxation period
///  exchange to be saved or not)
///------------------------------------------------------
/// typedef struct st_Options
typedef struct st_Options
{
  e_ConnectionType       tCnct;            /// Connection type (local/Framework access....)
  e_Crypted              e_CryptedLink;     /// encryption request on the buffer to send/receive
  clu32                 ulRepeat;         /// send the same buffer several times-0 means : do not sent-1 is default.
  clu32                 ulPeriod;         /// Period to wait before sending a new buffer. If 0, there is no waiting period between each send of data
  clu32                 ulMaxTimeout;     /// Max Period allowed before declaring that the data sent was not sent and returning an Error to upper layer
  clu8                  *pFileName;       /// Save the buffer to file if pFileName is not NULL
}t_Options;

///------------------------------------------------------
/// generic buffer structure (pointer to allocated memory
///------------------------------------------------------
/// typedef struct st_Buffer
typedef struct st_Buffer                   /// Generic Buffer structure to hold memory pointer and length of accessible data
{
  clu8 *pData;                              /// pointer on the buffer to send/receive
  clu32 ulLen;                              /// pointer on len of the buffer to send/receive
}t_Buffer;

///------------------------------------------------------
/// tuple encapsulate buffer type with additional info to encrypt it/tag it...
///------------------------------------------------------
struct st_Tuple
{
  t_Buffer  *ptBuf;                          /// pointer on Buffer of Data
  t_Options tOptions;                        /// Flags applied to current buffer (local save/repetition/ encryption.....
  cl8		cl8Time[80];					 /// each Tuple contains a field to set/get timing information
  struct st_Tuple *pNext;                    /// pointer on next buffer to send/receive
};

typedef struct	st_Tuple t_Tuple;

///------------------------------------------------------
/// CALLBACKS
/// structure pointing to stack function that can be called by HAL
///------------------------------------------------------
struct st_clSys_CallBackFunctions
{
  /// reset received from under layer
  e_Result (*fnReset_cb)			( clvoid *pReader, e_Result status );
  /// Data sent to network
  e_Result (*fnIOSendDataDone_cb)	( clvoid *pReader, clvoid *pTuple, e_Result status );
  /// Data has been received from Host
  e_Result (*fnIOData2Read_cb)	( clvoid *ppReader, clvoid *ppTuple, e_Result eStatus );
  // state update from under layer
  e_Result (*fnIOState_cb) ( clvoid *pCtxt, clvoid *pReader, clvoid *pDevice, e_Result eStatus );
  // OTA service update from underlayer
  e_Result (*fnOTAProgress_cb) ( clu32 eState, clvoid *pReader, clu32 u32Progress );
  // OTA send data to network notification
  e_Result (*fnOTASendDataDone_cb) ( clvoid *pReader, clvoid *pTuple, e_Result status );
  // OTA data has been received from underlayer
  e_Result (*fnOTAData2Read_cb) ( clvoid *ppReader, clvoid *ppTuple, e_Result eStatus );

};
typedef struct st_clSys_CallBackFunctions t_clSys_CallBackFunctions;


/// ******************************************************************
/// structure pointing to HAL functions linked to a reader
/// that are called by the library
/// ******************************************************************
/// typedef struct st_clReader_HalFunctions_s
struct st_clReader_HalFunctions_s
{
	/** register a reader from library to porting layers */
	e_Result (*fnIORegister)(  clvoid *pReader );

	/** unregister a reader from library to porting layers */
	e_Result (*fnIOUnregister)(  clvoid *pReader );

	/** reset network connection via socket */
	e_Result (*fnIOReset)(  clvoid *pReader );

	/** open a connection to a reader */
	e_Result (*fnIOOpenConnection)( clvoid *pReader );

	/** send data typically via socket connection */
	e_Result (*fnIOSendData)( clvoid *pReader, t_Buffer *ptBuff);

	/** get data list typically via socket connection */
	e_Result (*fnIOGetData)(  clvoid *pReader, t_Tuple **pptTuple );

	/** close connection to a reader */
	e_Result (*fnIOCloseConnection)( clvoid *pReader );

	/** get DNS table typically LAN */
	e_Result (*fnIOGetDNSTable)( clvoid **pptReaderList);

	/** set the IP range list where the readers can be found */
	e_Result (*fnSetIPRange)( t_ReaderRange *pRange );
};
typedef struct st_clReader_HalFunctions_s t_clReader_HalFuncs;

/// ******************************************************************
/// IP Reader parameters
/// ******************************************************************
struct st_IPReaderParams
{
  clu8     			aucIpAddr[15];		        /// IP address of the reader in the network
  clu64     		u64MacAddr;		            /// Mac address of the reader
  clu32				u32Port;					/// TCP/UDP Port where to initiate the connection
  t_ReaderRange		tRange;						/// contains range of IP address to scan (used by framework)
};
typedef struct st_IPReaderParams t_clIPReader_Params;

/// ******************************************************************
/// BlueTooth Reader parameters
/// ******************************************************************
struct st_BTReaderParams
{
  clu8     			u8Unused;		            /// BT address of the reader in the network
  t_ReaderRange		tRange;						/// contains range of BT address to scan (used by framework)
};
typedef struct st_BTReaderParams t_clBTReader_Params;

/// ******************************************************************
/// COM Reader parameters
/// ******************************************************************
struct st_COMReaderParams
{
	clu8     		aucPortName[15];		            /// IP address of the reader in the network
	e_COMBitDefs 	eByteSize;							/// Byte size for Transfer
	e_COMBitDefs 	eStopBits;							///	define stop bits on serial
	e_COMBitDefs 	eParityBits;						/// define parity bits on serial
	e_COMBitDefs 	eBaudRate;		                	/// Baud rate for transfer of the reader
	clu8			u8ReadIntervallTimeout;				/// Timeout not to hang if no reception
	clu8			u8ReadTotalTimeoutConstant;			/// Timeout not to hang if no reception
	clu8			u8ReadTotalTimeoutMultiplier;		/// Timeout not to hang if no reception
	clu8 			u8WriteTotalTimeoutConstant;		/// Timeout not to hang if no reception
	clu8			u8WriteTotalTimeoutMultiplier;		/// Timeout not to hang if no reception
	t_ReaderRange	tRange;								/// contains range of COM port to scan (used by framework)
	void 			*handleStatusMessage;
	void 			*handleThreadExitEvent;
};
typedef struct st_COMReaderParams t_clCOMReader_Params;

/*--------------------------------------------------------------------------*\
 * Parsing a frame on several buffers
\*--------------------------------------------------------------------------*/
struct st_FrameParsingParameters
{
		cl8	StartOfTuple;
		cl8 MiddleOfTuple;
		cl8 EndOfTuple;
		cl32 StartIndex;
		cl32 MiddleIndex;
		cl32 EndIndex;
		clu16 FrameCRC;
		clu8 *pDataStart;
		clu8 *pDataEnd;
};
typedef struct st_FrameParsingParameters t_FrameParsingParameters;

/// ******************************************************************
///------------------------------------------------------
/// Reader information (Address Mac/Ip/labels....)
///------------------------------------------------------
///
/// typedef struct st_Reader
/// ******************************************************************
struct st_Reader
{
  /** String to designate the IP reader (if any). Can be found in DNS table */
  e_ReaderType 			tType;				/// Type of reader (LANTRONIX, others....)
  e_State				eState;				/// State of a reader
  clu8      			aucLabel[64];		/// Friendly name of a reader
  clu64					u64Handle;			/// handle for this reader
  t_clIPReader_Params	tIPParams;			/// IP reader parameters
  t_clCOMReader_Params	tCOMParams;			/// COM reader parameters
  t_clBTReader_Params	tBT;				/// Bluetooth parameters
  t_FrameParsingParameters tParsingParams;
  t_RdrSynchro			tSync;				/// structure which holds tasks/threads for sending/receiving data as well as necessary signals for intertasks/threads synchronization
  t_Tuple				*p_TplList2Send;	/// tuple list which contain the tuples to be sent on this reader
  t_Tuple				*p_TplList2Read;	/// tuple list which contain the tuples to be READ FROM this reader
  t_clSys_CallBackFunctions		tCallBacks;	/// opaque pointer to callbacks functions used for read complete/write complete/reset complete
  t_clSys_CallBackFunctions		tCallBacksDiscover;	/// opaque pointer to callbacks functions used for read complete/write complete/reset complete
  t_clReader_HalFuncs	tReaderHalFuncs;	/// Hal functions related to low level access
  t_clReader_HalFuncs	tReaderDiscoverHalFuncs;/// Hal functions related to low level access for discover process
  t_CloverSense			tCloverSense;		/// information about radio card
  struct st_Reader   	*pNext;			    /// Pointer on the next Reader in the list. NULL if none
};
typedef struct st_Reader t_Reader;

/// ******************************************************************
/// structure pointing to HAL functions that are called by the library
/// ******************************************************************
/// typedef struct st_clSys_HalFunctions
struct st_clSys_HalFunctions_s
{
	/** semaphore create */
  e_Result (*fnSemaphoreCreate)( clvoid **pSem );

  /** semaphore wait */
  e_Result (*fnSemaphoreWait)( clvoid *pSem, clu32 u32ms);

  /** semaphore release */
  e_Result (*fnSemaphoreRelease)( clvoid *pSem );

  /** semaphore destroy */
  e_Result (*fnSemaphoreDestroy)( clvoid *pSem );

  e_Result (*fnMutexCreate)( clvoid **aAdrMutex );
  e_Result (*fnMutexDestroy)( clvoid **aAdrMutex );

  /** sleep in ms (blocking call) */
  e_Result (*fnWaitMs)( clu32 u32ms );

  /** allocate memory and returns allocated pointed pointer to data*/
  e_Result (*fnAllocMem)( clvoid **pptBuff, clu32 ulLen, const char* aFileName, int aLine );

  /** free memory from pointer*/
  e_Result (*fnFreeMem)( clvoid *ptBuff );

  /** free memory safely from pointer */
  e_Result (*fnFreeMemSafely)( clvoid **ptBuff );

  /** create thread */
  e_Result (*fnCreateThread)( clvoid *pnFnThread,  clu32 ulStackSize, clvoid *pParams, clvoid *p_tThreadId );

  /** Destroy thread */
  e_Result (*fnDestroyThread)( clvoid *tThreadId );

  /** Request to destroy this thread was issued */
  e_Result (*fnDestroyThreadAsked)( clu8 *pu8CancelRequest );

  /** set file io open */
  e_Result (*fnFileOpen)( cl8 *pFileName, e_FileOptions eOpts, clvoid **ppFileId);

  /** file write */
  e_Result (*fnFileWrite)( cl8 *pFile, clu8 *pu8Data, clu32 u32Len );

  /** file read */
  e_Result (*fnFileReadLn)( cl8 *pFile, clu8 **ppu8Data, clu32 *pu32Len);;

  /** file close */
  e_Result (*fnFileClose)( cl8 *pFile );

  /** file set position */
  e_Result (*fnFileSetPos)( cl8 *pFile, clu32 u32Pos );

  /** file get position */
  e_Result (*fnFileGetPos)( cl8 *pFile, clu32 *pu32Pos );

  /** print completion function */
  e_Result (*fnPrintComplete)( clvoid );

  /** get time from epoch and convert to string */
   e_Result (*fnGetTime)( cl8 *pTime, clu32 ulLen );

   /** get char from an opened file	*/
   e_Result (*fnFileGetChar)( clvoid *pFileId, cl8 *pcChar );

   /** load IP stack support on target OS **/
   e_Result (*fnIPStackSupportLoad)( clvoid );

   /** unload IP stack support on target OS **/
   e_Result (*fnIPStackSupportUnload)( clvoid );

   /** load COM stack support on target OS **/
   e_Result (*fnCOMStackSupportLoad)( clvoid );

   /** unload COM stack support on target OS **/
   e_Result (*fnCOMStackSupportUnload)( clvoid );

   /** load IP stack support on target OS **/
   e_Result (*fnBTStackSupportLoad)( clvoid );

   /** unload IP stack support on target OS **/
   e_Result (*fnBTStackSupportUnload)( clvoid );

   /** get elapsed time	**/
   cl64 (*fn_GetElapsedTime)( clvoid );

   /** trace functions **/
   void (*fnTrace0)( char *aFormatString, ... );
   void (*fnTrace1)( char *aFormatString, ... );

   void (*fnStackTrace)( void );
};
typedef struct st_clSys_HalFunctions_s t_clSys_HalFunctions;

///------------------------------------------------------
/// EndDevice (Clover Sense) information : name/address/
/// which reader it is connected to/encryption of the radio
///  link
///------------------------------------------------------
struct st_Device
{
	e_State		eState;						/// State of a device
	clu8      	aucLabel[64];              	/// String to designate the IP reader (if any)
	clu64     	u64Addr;		          	/// Physical address of the EndDevice in the Clover network
	e_Crypted  	e_CryptedLink;            	/// this value can be set via cl_DeviceSetEncryptionLevel and will apply to subsequent transfer
                                          	/// NONE : the link to this Device is unencrypted
                                  	  	    /// CRYPTO_APPLI: encryption applies at APPLICATION Layer only
                                          	/// CRYPTO_PHY : encryption applies to the whole datagram (PHY+APPLICATION)
	t_Tuple	*pTupleList;				    /// tuple list to be sent associated this device
	t_Reader  *ptReader;                    /// pointer to the Reader where the EndDevice is connected to
	struct 	st_Device   *ptNext;			/// pointer on the next EndDevice in the list of the reader. NULL if none
};
typedef struct st_Device t_Device;

///------------------------------------------------------
/// OTA parameters to start transfer/encryption
///------------------------------------------------------
struct st_CryptoParams
{
	t_Reader *ptEncryptionKeyReader;		/// pointer to reader that will handle the encryption of the data
	t_Reader *ptTargetReader;				/// pointer to the reader that will send data to CSL
};
typedef struct	st_CryptoParams t_CryptoParams;

///------------------------------------------------------
/// Specifies a Tuple to send and the destination Reader
///------------------------------------------------------
///
///typedef struct st_Tuple4Reader
typedef struct st_Tuple4Reader
{
  t_Tuple  tTuple;                 			/// Tuple list prepared to be sent to device
  t_Device tDestDevice;                     /// Destination Device
}t_Tuple4Reader;
//------------------------------------------------------
// Specifies the general file name for persistent storage
//------------------------------------------------------
/// typedef struct st_MenuFileDescriptor

typedef struct st_MenuFileDescriptor
{
	clu8	*pMenuDescriptionFile;		/// description of the menu with the different sub menu and variables to affect
	clu8	*pMenuVarDescription;		/// get the description the different variables (description, type.....)
	clu8	*pMenuVarValues		;		/// store the values of the different variables
}t_MenuFileDef;

typedef struct st_t_DebugParams
{
	cl64	u64Seconds;				/// contains time (to be completed by nanoseconds) of last call BRY_07042015_ERROR: must be a long NOT an unsigned long !!!!
	cl64	u64NanoSeconds;			/// contains time in nanoseconds ( to be be completed by u64Seconds) of last call BRY_07042015_ERROR: same, must be long !!!!
}
t_DebugParams;

typedef struct st_t_ConfigParams
{
	cl8	FullPathName[128];	// Full path name, where the lib is executing
}
t_ConfigParams;

//------------------------------------------------------
// Specifies the general context pointer for indirection in CSL code
//------------------------------------------------------
/// typedef struct st_cl_Context
typedef struct st_cl_Context
{
	e_StackSupport				eStackSupport;			/// holds support of IP/BT/COM for CSL framework
	t_clSys_CallBackFunctions 	*ptCallbacks;			/// points to Callbacks functions after read/write/reset
	t_clSys_CallBackFunctions 	*ptCallbacksDiscover;	/// points to Callbacks functions after a discover read/write/reset
	t_clSys_HalFunctions 		*ptHalFuncs;			/// points to Abstraction Layer function via redirection
	t_MenuFileDef				*ptMenuFileDef;			/// points to the variables for menu description if any
	t_GenSynchro				g_RegReader;			/// hold thread and signal for registration/unregistration of readers
	t_GenSynchro				tCrypto;				/// crypto support thread
	t_GenSynchro				tClbCtrl;				///	hold synchro signals to access callbacks and avoid multiple threads access on user data
	t_Reader					*ptCslReader;			/// Default reader in the framework containing all necessary parameters for communication
	t_clReader_HalFuncs			*p_IPReaderHal;			/// IP Reader HAL functions
	t_clReader_HalFuncs			*p_COMReaderHal;		/// COM Reader HAL functions
	t_clReader_HalFuncs			*p_BTReaderHal;			/// BT Reader HAL functions
	t_clReader_HalFuncs			*p_IPReaderDiscoverHal;	/// IP Reader Discover HAL functions
	t_clReader_HalFuncs			*p_COMReaderDiscoverHal;/// COM Reader Discover HAL functions
	t_clReader_HalFuncs			*p_BTReaderDiscoverHal;	/// BT Reader Discover HAL functions
	t_ConfigParams				tConfigParams;			/// Config parameters
	t_CryptoParams				tCryptoParams;			/// Crypto Parameters (used when invoking Crypto/OTA service
	t_DebugParams				tDebugParams;			/// debug parameters
}
t_clContext;


#endif /* CLSTRUCT_H_ */
/**************************************************/
