/**
 * @file CUTE_ACS_NSF_Common.cpp
 * Test functions ACS_NSF_Common are defined in this file
 */
/* INCLUDE SECTION */
/* inclusion of cute libraries */
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

/* inclusion of NSF Cute test header files */
#include "CUTE_ACS_NSF_Common.h"
#include "acs_nsf_common.h"

/* Inclusion of ACE library files */
#include <ace/ACE.h>
/*===========================================
 * Declaring Static variables
 * ===========================================*/
ACS_NSF_Common* Cute_ACS_NSF_Common::m_poNsfCommon = 0;

Cute_ACS_NSF_Common::Cute_ACS_NSF_Common()
{


}

Cute_ACS_NSF_Common::~Cute_ACS_NSF_Common()
{

}
/*=======================================================
 * Routine declaration
 *=======================================================*/

/*========================================================
 *  ROUTINE:vgetRegWord()
 *========================================================*/


void Cute_ACS_NSF_Common::vsetNodeSpecificParamsTest()
{
	int bResult=getPoNsfCommon()->setNodeSpecificParams();
	    ASSERTM("Error in vsetNodeSpecificParamsTest",bResult == 0);
}

/*========================================================
 *  ROUTINE:vsetAndgetRegWordForfailover()
 *========================================================*/

void Cute_ACS_NSF_Common::vsetAndgetRegWordForfailover()
{	int bResultget;
	int bResultset;
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      Set the Registration Word for NSF_NO_OF_FAILOVER      "<<endl;
	cout<<endl<<"************************************************************"<<endl;
    bResultset=getPoNsfCommon()->setRegWord("numberFailover",0);
    cout<<endl<<"The Return Code for the setRegWord of NSF_NO_OF_FAILOVER is :"<<bResultset<<endl;
    if(bResultset == 0)
    {
    	cout<<endl<<"************************************************************"<<endl;
    	cout<<endl<<"      Get the Registration Word for NSF_NO_OF_FAILOVER      "<<endl;
    	cout<<endl<<"************************************************************"<<endl;
    	ACE_UINT32 dummy = 0;
    	bResultget = getPoNsfCommon()->getRegWord("numberFailover",dummy);
    	cout<<endl<<"The Return Value of the getregWord(2nd Param):  "<<dummy<<endl;
    	cout<<endl<<"The Return Value for the getRegWord of NSF_NO_OF_FAILOVER is :  "<<bResultget<<endl;
    	if(bResultget != 0)
    	{
    	ASSERTM("Error in vgetRegWord",bResultget == 0);
    	}
    }
    else
    {
    ASSERTM("Error in vsetRegWord",bResultset == 0 );
    }
}

/*========================================================
 *  ROUTINE:vsetAndgetRegWordForAlarmstate2B()
 *========================================================*/
void Cute_ACS_NSF_Common::vsetAndgetRegWordForAlarmstate2B()
{
	int bResultget;
	int bResultset;
    cout<<endl<<"************************************************************"<<endl;
    cout<<endl<<"      Set the Registration Word for NSF_ALARMSTATE_NET2_B      "<<endl;
    cout<<endl<<"************************************************************"<<endl;
    cout<<endl<<"Update the value of alarm state 2B with '0'"<<endl;
    bResultset=getPoNsfCommon()->setRegWord("alarmState2B",0);
    cout<<endl<<"The Return Code for the setRegWord of NSF_ALARMSTATE_NET2_B is :"<<bResultset<<endl;
    if(bResultset == 0)
    {
    	cout<<endl<<"************************************************************"<<endl;
    	cout<<endl<<"      Get the Registration Word for NSF_ALARMSTATE_NET2_B      "<<endl;
    	cout<<endl<<"************************************************************"<<endl;
    	ACE_UINT32 alarmState = 0;
        bResultget = getPoNsfCommon()->getRegWord("alarmState2B",alarmState);
    	cout<<endl<<"The Return Value of the getregWord(2nd Param):  "<<alarmState<<endl;
    	cout<<endl<<"The Return Value for the sgetRegWord of NSF_ALARMSTATE_NET2_B is :  "<<bResultget<<endl;
    	if(bResultget != 0)
    	{
    		ASSERTM("Error in vgetRegWord",bResultget == 0);
    	}
    }
    else
    {
    	ASSERTM("Error in vsetRegWord",bResultset == 0 );
    }

    cout<<endl<<"Update the value of alarm state 2B with '1'"<<endl;
    bResultset=getPoNsfCommon()->setRegWord("alarmState2B",1);
    cout<<endl<<"The Return Code for the setRegWord of NSF_ALARMSTATE_NET2_B is :"<<bResultset<<endl;

    if(bResultset != 0)
        {
    	ASSERTM("Error in vsetRegWord",bResultset == 0 );
        }
}
/*========================================================
 *  ROUTINE:vsetAndgetRegWordForAlarmstate2B()
 *========================================================*/
