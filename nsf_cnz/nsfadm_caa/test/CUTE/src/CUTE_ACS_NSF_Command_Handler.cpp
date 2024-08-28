/**
 * @file CUTE_ACS_NSF_Command_Handler.cpp
 * Test functions CUTE_ACS_NSF_Command_Handler are defined in this file
 */
/* INCLUDE SECTION */
/* inclusion of cute libraries */
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

/* inclusion of NSF Cute test header files */
#include "CUTE_ACS_NSF_Command_Handler.h"
#include "acs_nsf_command_handler.h"
#include "acs_nsf_common.h"

/* Inclusion of ACE library files */
#include <ace/ACE.h>
using namespace std;

/*===========================================
 * Declaring Static variables
 * ===========================================*/
ACS_NSF_CommandHandler * Cute_NSF_Command_Handler::objCommandHandler =0;
ACS_NSF_Common* objCommon = new ACS_NSF_Common();

Cute_NSF_Command_Handler::Cute_NSF_Command_Handler()
{

}


Cute_NSF_Command_Handler::~Cute_NSF_Command_Handler()
{

}
/*=======================================================
 * Routine declaration
 *=======================================================*/

/*========================================================
 *  ROUTINE:vModifySurvellienceflag()
 *========================================================*/

void Cute_NSF_Command_Handler::vModifySurvellienceflag()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      		Modify call back for NSF_SURVELLIENCEACTIVEFLAG	 "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	ACS_APGCC_AttrModification *attrMods[0];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[0]->modAttr.attrName = new char[50];
	strcpy(attrMods[0]->modAttr.attrName, "surveillanceActiveFlag" );
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	int value=1;
	objCommon->setStartStopValue(1);
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(&value);
    int bResult=getCommandhandler()->modify(0,0,"networkSurveillanceMId=1",attrMods);
    cout<<endl<<"The Return Code for NSF_SURVELLIENCEACTIVEFLAG is : "<<bResult<<endl;
    ASSERTM("Modification for NSF_SURVELLIENCEACTIVEFLAG is failed",bResult==0);

}


/*========================================================
 *  ROUTINE:vModifyNumberFailover()
 *========================================================*/

void Cute_NSF_Command_Handler::vModifyNumberFailover()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      		Modify call back for NSF_NOOFFAILOVER			 "<<endl;
	cout<<endl<<"************************************************************"<<endl;

	ACS_APGCC_AttrModification *attrMods[0];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[0]->modAttr.attrName = new char[50];
	strcpy(attrMods[0]->modAttr.attrName, "numberFailover" );
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	int value=0;
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(&value);
    int bResult=getCommandhandler()->modify(0,0,"networkSurveillanceMId=1",attrMods);
    ASSERTM("Modification for NSF_NOOFFAILOVER is failed",bResult==0);

}
/*========================================================
 *  ROUTINE:vModifyRouterResponse()
 *========================================================*/

void Cute_NSF_Command_Handler::vModifyRouterResponse()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      		Modify call back for NSF_ROUTER_RESPONSE	     "<<endl;
	cout<<endl<<"************************************************************"<<endl;

	ACS_APGCC_AttrModification *attrMods[0];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[0]->modAttr.attrName = new char[50];
	strcpy(attrMods[0]->modAttr.attrName, "routerResponse" );
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	int value =3;
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(&value);
    int bResult=getCommandhandler()->modify(0,0,"networkSurveillanceMId=1",attrMods);
    ASSERTM("Modification for NSF_ROUTER_RESPONSE is failed",bResult==0);

}

/*========================================================
 *  ROUTINE:vModifyPingPeriod()
 *========================================================*/

void Cute_NSF_Command_Handler::vModifyPingPeriod()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      		Modify call back for NSF_PING_PERIOD	 "<<endl;
	cout<<endl<<"************************************************************"<<endl;

	ACS_APGCC_AttrModification *attrMods[0];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[0]->modAttr.attrName = new char[50];
	strcpy(attrMods[0]->modAttr.attrName, "pingPeriod" );
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	int value=60;
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(&value);
    int bResult=getCommandhandler()->modify(0,0,"networkSurveillanceMId=1",attrMods);
    ASSERTM("Modification for NSF_PING_PERIOD is failed",bResult==0);

}
/*========================================================
 *  ROUTINE:vModifyResetTime()
 *========================================================*/

void Cute_NSF_Command_Handler::vModifyResetTime()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      		Modify call back for NSF_RESET_TIME	 			 "<<endl;
	cout<<endl<<"************************************************************"<<endl;

	ACS_APGCC_AttrModification *attrMods[0];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[0]->modAttr.attrName = new char[50];
	strcpy(attrMods[0]->modAttr.attrName, "resetTime" );
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	int value=60;
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(&value);
    int bResult=getCommandhandler()->modify(0,0,"networkSurveillanceMId=1",attrMods);
    ASSERTM("Modification for NSF_RESET_TIME is failed",bResult==0);

}
/*========================================================
 *  ROUTINE:vModifyFailovers()
 *========================================================*/

