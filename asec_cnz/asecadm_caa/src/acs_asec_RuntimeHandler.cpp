#include "acs_asec_RuntimeObject.h"
#include <syslog.h>

#include <acs_apgcc_omhandler.h>
#include <acs_apgcc_paramhandling.h>

/****************************************************************************
 * 
 * REVISION HISTORY
 * 
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 * 2022-02-16  XSOWMED  Removed telnet and mts related code
 * 2020-03-20  XNEELKU  Updated Status printout of csadm for IPv6 IP's.
 * 2015-03-03  XFURULL  AXE IO, CBC cipher handling in csadm implementaion
 * 2014-05-22  XFABPAG  Fixed problems in displaying all IP addresses
 ****************************************************************************/

//------------------------------------------------------------------------------------
ACS_ASEC_RuntimeHandler::ACS_ASEC_RuntimeHandler(){
	log.Open("ASECBIN");
	theASECInfoClassName = const_cast<char *>("ASECConfigData");
	theASECInfoParentName = const_cast<char*>("acsSecurityMId=1");
}

//------------------------------------------------------------------------------------
ACS_ASEC_RuntimeHandler::~ACS_ASEC_RuntimeHandler(){
	log.Close();
}


//------------------------------------------------------------------------------------------------------------------
int ACS_ASEC_RuntimeHandler::launchCmd(string cmd, string& result ) {

        FILE *fp;
        char line[LINE_SIZE];
        char buff[LINE_SIZE];
	int cmdrCode = 0;
        fp = popen(cmd.c_str(),"r");
        if (fp == NULL){
                syslog(LOG_INFO,"ACS_ASEC_RuntimeHandler:launchCmd() - Error launching: [%s] \n",cmd.c_str());
                sprintf(buff,"ACS_ASEC_RuntimeHandler:launchCmd() - Error launching: [%s] \n",cmd.c_str());
                log.Write(buff,LOG_LEVEL_INFO);
                return -1;
        }


        while (fgets(line,LINE_SIZE,fp) != NULL) {
        }
	result = line;
        int status = pclose(fp);

        if (WIFEXITED(status) ){
                cmdrCode = WEXITSTATUS(status);

                if ((cmdrCode == 0) || (cmdrCode == 2))
                        return 0;
        }

        return 1;
}

//------------------------------------------------------------------------------------
void ACS_ASEC_RuntimeHandler::getPorts(string service,string &portnum){

	int found;
	string temp;
	FILE * fp;
	char line[LINE_SIZE];
	fp = fopen("/opt/ap/acs/conf/acs_asec_csadm_template.conf","r");
	if(fp == NULL)
	{
		char buff[100];
		syslog(LOG_INFO, "ACS_ASEC_RuntimeHandler:getPorts() - fopen Error. Error code[%d]\n", errno);
		sprintf(buff, "ACS_ASEC_RuntimeHandler:getPorts() - fopen Error. Error code[%d]\n", errno);
		log.Write(buff,LOG_LEVEL_INFO);
		return;
	}

	while(fgets(line,LINE_SIZE,fp)!= NULL)
	{
		temp=(string)line;
		if ((found=temp.find(service))==1)
		{
			char* token=strtok(line,":");
			token=strtok(NULL," :\n");
			portnum = (string)token;
		}
	}

	fclose(fp);
}

//------------------------------------------------------------------------------------
bool ACS_ASEC_RuntimeHandler::GetParams(string dnName,char* paramvalue){

        ACS_CC_ImmParameter paramToFind;
        char DnName[150];
        char ASEC_parent_name[] = "acsSecurityMId=1";
        paramToFind.attrName = new char[30];
        char attrName[] = "state";
        paramToFind.attrName = attrName;
        ACE_OS::strcpy(DnName,dnName.c_str());
        ACE_OS::strcat(DnName,",");
        ACE_OS::strcat(DnName,ASEC_parent_name);
        char accept[] = "ACCEPT";
        char allow[] = "ALLOW";
        char block[] = "BLOCK";
        bool myObjectAvailable = false;
	char buff[100];

        OmHandler omHandler;
        if (omHandler.Init() == ACS_CC_FAILURE)
        {
                return false;
        }

        bool myIMMResult = isObjectExistsASECCONFIGCLASS(dnName,myObjectAvailable);
        if (myIMMResult){
                if((myObjectAvailable))
                {
			if (omHandler.getAttribute(DnName, &paramToFind ) == ACS_CC_FAILURE){
				syslog(LOG_INFO,"ACS_ASEC_RuntimeHandler:GetParams() - Error. DN[%s] \n",DnName);
				sprintf(buff,"ACS_ASEC_RuntimeHandler:GetParams() - Error. DN[%s] \n",DnName);
				log.Write(buff,LOG_LEVEL_INFO);
				omHandler.Finalize();
				return false;
			}
			else
			{
				char *value = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
				if (strncmp(value,accept,strlen(accept)) == 0){
					ACE_OS::strncpy(paramvalue,allow,strlen(allow)+1);
				}
				else
				{
					ACE_OS::strncpy(paramvalue,block,strlen(block)+1);

				}
				sprintf(buff,"ACS_ASEC_RuntimeHandler:GetParams() - Param value = [%s]",paramvalue);
				log.Write(buff,LOG_LEVEL_INFO);
				omHandler.Finalize();
				return true;
			}

                }
                else
                {
                        syslog(LOG_INFO,"ACS_ASEC_RuntimeHandler:GetParams() - Object [%s] does not exist\n",DnName);
                        sprintf(buff,"ACS_ASEC_RuntimeHandler:GetParams() - Object [%s] does not exist\n",DnName);
                        log.Write(buff,LOG_LEVEL_INFO);
                        omHandler.Finalize();
                        return false;
                }

        }
        else
        {
                syslog(LOG_INFO,"ACS_ASEC_RuntimeHandler:GetParams() - is object exists function failed to operate");
                log.Write("ACS_ASEC_RuntimeHandler:GetParams() - is object exists function failed to operate",LOG_LEVEL_INFO);
                omHandler.Finalize();
                return false;
        }

}

