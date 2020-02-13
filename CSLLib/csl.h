/***************************************************************************/
/** @file	csl.h
* 					CLOVER-SECURED-LAYER
* @copyright				Copyright 2014
* @author F.DESPRES
* @n ************************************************************************
*	@name csl.h
*	@brief API and types definition for Clover Secured Layer
*	@n			 Manages
*	@n			* registration/data exchange with
*	@n				- readers
*	@n				- end devices
*	@n			* Cryptographic services
*	@n																		*/
/***************************************************************************/

#ifndef CSL_H_
#define CSL_H_
/***************************************************************************/
/* Generic definition IP Reader library										*/
/**************************************************************************/
#include "cltypes.h"		// contains portable types and defines
#include "clstructs.h"		// contains framework structures
#include "clhaldefs.h"		// contains function pointers definition

#ifdef DEBUG
	#include <stdio.h>
#endif

/*--------------------------------------------------------------------------*/

#define SIZE_STACK_THREAD (16*1024)

/*--------------------------------------------------------------------------*/

/***************************************************************************************/
/** @fn cl_sendData(  t_Device *pDevice, t_Reader *pReader, t_Tuple *pTuple
*		\n				e_CallType TsfType, clu32 *pTsfNb )
*
* @brief Application wants to send data list to End device via a specified Reader or in
*		\n	autonomous way (if pReader is NULL, this API tries to do it on its own)
*       \n This function prepares all related buffers to be sent to End Device and
*       \n encapsulates all calls to Reader/Service/Middleware/encryption to ease
*       \n customer development
*       \n it uses the function pointer fnSendDataToReader to send the data to the reader
*       \n a new call to this API will release memory allocated for previous call
* \n ****************************************************************************************
*  @param *pDevice			:(In) end device to address. Cannot be CL_NULL
*  @param *pReader       	:(In) Reader to address. Can be CL_NULL
*  @param *pTuple         	:(In) list of buffers to send. Cannot be CL_NULL
*  @param TsfType      		:(In) specifies if the call is blocking/non-blocking
* ---------------
*  @param *pTsfNb          : (Out) returns by the library. The transfer number is provided
*  \n                              by the library for callbacks treatment on completion of
*  \n                              transfer to know when the data was effectively issued
* @return e_Result
*  \n OK                        :  Result is OK
*  \n ERROR,                    : Failure on execution
*  \n MEM_ERR,                  :  Failure on memory management (failure,
*  \n                                 allocation ....)
*  \n PARAMS_ERR,               :  Inconsistent parameters
*  \n TIMEOUT_ERR,              :  Overrun on timing
*  \n TRANSFER_IN_PROCESS_ERR   :  A non blocking call is in progress. This
*  \n                             transfer is rejected. The user application
*  \n                             shall recall the API to ensure that this
*  \n                             request is proceeded
*************************************************************************************** */
e_Result cl_sendData( t_Device *pDevice, t_Reader *pReader, t_Tuple *pTuple, e_CallType TsfType, clu32 *pTsfNb );

/******************************************************************************************/
/** @fn : cl_getData( t_Device **ppDevice, t_Reader **ppReader, t_Tuple **ppTuple, e_CallType TsfType, clu32 *pTrfNb )
* \n
* @brief get Data from a reader (service data as an example) or end device
*       \n it is a blocking call of e_CallType is set to BLOCKING
*       \n if ppTuple is returned with NULL point no data is pushed back from underlayers
*       \n in the library the callback fnNetwData2Read_cb shall have return
*       \n first before calling this API if NON_BLOCKING was specified
*       \n All data shall be released from memory after usage
* ****************************************************************************************
*  @param 	**ppReader			: ( In ) Reader to address
*  \n                              if NULL pointer is passed, this call waits for any data
*  \n                              coming on any reader
*  @param	e_CallType TsfType   : ( In ) BLOCKING/NON BLOCKING call
* ---------------
*
*  @param 	**ppTuple      		: ( Out) pointer on read data from the reader
* \n                               if points to NULL, no data is coming from the underlayers
* @param 	**ppDevice			: ( Out) provide Device information where the data is coming from
* @param 	**ppReader     		: ( Out ) Provide Reader information where the data is coming from
* \n                               if NULL, no data is sent back
*  @param	*pTsfNb				: ( Out ) returns by the library. The transfer number is provided
* \n                               by the library for callbacks treatment on completion of
*  \n                              transfer to know when the data was effectively issued
* @return	e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* \n TIMEOUT_ERR,              :  Overrun on timing
* ****************************************************************************************/
e_Result cl_getData( t_Device **ppDevice, t_Reader **ppReader, e_CallType TsfType, t_Tuple **ppTuple, clu32 *pTrfNb );

