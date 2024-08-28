#ifndef ACS_CS_SECCallbackThread_h_
#define ACS_CS_SECCallbackThread_h_ 1

#include "ACS_CS_Thread.h"
#include "ACS_CS_SecApi.h"
#include "ACS_CS_LdapProxyHandler.h"

class ACS_CS_SECCallbackThread : public ACS_CS_Thread
{
public:
	ACS_CS_SECCallbackThread();
	~ACS_CS_SECCallbackThread();

	virtual int exec ();

private:
	ACS_CS_SecApi *ldapSecApi;
	ACS_CS_LdapProxyHandler *ldapProxyHandler;			//TR HW67630

};

#endif // ACS_CS_SecCallbackThread_h_

