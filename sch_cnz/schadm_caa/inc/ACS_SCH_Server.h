#ifndef ACS_SCH_SERVER_H_
#define ACS_SCH_SERVER_H_

#include <iostream>
#include <sys/eventfd.h>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include <ace/Task.h>
#include <syslog.h>
#include <vector>
#include <set>
#include "ACS_SCH_Logger.h"
#include "ACS_SCH_Util.h"
#include "ACS_SCH_Event.h"
#include "ACS_SCH_CrmObserver.h"
#include "ACS_SCH_ClusterCPObserver.h"
#include "ACS_SCH_ComputeResourceRole.h"
#include "ACS_SCH_Role.h"
#include "ACS_SCH_CrmHandler.h"
#include "ACS_CS_API.h"


/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */
class ACS_SCH_CrmObserver;
/*  typedef std::vector<unsigned short> CpIdentityList;
  // A CS instance wrapper class to guarantee created CS instances are always deleted
  template<typename T, T * (* pf_createCsInstance)(), void (* pf_deleteCsInstance)(T *)>
  class CsInstance
  {
    public:
      CsInstance();
      ~CsInstance();
      T * get() const;
    private:
      CsInstance(CsInstance const & ); // Not to be implemented
      CsInstance & operator =(CsInstance const & ); // Not to be implemented
      T * m_csInstance;
  };

typedef CsInstance<ACS_CS_API_CPGroup, &ACS_CS_API::createCPGroupInstance, &ACS_CS_API::deleteCPGroupInstance> CsCpGroupInstance;
template<typename T, T * (* pf_createCsInstance)(), void (* pf_deleteCsInstance)(T *)>
inline CsInstance<T, pf_createCsInstance, pf_deleteCsInstance>::CsInstance()
   : m_csInstance(pf_createCsInstance())
{
}

template<typename T, T * (* pf_createCsInstance)(), void (* pf_deleteCsInstance)(T *)>
inline CsInstance<T, pf_createCsInstance, pf_deleteCsInstance>::~CsInstance()
{
   pf_deleteCsInstance(m_csInstance);
}

template<typename T, T * (* pf_createCsInstance)(), void (* pf_deleteCsInstance)(T *)>
inline T * CsInstance<T, pf_createCsInstance, pf_deleteCsInstance>::get() const
{
   return m_csInstance;
}*/

struct STATE 
{
    int insStateVal;
    int operStateVal;
    int admStateVal;

    void set(int i,int o,int a)
    {
      insStateVal=i;
      operStateVal=o;
      admStateVal=a;
    }
};

class ACS_SCH_Server : public ACE_Task_Base{

public:
	ACS_SCH_Server();
	int svc();
	~ACS_SCH_Server();

	bool startWorkerThreads();
	void stopWorkerThreads();
	bool CreateObjects();
	bool setupCRRIMMCallBacks(); 
	bool setupRoleIMMCallBacks();
	bool setupCrmIMMCallBacks();
	bool setupCrmObserverIMMCallBacks();
	bool setupCPIMMCallBacks();
	bool CreateCrMObject();
	//bool createRoleObject(string rolename,int scalability);
	bool createRoleObjects();
	void createCRRObject(std::string,std::string,std::string,std::string);
	bool sanityCheck();
	void getListOfRoles(vector<string>& listOfdns,set<string> &listOfRoles, bool isCRR);
	bool isCRRAlignWithCR(vector<string>&crdnList, vector<string>& crrdnList);
	void makeModelAlign();
	void updateCRRState(STATE currentState,STATE expectedState,string dn);
	void displayModelState(STATE state);
	bool isValidationRequired(STATE state,string dn);
	void hideCrMSchema();
	ACS_CC_ReturnType modifyRulesAndRoles();

private:
	ACS_TRA_Logging *ACS_SCH_logging;	
	ACS_SCH_ComputeResourceRole* m_CRRHandler;
	ACS_SCH_Role* m_RoleHandler;
	ACS_SCH_CrmHandler* m_CrmHandler;
	ACS_SCH_CrmObserver* m_CrmObserver;
	ACS_SCH_ClusterCPObserver * m_clusterCpObserver;
	ACS_SCH_EventHandle shutdownEvent;
	bool isMultipleCPSystem;
	bool shutdownService;
	OmHandler immHandle;
};

#endif
