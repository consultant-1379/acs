#ifndef ACS_SCH_Util_h
#define ACS_SCH_Util_h 1

#include "ACS_SCH_Logger.h"
#include "ACS_SCH_Trace.h"
#include "ACS_CS_API.h"
#include "ACS_SCH_Event.h"
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include <string>

#define ONGOING 1
#define COMPLETED 0

typedef std::vector<unsigned short> CpIdentityList;
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
}

namespace SCH_Util
{
	static const char EVENT_NAME_SHUTDOWN[]    = "ACS_SCH_SHUTDOWN";
	//static const char EVENT_NAME_SCALING[]    = "ACS_SCH_SCALING";
	static const int Daemon_Timeout			 = 3;	 // Timeout 3 s, used by daemon
}

class ACS_SCH_Util
{
	private:
		//Variable declaration
		static int _scaling;
		//static string _BCName;
		//static ACS_SCH_EventHandle _scalingShutdownEventHandle;

	public:
		//Functions declaration
		static bool isModelAligned();
		static int getScalingStatus();
		static string getCurrentBCName();
		static void setScalingStatus(int val);
		static void setCurrentBCName(string BCName);
		static string getRoleName(string CRName);
		static std::string getManagedElement();
		static ACS_CS_API_NS::CS_API_Result getCpIdList(CpIdentityList & cpIdListOut, std::string const & group);
		static std::string getImmStringAttribute(string dnName, string attributeName);
		static int getImmIntAttribute(string dnName, string attributeName);
		static ACS_CC_ReturnType updateImmNameAttribute(string dnName, string attr, string val);
		static ACS_CC_ReturnType updateImmIntAttribute(string dnName, string attr, int val);
		static ACS_CC_ReturnType updateImmStringAttribute(string dnName, string attr, string val);
		static bool updateModelForManualCQACI(string bcName);
		static string getBcNameFromDn(string objName);
		static bool isScalingOngoing();
		static void printmtzln();
		static bool createRoleObject(string rolename,int scalability);
		static bool isVMScalable(string CRName);
		//static void cpStateObserverThread(void* param);
};

#endif
