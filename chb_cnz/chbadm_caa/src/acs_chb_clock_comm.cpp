/*=================================================================== */
   /**
   @file acs_chb_clock_comm.cpp

   Class method implementation for service.

   This module contains the implementation of class declared in
   the acs_chb_clock_comm.h module.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       27/01/2011     XCHEMAD        APG43 on Linux.
        **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#include <acs_chb_clock_comm.h>
#include <acs_chb_tra.h>


/*===================================================================
   ROUTINE: ACS_CHB_ClockComm
=================================================================== */

ACS_CHB_ClockComm::ACS_CHB_ClockComm () :
ACS_CHB_Event (ACS_CHB_processName),
cycletime (0)
{  
	DEBUG(1,"%s", "Executing ACS_CHB_ClockComm constructor");
	reminderFlags=0;
	myInitiator=0;
	U1 =0 ;
	U2 =0;
	Result =0;
	//Heap allocation moved here by qabpapi.
	#if 0
	myInitiator=new ACS_JTP_Conversation((char*)&ACS_CHB_Clock[0], BuffLength);
	if ( !myInitiator)
	{
	    DEBUG(1,"%s","ACS_JTP_Conversation Object could not be created");
	}
	#endif
} // End of constructor

/*===================================================================
   ROUTINE: ~ACS_CHB_ClockComm
=================================================================== */

ACS_CHB_ClockComm::~ACS_CHB_ClockComm ()
{
  	DEBUG(1,"%s", "Executing destructor");
	#if 0
 	if( myInitiator)
	{
  	    delete myInitiator;
	    myInitiator = 0;
	}
	#endif
} 

/*===================================================================
   ROUTINE: data2
=================================================================== */

bool ACS_CHB_ClockComm::data2(unsigned char* timebuf, int successiveCommFailures)
{ 
	char problemData [PROBLEMDATASIZE];   

	DEBUG(1,"%s", "Executing ACS_CHB_ClockComm::data2()");


	U1=U2=0;
	time_t t0;
	t0=clock();
	if(!myInitiator->jexdatareq(U1, U2, BuffLength, (char*)timebuf))
	{
	    //If this failed, JTP will have done a jexdiscreq, so respond to it, and get some error info
            myInitiator->jexdiscind(U1, U2, Result);
	    DEBUG(1,"%s", "Unable to read the CP time , jexdatareq failed when trying to get CP time");
	    ACE_OS::sprintf(problemData, "jexdatareq failed when trying to get CP time, JTP error code %d.Waited %ld s",
			Result, (clock()-t0)/CLOCKS_PER_SEC);
	    if(successiveCommFailures < 10)
	    {	
	    	event(ACS_CHB_DiscIndJTP,
				ACS_CHB_EVENT_STG,
				ACS_CHB_Cause_APfault,
				problemData,
				(char*)ACS_CHB_Text_ProblemJTP);
	    }
	    reminderFlags|=JEXDATAREQ_FAILED;
	    return false;
	}
	else
	{
	    DEBUG(1,"%s", "jexdatareq success when trying to get CP time");
	    reminderFlags&=~JEXDATAREQ_FAILED;
	}

	char* pData;
	unsigned short us;
	U1=U2=0;
	t0=clock();
	if(!myInitiator->jexdataind(U1, U2, us, pData))
	{
		//If this failed, probably due to a timeout, JTP will have done a jexdiscreq, so respond to it,
		//and get some error info
		myInitiator->jexdiscind(U1, U2, Result);
		DEBUG(1,"jexdataind failed when trying to get CP time, JTP error code %d", Result);
		ACE_OS::sprintf(problemData, "jexdataind failed when trying to get CP time, JTP error code %d. Waited %ld s",
			Result, (clock()-t0)/CLOCKS_PER_SEC);
		if(successiveCommFailures < 10)
		{
			event(	ACS_CHB_DiscIndJTP,
					ACS_CHB_EVENT_STG,
					ACS_CHB_Cause_APfault,
					problemData,
					(char*)ACS_CHB_Text_ProblemJTP);
		}
		reminderFlags|=JEXDATAIND_FAILED;
		return false;
	}
	else
	{
	    DEBUG(1,"%s", "jexdataind success when trying to get CP time.");
	    reminderFlags&=~JEXDATAIND_FAILED;
	}
	DEBUG(1,"data received length is %d", us);
	memcpy(timebuf, pData, 100);
	return true;
} 

