//******************************************************************************
//
//  NAME
//     acs_lm_jtp_service_R3.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//	    190 89-CAA nnn nnnn
//
//  AUTHOR
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_lm_jtp_service_R3.h"
#include <iostream>
using namespace std;

#if 0
ACS_LM_DefineTrace(ACS_LM_JTP_Service_R3);
#endif
/*=================================================================
	ROUTINE: ACS_LM_JTP_Service_R3 constructor
=================================================================== */
ACS_LM_JTP_Service_R3::ACS_LM_JTP_Service_R3()
:jtpService(NULL)
{
#if 0
	ACS_LM_PrintTrace(ACS_LM_JTP_Service_R3,"ACS_LM_JTP_Service_R3()");
#endif
	//cout<<"ACS_LM_JTP_Service_R3()"<<endl;

}//end of constructor
/*=================================================================
	ROUTINE: ACS_LM_JTP_Service_R3 destructor
=================================================================== */
ACS_LM_JTP_Service_R3::~ACS_LM_JTP_Service_R3()
{
#if 0
	ACS_LM_PrintTrace(ACS_LM_JTP_Service_R3,"~ACS_LM_JTP_Service_R3()");
#endif
	//cout<<"~ACS_LM_JTP_Service_R3()"<<endl;
}//end of destructor
/*=================================================================
	ROUTINE: getHandles
=================================================================== */
void ACS_LM_JTP_Service_R3::getHandles(int& noOfHandles, ACE_HANDLE*& handles)
{
	jtpService->getHandles(noOfHandles, handles);
}//end of getHandles
/*=================================================================
	ROUTINE: registerService
=================================================================== */
bool ACS_LM_JTP_Service_R3::registerService(const std::string &apService)
{
#if 0
	ACS_LM_PrintTrace(ACS_LM_JTP_Service_R3, "registerService()");
#endif
	//cout<<"In registerService method"<<endl;
	bool registered = false;
	if(jtpService == NULL)
	{
		jtpService = new ACS_JTP_Service_R3A((char*)apService.c_str());
		if ( jtpService != NULL)
		{
			if(!jtpService->jidrepreq())
			{
#if 0
				ACS_LM_PrintTrace(ACS_LM_JTP_Service_R3, "registerService() jtpService->jidrepreq() failed");
#endif
				//cout<<"registerService() jtpService->jidrepreq() failed "<<endl;
				delete jtpService;
				jtpService=NULL;
				return registered;
			}
			else
			{
				registered = true;
			}
		}
		else
		{
			registered = false;
		}
	}
	else
	{
		registered = true;
	}
#if 0
	ACS_LM_PrintTrace(ACS_LM_JTP_Service_R3, "registerService() registered="<<registered);
#endif
	//cout<<"registerService() registered = "<<registered<<endl;

	return registered;
}//end of registerService
/*=================================================================
	ROUTINE: accept
=================================================================== */
JtpNode* ACS_LM_JTP_Service_R3::accept()
{
#if 0
	ACS_LM_PrintTrace(ACS_LM_JTP_Service_R3, "accept()");
#endif
	JtpNode* jtpNode = NULL;

	ACS_JTP_Job_R3A* jtpJob = new ACS_JTP_Job_R3A();

	if(jtpService->accept(jtpJob, 0))
	{
		if(jtpJob->State() == ACS_JTP_Job_R3A::StateConnected)
		{
			unsigned short ud1 = 0;
			unsigned short ud2 = 0;

			unsigned short len = 512;
			char* msg;

			ACS_JTP_Job_R3A::JTP_Node node;
			if(jtpJob->jinitind(node, ud1, ud2, len, msg))
			{
				if(jtpJob->jinitrsp(ud1, ud2, 0))
				{
					jtpNode = new JtpNode();
					jtpNode->id = node.system_id;
					jtpNode->nodeName = node.node_name;
					if(node.system_type == (ACS_JTP_Job_R3A::JTP_SystemType)ACS_JTP_Conversation_R3A::SYSTEM_TYPE_BC)
					{
						jtpNode->type = ACS_LM_JTP_NODE_TYPE_BC;
					}
					else
					{
						jtpNode->type = ACS_LM_JTP_NODE_TYPE_CP;
					}
					unsigned short ret = 0;
					if(jtpJob->jresultreq(ud1,ud2,ret,len,msg))
					{
						//DEBUG ("%s","jresultreq send is success");
					}
				}
			}
		}
	}

	delete jtpJob;

	if(jtpNode != NULL)
	{
#if 0
		ACS_LM_PrintTrace(ACS_LM_JTP_Service_R3, "accept() jtpNode->id="<<jtpNode->id);
#endif
		//cout<<"accept() jtpNode->id = "<<jtpNode->id<<endl;
	}
	else
	{
#if 0
		ACS_LM_PrintTrace(ACS_LM_JTP_Service_R3, "accept() jtpNode == NULL");
#endif
		//cout<<"accept() jtpNode->id = NULL"<<endl;
	}
	return jtpNode;
}//end of accept
