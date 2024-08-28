#include "ACS_TRAPDS_Sender.h"

ACS_TRAPDS_Sender* ACS_TRAPDS_Sender::s_instance = 0;


ACS_TRAPDS_Sender::ACS_TRAPDS_Sender()
{
	local_addr_=new ACE_INET_Addr(MAX_PORT,"127.0.0.1");
	local_=new ACE_SOCK_Dgram(*local_addr_);
}

ACS_TRAPDS_Sender::~ACS_TRAPDS_Sender()
{
	    delete(local_addr_);
		delete(local_);
}

ACS_TRAPDS_Sender * ACS_TRAPDS_Sender::getInstance ()
{
	if (s_instance == 0)
	{
		s_instance = new (std::nothrow) ACS_TRAPDS_Sender();
	}
	return s_instance;

}

ACE_SOCK_Dgram *ACS_TRAPDS_Sender::getInetAddrSender()
{
	return local_;
}












