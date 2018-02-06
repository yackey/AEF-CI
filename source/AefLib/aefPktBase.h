/***************************************************************************/
/*!
**	\file 	aefPkt.h 
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
#ifndef _aefPktBase_h_
#define _aefPktBase_h_

class CPacket
{
public:
	virtual UNSIGNED8* Build(UNSIGNED8 *pBuf, UNSIGNED32 bufLen, UNSIGNED32 *pPktLen) = 0;
	virtual UNSIGNED32 ParseData(UNSIGNED32 connID, UNSIGNED8 *pBuf, UNSIGNED32 bufLen, CAefQueueTask* pTask, UNSIGNED32 msgRcvdId) = 0;
	virtual void RestartParser(UNSIGNED32 connID) {};
	virtual bool GetKeepaliveMessage(UNSIGNED8 **ppData, UNSIGNED32 *pDataLen) = 0;
	virtual void AddClient(UNSIGNED32 connID) {};
	virtual void RemoveClient(UNSIGNED32 connID) {};
};

#endif // _aefPktBase_h_
