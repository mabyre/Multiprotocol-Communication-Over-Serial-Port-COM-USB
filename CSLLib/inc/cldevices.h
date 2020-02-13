/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: cldevices.h                                                        */
/*                                                                           */
/*  Created on: 1 avril 2015                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/*  Description : routines to handle list of devices and properties          */
/*****************************************************************************/
#ifndef CLDEVICES_H_
#define CLDEVICES_H_

/*******************************************************************************************/
/* Name : e_Result cl_initDeviceSetDefaultFields( e_Crypted eCryptLink, 					*/
/*				clu64 u64DefaultIpAddr, clu8 *pucDeviceName, clu32 u32DeviceNameLen )     	*/
/* Description :                                                                            */
/*        init Device structure with default elements							            */
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*		e_Crypted eCryptLink            // specify default encryption mode for this device	*/
/*		clu64 u64DefaultIpAddr			// specify clover address of the device				*/
/*		clu8*        pucDeviceName;     // name of the Device								*/
/*		clu32        u32DeviceNameLen;  // length of the Device								*/
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*******************************************************************************************/
e_Result cl_initDeviceSetDefaultFields( e_Crypted eCryptLink, clu64 u64DefaultIpAddr, clu8 *pucDeviceName, clu32 u32DeviceNameLen );


/**************************************************************************/
/* Name : e_Result cl_DeviceFindInList( t_Device **ppDevice, tDevice *pDeviceFilter )             */
/* Description :                                                         	 */
/*        Find a Device in a list											*/
/**************************************************************************/
/* Parameters:                                                            	*/
/*  --------------                                                        	*/
/*  In: 	t_Device *pDeviceFilter: if one of the field is not CL_NULL,   */
/*			then the filtering will happen on this field					*/
/*                           	cannot be NULL                              */
/* ---------------                                                        	*/
/*  Out: 	t_Device **ppDevice: returns the pointer on filtered element */
/* Return value: e_Result                                                 	*/
/*  CL_OK                        :  Result is OK                             */
/*  CL_ERROR,                    : Failure on execution or list of Devices 	*/
/*												is empty                      */
/*  CL_MEM_ERR,                  :  Failure on memory management (failure,   */
/*                                  allocation ....)                      	*/
/*  CL_PARAMS_ERR,               :  Inconsistent parameters                  */
/**************************************************************************/
e_Result cl_DeviceFindInList( t_Device **ppDevice, t_Device *pDeviceFilter );

/**************************************************************************/
/* Name : e_Result cl_DeviceRemoveFromList( t_Device *pDevice)             */
/* Description :                                                         	 */
/*        Remove a Device from the list										*/
/**************************************************************************/
/* Parameters:                                                            	*/
/*  --------------                                                        	*/
/*  In: 	t_Device *pDevice: Remove the Device from the list. NULL not 	*/
/*			allowed															*/
/* ---------------                                                        	*/
/*  Out: 																	 */
/* Return value: e_Result                                                 	*/
/*  CL_OK                        :  Result is OK                             */
/*  CL_ERROR,                    : Failure on execution or list of Devices 	*/
/*												is empty                      */
/*  CL_MEM_ERR,                  :  Failure on memory management (failure,   */
/*                                  allocation ....)                      	*/
/*  CL_PARAMS_ERR,               :  Inconsistent parameters                  */
/**************************************************************************/
e_Result cl_DeviceRemoveFromList( t_Device *pDevice );



/*******************************************************************************************/
/* Name : e_Result cl_SetDeviceListEntry( t_Device *pDevice )                                */
/* Description :                                                                            */
/*        set first element of Devices list													*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In : t_Reader *pReader                                                                               */
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                    */
/*  OK                        :  Result is OK                                               */
/*  ERROR,                    : Failure on execution                                        */
/*  MEM_ERR,                  :  Failure on memory management (failure,                     */
/*                                  allocation ....)                                        */
/*******************************************************************************************/
e_Result cl_SetDeviceListEntry( t_Device *pDevice );

/*******************************************************************************************/
/* Name : e_Result cl_DeviceSetState( t_Device *pDevice, e_State eState ) 					*/
/* Description :                                                                            */
/*        set reader state	from a  reader friendly name and type							*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*			t_Device	*pDevice : set a device to a state (CONNECTED/UNCONNECTED/OTA/R_W... */
/*			e_ReaderState eState : set a device to a state (CONNECTED/UNCONNECTED/OTA/R_W... */
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  CL_ERROR                  :  Error on call    											*/
/*******************************************************************************************/
e_Result cl_DeviceSetState( t_Device *pDevice, e_State eState );

/*******************************************************************************************/
/* Name : e_Result cl_DeviceGetState(  e_State eState ) 									*/
/* Description :                                                                            */
/*        get device state from its friendly name and type									*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*			t_Device	*pDevice : get device  state (CONNECTED/UNCONNECTED/OTA/R_W... 		*/
/*			e_State eState : set a reader to a state (CONNECTED/UNCONNECTED/OTA/R_W... 		*/
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  CL_ERROR                  :  Error on call    											*/
/*******************************************************************************************/
e_Result cl_DeviceGetState( t_Device *pDevice, e_State *p_eState );

#endif /* CLDEVICES_H_ */
/**************************************************/
