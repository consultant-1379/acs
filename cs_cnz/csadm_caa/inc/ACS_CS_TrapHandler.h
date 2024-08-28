
#ifndef ACS_CS_TRAPHANDLER_H_
#define ACS_CS_TRAPHANDLER_H_ 123


#include "ACS_CS_Event.h"
#include "ACS_CS_Util.h"
#include "ACS_TRA_trace.h"
#include "ACS_CS_Thread.h"


class ACS_CS_ServiceHandler;
class ACS_CS_SNMPTrapReceiver;

//#include "ACS_CS_SNMPTrapReceiver.h"
//#include "ACS_CS_ServiceHandler.h"



class ACS_CS_TrapHandler : public ACS_CS_Thread
{

public:

	ACS_CS_TrapHandler();

	ACS_CS_TrapHandler(ACS_CS_ServiceHandler *service);

	virtual ~ACS_CS_TrapHandler();

	int exec();

	ACS_CS_ServiceHandler * getServiceHandler();

private:

    void printTRA(std::string mesg);

    ACS_CS_ServiceHandler *service;
    ACS_CS_SNMPTrapReceiver *trapManager;
    ACS_TRA_trace *_trace;

};

#endif /* ACS_CS_TRAPHANLER_H_ */
