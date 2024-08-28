//******************************************************************************
//
// NAME
//      acs_prc_brfImplementerThread.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2003.
// All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.

// DOCUMENT NO
//      19089-CAA 109 0520

// AUTHOR
//      2010-12-29 by xlucpet

// REVISION
//  -
//

// CHANGES

//
//      REV NO          DATE            NAME            DESCRIPTION
//                      2014-04-23      XSARSES         Fix for HS53134

// SEE ALSO
//
//
//******************************************************************************

#include "acs_prc_brfImplementerThread.h"

#define ACS_PRC_STRINGIZER(s) #s
#define ACS_PRC_STRINGIZE(s) ACS_PRC_STRINGIZER(s)

#if defined (ACS_PRCBIN_REVISION)
#define PRCBIN_REVISION ACS_PRC_STRINGIZE(ACS_PRCBIN_REVISION)
#else
#define PRCBIN_REVISION "UNKNOW"
#endif

const char dnObjName_node1[] = "brfPersistentDataOwnerId=ACS-PRC-1,brfParticipantContainerId=1";
const char dnObjName_node2[] = "brfPersistentDataOwnerId=ACS-PRC-2,brfParticipantContainerId=1";
char strError_no[1024] = {0};

acs_prc_brfImplementerThread::acs_prc_brfImplementerThread ( std::string local_node_id, std::string local_node_name ) {
	sleep = false;
	p_local_node_id = local_node_id;
	p_local_node_name = local_node_name;
}

acs_prc_brfImplementerThread::~acs_prc_brfImplementerThread() {
}

int acs_prc_brfImplementerThread::removePartecipant_brf ( string local_node_id, string local_host_name ) {

	ACS_CC_ReturnType returnCode;

	ACS_TRA_Logging Logging;
	Logging.Open("PRC");

	char* rdnValue;

	OmHandler immHandler;

    returnCode = immHandler.Init();

    if (returnCode != ACS_CC_SUCCESS){
        memset(strError_no, 0, sizeof(strError_no));
	    snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread::removePartecipant_brf() OmHandler.Init fails", PRCBIN_REVISION);
	    Logging.Write(strError_no, LOG_LEVEL_ERROR);
        Logging.Close();
        return returnCode;
    }

	if ( local_node_id == "1" )
		rdnValue = const_cast<char*>("brfPersistentDataOwnerId=ACS-PRC-1,brfParticipantContainerId=1");
	else if (local_node_id == "2")
		rdnValue = const_cast<char*>("brfPersistentDataOwnerId=ACS-PRC-2,brfParticipantContainerId=1");

	else{
		if(strcmp(local_host_name.c_str(), "SC-2-1") == 0)
		{
			rdnValue = const_cast<char*>("brfPersistentDataOwnerId=ACS-PRC-1,brfParticipantContainerId=1");
			memset(strError_no, 0, sizeof(strError_no));
			snprintf(strError_no,1024,"acs_prcmand %s - removePartecipant_brf - RDN ACS-PRC-1 fetched", PRCBIN_REVISION );
			Logging.Write(strError_no, LOG_LEVEL_TRACE);
		}

		else if(strcmp(local_host_name.c_str(), "SC-2-2") == 0)
		{
			rdnValue = const_cast<char*>("brfPersistentDataOwnerId=ACS-PRC-2,brfParticipantContainerId=1");
			memset(strError_no, 0, sizeof(strError_no));
			snprintf(strError_no,1024,"acs_prcmand %s - removePartecipant_brf - RDN ACS-PRC-2 fetched", PRCBIN_REVISION);
			Logging.Write(strError_no, LOG_LEVEL_TRACE);
		}

		else
		{
			memset(strError_no, 0, sizeof(strError_no));
			snprintf(strError_no,1024,"acs_prcmand %s - removePartecipant_brf: Wrong node information - Failure in delete BRF object", PRCBIN_REVISION);
			Logging.Write(strError_no, LOG_LEVEL_ERROR);
			Logging.Close();

			return -1;
		}

	}

	returnCode=immHandler.deleteObject( rdnValue );

	if( returnCode != ACS_CC_SUCCESS ){
        memset(strError_no, 0, sizeof(strError_no));
        snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread::removePartecipant_brf.deleteObject() - BRF Object [ %s ] remove fails.", PRCBIN_REVISION, rdnValue);
        Logging.Write(strError_no, LOG_LEVEL_ERROR);

		immHandler.Finalize();
		Logging.Close();
		return returnCode;
	} else {
       memset(strError_no, 0, sizeof(strError_no));
       snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread::removePartecipant_brf.deleteObject() - BRF Object [ %s ] remove success.",  PRCBIN_REVISION, rdnValue);
       Logging.Write(strError_no, LOG_LEVEL_ERROR);
    }    

	returnCode=immHandler.Finalize();

	if( returnCode != ACS_CC_SUCCESS){
		Logging.Close();
		return returnCode;
	}          

	Logging.Close();
	return 0;
}

