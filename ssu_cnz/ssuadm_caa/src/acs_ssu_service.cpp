#include <ace/ACE.h>
#include <ace/Process.h>
#include <ace/Event.h>
#include <algorithm>
#include <sys/stat.h>
#include "acs_ssu_service.h"
#include "acs_ssu_common.h"

ACS_SSU_Service::ACS_SSU_Service(const char* daemon_name, const char* username):ACS_APGCC_ApplicationManager(daemon_name, username)
{
	//DEBUG("%s","Entering ACS_SSU_Service::ACS_SSU_Service()");
	passiveToActive=0;
	m_ssuApplObj=0;
	//DEBUG("%s","Leaving ACS_SSU_Service::ACS_SSU_Service");
}

ACS_SSU_Service::~ACS_SSU_Service()
{
	DEBUG("%s","Entering ACS_SSU_Service::~ACS_SSU_Service()");
	// to be sure.
	this->shutdownApp(); 
	DEBUG("%s","Leaving ACS_SSU_Service::~ACS_SSU_Service");
}

ACS_APGCC_ReturnType ACS_SSU_Service::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG("%s","Entering ACS_SSU_Service::performStateTransitionToActiveJobs()");
	DEBUG("%s","Leaving ACS_SSU_Service::performStateTransitionToActiveJobs()");
	(void) previousHAState;
	return this->activateApp();

}

ACS_APGCC_ReturnType ACS_SSU_Service::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG("%s","Entering ACS_SSU_Service::performStateTransitionToPassiveJobs()");
	DEBUG("%s","Leaving ACS_SSU_Service::performStateTransitionToPassiveJobs()");
	(void) previousHAState;
	return this->passifyApp();
}

ACS_APGCC_ReturnType ACS_SSU_Service::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG("%s","Entering ACS_SSU_Service::performStateTransitionToQueisingJobs()");
	DEBUG("%s","Leaving ACS_SSU_Service::performStateTransitionToQueisingJobs");
	(void) previousHAState;
	return this->shutdownApp();
}

ACS_APGCC_ReturnType ACS_SSU_Service::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	DEBUG("%s","Entering ACS_SSU_Service::performStateTransitionToQuiescedJobs()");
	DEBUG("%s","Leaving ACS_SSU_Service::performStateTransitionToQuiescedJobs");
	(void) previousHAState;
	return this->shutdownApp();	
}

ACS_APGCC_ReturnType ACS_SSU_Service::performComponentHealthCheck(void)
{
	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType ACS_SSU_Service::performComponentTerminateJobs(void)
{
	DEBUG("%s","Entering ACS_SSU_Service::performComponentTerminateJobs()");
	DEBUG("%s","Exiting ACS_SSU_Service::performComponentTerminateJobs()");
	return this->shutdownApp();
}

ACS_APGCC_ReturnType ACS_SSU_Service::performComponentRemoveJobs(void)
{
	DEBUG("%s","Entering ACS_SSU_Service::performComponentRemoveJobs()");
	DEBUG("%s","Exiting ACS_SSU_Service::performComponentRemoveJobs()");
	return this->shutdownApp();
}

ACS_APGCC_ReturnType ACS_SSU_Service::performApplicationShutdownJobs()
{
	DEBUG("%s","Entering ACS_SSU_Service::performApplicationShutdownJobs()");
	DEBUG("%s","Exiting ACS_SSU_Service::performApplicationShutdownJobs()");
	return this->shutdownApp();
}

ACS_APGCC_ReturnType ACS_SSU_Service::activateApp()
{
	DEBUG("%s","Entering ACS_SSU_Service::activateApp()");
	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;
	std::string myPath("/data/aquota.group.new");
	if(ACS_SSU_Common::CheckFileExists(myPath.c_str()))
	{
		DEBUG("%s","Found  aquota.group.new file")
		ACE_OS::system("rm -rf /data/aquota.group.new");
		DEBUG("%s","Removed aquota.group.new file succesfully");
	}
	if ( 0 != this->m_ssuApplObj)
	{
		if (passiveToActive)
		{
			DEBUG("%s","passive->active transition. stop passive work before becoming active");
			this->m_ssuApplObj->stop();
			passiveToActive=0;
		} 
		else
		{
			DEBUG("%s","application is already active");
			rCode = ACS_APGCC_SUCCESS;
		}

	}
	else
	{
		ACE_NEW_NORETURN(this->m_ssuApplObj, SSU_Service_Loader());
		if (0 == this->m_ssuApplObj)
		{
			DEBUG("%s","failed to create the ssu instance");
		}
	}

	if ( 0 != this->m_ssuApplObj)
	{
		int res = this->m_ssuApplObj->active(this); // This will start active functionality. Will not return until myCLass is running
		if (res < 0)
		{
			// Failed to start
			delete this->m_ssuApplObj;
			this->m_ssuApplObj = 0;
		}
		else
		{
			DEBUG("%s","application is now activated by HA");
			rCode = ACS_APGCC_SUCCESS;
		}
	}
	DEBUG("%s","Exiting ACS_SSU_Service::activateApp()");
	return rCode;
}

ACS_APGCC_ReturnType ACS_SSU_Service::shutdownApp()
{
	DEBUG("%s","Entering ACS_SSU_Service::shutdownApp()");
	if ( 0 != this->m_ssuApplObj)
	{
		DEBUG("%s","Ordering ssu Application to shutdown");
		this->m_ssuApplObj->stop(); // This will initiate the application shutdown and will not return until application is stopped completely.
		DEBUG("%s","Deleting ssu App instance...");
		delete this->m_ssuApplObj;
		this->m_ssuApplObj=0;
	}
	else
	{
		DEBUG("%s","Shutting down is already Done!!");
	}
	DEBUG("%s","Exiting ACS_SSU_Service::shutdownApp()");
	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType ACS_SSU_Service::passifyApp()
{
	DEBUG("%s","Entering ACS_SSU_Service::passifyApp()");
	ACS_APGCC_ReturnType rCode = ACS_APGCC_FAILURE;
	passiveToActive=1;

	if (0 != this->m_ssuApplObj)
	{
		DEBUG("%s","application is already passive");
		rCode = ACS_APGCC_SUCCESS;
	}
	else
	{
		ACE_NEW_NORETURN(this->m_ssuApplObj, SSU_Service_Loader());
		if (0 == this->m_ssuApplObj)
		{
			DEBUG("%s","Failed to create ssu application instance.");
		}
		else
		{
			int res = this->m_ssuApplObj->passive(this); // This will start passive functionality and will not return until myCLass is running
			if (res < 0)
			{
				// Failed to start
				delete this->m_ssuApplObj;
				this->m_ssuApplObj = 0;
			}
			else
			{
				DEBUG("%s","App is now passivated by HA");
				rCode = ACS_APGCC_SUCCESS;
			}
		}	
	}
	DEBUG("%s","Exiting ACS_SSU_Service::passifyApp()");
	return rCode;
}


