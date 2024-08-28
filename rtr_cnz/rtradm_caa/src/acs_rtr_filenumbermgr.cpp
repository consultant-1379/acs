//********************************************************
//
//	COPYRIGHT Ericsson AB 2014.
//	All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson AB.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson AB or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//********************************************************

#include "acs_rtr_filenumbermgr.h"

#include "acs_rtr_macros.h"
#include "acs_rtr_tracer.h"

#include "ace/ACE.h"
#include "ace/Task.h"


ACS_RTR_TRACE_DEFINE(ACS_RTR_FILE_NUMBER_MGR)

//--------
// addNode
//--------
void ACS_RTR_FileNumberMgrImpl::addNode(const ACE_TCHAR* msgStore, unsigned short cpID)
{
	ACS_RTR_TRACE_FUNCTION;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_criticalSection);
	//check if the node already exists
	bool found = false;
	std::list<fileNum_t*>::iterator it;
	for (it = _fileNumberMap.begin(); it != _fileNumberMap.end(); it++)
	{
		fileNum_t *node = (*it);
		if ((node->cpID == cpID) && (strcmp(node->msname, msgStore) == 0))
		{
			found = true;
			break;
		}
	}

	if (!found)
	{
		//add new node
		fileNum_t *node = new fileNum_t();
		node->cpID = cpID;
		strncpy(node->msname, msgStore, MSGSTORE_SIZE - 1);
		node->fileNumber = 0;

		_fileNumberMap.push_back(node);
	}
}

//-------------------------------------------------
// get
// give the next sequence number for a new BLD file
//-------------------------------------------------
unsigned int ACS_RTR_FileNumberMgrImpl::get(const ACE_TCHAR* msgStore, unsigned short cpID)
{
	unsigned int num = 0;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_criticalSection);
	std::list<fileNum_t*>::iterator it;
	for (it = _fileNumberMap.begin(); it != _fileNumberMap.end(); it++)	{
		fileNum_t *node = (*it);
		if ((node->cpID == cpID) && (strcmp(node->msname, msgStore) == 0)) {
			(node->fileNumber)++;
			//Changes for the TR HP41899
			if (node->fileNumber > MAX_FILE_NUM) 
			{
				node->fileNumber = MIN_FILE_NUM;
				(node->fileNumber)++;
			}
			//Changes for the TR HP41899
			num = node->fileNumber;
			break;
		}
	}
	return num;
}

//---------
// decrease
//---------
void ACS_RTR_FileNumberMgrImpl::decrease(const ACE_TCHAR* msgStore, unsigned short cpID)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_criticalSection);
	std::list<fileNum_t*>::iterator it;
	for (it = _fileNumberMap.begin(); it != _fileNumberMap.end(); it++)
	{
		fileNum_t *node = (*it);
		if ((node->cpID == cpID) && (strcmp(node->msname, msgStore) == 0))
		{
			if ((node->fileNumber) > MIN_FILE_NUM) (node->fileNumber)--;
			break;
		}
	}
}

//--------------------------------------------------------------------------------
// setLCF
// initialize the file sequence number according to the last committed file on ACA
//--------------------------------------------------------------------------------
void ACS_RTR_FileNumberMgrImpl::setLCF(ACE_UINT32 lcf, const ACE_TCHAR* msgStore, unsigned short cpID)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_criticalSection);

	std::list<fileNum_t*>::iterator it;
	for (it = _fileNumberMap.begin(); it != _fileNumberMap.end(); it++)
	{
		fileNum_t *node = (*it);
		if ((node->cpID == cpID) && (strcmp(node->msname, msgStore) == 0))
		{
			if (lcf > MAX_FILE_NUM) lcf = MIN_FILE_NUM;
			node->fileNumber = lcf;
			break;
		}
	}
}

