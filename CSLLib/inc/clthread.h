/*****************************************************************************/
/*                           CLOVER SECURED LAYER                            */
/*                          SoDevLog Copyright 2016                          */
/*****************************************************************************/
/*  Name: clthread.h                                                         */
/*                                                                           */
/*  Created on: 1 avril 2016                                                 */
/*      Author: braby                                                        */
/*****************************************************************************/
/* Description: Internal Thread                                              */
/*****************************************************************************/
#ifndef CLTHREAD_H_
#define CLTHREAD_H_


e_Result clReaderWriteThread( clvoid *pCallingReader );

e_Result clReaderReadThread( clvoid *pCallingReader );

e_Result clCryptoThread( clvoid );

e_Result clRegisterThread( clvoid *fnThreadId, clu64 u64ThreadId );

e_Result clRegisteredThread( clu64 u64ThreadId, clvoid *pfnThreadId );


e_Result clUnregisterThread( clvoid *fnThreadId, clu64 u64ThreadId );



#endif /* CLTHREAD_H_ */
/**************************************************/
