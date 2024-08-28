//******************************************************************************
//
//  NAME
//     acs_lm_cmdserver.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//	    190 89-CAA nnn nnnn
//
//  AUTHOR
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_lm_cprestartmonitor.h"

#include "acs_lm_lksender.h"
#include <ace/ACE.h>
#include <ace/Handle_Set.h>
//ACE_HANDLE ACS_LM_CpRestartMonitor::stopThreadFds = 0;
bool ACS_LM_CpRestartMonitor::theStopSignal = false;
ACE_Recursive_Thread_Mutex ACS_LM_CpRestartMonitor::theMutex;
/*=================================================================
	ROUTINE: ACS_LM_CpRestartMonitor constructor
=================================================================== */
ACS_LM_CpRestartMonitor::ACS_LM_CpRestartMonitor():
 sentinel(NULL),
 persistent(NULL),
 eventHandler(NULL),
 theEndEvent(0)
{
	DEBUG("ACS_LM_CpRestartMonitor::ACS_LM_CpRestartMonitor() %s","Entering");
	//stopThreadFds = aStopThreadFds;
	theEndEvent = new ACE_Event(true,false,USYNC_THREAD,"LM_CPRESTART_MONITOR_EVENT");
	DEBUG("ACS_LM_CpRestartMonitor::ACS_LM_CpRestartMonitor() %s","Leaving");
	setStopSignal(false);
	setRegisterServiceResult(false);

}//end of constructor

/*=================================================================
	ROUTINE: ACS_LM_CpRestartMonitor destructor
=================================================================== */
ACS_LM_CpRestartMonitor::~ACS_LM_CpRestartMonitor(void)
{
	DEBUG("ACS_LM_CpRestartMonitor::~ACS_LM_CpRestartMonitor() %s","Entering");
	delete theEndEvent;
	DEBUG("ACS_LM_CpRestartMonitor::~ACS_LM_CpRestartMonitor() %s","Leaving");
}//end of destructor
/*=================================================================
	ROUTINE: setComponents
=================================================================== */
void ACS_LM_CpRestartMonitor::setComponents(ACS_LM_Sentinel* sentinel,
		ACS_LM_Persistent* persistent,
		ACS_LM_EventHandler* eventHandler)
{
	DEBUG("Entering %s","In ACS_LM_CpRestartMonitor::setComponents");
	this->sentinel = sentinel;
	this->persistent = persistent;
	this->eventHandler = eventHandler;
	DEBUG("Leaving %s","In ACS_LM_CpRestartMonitor::setComponents");

}
/*=================================================================
	ROUTINE: stop
=================================================================== */
void ACS_LM_CpRestartMonitor::stop()
{
	//CNI30_4_1761
	DEBUG("ACS_LM_CpRestartMonitor::stop() %s","Entering");
	setStopSignal(true);
	if(getRegisterServiceResult() == false)
	{
		ERROR("ACS_LM_CpRestartMonitor::stop() %s","The CP Restart monitor is in register phase");
		//THE CP REstart monitor is still trying to register . Stopping that loop.
		theEndEvent->pulse();
	}
	else
	{
		ERROR("ACS_LM_CpRestartMonitor::stop() %s","The CP Restart monitor is not in register phase");
	}
	DEBUG("ACS_LM_CpRestartMonitor::stop() %s","Leaving");
}//end of stop

/*=================================================================
	ROUTINE: start
=================================================================== */
void ACS_LM_CpRestartMonitor::start()
{
	DEBUG("ACS_LM_CpRestartMonitor::start() %s","Entering");
	activate();
	DEBUG("ACS_LM_CpRestartMonitor::start() %s","Leaving");

}

/*=================================================================
	ROUTINE: start
=================================================================== */

