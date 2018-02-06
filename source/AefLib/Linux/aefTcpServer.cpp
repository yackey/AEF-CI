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

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <map>

#include "aefTypesBase.h"
#include "aefMutexBase.h"
#include "aefTimerBase.h"
#include "aefTaskMessageBase.h"
#include "aefTaskBase.h"
#include "aefTcpServer.h"
#include "aefPktBase.h"

//#include "aefTcpServerBase.h"

/**************************************************************************/

/*!
 *     \brief      Default Constructor for CAefTcpServer class.
 *
 *     Default constructor.
 *
 ***************************************************************************/
CAefTcpServer::CAefTcpServer() : CAefTask("tTcpSrvr", 100, 0, 5000)
{
    Construct();
}


/**************************************************************************/

/*!
 * \brief  Constructor for CAefTcpServer class specifying task-related parameters.
 *
 * \param  name       Name of task.
 * \param  priority   Priority of task.
 * \param  options    Task intiailzation options
 * \param  stack      Task stack size
 ***************************************************************************/
CAefTcpServer::CAefTcpServer(char *name, int priority, int options, int stack)
: CAefTask(name, priority, options, stack)
{
    Construct();
}


/**************************************************************************/

/*!
 *     \brief      Method called by all constructors to initalized member variables, etc.
 *
 ***************************************************************************/
void CAefTcpServer::Construct()
{
    // we want a mutual-exclusion semaphore, i.e., recursive
    // Initialize semaphore for the valid socket map
    pthread_mutexattr_t mtAttr;
    pthread_mutexattr_init(&mtAttr);
    pthread_mutexattr_settype(&mtAttr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_semClientMap, &mtAttr);

    m_pParentTask = NULL; // no boss yet
    m_pBuf = NULL;
    m_bConfigured = false;

    m_ClientTimeout = 0;
    m_QosValue = 0;
    m_LastSend = TicksFromTOD();
    m_sockListen = AEF_ERROR;

    m_ClientMap.clear();
    m_ntickspersec = sysconf(_SC_CLK_TCK);
}

/**************************************************************************/

/*!
 *     \brief      Destructor for CAefTcpServer class.
 *
 *     Makes sure that the task associated with this CVxSerial instance is
 *     stopped, then frees any resources allocated by the class.
 *
 ***************************************************************************/
CAefTcpServer::~CAefTcpServer()
{
    // Clean up the task
    Stop();

    // delete the semaphores
    // TODO: make this conditional on actually creating it
    pthread_mutex_destroy(&m_semClientMap); // Delete the socket map semaphore
}

/**************************************************************************/

/*!
 * \brief Class Configuration
 *
 * Configures the TCP Server class parameters
 *
 * \param  port            Port to listen for inbound connection requests
 * \param  maxConnections  Maximum number of supported clients
 * \param  pTask           Task that should be notified of any events
 * \param  dataRcvdId      Message ID used when a valid message is received
 * \param  connectId       Message ID used when a client successfully connects to the server
 * \param  disconnectId    Message ID used when a client disconnects or the connection times out
 * \param  shutdownId      Message ID used when the server unexpectedly shuts down
 * \param  pPktObj         pointer to the CPacket-derived class that parses and builds the message protocol
 * \param  bufSize         Maximum number of bytes that can be sent to pClientObj at once
 * \param  timeout         If non-zero, indicates time (in ms) to disconnect a client if no valid packet is received
 * \param  keepalive       Time (in ms) to make sure that a message has been sent to all clients
 *
 * \return AEF_SUCCESS or AEF_ERROR
 *
 ***************************************************************************/