void Cute_NSF_Command_Handler::vModifyFailovers()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      		Modify call back for NSF_FAIL_OVERS				 "<<endl;
	cout<<endl<<"************************************************************"<<endl;

	ACS_APGCC_AttrModification *attrMods[0];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[0]->modAttr.attrName = new char[50];
	strcpy(attrMods[0]->modAttr.attrName, "failovers" );
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	int value=60;
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(&value);
    int bResult=getCommandhandler()->modify(0,0,"networkSurveillanceMId=1",attrMods);
    ASSERTM("Modification for NSF_FAIL_OVERS is failed",bResult==0);

}
/*========================================================
 *  ROUTINE:vModifyActiveFlag()
 *========================================================*/

void Cute_NSF_Command_Handler::vModifyActiveFlag()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      		Modify call back for NSF_ACTIVE_FLAG			 "<<endl;
	cout<<endl<<"************************************************************"<<endl;

	ACS_APGCC_AttrModification *attrMods[0];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[0]->modAttr.attrName = new char[50];
	strcpy(attrMods[0]->modAttr.attrName, "activeFlag" );
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	int value=1;
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(&value);
    int bResult=getCommandhandler()->modify(0,0,"networkSurveillanceMId=1",attrMods);
    ASSERTM("Modification for NSF_ACTIVE_FLAG is failed",bResult==0);

}
/*========================================================
 *  ROUTINE:vModifyFailoverPriority()
 *========================================================*/

void Cute_NSF_Command_Handler::vModifyFailoverPriority()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      		Modify call back for NSF_FAILOVER_PRIORITY   	 "<<endl;
	cout<<endl<<"************************************************************"<<endl;

	ACS_APGCC_AttrModification *attrMods[0];
	attrMods[0] = new ACS_APGCC_AttrModification();
	attrMods[0]->modAttr.attrName = new char[50];
	strcpy(attrMods[0]->modAttr.attrName, "failoverPriority" );
	attrMods[0]->modAttr.attrValuesNum = 1;
	attrMods[0]->modAttr.attrValues =  new void *[1];
	int value=0;
	attrMods[0]->modAttr.attrValues[0] = reinterpret_cast<void*>(&value);
    int bResult=getCommandhandler()->modify(0,0,"networkSurveillanceMId=1",attrMods);
    ASSERTM("Modification for NSF_FAILOVER_PRIORITY is failed",bResult==0);

}

void Cute_NSF_Command_Handler::vvalidateChange()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   Validate the Changes  for ping period             "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bool bResult=getCommandhandler()->validateChange(60,"pingPeriod");
	cout<<endl<<"The Returncode for validateChange for pingperiod : "<<bResult<<endl;
	ASSERTM("Validation for the pingperiod is failed",bResult==true);

	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   Validate the Changes  for RouterResponse             "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bResult=getCommandhandler()->validateChange(5,"routerResponse");
	cout<<endl<<"The Returncode for validateChange for RouterResponse : "<<bResult<<endl;
	ASSERTM("Validation for the RouterResponse is failed",bResult==true);


	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   Validate the Changes  for ResetTime             "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bResult=getCommandhandler()->validateChange(60,"resetTime");
	cout<<endl<<"The Returncode for validateChange for ResetTime : "<<bResult<<endl;
	ASSERTM("Validation for the ResetTime is failed",bResult==true);



	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   Validate the Changes  for Failovers             "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bResult=getCommandhandler()->validateChange(60,"failovers");
	cout<<endl<<"The Returncode for validateChange for Failovers : "<<bResult<<endl;
	ASSERTM("Validation for the Failovers is failed",bResult==true);


	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   Validate the Changes  for ActiveFlag             "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bResult=getCommandhandler()->validateChange(0,"activeFlag");
	cout<<endl<<"The Returncode for validateChange for ActiveFlag : "<<bResult<<endl;
	ASSERTM("Validation for the ActiveFlag is failed",bResult==true);


	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   Validate the Changes  for FailoverPriority             "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bResult=getCommandhandler()->validateChange(0,"failoverPriority");
	cout<<endl<<"The Returncode for validateChange for FailoverPriority : "<<bResult<<endl;
	ASSERTM("Validation for the FailoverPriority is failed",bResult==true);

}




