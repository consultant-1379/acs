#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "CUTE_ACS_CHB_mtzconv.h"
#include "time.h"
using namespace std;

ACS_CHB_mtzconv* CUTE_ACS_CHB_mtzconv::pMTZConv;
ACS_CHB_filehandler* CUTE_ACS_CHB_mtzconv::pfilehandler;

void CUTE_ACS_CHB_mtzconv::init()
{

	pfilehandler = new ACS_CHB_filehandler();
	pfilehandler->ReadZoneFile();

	pMTZConv = new ACS_CHB_mtzconv(pfilehandler);
}

void CUTE_ACS_CHB_mtzconv::destroy()
{
	delete pMTZConv;
	pMTZConv = 0;

	delete pfilehandler;
	pfilehandler = 0;
}
void CUTE_ACS_CHB_mtzconv::testGetTMZTime()
{

	struct tm Ttime;
	int TMZ;
	time_t currentTime;
	time(&currentTime);
	Ttime = *localtime(&currentTime);

	ASSERTM("GetTMZTime=",pMTZConv->get_TMZTime(&Ttime, 0));
}


void CUTE_ACS_CHB_mtzconv::testUTCtoLocalTime()
{
	time_t UTCTime(NULL);
	time(&UTCTime);
	struct tm LocalTime;

	ASSERT_EQUALM("UTCtoLocalTime Test Case 1",0, pMTZConv->UTCtoLocal(UTCTime, LocalTime, 0));
	ASSERT_EQUALM("UTCtoLocalTime Test Case 2",0, pMTZConv->UTCtoLocal(UTCTime, LocalTime, 1));
}

void CUTE_ACS_CHB_mtzconv::testLocalTimeToUTC()
{
	time_t ltime(NULL);
	time(&ltime);

	struct tm LocalTime = *localtime(&ltime);

	ASSERTM("LocalTimeToUTC Test Case 1 failed!!",(pMTZConv->LocalToUTC(&LocalTime, 0) != -1));
	ASSERTM("LocalTimeToUTC Test Case 2 failed!!",(pMTZConv->LocalToUTC(&LocalTime, 1) != -1));
}

void CUTE_ACS_CHB_mtzconv::testCalculateTheOffsetTime()
{
	int DSS, adjust, tz_value;
	//Assuming tz_value 0 Asia/Calcutta
	tz_value = 0;
	ASSERTM("CalculateTheOffsetTime Test Case 1 failed!!", (pMTZConv->CalculateTheOffset(tz_value,DSS,adjust) == 0));

	//Assuming tz_value 1 America/New_York
	tz_value = 1;
	ASSERTM("CalculateTheOffsetTime Test Case 2 failed!!", (pMTZConv->CalculateTheOffset(tz_value,DSS,adjust) > 0));
}

void CUTE_ACS_CHB_mtzconv::testCPTime()
{
	struct tm cpTime;
	ASSERTM("CPTime:=", (pMTZConv->TimeFromCP(cpTime) == 0));
}

cute::suite CUTE_ACS_CHB_mtzconv::make_suite_CUTE_ACS_CHB_mtzconv()
{
	cute::suite s;
	s.push_back(CUTE(CUTE_ACS_CHB_mtzconv::testUTCtoLocalTime));
	s.push_back(CUTE(CUTE_ACS_CHB_mtzconv::testLocalTimeToUTC));
	s.push_back(CUTE(CUTE_ACS_CHB_mtzconv::testCalculateTheOffsetTime));
	s.push_back(CUTE(CUTE_ACS_CHB_mtzconv::testCPTime));
	s.push_back(CUTE(CUTE_ACS_CHB_mtzconv::testGetTMZTime));
	return s;
}



