/***************************************************************************/
/*!
**	\file  aefTaskMessageInterface.h
**	\brief Defines the TaskMessage Interface.
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

#define	MAX_TASK_MESSAGE_LENGTH	sizeof(CAefTaskMessage)
#define	MAX_TASK_MESSAGE_DATA_LENGTH	32

#define	MAX_MQ_QUEUE_MESSAGES       10
#define	MAX_MQ_QUEUE_MESSAGE_SIZE   sizeof(CAefTaskMessage)

#define	TASK_MESSAGE_DATA_TYPE_LOCAL	0	// data is contained within the data array
#define	TASK_MESSAGE_DATA_TYPE_PTR		1	// data array contains a pointer to data that doesn't need to be freed
#define	TASK_MESSAGE_DATA_TYPE_ALLOCPTR	2	// data array contains a pointer to data that should be freed

// Message IDs 0 - 99 will be reserved for messages that are common across
// AEF tasks.
enum AEF_MESSAGE_ENUMS 
{
	AEF_TM_TIMER = 0,
}; 

// Task-specific message IDs should increment from this number upward...
#define TM_AEF_BASE_ID	100

typedef struct {
	unsigned int			Id;
	unsigned int			Length;
	unsigned char			DataType;
} TASK_MESSAGE_HEADER;

typedef struct {
	TASK_MESSAGE_HEADER		Header;
    unsigned char			Data[MAX_TASK_MESSAGE_DATA_LENGTH];
} TASK_MESSAGE_STRUCT;

/*!
*	\class CAefTaskMessage
*	\brief Class that implements messages that are sent to/from Tasks. 
*	
*	The TaskMessage object is created by application tasks to notify 
*	other tasks (or possibly itself) of a particular event. The TaskMessage
*	object includes an ID that identify the message and a data pointer to 
*	more information. 
*
*	The TaskMessage must be freed by the receiving task as well as the 
*	additional data pointer if not NULL.
*
*/
class aefTaskMessageInterface
{
public:
    virtual void SetId(int id) = 0;
    virtual int GetId() = 0;
    virtual unsigned char* GetData(unsigned int* pDataLen = NULL) = 0;
    virtual AEF_STATUS GetData(unsigned char* pBuf, unsigned int bufLen, unsigned int* pDataLen = NULL) = 0;
    virtual AEF_STATUS SetDataPtr(unsigned char *pData, unsigned int dataLen, bool bFreeDataPtr = false) = 0;
    virtual AEF_STATUS SetData(unsigned char *pMsgBuf, unsigned int bufLen) = 0;
    virtual void Cleanup() = 0;
};