void Cute_NSF_Command_Handler::vvalidateChangeNegative()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   Validate the Changes  for ping period             "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bool bResult=getCommandhandler()->validateChange(400,"pingPeriod");
	cout<<endl<<"The Returncode for validateChange for pingperiod : "<<bResult<<endl;
	ASSERTM("Validation for the pingperiod is failed",bResult==false);

	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   Validate the Changes  for RouterResponse             "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bResult=getCommandhandler()->validateChange(20,"routerResponse");
	cout<<endl<<"The Returncode for validateChange for RouterResponse : "<<bResult<<endl;
	ASSERTM("Validation for the RouterResponse is failed",bResult==false);


	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   Validate the Changes  for ResetTime             "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bResult=getCommandhandler()->validateChange(10,"resetTime");
	cout<<endl<<"The Returncode for validateChange for ResetTime : "<<bResult<<endl;
	ASSERTM("Validation for the ResetTime is failed",bResult==false);



	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   Validate the Changes  for Failovers             "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bResult=getCommandhandler()->validateChange(65537,"failovers");
	cout<<endl<<"The Returncode for validateChange for Failovers : "<<bResult<<endl;
	ASSERTM("Validation for the Failovers is failed",bResult==false);


	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   Validate the Changes  for ActiveFlag             "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bResult=getCommandhandler()->validateChange(3,"activeFlag");
	cout<<endl<<"The Returncode for validateChange for ActiveFlag : "<<bResult<<endl;
	ASSERTM("Validation for the ActiveFlag is failed",bResult==false);


	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   Validate the Changes  for FailoverPriority             "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bResult=getCommandhandler()->validateChange(4,"failoverPriority");
	cout<<endl<<"The Returncode for validateChange for FailoverPriority : "<<bResult<<endl;
	ASSERTM("Validation for the FailoverPriority is failed",bResult==false);

}
void Cute_NSF_Command_Handler::vvalidateNSValue()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   			Validate the NS Value	                 "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	string myErrorString = "";
	unsigned int myErrorCode =0;
	std::string myExpectedString;
	bool bResult = false;
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   			Setting setStartStopValue to 0	         "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	objCommon->setStartStopValue(0);
	bResult=getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString);
	cout<<endl<<"The Return code for validateNSValue when  aNetworkSurvellienceValue(0) :"<<bResult<<endl;
	ASSERTM("getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString) return value is failed",bResult==false);
	ASSERTM("getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString) error code is failed",myErrorCode==0);
	myExpectedString = "Network surveillance already stopped";
	bResult = false;
	bResult = (myExpectedString==myErrorString);
	if (bResult)
	{
		cout<<endl<<"Both the String are same"<<endl;
	}
	ASSERTM("getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString) error string is failed",bResult);




	bResult=getCommandhandler()->validateNSValue(1,myErrorCode,myErrorString);
	cout<<endl<<"The Return code for validateNSValue when  aNetworkSurvellienceValue(1) :"<<bResult<<endl;
	ASSERTM("getCommandhandler()->validateNSValue(1,myErrorCode,myErrorString) return value is failed",bResult==false);
	ASSERTM("getCommandhandler()->validateNSValue(1,myErrorCode,myErrorString) error code is failed",myErrorCode==1);
	myExpectedString = "Network surveillance not active. Cannot start";
	bResult = false;
	bResult = (myExpectedString==myErrorString);
	if (bResult)
	{
		cout<<endl<<"Both the String are same"<<endl;
	}
	ASSERTM("getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString) error string is failed",bResult);

	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   			Setting setStartStopValue to 1	         "<<endl;
	cout<<endl<<"************************************************************"<<endl;

	objCommon->setStartStopValue(1);
	bResult=getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString);
	cout<<endl<<"The Return code for validateNSValue when  aNetworkSurvellienceValue(0) :"<<bResult<<endl;
	ASSERTM("getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString) return value is failed",bResult==false);
	ASSERTM("getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString) error code is failed",myErrorCode==0);
	myExpectedString = "Network surveillance already stopped";
	bResult = false;
	bResult = (myExpectedString==myErrorString);
	if (bResult)
	{
		cout<<endl<<"Both the String are same"<<endl;
	}
	ASSERTM("getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString) error string is failed",bResult);


	bResult=getCommandhandler()->validateNSValue(1,myErrorCode,myErrorString);
	cout<<endl<<"The Return code for validateNSValue when  aNetworkSurvellienceValue(1) :"<<bResult<<endl;
	ASSERTM("getCommandhandler()->validateNSValue(1,myErrorCode,myErrorString) return value is failed",bResult==true);
	ASSERTM("getCommandhandler()->validateNSValue(1,myErrorCode,myErrorString) error code is failed",myErrorCode==0);
	myExpectedString = "Network surveillance started";
	bResult = false;
	bResult = (myExpectedString==myErrorString);
	if (bResult)
	{
		cout<<endl<<"Both the String are same"<<endl;
	}
	ASSERTM("getCommandhandler()->validateNSValue(1,myErrorCode,myErrorString) error string is failed",bResult);

	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   			Setting setStartStopValue to 2	         "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	objCommon->setStartStopValue(2);
	bResult=getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString);
	cout<<endl<<"The Return code for validateNSValue when  aNetworkSurvellienceValue(0) :"<<bResult<<endl;
	ASSERTM("getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString) return value is failed",bResult==true);
	ASSERTM("getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString) error code is failed",myErrorCode==0);
	myExpectedString = "Network surveillance stopped";
	bResult = false;
	if (bResult)
	{
		cout<<endl<<"Both the String are same"<<endl;
	}
	bResult = (myExpectedString==myErrorString);
	ASSERTM("getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString) error string is failed",bResult);

	myErrorCode =1;
	bResult=getCommandhandler()->validateNSValue(1,myErrorCode,myErrorString);
	cout<<endl<<"The Return code for validateNSValue when  aNetworkSurvellienceValue(1) :"<<bResult<<endl;
	ASSERTM("getCommandhandler()->validateNSValue(1,myErrorCode,myErrorString) return value is failed",bResult==false);
	ASSERTM("getCommandhandler()->validateNSValue(1,myErrorCode,myErrorString) error code is failed",myErrorCode==1);
	myExpectedString = "Network surveillance not active. Cannot start";
	bResult = false;
	bResult = (myExpectedString==myErrorString);
	if (bResult)
	{
		cout<<endl<<"Both the String are same"<<endl;
	}
	ASSERTM("getCommandhandler()->validateNSValue(1,myErrorCode,myErrorString) error string is failed",bResult);
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      	   			Setting setStartStopValue to 3	         "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	myErrorCode =0;
	objCommon->setStartStopValue(3);
	bResult=getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString);
	cout<<endl<<"The Return code for validateNSValue when  aNetworkSurvellienceValue(0) :"<<bResult<<endl;
	ASSERTM("getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString) return value is failed",bResult==true);
	ASSERTM("getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString) error code is failed",myErrorCode==0);
	myExpectedString = "Network surveillance stopped";
	bResult = false;
	bResult = (myExpectedString==myErrorString);
	if (bResult)
	{
		cout<<endl<<"Both the String are same"<<endl;
	}
	ASSERTM("getCommandhandler()->validateNSValue(0,myErrorCode,myErrorString) error string is failed",bResult);

	objCommon->setStartStopValue(3);
	bResult=getCommandhandler()->validateNSValue(1,myErrorCode,myErrorString);
	cout<<endl<<"The Return code for validateNSValue when  aNetworkSurvellienceValue(1) :"<<bResult<<endl;
	ASSERTM("getCommandhandler()->validateNSValue(1,myErrorCode,myErrorString) return value is failed",bResult==false);
	ASSERTM("getCommandhandler()->validateNSValue(1,myErrorCode,myErrorString) error code is failed",myErrorCode==0);
	myExpectedString = "Network surveillance already started";
	bResult = false;
	bResult = (myExpectedString==myErrorString);
	if (bResult)
	{
		cout<<endl<<"Both the String are same"<<endl;
	}
	ASSERTM("getCommandhandler()->validateNSValue(1,myErrorCode,myErrorString) error string is failed",bResult);

}
void Cute_NSF_Command_Handler::init()
{
	string nsfRootRdnname = "networkSurveillanceMId=1";
	string NetworkImplName = "NetworkImplmenter";
	objCommandHandler = new ACS_NSF_CommandHandler(objCommon,nsfRootRdnname,NetworkImplName,ACS_APGCC_ONE);
}



/*========================================================
 * ROUTINE:make_suite_Cute_NSF_Biostime_Recovery()
 *========================================================*/
cute::suite Cute_NSF_Command_Handler::make_suite_Cute_NSF_Command_Handler()
{
	cute::suite s;
	s.push_back(CUTE(vvalidateChange));
	s.push_back(CUTE(vvalidateChangeNegative));
	s.push_back(CUTE(vModifySurvellienceflag));
	s.push_back(CUTE(vModifyNumberFailover));
	s.push_back(CUTE(vModifyRouterResponse));
	s.push_back(CUTE(vModifyPingPeriod));
	s.push_back(CUTE(vModifyResetTime));
	s.push_back(CUTE(vModifyFailovers));
	s.push_back(CUTE(vModifyActiveFlag));
	s.push_back(CUTE(vModifyFailoverPriority));
	s.push_back(CUTE(vvalidateNSValue));

	s.push_back(CUTE(vCommandHandlerCleanup));
	return s;
}


void Cute_NSF_Command_Handler::vCommandHandlerCleanup()
{
	delete objCommandHandler;
	objCommandHandler = 0;
}

