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
#include <sys/types.h>
#include <sys/ioctl.h>
#include <map>
#include <termios.h>
#include <sys/time.h>
#include <unistd.h>

#include "aefTypesBase.h"
#include "aefMutexBase.h"
#include "aefTimerBase.h"
#include "aefTaskBase.h"
//#include "aefQueueTaskBase.h"
#include "aefPktBase.h"
#include "aefSerial.h"

/**************************************************************************/

/*!
 *     \brief      Constructor for CAefSerial class.
 *
 *     Default constructor.
 *
 ***************************************************************************/
CAefSerial::CAefSerial() : CAefTask("tSerial", 100, 0, 5000)
{
    Construct();
}


/**************************************************************************/

/*!
 * \brief  Constructor for CAefSerial class specifying task creation parameters
 *
 * \param  name      Name of task.
 * \param  priority  Priority of task.
 * \param  options   Task startup options
 * \param  stack     Task Stack size
 *
 ***************************************************************************/
CAefSerial::CAefSerial(const char *name, int priority, int options, int stack)
: CAefTask(name, priority, options, stack)
{
    Construct();
}

/**************************************************************************/

/*!
 *     \brief  Protected method called from all constructors to perform default initialization
 *
 *
 ***************************************************************************/
void CAefSerial::Construct()
{
    // Initialize the critical section object for the valid socket map
    // we want a binary semaphore
    sem_init(&m_semDev, 0 ,1);
    m_pParent = NULL; // no boss yet
    m_pBuf = NULL;
    m_bConfigured = false;
    m_fdSerial = AEF_ERROR;

    m_ClientTimeout = 0;
    m_KeepAliveTimeout = 0;

    m_baudRate = B9600;
    m_dataBits = 8;
    m_parity = 0;
    m_stopBits = 1;
    m_bPortUpdate = true;

    // TODO: not sure what to do. 8 bits isn't big enough
    // but its in the bowels of the base class
    m_sioOpts = (CS8 | CLOCAL); // 8 data bits, 1 stop bit, no parity
    
    m_ntickspersec = sysconf(_SC_CLK_TCK);
}

/**************************************************************************/

/*!
 *     \brief      Destructor for CAefSerial class.
 *
 *     Makes sure that the task associated with this CAefSerial instance is
 *     stopped, then frees any resources allocated by the class.
 *
 ***************************************************************************/
CAefSerial::~CAefSerial()
{
    // Clean up the task
    Stop();
    // delete the semaphores
    sem_destroy(&m_semDev);
}

/**************************************************************************/

/*!
 * \brief Class Configuration
 *
 * Configures the serial class parameters
 *
 * \param  pDevName     Serial device name to use
 * \param  pTask        pointer to class that should be notified of events
 * \param  dataRcvdId   message Id used to indicate that a new message has been received
 * \param  shutdownId   message Id used to indicate that the Serial task has unexpectedly shut down
 * \param  pPktObj      pointer to a CPacket-derived class that parses/builds the message protocol
 * \param  bufSize      [UNSIGNED32] Maximum number of bytes that can be sent to pClientObj at once
 * \param  timeout      [int] If non-zero, indicates time (in ms) after which no communication will result in shutdown
 * \param  keepalive    [int] Time (in ms) to make sure that a message has been sent to the remote side.
 *
 * \return AEF_SUCCESS or AEF_ERROR
 *
 ***************************************************************************/
