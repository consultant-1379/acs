#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "CUTE_acs_chb_cpobject.h"
#include "time.h"
using namespace std;

ACS_CHB_CP_object* CUTE_ACS_CHB_CP_object::CP_object;

void CUTE_ACS_CHB_CP_object::init()
{

	CP_object = new ACS_CHB_CP_object(APZ_type);
}

void CUTE_ACS_CHB_CP_object::destroy()
{
	delete CP_object;
	CP_object = 0;
}

void CUTE_ACS_CHB_CP_object::testConnect()
{

	int ReTries = 10, TimeToWait = 6;

	ASSERTM("Connection status failed",CP_object->connect(ReTries, TimeToWait) == ACS_CHB_OK);

}


void CUTE_ACS_CHB_CP_object::testget_Status()
{

	ASSERTM("Get Status of CP object failed", CP_object->get_status() == Connected);

}

void CUTE_ACS_CHB_CP_object::testget_fileDescriptor()
{

	ASSERTM("get_fileDescriptor failed!!",(CP_object->get_fileDescriptor() != ACE_INVALID_HANDLE));

}

void CUTE_ACS_CHB_CP_object::testget_name()
{
	 cout<<"CP_object->get_name()"<<CP_object->get_name()<<endl;

}

void CUTE_ACS_CHB_CP_object::testget_value()
{
	cout<<"CP_object->get_value()"<<CP_object->get_value()<<endl;

}

void CUTE_ACS_CHB_CP_object::testdisconnect()
{

	CP_object->disconnect();
	ASSERTM("Get Status of CP object failed", CP_object->get_status() == Disconnected);
}

cute::suite CUTE_ACS_CHB_CP_object::make_suite_CUTE_acs_chb_cp_object()
{
	cute::suite s;
	s.push_back(CUTE(CUTE_ACS_CHB_CP_object::testConnect));
	s.push_back(CUTE(CUTE_ACS_CHB_CP_object::testget_Status));
	s.push_back(CUTE(CUTE_ACS_CHB_CP_object::testget_fileDescriptor));
	s.push_back(CUTE(CUTE_ACS_CHB_CP_object::testget_name));
	s.push_back(CUTE(CUTE_ACS_CHB_CP_object::testget_value));
	s.push_back(CUTE(CUTE_ACS_CHB_CP_object::testdisconnect));
	return s;
}



