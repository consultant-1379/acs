/*=================================================================== */
   /**
   @file API_DSD_Server.cpp

   Class method implementation for DSD module.

   This module contains the implementation of class declared in
   the API_DSD_Server.h module

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
#include "API_DSD_Server.h"
#include "API_DSD_Session.h"
#include "global.h"


using namespace std;
using namespace DSD_API;

ACS_JTP_CriticalSection g_reg;
/*===================================================================
   ROUTINE: ACS_DSD_Server
=================================================================== */
ACS_DSD_Server::ACS_DSD_Server(DSD_API::ServiceMode sMode)
:ACS_DSD_API_Base(),
svcMode(sMode),
mServiceName(0),
mServiceDomain(0)
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_Server::ACS_DSD_Server\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("ACS_DSD_Server object created successfully\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Server::ACS_DSD_Server\n")));
}

/*===================================================================
   ROUTINE: ~ACS_DSD_Server
=================================================================== */
ACS_DSD_Server::~ACS_DSD_Server()
{
	if( mServiceName != 0)
	{
		delete[] mServiceName;
		mServiceName= 0;
	}

	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_Server::~ACS_DSD_Server()\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("ACS_DSD_Server object destroyed successfully.\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Server::~ACS_DSD_Server()\n")));
}

/*===================================================================
   ROUTINE: open
=================================================================== */
bool ACS_DSD_Server::open(const ACE_TCHAR* serviceId)
{
//	ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_Server::open\n")));

	//14000 - Port CP connects to
	//14001 - communication between the applications running on the same host.
	//14002 - Inter AP communication.

	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("ACS_DSD_Server opened successfully.\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Server::open\n")));
	return true;
}

/*===================================================================
   ROUTINE: open with ServiceMode
=================================================================== */
bool ACS_DSD_Server::open(DSD_API::ServiceMode serviceMode, const ACE_TCHAR* serviceId )
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_Server::open\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("ACS_DSD_Server opened successfully.\n")));

	//Un comment the following code and comment out the rest of the code to
	//test error condition.
	//It could be the case, some error happens while creating the resources.
	//setError(601, INTERNAL_ERROR);
	//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Error happened in ACS_DSD_Server::open\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Server::open\n")));
	//return false;

	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Server::open\n")));
	return true;
}

/*===================================================================
   ROUTINE: publish
=================================================================== */
bool ACS_DSD_Server::publish(const ACE_TCHAR* serviceName, const ACE_TCHAR* serviceDomain, bool isLocal )
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_Server::publish\n")));

	if ( serviceName != 0 && (ACE_OS::strlen(serviceName) < MAX_NAME_LENGTH )
			&& serviceDomain != 0 && (ACE_OS::strlen(serviceDomain) < MAX_DOMAIN_LENGTH) )
	{

		mServiceName = new (std::nothrow) ACE_TCHAR[strlen(serviceName)+1];
		if(mServiceName != 0)
		{
			ACE_OS::strcpy(mServiceName,serviceName);
			mServiceName[strlen(serviceName)] = '\0';
		}
		//Check if service is already registered.
		//if yes, return true.
		{
			ACS_JTP_CriticalSectionGuard _(g_reg);
			std::fstream fp1;
			fp1.open("register.txt",ios::in);
			if( fp1 != NULL)
			{
				while(!fp1.eof())
				{
					ACE_TCHAR str[MAX_NAME_LENGTH] ={0};
					fp1.getline(str,MAX_NAME_LENGTH,'\n');
					if( ACE_OS::strcmp((serviceName),str) == 0 )
					{
						return true;
					}
				}
			}
			fp1.close();

			std::fstream fp("register.txt",ios::out | ios::app);
			if( fp != NULL)
			{
				fp<<serviceName<<"\n";
			}

			fp.close();
		}
	//	ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("%s with Service Domain %s is successfully "
	//			"registered in Service Registration Table\n"),	serviceName, serviceDomain));
	//	ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Server::publish\n")));
		return true;
	}
	else
	{

		setError(301, ILLEGAL_USE_API);
//	ACE_DEBUG ((LM_TRACE, ACE_TEXT ("ERROR: Either Service Name or Service Domain is null or exceeds limit.\n")));
//	ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Server::publish\n")));
		return false;
	}

}


/*===================================================================
   ROUTINE: accept
=================================================================== */
bool ACS_DSD_Server::accept(ACS_DSD_Session& session, ACE_UINT32 timeout )
{
	ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_Server::accept\n")));

	//There could be an error in accept.
	//setError(604,INTERNAL_ERROR);
	//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Error occurred in Accept()\n)));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Server::accept\n")));
	//return false;
	ACE_OS::sleep(1);
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Accept successful.\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Server::accept\n")));
	return true;
}

/*===================================================================
   ROUTINE: close
=================================================================== */
void ACS_DSD_Server::close(void)
{
	{
		ACS_JTP_CriticalSectionGuard _(g_reg);

		std::fstream fp("register.txt",ios::in);
		vector<string> serviceNameVect;
		if( fp != NULL)
		{
			while(!fp.eof())
			{
				ACE_TCHAR str[MAX_NAME_LENGTH] ={0};
				fp.getline(str,MAX_NAME_LENGTH,'\n');
				if( str != 0 &&  mServiceName != 0 &&
						ACE_OS::strlen(str) > 0  && ACE_OS::strcmp(mServiceName,str) != 0 )
				{
					serviceNameVect.push_back(str);
				}
			}
		}
		fp.close();

		truncate("register.txt",0);

		fp.open("register.txt", ios::out );
		if( fp != NULL)
		{
			fp.seekp(0, ios::beg);
			for( ACE_UINT32 i =0 ; i<serviceNameVect.size(); i++)
			{
				fp<<serviceNameVect[i]<<"\n";
				fp.flush();
			}

		}

		fp.close();
	}
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_Server::close(void)\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Service registration is closed and successfully unregistered.\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_Server::close(void)\n")));

}
