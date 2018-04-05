/***************************************************************************/
/*!
 **	\file 	aefTcpServerInterface.h 
 **	\brief aefTcpServerInterface definition
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
#pragma once

#include <aefTypes.h>
class CAefQueueTask;
class aefPacketInterface;

/*!
 *	\class aefTcpServerInterface
 *	
 */
class aefTcpServerInterface
{
    // Class Methods
public:

    virtual AEF_STATUS Configure(uint16_t port, uint16_t maxconnections, // port Init
            CAefQueueTask* pTask, uint32_t dataRcvdId, // msg  Init
            uint32_t connectId, uint32_t disconnectId,
            uint32_t shutdownId, aefPacketInterface* pPktObj,
            uint32_t bufsize, int timeout = 0, int keepalive = 0) = 0;
    virtual AEF_STATUS Send(uint8_t* pData, uint32_t dataLen, AEF_TCP_CONN_ID connId = INVALID_CONN_ID) = 0;
    virtual uint32_t ClientGetCount() = 0;
    virtual inline bool IsConfigured() = 0;
    virtual void SetKeepalive(int timeout, int keepalive) = 0;
    virtual bool SetQos(uint8_t value) = 0;
    virtual AEF_STATUS GetClientInfo(AEF_TCP_CONN_ID connectionID, uint32_t& ipAdrs, uint16_t& port) = 0;
    virtual AEF_STATUS CloseConnection(AEF_TCP_CONN_ID connectionID) = 0;

protected:
    virtual AEF_STATUS Run() = 0;
};

