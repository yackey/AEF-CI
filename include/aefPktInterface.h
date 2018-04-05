/***************************************************************************/
/*!
**	\file 	aefPktInterface.h 
**	\brief Abstract class for parsing/building message packets
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

class aefPacketInterface
{
public:
	virtual uint8_t* Build(uint8_t *pBuf, uint32_t bufLen, uint32_t *pPktLen) = 0;
	virtual uint32_t ParseData(uint32_t connID, uint8_t *pBuf, uint32_t bufLen, CAefQueueTask* pTask, uint32_t msgRcvdId) = 0;
	virtual void RestartParser(uint32_t connID) = 0;
	virtual bool GetKeepaliveMessage(uint8_t **ppData, uint32_t *pDataLen) = 0;
	virtual void AddClient(uint32_t connID) = 0;
	virtual void RemoveClient(uint32_t connID) = 0;

    // the way it was zzzzz virtual void AddClient(uint32_t connID) {};
    // the way it was zzzzz virtual void RemoveClient(uint32_t connID) {};
};

