#include "acs_prc_eventmatrix_pipe_handler.h"
#include "acs_prc_types.h"
#include "acs_prc_api.h"
#include "acs_apgcc_omhandler.h"
#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <ace/LSOCK_Connector.h>

#define ACS_PRC_STRINGIZER(s) #s
#define ACS_PRC_STRINGIZE(s) ACS_PRC_STRINGIZER(s)

#if defined (ACS_PRCBIN_REVISION)
#define PRCBIN_REVISION ACS_PRC_STRINGIZE(ACS_PRCBIN_REVISION)
#else
#define PRCBIN_REVISION "UNKNOW"
#endif

#define SOFTWARE_INVENTORY  "CmwSwMUpgradePackage"

const char eventMatrix_pipe[] = "/var/run/ap/acs_prcevmaxfifo";
const char pipeName[] = "/var/run/ap/PrcManServer";

void __CLASS_NAME__::parseAPEvent ( char* buffer ){



	char* token;
	int index = 1;

	token = strtok( buffer, " " );

	while (token != NULL) {
		if ( index == 8 )
			process_name = (string)token;
		else if ( index == 9 )
			specific_problem = (string)token;

		index++;
		token = strtok (NULL, " ");
	}
}

void __CLASS_NAME__::performAction ( std::string action){

	ACS_PRC_API prcapi;

	if ( action == "failover" ) {

		char strErr_no[1024] = {0};
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,sizeof(strErr_no),"acs_prcmand %s - PRCEVA has to reboot the node due to EventMatrix action ( failover )", PRCBIN_REVISION );
	    Logging->Write(strErr_no, LOG_LEVEL_ERROR);

		prcapi.rebootNode(causedByEvent, "failover due to PRCEVA", true);

	}
	else if ( action == "changeactivenode" ){

		if ( prcapi.askForNodeState() != ACS_PRC_NODE_ACTIVE ){

			char other_Node_serverName[16] = {0};

			ifstream ifs;
			ifs.open("/etc/cluster/nodes/peer/hostname");
			if ( ifs.good())
				ifs.getline(other_Node_serverName, 16);
			ifs.close();

			ACE_SOCK_Connector  serverConnector;
			ACE_SOCK_Stream serverStream;
			ACE_INET_Addr server_addr(8881,other_Node_serverName);
			ACE_Time_Value val(3);

			char strErr_no[1024] = {0};

			if ( serverConnector.connect (serverStream, server_addr, &val  ) != -1){ // Connection to other node OK

				snprintf(strErr_no,sizeof(strErr_no),"acs_prcmand %s - PRCEVA has successfully established a connection with the acs_prcmand on the other node", PRCBIN_REVISION );
				Logging->Write(strErr_no, LOG_LEVEL_ERROR);

				if ( serverStream.send_n ("REBOOT_PRCEVA", 14 ) == -1 ){
					// Error
					memset(strErr_no, 0, sizeof(strErr_no));
					snprintf(strErr_no,sizeof(strErr_no),"acs_prcmand %s - PRCEVA has failed to sent REBBOT_PRCEVA to the other acs_prcmand", PRCBIN_REVISION );
					Logging->Write(strErr_no, LOG_LEVEL_ERROR);
				}
			}
			else {
				snprintf(strErr_no,sizeof(strErr_no),"acs_prcmand %s - PRCEVA - Connection to other node refused - skip the event", PRCBIN_REVISION );
				Logging->Write(strErr_no, LOG_LEVEL_ERROR);
				// Connection to other node refused - skip the event
			}

			serverStream.close();
		}
	}
	else if ( action == "stop" ){
		// No more supported in APG43L
	}
	else if ( action == "restart" ){
		// No more supported in APG43L
	}

	Logging->Close();
}

