
/*=================================================================== */
   /**

   @file acs_asec_dsdServer.cpp

   Class method implementation.

   This module contains the implementation of class declared in
	acs_asec_dsdserver.h

   @version N.N.N

   HISTORY

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       30/03/2015    XFURULL   AXE IO, CBC cipher handling in csadm implementation
   N/A       25/02/2011     NS       Initial Release
   ================================================================== */

#include "acs_asec_dsdServer.h"
#include "acs_asec_ObjectImplementer.h"
#include "acs_asec_global.h"

const int DSD_SHUTDOWN=112;
/*===================================================================
   ROUTINE: ACS_ASEC_DSDSrv constructor
=================================================================== */


ACS_ASEC_DSDSrv::ACS_ASEC_DSDSrv():
dsdServObj(0) 
{
	m_global_instance       = ACS_ASEC_Global::instance();
	shutDownDSD=0;
	log.Open("ASECBIN");	
	Session_server = 0;
}

/*===================================================================
   ROUTINE: StopDSDServer
=================================================================== */
bool ACS_ASEC_DSDSrv::StopDSDServer()
{
	log.Write("ACS_ASEC_DSDSrv:StopDSDServer() - Enter");
	if (dsdServObj) {
		int res = dsdServObj->close();
		char buff[180]={'\0'} ;
		snprintf(buff, 180, "dsdServObj->close() res : %d", res);
		log.Write(buff, LOG_LEVEL_INFO);
		delete dsdServObj;
		dsdServObj = 0;
	}
	return true;
}

//------------------------------------------------------------------
int ACS_ASEC_DSDSrv::open(){

	if (this->activate(THR_NEW_LWP|THR_JOINABLE) < 0) {
		log.Write("ACS_ASEC_DSDSrv:open() - Failed to start main svc thread.", LOG_LEVEL_ERROR);
		return -1;
	}
	return 0;
}


/*===================================================================
   ROUTINE: startAndConnectDsdServer
=================================================================== */

