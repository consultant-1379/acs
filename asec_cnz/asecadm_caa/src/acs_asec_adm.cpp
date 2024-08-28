   /**

   @file acs_asec_adm.cpp

   Class method implementation.

   This module contains the implementation of class declared in
        acs_asec_adm.h,acs_asec_dsdserver.h,acs_asec_csadmOIHandler.h module

   @version N.N.N

   HISTORY

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       25/02/2011     NS       Initial Release
   ================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <ace/Reactor.h>
#include "acs_asec_adm.h"
#include "acs_asec_global.h"
#include "acs_asec_reactorrunner.h"
#include "acs_asec_ObjectImplementer.h"
#include "acs_asec_dsdServer.h"
#include "acs_asec_hamanager.h"
#include "acs_asec_immhandler.h"
#include "acs_asec_oi_gsnhConnection.h"
#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"

const int SHUTDOWN=111;
//-------------------------------------------------------------------
ACS_ASEC_Adm::ACS_ASEC_Adm():
	m_haObj(0),
	m_reactorRunner(0),
	l_active(0),
	debugOn(0)
{
	log.Open("ASECBIN");

}

//-------------------------------------------------------------------
ACS_ASEC_Adm::~ACS_ASEC_Adm()
{
	log.Close();

}

//-------------------------------------------------------------------
int ACS_ASEC_Adm::active(asecHAClass* haObj ){

	if (0 == haObj) {
		log.Write("asec-adm-class:active() NULL haObj Found",LOG_LEVEL_ERROR);
		return -1;
	}

	m_haObj=haObj;
	log.Write("asec-adm-class:active() start invoked",LOG_LEVEL_ERROR);
	return this->start(0,0);
}	
//-------------------------------------------------------------------
int ACS_ASEC_Adm::start(int argc, char* argv[]) {

	ACE_UNUSED_ARG(argv);
	if (argc != 0 ){
		// debug mode
		l_active=0; // standby
		debugOn=1;
		if (argc == 1) {
			l_active=1;
		}	
	}
        //set ikev2 for ipv6 if node is virtual
        if(isVirtual()){
                applyIkev2ForIpv6();
        }

	// update sec certgroup
	applyCertGrouptoSec();

	int status = this->sig_shutdown_.register_handler(SIGINT, this);
	if (status < 0) {
		log.Write("asec-adm-class:start() register_handler(SIGINT,this) failed..",LOG_LEVEL_ERROR);
		return -1;
	}

	status = this->sig_shutdown_.register_handler(SIGTERM,this);
	if (status < 0) {
		log.Write("asec-adm-class:start() register_handler(SIGTERM,this) failed..",LOG_LEVEL_ERROR);
		return -1;
	}	

	ACE_NEW_NORETURN(m_reactorRunner, ACS_ASEC_ReactorRunner(ACS_ASEC_Global::instance()->reactor(), "csadm-main"));
	if (0 == m_reactorRunner) {
		log.Write("asec-adm-class:start() Failed to create ACS_ASEC_ReactorRunner",LOG_LEVEL_ERROR);
		return -1;
	}

	int res = m_reactorRunner->open();
	if (res < 0) {
		syslog(LOG_ERR, "asec-adm-class:start() - Failed to open ACS_ASEC_ReactorRunner");
		return -1;
	}
	
	return this->activate( THR_JOINABLE | THR_NEW_LWP );
}

//-------------------------------------------------------------------
int ACS_ASEC_Adm::passive(asecHAClass* haObj){
	
	if (0 == haObj) {
		log.Write("asec-adm-class:passive() NULL haObj Found",LOG_LEVEL_ERROR);
		return -1;
	}

	m_haObj=haObj;
	log.Write("asec-adm-class:passive() start invoked", LOG_LEVEL_INFO);
	return this->start(0,0);
}


//-------------------------------------------------------------------
void ACS_ASEC_Adm::stop() {

	ACE_Message_Block* mb=0;
	log.Write("asec-adm-class:stop() invoked", LOG_LEVEL_INFO);

	ACE_NEW_NORETURN(mb, ACE_Message_Block());
	if (mb == 0){
		log.Write("Failed create message for shutdown", LOG_LEVEL_ERROR);
	} 
	else {
		mb->msg_type(SHUTDOWN);

		if (this->putq(mb) < 0){
			mb->release();
			mb=0;
			log.Write("Failed to post msg SHUTDOWN", LOG_LEVEL_ERROR);
		}else{
			log.Write("SHUTDOWN Ordered Internally", LOG_LEVEL_ERROR);
		}
	}
}

//-------------------------------------------------------------------
int ACS_ASEC_Adm::handle_close(ACE_HANDLE, ACE_Reactor_Mask /*mask*/) {

	log.Write("asec-adm-class:handle_close() invoked", LOG_LEVEL_INFO);
	
	ACS_ASEC_Global::instance()->reactor()->remove_handler(this, ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL);
	return 0;
}

