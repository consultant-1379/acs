/*=================================================================== */
   /**
   @file acs_chb_mtzclass.cpp

   This module contains the implementation of class declared in
   the ACS_CHB_mtzclass.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/11/2010     XKUSATI   Initial Release
   **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <acs_chb_cp_object.h>
#include <acs_chb_mtzclass.h>
#include <acs_chb_mtzconv.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
using namespace std;

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */

static ACS_TRA_trace ACS_CHB_MTZCLASS_DebugTrace("ACS_CHB_MTZCLASS_DebugTrace", "C512");
static ACS_TRA_trace ACS_CHB_MTZCLASS_ErrorTrace("ACS_CHB_MTZCLASS_ErrorTrace", "C512");

ACS_CHB_filehandler   *ACS_CHB_mtzclass::pfilehandler = 0;
ACS_CHB_Threadhandler *ACS_CHB_mtzclass::pthreadhandler = 0;

/*=======================================================================
	ROUTINE: MTZInit
========================================================================*/
bool  ACS_CHB_mtzclass::MTZInit()
{
	if( pfilehandler == 0 )
	{
		pfilehandler   = new ACS_CHB_filehandler();
	}
	if(( pthreadhandler == 0)  && (pfilehandler != 0 ))
	{
		pthreadhandler = new ACS_CHB_Threadhandler(pfilehandler);
	}
	if( pfilehandler == 0 || pthreadhandler == 0 )
	{
		return false;
	}
	return true;
}

/*======================================================================
	ROUTINE: MTZDestroy
========================================================================*/
void ACS_CHB_mtzclass::MTZDestroy()
{
	if( pthreadhandler != 0 )
	{
		delete pthreadhandler;
		pthreadhandler = 0;
	}

	if( pfilehandler != 0 )
	{
		delete pfilehandler;
		pfilehandler = 0;
	}
}



/*===================================================================
   ROUTINE: ACS_CHB_mtzclass
=================================================================== */
ACS_CHB_mtzclass::ACS_CHB_mtzclass()
{
	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzclass Constructor");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	//Initialise CP_time;
	CP_time = 0;
	NTInterface = new ACS_CHB_mtzconv(pfilehandler);
	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzclass Constructor");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
}

/*===================================================================
   ROUTINE: ~ACS_CHB_mtzclass
=================================================================== */
ACS_CHB_mtzclass::~ACS_CHB_mtzclass()
{
	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ~ACS_CHB_mtzclass ");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	delete NTInterface;
	NTInterface = 0;
	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ~ACS_CHB_mtzclass ");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
}

/*===================================================================
   ROUTINE: makeSystemTimeConversion
=================================================================== */
struct tm* ACS_CHB_mtzclass::makeSystemTimeConversion(struct tm* timeToConvert,
                                                      int       daylightStatus,
                                                      int       TMZ)
{
	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzclass::makeSystemTimeConversion()");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
        
//	(void)daylightStatus;
	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzclass::makeSystemTimeConversion()");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	return NTInterface->get_TMZTime(timeToConvert,TMZ,daylightStatus);
	
}
/*===================================================================
   ROUTINE: TimeZoneAlarm
=================================================================== */
int ACS_CHB_mtzclass::TimeZoneAlarm(const int TMZvalue)
{
	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzclass::TimeZoneAlarm");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

	char ZoneMismatch = '0';
	
	if ((TMZvalue < 0) || (TMZvalue >=  MAX_NO_TIME_ZONES) )
	{
		if( ACS_CHB_MTZCLASS_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[256];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "TMZValue is not in the range of [0-23]");
			ACS_CHB_MTZCLASS_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
        return ACS_CHB_CONNECTION_ERROR;
	}

	ACS_CHB_CP_object timeZoneAlarm(Time_Zone_Alarm);
	
	//try to connect 2 times, 2 seconds each
	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Calling connect");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	if (timeZoneAlarm.connect(2,2) == ACS_CHB_FAIL)
	{
		if( ACS_CHB_MTZCLASS_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[256];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "connect failed.");
			ACS_CHB_MTZCLASS_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return ACS_CHB_CONNECTION_ERROR;
	}
	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Calling  get_status");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	if (timeZoneAlarm.get_status() == ACS_CHB_FAIL)
	{
		if( ACS_CHB_MTZCLASS_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[256];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "get_status failed");
			ACS_CHB_MTZCLASS_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return ACS_CHB_CONNECTION_ERROR;
	}
	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Calling  get_value");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	char *cpObjectValue = timeZoneAlarm.get_value();
	
	if (((int)strlen(cpObjectValue)) < ACS_CHB_NUMBER_OF_TZ)
	{
		timeZoneAlarm.disconnect();
		if( ACS_CHB_MTZCLASS_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[256];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "Invalid CPObject : %s , .Disconnecting...", cpObjectValue);
			ACS_CHB_MTZCLASS_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return ACS_CHB_CONNECTION_ERROR;
	}

	ZoneMismatch = cpObjectValue[TMZvalue];

	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Calling disconnect()");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	timeZoneAlarm.disconnect();

	if (ZoneMismatch == '0')
	{
		if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[256];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "Alarm is Ceased for %d" , TMZvalue);
			ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
		if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[256];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzclass::TimeZoneAlarm");
			ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
		return ACS_CHB_CEASED;
	}
	else
	{
		if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[256];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s%d", ACE_TEXT("Alarm is Issued for " ), TMZvalue);
			ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
		if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[256];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzclass::TimeZoneAlarm");
			ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
		return ACS_CHB_ISSUED;
	}

}