/******************************************************************************************/
/** @fn : cl_getDataCompleted( t_Tuple *pTuple )
 * \n
* 	@brief library free the memory of buffers used from previous read calls
* \n***************************************************************************************
*  @param *pTuple 				: (In) tuple coming from network.
*  \n							. The memory of buffers is freed by the library after having
*  \n							used data read from network
* @return e_Result
*  \n OK                        :  Result is OK
*  \n ERROR,                    : Failure on execution
*  \n MEM_ERR,                  :  Failure on memory management (failure,
*  \n                                 allocation ....)
*  \n PARAMS_ERR,               :  Inconsistent parameters
*  \n TIMEOUT_ERR,              :  Overrun on timing
* \n TRANSFER_IN_PROCESS_ERR   :  A non blocking call is in progress. This
*  \n                             transfer is rejected. The user application
*  \n                             shall recall the API to ensure that this
*  \n                             request is proceeded
 *************************************************************************************** */
e_Result cl_getDataCompleted( t_Tuple *pTuple );

/*******************************************************************************************/
/*                                  LIBRARY FUNCTIONS                                       */
/******************************************************************************************/


/*******************************************************************************************/
/** @fn e_Result clInit( clSys_HalFunctions_s *pSysContext
* \n                        CALSSys_CallBackFunctions_s *pCallBacks,
* \n						t_MenuFileDef *ptMenuFileDef,
* \n						e_StackSupport eStackSupport
* @brief Initialize the library with function pointers and callbacks
* \n
*****************************************************************************************
*  @param *pSysContext 			: (In) function pointers for library
*      \n if NULL, no errors are returned but empty functions are implemented
*      \n per default
*      \n the library initialize locks/semaphores/network access in this call
*   @param *pCallBacks 			: (In) function pointers for
*      \n                  			callbacks signaled by the library
*   @param *ptMenuFileDef 		: (In) file to store persistent parameters
*   @param eStackSupport 		: (In) specifies if IP/COM/BT support is required
*
* @return e_Result
*  \n OK                        :  Result is OK
*  \n ERROR,                    : Failure on execution
*  \n MEM_ERR,                  :  Failure on memory management (failure,
*  \n                                allocation ....)
*  \n PARAMS_ERR,               :  Inconsistent parameters
******************************************************************************************* */
e_Result clInit( e_StackSupport eStackSupport, t_clSys_HalFunctions *pSysContext, \
		t_clSys_CallBackFunctions *pCallBacks, t_clSys_CallBackFunctions *ptCallbacksDiscover, \
		t_Reader* pReader, \
		t_clReader_HalFuncs	*p_IPReaderDefaultHAL, t_clReader_HalFuncs *p_IPReaderDiscoverDefaultHAL, \
		t_clReader_HalFuncs	*p_COMReaderDefaultHAL, t_clReader_HalFuncs	*p_COMReaderDiscoverDefaultHAL, \
		t_clReader_HalFuncs	*p_BTReaderDefaultHAL, t_clReader_HalFuncs	*p_BTReaderDiscoverDefaultHAL, \
		const t_MenuFileDef *ptMenuFileDef );

/*--------------------------------------------------------------------------*/

e_Result csl_InitGlobalContext( e_StackSupport eStackSupport, t_clSys_HalFunctions *pSysContext, \
		t_clSys_CallBackFunctions *pCallBacks, t_clSys_CallBackFunctions *ptCallbacksDiscover, t_Reader* pReader, \
		t_clReader_HalFuncs *p_IPReaderDefaultHAL, t_clReader_HalFuncs *p_IPReaderDiscoverDefaultHAL, \
		t_clReader_HalFuncs	*p_COMReaderDefaultHAL, t_clReader_HalFuncs	*p_COMReaderDiscoverDefaultHAL, \
		t_clReader_HalFuncs	*p_BTReaderDefaultHAL, t_clReader_HalFuncs	*p_BTReaderDiscoverDefaultHAL, \
		const t_MenuFileDef *ptMenuFileDef, \
		cl8 *pFullPath);

/*--------------------------------------------------------------------------*/

e_Result csl_InitThreads( void );

/******************************************************************************************/
/* @fn e_Result csl_Close( clvoid );
* @brief : close the library and performs memory release/network closure...
* ***************************************************************************************
* @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
*                                  allocation ....)
*************************************************************************************** */
e_Result csl_Close( clvoid );

/*****************************************************************************/
/** @fn  e_Result cl_PrintOutCompletion(  clvoid );
* 	@brief : printf completion to fflush as an example
* **************************************************************************
* @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* ************************************************************************** */
e_Result cl_PrintOutCompletion( clvoid );

