#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "CUTE_ACS_CHB_filehandler.h"
#include "time.h"
using namespace std;

ACS_CHB_filehandler* CUTE_ACS_CHB_filehandler::pMTZfilehandler;
time_t CUTE_ACS_CHB_filehandler::CPtimeSec;
time_t CUTE_ACS_CHB_filehandler::written;
FileMapType fMap;

void CUTE_ACS_CHB_filehandler::init()
{
	pMTZfilehandler = new ACS_CHB_filehandler();
}

void CUTE_ACS_CHB_filehandler::destroy()
{
	delete pMTZfilehandler;
	pMTZfilehandler = 0;

}

void CUTE_ACS_CHB_filehandler::testValidateTimeZone()
{
	ASSERTM("ValidateTimeZone failed!!", pMTZfilehandler->validateTimeZone("Asia/America") != true);
	ASSERTM("ValidateTimeZone failed!!", pMTZfilehandler->validateTimeZone("(GMT+04:30)Asia/Kabul") == true);
}

void CUTE_ACS_CHB_filehandler::testReadZoneFile()
{
	ASSERTM("ReadZoneFile failed!!", pMTZfilehandler->ReadZoneFile(&fMap) == true);
}

void CUTE_ACS_CHB_filehandler::testWriteZoneFile()
{
	ASSERTM("WriteZoneFile failed!!", pMTZfilehandler->WriteZoneFile(&fMap) == true);
}

void CUTE_ACS_CHB_filehandler::testReadZoneFileCPtime()
{
	pMTZfilehandler->ReadZoneFileCPtime(CPtimeSec, written);
	ASSERTM("ReadZoneFileCPtime failed!!", pMTZfilehandler->ReadZoneFileCPtime(CPtimeSec, written) == true);
}
void CUTE_ACS_CHB_filehandler::testWriteZoneFileCPtime()
{

	ASSERTM("WriteZoneFileCPtime failed!!", pMTZfilehandler->WriteZoneFileCPtime(CPtimeSec, written) == true);
}

void CUTE_ACS_CHB_filehandler::testGetNTZone()
{
	TIME_ZONE_INFORMATION Tzi;
	ASSERTM("GetNTZOne!!", pMTZfilehandler->GetNTZone(0, Tzi) == true);
}

void CUTE_ACS_CHB_filehandler::testCheckTimeZone()
{
	TIME_ZONE_INFORMATION Tzi;
	ASSERTM("CheckTimeZone!!", pMTZfilehandler->CheckTimeZone("(GMT+04:30) Asia/Kabul", &Tzi) == true);
}

void CUTE_ACS_CHB_filehandler::testValidateTMZNumber()
{
	ASSERTM("ValidateTMZNumber", pMTZfilehandler->ValidateTMZNumber("we") == false);
	ASSERTM("ValidateTMZNumber", pMTZfilehandler->ValidateTMZNumber("1") == true);
}

cute::suite CUTE_ACS_CHB_filehandler::make_suite_CUTE_ACS_CHB_filehandler()
{
	cute::suite s;
	s.push_back(CUTE(CUTE_ACS_CHB_filehandler::testReadZoneFile));
	s.push_back(CUTE(CUTE_ACS_CHB_filehandler::testGetNTZone));
	s.push_back(CUTE(CUTE_ACS_CHB_filehandler::testReadZoneFileCPtime));
	s.push_back(CUTE(CUTE_ACS_CHB_filehandler::testWriteZoneFileCPtime));
	s.push_back(CUTE(CUTE_ACS_CHB_filehandler::testValidateTimeZone));
	s.push_back(CUTE(CUTE_ACS_CHB_filehandler::testValidateTMZNumber));
	s.push_back(CUTE(CUTE_ACS_CHB_filehandler::testCheckTimeZone));
	s.push_back(CUTE(CUTE_ACS_CHB_filehandler::testWriteZoneFile));


	return s;
}



