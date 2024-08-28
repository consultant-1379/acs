/*=================================================================== */
   /**
   @file API_DSD_Base.cpp

   Class method implementation for DSD module.

   This module contains the implementation of class declared in
   the API_DSD_Base.h module

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
#include "API_DSD_Base.h"
#include "global.h"

using namespace std;
using namespace DSD_API;

/*===================================================================
   ROUTINE: ACS_DSD_API_Base
=================================================================== */
ACS_DSD_API_Base::ACS_DSD_API_Base()
:error(0),
 errorText(0)
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_API_Base::ACS_DSD_API_Base()\n")));
	ACE_HANDLE h1 = ACE_INVALID_HANDLE;
	ACE_HANDLE h2 = ACE_INVALID_HANDLE;
	internalHandleList[0]=h1;
	internalHandleList[1]=h2;
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("ACS_DSD_API_Base object created successfully.\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_API_Base::ACS_DSD_API_Base()\n")));
}

/*===================================================================
   ROUTINE: ~ACS_DSD_API_Base
=================================================================== */
ACS_DSD_API_Base::~ACS_DSD_API_Base()
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_API_Base::~ACS_DSD_API_Base()\n")));
	setError(0,0);
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_API_Base::~ACS_DSD_API_Base()\n")));
}

/*===================================================================
   ROUTINE: getNode
=================================================================== */
void ACS_DSD_API_Base::getNode(DSD_API::ACS_DSD_Node& node , DSD_API::NodeSide nodeSide)
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_API_Base::getNode(DSD_API::ACS_DSD_Node& node , DSD_API::NodeSide nodeSide)\n")));

	if(nodeSide == NODE_SIDE_LOCAL )
	{
		node.nState = DSD_API::NODE_STATE_ACTIVE;
		ACE_OS::strcpy(node.nodeName, "LOCAL NODE");
		node.sysType = DSD_API::SYSTEM_TYPE_AP;
		node.systemId = DSD_API::ALARM_MASTER;
	}
	else
	{
		node.nState = DSD_API::NODE_STATE_ACTIVE;
		ACE_OS::strcpy(node.nodeName, "REMOTE NODE");
		node.sysType = DSD_API::SYSTEM_TYPE_CP;
		node.systemId = DSD_API::ALARM_MASTER;

	}
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_API_Base::getNode(DSD_API::ACS_DSD_Node& node , DSD_API::NodeSide nodeSide)\n")));
}

/*===================================================================
   ROUTINE: getLastError
=================================================================== */
ACE_INT32 ACS_DSD_API_Base::getLastError(void) const
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_API_Base::getLastError(void)\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_API_Base::getLastError(void)\n")));
	return error;
}

/*===================================================================
   ROUTINE: getLastErrorText
=================================================================== */
const ACE_TCHAR* ACS_DSD_API_Base::getLastErrorText(void) const
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_API_Base::getLastErrorText(void)\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_API_Base::getLastErrorText(void)\n")));
	return errorText;
}

/*===================================================================
   ROUTINE: setError
=================================================================== */
void ACS_DSD_API_Base::setError(ACE_INT32 err, const ACE_TCHAR* errText)
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_API_Base::setError\n")));
	//Copy the error number
	error = err;

	//Store the last error.
	if(errorText != 0)
	{
		delete[] errorText;
		errorText = 0;
	}
	if( errText != 0)
	{
		errorText = new ACE_TCHAR( ACE_OS::strlen(errText) + 1);
		ACE_OS::strcpy(errorText, errText);
	}
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_API_Base::setError\n")));
}

/*===================================================================
   ROUTINE: getHandles
=================================================================== */
void ACS_DSD_API_Base::getHandles(ACE_HANDLE*& handles, ACE_INT32& handleCount)
{
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Entering ACS_DSD_API_Base::getHandles(ACE_HANDLE*& handles, ACE_INT32& handleCount)\n")));

	//There could be error in getHandles.
	//setError(605, INTERNAL_ERROR);
	//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Error occurred while getting handles\n")));
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_API_Base::getHandles(ACE_HANDLE*& handles, ACE_INT32& handleCount)\n")));
	//handles= &internalHandleList[0];
	//handleCount = 0;

	internalHandleList[0] = (ACE_HANDLE)50000;
	internalHandleList[1] = (ACE_HANDLE)50001;
	handles = &internalHandleList[0];
	handleCount = 2;
	//ACE_DEBUG ((LM_TRACE, ACE_TEXT ("Leaving ACS_DSD_API_Base::getHandles(ACE_HANDLE*& handles, ACE_INT32& handleCount)\n")));
}