int __CLASS_NAME__::checkEventMatrix ( std::string process_name, std::string specific_problem, std::string& action ) {

    OmHandler prc_OmHandler;
    vector<string> tmp_vector;
    vector<string>::iterator it;
    vector<ACS_APGCC_ImmAttribute>::iterator it_object;
    string tmp_proc_name, tmp_action, tmp_spec_problem;
    ACS_APGCC_ImmObject object;
    ACS_CC_ReturnType returnCode;     // Start of TR HZ92239
    char strErr_no[1024] = {0};
    for (int counter=0;counter<3;counter++)
    {
        returnCode = prc_OmHandler.Init();
        if ( returnCode != ACS_CC_SUCCESS)
        {
           ::memset(strErr_no, 0, sizeof(strErr_no));
           ::snprintf(strErr_no,sizeof(strErr_no),"acs_prcmand %s - %s: OmHandler.Init() failed for %d time!",  PRCBIN_REVISION, __PRETTY_FUNCTION__,counter);
           Logging->Write(strErr_no, LOG_LEVEL_WARN);
           if ( counter >= 3)
           {
              ::memset(strErr_no, 0, sizeof(strErr_no));
              ::snprintf(strErr_no,sizeof(strErr_no), "acs_prcmand %s - %s: OmHandler.Init() is failed after 3 retries !!!",PRCBIN_REVISION, __PRETTY_FUNCTION__);
              Logging->Write(strErr_no, LOG_LEVEL_ERROR);
              Logging->Close();
              return returnCode;
           }
           else
           {
               usleep(500000);
               continue;
           }
           
         }
         else
                break;
     }// End of TR HZ92239 

    prc_OmHandler.getClassInstances("EventMatrix",tmp_vector);

    for ( it = tmp_vector.begin(); it != tmp_vector.end(); it++ ){

    	object.objName = *it;

    	prc_OmHandler.getObject( &object );

    	for ( it_object = object.attributes.begin(); it_object != object.attributes.end(); it_object++ ){
    		if ( (*it_object).attrName == "processName" ) {
    			tmp_proc_name = (char*)((*it_object).attrValues[0]);
    		}
    		else if ( (*it_object).attrName == "specificProblem" ){
    			stringstream ss;
    			ss << *(reinterpret_cast<int*>((*it_object).attrValues[0]));
    			tmp_spec_problem = ss.str();
    			ss.clear();
    		}
    		else if ( (*it_object).attrName == "eventAction" ){
    			tmp_action = (char*)((*it_object).attrValues[0]);
    		}
    	}

    	if (( tmp_proc_name == process_name ) && ( tmp_spec_problem == specific_problem )){
    		action = tmp_action;
    		//prc_OmHandler.Finalize();
                int retry = 0;//start of IA40986
                do
                {
                        returnCode = prc_OmHandler.Finalize();
                        if (returnCode != ACS_CC_SUCCESS){
                                ::memset(strErr_no, 0, sizeof(strErr_no));
                                ::snprintf(strErr_no,sizeof(strErr_no),"acs_prcmand %s - %s: OmHandler.Finalize() failed for %d time!",  PRCBIN_REVISION, __PRETTY_FUNCTION__,retry);
                                Logging->Write(strErr_no, LOG_LEVEL_WARN);
                                usleep(500000);
                                retry++;
                        }
                }while((returnCode != ACS_CC_SUCCESS) && ( retry < 3 ));
                if(returnCode != ACS_CC_SUCCESS){
                        ::memset(strErr_no, 0, sizeof(strErr_no));
                        ::snprintf(strErr_no,sizeof(strErr_no),"acs_prcmand %s - %s: OmHandler.Finalize() is failed after 3 retries !!!",  PRCBIN_REVISION, __PRETTY_FUNCTION__);
                        Logging->Write(strErr_no, LOG_LEVEL_WARN);
                }
    		return 0;
    	}
    }

    //prc_OmHandler.Finalize();
    int finalize_retry = 0;
    do
    {
        returnCode = prc_OmHandler.Finalize();
        if (returnCode != ACS_CC_SUCCESS){
                ::memset(strErr_no, 0, sizeof(strErr_no));
                ::snprintf(strErr_no,sizeof(strErr_no),"acs_prcmand %s - %s: OmHandler.Finalize() failed for %d time!",  PRCBIN_REVISION, __PRETTY_FUNCTION__,finalize_retry);
                Logging->Write(strErr_no, LOG_LEVEL_WARN);
                usleep(500000);
                finalize_retry++;
        }
    }while((returnCode != ACS_CC_SUCCESS) && ( finalize_retry < 3 ));
    if(returnCode != ACS_CC_SUCCESS){
        ::memset(strErr_no, 0, sizeof(strErr_no));
        ::snprintf(strErr_no,sizeof(strErr_no),"acs_prcmand %s - %s: OmHandler.Finalize() is failed after 3 retries !!!",  PRCBIN_REVISION, __PRETTY_FUNCTION__);
        Logging->Write(strErr_no, LOG_LEVEL_WARN);
    }// end of IA40986
	return -1;
}

