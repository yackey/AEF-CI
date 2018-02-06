/***************************************************************************/
/*!
 **	\file 	aefTask.h 
 **	\brief {Brief Description}
 **
 **	Avtec, Inc.
 **	100 Innovation Place
 **	Lexington, SC 29072 USA
 **	(803) 358-3600
 **
 **	Copyright (C) Avtec, Inc., 1990-present.
 **	All rights reserved. Copying, compilation, modification, distribution
 **	or any other use whatsoever of this material is strictly prohibited
 **	without the written consent of Avtec, Inc.
 *****************************************************************************/
#ifndef _aefSerialBase_h_
#define _aefSerialBase_h_

#include <aefTypesBase.h>
class CAefQueueTask;
class CPacket;

/*!
 *	\class CSerial
 *	\brief Class to implement Serial interface
 *	
 */

class CAefSerialBase
{
    // Class Methods
public:
    CAefSerialBase();
    virtual ~CAefSerialBase();

    virtual AEF_STATUS Configure(const char* pDevName, // device Init
                                 CAefQueueTask* pTask, UNSIGNED32 dataRcvdId, // msg  Init
                                 UNSIGNED32 shutdownMsgId, CPacket* pPktObj,
                                 UNSIGNED32 bufsize, int timeout = 0, int keepalive = 0) = 0;
    virtual AEF_STATUS Send(UNSIGNED8* pData, UNSIGNED32 dataLen) = 0;
    virtual inline bool IsConfigured() = 0;
    virtual void SetKeepalive(int timeout, int keepalive) = 0;
    virtual AEF_STATUS ConfigurePort(UNSIGNED32 baudRate,
                                     UNSIGNED8 dataBits,
                                     UNSIGNED8 parity,
                                     UNSIGNED8 stopBits) = 0;

protected:
    virtual AEF_STATUS Run() = 0;

};

#endif //_aefSerialBase_h_
