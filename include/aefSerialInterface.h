/***************************************************************************/
/*!
 **	\file 	aefSerialInterface.h 
 **	\brief {Brief DescriptionAbsrtac base class
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
 *	\class CSerial
 *	\brief Class to implement Serial interface
 *	
 */

class aefSerialInterface
{
    // Class Methods
public:

    virtual AEF_STATUS Configure(const char* pDevName, // device Init
                                 CAefQueueTask* pTask, uint32_t dataRcvdId, // msg  Init
                                 uint32_t shutdownMsgId, aefPacketInterface* pPktObj,
                                 uint32_t bufsize, int timeout = 0, int keepalive = 0) = 0;
    virtual AEF_STATUS Send(uint8_t* pData, uint32_t dataLen) = 0;
    virtual inline bool IsConfigured() = 0;
    virtual void SetKeepalive(int timeout, int keepalive) = 0;
    virtual AEF_STATUS ConfigurePort(uint32_t baudRate,
                                     uint8_t dataBits,
                                     uint8_t parity,
                                     uint8_t stopBits) = 0;

protected:
    virtual AEF_STATUS Run() = 0;

};

