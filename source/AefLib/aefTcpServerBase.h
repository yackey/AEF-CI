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
#ifndef _aefTcpServerBase_h_
#define _aefTcpServerBase_h_

#include <aefTypesBase.h>
class CAefQueueTask;
class CPacket;

/*!
 *	\class CAefTcpServerBase
 *	\brief Class to implement a TCP Server
 *	
 */
class CAefTcpServerBase
{
    // Class Methods
public:
    CAefTcpServerBase();
    CAefTcpServerBase(char *name, int priority, int options, int stack);
    virtual ~CAefTcpServerBase();
    virtual AEF_STATUS Configure(UNSIGNED16 port, UNSIGNED16 maxconnections, // port Init
            CAefQueueTask* pTask, UNSIGNED32 dataRcvdId, // msg  Init
            UNSIGNED32 connectId, UNSIGNED32 disconnectId,
            UNSIGNED32 shutdownId, CPacket* pPktObj,
            UNSIGNED32 bufsize, int timeout = 0, int keepalive = 0) = 0;
    virtual AEF_STATUS Send(UNSIGNED8* pData, UNSIGNED32 dataLen, AEF_TCP_CONN_ID connId = INVALID_CONN_ID) = 0;
    virtual UNSIGNED32 ClientGetCount() = 0;
    virtual inline bool IsConfigured() = 0;
    virtual void SetKeepalive(int timeout, int keepalive) = 0;
    virtual bool SetQos(UNSIGNED8 value) = 0;
    virtual AEF_STATUS GetClientInfo(AEF_TCP_CONN_ID connectionID, UNSIGNED32& ipAdrs, UNSIGNED16& port) = 0;
    virtual AEF_STATUS CloseConnection(AEF_TCP_CONN_ID connectionID) = 0;

protected:
    virtual AEF_STATUS Run() = 0;
};

#endif //_aefTcpServerBase_h_