/*===================================================================
   ROUTINE: UTCtoLocalTime
=================================================================== */
int ACS_CHB_mtzclass::UTCtoLocalTime(time_t UTCtime,
                                     struct tm & LocalTime,
                                     int TMZ)
{
	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzclass::UTCtoLocalTime()");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	// Make the time conversion from UTC to Local Time. 
	int r = NTInterface->UTCtoLocal(UTCtime,LocalTime,TMZ);

	if (r < 0)
	{
		if( ACS_CHB_MTZCLASS_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[256];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzclass::UTCtoLocalTime()");
			ACS_CHB_MTZCLASS_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return ACS_CHB_TIME_ERROR;
	}

	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzclass::UTCtoLocalTime()");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	return ACS_CHB_TIME_OK;
}


/*===================================================================
   ROUTINE: LocalTimeToUTC
=================================================================== */

time_t ACS_CHB_mtzclass::LocalTimeToUTC(struct tm* LocalTime, int TMZ)
{
	
	// Make the time conversion from Local Time to UTC.
	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzclass::LocalTimeToUTC()");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	time_t UTCtime = NTInterface->LocalToUTC(LocalTime,TMZ,0);

	if (UTCtime < 0)
	{
		if( ACS_CHB_MTZCLASS_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[256];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzclass::LocalTimeToUTC()");
			ACS_CHB_MTZCLASS_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return ACS_CHB_TIME_ERROR;
	}
	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzclass::LocalTimeToUTC()");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	return UTCtime;
}

/*===================================================================
   ROUTINE: CalculateTheOffsetTime
=================================================================== */
time_t ACS_CHB_mtzclass::CalculateTheOffsetTime(int TMZvalue, 
                                                int &DSS,
                                                int &adjust)
{ 
	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzclass::CalculateTheOffsetTime()");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	time_t OffsetTime = NTInterface->CalculateTheOffset(TMZvalue,DSS,adjust);
	if (OffsetTime < 0) 
	{
		if( ACS_CHB_MTZCLASS_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[256];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzclass::CalculateTheOffsetTime()");
			ACS_CHB_MTZCLASS_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return ACS_CHB_TIME_ERROR;
	}
	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzclass::CalculateTheOffsetTime()");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	return OffsetTime;
}

/*===================================================================
   ROUTINE: CPTime
=================================================================== */
int ACS_CHB_mtzclass::CPTime(struct tm& TimeOfCP)
{
	if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_mtzclass::CPTime()");
		ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	int returnedValue = NTInterface->TimeFromCP(TimeOfCP);

	if (returnedValue == 0)
	{
		if( ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[256];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzclass::CPTime()");
			ACS_CHB_MTZCLASS_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
		return ACS_CHB_TIME_OK;
	}
	if( ACS_CHB_MTZCLASS_ErrorTrace.ACS_TRA_ON())
	{
		char traceBuffer[256];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_mtzclass::CPTime()");
		ACS_CHB_MTZCLASS_ErrorTrace.ACS_TRA_event(1, traceBuffer);
	}
	return ACS_CHB_CPTIME_NOT_AVAILABLE;
}
