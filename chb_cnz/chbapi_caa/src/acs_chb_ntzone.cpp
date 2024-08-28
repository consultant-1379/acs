/*=================================================================== */
   /**
   @file acs_chb_ntzone.cpp

   Class method implementationn for ACS_CHB_NtZone class.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       25/11/2010     XTANAGG       Initial Release
   **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <acs_chb_ntzone.h>

static const char* tempDebug="ACS_CHB_NTZone_DebugTrace";
static const char* tempError="ACS_CHB_NTZone_ErrorTrace";
static const char* C512="C512";
static ACS_TRA_trace ACS_CHB_NTZone_DebugTrace(tempDebug, C512);
static ACS_TRA_trace ACS_CHB_NTZone_ErrorTrace(tempError, C512);

const char* space = " ";
/*===================================================================
   ROUTINE: ACS_CHB_NtZone
=================================================================== */
ACS_CHB_NtZone::ACS_CHB_NtZone() 
{
	memset(&Tzi,0,sizeof(TIME_ZONE_INFORMATION));	
	ACE_OS::strcpy(str_time_zone, space);
    if( ACS_CHB_NTZone_DebugTrace.ACS_TRA_ON())
    {
        char traceBuffer[256];
        memset(&traceBuffer, 0, sizeof(traceBuffer));
        sprintf(traceBuffer, "%s", "Entering ACS_CHB_NtZone::ACS_CHB_NtZone Constructor");
        ACS_CHB_NTZone_DebugTrace.ACS_TRA_event(1, traceBuffer);
    }
    //DEBUG(0, "%s", "Entering ACS_CHB_NtZone::ACS_CHB_NtZone Constructor");
    if( ACS_CHB_NTZone_DebugTrace.ACS_TRA_ON())
    {
        char traceBuffer[256];
        memset(&traceBuffer, 0, sizeof(traceBuffer));
        sprintf(traceBuffer, "%s", "Leaving ACS_CHB_NtZone::ACS_CHB_NtZone Constructor");
        ACS_CHB_NTZone_DebugTrace.ACS_TRA_event(1, traceBuffer);
    }
  //DEBUG(0, "%s", "Leaving ACS_CHB_NtZone::ACS_CHB_NtZone Constructor");
}

/*===================================================================
   ROUTINE: ACS_CHB_NtZone
=================================================================== */
ACS_CHB_NtZone::ACS_CHB_NtZone(const ACE_TCHAR* s, TIME_ZONE_INFORMATION *TZ) 
{ 
	if( ACS_CHB_NTZone_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_NtZone::ACS_CHB_NtZone Destructor");
		ACS_CHB_NTZone_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	//DEBUG(0, "%s", "Entering ACS_CHB_NtZone::ACS_CHB_NtZone Destructor");
	ACE_OS::strcpy(str_time_zone, s); 
	ACE_OS::memcpy( &Tzi , TZ , sizeof(Tzi) );
	if( ACS_CHB_NTZone_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_NtZone::ACS_CHB_NtZone Destructor");
		ACS_CHB_NTZone_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	//DEBUG(0, "%s", "Leaving ACS_CHB_NtZone::ACS_CHB_NtZone Destructor");
}

/*===================================================================
   ROUTINE: getTZI
=================================================================== */
TIME_ZONE_INFORMATION* ACS_CHB_NtZone::getTZI( )
{
        if( ACS_CHB_NTZone_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[256];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_NtZone::getTZI()");
            ACS_CHB_NTZone_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0, "%s", "Entering ACS_CHB_NtZone::getTZI()");
        if( ACS_CHB_NTZone_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[256];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_NtZone::getTZI()");
            ACS_CHB_NTZone_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0, "%s", "Leaving ACS_CHB_NtZone::getTZI()");
	return &Tzi;
}

/*===================================================================
   ROUTINE: operator=
=================================================================== */
ACS_CHB_NtZone ACS_CHB_NtZone::operator=(const ACS_CHB_NtZone& obj)
{ 
              // copy datastructure.
        if( ACS_CHB_NTZone_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[256];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_NtZone::operator=()");
            ACS_CHB_NTZone_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0, "%s", "Entering ACS_CHB_NtZone::operator=()");
	ACE_OS::strcpy(str_time_zone, obj.str_time_zone);
	ACE_OS::memcpy( &Tzi , &(obj.Tzi) , sizeof(Tzi) );
	    if( ACS_CHB_NTZone_DebugTrace.ACS_TRA_ON())
	    {
	        char traceBuffer[256];
	        memset(&traceBuffer, 0, sizeof(traceBuffer));
	        sprintf(traceBuffer, "%s", "Leaving ACS_CHB_NtZone::operator=()");
	        ACS_CHB_NTZone_DebugTrace.ACS_TRA_event(1, traceBuffer);
	    }
	//DEBUG(0, "%s", "Leaving ACS_CHB_NtZone::operator=()");
	return *this;
} 

/*===================================================================
   ROUTINE: get
=================================================================== */
ACE_TCHAR* ACS_CHB_NtZone::get()
{ 	
        if( ACS_CHB_NTZone_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[256];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Entering ACS_CHB_NtZone::get()");
            ACS_CHB_NTZone_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0, "%s", "Entering ACS_CHB_NtZone::get()");
        if( ACS_CHB_NTZone_DebugTrace.ACS_TRA_ON())
        {
            char traceBuffer[256];
            memset(&traceBuffer, 0, sizeof(traceBuffer));
            sprintf(traceBuffer, "%s", "Leaving ACS_CHB_NtZone::get()");
            ACS_CHB_NTZone_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0, "%s", "Leaving ACS_CHB_NtZone::get()");
	return str_time_zone;
}
