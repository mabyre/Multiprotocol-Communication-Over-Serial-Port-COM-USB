/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: cldevices.c                                                        */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/*  Description: routines to handle list of devices and properties           */
/*****************************************************************************/
#define _CL_DEVICES_C_

#include <stdio.h>
#include "..\csl.h"
#include "..\inc\cldevices.h"

#include "..\inc\clthread.h"
#include "..\inc\generic.h"
#include "..\inc\clreaders.h"

t_Device g_tDefaultDevice;

/*******************************************************************************************/
/* Name : e_Result cl_initDeviceSetDefaultFields( e_Crypted eCryptLink, 					*/
/*										clu64 u64DefaultIpAddr, clu8 *pucDeviceName,		*/
/*										clu32 u32DeviceNameLen )							*/
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
e_Result cl_initDeviceSetDefaultFields( e_Crypted eCryptLink, clu64 u64DefaultIpAddr, clu8 *pucDeviceName, clu32 u32DeviceNameLen  )
{
	clu32 i = 0;

	if ( u32DeviceNameLen > sizeof( g_tDefaultDevice.aucLabel ) )
		return ( CL_PARAMS_ERR );


	// Crypted link is set to required value (ex: NONE, APPLI, PHY)
	g_tDefaultDevice.e_CryptedLink = eCryptLink ;

	// next device is set to NULL per default
	g_tDefaultDevice.ptNext = CL_NULL;

	// connected reader is set to NULL per default
	g_tDefaultDevice.ptReader = CL_NULL;

	// Clover Network Address is set to u64DefaultIpAddr
	g_tDefaultDevice.u64Addr = u64DefaultIpAddr;

	// remote name of device is set to default value
	for (i =0; i < u32DeviceNameLen; i++)
		g_tDefaultDevice.aucLabel[i] = pucDeviceName[i];


	return ( CL_OK );
}

/*******************************************************************************************/
/* Name : e_Result cl_DeviceFillWithDefaultFields(  t_Device *p_Device ) 					*/
/* Description :                                                                            */
/*        fill current Device structure with default 										*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*******************************************************************************************/
e_Result cl_DeviceFillWithDefaultFields( t_Device *pDevice )
{
	clu32 i = 0;

	if ( pDevice == CL_NULL )
		return ( CL_PARAMS_ERR );


	// fill port
	pDevice->e_CryptedLink = g_tDefaultDevice.e_CryptedLink ;

	// fill name
	for (i =0; i < sizeof( g_tDefaultDevice.aucLabel ); i++)
		pDevice->aucLabel[i] = g_tDefaultDevice.aucLabel[i];


	return ( CL_OK );
}




/**************************************************************************/
/* Name : e_Result cl_DeviceAddToList( t_Device *pDevice);             */
/* Description :                                                          */
/*        Add pDevice to the list of Devices							*/
/**************************************************************************/
/* Parameters:                                                            */
/*  --------------                                                        */
/*  In: 	t_Device *pDevice: new Device to add to List			    */
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
e_Result cl_DeviceAddToList( t_Device *pDevice, t_Reader *pReader )
{
	// variables definition
	t_clContext *pCtxt 		= CL_NULL;
	t_Device *pCrtDevice	= CL_NULL;
	t_Device *pDeviceAllocated = CL_NULL;
	t_Reader *pReaderFromList = CL_NULL;
	e_Result status 		= CL_ERROR;
	clu8 u8Index			= 0;


	// check parameters
	if ( pDevice == CL_NULL )
		return ( CL_PARAMS_ERR );

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL )
		return ( CL_MEM_ERR );