AEF_STATUS CAefTcpServer::Configure(UNSIGNED16 port, UNSIGNED16 maxConnections, // port Init
                                    CAefQueueTask* pTask, UNSIGNED32 dataRcvdId, // msg  Init
                                    UNSIGNED32 connectId, UNSIGNED32 disconnectId,
                                    UNSIGNED32 shutdownId, CPacket* pPktObj,
                                    UNSIGNED32 bufSize, int timeout, int keepalive)
{
    AEF_STATUS result = AEF_ERROR;

    // Check if we are already initialized
    if (m_bConfigured == false)
    {
        m_pParentTask = pTask;
        m_DataRcvdId = dataRcvdId;
        m_ConnectId = connectId;
        m_DisconnectId = disconnectId;
        m_ShutdownId = shutdownId;
        m_pPktObj = pPktObj;
        m_BufSize = bufSize;
        m_Port = port;
        m_MaxConnections = maxConnections;
        SetKeepalive(timeout, keepalive);

        m_pBuf = new UNSIGNED8[bufSize];

        if (m_pBuf != NULL)
        {
            result = AEF_SUCCESS;
            m_bConfigured = true;
        }
    }

    // Return any errors
    return result;
}

/**************************************************************************/

/*!
 * \brief Updates keepalive settings
 *
 * Allows dynamic updates of the keepalive settings for the serial connection
 *
 * \param  timeout    If non-zero, indicates time (in ms) after which no communication will result in shutdown
 * \param  keepalive  Time (in ms) to make sure that a message has been sent to the remote side.
 *
 * \return N/A
 *
 ***************************************************************************/
void CAefTcpServer::SetKeepalive(int timeout, int keepalive)
{
    UNSIGNED32 newTimeout = MsToTimeTicks(timeout);
    UNSIGNED32 newKeepalive = MsToTimeTicks(keepalive);

    if (m_ClientTimeout != newTimeout)
    {
        m_ClientTimeout = newTimeout;
    }

    if (m_KeepAliveTimeout != newKeepalive)
    {
        m_KeepAliveTimeout = newKeepalive;
    }
}


/**************************************************************************/

/*!
 * \brief Send a message to all clients or a specific client
 *
 * \param  pData    Data buffer to send
 * \param  dataLen  Number of bytes in pData
 * \param  socket   socket to receive data, or ERROR to send data to all clients
 *
 * \return AEF_SUCCESS or AEF_ERROR
 *
 ***************************************************************************/
AEF_STATUS CAefTcpServer::Send(UNSIGNED8 *pData, UNSIGNED32 dataLen, AEF_TCP_CONN_ID socket)
{
    AEF_STATUS result = AEF_ERROR;
    CLIENTMAP::iterator it;
    VX_TCP_CLIENT_INFO *pci;

    // Make sure we are initialized
    if ((m_bConfigured == true) && (ClientGetCount() > 0))
    {
        if (socket == AEF_ERROR)
        {
            pthread_mutex_lock(&m_semClientMap);
            result = AEF_SUCCESS;
            // Loop through all of the clients
            for (it = m_ClientMap.begin(); (it != m_ClientMap.end()) && (result == AEF_SUCCESS);)
            {
                // Get a pointer to the socket entry
                pci = (VX_TCP_CLIENT_INFO *) (*it).second;
                result = SendSocket(pci->Socket, pData, dataLen);
                // Point to the next entry in the map
                it++;
            }
            pthread_mutex_unlock(&m_semClientMap);
        }
        else
        {
            result = SendSocket(socket, pData, dataLen);
        }
        if (result == AEF_SUCCESS)
        {
            m_LastSend = TicksFromTOD();
        }
    }

    return result;
}

/**************************************************************************/

/*!
 * \brief Send a message to a specific client
 *
 * \param  socket   socket of the client to receive data
 * \param  pData    Data buffer to send
 * \param  dataLen  Number of bytes in pData
 *
 * \return AEF_SUCCESS or AEF_ERROR
 *
 ***************************************************************************/
