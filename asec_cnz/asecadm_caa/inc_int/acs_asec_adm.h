#ifndef ACS_ASEC_ADM_H
#define ACS_ASEC_ADM_H

#include <unistd.h>
#include <syslog.h>
#include "ace/Task_T.h"
#include "ace/OS.h"
#include "ACS_TRA_Logging.h"
#include <ace/Sig_Handler.h>
#include <ace/Task.h>
#include <sys/eventfd.h>
#include "ace/OS_NS_poll.h"


using namespace std;

class asecHAClass;
class ACS_ASEC_ReactorRunner;
class ACS_ASEC_ObjImpl;
class ACS_ASEC_DSDSrv;
class ACS_ASEC_ImmHandler;

class ACS_ASEC_Adm:public ACE_Task<ACE_SYNCH> {

   private:
	   
    asecHAClass* m_haObj;
	ACS_ASEC_ReactorRunner* m_reactorRunner;
	ACE_Sig_Handler sig_shutdown_;
	ACE_UINT32 l_active;
	ACE_UINT32 debugOn;



   public:
        ACS_ASEC_Adm();
        ~ACS_ASEC_Adm();
	ACS_TRA_Logging log;

        int active(asecHAClass*);
        int passive(asecHAClass*);
        int start(int argc, char* argv[]);
        int svc();
        void stop();
        int close(u_long);
        int handle_close(ACE_HANDLE, ACE_Reactor_Mask /*mask*/);
        int handle_signal(int signum, siginfo_t*, ucontext_t *);
	bool executeCommand(const std::string& command, std::string& output);
	void applyCertGrouptoSec();
	void restartSecCertmToApplyCertGrp();
	bool isVirtual();
        void applyIkev2ForIpv6();
	void writeDefaultIkev1Config();
        void updateIkev2Version();
};

#endif /* ACS_ASEC_ADM_H */