int ACS_ASEC_RuntimeHandler::checkPS()
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
                log.Write(buff,LOG_LEVEL_INFO);
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
		log.Write(buff,LOG_LEVEL_INFO);
	}

	return ((atoi(attr_val)));
}




//------------------------------------------------------------------------------------
void ACS_ASEC_RuntimeHandler::immstatus(int adminOpid, fstream &file){

  log.Write("ACS_ASEC_RuntimeHandler:immstatus() - Enter",LOG_LEVEL_INFO);

  string FTP_DN = "asecConfigdataId=FTP";
  string CBC_DN = "asecConfigdataId=SSHCBC";
	char invalid[] = "INVALID";
  char param_val[LINE_SIZE];
  bool params_res;
  int isPsConfigured=0;
	//public_ipaddr_vector.clear();
	//ps_ipaddr_vector.clear();
	//vlan_ipaddr_vector.clear();

	// Check if Physical Seperation is Configured
	isPsConfigured=checkPS();
	if(isPsConfigured)
		log.Write("PS is configured... ",LOG_LEVEL_INFO);       
  // for FTP
  params_res = GetParams(FTP_DN,param_val);
  if (params_res){
    strncpy(FTP_STATE,param_val,strlen(param_val));
    FTP_STATE[strlen(param_val)] = '\0';
  }else{
    strncpy(FTP_STATE,invalid,strlen(invalid));
    FTP_STATE[strlen(invalid)] = '\0';
  }
  // for CBC
  params_res = GetParams(CBC_DN,param_val);
  if (params_res){
    strncpy(CBC_STATE,param_val,strlen(param_val));
    CBC_STATE[strlen(param_val)] = '\0';
  }else{
    strncpy(CBC_STATE,invalid,strlen(invalid));
    CBC_STATE[strlen(invalid)] = '\0';
  }
  
	/*char line[100];
  FILE *fp;
  FILE *fp_min;dminOpid
  FILE *dual_fp_min;
	FILE *ps_fp_min;
  FILE *vlan_fp_min;
  char* public_ipaddr;
  char* public_v6_ipaddr;
	char *ps_ipaddr;
	char *vlan_ipaddr;
	int status=0;*/	

  // Determine public interface by checking teaming status
  /*public_eth_interface = "eth1";
  if (isvAPG()){
    public_eth_interface = "eth1";
  } else if (checkTeamingStatus() == 1 || apgOamAccess() == 1){
	  /* it is assumed that in case of NOCABLE also
	   * pub interface is bond1
		 */
   // public_eth_interface = "bond1";
 //}	

	/*ps_eth_interface = "eth2";
	string public_v6_interface = " public_v6";
	string public_interface = " public ";
	string ps_interface = ps_eth_interface + " public2";
	char vlan_interface[] = "public_vlan";
  char buff[100];

  fp = popen("/opt/ap/apos/bin/clusterconf/clusterconf ip -D","r");
  if(fp == NULL){
    syslog(LOG_INFO, "ACS_ASEC_RuntimeHandler:immstatus() - popen Error. Error Code[%u]",errno);
		sprintf(buff,"ACS_ASEC_RuntimeHandler:immstatus() - popen Error. Error Code[%u]",errno);
    log.Write(buff,LOG_LEVEL_INFO);
    return;
  }

  while(fgets(line,100,fp)!= NULL)
  {
      if (strstr(line,public_interface.c_str())!=NULL){
			  if(line[0]!='#'){
           public_ipaddr=strrchr(line,' ');
	         public_ipaddr=public_ipaddr + 1;
           public_ipaddr_vector.push_back((string)public_ipaddr);
			  }else{
				  log.Write("public ip is commented..",LOG_LEVEL_INFO);
			  }
      }

      if (strstr(line,public_v6_interface.c_str())!=NULL){
                          if(line[0]!='#'){
           public_v6_ipaddr=strrchr(line,' ');
                 public_v6_ipaddr=public_v6_ipaddr + 1;
           public_v6_ipaddr_vector.push_back((string)public_v6_ipaddr);
                          }else{
                                  log.Write("public_v6 ip is commented..",LOG_LEVEL_INFO);
                          }
      }

    if (isPsConfigured){
      if (strstr(line,ps_interface.c_str())!=NULL){
	      if(line[0]!='#'){
				  	ps_ipaddr=strrchr(line,' ');
				  	ps_ipaddr=ps_ipaddr + 1;
				  	ps_ipaddr_vector.push_back((string)ps_ipaddr);
		    }else{
			  		log.Write("ps public ip is commented..",LOG_LEVEL_INFO);
	      }
      }
    }

    if (strstr(line,vlan_interface)!=NULL){
      if(line[0]!='#'){
        vlan_ipaddr=strrchr(line,' ');
	      vlan_ipaddr=vlan_ipaddr + 1;
        vlan_ipaddr_vector.push_back((string)vlan_ipaddr);
	      log.Write("VLAN ip pushed into vector..",LOG_LEVEL_INFO);
      }else{
		  	log.Write("VLAN ip is commented..",LOG_LEVEL_INFO);
	    }
    }
	}

  status = pclose(fp);
	if (WIFEXITED(status)){
		int ret=WEXITSTATUS(status);
		sprintf(buff,"ACS_ASEC_RuntimeHandler:immstatus() - return value is popen in immstatus is %d\n",ret);
		log.Write(buff,LOG_LEVEL_INFO);
	}

	string cmd_public_ips="/usr/bin/grep \"mip control nbi \" /cluster/etc/cluster.conf | grep -v \"#\" | cut -d ' ' -f6";
  fp_min = popen(cmd_public_ips.c_str(),"r");

	if(fp_min == NULL) {
    syslog(LOG_INFO, "ACS_ASEC_RuntimeHandler:immstatus() - popen Error. Error Code[%u]",errno);
    sprintf(buff,"ACS_ASEC_RuntimeHandler:immstatus() - popen Error. Error Code[%u]",errno);
    log.Write(buff,LOG_LEVEL_INFO);
    return;
	}

	while(fgets(line,100,fp_min)!= NULL){
    public_ipaddr_vector.push_back((string)line);
  }

  status = pclose(fp_min);
	if (WIFEXITED(status)){
		int ret=WEXITSTATUS(status);
		sprintf(buff,"ACS_ASEC_RuntimeHandler:immstatus() - return value is popen in immupdate is %d\n",ret);
		log.Write(buff,LOG_LEVEL_INFO);
	}

	string cmd_ps_ips="/usr/bin/grep \"mip control primary_ps " + ps_eth_interface +":1\" /cluster/etc/cluster.conf | grep -v \"#\" |  cut -d ' ' -f6";
  if (isPsConfigured){
    ps_fp_min = popen(cmd_ps_ips.c_str(),"r");

		while(fgets(line,100,ps_fp_min)!= NULL){
      ps_ipaddr_vector.push_back((string)line);
    }
    status = pclose(ps_fp_min);
		if (WIFEXITED(status))
		{
			int ret=WEXITSTATUS(status);
			sprintf(buff,"ACS_ASEC_RuntimeHandler:immupdate() - return value is popen in immupdate is %d\n",ret);
			log.Write(buff,LOG_LEVEL_INFO);
		}
  }

	vlan_fp_min = popen("/usr/bin/grep \"mip control public_vlan\" /cluster/etc/cluster.conf | grep -v \"#\" | cut -d ' ' -f6","r");

	while(fgets(line,100,vlan_fp_min)!= NULL)
	{
		vlan_mipaddr_vector.push_back((string)line);
		log.Write("VLAN mip pushed into vector..",LOG_LEVEL_INFO);
	}

	status = pclose(vlan_fp_min);
	if (WIFEXITED(status))
	{
		int ret=WEXITSTATUS(status);
		sprintf(buff,"ACS_ASEC_RuntimeHandler:immupdate() - return value is popen in immupdate is %d\n",ret);
		log.Write(buff,LOG_LEVEL_INFO);
	}

        string cmd_public_v6_ips="/usr/bin/grep \"mip control nbi_v6 \" /cluster/etc/cluster.conf | grep -v \"#\" | cut -d ' ' -f6";
  dual_fp_min = popen(cmd_public_v6_ips.c_str(),"r");

        if(dual_fp_min == NULL) {
    syslog(LOG_INFO, "ACS_ASEC_RuntimeHandler:immstatus() - popen Error. Error Code[%u]",errno);
    sprintf(buff,"ACS_ASEC_RuntimeHandler:immstatus() - popen Error. Error Code[%u]",errno);
    log.Write(buff,LOG_LEVEL_INFO);
    return;
        }

        while(fgets(line,100,dual_fp_min)!= NULL){
    public_v6_ipaddr_vector.push_back((string)line);
  }

  status = pclose(dual_fp_min);
        if (WIFEXITED(status)){
                int ret=WEXITSTATUS(status);
                sprintf(buff,"ACS_ASEC_RuntimeHandler:immstatus() - return value is popen in immupdate is %d\n",ret);
                log.Write(buff,LOG_LEVEL_INFO);
        }*/

        if (adminOpid == 4){
                printadminOpid_4(file);
        }
        else {

		printadminOpid_1(file);
	}

}

