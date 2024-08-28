#include "acs_prc_observerImplementer.h"
#include <fstream>

#define ACS_PRC_STRINGIZER(s) #s
#define ACS_PRC_STRINGIZE(s) ACS_PRC_STRINGIZER(s)

#if defined (ACS_PRCBIN_REVISION)
#define PRCBIN_REVISION ACS_PRC_STRINGIZE(ACS_PRCBIN_REVISION)
#else
#define PRCBIN_REVISION "UNKNOW"
#endif

const char bash_script_path[] = "/opt/ap/apos/conf/aposcfg_bash-bashrc-local.sh";
const char Node_A[] = "/storage/system/config/apos/nodeA_MEId";
const char Node_B[] = "/storage/system/config/apos/nodeB_MEId";
char Error_to_Log[1024] = {0};
ACS_TRA_Logging Observer_Logging;

acs_prc_observerImplementer::acs_prc_observerImplementer(): sleep(true), attribute_modified(false), MEId_attribute("") {
	Observer_Logging.Open("PRC");
}

acs_prc_observerImplementer::~acs_prc_observerImplementer() {
	Observer_Logging.Close();
}

void acs_prc_observerImplementer::adminOperationCallback(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_InvocationType /*invocation*/, const char* /*p_objName*/, ACS_APGCC_AdminOperationIdType /*operationId*/, ACS_APGCC_AdminOperationParamType**/*paramList*/ ) {
	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::adminOperationCallback", PRCBIN_REVISION );
	Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);
}

ACS_CC_ReturnType acs_prc_observerImplementer::create(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/, const char */*className*/, const char* /*parentName*/, ACS_APGCC_AttrValues **/*attr*/){
	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::create", PRCBIN_REVISION );
	Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType acs_prc_observerImplementer::deleted(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/, const char */*objName*/){
	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::deleted", PRCBIN_REVISION );
	Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType acs_prc_observerImplementer::modify(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/, const char */*objName*/, ACS_APGCC_AttrModification **attrMods){
	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::modify", PRCBIN_REVISION );
	Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

	attribute_modified = false;

	int i = 0;
	while( attrMods[i] ){

		if ( !strcmp ( attrMods[i]->modAttr.attrName, "networkManagedElementId" ) ){

			attribute_modified = true;

			if ( attrMods[i]->modAttr.attrValuesNum > 0 ){
				MEId_attribute = reinterpret_cast<char*>(attrMods[i]->modAttr.attrValues[0]);

				if ( MEId_attribute == "" )
					MEId_attribute = "1";

				break;
			}

			MEId_attribute = "1";
		}
		i++;
	}

	memset(Error_to_Log, 0, sizeof(Error_to_Log));

	if ( attribute_modified )
		snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::modify => networkManagedElementId modified", PRCBIN_REVISION );
	else
		snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::modify => networkManagedElementId not modified", PRCBIN_REVISION );

	Observer_Logging.Write(Error_to_Log,LOG_LEVEL_WARN);

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType acs_prc_observerImplementer::complete(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/){
	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::complete", PRCBIN_REVISION );
	Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

	return ACS_CC_SUCCESS;
}

void acs_prc_observerImplementer::abort(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/){
	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::abort", PRCBIN_REVISION );
	Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);
}

void acs_prc_observerImplementer::apply(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/){

	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::apply Start", PRCBIN_REVISION );
	Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

	if ( !attribute_modified ){

		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::apply => The networkManagedElementId has not been modified, skip file storing and script execution", PRCBIN_REVISION );
		Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::apply End", PRCBIN_REVISION );
		Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

		return;
	}

	string p_local_node_id;
	ifstream ifs;
	ofstream ofs;

	ifs.open("/etc/cluster/nodes/this/id");
	if (ifs.good())
		getline(ifs, p_local_node_id);
	else {
		ifs.close();

		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::apply Impossible to retrieve node id", PRCBIN_REVISION );
		Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::apply End", PRCBIN_REVISION );
		Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

		return;
	}
	ifs.close();

	if ( p_local_node_id == "1" )
		ofs.open(Node_A,ifstream::out);
	else if ( p_local_node_id == "2" )
		ofs.open(Node_B,ifstream::out);
	else {
		ofs.close();

		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::apply Wrong node id", PRCBIN_REVISION );
		Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::apply End", PRCBIN_REVISION );
		Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

		return;
	}

	ofs << MEId_attribute << std::endl;
	ofs.close();

	int retry = 3;

	while (( system(bash_script_path) != 0 ) && ( retry > 0 ) ){

		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::apply - script execution failed", PRCBIN_REVISION );
		Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

		::sleep(1);
		retry--;
	}

	if ( retry <= 0){
		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::apply - prompt change Not OK for three times", PRCBIN_REVISION );
		Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);
	}
	else {
		memset(Error_to_Log, 0, sizeof(Error_to_Log));
		snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::apply - prompt change OK", PRCBIN_REVISION );
		Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);
	}

	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::apply End", PRCBIN_REVISION );
	Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

}


ACS_CC_ReturnType acs_prc_observerImplementer::updateRuntime(const char* /*p_objName*/, const char** /*p_attrName*/){
	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::updateRuntime", PRCBIN_REVISION );
	Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

	return ACS_CC_SUCCESS;
}

int acs_prc_observerImplementer::svc ( void ){

	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::svc - Start", PRCBIN_REVISION );
	Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

	epoll_event  event_to_check_in, event_to_check_out;
	int   ret, ep_fd = ::epoll_create1 (EPOLL_CLOEXEC);

	event_to_check_in.events = EPOLLIN;
    ::epoll_ctl (ep_fd, EPOLL_CTL_ADD, this->getSelObj(), &event_to_check_in );

    while( !sleep )
    {
		ret = ::epoll_wait (ep_fd, &event_to_check_out, 1, 400);

		if ( ret == 0 )
			continue;
		else if ( ret == -1 ){
			if (! sleep) {
				::epoll_ctl (ep_fd, EPOLL_CTL_ADD, this->getSelObj(), &event_to_check_in );
			}
		}
		else {
			if ( sleep ) {
				memset(Error_to_Log, 0, sizeof(Error_to_Log));
				snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::svc - End", PRCBIN_REVISION );
				Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

				return 0;
			}
			this->dispatch(ACS_APGCC_DISPATCH_ALL);
		}
    }

	memset(Error_to_Log, 0, sizeof(Error_to_Log));
	snprintf(Error_to_Log,1024,"acs_prcmand %s - acs_prc_observerImplementer::svc - End", PRCBIN_REVISION );
	Observer_Logging.Write(Error_to_Log,LOG_LEVEL_ERROR);

	return 0;
}
