/*=================================================================== */
   /**
   @file fcscPrepare.cpp

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
==================================================================== */
#ifndef FCSC_PREPARE_H
#include "fcscPrepare.h"
#endif
/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: parseUrl
=================================================================== */
fcsc_DownloadMgr parseUrl(ACE_TCHAR *url)
{

	if (!strncasecmp(url, "ftp", 3))
	{
                ACE_TCHAR protocol[] = "ftp";
		fcsc_DownloadMgr dmgr(protocol);
		return dmgr;
	}
	else if(!strncasecmp(url, "sftp", 4))
	{
                ACE_TCHAR protocol[] = "sftp";
		fcsc_DownloadMgr dmgr(protocol);
		return dmgr;
	}
	fcsc_DownloadMgr dmgr;
	return dmgr;
}
/* end parseUrl */

/*===================================================================
   ROUTINE: checkIntegrityCompatibility
=================================================================== */
ACE_INT32 checkIntegrityCompatibility()
{
	//Extract from PCFBundle.tar
	 fcsc_Config cfg;
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
        cout << "Campaign Verification: OK" << endl;
        //check S/W compatibility
 	if (sw_comp_chker.wildCardCompare() != 0)
 	{
 	     cout << "Software level not Compatible with current version " << endl;//TODO: trautil trace
 	   	 return -1;
 	}
        cout << "Software Compatibility: OK" << endl;

	//Check F/W compatibility
        return 0;
}
/* end checkIntegrityCompatibility */

/*===================================================================
   ROUTINE: fcscPrepareWoOption
=================================================================== */

ACE_INT32 fcscPrepareWoOption()
{
		
    fcsc_Config cfg;
    string cmd;				
    vector<SdpRecord> updatedSdps;
    PCF_Util upd;
    upd.getUpdatedSDPList(updatedSdps);
    //cout << "********* updatedSdps.size() : " << updatedSdps.size() << endl;
    if(updatedSdps.size() == 0)
    {
        cout <<"No updates found; Software level is up-to-date" << endl;                return 0;
    }
	
    //SDP integrity Check
    SDP_IntegrityCheck sdp_int;
    if (sdp_int.checkSDPIntegrity() != 0)
    {
         cout << "The downloaded packages are corrupted."<< endl;//TODO : trautil
         cout << "Plz execute fcsctransfer once again" << endl;//TODO : trautil
         return -1;
    }


    //cp downloaded SDPs to APG repository/updated_CSP
    string src = fcsc_Config::FCSC_DOWNLOAD_PATH;
    string dest = fcsc_Config::FCSC_APG_REPOSITORY_PATH + fcsc_Config::FCSC_APG_UPDCSP_DIR;
    APG_RepositoryMgr dwnld_sdp_mgr(updatedSdps);
    //cout << "src: " << src << endl;
    //cout << "dest: " << dest << endl;

    if (dwnld_sdp_mgr.cpTo_APGRepository(src, dest) == -1)
        return -1;

    //cp SDPs from current_CSP to APG repository Updated_CSP
    string file_path = fcsc_Config::FCSC_APG_REPOSITORY_PATH + fcsc_Config::FCSC_APG_CURCSP_DIR + fcsc_Config::FCSC_PCF_FILE_NAME;
    cout << "file_path: " << file_path << endl;
    APG_RepositoryMgr repo_mgr(file_path.c_str());

    repo_mgr.get_SrcSdpList();
    src.clear();
   //cout << "APG repo dest: " << dest << endl;
    src = fcsc_Config::FCSC_APG_REPOSITORY_PATH + fcsc_Config::FCSC_APG_CURCSP_DIR;
    //cout << "APG repo src: " << src << endl;
    if (repo_mgr.cpTo_APGRepository(src, dest) == -1)
      	return -1;

    //coreMW cleanup & Import
    CoreMW_RepositoryMgr coremw_mgr;
    cmd.clear();
    cmd = string("cmw-repository-list | awk '{if( $2 == \"NotUsed\") print \"cmw-sdp-remove \"$1}'| sh; cmw-repository-list --campaign | awk '{print \"cmw-sdp-remove \"$1}' | sh");
    if (cfg.exec_cmd(cmd) != 0)
    {
        return -1;
    }
    if (coremw_mgr.CoreMW_Import() == -1)
       	return -1;

    cmd.clear();
    cmd = string("bash ./CoreMW_Import");
    if (cfg.exec_cmd(cmd) != 0)
    {
        return -1;
    }	
    return 0;
}
/* end fcscPrepareWoOption */

/*===================================================================
   ROUTINE: setEnvVariables
=================================================================== */
ACE_INT32 setEnvVariables()
{
	fcsc_Config cfg;
    string prot_value;
    if (cfg.readFromCfgFile("protocol",prot_value) == -1)
    	return -1;
    //cout<<"protocol "<<prot_value<<endl;
    fcsc_DownloadMgr dwnlMgr((ACE_TCHAR*)prot_value.c_str());
    return 0;
}
/* end setEnvVariables */


/*
 *Format1 for PULL with OSS: fcscprepare -u<URL> -uid<user id> -pwd<password> PULL
 *Format2 for PUSH without OSS: fcscprepare PUSH
 *Format3 for PULL without OSS: fcscprepare -d <PATH> PULL wo OSS
*/

