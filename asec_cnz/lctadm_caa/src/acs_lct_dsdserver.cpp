//******************************************************************************
//
// NAME
//      ACS_LCT_dsdserver.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2003.
// All rights reserved.
//
// The Copyright to the computer program(s) herein is the property
// of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with the written
// permission from Ericsson AB
// or in accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have been supplied.
//
// DESCRIPTION
//     Class method implementation for Object Implementor for Hardening service            
//              
//
// DOCUMENT NO
//      ----
//
// AUTHOR
//       XSATDEE
//
// CHANGES
//
//      REV NO          DATE            NAME            DESCRIPTION
//			2014-06-06      XCSRAKI         Rebase from APG43L 
//							welcomemessage system 
//							improvement(OP#345) on all ports
//
//****************************************************************************** 

/*=====================================================================
                INCLUDE DECLARATION SECTION
=====================================================================*/
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Task.h>
#include <ace/INET_Addr.h>
#include <acs_lct_dsdserver.h>
#include <acs_lct_server.h>
#include <ACS_APGCC_Util.H>
#include <acs_prc_api.h>
#include <ACS_DSD_Communicator.h>
#include <ACS_DSD_Session.h>
#include <acs_lct_tra.h>
#include <acs_apgcc_oihandler_V3.h>


#define LCTSERVICE
using namespace std;

ACS_LCT_dsdServer * dsdObj = new ACS_LCT_dsdServer();

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*=====================================================================
   ROUTINE: instantiate()
  =====================================================================*/

bool ACS_LCT_dsdServer::instantiate()
{
	DEBUG("%s","Inside instantiate method");
	dsdServObj = new (std::nothrow) ACS_DSD_Server(acs_dsd::SERVICE_MODE_INET_SOCKET);
        // to surpress the warnings!
        (void)CONFIG_IMM_CLASS_NAME;
        (void)CONFIG_IMM_CLASS_NAME1;
        (void)CONFIG_IMM_CLASS_NAME2;

	return true;
}

/*===================================================================
   ROUTINE: ACS_LCT_dsdServer constructor
=================================================================== */

ACS_LCT_dsdServer::ACS_LCT_dsdServer() 
:dsdServObj(0),dsdCliObj(0),Session_client(0), Session_server(0){

//	(void)CONFIG_IMM_CLASS_NAME;
//	(void)CONFIG_IMM_CLASS_NAME1;
} 

/*===================================================================
   ROUTINE: ACS_LCT_dsdServer destructor
=================================================================== */

ACS_LCT_dsdServer::~ACS_LCT_dsdServer()
{
	DEBUG("%s","acs_lct_dsdServer class destructor");
}

void ACS_LCT_dsdServer::ShutdownDSD() {
	
	if (dsdServObj)   // DSD Server object
	{
		DEBUG("%s", "ShutdownDSD..");
		DEBUG("%s","before closing dsdservobj");
		dsdServObj->close();
	//	dsdServObj->unregister();
		DEBUG("%s","before deleting dsdservobj");
		delete dsdServObj;
		dsdServObj = 0;
	}
} 

/*=====================================================================
   ROUTINE: closeDSDClient
  =====================================================================*/
void ACS_LCT_dsdServer::closeDSDClient()
{
	DEBUG("%s","Inside function closeDSDClient");

	if (Session_client)   // DSD client session object
	{
		Session_client->close();
		DEBUG("%s","acs_lct_dsdserver: DSD Client session object closed");
		delete Session_client;
		Session_client=0;
		DEBUG("%s","acs_lct_dsdserver: DSD Client Session object deleted");
	}

	if (dsdCliObj) //DSD Client object
	{
		delete dsdCliObj;
		DEBUG("%s","acs_lct_dsdserver: DSD Client object deleted");
		dsdCliObj = 0;
	}

	DEBUG("%s","acs_lct_dsdserver: End of closeDSDClient function");
}

/*=====================================================================
   ROUTINE: CheckActiveNode
 =====================================================================*/

int ACS_LCT_dsdServer::CheckActiveNode()
{
	ACS_PRC_API prcObj;
	return prcObj.askForNodeState();
}

