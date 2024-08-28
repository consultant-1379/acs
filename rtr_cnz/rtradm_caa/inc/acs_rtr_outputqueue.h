/*=================================================================== */
/**
   @file  acs_rtr_outputqueue.h 

   @brief Header file for rtr module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       24/01/2013   XHARBAV   Initial Release
==================================================================== */

#ifndef _ACS_RTR_OUTPUTQUEUE_H_
#define _ACS_RTR_OUTPUTQUEUE_H_

#include "acs_rtr_gohtransmitinfo.h"
#include <ace/Recursive_Thread_Mutex.h>
#include <list>


/**
   @brief     ACS_RTR_OutputQueue
 */
class ACS_RTR_OutputQueue {

public:

	/**
	 * @brief     Constructor for ACS_RTR_OutputQueue class.
	 * @param     createEvent
	 */
	ACS_RTR_OutputQueue(bool createEvent = true);

	virtual ~ACS_RTR_OutputQueue();
	/**
	 * @brief     getFirst
	 * @return    ACS_RTR_GohTransmitInfo
	 */
	ACS_RTR_GohTransmitInfo* getFirst();

	/**
	 * @brief     popData
	 * @return    void
	 */
	void popData();

	/**
	 * @brief     pushData
	 * @param     info
	 * @return    bool
	 */
	bool pushData(ACS_RTR_GohTransmitInfo* info);

	/**
	 * @brief     pushCpIdInfo
	 * @param     info
	 * @return    void
	 */
	void pushCpIdInfo(ACS_RTR_GohTransmitInfo* info);

	/**
	 * @brief     removeCpIdInfo
	 * @param     id
	 * @return    void
	 */
	void removeCpIdInfo(short id);

	/**
	 * @brief     queueEmpty
	 * @return    bool
	 */
	bool queueEmpty();

	/**
	 * @brief     queueSize
	 * @return    size_t
	 */
	size_t queueSize();

	/**
	 * @brief     deleteItems
	 * @param     numOfItems
	 * @return    unsigned long long
	 */
	unsigned long long deleteItems(ACE_UINT32 numOfItems);

	/**
	 * @brief     deleteItemsByBno
	 * @param     lastBno
	 * @return    unsigned long long
	 */
	unsigned long long deleteItemsByBno(unsigned lastBno);

	/**
	 * @brief     getItem
	 * @param     index
	 * @return    ACS_RTR_GohTransmitInfo
	 */
	ACS_RTR_GohTransmitInfo* getItem(int index);

	/**
	 * @brief     getHandle
	 * @return    ACE_HANDLE
	 */
	inline ACE_HANDLE getHandle() { return _queueHandle; }

private:
	/**
	 * @brief     _queueHandle
	 */
	ACE_HANDLE _queueHandle;

	/**
	 * @brief     maxSize
	 */
	unsigned int maxSize;

	/**
	 * @brief     _queueMutex
	 */
	ACE_Recursive_Thread_Mutex _queueMutex;

	/**
	 * @brief     _qInfo
	 */
	std::list<ACS_RTR_GohTransmitInfo*> _qInfo;
};

#endif // _ACS_RTR_OUTPUTQUEUE_H_
