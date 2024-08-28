/**
 * @file CUTE_ACS_NSF_Server.cpp
 * Test functions ACS_NSF_Server are defined in this file
 */
/* INCLUDE SECTION */
/* inclusion of cute libraries */
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

/* inclusion of NSF Cute test header files */
#include "CUTE_ACS_NSF_Server.h"
#include "CUTE_ACS_NSF_Common.h"
#include "acs_nsf_server.h"
#include "acs_nsf_types.h"
#include "acs_nsf_common.h"
#include <set>
/* Inclusion of ACE library files */
#include <ace/ACE.h>

/*===========================================
 * Declaring Static variables
 * ===========================================*/
ACS_NSF_Server* Cute_ACS_NSF_Server::m_poNsfServer = 0;
ACS_NSF_Common* Cute_ACS_NSF_Server::m_poNsfCommonptr= 0;

Cute_ACS_NSF_Server::Cute_ACS_NSF_Server()
{

}

Cute_ACS_NSF_Server::~Cute_ACS_NSF_Server()
{

}
/*=======================================================
 * Routine declaration
 *=======================================================*/

/*========================================================
 *  ROUTINE:vFindNode()
 *========================================================*/
#if 0
void Cute_ACS_NSF_Server::vFindNode(){
    int bResult=getPoNsfServer()->FindNode();
    ASSERTM("Unable to find the node",bResult!=0);

}

/*========================================================
 *  ROUTINE::vIsActiveNode()
 *========================================================*/

void Cute_ACS_NSF_Server::vIsActiveNode(){
    int bResult=getPoNsfServer()->IsActiveNode();
    ASSERTM("Unable to find the status of the node",bResult!=-1);

}

/*========================================================
 *  ROUTINE::vIsActive()
 *========================================================*/

void Cute_ACS_NSF_Server::vIsActive(){
    int bResult=getPoNsfServer()->IsActive();
    ASSERTM("Node is passive",bResult!=0);

}
#endif


void Cute_ACS_NSF_Server::vStartPingServer()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      				Start Pinging the Server                 "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	PingThreadStruct pts;
	ACE_thread_t res = 0;
	bool bResult=getPoNsfServer()->StartPingServer();
	res = ACE_Thread::self();
	cout<<"res :"<<res<<endl;
	cout<<"StartPingServer result :"<<bResult<<endl;
//	ASSERTM("Error in StartPingServer", res != NULL );
}
/*========================================================
 *  ROUTINE:vSaveSystemTime()
 *========================================================*/

void Cute_ACS_NSF_Server::vSaveSystemTime()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"                     Save the system time                   "<<endl;
	cout<<endl<<"************************************************************"<<endl;
    int bResult=getPoNsfServer()->SaveSystemTime();
    cout<<"The Result of the SaveSystemTime is :"<<bResult<<endl;
    ASSERTM("Error in Saving the system timer",bResult== 0);

}

/*========================================================
 *  ROUTINE:vGetPhaParameters()
 *========================================================*/

void Cute_ACS_NSF_Server::vGetPhaParameters()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"            Get The PHA parameter from the IMM              "<<endl;
	cout<<endl<<"************************************************************"<<endl;
    int bResult=getPoNsfServer()->GetPhaParameters();
    cout<<"The Result of the GetPhaParameters is :"<<bResult<<endl;
    ASSERTM("Error found in executing GetPhaParameters",bResult== 0);

}

/*========================================================
 *  ROUTINE:vSetDefaultFailoverCount()
 *========================================================*/

void Cute_ACS_NSF_Server::vSetDefaultFailoverCount()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"               Set The default FailOver Count               "<<endl;
	cout<<endl<<"************************************************************"<<endl;
    int bResult=getPoNsfServer()->SetDefaultFailoverCount();
    cout<<"The Result of the SetDefaultFailoverCount is :"<<bResult<<endl;
    ASSERTM("Error found in executing SetDefaultFailoverCount",bResult== 0);

}




/*========================================================
 *  ROUTINE:vSetDefaultFailoverCount()
 *========================================================*/

void Cute_ACS_NSF_Server::vSetDefaultLastFailoverTime()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"              Set The default Last FailOver Time            "<<endl;
	cout<<endl<<"************************************************************"<<endl;
    int bResult=getPoNsfServer()->SetDefaultLastFailoverTime();
    cout<<"The Result of the SetDefaultLastFailoverTime is :"<<bResult<<endl;
    ASSERTM("Error found in executing SetDefaultLastFailoverTime",bResult== 0);

}




