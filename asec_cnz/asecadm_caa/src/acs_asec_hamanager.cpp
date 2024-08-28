#include "acs_asec_hamanager.h"

//-------------------------------------------------------------------------------------------------------------------
asecHAClass::asecHAClass(const char* daemon_name, const char* user):
	ACS_APGCC_ApplicationManager(daemon_name, user),
	m_asecObj(0),
	active(false),		
	passiveToActive(0){

	log.Open("ASECBIN");
}

//-------------------------------------------------------------------------------------------------------------------
asecHAClass::~asecHAClass(){
	// to be sure.
	this->shutdownApp(); 
	log.Close();
}

//-------------------------------------------------------------------------------------------------------------------
ACS_APGCC_ReturnType asecHAClass::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState){
        (void) previousHAState;
         if(ACS_APGCC_AMF_HA_STANDBY == previousHAState)
                passiveToActive = 1;
	return this->activateApp();
}

//-------------------------------------------------------------------------------------------------------------------
ACS_APGCC_ReturnType asecHAClass::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState) {

	(void) previousHAState;
	return this->passifyApp();
}

//-------------------------------------------------------------------------------------------------------------------
ACS_APGCC_ReturnType asecHAClass::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState) {

	(void) previousHAState;
	return this->shutdownApp();
}


//-------------------------------------------------------------------------------------------------------------------
ACS_APGCC_ReturnType asecHAClass::performComponentTerminateJobs(void){

	return this->shutdownApp();
}

//-------------------------------------------------------------------------------------------------------------------
ACS_APGCC_ReturnType asecHAClass::performComponentRemoveJobs(void){

	return this->shutdownApp();
}

//-------------------------------------------------------------------------------------------------------------------
ACS_APGCC_ReturnType asecHAClass::performApplicationShutdownJobs(){

	return this->shutdownApp();	
}

//-------------------------------------------------------------------------------------------------------------------
ACS_APGCC_ReturnType asecHAClass::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState) {

	(void) previousHAState;
	return this->shutdownApp();
}


//-------------------------------------------------------------------------------------------------------------------
ACS_APGCC_ReturnType asecHAClass::activateApp() {

        ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;
        if ( 0 != this->m_asecObj) {
		if (passiveToActive){
			log.Write("asec-ha-class: passive->active transition. stop passive work before becomming active", LOG_LEVEL_INFO);
			this->m_asecObj->stop();
			this->m_asecObj->wait();
			passiveToActive=0;
		} 
		else {
			log.Write("asec-ha-class: 'csadm-service' is already active", LOG_LEVEL_INFO);
			rCode = ACS_APGCC_SUCCESS;
		}
                
	} else {	        
        	ACE_NEW_NORETURN(this->m_asecObj, ACS_ASEC_Adm());
		if (0 == this->m_asecObj) {
			log.Write("asec-ha-class: failed to create the instance", LOG_LEVEL_ERROR);		
			syslog(LOG_ERR, "asec-ha-class: failed to create the instance");
		}
	}

	active = true;
	if ( 0 != this->m_asecObj) {
     		int res = this->m_asecObj->active(this); // This will start active functionality. Will not return until myCLass is running
	       	if (res < 0) {
	       		// Failed to start
	               	delete this->m_asecObj;
	               	this->m_asecObj = 0;
	        } else {
	                log.Write("asec-ha-class: 'csadm-service' is now activated by HA", LOG_LEVEL_INFO);
			rCode = ACS_APGCC_SUCCESS;
		}
	}
        return rCode;
}

//-------------------------------------------------------------------------------------------------------------------
ACS_APGCC_ReturnType asecHAClass::passifyApp() {

	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;
 	//passiveToActive=1;
	active = false;
	if (0 != this->m_asecObj) {
		log.Write("asec-ha-class: 'csadm-service' is already passive", LOG_LEVEL_INFO);
		rCode = ACS_APGCC_SUCCESS;
	} else {
		ACE_NEW_NORETURN(this->m_asecObj, ACS_ASEC_Adm());
		if (0 == this->m_asecObj) {
			syslog(LOG_ERR, "asec-ha-class: failed to create the instance");
			log.Write("asec-ha-class: failed to create the instance", LOG_LEVEL_ERROR);
		}
		else {
			int res = this->m_asecObj->passive(this); // This will start passive functionality and will not return until myCLass is running
			if (res < 0) {
				// Failed to start
				delete this->m_asecObj;
				this->m_asecObj = 0;
			} else {
				log.Write("asec-ha-class: 'csadm-service' is now passivated by HA", LOG_LEVEL_INFO);
				rCode = ACS_APGCC_SUCCESS;
			}
		}	
	}
	return rCode;
}

//-------------------------------------------------------------------------------------------------------------------
ACS_APGCC_ReturnType asecHAClass::shutdownApp(){

	if ( 0 != this->m_asecObj){
		log.Write("asec-ha-class: Ordering 'csadm-service' to shutdown", LOG_LEVEL_INFO);
		this->m_asecObj->stop(); // This will initiate the application shutdown and will not return until application is stopped completely.

		log.Write("asec-ha-class: Waiting for csadm-service to shutdown...", LOG_LEVEL_INFO);
		this->m_asecObj->wait();

		log.Write( "asec-ha-class: Deleting csadm-service instance...", LOG_LEVEL_INFO);
		delete this->m_asecObj;
		this->m_asecObj=0;
	}
	else
		log.Write("asec-ha-class: shutdownApp already done", LOG_LEVEL_INFO);
	active = false;
	return ACS_APGCC_SUCCESS;
}


//-------------------------------------------------------------------------------------------------------------------
ACS_APGCC_ReturnType asecHAClass::performComponentHealthCheck(void) {

	/* Application has received health check callback from AMF. Check the
	 * sanity of the application and reply to AMF that you are ok.
	 *  
	 */

        log.Write("asec-ha-class: health-check success", LOG_LEVEL_INFO);
	return ACS_APGCC_SUCCESS;
}
//-------------------------------------------------------------------------------------------------------------------