int acs_prc_brfImplementerThread::createParticipant ( std::string local_node_id, std::string local_node_name ) {

	ACS_TRA_Logging Logging;

	vector<string> tmp_vector;
	vector<string>::iterator it;

	string tmp_rdn;

	Logging.Open("PRC");

	memset(strError_no, 0, sizeof(strError_no));
	snprintf(strError_no,1024,"acs_prcmand %s - createParticipant function Start ", PRCBIN_REVISION );
	Logging.Write(strError_no, LOG_LEVEL_ERROR);

	char attrdn[]= "brfPersistentDataOwnerId";
	char attrVersion[]="version";
	char attrBackupType[]="backupType";

	char *className = const_cast<char*>("BrfPersistentDataOwner");

	ACS_CC_ReturnType returnCode;
	int last_error = 0;
	OmHandler immHandler;

	do {
		returnCode = immHandler.Init();

		if ( returnCode != ACS_CC_SUCCESS ){
			last_error = immHandler.getInternalLastError();
			snprintf(strError_no,1024,"acs_prcmand %s - createParticipant function OmHandler.Init fails ( %i )", PRCBIN_REVISION, last_error );
			Logging.Write(strError_no, LOG_LEVEL_ERROR);
			usleep(500000); // wait 0,5 seconds before try again
		}
	}while ( ( returnCode != ACS_CC_SUCCESS ) && ( !sleep ) );

	memset(strError_no, 0, sizeof(strError_no));

	if ( sleep ){
		snprintf(strError_no,1024,"acs_prcmand %s - createParticipant BRF thread in closing phase", PRCBIN_REVISION );
		Logging.Write(strError_no, LOG_LEVEL_ERROR);
		Logging.Close();
		immHandler.Finalize();
		return 0;
	}

	memset(strError_no, 0, sizeof(strError_no));
	snprintf(strError_no,1024,"acs_prcmand %s - createParticipant function OmHandler.Init OK", PRCBIN_REVISION );
	Logging.Write(strError_no, LOG_LEVEL_ERROR);

	immHandler.getClassInstances("BrfPersistentDataOwner",tmp_vector);

	if ( local_node_id == "1" ){
		tmp_rdn = dnObjName_node1;
	}
	else if ( local_node_id == "2" ) {
		tmp_rdn = dnObjName_node2;
	}
	else{
		if(strcmp(local_node_name.c_str(), "SC-2-1") == 0)
		{
			tmp_rdn = dnObjName_node1;
			memset(strError_no, 0, sizeof(strError_no));
			snprintf(strError_no,1024,"acs_prcmand %s - createParticipant function - RDN ACS-PRC-1 fetched", PRCBIN_REVISION );
			Logging.Write(strError_no, LOG_LEVEL_TRACE);
		}

		else if(strcmp(local_node_name.c_str(), "SC-2-2") == 0)
		{
			tmp_rdn = dnObjName_node2;
			memset(strError_no, 0, sizeof(strError_no));
			snprintf(strError_no,1024,"acs_prcmand %s - createParticipant function  - RDN ACS-PRC-2 fetched", PRCBIN_REVISION);
			Logging.Write(strError_no, LOG_LEVEL_TRACE);
		}

		else
		{
			memset(strError_no, 0, sizeof(strError_no));
			snprintf(strError_no,1024,"acs_prcmand %s - createParticipant function : Wrong node information - Failure in create Participant", PRCBIN_REVISION);
			Logging.Write(strError_no, LOG_LEVEL_ERROR);
			Logging.Close();
			immHandler.Finalize();
			return -1;
		}

	}


	bool found = false;
	for ( it = tmp_vector.begin(); it != tmp_vector.end(); it++ ){
		if ( *it == tmp_rdn ){
			found = true;
			break;
		}
	}

	if ( found ){
		memset(strError_no, 0, sizeof(strError_no));
		snprintf(strError_no,1024,"acs_prcmand %s - createParticipant function BRF object already exist", PRCBIN_REVISION );
		Logging.Write(strError_no, LOG_LEVEL_ERROR);
		Logging.Close();
		immHandler.Finalize();
		return 0;
	}

	char* nomeParent = const_cast<char*>("brfParticipantContainerId=1");

	vector<ACS_CC_ValuesDefinitionType> AttrList;

	ACS_CC_ValuesDefinitionType attributeRDN;
	ACS_CC_ValuesDefinitionType attributeVersion;
	ACS_CC_ValuesDefinitionType attributeBackupType;

	attributeRDN.attrName = attrdn;
	attributeRDN.attrType = ATTR_STRINGT;
	attributeRDN.attrValuesNum = 1;

	char* rdnValue;

	if ( local_node_id == "1" )
		rdnValue = const_cast<char*>("brfPersistentDataOwnerId=ACS-PRC-1");
	else if ( local_node_id == "2" )
		rdnValue = const_cast<char*>("brfPersistentDataOwnerId=ACS-PRC-2");

	else{
                //changes for HV92897
		if(strcmp(local_node_name.c_str(), "SC-2-1") == 0)
		{
			rdnValue = const_cast<char*>("brfPersistentDataOwnerId=ACS-PRC-1");
			memset(strError_no, 0, sizeof(strError_no));
			snprintf(strError_no,1024,"acs_prcmand %s - createParticipant function - RDN ACS-PRC-1 fetched", PRCBIN_REVISION );
			Logging.Write(strError_no, LOG_LEVEL_TRACE);
		}

		else if(strcmp(local_node_name.c_str(), "SC-2-2") == 0)
		{
			rdnValue = const_cast<char*>("brfPersistentDataOwnerId=ACS-PRC-2");
			memset(strError_no, 0, sizeof(strError_no));
			snprintf(strError_no,1024,"acs_prcmand %s - createParticipant fu_brfnction  - RDN ACS-PRC-2 fetched", PRCBIN_REVISION);
			Logging.Write(strError_no, LOG_LEVEL_TRACE);
		}

		else
		{
			memset(strError_no, 0, sizeof(strError_no));
			snprintf(strError_no,1024,"acs_prcmand %s - createParticipant function : Wrong node information - Failure in fetching rdnvalue", PRCBIN_REVISION);
			Logging.Write(strError_no, LOG_LEVEL_ERROR);
			Logging.Close();
			immHandler.Finalize();
			return -1;
		}

	}


	void* value[1]={ reinterpret_cast<void*>(rdnValue)};
	attributeRDN.attrValues = value;

	attributeVersion.attrName = attrVersion;
	attributeVersion.attrType = ATTR_STRINGT;
	attributeVersion.attrValuesNum = 1;
	char* strValue = const_cast<char*>("1.0");
	void* valueStr[1]={ reinterpret_cast<void*>(strValue) };
	attributeVersion.attrValues = valueStr;

	attributeBackupType.attrName = attrBackupType;
	attributeBackupType.attrType = ATTR_INT32T;
	attributeBackupType.attrValuesNum = 1;
	int intAttributeBackupType = 1;
	void* valueInt[1]={ reinterpret_cast<void*>(&intAttributeBackupType)};
	attributeBackupType.attrValues = valueInt;

	AttrList.push_back(attributeRDN);
	AttrList.push_back(attributeVersion);
	AttrList.push_back(attributeBackupType);
	bool immHandleReInitNeeded = false;
	do {
		if(immHandleReInitNeeded) {		// TR HZ18947
			if(immHandler.Finalize() != ACS_CC_SUCCESS) {
				snprintf(strError_no,1024,"acs_prcmand %s - createParticipant function - immHandle is BAD_HANDLE. BUT OmHandler.Finalize() failed ( %i )", PRCBIN_REVISION, immHandler.getInternalLastError());
				Logging.Write(strError_no, LOG_LEVEL_ERROR);
			}
			returnCode = immHandler.Init();
			if (returnCode != ACS_CC_SUCCESS) {
				last_error = immHandler.getInternalLastError();
				immHandleReInitNeeded = true;
				memset(strError_no, 0, sizeof(strError_no));
				snprintf(strError_no,1024,"acs_prcmand %s - createParticipant function  - immHandle is BAD_HANDLE. BUT OmHandler.Init() failed ( %i )", PRCBIN_REVISION, last_error);
				Logging.Write(strError_no, LOG_LEVEL_ERROR);
				usleep(500000); // wait 0,5 seconds before try again
				continue;
			}
			else
				immHandleReInitNeeded = false;
		}
		returnCode = immHandler.createObject(className, nomeParent, AttrList);;

		if ( returnCode != ACS_CC_SUCCESS ) {
			last_error = immHandler.getInternalLastError();
			if(last_error == -9)	// SA_AIS_ERR_BAD_HANDLE
				immHandleReInitNeeded = true;		// TR HZ18947

			memset(strError_no, 0, sizeof(strError_no));
			snprintf(strError_no,1024,"acs_prcmand %s - createParticipant function OmHandler.createObject fails ( %i )", PRCBIN_REVISION, last_error );
			Logging.Write(strError_no, LOG_LEVEL_ERROR);

			usleep(500000); // wait 0,5 seconds before try again
		}
	}while ( ( returnCode != ACS_CC_SUCCESS ) && ( !sleep ) );

	memset(strError_no, 0, sizeof(strError_no));

	if ( sleep ){
		snprintf(strError_no,1024,"acs_prcmand %s - createParticipant BRF thread in closing phase", PRCBIN_REVISION );
	}
	else {
		snprintf(strError_no,1024,"acs_prcmand %s - createParticipant function OmHandler.createObject OK", PRCBIN_REVISION );
	}

	Logging.Write(strError_no, LOG_LEVEL_ERROR);

	returnCode = immHandler.Finalize();

	if ( returnCode != ACS_CC_SUCCESS ){
		memset(strError_no, 0, sizeof(strError_no));
		snprintf(strError_no,1024,"acs_prcmand %s - createParticipant function OmHandler.Finalize fails ( %i ) ", PRCBIN_REVISION, last_error );
		Logging.Write(strError_no, LOG_LEVEL_ERROR);
		Logging.Close();
		return 0;
	}

	memset(strError_no, 0, sizeof(strError_no));
	snprintf(strError_no,1024,"acs_prcmand %s - createParticipant function End ", PRCBIN_REVISION );
	Logging.Write(strError_no, LOG_LEVEL_ERROR);

	Logging.Close();

	return 0;
}

