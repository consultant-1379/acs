/*=================================================================== */
/**
   @file  acs_rtr_criticalsectionguard.h 

   @brief Header file for rtr module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       23/01/2013   XHARBAV   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef _ACS_RTR_CRITICAL_SECTION_GUARD_H_
#define _ACS_RTR_CRITICAL_SECTION_GUARD_H_
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/Recursive_Thread_Mutex.h>
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACS_RTR_CriticalSectionGuard
*/
/*=================================================================== */
class ACS_RTR_CriticalSectionGuard {
/*=====================================================================
                          PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                        CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================*/
/**
      @brief     Constructor for ACS_RTR_CriticalSectionGuard class.

      @param     cs
                  Object of ACE_Recursive_Thread_Mutex class.
*/
/*=================================================================== */
	inline explicit ACS_RTR_CriticalSectionGuard(ACE_Recursive_Thread_Mutex& cs) : _cs(cs), _canLeave(false) 
	{
		if (&_cs) 
		{
			try 
			{
				/*EnterCriticalSection(&_cs);*/
				_cs.acquire(); 
				_canLeave = true; 
			}
			catch (...) {}
		}
	}
/*=====================================================================
                          PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=====================================================================
                        CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================*/
/**
      @brief     Constructor for ACS_RTR_CriticalSectionGuard class.

      @param     rhs
                  Object of ACS_RTR_CriticalSectionGuard.
*/
/*=================================================================== */
	inline ACS_RTR_CriticalSectionGuard(const ACS_RTR_CriticalSectionGuard & rhs) : _cs(rhs._cs), _canLeave(false) 
	{
		if (&_cs)
		{
			try 
			{
				//EnterCriticalSection(&_cs); 
				_cs.acquire();
				_canLeave = true; 
			}
			catch (...) {}
		}
	}
/*=====================================================================
                          PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                        CLASS DESTRUCTOR
==================================================================== */
/*===================================================================*/
/**
      @brief     Destructor for ACS_RTR_CriticalSectionGuard class.
*/
/*=================================================================== */
	inline ~ACS_RTR_CriticalSectionGuard()
	{
		try 
		{
			if (_canLeave)
			{
				//LeaveCriticalSection(&_cs); 
				_cs.release();
			}
		} catch (...) {}
	}
/*=====================================================================
                          PRIVATE DECLARATION SECTION
==================================================================== */
private:
	/*inline ACS_RTR_CriticalSectionGuard & operator=(const ACS_RTR_CriticalSectionGuard & rhs) {
		if (this != &rhs) {
			try { if (_canLeave) LeaveCriticalSection(_cs); } catch (...) {}
			_canLeave = false;
			if (_cs = rhs._cs) try { EnterCriticalSection(_cs); _canLeave = true; } catch (...) {}
		}
		return *this;
	}*/

	//Fields
	//CRITICAL_SECTION& _cs;		//for TRs HK91566,HK82323
/*===================================================================*/
/**
      @brief     _cs
*/
/*=================================================================== */
	ACE_Recursive_Thread_Mutex& _cs;
/*===================================================================*/
/**
      @brief     _canLeave
*/
/*=================================================================== */
	bool _canLeave;
};

#endif //_ACS_RTR_CRITICAL_SECTION_GUARD_H_
