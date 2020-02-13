/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: netal_win32.c                                                      */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/* Description: hporting layer for network access in WIN32 environment       */
/*****************************************************************************/

//#ifdef WIN32
//#ifndef(__x86_64__)


#ifdef WIN32

#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <ctype.h>
#include <string.h>

#elif defined (linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

#include "..\..\csl.h"
#include "netal_win32.h"
#include "..\..\inc\generic.h"
#include "..\..\inc\cltuple.h"
#include "..\..\inc\clhelpers.h"
#include "..\..\inc\clreaders.h"

/******************************************************************************/
/* Name :  e_Result cl_NetwResetWin32( clvoid *ptReader );                       */
/* Description : reset the network connection to a reader                     */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader *pReader : pointer to a reader where to reset the connection*/
/*                           if NULL, reset complete network connection       */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                 */
/*  ERROR,                    : Failure on execution                          */
/*  MEM_ERR,                  :  Failure on memory management (failure,       */
/*                                  allocation ....)                          */
/*  PARAMS_ERR,               :  Inconsistent parameters                      */
/*  TIMEOUT_ERR,              :  Overrun on timing                            */
/******************************************************************************/
e_Result cl_NetwResetWin32(  clvoid *ptReader )
{
	return ( CL_OK );
}

/******************************************************************************/
/* Name :  e_Result cl_NetwRegisterWin32( clvoid *ptReader );                    */
/* Description : register a reader to the hosting os. It allows to implement  */
/*          a blocking call in the underneath layer on incoming packets from */
/*          the network and propagate a signal the library to the dedicated  */
/*          reader. Then the reader reads the data coming from the underneath*/
/*          layer                                                            */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader *pReader : pointer to a reader that this library deals with */
/*                           if NULL, returns PARAMS_ERR                      */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,        */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/*  TIMEOUT_ERR,              :  Overrun on timing                             */
/******************************************************************************/
e_Result cl_NetwRegisterWin32(  clvoid *ptReader )
{
	return ( CL_OK );
}


/******************************************************************************/
/* Name :  e_Result cl_NetwUnregisterWin32( clvoid *ptReader );                  */
/* Description : unregister a reader from the hosting os. It allows to remove */
/*          a blocking call in the underneath layer on incoming packets from */
/*          the network and not to propagate a signal the library to the     */
/*          reader.                                                          */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader *pReader : pointer to a reader that this library deals with */
/*                           if NULL, returns PARAMS_ERR                      */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,        */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/*  TIMEOUT_ERR,              :  Overrun on timing                             */
/******************************************************************************/
e_Result cl_NetwUnregisterWin32(  clvoid *ptReader )
{
	return ( CL_OK );
}

