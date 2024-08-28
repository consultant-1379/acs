//======================================================================
//
// NAME
//      HealthCheckService.h
//
// COPYRIGHT
//      Ericsson AB 2011 - All rights reserved
//
//      The Copyright to the computer program(s) herein is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the written permission from Ericsson
//      AB or in accordance with the terms and conditions stipulated in the agreement/contract
//      under which the program(s) have been supplied.
//
// DESCRIPTION
//      Server process for Health Check Service in APZ.
//      Based on 190 89-CAA xxxxx.
//
// DOCUMENT NO
//      190 89-CAA xxxxx
//
// AUTHOR
//      2011-04-30 by EGINSAN
// CHANGES
//     
//======================================================================

#ifndef HealthCheckService_h
#define HealthCheckService_h

#include "acs_hcs_jobexecutor.h"
#include "acs_hcs_healthcheckcmd.h"
#include "acs_hcs_healthcheckcmdfactory.h"
#include "acs_hcs_readyToUse_objectImpl.h"
#include "acs_hcs_inUse_objectImpl.h"
#include "acs_hcs_jobbased_objectImpl.h"
#include "acs_hcs_jobScheduler_objectImpl.h"
#include "acs_hcs_singleEvent_objectImpl.h"
#include "acs_hcs_periodicEvent_objectImpl.h"
#include "acs_hcs_calenderPeriodicEvent_objectImpl.h"
#include "acs_hcs_report_objectImpl.h"
#include "acs_hcs_progressReport_objectImpl.h"
#include "acs_hcs_brfinterface.h"

#include "ACS_APGCC_RuntimeOwner.h"
#include <ACS_APGCC_AmfTypes.h>

#define NBI_RULES "/data/opt/ap/internal_root/health_check/rules/"
#define NBI_REPORTS "/data/opt/ap/internal_root/health_check/reports/"
#define INTERNAL_PATH "/cluster/storage/system/config/acs_hc/"
#define READYTOUSEDN "readyToUseId=1,ruleFileManagerId=1,HealthCheckhealthCheckMId=1"
#define INUSEDN "inUseId=1,ruleFileManagerId=1,HealthCheckhealthCheckMId=1"

class acs_hcs_readyToUse_ObjectImpl;
class acs_hcs_inUse_ObjectImpl;
class acs_hcs_jobbased_ObjectImpl;
class acs_hcs_jobScheduler_ObjectImpl;
class acs_hcs_singleEvent_ObjectImpl;
class acs_hcs_periodicEvent_ObjectImpl;
class acs_hcs_calenderPeriodicEvent_ObjectImpl;
class acs_hcs_report_ObjectImpl;
class acs_hcs_periodicReport_ObjectImpl;

using namespace std;

namespace AcsHcs
{
	/**
	* HealthCheckService - The Main Health Check Service class to handle all the IO Communications
	*                      and to create the application threads.
	*/
	class HealthCheckService : /*public ACE_Task_Base,*/ public ACS_APGCC_RuntimeOwner_V2 
	{

	public:
		HealthCheckService();

		~HealthCheckService();

		/**
		 * start(): This is the entry point of Health Check Service Application.
		 * @param[in]: stopEvent Stop Event is to exit from the application gracefully on demand.
		 * @param[out]: return 0 on success otherwise -1.
		 */
		int start();
              
                int setupHCThread(AcsHcs::HealthCheckService*);
		void getCPIDs();
		bool setupIMMCallBacks();
		static bool jobCreated;
		static string sameJob;
		static bool stopRequested;	
		static bool theInteractiveMode;
		static bool isShutdownOngoing;
		//static int permitFlag;
		/*
			If this static variable permitFlag is true then backup will be successful
			If permitFlag is false then backup will be failed 
 		*/
		//static bool permitFlag;


		/*
			true means load operation is ongoing and backup will not be permitted
			false means backup can be permitted	
		*/
		static bool isLoadOngoing;

		/*
                        true means load operation is ongoing and backup will not be permitted
                        false means backup can be permitted
                */
		static bool isUnloadOngoing;

