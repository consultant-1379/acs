#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "CUTE_ACS_CHB_Threadhandler.h"
#include "time.h"
using namespace std;

ACS_CHB_Threadhandler* CUTE_ACS_CHB_Threadhandler::pMTZThreadhandler;
ACS_CHB_filehandler* CUTE_ACS_CHB_Threadhandler::pMTZfilehandler;

void CUTE_ACS_CHB_Threadhandler::init()
{
	pMTZfilehandler =  new ACS_CHB_filehandler();
	pMTZThreadhandler = new ACS_CHB_Threadhandler(pMTZfilehandler);
	ACE_OS::sleep(10);
}

void CUTE_ACS_CHB_Threadhandler::destroy()
{
	delete pMTZThreadhandler;
	pMTZThreadhandler = 0;

	delete pMTZfilehandler;
	pMTZfilehandler = 0;
}



