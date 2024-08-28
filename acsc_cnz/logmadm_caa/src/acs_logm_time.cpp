#include<acs_logm_time.h>

acs_logm_time::acs_logm_time()
{
	//DEBUG("Entering %s","acs_logm_time::acs_logm_time()");
	theTime = time(0);
	theTimeTM = localtime(&theTime);
	//DEBUG("Leaving %s","acs_logm_time::acs_logm_time()");

}

acs_logm_time::~acs_logm_time()
{
	//DEBUG("Entering %s","acs_logm_time::~acs_logm_time()");
	//DEBUG("Leaving %s","acs_logm_time::~acs_logm_time()");
}
bool acs_logm_time::operator < (const acs_logm_time& aTime)
{
	DEBUG("Entering %s","acs_logm_time::operator <()");

	if(theTime < aTime.theTime)
	{
		DEBUG("Leaving %s","acs_logm_time::operator <()");
		return true;
	}
	DEBUG("Leaving %s","acs_logm_time::operator <()");
	return false;

}
bool acs_logm_time::operator > (const acs_logm_time& aTime)
{
	DEBUG("Entering %s","acs_logm_time::operator >()");

	if(theTime > aTime.theTime)
	{
		DEBUG("Leaving %s","acs_logm_time::operator >()");
		return true;
	}
	DEBUG("Leaving %s","acs_logm_time::operator >()");
	return false;

}
double acs_logm_time::elapsedTime (const acs_logm_time& aTime)
{

  return difftime(aTime.theTime,theTime);
}

double acs_logm_time::diffTime (const acs_logm_time& aTime)
{

  return difftime(theTime,aTime.theTime);
}

double acs_logm_time::elapsedTimeWithCurrentTime ()
{
  acs_logm_time myCurrentTime;
  return difftime(myCurrentTime.theTime,theTime);
}

bool acs_logm_time::isTimeElapsed()
{
	DEBUG("Entering %s","acs_logm_time::isTimeElapsed()");
	acs_logm_time myCurrentTime;
	if( theTime < myCurrentTime.theTime)
	{
		DEBUG("Leaving %s"," Time is Elapsed acs_logm_time::isTimeElapsed()");
		return true;
	}
	DEBUG("Leaving %s","Time is Not Elapsed acs_logm_time::isTimeElapsed()");
	return false;


}
bool acs_logm_time::isTimeInFuture()
{
	DEBUG("Entering %s","acs_logm_time::isTimeInFuture()");
	acs_logm_time myCurrentTime;
	if( theTime < myCurrentTime.theTime)
	{
		DEBUG("Leaving %s","Time is not in future acs_logm_time::isTimeInFuture()");
		return false;
	}
	DEBUG("Leaving %s","Time is in futureacs_logm_time::isTimeInFuture()");
	return true;
}
void  acs_logm_time::addSeconds(int aSeconds)
{
	theTime += aSeconds;
	theTimeTM = localtime(&theTime);
}
void  acs_logm_time::addMinutes(int aMinutes)
{
	theTime += SECONDS_IN_MINUTE * aMinutes;
	theTimeTM = localtime(&theTime);
}
void  acs_logm_time::addHours(int aHours)
{
	theTime +=  SECONDS_IN_HOUR * aHours;
	theTimeTM = localtime(&theTime);
}
void  acs_logm_time::addDays(int aDays)
{
	theTime +=  SECONDS_IN_DAY * aDays;
	theTimeTM = localtime(&theTime);
}
void  acs_logm_time::setTime(int aHour,int aMin, int aSec)
{
	theTimeTM = localtime(&theTime);
	theTimeTM->tm_hour = aHour;
	theTimeTM->tm_min  = aMin;
	theTimeTM->tm_sec  = aSec;
	theTime = mktime(theTimeTM);
}
void acs_logm_time::logTimeData()
{
	DEBUG("The cleanup time is  %s",asctime(theTimeTM));
}














