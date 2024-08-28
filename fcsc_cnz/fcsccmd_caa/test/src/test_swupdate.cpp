



#include "test_swupdate.h"
#include "fcscPrepare.h"

SDP_IntegrityCheck*   TestSuite::sdpIntegrityChker = NULL;
Sw_Compatibility_Chk* TestSuite::swCompatiblityChker = NULL;
fcsc_DownloadMgr* TestSuite::fcscDownLoadMgr = NULL;
APG_RepositoryMgr* TestSuite::APGRepositoryMgr = NULL;
CoreMW_RepositoryMgr* TestSuite::CoreMWRepositoryMgr = NULL;

//extern int fcscPrepare();

void TestSuite::init()
{
	ACE_TCHAR protocol[] = "ftp";
	fcscDownLoadMgr = new fcsc_DownloadMgr(protocol);

	fcsc_Config cfg;
		string dummycmd = string("tar -xvf ") + fcsc_Config::FCSC_DOWNLOAD_PATH + string("PCFBundle.tar -C ")
						   + fcsc_Config::FCSC_DOWNLOAD_PATH;
				   //cout << "Executing command: " << dummycmd << endl;
				   if (cfg.exec_cmd(dummycmd) != 0)
				   {
					  cout << "Can't extract package names" << endl;
					  return;
				   }

	sdpIntegrityChker   = new SDP_IntegrityCheck();
	swCompatiblityChker = new Sw_Compatibility_Chk();
	string file_path = fcsc_Config::FCSC_FTP_PATH + fcsc_Config::FCSC_DOWNLOAD_DIR_NAME + fcsc_Config::FCSC_PCF_FILE_NAME;
	APGRepositoryMgr = new APG_RepositoryMgr(file_path.c_str());
	CoreMWRepositoryMgr = new CoreMW_RepositoryMgr();

}

void TestSuite::destroy()
{
	delete sdpIntegrityChker;
	delete swCompatiblityChker;
	delete fcscDownLoadMgr;
	delete APGRepositoryMgr;
	delete CoreMWRepositoryMgr;
}

void TestSuite::testSoftwareCompatibility()
{
	if(swCompatiblityChker->wildCardCompare() == 0)
		cout<<"New version is compatible"<<endl;
	else
		cout<<"New Version is not compatible"<<endl;
}

void TestSuite::testCampaignVerification()
{
	if(swCompatiblityChker->verifyCampaign() == 0)
		cout<<"Campiagn exists in CSP"<<endl;
	else
		cout<<"Campaign doesn't exist"<<endl;
}


void TestSuite::SDPCheckSumFileCreation()
{
    if(sdpIntegrityChker->checkSDPIntegrity() == 0)
	cout<<"successfully created"<<endl;
    else
	cout<<"failed in creating the checksum file"<<endl;
}

// fcsc_DownloadMgr
void TestSuite::testPrintDownloadPath()
{
	fcscDownLoadMgr->printDownloadPath();
}

void TestSuite::testCleanDownloadFolder()
{
    	if(fcscDownLoadMgr->cleanDownloadFolder() == 0)
	{
		cout<<"successfully deleted"<<endl;
	}
	else
		cerr<<"failed in CleanDownLoadFolder"<<endl;

}

void TestSuite::testGet_SrcSdpList()
{
	if( APGRepositoryMgr->get_SrcSdpList() == 0)
	{
		cout<<"successfull: testGet_SrcSdpList"<<endl;
	}
	else
		cerr<<"failed in testGet_SrcSdpList" <<endl;
}

void TestSuite::testCpTo_APGRepository()
{

	PCF_Util upd;
		vector<SdpRecord> updatedSdps;
		upd.getUpdatedSDPList(updatedSdps);
		//cout << "********* updatedSdps.size() : " << updatedSdps.size() << endl;
		if(updatedSdps.size() == 0)
		{
			cout <<"No upadates found; Software level is up-to-date" << endl;
			return;
		}

	string src = fcsc_Config::FCSC_DOWNLOAD_PATH;
	string dest = fcsc_Config::FCSC_APG_REPOSITORY_PATH + fcsc_Config::FCSC_APG_UPDCSP_DIR;
	APG_RepositoryMgr dwnld_sdp_mgr(updatedSdps);
	//cout << "src: " << src << endl;
	//cout << "dest: " << dest << endl;
	dwnld_sdp_mgr.cpTo_APGRepository(src, dest);


   //cp SDPs from current_CSP to APG repository Updated_CSP
   string file_path = fcsc_Config::FCSC_APG_REPOSITORY_PATH + fcsc_Config::FCSC_APG_CURCSP_DIR + fcsc_Config::FCSC_PCF_FILE_NAME;
	cout << "file_path: " << file_path << endl;
	APG_RepositoryMgr repo_mgr(file_path.c_str());
	repo_mgr.get_SrcSdpList();


	src.clear();

		//cout << "APG repo dest: " << dest << endl;
	src = fcsc_Config::FCSC_APG_REPOSITORY_PATH + fcsc_Config::FCSC_APG_CURCSP_DIR;
		//cout << "APG repo src: " << src << endl;
	if (repo_mgr.cpTo_APGRepository(src, dest) == 0)

		cout<<"successfull: cpTo_APGRepository"<<endl;
	else
		cerr<<"failed in cpTo_APGRepository" <<endl;


	/*if(APGRepositoryMgr->cpTo_APGRepository(fcsc_Config::FCSC_FTP_PATH,fcsc_Config::FCSC_APG_REPOSITORY_PATH) == 0)
	{
		cout<<"successfull: cpTo_APGRepository"<<endl;
	}
	else
		cerr<<"failed in cpTo_APGRepository" <<endl;*/

}


void TestSuite::testCoreMW_Cleanup()
{
	if(CoreMWRepositoryMgr->CoreMW_Cleanup() == 0)
	{
		cout<<"successfull: testCoreMW_Cleanup"<<endl;
	}
	else
		cerr<<"failed in testCoreMW_Cleanup" <<endl;

}
void TestSuite::testCoreMW_Import()
{
	if(CoreMWRepositoryMgr->CoreMW_Import() == 0)
	{
		cout<<"successfull: testCoreMW_Import"<<endl;
	}
	else
		cerr<<"failed in testCoreMW_Import"<<endl;

}

void TestSuite::testfcscPrepareWoOption()
{
	//manually copy PCFBundle.tar to SFTPVOL/new_CSP b4 this test
	ACE_TCHAR protocol[] = "SFTP";
	fcsc_DownloadMgr dm(protocol);
	if(fcscPrepareWoOption() == 0)
	{
		cout<<" successfull : fcscPrepareWoOption"<<endl;
	}
	else
	{
		cout<<"failed : fcscPrepareWoOption"<<endl;
	}
}
void TestSuite::testcheckIntegrityCompatibility()
{
	//manually copy PCFBundle.tar to SFTPVOL/new_CSP b4 this test
	ACE_TCHAR protocol[] = "SFTP";
	if(checkIntegrityCompatibility()== 0)
	{
		cout<<"successfull : checkIntegrityCompatibility"<<endl;
	}
	else
	{
		cout<<"failed in checkIntegrityCompatibility"<<endl;
	}

}