/*========================================================
 *  ROUTINE:vSetDefaultAlarmState()
 *========================================================*/

void Cute_ACS_NSF_Server::vSetDefaultAlarmState()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"               Set The default Alarm State               "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	int bResult=getPoNsfServer()->objCommon.getNode();
	if (bResult == 1)
	{
		cout<<endl<<"The Current node is 1"<<endl;
		int bResult=getPoNsfServer()->SetDefaultAlarmState(NODE_A);
		ASSERTM("Error found in executing SetDefaultAlarmState",bResult== 0);
	}
	else if(bResult == 2)
	{
		cout<<endl<<"The Current node is 2"<<endl;
		int bResult=getPoNsfServer()->SetDefaultAlarmState(NODE_B);
		ASSERTM("Error found in executing SetDefaultAlarmState",bResult== 0);
	}
	else
	{
		cout<<endl<<"The Current node is Undefined"<<endl;
	}


}




/*========================================================
 *  ROUTINE:vSetDefaultNetworkSurveillanceValue()
 *========================================================*/

void Cute_ACS_NSF_Server::vSetDefaultNetworkSurveillanceValue()
{
	cout<<endl<<"*************************************************************"<<endl;
	cout<<endl<<"Set The default NetworkSurveillanceValue with Activre flag (1)"<<endl;
	cout<<endl<<"************************************************************"<<endl;
    int bResult=getPoNsfServer()->SetDefaultNetworkSurveillanceValue(1);
    cout<<"The Result of the SetDefaultNetworkSurveillanceValue is :"<<bResult<<endl;
    ASSERTM("Error found in executing SetDefaultNetworkSurveillanceValue",bResult== 0);


    cout<<endl<<"*************************************************************"<<endl;
    cout<<endl<<"Set The default NetworkSurveillanceValue with Activre flag (0)"<<endl;
    cout<<endl<<"************************************************************"<<endl;
    bResult=getPoNsfServer()->SetDefaultNetworkSurveillanceValue(0);
    cout<<"The Result of the SetDefaultNetworkSurveillanceValue is :"<<bResult<<endl;
    ASSERTM("Error found in executing SetDefaultNetworkSurveillanceValue",bResult== 0);


}

/*========================================================
 *  ROUTINE:vSetDefaultIMMParamsSurveillanceflag1()
 *========================================================*/

void Cute_ACS_NSF_Server::vSetDefaultIMMParamsSurveillanceflag1()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"Set the Default IMM parameter when the ActiveSurviellence is 1"<<endl;
	cout<<endl<<"************************************************************"<<endl;

	int bResult=getPoNsfServer()->objCommon.getNode();
		if (bResult == 1)
		{
			cout<<endl<<"The Current node is 1"<<endl;
			int bResult=getPoNsfServer()->SetDefaultIMMParams(NODE_A,1);
			ASSERTM("Error found in executing vSetDefaultIMMParamsSurveillanceflag1",bResult== 0);
		}
		else if(bResult == 2)
		{
			cout<<endl<<"The Current node is 2"<<endl;
			int bResult=getPoNsfServer()->SetDefaultIMMParams(NODE_B,1);
			ASSERTM("Error found in executing vSetDefaultIMMParamsSurveillanceflag1",bResult== 0);
		}
		else
		{
			cout<<endl<<"The Current node is Undefined"<<endl;
		}


}

/*========================================================
 *  ROUTINE:vSetDefaultIMMParamsSurveillanceflag1()
 *========================================================*/

void Cute_ACS_NSF_Server::vSetDefaultIMMParamsSurveillanceflag0()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"Set the Default IMM parameter when the ActiveSurviellence is 0"<<endl;
	cout<<endl<<"************************************************************"<<endl;

	int bResult=getPoNsfServer()->objCommon.getNode();
	if (bResult == 1)
	{
		cout<<endl<<"The Current node is 1"<<endl;
		int bResult=getPoNsfServer()->SetDefaultIMMParams(NODE_A,0);
		ASSERTM("Error found in executing vSetDefaultIMMParamsSurveillanceflag0",bResult== 0);
	}
	else if(bResult == 2)
	{
		cout<<endl<<"The Current node is 2"<<endl;
		int bResult=getPoNsfServer()->SetDefaultIMMParams(NODE_B,0);
		ASSERTM("Error found in executing vSetDefaultIMMParamsSurveillanceflag0",bResult== 0);
	}
	else
	{
		cout<<endl<<"The Current node is Undefined"<<endl;
	}


}
/*========================================================
 *  ROUTINE:vGetConfigParameters()
 *========================================================*/