//-------------------------------------------------------------------
int ACS_ASEC_Adm::handle_signal(int signum, siginfo_t*, ucontext_t *) {

	switch (signum) {
		case SIGTERM:
			log.Write("asec-adm-class:handle_signal() - signal SIGTERM caught...", LOG_LEVEL_INFO);
			break;

		case SIGINT:
			log.Write("asec-adm-class:handle_signal() - signal SIGINT caught...", LOG_LEVEL_INFO);
			break;

		default:
			log.Write("asec-adm-class:handle_signal() - other sinal caught..", LOG_LEVEL_INFO);
			break;
	}		
	this->stop();
	return 0;
}

//-------------------------------------------------------------------
int ACS_ASEC_Adm::close(u_long /* flags */){

	int status = this->sig_shutdown_.remove_handler(SIGINT);
	if (status < 0) {
		log.Write("acs-asec-class:close() - remove_handler(SIGINT) failed.",LOG_LEVEL_ERROR);
	}	

	status = this->sig_shutdown_.remove_handler(SIGTERM);
	if (status < 0) {
		log.Write("acs-asec-class:close() - remove_handler(SIGTERM) failed.",LOG_LEVEL_ERROR);
	}	

	// Set flag
	ACS_ASEC_Global::instance()->shutdown_ordered(true);

	if (l_active){	
		// Close OI Implementer Instance
		(void)ACS_ASEC_Global::instance()->asecImpl()->close();
		log.Write("acs-asec-class:close() waiting for ACS_ASEC_ObjImpl Instance to close",LOG_LEVEL_INFO);
		(void)ACS_ASEC_Global::instance()->asecImpl()->wait();
		log.Write("acs-asec-class:close() ACS_ASEC_ObjImpl Instance now closed",LOG_LEVEL_INFO);

		(void)ACS_ASEC_Global::instance()->immHandler()->close();


	}
	else {
		// Close DSD Server Instance
		if (!m_haObj->active ) {
			if (!m_haObj->passiveToActive )
				ACS_ASEC_Global::instance()->asecDsd()->shutDownDSD=1;

			(void)ACS_ASEC_Global::instance()->asecDsd()->close();
			log.Write("acs-asec-class:close() waiting for ACS_ASEC_DSDSrv Instance to close",LOG_LEVEL_INFO);
			(void)ACS_ASEC_Global::instance()->asecDsd()->wait();
			log.Write("acs-asec-class:close() ACS_ASEC_DSDSrv Instance now closed",LOG_LEVEL_INFO);
                }
	}

	// Shutdown global instances
	ACS_ASEC_Global::instance()->deactivate();

	// STOP Reactor
	if (m_reactorRunner != 0) {
		m_reactorRunner->stop();
		m_reactorRunner->wait();
		delete m_reactorRunner;
		m_reactorRunner = 0;
	}

	return 0;
}

