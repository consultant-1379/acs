/*=================================================================== */
   /**

   @file acs_asec_csadmOIHandler.cpp

   Class method implementation.

   This module contains the implementation of class declared in
	acs_asec_csadmOIHandler.h,acs_asec_dsdserver.h,acs_asec_csadmOIHandler.h module

   @version N.N.N

   HISTORY

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       03/03/2015    XFURULL   AXE IO, CBC cipher handling in csadm implementation
   N/A       25/02/2011     NS       Initial Release
   ================================================================== */

//------------------------------------------------------------------
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */


#include <fstream>
#include "acs_asec_ObjectImplementer.h"
#include "acs_asec_global.h"
#include "acs_asec_hamanager.h"
#include <ace/Reactor.h>

/*===================================================================
   Default initialization.
=================================================================== */

char* ACS_ASEC_ObjImpl::FileOutputImplName = const_cast<char*>( "CSADM_Impl");
char* ACS_ASEC_ObjImpl::FileOutputId = const_cast<char*>( "asecAdminId=1,acsSecurityMId=1");
char* ACS_ASEC_ObjImpl::FileOutputParentName = const_cast<char*>( "acsSecurityMId=1");
char* ACS_ASEC_ObjImpl::FileOutputClassName = const_cast<char*>("AsecAdmin");

const int IMPL_CLOSE=115;
ACS_ASEC_ObjImpl *csadmObj=0;
int cmdrCode=0;

//------------------------------------------------------------------------------------------------------------------
ACS_ASEC_ObjImpl::ACS_ASEC_ObjImpl(){

	log.Open("ASECBIN");
	csadmObj=this;
	m_global_instance       = ACS_ASEC_Global::instance();
}

//------------------------------------------------------------------------------------------------------------------
ACS_ASEC_ObjImpl::~ACS_ASEC_ObjImpl(){
	
	log.Close();
}	

//------------------------------------------------------------------------------------------------------------------
ACE_HANDLE ACS_ASEC_ObjImpl::get_handle(void) const {

	return (ACE_HANDLE)this->selObj;	
}

//------------------------------------------------------------------------------------------------------------------
int ACS_ASEC_ObjImpl::launchCmd(string cmd, const char *mode) {

        FILE *fp;
        char line[LINE_SIZE];
	char buff[LINE_SIZE];

        fp = popen(cmd.c_str(),mode);
        if (fp == NULL){
		syslog(LOG_INFO,"ACS_ASEC_ObjImpl:launchCmd() - Error launching: [%s] \n",cmd.c_str());
		sprintf(buff,"ACS_ASEC_ObjImpl:launchCmd() - Error launching: [%s] \n",cmd.c_str());
                log.Write(buff,LOG_LEVEL_INFO);
		return -1;
        }


	while (fgets(line,LINE_SIZE,fp) != NULL) {
	}

	int status = pclose(fp);

        if (WIFEXITED(status) ){
		cmdrCode = WEXITSTATUS(status);

		if ((cmdrCode == 0) || (cmdrCode == 2))
			return 0;
       	}

	return 1;
}