//------------------------------------------------------------------------------------
void ACS_ASEC_RuntimeHandler::printadminOpid_1(fstream &output){

	char invalid[] = "INVALID";
	output<<"Communication Security Initialized"<<endl;
	output<<"Insecure protocol and insecure options of secure"<<endl;
	output<<"protocols status is as follows:"<<endl<<endl;

	if (strncmp(FTP_STATE,invalid,strlen(invalid)) != 0){
		output<<"FTP: "<<FTP_STATE<<endl;
		output<<"TELNET: "<<"NOT SUPPORTED"<<endl;
		output<<"MTS: "<<"NOT SUPPORTED"<<endl;
		output<<"SSHCBC: "<<CBC_STATE<<endl;
	}

}


//------------------------------------------------------------------------------------
void ACS_ASEC_RuntimeHandler::printadminOpid_4(fstream &output){

	char invalid[] = "INVALID";

        bool tcp_filter = true;

        output<<"Communication Security Status \n \n";
        output<<"Inbound connection status using insecure protocols and\n";    
        output<<"insecure options of secure protocol is as follows:\n \n";

        if (strncmp(FTP_STATE,invalid,strlen(invalid)) != 0){
                output<<"FTP: "<<FTP_STATE<<"\n";
        }
        else
        {
                tcp_filter = false;
        }
        output<<"TELNET: "<<"NOT SUPPORTED"<<"\n";
        output<<"MTS: "<<"NOT SUPPORTED"<<"\n";
        if (strncmp(CBC_STATE,invalid,strlen(invalid)) != 0){
                output<<"SSHCBC: "<<CBC_STATE<<"\n";
        }
        /*output<<"for the following ip addresses: \n";

	if((int)public_ipaddr_vector.size())
                output<<"\n";

        for (int i=0;i<(int)public_ipaddr_vector.size();i++)
        {
               output<<"        "<<public_ipaddr_vector[i].c_str();
        }

	if((int)public_v6_ipaddr_vector.size())
                output<<"\n";

	for (int i=0;i<(int)public_v6_ipaddr_vector.size();i++)
	{
               output<<"        "<<public_v6_ipaddr_vector[i].c_str();
	}
	
	if((int)ps_ipaddr_vector.size())
		output<<"\n";

        for (int i=0;i<(int)ps_ipaddr_vector.size();i++)
        {
                output<<"        "<<ps_ipaddr_vector[i].c_str();
        }

	if((int)vlan_ipaddr_vector.size())
		output<<"\n";

	int vlan_ipaddr_count=0, vlan_mipaddr_count=0;
	while(vlan_ipaddr_count<(int)vlan_ipaddr_vector.size())
	{
		output<<"        "<<vlan_ipaddr_vector[vlan_ipaddr_count].c_str();

		if(((vlan_ipaddr_count+1)%2)==0)
		{
			output<<"        "<<vlan_mipaddr_vector[vlan_mipaddr_count].c_str();
			++vlan_mipaddr_count;
			output<<"\n";
		}
		++vlan_ipaddr_count;	
	}*/

	if (tcp_filter){
		output<<"\nTCP filters exist.\n";
	}
}

