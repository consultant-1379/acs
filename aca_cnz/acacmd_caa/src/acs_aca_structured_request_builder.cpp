#include "acs_aca_structured_request_builder.h"

//------------------------------------------------------------------------------
//      Constructor
//------------------------------------------------------------------------------
ACS_ACA_StructuredRequestBuilder::ACS_ACA_StructuredRequestBuilder(void)
{
	msg = new StructuredRequest();
}

//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
ACS_ACA_StructuredRequestBuilder::~ACS_ACA_StructuredRequestBuilder(void)
{
	if (msg)
		delete(msg);
}

//------------------------------------------------------------------------------
// build msdls request:
//		Set the msdls command options
//------------------------------------------------------------------------------
void ACS_ACA_StructuredRequestBuilder::setOptions(unsigned short optmask)
{
	msg->setOptions(optmask);
}

//------------------------------------------------------------------------------
// build msdls request:
//		Set the CP source
//------------------------------------------------------------------------------
void ACS_ACA_StructuredRequestBuilder::setCPSource(const char* cpsource)
{
	msg->setCPSource(cpsource);
}

//------------------------------------------------------------------------------
// build message request:
//		Set the Message Store name
//------------------------------------------------------------------------------
void ACS_ACA_StructuredRequestBuilder::setMSGStoreName(const char* msname)
{
	msg->setMSGStoreName(msname);
}

//------------------------------------------------------------------------------
//		Set request message size
//------------------------------------------------------------------------------
void ACS_ACA_StructuredRequestBuilder::setSize(unsigned int& size)
{
	msg->SetSize(size);
}

//------------------------------------------------------------------------------
//		Set file name
//------------------------------------------------------------------------------
void ACS_ACA_StructuredRequestBuilder::setData(const void* data)
{
	msg->setData(data);
}

//------------------------------------------------------------------------------
//		Get request built
//------------------------------------------------------------------------------
void* ACS_ACA_StructuredRequestBuilder::getMessage()
{
	void* req = NULL;
	if (msg != NULL) 
		req = msg->getData();
	return req;
}

void ACS_ACA_StructuredRequestBuilder::setCpId(unsigned short cpID)
{
	msg->setCpId(cpID);
}
