//******************************************************************************
//
// NAME
//      acs_lct_adm.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2003.
// All rights reserved.
//
// The Copyright to the computer program(s) herein is the property
// of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with the written
// permission from Ericsson AB
// or in accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have been supplied.
//
// DESCRIPTION
//     code for lct service.           
//              
//
// DOCUMENT NO
//      ----
//
// AUTHOR
//       
//
// CHANGES
//
//      REV NO          DATE            NAME            DESCRIPTION
//			2012-25-04      XFURULL         welcomemessage system 
//							improvement(OP#345) on all ports
//******************************************************************************  


#include <acs_lct_adm.h>
#include <acs_lct_reactorrunner.h>
#include <acs_lct_server.h>

#define RELATIVETIME(x) (ACE_OS::time(NULL) + x)
const int SHUTDOWN=111;

ACS_LCT_Adm::ACS_LCT_Adm()
{
	DEBUG("%s","Inside ACS_LCT_Adm constructor");
	m_reactorAlreadyStop = false;
}
ACS_LCT_Adm::~ACS_LCT_Adm()
{
	if(m_reactorRunner != 0)
	{
		DEBUG("%s","Deleting m_reactorRunner in ACS_LCT_Adm destructor");
		delete m_reactorRunner;
		m_reactorRunner =0 ;
	}
	DEBUG("%s","Inside ACS_LCT_Adm destructor");
}

int ACS_LCT_Adm::active(ACS_LCT_Server* haObj ){

	m_haObj=haObj;
	DEBUG("%s", "active(): invoking APOS_HA_ReactorRunner()");
	ACE_NEW_NORETURN(m_reactorRunner, APOS_HA_ReactorRunner());
	if (0 == m_reactorRunner) {
		syslog(LOG_ERR, "%s() Failed to create APOS_HA_ReactorRunner", __func__);
		return -1;
	}	
	DEBUG("%s", "active(): invoking open()");
	m_reactorAlreadyStop = false;
	int res = m_reactorRunner->open();
	if (res < 0) {
		syslog(LOG_ERR, "Failed to start ACS_HA_ReactorRunner");
		m_reactorAlreadyStop = true;
		return -1;
	}	
	DEBUG("%s", "active(): invoking activate()");
	if( this->activate( THR_JOINABLE | THR_NEW_LWP ) < 0 ){
		syslog(LOG_ERR, "%s() Failed to start main svc thread.", __func__);
		return -1;
	}
	return 0;
}

int ACS_LCT_Adm::passive(ACS_LCT_Server* haObj ){

	m_haObj=haObj;
	DEBUG("%s", " ACS_LCT_Adm::passive");
	ACE_NEW_NORETURN(m_reactorRunner, APOS_HA_ReactorRunner());
	if (0 == m_reactorRunner) {
		syslog(LOG_ERR, "%s() Failed to create APOS_HA_ReactorRunner", __func__);
		return -1;
	}
	m_reactorAlreadyStop = false;
	int res = m_reactorRunner->open();
	if (res < 0) {
		syslog(LOG_ERR, "Failed to start ACS_HA_ReactorRunner");
		m_reactorAlreadyStop = true;
		return -1;
	}	
	if( this->activate( THR_JOINABLE | THR_NEW_LWP ) < 0 ){
		syslog(LOG_ERR, "%s() Failed to start main svc thread.", __func__);
		return -1;
	}
	return 0;
}

void ACS_LCT_Adm::stop() {
	// start shutdown activities.
	/* We were active and now losing active state due to some shutdown admin
	 * operation performed on our SU.
	 * Inform the thread to go to "stop" state
	 */
	
	//ACE_Thread_Manager::instance()->join(devmon_worker_thread_id);
	/* Inform the thread to go "stop" state */
		DEBUG("%s", " ACS_LCT_Adm::stop");
        ACE_Message_Block* mb=0;

        ACE_NEW_NORETURN(mb, ACE_Message_Block());
        if (mb == 0){
                syslog(LOG_ERR, "app-class:Failed create message SHUTDOWN");
        } else {
                mb->msg_type(SHUTDOWN);
                if (this->putq(mb) < 0){
                        mb->release();
                        mb=0;
                        syslog(LOG_ERR, "app-class:Failed to send msg SHUTDOWN");
                }else{
                        syslog(LOG_INFO, "app-class:SHUTDOWN Ordered Internally");
                }
        }
}