void Cute_ACS_NSF_Common::vsetAndgetRegWordForAlarmstateB()
{
	int bResultget;
	int bResultset;
    cout<<endl<<"************************************************************"<<endl;
    cout<<endl<<"      Set the Registration Word for NSF_ALARMSTATE_B        "<<endl;
    cout<<endl<<"************************************************************"<<endl;
    cout<<endl<<"Update the value of alarm stateB with '0'"<<endl;
    bResultset=getPoNsfCommon()->setRegWord("alarmStateB",0);
    cout<<endl<<"The Return Code for the setRegWord of NSF_ALARMSTATE_B is :"<<bResultset<<endl;
    if(bResultset == 0)
    {
    	cout<<endl<<"************************************************************"<<endl;
    	cout<<endl<<"      Get the Registration Word for NSF_ALARMSTATE_B        "<<endl;
    	cout<<endl<<"************************************************************"<<endl;
    	ACE_UINT32 alarmState = 0;
    	bResultget = getPoNsfCommon()->getRegWord("alarmStateB",alarmState);
    	cout<<endl<<"The Return Value of the getregWord(2nd Param):  "<<alarmState<<endl;
    	cout<<endl<<"The Return Value for the getregWord of NSF_ALARMSTATE_B is  "<<bResultget<<endl;
    	if(bResultget != 0)
    	{
    		ASSERTM("Error in vgetRegWord",bResultget == 0);
    	}
    }
    else
    {
    	ASSERTM("Error in vsetRegWord",bResultset == 0 );
    }
    cout<<endl<<"Update the value of alarm stateB with '1'"<<endl;
    bResultset=getPoNsfCommon()->setRegWord("alarmStateB",1);
    cout<<endl<<"The Return Code for the setRegWord of NSF_ALARMSTATE_B is :"<<bResultset<<endl;

    if(bResultset != 0)
        {
    	ASSERTM("Error in vsetRegWord",bResultset == 0 );
        }
}
/*========================================================
 *  ROUTINE:vsetAndgetRegWordForAlarmstate2A()
 *========================================================*/
void Cute_ACS_NSF_Common::vsetAndgetRegWordForAlarmstate2A()
{
	int bResultget;
	int bResultset;
    cout<<endl<<"************************************************************"<<endl;
    cout<<endl<<"      Set the Registration Word for NSF_ALARMSTATE_NET2_A        "<<endl;
    cout<<endl<<"************************************************************"<<endl;
    cout<<endl<<"Update the value of alarm state2A with '0'"<<endl;
    bResultset=getPoNsfCommon()->setRegWord("alarmState2A",0);
    cout<<endl<<"The Return Value for the setRegWord of NSF_ALARMSTATE_NET2_A is :"<<bResultset<<endl;
    if(bResultset == 0)
    {
    	cout<<endl<<"************************************************************"<<endl;
    	cout<<endl<<"      Get the Registration Word for NSF_ALARMSTATE_NET2_A        "<<endl;
    	cout<<endl<<"************************************************************"<<endl;
    	ACE_UINT32 alarmState = 0;
    	bResultget = getPoNsfCommon()->getRegWord("alarmState2A",alarmState);
    	cout<<endl<<"The Return Value of the getregWord(2nd Param):  "<<alarmState<<endl;
    	cout<<endl<<"The Return Value for the getregWord of NSF_ALARMSTATE_NET2_A is "<<bResultget<<endl;
    	if(bResultget != 0)
    	{
    		ASSERTM("Error in vgetRegWord",bResultget == 0);
    	}
    }
    else
    {
    	ASSERTM("Error in vsetRegWord",bResultset == 0 );
    }
    cout<<endl<<"Update the value of alarm state 2A with '1'"<<endl;
    bResultset=getPoNsfCommon()->setRegWord("alarmState2A",1);
    cout<<endl<<"The Return Code for the setRegWord of NSF_ALARMSTATE_NET2_A is :"<<bResultset<<endl;

    if(bResultset != 0)
        {
    	ASSERTM("Error in vsetRegWord",bResultset == 0 );
        }

}
/*========================================================
 *  ROUTINE:vsetAndgetRegWordForAlarmstateA()
 *========================================================*/