//------------------------------------------------------------------------------------
void ACS_ASEC_RuntimeHandler::immupdate(){

        log.Write("ACS_ASEC_RuntimeHandler:immupdate() - Enter",LOG_LEVEL_INFO);
        std::vector<std::string> List;
        char* prot_state = NULL;
        char buff[100];
        string portnum;
				int isPsConfigured=0;
        List.push_back("FTP");

        OmHandler omHandler;         
        if(omHandler.Init()==ACS_CC_SUCCESS){
        	log.Write("ACS_ASEC_RuntimeHandler:immupdate() - init() success",LOG_LEVEL_INFO);
        }
       	else
       	{
       		syslog(LOG_INFO,"ACS_ASEC_RuntimeHandler:immupdate() - init() Failed");
            log.Write("ACS_ASEC_RuntimeHandler:immupdate() - init() Failed", LOG_LEVEL_ERROR);
       	}

				/* iptables are defined on the interface
				 * eth1 - In general
				 * bond1 - when teaming is enabled
				 * eth1 and eth2 - when ps is configured
				 */
		     public_eth_interface = "eth1";
				 if (isvAPG()){
           public_eth_interface = "eth1";
				 } 	 
				 else if (checkTeamingStatus() == 1 || apgOamAccess() == 1){
					 /* it is assumed that in case of NOCABLE also
						* pub interface is bond1
						*/
					  public_eth_interface = "bond1";
				 }	

				 ps_eth_interface = "eth2";
				 /* Check if Physical Seperation is Configured
					*/
				 isPsConfigured=checkPS();
         for (unsigned int i=0;i<List.size();i++)
         {
        	int csadmOiImplementer_exists = 0;

            ACS_ASEC_RuntimeHandler::getPorts(List[i],portnum);

            int status;
            char line[100];
            FILE *fp;

			   		/*********************
			   		The format of clusterconf iptables -D is :
			   		iptables rules present are:
				 		1  iptables all -A INPUT -p tcp --dport 67 -i eth1 -j DROP
				 		2  iptables all -A INPUT -p udp --dport 67 -i eth1 -j DROP
				 		3  iptables all -A INPUT -p tcp --dport 161 -i eth1 -j DROP
				 		4  iptables all -A INPUT -p udp --dport 161 -i eth1 -j DROP
				 		5  iptables all -A INPUT -p tcp --dport 162 -i eth1 -j DROP
				 		6  iptables all -A INPUT -p udp --dport 162 -i eth1 -j DROP
			   		********************/
            string cmd="/opt/ap/apos/bin/clusterconf/clusterconf iptables -D | grep -E ";
						if (isPsConfigured){
							cmd=cmd+"'[[:space:]]"+public_eth_interface+"|[[:space:]]"+ps_eth_interface+"'";
						}else{
							cmd=cmd+"'[[:space:]]"+public_eth_interface+"'";
						}	
						cmd=cmd+" | awk '{print $7, $8, $9, $11, $NF}' | grep -w tcp";
						sprintf(buff, "ACS_ASEC_RuntimeHandler:immupdate() - cmd[%s]",cmd.c_str()); 
						log.Write(buff,LOG_LEVEL_INFO);

						string portToCompare;
            fp = popen(cmd.c_str(),"r");
            if(fp == NULL)
            {
            	syslog(LOG_INFO,"ACS_ASEC_RuntimeHandler:immupdate() - popen() Failed. Error code[%u]",errno);
                sprintf(buff,"ACS_ASEC_RuntimeHandler:immupdate() - popen() Failed. Error code[%u]",errno);
                log.Write(buff,LOG_LEVEL_INFO);
                return;
            }
            else
            {
            	sprintf(buff,"ACS_ASEC_RuntimeHandler:immupdate() - popen() success");
                log.Write(buff,LOG_LEVEL_INFO);
            }

            while(fgets(line,100,fp)!= NULL)
            {
            	//Start of the TR HU21326
            	portToCompare.clear();
            	if (portnum.empty() == false)
            	{
            		portToCompare.assign(" ");
            		portToCompare.append(portnum);
            		portToCompare.append(" ");
            	}
            	// End of the TR HU21326
                if (strstr(line,portToCompare.c_str())!=NULL)
                {
                	prot_state=strrchr(line,' ');
                    prot_state=prot_state + 1;
                    sprintf(buff,"ACS_ASEC_RuntimeHandler:immupdate() - [%s] protocol state for [%s]",List[i].c_str(),prot_state);
                    log.Write(buff,LOG_LEVEL_INFO);
                    csadmOiImplementer_exists = 0;
                    break;
                }
                else
                {
                	csadmOiImplementer_exists = 1;
                }
            }

            status= pclose(fp);
            if (WIFEXITED(status))
            {
            	int ret=WEXITSTATUS(status);
                sprintf(buff,"ACS_ASEC_RuntimeHandler:immupdate() - return value is popen in immupdate is %d\n",ret);
                log.Write(buff,LOG_LEVEL_INFO);
            }

            if (csadmOiImplementer_exists == 0){
            	createRuntimeObjectsforASECCONFIGCLASS(List[i],(string)prot_state);
            }
            else
            {
            	bool myObjectAvailable = false;
                string dn("asecConfigdataId=");
                dn.append(List[i]);
                bool myIMMResult = isObjectExistsASECCONFIGCLASS(dn,myObjectAvailable);

                if (myIMMResult)
                {
                	if((myObjectAvailable))
                    {
                		if (omHandler.deleteObject(dn.c_str()))
                        {
                			syslog(LOG_INFO,"ACS_ASEC_RuntimeHandler:immupdate() - succeeded to delete the existing runtime object\n");
                            log.Write("ACS_ASEC_RuntimeHandler:immupdate() - succeeded to delete the existing runtime object\n",LOG_LEVEL_INFO);
                        }
                        else
                        {
                        	syslog(LOG_INFO,"ACS_ASEC_RuntimeHandler:immupdate() - failed to delete the existing runtime object\n");
                            log.Write("ACS_ASEC_RuntimeHandler:immupdate() - failed to delete the existing runtime object\n",LOG_LEVEL_INFO);
                        }
                     }
                }
            }
        }
		char  buffer[2];
    FILE *fp;
		string cbc_state;
		fp = fopen("/opt/ap/acs/conf/acs_asec_sshcbc.conf","r");
		if(fp == NULL){
			syslog(LOG_INFO,"ACS_ASEC_RuntimeHandler:immupdate() - popen() Failed for CBC. Error code[%u]",errno);
      sprintf(buff,"ACS_ASEC_RuntimeHandler:immupdate() - popen() Failed for CBC. Error code[%u]",errno);
      log.Write(buff,LOG_LEVEL_INFO);
      return;
    }else{
			sprintf(buff,"ACS_ASEC_RuntimeHandler:immupdate() - fopen() for CBC success");
			log.Write(buff,LOG_LEVEL_INFO);
		}
		if(fscanf(fp,"%s",buffer) != 1)
		{
			(void)fclose(fp);
			syslog(LOG_ERR ,"Unable to Retreive the status from file");
			log.Write("Unable to Retreive the status from file",LOG_LEVEL_ERROR);
			return ;
		}
		if (fclose(fp) != 0 ) {
			syslog(LOG_ERR ,"Error! fclose sshcbc FAILED");
			log.Write("Error! fclose sshcbc FAILED",LOG_LEVEL_ERROR);
			return ;
		}

		int result = atoi(buffer);
    sprintf(buff,"ACS_ASEC_RuntimeHandler:immupdate() - Result for CBC. [%u]",result);
    log.Write(buff,LOG_LEVEL_INFO);
		if (result==0){
			cbc_state="DROP";
      sprintf(buff,"ACS_ASEC_RuntimeHandler:immupdate() - result [%u] and CBC state for [%s]",result,cbc_state.c_str()); 
      log.Write(buff,LOG_LEVEL_INFO); 
		}else if (result==1){
			cbc_state="ACCEPT";
      sprintf(buff,"ACS_ASEC_RuntimeHandler:immupdate() - result [%u] and CBC state for [%s]",result,cbc_state.c_str()); 
      log.Write(buff,LOG_LEVEL_INFO); 
		}else{
	    sprintf(buff,"ACS_ASEC_RuntimeHandler:immupdate() - CBC state for [%u]",result); 
	    log.Write(buff,LOG_LEVEL_INFO); 
		}
    createRuntimeObjectsforASECCONFIGCLASS("SSHCBC",(string)cbc_state);
    omHandler.Finalize();
}

