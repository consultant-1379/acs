#ifndef ACS_CS_BRFHANDLER_H_
#define ACS_CS_BRFHANDLER_H_
/*
 * ACS_CS_BrfHandler.h
 *
 *  Created on: APR 23, 2014
 *      Author: tcspada
 */



#include "ACS_CS_BrfPersistentDataOwner.h" 
#include "acs_apgcc_objectimplementerinterface_V3.h"
#include "acs_apgcc_paramhandling.h"
#include "ACS_CC_Types.h"
#include "acs_apgcc_adminoperationtypes.h"
#include "acs_apgcc_omhandler.h"
#include "acs_apgcc_adminoperation.h"
#include "ACS_APGCC_Util.H"
#include "ACS_TRA_Logging.h"
#include "ACS_CS_IMMCallbackThread.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImRepository.h"
#include <acs_apgcc_oihandler_V3.h>
#include "ACS_CS_NEHandler.h"
#include <iostream>
#include <ace/Task.h>
#include "ACS_CS_BrfSubscriber.h"

class ACS_CS_BrfPersistentDataOwner;
/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */
class ACS_CS_BrfHandler: public ACE_Task_Base {
 public:

	void initialize(ACS_CS_NEHandler* neHandler);
	void finalize ();

	virtual int svc(void);
	/**
	   @brief  		This method initializes a task and prepare it for execution
	*/
	virtual int open (void *args = 0);

	/**
	   @brief  		This method get the stop handle to terminate the svc thread
	*/

	bool setBrfImplementer();

	virtual void stop ();

	static  ACS_CS_BrfHandler* getInstance ();

	int createBrmPersistentDataOwnerObject();

	int checkBrmPersistentDataOwnerObject();

	void setBackupOngoing(bool backup);

     	bool isBackupOngoing();

	bool isSwitchOngoing();
	bool getSvcState() const {return m_svc_run;};	
	void setBrfImplementerState(bool state){isBrfImplementerSet = state;};
        
 private:

	ACS_CS_BrfHandler();


	virtual ~ACS_CS_BrfHandler();
	
	static ACS_CS_BrfHandler* m_instance;
	
	acs_apgcc_oihandler_V3 m_oiHandlerBrfPersistentDataOwner;

	ACS_CS_BrfPersistentDataOwner * m_oi_BrfPersistentDataOwner;

	//ACS_CS_EventHandle m_StopEvent;

	ACS_CS_EventHandle m_ShutdownEvent;	

	bool m_backup;	
	/**
	   @brief  	m_svc_run: svc state flag
	*/
	bool m_svc_run;
	
	bool isBrfImplementerSet;	
	
	ACS_CS_NEHandler* m_neHandler;
};

#endif /* ACS_CS_BRFHANDLER_H_ */