/*===================================================================
   ROUTINE: Usage
=================================================================== */
void Usage()
{
	 cout << "Syntax Error :"<< endl;
	 cout << "Usage :"<< endl;
	 cout << "Format1 for PULL with OSS: fcscprepare -u <URL> -uid <User id> -pwd <Password>"<<endl;
	 cout << "Format2 for PUSH without OSS: fcscprepare" << endl;
	 cout << "Format3 for PULL without OSS: fcscprepare -d <disk path for SDPs>" << endl;
}
/* end Usage */

/*===================================================================
   ROUTINE: ACE_TMAIN
=================================================================== */
ACE_INT32 ACE_TMAIN(ACE_INT32 argc, ACE_TCHAR *argv[])
{
    if (!(argc ==1 || argc == 3 || argc == 7))
    {
    	Usage();
    	cout << "1" <<endl;
    	return -1;
    }

    if (argc == 3 && strcasecmp(argv[1], "-d"))
    {
    	Usage();
        //cout << "3" <<endl;
    	return -1;
    }

    if (argc == 7 && strcasecmp(argv[1], "-u") && strcasecmp(argv[3], "-uid")
    	&& strcasecmp(argv[5], "-pwd"))
    {
    	Usage();
        //cout << "7" <<endl;
    	return -1;
    }

    switch(argc)
    {
      case 1:
      {
    	if (setEnvVariables() == -1)
    	{
    		cout << "Can't read Config file. Plz execute fscstransfer before this command" << endl;//TODO : trautil
    		return -1;
    	}
    	if (fcscPrepareWoOption() == -1)
    	    return -1;
    	break;
      }
      case 3: //fcscprepare -d <Path>
      {
    	//Copy PCFBundle.tar to download directory
        ACE_TCHAR protocol[] = "SFTP";
    	fcsc_DownloadMgr dm(protocol);
    	dm.cleanDownloadFolder();
    	vector<SdpRecord> updatedSdps;
    	SdpRecord rec;
    	rec.sdpName = fcsc_Config::FCSC_PCF_BUNDLE_NAME;
    	updatedSdps.push_back(rec);
    	APG_RepositoryMgr cp_pcf_bundle(updatedSdps, false);
    	if (cp_pcf_bundle.cpTo_APGRepository(argv[2], fcsc_Config::FCSC_DOWNLOAD_PATH) == -1)
    	    return -1;
    	if (checkIntegrityCompatibility() == -1)
    	    return -1;
    	updatedSdps.clear();
    	PCF_Util upd;
    	upd.getUpdatedSDPList(updatedSdps);
    	if(updatedSdps.size() == 0)
    	{
    		cout <<"No updates found; Software level is up-to-date" << endl;
    		return 0;
    	}

    	//Check free space
    	string newPCFpath = fcsc_Config::FCSC_DOWNLOAD_PATH + fcsc_Config::FCSC_PCF_FILE_NAME;
    	APG_RepositoryMgr spaceChk((const ACE_TCHAR*) newPCFpath.c_str());
    	if (spaceChk.checkFreeSpace(updatedSdps) == false)
    	{
    		cout <<"There is not enough free space. Free some disk space and try again" <<endl;
    	    return -1;
    	}

    	//copy updated SDPs from -d path to download folder
    	APG_RepositoryMgr cp_sdp_mgr(updatedSdps, false);
    	if (cp_sdp_mgr.cpTo_APGRepository(argv[2], fcsc_Config::FCSC_DOWNLOAD_PATH) == -1)
    	{
    	    cout << "file not copied"; return -1;
    	}

    	if (fcscPrepareWoOption() == -1)
    		return -1;
    	break;
      }
      case 7:
      {
    	//parse -u url & set download path
    	fcsc_DownloadMgr dmgr = parseUrl(argv[2]);
    	if(fcsc_Config::FCSC_DOWNLOAD_PATH == "")
    		return -1;
    	//dharm's code to download PCFBundle.tar
    	SdpRecord PCFrec;
    	PCFrec.sdpName="PCFBundle.tar";
    	vector<SdpRecord>files;
    	files.push_back(PCFrec);

    	dmgr.cleanDownloadFolder();
    	dmgr.download(string(argv[2]),string(argv[4]),string(argv[6]),files);
    	if (checkIntegrityCompatibility() == -1)
    		return -1;

    	PCF_Util update;
    	vector<SdpRecord> updatedSdps;
    	update.getUpdatedSDPList(updatedSdps);
    	//cout << "********* updatedSdps.size() : " << updatedSdps.size() << endl;
    	if(updatedSdps.size() == 0)
    	{
    		cout <<"No updates found; Software level is up-to-date" << endl;
    		return 0;
    	}
    	string newPCFpath = fcsc_Config::FCSC_DOWNLOAD_PATH + fcsc_Config::FCSC_PCF_FILE_NAME;
    	APG_RepositoryMgr spaceChk((const ACE_TCHAR*) newPCFpath.c_str());
    	if (spaceChk.checkFreeSpace(updatedSdps) == false)
    	{
    		cout <<"There is not enough free space. Free some disk space and try again" <<endl;
    		return -1;
    	}
    	//dharm's code to download SDPs
    	dmgr.download(string(argv[2]),string(argv[4]),string(argv[6]),updatedSdps);

    	if (fcscPrepareWoOption() == -1)
    	    return -1;
      }
   }

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