/******************************************************************************/
/* Name :  e_Result cl_NetwSendDataWin32( clvoid *ptReader, t_Buffer *ptBuff);   */
/* Description : send data to a reader via the network API. This access be    */
/*                shall be provided by the customer and vary depending on     */
/*              target host system OS                                         */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader *pReader : pointer to there reader where to send the data   */
/*                           if NULL, returns PARAMS_ERR                      */
/*        t_Buffer *ptBuff : pointer to data buffer                           */
/*                           if NULL, returns PARAMS_ERR                      */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*                          * OK                                              */
/*                          * ERROR:   Failure on execution					  */
/*                          * MEM_ERROR : Memory Error (failure, allocation..)*/
/*                          * PARAMS_ERR : Inconsistent parameters            */
/*                          * TIMEOUT_ERR : Timeout when sending data over network */
/*                          * TRANSFER_IN_PROCESS_ERR: a transfer on this     */
/*                          reader is already ongoing                         */
/******************************************************************************/
e_Result cl_NetwSendDataWin32( clvoid *ptReader, t_Buffer *ptBuff)
{
	t_clContext *pCtxt 		= CL_NULL;
	e_Result status 		= CL_NULL;
	clu32 	iReturnCode 	= 0;
	clu8 	*pData 			= CL_NULL;
	clu8 	*pTrailerData 	= CL_NULL;
    t_Tuple *ptTuple		= CL_NULL;
    t_Buffer *pLocalBuf		= CL_NULL;
    t_Reader *pReader		= (t_Reader *)ptReader;
	SOCKADDR_IN sin = { 0 };

	// check incoming parameters
	if ( pReader == CL_NULL )
		return ( CL_PARAMS_ERR );

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


	for (;;)
	{
		DEBUG_PRINTF("NetwSendDataWin32:Thread: BEGIN");

		if (ptBuff->ulLen> 100)
			return (CL_ERROR );

		//----
		// perform a local copy of incoming buff to apply format change with serial support without corrupting incoming data (which is used subsequently in CSL)
		//----
		// allocate memory for structure t_Buffer
		//if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnAllocMem( ( clvoid **)&pLocalBuf, sizeof( t_Buffer ) ) ) )
		if ( CL_FAILED( status = csl_malloc( ( clvoid **)&pLocalBuf, sizeof( t_Buffer ) ) ) )
			return ( CL_ERROR );

		//if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnAllocMem( ( clvoid **)&pData, ptBuff->ulLen ) ) )
		if ( CL_FAILED( status = csl_malloc( ( clvoid **)&pData, ptBuff->ulLen ) ) )
		{
			pCtxt->ptHalFuncs->fnFreeMem( pLocalBuf );
			return ( CL_ERROR );
		}
		pLocalBuf->pData = pData;
		memcpy( pLocalBuf->pData, ptBuff->pData, ptBuff->ulLen);
		pLocalBuf->ulLen = ptBuff->ulLen;

		// add the low level encapsulation if necessary
		if ( CL_FAILED( status = cl_HelperEncapsulateToSerial( pReader, pLocalBuf ) ) )
		{
			DEBUG_PRINTF("Failed on cl_HelperEncapsulateToSerial\n ");
			return ( CL_ERROR );
		}

		// send data to reader over IP
		if ( ( iReturnCode = send ( pReader->u64Handle, (CONST cl8 *) pLocalBuf->pData, (cl32) pLocalBuf->ulLen, 0) ) < 0)
		{
			// get error from errno
			switch ( errno )
			{
				case ( WSANOTINITIALISED ):
						DEBUG_PRINTF("A successful WSAStartup call must occur before using this function. \n");
						break;
				case ( WSAENETDOWN ):
						DEBUG_PRINTF(" The network subsystem has failed. \n");
						break;
				case ( WSAEACCES ):
						DEBUG_PRINTF(" The requested address is a broadcast address, but the appropriate flag was not set. Call setsockopt with the SO_BROADCAST socket option to enable use of the broadcast address. ");
						break;
				case ( WSAEINTR ):
						DEBUG_PRINTF(" A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall.");
						break;
				case ( 	WSAEINPROGRESS ):
						DEBUG_PRINTF(" A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n");
						break;
				case ( WSAEFAULT ):
						DEBUG_PRINTF(" The buf parameter is not completely contained in a valid part of the user address space.\n");
						break;
				case ( WSAENETRESET ):
						DEBUG_PRINTF(" The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress.\n");
						break;
				case ( WSAENOBUFS ):
						DEBUG_PRINTF("No buffer space is available.\n");
						break;
				case ( WSAENOTCONN ):
						DEBUG_PRINTF(" The socket is not connected.\n");
						break;
				case ( WSAENOTSOCK ):
						DEBUG_PRINTF(" The descriptor is not a socket. \n ");
						break;
				case ( WSAEOPNOTSUPP ):
						DEBUG_PRINTF(" MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only receive operations.");
						break;
				case ( WSAESHUTDOWN ):
						DEBUG_PRINTF(" The socket has been shut down; it is not possible to send on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH.");
						break;
				case ( WSAEWOULDBLOCK ):
						DEBUG_PRINTF(" The socket is marked as nonblocking and the requested operation would block. \n");
						break;
				case ( WSAEMSGSIZE ):
						DEBUG_PRINTF( "The socket is message oriented, and the message is larger than the maximum supported by the underlying transport.");
						break;
				case ( WSAEHOSTUNREACH ):
						DEBUG_PRINTF( "The remote host cannot be reached from this host at this time.");
						break;
				case ( WSAEINVAL ):
						DEBUG_PRINTF( "The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled.");
						break;
				case ( WSAECONNABORTED ):
						DEBUG_PRINTF( "The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.");
						break;
				case ( WSAECONNRESET ):
						DEBUG_PRINTF( "The virtual circuit was reset by the remote side executing a hard or abortive close. For UDP sockets, the remote host was unable to deliver a previously sent UDP datagram and responded with a Port \n");
						DEBUG_PRINTF("Unreachable ICMP packet. The application should close the socket as it is no longer usable.\n");
						break;
				case ( WSAETIMEDOUT ):
						DEBUG_PRINTF( "The connection has been dropped, because of a network failure or because the system on the other end went down without notice.");
						break;
			}
			status = CL_ERROR;
		}
		else // send is successfull
			status = CL_OK;

		break;
	}

    if ( pLocalBuf )
    {
    	pData = pLocalBuf->pData;
    	pCtxt->ptHalFuncs->fnFreeMem( pLocalBuf );
    }
    if ( pData )
    	pCtxt->ptHalFuncs->fnFreeMem( pData );

	return ( status );
}