// BRY_28092015 encore les conneries de FD !
//	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )
//		return ( CL_ERROR );

	// get head of list of Devices
	if ( CL_FAILED( status = cl_GetDeviceListEntry( &pCrtDevice ) ) )
		return ( CL_MEM_ERR );

	// allocate memory from hosting environment
	status = csl_malloc( (clvoid **)&pDeviceAllocated, sizeof( t_Device ) );
	// if FAILED, exit
	if (( CL_FAILED (status) ) | ( pDeviceAllocated == CL_NULL))
		return ( CL_MEM_ERR );


	// ensure that pNext is NULL for correct chaining of the list
	pDeviceAllocated->ptNext = CL_NULL;

	// if list was empty, initialize it with this allocated memory
	if ( pCrtDevice == CL_NULL )
	{
		// get last non NULL element in the list
		pCrtDevice = pDeviceAllocated;
		cl_SetDeviceListEntry( pDeviceAllocated );
	}
	else
	{
		// get last non NULL element in the list
		while ( pCrtDevice->ptNext != CL_NULL )
		{
			pCrtDevice = (t_Device *)( pCrtDevice->ptNext);
		}
		// fill next with allocated memory
		pCrtDevice->ptNext = (struct t_Device *)pDeviceAllocated;

		// displace to new one
		pCrtDevice = ( t_Device *) pCrtDevice->ptNext ;
	}


	do
	{
		// ..... and data
		pCrtDevice->u64Addr = pDevice->u64Addr;
		pCrtDevice->e_CryptedLink = pDevice->e_CryptedLink;
		for ( u8Index = 0; u8Index < (sizeof(pCrtDevice->aucLabel) ); u8Index++ )
			pCrtDevice->aucLabel[u8Index] = pDevice->aucLabel[u8Index];

		// link to reader from list as we don't play with original variable in CSL
		status =  cl_readerFindInList( &pReaderFromList, pReader );
		pCrtDevice->ptReader = pReaderFromList;

		pCrtDevice->ptNext = CL_NULL;

		break;
	}while (1);

	if ( pCrtDevice != CL_NULL )
	{
		if ( pCrtDevice->ptReader != CL_NULL )
		{
			DEBUG_PRINTF("Device %s connected to Reader %s \n", pCrtDevice->aucLabel, pCrtDevice->ptReader->aucLabel );
		}
		else
		{
			DEBUG_PRINTF("Device %s added but not connected to any reader\n", pCrtDevice->aucLabel);
		}

	}

	status = CL_OK;

	return ( status );
}

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
e_Result cl_DeviceFindInList( t_Device **ppDevice, t_Device *pDeviceFilter )
{
	// variables definition
	t_clContext *pCtxt 		= CL_NULL;
	t_Device *pCrtDevice	= CL_NULL;
	e_Result status 		= CL_ERROR;
	clu8 u8Index			= 0;

	// check parameters
	if ( pDeviceFilter == CL_NULL )
		return ( CL_PARAMS_ERR );

	// get Head of Devices' list
	if ( CL_FAILED( status = cl_GetDeviceListEntry( &pCrtDevice ) ) )
		return ( CL_MEM_ERR );

	// if list of Devices is NULL, we cannot find a Device that satisfies the conditions=> exit on CL_ERROR
	if ( pCrtDevice == CL_NULL )
		return ( CL_ERROR );

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );


	while ( pCrtDevice != CL_NULL )
	{

		// filter on IP address?
		if ( pDeviceFilter->u64Addr != 0 )
		{
			if ( pCrtDevice->u64Addr == pDeviceFilter->u64Addr )
				status = CL_OK;
		}

		// filter on name
		for ( u8Index = 0; u8Index < sizeof( pCrtDevice->aucLabel ) ; u8Index++ )
		{
			status = CL_OK;
			if ( pCrtDevice->aucLabel[u8Index] != pDeviceFilter->aucLabel[u8Index] )
			{
				status = CL_ERROR;
				break;
			}
		}

		// filter on Link ?
		if ( pDeviceFilter->e_CryptedLink != 0 )
		{
			if ( pCrtDevice->e_CryptedLink == pDeviceFilter->e_CryptedLink )
				status = CL_OK;
		}


		// if a match was found, exit on success and returns the correct Device.
		if ( status == CL_OK )
		{
			*ppDevice = pCrtDevice;
			break;
		}

		// points on next element of the Devices list
		pCrtDevice = (t_Device *)pCrtDevice->ptNext;
	};

	return ( status );
}