AEF_STATUS CAefTcpServer::SendSocket(AEF_TCP_CONN_ID socket, UNSIGNED8* pData, UNSIGNED32 dataLen)
{
    AEF_STATUS result = AEF_ERROR;
    UNSIGNED8* pPkt;
    UNSIGNED32 pktLen;
    pPkt = m_pPktObj->Build(pData, dataLen, &pktLen);
    if (pPkt != NULL)
    {
        // Make sure we are initialized
        if (m_bConfigured == true)
        {
            pthread_mutex_lock(&m_semClientMap);
            // Check to see if the socket is in the socket map... if it isn't, then we
            //	need to ignore the message, as the socket is currently closed...
            if (IsValidSocket(socket))
            {
                if (write(socket, (char*) pPkt, pktLen) == (int) pktLen)
                {
                    result = AEF_SUCCESS;
                }
            }
            pthread_mutex_unlock(&m_semClientMap);
        }
        delete [] pPkt;
    }
    return result;
}

/**************************************************************************/

/*!
 * \brief Initialize the running task, just before entering the main processing loop
 *
 * \param  pTaskData  task data passed down to base class
 *
 * \return AEF_SUCCESS or AEF_ERROR
 *
 ***************************************************************************/
AEF_STATUS CAefTcpServer::Init(void* pTaskData)
{
    AEF_STATUS result = AEF_ERROR;
    struct sockaddr_in localAdrs;
    int sockaddrsize;

    if ((CAefTask::Init(pTaskData) == AEF_SUCCESS) && (m_bConfigured == true))
    {
        sockaddrsize = sizeof (struct sockaddr_in);
        bzero((char*) &localAdrs, sockaddrsize);
        localAdrs.sin_family = AF_INET;
        localAdrs.sin_port = htons(m_Port);
        localAdrs.sin_addr.s_addr = htonl(INADDR_ANY);

        m_sockListen = socket(AF_INET, SOCK_STREAM, 0);

        if (m_sockListen != AEF_ERROR)
        {
            if ((SetSocketOptions(m_sockListen) != AEF_ERROR) &&
                    (bind(m_sockListen, (struct sockaddr*) &localAdrs, sockaddrsize) != AEF_ERROR) &&
                    (listen(m_sockListen, 1) != AEF_ERROR))
            {
                result = AEF_SUCCESS;
                FD_ZERO(&m_setSelect);
                FD_SET(m_sockListen, &m_setSelect);
            }
            else
            {
                close(m_sockListen);
                m_sockListen = AEF_ERROR;
            }
        }
    }
    return result;
}

/**************************************************************************/

/*!
 * \brief Main processing loop; receives serial data and alerts the client object.
 *
 * \return Exit code (?)
 *
 ***************************************************************************/
