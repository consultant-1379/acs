/*
 * ACS_PRC_prcispThread.cpp
 *
 *  Created on: Nov 22, 2010
 *      Author: xpaomaz
 */

#include "ACS_PRC_prcispThread.h"
#include "acs_apgcc_paramhandling.h"
#include "acs_apgcc_omhandler.h"
#include <fstream>

void other_node_restart_logrotd( long long logSize ) {

	vector<std::string> pDataList;
	vector<std::string>::iterator it;

	char long_to_string[16] = {0};
    snprintf(long_to_string,16,"%lld",logSize);

    fstream inFile;
    inFile.open("/etc/logrot.d/default",ios::in);

    std::string tmp;

    char buffer[256] = {0};

    while ( inFile.good() ){

    	inFile.getline(buffer,256);

    	tmp = std::string ( buffer );

    	if ( tmp.find("prcisp_syslog.log") != string::npos ){
    		tmp = "/cluster/etc/ap/acs/prc/prcisp_syslog.log " + std::string(long_to_string) + " 3";
    		//tmp = "/cluster/etc/ap/acs/prc/prcisp_syslog.log 3 " + std::string(long_to_string);
    	}

    	pDataList.push_back( tmp );
    	std::cout<<tmp<<std::endl;
    }

    inFile.close();

    fstream outFile;
    outFile.open("/etc/logrot.d/default",ios::out);

    for ( it = pDataList.begin(); it != pDataList.end(); it++ ){
    	outFile << (*it) << std::endl;
    }

    outFile.close();

	int status;
	string cmd = "service logrotd restart &>/dev/null";

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

ACS_PRC_prcispThread::ACS_PRC_prcispThread() {
	// TODO Auto-generated constructor stub

}

ACS_PRC_prcispThread::~ACS_PRC_prcispThread() {
	// TODO Auto-generated destructor stub
}

int ACS_PRC_prcispThread:: svc ( void ){

	ifstream ifs;
	string this_hostname;
	long long maxlogSize = 0;

	ifs.open("/etc/cluster/nodes/this/hostname");

	if ( ifs.good())
		getline(ifs,this_hostname);

	ifs.close();

	acs_apgcc_paramhandling ispLogSize;

	ACE_INET_Addr sockAddress(8882,this_hostname.c_str());
	ACE_SOCK_Acceptor sockAcceptor;
	ACE_SOCK_Stream sockStream;
	char buffer[128];

	if (sockAcceptor.open (sockAddress) == -1){
		sockAcceptor.close();
		return -1;
	}

	while (1){

		if ( sockAcceptor.accept(sockStream,0,0,0) == -1 ){
			sockAcceptor.close();
			return -1;
		}
		else {
			sockStream.recv(buffer,128);
			if ( strcmp(buffer,"UPDATE_LOG_SIZE") == 0) { // Other node has been rebooted

			    OmHandler prc_OmHandler;
			    vector<string> tmp_vector;

			    prc_OmHandler.Init();

			    prc_OmHandler.getClassInstances("ProcessControl",tmp_vector);

			    prc_OmHandler.Finalize();

			    sleep(1);
				ispLogSize.getParameter("ispConfigId=1,"+tmp_vector[0], "maxLogSize", &maxlogSize);

				std::cout<<"UPDATE_LOG_SIZE"<<std::endl;
				other_node_restart_logrotd ( maxlogSize / 2);

			}
		}

	}/*end while 1*/

	sockStream.close();
	sockAcceptor.close();
	return 0;

}/*end svc*/
