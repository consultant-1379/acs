
#include "ACS_CS_TrapHandler.h"
#include "ACS_CS_SNMPTrapReceiver.h"


ACS_CS_TrapHandler::ACS_CS_TrapHandler()
{
	// TODO Auto-generated constructor stub

}

ACS_CS_TrapHandler::~ACS_CS_TrapHandler()
{

	if(trapManager)
		trapManager->unsubscribe();

	delete(_trace);

}



ACS_CS_TrapHandler::ACS_CS_TrapHandler(ACS_CS_ServiceHandler *service)
{
	_trace = new ACS_TRA_trace("ACS_CS_TrapHandler");

	this->service=service;

	trapManager=new ACS_CS_SNMPTrapReceiver(this);

}

int ACS_CS_TrapHandler::exec()
{
	bool fl=false;

	ACS_CS_EventHandle shutdownEv = 0;

	ACS_CS_EventHandle validHandles[1];

	shutdownEv= ACS_CS_Event::OpenNamedEvent(ACS_CS_NS::EVENT_NAME_SHUTDOWN);

	validHandles[0]=shutdownEv;

	int validIndex=1;

	const int WAIT_FAILED=-1;
	const int WAIT_TIMEOUT= -2;

	while(!fl)
	{
		int status = ACS_CS_Event::WaitForEvents(validIndex,validHandles,2000);

		if (status == WAIT_FAILED)
		{
			char tmpStr[512] = {0};
			snprintf(tmpStr, sizeof(tmpStr) - 1, "[%s@%d] ACS_CS_TrapHandler event error no.", __FUNCTION__, __LINE__);
			printTRA(tmpStr);

		}
		else
		{

			if (status == WAIT_TIMEOUT)
			{
				if (trapManager->subscribe(ACS_CS_NS::ACS_CS_PROCESS_NAME) != ACS_TRAPDS::Result_Success)
				{
					char tmpStr[512] = {0};
					snprintf(tmpStr, sizeof(tmpStr) - 1, "[%s@%d] ACS_CS_TrapHandler Trap receiver FAILED! no.", __FUNCTION__, __LINE__);
					printTRA(tmpStr);
					printf("%s\n",tmpStr);

				}
				else
				{
					fl=true;

					char tmpStr[512] = {0};
					snprintf(tmpStr, sizeof(tmpStr) - 1, "[%s@%d] ACS_CS_TrapHandler Trap receiver SUCCESS! no.", __FUNCTION__, __LINE__);
					printf("%s\n",tmpStr);
					printTRA(tmpStr);
				}
			}
			else
			{
				if(status == (validIndex - 1))
				{
					char tmpStr[512] = {0};
					snprintf(tmpStr, sizeof(tmpStr) - 1, "[%s@%d] ACS_CS_TrapHandler Trap receiver SHUTDOWN event! no.", __FUNCTION__, __LINE__);
					printTRA(tmpStr);

					fl=true;
				}
			}
		}
	}


	return 0;
}



void ACS_CS_TrapHandler::printTRA(std::string mesg)
 {
 	if (_trace->ACS_TRA_ON())
 	{
 		char tmpStr[512] = {0};
 		snprintf(tmpStr, sizeof(tmpStr) - 1, mesg.c_str());
 		_trace->ACS_TRA_event(1, tmpStr);

 	}
 }


ACS_CS_ServiceHandler * ACS_CS_TrapHandler::getServiceHandler()
{
	return service;
}

