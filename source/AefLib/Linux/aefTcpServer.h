/*****************************************************************************
 *
 *	aefLinuxTcpServer.h
 *
 *   Copyright (C) Avtec, Inc., 1990-2007.
 *
 *   Avtec, Inc.
 *   4335 Augusta Hwy.
 *   Gilbert, SC 29054 USA
 *   (803) 892-2181
 *
 *   All rights reserved. Copying, compilation, modification, distribution
 *   or any other use whatsoever of this material is strictly prohibited
 *   without the written consent of Avtec, Inc.
 *
 *****************************************************************************/
#ifndef _aefLinuxTcpServer_h_
#define _aefLinuxTcpServer_h_

#include <aefTypes.h>
#include <aefTcpServerBase.h>
#include "aefQueueTask.h"

class CAefTcpServer : public CAefTask, public CAefTcpServerBase
{
    // Class Methods
public:
    CAefTcpServer();
    CAefTcpServer(char *name, int priority, int options, int stack);
    virtual ~CAefTcpServer();
    virtual AEF_STATUS Configure(UNSIGNED16 port, UNSIGNED16 maxconnections, // port Init
                                 CAefQueueTask* pTask, UNSIGNED32 dataRcvdId, // msg  Init
                                 UNSIGNED32 connectId, UNSIGNED32 disconnectId,
                                 UNSIGNED32 shutdownId, CPacket* pPktObj,
                                 UNSIGNED32 bufsize, int timeout = 0, int keepalive = 0);
    virtual AEF_STATUS Send(UNSIGNED8* pData, UNSIGNED32 dataLen, AEF_TCP_CONN_ID connId = INVALID_CONN_ID);
    virtual UNSIGNED32 ClientGetCount();
    virtual inline bool IsConfigured()
    {
        return m_bConfigured;
    };
    virtual void SetKeepalive(int timeout, int keepalive);
    virtual bool SetQos(UNSIGNED8 value);
    virtual AEF_STATUS GetClientInfo(AEF_TCP_CONN_ID connectionID, UNSIGNED32& ipAdrs, UNSIGNED16& port);
    virtual AEF_STATUS CloseConnection(AEF_TCP_CONN_ID connectionID);

protected:
    virtual AEF_STATUS Run();
protected:
    virtual AEF_STATUS Init(void* pTaskData = NULL);
    virtual AEF_STATUS Exit(AEF_STATUS exitCode);
    virtual void Construct();
    virtual AEF_STATUS SendSocket(AEF_TCP_CONN_ID socket, UNSIGNED8* pData, UNSIGNED32 dataLen);
    virtual AEF_STATUS AddClient(AEF_TCP_CONN_ID socket, struct sockaddr_in* pAddr, int addrLen);
    virtual void RemoveClient(AEF_TCP_CONN_ID socket);
    virtual void RemoveAllClients();
    virtual bool IsValidSocket(AEF_TCP_CONN_ID socket);
    virtual AEF_STATUS SetSocketOptions(AEF_TCP_CONN_ID socket);
    virtual void ReceiveTimeoutCheck(void);
    virtual void KeepAliveCheck(void);
    virtual UNSIGNED32 TicksFromMs(UNSIGNED32 ms);
    virtual UNSIGNED32 TicksFromTOD();
protected:
    CAefQueueTask* m_pParentTask; // parent task
    UNSIGNED32 m_DataRcvdId; // data received message ID
    UNSIGNED32 m_ConnectId; // client connected message ID
    UNSIGNED32 m_DisconnectId; // client disconnected message ID
    UNSIGNED32 m_ShutdownId; // server unexpectedly shut down message ID
    CPacket* m_pPktObj;
    UNSIGNED16 m_Port; // Port number being monitored
    UNSIGNED16 m_MaxConnections; // Maximum number of simultaneous clients
    bool m_bConfigured; // If TRUE, object is initialized
    UNSIGNED32 m_BufSize; // Read buffer size
    UNSIGNED8* m_pBuf;
    int m_QosValue;
    UNSIGNED32 m_ClientTimeout; // Milliseconds before a non-responding
    // client is terminated.
    UNSIGNED32 m_KeepAliveTimeout;

protected:
    UNSIGNED32 m_LastSend;
    AEF_TCP_CONN_ID m_sockListen;
    pthread_mutex_t m_semClientMap;
    CLIENTMAP m_ClientMap;
    fd_set m_setSelect;
    SIGNED32 m_exitReason;
    UNSIGNED32 m_ntickspersec;
};

#endif // _aefLinuxTcpServer_h_