//------------------------------------------------------------------------------------------------------------------
bool ACS_ASEC_ObjImpl::setExitCodeforcsadm(int errorid, int errortextid){
	
	log.Write("ACS_ASEC_ObjImpl:setExitCodeforcsadm() - enter",LOG_LEVEL_INFO);
	
	OmHandler omHandler;

	char buff[200];

        ACS_CC_ImmParameter paramTomodify1;
	ACS_CC_ImmParameter paramTomodify2;
        char DnName[] = "asecErrorClassId=1,acsSecurityMId=1";
        char attrName1[] = "errorId";
	char attrName2[] = "errorTextId";
 
	paramTomodify1.attrName = attrName1;
	paramTomodify1.attrType = ATTR_INT32T;
	paramTomodify1.attrValuesNum = 1;
	syslog(LOG_INFO, "paramTomodify1.attrValues created");
	paramTomodify1.attrValues = new void*[paramTomodify1.attrValuesNum];
	paramTomodify1.attrValues[0] = reinterpret_cast<void*>(&errorid);


	paramTomodify2.attrName = attrName2;
        paramTomodify2.attrType = ATTR_INT32T;
        paramTomodify2.attrValuesNum = 1;
	syslog(LOG_INFO, "paramTomodify2.attrValues created");
        paramTomodify2.attrValues = new void*[paramTomodify2.attrValuesNum];
        paramTomodify2.attrValues[0] = reinterpret_cast<void*>(&errortextid);

        if (omHandler.Init() == ACS_CC_FAILURE)
        {
		log.Write("ACS_ASEC_ObjImpl:setExitCodeforcsadm() - omHandler Init() failed", LOG_LEVEL_ERROR);
                return false;
        }

	if ((omHandler.modifyAttribute(DnName, &paramTomodify1 ) == ACS_CC_FAILURE) || (omHandler.modifyAttribute(DnName, &paramTomodify2 ) == ACS_CC_FAILURE)) {
        	syslog(LOG_INFO,"ACS_ASEC_ObjImpl:setExitCodeforcsadm() - set parameter failed for [%s] \n",DnName);
                sprintf(buff,"ACS_ASEC_ObjImpl:setExitCodeforcsadm() - set parameter failed for [%s] \n",DnName);
                log.Write(buff,LOG_LEVEL_INFO);
                omHandler.Finalize();
                return false;
	}
	log.Write("ACS_ASEC_ObjImpl:setExitCodeforcsadm() - modifyAttribute success", LOG_LEVEL_INFO);
	omHandler.Finalize();
	return true;
}