//Start of TR HX84520
int __CLASS_NAME__::getSwUpgradeOnGoing (){

	char strErr_no[1024] = {0};
	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,sizeof(strErr_no),"Entry in getSwUpgradeOnGoing()");
	Logging->Write(strErr_no, LOG_LEVEL_TRACE);

	ACS_CC_ReturnType returnCode;
	int last_error = 0;
	OmHandler immHandler;
	vector<string> tmp_vector;

	returnCode=immHandler.Init();

	if( returnCode != ACS_CC_SUCCESS ){
		last_error = immHandler.getInternalLastError();
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,sizeof(strErr_no),"getSwUpgradeOnGoing function. Init fails (%i)", last_error);
		Logging->Write(strErr_no, LOG_LEVEL_ERROR);
		return 0;
	}

	returnCode = immHandler.getClassInstances(SOFTWARE_INVENTORY,tmp_vector);

	if( (returnCode != ACS_CC_SUCCESS) || (tmp_vector.size()==0) ){
		last_error = immHandler.getInternalLastError();
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,sizeof(strErr_no),"Couldn't find CmwSwMUpgradePackage list (%i)", last_error);
		Logging->Write(strErr_no, LOG_LEVEL_ERROR);
		immHandler.Finalize();
		return 0;
	}


	string rdn_Software;
	int isSwUpgrade_flag = 0;

	ACS_CC_ImmParameter Software_DN;
	Software_DN.attrName = const_cast<char*>("reportProgress");

	ACS_CC_ImmParameter Software_actionId;
	Software_actionId.attrName = const_cast<char*>("actionId");


	for ( unsigned int i = 0; i < tmp_vector.size(); i++){

		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,sizeof(strErr_no),"CmwSwMUpgradePackage list %s", tmp_vector[i].c_str());
		Logging->Write(strErr_no, LOG_LEVEL_INFO);

		returnCode = immHandler.getAttribute(tmp_vector[i].c_str(), &Software_DN);

		if ( returnCode != ACS_CC_SUCCESS ){
			last_error = immHandler.getInternalLastError();
			memset(strErr_no, 0, sizeof(strErr_no));
			snprintf(strErr_no,sizeof(strErr_no),"getAttribute of reportProgress DN is failed (%i)", last_error);
			Logging->Write(strErr_no, LOG_LEVEL_ERROR);
			continue;
		}

		if(Software_DN.attrValues[0] != 0){
			rdn_Software = reinterpret_cast<char*>(Software_DN.attrValues[0]);
		}

		returnCode = immHandler.getAttribute( rdn_Software.c_str(), &Software_actionId);

		if ( returnCode != ACS_CC_SUCCESS ){
			memset(strErr_no, 0, sizeof(strErr_no));
			snprintf(strErr_no,sizeof(strErr_no),"getAttribute of actionId is not successful");
			Logging->Write(strErr_no, LOG_LEVEL_ERROR);
			continue;
		}

		if (Software_actionId.attrValues[0] != 0){
			memset(strErr_no, 0, sizeof(strErr_no));
			snprintf(strErr_no,sizeof(strErr_no),"CmwSwMUpgradePackage Software actionId %d", *(reinterpret_cast<int*>(Software_actionId.attrValues[0])));
			Logging->Write(strErr_no, LOG_LEVEL_DEBUG);

			if ( *(reinterpret_cast<int*>(Software_actionId.attrValues[0])) == 1 ){ // Software Upgrade ongoing

				isSwUpgrade_flag = 1;
				memset( strErr_no, 0, sizeof(strErr_no));
				snprintf(strErr_no,sizeof(strErr_no),"isSwUpgrade_flag = %d", isSwUpgrade_flag );
				Logging->Write(strErr_no, LOG_LEVEL_WARN);
				break;
			}
		}
	}

	returnCode=immHandler.Finalize();

	if( returnCode != ACS_CC_SUCCESS){
		memset(strErr_no, 0, sizeof(strErr_no));
		snprintf(strErr_no,sizeof(strErr_no),"ImmHandler.Finalize is not successful");
		Logging->Write(strErr_no, LOG_LEVEL_ERROR);
		return 0;
	}

	memset(strErr_no, 0, sizeof(strErr_no));
	snprintf(strErr_no,sizeof(strErr_no),"Exit from getSwUpgradeOnGoing()");
	Logging->Write(strErr_no, LOG_LEVEL_TRACE);

	return isSwUpgrade_flag;

}

