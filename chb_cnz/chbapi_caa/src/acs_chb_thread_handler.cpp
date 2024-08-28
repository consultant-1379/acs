/*=================================================================== */
   /**
   @file acs_chb_thread_handler.cpp

   Class method implementationn for ACS_CHB_Threadhandler class.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       25/11/2010     TA       Initial Release
   **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <acs_chb_thread_handler.h>
#include <ACS_APGCC_CommonLib.h>
#include <acs_prc_api.h>

#define RELATIVETIME(x)    (ACE_OS::time(NULL) + x)  

static ACS_TRA_trace ACS_CHB_MTZ_TH_DebugTrace("ACS_CHB_MTZ_TH_DebugTrace", "C512");
static ACS_TRA_trace ACS_CHB_MTZ_TH_ErrorTrace("ACS_CHB_MTZ_TH_ErrorTrace", "C512");
/*===================================================================
   ROUTINE: ACS_CHB_Threadhandler
=================================================================== */
ACS_CHB_Threadhandler::ACS_CHB_Threadhandler(ACS_CHB_filehandler* pfilehandler)
{
        if( ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[256];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering ACS_CHB_Threadhandler::ACS_CHB_Threadhandler Constructor");
                ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0, "%s", "ACS_CHB_Threadhandler::ACS_CHB_Threadhandler Constructor");

	pThreadStopEvent = NULL;
	pf = NULL;	
	threadGrpId = 0;
	threadId = 0;
	if( pfilehandler != 0)
	{
		StartThread(pfilehandler);
	}
        if( ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[256];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving ACS_CHB_Threadhandler::ACS_CHB_Threadhandler Constructor");
                ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0, "%s", "Leaving ACS_CHB_Threadhandler::ACS_CHB_Threadhandler Constructor");
}


/*===================================================================
   ROUTINE: ~ACS_CHB_Threadhandler
=================================================================== */
ACS_CHB_Threadhandler::~ACS_CHB_Threadhandler()
{
        if( ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[256];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering ACS_CHB_Threadhandler::ACS_CHB_Threadhandler Destructor");
                ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0, "%s", "Entering ACS_CHB_Threadhandler::ACS_CHB_Threadhandler Destructor");
	if( pThreadStopEvent != 0 )
	{
		pThreadStopEvent->signal();
	}

	if( threadId != 0 )
	{
		ACE_Thread_Manager::instance()->join(threadId);
	}

	delete pThreadStopEvent;
	pThreadStopEvent = 0;

        if( ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[256];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving ACS_CHB_Threadhandler::ACS_CHB_Threadhandler Destructor");
                ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0, "%s", "Leaving ACS_CHB_Threadhandler::ACS_CHB_Threadhandler Destructor");
	
}

/*===================================================================
   ROUTINE: CheckCFGfile
=================================================================== */
void* ACS_CHB_Threadhandler::CheckCFGfile(void* p)
{
        if( ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[256];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering ACS_CHB_Threadhandler::CheckCFGfile()");
                ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0, "%s", "Entering ACS_CHB_Threadhandler::CheckCFGfile()");
	char FilePath[PATHLENGTH];
	char ACS_DATAFilePath[PATHLENGTH];
	ACE_OS::memset(FilePath,0,sizeof(FilePath));
	ACE_OS::memset(ACS_DATAFilePath,0,sizeof(ACS_DATAFilePath));
	int pathlength = PATHLENGTH;

	//Now construct the path to .cfg file
	ACS_APGCC_CommonLib oComLib;
	ACS_APGCC_DNFPath_ReturnTypeT retCode =  oComLib.GetDataDiskPath("ACS_DATA", FilePath, pathlength);
	if( retCode != ACS_APGCC_DNFPATH_SUCCESS )
	{
		return 0;
	}
	ACE_OS::strcpy( ACS_DATAFilePath,FilePath);
	ACE_OS::strcat(FilePath, "/");
	ACE_OS::strcat(FilePath, MTZDIR);
	ACE_OS::strcat(FilePath, FILENAME);

	ACS_CHB_Threadhandler* pT = (ACS_CHB_Threadhandler*)p;

	int result = 0;
	// Try to create the MTZ directory
	// If on passive node we will loop until we get active (or get a stopEvent)

#if 0
	ACE_Time_Value tm(1,0);
#endif

	ACE_Time_Value tm;
	tm.sec(ACE_OS::time(0));
	
	if( ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[256];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s%s", " ACS_CHB_Threadhandler::CheckCFGfile(),FilePath is   ",FilePath);
                ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

	while(1)
	{
		//int nodeState = 1;
		//ACS_PRC_API prcObj;
		//nodeState = prcObj.askForNodeState();
		// IMM is not able to handle many Calls due to which its crashing.Each call to askForNodeState() fetches
		//4 IMM calls. So, calling askForNodeState() many times in while loop is risky.
		struct stat myStat;
		if (stat(ACS_DATAFilePath, &myStat) != -1)
		{
			if (S_ISDIR(myStat.st_mode))
			{
				//We are at active node.
				if( ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[256];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "ACS_CHB_Threadhandler::CheckCFGfile(), in Active Node");
					ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}
				break;
			}
		}
#if 0
		if( nodeState == 1 ) //active node
		{
			break;
		}
#endif
		else
		{
			tm.sec(tm.sec() + 1);
			if( ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_ON())
			{
				char traceBuffer[256];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "ACS_CHB_Threadhandler::CheckCFGfile(), in Passive  Node");
				ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}
			result = pT->pThreadStopEvent->wait(&tm);
			//result = pT->pThreadStopEvent->wait(&tm,0);
			if( result == 0)
			{
				return 0;
			}
		}
	}

	
	// At this point we have managed to access the directory.
	// Initiate a File Read.
	(pT->pf)->ReadZoneFile();

	
	// At this point we have managed to access the directory to be
	// monitored and we want to enter the notification loop (forever)
	//TR HP27240
	//Modified the timeout value to 1000micro secs

	//ACE_Time_Value tm1(0,1000000);

	time_t oldTime(NULL);
	bool isInitial = true;
	
	if( ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[256];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "ACS_CHB_Threadhandler::CheckCFGfile(),Before Entering into Do While loop,");
                ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

	char traceBuffer[256];
	int res = 1;
	tm.sec(ACE_OS::time(0));
	do
	{

		//ACE_stat statBuffer;

		ACE_stat statBuffer;
		if(ACE_OS::stat(FilePath, &statBuffer)== 0)	
		{

			if( ACE_OS::difftime(statBuffer.st_mtime, oldTime) != 0 && !isInitial)
			{
				
				(pT->pf)->ReadZoneFile();

			}
			oldTime = statBuffer.st_mtime;
			isInitial = false;
		}
		else
		{
			if( ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_ON())
			{
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s%s", " ACS_CHB_Threadhandler::CheckCFGfile(), Not able to access ",FilePath);
				ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}
		}

		tm.sec(tm.sec() + 1);
		if( ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_ON())
		{
                	memset(&traceBuffer, 0, sizeof(traceBuffer));
                	sprintf(traceBuffer, "ACS_CHB_Threadhandler::CheckCFGfile(), in do While loop, timeout sec= %d", tm.sec());
                	ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_event(1, traceBuffer);

 		}

		res = pT->pThreadStopEvent->wait(&tm);
		if( ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_ON())
		{
                	memset(&traceBuffer, 0, sizeof(traceBuffer));
                	sprintf(traceBuffer, "ACS_CHB_Threadhandler::CheckCFGfile(), in do While loop, wait result= %d", res);
                	ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_event(1, traceBuffer);

 		}

	}

	while(res);
	//while(pT->pThreadStopEvent->wait(&tm1,0));
        if( ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_ON())
        {
		char traceBuffer[256];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving ACS_CHB_Threadhandler::CheckCFGfile()");
                ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0, "%s", "Leaving ACS_CHB_Threadhandler::CheckCFGfile()");
	return 0;
}