void Cute_ACS_NSF_Common::vsetAndgetRegWordForAlarmstateA()
{
	int bResultget;
	int bResultset;
    cout<<endl<<"************************************************************"<<endl;
    cout<<endl<<"      Set the Registration Word for NSF_ALARMSTATE_A        "<<endl;
    cout<<endl<<"************************************************************"<<endl;
    cout<<endl<<"Update the value of alarm stateA with '0'"<<endl;
    bResultset=getPoNsfCommon()->setRegWord("alarmStateA",0);
    cout<<endl<<"The Return Value for the setRegWord of NSF_ALARMSTATE_A is :"<<bResultset<<endl;
    if(bResultset == 0)
    {
    	cout<<endl<<"************************************************************"<<endl;
    	cout<<endl<<"      Get the Registration Word for NSF_ALARMSTATE_A        "<<endl;
    	cout<<endl<<"************************************************************"<<endl;
    	ACE_UINT32 alarmState = 0;
    	bResultget = getPoNsfCommon()->getRegWord("alarmStateA",alarmState);
    	cout<<endl<<"The Return Value of the getregWord(2nd Param):  "<<alarmState<<endl;
    	cout<<endl<<"The Return Value for the getregWord of NSF_ALARMSTATE_A is "<<bResultget<<endl;
    	if(bResultget != 0)
    	{
    		ASSERTM("Error in vgetRegWord",bResultget == 0);
    	}
    }
    else
    {
    	ASSERTM("Error in vsetRegWord",bResultset == 0 );
    }

    cout<<endl<<"Update the value of alarm stateA with '1'"<<endl;
    bResultset=getPoNsfCommon()->setRegWord("alarmStateA",1);
    cout<<endl<<"The Return Code for the setRegWord of NSF_ALARMSTATE_A is :"<<bResultset<<endl;

    if(bResultset != 0)
    {
    	ASSERTM("Error in vsetRegWord",bResultset == 0 );
    }

}
/*========================================================
 *  ROUTINE:vsetAndgetRegWordForSurvellianceActFlag()
 *========================================================*/
void Cute_ACS_NSF_Common::vsetAndgetRegWordForSurvellianceActFlag()
{
	int bResultget;
	int bResultset;

    cout<<endl<<"************************************************************"<<endl;
    cout<<endl<<"      Set the Registration Word for NSF_SURVELLIENCEACTIVEFLAG        "<<endl;
    cout<<endl<<"************************************************************"<<endl;
    bResultset=getPoNsfCommon()->setRegWord("surveillanceActiveFlag",0);
    cout<<endl<<"The Return Value for the setRegWord of NSF_SURVELLIENCEACTIVEFLAG is :"<<bResultset<<endl;
    if(bResultset == 0)
    {
    	cout<<endl<<"************************************************************"<<endl;
    	cout<<endl<<"      Get the Registration Word for NSF_SURVELLIENCEACTIVEFLAG        "<<endl;
    	cout<<endl<<"************************************************************"<<endl;
    	ACE_UINT32 alarmState = 0;
    	bResultget = getPoNsfCommon()->getRegWord("surveillanceActiveFlag",alarmState);
    	cout<<endl<<"The Return Value of the getregWord(2nd Param):  "<<alarmState<<endl;
    	cout<<endl<<"The Return Value for the getregWord of NSF_SURVELLIENCEACTIVEFLAG is "<<bResultget<<endl;
    	if(bResultget != 0)
    	{
    		ASSERTM("Error in vgetRegWord",bResultget == 0);
    	}
    }
    else
    {
    	ASSERTM("Error in vsetRegWord",bResultset == 0 );
    }
}
/*========================================================
 *  ROUTINE:vsetAndgetRegWordForNsfDummyFlag()
 *========================================================*/