//------------------------------------------------------------------------------------------------------------------
void ACS_ASEC_ObjImpl::saImmOiAdminOperationCallback(SaImmOiHandleT l_immOiHandle,
                                          SaInvocationT invocation,
                                          const SaNameT *csadmOiImplementerectName,
                                          SaImmAdminOperationIdT opId, const SaImmAdminOperationParamsT_2 **params)
{

	ACE_UNUSED_ARG(csadmOiImplementerectName);

	int retCode;
	int connect_flag = 0;
	int exitcodestatus = 0;
	int adminOPid = 0;
	char buff[150];

	csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - enter",LOG_LEVEL_INFO);
	std::string myparam;

	csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - connecting to DSD server",LOG_LEVEL_INFO);
	ACS_ASEC_RuntimeHandler	*csadmRuntimeImplementer = new ACS_ASEC_RuntimeHandler;
	syslog(LOG_INFO, "csadmRuntimeImplementer created");
	
	//DSD client logic getting added

	ACS_DSD_Client* dsdCliObj = new (std::nothrow) ACS_DSD_Client();
	syslog(LOG_INFO, "dsdCliObj created");
	ACS_DSD_Session* Session_client = new (std::nothrow) ACS_DSD_Session();
	syslog(LOG_INFO, "Session_client created");
		
        if (dsdCliObj)
        {
                if (Session_client)
                {
                        string serviceName;
			retCode=csadmObj->GetNodeId();

    			if (retCode == -1) {
				csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - Error finding node-id", LOG_LEVEL_ERROR);
			}
			else if (retCode == 1){
				serviceName = "CSADM_B";
			}
    			else if (retCode == 2){
				serviceName = "CSADM_A";
   			}
			else
			{
				csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - Invalid node-id", LOG_LEVEL_ERROR);
			}

                	sprintf(buff,"ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - DSD service name to be connected is %s",serviceName.c_str());  
		    	csadmObj->log.Write(buff,LOG_LEVEL_INFO);
			int resConnect =  dsdCliObj->connect(*Session_client, serviceName, "CSADM", acs_dsd::SYSTEM_ID_PARTNER_NODE, acs_dsd::NODE_STATE_UNDEFINED);
                	sprintf(buff,"ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - resConnect: %d",resConnect);
			csadmObj->log.Write(buff,LOG_LEVEL_INFO);
			if (resConnect != 0){
				connect_flag = 1;
			}
		}
                else
                {
                        syslog(LOG_INFO,"ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - Session_client Memory allocation failed\n");
                	csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - Session_client Memory allocation failed ",LOG_LEVEL_INFO);
			connect_flag = 1;
			csadmObj->setExitCodeforcsadm(1,1);
			exitcodestatus = 1;
		}
    	}
    	else
    	{
        	syslog(LOG_INFO,"ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - dsdCliObj Mem alloc failed");
		csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - dsdCliObj Mem alloc failed", LOG_LEVEL_ERROR);
		connect_flag = 1;
		csadmObj->setExitCodeforcsadm(1,1);
		exitcodestatus = 1;
    	}

	const char* mode = "r";
        int opId_status = 1;
	fstream log_file;

        log_file.open("/tmp/csadm.printout",ios::out |ios::trunc);

        if (!log_file.is_open())
        {
                syslog(LOG_INFO,"ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - unable to open logfile csadm.printout");
                csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - unable to open logfile csadm.printout", LOG_LEVEL_INFO);
		csadmObj->setExitCodeforcsadm(1,1);
		exitcodestatus = 1;
                return;
        }
	
	if (connect_flag == 0){
		
	//continue further

	if (params[0] != 0){
		std::string my_param ((*((SaStringT*)params[0]->paramBuffer)));
		sprintf(buff,"ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - param value is :[%s]",my_param.c_str());
		csadmObj->log.Write(buff,LOG_LEVEL_INFO);
		myparam = my_param;
	}

        switch (opId)
        {
                case 1:
                        csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - Performing Operation 1...", LOG_LEVEL_INFO);
			adminOPid = 1;
			if (myparam.compare("tcp") == 0){
				string cmdToLaunch("/opt/ap/acs/bin/acs_asec_csadm_operations -p tcp -r on");
                        	if (csadmObj->launchCmd(cmdToLaunch, mode) == 0 ) {
					csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - tcp rules initialised",LOG_LEVEL_INFO);
				}
				else
				{
					csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - csadm -i failed",LOG_LEVEL_INFO);
					csadmObj->setExitCodeforcsadm(3,3);
					exitcodestatus = 1;
				}
			}
			
			if (myparam.compare("all") == 0){
                                string cmdToLaunch("/opt/ap/acs/bin/acs_asec_csadm_operations -p all -r on");
                                if (csadmObj->launchCmd(cmdToLaunch, mode) == 0 ) {
                               		csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback()- tcp rules initialised",LOG_LEVEL_INFO);	
				 }
                                else
				{
					csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - csadm -i failed",LOG_LEVEL_INFO);
					csadmObj->setExitCodeforcsadm(3,3);
					exitcodestatus = 1;
				}
			}

			if (cmdrCode == 0) {
                                csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - launching immupdate for admin operation 1 \n",LOG_LEVEL_INFO);
				csadmRuntimeImplementer->immupdate();
				csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - checking the initialisation status \n",LOG_LEVEL_INFO);
				csadmRuntimeImplementer->immstatus(adminOPid, log_file);
                        }
                        break;

                case 2:
			csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - Performing Operation 2...",LOG_LEVEL_INFO);
			adminOPid = 2;
			if (myparam.compare("all") == 0){
                                string cmdToLaunch("/opt/ap/acs/bin/acs_asec_csadm_operations -p all -r off -s on");
                                if (csadmObj->launchCmd(cmdToLaunch,mode) == 0 ) {
					 csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - protocols allowed",LOG_LEVEL_INFO);
					 if ( cmdrCode == 2 ){
						log_file<<"Insecure protocols and insecure options of secure protocols are already allowed"<<endl;
					 }
					else
					{	
                                		csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - launching immupdate for admin operation 2 \n",LOG_LEVEL_INFO);
						csadmRuntimeImplementer->immupdate();
						log_file<<"Insecure protocols and insecure options of secure protocols are allowed"<<endl;
					}
                                }
                                else
				{
					csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - csadm -a failed",LOG_LEVEL_INFO);
					csadmObj->setExitCodeforcsadm(4,4);
					exitcodestatus = 1;
				}
			 }
			 if (myparam.compare("ftp") == 0){
                                string cmdToLaunch("/opt/ap/acs/bin/acs_asec_csadm_operations -p tcp -r off -f on");
                                if (csadmObj->launchCmd(cmdToLaunch,mode) == 0 ) {
					csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - ftp protocol allowed",LOG_LEVEL_INFO);
					 if ( cmdrCode == 2 ){
                                                log_file<<"FTP protocol is already allowed"<<endl;
                                         }
					 else 
					 {
						csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - launching immupdate for admin operation 2 \n",LOG_LEVEL_INFO);
                                		csadmRuntimeImplementer->immupdate();
						log_file<<"FTP protocol is allowed"<<endl;
					 }

                                }
                                else
				{
					csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - csadm -a ftp failed",LOG_LEVEL_INFO);
					csadmObj->setExitCodeforcsadm(5,5);
					exitcodestatus = 1;
				}
			}
			 if (myparam.compare("cbc") == 0){
                                string cmdToLaunch("/opt/ap/acs/bin/acs_asec_csadm_operations -c on"); 
                                if (csadmObj->launchCmd(cmdToLaunch,mode) == 0 ) {
					csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - cbc protocol allowed",LOG_LEVEL_INFO);
					 if ( cmdrCode == 2 ){
                                                log_file<<"CBC mode of operation is already allowed for SSH"<<endl;
                                         }
					 else 
					 {
						csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - launching immupdate for admin operation 2 \n",LOG_LEVEL_INFO);
                                		csadmRuntimeImplementer->immupdate();
						log_file<<"CBC mode of operation is allowed for SSH"<<endl;
					 }

                                }
                                else
				{
					csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - csadm -a sshcbc failed",LOG_LEVEL_INFO);
					csadmObj->setExitCodeforcsadm(8,8);
					exitcodestatus = 1;
				}
			}
			if (cmdrCode == 1){
				csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - invalidity occured \n",LOG_LEVEL_INFO);
				log_file<<"Security Communication invalid"<<endl;
				log_file<<"Reinitialize with command csadm -i"<<endl;
			}

                        break;
		case 3:
			 csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - Performing Operation 3...",LOG_LEVEL_INFO);
			 adminOPid = 3;
			 if (myparam.compare("all") == 0){
                                string cmdToLaunch("/opt/ap/acs/bin/acs_asec_csadm_operations -p all -r off -s off");
                                if (csadmObj->launchCmd(cmdToLaunch,mode) == 0 ) {
					csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - protocols blocked",LOG_LEVEL_INFO);
					if ( cmdrCode == 2 ){
                                                log_file<<"Insecure protocols and insecure options of secure protocols are already blocked"<<endl;
                                        }
					else
					{
                                                csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - launching immupdate for admin operation 3 \n",LOG_LEVEL_INFO);
						csadmRuntimeImplementer->immupdate();
						log_file<<"Insecure protocols and insecure options of secure protocols are blocked"<<endl;
					}			
                                }
                                else
				{
					csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - csadm -b failed",LOG_LEVEL_INFO);
					csadmObj->setExitCodeforcsadm(9,9);
					exitcodestatus = 1;
				}
			}

                         if (myparam.compare("ftp") == 0){
                                string cmdToLaunch("/opt/ap/acs/bin/acs_asec_csadm_operations -p tcp -r off -f off");
                                if (csadmObj->launchCmd(cmdToLaunch,mode) == 0 ) {
					 csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - ftp protocol blocked",LOG_LEVEL_INFO);
					 if ( cmdrCode == 2 ){
                                                log_file<<"FTP protocol is already blocked"<<endl;
                                         }
					 else
                                         {
                                                csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - launching immupdate for admin operation 3 \n",LOG_LEVEL_INFO);
						csadmRuntimeImplementer->immupdate();
                                                log_file<<"FTP protocol is blocked"<<endl;
                                         }
                                }
                                else
				{
					csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - csadm -b ftp failed",LOG_LEVEL_INFO);
					csadmObj->setExitCodeforcsadm(10,10);
					exitcodestatus = 1;
				}
			 }
                        if (myparam.compare("cbc") == 0){
                                string cmdToLaunch("/opt/ap/acs/bin/acs_asec_csadm_operations -c off");   
                                if (csadmObj->launchCmd(cmdToLaunch,mode) == 0 ) {
					 csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - ssh-cbc blocked",LOG_LEVEL_INFO);
					 if ( cmdrCode == 2 ){
                                                log_file<<"CBC mode of operation is already blocked for SSH"<<endl;
                                         }
					 else
					 {
                                                csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - launching immupdate for admin operation 3 \n",LOG_LEVEL_INFO);
						csadmRuntimeImplementer->immupdate();
						log_file<<"CBC mode of operation is blocked for SSH"<<endl;
					 }

                                }
                                else
				{
					csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - csadm -b sshcbc failed",LOG_LEVEL_INFO);
					csadmObj->setExitCodeforcsadm(14,14);
					exitcodestatus = 1;
				}
                        }

			if (cmdrCode == 1){
				csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - invalidity occured \n",LOG_LEVEL_INFO);
                                log_file<<"Security Communication invalid"<<endl;
                                log_file<<"Reinitialize with command csadm -i"<<endl;
                        }
			break;
			
		case 4:
			csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - Performing Operation 4...",LOG_LEVEL_INFO);
			adminOPid = 4;
			string cmdToLaunch("/opt/ap/acs/bin/acs_asec_csadm_operations -p all -r off");
			if (csadmObj->launchCmd(cmdToLaunch, mode) == 0 ) {
					csadmRuntimeImplementer->immstatus(adminOPid, log_file);
                        }
			else
			{
                                csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - csadm -s failed",LOG_LEVEL_INFO);
                                csadmObj->setExitCodeforcsadm(15,15);
				exitcodestatus = 1;
			}

			if (cmdrCode == 1){
				csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - invalidity occured \n",LOG_LEVEL_INFO);
                                log_file<<"Security Communication invalid"<<endl;
                                log_file<<"Reinitialize with command csadm -i"<<endl;
                        }

			opId_status = 4;
			break;			
						
			
               /* default:
                        break;
		*/		

	}        

	if (opId_status != 4){
		char buffer[100];
		int resSend = Session_client->send("RESTART",100);
		csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - message RESTART sent to passive node",LOG_LEVEL_INFO);
		if ( resSend >= 0)
		{
		    int nBytes = Session_client->recv(buffer, sizeof(buffer));
            	    if (nBytes > 0)
            	    {
			sprintf(buff,"ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - Recieved message is %s",buffer);
			csadmObj->log.Write(buff,LOG_LEVEL_INFO);
			
		     }
		}
	}
	Session_client->close();
	syslog(LOG_INFO, "Session_client deleted");
      	delete Session_client;
      	Session_client = 0;
	if (dsdCliObj){
		delete dsdCliObj;
		syslog(LOG_INFO, "Session_client deleted");
                dsdCliObj = 0;
        }

	if (exitcodestatus == 0){

       		csadmObj->setExitCodeforcsadm(0,0); 
	}
	SaAisErrorT rc = saImmOiAdminOperationResult(l_immOiHandle, invocation, SA_AIS_OK);
      	if(rc != SA_AIS_OK)
       	{
		sprintf(buff,"ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - Failed in Operation %d\n",(int)opId);
		csadmObj->log.Write(buff,LOG_LEVEL_INFO);
       	}
	else
	{	
		csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - Operation Successfull\n");
	}
	} // if connection==0 close

	else
	{
		csadmObj->log.Write("ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - DSD client connect failed",LOG_LEVEL_INFO);
		csadmObj->setExitCodeforcsadm(16,16);
		SaAisErrorT rc = saImmOiAdminOperationResult(l_immOiHandle, invocation, SA_AIS_OK);
        	if(rc != SA_AIS_OK)
        	{
                	syslog(LOG_INFO,"ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - Failed in Operation %d\n",(int)opId);
			sprintf(buff,"ACS_ASEC_ObjImpl:saImmOiAdminOperationCallback() - Failed in Operation %d\n",(int)opId);
			csadmObj->log.Write(buff,LOG_LEVEL_INFO);
        	}

        }
	log_file.flush();	
	log_file.close();
}


