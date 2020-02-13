/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: clreaders.c                                                        */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/* Description: APi to manage readers                                        */
/*****************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "..\csl.h"
#include "..\inc\clreaders.h"
#include "..\inc\clthread.h"
#include "..\inc\generic.h"
#include "..\inc\cltuple.h"
#include "..\inc\clhelpers.h"

#include "pthread.h" // TODO : this is not portable
#include <windows.h>

#define USE_WRITE_READ_THREAD_IN_SESSION_MODE

/*--------------------------------------------------------------------------*/

DWORD WaitForThreadsToExit( t_Reader *pReader, int msTimeout );

/*******************************************************************************************/
/* Name : e_Result cl_initReaderSetDefaultFields( 	e_StackSupport eType, 					*/
/*													clu8 *pucReaderName, 					*/
/*													clu32 u32ReaderNameLen, 				*/
/*													t_clIPReader_Params *pIPReaderParams, 	*/
/*													t_clCOMReader_Params *pCOMReaderParams, */
/*													t_clBTReader_Params *pBTReaderParams	*/
/* Description :                                                                            */
/*        init reader structure with default elements							            */
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*		e_StackSupport eType, 			: IP /Bluetooth / COM serial						*/
/*		clu8*        pucReaderName;     : name of the reader								*/
/*		clu32        u32ReaderNameLen;  : length of the reader								*/
/*		t_clIPReader_Params *pIPReaderParams :	IP reader parameters 						*/
/*		t_clCOMReader_Params *pCOMReaderParams:  COM reader parameters						*/
/*		t_clBTReader_Params *pBTReaderParams: 	Bluetooth reader parameters					*/
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*******************************************************************************************/
e_Result cl_initReaderSetDefaultFields( e_StackSupport eType, clu8 *pucReaderName, clu32 u32ReaderNameLen, t_clIPReader_Params *pIPReaderParams, t_clCOMReader_Params *pCOMReaderParams, t_clBTReader_Params *pBTReaderParams )
{
	// variables definition
	t_clContext *pCtxt 			= CL_NULL;
	e_Result	status			= CL_ERROR;

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptCslReader == CL_NULL )
		return ( CL_ERROR );


	if ( u32ReaderNameLen > sizeof( pCtxt->ptCslReader->aucLabel ) )
		return ( CL_PARAMS_ERR );

	if ( ( pIPReaderParams == CL_NULL ) & ( pCOMReaderParams == CL_NULL ) & ( pBTReaderParams == CL_NULL ) )
		return ( CL_PARAMS_ERR );

	if ( ! ( eType & READER_TYPE_MASK) )
		return ( CL_PARAMS_ERR );

	// ensures default reader is not pointing to anything else
	pCtxt->ptCslReader->pNext = CL_NULL;

	// save type of reader by default
	pCtxt->ptCslReader->tType = eType;

	// save default name
	memcpy( pCtxt->ptCslReader->aucLabel, pucReaderName, u32ReaderNameLen );

	if ( pIPReaderParams )
	{
		// copy data
		memcpy( pCtxt->ptCslReader->tIPParams.aucIpAddr, pIPReaderParams->aucIpAddr, sizeof( pIPReaderParams->aucIpAddr ) ) ;	// save IP address
        pCtxt->ptCslReader->tIPParams.u32Port               = pIPReaderParams->u32Port;										// save TCP port
		pCtxt->ptCslReader->tIPParams.u64MacAddr 			= pIPReaderParams->u64MacAddr;										// save MAC address
		pCtxt->ptCslReader->tIPParams.tRange.u64startRange	= pIPReaderParams->tRange.u64startRange;							// save start IP address range for scan and detection
		pCtxt->ptCslReader->tIPParams.tRange.u64stopRange	= pIPReaderParams->tRange.u64stopRange;								// save stop IP address range for scan and detection
        pCtxt->ptCslReader->tIPParams.tRange.u32Port        = pIPReaderParams->u32Port;										// save TCP port to scan to
		pCtxt->ptCslReader->tIPParams.tRange.pNext			= pIPReaderParams->tRange.pNext;
	}
	if ( pCOMReaderParams )
	{
		// copy data
		memcpy( pCtxt->ptCslReader->tCOMParams.aucPortName, pCOMReaderParams->aucPortName, sizeof( pCOMReaderParams->aucPortName ) );
        pCtxt->ptCslReader->tCOMParams.eParityBits                      = pCOMReaderParams->eParityBits;
		pCtxt->ptCslReader->tCOMParams.eStopBits 						= pCOMReaderParams->eStopBits;
		pCtxt->ptCslReader->tCOMParams.eBaudRate 						= pCOMReaderParams->eBaudRate;
		pCtxt->ptCslReader->tCOMParams.eByteSize 						= pCOMReaderParams->eByteSize;
		pCtxt->ptCslReader->tCOMParams.u8ReadIntervallTimeout 			= pCOMReaderParams->u8ReadIntervallTimeout;
		pCtxt->ptCslReader->tCOMParams.u8ReadTotalTimeoutConstant 		= pCOMReaderParams->u8ReadTotalTimeoutConstant;
		pCtxt->ptCslReader->tCOMParams.u8ReadTotalTimeoutMultiplier 	= pCOMReaderParams->u8ReadTotalTimeoutMultiplier;
        pCtxt->ptCslReader->tCOMParams.u8WriteTotalTimeoutConstant      = pCOMReaderParams->u8WriteTotalTimeoutConstant;
		pCtxt->ptCslReader->tCOMParams.u8WriteTotalTimeoutMultiplier 	= pCOMReaderParams->u8WriteTotalTimeoutMultiplier;
        pCtxt->ptCslReader->tCOMParams.tRange.u64startRange             = pCOMReaderParams->tRange.u64startRange;
        pCtxt->ptCslReader->tCOMParams.tRange.u64stopRange				= pCOMReaderParams->tRange.u64stopRange;
        pCtxt->ptCslReader->tCOMParams.tRange.u32Port                   = 0;
        pCtxt->ptCslReader->tCOMParams.tRange.pNext                     = pCOMReaderParams->tRange.pNext;
	}

	if ( pBTReaderParams )
	{
		// copy data
		pCtxt->ptCslReader->tBT.u8Unused 				= pBTReaderParams->u8Unused ;
		pCtxt->ptCslReader->tBT.tRange.u64startRange	= pBTReaderParams->tRange.u64startRange;
        pCtxt->ptCslReader->tBT.tRange.u64stopRange     = pBTReaderParams->tRange.u64startRange;
        pCtxt->ptCslReader->tBT.tRange.u32Port          = 0;
		pCtxt->ptCslReader->tBT.tRange.pNext			= pBTReaderParams->tRange.pNext;
	}

	return ( CL_OK );
}

#define qint64 long long

/*--------------------------------------------------------------------------*\
 * Supress 'filename.exe' from aFullExeName
 *--------------------------------------------------------------------------*
 * aFullExeName : is directory + filename
 * aDirectoryWithoutExeName : will be only directory
 * important remarque : try to set lgExe to strlen(sPos)-1 is a
 * really bad mistake
\*--------------------------------------------------------------------------*/
int GetDirectoryName(char* aFullExeName, char* aDirectoryWithoutExeName)
{
    char str[STRING_LENGTH];
    char* sPos = str;
    size_t lgTot; // total lenght
    size_t lgExe; // lenght of executable name
    size_t lg;    // lenght to copy to dest
    char separator = '\\';

    sPos = strrchr(aFullExeName, separator);
    lgExe = strlen(sPos) - 1;
    lgTot = strlen(aFullExeName);

    lg = lgTot - lgExe;
    strcpy(aDirectoryWithoutExeName, aFullExeName);
    aDirectoryWithoutExeName[lg] = EOS;

    int lg_ret = strlen(aDirectoryWithoutExeName);
    return lg_ret;
}

/*-----------------------------------------------------------------------------------------*\
 * Get all readers from the file: "readers_list.txt"
 * Fill friendlyNames with the friendly names of readers found
 * Add Readers To the List
\*-----------------------------------------------------------------------------------------*/
e_Result cl_InitReadersFromFile(char **friendlyNames)
{
	FILE       	   *file_in;
	errno_t			err_no;
	t_clContext    *pCtxt		= CL_NULL;
	e_Result		status			= CL_ERROR;
	char 			ligne[STRING_LENGTH];
	char 			directory_name[STRING_LENGTH];
	char 			file_name[STRING_LENGTH];
	char 			file_on_disk[STRING_LENGTH] = "readers_list.txt";
    char buf[1024];
    e_ReaderType eType;
    t_Reader tNewReader;
    t_Reader *pReaderInList = CL_NULL;
    size_t ndReaders = 0;
    int iState = 0;
    int iFieldLen = 0;
    long long iFirstIndex ;
    qint64 iLastIndex ;
    qint64 iFoundIndex = 0;
    bool bFound = false;

    int i64LineLen = 0;

	directory_name[0] = EOS;
	file_name[0] = EOS;

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
	{
		DEBUG_PRINTF("cl_InitReadersFromFile: GetContext FAILED !");
	}

	DEBUG_PRINTF("cl_InitReadersFromFile: BEGIN");

	// Open file "readers_list.txt"
	file_in = fopen(file_on_disk, "r");
	if (file_in == NULL)
	{
		GetDirectoryName(pCtxt->tConfigParams.FullPathName, directory_name);
		strcat(file_name, directory_name);
		strcat(file_name, file_on_disk);

		file_in = fopen(file_name, "r");
	}
	if (file_in == 0)
	{
		if ( file_name[0] == EOS )
		{
			DEBUG_PRINTF("cl_InitReadersFromFile: ERROR: Impossible d'ouvrir le fichier %s !", file_on_disk);
		}
		else
		{
			DEBUG_PRINTF("cl_InitReadersFromFile: ERROR: Impossible d'ouvrir le fichier %s !", file_name);
		}
		return CL_ERROR;
	}
	else
	{
		if ( file_name[0] == EOS )
		{
			DEBUG_PRINTF("File IsOpen: %s", file_on_disk);
		}
		else
		{
			DEBUG_PRINTF("File IsOpen: %s", file_name);
		}
	}

	//------------------------------------------------------------------------
	// For all lines in file
	// Fill the reader then add it to the list
	//------------------------------------------------------------------------
	while ( fgets( ligne, STRING_LENGTH, file_in ) != NULL )
	{
		i64LineLen = strlen(ligne);
		strcpy(buf,ligne);

		iFirstIndex = iLastIndex = iFoundIndex = 0;
		char aucArg[64];
		bFound = false;
		iFirstIndex = iLastIndex = iState = 0;
		eType = UNKNOWN_READER_TYPE;

		// don't parse line beginning with ';'
		if ( buf[0] == ';' )
			continue;

		DEBUG_PRINTF(ligne);

		// parse line to capture each field
		int i = 0;
		for ( i = 0; i < i64LineLen; i++ )
		{
			// look for ";"
			if ( buf[i]==';')
			{
				if ( i > 0 )
				{
					iLastIndex = i - 1;
					bFound = true;
				}
			}

			if (bFound == true) // a new field is found => get it
			{
				// prepare array to get new arguments
				memset( aucArg, 0, sizeof( aucArg ) );
				memcpy( aucArg, &buf[iFirstIndex], iLastIndex - iFirstIndex + 1);
				iFieldLen = iLastIndex - iFirstIndex + 1;

				// depending on state, fill either field
				switch ( iState )
				{
					case (0):    //  get Reader Type
					{
						// if first element;
						if ( !memcmp( aucArg, "Serial", strlen("Serial")))
						{

							if ( CL_FAILED( cl_ReaderFillWithDefaultFields( &tNewReader, COM_READER_TYPE ) ) )
								break;
							eType = COM_READER_TYPE;
							bFound = false;
							iFirstIndex = i + 1;
							iState = 1;
							continue;

						}
						// check if IP reader
						if ( !memcmp( aucArg, "IP", strlen("IP")))
						{

							if ( CL_FAILED( cl_ReaderFillWithDefaultFields( &tNewReader, IP_READER_TYPE ) ) )
								break;
							eType = IP_READER_TYPE;
							iFirstIndex = i + 1;
							bFound = false;
							iState = 1;
							continue;
						}
						if ( !memcmp( aucArg, "BlueTooth", strlen("BlueTooth")))
						{

							if ( CL_FAILED( cl_ReaderFillWithDefaultFields( &tNewReader, BT_READER_TYPE ) ) )
								break;
							eType = BT_READER_TYPE;
							iFirstIndex = i + 1;
							iState = 1;
							bFound = false;
							continue;
						}
						break;
					}
					case (1):      // get reader name
					{
						memset( tNewReader.aucLabel, 0, sizeof( tNewReader.aucLabel ) );
						memcpy( tNewReader.aucLabel, &buf[iFirstIndex], iFieldLen );
						if ( friendlyNames != NULL )
						{
							memcpy( &friendlyNames[ndReaders * (sizeof(tNewReader.aucLabel)/sizeof(void *))], tNewReader.aucLabel, iFieldLen ); // TODO : addresse calculation is not good
						}
						ndReaders += 1;
						bFound = false;
						iFirstIndex = i + 1;
						iState = 2;
						continue;
						break;
					}

					case (2): // get serial com port/ip address
					{
						switch ( eType)
						{
							case ( COM_READER_TYPE ):
							{
								// set to 0 all bytes
								memset( tNewReader.tCOMParams.aucPortName, 0, sizeof( tNewReader.tCOMParams.aucPortName ) );

								// copy reader name
								memcpy( tNewReader.tCOMParams.aucPortName, &buf[iFirstIndex], iFieldLen );
								bFound = false;
								iFirstIndex = i + 1;
								iState = 3;
								break;
							}
							case ( IP_READER_TYPE):
							{
								memset( tNewReader.tIPParams.aucIpAddr, 0, sizeof( tNewReader.tIPParams.aucIpAddr ) );
								memcpy( tNewReader.tIPParams.aucIpAddr, &buf[iFirstIndex], iFieldLen);
								bFound = false;
								iFirstIndex = i + 1;
								iState = 3;
								continue;
								break;
							}
							case ( BT_READER_TYPE ):
							{
								bFound = false;
								iFirstIndex = i + 1;
								iState = 3;
								break;
							}
						}
						break;
					}
					case ( 3 ): // get serial port/ip tcp port
					{
						switch (eType)
						{
							case ( COM_READER_TYPE ):
							{
								if (!memcmp( &buf[iFirstIndex], "9600", strlen("9600")))
									tNewReader.tCOMParams.eBaudRate = CL_COM_BAUDRATE_9600;

								if (!memcmp( &buf[iFirstIndex], "19200", strlen("19200")))
									tNewReader.tCOMParams.eBaudRate = CL_COM_BAUDRATE_19200;

								if (!memcmp( &buf[iFirstIndex], "38400", strlen("38400")))
									tNewReader.tCOMParams.eBaudRate = CL_COM_BAUDRATE_38400;

								if (!memcmp( &buf[iFirstIndex], "57600", strlen("57600")))
									tNewReader.tCOMParams.eBaudRate = CL_COM_BAUDRATE_57600;

								if (!memcmp( &buf[iFirstIndex], "115200", strlen("115200")))
									tNewReader.tCOMParams.eBaudRate = CL_COM_BAUDRATE_115200;

								bFound = false;
								iFirstIndex = i + 1;
								iState = 4;
								break;
							}
							case ( IP_READER_TYPE ):
							{
								if ( iFieldLen == 4 )   // IPV4
								{
									// TODO no IP for the moment
//									QByteArray ByteArray2UTF8( (const char *)&buf[iFirstIndex], iFieldLen );
//									char *pData = ByteArray2UTF8.data();
//									tNewReader.tIPParams.u32Port = atoi( pData );
								}

								bFound = false;
								iFirstIndex = i + 1;
								iState = 4;
								break;

							}
							case ( BT_READER_TYPE ):
							{
								bFound = false;
								iFirstIndex = i + 1;
								iState = 4;
								break;
							}
							default:
							{
								bFound = false;
								iFirstIndex = i + 1;
								iState = 4;
							   break;
							}
						}
						break;
					}
					case ( 4 ): // get additionnal data
					{
						switch (eType)
						{
							case ( COM_READER_TYPE ):
							{
								// convert ASCII to Hex
								clu32 ulOutLen = sizeof( tNewReader.tCloverSense.au8RadioAddress );
								ConvertASCIToHex( (clu8*)&buf[iFirstIndex], iFieldLen, (clu8*)tNewReader.tCloverSense.au8RadioAddress, &ulOutLen );
								bFound = false;
								iFirstIndex = i + 1;
								iState = 5;
								break;
							}
							case ( IP_READER_TYPE ):
							{
								// convert ASCII to Hex
								clu32 ulOutLen = sizeof( tNewReader.tCloverSense.au8RadioAddress );
								ConvertASCIToHex( (clu8*)&buf[iFirstIndex], iFieldLen, (clu8*)tNewReader.tCloverSense.au8RadioAddress, &ulOutLen );
								bFound = false;
								iFirstIndex = i + 1;
								iState = 5;
								break;

							}
							case ( BT_READER_TYPE ):
							{
								bFound = false;
								iFirstIndex = i + 1;
								iState = 5;
								break;
							}
							default:
							{
								bFound = false;
								iFirstIndex = i + 1;
								iState = 4;
								break;
							}
						}

						break;
					}
					case ( 5 ): // get additionnal data
					{
						switch (eType)
						{
							case ( COM_READER_TYPE ):
							{
								bFound = false;
								iFirstIndex = i + 1;
								iState = 6;
								break;
							}
							case ( IP_READER_TYPE ):
							{
								bFound = false;
								iFirstIndex = i + 1;
								iState = 6;
								break;

							}
							case ( BT_READER_TYPE ):
							{
								bFound = false;
								iFirstIndex = i + 1;
								iState = 6;
								break;
							}
							default:
							{
								bFound = false;
								iFirstIndex = i + 1;
								iState = 4;
								break;
							}
						}

						break;
					}
					default:
					{
						bFound = false;
						iFirstIndex = i + 1;
						break;
					}
				}
			}
		}

		// Create task&semaphore for write&read to the Reader
		cl_readerAddToList( &tNewReader );

		// test if the reader is populated in the list
		if ( CL_FAILED( cl_readerFindInList( &pReaderInList, &tNewReader ) ) )
		{
			DEBUG_PRINTF("cl_InitReadersFromFile: FAILED with: %s", tNewReader.aucLabel);
		}
		else
		{
			DEBUG_PRINTF("Reader added into Reader's list: %s", tNewReader.aucLabel);
		}
	}

	if (file_in)
	{
		err_no = fclose(file_in);
		if (err_no != 0)
	    {
			DEBUG_PRINTF("cl_InitReadersFromFile: ERROR: Impossible de fermer le fichier !");
	    }
	}

	DEBUG_PRINTF("cl_InitReadersFromFile: END");
	return ( CL_OK );
}

