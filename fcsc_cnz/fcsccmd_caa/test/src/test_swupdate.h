
#include "fcsc_Common.h"
#include "fcsc_CompatibilityChecker.h"
#include "fcsc_DownloadMgr.h"
#include "fcsc_RepositoryMgr.h"

#ifndef FCSC_PREPARE_H
#include "fcscPrepare.h"
#endif

#include<iostream>
using namespace std;

class TestSuite
{
	public:
		// test functions for software compatibility
		static void testSoftwareCompatibility();
		static void testCampaignVerification();

		// test function for SDP integrity check
		static void SDPCheckSumFileCreation();

	        //fcsc_DownloadMgr
	     	static void testPrintDownloadPath();
	     	static void testCleanDownloadFolder();

        	//APG_RepositoryMgr
	      static void testGet_SrcSdpList();
	      static void testCpTo_APGRepository();

	        //CoreMW_RepositoryMgr
	     static void testCoreMW_Cleanup();
	     static void testCoreMW_Import();

		// fcscPrepare
		static void testFcscprepare();

		static void testfcscPrepareWoOption();
		static void testcheckIntegrityCompatibility();

		// creation and delation for static objects
		static void init();
		static void destroy();

	private:
		static SDP_IntegrityCheck*   sdpIntegrityChker;
		static Sw_Compatibility_Chk* swCompatiblityChker;
		static fcsc_DownloadMgr* fcscDownLoadMgr;
		static APG_RepositoryMgr* APGRepositoryMgr;
		static CoreMW_RepositoryMgr* CoreMWRepositoryMgr;
};





