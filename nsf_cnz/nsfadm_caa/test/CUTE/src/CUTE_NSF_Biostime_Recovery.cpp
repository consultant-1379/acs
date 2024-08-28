/**
 * @file CUTE_NSF_Biostime_Recovery.cpp
 * Test functions ACS_NSF_Biostime_Recovery are defined in this file
 */
/* INCLUDE SECTION */
/* inclusion of cute libraries */
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

/* inclusion of NSF Cute test header files */
#include "CUTE_NSF_Biostime_Recovery.h"
#include "acs_nsf_biostime_recovery.h"

/* Inclusion of ACE library files */
#include <ace/ACE.h>
using namespace std;

/*===========================================
 * Declaring Static variables
 * ===========================================*/
NSF_Biostime_Recovery * Cute_NSF_Biostime_Recovery::m_poBiosTime =0;


Cute_NSF_Biostime_Recovery::Cute_NSF_Biostime_Recovery()
{

}


Cute_NSF_Biostime_Recovery::~Cute_NSF_Biostime_Recovery()
{

}
/*=======================================================
 * Routine declaration
 *=======================================================*/

/*========================================================
 *  ROUTINE:Cute_NSF_Biostime_Recovery()
 *========================================================*/

/*========================================================
 *  ROUTINE:vCheckForTimestampFile()
 *========================================================*/

void Cute_NSF_Biostime_Recovery::vCheckForTimestampFile()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      		Check For NSF system time stamp file      		 "<<endl;
	cout<<endl<<"************************************************************"<<endl;
    bool bResult=getPoBiosTime()->CheckForTimestampFile();
    ASSERTM("Time stamp file not existing",bResult==true);

}

/*========================================================
 *  ROUTINE:vGetFileTime()
 *========================================================*/

void Cute_NSF_Biostime_Recovery::vGetFileTime()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"      		Get  the NSF system time stamp file      		 "<<endl;
	cout<<endl<<"************************************************************"<<endl;
    bool bResult=getPoBiosTime()->GetFileTime();
    ASSERTM("Unable to read the time stamp file",bResult==true);

}

/*========================================================
 *  ROUTINE:vCheckForPrivileges()
 *========================================================*/

void Cute_NSF_Biostime_Recovery::vCheckForPrivileges()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"Check for the privileges to adjust NSF system time stamp file"<<endl;
	cout<<endl<<"************************************************************"<<endl;
    bool bResult=getPoBiosTime()->CheckForPrivileges();
    ASSERTM("Insufficient privileges to adjust the system time",bResult==true);

}

/*========================================================
 *  ROUTINE:vCheckCurrentTimeWithFileTime()
 *========================================================*/

void Cute_NSF_Biostime_Recovery::vCheckCurrentTimeWithFileTime()
{
	cout<<endl<<"************************************************************"<<endl;
	cout<<endl<<"				Check the system time with file time		 "<<endl;
	cout<<endl<<"************************************************************"<<endl;
	bool bResult=getPoBiosTime()->CheckCurrentTimeWithFileTime();
	ASSERTM("System is updated, no Power loss",bResult==true);

}

void Cute_NSF_Biostime_Recovery::init()
{
	m_poBiosTime = new NSF_Biostime_Recovery();
}



/*========================================================
 * ROUTINE:make_suite_Cute_NSF_Biostime_Recovery()
 *========================================================*/
cute::suite Cute_NSF_Biostime_Recovery::make_suite_Cute_NSF_Biostime_Recovery(){
	cute::suite s;

	s.push_back(CUTE(vCheckForTimestampFile));
	s.push_back(CUTE(vGetFileTime));
	s.push_back(CUTE(vCheckForPrivileges));
	s.push_back(CUTE(vCheckCurrentTimeWithFileTime));
	s.push_back(CUTE(vBiosTimeCleanup));
	return s;
}


void Cute_NSF_Biostime_Recovery::vBiosTimeCleanup()
{
	delete m_poBiosTime;
	m_poBiosTime = 0;
}

