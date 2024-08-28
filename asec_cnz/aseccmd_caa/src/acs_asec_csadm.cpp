
/*===i=============================================================== */
   /**
   @file acs_asec_csadm.cpp

   Class method implementation.
   
   This module contains the implementation of class declared in 
   the acs_asec_csadm.h module

   @version N.N.N

   HISTORY

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A	     16/02/2022    XSOWMED   Removed telnet code
   N/A	     03/03/2015    XFURULL   AXE IO, CBC cipher handling in csadm implementation
   N/A       25/02/2011     NS       Initial Release
=================================================================== */

/*=================================================================== 
                        INCLUDE DECLARATION SECTION
=================================================================== */


#include "acs_asec_csadm.h"
#include "acs_prc_api.h"
#include "acs_asec_ObjectImplementer.h"
#include <fstream>
#include <unistd.h>
#include <csignal>

ACS_TRA_Logging log_tra;
char buff[LINE_SIZE];
int ret_val = 0;
int admin_op_status = 0;
static int interuptRcvd = 0;

bool isLockFileExist(void)
{
	if ( 0 == access("/tmp/csadm.lock", F_OK) ) {
		return true;
	}
	else {
		return false;
	}
}

Lock::Lock()
{
	lock_acquired = false;
}

bool Lock::createLockfile()
{
	FILE *fp = NULL;
	fp = fopen("/tmp/csadm.lock", "w");
	if(NULL != fp) {
		unsigned int newPid = getpid();
		//syslog(LOG_INFO,"csadm lock file created with pid [%d]",newPid);
		fprintf(fp,"%d",newPid );
		fclose(fp);
		return true;
	}
	else {
		syslog(LOG_INFO,"csadm lock file open failed");
		return false;
	}
}

bool Lock::acquireLock()
{
	char runPid[10]={'\0'};
	bool pidFound=false;
	FILE *rfp = NULL;
	rfp = fopen("/tmp/csadm.lock", "r");
	if( NULL != rfp ) {
		while(!feof(rfp))
		{
			if( (fscanf(rfp, "%s",runPid)) > 0 ) {
				pidFound=true;
				syslog(LOG_INFO,"csadm already running pid: %s",runPid);
			}
			break;
		}
		fclose(rfp);
	}

	if (true == pidFound) {
		string pid = runPid;
		string pidFile = "/proc/"+pid;
		syslog(LOG_INFO,"csadm pidFile : %s",pidFile.c_str());
		if ( 0 == access(pidFile.c_str(), F_OK) ) {
			lock_acquired=false;
		}
		else {
			lock_acquired = createLockfile();
		}
	}
	else {
		lock_acquired = createLockfile();
	}

	return lock_acquired;
}

void Lock::releaseLock()
{
	if (lock_acquired == true) {
		int res=0;
		//if (unlink("/tmp/csadm.lock") != 0) {
		res =unlink("/tmp/csadm.lock");
		if ( 0 != res ) {
			syslog(LOG_INFO,"failed [%d] to remove csadm lock file", errno);
		}
		lock_acquired=false;
	}
}

Lock::~Lock()
{
	releaseLock();
}

/*=================================================================== 
   ROUTINE: invokeAdminOperation
=================================================================== */

