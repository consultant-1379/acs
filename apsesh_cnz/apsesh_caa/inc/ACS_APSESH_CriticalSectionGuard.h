//	*****************************************************************************
//
//	.NAME
//	    ACS_APSESH_CriticalSectionGuard
//	.LIBRARY 3C++
//	.PAGENAME ACS_APSESH
//	.HEADER  ACS_APSESH
//
//	.COPYRIGHT
//	 COPYRIGHT Ericsson 2009.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2009.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2009 or in
//	accordance with the terms and conditions stipulated in the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	.DESCRIPTION
//	   See below
//
//	AUTHOR
//	   enatmeu
//
//	*****************************************************************************


#ifndef ACS_APSESH_CriticalSectionGuard_h
#define ACS_APSESH_CriticalSectionGuard_h 1

// #include <winsock2.h>
#include "ace/Mutex.h"

class ACS_APSESH_CriticalSectionGuard 
{
  public:
      explicit ACS_APSESH_CriticalSectionGuard (ACE_Mutex *cs);

      virtual ~ACS_APSESH_CriticalSectionGuard();

  private:
       //CRITICAL_SECTION *m_cs;
       ACE_Mutex *m_cs;

       bool m_canLeave;

};

inline ACS_APSESH_CriticalSectionGuard::ACS_APSESH_CriticalSectionGuard (ACE_Mutex *cs)
  :m_cs(cs), m_canLeave(false)
{
	//if (m_cs) try { EnterCriticalSection(m_cs); m_canLeave = true; } catch (...) {}
	if (m_cs) try { m_cs->acquire(); m_canLeave = true; } catch (...) {}
}


inline ACS_APSESH_CriticalSectionGuard::~ACS_APSESH_CriticalSectionGuard()
{

	//try { if (m_canLeave) LeaveCriticalSection(m_cs); } catch (...) {}
	try { if (m_canLeave) m_cs->release(); } catch (...) {}
}

#endif
