#include "acs_hcs_progressReport_objectImpl.h"
#include "acs_hcs_progressReportHandler.h"
#include <ace/OS_NS_poll.h>

//******************************************************************************
acs_hcs_progressReportHandler::acs_hcs_progressReportHandler(acs_hcs_progressReport_ObjectImpl *Impl)
:  theImplementer(Impl)
 , theIsStop(false)

{
	DEBUG("%s","In constructor of acs_hcs_progressReportHandler ");
}
//----------------------------------------------------------------------------
acs_hcs_progressReportHandler::~acs_hcs_progressReportHandler()
{
	DEBUG("%s","In destructor of acs_hcs_progressReportHandler ");
        //theImplementer = NULL;
}
//----------------------------------------------------------------------------
void
acs_hcs_progressReportHandler::stop()
{
	DEBUG("%s","Entering acs_hcs_progressReportHandler::stop( ");	
	cout<<" in handlet stop(()"<<endl;
        theIsStop = true;
	cout<<"theIsStop now is :"<<theIsStop<<endl;
	DEBUG("%s","Leaving acs_hcs_progressReportHandler::stop( ");
        //theImplementer = NULL;  //TR HP67391
}
//----------------------------------------------------------------------------
int
acs_hcs_progressReportHandler::svc(void)
{
	DEBUG("%s","Entering acs_hcs_progressReportHandler::svc( ");
        struct pollfd myPollFds[1];
        myPollFds[0].fd = theImplementer->getSelObj();
        myPollFds[0].events = POLLIN;
        int ret;

        while(!theIsStop)
        {
		//ERROR("%s %d","revents of polling in this case is",myPollFds[0].revents);

	        myPollFds[0].revents = 0;
                ret = poll(myPollFds, 1, 500);

                if (ret == 0)
                {
                }
                else if (ret == -1)
                {
                }
		else if (theImplementer != NULL)
		{
			if(!theIsStop)
			{
				for (int i=0; i < 3; ++i)
				{
					if(theImplementer->dispatch(ACS_APGCC_DISPATCH_ALL) != ACS_CC_SUCCESS)
					{
						DEBUG("%s","Dispatch failed for progress report");
						sleep(0.1);
						//for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
						continue;
					}
					else
					{
						DEBUG("%s","Dispatch is successful");
						break;
					}
				}
				/*cout<<"Before calling dispatch"<<endl;
				theImplementer->dispatch(ACS_APGCC_DISPATCH_ALL);
				cout<<"After calling dispatch"<<endl;*/
			}
		}
	}
	DEBUG("%s"," Leaving acs_hcs_progressReportHandler::svc(");
	return 0;
}