/*===================================================================
   ROUTINE: terminate
=================================================================== */

bool ACS_CHB_ClockComm::terminate ()
{ 

	DEBUG(1,"%s", "Executing ACS_CHB_ClockComm::terminate()");

  	// Disconnect session
	if(myInitiator && (!myInitiator->jexdiscreq(U1, U2, 0)))
	{
                event(	ACS_CHB_BadReturnCodeJTP,
					ACS_CHB_EVENT_STG,
					ACS_CHB_Cause_APfault,
					"jexdiscreq failed in ACS_CHB_ClockComm",
					(char*)ACS_CHB_Text_ProblemJTP);
		if( myInitiator != 0 )
        	{
			delete myInitiator;
			myInitiator = 0;
        	}



		return true; //false; Continue as if OK, initiate2 will fail and cause suicide if really bad.
						//Done this because jexdisreq may return false even if the only error is that the connection
						//is already down.
	}

	if( myInitiator != 0 )
	{
		delete myInitiator;
		myInitiator = 0;
	}

	return true;
}

/*===================================================================
   ROUTINE: getParameters
=================================================================== */

ACS_CC_ReturnType ACS_CHB_ClockComm::getParameters (OmHandler & aOmHandler)
{ 
  	ACS_CC_ReturnType rcode = ACS_CC_SUCCESS;
  	char    problemData [PROBLEMDATASIZE] = { 0 };   

  	DEBUG(1, "%s", "Entering ACS_CHB_ClockComm::getParameters");

	std::string dnObject("");
	//dnObject.append(CLOCKSYNC_PARAM_OBJ_DN);
        //dnObject.append(",");
	dnObject.append(ACS_CHB_Common::dnOfCsyncRootObj);

  	DEBUG(1, "%s", "Fetch cycleTime");

	ACS_CC_ImmParameter *paramToFind  = new ACS_CC_ImmParameter();
	
	paramToFind->attrName = (char*)ACS_CHB_cycleTime;

  	if ( aOmHandler.getAttribute( dnObject.c_str(), paramToFind ) == ACS_CC_SUCCESS )
  	{
    	    cycletime = *( reinterpret_cast < unsigned short *> (*(paramToFind->attrValues))) ;
	    if(cycletime < 500 || cycletime > 1000)
            {
                DEBUG(1, "%s","Receievd value is out of range. Setting cycleTime to default value");
		cycletime = 600;
            }

	    DEBUG(1,"Cycle time is %u", cycletime);
	    //HP38217  Commented cycle time hard coding
#if 0
	    cycletime = 300; //Make it 5 min instead, to curb drift, which can be >2 s in 10 m
#endif
#ifdef _DEBUG
	    cycletime=30; //When debugging , get on with it!
#endif
  	}
  	else
  	{
    	    // Event handling

    	    ACE_OS::snprintf (problemData, sizeof(problemData)/sizeof(*problemData) - 1,
			("%d:Error when reading the IMM parameter %s"), 
			 __LINE__,
			 ACS_CHB_cycleTime);
            event (ACS_CHB_ReadFailedPHA,
           		ACS_CHB_EVENT_STG,
           		ACS_CHB_Cause_APfault,
           		problemData, 
           		ACS_CHB_Text_ProblemPHA);

    	    rcode = ACS_CC_FAILURE;
  	} 
	
	delete paramToFind;
	paramToFind = 0;

        DEBUG(1, "%s", "Leaving ACS_CHB_ClockComm::getParameters");

  	return rcode;
} // End of getParameters