//------------------------------------------------------------------------------------
bool ACS_ASEC_RuntimeHandler::createRuntimeObjectsforASECCONFIGCLASS(string protocol,string status)
{
        ACE_INT32 numAttr = 3;
        vector<ACS_CC_ValuesDefinitionType> AttrList;
	char buff[100]={'\0'};

        //syslog(LOG_INFO,"csadmservice:createRuntimeObjectsforASECCONFIGCLASS: In createRuntimeObjectforASECCONFIGCLASS function\n");

        log.Write("ACS_ASEC_RuntimeHandler:createRuntimeObjectsforASECCONFIGCLASS() - Enter",LOG_LEVEL_INFO);
        ACS_CC_ValuesDefinitionType *attributes = new ACS_CC_ValuesDefinitionType[numAttr];

        string ASEC_RDN = "asecConfigdataId";
        string ASEC_DN_VAL = "asecConfigdataId=";
        ASEC_DN_VAL.append(protocol);

        char asec_rdn[LINE_SIZE];
        //syslog(LOG_INFO,"ASEC_RDN value = [%s]",ASEC_RDN.c_str());
        sprintf(buff,"ASEC_RDN value = [%s]",ASEC_RDN.c_str());
        log.Write(buff,LOG_LEVEL_INFO);

        //syslog(LOG_INFO,"length of ASEC RDN = [%d]",(int)strlen(ASEC_RDN.c_str()));
        sprintf(buff,"length of ASEC RDN = [%d]",(int)strlen(ASEC_RDN.c_str()));
        log.Write(buff,LOG_LEVEL_INFO);

        strncpy(asec_rdn,ASEC_RDN.c_str(),(int)strlen(ASEC_RDN.c_str()));
        asec_rdn[strlen(ASEC_RDN.c_str())] = '\0';

        char asec_dn_val[LINE_SIZE];
        strncpy(asec_dn_val,ASEC_DN_VAL.c_str(),strlen(ASEC_DN_VAL.c_str()));
        asec_dn_val[strlen(ASEC_DN_VAL.c_str())] = '\0';

        char Protocol[LINE_SIZE];
        strncpy(Protocol,protocol.c_str(),strlen(protocol.c_str()));
        Protocol[strlen(protocol.c_str())] = '\0';

        char Status[LINE_SIZE];
        strncpy(Status,status.c_str(),strlen(status.c_str()));
        Status[strlen(status.c_str())] = '\0';

        char ATTR1[] = "protocolName";
        char ATTR2[] = "state";


        //Fill the protocol_name Attribute

        attributes[0].attrName = asec_rdn;
        attributes[0].attrType = ATTR_STRINGT;
        attributes[0].attrValuesNum = 1;
        void* valueRDN[1]={reinterpret_cast<void*>(asec_dn_val)};
        attributes[0].attrValues = valueRDN;
        AttrList.push_back(attributes[0]);

        attributes[1].attrName = ATTR1;
        attributes[1].attrType = ATTR_STRINGT;
        attributes[1].attrValuesNum = 1;
        void* valueProtocol[1]={reinterpret_cast<void*>(Protocol)};
        attributes[1].attrValues = valueProtocol;
        AttrList.push_back(attributes[1]);


        attributes[2].attrName = ATTR2;
        attributes[2].attrType = ATTR_STRINGT;
        attributes[2].attrValuesNum = 1;
        void* valueState[1]={reinterpret_cast<void*>(Status)};
        attributes[2].attrValues = valueState;//valueString1;
        AttrList.push_back(attributes[2]);

        //need to check if that csadmOiImplementerect already exists

        bool myObjectAvailable = false;
        bool ret_status = true;

        string dn("asecConfigdataId=");
        dn.append(protocol);


        bool myIMMResult = isObjectExistsASECCONFIGCLASS(dn,myObjectAvailable);
        sprintf(buff, "ACS_ASEC_RuntimeHandler:createRuntimeObjectsforASECCONFIGCLASS() - myIMMResult[%d]",myIMMResult);
        log.Write(buff, LOG_LEVEL_INFO);

        OmHandler omHandler;
        if(omHandler.Init()==ACS_CC_SUCCESS){
		log.Write("ACS_ASEC_RuntimeHandler:createRuntimeObjectsforASECCONFIGCLASS() - omHandler.Init Sucess", LOG_LEVEL_INFO);
        }
       	else
       	{
		log.Write("ACS_ASEC_RuntimeHandler:createRuntimeObjectsforASECCONFIGCLASS() - omHandler.Init Failed", LOG_LEVEL_ERROR);
		return false;
	}

        if(myIMMResult)
        {
                //in modification block, call imm modify method.

                if(myObjectAvailable){

                        ACS_CC_ImmParameter parToModify;
                        char DnName[150];
                        ACE_OS::strcpy(DnName,dn.c_str());
                        ACE_OS::strcat(DnName,",");
                        ACE_OS::strcat(DnName,theASECInfoParentName);


                        parToModify.attrName = (char *)"state";
                        parToModify.attrType = ATTR_STRINGT;
                        parToModify.attrValuesNum = 1;
                        void* new_value[1] = {reinterpret_cast<void*>(Status)};
                        parToModify.attrValues = new_value;

                        if(omHandler.modifyAttribute(DnName,&parToModify) == ACS_CC_FAILURE) {
				log.Write("ACS_ASEC_RuntimeHandler:createRuntimeObjectsforASECCONFIGCLASS() - modifyAttribute", LOG_LEVEL_INFO);
                                ret_status = false;
                        }
                }
                else
                {
#if 0
                        for ( int i=0 ; i<3 ;i++) {
				ACS_CC_ValuesDefinitionType tempValDefType;
				tempValDefType = AttrList[i];
				std::string myStr((reinterpret_cast<char*>(*(tempValDefType.attrValues))));
			}
#endif

                        if (omHandler.createObject(theASECInfoClassName,theASECInfoParentName,AttrList ) == ACS_CC_FAILURE) {
                                log.Write("failed to create the object",LOG_LEVEL_ERROR);
                                ret_status = false;
                        }

                }
        }
        AttrList.clear();
        delete[] attributes;
        omHandler.Finalize();
        return ret_status;
}