int acs_prc_brfImplementerThread:: svc ( void ){

	ACS_CC_ReturnType returnCode;

	Logging.Open("PRC");

	if( createParticipant ( p_local_node_id, p_local_node_name ) < 0 ) {
		memset(strError_no, 0, sizeof(strError_no));
		snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread - failure in create brf participant - End", PRCBIN_REVISION );
		Logging.Write(strError_no, LOG_LEVEL_WARN);
		Logging.Close();

		return 0;

	}

	if ( sleep ){

		memset(strError_no, 0, sizeof(strError_no));
		snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread shutdown phase - Start", PRCBIN_REVISION);
		Logging.Write(strError_no, LOG_LEVEL_ERROR);

		int remove_error = 0;
		int retry = 5;
		do {
			remove_error = removePartecipant_brf ( p_local_node_id, p_local_node_name );
			if ( remove_error != ACS_CC_SUCCESS ){
				if ( remove_error == -12 ){ // object doesn't exist
					memset(strError_no, 0, sizeof(strError_no));
					snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread shutdown phase - BRF object doesn't exist - End", PRCBIN_REVISION);
					Logging.Write(strError_no, LOG_LEVEL_ERROR);
					Logging.Close();
					return 0;
				}
				else {
					memset(strError_no, 0, sizeof(strError_no));
					snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread shutdown phase - removePartecipant_brf fails ( %i )", PRCBIN_REVISION, remove_error);
					Logging.Write(strError_no, LOG_LEVEL_ERROR);
				}
				usleep(500000); // wait 0,5 seconds before try again
				retry--;
			}
		}while ( ( remove_error != ACS_CC_SUCCESS) && ( retry >= 0 ) );

		memset(strError_no, 0, sizeof(strError_no));
		snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread shutdown phase - BRF object removed - End", PRCBIN_REVISION );
		Logging.Write(strError_no, LOG_LEVEL_WARN);
		Logging.Close();
		return 0;
	}

	if ( p_local_node_id == "1" ){
		BRFImplementer.setObjName (dnObjName_node1);
		BRFImplementer.setImpName ("PRCMAN_BRF_1");
	}
	else if (p_local_node_id == "2"){ 
		BRFImplementer.setObjName (dnObjName_node2);
		BRFImplementer.setImpName ("PRCMAN_BRF_2");
	}
	else{
                //changes for HV92897
		if(strcmp(p_local_node_name.c_str(), "SC-2-1") == 0)
		{
			BRFImplementer.setObjName (dnObjName_node1);
			BRFImplementer.setImpName ("PRCMAN_BRF_1");
			memset(strError_no, 0, sizeof(strError_no));
			snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread - RDN ACS-PRC-1 fetched", PRCBIN_REVISION );
			Logging.Write(strError_no, LOG_LEVEL_TRACE);
		}

		else if(strcmp(p_local_node_name.c_str(), "SC-2-2") == 0)
		{
			BRFImplementer.setObjName (dnObjName_node2);
			BRFImplementer.setImpName ("PRCMAN_BRF_2");
			memset(strError_no, 0, sizeof(strError_no));
			snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread - RDN ACS-PRC-2 fetched", PRCBIN_REVISION);
			Logging.Write(strError_no, LOG_LEVEL_TRACE);
		}

		else
		{
			memset(strError_no, 0, sizeof(strError_no));
			snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread: Wrong node information - Failure in delete BRF object - End", PRCBIN_REVISION);
			Logging.Write(strError_no, LOG_LEVEL_ERROR);
			Logging.Close();

			return -1;
		}

	}

	BRFImplementer.setScope ( ACS_APGCC_ONE );

	if ( !sleep ){
		do {
			returnCode = oiHandler.addObjectImpl(&BRFImplementer);
			if ( returnCode != ACS_CC_SUCCESS ){
				memset(strError_no, 0, sizeof(strError_no));
				snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread function oiHandler.addObjectImpl fails", PRCBIN_REVISION);
				Logging.Write(strError_no, LOG_LEVEL_ERROR);
				usleep(500000); // wait 0,5 seconds before try again
			}
		}while ( ( returnCode != ACS_CC_SUCCESS ) && ( !sleep ) );
	}

	memset(strError_no, 0, sizeof(strError_no));
	snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread BRF object created successfully and implementer is set", PRCBIN_REVISION );
	Logging.Write(strError_no, LOG_LEVEL_WARN);

	int ret;
	struct pollfd fds[1];

	fds[0].fd = BRFImplementer.getSelObj();
	fds[0].events = POLLIN;

	while( !sleep )
	{
		ret = poll(fds, 1, 500);

		if ( ret == 0 ) continue;

		if (ret == -1)
			printf("poll error: \n");
		else {
			if ( sleep ) {
				//return 0;
				break;
			}
			BRFImplementer.dispatch(ACS_APGCC_DISPATCH_ALL);
		}
	}

	memset(strError_no, 0, sizeof(strError_no));
	snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread shutdown phase - Start", PRCBIN_REVISION);
	Logging.Write(strError_no, LOG_LEVEL_ERROR);

	do {
		returnCode = oiHandler.removeObjectImpl(&BRFImplementer);
		if ( returnCode != ACS_CC_SUCCESS ){
			memset(strError_no, 0, sizeof(strError_no));
			snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread function oiHandler.removeObjectImpl fails", PRCBIN_REVISION);
			Logging.Write(strError_no, LOG_LEVEL_ERROR);
			usleep(500000); // wait 0,5 seconds before try again
		}
	}while ( ( returnCode != ACS_CC_SUCCESS ) && ( !sleep ) );

	int remove_error = 0;
	int retry = 10;
	do {
		remove_error = removePartecipant_brf ( p_local_node_id, p_local_node_name );
		if ( remove_error != ACS_CC_SUCCESS ){
			if ( remove_error == -12 ){ // object doesn't exist
				memset(strError_no, 0, sizeof(strError_no));
				snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread shutdown phase - BRF object doesn't exist - End", PRCBIN_REVISION);
				Logging.Write(strError_no, LOG_LEVEL_ERROR);
				Logging.Close();
				return 0;
			}
			else {
				memset(strError_no, 0, sizeof(strError_no));
				snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread shutdown phase - removePartecipant_brf fails ( %i )", PRCBIN_REVISION, remove_error);
				Logging.Write(strError_no, LOG_LEVEL_ERROR);
			}
			usleep(500000); // wait 0,5 seconds before try again
			retry--;
		}
	}while ( ( remove_error != ACS_CC_SUCCESS) && ( retry >= 0 ) );

	memset(strError_no, 0, sizeof(strError_no));
	snprintf(strError_no,1024,"acs_prcmand %s - acs_prc_brfImplementerThread shutdown phase - BRF object removed - End", PRCBIN_REVISION );
	Logging.Write(strError_no, LOG_LEVEL_WARN);
	Logging.Close();

	return 0;
}