bool ACS_LM_CpRestartMonitor::registerService(ACS_LM_JTP_Service* & jtpService)
{

	bool myResult = false;
	setRegisterServiceResult(false);
	jtpService = ACS_LM_JTP_Service::create(3);
	DEBUG (" ACS_LM_CpRestartMonitor::svc() %s","Before jtpService->registerService");
	try
	{
		myResult = jtpService->registerService(AP_JTP_SERVICE);
		setRegisterServiceResult(myResult);
	}
	catch(...)
	{
		ERROR (" ACS_LM_CpRestartMonitor::svc() %s","jtpService->registerService - CATCH BLOCK");
	}
	if (myResult == false)
	{
		if(jtpService != NULL)
		{
			delete jtpService;
			jtpService = NULL;
		}
		ERROR (" ACS_LM_CpRestartMonitor::svc() %s","jtpService->registerService- failed");
	}
	return myResult;

}
int ACS_LM_CpRestartMonitor::svc()
{
	DEBUG("ACS_LM_CpRestartMonitor::svc() %s","Entering");
	ACS_LM_JTP_Service* jtpService;
	bool myReturnVal = registerService(jtpService);

	if(myReturnVal == false)
	{
		INFO("ACS_LM_CpRestartMonitor::svc() %s","registerService failed. Trying after 10 seconds");
		ACE_Time_Value oTimeValue(10,0);
		bool myContinue = true;
		while(myContinue)
		{
			int myRet = theEndEvent->wait(&oTimeValue,0);
			if(myRet < 0)
			{
				int myErrNum = ACE_OS::last_error();
				if( myErrNum == 62 )
				{
					INFO("ACS_LM_CpRestartMonitor::svc() %s","Time out happened on theEndEvent->wait(&oTimeValue,0)");
					myReturnVal = registerService(jtpService);
					if(myReturnVal == true)
					{
						myContinue = false;
						// Trying to register again after 10 seconds
					}
				}
				else
				{
					ERROR("ACS_LM_CpRestartMonitor::svc() ACE_OS::last_error() is %d",myErrNum);
					return 0;
				}

			}
			else if(myRet == 0)
			{
				DEBUG("ACS_LM_CpRestartMonitor::svc() %s","Stop signal received");
				return 0;
			}
			else
			{
				ERROR("ACS_LM_CpRestartMonitor::svc() %s","return value from theEndEvent->wait is neither zero nor negative value");
				return 0;
			}

		}
	}
	else
	{
		DEBUG("ACS_LM_CpRestartMonitor::svc() %s","registerservice is sucessful");
	}

	if(getRegisterServiceResult() == true)
	{

		DEBUG (" ACS_LM_CpRestartMonitor::svc() %s","jtpService->registerService - Success");

		int handleCount = 30;
		JTP_HANDLE* pHandles = new JTP_HANDLE[30]; //check the number of handles hard coded

		jtpService->getHandles(handleCount, pHandles);

		if (handleCount == 0)
		{
			ERROR("ACS_LM_CpRestartMonitor::svc() %s", "handleCount is zero");
			return 0;
		}
		else
		{
			INFO("ACS_LM_CpRestartMonitor::svc() handleCount = %d", handleCount);
		}

		JTP_HANDLE * hndWait = new(std::nothrow) JTP_HANDLE[handleCount];

		for(int i=0; i < handleCount; i++)
		{
			hndWait[i] = pHandles[i];
		}
			
		delete []pHandles; //TBD
		struct pollfd* myPollfd = new pollfd[handleCount];
		while(1)
		{
			if(  myPollfd == NULL )	
			{
				break;
			}
			else
			{	
				for(int ii = 0 ; ii<handleCount ;++ii )
				{
					myPollfd[ii].fd = hndWait[ii];
					myPollfd[ii].events = POLLIN | POLLRDHUP | POLLHUP | POLLERR;
					myPollfd[ii].revents = 0;
				}
				ACE_Time_Value selectTime;
				selectTime.set(1);	// 1 second
				ACE_INT32 ret;
				ret=ACE_OS::poll (myPollfd, 1, &selectTime);
				//Check for timeout.
				if (ret == 0)
				{
					// DEBUG("ACS_LM_CpRestartMonitor::svc() %s","Time out occurred from POLL ");
					if(getStopSignal() == true)
					{
						INFO("ACS_LM_CpRestartMonitor::svc() %s", "Stop event signalled while waiting for a CP restart");
						if (myPollfd != NULL)
						{
							DEBUG("%s" ,"in if mypollfd");
							delete[] myPollfd;
							myPollfd = 0;
						}
						break;
					}
				}
				else if( ret < 0 )
				{
					if( ACE_OS::last_error() == EINTR )
					{
						ERROR("ACS_LM_CpRestartMonitor::svc() %s", "ACE_OS::last_error() == EINTR");
							continue;
					}
					else
					{
						ERROR("ACS_LM_CpRestartMonitor::svc() %s","Error occurred while waiting for multiple handles ,errno: %d",errno);
						if (myPollfd != NULL)
                                               	{
                                	        	DEBUG("%s" ,"in 2nd if mypollfd");
                		                        delete[] myPollfd;
		                                        myPollfd =0;
                                              	}	

						break;
					}
				}
				else
				{
					for(int i=0; i < handleCount; i++)
					{
						if(myPollfd[i].revents == POLLIN)
						{
							JtpNode* jtpNode = jtpService->accept();
							INFO("ACS_LM_CpRestartMonitor.svc() :The return value of the jtpService->accept() is :%d",jtpNode);
							//DEBUG(" ACS_LM_CpRestartMonitor::svc()  CP id = %d restarted ",jtpNode->id);
							ACS_LM_LkSender::sendLks(ACS_LM_SENDER_CPRESTART,sentinel,persistent,eventHandler,jtpNode);
							if(getStopSignal() == true)
							{
								INFO("ACS_LM_CpRestartMonitor::svc() %s", "Stop event signalled in the process of sending data to CP");
								if (myPollfd != NULL)
								{
									DEBUG("%s", "in else for loop befor delete");
									delete [] myPollfd;
									myPollfd = 0;
								}
								break;
							}
						}
					}
				}
			}
			
		}
		if (hndWait)
		{
			delete[] hndWait;
		}
	}
	else
	{
		ERROR(" ACS_LM_CpRestartMonitor::svc() %s","jtpService->registerService - FAILED");

	}

	DEBUG("ACS_LM_CpRestartMonitor::svc() %s","Leaving");
	return 0;

}

bool ACS_LM_CpRestartMonitor::getStopSignal()
{
	ACS_LM_CpRestartMonitor::theMutex.acquire();
	bool myStatus = theStopSignal;
	ACS_LM_CpRestartMonitor::theMutex.release();
	return myStatus;
}
void ACS_LM_CpRestartMonitor::setStopSignal(bool mySignal)
{
	ACS_LM_CpRestartMonitor::theMutex.acquire();
	theStopSignal = mySignal;
	ACS_LM_CpRestartMonitor::theMutex.release();

}
bool ACS_LM_CpRestartMonitor::getRegisterServiceResult()
{
	ACS_LM_CpRestartMonitor::theMutex.acquire();
	bool myStatus = theRegisterServiceResult;
	ACS_LM_CpRestartMonitor::theMutex.release();
	return myStatus;
}
void ACS_LM_CpRestartMonitor::setRegisterServiceResult(bool mySignal)
{
	ACS_LM_CpRestartMonitor::theMutex.acquire();
	theRegisterServiceResult = mySignal;
	ACS_LM_CpRestartMonitor::theMutex.release();

}