//------------------------------------------------------------------------------------
bool ACS_ASEC_RuntimeHandler::deleteRuntimeObject(const char * dnName)
{
        ACS_CC_ReturnType returnCode;
        char DnName[150];
        char ASEC_parent_name[] = "acsSecurityMId=1";
	char buff[100];

        ACE_OS::strcpy(DnName,dnName);
        ACE_OS::strcat(DnName,",");
        ACE_OS::strcat(DnName,ASEC_parent_name);

        syslog(LOG_INFO,"ACS_ASEC_RuntimeHandler:deleteRuntimeObject() - DN[%s]\n",DnName);
        sprintf(buff,"ACS_ASEC_RuntimeHandler:deleteRuntimeObject() - DN[%s]\n",DnName);
        log.Write(buff,LOG_LEVEL_INFO);

	OmHandler omHandler;
        if(omHandler.Init()==ACS_CC_SUCCESS){
		log.Write("ACS_ASEC_RuntimeHandler::deleteRuntimeObject - omHandler.Init Sucess", LOG_LEVEL_INFO);
        }
       	else {
		log.Write("ACS_ASEC_RuntimeHandler::deleteRuntimeObject - omHandler.Init Failed", LOG_LEVEL_ERROR);
		return false;
	}

        returnCode = omHandler.deleteObject(DnName);

        if(returnCode == ACS_CC_SUCCESS) {
		omHandler.Finalize();
                return true;
        }
        else {
		log.Write("ACS_ASEC_RuntimeHandler::deleteRuntimeObject - deleteObject Failed", LOG_LEVEL_ERROR);
		omHandler.Finalize();
                return false;
        }

}

