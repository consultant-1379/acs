//	Copyright Ericsson AB 2007. All rights reserved.

// ACS_CS_TableHandler
#include "ACS_CS_TableHandler.h"
#include "ACS_CS_TableRequest.h"

#include "ACS_CS_Trace.h"


ACS_CS_Trace_TDEF(ACS_CS_TableHandler_TRACE);

// Class ACS_CS_TableHandler 
clockMap ACS_CS_TableHandler::globalClockMap;
ACE_RW_Mutex ACS_CS_TableHandler::glcMutex;

ACS_CS_TableHandler::ACS_CS_TableHandler() :
		logicalClock(0)
{  

}


ACS_CS_TableHandler::~ACS_CS_TableHandler()
{

}


int ACS_CS_TableHandler::newTableOperationRequest(ACS_CS_ImModelSubset */* subset */)
{
    ACS_CS_TRACE((ACS_CS_TableHandler_TRACE,
            "ACS_CS_TableHandler::newTableOperationRequest()\n"
            "No implementation, exiting from base class!"));

    return 0;   // Should be implemented in the inherited classes
}

 void ACS_CS_TableHandler::setGlobalLogicalClock ( ACS_CS_Protocol::CS_Scope_Identifier scope, uint16_t clockValue)
{
     (void) glcMutex.acquire_write();
        //globalClockMap.insert(std::pair<ACS_CS_Protocol::CS_Scope_Identifier,uint16_t>(scope,clockValue));
		globalClockMap[scope]=clockValue;
	 (void) glcMutex.release();
	 
     ACS_CS_TRACE((ACS_CS_TableHandler_TRACE,"ACS_CS_TableHandler::setGlobalLogicalClockMap() scope = %d  \n",scope)); 
		     ACS_CS_TRACE((ACS_CS_TableHandler_TRACE,"ACS_CS_TableHandler::setGlobalLogicalClockMap()\n clock_val = %d  \n",clockValue));
		     ACS_CS_TRACE((ACS_CS_TableHandler_TRACE,"ACS_CS_TableHandler::setGlobalLogicalClockMap()\n clock = %d  \n",globalClockMap[scope]));
}

 clockMap ACS_CS_TableHandler::getGlobalLogicalClockMap()
{
   (void) glcMutex.acquire_read();
	clockMap tempMap = ACS_CS_TableHandler::globalClockMap;
   (void) glcMutex.release();
   
    return tempMap;
}
