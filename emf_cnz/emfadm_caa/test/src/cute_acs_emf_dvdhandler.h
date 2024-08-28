#include "acs_emf_dvdhandler.h"
#include "acs_emf_param.h"
#include <acs_emf_defs.h>
#include <acs_emf_mediahandler.h>
#include<iostream>
using namespace std;

class TestSuite
{
	public:
		// test function for SDP integrity check
		static void DVDOperationCheckForFromDVD();
		static void DVDOperationCheckForToDVD();
		static void DVDOperationCheckForMediaInfo();
		static void DVDOperationCheckForOwnerInfo();
		static void DVDOperationCheckForFromDVDToCpAllData();
		static void DVDOperationCheckForFromDVDToCpSpecData();
		// creation and delation for static objects
		static void init();
		static void destroy();

	private:
		//static ACS_EMF_DVDHandler* acs_emf_dvdhandler;
		static ACS_EMF_MEDIAHandler* acs_emf_dvdhandler;

};