//------------------------------------------------------------------------------------------------------------------------------------------
SaAisErrorT ACS_ASEC_ObjImpl::registerOiImplementer(){
        
	log.Write("ACS_ASEC_ObjImpl:registerOiImplementer() - enter", LOG_LEVEL_INFO);
	SaAisErrorT rc;
	static SaVersionT immVersion = { 'A', 2, 1 };
	char buff[LINE_SIZE];

 	rc = saImmOiInitialize_2(&immOiHandle, &callbacks, &immVersion);
        if( rc != SA_AIS_OK){
                syslog(LOG_INFO,"ACS_ASEC_ObjImpl:registerOiImplementer() - saImmOiInitialize_2  ERROR CODE %u\n",rc);
		sprintf(buff,"ACS_ASEC_ObjImpl:registerOiImplementer() - saImmOiInitialize_2  ERROR CODE %u\n",rc);
		log.Write(buff,LOG_LEVEL_INFO);
                return rc;
        }

        implementerName = (char*)ACS_ASEC_ObjImpl::FileOutputImplName;;
        rc = saImmOiImplementerSet(immOiHandle, implementerName);
        if( rc != SA_AIS_OK){
                syslog(LOG_INFO,"ACS_ASEC_ObjImpl:registerOiImplementer() -  saImmOiImplementerSet  ERROR CODE %u\n",rc);
		sprintf(buff,"ACS_ASEC_ObjImpl:registerOiImplementer() - saImmOiImplementerSet  ERROR CODE %u\n",rc);		
		log.Write(buff,LOG_LEVEL_INFO);
                return rc;
        }

	std::string dnName = ACS_ASEC_ObjImpl::FileOutputId;
        csadmOiImplementerectName.length = dnName.length();
        memcpy(csadmOiImplementerectName.value, dnName.c_str(),csadmOiImplementerectName.length);
        const SaNameT *csadmOiImplementerectNames[1];
        csadmOiImplementerectNames[0] = &csadmOiImplementerectName;
	rc = saImmOiObjectImplementerSet(immOiHandle, csadmOiImplementerectNames[0], SA_IMM_ONE );
        if(rc != SA_AIS_OK){
		syslog(LOG_INFO,"ACS_ASEC_ObjImpl:registerOiImplementer() - saImmOiObjectImplementerSet FAILED. Error Code %u", rc);
		sprintf(buff,"ACS_ASEC_ObjImpl:registerOiImplementer() - saImmOiObjectImplementerSet FAILED. Error Code %u", rc);
		log.Write(buff,LOG_LEVEL_INFO);
                return rc;
        }

        rc = saImmOiSelectionObjectGet(immOiHandle, &selObj);
        if(rc != SA_AIS_OK )
        {
        	syslog(LOG_INFO,"ACS_ASEC_ObjImpl:registerOiImplementer() - saImmOiSelectionObjectGet FAILED. Error Code %u", rc);
		sprintf(buff,"ACS_ASEC_ObjImpl:registerOiImplementer() - saImmOiSelectionObjectGet FAILED. Error Code %u", rc);
		log.Write(buff,LOG_LEVEL_INFO);
		return rc;
        }

	return rc;
}

