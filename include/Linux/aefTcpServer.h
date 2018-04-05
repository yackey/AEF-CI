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

#include <aefLinuxTypes.h>
#include <aefTcpServerInterface.h>
#include "aefQueueTask.h"

class CAefTcpServer : public CAefTask, public aefTcpServerInterface
{
    // Class Methods
public:
    CAefTcpServer();
    CAefTcpServer(char *name, int priority, int options, int stack);
    virtual ~CAefTcpServer();
    virtual AEF_STATUS Configure(uint16_t port, uint16_t maxconnections, // port Init
                                 CAefQueueTask* pTask, uint32_t dataRcvdId, // msg  Init
                                 uint32_t connectId, uint32_t disconnectId,
                                 uint32_t shutdownId, aefPacketInterface* pPktObj,
                                 uint32_t bufsize, int timeout = 0, int keepalive = 0);
    virtual AEF_STATUS Send(uint8_t* pData, uint32_t dataLen, AEF_TCP_CONN_ID connId = INVALID_CONN_ID);
    virtual uint32_t ClientGetCount();
    virtual inline bool IsConfigured()
    {
        return m_bConfigured;
    };
    virtual void SetKeepalive(int timeout, int keepalive);
    virtual bool SetQos(uint8_t value);
    virtual AEF_STATUS GetClientInfo(AEF_TCP_CONN_ID connectionID, uint32_t& ipAdrs, uint16_t& port);
    virtual AEF_STATUS CloseConnection(AEF_TCP_CONN_ID connectionID);

protected:
    virtual AEF_STATUS Run();
protected:
    virtual AEF_STATUS Init(void* pTaskData = NULL);
    virtual AEF_STATUS Exit(AEF_STATUS exitCode);
    virtual void Construct();
    virtual AEF_STATUS SendSocket(AEF_TCP_CONN_ID socket, uint8_t* pData, uint32_t dataLen);
    virtual AEF_STATUS AddClient(AEF_TCP_CONN_ID socket, struct sockaddr_in* pAddr, int addrLen);
    virtual void RemoveClient(AEF_TCP_CONN_ID socket);
    virtual void RemoveAllClients();
    virtual bool IsValidSocket(AEF_TCP_CONN_ID socket);
    virtual AEF_STATUS SetSocketOptions(AEF_TCP_CONN_ID socket);
    virtual void ReceiveTimeoutCheck(void);
    virtual void KeepAliveCheck(void);
    virtual uint32_t TicksFromMs(uint32_t ms);
    virtual uint32_t TicksFromTOD();
protected:
    CAefQueueTask* m_pParentTask; // parent task
    uint32_t m_DataRcvdId; // data received message ID
    uint32_t m_ConnectId; // client connected message ID
    uint32_t m_DisconnectId; // client disconnected message ID
    uint32_t m_ShutdownId; // server unexpectedly shut down message ID
    aefPacketInterface* m_pPktObj;
    uint16_t m_Port; // Port number being monitored
    uint16_t m_MaxConnections; // Maximum number of simultaneous clients
    bool m_bConfigured; // If TRUE, object is initialized
    uint32_t m_BufSize; // Read buffer size
    uint8_t* m_pBuf;
    int m_QosValue;
    uint32_t m_ClientTimeout; // Milliseconds before a non-responding
    // client is terminated.
    uint32_t m_KeepAliveTimeout;

protected:
    uint32_t m_LastSend;
    AEF_TCP_CONN_ID m_sockListen;
    pthread_mutex_t m_semClientMap;
    CLIENTMAP m_ClientMap;
    fd_set m_setSelect;
    int32_t m_exitReason;
    uint32_t m_ntickspersec;
};

#endif // _aefLinuxTcpServer_h_