//-------------------------------------------------------------------
int ACS_ASEC_Adm::svc() {

	bool errorDetected=false;
	bool done=false;

	const int MAX_IMM_RETRY = 3;
	//Retry mechanism
	int retryCount = 0;
	int res = 0;

	if (debugOn){
		if (l_active){
			if (ACS_ASEC_Global::instance()->asecImpl()->open(m_haObj) < 0){
				log.Write("acs-asec-class:svc() - Failed to create OI implementer Instance", LOG_LEVEL_ERROR);
				errorDetected=true;
			}
			do{
				res = ACS_ASEC_Global::instance()->immHandler()->open();
				if(res == 0){
					log.Write("asec-adm-class IMM_HANDER OPENED",LOG_LEVEL_ERROR);
					break;
				}
				else{
					++retryCount;
					sleep(1);
				}
			}while(MAX_IMM_RETRY > retryCount );
		}
		else{
			if (ACS_ASEC_Global::instance()->asecDsd()->open() < 0){
				log.Write("acs-asec-class:svc() - Failed to create DSD Server Instance", LOG_LEVEL_ERROR);
				errorDetected=true;
			}
		}
	}
	else{
		if (m_haObj->active){
			if (ACS_ASEC_Global::instance()->asecImpl()->open(m_haObj) < 0){
				log.Write("acs-asec-class:svc() - Failed to create OI implementer Instance", LOG_LEVEL_ERROR);
				errorDetected=true;
			}
			res = ACS_ASEC_Global::instance()->immHandler()->open();
			if(res < 0){
				log.Write("acs-asec-class:svc() - Failed to create Implementer instance",LOG_LEVEL_ERROR);
				errorDetected=true;
			}
			l_active=1;
		}
		else{
			log.Write("acs-asec-class:svc() - Entered Passive Functionality", LOG_LEVEL_INFO);
			if (ACS_ASEC_Global::instance()->asecDsd()->open() < 0){
				log.Write("acs-asec-class:svc() - Failed to create DSD Server Instance", LOG_LEVEL_ERROR);
				errorDetected=true;
			}
		}
	}	
	// before starting the thread, check if we have seen any big faults
	if (errorDetected) {
		if (m_haObj){
			// Report to AMF that we want to restart of ourselves
			ACS_APGCC_ReturnType result = m_haObj->componentReportError(ACS_APGCC_COMPONENT_RESTART);
			if (result == ACS_APGCC_SUCCESS) {
				return -1;
			}
		}
		exit (EXIT_FAILURE);
	}

	log.Write("acs-asec-class:svc() - Thread is running now", LOG_LEVEL_INFO);
	ACE_Message_Block* mb=0;
	while (!done){
		int res = this->getq(mb);
		syslog(LOG_INFO, "ACS_ASEC_Adm::svc() - return value of getq :  [%d] ",res);
		if (res < 0) {
			log.Write("acs-asec-class:svc() - getq timeout/error", LOG_LEVEL_ERROR);	
			break;
		}	

		//Checked received message
		switch( mb->msg_type() ){
			
			case SHUTDOWN: {
				log.Write("acs-asec-class:svc() - SHUTDOWN Received");
				mb->release();
				mb=0;
				done=true;
				break;
			}		
			
			default: {
	    			mb->release();
				mb=0;
				log.Write("acs_asec_class:svc() - Unknown message received:", LOG_LEVEL_ERROR);
				break;
			}				
		} // end of switch
		
	}// end of while
	return 0;
}