SaAisErrorT  ACS_ASEC_csadm::invokeAdminOperation(const std::string & i_dn, unsigned int i_operationId ,
                                const SaImmAdminOperationParamsT_2 ** i_params, SaTimeT i_timeout,SaAisErrorT &aOIValidationReturnValue){
	
/*=================================================================== 
   Default initialization.
=================================================================== */

	SaAisErrorT rc;
        SaNameT objectName;
        objectName.length = i_dn.length();
        memcpy(objectName.value, i_dn.c_str(), objectName.length);

        const SaNameT *objectNames[2];
        objectNames[0] = &objectName;
        objectNames[1] = NULL;
	SaImmAdminOwnerHandleT m_ownerHandle;
	SaImmHandleT m_omHandle;

	
/*=================================================================== 
   Place code hereinafter
=================================================================== */

        if( i_params == NULL)
        {
                i_params = ( const SaImmAdminOperationParamsT_2 ** )realloc(NULL, sizeof(SaImmAdminOperationParamsT_2 *));
                i_params[0] = NULL;
        }
        else
        {

        }

        SaVersionT s_immVersion = { 'A', 2, 1 };

        rc = saImmOmInitialize(&m_omHandle, NULL, &s_immVersion);
        if( rc != SA_AIS_OK){
		syslog(LOG_INFO,"saImmOmInitialize FAILED error code[%d]",rc);
                sprintf(buff,"saImmOmInitialize FAILED error code[%d]",rc);
		log_tra.Write(buff,LOG_LEVEL_INFO);
		delete i_params;
		return rc;
        }

        rc = saImmOmAdminOwnerInitialize(m_omHandle, (char*)"APG", SA_TRUE, &m_ownerHandle);
        if( rc != SA_AIS_OK)
        {
		syslog(LOG_INFO,"saImmOmAdminOwnerInitialize FAILED error code[%d]",rc);
		sprintf(buff,"saImmOmAdminOwnerInitialize FAILED error code[%d]",rc);
		log_tra.Write(buff,LOG_LEVEL_INFO);

		delete i_params;
                return rc;
        }
        rc = saImmOmAdminOwnerSet(m_ownerHandle, objectNames, SA_IMM_ONE);
        if( rc != SA_AIS_OK)
        {
		syslog(LOG_INFO,"saImmOmAdminOwnerSet FAILED: error code[%d]", rc);
		sprintf(buff,"saImmOmAdminOwnerSet FAILED: error code[%d]", rc);
		log_tra.Write(buff,LOG_LEVEL_INFO);
                rc = saImmOmAdminOwnerFinalize(m_ownerHandle);
                if (SA_AIS_OK != rc) {
			syslog(LOG_INFO,"saImmOmAdminOwnerFinalize FAILED error code[%d]",rc);
			sprintf(buff,"saImmOmAdminOwnerFinalize FAILED error code[%d]",rc);
			log_tra.Write(buff,LOG_LEVEL_INFO);
			delete i_params;
                        return rc;
                }
                rc = saImmOmFinalize(m_omHandle);
                if (SA_AIS_OK != rc)
                {
			delete i_params;
                        return rc;
                }
		delete i_params;
                return rc;
        }
	

        rc = saImmOmAdminOperationInvoke_2(m_ownerHandle, &objectName, 0, i_operationId, i_params,
                        &aOIValidationReturnValue, SA_TIME_ONE_SECOND * i_timeout);
        if( rc != SA_AIS_OK )
        {
		syslog(LOG_INFO,"saImmOmAdminOperationInvoke_2 FAILED. Error Code[%d]",rc);
		sprintf(buff,"saImmOmAdminOperationInvoke_2 FAILED. Error Code[%d]",rc);
		log_tra.Write(buff,LOG_LEVEL_INFO);
                return rc;
        }
        // Clean Up the Admin Handle and OM Hanlde
	 rc = saImmOmAdminOwnerFinalize(m_ownerHandle);
        if (SA_AIS_OK != rc)
        {
                return rc;
        }
        rc = saImmOmFinalize(m_omHandle);
        if (SA_AIS_OK != rc) {
                return rc;
        }

        return rc;
}


/*===================================================================
   ROUTINE: CreateIMMParam
=================================================================== */

void ACS_ASEC_csadm::CreateIMMParam(SaImmAdminOperationParamsT_2* & aParam , std::string arg){
	aParam = (SaImmAdminOperationParamsT_2 *) malloc(sizeof(SaImmAdminOperationParamsT_2));
        char *tmp = strdup("inp_param");
        aParam->paramName = tmp;
        aParam->paramType = SA_IMM_ATTR_SASTRINGT;
        aParam->paramBuffer = immutil_new_attrValue(arg.c_str());
}