AEF_STATUS CAefSerial::Configure(const char* pDevName,
                                 CAefQueueTask* pTask, UNSIGNED32 dataRcvdId, UNSIGNED32 shutdownId,
                                 CPacket* pPktObj, UNSIGNED32 bufSize, int timeout,
                                 int keepalive)
{
    AEF_STATUS result = AEF_ERROR;

    // Check if we are already initialized
    if (m_bConfigured == false)
    {
        // device-related initialization
        m_pDevName = pDevName;

        // message-related initialization
        m_pParent = pTask;
        m_ShutdownId = shutdownId;
        m_DataRcvdId = dataRcvdId;
        m_pPktObj = pPktObj;
        m_BufSize = bufSize;
        SetKeepalive(timeout, keepalive);

        m_fdSerial = AEF_ERROR;

        m_pBuf = new UNSIGNED8[bufSize];

        if ((m_pBuf != NULL) && (m_pParent != NULL))
        {
            result = AEF_SUCCESS;
            m_bConfigured = true;
        }
        else if (m_pBuf != NULL)
        {
            delete [] m_pBuf;
            m_pBuf = NULL;
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
void CAefSerial::SetKeepalive(int timeout, int keepalive)
{
    m_ClientTimeout = MsToTimeTicks(timeout);
    m_KeepAliveTimeout = MsToTimeTicks(keepalive);
}

/**************************************************************************/

/*!
 * \brief Updates serial port settings
 *
 * Allows dynamic updates to the connection settings for the serial connection
 *
 * \param  timeout    If non-zero, indicates time (in ms) after which no communication will result in shutdown
 * \param  keepalive  Time (in ms) to make sure that a message has been sent to the remote side.
 *
 * \return N/A
 *
 ***************************************************************************/
AEF_STATUS CAefSerial::ConfigurePort(UNSIGNED32 baudRate, UNSIGNED8 dataBits, UNSIGNED8 parity, UNSIGNED8 stopBits)
{
    bool bUpdate = false;

    if (m_baudRate != baudRate)
    {
        m_baudRate = baudRate;
        bUpdate = true;
    }

    if (m_dataBits != dataBits)
    {
        m_dataBits = dataBits;
        m_sioOpts &= ~CSIZE; // reset data size bits
        if (m_dataBits == 7)
        {
            m_sioOpts |= CS7; // 7 data bits
        }
        else
        {
            m_sioOpts |= CS8; // 8 data bits
        }
        bUpdate = true;
    }

    if (m_parity != parity)
    {
        m_parity = parity;
        m_sioOpts &= ~(PARENB | PARODD); // reset parity-related bits
        if (m_parity == 1)
        {
            m_sioOpts |= PARENB | PARODD; // odd parity
        }
        else if (m_parity == 2)
        {
            m_sioOpts |= PARENB; // even parity
        }
        bUpdate = true;
    }

    if (m_stopBits != stopBits)
    {
        m_stopBits = stopBits;
        if (m_stopBits == 2)
        {
            m_sioOpts |= CSTOPB; // 2 stop bits TODO: is this right ?
        }
        else
        {
            m_sioOpts &= ~CSTOPB; // 1 stop bit
        }
        bUpdate = true;
    }

    if (bUpdate == true)
    {
        m_bPortUpdate = true;
    }

    return AEF_SUCCESS;
}


/**************************************************************************/

/*!
 * \brief Transmits a message over the serial device
 *
 * \param  pData    Data buffer to send
 * \param  dataLen  Number of bytes in pData
 *
 * \return AEF_SUCCESS or AEF_ERROR
 *
 ***************************************************************************/
AEF_STATUS CAefSerial::Send(UNSIGNED8 *pData, UNSIGNED32 dataLen)
{
    AEF_STATUS result = AEF_ERROR;
    UNSIGNED8* pPkt;
    UNSIGNED32 pktLen;
    struct timespec ts;

    pPkt = m_pPktObj->Build(pData, dataLen, &pktLen);

    if (pPkt != NULL)
    {
        // Make sure we are initialized
        ts.tv_nsec = 0;
        ts.tv_sec = 1;
        int nRet = sem_timedwait(&m_semDev, &ts);
        if (nRet == AEF_SUCCESS)
        {
            if (m_fdSerial != AEF_ERROR)
            {
                if (write(m_fdSerial, (char*) pPkt, pktLen) == (int) pktLen)
                {
                    result = AEF_SUCCESS;
                    m_LastSend = TicksFromTOD();
                }
            }
            sem_post(&m_semDev);
        }

        delete[] pPkt;
    }

    return result;
}


/**************************************************************************/

/*!
 * \brief Virtual method Initialize the AefSerial task
 *
 * \return AEF_SUCCESS or AEF_ERROR
 *
 ***************************************************************************/
AEF_STATUS CAefSerial::Init(void *pTaskData)
{
    AEF_STATUS result = AEF_ERROR;

    if (CAefTask::Init(pTaskData) == AEF_SUCCESS)
    {
        // take device semaphore until we set up the serial port
        sem_wait(&m_semDev);

        // open the serial port
        m_fdSerial = open(m_pDevName, O_RDWR, 0);

        if (m_fdSerial != AEF_ERROR)
        {
            // TODO: this could all be fantasy
            struct termios options;
            // ioctl(m_fdSerial, FIOBAUDRATE, m_baudRate
            cfsetspeed(&options, m_baudRate);
            //ioctl(m_fdSerial, FIOSETOPTIONS, OPT_RAW
            cfmakeraw(&options);
            //ioctl(m_fdSerial, SIO_HW_OPTS_SET, m_sioOpts
            options.c_cflag = m_sioOpts;
            // ioctl(m_fdSerial, FIOFLUSH, 0
            
            if (((tcsetattr(m_fdSerial, TCSANOW, &options)) != AEF_ERROR) &&
                 ((tcflush(m_fdSerial, TCIOFLUSH)) != AEF_ERROR))
            {
                m_bPortUpdate = false;
                result = AEF_SUCCESS;
            }
            else
            {
                close(m_fdSerial);
                m_fdSerial = AEF_ERROR;
            }
        }
        sem_post(&m_semDev);
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
AEF_STATUS CAefSerial::Run()
{
    fd_set origSelSet;
    fd_set selSet;
    struct timeval selTmout;
    int selResult;
    int bytesRead;

    selTmout.tv_sec = 0;
    selTmout.tv_usec = 10000;

    m_exitReason = 0;

    FD_ZERO(&origSelSet);
    FD_SET(m_fdSerial, &origSelSet);

    m_LastRcvd = m_LastSend = TicksFromTOD();

    // modify the last send time to guarantee that we can immediately send
    // out a keepalive message (if a keepalive timeout has been configured,
    // that is)
    m_LastSend -= (m_KeepAliveTimeout * 2);

    while (m_bTaskRunning == true)
    {
        selSet = origSelSet;

        selResult = select(m_fdSerial + 1, &selSet, NULL, NULL, &selTmout);
        if (selResult != AEF_ERROR)
        {
            if (m_bPortUpdate == true)
            {
            // TODO: this could all be fantasy (times 2)
            struct termios options;
            // ioctl(m_fdSerial, FIOBAUDRATE, m_baudRate
            cfsetspeed(&options, m_baudRate);
            //ioctl(m_fdSerial, FIOSETOPTIONS, OPT_RAW
            cfmakeraw(&options);
            //ioctl(m_fdSerial, SIO_HW_OPTS_SET, m_sioOpts
            options.c_cflag = m_sioOpts;
            // ioctl(m_fdSerial, FIOFLUSH, 0
            
            if (((tcsetattr(m_fdSerial, TCSANOW, &options)) != AEF_ERROR) &&
                 ((tcflush(m_fdSerial, TCIOFLUSH)) != AEF_ERROR))                
                {
                    m_pPktObj->RestartParser(0);
                    m_LastRcvd = TicksFromTOD();
                }
                else
                {
                    m_exitReason = 2; // reconfiguration failed
                    m_bTaskRunning = false;
                }
                selResult = 0; // don't bother processing anything at this point
                m_bPortUpdate = false;
            }

            if (selResult > 0)
            {
                if (FD_ISSET(m_fdSerial, &selSet))
                {
                    if ((bytesRead = read(m_fdSerial, (char*) m_pBuf, m_BufSize)) != AEF_ERROR)
                    {
                        if (bytesRead > 0)
                        {
                            // if client says a complete and valid message has been received
                            // (i.e., client returns a "1"), update m_LastRcvd
                            if (m_pPktObj->ParseData(0, m_pBuf, bytesRead, m_pParent, m_DataRcvdId) == 1)
                            {
                                m_LastRcvd = TicksFromTOD();
                            }
                        }
                    }
                }
            }
            ReceiveTimeoutCheck();
            KeepAliveCheck();
        }
        else
        {
            // shouldn't receive AEF_ERROR here, so bail out?
            m_bTaskRunning = false;
            // send an error back?
        }
    }

    return AEF_SUCCESS;
}


/**************************************************************************/

/*!
 * \brief Perform task cleanup just before task is about to exit
 *
 * \return AEF_SUCCESS or AEF_ERROR
 *
 ***************************************************************************/
AEF_STATUS CAefSerial::Exit(AEF_STATUS exitCode)
{
    // close the device
    sem_wait(&m_semDev);
    close(m_fdSerial);
    m_fdSerial = AEF_ERROR;
    sem_post(&m_semDev);

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

        m_pParent->SendMessage(&msg);
    }

    return CAefTask::Exit(exitCode);
}

/**************************************************************************/

/*!
 * \brief Check to see if communcation from far end has stopped.
 *
 ***************************************************************************/
void CAefSerial::ReceiveTimeoutCheck(void)
{
    UNSIGNED32 curticks;

    if ((m_bConfigured == true) && m_ClientTimeout)
    {
        curticks = TicksFromTOD();

        if (curticks - m_LastRcvd > m_ClientTimeout)
        {
            // close the serial port down?
            m_bTaskRunning = false;
            m_exitReason = 1; // timeout
        }
    }
}

/**************************************************************************/

/*!
 * \brief Determine if a keepalive message should be sent
 *
 ***************************************************************************/
void CAefSerial::KeepAliveCheck(void)
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
*   \brief Convert a millisecond value into a tick count
*
*   \return The number "ticks" equal to the number of milliseconds passed in
*
***************************************************************************/
UNSIGNED32 CAefSerial::TicksFromMs(UNSIGNED32 ms)
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
UNSIGNED32 CAefSerial::TicksFromTOD()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    UNSIGNED32 ms = (tv.tv_usec/1000);
    ms += (tv.tv_sec * 1000);
    return TicksFromMs(ms);
}