/*=====================================================================
   ROUTINE: launchCommand to launch command in bash
  =====================================================================*/
int ACS_LCT_dsdServer::launchCommand(char *cmd_string)
{
	int retCode=0;
	
        DEBUG("%s","launchCommand called to launch command in bash");
	retCode=ACE_OS::system(cmd_string);
	if (retCode == -1){
         	DEBUG("%s","system call failed while executing command");
	}

	retCode=WEXITSTATUS(retCode);
	if (retCode != 0) {
         	DEBUG("%s","Command execution failed while modifying cluster config files");
	}
	return retCode;
}

/*=====================================================================
ROUTINE: modify_config_file for Service startup
  =====================================================================*/
bool ACS_LCT_dsdServer::modify_config_file(string fin_name, string fin_val)
{
	string cmd, opt, param, cmd_to_launch;
        cmd="hardening";
	bool isWelcomeMsg = false;

    	if (( strcmp(fin_name.c_str(),"maximumAccountAge") == 0 ))
    	{	
		opt="-s -e";
    	}
	if (( strcmp(fin_name.c_str(),"maximumPasswordAge") == 0 ))
        {
                opt="-x";
        }

    	if (( strcmp(fin_name.c_str(),"minimumPasswordAge") == 0 ))
    	{
		opt="-n";
    	}

	else if (( strcmp(fin_name.c_str(),"minimumPasswordLength") == 0 ))
    	{
		opt="-m";
    	}
	else if (( strcmp(fin_name.c_str(),"passwordHistorySize") == 0 ))
     	{
		opt="-h";
     	}
	else if (( strcmp(fin_name.c_str(),"lockoutBadCount") == 0 ))
     	{
		opt="-b";
     	}
	else if (( strcmp(fin_name.c_str(),"lockoutDuration") == 0 ))
     	{
		opt="-d";
     	}

	else if (( strcmp(fin_name.c_str(),"inactivityTimer") == 0 ))
     	{
		opt="-t";
     	}
	else if (( strcmp(fin_name.c_str(),"welcomeMessage") == 0 ))
     	{
		opt="-w";
		isWelcomeMsg = true;
     	}
     	param=fin_val;

     	string chkAudit=fin_name.substr(0,5);
     	if (( strcmp(chkAudit.c_str(),"audit") == 0 ))
     	{
		opt="-s -a"; // -s is to say that -a is called during service startup
		param=fin_name+":"+fin_val;
     	}	
	if(isWelcomeMsg){
		cmd_to_launch=cmd+" "+opt+" \""+param + "\"";   		
	}
	else {
		cmd_to_launch=cmd+" "+opt+" "+param;   		
	}
        char command[cmd_to_launch.length()];
        sprintf(command,"%s",cmd_to_launch.c_str());
	DEBUG("%s","Command executing in modify_config_file function startup at cluster level");
        int rCode=launchCommand(command);
        if (rCode==0)
        	return true;
	else
        	return false;
}

/*=====================================================================
   ROUTINE: modify_config_file 
  =====================================================================*/
bool ACS_LCT_dsdServer::modify_config_file(char* cmd, int &aErrCode, string &aErrMsg)
{
	string cmd1="hardening";
        DEBUG("%s","Locking config files on local node");
        string opt="-l";
        char command[cmd1.length()+opt.length()];
        sprintf(command, "%s %s" ,cmd1.c_str(),opt.c_str());
        launchCommand(command);
	
        DEBUG("%s","Modifying config files on local node");
	DEBUG("%s" "%s","Command to be launched: ", cmd);
	int rCode=launchCommand(cmd);	
        if (rCode != 0)
        {
        	DEBUG("%s","modify callback: Error while modifying configuration file");
		if (rCode == 3){
                	aErrCode=15;
                	aErrMsg="Unreasonable value: maximumAccountAge must be greater than MinimumPasswordAge";
			return false;
		}
		else if (rCode == 2){
                        aErrCode=16;
                        aErrMsg="Internal Error: Invalid character/value provided";
                        return false;
                }
		else if (rCode == 4){
                	aErrCode=4;
                	aErrMsg="Internal Error: Wrong/Missing configuration file";
			return false;
		}
		else{
			aErrCode=5;
			aErrMsg="Unable to connect to server";
			return false;
		}
	}
        else
        {
        	DEBUG("%s","modify_config_file: local configuration files successfully modified");
	}
	return true;
}