/*--------------------------------------------------------------------------*/

void ConvertASCIToHex( unsigned char *pStringBuf, unsigned inLen, unsigned char *pOutData, unsigned long *pu32OutLen )
{
    clu8 ucNibble = 0;
    if (!pStringBuf )
        return;

    if (!pOutData)
        return;

    if ( !inLen )
        return;

    if ( !pu32OutLen )
        return;

    if ( (*pu32OutLen) < (inLen / 2) ) // BRY_3004 ce n'est pas l'adresse que l'on veut comparer !!!! .... ?????
        return;

    /* convert ASCII to byte */
    int i = 0;
    for (i = 0; i < inLen; i++ )
    {
        if (!(i%2))
        {
            if (( pStringBuf[i]>= 0x30 ) & ( pStringBuf[i] <= 0x39))
                ucNibble = (pStringBuf[i]- 0x30)<<4;
            else
            {
                if (( pStringBuf[i] >= 0x41 ) & ( pStringBuf[i] <0x47))
                    ucNibble = (pStringBuf[i]- 0x41 + 0x0A)<<4;
            }
        }
        else
        {
            if (( pStringBuf[i]>= 0x30 ) & ( pStringBuf[i] <= 0x39))
                ucNibble |= (pStringBuf[i]- 0x30)&0x0F;
            else
            {
                if (( pStringBuf[i] >= 0x41 ) & ( pStringBuf[i] <0x47))
                    ucNibble |= (pStringBuf[i]- 0x41 + 0x0A)&0x0F;
            }
            pOutData[i/2]= ucNibble;
        }
    }
}

/*---------------------------------------------------------------------------*\
 * http://stackoverflow.com/questions/6933039/
 * convert-two-ascii-bytes-in-one-hexadecimal-byte
 */
unsigned char* ConvertStringToHexa( const char *in, size_t len, unsigned char *out )
{
	unsigned int i, t, hn, ln;

	for ( t = 0, i = 0; i < len; i += 2, ++t )
	{
		hn = in[i] > '9' ? in[i] - 'A' + 10 : in[i] - '0';
		ln = in[i+1] > '9' ? in[i+1] - 'A' + 10 : in[i+1] - '0';

		out[t] = (hn << 4 ) | ln;
	}

	return out;
}

/*--------------------------------------------------------------------------*/

void ConvertByteToAscii( unsigned char *pData, unsigned long u32Len, char *pDataInString )
{
    clu8 ucNibble = 0;

    if (!*pDataInString )
        return;

    if (!pData)
        return;

    /* convert from byte to ASCII */
    size_t BufSize = u32Len * 3 + 2 * 3;
    char buff[BufSize];
    memset( buff, 0, sizeof(buff));

    unsigned long i = 0;
    for ( i = 0; i < u32Len; i++ )
    {
        // get nibble MSB
        ucNibble = (((pData[i])>>4)&0x0F);
        if (( ucNibble >=0x00) & ( ucNibble <=0x09))
        	buff[i*3]    =   0x30+ucNibble;
        if (( ucNibble >=0x0A) & ( ucNibble <=0x0F))
        	buff[i*3]    =   0x41+(ucNibble-0x0A);

        ucNibble =  ((pData[i])&0x0F);
        if (( ucNibble >=0x00) & ( ucNibble <=0x09))
        	buff[i*3+1]    =   0x30+ucNibble;
        if (( ucNibble >=0x0A) & ( ucNibble <=0x0F))
        	buff[i*3+1]    =   0x41+(ucNibble-0x0A);

        buff[i*3+2] = 0x20;
    }
    buff[ u32Len * 3] = 0;

    strncpy(pDataInString, buff, BufSize);
}

/*******************************************************************************************/
/* Name : e_Result cl_ReaderFillWithDefaultFields(  t_Reader *p_Reader, e_ReaderType eType ) */
/* Description :                                                                            */
/*        fill current reader structure with default params according to default params		*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*			t_Reader *p_Reader	: reader to fill											*/
/*			e_StackSupport	eType: Type of Reader to fill									*/
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*******************************************************************************************/
e_Result cl_ReaderFillWithDefaultFields( t_Reader *pReader, e_ReaderType eType )
{
	// variables definition
	t_clContext *pCtxt 	= CL_NULL;
	e_Result 	status	= CL_ERROR;

	if ( pReader == CL_NULL )
		return ( CL_PARAMS_ERR );

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( ( !( eType & IP_READER_TYPE ) ) & ( ! ( eType & COM_READER_TYPE ) ) & ( !( eType & BT_READER_TYPE ) ) )
		return ( CL_ERROR );

	// check that the reader type corresponds to the type of support of CSL in terms of IO (IP or COM or BT )
	if ( eType & IP_READER_TYPE )
	{
		if ( !( pCtxt->eStackSupport & IP_STACK_SUPPORT ) )
			return ( CL_ERROR );
	}

	if ( eType & COM_READER_TYPE )
	{
		if ( !( pCtxt->eStackSupport & COM_STACK_SUPPORT ) )
			return ( CL_ERROR );
	}

	if ( eType & BT_READER_TYPE )
	{
		if ( !( pCtxt->eStackSupport & BT_STACK_SUPPORT ) )
			return ( CL_ERROR );
	}

    // set all parameters of reader to CL_NULL
    memset( pReader, 0, sizeof( t_Reader) );

    // set ->pNext to default value from framework
    pReader->pNext = pCtxt->ptCslReader->pNext;

	// save type of reader
	pReader->tType = eType;

	// set reader to default state
	pReader->eState = STATE_DEFAULT;

	// set blocking call as defined in framework
	pReader->tSync.eCallType =pCtxt->ptCslReader->tSync.eCallType;

	// copy default name
    memcpy( pReader->aucLabel, pCtxt->ptCslReader->aucLabel, sizeof( pReader->aucLabel ) );

    // copy default radio address information
    memcpy( pReader->tCloverSense.au8RadioAddress, pCtxt->ptCslReader->tCloverSense.au8RadioAddress, sizeof( pCtxt->ptCslReader->tCloverSense.au8RadioAddress ) );

	// if it is an IP reader
	if ( eType & IP_READER_TYPE )
	{
		memset( pReader->tIPParams.aucIpAddr, 0, sizeof( pReader->tIPParams.aucIpAddr ) );
		memcpy( pReader->tIPParams.aucIpAddr, pCtxt->ptCslReader->tIPParams.aucIpAddr, sizeof( pReader->tIPParams.aucIpAddr ) ) ;
		pReader->tIPParams.u32Port 				= pCtxt->ptCslReader->tIPParams.u32Port;
		pReader->tIPParams.u64MacAddr 			= pCtxt->ptCslReader->tIPParams.u64MacAddr;
		pReader->tIPParams.tRange.u64startRange	= pCtxt->ptCslReader->tIPParams.tRange.u64startRange;
		pReader->tIPParams.tRange.u64stopRange	= pCtxt->ptCslReader->tIPParams.tRange.u64stopRange;
		pReader->tIPParams.tRange.u32Port		= pCtxt->ptCslReader->tIPParams.tRange.u32Port;
		pReader->tIPParams.tRange.pNext			= pCtxt->ptCslReader->tIPParams.tRange.pNext;

	}

	// if it is COM reader
	if ( eType & COM_READER_TYPE )
	{
		// copy data
		memcpy( pReader->tCOMParams.aucPortName, pCtxt->ptCslReader->tCOMParams.aucPortName, sizeof( pReader->tCOMParams.aucPortName ) );
		pReader->tCOMParams.eParityBits 					= pCtxt->ptCslReader->tCOMParams.eParityBits;
		pReader->tCOMParams.eStopBits 						= pCtxt->ptCslReader->tCOMParams.eStopBits;
		pReader->tCOMParams.eBaudRate 						= pCtxt->ptCslReader->tCOMParams.eBaudRate;
		pReader->tCOMParams.eByteSize 						= pCtxt->ptCslReader->tCOMParams.eByteSize;
		pReader->tCOMParams.u8ReadIntervallTimeout 			= pCtxt->ptCslReader->tCOMParams.u8ReadIntervallTimeout;
		pReader->tCOMParams.u8ReadTotalTimeoutConstant 		= pCtxt->ptCslReader->tCOMParams.u8ReadTotalTimeoutConstant;
		pReader->tCOMParams.u8ReadTotalTimeoutMultiplier 	= pCtxt->ptCslReader->tCOMParams.u8ReadTotalTimeoutMultiplier;
		pReader->tCOMParams.u8WriteTotalTimeoutConstant 	= pCtxt->ptCslReader->tCOMParams.u8WriteTotalTimeoutConstant ;
		pReader->tCOMParams.u8WriteTotalTimeoutMultiplier 	= pCtxt->ptCslReader->tCOMParams.u8WriteTotalTimeoutMultiplier ;
		pReader->tCOMParams.tRange.u64startRange			= pCtxt->ptCslReader->tCOMParams.tRange.u64startRange;
		pReader->tCOMParams.tRange.u64stopRange				= pCtxt->ptCslReader->tCOMParams.tRange.u64stopRange;
		pReader->tCOMParams.tRange.u32Port					= pCtxt->ptCslReader->tCOMParams.tRange.u32Port;
		pReader->tCOMParams.tRange.pNext					= pCtxt->ptCslReader->tCOMParams.tRange.pNext;
	}

	// if it is a BT reader
	if ( eType & BT_READER_TYPE )
	{
		// copy data
		pReader->tBT.u8Unused 						= pCtxt->ptCslReader->tBT.u8Unused;
		pReader->tBT.tRange.u64startRange			= pCtxt->ptCslReader->tBT.tRange.u64startRange;
		pReader->tBT.tRange.u64stopRange			= pCtxt->ptCslReader->tBT.tRange.u64stopRange;
		pReader->tBT.tRange.u32Port					= pCtxt->ptCslReader->tBT.tRange.u32Port;
		pReader->tBT.tRange.pNext					= pCtxt->ptCslReader->tBT.tRange.pNext;
	}

	return ( CL_OK );
}