		//static int isBrfcPermitted();
		static bool isBrfcPermitted();
		static bool diskFull;
		static bool folderDeleted;
		static bool progressDeleted;
		static int unload_count;
		void stopOIThreads();
		static string job;	
		ACE_INT32 m_threadGroupId;
		static void load(void* arg);
		static void unload(void* arg);
		static void execute(void* arg);
		static void scheduledExecute(void* arg);
		static void triggerExecute(void* arg);
		static void createCategory(std::string category, std::list<DocumentXml::rule>& ruleinfo, std::string ruleSetID);
		static int getCategoryList(std::vector<string> &CategoryList);
		static void displayCategoryList(std::vector<string> &CategoryList);
		static void displayRuleList(std::list<DocumentXml::rule> &ruleinfo);
		static void getRuleDetailsOfaCategory(string category, std::list<DocumentXml::rule>& ruleinfo, AcsHcs::DocumentXml::MultiMap theMap);
		static void createRule(string parentDN, std::list<DocumentXml::rule>::const_iterator it, std::string ruleSetID);
		ACS_CC_ReturnType updateCallback(const char*, const char**);
		void adminOperationCallback(ACS_APGCC_OiHandle, ACS_APGCC_InvocationType, const char*, ACS_APGCC_AdminOperationIdType, ACS_APGCC_AdminOperationParamType**);
		void createProgressReport(std::string parent);
		static void updateProgress(int state, int result, std::string resultInfo, int index, bool completed,string addInfo="");
		static void getJobAttributes(std::string jobName, std::string DN);
		static void trimRuleReference(std::string& ruleToTrim, std::string RDN);
		static bool isJobNeedToTrigger(string job, string category);	
		static void spawnExecutionThread(string job ,bool scheduleExecution=false);
		
		/*static void createBrfcThread();
		static int createParticipant();
		static int deleteParticipant();
		static void createBrfcRunTimeOwner();
		static bool initializeBrfcRunTimeOwner();
		static acs_hcs_brfimplementer* theHcBrfcImpl;
		static acs_hcs_brfthread* theHcBrfcRuntimeHandlerThreadPtr;*/
		

	private: 
		/**
		 * HealthCheckCmd is the basic class for assigning the specific object of the command executed.
		 */
		//HealthCheckCmd* cmdProcessor_;

		/*
		 * createHCSDirectories(): To create HCS directories and assign appropriate ACLs
		 * param[out]: returns true on success otherwise false
		 */

		static bool createHCSDirectories();

		/*
		 * addSrvUsrLocalGroup(): To add the service user to the ACSADMG, MCSADMG and CPADMG localgroups
		 * param[in] localGroup: takes localgroup as the input.
		 * param[in]: userName: username is the random user account name.
		 */

		// 
		void addSrvUsrLocalGroup(const string localGroup,string& userName);

		/*
		 * cpuMemUsgThread(): This thread is to update CPU Usage and Memory Usage Samples.
		 */

		//static ACE_UINT32 cpuMemUsgThread(void *lpvParam);
		static void svc(void);

		/**
		 * This Method is used to set the Pipe Security to allow the users to connect to the pipe.
		 * @Params[return ]: returns 1 in case of success or 0 for failure.
		 */

		bool SetPipeSecurity();

		/**
		 * srctSecurityPipe is a structure for Security Descriptor of the Pipe.
		 */

		//SECURITY_DESCRIPTOR        srctSecurityPipe;
		//PACL                       pACL;
		
		acs_hcs_readyToUse_ObjectImpl* ReadyToUseHandler;
		acs_hcs_inUse_ObjectImpl* InUseHandler;
		acs_hcs_jobbased_ObjectImpl* JobBasedHandler;
		acs_hcs_jobScheduler_ObjectImpl* JobSchedulerHandler;
		acs_hcs_singleEvent_ObjectImpl* SingleEventHandler;
		acs_hcs_periodicEvent_ObjectImpl* PeriodicEventHandler;
		acs_hcs_calenderPeriodicEvent_ObjectImpl* CalenderPeriodicEventHandler;
	//	acs_hcs_report_ObjectImpl* ReportHandler;
		acs_hcs_progressReport_ObjectImpl* ProgressReportImpl;
	};
}

#endif