/*=====================================================================
   ROUTINE: FindNode
  =====================================================================*/

int ACS_LCT_dsdServer::FindNode()
{
	FILE* fp;
	ACE_TCHAR buff[10];
	ACE_INT32 node_id=0;
	DEBUG("%s","Inside Find Node function");
	fp = ACE_OS::fopen(NODE_ID_FILE,"r");
	if ( fp == NULL ) {
		DEBUG("%s","NODE ID file open failed");
		ERROR("%s","Internal Program Error. Exit Code = 26");
		return -1;
	}

	if (fscanf(fp ,"%50s" ,buff) != 1 ) {
		(void)fclose(fp);
		DEBUG("%s","Unable to Retreive the node id from file");
		ERROR("%s","Internal Program Error. Exit Code = 26");
		return -1;
        }

	if (ACE_OS::fclose(fp) != 0 ) {
		DEBUG("%s","NODE ID file close failed");
		ERROR("%s","Internal Program Error. Exit Code = 26");
		return -1;
        }

	node_id= ACE_OS::atoi(buff);
	return node_id;
} 

/*===================================================================
   ROUTINE: DSDServerfunc
=================================================================== */

bool ACS_LCT_dsdServer::DSDServerfunc()
{

	bool ret_val=true;
	string return_val="success";

	if (!dsdServObj)
	{
		DEBUG("%s","DSDServerfunc():DSD Server object is a NULL pointer");
		ERROR("%s","Internal Program Error. Exit code = 26");
		return false;
	}
	// Create Session_server Object
	Session_server = new (std::nothrow) ACS_DSD_Session();
	if (Session_server){
		//DEBUG("%s","Session_server Created");
		ret_val=true;
	}	
	else {
		DEBUG("%s","Session_server Creation Failed");
		return false;
	}	
	
	if (Session_server)
	{
		int res;
		int ret = 0;
		ACS_DSD_Node node;
		res = dsdServObj->accept(*Session_server, 2000); // wait for 2 sec on accept
		if (res == 18){
			//18=TIMOUT EXPIRES ON ACCEPT
			//Check if there is any SHUTDOWN message on Queue
			Session_server->close();
                	delete Session_server;
			return true;	
		}	

		if( ret < 0){
			ERROR("%s","error in get handles for heart beat child, returning.. ");
			ERROR("%s","Internal Program Error. Exit Code = 26");
			Session_server->close();
                	delete Session_server;
			return false;
		}	
		string err_txt1="";
		err_txt1=Session_server->last_error_text();
			
		DEBUG("%s : %s","Error text: ", err_txt1.c_str());
		DEBUG("%s","Client connection accepted by server");
		
		string cmd;
		int strlen=MAX_BYTES+20;
		char buffer[strlen];
		ACE_OS::memset(buffer, 0, sizeof(buffer));
		DEBUG("%s","Before recv ");
		int nBytes = Session_server->recv(buffer, sizeof(buffer));
		string err_txt="";
		err_txt=Session_server->last_error_text();
						
		DEBUG("%s : %s","Error text: ", err_txt.c_str());
		if (nBytes < 0)
		{
			DEBUG("%s","DSDServerfunc: No data received from client");
			ERROR("%s","Internal Program Error. Exit code = 26");
			Session_server->close();
                	delete Session_server;
			return false;
		}
		else
		{
			DEBUG("%s","dsdServerfunc: Server received data from client");
			char * message;
			message= new char[strlen];
			strncpy(message,buffer,strlen);
			string msg=string (message);
			bool ret_changes=true;
			ACE_INT32 aErrCode;	
			string aErrMsg;
			DEBUG("%s","dsdServerfunc: Modify config file function called");
			ret_changes = modify_config_file(message, aErrCode, aErrMsg);
				
			if (ret_changes)
			{
				DEBUG("%s","dsdServerfunc: Operation successful on passive node");
				return_val="success";
			}
			else
			{
				DEBUG("%s","dsdServerfunc: Operation failed on passive node");
				ERROR("%s","Internal error. File missing / File error. Exit Code = 1");
				ret_val=false;
				return_val="failed";
			}

			if (nBytes  >= 0)
			{
				DEBUG("%s","dsdServerfunc: Sending acknowledgement to client");
				strcpy(message,return_val.c_str());
				int nbytes= Session_server->send(message,150);

				if (nbytes >= 0)
				{
					DEBUG("%s","dsdServerfunc: Acknowledgment sent from server to client");
				}
				else
				{
					DEBUG("%s","dsdServerfunc: Acknowledgment send failed from server to client");
					ERROR("%s","Internal Program Error. Exit Code = 26");
					ret_val=false;
				}
			}
		}				
				
	}//end of if (Session_server)
	else
	{
               	DEBUG("%s","DSDServerfunc: Memory allocation failed while creating ACS_DSD_Session object");
               	ERROR("%s","Internal Program Error. Exit Code = 26");
               	ret_val = false;
        }

	Session_server->close();
       	delete Session_server;
	return ret_val;
}