//-------------------------------------------------------------------
bool ACS_ASEC_Adm::executeCommand(const std::string& command, std::string& output){
	log.Write("ACS_ASEC_Adm::executeCommand start", LOG_LEVEL_INFO);
	bool result = false;
	FILE* pipe = popen(command.c_str(), "r");
	if(NULL != pipe){
		char rowOutput[1024]={'\0'};
		while(!feof(pipe)){
			// get the cmd output
			if(fgets(rowOutput, 1023, pipe) != NULL){
				std::size_t len = strlen(rowOutput);
				// remove the newline
				if( rowOutput[len-1] == '\n' ) rowOutput[len-1] = 0;
				output.append(rowOutput);
			}
		}
		// wait cmd termination
		int exitCode = pclose(pipe);
		// get the exit code from the exit status
		result = (WEXITSTATUS(exitCode) == 0);
	}
	log.Write("ACS_ASEC_Adm::executeCommand end",LOG_LEVEL_INFO);
	syslog(LOG_INFO, "ACS_ASEC_Adm::executeCommand result:%s\n, command : %s\n, command output:%s\n", (result ? "TRUE" : "FALSE"), command.c_str(), output.c_str());
	return result;
}
//-------------------------------------------------------------------
void ACS_ASEC_Adm::applyCertGrouptoSec(){
	fstream file;
	string line, commandOutput;
	string group = "CERTGRP";
	string sec = "sec-cert";
	bool append = false;
	file.open("/etc/group");
	string appendCmd = "sed -i '/CERTGRP/ s/$/,sec-cert/' /etc/group";

	if(file.is_open()){
		for(unsigned int curLine = 0; getline(file, line); curLine++){
			if (line.find(group.c_str()) != string::npos) {
				if(line.find(sec.c_str()) == string::npos) {
					append = true;
					break;
				}
			}
		}
	}
	file.close();
	if(append == true){
		executeCommand(appendCmd,commandOutput);
		restartSecCertmToApplyCertGrp();
	}
}
//-------------------------------------------------------------------
void ACS_ASEC_Adm::restartSecCertmToApplyCertGrp(){
       std::string cmdToRestartSecCertmNode1,cmdToRestartSecCertmNode2;
       string commandOutput;
       cmdToRestartSecCertmNode1.assign("cmw-utility amfadm restart safComp=Sec-CertM-Oi,safSu=SC-1,safSg=2N,safApp=ERIC-apg.nbi.aggregation.service");
       cmdToRestartSecCertmNode2.assign("cmw-utility amfadm restart safComp=Sec-CertM-Oi,safSu=SC-2,safSg=2N,safApp=ERIC-apg.nbi.aggregation.service");
       executeCommand(cmdToRestartSecCertmNode1,commandOutput);
       executeCommand(cmdToRestartSecCertmNode2,commandOutput);
}
//-------------------------------------------------------------------
bool ACS_ASEC_Adm::isVirtual()
{
        ACS_CS_API_CommonBasedArchitecture::ArchitectureValue nodeArchitecture;
        if(ACS_CS_API_NS::Result_Success == ACS_CS_API_NetworkElement::getNodeArchitecture(nodeArchitecture))
        {
                if(ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED == nodeArchitecture)
                        return true;
        }
        return false;
}
//----------------------------------------------------------------------