void Cute_ACS_NSF_Server::vGetConfigParameters()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"            Get The config parameter from the IMM              "<<endl;
	cout<<endl<<"************************************************************"<<endl;
    int bResult=getPoNsfServer()->GetConfigParameters();
    ASSERTM("Error found in executing GetConfigParameters",bResult== 0);

}



/*========================================================
 *  ROUTINE:vGetDefaultGateways()
 *========================================================*/

void Cute_ACS_NSF_Server::vGetDefaultGateways()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"            		Get The Default Gateway                  "<<endl;
	cout<<endl<<"************************************************************"<<endl;

	int bResult=getPoNsfServer()->GetDefaultGateways();
	ASSERTM("Error found in executing the GetDefaultGateways",bResult== 0);

	vector<string>::iterator it1, it2;
	vector <string> GateWays;

	FILE* pipe = popen("route -n | grep 'UG[ \t]' | awk '{print $2}'", "r");

	char buffer[128];
	while(!feof(pipe))
	{
		if(fgets(buffer, 128, pipe) != NULL)
		{
			GateWays.push_back(buffer);
		}
	}
	for(it2 = GateWays.begin(); it2 != GateWays.end(); it2++)
	{
		cout<<"The Default GateWays is/are:"<<*it2<<endl;
	}

	pclose(pipe);


	for(it1 = getPoNsfServer()->sGatewayVector.begin(); it1 != getPoNsfServer()->sGatewayVector.end(); it1++)
	{
		cout<<"The sGatewayVector is/are :"<<*it1<<endl;
	}

	multiset<string> set1( getPoNsfServer()->sGatewayVector.begin(),  getPoNsfServer()->sGatewayVector.end());
	multiset<string> set2(GateWays.begin(), GateWays.end());

	bool equal_data = (set1 == set2);

	ASSERTM("Error Found in executing in GetDefaultGateways",equal_data == TRUE);


}

/*========================================================
 *  ROUTINE:vGetDefaultGateways2()
 *========================================================*/

void Cute_ACS_NSF_Server::vGetDefaultGateways2()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"            		Get The Default Gateway 2                  "<<endl;
	cout<<endl<<"************************************************************"<<endl;

	int bResult=getPoNsfServer()->GetDefaultGateways2();
	ASSERTM("Error found in executing the GetDefaultGateways2",bResult== 0);

	vector<string>::iterator it1, it2;
	vector <string> GateWays2;

	FILE* pipe = popen("route -n | grep 'UG[ \t]' | awk '{print $2}'", "r");

	char buffer[128];
	while(!feof(pipe))
	{
		if(fgets(buffer, 128, pipe) != NULL)
		{
			GateWays2.push_back(buffer);
		}
	}
	for(it2 = GateWays2.begin(); it2 != GateWays2.end(); it2++)
	{
		cout<<"The Default GateWays2 is/are:"<<*it2<<endl;
	}

	pclose(pipe);


	for(it1 = getPoNsfServer()->sGatewayVector2.begin(); it1 != getPoNsfServer()->sGatewayVector2.end(); it1++)
	{
		cout<<"The sGatewayVector2 is/are :"<<*it1<<endl;
	}

	multiset<string> set1( getPoNsfServer()->sGatewayVector2.begin(),  getPoNsfServer()->sGatewayVector2.end());
	multiset<string> set2(GateWays2.begin(), GateWays2.end());

	bool equal_data = (set1 == set2);
	cout<<endl<<"Check for the Physical Separation in the STP.if No then below is the expected error"<<endl;
	ASSERTM("Error Found in executing in GetDefaultGateways",equal_data == TRUE);


}




/*========================================================
 *  ROUTINE:vCeaseAllAlarms()
 *========================================================*/