/*===================================================================
   ROUTINE: immutil_new_attrValue
=================================================================== */

void* ACS_ASEC_csadm::immutil_new_attrValue(const char *str){
	 void *attrValue = NULL;
	 attrValue = malloc(sizeof(SaStringT));
         *((SaStringT *)attrValue) = strdup(str);
	return attrValue;
}


/*===================================================================
   ROUTINE: getExitCode
=================================================================== */

int ACS_ASEC_csadm::getExitCode(int &p_exitcode,int &p_exittextcode){
	OmHandler omHandler;

	ACS_CC_ImmParameter paramTofind1;
	ACS_CC_ImmParameter paramTofind2;
	char DnName[] = "asecErrorClassId=1,acsSecurityMId=1";
	char attrName1[] = "errorId";
	char attrName2[] = "errorTextId";
	paramTofind1.attrName = attrName1;
	paramTofind2.attrName = attrName2;
	char buff[200];
	int val1 = -1;
	int val2 = -1;

	if (omHandler.Init() == ACS_CC_FAILURE)
	{
		return FALSE;
	}

	if ((omHandler.getAttribute(DnName,&paramTofind1 ) == ACS_CC_FAILURE)){ 
		syslog(LOG_INFO,"csadmservice:getAttribute: get parameter failed for [%s] attribute [%s] \n",DnName,attrName1);
		sprintf(buff,"csadmservice:getAttribute: get parameter failed for [%s] attribute [%s] \n",DnName,attrName1);
		log_tra.Write(buff,LOG_LEVEL_INFO);
		omHandler.Finalize();
		return FALSE;
	}

	else
	{

		if(paramTofind1.attrValuesNum == 0){
			syslog(LOG_INFO,"csadm: The attribute [%s] has no value set\n",attrName1);
		}
		else
		{
			//syslog(LOG_INFO,"csadm: getattribute success for [%s]",attrName1); 
			val1 = *(reinterpret_cast<int*>(*(paramTofind1.attrValues)));
		}

	}

	if ((omHandler.getAttribute(DnName,&paramTofind2 ) == ACS_CC_FAILURE)){
		syslog(LOG_INFO,"csadmservice:getAttribute: get parameter failed for [%s] attribute [%s] \n",DnName,attrName2);
		sprintf(buff,"csadmservice:getAttribute: get parameter failed for [%s] attribute [%s] \n",DnName,attrName2);
		log_tra.Write(buff,LOG_LEVEL_INFO);
		omHandler.Finalize();
		return FALSE;
	}

	else
	{

		if(paramTofind2.attrValuesNum == 0){
			syslog(LOG_INFO,"csadm: The attribute [%s] has no value set\n",attrName2);
		}
		else
		{
			syslog(LOG_INFO,"csadm: getattribute success for [%s]",attrName2);
			val2 = *(reinterpret_cast<int*>(*(paramTofind2.attrValues)));
		}

	}

	p_exitcode = val1;
	p_exittextcode = val2;
	omHandler.Finalize();
	return TRUE;

}