/*===================================================================
   ROUTINE: StartDSDServer 
=================================================================== */

bool ACS_LCT_dsdServer::StartDSDServer() 
{
	ACE_INT32 retCode;
	bool ret_val = true;
    
	DEBUG("%s","StartDSDServer function");

    	if (!dsdServObj)
   	{
    		DEBUG("%s","DSD Server object is a NULL pointer");
		return false;
    	}
    	string serviceName = "";

	retCode=FindNode();
    	if (retCode == -1) {
    		DEBUG("%s","Error finding the node id");
    		return false;
	}
	else if (retCode == 1){
		serviceName = "LCT_A";
	}
    	else if (retCode == 2){
		serviceName = "LCT_B";
   	}
	else
	{	
		DEBUG("%s","Invalid node id received");
		return false;
	}

    	if (dsdServObj)
        {
    		int resConnect=0;
    		DEBUG("%s","Opening DSD Server");
    		resConnect =  dsdServObj->open();
    		if (resConnect >= 0)
    		{
    			DEBUG("%s","Server open success");
    		}
		else
		{
			string err_txt="";
			err_txt=dsdServObj->last_error_text();
			DEBUG("%s","Server still not open");
			return false;
		}
               	int result = dsdServObj->publish(serviceName, "APOS", acs_dsd::SERVICE_VISIBILITY_GLOBAL_SCOPE);
               	if (result >= 0)
               	{
              		DEBUG("%s","DSD server publish successful");
               	}
                else
               	{
                		DEBUG("%s","DSD server publish failed");
				dsdServObj->close();
				return false;
		}
        }
        else
        {
            	DEBUG("%s","Memory allocation failed while creating DSD Server object");
    		ret_val=false;
	}

    	return ret_val;
} 

/*===================================================================
   ROUTINE: DSDClientfunc
=================================================================== */

