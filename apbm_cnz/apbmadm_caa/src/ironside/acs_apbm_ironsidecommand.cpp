
#include "acs_apbm_ironsidecommand.h"
#include "acs_apbm_logger.h"
#include <stdio.h>
#include <stdlib.h>

//typedef std::map <std::string,std::string> nresponse;


acs_apbm_ironsidecommand::acs_apbm_ironsidecommand(char target_a[16], char target_b[16]):
			_sender(0),
			commandBuilder(target_a, target_b)
{

}

acs_apbm_ironsidecommand::~acs_apbm_ironsidecommand() {

	if (_sender)
		delete(_sender);
}

void acs_apbm_ironsidecommand::setSender(acs_apbm_sender* send)
{
	if (_sender)
	{
		delete _sender;
		_sender = 0;
	}

	_sender = send;
}

const char *acs_apbm_ironsidecommand::getErrorMessage()
{
	return message_error;
}

int acs_apbm_ironsidecommand::getErrorType()
{
	return message_error_type;
}


int acs_apbm_ironsidecommand::getErrorSeverity()
{
	return message_error_severity;
}

const char* acs_apbm_ironsidecommand::getErrorMessageInfo()
{
	return message_error_info;
}

int acs_apbm_ironsidecommand::executeUdp(std::string udp_filter,std::string &udp_data)
{
   int result=0;
   ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Entering Method %s", __FUNCTION__);
   cout << "acs_apbm_ironsidecommand::executeUDP : Entering Method"<<endl;

   std::string res("");

   result=_sender->sendUdpMessage(udp_filter);

 ACS_APBM_LOG(LOG_LEVEL_DEBUG, "result %d",result);
   if(result==0)
   {
     udp_data=_sender->getUdpData();
   }
	
   return result;

}
