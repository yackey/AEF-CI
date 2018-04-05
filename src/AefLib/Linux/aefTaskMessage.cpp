/***************************************************************************/
/*!
**	\file  aefTaskMessage.cpp
**	\brief Implements the TaskMessage object. 
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
#include "aefTypes.h"
#include "aefTaskMessage.h"

/*************************************************************************/
/*!
*  \brief  Constructor for the CAefTaskMessage class
*
*  \param  id  message identifier (defaults to 0)
*
***************************************************************************/
CAefTaskMessage::CAefTaskMessage(int id /* = 0 */)
{
	memset(&m_message, 0, sizeof(m_message));
	m_message.Header.Id = id;
	m_message.Header.DataType = TASK_MESSAGE_DATA_TYPE_LOCAL;
}

/*************************************************************************/
/*!
*	\brief	CTask Message destructor
*
*
***************************************************************************/
CAefTaskMessage::~CAefTaskMessage() 
{

}

/*************************************************************************/
/*!
*	\brief	Sets the message id for this instance.
*
*
***************************************************************************/
void CAefTaskMessage::SetId(int id) 
{
	m_message.Header.Id = id;
}

/*************************************************************************/
/*!
*	\brief	Gets the message id for this instance.
*
*	\return The message ID
***************************************************************************/
int CAefTaskMessage::GetId()
{
	return (m_message.Header.Id);
}

/*************************************************************************/
/*!
*	\brief	Copies data into the message structure
*
*	\param	pMsgBuf	Data buffer
*	\param	bufLen	Number of bytes in data buffer
*
*	\return	AEF_SUCCESS or AEF_ERROR
***************************************************************************/
AEF_STATUS CAefTaskMessage::SetData(unsigned char *pMsgBuf, unsigned int bufLen)
{
	AEF_STATUS	result = AEF_ERROR;
	
	if (bufLen <= MAX_TASK_MESSAGE_DATA_LENGTH) {
		memcpy(&m_message.Data[0], pMsgBuf, bufLen);
		m_message.Header.Length = bufLen;
		m_message.Header.DataType = TASK_MESSAGE_DATA_TYPE_LOCAL;
		result = AEF_SUCCESS;
	}
	else
	{
		uint8_t*	pDataBuf = new uint8_t[bufLen];

		if (pDataBuf != NULL) {
			memcpy(pDataBuf, pMsgBuf, bufLen);
			if ((result = SetDataPtr(pDataBuf, bufLen, true)) != AEF_SUCCESS) {
				delete [] pDataBuf;
			}
		}
	}
	return result;
}

/*************************************************************************/
/*!
*	\brief	Stores a data pointer instead of actual data
*
*	\param	pData			Data buffer
*	\param	dataLen			Number of bytes in data buffer
*	\param	bFreeDataPtr	Indicates if the class is responsible for freeing this memory
*
*	\return	AEF_SUCCESS
***************************************************************************/
AEF_STATUS CAefTaskMessage::SetDataPtr(unsigned char *pData, unsigned int dataLen, bool bFreeDataPtr /* false */)
{
	AEF_STATUS	result = AEF_SUCCESS;

	memcpy(&m_message.Data[0], &pData, sizeof(void*));
	m_message.Header.Length = dataLen;
	if (bFreeDataPtr == true) {
		m_message.Header.DataType = TASK_MESSAGE_DATA_TYPE_ALLOCPTR;
	}
	else {
		m_message.Header.DataType = TASK_MESSAGE_DATA_TYPE_PTR;
	}

	return result;
}

/*************************************************************************/
/*!
*	\brief	Gets a pointer to the data for this message instance.
*
*	\param	pDataLen	If non-NULL, the data length is returned in this field
*
*	\return A pointer to the message data
***************************************************************************/
unsigned char* CAefTaskMessage::GetData(unsigned int* pDataLen /* = NULL */)
{
	unsigned char *pData = NULL;

	if (m_message.Header.Length != 0) {
		if (m_message.Header.DataType == TASK_MESSAGE_DATA_TYPE_LOCAL) {
			pData = &m_message.Data[0];
		}
		else {
			pData = *(unsigned char**)&m_message.Data[0];
		}
		
		if (pDataLen != NULL) {
			*pDataLen = m_message.Header.Length;
		}
	}

	return(pData);
}


/*************************************************************************/
/*!
*  \brief  Copies message data into a buffer supplied by the caller.
*
*  \param  pBuf      pointer to allocated data buffer
*  \param  bufLen    length of buffer pointed to by pBuf
*  \param  pDataLen  if non-NULL, actual data length will be returned to the caller upon successful execution
*
*  \return AEF_SUCCESS if data could be copied into the buffer, AEF_ERROR otherwise
***************************************************************************/
AEF_STATUS CAefTaskMessage::GetData(unsigned char* pBuf, unsigned int bufLen,  unsigned int* pDataLen /* = NULL */)
{
	AEF_STATUS		result = AEF_ERROR;
	unsigned char*	pData;

	if ((m_message.Header.Length != 0) && (m_message.Header.Length <= bufLen))  {
		if (m_message.Header.DataType == TASK_MESSAGE_DATA_TYPE_LOCAL) {
			pData = &m_message.Data[0];
		}
		else {
			pData = *(unsigned char**)&m_message.Data[0];
		}
		memcpy(pBuf, pData, m_message.Header.Length);

		if (pDataLen != NULL) {
			*pDataLen = m_message.Header.Length;
		}
		result = AEF_SUCCESS;
	}

	return result;
}

/*************************************************************************/
/*!
*	\brief	If class instance holds a data pointer that needs to be freed, it is freed in this method.
*
***************************************************************************/
void CAefTaskMessage::Cleanup()
{
	if (m_message.Header.DataType == TASK_MESSAGE_DATA_TYPE_ALLOCPTR) {
		unsigned char *pData;

		pData = *(unsigned char**)&m_message.Data[0];
		delete [] pData;
		m_message.Header.Length = 0;
	}
}
