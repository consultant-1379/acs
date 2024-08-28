

/*=================================================================== */
   /**
   @file fcscTransfer.cpp

   Class method implementation for SWUpdate module.

   This module contains the implementation of class declared in
   the SWUpdate module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */
#ifndef FCSC_PCF_COMMON_H
#include "fcsc_Common.h"
#endif
#ifndef FCSC_APG_REP_H
#include "fcsc_RepositoryMgr.h"
#endif
#ifndef FCSC_COMP_CHK_H
#include "fcsc_CompatibilityChecker.h"
#endif
#ifndef FCSC_DOWNLOADMGR_H_
#include "fcsc_DownloadMgr.h"
#endif
/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */
#define OSSPATH "/cluster/data/OSS_Source/"

/*
 * Format1 :fcsctransfer -path -protocol FTP|SFTP
 * Format2 :fcsctransfer -list
 * Operator will execute Format1 first & it will print the download path depending on protocol
 * Then operator has to push PCFBundle.tar & certificate downloaded from ericsson site
 * Then operator will execute Format2; it will provide the list of updated SDPs to be pushed
 */
/*===================================================================
   ROUTINE: usage
=================================================================== */
void usage()
{
   cout << "Syntax Error :"<< endl;
   cout << "Usage :"<< endl;
   cout << "Format1: fcsctransfer -path -protocol FTP|SFTP [-list]"<<endl;
   cout << "Format2: fcsctransfer -list" << endl;
}
/* end usage */

/*===================================================================
   ROUTINE: ACE_TMAIN
=================================================================== */

ACE_INT32 ACE_TMAIN(ACE_INT32 argc, ACE_TCHAR *argv[])
{

	//if (argc < 2 || argc > 3)
	if (!(argc == 2 || argc == 4))
	{
		usage();
		return -1;
	}
	//remove & create new_CSP

	if (argc == 4 && (strcasecmp(argv[3], "FTP") != 0 && strcasecmp(argv[3], "SFTP") != 0) )
	{
		usage();
		return -1;
	}
	//cout << argc << "  " << argv[2] << endl;
    if (argc == 2 && strcmp(argv[1], "-list") != 0)
    {
    	usage();
    	return -1;
    }


    //delete & create download directory with appropriate permission
    //init DownloadMgr
    if (argc == 4)
    {
       fcsc_Config cfg;
       fcsc_DownloadMgr dwld_mgr(argv[3]);
       string cmd;
       cmd = string("rm -rf ") + fcsc_Config::FCSC_DOWNLOAD_PATH;
       cfg.exec_cmd(cmd);
       cmd.clear();
       cmd = string("mkdir ") + fcsc_Config::FCSC_DOWNLOAD_PATH + string(";chmod 755 ") + fcsc_Config::FCSC_DOWNLOAD_PATH;
       if (cfg.exec_cmd(cmd) != 0)
       {
    	  cout << "Can't create download folder" << endl;
    	  return -1;
       }
       else
       {
    	   dwld_mgr.printDownloadPath();
    	   cfg.writeToCfgFile(string("protocol"),string(argv[3]));
       }
    }

    //for Format2
    if (argc == 2)
    {
    	fcsc_Config cfg;
    	string prot_value;
    	cfg.readFromCfgFile("protocol",prot_value);
    	cout<<"protocol "<<prot_value<<endl;
    	fcsc_DownloadMgr dwld_mgr((char*)prot_value.c_str());

    	//Extract from PCFBundle.tar
        string cmd;
    	cmd = string("tar -xvf ") + fcsc_Config::FCSC_DOWNLOAD_PATH + string("PCFBundle.tar -C ")
    				   + fcsc_Config::FCSC_DOWNLOAD_PATH;
    	if (cfg.exec_cmd(cmd) != 0)
    	{
    	    cout << "Can't extract package names" << endl;
    		return -1;
    	}
    	//check PCF integrity
    	cmd.clear();
    	cmd = string("openssl verify -CAfile ") + fcsc_Config::FCSC_DOWNLOAD_PATH
    				    + string("CAcert.pem -purpose any ") + fcsc_Config::FCSC_DOWNLOAD_PATH
    				    + string("newcert.pem") + string(" 2>&1");

    	if (cfg.exec_cmd(cmd) != 0)
    	{
    		cout << "Certificate verification failed"<<endl;//TODO: trautil trace
    		return -1;
    	}

    	cmd.clear();
    	cmd = string("openssl x509 -inform PEM -in ") + fcsc_Config::FCSC_DOWNLOAD_PATH
    		  + string("newcert.pem -pubkey -outform PEM -out ")
    		  + fcsc_Config::FCSC_DOWNLOAD_PATH + string("temp > ")
    		  + fcsc_Config::FCSC_DOWNLOAD_PATH + string("pubkey.pem")+ string(" 2>&1");

    	//cout << "Executing command: " << cmd << endl;
    	if (cfg.exec_cmd(cmd) != 0)
    	{
    		cout << "Public key corrupted" << endl;//TODO: trautil trace
    		return -1;
    	}
    	cmd.clear();
    	cmd = string("openssl dgst -verify ") + fcsc_Config::FCSC_DOWNLOAD_PATH
   			  + string("pubkey.pem -signature ") + fcsc_Config::FCSC_DOWNLOAD_PATH + string("PCF.sign  ")
   			  + fcsc_Config::FCSC_DOWNLOAD_PATH + string("PCF.xml") + string(" 2>&1");

   		//cout << "Executing command: " << cmd << endl;
   		if (cfg.exec_cmd(cmd) != 0)
   		{
   			cout << "Public key verification failed" << endl;//TODO: trautil trace
   			return -1;
   		}
   		//verify Campaign
   		Sw_Compatibility_Chk sw_comp_chker;
   	    if (sw_comp_chker.verifyCampaign() != 0)
   		{
   		     cout << "Campaign verification failed. PCF file not correct " << endl;//TODO: trautil trace
   		     return -1;
   		}

   	    //check S/W compatibility
   	 	if (sw_comp_chker.wildCardCompare() != 0)
   	 	{
   	 	     cout << "Software level not Compatible with current version " << endl;//TODO: trautil trace
   	 	   	 return -1;
   	 	}

   	 	//Check F/W compatibility

   	 	//list updated SDPs
    	PCF_Util upd;
    	vector<SdpRecord> updatedSdps;
    	upd.getUpdatedSDPList(updatedSdps);
    	//cout << "********* updatedSdps.size() : " << updatedSdps.size() << endl;
    	if(updatedSdps.size() == 0)
    	{
    		cout <<"No updates found; Software level is up-to-date" << endl;
    		return 0;
    	}
    	ACE_UINT32 i;
    	cout << "List of updated packages to be pushed : " << endl;
    	for (i=0; i < updatedSdps.size(); i++)
    	{
    		cout << updatedSdps[i].sdpName << " " << updatedSdps[i].rState << endl;
    	}
    	string newPCFpath = fcsc_Config::FCSC_DOWNLOAD_PATH + fcsc_Config::FCSC_PCF_FILE_NAME;
   		APG_RepositoryMgr spaceChk((const ACE_TCHAR*) newPCFpath.c_str());
   		if (spaceChk.checkFreeSpace(updatedSdps) == false)
   		{
   			cout <<"There is not enough free space. Free some disk space and try again" <<endl;
   			return -1;
   		}
    }
	//cfg.writeToCfgFile(string("protocol"),string(argv[3]));
    return 0;
}
/* end ACE_TMAIN */

//******************************************************************************

//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2001-2004
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------