//------------------------------------------------------------------------------------
bool ACS_ASEC_RuntimeHandler::isObjectExistsASECCONFIGCLASS(std::string& aObjectRdn , bool& isObjectAvailable)
{

        log.Write("ACS_ASEC_RuntimeHandler:isObjectExistsASECCONFIGCLASS() - Enter", LOG_LEVEL_INFO);

        char DnName[150];
        bool myResult = true;
        ACS_CC_ImmParameter paramToFind;
        paramToFind.attrName = new char[30];
        char ASEC_parent_name[] = "acsSecurityMId=1";
        char RDNattr[] = "asecConfigdataId";
        ACE_OS::strcpy(DnName,aObjectRdn.c_str());
        ACE_OS::strcat(DnName,",");
        ACE_OS::strcat(DnName,ASEC_parent_name);
        paramToFind.attrName = RDNattr ;

	OmHandler omHandler;
        if(omHandler.Init()==ACS_CC_SUCCESS){
		log.Write("ACS_ASEC_RuntimeHandler::isObjectExistsASECCONFIGCLASS - omHandler.Init Sucess", LOG_LEVEL_INFO);
        }
       	else {
		log.Write("ACS_ASEC_RuntimeHandler::isObjectExistsASECCONFIGCLASS - omHandler.Init Failed", LOG_LEVEL_ERROR);
		return false;
	}


        if (omHandler.getAttribute(DnName, &paramToFind ) == ACS_CC_FAILURE)
        {
                isObjectAvailable = false;
        }
        else
        {
                isObjectAvailable = true;
        }
	char buff[150];
	sprintf(buff, "ACS_ASEC_RuntimeHandler:isObjectExistsASECCONFIGCLASS() - isObjectAvailable[%d]",isObjectAvailable);	
        log.Write(buff, LOG_LEVEL_INFO);

        if (omHandler.Finalize() == ACS_CC_FAILURE) {
                log.Write("ACS_ASEC_RuntimeHandler:isObjectExistsASECCONFIGCLASS() - Finalize FAILED", LOG_LEVEL_ERROR);
                myResult = false;
        }

        log.Write("ACS_ASEC_RuntimeHandler:isObjectExistsASECCONFIGCLASS() - Leave", LOG_LEVEL_INFO);

        return myResult;
}

//------------------------------------------------------------------------------------
int ACS_ASEC_RuntimeHandler::checkTeamingStatus()
{
        int teamingStatus = -1;
        OmHandler omManager;
        acs_apgcc_paramhandling pha;
        ACS_CC_ReturnType result;
        char buff[250];

        // Try initialize imm connection to IMM
        result = omManager.Init();

        if (result != ACS_CC_SUCCESS)
        {
                sprintf(buff,"Call 'OmHandler Init' failed: cannot get the class name!");
                log.Write(buff, LOG_LEVEL_ERROR);
                return teamingStatus;
        }

        char northBoundClassName[] = "NorthBound";
        std::vector<std::string> p_dnList;
        result = omManager.getClassInstances(northBoundClassName, p_dnList);
        if (result != ACS_CC_SUCCESS || (p_dnList.size() != 1) ) 
        {
                // Exiting on error either an error occurred or more than one instance found
                sprintf(buff, "Call 'getClassInstances' failed: cannot get the class name!");
                log.Write(buff, LOG_LEVEL_ERROR);
                omManager.Finalize();
                return teamingStatus;
        }

        string classInstanceName;
        classInstanceName = p_dnList[0];
        omManager.Finalize();
        char attribName[] = "teamingStatus";

        ACS_CC_ReturnType imm_result = pha.getParameter(classInstanceName.c_str(), attribName, &teamingStatus);
        if (imm_result != ACS_CC_SUCCESS) 
        {
                // ERROR: getting parameter value from IMM
                sprintf(buff, "Call 'getParameter' failed: cannot retrieve the teaming status parameter from IMM: "
                "internal last error == %d: internal last error text == '%s'", pha.getInternalLastError(), pha.getInternalLastErrorText());
                log.Write(buff, LOG_LEVEL_ERROR);
                return -1;
        }

        // TODO: remove this line
        sprintf(buff, "teaming status: %d", teamingStatus);
        log.Write(buff, LOG_LEVEL_ERROR);
        return teamingStatus;
}

