#include "acs_aca_structured_request.h"
#include <ace/ACE.h>

static ACS_TRA_trace ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace("ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace", "C512");

//------------------------------------------------------------------------------
//      Constructor
//------------------------------------------------------------------------------
StructuredRequest::StructuredRequest(void)
{
	msdls_req = (aca_msdls_req_t*)malloc(sizeof(aca_msdls_req_t));
	memset((void *)msdls_req, 0, sizeof(aca_msdls_req_t));
}

//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
StructuredRequest::~StructuredRequest(void)
{
	if (msdls_req)	
		free(msdls_req);
}

//------------------------------------------------------------------------------
//      Get request structure
//------------------------------------------------------------------------------
void* StructuredRequest::getData()
{
        if( ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering  StructuredRequest::getData()");
                ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }


	return (void*)msdls_req;
}

//------------------------------------------------------------------------------
//      Setters
//------------------------------------------------------------------------------
void StructuredRequest::setOptions(unsigned short optmask)
{
        if( ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering  StructuredRequest::setOptions()");
                ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

	msdls_req->optmask = optmask;
        if( ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving  StructuredRequest::setOptions()");
                ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

}

void StructuredRequest::setCPSource(const char* cpsource)
{
        if( ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering  StructuredRequest::setCPSource()");
                ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

	if (cpsource)
		memcpy(msdls_req->cpsource, cpsource, CPSOURCE_SIZE);
        if( ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving  StructuredRequest::setCPSource()");
                ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

}

void StructuredRequest::setMSGStoreName(const char* msname)
{
        if( ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering  StructuredRequest::setMSGStoreName()");
                ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

	if (msname)
		memcpy(msdls_req->msname, msname, MSGSTORE_SIZE);

        if( ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving  StructuredRequest::setMSGStoreName()");
                ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

}

void StructuredRequest::setData(const void* data)
{
        if( ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering  StructuredRequest::setData()");
                ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

	if (data)
		memcpy(msdls_req->data, data, DATA_SIZE);

        if( ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving  StructuredRequest::setData()");
                ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

}

void StructuredRequest::SetSize(unsigned int& ReqSize)
{
        if( ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering  StructuredRequest::SetSize()");
                ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

	ReqSize = sizeof(aca_msdls_req_t);
        if( ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving  StructuredRequest:SetSize:()");
                ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

}

void StructuredRequest::setCpId(unsigned short cpID)
{
        if( ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering  StructuredRequest::setCpId()");
                ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

	msdls_req->cpID = cpID;
        if( ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                ACE_OS::memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving  StructuredRequest:setCpId()");
                ACS_ACA_CMD_STRUCT_REQUEST_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

}