bool ACS_LCT_dsdServer::DSDClientfunc(string attribute, int &aErrCode, string &aErrMsg)
{
	string szBuff;
 	char * cResult;
	int str_length=MAX_BYTES+20;
	cResult=new char[str_length];
	bool ret_val = true;
	sprintf(cResult, "%s" ,attribute.c_str());

    DEBUG("%s","Inside DSDClientfunc function");
    if(Session_client)
    {
        DEBUG("%s","DSDClientfunc: Client sending data to server");
        int resSend = Session_client->send(cResult,str_length);

        if ( resSend >= 0)
        {
        	char* message= new char[200];
    		size_t maxBytes=150;
            	DEBUG("%s","DSDClientfunc: Hardening data sent from client to server");
	        int resend=Session_client->recv(message,maxBytes);
        	string ack_msg=(string) message;
            	if (resend > 0)
            	{
	                DEBUG("%s","DSDClientfunc: Client received acknowledgment from server");
        	        if (ack_msg.compare("failed")==0)
                	{
	                	ret_val=false;
        	        	ERROR("%s","Internal error on passive node");
                		ERROR("%s","Internal error. File missing / File error. Exit Code = 1");
				aErrMsg="Internal error : Missing/Wrong configuration file on passive node";
				aErrCode=4;
        	        }
                	else if (ack_msg.compare("success")==0)
			{ 	
				ret_val=true; 
			}
            	}
	        else
        	{
	                DEBUG("%s","DSDClientfunc: No acknowledgment received from server");
	                ERROR("%s","Internal Program Error. Exit Code = 26");
			aErrMsg="Error connecting to server";
			aErrCode=117;
        	       	ret_val = false;
            	}
		delete [] message;
        }
        else
        {
             DEBUG("%s","DSDClientfunc: No data sent from client to server");
             ERROR("%s","Internal Program Error. Exit Code = 26");
	     aErrMsg="Error connecting to server";
             aErrCode=117;
             string err_txt="";
             err_txt=Session_client->last_error_text();
             //syslog(LOG_INFO, "Client last error: %s", err_txt.c_str());
             ret_val=false;
        }
    } // session client
    else
    {
    	DEBUG("%s","DSDClientfunc: Client Session object does not exist");
    	ERROR("%s","Internal Program Error. Exit Code = 26");
        aErrMsg="Error connecting to server";
        aErrCode=117;
        ret_val = false;
	delete [] cResult;
    }
	return ret_val;
}

/*===================================================================
   ROUTINE: StartDSDClient
=================================================================== */

bool ACS_LCT_dsdServer::StartDSDClient()
	{

	bool ret_val = true;
	dsdCliObj = new (std::nothrow) ACS_DSD_Client();
	if (dsdCliObj)
	{
	    DEBUG("%s","StartDSDClient:DSD Client creation success");
	}
	else
	{
	   	DEBUG("%s","StartDSDClient: DSD Client creation failed");
	   	ERROR("%s","Internal Program Error. Exit Code = 26");
	   	ret_val=false;
	}
    Session_client = new (std::nothrow) ACS_DSD_Session();
    DEBUG("%s","StartDSDClient: Inside StartDSDClient function");

    if (dsdCliObj)
    {
    	if (Session_client)
    	{
	    int retCode;
    	    string serviceName;
            char buffer[100];
            ACE_OS::memset(buffer, 0, sizeof(buffer));
            retCode=ACS_LCT_dsdServer::FindNode();
            if (retCode == -1)
            {
            	DEBUG("%s","StartDSDClient: Error finding the node id");
            	ERROR("%s","Internal Program Error. Exit Code = 26");
                ret_val = false;
            }
            else if (retCode == 1)
            {
            	serviceName = "LCT_B";
            }
            else if (retCode == 2)
            {
            	serviceName = "LCT_A";
            }
            else
            {
                DEBUG("%s","StartDSDClient: Invalid node id received");
                ERROR("%s","Internal Program Error. Exit Code = 26");
                ret_val = false;
            }
            int resConnect=0;
            	flag=0;
                DEBUG("%s","StartDSDClient: Before client connect");
                resConnect =  dsdCliObj->connect(*Session_client, serviceName, "APOS", acs_dsd::SYSTEM_ID_PARTNER_NODE, acs_dsd::NODE_STATE_ACTIVE);
                DEBUG("%s","StartDSDClient: After client connect");
                if (resConnect == 0)
                {
		     DEBUG("%s","StartDSDClient: Client connected to server");
                }
                else
                {
                    DEBUG("%s","StartDSDClient: Client not yet connected to server");
                    string err_txt="";
                    err_txt=dsdCliObj->last_error_text();
                    ret_val = false;
                }
          }
    	else
    	{
            DEBUG("%s","Memory allocation failed while creating ACS_DSD_Session object");
            ERROR("%s","Internal Program Error. Exit Code = 26");
            ret_val = false;
        }
    } else
    {
        DEBUG("%s","DSD Client object is a NULL pointer");
        ERROR("%s","Internal Program Error. Exit Code = 26");
        ret_val = false;
    }

	return ret_val;
}
	
//=====================================================================