void ACS_ASEC_Adm::applyIkev2ForIpv6(){
        string ipVersionStr,ikeVersionStr;
        ifstream ipverStream("/storage/system/config/apos/apg_protocol_version_type", ios::in);
        ifstream ikeverStream("/cluster/etc/ikeversion", ios::in);
        if(ipverStream.is_open()){
                while (ipverStream >> ipVersionStr);
                ipverStream.close();
        }
        else
                syslog(LOG_INFO, "asec-adm-class: /storage/system/config/apos/apg_protocol_version_type file not found or empty");

        if(ikeverStream.is_open()){
                while (ikeverStream >> ikeVersionStr);
                ikeverStream.close();
        }
        else
                syslog(LOG_INFO, "asec-adm-class: /cluster/etc/ikeversion file not found or empty");

        if(!strcmp(ipVersionStr.c_str(),"6") && (!strcmp(ikeVersionStr.c_str(),"IKEV1")))
        {
                syslog(LOG_INFO, "asec-adm-class: ipversion is 6 and ikeversion is IKEV1");
                int result = -1;
                string removeRacconFilesCmd;
                removeRacconFilesCmd = "/usr/bin/rm -r /etc/racoon/*";
                result = system(removeRacconFilesCmd.c_str());
                if(0 != result){
                         syslog(LOG_INFO, "asec-adm-class: remove racoon files failed");
                }
                else
                        syslog(LOG_INFO, "asec-adm-class: removed racoon files success");
                writeDefaultIkev1Config();
                updateIkev2Version();
        }
        else
        syslog(LOG_INFO, "asec-adm-class: either ipversion is not 6 or ike version is not 1");

}
//-------------------------------------------------------------------
void ACS_ASEC_Adm::writeDefaultIkev1Config(){
        string line;
        ifstream readPskOrigFile ("/opt/ap/acs/conf/psk.txt.original",ios::in);
        ofstream writePskFile("/etc/racoon/psk.txt",ios::out|ios::trunc);
        ifstream readSetkeyConfOrigFile("/opt/ap/acs/conf/setkey.conf.original",ios::in);
        ofstream writeSetkeyConfFile("/etc/racoon/setkey.conf",ios::out|ios::trunc);
        ifstream readRacoonConfOrigFile ("/opt/ap/acs/conf/racoon.conf.original",ios::in);
        ofstream writeRacoonConfFile ("/etc/racoon/racoon.conf",ios::out|ios::trunc);

        if (writePskFile.is_open())
        {
                if (readPskOrigFile.is_open())
                {
                        while ( getline (readPskOrigFile,line) )
                        {
                                writePskFile << line << '\n';
                        }
                        syslog(LOG_INFO, "asec-adm-class:write default psk success");
                        readPskOrigFile.close();
                }
                else
                        syslog(LOG_INFO, "asec-adm-class:Unable to open read psk original file");
                 writePskFile.close();
        }
         else
                syslog(LOG_INFO, "asec-adm-class:Unable to open write psk file");

        if (writeSetkeyConfFile.is_open())
        {
                if (readSetkeyConfOrigFile.is_open())
                {
                        while ( getline (readSetkeyConfOrigFile,line) )
                        {
                                writeSetkeyConfFile << line << '\n';
                        }
                        syslog(LOG_INFO, "asec-adm-class:write default setkey conf success");
                        readSetkeyConfOrigFile.close();
                }
                else
                        syslog(LOG_INFO, "asec-adm-class:Unable to open read setkey conf original file");
                 writeSetkeyConfFile.close();
        }
         else
                syslog(LOG_INFO, "asec-adm-class:Unable to open write setkey conf file");

        if (writeRacoonConfFile.is_open())
        {
                if (readRacoonConfOrigFile.is_open())
                {
                        while ( getline (readRacoonConfOrigFile,line) )
                        {
                                writeRacoonConfFile << line << '\n';
                        }
                        syslog(LOG_INFO, "asec-adm-class:write default racoon conf success");
                        readRacoonConfOrigFile.close();
                }
                else
                        syslog(LOG_INFO, "asec-adm-class:Unable to open read racoon conf original file");
                 writeRacoonConfFile.close();
        }
        else
                syslog(LOG_INFO, "asec-adm-class:Unable to open write racoon conf file");
}
//--------------------------------------------------------------------
void ACS_ASEC_Adm::updateIkev2Version(){
        ofstream ikev2Stream1("/cluster/etc/ikeversion",ios::out|ios::trunc);
        ofstream ikev2Stream2("/storage/system/config/asec/strongswan/ikeversion",ios::out|ios::trunc);
        if(ikev2Stream1.is_open()){
                ikev2Stream1 << "IKEV2" << "\n";
                ikev2Stream1.close();
        }
        else
                syslog(LOG_INFO, "asec-adm-class: write IKEV2 to cluster failed! ");
        if(ikev2Stream2.is_open()){
                ikev2Stream2 << "IKEV2" << "\n";
                ikev2Stream2.close();
        }
        else
                syslog(LOG_INFO, "asec-adm-class: write IKEV2 to storage failed! ");
}
//--------------------------------------------------------------------



