/****************************************************************************/
/*!
 **  \file   aefLinuxTcpServer.cpp
 **  \brief  VxWorks TCP Server class
 **
 **  The routines in this file implement TCP server class/task to handle
 **  multiple client connections.  A task will be created to listen for
 **  inbound connection attempts and receive data from connected clients.  A
 **  message may be sent to all connected clients by using the Send function.
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

#include "aefTypesBase.h"
#include "aefTcpServerBase.h"

/**************************************************************************/

/*!
 *     \brief      Default Constructor for CAefTcpServer class.
 *
 *     Default constructor.
 *
 ***************************************************************************/
CAefTcpServerBase::CAefTcpServerBase()
{

}

/**************************************************************************/

/*!
 *     \brief      Destructor for CAefTcpServer class.
 *
 *     Makes sure that the task associated with this CVxSerial instance is
 *     stopped, then frees any resources allocated by the class.
 *
 ***************************************************************************/
CAefTcpServerBase::~CAefTcpServerBase()
{

}

///**************************************************************************/
//
///*!
// * \brief Class Configuration
// *
// * Configures the TCP Server class parameters
// *
// * \param  port            Port to listen for inbound connection requests
// * \param  maxConnections  Maximum number of supported clients
// * \param  pTask           Task that should be notified of any events
// * \param  dataRcvdId      Message ID used when a valid message is received
// * \param  connectId       Message ID used when a client successfully connects to the server
// * \param  disconnectId    Message ID used when a client disconnects or the connection times out
// * \param  shutdownId      Message ID used when the server unexpectedly shuts down
// * \param  pPktObj         pointer to the CPacket-derived class that parses and builds the message protocol
// * \param  bufSize         Maximum number of bytes that can be sent to pClientObj at once
// * \param  timeout         If non-zero, indicates time (in ms) to disconnect a client if no valid packet is received
// * \param  keepalive       Time (in ms) to make sure that a message has been sent to all clients
// *
// * \return AEF_SUCCESS or AEF_ERROR
// *
// ***************************************************************************/
//AEF_STATUS CAefTcpServer::Configure(UNSIGNED16 port, UNSIGNED16 maxConnections, // port Init
//                                    CAefQueueTask* pTask, UNSIGNED32 dataRcvdId, // msg  Init
//                                    UNSIGNED32 connectId, UNSIGNED32 disconnectId,
//                                    UNSIGNED32 shutdownId, CPacket* pPktObj,
//                                    UNSIGNED32 bufSize, int timeout, int keepalive)
//{
//    AEF_STATUS result = AEF_ERROR;
//
//    // Check if we are already initialized
//    if (m_bConfigured == false)
//    {
//        m_pParentTask = pTask;
//        m_DataRcvdId = dataRcvdId;
//        m_ConnectId = connectId;
//        m_DisconnectId = disconnectId;
//        m_ShutdownId = shutdownId;
//        m_pPktObj = pPktObj;
//        m_BufSize = bufSize;
//        m_Port = port;
//        m_MaxConnections = maxConnections;
//        SetKeepalive(timeout, keepalive);
//
//        m_pBuf = new UNSIGNED8[bufSize];
//
//        if (m_pBuf != NULL)
//        {
//            result = AEF_SUCCESS;
//            m_bConfigured = true;
//        }
//    }
//
//    // Return any errors
//    return result;
//}
//
///**************************************************************************/
//
///*!
// * \brief Updates keepalive settings
// *
// * Allows dynamic updates of the keepalive settings for the serial connection
// *
// * \param  timeout    If non-zero, indicates time (in ms) after which no communication will result in shutdown
// * \param  keepalive  Time (in ms) to make sure that a message has been sent to the remote side.
// *
// * \return N/A
// *
// ***************************************************************************/
//void CAefTcpServer::SetKeepalive(int timeout, int keepalive)
//{
//    UNSIGNED32 newTimeout = MsToTimeTicks(timeout);
//    UNSIGNED32 newKeepalive = MsToTimeTicks(keepalive);
//
//    if (m_ClientTimeout != newTimeout)
//    {
//        m_ClientTimeout = newTimeout;
//    }
//
//    if (m_KeepAliveTimeout != newKeepalive)
//    {
//        m_KeepAliveTimeout = newKeepalive;
//    }
//}
//
//
///**************************************************************************/
//
///*!
// * \brief Send a message to all clients or a specific client
// *
// * \param  pData    Data buffer to send
// * \param  dataLen  Number of bytes in pData
// * \param  socket   socket to receive data, or ERROR to send data to all clients
// *
// * \return AEF_SUCCESS or AEF_ERROR
// *
// ***************************************************************************/
//AEF_STATUS CAefTcpServer::Send(UNSIGNED8 *pData, UNSIGNED32 dataLen, AEF_TCP_CONN_ID socket)
//{
//    AEF_STATUS result = AEF_ERROR;
//    CLIENTMAP::iterator it;
//    VX_TCP_CLIENT_INFO *pci;
//
//    // Make sure we are initialized
//    if ((m_bConfigured == true) && (ClientGetCount() > 0))
//    {
//        if (socket == AEF_ERROR)
//        {
//            pthread_mutex_lock(&m_semClientMap);
//            result = AEF_SUCCESS;
//            // Loop through all of the clients
//            for (it = m_ClientMap.begin(); (it != m_ClientMap.end()) && (result == AEF_SUCCESS);)
//            {
//                // Get a pointer to the socket entry
//                pci = (VX_TCP_CLIENT_INFO *) (*it).second;
//                result = SendSocket(pci->Socket, pData, dataLen);
//                // Point to the next entry in the map
//                it++;
//            }
//            pthread_mutex_unlock(&m_semClientMap);
//        }
//        else
//        {
//            result = SendSocket(socket, pData, dataLen);
//        }
//        if (result == AEF_SUCCESS)
//        {
//            m_LastSend = TicksFromTOD();
//        }
//    }
//
//    return result;
//}
//
//
//
///**************************************************************************/
//
///*!
// * \brief Sets standard socket options for client socket descriptors
// *
// * \param  socket  socket descriptor to be modified
// *
// * \return AEF_SUCCESS or AEF_ERROR
// *
// ***************************************************************************/
//AEF_STATUS CAefTcpServer::SetSocketOptions(AEF_TCP_CONN_ID socket)
//{
//    AEF_STATUS result;
//    int sockopt = 1;
//
//    if ((setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char*) &sockopt, sizeof (int)) == AEF_ERROR) ||
//            (setsockopt(socket, SOL_SOCKET, SO_REUSEPORT, (char*) &sockopt, sizeof (int)) == AEF_ERROR) ||
//            (setsockopt(socket, IPPROTO_IP, IP_TOS, (char*) &m_QosValue, sizeof (int)) == AEF_ERROR))
//    {
//        result = AEF_ERROR;
//    }
//    else
//    {
//        result = AEF_SUCCESS;
//    }
//    return result;
//}
//
///**************************************************************************/
//
///*!
// *  \brief  Allow caller from a different thread to close a connection.
// *
// *  Note that the connection is not destroyed immediately.  Caller will
// *  receive the disconnect message "event" when the connection is actually
// *  closed.
// *
// *  \param  connectionID  identifier for the connection to be removeed
// *
// *  \return  AEF_SUCCESS is connection is sucessfully flagged for closing,
// *           AEF_ERROR if connection couldn't be found.
// ***************************************************************************/
//AEF_STATUS CAefTcpServer::CloseConnection(AEF_TCP_CONN_ID connectionID)
//{
//    AEF_STATUS result = AEF_ERROR;
//    CLIENTMAP::iterator it;
//    VX_TCP_CLIENT_INFO *pci;
//
//    pthread_mutex_lock(&m_semClientMap); // Lock the socket map
//
//    if ((it = m_ClientMap.find(connectionID)) != m_ClientMap.end())
//    {
//        pci = (VX_TCP_CLIENT_INFO *) (*it).second; // Get a pointer to the object
//
//        // simply flag the client connection to be deleted at the next
//        // opportunity
//        pci->bDelete = 1;
//        result = AEF_SUCCESS;
//    }
//
//    pthread_mutex_unlock(&m_semClientMap); // Unlock the socket map
//
//    return result;
//}
//
///**************************************************************************/
///*!
//*   \brief Convert a millisecond value into a tick count
//*
//*   \return The number "ticks" equal to the number of milliseconds passed in
//*
//***************************************************************************/
//UNSIGNED32 CAefTcpServer::TicksFromMs(UNSIGNED32 ms)
//{
//    UNSIGNED32 nTps = (ms/1000) * m_ntickspersec;
//    return nTps;
//}
//
///**************************************************************************/
///*!
//*   \brief Convert a millisecond value into a tick count
//*
//*   \return The number "ticks" equal to the number of milliseconds passed in
//*
//***************************************************************************/
//UNSIGNED32 CAefTcpServer::TicksFromTOD()
//{
//    struct timeval tv;
//    gettimeofday(&tv, 0);
//    UNSIGNED32 ms = (tv.tv_usec/1000);
//    ms += (tv.tv_sec * 1000);
//    return TicksFromMs(ms);
//}