int ACS_LCT_Adm::svc() {
	DEBUG("%s", " ACS_LCT_Adm::svc");

        bool done=false;
        int res=0;
	bool IsDsdPublished=false;

	(void)CONFIG_IMM_CLASS_NAME;
	(void)CONFIG_IMM_CLASS_NAME1;
	(void)CONFIG_IMM_CLASS_NAME2;
	ACE_Message_Block* mb=0;

	DEBUG("%s", "Application is thread is starting...");
	DEBUG("%s", "LCT_Adm: Thread started :");


	if(m_haObj->Is_Active){
		ACS_CC_ReturnType myReturnErrorCode;
		DEBUG("%s","LCT_Adm: Application is Active");
		myReturnErrorCode=m_haObj->startFunctionality();

		if(myReturnErrorCode != ACS_CC_FAILURE)
		{
			DEBUG("%s","LCT_Adm: LCT Server started");
		}
		else
		{
			DEBUG("%s","LCT_Adm: LCT Server Failed");
			ERROR("%s","Internal program error. Exit code: 26");
		}
	}else if(!m_haObj->Is_Active) {
		        syslog(LOG_ERR, "LCT_Adm: Application is Passive");
			DEBUG("%s","svc: Thread:: Application is Passive");
			// Create Global dsdServer Object
			dsdObj->instantiate();
			IsDsdPublished=dsdObj->StartDSDServer();
	}

	//ACE_Time_Value timeout;
	ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeout;
        timeout = this->gettimeofday ();
        timeout += ACE_Time_Value (0,50000);

        while (!done){
                res = this->getq(mb, &timeout);
                //Checked received message
		if (res == 0) {
                	switch( mb->msg_type() ){

                        	case SHUTDOWN: {
                                	syslog(LOG_INFO, "LCT_Adm: received SHUTDOWN");
					//if (!m_haObj->Is_Active && !m_haObj->passiveToActive) {
					if (!m_haObj->Is_Active) {
						dsdObj->ShutdownDSD();
					}	
                                	mb->release();
                                	mb=0;
                                	done=true;
                                	break; //switch break
                        	}
						       
                        	default: {
                                	mb->release();
                                	mb=0;
                                	syslog(LOG_ERR, "app-class:[%d] Unknown message received:", mb->msg_type());
                                	break; //switch break
				}
                	}
		}	
		if (done)
		    break; // while break
			
		// If active, wait on getq
		if (m_haObj->Is_Active) {
        		timeout += ACE_Time_Value (1800,0);
			continue;	
		}
		if (!IsDsdPublished) {
			IsDsdPublished=dsdObj->StartDSDServer();
			if (!IsDsdPublished) {
        			timeout += ACE_Time_Value (10,0);
				continue;
			}
		}	
		
		bool rt= dsdObj->DSDServerfunc();
		if (!rt) {				
			syslog(LOG_INFO, "DSD Server functionality failed");
			DEBUG("%s","svc: DSD Server functionality failed");
			ERROR("%s","Internal program error. Exit code: 26");
		}
        }// end of while

        return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------
int ACS_LCT_Adm::close(u_long /* flags */){

	if(m_haObj->Is_Active){
		//DEBUG("%s", "ACS_LCT_Adm::close() - sending shutdown signal to hardening thread");
		//ACE_Thread_Manager::instance()->kill(m_haObj->hardeningThreadId, SIG_SHUTDOWN_HARDENING);
		DEBUG("%s", "ACS_LCT_Adm::close() - shuttdown hardening thread");
		m_haObj->shutdown_hardening();
		DEBUG("%s", "ACS_LCT_Adm::close() - waiting for hardeningThreadId thread to join");
		ACE_Thread_Manager::instance()->join(m_haObj->hardeningThreadId);
		DEBUG("%s", "..done");

		//DEBUG("%s", "ACS_LCT_Adm::close() - sending shutdown signal to tsuser thread");
		//ACE_Thread_Manager::instance()->kill(m_haObj->tsUsersThreadId, SIG_SHUTDOWN_TSUSERS);
		DEBUG("%s", "ACS_LCT_Adm::close() - shuttdown tsuser thread");
		m_haObj->shutdown_tsusers();
		DEBUG("%s", "ACS_LCT_Adm::close() - waiting for tsUsersThreadId thread to join");
		ACE_Thread_Manager::instance()->join(m_haObj->tsUsersThreadId);
		DEBUG("%s", "..done");
		
		DEBUG("%s", "ACS_LCT_Adm::close() - shuttdown securityhandling thread");
		m_haObj->shutdown_securityhandling();
		DEBUG("%s", "ACS_LCT_Adm::close() - waiting for securityhandlingThreadID  thread to join");
		ACE_Thread_Manager::instance()->join(m_haObj->securityhandlingThreadId);
		DEBUG("%s", "..done");
	}
	if(m_reactorAlreadyStop == true){return 0;}
	// Stop reactor
	if (m_reactorRunner != 0) {
		m_reactorAlreadyStop = true;
		DEBUG("%s", "close(u_long): invoking stop");
		m_reactorRunner->stop();
		DEBUG("%s", "close(u_long): invoking wait");
		m_reactorRunner->wait();
		DEBUG("%s", "close(u_long): deleting m_reactorRunner");
		delete m_reactorRunner;
		m_reactorRunner = 0;
	}	
	return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------
