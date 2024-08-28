/*=================================================================== */
   /**
   @file API_DSD_Client.cpp

   Class method implementation for DSD module.

   This module contains the implementation of class declared in
   the API_DSD_Client.h module

   @version N.N.N

   @documentno CAA 109 0870

   @copyright Ericsson AB, Sweden 2010. All rights reserved.
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       10/08/2010     TA       Initial Release
   **/
/*=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "API_DSD_Client.h"
#include "API_DSD_Session.h"
#include "global.h"

using namespace std;
using namespace DSD_API;

/*===================================================================
   ROUTINE: ACS_DSD_Client
=================================================================== */
ACS_DSD_Client::ACS_DSD_Client()
:ACS_DSD_API_Base()
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_Client::ACS_DSD_Client()\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("ACS_DSD_Client object created successfully.\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Client::ACS_DSD_Client()\n")));
}

/*===================================================================
   ROUTINE: ~ACS_DSD_Client
=================================================================== */
ACS_DSD_Client::~ACS_DSD_Client()
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_Client::~ACS_DSD_Client()\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("ACS_DSD_Client object destroyed successfully.\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Client::~ACS_DSD_Client()\n")));
}

/*===================================================================
   ROUTINE: query
=================================================================== */
bool ACS_DSD_Client::query(const ACE_TCHAR * serviceName,
			   const ACE_TCHAR * serviceDomain,
			   DSD_API::SystemType scope,
			   std::vector<DSD_API::ACS_DSD_Node>& reachableNodes,
			   std::vector<DSD_API::ACS_DSD_Node>& unReachableNodes,
			   ACE_UINT32 milliseconds)
{
//	ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_Client::query\n")));

	if (ACE_OS::strlen(serviceName) > MAX_NAME_LENGTH || ACE_OS::strlen(serviceDomain) >MAX_DOMAIN_LENGTH )
	{
		//Set error.
		setError(310, ILLEGAL_USE_API);
//		ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Either service name or service domain length exceeds allowed limit\n")));
//		ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Client::query\n")));
		return false;
	}

	//Un comment and comment remaining code to test failure conditions.
	//DSD host in unreachable, send error.
	//setError(610, INTERNAL_ERROR);
	//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Unable to connect to remote DSD node\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Client::query\n")));
	//return false;

	//Create a node object.
	DSD_API::ACS_DSD_Node reachableNode;
	reachableNode.nState = DSD_API::NODE_STATE_ACTIVE;
	ACE_OS::strcpy(reachableNode.nodeName, "TEST NODE1");
	reachableNode.sysType = DSD_API::SYSTEM_TYPE_CP;
	reachableNode.systemId = DSD_API::CLOCK_MASTER;

	//Add to the vector
	reachableNodes.push_back(reachableNode);

	for( ACE_UINT32 itr = 0 ; itr < reachableNodes.size(); itr++)
	{
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("\n Printing Reachable Nodes Details\n")));
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Node State:\t%d\n"),reachableNodes[itr].nState));
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Node Name:\t%s\n"), (reachableNodes[itr].nodeName)));
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Node System Type:\t%d\n"),reachableNodes[itr].sysType));
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Node System Id:\t%d\n"), reachableNodes[itr].systemId));
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("*********************************\n")));
	}
	//Create a node object for unreachable nodes.

	DSD_API::ACS_DSD_Node unReachableNode;
	unReachableNode.nState = DSD_API::NODE_STATE_ACTIVE;
	ACE_OS::strcpy(unReachableNode.nodeName, "TEST NODE2");
	unReachableNode.sysType = DSD_API::SYSTEM_TYPE_CP;
	unReachableNode.systemId = DSD_API::CLOCK_MASTER;

	unReachableNodes.push_back(unReachableNode);

	for( ACE_UINT32 itr = 0 ; itr < unReachableNodes.size(); itr++)
	{
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("\n Printing unReachable Nodes Details\n")));
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Node State:\t%d\n"),unReachableNodes[itr].nState));
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Node Name:\t%s\n"), (unReachableNodes[itr].nodeName)));
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Node System Type:\t%d\n"),unReachableNodes[itr].sysType));
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Node System Id:\t%d\n"), unReachableNodes[itr].systemId));
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("*********************************\n")));
	}
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Client::query\n")));
	return true;

}

/*===================================================================
   ROUTINE: connect
=================================================================== */
bool ACS_DSD_Client::connect(ACS_DSD_Session& session,
				const ACE_TCHAR * serviceName,
				const ACE_TCHAR * serviceDomain,
				const DSD_API::ACS_DSD_Node* node)
{
	ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_Client::connect\n")));

	if ((ACE_OS::strlen(serviceName) > MAX_NAME_LENGTH) || (ACE_OS::strlen(serviceDomain) >MAX_DOMAIN_LENGTH ))
	{
		//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("ServiceName = %s\n"), serviceName));
		//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("ServiceDomain = %s\n"), serviceDomain));
		//Set error.
		setError(311, ILLEGAL_USE_API);
		//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Either service name or service domain length exceeds allowed limit\n")));
		//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Client::connect\n")));
		return false;
	}

	std::fstream fp;
	fp.open("register.txt",ios::in);
	if( fp != NULL)
	{
		fp.seekp(0);
		while(!fp.eof())
		{
			ACE_TCHAR str[MAX_NAME_LENGTH] ={};
			fp.getline(str,MAX_NAME_LENGTH,'\n');
			if( ACE_OS::strcmp((serviceName),str) == 0 )
			{
		//		ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Connection with established successfully\n")));
		//		ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Client::connect\n")));
				fp.close();
				return true;

			}
		}
	}
	fp.close();
	return false;
}