//------------------------------------------------------------------------------------
int ACS_ASEC_RuntimeHandler::apgOamAccess()
{
	int apgOamAccess = -1;
	OmHandler omManager;
 	acs_apgcc_paramhandling pha;
 	ACS_CC_ReturnType result;
 	char buff[250];

 	// Try initialize imm connection to IMM
 	result = omManager.Init();
 	if (result != ACS_CC_SUCCESS)
 	{
		sprintf(buff, "ACS_ASEC_RuntimeHandler::apgOamAccess: 'omHandler.Init()' failed");
    log.Write(buff, LOG_LEVEL_ERROR);
    return apgOamAccess;
 	}

 	char axeFunctionsClassName[] = "AxeFunctions";
 	std::vector<std::string> p_dnList;
 	result = omManager.getClassInstances(axeFunctionsClassName, p_dnList);
 	if (result != ACS_CC_SUCCESS || (p_dnList.size() != 1) ) 
 	{
		// Exiting on error either an error occurred or more than one instance found
    sprintf(buff, "ACS_ASEC_RuntimeHandler::apgOamAccess: omManager.getClassInstances() failed");
    log.Write(buff, LOG_LEVEL_ERROR);
    omManager.Finalize();
    return apgOamAccess;
 	}

 	string classInstanceName;
 	classInstanceName = p_dnList[0];
 	omManager.Finalize();
 	char attribName[] = "apgOamAccess";

 	ACS_CC_ReturnType imm_result = pha.getParameter(classInstanceName.c_str(), attribName, &apgOamAccess);
 	if (imm_result != ACS_CC_SUCCESS) 
 	{
  	 // ERROR: getting parameter value from IMM
     sprintf(buff, "Call 'getParameter' failed: cannot retrieve the teaming status parameter from IMM: "
     "internal last error == %d: internal last error text == '%s'", pha.getInternalLastError(), pha.getInternalLastErrorText());
     log.Write(buff, LOG_LEVEL_ERROR);
     return -1;
  }

 	sprintf(buff, "apgOamAccess: %d", apgOamAccess);
 	log.Write(buff, LOG_LEVEL_ERROR);
 	return apgOamAccess;
}

//------------------------------------------------------------------------------------
int ACS_ASEC_RuntimeHandler::isvAPG()
{
	int apgShelfArchitecture = -1;
	OmHandler omManager;
 	acs_apgcc_paramhandling pha;
 	ACS_CC_ReturnType result;
 	char buff[250];
	const int TRUE=1;
	const int FALSE=0;

 	// Try initialize imm connection to IMM
 	result = omManager.Init();
 	if (result != ACS_CC_SUCCESS)
 	{
		sprintf(buff, "ACS_ASEC_RuntimeHandler::isvAPG: 'omHandler.Init()' failed");
    log.Write(buff, LOG_LEVEL_ERROR);
    return apgShelfArchitecture;
 	}

 	char axeFunctionsClassName[] = "AxeFunctions";
 	std::vector<std::string> p_dnList;
 	result = omManager.getClassInstances(axeFunctionsClassName, p_dnList);
 	if (result != ACS_CC_SUCCESS || (p_dnList.size() != 1) ) 
 	{
		// Exiting on error either an error occurred or more than one instance found
    sprintf(buff, "ACS_ASEC_RuntimeHandler::isvAPG: omManager.getClassInstances() failed");
    log.Write(buff, LOG_LEVEL_ERROR);
    omManager.Finalize();
    return apgShelfArchitecture;
 	}

 	string classInstanceName;
 	classInstanceName = p_dnList[0];
 	omManager.Finalize();
 	char attribName[] = "apgShelfArchitecture";

 	ACS_CC_ReturnType imm_result = pha.getParameter(classInstanceName.c_str(), attribName, &apgShelfArchitecture);
 	if (imm_result != ACS_CC_SUCCESS) 
 	{
  	 // ERROR: getting parameter value from IMM
     sprintf(buff, "Call 'getParameter' failed: cannot retrieve the teaming status parameter from IMM: "
     "internal last error == %d: internal last error text == '%s'", pha.getInternalLastError(), pha.getInternalLastErrorText());
     log.Write(buff, LOG_LEVEL_ERROR);
     return -1;
  }

 	sprintf(buff, "apgShelfArchitecture: %d", apgShelfArchitecture);
 	log.Write(buff, LOG_LEVEL_ERROR);
	if (apgShelfArchitecture == 3){
		return TRUE;
	}	
 	return FALSE;
}

//------------------------------------------------------------------------------------