/*===================================================================
   ROUTINE: displayErrorMessage
=================================================================== */
void ACS_ASEC_csadm::displayErrorMessage(){
	bool ret = false;
	int p_msgcode;
	int p_code;
	if (admin_op_status == 0){
		ret = getExitCode(p_code,p_msgcode);
        	if ( ret == true )
        	{
			//syslog(LOG_INFO,"csadm: success in retrieving error codes from imm");
			if (p_msgcode == 1){
				cout<<"Error when executing (general fault)"<<endl;
			}     
			else if (p_msgcode == 3){
				cout<<"csadm -i failed"<<endl;
			}
			else if (p_msgcode == 4){
                        	cout<<"csadm -a failed"<<endl;
                	}
			else if (p_msgcode == 5){
                        	cout<<"csadm -a ftp failed"<<endl;
                	}
			else if (p_msgcode == 6){
                        	cout<<"csadm -a telnet failed"<<endl;
                	}
			else if (p_msgcode == 7){
                        	cout<<"csadm -a mts failed"<<endl;
                	}
			else if (p_msgcode == 8){
                        	cout<<"csadm -a sshcbc failed"<<endl;
                	}
			else if (p_msgcode == 9){
                        	cout<<"csadm -b failed"<<endl;
                	}
			else if (p_msgcode == 10){
				cout<<"csadm -b ftp failed"<<endl;
			}
			else if (p_msgcode == 11){
				cout<<"csadm -b telnet failed"<<endl;
			}
			else if (p_msgcode == 12){
                        	cout<<"csadm -b mts failed"<<endl;
                	}
			else if (p_msgcode == 14){
                        	cout<<"csadm -b sshcbc failed"<<endl;
                	}
			else if (p_msgcode == 15){
                        	cout<<"csadm -s failed"<<endl;
                	}
			else if (p_msgcode == 18){
                        	cout<<"Admin operation failed\n"<<endl;
                	}
			else if (p_msgcode == 16){
                        	cout<<"dsd server connection failed"<<endl;
                	}
		
			ret_val = p_code;

        	}
		else
		{
			syslog(LOG_INFO,"csadm: failure in retrieving error code values");
			cout<<"Error handling failure\n"<<endl;
			ret_val = 17; 	
		}
	}

	if (admin_op_status == 1){
		cout<<"Admin operation failed\n"<<endl;
		ret_val = 15;
	}

}

/*===================================================================
   ROUTINE: callIMMAdminOp
=================================================================== */


void ACS_ASEC_csadm::callIMMAdminOp(const unsigned int adminOperationId, std::string arg)
{

	Lock lockObj;
	if(false == lockObj.acquireLock()){
		cout <<"csadm operation is already in progress\n"<<endl;
		return;
	}

/*===================================================================
   Default initialization.
=================================================================== */

	SaImmAdminOperationParamsT_2 *param;
	CreateIMMParam(param,arg);
	
	const SaImmAdminOperationParamsT_2 **params;
        params = (const SaImmAdminOperationParamsT_2**) realloc(NULL, sizeof(SaImmAdminOperationParamsT_2 *));
        params[0] = NULL;
	string p_msg;

/*=================================================================== 
   Place code hereinafter
=================================================================== */


	if (!arg.empty()){
		int params_len = 2;
        	params = (const SaImmAdminOperationParamsT_2**) realloc(params, (params_len + 1) * sizeof(SaImmAdminOperationParamsT_2 *));
		params[0] = param;
		params[1] = NULL;
	}

	if( (adminOperationId < INIT) || (adminOperationId > STATUS) ){
		syslog(LOG_INFO,"csadm:callIMMAdminOp: invalid admin method called");
		log_tra.Write("csadm:callIMMAdminOp: invalid admin method called",LOG_LEVEL_INFO);
	}
	
        SaAisErrorT myOIValidationReturnValue = SA_AIS_OK;
	 // Check if a lock file exists or not.


        SaAisErrorT myImmAdmReturnValue;
	if (STATUS == adminOperationId){
        	myImmAdmReturnValue = invokeAdminOperation(RDN_NAME,adminOperationId,NULL,500,myOIValidationReturnValue);
	}
	else
	{
		myImmAdmReturnValue = invokeAdminOperation(RDN_NAME,adminOperationId,params,500,myOIValidationReturnValue);
	}

        if(myImmAdmReturnValue == SA_AIS_ERR_TIMEOUT) {
		syslog(LOG_INFO,"csadm:callIMMAdminOp: Timeout error occured");
		log_tra.Write("csadm:callIMMAdminOp: Timeout error occured",LOG_LEVEL_INFO);
      	}
        else if(myImmAdmReturnValue != SA_AIS_OK) {
		syslog(LOG_INFO,"csadm:callIMMAdminOp: IMM Admin operation failed");
		log_tra.Write("csadm:callIMMAdminOp: IMM Admin operation failed",LOG_LEVEL_INFO);
		admin_op_status = 1;
		displayErrorMessage();
	}
        else {
            	if(myOIValidationReturnValue != SA_AIS_OK) {
			syslog(LOG_INFO,"csadm:callIMMAdminOp: OI validation failed");
			log_tra.Write("csadm:callIMMAdminOp: OI validation failed",LOG_LEVEL_INFO);
			displayErrorMessage();
		}
               	else {
                    	//Success Case -  Do nothing
			//syslog(LOG_INFO,"csadm:callIMMAdminOp: OI validation success");
			log_tra.Write("csadm:callIMMAdminOp: OI validation success",LOG_LEVEL_INFO);
			displayErrorMessage();
               	}
			
		fstream log_file;
               	string line;

               	log_file.open("/tmp/csadm.printout",ios::in);
		if ( log_file.is_open()) {
			while( log_file.good()) {
				getline(log_file, line);
				cout <<line<<endl;
			}

			log_file.close();
			//system("rm -f /tmp/csadm.printout");
			if (unlink("/tmp/csadm.printout") != 0){
				syslog(LOG_INFO,"csadm unlink failed [%d] removing csadm printout",errno);
			}

		}
 	}
}