/*===================================================================
   ROUTINE: initiate2
=================================================================== */

bool ACS_CHB_ClockComm::initiate2(int successiveCommInitiateFailures)
{
	char	problemData [PROBLEMDATASIZE];

	myInitiator = new ACS_JTP_Conversation((char*)&ACS_CHB_Clock[0], BuffLength);
	if ( !myInitiator)
	{
		DEBUG(1,"%s","ACS_JTP_Conversation Object could not be created");
		return false;
	}

	ACS_JTP_Conversation::JTP_Node* pNode = new ACS_JTP_Conversation::JTP_Node; // Struct Node with systemId.
	if ( !pNode)
	{
		DEBUG(1,"%s","ACS_JTP_Conversation::JTP_Node Object could not be created");
		terminate();
		return false;
	}

	pNode->system_id = ACS_JTP_Conversation_R3A::SYSTEM_ID_CP_CLOCK_MASTER;
	//pNode->system_id = ACS_JTP_Conversation_R3A::SYSTEM_ID_THIS_NODE;
	pNode->node_state = ACS_JTP_Conversation_R3A::NODE_STATE_ACTIVE;

	//Start TR HP83493
	pNode->node_side = ACS_JTP_Conversation_R3A::NODE_SIDE_UNDEFINED;
	ACE_OS::strcpy(pNode->system_name,"0");
	ACE_OS::strcpy(pNode->node_name,"0");
	pNode->system_type = ACS_JTP_Conversation_R3A::SYSTEM_TYPE_UNKNOWN;
	//End TR HP83493

	ACE_TCHAR nodeHostName[32];
	gethostname(nodeHostName, sizeof(nodeHostName));
	DEBUG(1,"Executing `initiate2',this computer is %s", nodeHostName);


	//Override the default timeout (5 s)
	myInitiator->setTimeOut(InitTimeLimit); //InitTimeLimit is currently 10 s

	// Initiate JTP conversation

	DEBUG(1,"%s", "Initiating the JTP conversation by calling jexinitreq");

	U1=U2=0;
	if(!myInitiator->jexinitreq(pNode, U1, U2))
	{
		myInitiator->jexdiscind(U1, U2, Result);
		DEBUG(1,"JTP conversation initiation request failed, JTP reason code %d", Result);
		ACE_OS::sprintf(problemData, "JTP conversation initiation request failed, JTP reason code %d", Result);
		if(successiveCommInitiateFailures < 10)
		{
			event(	ACS_CHB_ConnectFault,
					ACS_CHB_EVENT_STG,
					ACS_CHB_Cause_APfault,
					problemData,
					ACS_CHB_Text_ProblemAPMA);
		}
		terminate();
		delete pNode;
		pNode = 0;
		return false;
	}
	if(!myInitiator->jexinitconf(U1, U2, Result)) //In jtptest they say it can't fail if jexinitreq(), but
		//then it does return bool, and Result can apparently contain an error code, so
	{ 
		//Get here if confirmation failed 
		DEBUG(1,"JTP conversation initiation confirmation failed, reason code %d", Result);
		ACE_OS::sprintf(problemData, "JTP conversation initiation confirmation failed, reason code %d", Result);
		if(successiveCommInitiateFailures < 10)
		{
			event(	ACS_CHB_ConnectFault,
					ACS_CHB_EVENT_STG,
					ACS_CHB_Cause_APfault,
					problemData,
					ACS_CHB_Text_ProblemAPMA);
		}
		terminate();
		delete pNode;
		pNode = 0;
		return false;
	}

	myInitiator->setTimeOut(DataTimeLimit); //Now applies to jexdataind, we hope.

	delete pNode;
	pNode = 0;

	return true;

} // End of initiate2