void Cute_ACS_NSF_Common::vsetAndgetRegWordForNsfDummyFlag()//Negative Test Function
{
	int bResultget;
	int bResultset;
    cout<<endl<<"************************************************************"<<endl;
    cout<<endl<<"      Set the Registration Word for Wrong_Value_Name        "<<endl;
    cout<<endl<<"************************************************************"<<endl;
    bResultset=getPoNsfCommon()->setRegWord("NsfDummyFlag",0);
    cout<<endl<<"The Return Value for the setRegWord of Wrong_Value_Name is :"<<bResultset<<endl;
    if(bResultset == 0)
    {
    	cout<<endl<<"************************************************************"<<endl;
    	cout<<endl<<"      Get the Registration Word for Wrong_Value_Name        "<<endl;
    	cout<<endl<<"************************************************************"<<endl;
    	ACE_UINT32 alarmState = 0;
    	bResultget = getPoNsfCommon()->getRegWord("NsfDummyFlag",alarmState);
    	cout<<endl<<"The Return Value of the getregWord(2nd Param):  "<<alarmState<<endl;
    	cout<<endl<<"The Return Value for the getregWord of Wrong_Value_Name is "<<bResultget<<endl;
    	if(bResultget != 0)
    	{
    		ASSERTM("Error in vgetRegWord",bResultget == 0);
    	}
    }
    else
    {
    	cout<<endl<<"The above returncode is expected as the value name is incorrect"<<endl;
    	cout<<endl<<"The correct returncode is zero"<<endl;
//    	ASSERTM("Error in vsetRegWord.",bResultset == 0 );
    }
}

/*========================================================
 *  ROUTINE:vgetRegTime()
 *========================================================*/

void Cute_ACS_NSF_Common::vgetRegTime()
{
    ACE_UINT32 nsfTime = 0;
    cout<<endl<<"************************************************************"<<endl;
    cout<<endl<<"      Get the Registration Time for NSF_LASTFAILOVERTIME        "<<endl;
    cout<<endl<<"************************************************************"<<endl;
    int bResult=getPoNsfCommon()->getRegTime("lastFailoverTime",nsfTime);
    cout<<endl<<"The Return Value for the getregTime of NSF_LASTFAILOVERTIME is "<<bResult<<endl;
    ASSERTM("Error in vgetRegTime",bResult == 0);

}
/*========================================================
 *  ROUTINE:vgetRegTime()
 *========================================================*/

void Cute_ACS_NSF_Common::vgetRegTimeNegative()
{
    ACE_UINT32 nsfTime = 0;
    cout<<endl<<"************************************************************"<<endl;
    cout<<endl<<"    Negative Test Of the Functionality for getRegTime()       "<<endl;
    cout<<endl<<"      Get the Registration Time for Wrong_Value_Name       "<<endl;
    cout<<endl<<"************************************************************"<<endl;
    int bResult=getPoNsfCommon()->getRegTime("lastFailoverTime123",nsfTime);
    cout<<endl<<"The Return Value for the getregTime of Wrong_Value_Name is "<<bResult<<endl;
    ASSERTM("Error in vgetRegTime",bResult == -1);

}

/*========================================================
 *  ROUTINE:vsetRegTime()
 *========================================================*/

