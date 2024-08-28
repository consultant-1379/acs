/*
 * ACS_PRC_RunTimeOwner.cpp
 *
 *  Created on: Dec 28, 2010
 *      Author: xlucpet
 */

#include "ACS_PRC_IspAdminOperationImplementer.h"
#include "acs_apgcc_omhandler.h"
#include <vector>
#include <fstream>
#include <string>
#include <iterator>
#include "ace/SOCK_Connector.h"
#include <ace/LSOCK_Acceptor.h>
#include <ace/LSOCK_Connector.h>

const char PRC_ISP_CONF_FILE[] = "/etc/logrot.d/acs_prc_isp_conf";

void removeLog( ) {

	::remove("/cluster/etc/ap/acs/prc/prcisp_syslog.log");
	::remove("/cluster/etc/ap/acs/prc/prcisp_syslog.log.1");
	::remove("/cluster/etc/ap/acs/prc/prcisp_syslog.log.2");
}

void update_restart_logrotd( long long logSize ) {

    char isp_conf_file_row[128] = {0};
    snprintf(isp_conf_file_row,sizeof(isp_conf_file_row),"/cluster/etc/ap/acs/prc/prcisp_syslog.log %lld 3",logSize);

    ::remove(PRC_ISP_CONF_FILE);
	int isp_conf_file = -1;

	if ( (isp_conf_file = ::open(PRC_ISP_CONF_FILE, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR )) != -1 ){

		::write(isp_conf_file, isp_conf_file_row, strlen(isp_conf_file_row));

		::close(isp_conf_file);

		int status;

		string cmd = "/usr/bin/systemctl restart lde-logrot.service &>/dev/null";

		pid_t pid = fork();
		if (pid == 0) {
			if(execlp("sh","sh", "-c", cmd.c_str(), (char *) NULL) == -1){
				printf("Error: execlp: %d \n",errno);
			}
		}
		else if (pid < 0)
			printf("Fatal error fork() failed. %d \n",errno);

		waitpid(pid, &status, 0);
	}
}

ACS_PRC_IspAdminOperationImplementer::ACS_PRC_IspAdminOperationImplementer() {

}

ACS_PRC_IspAdminOperationImplementer::~ACS_PRC_IspAdminOperationImplementer() {

}

void ACS_PRC_IspAdminOperationImplementer::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* /*p_objName*/,
		ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType** /*paramList*/) {

	if ( operationId == 1) {
		removeLog();
		this->adminOperationResult( oiHandle , invocation, 1 );
	}
	else
		this->adminOperationResult( oiHandle , invocation, 28 );

}

ACS_CC_ReturnType ACS_PRC_IspAdminOperationImplementer::create(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/, const char */*className*/, const char* /*parentName*/, ACS_APGCC_AttrValues **/*attr*/){

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_PRC_IspAdminOperationImplementer::deleted(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/, const char */*objName*/){

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_PRC_IspAdminOperationImplementer::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods){

	ACS_APGCC_CcbHandle ccbHandleVal;
	ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;
	ACS_CC_ImmParameter par;
	int increment = 64;
	long long new_value;

	ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal );

	if ( retVal == ACS_CC_FAILURE )
		return retVal;

	new_value = increment * ((*reinterpret_cast<long long *>(attrMods[0]->modAttr.attrValues[0]) + increment - 1)/increment);

	par.attrName = attrMods[0]->modAttr.attrName;
	par.attrType = attrMods[0]->modAttr.attrType;
	par.attrValuesNum = 1;
	par.attrValues = new void*[par.attrValuesNum];
	par.attrValues[0] = reinterpret_cast<void*>(&new_value);

	retVal = modifyObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, objName, &par);

	if ( retVal == ACS_CC_FAILURE )
		return retVal;

	retVal = applyAugmentCcb (ccbHandleVal);

	if ( retVal == ACS_CC_FAILURE )
		return retVal;

	update_restart_logrotd ( new_value / 2);

	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_PRC_IspAdminOperationImplementer::complete(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/){

	return ACS_CC_SUCCESS;
}

void ACS_PRC_IspAdminOperationImplementer::abort(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/){

}

void ACS_PRC_IspAdminOperationImplementer::apply(ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/){

	std::string remote_hostname;
	ifstream ifs;

	ifs.open("/etc/cluster/nodes/peer/hostname");
	if (ifs.good())
		getline(ifs, remote_hostname);
	ifs.close();
        //TR HV63313 change start
	//ACE_SOCK_Connector serverConnector;
	//ACE_SOCK_Stream serverStream;
	//ACE_INET_Addr server_addr(8882,remote_hostname.c_str());

	//serverConnector.connect ( serverStream, server_addr );

	//serverStream.send_n ( "UPDATE_LOG_SIZE", 16 );

        //serverStream.close();
   	//TR HV63313 change stop   //As the prcisp_syslog.log rotation is handled by /etc/logrot.d/acs_prc_isp_conf file
                              // commenting this part to avoid duplication in /etc/logrot.d/default

}

ACS_CC_ReturnType ACS_PRC_IspAdminOperationImplementer::updateRuntime(const char* /*p_objName*/, const char* /*p_attrName*/){

	return ACS_CC_SUCCESS;
}
