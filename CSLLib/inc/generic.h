/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
#include "..\csl.h"

#ifndef GENERIC_H_
#define GENERIC_H_

#ifdef GENERIC_C_
	t_clContext g_tCtxt;
	t_Reader *g_ptReader 				= CL_NULL;
	t_Reader *g_ptLastAccessedReader 	= CL_NULL;
	t_Device *g_ptDefaultDevice 		= CL_NULL;

#else
	extern t_clContext g_tCtxt;
	extern t_Reader *g_ptReader;
	extern t_Reader *g_ptLastAccessedReader;
	extern t_Device *g_ptDefaultDevice;

#endif // GENERIC_C_C


	/*******************************************************************************************/
	/* Name : e_Result cl_GetReaderListEntry( t_Reader **pReader )                                */
	/* Description :                                                                            */
	/*        return readers list entry            												*/
	/*******************************************************************************************/
	/* Parameters:                                                                              */
	/*  --------------                                                                          */
	/*  In : t_Reader **ppReader : pointer to first reader in the list							*/
	/* ---------------                                                                          */
	/*  Out: none                                                                               */
	/* Return value: e_Result                                                                    */
	/*  OK                        :  Result is OK                                               */
	/*  ERROR,                    : Failure on execution                                        */
	/*  MEM_ERR,                  :  Failure on memory management (failure,                     */
	/*                                  allocation ....)                                        */
	/*******************************************************************************************/
	e_Result cl_GetReaderListEntry( t_Reader **ppReader );

	/*******************************************************************************************/
	/* Name : e_Result cl_SetReaderListEntry( t_Reader *pReader )                                */
	/* Description :                                                                            */
	/*        return readers list entry            												*/
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
	e_Result cl_SetReaderListEntry( t_Reader *pReader );


	/*******************************************************************************************/
	/* Name : e_Result cl_GetDeviceListEntry( t_Device **ppDevice )                             */
	/* Description :                                                                            */
	/*        return get Head of Devices' list            										*/
	/*******************************************************************************************/
	/* Parameters:                                                                              */
	/*  --------------                                                                          */
	/*  In : t_Device **ppDevice: pointer to first device in the list							 */
	/* ---------------                                                                          */
	/*  Out: none                                                                               */
	/* Return value: e_Result                                                                    */
	/*  OK                        :  Result is OK                                               */
	/*  ERROR,                    : Failure on execution                                        */
	/*  MEM_ERR,                  :  Failure on memory management (failure,                     */
	/*                                  allocation ....)                                        */
	/*******************************************************************************************/
	e_Result cl_GetDeviceListEntry( t_Device **ppDevice );

	/******************************************************************************/
	/* Name :      e_Result cl_GetElapsedTime(  )           					*/
	/* Description : returns time elapsed between each call, 0 if ERROR				*/
	/*                                                                            	*/
	/******************************************************************************/
	/* Parameters:                                                                	*/
	/*  --------------                                                            	*/
	/*  In :  none																	*/
	/* ---------------                                                            	*/
	/*  Out: none                                                                 	*/
	/* Return value: time in millisecond                                          	*/
	/******************************************************************************/
	cl64 cl_GetElapsedTime( );

#endif /* GENERIC_H_ */
