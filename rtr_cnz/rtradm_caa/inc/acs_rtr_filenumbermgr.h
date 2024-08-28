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

#ifndef ACS_RTR_FILENUMBERMGR_H
#define ACS_RTR_FILENUMBERMGR_H

#include <stdio.h>
#include <stdlib.h>
#include <list>

#include <ace/RW_Thread_Mutex.h>
#include <ace/Singleton.h>
#include "ace/Recursive_Thread_Mutex.h"

using namespace std;

#define MSGSTORE_SIZE	16
#define MAX_FILE_NUM 9999
#define MIN_FILE_NUM 0

/**
 * @brief     ACS_RTR_FileNumberMgr
 */
class ACS_RTR_FileNumberMgrImpl
{
	friend class ACE_Singleton<ACS_RTR_FileNumberMgrImpl, ACE_Recursive_Thread_Mutex>;

	/**
	 * @brief     fileNum_t
	 */
	typedef struct
	{
		ACE_TCHAR msname[MSGSTORE_SIZE];
		unsigned short	cpID;
		ACE_UINT32 fileNumber;
	} fileNum_t;

public:

	ACE_UINT32 get(const ACE_TCHAR* msgStore, unsigned short cpID = ~0);
	void decrease(const ACE_TCHAR* msgStore, unsigned short cpID = ~0);
	void setLCF(ACE_UINT32 lcf, const ACE_TCHAR* msgStore, unsigned short cpID = ~0);
	void addNode(const ACE_TCHAR* msgStore, unsigned short cpID = ~0);

private:

	inline ACS_RTR_FileNumberMgrImpl() : _criticalSection() {}
	inline virtual ~ACS_RTR_FileNumberMgrImpl() {}

	std::list<fileNum_t*> _fileNumberMap;
	ACE_Recursive_Thread_Mutex _criticalSection;
};

typedef ACE_Singleton<ACS_RTR_FileNumberMgrImpl, ACE_Recursive_Thread_Mutex> ACS_RTR_FileNumberMgr;

#endif