int ACS_ASEC_DSDSrv::svc() {

	bool done=false;
	bool publish_done=false;
	bool accept_done=false;
	int res=0;

	// Create DSD Server Object.
        ACE_NEW_NORETURN(dsdServObj, ACS_DSD_Server(acs_dsd::SERVICE_MODE_INET_SOCKET));
        if (dsdServObj == NULL) {
                syslog(LOG_ERR,"ACS_ASEC_DSDSrv:srv() - dsdServObj Object creation Failed");
                log.Write("ACS_ASEC_DSDSrv:srv() - dsdServObj Object creation Failed",LOG_LEVEL_INFO);
		return -1;
	}

	log.Write("ACS_ASEC_DSDSrv::svc() - Thread is running now", LOG_LEVEL_INFO);
	// Publish DSD Server
	publish_done=publish();

	//ACE_Time_Value timeout;
        ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeout;
        timeout = this->gettimeofday ();
        timeout += ACE_Time_Value (0,50000);
	//timeout.set(RELATIVETIME(0), 50000);
	ACE_Message_Block* mb=0;

	while (!done){

		res = this->getq(mb, &timeout);
		if (res == 0){ 
		//Checked received message
			switch( mb->msg_type() ){

				case DSD_SHUTDOWN: {
					log.Write("ACS_ASEC_DSDSrv:svc() - DSD-SHUTDOWN Received");
					mb->release();
					mb=0;
					done=true;
					break;
				}

				default: {
					mb->release();
					mb=0;
					syslog(LOG_ERR, "ACS_ASEC_DSDSrv:svc() - [%d] Unknown message received:", mb->msg_type());
					log.Write("ACS_ASEC_DSDSrv:svc() - Unknown message received", LOG_LEVEL_ERROR);
					break;
				}
			}
		}

		if (done){
			break; // DSD_SHUTDOWN recieved
		}
		
		// set next iteration timeout
		//timeout.sec(RELATIVETIME(60));
	
		if (!publish_done){
			publish_done=publish();
		}		

		if (publish_done){
			accept_done=accept();
		}
		else {
			//timeout.sec(RELATIVETIME(5));
        		timeout += ACE_Time_Value (5,0);
		}	

		if (!accept_done && publish_done){
			dsdServObj->close();
			publish_done=false;
			//timeout.sec(RELATIVETIME(5));
        		timeout += ACE_Time_Value (5,0);
		}

	} // enod of while
	
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------------
bool ACS_ASEC_DSDSrv::publish(){

	// Open DSD Server
	int result = dsdServObj->open();
	if (result < 0) {
		log.Write("ACS_ASEC_DSDSrv:publish() - open-failed", LOG_LEVEL_ERROR);
		string err_txt="";
		err_txt=dsdServObj->last_error_text();
		const int MAXBUFF=300;
		char buff[MAXBUFF];
		snprintf(buff, MAXBUFF-1, "Server open last error: %s", err_txt.c_str());
		log.Write(buff, LOG_LEVEL_INFO);
		return false;
	}
	log.Write("ACS_ASEC_DSDSrv:publish() - DSD Servier open-successful");

	// Publish DSD Server now
	ACE_INT32 retCode;
	string serviceName = "";
	retCode=GetNodeId();
	if (retCode < 0) {
		log.Write("ACS_ASEC_DSDSrv:publish() - error finding node-id");
		return false;
	}
	else if (retCode == 1){
		serviceName = "CSADM_A";
	}
	else if (retCode == 2){
		serviceName = "CSADM_B";
	}
	
	retCode = dsdServObj->publish(serviceName, "CSADM", acs_dsd::SERVICE_VISIBILITY_GLOBAL_SCOPE);
	if(retCode < 0) {
		log.Write("ACS_ASEC_DSDSrv:publish() - publish-failed", LOG_LEVEL_ERROR);	
		dsdServObj->close();
		return false;
	}
	log.Write("ACS_ASEC_DSDSrv:publish() - DSD Servier publish-successful");
	
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
bool ACS_ASEC_DSDSrv::accept(){

	bool result=false;
	// Create Session Server Object
	Session_server = new (std::nothrow) ACS_DSD_Session();
	if (!Session_server){
		log.Write("ACS_ASEC_DSDSrv:accept() - Session_server Memory allocation Failed", LOG_LEVEL_ERROR);
		syslog(LOG_INFO, "ACS_ASEC_DSDSrv:accept() : Session_server Memory allocation Failed");
		return result;
	}
	
	// dsd server accept. This is timeout call
	int res = dsdServObj->accept(*Session_server, 2000);
	/* handle the timeout case first*/
	if (res == 18) { //18=TIMOUT EXPIRES ON ACCEPT
		result=true;
		Session_server->close();
	        delete Session_server;
        	Session_server = 0;
		return result;
	}

	if(res >= 0) {
		log.Write("ACS_ASEC_DSDSrv:accept() - accept success", LOG_LEVEL_INFO);
		char buffer[100];
		ACE_OS::memset(buffer, 0, sizeof(buffer));
		int nBytes = Session_server->recv(buffer, sizeof(buffer));
		if(nBytes  >= 0) {
			if(strcmp(buffer, "RESTART") == 0) {
				//string cmdToLaunch1("cluster config -r");
				const char * mode = "r";
				//int rCode=launchCmd(cmdToLaunch1,mode);
				//if (rCode == 0)
				//	log.Write ("ACS_ASEC_DSDSrv:accept() - cluster config re-load success", LOG_LEVEL_INFO);
				//else
				//	log.Write ("ACS_ASEC_DSDSrv:accept() - cluster config re-load failed", LOG_LEVEL_ERROR);
				// cluster config reload on passive node causing problems in ssh configuration files
				// clu_mgmt script taking care to reload cluster on both nodes  
				string cmdToLaunch2("/opt/ap/apos/bin/servicemgmt/servicemgmt restart lde-iptables.service");
				int rCode=launchCmd(cmdToLaunch2, mode);
				if (rCode == 0)
					log.Write ("ACS_ASEC_DSDSrv:accept() - iptable service restart success", LOG_LEVEL_INFO);
				else
					log.Write ("ACS_ASEC_DSDSrv:accept() - iptable service restart failed", LOG_LEVEL_ERROR);
		
				char buffer1[] = "DONE";
				int send_output = Session_server->send(buffer1,10);
				log.Write("ACS_ASEC_DSDSrv:accept() - response message sent to other node", LOG_LEVEL_INFO);	
				char buff[100];
				sprintf(buff, "Return Code:[%d]", send_output);
				log.Write(buff,LOG_LEVEL_INFO);
				result=true;
			}
		}
		else {
			log.Write("ACS_ASEC_DSDSrv:accept() - no-data received", LOG_LEVEL_ERROR);
			// might be the result of operation 4 (csadm -s)
			result=true;
		}
	}
	else {
		log.Write("ACS_ASEC_DSDSrv:accept() - accept() failed", LOG_LEVEL_ERROR);
		result=false;
	}

	Session_server->close();
	delete Session_server;
	Session_server = 0;

	return result;
}

//------------------------------------------------------------------------------------------------------------------------------
int ACS_ASEC_DSDSrv::launchCmd(string cmd, const char *mode) {

	FILE *fp;
	char line[LINE_SIZE];
	int rCode;
		
	fp = popen(cmd.c_str(),mode);
	if (fp == NULL){
		log.Write("ACS_ASEC_DSDSrv:launchCmd() - error launching popen()", LOG_LEVEL_ERROR);
		return -1;
	}

	while (fgets(line,LINE_SIZE,fp) != NULL) {
	}

	int status = pclose(fp);
	if (WIFEXITED(status) ){
		rCode = WEXITSTATUS(status);
		if ((rCode == 0) || (rCode == 2))
			return 0;
	}

	return 1;
}

//------------------------------------------------------------------------------------------------------------------------------
ACS_ASEC_DSDSrv::~ACS_ASEC_DSDSrv()
{
   // if (dsdServObj){
   //	delete dsdServObj;
   //    	dsdServObj = 0;
   // }

    log.Close();
}

//------------------------------------------------------------------------------------------------------------------------------
int ACS_ASEC_DSDSrv::GetNodeId()
{
	FILE* fp;
	ACE_TCHAR buff[10];
	ACE_TCHAR buffer[100];
	ACE_INT32 node_id=0;

	fp = ACE_OS::fopen(NODE_ID_FILE,"r");

	if ( fp == NULL ) {
		syslog(LOG_ERR,"RDE_Agent: Error! fopen FAILED");
		log.Write("RDE_Agent: Error! fopen FAILED",LOG_LEVEL_ERROR);
		return -1;
	}

	if (fscanf(fp ,"%s" ,buff) != 1 ) {
		(void)fclose(fp);
		syslog(LOG_ERR ,"Unable to Retreive the node id from file [ %s ]" ,NODE_ID_FILE);
		sprintf(buffer,"Unable to Retreive the node id from file [ %s ]" ,NODE_ID_FILE);
		log.Write(buffer,LOG_LEVEL_ERROR);
		return -1;
	}

	if (ACE_OS::fclose(fp) != 0 ) {
		syslog(LOG_ERR ,"Error! fclose FAILED");
		log.Write("Error! fclose FAILED",LOG_LEVEL_ERROR);
		return -1;
	}

	node_id= ACE_OS::atoi(buff);
	return node_id;
}

//----------------------------------------------------------------------
int ACS_ASEC_DSDSrv::close(){

	ACE_Message_Block* mb=0;
        log.Write("ACS_ASEC_DSDSrv:stop() invoked", LOG_LEVEL_INFO);

        ACE_NEW_NORETURN(mb, ACE_Message_Block());
        if (mb == 0){
                log.Write("Failed create message for dsd-shutdown", LOG_LEVEL_ERROR);
        }
        else {
                mb->msg_type(DSD_SHUTDOWN);

                if (this->putq(mb) < 0){
                        mb->release();
                        mb=0;
                        log.Write("Failed to post msg DSD_SHUTDOWN", LOG_LEVEL_ERROR);
                }else{
                        log.Write("DSD_SHUTDOWN Ordered Internally", LOG_LEVEL_ERROR);
                }
        }

	return 0;
}

//----------------------------------------------------------------------
int ACS_ASEC_DSDSrv::close(u_long) {

	this->StopDSDServer();
	log.Write("ACS_ASEC_DSDSrv:close(u_long) - Thread terminated", LOG_LEVEL_INFO);
	return 0;
}
//----------------------------------------------------------------------