void Cute_ACS_NSF_Common::vsetRegTime()
{
	cout<<endl<<"************************************************************"<<endl;
    cout<<endl<<"    Negative Test Of the Functionality for setRegTime()       "<<endl;
	cout<<endl<<"      Set the Registration Time for NSF_LASTFAILOVERTIME        "<<endl;
	cout<<endl<<"************************************************************"<<endl;
    int bResult=getPoNsfCommon()->setRegTime("lastFailoverTime");
    cout<<endl<<"The Return Value for the setregTime of NSF_LASTFAILOVERTIME is "<<bResult<<endl;
    ASSERTM("Error in vsetRegTime",bResult == 0);

}

/*========================================================
 *  ROUTINE:vsetRegTimeNegative()
 *========================================================*/
void Cute_ACS_NSF_Common::vsetRegTimeNegative()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      Set the Registration Time for Wrong_Value_Name        "<<endl;
	cout<<endl<<"************************************************************"<<endl;
    int bResult=getPoNsfCommon()->setRegTime("lastFailoverTime123");
    cout<<endl<<"The Return Value for the setregTime of Wrong_Value_Name is "<<bResult<<endl;
    ASSERTM("Error in vsetRegTime",bResult == 1);

}
/*========================================================
 *  ROUTINE:vcalculateStartStopValueactive()
 *========================================================*/

void Cute_ACS_NSF_Common::vcalculateStartStopValueactive()
{
	int surveillanceActiveFlag=1;
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"Calculate the start_stop value for the NSF service(surveillanceActiveFlag=1)"<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bool bResult=getPoNsfCommon()->calculateStartStopValue(1);
	if(bResult)
	{
		cout<<endl<<"The start_stop value for the service is calculated successfully"<<endl;
	}
	else
	{
	ASSERTM("Error found in executing calculateStartStopValue",bResult == true);
	}


	cout<<endl<<"The value of startstop when surveillanceActiveFlag=1,activeFlag=1"<<endl;
	getPoNsfCommon()->setStartStopValue(3);
	int theStartstopvalue=getPoNsfCommon()->getStartStopValue();
	cout<<endl<<"The value of startstop from getStartStopValue when surveillanceActiveFlag=1,activeFlag=1 is:  "<<theStartstopvalue<<endl;


	surveillanceActiveFlag=0;
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"Calculate the start_stop value for the NSF service(surveillanceActiveFlag=0)"<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bResult=getPoNsfCommon()->calculateStartStopValue(1);
	if(bResult)
	{
		cout<<endl<<"The start_stop value for the service is calculated successfully"<<endl;
	}
	else
	{
		ASSERTM("Error found in executing calculateStartStopValue",bResult == true);
	}


	cout<<endl<<"The value of startstop when surveillanceActiveFlag=0,activeFlag=1"<<endl;
	getPoNsfCommon()->setStartStopValue(1);
	theStartstopvalue=getPoNsfCommon()->getStartStopValue();
	cout<<endl<<"The value of startstop from getStartStopValue when surveillanceActiveFlag=0,activeFlag=1 is:  "<<theStartstopvalue<<endl;

}
/*========================================================
 *  ROUTINE:vcalculateStartStopValuepassive()
 *========================================================*/
void Cute_ACS_NSF_Common::vcalculateStartStopValuepassive()
{
	int surveillanceActiveFlag=1;
		cout<<endl<<"************************************************************"<<endl;
		cout<<endl<<"Calculate the start_stop value for the NSF service(surveillanceActiveFlag=1)"<<endl;
		cout<<endl<<"************************************************************"<<endl;
		bool bResult=getPoNsfCommon()->calculateStartStopValue(0);
		if(bResult)
		{
			cout<<endl<<"The start_stop value for the service is calculated successfully"<<endl;
		}
		else
		{
		ASSERTM("Error found in executing calculateStartStopValue",bResult == true);
		}

		cout<<endl<<"The value of startstop when surveillanceActiveFlag=1,activeFlag=0"<<endl;
		getPoNsfCommon()->setStartStopValue(2);
		int theStartstopvalue=getPoNsfCommon()->getStartStopValue();
		cout<<endl<<"The value of startstop from getStartStopValue when surveillanceActiveFlag=1,activeFlag=0 is:  "<<theStartstopvalue<<endl;


		surveillanceActiveFlag=0;
		cout<<endl<<"************************************************************"<<endl;
		cout<<endl<<"Calculate the start_stop value for the NSF service(surveillanceActiveFlag=0)"<<endl;
		cout<<endl<<"************************************************************"<<endl;
		bResult=getPoNsfCommon()->calculateStartStopValue(0);
		if(bResult)
		{
			cout<<endl<<"The start_stop value for the service is calculated successfully"<<endl;
		}
		else
		{
			ASSERTM("Error found in executing calculateStartStopValue",bResult == true);
		}

		cout<<endl<<"The value of startstop when surveillanceActiveFlag=0,activeFlag=0"<<endl;
		getPoNsfCommon()->setStartStopValue(0);
		theStartstopvalue=getPoNsfCommon()->getStartStopValue();
		cout<<endl<<"The value of startstop from getStartStopValue when surveillanceActiveFlag=0,activeFlag=0 is:  "<<theStartstopvalue<<endl;

}