void Cute_ACS_NSF_Server::vCeaseAllAlarms()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"  Cease All The Raised Alarms For DefaultGateways(state=0)  "<<endl;
	cout<<endl<<"************************************************************"<<endl;
    int bResult=getPoNsfServer()->CeaseAllAlarms(0);
    ASSERTM("Error found in executing CeaseAllAlarms",bResult== 0);

	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"  Cease All The Raised Alarms For DefaultGateways(state=1)  "<<endl;
	cout<<endl<<"************************************************************"<<endl;
    bResult=getPoNsfServer()->CeaseAllAlarms(1);
    ASSERTM("Error found in executing CeaseAllAlarms",bResult== 0);

}

/*========================================================
 *  ROUTINE:vCeaseAllAlarms2()
 *========================================================*/

void Cute_ACS_NSF_Server::vCeaseAllAlarms2()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"  Cease All The Raised Alarms For DefaultGateways2(state=0)  "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	int bResult=getPoNsfServer()->CeaseAllAlarms2(0);
	ASSERTM("Error found in executing CeaseAllAlarms2",bResult== 0);


	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"  Cease All The Raised Alarms For DefaultGateways2(state=1)  "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bResult=getPoNsfServer()->CeaseAllAlarms2(1);
	ASSERTM("Error found in executing CeaseAllAlarms2",bResult== 0);


}


/*========================================================
 *  ROUTINE:vsetNodeSpecificParamsTest()
 *========================================================*/


void Cute_ACS_NSF_Server::vsetNodeSpecificParamsTest()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"               Set The Node Specific Parameter              "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	int bResult=getPoNsfServer()->objCommon.setNodeSpecificParams();
	ASSERTM("Error in vsetNodeSpecificParamsTest",bResult == 0);
}



/*========================================================
 *  ROUTINE:vStopServer()
 *========================================================*/

void Cute_ACS_NSF_Server::vStopServer()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"           Stop The Network Surveillance Server             "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	getPoNsfServer()->StopServer();
//	ASSERTM("Error found in executing StopServer",bResult== 0);
}
/*========================================================
 *  ROUTINE:vTerminateGatewaySup()
 *========================================================*/

void Cute_ACS_NSF_Server::vTerminateGatewaySup()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"              Terminate the gateways forcefully             "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	getPoNsfServer()->TerminateGatewaySup(1);
//	ASSERTM("Error found in executing StopServer",bResult== 0);
}




/*========================================================
 *  ROUTINE:vOkToFailover()
 *========================================================*/

void Cute_ACS_NSF_Server::vOkToFailover()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"                   Say Ok To Do Failover                    "<<endl;
	cout<<endl<<"************************************************************"<<endl;
    int bResult=getPoNsfServer()->OkToFailover();
    ASSERTM("Error in OkToFailover",bResult!=0);

}



void Cute_ACS_NSF_Server::vVerifyHeartBeat()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"              		Verify Heart Beat             			 "<<endl;
	cout<<endl<<"************************************************************"<<endl;

    int bResult=getPoNsfServer()->VerifyHeartBeat();
    cout<<"VerifyHeartBeat result :"<<bResult<<endl;
    ASSERTM("Error in VerifyHeartBeat",bResult== 0);

}



void Cute_ACS_NSF_Server::vCountBlockedGw()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"          Count the No of Blocked GW(alarmstatus=0)         "<<endl;
	cout<<endl<<"************************************************************"<<endl;

    int bResult=getPoNsfServer()->CountBlockedGw(0,1);
    cout<<endl<<"The Return Code for the CountBlockedGw(0,1) is :"<<bResult<<endl;
    ASSERTM("Error Found in counting the no. of Blocked GW",bResult== 0);


	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"          Count the No of Blocked GW(alarmstatus=1)         "<<endl;
	cout<<endl<<"************************************************************"<<endl;

    bResult=getPoNsfServer()->CountBlockedGw(1,1);
    cout<<endl<<"The Return Code for the CountBlockedGw(1,1) is :"<<bResult<<endl;
    ASSERTM("Error Found in counting the no. of Blocked GW",bResult== 0);
}


void Cute_ACS_NSF_Server::vPingThread()
{

	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"          				Ping Thread         				 "<<endl;
	cout<<endl<<"************************************************************"<<endl;

    int bResult=getPoNsfServer()->PingThread();
    cout<<"PingThread result :"<<bResult<<endl;
    ASSERTM("Error in PingThread",bResult != 0);

}
/*========================================================
 *  ROUTINE:vReportEvent()
 *========================================================*/