AEF_STATUS CAefTcpServer::Run()
{
    struct sockaddr_in addr;
    socklen_t addrlen;
    int fdNew;
    fd_set selSet;
    struct timeval selTmout;
    int selResult;
    int bytesRead;
    CLIENTMAP::iterator it;
    VX_TCP_CLIENT_INFO *pci;
    SIGNED32 watchdogTime;
    SIGNED32 currentTs;

    // TODO: I think this is correct
    //  some are absolute, some ain't
    selTmout.tv_sec = 0;
    selTmout.tv_usec = 500000;
    addrlen = sizeof (addr);

    RegisterWatchdogKick();
    // TODO: need to see what is expected here
    watchdogTime = (SIGNED32) GetTimestamp();

    m_exitReason = 0;
    while (m_bTaskRunning == true)
    {
        selSet = m_setSelect;
        selResult = select(FD_SETSIZE, &selSet, NULL, NULL, &selTmout);
        if (selResult != AEF_ERROR)
        {
            // loop through connected sockets, looking for sockets flagged to be closed
            pthread_mutex_lock(&m_semClientMap); // Lock the socket map

            for (it = m_ClientMap.begin(); it != m_ClientMap.end();)
            {
                pci = (VX_TCP_CLIENT_INFO *) (*it).second;
                it++;
                if (pci->bDelete)
                {
                    RemoveClient(pci->Socket);
                }
            }
            pthread_mutex_unlock(&m_semClientMap); // Unlock the socket map

            // now look for activity on the remaining sockets
            if (selResult > 0)
            {
                // loop through connected sockets
                pthread_mutex_lock(&m_semClientMap); // Lock the socket map
                for (it = m_ClientMap.begin(); it != m_ClientMap.end();)
                {
                    pci = (VX_TCP_CLIENT_INFO *) (*it).second;
                    if (FD_ISSET(pci->Socket, &selSet))
                    {
                        if ((bytesRead = recv(pci->Socket, (char*) m_pBuf, m_BufSize, 0)) != AEF_ERROR)
                        {
                            if (bytesRead > 0)
                            {
                                // if client says a complete and valid message has been received
                                // (i.e., client returns a "1"), update m_LastRcvd
                                if (m_pPktObj->ParseData(pci->Socket, m_pBuf, bytesRead, m_pParentTask, m_DataRcvdId) == 1)
                                {
                                    pci->LastRcv = TicksFromTOD();
                                }
                            }
                            it++;
                            if (bytesRead == 0)
                            {
                                RemoveClient(pci->Socket);
                            }
                        }
                        else
                        {
                            it++;
                            RemoveClient(pci->Socket);
                        }
                    }
                    else
                    {
                        it++;
                    }
                }
                pthread_mutex_unlock(&m_semClientMap); // Unlock the socket map

                // now check the select socket
                if (FD_ISSET(m_sockListen, &selSet))
                {
                    // accept the new guy
                    fdNew = accept(m_sockListen, (struct sockaddr *) &addr, &addrlen);
                    if (fdNew != AEF_ERROR)
                    {
                        // if we haven't reached our max connections, add him to the list
                        if (AddClient(fdNew, &addr, addrlen) == AEF_SUCCESS)
                        {
                            CAefTaskMessage msg(m_ConnectId);
                            msg.SetData((unsigned char*) &fdNew, sizeof (fdNew));
                            m_pParentTask->SendMessage(&msg);
                        }
                        else
                        {
                            // if we couldn't add it to the list for any reason,
                            // close the socket
                            close(fdNew);
                        }
                    }
                }
            }
            ReceiveTimeoutCheck();
            KeepAliveCheck();

            currentTs = (SIGNED32) GetTimestamp();
            if ((currentTs - watchdogTime) >= (SIGNED32) MsToTimeTicks(1000))
            {
                WatchdogReport();
                watchdogTime = currentTs;
            }
        }
        else
        {
            // shouldn't receive ERROR here, so bail out?
            m_bTaskRunning = false;
            m_exitReason = 1;
            // send an error back?
        }
    }

    if (m_exitReason == 0)
    {
        // stop kicking the dog only on a normal shutdown
        UnregisterWatchdogKick();
    }

    close(m_sockListen);
    m_sockListen = AEF_ERROR;
    FD_ZERO(&m_setSelect);

    return AEF_SUCCESS;
}

/**************************************************************************/

/*!
 * \brief Perform task cleanup just before task is about to exit
 *
 * \return AEF_SUCCESS or AEF_ERROR
 *
 ***************************************************************************/
AEF_STATUS CAefTcpServer::Exit(AEF_STATUS exitCode)
{
    RemoveAllClients();
    // Reset the variables
    m_bConfigured = false;
    if (m_pBuf != NULL)
    {
        delete [] m_pBuf;
        m_pBuf = NULL;
    }
    if (m_exitReason != 0)
    {
        // we had an asynchronous failure, so alert the client
        CAefTaskMessage msg(m_ShutdownId);
        m_pParentTask->SendMessage(&msg);
    }
    return CAefTask::Exit(exitCode);
}

/**************************************************************************/

/*!
 * \brief Sets standard socket options for client socket descriptors
 *
 * \param  socket  socket descriptor to be modified
 *
 * \return AEF_SUCCESS or AEF_ERROR
 *
 ***************************************************************************/