/*========================================================
 *  ROUTINE:vgetNode()
 *========================================================*/
void Cute_ACS_NSF_Common::vgetNode()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"                    Status of The Node(1/2)                  "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	int bResult=getPoNsfCommon()->getNode();
	if (bResult == 1)
	{
		cout<<endl<<"The Current node is 1"<<endl;
	}
	else if(bResult == 2)
	{
		cout<<endl<<"The Current node is 2"<<endl;
	}
	else
	{
		cout<<endl<<"The Current node is Undefined"<<endl;
	}
}
/*========================================================
 *  ROUTINE:vgetActiveNodeFlag()
 *========================================================*/
void Cute_ACS_NSF_Common::vgetActiveNodeFlag()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"              Status of The Node(Active/Passive)             "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bool bResult=getPoNsfCommon()->getActiveNodeFlag();
	if (bResult == true)
	{
		cout<<endl<<"The Current node is Active"<<endl;
	}
	else
	{
		cout<<endl<<"The Current node is Passive"<<endl;
	}

}

/*========================================================
 *  ROUTINE:init()
 *========================================================*/

void Cute_ACS_NSF_Common::init()
{

	m_poNsfCommon = new ACS_NSF_Common();
}



/*========================================================
 * ROUTINE:make_suite_Cute_ACS_NSF_Common()
 *========================================================*/

cute::suite Cute_ACS_NSF_Common::make_suite_Cute_ACS_NSF_Common(){
	cute::suite s;
	int returnvalue=ACS_NSF_Common::fetchDnOfRootObjFromIMM();
	if (returnvalue == 0)
	{
		s.push_back(CUTE(vsetNodeSpecificParamsTest));
	}
	else
	{
		ASSERTM("Error in fetchDnOfRootObjFromIMM",returnvalue == 0);

	}
	s.push_back(CUTE(vsetAndgetRegWordForfailover));
	s.push_back(CUTE(vsetAndgetRegWordForAlarmstate2B));
	s.push_back(CUTE(vsetAndgetRegWordForAlarmstateB));
	s.push_back(CUTE(vsetAndgetRegWordForAlarmstate2A));
	s.push_back(CUTE(vsetAndgetRegWordForAlarmstateA));
	s.push_back(CUTE(vsetAndgetRegWordForSurvellianceActFlag));
	s.push_back(CUTE(vsetAndgetRegWordForNsfDummyFlag));
	s.push_back(CUTE(vgetRegTime));
	s.push_back(CUTE(vgetRegTimeNegative));
	s.push_back(CUTE(vsetRegTime));
	s.push_back(CUTE(vsetRegTimeNegative));
	s.push_back(CUTE(vcalculateStartStopValueactive));
	s.push_back(CUTE(vcalculateStartStopValuepassive));
	s.push_back(CUTE(vgetNode));
	s.push_back(CUTE(vgetActiveNodeFlag));
	s.push_back(CUTE(vNsfCommonCleanup));
	return s;

}


void Cute_ACS_NSF_Common::vNsfCommonCleanup()
{
	cout<<"Entering vNsfCommonCleanup"<<endl;
	delete m_poNsfCommon;
	m_poNsfCommon = 0;
	cout<<"Leaving vNsfCommonCleanup"<<endl;
}
