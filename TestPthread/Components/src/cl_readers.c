/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: cl_readers.c                                                       */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/*  Description : APi to manage readers                                      */
/*****************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "csl.h"
#include "cl_readers.h"
#include "cltypes.h"

extern int GetDirectoryName(char* aFullExeName, char* aDirectoryWithoutExeName);

extern char FullExeFileNameWithDirectory[1024];
#define qint64 long long

/*-----------------------------------------------------------------------------------------*\
 * Get the reader that have the "friendlyName" in file readers_list.txt and initialize it
\*-----------------------------------------------------------------------------------------*/
e_Result cl_InitReaderFromFileByFriendlyName( char* friendlyName, t_Reader* aReader )
{
	FILE       *file_in;
	errno_t		err_no;
	t_clContext *pCtxt 			= CL_NULL;
	e_Result	status			= CL_ERROR;
	char ligne[STRING_LENGTH];
	char directory_name[STRING_LENGTH];
	char file_name[STRING_LENGTH];
	char file_on_disk[STRING_LENGTH] = "readers_list.txt";

    char buf[1024];
    e_ReaderType eType;
    //t_Reader tNewReader;
    //t_Reader *pReaderInList = CL_NULL;
    int iState = 0;
    int iFieldLen = 0;
    long long iFirstIndex ;
    qint64 iLastIndex ;
    qint64 iFoundIndex = 0;
    bool bFound = false;
    bool aReaderWithFriendlyNameFound = false;

    int i64LineLen = 0;

	directory_name[0] = EOS;
	file_name[0] = EOS;

	if ( CL_FAILED( status = cl_GetContext( &pCtxt ) ) )
	{
		DEBUG_PRINTF("cl_InitReaderFromFileByFriendlyName: GetContext FAILED !");
	}

	DEBUG_PRINTF("cl_InitReaderFromFileByFriendlyName: BEGIN");

	// Open file "readers_list.txt"
	file_in = fopen(file_on_disk, "r");
	if (file_in == NULL)
	{
		GetDirectoryName(FullExeFileNameWithDirectory, directory_name);
		strcat(file_name, directory_name);
		strcat(file_name, file_on_disk);

		file_in = fopen(file_name, "r");
	}
	if (file_in == 0)
	{
		DEBUG_PRINTF("Impossible d'ouvrir le fichier %s !", file_name);
		return CL_ERROR;
	}

	//------------------------------------------------------------------------
	// For all lines in file
	// Fill the reader then add it to the list
	//------------------------------------------------------------------------
	while ( fgets( ligne, STRING_LENGTH, file_in ) != NULL && aReaderWithFriendlyNameFound == false )
	{
		i64LineLen = strlen(ligne);
		strcpy(buf,ligne);

		iFirstIndex = iLastIndex = iFoundIndex = 0;
		char aucArg[64];
		bFound = false;
		iFirstIndex = iLastIndex = iState = 0;
		eType = UNKNOWN_READER_TYPE;

		// don't parse line beginning with ';' ligne is commented
		if ( buf[0] == ';' )
			continue;

		DEBUG_PRINTF(ligne);

		// parse line to capture each field
		int i = 0;
		for ( i = 0; i < i64LineLen; i++ )
		{
			// look for ";"
			if ( buf[i] == ';' )
			{
				if ( i > 0 )
				{
					iLastIndex = i - 1;
					bFound = true;
				}
			}

			if ( bFound == true ) // a new field is found => get it
			{
				// prepare array to get new arguments
				memset( aucArg, 0, sizeof( aucArg ) );
				memcpy( aucArg, &buf[iFirstIndex], iLastIndex - iFirstIndex + 1 );
				iFieldLen = iLastIndex - iFirstIndex + 1;

				// depending on state, fill either field
				switch ( iState )
				{
					case (0):    //  get Reader Type
					{
						// if first element;
						if ( !memcmp( aucArg, "Serial", strlen("Serial")))
						{
							if ( CL_FAILED( cl_ReaderFillWithDefaultFields( aReader, COM_READER_TYPE ) ) )
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

							if ( CL_FAILED( cl_ReaderFillWithDefaultFields( aReader, IP_READER_TYPE ) ) )
								break;
							eType = IP_READER_TYPE;
							iFirstIndex = i + 1;
							bFound = false;
							iState = 1;
							continue;
						}
						if ( !memcmp( aucArg, "BlueTooth", strlen("BlueTooth")))
						{
							if ( CL_FAILED( cl_ReaderFillWithDefaultFields( aReader, BT_READER_TYPE ) ) )
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
						memset( aReader->aucLabel, 0, sizeof(aReader->aucLabel ) );
						memcpy(aReader->aucLabel, &buf[iFirstIndex], iFieldLen );
						bFound = false;
						iFirstIndex = i + 1;
						iState = 2;
						int result = memcmp( aReader->aucLabel, friendlyName, strlen(friendlyName) );
						if (result == 0 )
						{
							aReaderWithFriendlyNameFound = true;
						}
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
								memset(aReader->tCOMParams.aucPortName, 0, sizeof(aReader->tCOMParams.aucPortName ) );

								// copy reader name
								memcpy(aReader->tCOMParams.aucPortName, &buf[iFirstIndex], iFieldLen );
								bFound = false;
								iFirstIndex = i + 1;
								iState = 3;
								break;
							}
							case ( IP_READER_TYPE):
							{
								memset(aReader->tIPParams.aucIpAddr, 0, sizeof(aReader->tIPParams.aucIpAddr ) );
								memcpy(aReader->tIPParams.aucIpAddr, &buf[iFirstIndex], iFieldLen);
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
									aReader->tCOMParams.eBaudRate = CL_COM_BAUDRATE_9600;

								if (!memcmp( &buf[iFirstIndex], "19200", strlen("19200")))
									aReader->tCOMParams.eBaudRate = CL_COM_BAUDRATE_19200;

								if (!memcmp( &buf[iFirstIndex], "38400", strlen("38400")))
									aReader->tCOMParams.eBaudRate = CL_COM_BAUDRATE_38400;

								if (!memcmp( &buf[iFirstIndex], "57600", strlen("57600")))
									aReader->tCOMParams.eBaudRate = CL_COM_BAUDRATE_57600;

								if (!memcmp( &buf[iFirstIndex], "115200", strlen("115200")))
									aReader->tCOMParams.eBaudRate = CL_COM_BAUDRATE_115200;

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
//									aReader->tIPParams.u32Port = atoi( pData );
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
								clu32 ulOutLen = sizeof(aReader->tCloverSense.au8RadioAddress );
								ConvertASCIToHex( (clu8*)&buf[iFirstIndex], iFieldLen, (clu8*)aReader->tCloverSense.au8RadioAddress, &ulOutLen );
								bFound = false;
								iFirstIndex = i + 1;
								iState = 5;
								break;
							}
							case ( IP_READER_TYPE ):
							{
								// convert ASCII to Hex
								clu32 ulOutLen = sizeof(aReader->tCloverSense.au8RadioAddress );
								ConvertASCIToHex( (clu8*)&buf[iFirstIndex], iFieldLen, (clu8*)aReader->tCloverSense.au8RadioAddress, &ulOutLen );
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
	}

	if (file_in)
	{
		err_no = fclose(file_in);
		if (err_no != 0)
	    {
			DEBUG_PRINTF("cl_InitReaderFromFileByFriendlyName: Impossible de fermer le fichier !");
	    }
	}

	if ( aReaderWithFriendlyNameFound )
	{
		DEBUG_PRINTF("cl_InitReaderFromFileByFriendlyName: Reader found : %s", aReader->aucLabel);
		return CL_OK;
	}

	DEBUG_PRINTF("cl_InitReaderFromFileByFriendlyName: END");

	return ( CL_PARAMS_ERR );
}