/******************************************************************************/
/* Name :  e_Result cl_NetwGetDataWin32( clvoid *ptReader, t_Tuple **pptTuple);  */
/* Description : read data from a reader via the network API. This access be    */
/*                shall be provided by the customer and vary depending on     */
/*              target host system OS                                         */
/*              Implements a call which is non blocking or not to allow polling */
/*              on different readers. Only one thread is passing from one reader*/
/*              to the others                                                 */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader *pReader : pointer to there reader where to send the data   */
/*                           if NULL, returns PARAMS_ERR                      */
/*        t_Tuple **pptTuple: pointer to tuple list. Allocation is provided  */
/*                            by underlayer. Allocation shall be done using   */
/*                            fnAllocMem to allow library to free it properly */
/*                           if NULL or points to NULL, returns PARAMS_ERR    */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*                          * OK                                              */
/*                          * ERROR:   Failure on execution						*/
/*                          * MEM_ERROR : Memory Error (failure, allocation..)*/
/*                          * PARAMS_ERR : Inconsistent parameters            */
/*                          * TIMEOUT_ERR : Timeout when sending data over network */
/*                          * TRANSFER_IN_PROCESS_ERR: a transfer on this     */
/*                          reader is already ongoing                         */
/******************************************************************************/
e_Result cl_NetwGetDataWin32(  clvoid *ptReader, t_Tuple **pptTuple )
{
	t_clContext *pCtxt 		= CL_NULL;
	e_Result 	status 		= CL_ERROR;
	cl32		cl32Len		= 0 ;
	clu8 		*pData		= CL_NULL;
    t_Reader *pReader		= (t_Reader *)ptReader;

	/* check incoming paramaters */
	if ( pReader == CL_NULL )
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

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL)
		return ( CL_ERROR );

	// allocate memory to receive data coming from underlayers
	status = csl_malloc( ( clvoid **)&pData, 512 );	// max size is doubled as we can have short buffers concatenated together.
	if ( CL_FAILED( status ))
		return ( CL_MEM_ERR );

	// initialize with 0
	memset( pData, 0, 512 );

	// receive in TCP usual mode
	cl32Len = recv( pReader->u64Handle, (cl8 *)pData, 512, 0);

	if (cl32Len < 0)
	{
		DEBUG_PRINTF("recv failed with : %x", (clu32)cl32Len );
		// release allocated
		if ( pCtxt->ptHalFuncs->fnFreeMem != CL_NULL )
			pCtxt->ptHalFuncs->fnFreeMem( (clvoid *)pData );

		// return error
		return ( CL_ERROR );
	}
	else
	{
		// parse the incoming buffer and prepare the tuple list for upper layer
		// the format of underlaying driver can concatenate several buffers together
		status = cl_HelperNetworkToTuples( pReader, pData, cl32Len, pptTuple );
	}
	// free unused memory as helper created tuple list and this buffer won't be used in upper layers
	if ( pData != CL_NULL )
		pCtxt->ptHalFuncs->fnFreeMem( (clvoid *)pData );

	// exit
	return ( status );
}



/******************************************************************************/
/* Name :  e_Result cl_NetwGetDNSTableWin32(  clvoid **pptReaderList );         */
/* Description : read the DNS table from the target host system OS            */
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader **pptReaderList : pointer to the list of readers registered */
/*                           in the host system                               */
/*                if NULL, no Readers are present in the system               */
/*                it is called by the library after a successful fnNetwReset */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*  OK                        :  Result is OK                                  */
/*  ERROR,                    : Failure on execution                           */
/*  MEM_ERR,                  :  Failure on memory management (failure,        */
/*                                  allocation ....)                           */
/*  PARAMS_ERR,               :  Inconsistent parameters                       */
/*  TIMEOUT_ERR,              :  Overrun on timing                             */
/******************************************************************************/
e_Result cl_NetwGetDNSTableWin32( clvoid **pptReaderList)
{
	return ( CL_OK );
}