AEF_STATUS CAefTcpServer::SetSocketOptions(AEF_TCP_CONN_ID socket)
{
    AEF_STATUS result;
    int sockopt = 1;

    if ((setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char*) &sockopt, sizeof (int)) == AEF_ERROR) ||
            // todo this don't work (setsockopt(socket, SOL_SOCKET, SO_REUSEPORT, (char*) &sockopt, sizeof (int)) == AEF_ERROR) ||
            (setsockopt(socket, IPPROTO_IP, IP_TOS, (char*) &m_QosValue, sizeof (int)) == AEF_ERROR))
    {
        result = AEF_ERROR;
    }
    else
    {
        result = AEF_SUCCESS;
    }
    return result;
}

/**************************************************************************/

/*!
 * \brief Check all connected clients to verify they are still sending valid packets.
 *
 ***************************************************************************/
void CAefTcpServer::ReceiveTimeoutCheck(void)
{
    CLIENTMAP::iterator it;
    VX_TCP_CLIENT_INFO *pci;

    if ((m_bConfigured == true) && m_ClientTimeout)
    {
        pthread_mutex_lock(&m_semClientMap); // Lock the socket map
        for (it = m_ClientMap.begin(); it != m_ClientMap.end();)
        {
            pci = (VX_TCP_CLIENT_INFO *) (*it).second;
            if (TicksFromTOD() - pci->LastRcv > m_ClientTimeout)
            {
                it++;
                RemoveClient(pci->Socket);
            }
            else
            {
                it++;
            }
        }
        pthread_mutex_unlock(&m_semClientMap); // Unlock the socket map
    }
}

/**************************************************************************/

/*!
 * \brief Determine if a keepalive message should be sent to the clients
 *
 ***************************************************************************/
void CAefTcpServer::KeepAliveCheck(void)
{
    UNSIGNED8 *pData;
    UNSIGNED32 dataLen;

    if (m_KeepAliveTimeout)
    {
        if (((TicksFromTOD() - m_LastSend) > m_KeepAliveTimeout) &&
                (m_pPktObj->GetKeepaliveMessage(&pData, &dataLen) == true))
        {
            Send(pData, dataLen);
        }
    }
}


/**************************************************************************/

/*!
 * \brief Add a client to the client map
 *
 * \param  socket  socket descriptor to be added
 *
 * \return AEF_SUCCESS or AEF_ERROR
 *
 ***************************************************************************/
AEF_STATUS CAefTcpServer::AddClient(AEF_TCP_CONN_ID socket, struct sockaddr_in* pAddr, int addrLen)
{
    VX_TCP_CLIENT_INFO *pClientInfo;
    AEF_STATUS result = AEF_ERROR;
    pthread_mutex_lock(&m_semClientMap);
    if (ClientGetCount() < m_MaxConnections)
    {
        if (SetSocketOptions(socket) == AEF_SUCCESS)
        {
            pClientInfo = new VX_TCP_CLIENT_INFO;
            if (pClientInfo != NULL)
            {
                if (addrLen != 0)
                {
                    pClientInfo->IPAddress = pAddr->sin_addr.s_addr;
                    pClientInfo->Port = pAddr->sin_port;
                }
                else
                {
                    pClientInfo->IPAddress = 0;
                    pClientInfo->Port = 0;
                }
                pClientInfo->Socket = socket;
                pClientInfo->LastRcv = TicksFromTOD();
                pClientInfo->bDelete = 0;
                m_ClientMap.insert(CLIENTMAP::value_type(socket, pClientInfo));
                FD_SET(socket, &m_setSelect);
                // Tell the packet parser about the new client...
                m_pPktObj->AddClient(socket);
                result = AEF_SUCCESS;
            }
        }
    }
    pthread_mutex_unlock(&m_semClientMap);
    return (result);
}