/*--------------------------------------------------------------------------*\
 * FD c'est absoluement incroyable !!!
 * j'ai trouv� du code du genre :
 * if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
 * c'est de niveau 0, il est NULL
 * 11/12/2015 - Faire attention
 * � la fonction cl_readerFindInList ... qui retourne OK si elle a trouv�
 * ou si le pointeur est NULL !!! c'est mortel
\*--------------------------------------------------------------------------*/
e_Result cl_readerAddToList( t_Reader *ptReader)
{
	// variables definition
	t_clContext *pCtxt 			= CL_NULL;
	t_Reader *pCrtReader		= CL_NULL;
	t_Reader *pAllocatedReader	= CL_NULL;
	t_Reader *pFoundReader		= CL_NULL;
	t_Reader *pReader			= CL_NULL;
	e_Result status 			= CL_ERROR;
	clu8 u8Index				= 0;

	pReader = ptReader;

	// check parameters
	if ( pReader == CL_NULL )
		return ( CL_PARAMS_ERR );

	if ( CL_FAILED( cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL )
		return ( CL_MEM_ERR );

	// check if the reader list is ok and if this reader is already in the list
	if ( ( cl_readerFindInList( &pFoundReader, pReader ) == CL_OK ) && ( pFoundReader != CL_NULL )  )
		return ( CL_ERROR );

	DEBUG_PRINTF("cl_readerAddToList: BEGIN");

	// if the reader is already in the list => ok: exit
	if ( pFoundReader != CL_NULL )
	{
		// update last reader accessed
		cl_ReaderSetLastAccessed( pFoundReader );
		return ( CL_OK );
	}

	// if not, then proceed its registration in the list of readers

	// get head of readers
	if ( CL_FAILED( status = cl_GetReaderListEntry( &pCrtReader ) ) )
		return ( CL_MEM_ERR );

	// allocate memory from hosting environment
	//status = pCtxt->ptHalFuncs->fnAllocMem( (clvoid **)&pAllocatedReader, sizeof( t_Reader ) );
	status = csl_malloc( (clvoid **)&pAllocatedReader, sizeof( t_Reader ) );

	// if FAILED, exit
	if ( CL_FAILED (status) )
		return ( CL_MEM_ERR );

	// reset all fields in new allocated reader
	memset( pAllocatedReader, 0, sizeof( t_Reader ) );

	// ensure that pNext is NULL for correct chaining of the list
	pAllocatedReader->pNext = CL_NULL;

	// ensure that tuple list is NULL for this new reader
	pAllocatedReader->p_TplList2Send = CL_NULL;
	pAllocatedReader->p_TplList2Read = CL_NULL;

	// if list was empty, initialize it with this allocated memory
	if ( pCrtReader == CL_NULL )
	{
		// get last non NULL element in the list
		pCrtReader = pAllocatedReader;
		cl_SetReaderListEntry( pAllocatedReader );
	}
	else
	{
		// get last non NULL element in the list
		while ( pCrtReader->pNext != CL_NULL )
		{
			pCrtReader = (t_Reader *)( pCrtReader->pNext );
		}
		// fill next with allocated memory
		pCrtReader->pNext = (t_Reader *)pAllocatedReader;

		// displace to new one
		pCrtReader = ( t_Reader *) pCrtReader->pNext ;
	}

	// prefill the readers with frameworks elements if any
	if ( CL_FAILED ( status = cl_ReaderFillWithDefaultFields( pCrtReader, pReader->tType ) ) )
		return CL_ERROR;

	// ..... and data
	memcpy( pCrtReader->tCloverSense.au8RadioAddress, pReader->tCloverSense.au8RadioAddress, sizeof( pReader->tCloverSense.au8RadioAddress) ) ;

	// depending on reader type ... fill different structure
	switch ( (pReader->tType & READER_TYPE_MASK) )
	{
		case ( IP_READER_TYPE ):	// copy parameters for IP readers
		{
			DEBUG_PRINTF("cl_readerAddToList: IP_READER_TYPE");


			status = CL_OK;
			break;
		}
		case ( COM_READER_TYPE ):	// copy parameters for COM readers
		{
			DEBUG_PRINTF("cl_readerAddToList: COM_READER_TYPE");

			pCrtReader->tCOMParams.eStopBits	= pReader->tCOMParams.eStopBits;										///	define stop bits on serial
			pCrtReader->tCOMParams.eParityBits	= pReader->tCOMParams.eParityBits;										/// define parity bits on serial
			pCrtReader->tCOMParams.u8ReadIntervallTimeout	= pReader->tCOMParams.u8ReadIntervallTimeout;				/// Timeout not to hang if no reception
			pCrtReader->tCOMParams.u8ReadTotalTimeoutConstant	= pReader->tCOMParams.u8ReadTotalTimeoutConstant;		/// Timeout not to hang if no reception
			pCrtReader->tCOMParams.u8ReadTotalTimeoutMultiplier	= pReader->tCOMParams.u8ReadTotalTimeoutMultiplier;		/// Timeout not to hang if no reception
			pCrtReader->tCOMParams.u8WriteTotalTimeoutConstant	= pReader->tCOMParams.u8WriteTotalTimeoutConstant;		/// Timeout not to hang if no reception
			pCrtReader->tCOMParams.u8WriteTotalTimeoutMultiplier	= pReader->tCOMParams.u8WriteTotalTimeoutMultiplier;/// Timeout not to hang if no reception
			memcpy( pCrtReader->tCOMParams.aucPortName, pReader->tCOMParams.aucPortName, sizeof( pReader->tCOMParams.aucPortName) );

			DEBUG_PRINTF("cl_readerAddToList: aucPortName: %s", pCrtReader->tCOMParams.aucPortName);

			// use HAL functions from the framework if the user didn"t provide it for the reader
			if ( pReader->tReaderHalFuncs.fnIOCloseConnection == CL_NULL )
			{
				if ( pCtxt->p_COMReaderHal != CL_NULL )
					pCrtReader->tReaderHalFuncs.fnIOCloseConnection = pCtxt->p_COMReaderHal->fnIOCloseConnection ;
			}
			else
				pCrtReader->tReaderHalFuncs.fnIOCloseConnection = ptReader->tReaderHalFuncs.fnIOCloseConnection;

			if ( pReader->tReaderHalFuncs.fnIOGetDNSTable == CL_NULL )
			{
				if ( pCtxt->p_COMReaderHal != CL_NULL )
					pCrtReader->tReaderHalFuncs.fnIOGetDNSTable = pCtxt->p_COMReaderHal->fnIOGetDNSTable ;
			}
			else
				pCrtReader->tReaderHalFuncs.fnIOGetDNSTable = ptReader->tReaderHalFuncs.fnIOGetDNSTable;

			if ( pReader->tReaderHalFuncs.fnIOGetData == CL_NULL )
			{
				if ( pCtxt->p_COMReaderHal != CL_NULL )
					pCrtReader->tReaderHalFuncs.fnIOGetData = pCtxt->p_COMReaderHal->fnIOGetData ;
			}
			else
				pCrtReader->tReaderHalFuncs.fnIOGetData = ptReader->tReaderHalFuncs.fnIOGetData;

			if ( pReader->tReaderHalFuncs.fnIOOpenConnection == CL_NULL )
			{
				if ( pCtxt->p_COMReaderHal != CL_NULL )
					pCrtReader->tReaderHalFuncs.fnIOOpenConnection = pCtxt->p_COMReaderHal->fnIOOpenConnection ;
			}
			else
				pCrtReader->tReaderHalFuncs.fnIOOpenConnection = ptReader->tReaderHalFuncs.fnIOOpenConnection;

			if ( pReader->tReaderHalFuncs.fnIORegister == CL_NULL )
			{
				if ( pCtxt->p_COMReaderHal != CL_NULL )
					pCrtReader->tReaderHalFuncs.fnIORegister = pCtxt->p_COMReaderHal->fnIORegister ;
			}
			else
				pCrtReader->tReaderHalFuncs.fnIORegister = ptReader->tReaderHalFuncs.fnIORegister;

			if ( pReader->tReaderHalFuncs.fnIOReset == CL_NULL )
			{
				if ( pCtxt->p_COMReaderHal != CL_NULL )
					pCrtReader->tReaderHalFuncs.fnIOReset = pCtxt->p_COMReaderHal->fnIOReset ;
			}
			else
				pCrtReader->tReaderHalFuncs.fnIOReset = ptReader->tReaderHalFuncs.fnIOReset;

			if ( pReader->tReaderHalFuncs.fnIOSendData == CL_NULL )
			{
				if ( pCtxt->p_COMReaderHal != CL_NULL )
					pCrtReader->tReaderHalFuncs.fnIOSendData = pCtxt->p_COMReaderHal->fnIOSendData ;
			}
			else
				pCrtReader->tReaderHalFuncs.fnIOSendData = ptReader->tReaderHalFuncs.fnIOSendData;

			if ( pReader->tReaderHalFuncs.fnIOUnregister == CL_NULL )
			{
				if ( pCtxt->p_COMReaderHal != CL_NULL )
					pCrtReader->tReaderHalFuncs.fnIOUnregister = pCtxt->p_COMReaderHal->fnIOUnregister ;
			}
			else
				pCrtReader->tReaderHalFuncs.fnIOUnregister = ptReader->tReaderHalFuncs.fnIOUnregister;

			if ( pReader->tReaderHalFuncs.fnSetIPRange == CL_NULL )
			{
				if ( pCtxt->p_COMReaderHal != CL_NULL )
					pCrtReader->tReaderHalFuncs.fnSetIPRange = pCtxt->p_COMReaderHal->fnSetIPRange ;
			}
			else
				pCrtReader->tReaderHalFuncs.fnSetIPRange = ptReader->tReaderHalFuncs.fnSetIPRange;

			// use Discover HAL functions from the framework if the user didn"t provide it for the reader
			if ( pReader->tReaderDiscoverHalFuncs.fnIOCloseConnection == CL_NULL )
			{
				if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
					pCrtReader->tReaderDiscoverHalFuncs.fnIOCloseConnection = pCtxt->p_COMReaderDiscoverHal->fnIOCloseConnection ;
			}
			else
				pCrtReader->tReaderDiscoverHalFuncs.fnIOCloseConnection = ptReader->tReaderDiscoverHalFuncs.fnIOCloseConnection;

			if ( pReader->tReaderDiscoverHalFuncs.fnIOGetDNSTable == CL_NULL )
			{
				if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
					pCrtReader->tReaderDiscoverHalFuncs.fnIOGetDNSTable = pCtxt->p_COMReaderDiscoverHal->fnIOGetDNSTable ;
			}
			else
				pCrtReader->tReaderDiscoverHalFuncs.fnIOGetDNSTable = ptReader->tReaderDiscoverHalFuncs.fnIOGetDNSTable;

			if ( pReader->tReaderDiscoverHalFuncs.fnIOGetData == CL_NULL )
			{
				if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
					pCrtReader->tReaderDiscoverHalFuncs.fnIOGetData = pCtxt->p_COMReaderDiscoverHal->fnIOGetData ;
			}
			else
				pCrtReader->tReaderDiscoverHalFuncs.fnIOGetData = ptReader->tReaderDiscoverHalFuncs.fnIOGetData;

			if ( pReader->tReaderDiscoverHalFuncs.fnIOOpenConnection == CL_NULL )
			{
				if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
					pCrtReader->tReaderDiscoverHalFuncs.fnIOOpenConnection = pCtxt->p_COMReaderDiscoverHal->fnIOOpenConnection ;
			}
			else
				pCrtReader->tReaderDiscoverHalFuncs.fnIOOpenConnection = ptReader->tReaderDiscoverHalFuncs.fnIOOpenConnection;

			if ( pReader->tReaderDiscoverHalFuncs.fnIORegister == CL_NULL )
			{
				if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
					pCrtReader->tReaderDiscoverHalFuncs.fnIORegister = pCtxt->p_COMReaderDiscoverHal->fnIORegister ;
			}
			else
				pCrtReader->tReaderDiscoverHalFuncs.fnIORegister = ptReader->tReaderDiscoverHalFuncs.fnIORegister;

			if ( pReader->tReaderDiscoverHalFuncs.fnIOReset == CL_NULL )
			{
				if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
					pCrtReader->tReaderDiscoverHalFuncs.fnIOReset = pCtxt->p_COMReaderDiscoverHal->fnIOReset ;
			}
			else
				pCrtReader->tReaderDiscoverHalFuncs.fnIOReset = ptReader->tReaderDiscoverHalFuncs.fnIOReset;

			if ( pReader->tReaderDiscoverHalFuncs.fnIOSendData == CL_NULL )
			{
				if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
					pCrtReader->tReaderDiscoverHalFuncs.fnIOSendData = pCtxt->p_COMReaderDiscoverHal->fnIOSendData ;
			}
			else
				pCrtReader->tReaderDiscoverHalFuncs.fnIOSendData = ptReader->tReaderDiscoverHalFuncs.fnIOSendData;

			if ( pReader->tReaderDiscoverHalFuncs.fnIOUnregister == CL_NULL )
			{
				if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
					pCrtReader->tReaderDiscoverHalFuncs.fnIOUnregister = pCtxt->p_COMReaderDiscoverHal->fnIOUnregister ;
			}
			else
				pCrtReader->tReaderDiscoverHalFuncs.fnIOUnregister = ptReader->tReaderDiscoverHalFuncs.fnIOUnregister;

			if ( pReader->tReaderDiscoverHalFuncs.fnSetIPRange == CL_NULL )
			{
				if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
					pCrtReader->tReaderDiscoverHalFuncs.fnSetIPRange = pCtxt->p_COMReaderDiscoverHal->fnSetIPRange ;
			}
			else
				pCrtReader->tReaderDiscoverHalFuncs.fnSetIPRange = ptReader->tReaderDiscoverHalFuncs.fnSetIPRange;


			status =	CL_OK;
			break;
		}
		case ( BT_READER_TYPE ) :	// copy parameters for BT reader (no support yet-placeholder)
		{
			DEBUG_PRINTF("cl_readerAddToList: COM_READER_TYPE");
			status =	CL_OK;
			break;
		}
		default:
		{
			DEBUG_PRINTF("Unknown reader type");
			status = CL_ERROR;
			break;
		}
	}

	pCrtReader->tType = pReader->tType;

	// copy its user-friendly name
	memcpy( pCrtReader->aucLabel, pReader->aucLabel, sizeof( pCrtReader->aucLabel) );

	//--------------------------------------------------
	// copy callback for send/receive/reset if specified for this reader otherwise copy default from project
	// 1. for read data from IOs
	if ( pReader->tCallBacks.fnIOData2Read_cb != CL_NULL )
		pCrtReader->tCallBacks.fnIOData2Read_cb	= pReader->tCallBacks.fnIOData2Read_cb ;
	else
	{
		if ( pCtxt->ptCallbacks != CL_NULL )
			pCrtReader->tCallBacks.fnIOData2Read_cb		= pCtxt->ptCallbacks->fnIOData2Read_cb;
	}


	// 2. for send data to IOs
	if ( pReader->tCallBacks.fnIOSendDataDone_cb != CL_NULL )
		pCrtReader->tCallBacks.fnIOSendDataDone_cb		= pReader->tCallBacks.fnIOSendDataDone_cb ;
	else
	{
		if ( pCtxt->ptCallbacks != CL_NULL )
			pCrtReader->tCallBacks.fnIOSendDataDone_cb		= pCtxt->ptCallbacks->fnIOSendDataDone_cb;
	}

	// 3. for reset callback on reader
	if ( pReader->tCallBacks.fnReset_cb != CL_NULL )
			pCrtReader->tCallBacks.fnReset_cb		= pReader->tCallBacks.fnReset_cb ;
		else
		{
			if ( pCtxt->ptCallbacks != CL_NULL )
				pCrtReader->tCallBacks.fnReset_cb		= pCtxt->ptCallbacks->fnReset_cb;
		}

	//--------------------------------------------------
	// same for discover callbacks
	// 1. for read data from IOs
	if ( pReader->tCallBacksDiscover.fnIOData2Read_cb != CL_NULL )
		pCrtReader->tCallBacksDiscover.fnIOData2Read_cb		= pReader->tCallBacksDiscover.fnIOData2Read_cb ;
	else
	{
		if ( pCtxt->ptCallbacksDiscover != CL_NULL )
			pCrtReader->tCallBacksDiscover.fnIOData2Read_cb		= pCtxt->ptCallbacksDiscover->fnIOData2Read_cb;
	}

	// 2. for send data to IOs
	if ( pReader->tCallBacksDiscover.fnIOSendDataDone_cb != CL_NULL )
		pCrtReader->tCallBacksDiscover.fnIOSendDataDone_cb		= pReader->tCallBacksDiscover.fnIOSendDataDone_cb ;
	else
	{
		if ( pCtxt->ptCallbacksDiscover != CL_NULL )
		{
			pCrtReader->tCallBacksDiscover.fnIOSendDataDone_cb		= pCtxt->ptCallbacksDiscover->fnIOSendDataDone_cb;
		}
	}

	//------------------------------------------------------
	// same for OTA callbacks
	if ( pReader->tCallBacks.fnOTAData2Read_cb != CL_NULL )
		pCrtReader->tCallBacks.fnOTAData2Read_cb		= pReader->tCallBacks.fnOTAData2Read_cb ;
	else
	{
		if ( pCtxt->ptCallbacks != CL_NULL )
			pCrtReader->tCallBacks.fnOTAData2Read_cb		= pCtxt->ptCallbacks->fnOTAData2Read_cb;
	}

	if ( pReader->tCallBacks.fnOTAProgress_cb != CL_NULL )
		pCrtReader->tCallBacks.fnOTAProgress_cb		= pReader->tCallBacks.fnOTAProgress_cb ;
	else
	{
		if ( pCtxt->ptCallbacks != CL_NULL )
			pCrtReader->tCallBacks.fnOTAProgress_cb		= pCtxt->ptCallbacks->fnOTAProgress_cb;
	}

	if ( pReader->tCallBacks.fnOTASendDataDone_cb != CL_NULL )
		pCrtReader->tCallBacks.fnOTASendDataDone_cb		= pReader->tCallBacks.fnOTASendDataDone_cb ;
	else
	{
		if ( pCtxt->ptCallbacks != CL_NULL )
			pCrtReader->tCallBacks.fnOTASendDataDone_cb		= pCtxt->ptCallbacks->fnOTASendDataDone_cb;
	}

	// 3. for reset callback on reader
	if ( pReader->tCallBacksDiscover.fnReset_cb != CL_NULL )
		pCrtReader->tCallBacksDiscover.fnReset_cb		= pReader->tCallBacksDiscover.fnReset_cb ;
	else
	{
		if ( pCtxt->ptCallbacksDiscover != CL_NULL )
			pCrtReader->tCallBacksDiscover.fnReset_cb	= pCtxt->ptCallbacksDiscover->fnReset_cb;
	}

	if ( pReader->tCallBacks.fnReset_cb != CL_NULL )
		pCrtReader->tCallBacks.fnReset_cb		= pReader->tCallBacks.fnReset_cb ;
	else
	{
		if ( pCtxt->ptCallbacks != CL_NULL )
			pCrtReader->tCallBacks.fnReset_cb	= pCtxt->ptCallbacks->fnReset_cb;
	}

	//
	// Parsing parameters
	//
	pCrtReader->tParsingParams.StartIndex = 0;
	pCrtReader->tParsingParams.EndIndex = 0;
	pCrtReader->tParsingParams.StartOfTuple = 0;
	pCrtReader->tParsingParams.EndOfTuple = 0;
	pCrtReader->tParsingParams.MiddleIndex = 0;
	pCrtReader->tParsingParams.pDataStart = CL_NULL;
	pCrtReader->tParsingParams.pDataEnd = CL_NULL;


	// set the synchronization tags between read/write thread properly
	// retries at low level
	pCrtReader->tSync.u32Retries = pCtxt->ptCslReader->tSync.u32Retries;

	// default state for synchronization status between read and write threads.
	pCrtReader->tSync.eStatus = pCtxt->ptCslReader->tSync.eStatus;

	// default timeout on low level exchange of reader
	pCrtReader->tSync.u32SerialTimeout = pCtxt->ptCslReader->tSync.u32SerialTimeout;

	//
	// Multithreading Objects
	//

	// create signals for read thread
	status = pCtxt->ptHalFuncs->fnSemaphoreCreate( &pCrtReader->tSync.pSgl4Read );

	// create signal for write thread
	status = pCtxt->ptHalFuncs->fnSemaphoreCreate( &pCrtReader->tSync.pSgl4Write );

	// create signal for write completion at under layers
	status = pCtxt->ptHalFuncs->fnSemaphoreCreate( &pCrtReader->tSync.pSgl4WriteComplete );

	// create signal for read successful completion at under layers
	status = pCtxt->ptHalFuncs->fnSemaphoreCreate( &pCrtReader->tSync.pSgl4ReadComplete );

	// Mutex to avoid more than on thread wreiting on SerialPortCom
	status =  pCtxt->ptHalFuncs->fnMutexCreate( &pCrtReader->tSync.mutexWriteOnPortCom );

	DEBUG_PRINTF("cl_readerAddToList: status: %s", status == 0 ? "OK" : "NOT OK!");

	if ( CL_FAILED( status ) )// deallocate properly memory/elements in the list....
	{
		// destroy signals
		if ( pCtxt->ptHalFuncs->fnSemaphoreDestroy != CL_NULL)
		{
			pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4Write );
			pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4Read );
			pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4ReadComplete );
			pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4WriteComplete );
		}

		// destroy threads
		if ( pCtxt->ptHalFuncs->fnDestroyThread != CL_NULL )
		{

			pCtxt->ptHalFuncs->fnDestroyThread( pCrtReader->tSync.tThreadId4Read );
			pCtxt->ptHalFuncs->fnDestroyThread( pCrtReader->tSync.tThreadId4Write );
		}

		// finally, deallocate memory if needed
		if (pReader != CL_NULL)
		{
			pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&pReader );
			pReader = CL_NULL;
		}
		DEBUG_PRINTF("cl_readerAddToList: status: FAILED");
	}

	// update last reader accessed
	cl_ReaderSetLastAccessed( pCrtReader );

	DEBUG_PRINTF("cl_readerAddToList: END");

	return ( status );
}

