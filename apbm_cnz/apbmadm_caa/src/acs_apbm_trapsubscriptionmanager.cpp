/*
 * acs_apbm_trapsubscriptionmanager.cpp
 *
 *  Created on: Sep 30, 2011
 *      Author: xassore
 */
#include <ace/Guard_T.h>

#include "acs_apbm_programmacros.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_trapmessage.h"
#include "acs_apbm_boardsessionhandler.h"
#include "acs_apbm_trapsubscriptionmanager.h"


trapdata & trapdata::operator= (const trapdata & from_trapdata)
	    {
	        if (this != & from_trapdata) {
	           // copy the elements
	        	this->trapType = from_trapdata.trapType;
	        	this->message_len = from_trapdata.message_len;
	        	strncpy(this->message, from_trapdata.message, from_trapdata.message_len);
	        	 this->values = from_trapdata.values;
	        }
	        return *this;
	    }


int __CLASS_NAME__::addSubscriber(int32_t slotmask, acs_apbm_boardsessionhandler *pSessionHandler)
	{
	  int slot_num;

	  if(pSessionHandler == NULL){
	  		ACS_APBM_LOG(LOG_LEVEL_ERROR, "NULL value passed for pSessionHandler !!!)");
	  		return TRAPSUB_SUBSCRIBER_ADD_ERR;
	  }
	  ACE_Guard<ACE_Recursive_Thread_Mutex> guard (this->_trapSubscription_mutex);

	  for(slot_num=0; slot_num < ACS_APBM_MAX_NUM_SLOT; slot_num++){
		if(slot_num == 27)
		{
			slotmask>>=1;
			slot_num++;
		}
		if(IS_BIT_1(slotmask) != 0){
			_boardtrapSubscriberList[slot_num].push_back(pSessionHandler);
			_boardtrapSubscriberList[slot_num].unique();
		}
		slotmask>>=1;
	  }
	  ACS_APBM_LOG(LOG_LEVEL_INFO, "Subscription request completed successfully (pSessionHandler == %p)!)", pSessionHandler);
	  return TRAPSUB_SUCCESS;
}


int __CLASS_NAME__::removeSubscriber(acs_apbm_boardsessionhandler *pSessionHandler)
	{

	  if(pSessionHandler == NULL){
		  ACS_APBM_LOG(LOG_LEVEL_ERROR, "NULL value passed for pSessionHandler !!!)");
	  		return TRAPSUB_SUBSCRIBER_ADD_ERR;
	  }
	  ACE_Guard<ACE_Recursive_Thread_Mutex> guard (this->_trapSubscription_mutex);

	  for(int slotId=0; slotId < ACS_APBM_MAX_NUM_SLOT; slotId++){
	
		if(slotId == 27) continue;
		for(TrapSubscriberList::iterator it = _boardtrapSubscriberList[slotId].begin(); it != _boardtrapSubscriberList[slotId].end(); ++it) {
	          if(*it == pSessionHandler){
                     _boardtrapSubscriberList[slotId].erase(it);
                     ACS_APBM_LOG(LOG_LEVEL_INFO, "Subscription removed (pSessionHandler == %p ,  slot == %d)!",pSessionHandler, slotId);
                     break;
	          }
	      }
	  }

	  return TRAPSUB_SUCCESS;
	}

	// this methos are used to store trap info received from SNMP agent
int __CLASS_NAME__::notifyTrapInfo(long  slotId, trapdata &trapMsg)
	{
		if ((slotId < 0) || (slotId >= ACS_APBM_MAX_NUM_SLOT) || (slotId == 27) ){
			ACS_APBM_LOG(LOG_LEVEL_ERROR, " bad parameter value ! (slotId == %ld)",slotId );
			return	TRAPSUB_BAD_PARAMETER_VALUE;
		}
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard (this->_trapSubscription_mutex);

		// add the newtrapinfo reference to all subscriber
		for (TrapSubscriberList::iterator it = _boardtrapSubscriberList[slotId].begin();
				                                                           it != _boardtrapSubscriberList[slotId].end(); ++it){

				// send Notification to   psessionHandler =
			 ACS_APBM_LOG(LOG_LEVEL_INFO, "Trap notification sent to sessionHandler %p ", *it);
			 (*it)->notifyTrap(trapMsg);
		}

		return TRAPSUB_SUCCESS;
	}

/*
 * acs_apbm_trapsubscription1.cpp
 *
 *  Created on: Oct 5, 2011
 *      Author: xassore
 */