/**************************************************************************/

/*!
 * \brief Remove a client from the client map and disconnect the client
 *
 * \param  socket  socket descriptor to be removeed
 *
 ***************************************************************************/
void CAefTcpServer::RemoveClient(AEF_TCP_CONN_ID socket)
{
    CLIENTMAP::iterator it;
    VX_TCP_CLIENT_INFO *pci;

    pthread_mutex_lock(&m_semClientMap); // Lock the socket map

    if ((it = m_ClientMap.find(socket)) != m_ClientMap.end())
    {
        close(socket); // Close the physical socket
        pci = (VX_TCP_CLIENT_INFO *) (*it).second; // Get a pointer to the object
        delete pci; // Delete the SOCKET_INFO object
        m_ClientMap.erase(socket); // Delete the from the client map
        FD_CLR(socket, &m_setSelect);

        // Tell the packet parser about a client has been removed...
        m_pPktObj->RemoveClient(socket);

        CAefTaskMessage msg(m_DisconnectId);

        msg.SetData((unsigned char*) &socket, sizeof (socket));
        m_pParentTask->SendMessage(&msg);
    }

    pthread_mutex_unlock(&m_semClientMap); // Unlock the socket map
}


/**************************************************************************/

/*!
 * \brief Remove all clients from the client map and disconnect them
 *
 ***************************************************************************/
void CAefTcpServer::RemoveAllClients()
{
    CLIENTMAP::iterator it;

    // Close all of the remaining client sockets
    pthread_mutex_lock(&m_semClientMap);
    // Loop through all of the clients
    for (it = m_ClientMap.begin(); it != m_ClientMap.end(); it++)
    {
        // Close the socket
        close((*it).first);
        FD_CLR((*it).first, &m_setSelect);
        delete (*it).second;

        // Tell the packet parser about a client has been removed...
        m_pPktObj->RemoveClient((*it).first);
    }

    // Clear the map
    m_ClientMap.clear();

    pthread_mutex_unlock(&m_semClientMap);
}

/**************************************************************************/

/*!
 * \brief Determine the current number of connected clients
 *
 * \return the number of connected clients
 *
 ***************************************************************************/
UNSIGNED32 CAefTcpServer::ClientGetCount()
{
    UNSIGNED32 count;

    pthread_mutex_lock(&m_semClientMap); // Lock the socket map
    count = (UNSIGNED32) m_ClientMap.size();
    pthread_mutex_unlock(&m_semClientMap); // Unlock the socket map

    return count;
}

/**************************************************************************/

/*!
 * \brief Attempt to determine the validity of the socket
 *
 * \param  socket  socket descriptor in question
 *
 * \return true or false
 *
 ***************************************************************************/
bool CAefTcpServer::IsValidSocket(AEF_TCP_CONN_ID socket)
{
    bool valid;
    CLIENTMAP::iterator it;

    pthread_mutex_lock(&m_semClientMap); // Lock the socket map
    it = m_ClientMap.find(socket); // Attempt to find in the map
    if (it != m_ClientMap.end())
    {
        valid = true;
    }
    else
    {
        valid = false;
    }
    pthread_mutex_unlock(&m_semClientMap); // Unlock the socket map

    return valid;
}


/**************************************************************************/

/*!
 * \brief Set the Term-Of-Service bits in the IP header
 *
 * \param  value  bit pattern for the TOS bits
 *
 * \return true (success) or false (failure)
 *
 ***************************************************************************/