/******************************************************************************/
/* Name :  e_Result (cl_NetwOpenConnectionWin32)( clvoid *ptReader );  		*/
/* Description : open a connection to a reader    								*/
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader *pReader : pointer to there reader to connect to with ip    */
/*                           address and port			                      */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                     */
/*                          * OK   : connection ok                            */
/*                          * PARAMS_ERR : Inconsistent parameters            */
/*                          * TIMEOUT_ERR : no answer from the reader at this address */
/*							i.e : no reader at all							*/
/******************************************************************************/
e_Result cl_NetwOpenConnectionWin32( clvoid *ptReader )
{
	SOCKET 		sock;
	SOCKADDR_IN sin = { 0 };
	cl8			cl8OptFlag		=	1;
    t_Reader 	*pReader		= (t_Reader *)ptReader;
	struct timeval timeout;
	clu32 u32BroadcastOpt		=	1;


	// check incoming parameters
	if ( pReader == CL_NULL )
		return ( CL_PARAMS_ERR );

	// declare new socket for this reader
	sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//sock = socket( AF_INET, SOCK_STREAM, 0) ;//IPPROTO_TCP );
	if ( sock == INVALID_SOCKET )
		return ( CL_PARAMS_ERR );

	// try to connect to the TCP server of the reader
	sin.sin_addr.s_addr = inet_addr( pReader->tIPParams.aucIpAddr ); 	//= *pReader->u64IpAddr; /* l'adresse se trouve dans le champ h_addr de la structure hostinfo */
	sin.sin_port = htons( pReader->tIPParams.u32Port ); 		/* on utilise htons pour le port */
	sin.sin_family = AF_INET;

	if ( connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR )
	{
		// get details on error from network
		switch ( errno )
		{
			case ( WSANOTINITIALISED ):
				DEBUG_PRINTF("A successful WSAStartup call must occur before using this function.");
				break;
			case (WSAENETDOWN):
				DEBUG_PRINTF("The network subsystem has failed.");
				break;
			case ( WSAEADDRINUSE ):
				DEBUG_PRINTF("The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs when executing bind, but could be delayed until the connect function if the bind was to a wildcard address (INADDR_ANY or in6addr_any) for the local IP address. A specific address needs to be implicitly bound by the connect function.");
				break;
			case ( WSAEINTR ):
				DEBUG_PRINTF(" The blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.");
				break;
			case ( WSAEINPROGRESS ):
				DEBUG_PRINTF(" A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
				break;
			case ( WSAEALREADY):
				DEBUG_PRINTF(" A nonblocking connect call is in progress on the specified socket.");
				DEBUG_PRINTF("Note  In order to preserve backward compatibility, this error is reported as WSAEINVAL to Windows Sockets 1.1 applications that link to either Winsock.dll or Wsock32.dll.");
				break;
			case ( WSAEADDRNOTAVAIL):
				DEBUG_PRINTF("The remote address is not a valid address (such as INADDR_ANY or in6addr_any) .");
				break;
			case ( WSAEAFNOSUPPORT ):
				DEBUG_PRINTF("Addresses in the specified family cannot be used with this socket.");
				break;
			case ( WSAECONNREFUSED ):
				DEBUG_PRINTF("The attempt to connect was forcefully rejected.");
				break;
			case ( WSAEFAULT):
				DEBUG_PRINTF("The sockaddr structure pointed to by the name contains incorrect address format for the associated address family or the namelen parameter is too small. This error is also returned if the sockaddr structure pointed to by the name parameter with a length specified in the namelen parameter is not in a valid part of the user address space.");
				break;
			case ( WSAEINVAL):
				DEBUG_PRINTF("The parameter is a listening socket.");
				break;
			case ( WSAEISCONN):
				DEBUG_PRINTF("The socket is already connected (connection-oriented sockets only).");
				break;
			case (WSAENETUNREACH):
				DEBUG_PRINTF( " The network cannot be reached from this host at this time.");
				break;
			case ( WSAEHOSTUNREACH ):
				DEBUG_PRINTF( "A socket operation was attempted to an unreachable host.");
				break;
			case ( WSAENOBUFS ):
				DEBUG_PRINTF("Note  No buffer space is available. The socket cannot be connected.\n");
				break;
			case ( 	WSAENOTSOCK ):
				DEBUG_PRINTF("The descriptor specified in the s parameter is not a socket.\n");
				break;
			case (	WSAETIMEDOUT ):
				DEBUG_PRINTF("An attempt to connect timed out without establishing a connection.\n");
				break;
			case ( WSAEWOULDBLOCK ):
				DEBUG_PRINTF(" The socket is marked as nonblocking and the connection cannot be completed immediately.");
				break;
			case ( WSAEACCES ):
				DEBUG_PRINTF("An attempt to connect a datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled.");
				break;
			default:
				DEBUG_PRINTF("Connect failed: Undef errno %x IP address %s port %d\n", errno, pReader->tIPParams.aucIpAddr, pReader->tIPParams.u32Port );
//				DEBUG_PRINTF( errno );
				break;
		}
		closesocket( sock );
		return ( CL_TIMEOUT_ERR );
	}

	// set socket option to force flushing out of data each time there is a data available to send
	if ( setsockopt( sock, IPPROTO_TCP, TCP_NODELAY, &cl8OptFlag, sizeof( cl8OptFlag)) == SOCKET_ERROR )
	{
		// get details on error from network
		switch ( errno )
		{
			case ( WSANOTINITIALISED ):
				DEBUG_PRINTF("A successful WSAStartup call must occur before using this function.");
				break;
			case	( WSAENETDOWN ):
				DEBUG_PRINTF("The network subsystem has failed.");
				break;
			case ( WSAEFAULT ):
				DEBUG_PRINTF("The buffer pointed to by the optval parameter is not in a valid part of the process address space or the optlen parameter is too small.");
				break;
			case ( WSAEINPROGRESS ):
				DEBUG_PRINTF("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
				break;
			case ( WSAEINVAL ):
				DEBUG_PRINTF("The level parameter is not valid, or the information in the buffer pointed to by the optval parameter is not valid.");
				break;
			case ( WSAENETRESET ):
				DEBUG_PRINTF("The connection has timed out when SO_KEEPALIVE is set.");
				break;
			case ( WSAENOPROTOOPT ):
				DEBUG_PRINTF("The option is unknown or unsupported for the specified provider or socket (see SO_GROUP_PRIORITY limitations).");
				break;
			case ( WSAENOTCONN ):
				DEBUG_PRINTF( " The connection has been reset when SO_KEEPALIVE is set. ");
				break;
			case ( WSAENOTSOCK ):
				DEBUG_PRINTF("The descriptor is not a socket.");
				break;
			default:
				DEBUG_PRINTF("setsockopt failed: Undef errno %x IP address %s port %d\n", errno, pReader->tIPParams.aucIpAddr, pReader->tIPParams.u32Port );
				break;
		}
		closesocket( sock );
		return ( CL_TIMEOUT_ERR );
	}
	pReader->u64Handle = sock;

	return ( CL_OK );
}

/*******************************************************************************/
/* Name :  e_Result (cl_NetwOpenConnectionDiscoverWin32)( clvoid *ptReader );  	*/
/* Description : open a connection to a reader to be used for discover of others readers */
/*	for IP readers, this function declares UDP sockets for discover instead of	*/
/*	standard TCP																*/
/******************************************************************************/
/* Parameters:                                                                	*/
/*  --------------                                                            	*/
/*  In : t_Reader *pReader : pointer to there reader to connect to with ip    	*/
/*                           address and port			                      	*/
/* ---------------                                                            	*/
/*  Out: none                                                                 	*/
/* Return value: e_Result                                                     	*/
/*                          * OK   : connection ok                            	*/
/*                          * PARAMS_ERR : Inconsistent parameters            	*/
/*                          * TIMEOUT_ERR : no answer from the reader at this address */
/*							i.e : no reader at all								*/
/******************************************************************************/
e_Result cl_NetwOpenConnectionDiscoverWin32( clvoid *ptReader )
{
	SOCKET 		sock;
	SOCKADDR_IN sin = { 0 };
	cl8			cl8OptFlag		=	1;
    t_Reader 	*pReader		= (t_Reader *)ptReader;
	struct timeval timeout;
	clu32 u32BroadcastOpt		=	1;


	// check incoming parameters
	if ( pReader == CL_NULL )
		return ( CL_PARAMS_ERR );

	// if we are in discover => open an UDP connection
	// to discover all lantronix devices:
	// send udp broadcast:  00 00 00 F8 to udp port 30718 => answer :  00 00 00 F9 plus 4 hex values = Lantronix reader ip addresses
	// declare new UDP socket for this reader
	sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

	if ( sock == INVALID_SOCKET )
		return ( CL_PARAMS_ERR );

	if ( setsockopt( sock, SOL_SOCKET, SO_BROADCAST, (char *)&u32BroadcastOpt, sizeof(u32BroadcastOpt) ) )
	{
		switch (errno)
		{
			case ( WSANOTINITIALISED ):
				DEBUG_PRINTF("A successful WSAStartup call must occur before using this function.\n");
				break;
			case (WSAENETDOWN):
				DEBUG_PRINTF("The network subsystem has failed.\n");
				break;
			case ( WSAEFAULT ):
				DEBUG_PRINTF(" The buffer pointed to by the optval parameter is not in a valid part of the process address space or the optlen parameter is too small.\n");
				break;
			case ( WSAEINPROGRESS ):
				DEBUG_PRINTF(" A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
				break;
			case ( WSAEINVAL):
				DEBUG_PRINTF("Some invalid argument was supplied (for example, specifying an invalid level to the setsockopt function). In some instances, it also refers to the current state of the socket�for instance, calling accept on a socket that is not listening\n");
				break;
			case ( WSAEMFILE ):
				DEBUG_PRINTF(" Too many open files.Too many open sockets. Each implementation may have a maximum number of socket handles available, either globally, per process, or per thread.\n");
				break;
			case ( WSAEWOULDBLOCK):
				DEBUG_PRINTF("Resource temporarily unavailable. This error is returned from operations on nonblocking sockets that cannot be completed immediately, for example recv when no data is queued to be read from the socket. It is a nonfatal error, and the operation should be retried later. It is normal for WSAEWOULDBLOCK to be reported as the result from calling connect on a nonblocking SOCK_STREAM socket, since some time must elapse for the connection to be established.\n");
				break;
			case ( WSAEAFNOSUPPORT ):
				DEBUG_PRINTF("Addresses in the specified family cannot be used with this socket.");
				break;
			case ( WSAEALREADY):
				DEBUG_PRINTF("Operation already in progress. An operation was attempted on a nonblocking socket with an operation already in progress�that is, calling connect a second time on a nonblocking socket that is already connecting, or canceling an asynchronous request (WSAAsyncGetXbyY) that has already been canceled or completed.");
				break;
			default:
				DEBUG_PRINTF("Connect failed: Undef errno %x IP address %s port %d\n", errno, pReader->tIPParams.aucIpAddr, pReader->tIPParams.u32Port );
				DEBUG_PRINTF( errno );
				break;
		}
		closesocket( sock );
		pReader->u64Handle = CL_NULL;
		return ( CL_TIMEOUT_ERR );
	}
	/* bind this socket to any data coming from the UDP port */
	sin.sin_family = AF_INET;
	sin.sin_port = htons(pReader->tIPParams.u32Port);
	sin.sin_addr.s_addr = INADDR_ANY;
	memset( sin.sin_zero, 0, sizeof( sin.sin_zero));

	if (bind(sock,(struct sockaddr *)&sin, sizeof(struct sockaddr)) == -1)
	{
		DEBUG_PRINTF("Bind error %x", errno);
		closesocket( sock );
		return ( CL_ERROR );
	}
	pReader->u64Handle = sock;

	return ( CL_OK );
}

/******************************************************************************/
/* Name :  e_Result cl_NetwSendDataDiscoverWin32( clvoid *ptReader, t_Buffer *ptBuff);   */
/* Description : send data to a reader via the network API. This access be    */
/*                shall be provided by the customer and vary depending on     */
/*              target host system OS                                         */
/*				This API, for discover process, send data over UDP socket	*/
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader *pReader : pointer to there reader where to send the data   */
/*                           if NULL, returns PARAMS_ERR                      */
/*        t_Buffer *ptBuff : pointer to data buffer                           */
/*                           if NULL, returns PARAMS_ERR                      */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*                          * OK                                              */
/*                          * ERROR:   Failure on execution					  */
/*                          * MEM_ERROR : Memory Error (failure, allocation..)*/
/*                          * PARAMS_ERR : Inconsistent parameters            */
/*                          * TIMEOUT_ERR : Timeout when sending data over network */
/*                          * TRANSFER_IN_PROCESS_ERR: a transfer on this     */
/*                          reader is already ongoing                         */
/******************************************************************************/
e_Result cl_NetwSendDataDiscoverWin32( clvoid *ptReader, t_Buffer *ptBuff)
{
	t_clContext	*pCtxt 			= CL_NULL;
	e_Result 	status 			= CL_NULL;
	clu32 		iReturnCode 	= 0;
	clu8 		*pData 			= CL_NULL;
	clu8 		*pTrailerData 	= CL_NULL;
    t_Tuple 	*ptTuple		= CL_NULL;
    t_Reader 	*pReader		= (t_Reader *)ptReader;
    t_Buffer	*pLocalBuf		= CL_NULL;
	SOCKADDR_IN sin 			= { 0 };

	// check incoming parameters
	if ( pReader == CL_NULL )
		return ( CL_PARAMS_ERR );

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

	DEBUG_PRINTF("\n 1. cl_NetwSendDataDiscoverWin32 %d \n", ptBuff->ulLen);

	if ( ptBuff->ulLen > 100)
		return (CL_ERROR);
	//----
	// perform a local copy of incoming buff to apply format change with serial support without corrupting incoming data (which is used subsequently in CSL)
	//----
	// allocate memory for structure t_Buffer
	if ( CL_FAILED( status = csl_malloc( ( clvoid **)&pLocalBuf, sizeof( t_Buffer ) ) ) )
		return ( CL_ERROR );

	if ( CL_FAILED( status = csl_malloc( ( clvoid **)&pData, ptBuff->ulLen ) ) )
	{
		pCtxt->ptHalFuncs->fnFreeMem( pLocalBuf );
		return ( CL_ERROR );
	}
	pLocalBuf->pData = pData;
	memcpy( pLocalBuf->pData, ptBuff->pData, ptBuff->ulLen);
	pLocalBuf->ulLen = ptBuff->ulLen;

	// for UDP of the reader
	sin.sin_addr.s_addr = inet_addr( pReader->tIPParams.aucIpAddr ); 	//= *pReader->u64IpAddr; /* l'adresse se trouve dans le champ h_addr de la structure hostinfo */
	sin.sin_port = htons( pReader->tIPParams.u32Port ); 		/* on utilise htons pour le port */
	sin.sin_family = AF_INET;

	// send data to reader over IP
	if ( ( iReturnCode = sendto ( pReader->u64Handle, (CONST cl8 *) pLocalBuf->pData, (cl32) pLocalBuf->ulLen, 0, (SOCKADDR *)&sin, sizeof( sin) ) ) < 0)
	{
		// get error from errno
		switch ( errno )
		{
			case ( WSANOTINITIALISED ):
					DEBUG_PRINTF("A successful WSAStartup call must occur before using this function. \n");
					break;
			case ( WSAENETDOWN ):
					DEBUG_PRINTF(" The network subsystem has failed. \n");
					break;
			case ( WSAEACCES ):
					DEBUG_PRINTF(" The requested address is a broadcast address, but the appropriate flag was not set. Call setsockopt with the SO_BROADCAST socket option to enable use of the broadcast address. ");
					break;
			case ( WSAEINTR ):
					DEBUG_PRINTF(" A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall.");
					break;
			case ( 	WSAEINPROGRESS ):
					DEBUG_PRINTF(" A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n");
					break;
			case ( WSAEFAULT ):
					DEBUG_PRINTF(" The buf parameter is not completely contained in a valid part of the user address space.\n");
					break;
			case ( WSAENETRESET ):
					DEBUG_PRINTF(" The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress.\n");
					break;
			case ( WSAENOBUFS ):
					DEBUG_PRINTF("No buffer space is available.\n");
					break;
			case ( WSAENOTCONN ):
					DEBUG_PRINTF(" The socket is not connected.\n");
					break;
			case ( WSAENOTSOCK ):
					DEBUG_PRINTF(" The descriptor is not a socket. \n ");
					break;
			case ( WSAEOPNOTSUPP ):
					DEBUG_PRINTF(" MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only receive operations.");
					break;
			case ( WSAESHUTDOWN ):
					DEBUG_PRINTF(" The socket has been shut down; it is not possible to send on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH.");
					break;
			case ( WSAEWOULDBLOCK ):
					DEBUG_PRINTF(" The socket is marked as nonblocking and the requested operation would block. \n");
					break;
			case ( WSAEMSGSIZE ):
					DEBUG_PRINTF( "The socket is message oriented, and the message is larger than the maximum supported by the underlying transport.");
					break;
			case ( WSAEHOSTUNREACH ):
					DEBUG_PRINTF( "The remote host cannot be reached from this host at this time.");
					break;
			case ( WSAEINVAL ):
					DEBUG_PRINTF( "The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled.");
					break;
			case ( WSAECONNABORTED ):
					DEBUG_PRINTF( "The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.");
					break;
			case ( WSAECONNRESET ):
					DEBUG_PRINTF( "The virtual circuit was reset by the remote side executing a hard or abortive close. For UDP sockets, the remote host was unable to deliver a previously sent UDP datagram and responded with a Port \n");
					DEBUG_PRINTF("Unreachable ICMP packet. The application should close the socket as it is no longer usable.\n");
					break;
			case ( WSAETIMEDOUT ):
					DEBUG_PRINTF( "The connection has been dropped, because of a network failure or because the system on the other end went down without notice.");
					break;
		}
		status = CL_ERROR;
	}
	else // send is successfull
		status = CL_OK;

    if ( pLocalBuf )
    {
    	pData = pLocalBuf->pData;
    	pCtxt->ptHalFuncs->fnFreeMem( pLocalBuf );
    }
    if ( pData )
    	pCtxt->ptHalFuncs->fnFreeMem( pData );

	DEBUG_PRINTF("\n 2. cl_NetwSendDataDiscoverWin32 %d \n", ptBuff->ulLen);

	return ( status );
}

/******************************************************************************/
/* Name :  e_Result cl_NetwGetDataDiscoverWin32( clvoid *ptReader, t_Tuple **pptTuple);  */
/* Description : read data from a reader via the network API. This access be    */
/*                shall be provided by the customer and vary depending on     	*/
/*              target host system OS                                         	*/
/*              Implements a call which is non blocking or not to allow polling */
/*              on different readers. Only one thread is passing from one reader*/
/*              to the others                                                 	*/
/*				This API, for discover process, receive UDP data				*/
/*                                                                            	*/
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader *pReader : pointer to there reader where to send the data   */
/*                           if NULL, returns PARAMS_ERR                      */
/*        t_Tuple **pptTuple: pointer to tuple list. Allocation is provided  */
/*                            by underlayer. Allocation shall be done using   */
/*                            fnAllocMem to allow library to free it properly */
/*                           if NULL or points to NULL, returns PARAMS_ERR    */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                      */
/*                          * OK                                              */
/*                          * ERROR:   Failure on execution						*/
/*                          * MEM_ERROR : Memory Error (failure, allocation..)*/
/*                          * PARAMS_ERR : Inconsistent parameters            */
/*                          * TIMEOUT_ERR : Timeout when sending data over network */
/*                          * TRANSFER_IN_PROCESS_ERR: a transfer on this     */
/*                          reader is already ongoing                         */
/******************************************************************************/
e_Result cl_NetwGetDataDiscoverWin32(  clvoid *ptReader, t_Tuple **pptTuple )
{
	t_clContext *pCtxt 		= CL_NULL;
	e_Result 	status 		= CL_ERROR;
	cl32		cl32Len		= 0 ;
	cl32		cl32FromLen		= 0 ;
	clu8 		*pData		= CL_NULL;
    t_Reader *pReader		= (t_Reader *)ptReader;
	SOCKADDR_IN sin = { 0 };
	cl32	erroris;

	/* check incoming paramaters */
	if ( pReader == CL_NULL )
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

	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL)
		return ( CL_ERROR );

	// allocate memory to receive data coming from underlayers
	status = csl_pmalloc( ( clvoid **)&pData, 2048 );	// max size is doubled as we can have short buffers concatenated together.
	if ( CL_FAILED( status ))
		return ( CL_MEM_ERR );

	// initialize with 0
	memset( pData, 0, 2048 );

	cl32FromLen = sizeof( sin );
	//

	// receive data from UDP socket
	cl32Len = recvfrom( (SOCKET)pReader->u64Handle, (cl8 *)pData, 2048, 0, ( SOCKADDR *)&sin, (cl32 *)&cl32FromLen );

	if (cl32Len < 0)
	{
		errno = WSAGetLastError();
		DEBUG_PRINTF("Error is %x", erroris);
		switch (errno)
		{
			case (WSANOTINITIALISED):
			{
				DEBUG_PRINTF("A successful WSAStartup call must occur before using this function.\n");
				break;
			}
			case ( WSAENETDOWN ):
			{
				DEBUG_PRINTF("The network subsystem has failed.\n");
				break;
			}
			case ( WSAEFAULT):
			{
				DEBUG_PRINTF("The buffer pointed to by the buf or from parameters are not in the user address space, or the fromlen parameter is too small to accommodate the source address of the peer address.\n");
				break;
			}
			case ( WSAEINTR):
			{
				DEBUG_PRINTF("The (blocking) call was canceled through WSACancelBlockingCall.\n");
				break;
			}
			case ( WSAEINPROGRESS):
			{
				DEBUG_PRINTF("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.\n");
				break;
			}
			case ( WSAEINVAL):
			{
				DEBUG_PRINTF("The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled, or (for byte stream-style sockets only) len was zero or negative.\n");
				break;
			}
			case ( WSAEISCONN):
			{
				DEBUG_PRINTF("The socket is connected. This function is not permitted with a connected socket, whether the socket is connection oriented or connectionless.\n");
				break;
			}
			case ( WSAENETRESET):
			{
				DEBUG_PRINTF("For a datagram socket, this error indicates that the time to live has expired.\n");
				break;
			}
			case ( WSAENOTSOCK):
			{
				DEBUG_PRINTF("The descriptor in the s parameter is not a socket.\n");
				break;
			}
			case ( WSAEOPNOTSUPP):
			{
				DEBUG_PRINTF("MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations.\n");
				break;
			}
			case ( WSAESHUTDOWN):
			{
				DEBUG_PRINTF("The socket has been shut down; it is not possible to recvfrom on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH.\n");
				break;
			}
			case ( WSAEWOULDBLOCK):
			{
				DEBUG_PRINTF("The socket is marked as nonblocking and the recvfrom operation would block.\n");
				break;
			}
			case ( WSAEMSGSIZE):
			{
				DEBUG_PRINTF("The message was too large to fit into the buffer pointed to by the buf parameter and was truncated.\n");
				break;
			}
			case ( WSAETIMEDOUT):
			{
				DEBUG_PRINTF("The connection has been dropped, because of a network failure or because the system on the other end went down without notice.\n");
				break;
			}
			case ( WSAECONNRESET):
			{
				DEBUG_PRINTF("The virtual circuit was reset by the remote side executing a hard or abortive close. The application should close the socket; it is no longer usable. On a UDP-datagram socket this error indicates a previous send operation resulted in an ICMP Port Unreachable message.\n");
				break;
			}
			default:
			{
				DEBUG_PRINTF("recvfrom : unknown error %x\n", errno );
				break;
			}
		}
		// return error
		status =  CL_ERROR;
	}
	else
	{
		// parse the incoming buffer and prepare the tuple list for upper layer
		// the format of underlaying driver can concatenate several buffers together
		status = cl_HelperDiscoverNetworkToTuples( pReader, pData, cl32Len, pptTuple );
	}
	// exit
	return ( status );
}

/******************************************************************************/
/* Name :  e_Result (cl_NetwCloseConnectionWin32)( clvoid *ptReader );  		*/
/* Description : close a connection to a reader    								*/
/*                                                                            */
/******************************************************************************/
/* Parameters:                                                                */
/*  --------------                                                            */
/*  In : t_Reader *pReader : pointer to there reader to close connection    */
/* ---------------                                                            */
/*  Out: none                                                                 */
/* Return value: e_Result                                                     */
/*                          * OK   : connection ok                            */
/*                          * PARAMS_ERR : Inconsistent parameters            */
/******************************************************************************/
e_Result cl_NetwCloseConnectionWin32( clvoid *ptReader )
{
    t_Reader *pReader		= (t_Reader *)ptReader;

	if ( pReader == CL_NULL ) // unload winsock
		return ( CL_ERROR );
	else	// close the connection for a specific reader
		closesocket( pReader->u64Handle );

	return ( CL_OK );
}

/*****************************************************************************/
/* e_Result cl_IPStackSupportLoadWin32( clvoid)								*/
/*																			*/
/* Description : load IP stack support on target OS							*/
/*****************************************************************************/
/* Return Value : 															*/
/*   OK                        :  Result is OK								*/
/*   ERROR,                    : Failure on execution						*/
/****************************************************************************/
e_Result cl_IPStackSupportLoadWin32( clvoid )
{
	WSADATA wsa;

	DEBUG_PRINTF("cl_IPStackSupportLoadWin32:Initialising Winsock..."); // BRY_30032015 fflush(stdout); supprim�
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		DEBUG_PRINTF("Failed. Error Code : ");
		DEBUG_PRINTF( (char *)WSAGetLastError() );
		return ( CL_ERROR );
	}

	return ( CL_OK );
}
/*****************************************************************************/
/* e_Result cl_IPStackSupportUnloadWin32( clvoid)								*/
/*																			*/
/* Description : unload IP stack support on target OS						*/
/*****************************************************************************/
/* Return Value : 															*/
/*   OK                        :  Result is OK								*/
/*****************************************************************************/
e_Result cl_IPStackSupportUnloadWin32( clvoid )
{
	WSACleanup();
	return ( CL_OK );
}

//#endif // 32 bits
//#endif // WIN32
// to discover all lantronix devices:
// send udp broadcast:  00 00 00 F8 to udp port 30718 => answer :  00 00 00 F9 plus 4 hex values = Lantronix reader ip addresses
