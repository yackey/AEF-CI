/****************************************************************************/
/*!
 **  \file  aefLinuxSerial.cpp
 **  \brief VxWorks Serial Transmission class
 **
 **  The routines in this file implement a class/task to handle communication
 **  over a single serial device.  A task will be created to receive data from
 **  the serial device.
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
#include "aefSerialBase.h"

/**************************************************************************/

/*!
 *     \brief      Constructor for CAefSerialBase class.
 *
 *     Default constructor.
 *
 ***************************************************************************/
CAefSerialBase::CAefSerialBase()
{
}

/**************************************************************************/

/*!
 *     \brief      Destructor for CAefSerialBase class.
 *
 *     Makes sure that the task associated with this CAefSerialBase instance is
 *     stopped, then frees any resources allocated by the class.
 *
 ***************************************************************************/
CAefSerialBase::~CAefSerialBase()
{
}

///**************************************************************************/
//
///*!
// * \brief Class Configuration
// *
// * Configures the serial class parameters
// *
// * \param  pDevName     Serial device name to use
// * \param  pTask        pointer to class that should be notified of events
// * \param  dataRcvdId   message Id used to indicate that a new message has been received
// * \param  shutdownId   message Id used to indicate that the Serial task has unexpectedly shut down
// * \param  pPktObj      pointer to a CPacket-derived class that parses/builds the message protocol
// * \param  bufSize      [UNSIGNED32] Maximum number of bytes that can be sent to pClientObj at once
// * \param  timeout      [int] If non-zero, indicates time (in ms) after which no communication will result in shutdown
// * \param  keepalive    [int] Time (in ms) to make sure that a message has been sent to the remote side.
// *
// * \return AEF_SUCCESS or AEF_ERROR
// *
// ***************************************************************************/
//AEF_STATUS CAefSerialBase::Configure(const char* pDevName,
//                                 CAefQueueTask* pTask, UNSIGNED32 dataRcvdId, UNSIGNED32 shutdownId,
//                                 CPacket* pPktObj, UNSIGNED32 bufSize, int timeout,
//                                 int keepalive)
//{
//    AEF_STATUS result = AEF_ERROR;
//
//    // Check if we are already initialized
//    if (m_bConfigured == false)
//    {
//        // device-related initialization
//        m_pDevName = pDevName;
//
//        // message-related initialization
//        m_pParent = pTask;
//        m_ShutdownId = shutdownId;
//        m_DataRcvdId = dataRcvdId;
//        m_pPktObj = pPktObj;
//        m_BufSize = bufSize;
//        SetKeepalive(timeout, keepalive);
//
//        m_fdSerial = AEF_ERROR;
//
//        m_pBuf = new UNSIGNED8[bufSize];
//
//        if ((m_pBuf != NULL) && (m_pParent != NULL))
//        {
//            result = AEF_SUCCESS;
//            m_bConfigured = true;
//        }
//        else if (m_pBuf != NULL)
//        {
//            delete [] m_pBuf;
//            m_pBuf = NULL;
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
//void CAefSerialBase::SetKeepalive(int timeout, int keepalive)
//{
//    m_ClientTimeout = MsToTimeTicks(timeout);
//    m_KeepAliveTimeout = MsToTimeTicks(keepalive);
//}
//
///**************************************************************************/
//
///*!
// * \brief Transmits a message over the serial device
// *
// * \param  pData    Data buffer to send
// * \param  dataLen  Number of bytes in pData
// *
// * \return AEF_SUCCESS or AEF_ERROR
// *
// ***************************************************************************/
//AEF_STATUS CAefSerialBase::Send(UNSIGNED8 *pData, UNSIGNED32 dataLen)
//{
//    AEF_STATUS result = AEF_ERROR;
//    UNSIGNED8* pPkt;
//    UNSIGNED32 pktLen;
//    struct timespec ts;
//
//    pPkt = m_pPktObj->Build(pData, dataLen, &pktLen);
//
//    if (pPkt != NULL)
//    {
//        // Make sure we are initialized
//        ts.tv_nsec = 0;
//        ts.tv_sec = 1;
//        int nRet = sem_timedwait(&m_semDev, &ts);
//        if (nRet == AEF_SUCCESS)
//        {
//            if (m_fdSerial != AEF_ERROR)
//            {
//                if (write(m_fdSerial, (char*) pPkt, pktLen) == (int) pktLen)
//                {
//                    result = AEF_SUCCESS;
//                    m_LastSend = TicksFromTOD();
//                }
//            }
//            sem_post(&m_semDev);
//        }
//
//        delete[] pPkt;
//    }
//
//    return result;
//}
//
///**************************************************************************/
//
///*!
// * \brief Perform task cleanup just before task is about to exit
// *
// * \return AEF_SUCCESS or AEF_ERROR
// *
// ***************************************************************************/
//AEF_STATUS CAefSerialBase::Exit(AEF_STATUS exitCode)
//{
//    // close the device
//    sem_wait(&m_semDev);
//    close(m_fdSerial);
//    m_fdSerial = AEF_ERROR;
//    sem_post(&m_semDev);
//
//    // Reset the variables
//    m_bConfigured = false;
//
//    if (m_pBuf != NULL)
//    {
//        delete [] m_pBuf;
//        m_pBuf = NULL;
//    }
//
//    if (m_exitReason != 0)
//    {
//        // we had an asynchronous failure, so alert the client
//        CAefTaskMessage msg(m_ShutdownId);
//
//        m_pParent->SendMessage(&msg);
//    }
//
//    return CAefTask::Exit(exitCode);
//}
//
///**************************************************************************/
//
///*!
// * \brief Check to see if communcation from far end has stopped.
// *
// ***************************************************************************/
//void CAefSerialBase::ReceiveTimeoutCheck(void)
//{
//    UNSIGNED32 curticks;
//
//    if ((m_bConfigured == true) && m_ClientTimeout)
//    {
//        curticks = TicksFromTOD();
//
//        if (curticks - m_LastRcvd > m_ClientTimeout)
//        {
//            // close the serial port down?
//            m_bTaskRunning = false;
//            m_exitReason = 1; // timeout
//        }
//    }
//}
//
///**************************************************************************/
//
///*!
// * \brief Determine if a keepalive message should be sent
// *
// ***************************************************************************/
//void CAefSerialBase::KeepAliveCheck(void)
//{
//    UNSIGNED8 *pData;
//    UNSIGNED32 dataLen;
//
//    if (m_KeepAliveTimeout)
//    {
//        if (((TicksFromTOD() - m_LastSend) > m_KeepAliveTimeout) &&
//                (m_pPktObj->GetKeepaliveMessage(&pData, &dataLen) == true))
//        {
//            Send(pData, dataLen);
//        }
//    }
//}