int __CLASS_NAME__::getRestoreOnGoing (){

	OmHandler immHandle;
	vector<ACS_APGCC_ImmAttribute *> attributes;
	ACS_CC_ReturnType result;
	vector<string> tmp_vector;

	result = immHandle.Init();

	if ( result != ACS_CC_SUCCESS ){
		return 0;
	}

	result = immHandle.getClassInstances("BrMBrmBackup",tmp_vector);

	if ( result != ACS_CC_SUCCESS ){
		immHandle.Finalize();
		return 0;
	}

	string rdn_Backup;
	int isRestore_flag = 0;

	ACS_CC_ImmParameter Backup_DN;
	Backup_DN.attrName = const_cast<char*>("progressReport");

	ACS_CC_ImmParameter Backup_actionId;
	Backup_actionId.attrName = const_cast<char*>("actionId");

	ACS_CC_ImmParameter Backup_result;
	Backup_result.attrName = const_cast<char*>("result");

	for ( unsigned int i = 0; i < tmp_vector.size(); i++){

		result = immHandle.getAttribute(tmp_vector[i].c_str(), &Backup_DN);

		if ( result != ACS_CC_SUCCESS ){

			if ( immHandle.getInternalLastError() == -12 ){ // Invalid parameter
				isRestore_flag = -1;
				break;
			}

			continue;
		}

		if (Backup_DN.attrValues[0] == 0) continue;

		rdn_Backup = reinterpret_cast<char*>(Backup_DN.attrValues[0]);

		result = immHandle.getAttribute( rdn_Backup.c_str(), &Backup_actionId);

		if ( result != ACS_CC_SUCCESS ){
			continue;
		}

		if ( *(reinterpret_cast<int*>(Backup_actionId.attrValues[0])) == 3 ){ // Restore ongoing

			result = immHandle.getAttribute( rdn_Backup.c_str(), &Backup_result);

			if ( result != ACS_CC_SUCCESS ){
				continue;
			}

			if ( *(reinterpret_cast<int*>(Backup_result.attrValues[0])) == 3 ) { // result is NOT_AVAILABLE
				isRestore_flag = 1;
				break;
			}
		}
	}

	if (isRestore_flag == -1){		//  No restore in progress, this could be due to the progressReport attribute
		//  not defined in IMM. To assure the Backward Compatibility,
		//  a check on the asyncActionProgress attribute is executed.

		Backup_DN.attrName = const_cast<char*>("asyncActionProgress");

		for ( unsigned int i = 0; i < tmp_vector.size(); i++){

			result = immHandle.getAttribute(tmp_vector[i].c_str(), &Backup_DN);

			if ( result != ACS_CC_SUCCESS )  continue;

			if ( Backup_DN.attrValues[0] == 0 )  continue;
			rdn_Backup = reinterpret_cast<char*>(Backup_DN.attrValues[0]);

			result = immHandle.getAttribute( rdn_Backup.c_str(), &Backup_actionId);

			if ( result != ACS_CC_SUCCESS )  continue;

			if ( *(reinterpret_cast<int*>(Backup_actionId.attrValues[0])) == 3 ){     // Restore ongoing

				result = immHandle.getAttribute( rdn_Backup.c_str(), &Backup_result);

				if ( result != ACS_CC_SUCCESS )  continue;

				if ( *(reinterpret_cast<int*>(Backup_result.attrValues[0])) == 3 ) {   // result is NOT_AVAILABLE
					isRestore_flag = 1;
					char strErr_no[1024] = {0};
					memset( strErr_no, 0, sizeof(strErr_no));
					snprintf(strErr_no,sizeof(strErr_no),"isRestore_flag = %d", isRestore_flag );
					Logging->Write(strErr_no, LOG_LEVEL_WARN);
					break;
				}
			}
		}
	}
	immHandle.Finalize();

	return isRestore_flag;
}
//End of TR HX84520

__CLASS_NAME__::__CLASS_NAME__() {

	Logging = new ACS_TRA_Logging();
	Logging->Open("PRC");

	sleep = false;
	//Start of TR HX84520
	counter = 0;
	event_time = 0;
	time1 = 0, time2 = 0, time3 = 0;
	isSwUpgrade_flag = 0;
	isRestore_flag = 0;
	//End of TR HX84520
}

__CLASS_NAME__::~__CLASS_NAME__() {
	delete Logging;
}