/**************************************************************************/
/* Name : e_Result cl_DeviceRemoveFromList( t_Device *pDevice)             */
/* Description :                                                         	*/
/*        Remove a Device from the list										*/
/**************************************************************************/
/* Parameters:                                                            	*/
/*  --------------                                                        	*/
/*  In: 	t_Device *pDevice: Remove the Device from the list. NULL not 	*/
/*			allowed															*/
/* ---------------                                                        	*/
/*  Out: 																	*/
/* Return value: e_Result                                                 	*/
/*  CL_OK                        :  Result is OK                            */
/*  CL_ERROR,                    : Failure on execution or list of Devices 	*/
/*												is empty                    */
/*  CL_MEM_ERR,                  :  Failure on memory management (failure,  */
/*                                  allocation ....)                      	*/
/*  CL_PARAMS_ERR,               :  Inconsistent parameters                 */
/**************************************************************************/
e_Result cl_DeviceRemoveFromList( t_Device *pDevice )
{

	// variables definition
	t_clContext *pCtxt 		= CL_NULL;
	t_Device *pCrtDevice	= CL_NULL;
	t_Device *pPrvDevice	= CL_NULL;
	e_Result status 		= CL_ERROR;

	// check parameters
	if ( pDevice == CL_NULL )
		return ( CL_PARAMS_ERR );

	// get Head of Devices' list
	if ( CL_FAILED( status = cl_GetDeviceListEntry( &pCrtDevice ) ) )
		return ( CL_MEM_ERR );
	// if list of Devices is NULL, we cannot find a Device that satisfies the conditions=> exit on CL_ERROR
	if ( pCrtDevice == CL_NULL )
		return ( CL_ERROR );

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )
		return ( CL_MEM_ERR );


	// save pointer to memory to deallocate
	pPrvDevice = pCrtDevice;

	// points CrtDevice to next Device (can be NULL)
	pCrtDevice = ( t_Device *)pCrtDevice->ptNext;


	// if first Device is the one we want to remove, copy the second element in first one then remove.
	// if second element is NULL then the list will be empty
	if ( pPrvDevice == pDevice )
	{
		// free allocated memory
		pCtxt->ptHalFuncs->fnFreeMem( pPrvDevice );

		return ( CL_OK );
	}

	// parse the list
	while ( pCrtDevice != CL_NULL )
	{
		if ( pCrtDevice == pDevice ) // Current Device is the one to remove from the list
		{
			// Previous Device will point to next element of the list
			pPrvDevice->ptNext = pCrtDevice->ptNext;

			// free allocated memory of Current Device
			pCtxt->ptHalFuncs->fnFreeMem( pCrtDevice );

			// exit
			return ( CL_OK );
		}
		else
		{
			// Previous Device is current
			pPrvDevice = pCrtDevice;

			// current is next
			pCrtDevice = (t_Device *)pCrtDevice->ptNext;
		}
	}

	return ( CL_ERROR );
}



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
e_Result cl_SetDeviceListEntry( t_Device *pDevice )
{
	g_ptDefaultDevice = pDevice;	// pointer on reader entrance

	return ( CL_OK );
}

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
e_Result cl_DeviceSetState( t_Device *pDevice, e_State eState )
{
	// check incoming parameters
	if ( pDevice == CL_NULL )
		return ( CL_ERROR );

	if ( (eState != STATE_DEFAULT) & (eState != STATE_INIT) & (eState != STATE_CONNECT) & (eState != STATE_DISCONNECT) & (eState != STATE_ERROR) & (eState != STATE_OK) )
		return ( CL_ERROR );

	// set state
	pDevice->eState = eState;

	return ( CL_OK );
}
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
e_Result cl_DeviceGetState( t_Device *pDevice, e_State *p_eState )
{
	if ( pDevice == CL_NULL)
		return ( CL_ERROR );

	if ( p_eState == CL_NULL)
		return ( CL_ERROR );

	*p_eState = pDevice->eState;

	return ( CL_OK );
}