void Cute_ACS_NSF_Server::vReportEvent()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"          				Report Events        				 "<<endl;
	cout<<endl<<"************************************************************"<<endl;

    int bResult=ReportEvent(1,"EVENT","AP INTERNAL FAULT","Reference","Reference 2","ICMP problem");
    ASSERTM("Error in vReportEvent",bResult== 1);

}

/*========================================================
 *  ROUTINE:NetworkSurveillance()
 *========================================================*/

void Cute_ACS_NSF_Server::vNetworkSurveillance()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"          		Network Surveillance Method        			 "<<endl;
	cout<<endl<<"************************************************************"<<endl;

	bool bResult=getPoNsfServer()->NetworkSurveillance();
	cout<<"Result of NetworkSurveillance is:"<<bResult<<endl;
	ASSERTM("Error found in executing the NetworkSurveillance method",bResult == 0);

}

/*========================================================
 *  ROUTINE:vCheckIfFailoverForPS()
 *========================================================*/
void Cute_ACS_NSF_Server::vCheckIfFailoverForPS()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"          		   Check Failover for PS        			     "<<endl;
	cout<<endl<<"************************************************************"<<endl;

	bool bResult=getPoNsfServer()->CheckIfFailoverForPS();
	cout<<"Result of CheckIfFailoverForPS is:"<<bResult<<endl;
	ASSERTM("Error found in executing the CheckIfFailoverForPS method",bResult == true);

}

/*========================================================
 *  ROUTINE:init()
 *========================================================*/

void Cute_ACS_NSF_Server::init()
{
	m_poNsfServer = new ACS_NSF_Server();
//	m_poNsfServer->objCommon.setNodeSpecificParams();
}



/*========================================================
 * ROUTINE:make_suite_Cute_NSF_Biostime_Recovery()
 *========================================================*/

cute::suite Cute_ACS_NSF_Server::make_suite_Cute_ACS_NSF_Server(){
	cute::suite s;
	int returnvalue=m_poNsfServer->objCommon.fetchDnOfRootObjFromIMM();
	if (returnvalue == 0)
	{
		s.push_back(CUTE(vsetNodeSpecificParamsTest));
	}
	else
	{
		ASSERTM("Error in fetchDnOfRootObjFromIMM",returnvalue == 0);

	}

	s.push_back(CUTE(vStartPingServer));
	s.push_back(CUTE(vSaveSystemTime));
	s.push_back(CUTE(vGetPhaParameters));
	s.push_back(CUTE(vSetDefaultFailoverCount));
	s.push_back(CUTE(vSetDefaultLastFailoverTime));
	s.push_back(CUTE(vSetDefaultAlarmState));
	s.push_back(CUTE(vSetDefaultNetworkSurveillanceValue));
	s.push_back(CUTE(vSetDefaultIMMParamsSurveillanceflag1));
	s.push_back(CUTE(vSetDefaultIMMParamsSurveillanceflag0));
	s.push_back(CUTE(vGetConfigParameters));
	s.push_back(CUTE(vGetDefaultGateways));
	s.push_back(CUTE(vGetDefaultGateways2));
	s.push_back(CUTE(vCeaseAllAlarms));
	s.push_back(CUTE(vCeaseAllAlarms2));
	s.push_back(CUTE(vTerminateGatewaySup));
	s.push_back(CUTE(vStopServer));
	s.push_back(CUTE(vOkToFailover));
	s.push_back(CUTE(vVerifyHeartBeat));
	s.push_back(CUTE(vCountBlockedGw));
	s.push_back(CUTE(vPingThread));
//	s.push_back(CUTE(vNetworkSurveillance));
	s.push_back(CUTE(vCheckIfFailoverForPS));
	s.push_back(CUTE(vReportEvent));
	s.push_back(CUTE(vNsfServerCleanup));
	return s;
}


void Cute_ACS_NSF_Server::vNsfServerCleanup()
{
	cout<<"Entering vNsfServerCleanup"<<endl;
	delete m_poNsfServer;
	m_poNsfServer = 0;
	cout<<"Leaving vNsfServerCleanup"<<endl;
}

