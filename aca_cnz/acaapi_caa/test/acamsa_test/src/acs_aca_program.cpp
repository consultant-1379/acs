//#define CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>


//#include <tchar.h>
#include <iostream>
#include <stdexcept>
#include <new>
#include <sys/time.h>
#include <sys/types.h>

#include <list>

#include "acs_aca_message_store_interface.h"
#include "acs_aca_message_store_client.h"
#include "acs_aca_message_store_message.h"
#include "ACS_CS_API.h"

#include "acs_aca_heap_agent.h"
#include <ace/ACE.h>

namespace {
	void pfxPrintHelp(const char * progName) {
		std::cout << "USAGE" << std::endl
							<< "  " << progName << " [MS NAME] [CP ID/CpSite] {commmit num}" << std::endl;
	}

	int pfxCheckMultipleCPSystem(bool & flag) {
		ACS_CS_API_NS::CS_API_Result result;

		for (int i = 0; i < 8; ++i, ACE_OS::sleep(0.3)) std::cout << '.';
		
		try { result = ACS_CS_API_NetworkElement::isMultipleCPSystem(flag); }
		catch (...) {
			std::cout << "FAILURE!" << std::endl
								<< "'ACS_CS_API_NetworkElement::isMultipleCPSystem(...' failed with a UNRECOGNIZED EXCEPTION!" << std::endl;
			return 3;
		}

		if (result != ACS_CS_API_NS::Result_Success) {
			char traceMessage[128] = {0};
			ACE_OS::snprintf(traceMessage, sizeof(traceMessage) - 1, "'ACS_CS_API_NetworkElement::isMultipleCPSystem(...' returned with error code = %d", result);
			std::cout << "FAILURE!" << std::endl
								<< traceMessage << std::endl;
			return 3;
		}

		std::cout << "OK!" << std::endl;

		return 0;
	}
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		std::cout << "Syntax error on command." << std::endl;
		pfxPrintHelp(argv[0]);
		return 1;
	}

	std::cout << "Welcome to RTR simulation system!" << std::endl;
	ACE_OS::sleep(0.1);
	std::cout << "Wait please, I am checking the system environment";
	
	bool isMultipleCPSystem = false;
	
	if (pfxCheckMultipleCPSystem(isMultipleCPSystem)) {
		std::cout << "Cannot recognize the CP system type (multiple or single CP)!" << std::endl;
		return 1;
	}
	
	std::cout << "The system is " << (isMultipleCPSystem ? "Multiple" : "Single") << " CP System."  <<std::endl;
	
	unsigned cpSystemId = (isMultipleCPSystem ? atoi(argv[2]) : 0);
	//Debug :to be removed later
	std::cout<< "Log1"<<endl;	
	ACS_ACA_MessageStoreClient * pClient = 0;
	int errorLevel = 0;
	
	int msgToCommit = 0;
	//unsigned long msgToCommit = 0;
	if (argc >= 4) msgToCommit = strtoul(argv[3], 0, 10);
	std::cout<< "Log2"<<endl;	
	
	for (;;) {
		std::cout<< "Entering Loop"<<endl;	
		pClient = isMultipleCPSystem ? new ACS_ACA_MessageStoreClient(argv[1], cpSystemId): new ACS_ACA_MessageStoreClient(argv[1], argv[2]);
		std::cout<< "Entering Loop 1"<<endl;	
		
		if (!pClient) {
			std::cout << "new operator failed to allocate system heap memory! Probably low memory condition!" << std::endl;
			errorLevel = 5;
			break;
		}

		std::cout << "Connecting to message store service...";

		errorLevel = -1;
		try {
			int res=pClient->connect();
			cout<<"returing res :"<<res<<endl;
			if (res==1) errorLevel = 0;
			else { cout<<"Error Code:"<<res<<endl; std::cout << "FAILED! Error code: " << pClient->getLastErrorName(); } 
		} catch(std::runtime_error & ex) { std::cout << "FAILED with this exception: " << ex.what();
		} catch(std::bad_alloc & ex) { std::cout << "FAILED with BAD ALLOC: " << ex.what();
		} catch (...) { std::cout << "FAILED with an unrecognized exception!";
		}

		if (errorLevel == -1) {
			delete pClient;
			ACE_OS::sleep(10);
			continue;
		}
		
		std::cout << "OK!" << std::endl;

		ACS_ACA_MessageStoreMessage message;
		DWORD waitResult = 0;
		bool restartFor = false;
		//unsigned long commitCountDown = msgToCommit;
		int commitCountDown = msgToCommit;
	
		for (bool again = true; again; restartFor = false) {
			do {
				if (!pClient->getMessage(message)) {
					std::cout << std::endl << "'client.getMessage(...' failed to get message! Error code: " << pClient->getLastErrorName() << std::endl;
					restartFor = true;
					again = false;
					break;
				} else {
 					if (message.getState() != ACS_ACA_MessageStoreMessage::Empty) {
						if (message.getState() != ACS_ACA_MessageStoreMessage::Filled)
							std::cout << "  *** MESSAGE IS NOT FILLED ???? ****" << std::endl;
						// if (message.getState() == ACS_ACA_MessageStoreMessage::Filled) {
							// int bufln = message.getDataLength();
							// std::cout << "   ### DEBUG: bufln = " << bufln << ", ";
							// char * buf = 0;
							// try { buf = new char[bufln]; }
							// catch (std::bad_alloc & ex) { std::cout << "BAD ALLOCATION EXCEPTION: '" << ex.what() << "', "; }
							// catch (...) { std::cout << "EXCEPTION ON NEW -> "; }
							// if (buf) {
								// std::cout << "buf = " << reinterpret_cast<void *>(buf) << ", ";
								// memcpy(buf, message.getData(), bufln);
								// std::cout << "memcpy OK, ";
								// lst.push_back(buf);
								// std::cout << "push_back OK" << std::endl;
							// } else std::cout << "buf IS ZERO ???" << std::endl;
						// } else std::cout << "  *** MESSAGE IS NOT FILLED ???? ****" << std::endl;

						std::cout << "\rMSG_ID = " << message.getMessageId() << ", "
											<< "MSG_DATA_LENGTH = " << message.getDataLength() << ", "
											<< "MSG_STATE = " << message.getState() << ", "
											<< "MSG_POINTER_DATA = " << reinterpret_cast<const void *>(message.getData()) << std::endl;
						if (--commitCountDown == 0) {
							commitCountDown = msgToCommit;
							std::cout << std::endl << "-- COMMITTING...";
							std::cout << (pClient->commit(msgToCommit) ? "OK!" : "FAILED!") << std::endl;
						}
					} else std::cout << std::endl << "  *** MESSAGE IS EMPTY ???? ****" << std::endl;
				}
			} while (message.getState() != ACS_ACA_MessageStoreMessage::Empty);

			if (restartFor) continue;
			
			std::cout << " -- WAITING FOR NEXT MESSAGE...\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
			fd_set rfds;
			struct timeval tv;
			int retval = 0;

			do {
				tv.tv_sec = 0.3;
				tv.tv_usec = 0;
				FD_ZERO(&rfds);
				FD_SET(pClient->getHandle(),&rfds);
				retval = select(1, &rfds, NULL, NULL, &tv);

				if(retval == 0) // timeout has occurred
				{
					std::cout << "                               \b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
					ACE_OS::sleep(0.08);
					std::cout << " -- WAITING FOR NEXT MESSAGE...\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
				}
				else if (retval == -1) // a fault has occurred
				{
					std::cout << "FAILED!" << std::endl;
					again = false;
				}
			/*	switch (waitResult = WaitForSingleObject(pClient->getHandle(), 300)) {
				case WAIT_FAILED:
					std::cout << "FAILED!" << std::endl;
					again = false;
					break;
				case WAIT_TIMEOUT:
					std::cout << "                               \b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
					Sleep(80);
					std::cout << " -- WAITING FOR NEXT MESSAGE...\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
					break;
				default: */ /*std::cout << "WAKE-UP: try to get message!";*/ break;
			}
			while (retval == 0);
			std::cout << "                               \b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
		}

		std::cout << std::endl << "Disconnecting from message store...";
		
		if (!pClient->disconnect()) {
			std::cout << "FAILED!" << std::endl
								<< "'client.disconnect()' failed to disconnect! Error code: " << pClient->getLastErrorName() << std::endl;
		} else std::cout << "OK!!" << std::endl;

		delete pClient;
	}
	
	return errorLevel;
}
