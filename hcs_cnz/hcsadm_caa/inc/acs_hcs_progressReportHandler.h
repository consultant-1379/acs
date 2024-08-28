#ifndef SCPS_COUNTERRUNTIMEHANDLER_H_
#define SCPS_COUNTERRUNTIMEHANDLER_H_

#include <sys/poll.h>
#include <ACS_CC_Types.h>
#include <iostream>
#include "acs_hcs_progressReport_objectImpl.h"
#include <ace/Task.h>
#include <ace/OS_NS_poll.h>

class acs_hcs_progressReport_ObjectImpl;

class acs_hcs_progressReportHandler : public ACE_Task_Base {
public :
        acs_hcs_progressReportHandler(acs_hcs_progressReport_ObjectImpl *Impl);
        ~acs_hcs_progressReportHandler();
        void stop();
        int svc(void);
private:
        acs_hcs_progressReport_ObjectImpl *theImplementer;
        bool theIsStop;
};
#endif 