/**************************************************************************/
/* Name : e_Result cl_readerAddToList( t_Reader *pReader);             		*/
/* Description :                                                          	*/
/*        Add pReader to the list of Readers								*/
/**************************************************************************/
/* Parameters:                                                            	*/
/*  --------------                                                        	*/
/*  In: 	t_Reader *pReader: new reader to add to List			    	*/
/*                           	cannot be NULL                              */
/* ---------------                                                        	*/
/*  Out: none                                                             	*/
/* Return value: e_Result                                                 	*/
/*  CL_OK                        :  Result is OK                            */
/*  CL_ERROR,                    : Failure on execution                     */
/*  CL_MEM_ERR,                  :  Failure on memory management (failure,  */
/*                                  allocation ....)                      	*/
/*  CL_PARAMS_ERR,               :  Inconsistent parameters                 */
/**************************************************************************/
e_Result OLD_THING_THAT_STINKS_cl_readerAddToList( t_Reader *ptReader)
{
	// variables definition
	t_clContext *pCtxt 			= CL_NULL;
	t_Reader *pCrtReader		= CL_NULL;
	t_Reader *pAllocatedReader	= CL_NULL;
	t_Reader *pFoundReader		= CL_NULL;
	t_Reader *pReader			= CL_NULL;
	e_Result status 			= CL_ERROR;
	clu8 u8Index				= 0;

	pReader = ptReader ;
	// check parameters
	if ( pReader == CL_NULL )
		return ( CL_PARAMS_ERR );

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL )
		return ( CL_MEM_ERR );

	// check if the reader list is ok and if this reader is already in the list
	if ( CL_FAILED (status =  cl_readerFindInList( &pFoundReader, pReader ) ) )
		return ( CL_ERROR );

	DEBUG_PRINTF("cl_readerAddToList: BEGIN");

	// if the reader is already in the list => ok: exit
	if ( pFoundReader != CL_NULL )
	{
		// update last reader accessed
		cl_ReaderSetLastAccessed( pFoundReader );
		return ( CL_OK );
	}
	// if not, then proceed its registration in the list of readers

	// get head of readers
	if ( CL_FAILED( status = cl_GetReaderListEntry( &pCrtReader ) ) )
		return ( CL_MEM_ERR );

	// allocate memory from hosting environment
	//status = pCtxt->ptHalFuncs->fnAllocMem( (clvoid **)&pAllocatedReader, sizeof( t_Reader ) );
	status = csl_malloc( (clvoid **)&pAllocatedReader, sizeof( t_Reader ) );

	// if FAILED, exit
	if ( CL_FAILED (status) )
		return ( CL_MEM_ERR );

	// reset all fields in new allocated reader
	memset( pAllocatedReader, 0, sizeof( t_Reader ) );

	// ensure that pNext is NULL for correct chaining of the list
	pAllocatedReader->pNext = CL_NULL;

	// ensure that tuple list is NULL for this new reader
	pAllocatedReader->p_TplList2Send = CL_NULL;
	pAllocatedReader->p_TplList2Read = CL_NULL;

	// if list was empty, initialize it with this allocated memory
	if ( pCrtReader == CL_NULL )
	{
		// get last non NULL element in the list
		pCrtReader = pAllocatedReader;
		cl_SetReaderListEntry( pAllocatedReader );
	}
	else
	{
		// get last non NULL element in the list
		while ( pCrtReader->pNext != CL_NULL )
		{
			pCrtReader = (t_Reader *)( pCrtReader->pNext);
		}
		// fill next with allocated memory
		pCrtReader->pNext = (t_Reader *) pAllocatedReader;

		// displace to new one
		pCrtReader = ( t_Reader *) pCrtReader->pNext ;
	}

	// fill element of the list
	do
	{
		// prefill the readers with frameworks elements if any
		if ( CL_FAILED ( status = cl_ReaderFillWithDefaultFields( pCrtReader, pReader->tType ) ) )
			break;;

		// ..... and data
		memcpy( pCrtReader->tCloverSense.au8RadioAddress, pReader->tCloverSense.au8RadioAddress, sizeof( pReader->tCloverSense.au8RadioAddress) ) ;

		// depending on reader type ... fill different structure
		switch ( (pReader->tType & READER_TYPE_MASK) )
		{
			case ( IP_READER_TYPE ):	// copy parameters for IP readers
			{
				DEBUG_PRINTF("cl_readerAddToList: IP_READER_TYPE");

				pCrtReader->tIPParams.u64MacAddr = pReader->tIPParams.u64MacAddr;
				pCrtReader->tIPParams.u32Port = pReader->tIPParams.u32Port;
				memcpy( pCrtReader->tIPParams.aucIpAddr, pReader->tIPParams.aucIpAddr, sizeof( pReader->tIPParams.aucIpAddr) );

				// use HAL functions from the framework if the user didn"t provide it for the reader
                if ( pReader->tReaderHalFuncs.fnIOCloseConnection == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderHal != CL_NULL )
                        pCrtReader->tReaderHalFuncs.fnIOCloseConnection = pCtxt->p_IPReaderHal->fnIOCloseConnection ;
                }
                else
                    pCrtReader->tReaderHalFuncs.fnIOCloseConnection = ptReader->tReaderHalFuncs.fnIOCloseConnection;

                if ( pReader->tReaderHalFuncs.fnIOGetDNSTable == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderHal != CL_NULL )
                        pCrtReader->tReaderHalFuncs.fnIOGetDNSTable = pCtxt->p_IPReaderHal->fnIOGetDNSTable ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOGetDNSTable = ptReader->tReaderHalFuncs.fnIOGetDNSTable ;

                if ( pReader->tReaderHalFuncs.fnIOGetData == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderHal != CL_NULL )
                        pCrtReader->tReaderHalFuncs.fnIOGetData = pCtxt->p_IPReaderHal->fnIOGetData ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOGetData = ptReader->tReaderHalFuncs.fnIOGetData ;

                if ( pReader->tReaderHalFuncs.fnIOOpenConnection == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderHal != CL_NULL )
                        pCrtReader->tReaderHalFuncs.fnIOOpenConnection = pCtxt->p_IPReaderHal->fnIOOpenConnection ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOOpenConnection = ptReader->tReaderHalFuncs.fnIOOpenConnection ;

                if ( pReader->tReaderHalFuncs.fnIORegister == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderHal != CL_NULL )
                        pCrtReader->tReaderHalFuncs.fnIORegister = pCtxt->p_IPReaderHal->fnIORegister ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIORegister = ptReader->tReaderHalFuncs.fnIORegister ;

                if ( pReader->tReaderHalFuncs.fnIOReset == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderHal != CL_NULL )
                        pCrtReader->tReaderHalFuncs.fnIOReset = pCtxt->p_IPReaderHal->fnIOReset ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOReset = ptReader->tReaderHalFuncs.fnIOReset ;

                if ( pReader->tReaderHalFuncs.fnIOSendData == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderHal != CL_NULL )
                        pCrtReader->tReaderHalFuncs.fnIOSendData = pCtxt->p_IPReaderHal->fnIOSendData ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOSendData = ptReader->tReaderHalFuncs.fnIOSendData ;

                if ( pReader->tReaderHalFuncs.fnIOUnregister == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderHal != CL_NULL )
                        pCrtReader->tReaderHalFuncs.fnIOUnregister = pCtxt->p_IPReaderHal->fnIOUnregister ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOUnregister = ptReader->tReaderHalFuncs.fnIOUnregister ;

                if ( pReader->tReaderHalFuncs.fnSetIPRange == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderHal != CL_NULL )
                        pCrtReader->tReaderHalFuncs.fnSetIPRange = pCtxt->p_IPReaderHal->fnSetIPRange ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnSetIPRange = ptReader->tReaderHalFuncs.fnSetIPRange ;


				// use Discover HAL functions from the framework if the user didn"t provide it for the reader
                if ( pReader->tReaderDiscoverHalFuncs.fnIOCloseConnection == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOCloseConnection = pCtxt->p_IPReaderDiscoverHal->fnIOCloseConnection ;
                }
                else
                    pCrtReader->tReaderDiscoverHalFuncs.fnIOCloseConnection = ptReader->tReaderDiscoverHalFuncs.fnIOCloseConnection;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOGetDNSTable == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOGetDNSTable = pCtxt->p_IPReaderDiscoverHal->fnIOGetDNSTable ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOGetDNSTable = ptReader->tReaderDiscoverHalFuncs.fnIOGetDNSTable ;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOGetData == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOGetData = pCtxt->p_IPReaderDiscoverHal->fnIOGetData ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOGetData = ptReader->tReaderDiscoverHalFuncs.fnIOGetData ;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOOpenConnection == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOOpenConnection = pCtxt->p_IPReaderDiscoverHal->fnIOOpenConnection ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOOpenConnection = ptReader->tReaderDiscoverHalFuncs.fnIOOpenConnection ;

                if ( pReader->tReaderDiscoverHalFuncs.fnIORegister == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIORegister = pCtxt->p_IPReaderDiscoverHal->fnIORegister ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIORegister = ptReader->tReaderDiscoverHalFuncs.fnIORegister ;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOReset == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOReset = pCtxt->p_IPReaderDiscoverHal->fnIOReset ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOReset = ptReader->tReaderDiscoverHalFuncs.fnIOReset ;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOSendData == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOSendData = pCtxt->p_IPReaderDiscoverHal->fnIOSendData ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOSendData = ptReader->tReaderDiscoverHalFuncs.fnIOSendData ;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOUnregister == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOUnregister = pCtxt->p_IPReaderDiscoverHal->fnIOUnregister ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOUnregister = ptReader->tReaderDiscoverHalFuncs.fnIOUnregister ;

                if ( pReader->tReaderDiscoverHalFuncs.fnSetIPRange == CL_NULL )
                {
                    if ( pCtxt->p_IPReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnSetIPRange = pCtxt->p_IPReaderDiscoverHal->fnSetIPRange ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnSetIPRange = ptReader->tReaderDiscoverHalFuncs.fnSetIPRange ;
				status =	CL_OK;
				break;
			}
			case ( COM_READER_TYPE ):	// copy parameters for COM readers
			{
				DEBUG_PRINTF("cl_readerAddToList: COM_READER_TYPE");

				pCrtReader->tCOMParams.eStopBits	= pReader->tCOMParams.eStopBits;										///	define stop bits on serial
				pCrtReader->tCOMParams.eParityBits	= pReader->tCOMParams.eParityBits;										/// define parity bits on serial
				pCrtReader->tCOMParams.u8ReadIntervallTimeout	= pReader->tCOMParams.u8ReadIntervallTimeout;				/// Timeout not to hang if no reception
				pCrtReader->tCOMParams.u8ReadTotalTimeoutConstant	= pReader->tCOMParams.u8ReadTotalTimeoutConstant;		/// Timeout not to hang if no reception
				pCrtReader->tCOMParams.u8ReadTotalTimeoutMultiplier	= pReader->tCOMParams.u8ReadTotalTimeoutMultiplier;		/// Timeout not to hang if no reception
				pCrtReader->tCOMParams.u8WriteTotalTimeoutConstant	= pReader->tCOMParams.u8WriteTotalTimeoutConstant;		/// Timeout not to hang if no reception
				pCrtReader->tCOMParams.u8WriteTotalTimeoutMultiplier	= pReader->tCOMParams.u8WriteTotalTimeoutMultiplier;/// Timeout not to hang if no reception
				memcpy( pCrtReader->tCOMParams.aucPortName, pReader->tCOMParams.aucPortName, sizeof( pReader->tCOMParams.aucPortName) );

				DEBUG_PRINTF("cl_readerAddToList: aucPortName: %s", pCrtReader->tCOMParams.aucPortName);

				// use HAL functions from the framework if the user didn"t provide it for the reader
                if ( pReader->tReaderHalFuncs.fnIOCloseConnection == CL_NULL )
                {
                	if ( pCtxt->p_COMReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnIOCloseConnection = pCtxt->p_COMReaderHal->fnIOCloseConnection ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOCloseConnection = ptReader->tReaderHalFuncs.fnIOCloseConnection;

                if ( pReader->tReaderHalFuncs.fnIOGetDNSTable == CL_NULL )
                {
                	if ( pCtxt->p_COMReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnIOGetDNSTable = pCtxt->p_COMReaderHal->fnIOGetDNSTable ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOGetDNSTable = ptReader->tReaderHalFuncs.fnIOGetDNSTable;

                if ( pReader->tReaderHalFuncs.fnIOGetData == CL_NULL )
                {
                	if ( pCtxt->p_COMReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnIOGetData = pCtxt->p_COMReaderHal->fnIOGetData ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOGetData = ptReader->tReaderHalFuncs.fnIOGetData;

                if ( pReader->tReaderHalFuncs.fnIOOpenConnection == CL_NULL )
                {
                	if ( pCtxt->p_COMReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnIOOpenConnection = pCtxt->p_COMReaderHal->fnIOOpenConnection ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOOpenConnection = ptReader->tReaderHalFuncs.fnIOOpenConnection;

                if ( pReader->tReaderHalFuncs.fnIORegister == CL_NULL )
                {
                	if ( pCtxt->p_COMReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnIORegister = pCtxt->p_COMReaderHal->fnIORegister ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIORegister = ptReader->tReaderHalFuncs.fnIORegister;

                if ( pReader->tReaderHalFuncs.fnIOReset == CL_NULL )
                {
                	if ( pCtxt->p_COMReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnIOReset = pCtxt->p_COMReaderHal->fnIOReset ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOReset = ptReader->tReaderHalFuncs.fnIOReset;

                if ( pReader->tReaderHalFuncs.fnIOSendData == CL_NULL )
                {
                	if ( pCtxt->p_COMReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnIOSendData = pCtxt->p_COMReaderHal->fnIOSendData ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOSendData = ptReader->tReaderHalFuncs.fnIOSendData;

                if ( pReader->tReaderHalFuncs.fnIOUnregister == CL_NULL )
                {
                	if ( pCtxt->p_COMReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnIOUnregister = pCtxt->p_COMReaderHal->fnIOUnregister ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOUnregister = ptReader->tReaderHalFuncs.fnIOUnregister;

                if ( pReader->tReaderHalFuncs.fnSetIPRange == CL_NULL )
                {
                	if ( pCtxt->p_COMReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnSetIPRange = pCtxt->p_COMReaderHal->fnSetIPRange ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnSetIPRange = ptReader->tReaderHalFuncs.fnSetIPRange;

				// use Discover HAL functions from the framework if the user didn"t provide it for the reader
                if ( pReader->tReaderDiscoverHalFuncs.fnIOCloseConnection == CL_NULL )
                {
                    if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOCloseConnection = pCtxt->p_COMReaderDiscoverHal->fnIOCloseConnection ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOCloseConnection = ptReader->tReaderDiscoverHalFuncs.fnIOCloseConnection;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOGetDNSTable == CL_NULL )
                {
                    if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOGetDNSTable = pCtxt->p_COMReaderDiscoverHal->fnIOGetDNSTable ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOGetDNSTable = ptReader->tReaderDiscoverHalFuncs.fnIOGetDNSTable;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOGetData == CL_NULL )
                {
                    if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOGetData = pCtxt->p_COMReaderDiscoverHal->fnIOGetData ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOGetData = ptReader->tReaderDiscoverHalFuncs.fnIOGetData;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOOpenConnection == CL_NULL )
                {
                    if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOOpenConnection = pCtxt->p_COMReaderDiscoverHal->fnIOOpenConnection ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOOpenConnection = ptReader->tReaderDiscoverHalFuncs.fnIOOpenConnection;

                if ( pReader->tReaderDiscoverHalFuncs.fnIORegister == CL_NULL )
                {
                    if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIORegister = pCtxt->p_COMReaderDiscoverHal->fnIORegister ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIORegister = ptReader->tReaderDiscoverHalFuncs.fnIORegister;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOReset == CL_NULL )
                {
                    if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOReset = pCtxt->p_COMReaderDiscoverHal->fnIOReset ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOReset = ptReader->tReaderDiscoverHalFuncs.fnIOReset;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOSendData == CL_NULL )
                {
                    if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOSendData = pCtxt->p_COMReaderDiscoverHal->fnIOSendData ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOSendData = ptReader->tReaderDiscoverHalFuncs.fnIOSendData;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOUnregister == CL_NULL )
                {
                    if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOUnregister = pCtxt->p_COMReaderDiscoverHal->fnIOUnregister ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOUnregister = ptReader->tReaderDiscoverHalFuncs.fnIOUnregister;

                if ( pReader->tReaderDiscoverHalFuncs.fnSetIPRange == CL_NULL )
                {
                    if ( pCtxt->p_COMReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnSetIPRange = pCtxt->p_COMReaderDiscoverHal->fnSetIPRange ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnSetIPRange = ptReader->tReaderDiscoverHalFuncs.fnSetIPRange;


				status =	CL_OK;
				break;
			}
			case ( BT_READER_TYPE ) :	// copy parameters for BT reader (no support yet-placeholder)
			{
				DEBUG_PRINTF("cl_readerAddToList: COM_READER_TYPE");

                if ( pReader->tReaderHalFuncs.fnIOCloseConnection == CL_NULL )
                {
                	if ( pCtxt->p_BTReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnIOCloseConnection = pCtxt->p_BTReaderHal->fnIOCloseConnection ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOCloseConnection = ptReader->tReaderHalFuncs.fnIOCloseConnection;

                if ( pReader->tReaderHalFuncs.fnIOGetDNSTable == CL_NULL )
                {
                	if ( pCtxt->p_BTReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnIOGetDNSTable = pCtxt->p_BTReaderHal->fnIOGetDNSTable ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOGetDNSTable = ptReader->tReaderHalFuncs.fnIOGetDNSTable;

                if ( pReader->tReaderHalFuncs.fnIOGetData == CL_NULL )
                {
                	if ( pCtxt->p_BTReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnIOGetData = pCtxt->p_BTReaderHal->fnIOGetData ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOGetData = ptReader->tReaderHalFuncs.fnIOGetData;

                if ( pReader->tReaderHalFuncs.fnIOOpenConnection == CL_NULL )
                {
                	if ( pCtxt->p_BTReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnIOOpenConnection = pCtxt->p_BTReaderHal->fnIOOpenConnection ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOOpenConnection = ptReader->tReaderHalFuncs.fnIOOpenConnection;

                if ( pReader->tReaderHalFuncs.fnIORegister == CL_NULL )
                {
                	if ( pCtxt->p_BTReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnIORegister = pCtxt->p_BTReaderHal->fnIORegister ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIORegister = ptReader->tReaderHalFuncs.fnIORegister;

                if ( pReader->tReaderHalFuncs.fnIOReset == CL_NULL )
                {
                	if ( pCtxt->p_BTReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnIOReset = pCtxt->p_BTReaderHal->fnIOReset ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOReset = ptReader->tReaderHalFuncs.fnIOReset;

                if ( pReader->tReaderHalFuncs.fnIOSendData == CL_NULL )
                {
                	if ( pCtxt->p_BTReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnIOSendData = pCtxt->p_BTReaderHal->fnIOSendData ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOSendData = ptReader->tReaderHalFuncs.fnIOSendData;

                if ( pReader->tReaderHalFuncs.fnIOUnregister == CL_NULL )
                {
                	if ( pCtxt->p_BTReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnIOUnregister = pCtxt->p_BTReaderHal->fnIOUnregister ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnIOUnregister = ptReader->tReaderHalFuncs.fnIOUnregister;

                if ( pReader->tReaderHalFuncs.fnSetIPRange == CL_NULL )
                {
                	if ( pCtxt->p_BTReaderHal != CL_NULL )
                		pCrtReader->tReaderHalFuncs.fnSetIPRange = pCtxt->p_BTReaderHal->fnSetIPRange ;
                }
				else
					pCrtReader->tReaderHalFuncs.fnSetIPRange = ptReader->tReaderHalFuncs.fnSetIPRange;

                // HAL for discover process

                if ( pReader->tReaderDiscoverHalFuncs.fnIOCloseConnection == CL_NULL )
                {
                    if ( pCtxt->p_BTReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOCloseConnection = pCtxt->p_BTReaderDiscoverHal->fnIOCloseConnection ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOCloseConnection = ptReader->tReaderDiscoverHalFuncs.fnIOCloseConnection;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOGetDNSTable == CL_NULL )
                {
                    if ( pCtxt->p_BTReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOGetDNSTable = pCtxt->p_BTReaderDiscoverHal->fnIOGetDNSTable ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOGetDNSTable = ptReader->tReaderDiscoverHalFuncs.fnIOGetDNSTable;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOGetData == CL_NULL )
                {
                    if ( pCtxt->p_BTReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOGetData = pCtxt->p_BTReaderDiscoverHal->fnIOGetData ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOGetData = ptReader->tReaderDiscoverHalFuncs.fnIOGetData;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOOpenConnection == CL_NULL )
                {
                    if ( pCtxt->p_BTReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOOpenConnection = pCtxt->p_BTReaderDiscoverHal->fnIOOpenConnection ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOOpenConnection = ptReader->tReaderDiscoverHalFuncs.fnIOOpenConnection;

                if ( pReader->tReaderDiscoverHalFuncs.fnIORegister == CL_NULL )
                {
                    if ( pCtxt->p_BTReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIORegister = pCtxt->p_BTReaderDiscoverHal->fnIORegister ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIORegister = ptReader->tReaderDiscoverHalFuncs.fnIORegister;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOReset == CL_NULL )
                {
                    if ( pCtxt->p_BTReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOReset = pCtxt->p_BTReaderDiscoverHal->fnIOReset ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOReset = ptReader->tReaderDiscoverHalFuncs.fnIOReset;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOSendData == CL_NULL )
                {
                    if ( pCtxt->p_BTReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOSendData = pCtxt->p_BTReaderDiscoverHal->fnIOSendData ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOSendData = ptReader->tReaderDiscoverHalFuncs.fnIOSendData;

                if ( pReader->tReaderDiscoverHalFuncs.fnIOUnregister == CL_NULL )
                {
                    if ( pCtxt->p_BTReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnIOUnregister = pCtxt->p_BTReaderDiscoverHal->fnIOUnregister ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnIOUnregister = ptReader->tReaderDiscoverHalFuncs.fnIOUnregister;

                if ( pReader->tReaderDiscoverHalFuncs.fnSetIPRange == CL_NULL )
                {
                    if ( pCtxt->p_BTReaderDiscoverHal != CL_NULL )
                        pCrtReader->tReaderDiscoverHalFuncs.fnSetIPRange = pCtxt->p_BTReaderDiscoverHal->fnSetIPRange ;
                }
				else
					pCrtReader->tReaderDiscoverHalFuncs.fnSetIPRange = ptReader->tReaderDiscoverHalFuncs.fnSetIPRange;

				status =	CL_OK;
				break;
			}
			default:
			{
				DEBUG_PRINTF("Unknown reader type");
				status = CL_ERROR;
				break;
			}
		}

		pCrtReader->tType 	= pReader->tType;

		// copy its user-friendly name
		memcpy( pCrtReader->aucLabel, pReader->aucLabel, sizeof( pCrtReader->aucLabel) );

		//--------------------------------------------------
		// copy callback for send/receive/reset if specified for this reader otherwise copy default from project
		// 1. for read data from IOs
		if ( pReader->tCallBacks.fnIOData2Read_cb != CL_NULL )
			pCrtReader->tCallBacks.fnIOData2Read_cb		= pReader->tCallBacks.fnIOData2Read_cb ;
		else
		{
			if ( pCtxt->ptCallbacks != CL_NULL )
				pCrtReader->tCallBacks.fnIOData2Read_cb		= pCtxt->ptCallbacks->fnIOData2Read_cb;
		}


		// 2. for send data to IOs
		if ( pReader->tCallBacks.fnIOSendDataDone_cb != CL_NULL )
			pCrtReader->tCallBacks.fnIOSendDataDone_cb		= pReader->tCallBacks.fnIOSendDataDone_cb ;
		else
		{
			if ( pCtxt->ptCallbacks != CL_NULL )
				pCrtReader->tCallBacks.fnIOSendDataDone_cb		= pCtxt->ptCallbacks->fnIOSendDataDone_cb;
		}

		// 3. for reset callback on reader
		if ( pReader->tCallBacks.fnReset_cb != CL_NULL )
				pCrtReader->tCallBacks.fnReset_cb		= pReader->tCallBacks.fnReset_cb ;
			else
			{
				if ( pCtxt->ptCallbacks != CL_NULL )
					pCrtReader->tCallBacks.fnReset_cb		= pCtxt->ptCallbacks->fnReset_cb;
			}

		//--------------------------------------------------
		// same for discover callbacks
		// 1. for read data from IOs
		if ( pReader->tCallBacksDiscover.fnIOData2Read_cb != CL_NULL )
			pCrtReader->tCallBacksDiscover.fnIOData2Read_cb		= pReader->tCallBacksDiscover.fnIOData2Read_cb ;
		else
		{
			if ( pCtxt->ptCallbacksDiscover != CL_NULL )
				pCrtReader->tCallBacksDiscover.fnIOData2Read_cb		= pCtxt->ptCallbacksDiscover->fnIOData2Read_cb;
		}

		// 2. for send data to IOs
		if ( pReader->tCallBacksDiscover.fnIOSendDataDone_cb != CL_NULL )
			pCrtReader->tCallBacksDiscover.fnIOSendDataDone_cb		= pReader->tCallBacksDiscover.fnIOSendDataDone_cb ;
		else
		{
			if ( pCtxt->ptCallbacksDiscover != CL_NULL )
			{
				pCrtReader->tCallBacksDiscover.fnIOSendDataDone_cb		= pCtxt->ptCallbacksDiscover->fnIOSendDataDone_cb;
			}
		}

		//------------------------------------------------------
		// same for OTA callbacks
		if ( pReader->tCallBacks.fnOTAData2Read_cb != CL_NULL )
			pCrtReader->tCallBacks.fnOTAData2Read_cb		= pReader->tCallBacks.fnOTAData2Read_cb ;
		else
		{
			if ( pCtxt->ptCallbacks != CL_NULL )
				pCrtReader->tCallBacks.fnOTAData2Read_cb		= pCtxt->ptCallbacks->fnOTAData2Read_cb;
		}

		if ( pReader->tCallBacks.fnOTAProgress_cb != CL_NULL )
			pCrtReader->tCallBacks.fnOTAProgress_cb		= pReader->tCallBacks.fnOTAProgress_cb ;
		else
		{
			if ( pCtxt->ptCallbacks != CL_NULL )
				pCrtReader->tCallBacks.fnOTAProgress_cb		= pCtxt->ptCallbacks->fnOTAProgress_cb;
		}

		if ( pReader->tCallBacks.fnOTASendDataDone_cb != CL_NULL )
			pCrtReader->tCallBacks.fnOTASendDataDone_cb		= pReader->tCallBacks.fnOTASendDataDone_cb ;
		else
		{
			if ( pCtxt->ptCallbacks != CL_NULL )
				pCrtReader->tCallBacks.fnOTASendDataDone_cb		= pCtxt->ptCallbacks->fnOTASendDataDone_cb;
		}



		// 3. for reset callback on reader
		if ( pReader->tCallBacksDiscover.fnReset_cb != CL_NULL )
			pCrtReader->tCallBacksDiscover.fnReset_cb		= pReader->tCallBacksDiscover.fnReset_cb ;
		else
		{
			if ( pCtxt->ptCallbacksDiscover != CL_NULL )
				pCrtReader->tCallBacksDiscover.fnReset_cb	= pCtxt->ptCallbacksDiscover->fnReset_cb;
		}

		if ( pReader->tCallBacks.fnReset_cb != CL_NULL )
			pCrtReader->tCallBacks.fnReset_cb		= pReader->tCallBacks.fnReset_cb ;
		else
		{
			if ( pCtxt->ptCallbacks != CL_NULL )
				pCrtReader->tCallBacks.fnReset_cb	= pCtxt->ptCallbacks->fnReset_cb;
		}

        //--------------------------------------------------
		// then, instantiate necessary synchronization/threads/tasks

		// set the synchronization tags between read/write thread properly
		// retries at low level
		pCrtReader->tSync.u32Retries = pCtxt->ptCslReader->tSync.u32Retries;

		// default state for synchronization status between read and write threads.
		pCrtReader->tSync.eStatus = pCtxt->ptCslReader->tSync.eStatus;

		// default timeout on low level exchange of reader
		pCrtReader->tSync.u32SerialTimeout	=	pCtxt->ptCslReader->tSync.u32SerialTimeout;

		// create signals for read thread
		status = pCtxt->ptHalFuncs->fnSemaphoreCreate( &pCrtReader->tSync.pSgl4Read );
//		if (CL_FAILED( status ) ) break;

		// BRY_2404 surtout pas !!!
//		// ensure the semaphore is released at the origin
//		if ( CL_FAILED( pCtxt->ptHalFuncs->fnSemaphoreRelease( pCrtReader->tSync.pSgl4Read ) ) )
//				break;

		// create signal for write thread
		status = pCtxt->ptHalFuncs->fnSemaphoreCreate( &pCrtReader->tSync.pSgl4Write );
//		if (CL_FAILED( status ) ) break;

		// BRY_2404
//		// ensure the semaphore is released at the origin
//		if ( CL_FAILED( pCtxt->ptHalFuncs->fnSemaphoreRelease( pCrtReader->tSync.pSgl4Write ) ) )
//				break;

		// create signal for write completion at under layers
		status = pCtxt->ptHalFuncs->fnSemaphoreCreate( &pCrtReader->tSync.pSgl4WriteComplete );
//		if (CL_FAILED( status ) ) break;

		// BRY_2404
//		// ensure the semaphore is released at the origin
//		if ( CL_FAILED( pCtxt->ptHalFuncs->fnSemaphoreRelease( pCrtReader->tSync.pSgl4WriteComplete ) ) )
//				break;

		// create signal for read successful completion at under layers
		status = pCtxt->ptHalFuncs->fnSemaphoreCreate( &pCrtReader->tSync.pSgl4ReadComplete );
//		if (CL_FAILED( status ) ) break;


		// BRY_2404
//		// ensure the semaphore is released at the origin
//		if ( CL_FAILED( pCtxt->ptHalFuncs->fnSemaphoreRelease( pCrtReader->tSync.pSgl4ReadComplete ) ) )
//				break;

		// create thread for read

		// BRY_05102015
//		status = pCtxt->ptHalFuncs->fnCreateThread( cl_ReaderAndStatusProc, SIZE_STACK_THREAD, pCrtReader, &pCrtReader->tSync.tThreadId4Read );

		// BRY_06102015
//		pCrtReader->tSync.tThreadId4Read = CreateThread
//		(
//			NULL,
//			0,
//			(LPTHREAD_START_ROUTINE) cl_ReaderAndStatusProc,
//			(LPVOID) pCrtReader,
//			0,
//			&pCrtReader->tSync.tThreadId4Read
//		);
//
//		pCrtReader->tCOMParams.handleThreadExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//	    if (pCrtReader->tCOMParams.handleThreadExitEvent == NULL)
//	    	DEBUG_PRINTF("cl_readerAddToList: CreateEvent (Thread exit event)");
//
////		if (CL_FAILED( status ) ) break;
//
//		if ( pCrtReader->tType == IP_READER_TYPE)
//		{
//			DEBUG_PRINTF("Reader %s Thread id is %d \n", pCrtReader->tIPParams.aucIpAddr, pCrtReader->tSync.tThreadId4Read );
//		}
//		// register this thread to enable closure on failure
////		status = clRegisterThread( clReaderReadThread, pCrtReader->tSync.tThreadId4Read );
////		if (CL_FAILED( status ) ) break;
//
//		// create thread for write
//		status = pCtxt->ptHalFuncs->fnCreateThread( clReaderWriteThread, SIZE_STACK_THREAD, pCrtReader, &pCrtReader->tSync.tThreadId4Write );
////		if (CL_FAILED( status ) ) break;
//
//		// register this thread to enable closure on failure
//		status = clRegisterThread( clReaderWriteThread, pCrtReader->tSync.tThreadId4Write );
////		if (CL_FAILED( status ) ) break;
//
//		// Initialise mutex
//		// SESSION_MODE_CANCELED pCtxt->ptHalFuncs->fnMutexCreate(&pCrtReader->tSync.mutexReadIncomingWR);
//
//		// set the reader to STATE_INIT to signal it is ready to be connected
//		pCrtReader->eState =	STATE_INIT ;

		DEBUG_PRINTF("cl_readerAddToList: status: %s", status == 0 ? "OK" : "NOT OK!");

		// Quel CON ce FD � quoi �a sert Cr�tin !
		break;
	}
	while (1);

	if ( CL_FAILED( status ) )// deallocate properly memory/elements in the list....
	{
		// destroy signals
		if ( pCtxt->ptHalFuncs->fnSemaphoreDestroy != CL_NULL)
		{
			pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4Write );
			pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4Read );
			pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4ReadComplete );
			pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4WriteComplete );
		}

		// destroy threads
		if ( pCtxt->ptHalFuncs->fnDestroyThread != CL_NULL )
		{

			pCtxt->ptHalFuncs->fnDestroyThread( pCrtReader->tSync.tThreadId4Read );
			pCtxt->ptHalFuncs->fnDestroyThread( pCrtReader->tSync.tThreadId4Write );
		}

		// finally, deallocate memory if needed
		if (pReader != CL_NULL)
		{
			pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&pReader );
			pReader = CL_NULL;
		}
		DEBUG_PRINTF("cl_readerAddToList: status: FAILED");
	}

	// update last reader accessed
	cl_ReaderSetLastAccessed( pCrtReader );

	DEBUG_PRINTF("cl_readerAddToList: END");

	return ( status );
}

/*--------------------------------------------------------------------------*\
 * Le probleme est que cl_readerAddToList old stink shit de FF startait
 * les threads alors que c'est pas utile
\*--------------------------------------------------------------------------*/
e_Result cl_ReaderStartThreads( t_Reader *ptReader )
{
	t_Reader *pReaderFound = CL_NULL;
	t_Reader *pReader = ptReader;
	e_Result status = CL_ERROR;
	t_clContext *pCtxt = CL_NULL;

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( CL_FAILED( status = cl_readerFindInListByAddress( &pReaderFound, pReader ) ) )
	{
		DEBUG_PRINTF("cl_ReaderStartThreads: cl_readerFindInListByAddress: Reader NOT FOUNDED !");
		return CL_ERROR;
	}

	// Create thread for Read
	pReaderFound->tSync.tThreadId4Read = CreateThread
	(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE) cl_ReaderAndStatusProc,
		(LPVOID) pReaderFound,
		0,
		&pReaderFound->tSync.tThreadId4Read
	);

	if ( pReaderFound->tSync.tThreadId4Read == NULL )
	{
		DEBUG_PRINTF("cl_ReaderStartThreads: tThreadId4Read: ERROR NULL");
		return CL_ERROR;
	}

	pReaderFound->tCOMParams.handleThreadExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (pReaderFound->tCOMParams.handleThreadExitEvent == NULL)
	{
		DEBUG_PRINTF("cl_ReaderStartThreads: handleThreadExitEvent: ERROR NULL");
		return CL_ERROR;
	}

	// Create thread for Write
	status = pCtxt->ptHalFuncs->fnCreateThread( clReaderWriteThread, SIZE_STACK_THREAD, pReaderFound, &pReaderFound->tSync.tThreadId4Write );

	return status;
}

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
e_Result cl_readerFindInList( t_Reader **ppReader, t_Reader *pReaderFilter )
{
	// variables definition
	t_clContext *pCtxt 		= CL_NULL;
	t_Reader *pCrtReader	= CL_NULL;
	e_Result status 		= CL_ERROR;

	// check parameters
	if ( pReaderFilter == CL_NULL )
		return ( CL_PARAMS_ERR );

	// get Head of readers' list
	if ( CL_FAILED( status = cl_GetReaderListEntry( &pCrtReader ) ) )
		return ( CL_MEM_ERR );

	// if list of readers is NULL, none was added... it is not a bug... continue}

	// check others parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL )
		return ( CL_MEM_ERR );

	DEBUG_PRINTF("cl_readerFindInList: BEGIN");

	while ( pCrtReader != CL_NULL )
	{

		// filter on IP address?
		status = CL_ERROR;
        do
        {
            if ( pCrtReader->tType & IP_READER_TYPE )
            {
                // filter on IP address
                if (  memcmp( pCrtReader->tIPParams.aucIpAddr, pReaderFilter->tIPParams.aucIpAddr, sizeof( pCrtReader->tIPParams.aucIpAddr )) )
                    break;

                // filter on Mac address?
                if ( pReaderFilter->tIPParams.u64MacAddr != 0 )
                {
                    if ( pCrtReader->tIPParams.u64MacAddr != pReaderFilter->tIPParams.u64MacAddr )
                        break;
                }

                // filter on name
                if (memcmp( pCrtReader->aucLabel, pReaderFilter->aucLabel, sizeof( pCrtReader->aucLabel )) )
                    break;

                // Filter on port
                if ( pReaderFilter->tIPParams.u32Port != 0 )
                {
                    if ( pCrtReader->tIPParams.u32Port == pReaderFilter->tIPParams.u32Port )
                        status = CL_OK;
                    else
                        break;
                }
                else
                    status = CL_OK;
            }

            if ( pCrtReader->tType & COM_READER_TYPE )
            {
            	DEBUG_PRINTF1("cl_readerFindInList: COM_READER_TYPE");

                status = CL_ERROR;

                if ( pCrtReader->tCOMParams.eStopBits != pReaderFilter->tCOMParams.eStopBits )										///	define stop bits on serial
                    break;

                if ( pCrtReader->tCOMParams.eParityBits	!= pReaderFilter->tCOMParams.eParityBits )
                    break; 																										/// define parity bits on serial

                if ( pCrtReader->tCOMParams.u8ReadIntervallTimeout	!= pReaderFilter->tCOMParams.u8ReadIntervallTimeout)
                    break;																								/// Timeout not to hang if no reception

                if ( pCrtReader->tCOMParams.u8ReadTotalTimeoutConstant	!= pReaderFilter->tCOMParams.u8ReadTotalTimeoutConstant )	// TODO	/// Timeout not to hang if no reception
                    break;
                if (pCrtReader->tCOMParams.u8ReadTotalTimeoutMultiplier	!= pReaderFilter->tCOMParams.u8ReadTotalTimeoutMultiplier )		/// Timeout not to hang if no reception
                    break;

                if (pCrtReader->tCOMParams.u8WriteTotalTimeoutConstant	!= pReaderFilter->tCOMParams.u8WriteTotalTimeoutConstant )	/// Timeout not to hang if no reception
                    break;

                if (pCrtReader->tCOMParams.u8WriteTotalTimeoutMultiplier	!= pReaderFilter->tCOMParams.u8WriteTotalTimeoutMultiplier )/// Timeout not to hang if no reception
                    break;

                if ( memcmp( pCrtReader->tCOMParams.aucPortName, pReaderFilter->tCOMParams.aucPortName, sizeof( pReaderFilter->tCOMParams.aucPortName) ) )
                    break;

                status = CL_OK;
            }

            if  ( pCrtReader->tType & BT_READER_TYPE )
            {
                if ( pCrtReader->tBT.u8Unused != pReaderFilter->tBT.u8Unused ) /// Timeout not to hang if no reception
                    break;
                status = CL_OK;
            }
            break;

        } while ( 1 );

		// if a match was found, exit on success and returns the correct reader.
		if ( status == CL_OK )
		{
			DEBUG_PRINTF("cl_readerFindInList: aucLabel: %s", pCrtReader->aucLabel );

			*ppReader = pCrtReader;
			break;
		}

		// points on next element of the readers list
		pCrtReader = (t_Reader *)pCrtReader->pNext;
	};

	DEBUG_PRINTF("cl_readerFindInList: END");

	return ( CL_OK );
}

/*--------------------------------------------------------------------------*/

e_Result cl_readerFindInListByFriendlyName( t_Reader **ppReader, char *aReaderFriendlyName )
{
	// variables definition
	t_clContext *pCtxt 		= CL_NULL;
	t_Reader *pCrtReader	= CL_NULL;
	e_Result status 		= CL_ERROR;

	// check parameters
	if ( aReaderFriendlyName == CL_NULL )
		return ( CL_PARAMS_ERR );

	// get Head of readers' list
	if ( CL_FAILED( status = cl_GetReaderListEntry( &pCrtReader ) ) )
		return ( CL_MEM_ERR );

	// if list of readers is NULL, none was added... it is not a bug... continue}

	// check others parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL )
		return ( CL_MEM_ERR );

	DEBUG_PRINTF1("cl_readerFindInListByFriendlyName: BEGIN");

	while ( pCrtReader != CL_NULL )
	{
		// filter on IP address?
		status = CL_ERROR;
        do
        {
            if ( pCrtReader->tType & IP_READER_TYPE )
            {
            }

            if ( pCrtReader->tType & COM_READER_TYPE )
            {
            	DEBUG_PRINTF1("cl_readerFindInListByFriendlyName: COM_READER_TYPE");
                if ( memcmp( pCrtReader->aucLabel, aReaderFriendlyName, 11 ) ) // XX XX XX XX = 11 chars
                    break;

                status = CL_OK;
            }

            if  ( pCrtReader->tType & BT_READER_TYPE )
            {
            }
            break;

        } while ( 1 );

		// if a match was found, exit on success and returns the correct reader.
		if ( status == CL_OK )
		{
			DEBUG_PRINTF("cl_readerFindInListByFriendlyName: aucLabel: %s FOUNDED", pCrtReader->aucLabel );

			*ppReader = pCrtReader;
			break;
		}

		// points on next element of the readers list
		pCrtReader = (t_Reader *)pCrtReader->pNext;
	}

	DEBUG_PRINTF1("cl_readerFindInListByFriendlyName: END");

	return status;
}

/*--------------------------------------------------------------------------*/

e_Result cl_readerFindInListByAddress( t_Reader **ppReader, t_Reader *pReaderAdress )
{
	// variables definition
	t_clContext *pCtxt 		= CL_NULL;
	t_Reader *pCrtReader	= CL_NULL;
	e_Result status 		= CL_ERROR;

	// check parameters
	if ( pReaderAdress == CL_NULL )
		return ( CL_PARAMS_ERR );

	// get Head of readers' list
	if ( CL_FAILED( status = cl_GetReaderListEntry( &pCrtReader ) ) )
		return ( CL_MEM_ERR );

	// check others parameters
	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL )
		return ( CL_MEM_ERR );

	DEBUG_PRINTF("cl_readerFindInListByAddress: BEGIN");

	while ( pCrtReader != CL_NULL )
	{
		if  ( pCrtReader == pReaderAdress )
		{
			DEBUG_PRINTF("cl_readerFindInListByAddress: Founded: aucLabel: %s", pCrtReader->aucLabel );
			*ppReader = pCrtReader;
			status = CL_OK;
			break;
		}

		// Points to the next element of the readers list
		pCrtReader = (t_Reader *)pCrtReader->pNext;
	}

	DEBUG_PRINTF("cl_readerFindInListByAddress: END");

	return CL_OK;
}

/**************************************************************************/
/* Name : e_Result cl_ReaderRemoveFromList( t_Reader *pReader)             */
/* Description :                                                         	 */
/*        Remove a reader from the list										*/
/**************************************************************************/
/* Parameters:                                                            	*/
/*  --------------                                                        	*/
/*  In: 	t_Reader *pReader: Remove the reader from the list. NULL not 	*/
/*			allowed															*/
/* ---------------                                                        	*/
/*  Out: 																	 */
/* Return value: e_Result                                                 	*/
/*  CL_OK                        :  Result is OK                             */
/*  CL_ERROR,                    : Failure on execution or list of readers 	*/
/*												is empty                      */
/*  CL_MEM_ERR,                  :  Failure on memory management (failure,   */
/*                                  allocation ....)                      	*/
/*  CL_PARAMS_ERR,               :  Inconsistent parameters                  */
/**************************************************************************/
e_Result cl_ReaderRemoveFromList( t_Reader *pReader )
{
	t_clContext *pCtxt 		= CL_NULL;
	t_Reader *pCrtReader	= CL_NULL;
	t_Reader *pPrvReader	= CL_NULL;
	t_Reader *pTempReader	= CL_NULL;

	e_Result status 		= CL_ERROR;

	// check parameters
	if ( pReader == CL_NULL )
		return CL_PARAMS_ERR;

	// get Head of readers' list
	if ( CL_FAILED( status = cl_GetReaderListEntry( &pCrtReader ) ) )
		return CL_MEM_ERR;

	// if list of readers is NULL, we cannot find a reader that satisfies the conditions=> exit on CL_ERROR
	if ( pCrtReader == CL_NULL )
		return CL_ERROR;

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return CL_ERROR;

	if ( pCtxt == CL_NULL )
		return CL_ERROR;

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return CL_ERROR;

// BRY_28092015 encore des conneries � FD !!!!!
//	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )
//		return ( CL_MEM_ERR );

	//
	// Parse Readers list
	//
	while ( pCrtReader != CL_NULL )
	{
		if ( pCrtReader == pReader ) // Current Reader is the one to remove from the list
		{
			//
			// Previous reader will point to next element of the list
			// if we remove the first element of the list, then ...
			// start the list of readers with the second element of the list
			//
            if ( !pPrvReader )
                pPrvReader = pCrtReader->pNext;
            else // if we remove a subsequent reader, then update the linking pointers between each
            	pPrvReader->pNext = pCrtReader->pNext;

            DEBUG_PRINTF("cl_ReaderRemoveFromList: Reader: %s", pCrtReader->aucLabel);

            // Non
//			if ( pCrtReader->eState == STATE_CONNECT )
//			{
//				if ( WaitForThreadsToExit( pCrtReader, 1000 ) != WAIT_OBJECT_0 ) // 20000
//				{
//					DEBUG_PRINTF( "cl_ReaderRemoveFromList: WaitForThreadsToExit: ERROR" );
//				}
//			}

            // Non ! Le reader peut etre deconnecte et la threadRead exister
			if ( pCrtReader->tSync.tThreadId4Read != NULL )
			{
				if ( WaitForThreadsToExit( pCrtReader, 1000 ) != WAIT_OBJECT_0 ) // 20000
				{
					DEBUG_PRINTF( "cl_ReaderRemoveFromList: WaitForThreadsToExit: ERROR" );
				}
			}

            if ( pCtxt->ptHalFuncs->fnDestroyThread != CL_NULL )
            {
//            	if ( pCrtReader->tType == IP_READER_TYPE )
//            	{
//            		DEBUG_PRINTF("Reader IP %s destroy thread %d\n", pCrtReader->tIPParams.aucIpAddr, pCrtReader->tSync.tThreadId4Read );
//            	}

            	// Destroy mutex (if reader is not connected mutex is not used)
				// BRY_: Wait for the end of Lock before suppress the mutex
            	// SESSION_MODE_CANCELED
//				DEBUG_PRINTF("cl_ReaderRemoveFromList: Mutex_Destroy: lock");
//				pthread_mutex_lock( &pCrtReader->tSync.mutexReadIncomingWR ); // necessar in SessionMode
//				pthread_mutex_unlock( &pCrtReader->tSync.mutexReadIncomingWR ); // necessar in SessionMode
//				pCtxt->ptHalFuncs->fnMutexDestroy( &pCrtReader->tSync.mutexReadIncomingWR );
//				DEBUG_PRINTF("cl_ReaderRemoveFromList: Mutex_Destroy: ok");

				//
				// Destroy Reader's threads
				//
//            	DEBUG_PRINTF("cl_ReaderRemoveFromList: Destroy_Thread");
//            	if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnDestroyThread( pCrtReader->tSync.tThreadId4Read ) ) )
//            	{
//            		DEBUG_PRINTF("cl_ReaderRemoveFromList: tThreadId4Read: FAILED");
//            	}
//
            	if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnDestroyThread( pCrtReader->tSync.tThreadId4Write ) ) )
            	{
            		DEBUG_PRINTF("cl_ReaderRemoveFromList: tThreadId4Write: FAILED");
            	}
            }

			// Destroy signals
			if ( pCtxt->ptHalFuncs->fnSemaphoreDestroy != CL_NULL)
			{
				DEBUG_PRINTF("cl_ReaderRemoveFromList: Destroy_Semaphore: BEGIN");
				status = pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4Write );
				if ( status != CL_OK )
				{
					DEBUG_PRINTF("cl_ReaderRemoveFromList: SemaphoreDestroy: pSgl4Write ERROR");
				}
				else
				{
					DEBUG_PRINTF("cl_ReaderRemoveFromList: SemaphoreDestroy: pSgl4Write OK");
				}

				status = pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4Read );
				if ( status != CL_OK )
				{
					DEBUG_PRINTF("cl_ReaderRemoveFromList: SemaphoreDestroy: pSgl4Read ERROR");
				}
				else
				{
					DEBUG_PRINTF("cl_ReaderRemoveFromList: SemaphoreDestroy: pSgl4Read OK");
				}

				status = pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4ReadComplete );
				if ( status != CL_OK )
				{
					DEBUG_PRINTF("cl_ReaderRemoveFromList: SemaphoreDestroy: pSgl4ReadComplete ERROR");
				}
				else
				{
					DEBUG_PRINTF("cl_ReaderRemoveFromList: SemaphoreDestroy: pSgl4ReadComplete OK");
				}

				status = pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4WriteComplete );
				if ( status != CL_OK )
				{
					DEBUG_PRINTF("cl_ReaderRemoveFromList: SemaphoreDestroy: pSgl4WriteComplete ERROR");
				}
				else
				{
					DEBUG_PRINTF("cl_ReaderRemoveFromList: SemaphoreDestroy: pSgl4WriteComplete OK");
				}

				DEBUG_PRINTF("cl_ReaderRemoveFromList: Destroy_Semaphore: END");
			}

            //
            // Close Communications
			//
            // If I remove Comm first a thread still running will crash while attempting to Comm
			// If I destroy threads first a Comm still running will crash
			// The only way is to wait for the end of thread execution
			//
			if ( pCrtReader->eState == STATE_CONNECT )
			{
				pCrtReader->tReaderHalFuncs.fnIOCloseConnection( pCrtReader );
				DEBUG_PRINTF("cl_ReaderRemoveFromList: Close_Connection: %s", pCrtReader->tCOMParams.aucPortName );
			}
			else
			{
				DEBUG_PRINTF("cl_ReaderRemoveFromList: Reader: %s NOT_CONNECTED", pCrtReader->tCOMParams.aucPortName );
			}

			// Get Head of readers' list
			if ( CL_FAILED( status = cl_GetReaderListEntry( &pTempReader ) ) )
				return ( CL_MEM_ERR );

			if ( pTempReader == pCrtReader )
				cl_SetReaderListEntry( pPrvReader );

			// free allocated memory of Current Reader
			pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&pCrtReader );

			// exit
			return ( CL_OK );
		}
		else
		{
			// Previous reader is current
			pPrvReader = pCrtReader;

			// current is next
			pCrtReader = (t_Reader *)pCrtReader->pNext;
		}
	}

	return ( CL_ERROR );
}

DWORD WaitForThreadsToExit( t_Reader *pReader, int msTimeout )
{
    HANDLE hThreads[2];
    DWORD  dwRes;

    hThreads[0] = pReader->tSync.tThreadId4Read;

    DEBUG_PRINTF("WaitForThreadsToExit: %s", pReader->tCOMParams.aucPortName );

	SetEvent( pReader->tCOMParams.handleThreadExitEvent );

	dwRes = WaitForMultipleObjects(1, hThreads, TRUE, msTimeout);
	switch( dwRes )
	{
		case WAIT_OBJECT_0:
		case WAIT_OBJECT_0 + 1:

			dwRes = WAIT_OBJECT_0;

			break;

		case WAIT_TIMEOUT:

			if ( WaitForSingleObject( pReader->tSync.tThreadId4Read, 0) == WAIT_TIMEOUT )
			{
				DEBUG_PRINTF("WaitForThreadsToExit: ReadThread didn't exit: ERROR");
				OutputDebugString("Reader/Status Thread didn't exit.\n\r");
			}

			if ( WaitForSingleObject( pReader->tSync.tThreadId4Write, 0) == WAIT_TIMEOUT)
			{
				DEBUG_PRINTF("WaitForThreadsToExit: WriteThread didn't exit: ERROR");
				OutputDebugString("Writer Thread didn't exit.\n\r");
			}

			break;

		default:
			DEBUG_PRINTF("WaitForThreadsToExit: WaitForMultipleObjects: ERROR");
			break;
	}

    //
    // reset thread exit event here
    //
    ResetEvent( pReader->tCOMParams.handleThreadExitEvent );

    return dwRes;
}

/*--------------------------------------------------------------------------*/

e_Result cl_ReaderFreeListOfTuples( t_Tuple **pTuple )
{
	t_clContext *pCtxt 		= CL_NULL;
	e_Result status 		= CL_ERROR;

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return CL_ERROR;

	t_Tuple	*pTuple2ReadIncoming = *pTuple;
	while ( pTuple2ReadIncoming != NULL )
	{
		t_Tuple *pTupleNext = (t_Tuple *)pTuple2ReadIncoming->pNext;
		pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2ReadIncoming->ptBuf->pData );
		pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2ReadIncoming->ptBuf );
		pCtxt->ptHalFuncs->fnFreeMemSafely( (clvoid **)&pTuple2ReadIncoming );
		pTuple2ReadIncoming = pTupleNext;
	}
	*pTuple = CL_NULL;

	return status;
}

/**************************************************************************/
/* Name : e_Result cl_ReaderRemoveFromList( t_Reader *pReader)             */
/* Description :                                                         	 */
/*        Remove a reader from the list										*/
/**************************************************************************/
/* Parameters:                                                            	*/
/*  --------------                                                        	*/
/*  In: 	t_Reader *pReader: Remove the reader from the list. NULL not 	*/
/*			allowed															*/
/* ---------------                                                        	*/
/*  Out: 																	 */
/* Return value: e_Result                                                 	*/
/*  CL_OK                        :  Result is OK                             */
/*  CL_ERROR,                    : Failure on execution or list of readers 	*/
/*												is empty                      */
/*  CL_MEM_ERR,                  :  Failure on memory management (failure,   */
/*                                  allocation ....)                      	*/
/*  CL_PARAMS_ERR,               :  Inconsistent parameters                  */
/**************************************************************************/
e_Result OLD_THING_THAT_STINKS_cl_ReaderRemoveFromList( t_Reader *pReader )
{
	// variables definition
	t_clContext *pCtxt 		= CL_NULL;
	t_Reader *pCrtReader	= CL_NULL;
	t_Reader *pPrvReader	= CL_NULL;
	t_Reader *pTempReader	= CL_NULL;
	t_Tuple	*pCrtTuple		= CL_NULL;
	t_Tuple	*pNextTuple		= CL_NULL;
	e_Result status 		= CL_ERROR;

	// check parameters
	if ( pReader == CL_NULL )
		return ( CL_PARAMS_ERR );

	// get Head of readers' list
	if ( CL_FAILED( status = cl_GetReaderListEntry( &pCrtReader ) ) )
		return ( CL_MEM_ERR );

	// if list of readers is NULL, we cannot find a reader that satisfies the conditions=> exit on CL_ERROR
	if ( pCrtReader == CL_NULL )
		return ( CL_ERROR );

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

// BRY_28092015 encore des conneries � FD !!!!!
//	if ( pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )
//		return ( CL_MEM_ERR );

	//
	// Parse Readers list
	//
	while ( pCrtReader != CL_NULL )
	{
		if ( pCrtReader == pReader ) // Current Reader is the one to remove from the list
		{
			//
			// Previous reader will point to next element of the list
			// if we remove the first element of the list, then ...
			// start the list of readers with the second element of the list
			//
            if ( !pPrvReader )
                pPrvReader = pCrtReader->pNext;
            else // if we remove a subsequent reader, then update the linking pointers between each
            	pPrvReader->pNext = pCrtReader->pNext;

            DEBUG_PRINTF("cl_ReaderRemoveFromList: Reader: %s", pCrtReader->aucLabel);

            if ( WaitForThreadsToExit( pCrtReader, 1000 ) != WAIT_OBJECT_0 ) // 20000
            {
            	DEBUG_PRINTF( "cl_ReaderRemoveFromList: WaitForThreadsToExit: ERROR" );
            }

            if ( pCtxt->ptHalFuncs->fnDestroyThread != CL_NULL )
            {
//            	if ( pCrtReader->tType == IP_READER_TYPE )
//            	{
//            		DEBUG_PRINTF("Reader IP %s destroy thread %d\n", pCrtReader->tIPParams.aucIpAddr, pCrtReader->tSync.tThreadId4Read );
//            	}

            	// Destroy mutex (if reader is not connected mutex is not used)
				// BRY_: Wait for the end of Lock before suppress the mutex
            	// SESSION_MODE_CANCELED
//				DEBUG_PRINTF("cl_ReaderRemoveFromList: Mutex_Destroy: lock");
//				pthread_mutex_lock( &pCrtReader->tSync.mutexReadIncomingWR ); // necessar in SessionMode
//				pthread_mutex_unlock( &pCrtReader->tSync.mutexReadIncomingWR ); // necessar in SessionMode
//				pCtxt->ptHalFuncs->fnMutexDestroy( &pCrtReader->tSync.mutexReadIncomingWR );
//				DEBUG_PRINTF("cl_ReaderRemoveFromList: Mutex_Destroy: ok");

				//
				// Destroy Reader's threads
				//
//            	DEBUG_PRINTF("cl_ReaderRemoveFromList: Destroy_Thread");
//            	if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnDestroyThread( pCrtReader->tSync.tThreadId4Read ) ) )
//            	{
//            		DEBUG_PRINTF("cl_ReaderRemoveFromList: tThreadId4Read: FAILED");
//            	}
//
            	if ( CL_FAILED( status = pCtxt->ptHalFuncs->fnDestroyThread( pCrtReader->tSync.tThreadId4Write ) ) )
            	{
            		DEBUG_PRINTF("cl_ReaderRemoveFromList: tThreadId4Write: FAILED");
            	}
            }

            // BRY_29092015 l� aussi j'ai envie d'attendre que les threads soient stopp�es ...
//            pCtxt->ptHalFuncs->fnWaitMs( 150 );

			// Destroy signals
			if ( pCtxt->ptHalFuncs->fnSemaphoreDestroy != CL_NULL)
			{
				DEBUG_PRINTF("cl_ReaderRemoveFromList: Destroy_Semaphore: BEGIN");
				status = pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4Write );
				if ( status != CL_OK )
				{
					DEBUG_PRINTF("cl_ReaderRemoveFromList: SemaphoreDestroy: pSgl4Write ERROR");
				}
				else
				{
					DEBUG_PRINTF("cl_ReaderRemoveFromList: SemaphoreDestroy: pSgl4Write OK");
				}

				status = pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4Read );
				if ( status != CL_OK )
				{
					DEBUG_PRINTF("cl_ReaderRemoveFromList: SemaphoreDestroy: pSgl4Read ERROR");
				}
				else
				{
					DEBUG_PRINTF("cl_ReaderRemoveFromList: SemaphoreDestroy: pSgl4Read OK");
				}

				status = pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4ReadComplete );
				if ( status != CL_OK )
				{
					DEBUG_PRINTF("cl_ReaderRemoveFromList: SemaphoreDestroy: pSgl4ReadComplete ERROR");
				}
				else
				{
					DEBUG_PRINTF("cl_ReaderRemoveFromList: SemaphoreDestroy: pSgl4ReadComplete OK");
				}

				status = pCtxt->ptHalFuncs->fnSemaphoreDestroy( pCrtReader->tSync.pSgl4WriteComplete );
				if ( status != CL_OK )
				{
					DEBUG_PRINTF("cl_ReaderRemoveFromList: SemaphoreDestroy: pSgl4WriteComplete ERROR");
				}
				else
				{
					DEBUG_PRINTF("cl_ReaderRemoveFromList: SemaphoreDestroy: pSgl4WriteComplete OK");
				}

				DEBUG_PRINTF("cl_ReaderRemoveFromList: Destroy_Semaphore: END");
			}

            //
            // Close Communications
			//
            // If I remove Comm first a thread still running will crash while attempting to Comm
			// If I destroy threads first a Comm still running will crash
			// The only way is to wait for the end of thread execution
			//
			if ( pCrtReader->eState == STATE_CONNECT )
			{
				pCrtReader->tReaderHalFuncs.fnIOCloseConnection( pCrtReader );
				DEBUG_PRINTF("cl_ReaderRemoveFromList: Close_Connection: %s", pCrtReader->tCOMParams.aucPortName );
			}
			else
			{
				DEBUG_PRINTF("cl_ReaderRemoveFromList: Reader: %s NOT_CONNECTED", pCrtReader->tCOMParams.aucPortName );
			}

            // Look at the Fool FD stuffs :
//            // force a close connection if not done already
//			DEBUG_PRINTF("cl_ReaderRemoveFromList: Close_Connection");
//            pCrtReader->tReaderHalFuncs.fnIOCloseConnection( pCrtReader );

//            // Free any tuple in SEND queue for this reader
//            pCrtTuple = pCrtReader->p_TplList2Send;
//            if ( pCrtTuple )
//            	pNextTuple = pCrtTuple->pNext;
//
//            DEBUG_PRINTF("cl_ReaderRemoveFromList: fnFreeMem p_TplList2Send");
//            while ( pCrtTuple )
//            {
//                if ( pCrtTuple )
//                	pNextTuple = pCrtTuple->pNext;
//
//                if ( pCrtTuple->ptBuf)
//                {
//                	DEBUG_PRINTF("cl_ReaderRemoveFromList: len [%d]", pCrtTuple->ptBuf->ulLen);
//
//                	if ( pCrtTuple->ptBuf->pData )
//                		pCtxt->ptHalFuncs->fnFreeMem( pCrtTuple->ptBuf->pData );
//
//                	pCtxt->ptHalFuncs->fnFreeMem( pCrtTuple->ptBuf );
//                }
//                pCtxt->ptHalFuncs->fnFreeMem( pCrtTuple );
//                pCrtTuple = pNextTuple;
//            }
//            // set correct indicator
//            pCrtReader->p_TplList2Send = CL_NULL;

//            // Free any tuple in READ queue for this reader
//            pCrtTuple = pCrtReader->p_TplList2Read;
//            if ( pCrtTuple )
//            	pNextTuple = pCrtTuple->pNext;
//
//            DEBUG_PRINTF("cl_ReaderRemoveFromList: fnFreeMem p_TplList2Read");
//            while ( pCrtTuple )
//            {
//                if ( pCrtTuple )
//                	pNextTuple = pCrtTuple->pNext;
//
//                if ( pCrtTuple->ptBuf)
//                {
//                	DEBUG_PRINTF("cl_ReaderRemoveFromList: len [%d]", pCrtTuple->ptBuf->ulLen);
//
//                	if ( pCrtTuple->ptBuf->pData )
//                		pCtxt->ptHalFuncs->fnFreeMem( pCrtTuple->ptBuf->pData );
//
//                	pCtxt->ptHalFuncs->fnFreeMem( pCrtTuple->ptBuf );
//                }
//                pCtxt->ptHalFuncs->fnFreeMem( pCrtTuple );
//                pCrtTuple = pNextTuple;
//            }
//            // set correct indicator
//            pCrtReader->p_TplList2Read = CL_NULL;

			//**************
			// if we removed first reader of the list, displace first element to second one
			//**************
			// get Head of readers' list
			if ( CL_FAILED( status = cl_GetReaderListEntry( &pTempReader ) ) )
				return ( CL_MEM_ERR );

			if ( pTempReader == pCrtReader )
				cl_SetReaderListEntry( pPrvReader );

			// free allocated memory of Current Reader
			pCtxt->ptHalFuncs->fnFreeMemSafely( (void **)&pCrtReader );

			// exit
			return ( CL_OK );
		}
		else
		{
			// Previous reader is current
			pPrvReader = pCrtReader;

			// current is next
			pCrtReader = (t_Reader *)pCrtReader->pNext;
		}
	}

	return ( CL_ERROR );
}

/*****************************************************************************/
/* Name : e_Result cl_ReaderRegistrationThread( t_Reader *ppReader)           */
/* Description :                                                         	 */
/*        thread which handles registration/unregistration of readers in CSL */
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
e_Result cl_ReaderRegistrationThread( void)
{
	t_clContext 	*pCtxt 					= CL_NULL;
	e_Result 		status 					= CL_ERROR;
	t_ReaderRange 	*pRange					= CL_NULL;
	clu64			i64Range				= 0;
	t_Reader 		tLocalReader;
	t_Reader 		*pReader2Register 		= CL_NULL;
	t_Reader 		*pReaderAlreadyInList 	= CL_NULL;
    clu64 			u64StartRange			= 0;
    clu64			u64StopRange 			= 0;
	clu8			u8ReaderIndex			= 0;
	t_Reader		*pReaderInList			= CL_NULL;
	clu8			u8SearchDone			= 0;
	//1 6 3 @
    cl8				ac8COMDiscover[]		= { 0x01, 0x06, 0x02 };
	cl8				ac8IPDiscover[]			= { 0x00, 0x00, 0x00, 0xF8 };
    clu8            *pDiscover              = CL_NULL;
    clu32			u32COMDiscLen			=	0;
    clu32			u32IPDiscLen			=	0;
	t_Tuple			tTuple4Discover;
	clu32 			u32TsfNb				=	0;
	cl8				aucTempName[16];

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL)
		return ( CL_ERROR );

	if ( pCtxt->ptCallbacks == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs == CL_NULL )
		return ( CL_ERROR );

	// check semaphore was initiated for registration thread
	if ( pCtxt->ptHalFuncs->fnSemaphoreWait == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnSemaphoreRelease == CL_NULL )
		return ( CL_ERROR );

	if (pCtxt->ptHalFuncs->fnWaitMs == CL_NULL )
		return ( CL_ERROR );

// Bry_28092015 encore les conneries de FD !!!
//	if (pCtxt->ptHalFuncs->fnFreeMem == CL_NULL )
//		return ( CL_ERROR );

	if ( pCtxt->ptHalFuncs->fnAllocMem == CL_NULL )
		return ( CL_ERROR );

	if ( pCtxt->ptCslReader == CL_NULL )
		return ( CL_ERROR );

	u32COMDiscLen = sizeof( ac8COMDiscover );
	u32IPDiscLen = sizeof( ac8IPDiscover );

	// helper to initialize tuples with default fields for subsequent transfers
	cl_initTupleSetDefaultFields( TSP_LAYER_SET_LOCAL, NONE, 0, 0, 0, CL_NULL  );

	for (;;)
	{
		DEBUG_PRINTF("ReaderRegistrationThread: BEGIN");

        if ( (( pCtxt->eStackSupport & COM_STACK_SUPPORT )!=0) & ( pCtxt->p_COMReaderDiscoverHal != CL_NULL ) ) // if search was not done and Discover is enabled
		{
			// get range
			u64StartRange 	= pCtxt->ptCslReader->tCOMParams.tRange.u64startRange;
			u64StopRange 	= pCtxt->ptCslReader->tCOMParams.tRange.u64stopRange;

			if ( u8SearchDone == 0)
			{
				for ( i64Range = u64StartRange; i64Range < u64StopRange; i64Range++)
				{
					if ( CL_FAILED( cl_ReaderFillWithDefaultFields( &tLocalReader, COM_READER_TYPE ) ) )
						break;

					// initialize the port name with "COM"
					strcpy( (cl8*)tLocalReader.tCOMParams.aucPortName, "COM" );
					itoa( (clu32)i64Range, &tLocalReader.tCOMParams.aucPortName[3], 10 );
					tLocalReader.tCallBacksDiscover.fnIOData2Read_cb = cl_ClbkDiscover_ReadDone;	// specify the inner callback used to get data from discover
					if ( CL_FAILED( cl_readerAddToList( &tLocalReader ) ) )
						break;


					// get reader in the list
					if ( CL_FAILED( cl_readerFindInList( &pReaderInList, &tLocalReader ) ) )
						break;

					if ( pReaderInList )
					{
						// if already in DISCOVER.... do not reask to reopen port and so on
						if ( pReaderInList->eState != STATE_DISCOVER )
						{
							// set state for reader. If an error happens while trying to connect, we check the status afterwards
							// so not trying return code at this point is ok
							cl_ReaderSetState( pReaderInList, STATE_DISCOVER );
						}
						// at this stage, the reader shall be in DISCOVER mode if present. Otherwise, remove it from the list
						if ( pReaderInList->eState != STATE_DISCOVER )
						{
							if ( CL_FAILED( cl_ReaderRemoveFromList( pReaderInList ) ) )
								break;
						}
						else
						{
							// points to default frame for Clover discover
							pDiscover = &ac8COMDiscover[0];
						    // initialize a tuple default flags with memory
                            if ( CL_FAILED( cl_initTuple( &tTuple4Discover, CL_NULL, &pDiscover, u32COMDiscLen ) ) )
						    	cl_ReaderRemoveFromList( pReaderInList );
						    else
						    {
								// now a COM port is detected : check if it is a valid reader
								if ( CL_FAILED( cl_sendData( CL_NULL, pReaderInList, &tTuple4Discover, NON_BLOCKING, &u32TsfNb ) ) )
								{
									cl_ReaderRemoveFromList( pReaderInList );
								}
								else
								{
									DEBUG_PRINTF("Reader %s discovered\n", pReaderInList->tCOMParams.aucPortName );
									// wait 1s before checking a new reader popped in
									if ( pCtxt->ptHalFuncs->fnWaitMs != CL_NULL )
							            pCtxt->ptHalFuncs->fnWaitMs( 5000 );

								}

						    }

						}
					}
					else
					{
						DEBUG_PRINTF("Reader registration thread issue %d!!!!!!", i64Range);
						break;
					}
				}
				u8SearchDone = 1;
			}
		}

        if ( (( pCtxt->eStackSupport & IP_STACK_SUPPORT )!= 0) & ( pCtxt->p_IPReaderDiscoverHal != CL_NULL ))	// if Discover is enabled
		{

			// get range
			u64StartRange 	= pCtxt->ptCslReader->tIPParams.tRange.u64startRange;
			u64StopRange 	= pCtxt->ptCslReader->tIPParams.tRange.u64stopRange;
			u8SearchDone = 0;
			if ( u8SearchDone == 0)
			{

				if ( CL_FAILED( cl_ReaderFillWithDefaultFields( &tLocalReader, IP_READER_TYPE ) ) )
						break;

				// prepare to send an UDP frame to the network to
				i64Range = 0xC0A800FF;


                memset( &tLocalReader.tIPParams.aucIpAddr[0], 0, sizeof( tLocalReader.tIPParams.aucIpAddr ));
                memset( aucTempName, 0, sizeof( aucTempName) );
                itoa( (clu32)((i64Range&0xFF000000)>>24), aucTempName, 10 );
                strcat( tLocalReader.tIPParams.aucIpAddr, aucTempName );
                strcat( tLocalReader.tIPParams.aucIpAddr, "." );
                memset( aucTempName, 0, sizeof( aucTempName) );
                itoa( (clu32)((i64Range&0x00FF0000)>>16), aucTempName, 10 );
                strcat( tLocalReader.tIPParams.aucIpAddr, aucTempName );
                strcat( tLocalReader.tIPParams.aucIpAddr, "." );
                memset( aucTempName, 0, sizeof( aucTempName) );
                itoa( (clu32)((i64Range&0x0000FF00)>>8), aucTempName, 10 );
                strcat( tLocalReader.tIPParams.aucIpAddr, aucTempName );
                strcat( tLocalReader.tIPParams.aucIpAddr, "." );
                memset( aucTempName, 0, sizeof( aucTempName) );
                itoa( (clu32)((i64Range&0x000000FF)), aucTempName, 10 );
                strcat( tLocalReader.tIPParams.aucIpAddr, aucTempName );

                memset( tLocalReader.aucLabel, 0, sizeof( tLocalReader.aucLabel) );
                strcpy( (cl8*)tLocalReader.aucLabel, "IP_Reader" );
                strcat( tLocalReader.aucLabel, tLocalReader.tIPParams.aucIpAddr);

                tLocalReader.tIPParams.u32Port = 30718;
                tLocalReader.tCallBacksDiscover.fnIOData2Read_cb = cl_ClbkDiscover_ReadDone;	// specify the inner callback used to get data from discover

                if ( CL_FAILED( cl_readerAddToList( &tLocalReader ) ) )
					break;

				// get reader in the list
				if ( CL_FAILED( cl_readerFindInList( &pReaderInList, &tLocalReader ) ) )
					break;

				if ( pReaderInList )
				{
					// if already in DISCOVER.... do not reask to reopen port and so on
					if ( pReaderInList->eState != STATE_DISCOVER )
					{
						// set state for reader. If an error happens while trying to connect, we check the status afterwards
						// so not trying return code at this point is ok
						cl_ReaderSetState( pReaderInList, STATE_DISCOVER );
					}
					// at this stage, the reader shall be in DISCOVER mode if present. Otherwise, remove it from the list
					if ( pReaderInList->eState != STATE_DISCOVER )
					{
						if ( CL_FAILED( cl_ReaderRemoveFromList( pReaderInList ) ) )
							break;
					}
					else
					{
						// point to LANTRONIX frame
				    	pDiscover = &ac8IPDiscover[0];
					    // initialize a tuple default flags with memory
                        if ( CL_FAILED( cl_initTuple( &tTuple4Discover, CL_NULL, &pDiscover, u32IPDiscLen ) ) )
					    	cl_ReaderRemoveFromList( pReaderInList );
					    else
					    {
							// now broadcast on UDP and wait for a while to get all devices pushing their data to us
							if ( CL_FAILED( cl_sendData( CL_NULL, pReaderInList, &tTuple4Discover, NON_BLOCKING, &u32TsfNb ) ) )
							{
								cl_ReaderRemoveFromList( pReaderInList );
							}
							else
							{
								DEBUG_PRINTF("Fake IP reader polling  %s connected\n", pReaderInList->aucLabel );
							}
					    }

					}
				}
				else
				{
					DEBUG_PRINTF("Reader registration thread issue %d!!!!!!", i64Range);
					break;
				}
			}



/*				for ( i64Range = u64StartRange; i64Range < u64StopRange; i64Range++)
				{
					if ( CL_FAILED( cl_ReaderFillWithDefaultFields( &tLocalReader, IP_READER_TYPE ) ) )
						break;

					// initialize the port name with "IP_Reader"
//					strcpy( (cl8*)tLocalReader.tCOMParams.aucPortName, "IP" );
//					itoa( (clu32)i64Range, &tLocalReader.tCOMParams.aucPortName[3], 10 );
                    memset( &tLocalReader.tIPParams.aucIpAddr[0], 0, sizeof( tLocalReader.tIPParams.aucIpAddr ));
                    memset( aucTempName, 0, sizeof( aucTempName) );
                    itoa( (clu32)((i64Range&0xFF000000)>>24), aucTempName, 10 );
                    strcat( tLocalReader.tIPParams.aucIpAddr, aucTempName );
                    strcat( tLocalReader.tIPParams.aucIpAddr, "." );
                    memset( aucTempName, 0, sizeof( aucTempName) );
                    itoa( (clu32)((i64Range&0x00FF0000)>>16), aucTempName, 10 );
                    strcat( tLocalReader.tIPParams.aucIpAddr, aucTempName );
                    strcat( tLocalReader.tIPParams.aucIpAddr, "." );
                    memset( aucTempName, 0, sizeof( aucTempName) );
                    itoa( (clu32)((i64Range&0x0000FF00)>>8), aucTempName, 10 );
                    strcat( tLocalReader.tIPParams.aucIpAddr, aucTempName );
                    strcat( tLocalReader.tIPParams.aucIpAddr, "." );
                    memset( aucTempName, 0, sizeof( aucTempName) );
                    itoa( (clu32)((i64Range&0x000000FF)), aucTempName, 10 );
                    strcat( tLocalReader.tIPParams.aucIpAddr, aucTempName );

                    memset( tLocalReader.aucLabel, 0, sizeof( tLocalReader.aucLabel) );
                    strcpy( (cl8*)tLocalReader.aucLabel, "IP_Reader" );
                    strcat( tLocalReader.aucLabel, tLocalReader.tIPParams.aucIpAddr);

                    if ( CL_FAILED( cl_readerAddToList( &tLocalReader ) ) )
						break;

                    // set state for reader. If an error happens while trying to connect, we check the status afterwards
                    // so not trying return code at this point is ok
                    cl_ReaderSetState( &tLocalReader, STATE_CONNECT );

					// get reader in the list
					if ( CL_FAILED( cl_readerFindInList( &pReaderInList, &tLocalReader ) ) )
						break;

					if ( pReaderInList )
					{
						if ( pReaderInList->eState != STATE_CONNECT )
						{
							if ( CL_FAILED( cl_ReaderRemoveFromList( pReaderInList ) ) )
								break;
						}
						else
						{
						    // initialize a tuple default flags with memory
                            if ( CL_FAILED( cl_initTuple( &tTuple4Discover, CL_NULL, &pDiscover, sizeof( ac8Discover ) ) ) )
						    	cl_ReaderRemoveFromList( pReaderInList );
						    else
						    {
								// now a COM port is detected : check if it is a valid reader
								if ( CL_FAILED( cl_sendData( CL_NULL, pReaderInList, &tTuple4Discover, NON_BLOCKING, &u32TsfNb ) ) )
								{
									cl_ReaderRemoveFromList( pReaderInList );
								}
								else
								{
									DEBUG_PRINTF("Reader %s connected\n", pReaderInList->tCOMParams.aucPortName );
								}
						    }

						}
					}
					else
					{
						DEBUG_PRINTF("Reader registration thread issue %d!!!!!!", i64Range);
						break;
					}
				}
				u8SearchDone = 1;
			}
*/
		}

        DEBUG_PRINTF("ReaderRegistrationThread: WAIT");

		// wait 1s before checking a new reader popped in
		if ( pCtxt->ptHalFuncs->fnWaitMs != CL_NULL )
            pCtxt->ptHalFuncs->fnWaitMs( 3600*1000 );

	} // for (;;)

	// default exit on error
	if ( status == CL_ERROR )
	{
		DEBUG_PRINTF("ReaderRegistrationThread:CL_ERROR");
		return ( CL_ERROR );
	}

	DEBUG_PRINTF("ReaderRegistrationThread:CL_OK");
	return ( CL_OK );
}

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
e_Result cl_getReader_List( t_Reader **pptReaderList)
{
	e_Result status = CL_ERROR;

	status = cl_GetReaderListEntry( pptReaderList );

	return ( status );
}

/**************************************************************************/
/* Name : e_Result cl_ReaderGetSerialEncapsLen( t_Reader *pReader);       */
/* Description :                                                          */
/*        this API return the length of the encapsulation to add to each buffer */
/*		  coming from userland before sending it on the network to a reader*/
/*		  This reader can get a trailer to add to the buffer due to its   */
/*		  specificity/setup												  */
/**************************************************************************/
/* Parameters:                                                            */
/*  --------------                                                        */
/*  In: t_Reader *pReader: chained list of readers in the system 	  */
/*                           	can be NULL                               */
/* ---------------                                                        */
/*  Out: none                                                             */
/* Return value: Length of the trailer. NULL if reader type is unknown	  */
/**************************************************************************/
clu32 cl_ReaderGetSerialEncapsLen( t_Reader *pReader )
{
	clu32 u32Len;

	/* check incoming parameter */
	if ( pReader == CL_NULL )
		return ( 0 );

	/* check reader type */
	switch ( pReader->tType )
	{
		case (IP_READER_TYPE):
		case ( LANTRONIX_READER_TYPE ):
		{
			u32Len = IP_READER_SERIAL_PROTOCOL_LEN;
			break;
		}
		case ( COM_READER_TYPE ):
		{
			u32Len = READER_SERIAL_PROTOCOL_LEN;
			break;
		}
		case ( UNKNOWN_READER_TYPE ):
		default:
			u32Len = 0;
	}

	/* returned value */
	return ( u32Len );
}

/**************************************************************************/
/* Name : e_Result cl_ReaderGetSerialEncapsLen( t_Reader *pReader);       */
/* Description :                                                          */
/*        this API return the length of the trailerto add to each buffer */
/*		  coming from userland before sending it on the network to a reader*/
/*		  This reader can get a trailer to add to the buffer due to its   */
/*		  specificity/setup												  */
/**************************************************************************/
/* Parameters:                                                            */
/*  --------------                                                        */
/*  In: t_Reader *pReader: chained list of readers in the system 	  */
/*                           	can be NULL                               */
/* ---------------                                                        */
/*  Out: none                                                             */
/* Return value: Length of the trailer. NULL if reader type is unknown	  */
/**************************************************************************/
clu32 cl_ReaderGetSerialEncapsTrailerLen( t_Reader *pReader )
{
	clu32 u32Len;

	/* check incoming parameter */
	if ( pReader == CL_NULL )
		return ( 0 );

	/* check reader type */
	switch ( pReader->tType )
	{
		case (IP_READER_TYPE):
		case ( LANTRONIX_READER_TYPE ):
		{
			u32Len = IP_READER_SERIAL_PROTOCOL_TRAILER_LEN;
			break;
		}
		case ( COM_READER_TYPE ):
		{
			u32Len = READER_SERIAL_PROTOCOL_TRAILER_LEN;
			break;
		}
		case ( UNKNOWN_READER_TYPE ):
		default:
			u32Len = 0;
	}

	/* returned value */
	return ( u32Len );
}

/**************************************************************************/
/* Name : e_Result cl_ReaderGetSerialEncapsFinishLen( t_Reader *pReader);       */
/* Description :                                                          */
/*        this API return the length of the finish datato add to each buffer */
/*		  coming from userland before sending it on the network to a reader*/
/*		  This reader can get a trailer to add to the buffer due to its   */
/*		  specificity/setup												  */
/**************************************************************************/
/* Parameters:                                                            */
/*  --------------                                                        */
/*  In: t_Reader *pReader: chained list of readers in the system 	  */
/*                           	can be NULL                               */
/* ---------------                                                        */
/*  Out: none                                                             */
/* Return value: Length of the trailer. NULL if reader type is unknown	  */
/**************************************************************************/
clu32 cl_ReaderGetSerialEncapsFinishLen( t_Reader *pReader )
{
	clu32 u32Len;

	/* check incoming parameter */
	if ( pReader == CL_NULL )
		return ( 0 );

	/* check reader type */
	switch ( pReader->tType )
	{
		case ( IP_READER_TYPE ):
		case ( LANTRONIX_READER_TYPE ):
		{
			u32Len = IP_READER_SERIAL_PROTOCOL_FINISH_LEN;
			break;
		}
		case ( COM_READER_TYPE ):
		{
			u32Len = READER_SERIAL_PROTOCOL_FINISH_LEN;
			break;
		}
		case ( UNKNOWN_READER_TYPE ):
		default:
			u32Len = 0;
	}

	/* returned value */
	return ( u32Len );
}


/**************************************************************************/
/* Name : e_Result cl_ReaderSetLastAccessed( t_Reader *pReader )		 */
/* Description :                                                          */
/*        this API set the pointer of the reader which is	 accessed    */
/*		  It can be used for scripting as an example to be able to chain */
/*		  on a reader, several commands, without respecifying it		*/
/**************************************************************************/
/* Parameters:                                                            */
/*  --------------                                                        */
/*  In: t_Reader *pReader: pointer on the reader						  */
/*                           	can be NULL                               */
/* ---------------                                                        */
/*  Out: none                                                             */
/* Return value: CL_OK  												  */
/**************************************************************************/
e_Result cl_ReaderSetLastAccessed( t_Reader *pReader )
{
	e_Result status = CL_OK;

	g_ptLastAccessedReader = pReader;

	return ( status );
}

/****************************************************************************/
/* Name : e_Result cl_ReaderSetLastAccessed( t_Reader **ppReader )		 	*/
/* Description :                                                          	*/
/*        this API set the pointer of the reader which is	 accessed    	*/
/*		  It can be used for scripting as an example to be able to chain 	*/
/*		  on a reader, several commands, without respecifying it			*/
/**************************************************************************/
/* Parameters:                                                            	*/
/*  --------------                                                        	*/
/*  In/Out: t_Reader **ppReader: pointer on the reader last accessed	  	*/
/*                           	cannot be NULL                              */
/* ---------------                                                        	*/
/*  Out: none                                                             	*/
/* Return value: CL_OK  												  	*/
/*				CL_ERROR: Wrong parameter									*/
/**************************************************************************/
e_Result cl_ReaderGetLastAccessed( t_Reader **ppReader )
{
	e_Result status = CL_ERROR;

	if ( ppReader == CL_NULL )
		return ( status );

	// get last reader accessed
	*ppReader = g_ptLastAccessedReader;

	status = CL_OK;

	return ( status );
}


//*******************************************************************************************/
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
e_Result cl_ReaderSetState( t_Reader *pReader, e_State eState )
{
	t_clContext *pCtxt 			= CL_NULL;
	e_Result status 			= CL_ERROR;
	t_Reader *pReaderFromList 	= CL_NULL;
	t_Reader tLocalReader;

	// check incoming parameters
	if ( pReader == CL_NULL )
		return ( CL_ERROR );

	if ( (eState != STATE_DEFAULT) & (eState != STATE_INIT) & (eState != STATE_CONNECT) & (eState != STATE_DISCONNECT) & (eState != STATE_ERROR) & (eState != STATE_OK) & (eState != STATE_DISCOVER) & (eState != STATE_OTA))
		return ( CL_ERROR );

	// get context to have access to function pointers for memory/thread managment on platform
	if ( CL_FAILED(status =  cl_GetContext( &pCtxt )) )
		return ( CL_ERROR );

	if ( pCtxt == CL_NULL )
		return ( CL_ERROR );

	// test if the reader is populated in the list
	if ( CL_FAILED( cl_readerFindInList( &pReaderFromList ,pReader ) ) )
	{
		DEBUG_PRINTF("cl_ReaderSetState: cl_readerFindInList: FAILED");
		return CL_ERROR;
	}

	switch ( eState )
	{
		case ( STATE_DISCOVER ):
		{
			// set in DISCOVER mode for now, if failing, we will update the status accordingly
			if ( pReaderFromList->eState != STATE_DISCOVER)// if already discovered do not reenter this stage
			{
					if ( pReaderFromList->tReaderDiscoverHalFuncs.fnIOOpenConnection != CL_NULL )
						status = pReaderFromList->tReaderDiscoverHalFuncs.fnIOOpenConnection( (clvoid *)pReaderFromList);
			}
			else
				status = CL_OK;

			if ( CL_FAILED( status ) )
				pReaderFromList->eState = STATE_ERROR;
			else
				pReaderFromList->eState = STATE_DISCOVER;

			break;
		}

		case ( STATE_CONNECT ):
		{
			// if we were previoulsy in discover state => close all connections
			if ( pReaderFromList->eState == STATE_DISCOVER )
			{
				// copy existing reader to new reader
				memcpy( &tLocalReader, pReaderFromList, sizeof( t_Reader ) );

				// remove existing from list to enable safe and clean cancellation of low-level drivers
				if ( CL_FAILED( status = cl_ReaderRemoveFromList( pReaderFromList ) ) )
					break;

				// wait 1s before checking a new reader popped in
				if ( pCtxt->ptHalFuncs->fnWaitMs != CL_NULL )
					pCtxt->ptHalFuncs->fnWaitMs( 1000 );

				// add the saved reader to list
				if ( CL_FAILED( status = cl_readerAddToList( &tLocalReader ) ) )
					break;

				// test if the reader is populated in the list
				if ( CL_FAILED( status = cl_readerFindInList( &pReaderFromList, &tLocalReader ) ) )
					break;
			}

			if ( pReaderFromList->tReaderHalFuncs.fnIOOpenConnection != CL_NULL )
				status = pReaderFromList->tReaderHalFuncs.fnIOOpenConnection( (clvoid *)pReaderFromList);

			if ( CL_FAILED( status ) )
				pReaderFromList->eState = STATE_ERROR;
			else
				pReaderFromList->eState = STATE_CONNECT;

			break;
		}

		case ( STATE_DISCONNECT ):
		{
			pReaderFromList->eState = STATE_ERROR;

			if ( pReaderFromList->tReaderHalFuncs.fnIOCloseConnection != CL_NULL )
			{
				if ( CL_SUCCESS( status = pReader->tReaderHalFuncs.fnIOCloseConnection( (clvoid *)pReaderFromList) ) )
					pReaderFromList->eState = STATE_DISCONNECT;
			}
			break;
		}
		case ( STATE_OTA ):
		{
			pReaderFromList->eState = STATE_OTA;
			break;
		}
		case ( STATE_DEFAULT ):
		case ( STATE_INIT ):
		case ( STATE_ERROR ):
		default:
		{
			// set state
			pReaderFromList->eState = eState;
			break;
		}
	}

	if ( pReaderFromList->tCallBacks.fnIOState_cb != CL_NULL)
		pReaderFromList->tCallBacks.fnIOState_cb( CL_NULL, pReaderFromList, CL_NULL, status );
	else
	{
		if ( pCtxt->ptCallbacks->fnIOState_cb != CL_NULL )
			pCtxt->ptCallbacks->fnIOState_cb( CL_NULL, pReaderFromList, CL_NULL, status );
	}

	return ( status );
}

/*******************************************************************************************/
/* Name : e_Result cl_ReaderSetState(  e_ReaderState eState ) 								*/
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
e_Result cl_ReaderGetState( t_Reader *pReader, e_State *p_eState )
{
	if ( pReader == CL_NULL)
		return ( CL_ERROR );

	if ( p_eState == CL_NULL)
		return ( CL_ERROR );

	*p_eState = pReader->eState;

	return ( CL_OK );
}

// BRY_FOR_DEBUGPURPOSE
void PRINT_DEBUG_STATUS(char *aHead, char* aHeader, e_SERIAL_RetryProtocolDef status);

/*******************************************************************************************/
/* Name : e_Result cl_ReaderSetSyncStatus(  t_Reader *pReader, e_SERIAL_RetryProtocolDef eSyncStatus ) 		*/
/* Description :                                                                            */
/*        set synchronization state between read/write threads								*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*			*pReader	:	pointer on reader												*/
/*			eSyncStatus	:	synchonization status											*/
/* ---------------                                                                          */
/*  Out: none                                                                               */
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  CL_ERROR                  :  Error on call    											*/
/*******************************************************************************************/
e_Result cl_ReaderSetSyncStatus(char *head, t_Reader *pReader, e_SERIAL_RetryProtocolDef eSyncStatus )
{
	if ( pReader == CL_NULL)
		return ( CL_ERROR );

	pReader->tSync.eStatus = eSyncStatus;

	PRINT_DEBUG_STATUS(head, "SET", eSyncStatus );

	return ( CL_OK );
}

/*******************************************************************************************/
/* Name : e_Result cl_ReaderGetSyncStatus(  t_Reader *pReader, e_SERIAL_RetryProtocolDef *p_eSyncStatus ) 		*/
/* Description :                                                                            */
/*        sget synchronization state between read/write threads								*/
/*******************************************************************************************/
/* Parameters:                                                                              */
/*  --------------                                                                          */
/*  In :                                                                               		*/
/*			*pReader	:	pointer on reader												*/
/* ---------------                                                                          */
/*  Out:                                                                                	*/
/*			*p_eSyncStatus	:	synchonization status										*/
/* Return value: e_Result                                                                   */
/*  OK                        :  Result is OK                                               */
/*  CL_ERROR                  :  Error on call    											*/
/*******************************************************************************************/
e_Result cl_ReaderGetSyncStatus(char *head, t_Reader *pReader, e_SERIAL_RetryProtocolDef *p_eSyncStatus )
{
	if ( pReader == CL_NULL)
		return ( CL_ERROR );

	if ( p_eSyncStatus == CL_NULL )
		return ( CL_ERROR );

	*p_eSyncStatus = pReader->tSync.eStatus;

	PRINT_DEBUG_STATUS(head, "GET", pReader->tSync.eStatus );

	return ( CL_OK );
}

/*--------------------------------------------------------------------------*/

void PRINT_DEBUG_STATUS(char *aHead, char* aHeader, e_SERIAL_RetryProtocolDef status)
{
#ifndef BRY_FOR_DEBUGPURPOSE

	switch ( status )
	{
		case ( CL_SERIAL_PROT_DEFAULT ):
		{
			DEBUG_PRINTF("%s_%s:CL_SERIAL_PROT_DEFAULT [etime:%d]", aHead, aHeader, cl_GetElapsedTime());	// by default, the Write/Read thread synchro is in this state
			break;
		}
//		case ( CL_SERIAL_PROT_R_INCOMING ):
//		{
//			DEBUG_PRINTF("%s_%s:CL_SERIAL_PROT_R_INCOMING [etime:%d]", aHead, aHeader, cl_GetElapsedTime());
//			break;
//		}
		case ( CL_SERIAL_PROT_W_WAIT_FOR_ACK ):
		{
			DEBUG_PRINTF("%s_%s:CL_SERIAL_PROT_W_WAIT_FOR_ACK [etime:%d]", aHead, aHeader, cl_GetElapsedTime());	// a write was issued by the write thread. Write is waiting for Read Thread to receive 0x06
			break;
		}
		case ( CL_SERIAL_PROT_R_RECEIVED_ACK ):
		{
			DEBUG_PRINTF("%s_%s:CL_SERIAL_PROT_R_RECEIVED_ACK [etime:%d]", aHead, aHeader, cl_GetElapsedTime());	// 0x06 was received from read thread. Depending on type of Command sent, either Wait for the answer or exit
			break;
		}
		case ( CL_SERIAL_PROT_R_RECEIVED_NACK ):
		{
			DEBUG_PRINTF("%s_%s:CL_SERIAL_PROT_R_RECEIVED_NACK [etime:%d]", aHead, aHeader, cl_GetElapsedTime());	// 0x15 was received from read thread. Reemit data to reader
			break;
		}
		case ( CL_SERIAL_PROT_W_WAIT_FOR_ANSW ):
		{
			DEBUG_PRINTF("%s_%s:CL_SERIAL_PROT_W_WAIT_FOR_ANSW [etime:%d]", aHead, aHeader, cl_GetElapsedTime());	// Wait for Answer from the reader (timeout..)
			break;
		}
		case ( CL_SERIAL_PROT_R_WRITE_ACK_REQUIRED ):	// a correct answer was received from the reader, Write thread Shall acknowledge it with 0x06
		{
			DEBUG_PRINTF("%s_%s:CL_SERIAL_PROT_R_WRITE_ACK_REQUIRED [etime:%d]", aHead, aHeader, cl_GetElapsedTime());
			break;
		}
		case ( CL_SERIAL_PROT_W_END_REQUIRED ):	// write thread requires that the read thread acknowledge this
		{
			DEBUG_PRINTF("%s_%s:CL_SERIAL_PROT_W_END_REQUIRED [etime:%d]", aHead, aHeader, cl_GetElapsedTime());
			break;
		}
		case ( CL_SERIAL_PROT_R_END_GRANTED ):	// acknowledge is done, we can process next tuple
		{
			DEBUG_PRINTF("%s_%s:CL_SERIAL_PROT_R_END_GRANTED [etime:%d]", aHead, aHeader, cl_GetElapsedTime());
			break;
		}
		case ( CL_SERIAL_PROT_W_END_DONE ):	// end performed
		{
			DEBUG_PRINTF("%s_%s:CL_SERIAL_PROT_W_END_DONE [etime:%d]", aHead, aHeader, cl_GetElapsedTime());
			break;
		}

		case ( CL_SERIAL_PROT_UNKNOWN ):
		default:
		{
			DEBUG_PRINTF("%s_%s:CL_SERIAL_PROT_UNKNOWN:0x%02x [etime:%d]", aHead, aHeader, status, cl_GetElapsedTime()); // no idea of what to do with that ....
			break;
		}
	}

	//cl_WaitMsWin32(10); // BRY Debug purpose

#endif
}
