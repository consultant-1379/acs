/** @file acs_apbm_scxlagimmhandler.h
 *      @brief
 *      @author XSUNACH (Anil Achary)
 *      @date 2015-03-11
 *
 *      COPYRIGHT Ericsson AB, 2015
 *      All rights reserved.
 *
 *      The information in this document is the property of Ericsson.
 *      Except as specifically authorized in writing by Ericsson, the receiver of
 *      this document shall keep the information contained herein confidential and
 *      shall protect the same in whole or in part from disclosure and dissemination
 *      to third parties. Disclosure and disseminations to the receivers employees
 *      shall only be made on a strict need to know basis.
 *
 *      REVISION INFO
 *      +=======+============+==============+=====================================+
 *      | REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *      +=======+============+==============+=====================================+
 *      | R-001 | 2015-03-11 | xsunach      | CNI 33/6-441                        |
 *      +=======+============+==============+=====================================+
 */

#include "acs_apbm_snmpmanager.h"
#include "acs_apbm_macrosconstants.h"
#include "acs_apbm_serverworkingset.h"
#include <ACS_CC_Types.h>
#include <acs_apgcc_adminoperationtypes.h>
#include <acs_apgcc_omhandler.h>
#include <vector>
#include <acs_apgcc_objectimplementerinterface_V3.h>
#include <acs_apgcc_oihandler_V3.h>

#include <ace/Task.h>
#include <sys/poll.h>

#define LAG_IMPLEMENTER_THREAD_POLL_TIMEOUT_MS  2000  /* milliseconds */

const std::string IMMCLASSNAME_LAG = "AxeEquipmentLag"; // write the class name of the lag
const std::string LAG_IMPLEMENTER_NAME = "lagImplementer";
const std::string IMMOBJECTNAME_LAG = "lag";
const std::string IMMHWMGMTPATH = ",hardwareMgmtId=1,AxeEquipmentequipmentMId=1";
const std::string IMMOTHERBALDEA = "otherBladeId=0,shelfId=";
const std::string IMMOTHERBALDEB = "otherBladeId=25,shelfId=";

const std::string RDN_IMMLAGROOT_A = "lagId=0";
const std::string RDN_IMMLAGROOT_B = "lagId=25";
const std::string RDN_IMMLAGROOT = "lagId=1";
#define PARENT_RDN_A "otherBladeId=0"
#define PARENT_RDN_B "otherBladeId=25"

#define ADMINSTRATIVE_STATE_ATTR "administrativeState"
#define OPERATIONAL_STATE_ATTR "operationalState"

#define UNLOCKED 1
#define LOCKED 0

class lagImplementerThread;

class acs_apbm_scxlagimmhandler : public acs_apgcc_objectimplementerinterface_V3{
public:

	/*tr HR55108 added*/
	static ACE_Recursive_Thread_Mutex _acs_hwi_mutex_m_map;

	acs_apbm_scxlagimmhandler(acs_apbm_serverworkingset *serverWorkingSet);
	virtual ~acs_apbm_scxlagimmhandler();
	/*inherited methods APGCC*/

	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
			ACS_APGCC_InvocationType invocation,
			const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
			ACS_APGCC_AdminOperationParamType**paramList);


	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);

	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	ACS_CC_ReturnType updateRuntime(const char *objName, const char **p_nameAttr);

	void createIMMLagClass();
	int createLAGRootClass(const char* my_shelf_addr,int slot);

	int openScxLagHandler();
	int closeScxLagHandler();

	int openListener();
	int openClassImplementer();
	int closeListener();
	int closeClassImplementer();

private:
	void defineLagAttributes (vector<ACS_CC_AttrDefinitionType> &classAttributes);

	bool m_openDone;
	bool m_immConfingCloseOK;

	acs_apbm_snmpmanager *m_snmpManager;
	acs_apbm_serverworkingset *m_serverWorkingSet;
	lagImplementerThread *lagImmImplementerThread;
	acs_apgcc_oihandler_V3 oiHandler;
};
class lagImplementerThread : public ACE_Task_Base {
public :



	/**Constructor**/
	lagImplementerThread(acs_apbm_scxlagimmhandler *immHandler){

		lagimplementer = immHandler;
		isStop = false;
	}

	~lagImplementerThread() {}

	/**This method is used to stop the thread**/
	void stop(){
		isStop = true;
	}

	/** Thread loop **/
	int svc(void){

		int ret;
		struct pollfd fds[1];
		fds[0].fd = lagimplementer->getSelObj();
		fds[0].events = POLLIN;

		while(!isStop) {

			ret = poll(fds, 1, LAG_IMPLEMENTER_THREAD_POLL_TIMEOUT_MS);
			if (ret == 0) {
				//cout<<"timeout off"<<endl;
			}else if (ret == -1){
				//std::cout << " POLL ERROR !" << std::endl;
			}else{
				//std::cout << "Dispatch to IMM Implementer " << std::endl;
				lagimplementer->dispatch(ACS_APGCC_DISPATCH_ALL);
			}
		}
		return 0;
	}

private:

	acs_apbm_scxlagimmhandler *lagimplementer;
	bool isStop;

};