/*===================================================================
   ROUTINE: StartThread
=================================================================== */
bool ACS_CHB_Threadhandler::StartThread(ACS_CHB_filehandler* pfilehandler)
{
        if( ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[256];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Entering ACS_CHB_Threadhandler::StartThread()");
                ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
        //DEBUG(0, "%s", "Entering ACS_CHB_Threadhandler::StartThread()");
	pf = pfilehandler;

	// Create stop event
	//
	pThreadStopEvent = new ACE_Event(true, false, USYNC_THREAD, "CFG_THREAD_TERMINATE_EVENT");

	if(pThreadStopEvent == NULL)
	{
              if( ACS_CHB_MTZ_TH_ErrorTrace.ACS_TRA_ON())
              {
                      char traceBuffer[256];
                      memset(&traceBuffer, 0, sizeof(traceBuffer));
                      sprintf(traceBuffer, "%s", "Leaving ACS_CHB_Threadhandler::StartThread()");
                      ACS_CHB_MTZ_TH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
              }
              //ERROR(0, "%s", "Leaving ACS_CHB_Threadhandler::StartThread()");
              return ACS_CHB_error;
	}

	// try to read ZoneFile, don't do this after we have started the
	// thread because it also use ReadZoneFile().
	//ret = pf->ReadZoneFile();
	
	void* (*func)(void*);		// Set function pointers to the functions
	func = &(ACS_CHB_Threadhandler::CheckCFGfile); // to execute in separate threads.		*/

	const ACE_TCHAR* lpszThreadName = "CFGThread";
	threadGrpId = ACE_Thread_Manager::instance()->spawn( func,
							this,
							THR_NEW_LWP | THR_JOINABLE| THR_INHERIT_SCHED,
							&threadId,
							0,
							ACE_DEFAULT_THREAD_PRIORITY,
							threadGrpId,
							0,
							ACE_DEFAULT_THREAD_STACKSIZE,
							&lpszThreadName);


	if (threadGrpId == -1)
	{
                if( ACS_CHB_MTZ_TH_ErrorTrace.ACS_TRA_ON())
                {
                        char traceBuffer[256];
                        memset(&traceBuffer, 0, sizeof(traceBuffer));
                        sprintf(traceBuffer, "%s", "Leaving ACS_CHB_Threadhandler::StartThread()");
                        ACS_CHB_MTZ_TH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
                }
                //ERROR(0, "%s", "Leaving ACS_CHB_Threadhandler::StartThread()");
		return ACS_CHB_error;
	}
        if( ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[256];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving ACS_CHB_Threadhandler::StartThread()");
                ACS_CHB_MTZ_TH_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//DEBUG(0, "%s", "Leaving ACS_CHB_Threadhandler::StartThread()");
	return ACS_CHB_success;
}
