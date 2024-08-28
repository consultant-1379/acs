#ifndef APG_APP2_APPCLASS_H
#define APG_APP2_APPCLASS_H

#include "unistd.h"
#include "syslog.h"
#include <ace/Task_T.h>
#include <ace/OS.h>
#include "ace/ACE.h"
#include <ace/Monotonic_Time_Policy.h>

class ACS_LCT_Server;
class APOS_HA_ReactorRunner;

//class ACS_LCT_Adm:public ACE_Task<ACE_SYNCH> {
class ACS_LCT_Adm:public ACE_Task<ACE_SYNCH, ACE_Monotonic_Time_Policy> {

   private:
	ACS_LCT_Server* m_haObj;
	APOS_HA_ReactorRunner* m_reactorRunner; 

   public:
	ACS_LCT_Adm();
	~ACS_LCT_Adm();
	int active(ACS_LCT_Server*);
	int svc();
	int passive(ACS_LCT_Server*);
	void stop();
	int close(u_long);
	bool m_reactorAlreadyStop;
}; 

#endif /* APG_APP2_APPCLASS_H */