bool CAefTcpServer::SetQos(UNSIGNED8 value)
{
    bool success = true;

    if (value != m_QosValue)
    {
        CLIENTMAP::iterator it;
        VX_TCP_CLIENT_INFO *pci;
        m_QosValue = value;
        if (m_sockListen != AEF_ERROR)
        {
            setsockopt(m_sockListen, IPPROTO_IP, IP_TOS, (char*) &m_QosValue, sizeof (m_QosValue));
        }
        pthread_mutex_lock(&m_semClientMap); // Lock the socket map
        // Loop through all of the clients
        for (it = m_ClientMap.begin(); it != m_ClientMap.end(); it++)
        {
            // Get a pointer to the overlapped object
            pci = (*it).second;
            setsockopt(pci->Socket, IPPROTO_IP, IP_TOS, (char*) &m_QosValue, sizeof (m_QosValue));
        }
        pthread_mutex_unlock(&m_semClientMap); // Unlock the socket map
    }

    return success;
}


/**************************************************************************/

/*!
 * \brief Obtain the IP Address and port of the remote client associated with the given connection identifier.
 *
 * \param  connectionID  numeric identifier for a specific client connection
 * \param  ipAdrs        recevies the IP Address of the client upon a successful return
 * \param  port          receives the remote port of the client upon a successful return
 *
 * \return AEF_SUCCESS if connection is found and address is known, AEF_ERROR otherwise.
 *
 ***************************************************************************/
AEF_STATUS CAefTcpServer::GetClientInfo(AEF_TCP_CONN_ID connectionID, UNSIGNED32& ipAdrs, UNSIGNED16& port)
{
    AEF_STATUS result = AEF_ERROR;
    CLIENTMAP::iterator it;
    VX_TCP_CLIENT_INFO* pCInfo;

    pthread_mutex_lock(&m_semClientMap); // Lock the socket map
    if ((it = m_ClientMap.find(connectionID)) != m_ClientMap.end())
    {
        pCInfo = (*it).second;
        if (pCInfo->IPAddress != 0)
        {
            ipAdrs = pCInfo->IPAddress;
            port = pCInfo->Port;
            result = AEF_SUCCESS;
        }
    }
    pthread_mutex_unlock(&m_semClientMap); // Unlock the socket map
    return result;
}

/**************************************************************************/

/*!
 *  \brief  Allow caller from a different thread to close a connection.
 *
 *  Note that the connection is not destroyed immediately.  Caller will
 *  receive the disconnect message "event" when the connection is actually
 *  closed.
 *
 *  \param  connectionID  identifier for the connection to be removeed
 *
 *  \return  AEF_SUCCESS is connection is sucessfully flagged for closing,
 *           AEF_ERROR if connection couldn't be found.
 ***************************************************************************/
AEF_STATUS CAefTcpServer::CloseConnection(AEF_TCP_CONN_ID connectionID)
{
    AEF_STATUS result = AEF_ERROR;
    CLIENTMAP::iterator it;
    VX_TCP_CLIENT_INFO *pci;

    pthread_mutex_lock(&m_semClientMap); // Lock the socket map

    if ((it = m_ClientMap.find(connectionID)) != m_ClientMap.end())
    {
        pci = (VX_TCP_CLIENT_INFO *) (*it).second; // Get a pointer to the object

        // simply flag the client connection to be deleted at the next
        // opportunity
        pci->bDelete = 1;
        result = AEF_SUCCESS;
    }

    pthread_mutex_unlock(&m_semClientMap); // Unlock the socket map

    return result;
}

/**************************************************************************/
/*!
*   \brief Convert a millisecond value into a tick count
*
*   \return The number "ticks" equal to the number of milliseconds passed in
*
***************************************************************************/
UNSIGNED32 CAefTcpServer::TicksFromMs(UNSIGNED32 ms)
{
    UNSIGNED32 nTps = (ms/1000) * m_ntickspersec;
    return nTps;
}

/**************************************************************************/
/*!
*   \brief Convert a millisecond value into a tick count
*
*   \return The number "ticks" equal to the number of milliseconds passed in
*
***************************************************************************/
UNSIGNED32 CAefTcpServer::TicksFromTOD()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    UNSIGNED32 ms = (tv.tv_usec/1000);
    ms += (tv.tv_sec * 1000);
    return TicksFromMs(ms);
}