//------------------------------------------------------------------------------------------------------------------------------------------
void ACS_ASEC_ObjImpl::Oifinalize(){

	SaAisErrorT errorCode;

	log.Write("ACS_ASEC_ObjImpl:Oifinalize() - invoked", LOG_LEVEL_INFO);
	errorCode = saImmOiFinalize(this->immOiHandle);
	if (errorCode != SA_AIS_OK){
		char buffer[20];
		sprintf(buffer, "ERROR CODE %d", errorCode);
		log.Write("ACS_ASEC_ObjImpl:Oifinalize() - saImmOiFinalize Failed", LOG_LEVEL_ERROR);
		log.Write(buffer, LOG_LEVEL_ERROR);
		syslog(LOG_ERR, "ACS_ASEC_ObjImpl:Oifinalize() - saImmOiFinalize Failed, Error code [%d]",errorCode);
	}
}

//----------------------------------------------------------------------------------------------------------------------------------
int ACS_ASEC_ObjImpl::handle_input(ACE_HANDLE fd){

	ACE_UNUSED_ARG(fd);
	log.Write("ACS_ASEC_ObjImpl:handle_input() - enter", LOG_LEVEL_INFO);	
	SaAisErrorT rCode;

	rCode=saImmOiDispatch(this->immOiHandle, SA_DISPATCH_ALL);
	if (rCode != SA_AIS_OK){
		char buff[LINE_SIZE];
		sprintf(buff,"ACS_ASEC_ObjImpl:handle_input() - dispatch Failed. Error Code:[%u]", rCode);
		log.Write(buff, LOG_LEVEL_ERROR);
	} else {
		log.Write("ACS_ASEC_ObjImpl:handle_input() - dispatch success");	
	}

	return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------
int ACS_ASEC_ObjImpl::handle_close(ACE_HANDLE /* fd */, ACE_Reactor_Mask /* mask */) {

	log.Write("ACS_ASEC_ObjImpl:handle_close() - Invoked", LOG_LEVEL_INFO);

	return 0;
}	

//----------------------------------------------------------------------------------------------------------------------------------
int ACS_ASEC_ObjImpl::open(asecHAClass *haObj){
	
	m_haObj=haObj;
	int rCode=0;

	if (this->activate(THR_NEW_LWP|THR_JOINABLE) < 0) {
		log.Write("ACS_ASEC_ObjImpl:open() - Failed to start OI thread", LOG_LEVEL_INFO);
		rCode=-1;
	}	

	return rCode;
}

//----------------------------------------------------------------------------------------------------------------------------------
int ACS_ASEC_ObjImpl::svc(){

	bool errorDetected=false;
	bool running=true;
	SaAisErrorT rCode;
	
	if (!errorDetected){
		rCode=this->registerOiImplementer();
		if (rCode != SA_AIS_OK){
			log.Write("ACS_ASEC_ObjImpl:svc() - registerOiImplementer Failed", LOG_LEVEL_ERROR);
			log.Write("ACS_ASEC_ObjImpl:svc() - releasing Oi Handle", LOG_LEVEL_INFO);
			this->Oifinalize();
			errorDetected=true;
		}
	}

	//register sleObj handle 
	if (!errorDetected) {
		int status = ACS_ASEC_Global::instance()->reactor()->register_handler(this, ACE_Event_Handler::READ_MASK);
		if (status < 0) {
			log.Write("ACS_ASEC_ObjImpl:svc() - register_handler(this, ACE_Event_Handler::READ_MASK) Failed", LOG_LEVEL_ERROR);
			errorDetected=true;
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
	
	log.Write("ACS_ASEC_ObjImpl:svc() - registerOiImplementer success", LOG_LEVEL_INFO);
	log.Write("ACS_ASEC_ObjImpl:svc() - thread running", LOG_LEVEL_INFO);

	ACE_Message_Block* mb = 0;

	while (running){
		try {
			if (this->getq(mb) < 0){
				log.Write("ACS_ASEC_ObjImpl:svc() - getq() failed", LOG_LEVEL_ERROR);
				break;
			}

			// Check msg type
			switch(mb->msg_type()){
				
				case IMPL_CLOSE:
						log.Write("ACS_ASEC_ObjImpl:svc() - IMPL_CLOSE received", LOG_LEVEL_INFO);
						mb->release();
						running=false;
						break;
				default:
						log.Write("ACS_ASEC_ObjImpl:svc() - invalid message received", LOG_LEVEL_ERROR);
						mb->release();
						running=false;
						break;
			}// end of swith
		} // end of try

		catch(...){
			log.Write("ACS_ASEC_ObjImpl:svc() - New Exception", LOG_LEVEL_ERROR);
		}	
	} // end of while

	return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------
int ACS_ASEC_ObjImpl::close(){

	ACE_Message_Block* mb = 0;
	ACE_NEW_NORETURN( mb, ACE_Message_Block());

	int rCode=0;
	if (0 == mb) {
		log.Write("ACS_ASEC_ObjImpl::close() - Failed to create mb object", LOG_LEVEL_ERROR);
		rCode=-1;
	}
	
	log.Write("ACS_ASEC_ObjImpl::close() - Posting IMPL_CLOSE to main thread", LOG_LEVEL_INFO);
	if (rCode != -1){
		mb->msg_type(IMPL_CLOSE);
		if (this->putq(mb) < 0){
			log.Write("ACS_ASEC_ObjImpl::close() - Fail to POST IMPL_CLOSE", LOG_LEVEL_ERROR);
			mb->release();
			rCode=-1;
		}
	}

	return rCode;
}

//----------------------------------------------------------------------------------------------------------------------------------
int ACS_ASEC_ObjImpl::close(u_long) {

	log.Write("ACS_ASEC_ObjImpl:close(u_long) - invoked", LOG_LEVEL_INFO);
	// check that we're really shutting down.
	// ACS_USA_Adm::close' sets it to 'true
	if (!m_global_instance->shutdown_ordered()) {
		log.Write("ACS_ASEC_ObjImpl:close(u_long) Abnormal shutdown", LOG_LEVEL_ERROR);
		exit(EXIT_FAILURE);
	}

	// Close the handler
	log.Write("ACS_ASEC_ObjImpl:close(u_long) - remove_handler invoked", LOG_LEVEL_INFO);
	int status=ACS_ASEC_Global::instance()->reactor()->remove_handler(this, ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL);
	if (status < 0){
		log.Write("ACS_ASEC_ObjImpl:close(u_long) - remove_handler failed", LOG_LEVEL_ERROR);	
	}	
	
	// Close OI handler
	this->Oifinalize();

	return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------
int ACS_ASEC_ObjImpl::GetNodeId(){

        FILE* fp;
        ACE_TCHAR buff[10];
        ACE_TCHAR buffer[100];
        ACE_INT32 node_id=0;

        fp = ACE_OS::fopen(NODE_ID_FILE,"r");

        if ( fp == NULL ) {
                syslog(LOG_ERR,"ACS_ASEC_ObjImpl:GetNodeId() -  Error! fopen FAILED");
                log.Write("ACS_ASEC_ObjImpl:GetNodeId() -  Error! fopen FAILED",LOG_LEVEL_ERROR);
                return -1;
        }

        if (fscanf(fp ,"%s" ,buff) != 1 ) {
                (void)fclose(fp);
                syslog(LOG_ERR ,"ACS_ASEC_ObjImpl:GetNodeId() - Unable to Retreive the node id from file [ %s ]" ,NODE_ID_FILE);
                sprintf(buffer,"ACS_ASEC_ObjImpl:GetNodeId() - Unable to Retreive the node id from file [ %s ]" ,NODE_ID_FILE);
                log.Write(buffer,LOG_LEVEL_ERROR);
                return -1;
        }

        if (ACE_OS::fclose(fp) != 0 ) {
                syslog(LOG_ERR ,"ACS_ASEC_ObjImpl:GetNodeId() - Error! fclose FAILED");
                log.Write("ACS_ASEC_ObjImpl:GetNodeId() - Error! fclose FAILED",LOG_LEVEL_ERROR);
                return -1;
        }

        node_id= ACE_OS::atoi(buff);
        return node_id;
}
//----------------------------------------------------------------------------------------------------------------------------------