/*=================================================================== 
   ROUTINE: Usage()
=================================================================== */
	
void ACS_ASEC_csadm::Usage(){
        cout << "Usage:"                                                         << endl;
        printf("%s%-13s%s\n","csadm"," -s","Show communication security status");
	printf("%s%-13s%s\n","csadm"," -a [-f] [ftp/sshcbc]"," Allow specific insecure protocol and SSHCBC mode of operation [forced, no dialogue]");
	printf("%s%-13s%s\n","csadm"," -b [-f] [ftp/sshcbc]"," Block specific insecure protocol and SSHCBC mode of operation [forced, no dialogue]");
        printf("%s%-13s%s\n","csadm"," -i [-f]", "Initialize communication security [forced, no dialogue]");
	
}


/*===================================================================
   ROUTINE: confirm()
=================================================================== */

bool ACS_ASEC_csadm::confirm()
{
	while(1){
		cout << "Are you sure you want to do these changes" << endl; // common confirmation request
		cout << "[y=yes, n=no]?: ";
		string answer;
		//cin >> answer;
		getline(cin,answer);

		if ((answer.compare("y") == 0))
			return true;
		else if ((answer.compare("n") == 0))
			 return false;
		else
			continue;
	}	
}


/*===================================================================
   ROUTINE: CheckActiveNode()
=================================================================== */

//bool ACS_ASEC_csadm::CheckActiveNode()
//{
     
/*===================================================================
   Default initialization.
=================================================================== */

//	FILE *fp;
 //       char line[LINE_SIZE];
//        char *p;
//        char active[] = "ACTIVE";
//	bool status=false;

/*===================================================================
   Place code hereinafter
=================================================================== */


//        fp = popen("/opt/ap/apos/bin/nodestate","r");
//        if(fp == NULL)
//        {
//               syslog(LOG_INFO,"csadm:callIMMAdminOp: error in popen of CheckActiveNode()\n");
//               printf("%d\n",errno);
//        }

//        while(fgets(line,100,fp) != NULL){

//                syslog(LOG_INFO,"line = [%s]",line);
//                p = strrchr(line,' ');
//                p = p + 1;
//                syslog(LOG_INFO,"node is [%s]",p);
//                if (strncmp(p,active,strlen(active)) == 0){
//                        status=true;
//                }
//                else
//                        status=false;

//        }

  //      pclose(fp);
	
//	return status;

//}

ACE_INT32 ACS_ASEC_csadm::CheckActiveNode() {
	
	ACS_PRC_API prcObj;
        int nodeState = prcObj.askForNodeState();
        return nodeState;
}