int __CLASS_NAME__::svc ( void ){

	int error = 0, fd = -1;

	char buffer[8192] = {0};
	string action;

	if ( (fd = ::open(eventMatrix_pipe, O_RDONLY | O_NONBLOCK )) == -1 ){
		char strErr_no[1024] = {0};
		int error = ACE_OS::last_error();
		snprintf(strErr_no,sizeof(strErr_no),"acs_prcmand %s - %s - ::open from eventMatrix_pipe fails, errno = %d (%s)", PRCBIN_REVISION, __PRETTY_FUNCTION__, error, strerror(error));
		Logging->Write(strErr_no, LOG_LEVEL_ERROR);
		return -1;
	}

	while ( !sleep ){

		// ::read system call returns :
		// -1 on error and the errno will be set appropriately
		// 0 in case of EOF
		// >0 number of byte read

		if ( ::read ( fd, buffer, sizeof(buffer)) > 0 ){

			parseAPEvent ( buffer );


			if ( checkEventMatrix( process_name, specific_problem, action ) == 0){
				//Start of TR HX84520
				if(specific_problem == "9030"){

					ACE_LSOCK_Stream Stream;
					ACE_LSOCK_Connector Connector;
					ACE_UNIX_Addr addr ( pipeName );
					char strErr_no[1024] = {0};
					char buffer[16] = {0};
					ACE_Time_Value val(1);

					if ( Connector.connect ( Stream, addr, &val) >= 0 ){
						if ( Stream.send_n ( "GET_SOFTWARE_UPDATE", 20 ) >= 0 ){
							if ( Stream.recv (buffer, 16) >= 0 ){

								if ( strncmp(buffer,"TRUE",4) == 0 ){
									isSwUpgrade_flag = 1;
									memset( strErr_no, 0, sizeof(strErr_no));
									snprintf(strErr_no,sizeof(strErr_no),"GET_SOFTWARE_UPDATE - isSwUpgrade_flag = %d", isSwUpgrade_flag );
									Logging->Write(strErr_no, LOG_LEVEL_WARN);

								}
								else
									isSwUpgrade_flag = getSwUpgradeOnGoing();

								memset( buffer, 0, sizeof(buffer));
							}
						}
						else
							Stream.close();

						Stream.close();
					}

					isRestore_flag = getRestoreOnGoing();

					if(isSwUpgrade_flag == 1 || isRestore_flag == 1)
					{

						char strErr_no[1024] = {0};

						counter++;

						event_time = time(0);

						memset(strErr_no, 0, sizeof(strErr_no));
						snprintf(strErr_no,sizeof(strErr_no),"event_time is %ld, counter value is %d", event_time, counter);
						Logging->Write(strErr_no, LOG_LEVEL_WARN);

						switch(counter){
						case 1:
							time1 = event_time;
							break;
						case 2:
							time2 = event_time;
							break;
						case 3:
							time3 = event_time;
							break;
						}

						if((counter == 2) && (difftime(time2, time1) >= 125)){
							counter = 1;
							time1 = time2;
						}
						if((counter == 3) && (difftime(time3, time2) >= 125)){
							counter = 1;
							time1 = time3;
						}
						if (counter == 3){

							memset(strErr_no, 0, sizeof(strErr_no));
							snprintf(strErr_no,sizeof(strErr_no),"counter value is %d", counter);
							Logging->Write(strErr_no, LOG_LEVEL_WARN);

							performAction( action );
							counter = 0;
						}
					}
					else
						performAction( action );
				}
				//End of TR HX84520
			}

			memset( buffer, 0, sizeof(buffer));
		}
		else {
			int error = ACE_OS::last_error();

			char strErr_no[1024] = {0};

			switch ( error ){
			case EAGAIN : // No data to read, exit from switch
				break;
			case EBADF  : // Invalid file descriptor
			case EINVAL : // Invalid file descriptor, open again the pipe
				::close(fd);
				fd = ::open(eventMatrix_pipe, O_RDONLY | O_NONBLOCK );
			case EINTR  : // ::read interrupted before / during reading
			default:
				snprintf(strErr_no,sizeof(strErr_no),"acs_prcmand %s - %s - ::read from eventMatrix_pipe fails, errno = %d (%s)", PRCBIN_REVISION, __PRETTY_FUNCTION__, error, strerror(error));
				Logging->Write(strErr_no, LOG_LEVEL_ERROR);
				break;
			}

			usleep(300000); // sleep 0.3 seconds

		}
	}

	::close(fd);

    return error;
}
