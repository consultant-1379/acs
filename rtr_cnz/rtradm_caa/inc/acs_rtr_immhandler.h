/*
 * * @file acs_rtr_immhandler.h
 *	@brief
 *	Header file for ACS_RTR_ImmHandler class.
 *  This module contains the declaration of the class ACS_RTR_ImmHandler.
 *
 *	@author qvincon (Vincenzo Conforti)
 *	@date 2013-12-03
 *	@version 1.0.0
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 1.0.0  | 2013-12-03 | qvincon      | File created.                       |
 *	+========+============+==============+=====================================+
 */

/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_RTR_IMMHANDLER_H_
#define ACS_RTR_IMMHANDLER_H_

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include "ace/Task.h"

#include <string>

/*===================================================================
                        CLASS FORWARD DECLARATION SECTION
=================================================================== */
class ACS_RTR_Server;
class ACS_RTR_Statistics_ObjectImpl;
class ACS_RTR_CpStatistics_ObjectImpl;
class ACS_RTR_FileBased_ObjectImpl;
class ACS_RTR_HashKey_ObjectImpl;
class ACS_RTR_BlockBased_ObjectImpl;
class ACE_Reactor;

/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */

class ACS_RTR_ImmHandler: public ACE_Task_Base
{
  public:

		/**
			@brief	Constructor of ACS_RTR_ImmHandler class
		*/
		ACS_RTR_ImmHandler(ACS_RTR_Server* rtrServer);

		/**
			@brief	Destructor of ACS_RTR_ImmHandler class
		*/
		virtual ~ACS_RTR_ImmHandler();

		/**
		   	@brief 	This method initializes a task and prepare it for execution
		*/
		virtual int open(void *args = 0);

		/**
		   @brief  	Run by a daemon thread
		*/
		virtual int svc();

		/**
		   @brief 	This method signal the svc thread termination
		*/
		void stopImmHandler();

  private:

		/**
		   @brief  	This method register the OIs
		*/
		bool registerImmOI();

		/**
		   @brief  	This method creates and register the File Base OI
		*/
		bool setFileBaseOI();

		/**
		   @brief  	This method unregister and delete the File Base OI
		*/
		void removeFileBaseOI();

		/**
		   @brief  	This method creates and register the Hash Key OI
		*/
		bool setHashKeyOI();

		/**
		   @brief  	This method unregister and delete the Hash Key OI
		*/
		void removeHashKeyOI();

		/**
		   @brief  	This method creates and register the Block Base OI
		*/
		bool setBlockBaseOI();

		/**
		   @brief  	This method unregister and delete the Block Base OI
		*/
		void removeBlockBaseOI();

		/**
		   @brief  	This method creates and register the StatisticsInfo OI
		*/
		bool setStatisticsInfoOI();

		/**
		   @brief  	This method unregister and delete the StatisticsInfo OI
		*/
		void removeStatisticsInfoOI();

		/**
		   @brief  	This method creates and register the CpStatisticsInfo OI
		*/
		bool setCpStatisticsInfoOI();

		/**
		   @brief  	This method unregister and delete the CpStatisticsInfo OI
		*/
		void removeCpStatisticsInfoOI();

		/**
		   @brief   m_oi_BlockBasedJob
		*/
		ACS_RTR_Server* m_RTRServerObj;

		/**
		   @brief   m_oi_FileBasedJob
		*/
		ACS_RTR_FileBased_ObjectImpl* m_oi_FileBasedJob;
		
		/**
		   @brief   m_oi_Hashkey
		*/
		ACS_RTR_HashKey_ObjectImpl* m_oi_Hashkey;

		/**
		   @brief   m_oi_BlockBasedJob
		*/
		ACS_RTR_BlockBased_ObjectImpl* m_oi_BlockBasedJob;

		/**
		   @brief   m_oi_BlockBasedJob
		*/
		ACS_RTR_Statistics_ObjectImpl* m_oi_StatisticInfo;

		/**
		   @brief   m_oi_BlockBasedJob
		*/
		ACS_RTR_CpStatistics_ObjectImpl* m_oi_CpStatisticInfo;

		/**
			@brief  m_ImmReactor
		*/
		ACE_Reactor* m_ImmReactor;

		/**
			@brief  m_StopRequest
		*/
		bool m_StopRequest;
};

#endif /* ACS_RTR_IMMHANDLER_H_ */