/*******************************************************************************************/
/* Name : e_Result cl_GetContext( t_clContext **ppCtxt )                                      */
/* Description :                                                                            */
/*        return function pointers on the CSL functions provided by the customer            */
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : t_CslContext **pCtxt                                                                               */
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*  MEM_ERR,                  :  Failure on memory management (failure,                     */
/*                                  allocation ....)                                        */
/*******************************************************************************************/
e_Result cl_GetContext( t_clContext **ppCtxt );

/*******************************************************************************************/
/* Name : e_Result cl_FreeMem( clvoid *pData )					                             */
/* Description :                                                                            */
/*        free memory from user land	            										*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : clvoid *pData : pointer to buffer to free											 */
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*  MEM_ERR,                  :  Failure on memory management (failure,                     */
/*                                  allocation ....)                                        */
/*******************************************************************************************/
e_Result cl_FreeMem( clvoid *pData );

/****************************************************************************************/
/*            				TUPLE ACCESS API	   										*/
/****************************************************************************************/

/*******************************************************************************************/
/* Name : e_Result cl_initTuple(  t_Tuple *pTuple,  clu8 *pu8Data, clu32 u32Len )			*/
/* Description :                                                                            */
/*        initialize a tuple using the default flags and a buffer of data (memory is		*/
/*		allocated by the caller if necessary												*/
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
e_Result cl_initTuple( t_Tuple *pTuple, t_Buffer *pBuff, clu8 **ppu8Data, clu32 u32Len );

/****************************************************************************************/
/*            				READERS ACCESS API	   										*/
/****************************************************************************************/
/*******************************************************************************************/
/* Name : e_Result cl_ReaderFillWithDefaultFields(  t_Reader *p_Reader, e_Reader eType ) */
/* Description :                                                                            */
/*        fill current reader structure with default params according to default params		*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*			t_Reader *p_Reader	: reader to fill											*/
/*			e_ReaderType	eType: Type of Reader to fill									*/
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*******************************************************************************************/
e_Result cl_ReaderFillWithDefaultFields( t_Reader *pReader, e_ReaderType eType );

/**************************************************************************/
/* Name : e_Result cl_readerAddToList( t_Reader *pReader);             */
/* Description :                                                          */
/*        Add pReader to the list of Readers							*/
/**************************************************************************/
/* Parameters:                                                            */
/*  --------------                                                        */
/*  In: 	t_Reader *pReader: new reader to add to List			    */
/*                           	cannot be NULL                                 */
/* ---------------                                                        */
/*  Out: none                                                             */
/* Return value: e_Result                                                 */
/*  CL_OK                        :  Result is OK                             */
/*  CL_ERROR,                    : Failure on execution                      */
/*  CL_MEM_ERR,                  :  Failure on memory management (failure,   */
/*                                  allocation ....)                      */
/*  CL_PARAMS_ERR,               :  Inconsistent parameters                  */
/**************************************************************************/
e_Result cl_readerAddToList( t_Reader *pReader);

/**************************************************************************/
/* Name : e_Result cl_readerFindInList( t_Reader **ppReader, tReader *pReaderFilter )             */
/* Description :                                                         	 */
/*        Find a reader in a list											*/
/**************************************************************************/
/* Parameters:                                                            	*/
/*  --------------                                                        	*/
/*  In: 	t_Reader *pReaderFilter: if one of the field is not CL_NULL,   */
/*			then the filtering will happen on this field					*/
/*                           	cannot be NULL                              */
/* ---------------                                                        	*/
/*  Out: 	t_Reader **ppReader: returns the pointer on filtered element */
/* Return value: e_Result                                                 	*/
/*  CL_OK                        :  Result is OK                             */
/*  CL_ERROR,                    : Failure on execution or list of readers 	*/
/*												is empty                      */
/*  CL_MEM_ERR,                  :  Failure on memory management (failure,   */
/*                                  allocation ....)                      	*/
/*  CL_PARAMS_ERR,               :  Inconsistent parameters                  */
/**************************************************************************/
e_Result cl_readerFindInList( t_Reader **ppReader, t_Reader *pReaderFilter );

/*--------------------------------------------------------------------------*/

e_Result cl_readerFindInListByFriendlyName( t_Reader **ppReader, char *aReaderFriendlyName );

/*--------------------------------------------------------------------------*/

e_Result cl_readerFindInListByAddress( t_Reader **ppReader, t_Reader *pReaderAdress );

/*--------------------------------------------------------------------------*/

e_Result cl_ReaderStartThreads( t_Reader *ptReader );


/**************************************************************************/
/* Name : e_Result cl_getReader_List( t_Reader **pptReaderList);                */
/* Description :                                                          */
/*        this API is used to get the readers list available in the system*/
/**************************************************************************/
/* Parameters:                                                            */
/*  --------------                                                        */
/*  In/Out: tReader **pptReaderList: chained list of readers in the system */
/*                           	can be NULL                                 */
/* ---------------                                                        */
/*  Out: none                                                             */
/* Return value: e_Result                                                  */
/*  OK                        :  Result is OK                             */
/*  ERROR,                    : Failure on execution                      */
/*  MEM_ERR,                  :  Failure on memory management (failure,   */
/*                                  allocation ....)                      */
/*  PARAMS_ERR,               :  Inconsistent parameters                  */
/**************************************************************************/
e_Result cl_getReader_List( t_Reader **pptReaderList);
/****************************************************************************************/
/*            				DEVICES ACCESS API	   										*/
/****************************************************************************************/

/**************************************************************************/
/* Name : e_Result cl_DeviceAddToList( t_Device *pDevice);             		*/
/* Description :                                                          	*/
/*        Add pDevice to the list of Devices and link it to a Reader		*/
/**************************************************************************/
/* Parameters:                                                            	*/
/*  --------------                                                        	*/
/*  In: 	t_Device *pDevice: new Device to add to List			    	*/
/*                           	cannot be NULL                              */
/*			t_Reader *pReader: Reader that the device is linked to			*/
/* ---------------                                                        	*/
/*  Out: none                                                             	*/
/* Return value: e_Result                                                 	*/
/*  CL_OK                        :  Result is OK                             */
/*  CL_ERROR,                    : Failure on execution                      */
/*  CL_MEM_ERR,                  :  Failure on memory management (failure,   */
/*                                  allocation ....)                      	*/
/*  CL_PARAMS_ERR,               :  Inconsistent parameters                  */
/**************************************************************************/
e_Result cl_DeviceAddToList( t_Device *pDevice, t_Reader *pReader );

/*******************************************************************************************/
/* Name : e_Result cl_ReaderSetState(  e_ReaderState eState ) 								*/
/* Description :                                                                            */
/*        set reader state	from a  reader friendly name and type							*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*			t_Reader	*pReader : set a reader to a state (CONNECTED/UNCONNECTED/OTA/R_W... */
/*			e_ReaderState eState : set a reader to a state (CONNECTED/UNCONNECTED/OTA/R_W... */
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  CL_ERROR                  :  Error on call    											*/
/*******************************************************************************************/
e_Result cl_ReaderSetState( t_Reader *pReader, e_State eState );

/*******************************************************************************************/
/* Name : e_Result cl_ReaderGetState(  e_ReaderState eState ) 								*/
/* Description :                                                                            */
/*        get reader state from its friendly name and type									*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*			e_ReaderState eState : set a reader to a state (CONNECTED/UNCONNECTED/OTA/R_W... */
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  CL_ERROR                  :  Error on call    											*/
/*******************************************************************************************/
e_Result cl_ReaderGetState( t_Reader *pReader, e_State *p_eState );


/******************************************************************************************/
/*            				PERSISTENT STORAGE API	   									*/
/******************************************************************************************/

/*******************************************************************************************/
/* Name : e_Result cl_SetParams( clu8 	*pParams, clu32 u32ParamsLen, clu8	**ppValue		*/
/* 												, clu32 *pu32ValueLen	)					*/
/* Description :                                                                            */
/*        from an application, get access to a variable and retreive its values				*/
/*		Caution:!!!! this design is for one application only. No support of multi-thread	*/
/*				or multi-application is possible!!!!! 										*/
/*		Additional semaphore must be set in place outside CSL								*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : 																					*/
/*			clu8 	*pParams	: string containing the variable to write					*/
/*			clu32	u32ParamsLen: Length of Parameter to get value from						*/
/*			clu8	*ppValue	: value to set in file										*/
/*			clu32	u32ValueLen : length of value to write									*/

/* 	Return value: e_Result                                                                  */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result	cl_SetParams( clu8 	*pParams, clu32 u32ParamsLen, clu8	*pValue, clu32 u32ValueLen );
/*******************************************************************************************/
/* Name : e_Result cl_GetParams( clu8 	*pParams, clu32 u32ParamsLen, clu8	*pValue			*/
/* , clu32 u32ValueLen )																	*/
/* Description :                                                                            */
/*        from an application, get access to a variable and retreive its values				*/
/*		Caution:!!!! this design is for one application only. No support of multi-thread	*/
/*				or multi-application is possible!!!!! 										*/
/*		Additional semaphore must be set in place outside CSL								*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : 																					*/
/*			clu8 	*pParams	: string containing the variable to write					*/
/*			clu32	u32ParamsLen: Length of Parameter to get value from						*/
/*			clu8	u8RemoveTrailerSpace : remove any space before data 					*/
/*	Out :																					*/
/*			clu8	**ppValue	: value returned from file read								*/
/*			clu32	*pu32ValueLen : length of read value									*/
/*	Out :	none 																			*/
/* 	Return value: e_Result                                                                  */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*******************************************************************************************/
e_Result cl_GetParams( clu8 *pParams, clu32 u32ParamsLen, clu8	**ppValue, clu32 *pu32ValueLen, clu8 clbRemoveTrailerSpace);

/******************************************************************************************/
/*            								OTA SERVICE API    								*/
/******************************************************************************************/
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
e_Result cl_startOTA( t_Reader *pEncryptionKeyReader, t_Reader *p_TargetReader);

/******************************************************************************************/
/*            								CRYPTO API    									*/
/******************************************************************************************/


/*******************************************************************************************/
/** @fn : e_Result clDefineKeysSet( t_Device *pDevice, t_Buffer *pPwd , e_OptionsKey eKeyType  );
* @brief :
* \n       define a set of 8 keys and config_word (key usage of these 8keys)  in the Clover
* \n       system from a password generated by the end user

* @param *pDevice				: ( In ) designate the End Device where to apply to key set/config word
* @param *pPwd					: ( In ) buffer containing the password
* \n               					this Buffer shall be 16bytes long
* \n               					if NULL or length of Buffer is not 16bytes, PARAMS_ERR is returned
* @param  eKeyType:				: ( In )
* \n             				if PROD_KEY: the Production Key subset is selected, config_word of
* \n             				is selected
* \n             				if NEW_KEY: A new subset is selected, a new config_word is selected in the
* \n             				key subset to encrypt the medium

*  @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* \n TIMEOUT_ERR,              :  Overrun on timing
* *************************************************************************************** */
e_Result clDefineKeysSet( t_Device *pDevice, t_Buffer *pPwd , e_OptionsKey eKeyType );

/*******************************************************************************************/
/** @fn : e_Result cl_DeviceSetEncryptionLevel( t_Device *pDevice, e_OptionsKey eKeyType  );
*  @brief : define default encryption level for a device
*  \n      if in the t_Tuple, encryption set is not set (DEFAULT)
* ***************************************************************************************
* @param *pDevice				: ( In ) designate the End Device where to set default encryption level
* @param eDefaultCryptValue		: ( In )
* \n             if NONE: no encryption is set between end-user application and end-device
* \n             if CRYPTO_APPLI : only Application layer of radio datagrams is encrypted
* \n             if CRYPTO_PHY : PHY layer and APPLICATION Layer at radio levels are encrypted

* @return e_Result
* \n OK                        :  Result is OK
* \n ERROR,                    : Failure on execution
* \n MEM_ERR,                  :  Failure on memory management (failure,
* \n                                 allocation ....)
* \n PARAMS_ERR,               :  Inconsistent parameters
* *************************************************************************************** */
e_Result cl_DeviceSetEncryptionLevel( t_Device *pDevice, e_Crypted eDefaultCryptValue );


/***************************************************************************/
/*  DEBUG : AUTO-COMPLETION for INTERPRETATION */
/***************************************************************************/
/*typedef struct st_DebugInfo
{
  clu32 u32DefineValue;         // Value of define
  clu8 *pu8StringRep;           // 'String" representation of the define
  clu8 *pu8FullDescription;     // full description of the command
}t_DebugInfo;
*/
//------------------------------------------------------
// Debug info
//------------------------------------------------------
/*t_DebugInfo atDebugInfo[] =
{
  { TSP_LAYER_SET_LOCAL,          "TSP_LAYER_SET_LOCAL",            "Set configuration in reader" },
  { TSP_LAYER_SET_DEVICE,         "TSP_LAYER_SET_DEVICE",           "Send data to the device (framework)" },
  { TSP_LAYER_ANSW_DEVICE,        "TSP_LAYER_ANSW_DEVICE",          "Get data from device (framework)" },
  { TSP_LAYER_REQ_CLV_STACK,      "TSP_LAYER_REQ_CLV_STACK",        "Request to Clover Stack " },
  { TSP_LAYER_ANSW_CLV_STACK,     "TSP_LAYER_ANSW_CLV_STACK",       "Answer from Clover Stack" },
  { TSP_LAYER_REQ_LOCAL_SVC,      "TSP_LAYER_REQ_LOCAL_SVC",        "Request local service" },
  { TSP_LAYER_ANSW_LOCAL_SVC,     "TSP_LAYER_ANSW_LOCAL_SVC",       "Answer from local service" },
  { TSP_LAYER_ANSW_ERR_LOCAL_SVC, "TSP_LAYER_ANSW_ERR_LOCAL_SVC",   "Error from local service" },
  { TSP_LAYER_REQ_LOCAL_MW,       "TSP_LAYER_REQ_LOCAL_MW",         "Request to Local Middleware" },
  { TSP_LAYER_ANSW_LOCAL_MW,      "TSP_LAYER_ANSW_LOCAL_MW",        "Answer from Local Middleware" },
  { TSP_LAYER_ERR_MSG,            "TSP_LAYER_ERR_MSG",              "Error Message" },

};

t_DebugFunctionInfo atDebugFnInfo[] =
{


}
*/

/** @def DEBUG_PRINTF( x... )
 * @brief	placeholder for Debug printout (can be mapped to serial port, stdout....
 */
#ifdef JAMAIS

#define DEBUG_PRINTF( x... )	{	printf( x ); cl_PrintOutCompletion();};	// used for portability- with Eclispe and MinGW, printf is blocked.
																		// A completion function is used to fflush stoud and enable the printout in the console

#else

extern t_clContext g_tCtxt;

#define DEBUG_PRINTF g_tCtxt.ptHalFuncs->fnTrace0
#define DEBUG_PRINTF0 g_tCtxt.ptHalFuncs->fnTrace0
#define DEBUG_PRINTF1 g_tCtxt.ptHalFuncs->fnTrace1 // deeper in debuging

#define csl_malloc(x, n)  g_tCtxt.ptHalFuncs->fnAllocMem(x, n, __FILE__, __LINE__)
#define csl_pmalloc(x, n) pCtxt->ptHalFuncs->fnAllocMem(x, n, __FILE__, __LINE__)

#endif

#ifdef CSL_C
#include "clsupport.h"

// initialization of the callbacks to retreive data
t_clSys_CallBackFunctions g_tCallbacks =
{
        .fnIOData2Read_cb 		= 	cl_IOData2ReadWin32_cb,			/// callback used when receiving data from IOs level
        .fnIOSendDataDone_cb	=	cl_IOSendDataDoneWin32_cb	,	/// callback to signal that the data was effectively send to the IOs level
        .fnReset_cb				= 	cl_IOResetWin32_cb,				/// callback to signal end of reset of IOs level
        .fnIOState_cb           =   cl_IOStateWin32_cb,				/// callback to signal change of state of CSL framework/readers/devices
        .fnOTAProgress_cb		=	cl_OTAProgress_cb,				///	callback to signal change in the OTA process
        .fnOTASendDataDone_cb	=	cl_OTASendDataDone_cb,			/// callback to signal that data was sent to reader while in OTA process
        .fnOTAData2Read_cb		=	cl_OTAData2Read_cb,				/// callback to signal that data was read from reader while in OTA process
};

// initialization of the callbacks to detect readers
t_clSys_CallBackFunctions g_tCallbacksDiscover =
{
        .fnIOData2Read_cb 		= 	cl_DiscoverData2ReadWin32_cb,			/// callback used when receiving data from IOs level
        .fnIOSendDataDone_cb	=	cl_DiscoverSendDataDoneWin32_cb	,		/// callback to signal that the data was effectively send to the IOs level
        .fnReset_cb				= 	cl_IOResetWin32_cb,					/// callback to signal end of reset of IOs level
        .fnIOState_cb           =   cl_IOStateWin32_cb,					/// callback to signal change of state of CSL framework/readers/devices
        .fnOTAProgress_cb		=	CL_NULL,							///	callback to signal change in the OTA process
        .fnOTASendDataDone_cb	=	CL_NULL,							/// callback to signal that data was sent to reader while in OTA process
        .fnOTAData2Read_cb		=	CL_NULL,							/// callback to signal that data was read from reader while in OTA process

};

// initialization of the abstraction layer to comply to the OS requirements.
t_clSys_HalFunctions g_tHalFunc =
{
        .fnCreateThread 		= 	cl_CreateThreadWin32	,
        .fnDestroyThread 		= 	cl_DestroyThreadWin32	,
        .fnDestroyThreadAsked	= 	cl_DestroyThreadRequestedWin32,
        .fnAllocMem 			=	cl_AllocMemWin32    	,
        .fnFreeMem 				= 	cl_FreeMemWin32			,
		.fnFreeMemSafely		= 	cl_FreeMemSafelyWin32		,
        .fnSemaphoreCreate		= 	cl_SemaphoreCreateWin32 ,
        .fnSemaphoreRelease 	= 	cl_SemaphoreReleaseWin32,
        .fnSemaphoreWait 		= 	cl_SemaphoreWaitWin32	,
        .fnSemaphoreDestroy		= 	cl_SemaphoreDestroyWin32,
		.fnMutexCreate			=   cl_MutexCreateWin32,
		.fnMutexDestroy			= 	cl_MutexDestroyWin32,
        .fnWaitMs 				= 	cl_WaitMsWin32,
        .fnGetTime				=	cl_GetTimeWin32,
        .fnFileOpen				=   cl_FlOpenWin32,
        .fnFileWrite			=	cl_FlWriteWin32,
        .fnFileReadLn			=	cl_FlReadLnWin32,
        .fnFileGetPos			=	cl_FlGetPosWin32,
        .fnFileSetPos			=	cl_FlSetPosWin32,
        .fnFileClose			=	cl_FlCloseWin32,
        .fnFileGetChar			=	cl_FileGetChrWin32,
        .fnPrintComplete		= 	cl_PrintCompleteWin32,
        .fnIPStackSupportLoad	=	cl_IPStackSupportLoadWin32,
        .fnIPStackSupportUnload	=	cl_IPStackSupportUnloadWin32,
        .fnCOMStackSupportLoad	=	cl_COMStackSupportLoadWin32,
        .fnCOMStackSupportUnload=	cl_COMStackSupportUnloadWin32,
        .fnBTStackSupportLoad	=	CL_NULL,
        .fnBTStackSupportUnload	=	CL_NULL,
		.fnTrace0               =   cl_Trace,
		.fnTrace1               =   cl_Trace,
		.fnStackTrace			= 	cl_StackTrace,
        .fn_GetElapsedTime		=	cl_GetElapsedTimeHighPerformanceWin32,
};

// initialization of HAL functions for IP readers
t_clReader_HalFuncs	g_IPReaderDefaultHAL =
{
		.fnIORegister 			= 	cl_NetwRegisterWin32	,
		.fnIOUnregister 		= 	cl_NetwUnregisterWin32	,
		.fnIOGetDNSTable 		= 	CL_NULL, //cl_NetwGetDNSTableWin32	,
		.fnIOGetData 			= 	cl_NetwGetDataWin32		,
		.fnIOReset 				= 	cl_NetwResetWin32		,
		.fnIOSendData 			= 	cl_NetwSendDataWin32	,
		.fnSetIPRange			= 	cl_SetIPRangeWin32,
		.fnIOOpenConnection		=	cl_NetwOpenConnectionWin32,
		.fnIOCloseConnection	=	cl_NetwCloseConnectionWin32,
};

// initialization of HAL functions for discover functionnality for IP reader
t_clReader_HalFuncs	g_IPReaderDiscoverDefaultHAL =
{
		.fnIORegister 			= 	cl_NetwRegisterWin32	,
		.fnIOUnregister 		= 	cl_NetwUnregisterWin32	,
		.fnIOGetDNSTable 		= 	CL_NULL, //cl_NetwGetDNSTableWin32	,
		.fnIOGetData 			= 	cl_NetwGetDataDiscoverWin32		,
		.fnIOReset 				= 	cl_NetwResetWin32		,
		.fnIOSendData 			= 	cl_NetwSendDataDiscoverWin32	,
		.fnSetIPRange			= 	cl_SetIPRangeWin32,
		.fnIOOpenConnection		=	cl_NetwOpenConnectionDiscoverWin32,
		.fnIOCloseConnection	=	cl_NetwCloseConnectionWin32,
};

// initialization of HAL functions for IP readers
t_clReader_HalFuncs	g_COMReaderDefaultHAL =
{
		.fnIORegister 			= 	cl_COMAlRegisterWin32	,
		.fnIOUnregister 		= 	cl_COMAlUnregisterWin32	,
		.fnIOGetDNSTable 		= 	CL_NULL, //cl_NetwGetDNSTableWin32	,
		.fnIOGetData 			= 	cl_COMAlGetDataWin32	,
		.fnIOReset 				= 	cl_COMAlResetWin32		,
		.fnIOSendData 			= 	cl_COMAlSendDataWin32	,
		.fnSetIPRange			= 	CL_NULL,
		.fnIOOpenConnection		=	cl_COMAlOpenConnectionWin32,
		.fnIOCloseConnection	=	cl_COMAlCloseConnectionWin32,
};
// initialization of HAL functions for discover functionnality for COM reader
t_clReader_HalFuncs	g_COMReaderDiscoverDefaultHAL =
{
		.fnIORegister 			= 	cl_COMAlRegisterWin32	,
		.fnIOUnregister 		= 	cl_COMAlUnregisterWin32	,
		.fnIOGetDNSTable 		= 	CL_NULL, //cl_NetwGetDNSTableWin32	,
		.fnIOGetData 			= 	cl_COMAlGetDataWin32	,
		.fnIOReset 				= 	cl_COMAlResetWin32		,
		.fnIOSendData 			= 	cl_COMAlSendDataWin32	,
		.fnSetIPRange			= 	CL_NULL,
		.fnIOOpenConnection		=	cl_COMAlOpenConnectionWin32,
		.fnIOCloseConnection	=	cl_COMAlCloseConnectionWin32,
};
// initialization of default reader used by the framework
t_Reader g_tCSLDefaultReader=
{
        (e_ReaderType) (COM_READER_TYPE),
        STATE_DEFAULT,              	// this is the default state of a reader when created. Always the same
        "Reader on COM128", 			// explicit name
        CL_NULL,						// handle to reader  (filled by software)
        {	// IP Settings
                "192.168.0.201", 		//		192.168.0.201
                0x00000080A3923646, 	//Mac Address
                IP_READER_DEFAULT_TCP_PORT,	// connection port: telnet
                {
                	0xC0A80001,			// start IP address of the range
                	0xC0A800FF,			// stop IP address of the range to scan
                	IP_READER_DEFAULT_TCP_PORT,		// port to scan on
                	CL_NULL,			// pointer to a linked list of range if any (none here)
                },
        },
        {	// COM settings
                "COM128",				// com port to connect to
                CL_COM_BYTESIZE_8BITS,	// Byte size for transfer
                CL_COM_STOPBITS_10BIT,	// 0 stop bit
                CL_COM_PARITYBIT_NONE, 	// Parity bit none
                CL_COM_BAUDRATE_115200, // baud rate
                50,						// read intervall timeout
                50,						// read total timeout base
                10,						// read total timeout multiplier
                50,						// write total timeout base
                10,						// write total timeout mutliplier
                {
                	27,					// start COM port to connect to : 0
                	35,				// end of range of COM port to connect to : 256
                	CL_NULL,			// TCP port: N/A
                	CL_NULL,			// Linked list with an other range of readers: N/A
                },
        },
        {	// BT settings
                0,
                {
                	CL_NULL,			// start BT port to connect to : N/A
                	CL_NULL,			// end of range of COM port to connect to : N/A0
                	CL_NULL,			// TCP port: N/A
                	CL_NULL,			// Linked list with an other range of readers: N/A
                },

        },
        { // tRdrSynchro
                CL_NULL,	// Thread Id For Read Operations : auto filled when adding the reader to the list of readers
                CL_NULL,	// Thread Id For Write Operations : auto filled when adding the reader to the list of readers
                CL_NULL,	// Signal For Read Operations : auto filled when adding the reader to the list of readers
                CL_NULL,	// Signal For Write Operations : auto filled when adding the reader to the list of readers
                CL_NULL, 	//	signal that an underlayer write was performed
                CL_NULL,	// signal to synchronize Write and Read threads
                CSL_SERIAL_MAX_TIMEOUT,	// max timeout allowed on low level data
                CSL_SERIAL_MAX_REPEATS, /// hold retries count for serial low layer repeat protocol
                CL_SERIAL_PROT_DEFAULT,	// default status of synchronization between write and read threads
                NON_BLOCKING, // specify BLOCKING read or asynchronous read via callbacks
        },
        CL_NULL,			// Tuple list to be sent and attached to this reader
        CL_NULL,			// Tuple list to be read and attached to this reader
        {	// STANDARD TRANSMISSION
                CL_NULL,		// callback used on completion of reset of network stack
                CL_NULL,		// callback used when data was pushed out to the reader
                CL_NULL,		// callback used when data is coming from reader
                CL_NULL,		// callback used to signal that the reader changed of state
        },
        {	// DISCOVER CALLBACKS
                CL_NULL,		// callback used on completion of reset of network stack
                CL_NULL,		// callback used when data was pushed out to the reader
                CL_NULL,		// callback used when data is coming from reader
                CL_NULL,		// callback used to signal that the reader changed of state
        },
        {		// TCP or COM standard DATA
                CL_NULL,		// Use default from framework passed in clInit: register a reader from library to porting layers
                CL_NULL,		// Use default from framework passed in clInit: unregister a reader from library to porting layers
                CL_NULL,		// Use default from framework passed in clInit: reset connection to reader
                CL_NULL,		// Use default from framework passed in clInit: open connection
                CL_NULL,		// Use default from framework passed in clInit: send data
                CL_NULL,		// Use default from framework passed in clInit: get data
                CL_NULL,		// Use default from framework passed in clInit: close connection to reader
                CL_NULL,		// Use default from framework passed in clInit: get DNS table
                CL_NULL,		// Use default from framework passed in clInit: set ip range
        },
        {		// DISCOVER
                CL_NULL,		// Use default from framework passed in clInit: register a reader from library to porting layers
                CL_NULL,		// Use default from framework passed in clInit: unregister a reader from library to porting layers
                CL_NULL,		// Use default from framework passed in clInit: reset connection to reader
                CL_NULL,		// Use default from framework passed in clInit: open connection
                CL_NULL,		// Use default from framework passed in clInit: send data
                CL_NULL,		// Use default from framework passed in clInit: get data
                CL_NULL,		// Use default from framework passed in clInit: close connection to reader
                CL_NULL,		// Use default from framework passed in clInit: get DNS table
                CL_NULL,		// Use default from framework passed in clInit: set ip range
        },
        {
        		{ 0, 0, 0, 0, 0, 0 },	/// default radio address of the clover sense in the reader
        		CL_NULL,			/// pointer on next clover sense board in the reader
        },
        CL_NULL			// Pointer on next reader : auto filled when adding the reader to the list of Readers
};


// initialization of persistent storage files.
t_MenuFileDef g_tMenuFileDef =
{
        ".\\menu_description.xml",		// get the description of the menu
        ".\\var_description.xml",		// get the description of the variables ( type, len....)
        "menu_var_values.xml",		// get the values of the variables

};

#else	// CSL_C
// initialization of the callbacks to retreive data
extern t_clSys_CallBackFunctions g_tCallbacks;

// initialization of the callbacks for the automatic discover services
extern t_clSys_CallBackFunctions g_tCallbacksDiscover;

// initialization of the abstraction layer to comply to the OS requirements.
extern t_clSys_HalFunctions g_tHalFunc;

// default reader used in CSL framework
extern t_Reader g_tCSLDefaultReader;

// definition of the IP reader Abstraction Layer
extern t_clReader_HalFuncs	g_IPReaderDefaultHAL;

// definition of the IP reader Abstraction Layer for discover process
extern	t_clReader_HalFuncs	g_IPReaderDiscoverDefaultHAL;

// definition of the COM reader Abstraction Layer
extern t_clReader_HalFuncs	g_COMReaderDefaultHAL;

// definition of the COM readers Abstraction Layer for discover process
extern	t_clReader_HalFuncs	g_COMReaderDiscoverDefaultHAL;

// definition of the persistent storage files
extern t_MenuFileDef g_tMenuFileDef;

#endif // CSL_C

#endif // CSL_H_