bool ACS_ASEC_csadm::isCSADMServiceRunning(std::string aServiceName)
{
	FILE *fp;
	char cmd[100];
	int ret = -1;
	int status = -1;
	std::string commandtoPass = "ps -ea | grep ";
	commandtoPass += aServiceName;
	commandtoPass += " | grep -v \"grep\" 1>/dev/null 2>/dev/null";
	ACE_OS::sprintf(cmd,commandtoPass.c_str());
	fp = popen(cmd,"r");

	if (fp == NULL)
	{
		return false;
	}
	status = pclose(fp);
	if (WIFEXITED(status))
	{
		ret=WEXITSTATUS(status);
	}
	if (ret == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


int checkPS()
{
        FILE *fp;
        char line[100];
        char attr[]="physicalSeparationStatus";
        char* attr_val=(char*)"";
        char buff[100];

        fp = popen("immlist -a physicalSeparationStatus northBoundId=1,networkConfigurationId=1","r");

        if(fp == NULL)
        {
                syslog(LOG_INFO, "ACS_ASEC_RuntimeHandler:checkPS() - popen Error. Error Code[%u]",errno);
                sprintf(buff,"ACS_ASEC_RuntimeHandler:checkPS() - popen Error. Error Code[%u]",errno);
                log_tra.Write(buff,LOG_LEVEL_INFO);
                return 0;
        }

        while(fgets(line,100,fp)!= NULL)
        {
                if (strstr(line,attr)!=NULL){
                        attr_val=strrchr(line,'=');
                        attr_val=attr_val + 1;
                }
        }
	int status = pclose(fp);
	if (WIFEXITED(status))
	{
		int ret=WEXITSTATUS(status);
		sprintf(buff,"ACS_ASEC_RuntimeHandler:CheckPS() - return value is popen in immupdate is %d\n",ret);
		log_tra.Write(buff,LOG_LEVEL_INFO);
	}

        return (atoi(attr_val));
}

void signalHandler( int signum )
{
	if( interuptRcvd == 0 ){
		interuptRcvd=1; 
		if ( true == isLockFileExist() ) {
			if( signum == SIGTERM || signum == SIGINT || signum == SIGTSTP || signum == SIGQUIT){
				syslog(LOG_INFO,"csadm:interrupt recieved");
				std::cout<<"\ncsadm operation is in progress\n"<<std::endl;
			}
		}
		else{
			cout<<"\nCommand execution aborted by user\n"<<endl;
			exit(1);
		}
	}
}
        

/*=================================================================== */
/**
   @brief           OM main method.

   main:            Main Method

                    

   @return          int

   **/
/*=================================================================== */



int main(int argc, char * argv[])
{

	ACS_ASEC_csadm *csadm = new ACS_ASEC_csadm;
	std::string service = "acs_asec_csadmd";	
	ACE_INT32 n_state;
	int psstate;
	
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	//signal(SIGTSTP, signalHandler);
	signal(SIGQUIT, signalHandler);

        n_state = csadm->CheckActiveNode();
	psstate = checkPS();
        if (n_state == -1) {
        	syslog(LOG_ERR, "Unable to find the NodeState.. Exiting");
                cout<<"Unable to find the nodestate\n"<<endl;
                return 1;
        }

	if (n_state != 1) { // passive node
		cout<<"This command is used on an active node!\n"<<endl;
		return 1;
	}
		
	if (!(csadm->isCSADMServiceRunning(service))){
		cout<<"Error when executing (general fault)\n"<<endl;
		return 1;	
	}

	if (argc > 4){
		cout << "Incorrect usage"<<endl;	
		csadm->Usage();
		return 2;
        }

        if (argc > 1){
                if (strcmp(argv[1],"-i") == 0){
			if (argc == 3){
				if (strcmp(argv[2],"-f") == 0){
					csadm->callIMMAdminOp(INIT,"tcp");
				}
				else
				{
					cout << "Incorrect usage"<<endl;
					csadm->Usage();
					return 2;
				}
			}
			else if (argc == 2){
				
				if(!psstate)	
				{
				 	cout << "This function will initialize inbound connections using"<<endl;
	                                cout << "insecure protocols and insecure options of secure"<<endl;
        	                        cout << "protocols on interface public"<<endl;
				}	
				else
				{
					cout << "This function will initialize inbound connections using"<<endl;
					cout << "insecure protocols and insecure options of secure"<<endl;
					cout << "protocols on interface public and public_2"<<endl;
				}
                                 if (csadm->confirm()){
					csadm->callIMMAdminOp(INIT,"tcp");
				 }
				 else
                                 {
                                        cout<<"Command execution aborted by user\n"<<endl;
                                        return 1;
                                 }
                        
                	}
			else
			{
				cout << "Incorrect usage"<<endl;
				csadm->Usage();
				return 2;
			}
		}
		else if (strcmp(argv[1],"-a") == 0){
			 if (argc == 3){
				if (strcmp(argv[2],"telnet") == 0){
                                        cout << "TELNET protocol is NOT SUPPORTED\n" <<endl;
					csadm->Usage();
                                        return 2;
                                }
                                else if (strcmp(argv[2],"mts") == 0){
                                        cout << "TELNET protocol is NOT SUPPORTED\n" <<endl;
					csadm->Usage();
                                        return 2;
                                }
				else if (strcmp(argv[2],"ftp") == 0){
					if(!psstate)
					{	
						cout << "This function will allow inbound connections using" << endl; 
						cout << "insecure protocol FTP on interface public" << endl;
					}
                                        else
                                        {
                                                cout << "This function will allow inbound connections using" << endl;
                                                cout << "insecure protocol FTP on interface public and public_2" << endl;
                                        }

					if (csadm->confirm()){	 
						csadm->callIMMAdminOp(ALLOW,"ftp");
					}
					else
					{
						//Do Nothing
						cout<<"Command execution aborted by user\n"<<endl;
						return 1;
					}
                                 }
				else if (strcmp(argv[2],"sshcbc") == 0){
                                         cout << "This function will allow SSHCBC mode of operation" << endl;
					 if (csadm->confirm()){
						csadm->callIMMAdminOp(ALLOW,"cbc");
					 }
					 else
					 {
						 //Do nothing
						 cout<<"Command execution aborted by user\n"<<endl;
						 return 1;
					 }
				}	
	                        else if (strcmp(argv[2],"-f") == 0){
					csadm->callIMMAdminOp(ALLOW,"all");
				}
				else
				{
					cout << "Incorrect usage"<<endl;
					csadm->Usage();
					return 2;
				}
			}
			else if (argc == 4){ 
				if (strcmp(argv[2],"-f") == 0){
                                         if (strcmp(argv[3],"ftp") == 0){
						csadm->callIMMAdminOp(ALLOW,"ftp");
                                         }
                                         else if (strcmp(argv[3],"sshcbc") == 0){
						csadm->callIMMAdminOp(ALLOW,"cbc");
                                         }
					 else if (strcmp(argv[3],"telnet") == 0){
                                        	cout << "TELNET protocol is NOT SUPPORTED\n" <<endl;
						csadm->Usage();
                                        	return 2;
	                                 }
					 else if (strcmp(argv[3],"mts") == 0){
                                                cout << "TELNET protocol is NOT SUPPORTED\n" <<endl;
						csadm->Usage();
                                                return 2;
					 }
                                         else
                                         {
                                                cout << "Incorrect usage"<<endl;
                                                csadm->Usage();
                                                return 2;
                                         }
				 }
                                 else 
                                 {
					cout << "Incorrect usage"<<endl; 
                                        csadm->Usage();
					return 2;
                                 }
                        }
                        else
                        {
                                if(!psstate)
                                {
                                        cout << "This function will allow inbound connections using" << endl;
                                        cout << "insecure protocols and insecure options of secure" << endl;
                                        cout << "protocols on interface public" << endl;
                                }
                                else
                                {
                                        cout << "This function will allow inbound connections using" << endl;
                                        cout << "insecure protocols and insecure options of secure" << endl;
                                        cout << "protocols on interface public and public_2" << endl;
                                }
				
				if (csadm->confirm()){					
					csadm->callIMMAdminOp(ALLOW,"all");
				}
				else
				{
					//Do nothing
					cout<<"Command execution aborted by user\n"<<endl;
					return 1 ;
				}
                        }
                }

		else if (strcmp(argv[1],"-b") == 0){
                         if (argc == 3){
				if (strcmp(argv[2],"telnet") == 0){
                                        cout << "TELNET protocol is NOT SUPPORTED\n"<<endl;
					csadm->Usage();
                                        return 2;
                                }
                                else if (strcmp(argv[2],"mts") == 0){
                                        cout << "TELNET protocol is NOT SUPPORTED\n"<<endl;
					csadm->Usage();
                                        return 2;
                                }
                                else if(strcmp(argv[2],"ftp") == 0){
					if(!psstate)
					{				
				       		cout << "This function will block inbound connections using" << endl;
	                                	cout << "insecure protocol FTP on interface public" << endl;
					}
					else
					{
						cout << "This function will block inbound connections using" << endl;
						cout << "insecure protocol FTP on interface public and public_2" << endl;
					}
                                	if (csadm->confirm()){
						csadm->callIMMAdminOp(BLOCK,"ftp");
					}
					else
					{
						//do nothing...
						cout<<"Command execution aborted by user\n"<<endl;
						return 1;
					}
                                }
				else if (strcmp(argv[2],"sshcbc") == 0){
                                         cout << "This function will block SSHCBC mode of operation" << endl;
					 if (csadm->confirm()){
						csadm->callIMMAdminOp(BLOCK,"cbc");
					 }
					 else
					 {
						 //Do nothing
						 cout<<"Command execution aborted by user\n"<<endl;
						 return 1;
					 }
				}	
				else if (strcmp(argv[2],"-f") == 0){
					csadm->callIMMAdminOp(BLOCK,"all");
				}
                                else
                                {
					cout << "Incorrect usage"<<endl;
                                        csadm->Usage();
					return 2;
                                }
                        }
			else if (argc == 4){
				if (strcmp(argv[2],"-f") == 0){
					 if (strcmp(argv[3],"ftp") == 0){
						csadm->callIMMAdminOp(BLOCK,"ftp");
                                	 }
                                 	 else if (strcmp(argv[3],"sshcbc") == 0){
						csadm->callIMMAdminOp(BLOCK,"cbc");
                                 	 }
					 else
                                 	 {
						cout << "Incorrect usage"<<endl; 
                                        	csadm->Usage();
                                        	return 2;
                                 	 }

				}
				else
				{
					cout << "Incorrect usage"<<endl;
					csadm->Usage();
					return 2;
				}
			}
                        else
                        {
				if(!psstate)
				{
					cout << "This function will block inbound connections using" << endl;
                                        cout << "insecure protocols and insecure options of secure" << endl;
                                        cout << "protocols on interface public" << endl;
				}
				else
				{
					cout << "This function will block inbound connections using" << endl;
                                        cout << "insecure protocols and insecure options of secure" << endl;
                                        cout << "protocols on interface public and public_2" << endl;
				}
				if (csadm->confirm()){
					 csadm->callIMMAdminOp(BLOCK,"all");
                                }
                                else
				{
					//do nothing...	
					cout<<"Command execution aborted by user\n"<<endl;
					return 1;
					
				}
                        }
                }

		else if (strcmp(argv[1],"-s") == 0){
			if (argc > 2){
				cout << "Incorrect usage"<<endl;
				csadm->Usage();
				return 2;
			}
			else
			{
				csadm->callIMMAdminOp(STATUS,"");
			}
		}
                else
                {
			if(strcmp(argv[1],"-h") == 0){
	                        csadm->Usage();
        	                return 0;
			}
			else
			{
				cout << "Incorrect usage"<<endl;
				csadm->Usage();
				return 2;
			}
                }
        }
	else
	{
		 cout << "Incorrect usage"<<endl;
		 csadm->Usage();
                 return 2;
        }

	return ret_val;
}	
