
/*=================================================================== */
/**
   @file acs_logm_logmaint.cpp

   Class method implementation for LOGM module.

   This module contains the implementation of class declared in
   the acs_logm_logmaint.h module

   @version 1.0.0

 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       21/10/2010     XKUSATI  Initial Release
   N/A       17/01/2013     XCSRPAD  Deletion of unwanted CMX log files
==================================================================== */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <deque>
#include <sstream>
#include <ace/Event.h>
#include <acs_prc_api.h>
#include <acs_logm_types.h>
#include <acs_logm_common.h>
#include <acs_logm_logmaint.h>
//#include <acs_logm_logparam_oihandler.h>
//#include <acs_logm_srvparam_oihandler.h>

/*===================================================================
			   VARIABLE DECLARATION
=================================================================== */
/*=================================================================== */
/**
   @brief logFileContainer
 */
/*=================================================================== */
deque<string> logFileContainer;
/*=================================================================== */
/**
   @brief logFileContainerSCX
 */
/*=================================================================== */
deque<string> logFileContainerSCX;
/*=================================================================== */
/**
   @brief logFileContainerCMX
 */
/*=================================================================== */
deque<string> logFileContainerCMX;
/*=================================================================== */
/**
   @brief logFileContainerWC
 */
/*=================================================================== */
deque<string> logFileContainerWC;
/*=================================================================== */
/**
   @brief logFileContainerfolder
 */
/*=================================================================== */

deque<string> logFileContainerfolder;
/*=================================================================== */
/**
   @brief logFileContainerWC_SCX
 */
/*=================================================================== */

deque<string> logFileContainerWC_SCX;
/*=================================================================== */
/**
   @brief logFileContainerWC_CMX
 */
/*=================================================================== */

deque<string> logFileContainerWC_CMX;
/*=================================================================== */
/**
   @brief ACS_ACSC_Logmaint::isStatusActive
 */
/*=================================================================== */
bool ACS_ACSC_Logmaint::isStatusActive = true;

/*===================================================================
   ROUTINE: ACS_ACSC_Logmaint
=================================================================== */
ACS_ACSC_Logmaint::ACS_ACSC_Logmaint(ACE_Event* poEvent): m_phEndEvent(poEvent)
{

	if( theOmHandler.Init() == ACS_CC_FAILURE )
	{
		DEBUG("%s","Error occurred while initializing APGCC OmHandler");
		DEBUG("%s","Exiting from Logmaint_constructor");
	}


}//End of Constructor

/*===================================================================
   ROUTINE: ~ACS_ACSC_Logmaint
=================================================================== */
ACS_ACSC_Logmaint::~ACS_ACSC_Logmaint() 
{
	if( theOmHandler.Finalize() == ACS_CC_FAILURE )
	{
		ERROR("%s", "Error occured while finalizing Omhandler");
	}
}//End of Destructor


/*===================================================================
   ROUTINE: cleanLogFilesInteractive
=================================================================== */
void ACS_ACSC_Logmaint::cleanLogFilesInteractive() 
{
	DEBUG("%s", "Entering cleanLogFilesInteractive" );   

	double dSecondsToWaitTest;

	_ACS_LOGM_SERVICEPARAMS ptrParams;
	memset(&ptrParams, 0, sizeof( _ACS_LOGM_SERVICEPARAMS ));

	DEBUG("%s","Fetching service trigger parameters from IMM...\n");
	DEBUG("%s","ACS_ACSC_Logmaint::cleanLogFilesInteractive() - Calling GetServiceparams()");

	ACE_INT32 serviceParamsStatus = GetServiceparams(&ptrParams);

	DEBUG("Status of fetching service trigger parameters from IMM : %d",serviceParamsStatus);

	if(serviceParamsStatus != SUCCESS)
	{
		DEBUG("%s","Error occured while reading service trigger parameters from IMM");
	}
	else
	{
		DEBUG("%s","All parameters returned successfully from  IMM!!! \n");

		DEBUG("Received service parameters (Hour) from IMM : %d",(ptrParams.iTimeHH));
		DEBUG("Received service parameters (MIN) from IMM :  %d",(ptrParams.iTimeMM));
		DEBUG("Received service parameters (FREQ) from IMM : %d",(ptrParams.iFreq));
		DEBUG("Received service parameters (LogTypeCount) from IMM :  %d",(ptrParams.iLogTypCnt));
		DEBUG("Received service parameters (iStsTrigger) from IMM : %d\n",(ptrParams.iStsTrg));
		DEBUG("Received service parameters (iFileCntFreq) from IMM : %d\n",(ptrParams.iFileCntFreq));
		ACE_UINT32 dwTimerLowValue = 0;
		ACE_UINT32 dwTimerHighValue= 0;

		GetInitialTimeToWait(&ptrParams,&dSecondsToWaitTest);
		if( dSecondsToWaitTest < 0 )
		{
			dSecondsToWaitTest = 0;
		}

		DEBUG("Initial time to wait for service trigger : %d Seconds",(dSecondsToWaitTest));
		double timeForWait = 0;
		timeForWait = dSecondsToWaitTest;
		ACE_Time_Value oTimeValue(timeForWait);
		DEBUG("Number of times the cleanup activity has executed since service startup : %d\n",(oTimeValue.sec()));

		//***************************************************************************
		//Event terminates when user press ctrl+C
		//Event terminates when the time for debug mode elapses
		//***************************************************************************

		while( m_phEndEvent->wait(&oTimeValue,0))
		{
			DEBUG("%s","Cleanup activity started ...\n");

			DeleteLogFiles (ptrParams.iLogTypCnt, dwTimerLowValue, dwTimerHighValue);
			DEBUG("%s","Cleanup activity stopped ...\n");

			DEBUG("%s","ACS_ACSC_Logmaint::cleanLogFilesInteractive() - Calling GetServiceparams() in wait call");

			serviceParamsStatus = GetServiceparams(&ptrParams);

			if( serviceParamsStatus == SUCCESS)
			{
				double dSecondsToWait = 0;

				GetInitialTimeToWait(&ptrParams,&dSecondsToWaitTest);

				if(ptrParams.iFreq == DAILY)
				{
					DEBUG("%s","******Cleanup activities will take place daily********");
					dSecondsToWait = SECONDS_IN_DAY + dSecondsToWaitTest;
					DEBUG("Sleeping for %f seconds  ...",dSecondsToWait);

					ACE_Time_Value oTimeValueDay(dSecondsToWait);
					oTimeValue = oTimeValueDay;
				}
				else if(ptrParams.iFreq == WEEKLY)
				{
					DEBUG("%s","\n\n******Cleanup activities will take place Weekly********");

					dSecondsToWait = SECONDS_IN_WEEK + dSecondsToWaitTest;

					DEBUG("Sleeping for %f seconds  ...", dSecondsToWait);

					ACE_Time_Value oTimeValueDay(dSecondsToWait);
					oTimeValue = oTimeValueDay;
				}
				else
				{
					ERROR("%s","Configuration error : Invalid frequency!!!\n");
				}
			}
			else
			{
				ERROR("%s", "Error occured while reading Service Parameters");
				return;
			}
		}

	}
	DEBUG("%s","ACS_ACSC_Logmaint Thread Exiting....");
}//End of cleanLogFilesInteractive

/*===================================================================
   ROUTINE: cleanSCXLogFilesInteractive
=================================================================== */
void ACS_ACSC_Logmaint::cleanSCXLogFilesInteractive() 
{
	DEBUG("%s", "Entering cleanSCXLogFilesInteractive" );   

	_ACS_LOGM_SERVICEPARAMS ptrParams;
	memset(&ptrParams, 0, sizeof( _ACS_LOGM_SERVICEPARAMS ));

	DEBUG("%s","Fetching service trigger parameters from IMM...\n");
	DEBUG("%s","ACS_ACSC_Logmaint::cleanSCXLogFilesInteractive() - Calling GetServiceparams()");

	ACE_INT32 serviceParamsStatus = GetServiceparams(&ptrParams);

	DEBUG("Status of fetching service trigger parameters from IMM : %d",serviceParamsStatus);

	if(serviceParamsStatus != SUCCESS)
	{
		DEBUG("%s","Error occured while reading service trigger parameters from IMM");
	}
	else
	{
		DEBUG("%s","All parameters returned successfully from  IMM!!! \n");

		DEBUG("Received service parameters (Hour) from IMM : %d",(ptrParams.iTimeHH));
		DEBUG("Received service parameters (MIN) from IMM :  %d",(ptrParams.iTimeMM));
		DEBUG("Received service parameters (FREQ) from IMM : %d",(ptrParams.iFreq));
		DEBUG("Received service parameters (LogTypeCount) from IMM :  %d",(ptrParams.iLogTypCnt));
		DEBUG("Received service parameters (iStsTrigger) from IMM : %d\n",(ptrParams.iStsTrg));
		DEBUG("Received service parameters (iFileCntFreq) from IMM : %d\n",(ptrParams.iFileCntFreq));
		ACE_UINT32 dwTimerLowValue = 0;
		ACE_UINT32 dwTimerHighValue= 0;
		//Case of SCX file
		//Check the number of SCX files for every 15 mins
		ACE_Time_Value oTimeValue(10);
		while( m_phEndEvent->wait(&oTimeValue,0))
		{
			DEBUG("%s","Cleanup activity started for SCX ...\n");

			DeleteExtraFiles(ptrParams.iLogTypCnt,dwTimerLowValue,
					dwTimerHighValue);

			DEBUG("%s","Cleanup activity stopped for SCX...\n");
			DEBUG("%s","ACS_ACSC_Logmaint::cleanSCXLogFilesInteractive() - Calling GetServiceparams() in wait call");

			serviceParamsStatus = GetServiceparams(&ptrParams);

			if( serviceParamsStatus == SUCCESS)
			{

				double dSecondsToWait = 0;
				dSecondsToWait = 60*ptrParams.iFileCntFreq;

				ACE_Time_Value oTimeValueDay(dSecondsToWait);

				oTimeValue = oTimeValueDay;

			}
			else
			{
				ERROR("%s", "Error occured while reading Service Parameters");
				return;
			}
		}//end of while
	}
	DEBUG("%s","ACS_ACSC_Logmaint SCX Thread Exiting....");

}//End of cleanSCXLogFilesInteractive
/*===================================================================
   ROUTINE: cleanCMXLogFilesInteractive
=================================================================== */
void ACS_ACSC_Logmaint::cleanCMXLogFilesInteractive() 
{
	DEBUG("%s", "Entering cleanCMXLogFilesInteractive" );   

	_ACS_LOGM_SERVICEPARAMS ptrParams;
	memset(&ptrParams, 0, sizeof( _ACS_LOGM_SERVICEPARAMS ));

	DEBUG("%s","Fetching service trigger parameters from IMM...\n");
	DEBUG("%s","ACS_ACSC_Logmaint::cleanCMXLogFilesInteractive() - Calling GetServiceparams()");

	ACE_INT32 serviceParamsStatus = GetServiceparams(&ptrParams);

	DEBUG("Status of fetching service trigger parameters from IMM : %d",serviceParamsStatus);

	if(serviceParamsStatus != SUCCESS)
	{
		DEBUG("%s","Error occured while reading service trigger parameters from IMM");
	}
	else
	{
		DEBUG("%s","All parameters returned successfully from  IMM!!! \n");

		DEBUG("Received service parameters (Hour) from IMM : %d",(ptrParams.iTimeHH));
		DEBUG("Received service parameters (MIN) from IMM :  %d",(ptrParams.iTimeMM));
		DEBUG("Received service parameters (FREQ) from IMM : %d",(ptrParams.iFreq));
		DEBUG("Received service parameters (LogTypeCount) from IMM :  %d",(ptrParams.iLogTypCnt));
		DEBUG("Received service parameters (iStsTrigger) from IMM : %d\n",(ptrParams.iStsTrg));
		DEBUG("Received service parameters (iFileCntFreq) from IMM : %d\n",(ptrParams.iFileCntFreq));
		ACE_UINT32 dwTimerLowValue = 0;
		ACE_UINT32 dwTimerHighValue= 0;
		//Case of CMX file
		//Check the number of CMX files for every 15 mins
		ACE_Time_Value oTimeValue(10);
		while( m_phEndEvent->wait(&oTimeValue,0))
		{
			DEBUG("%s","Cleanup activity started for CMX ...\n");

			DeleteExtraFiles(ptrParams.iLogTypCnt,dwTimerLowValue,
					dwTimerHighValue);

			DEBUG("%s","Cleanup activity stopped for CMX...\n");
			DEBUG("%s","ACS_ACSC_Logmaint::cleanCMXLogFilesInteractive() - Calling GetServiceparams() in wait call");

			serviceParamsStatus = GetServiceparams(&ptrParams);

			if( serviceParamsStatus == SUCCESS)
			{

				double dSecondsToWait = 0;
				dSecondsToWait = 60*ptrParams.iFileCntFreq;

				ACE_Time_Value oTimeValueDay(dSecondsToWait);

				oTimeValue = oTimeValueDay;

			}
			else
			{
				ERROR("%s", "Error occured while reading Service Parameters");
				return;
			}
		}//end of while
	}
	DEBUG("%s","ACS_ACSC_Logmaint CMX Thread Exiting....");

}//End of cleanCMXLogFilesInteractive
/*===================================================================
//ROUTINE: cleanCMXLogFilesCoreMiddleWare
=================================================================== */

void ACS_ACSC_Logmaint::cleanCMXLogFilesCoreMiddleWare()
{
	DEBUG("%s", "Entering cleanCMXLogFilesCoreMiddleWare");
	int nodeState = 0;
        ACS_PRC_API prcObj;
#if 0
	//Register the exit handler for Core MiddleWare Health Check.
	if( ThrExitHandler::init() == false )
	{
		ERROR("%s", "Error occured while registering exit handler");
		ERROR("%s", "Leaving cleanCMXLogFilesCoreMiddleWare");
		return ;
	}
#endif
	_ACS_LOGM_SERVICEPARAMS ptrParams;
	memset( &ptrParams, 0, sizeof( _ACS_LOGM_SERVICEPARAMS ));

	DEBUG("%s","Fetching service trigger parameters from IMM...\n");
	DEBUG("%s","ACS_ACSC_Logmaint::cleanCMXLogFilesCoreMiddleWare() - Calling GetServiceparams() ");
	ACE_INT32 serviceParamsStatus = GetServiceparams(&ptrParams);

	DEBUG("Status of fetching service trigger parameters from IMM : %d",serviceParamsStatus);

	if(serviceParamsStatus == SUCCESS )
	{
		DEBUG("%s","All parameters retrieved successfully from  IMM!!! \n");

		DEBUG("Received service parameters (Hour) from IMM : %d",(ptrParams.iTimeHH));
		DEBUG("Received service parameters (MIN) from IMM :  %d",(ptrParams.iTimeMM));
		DEBUG("Received service parameters (FREQ) from IMM : %d",(ptrParams.iFreq));
		DEBUG("Received service parameters (LogTypeCount) from IMM :  %d",(ptrParams.iLogTypCnt));
		DEBUG("Received service parameters (iStsTrigger) from IMM : %d\n",(ptrParams.iStsTrg));
		DEBUG("Received service parameters (iFileCntFreq) from IMM : %d\n",(ptrParams.iFileCntFreq));

		//Case of CMX Log Files. Every 15 Min NUmber of files is counted and if gteater than 
		//threshold value remove extra files.

		ACE_UINT32 dwTimerLowValue = 0;
		ACE_UINT32 dwTimerHighValue = 0;
		ACE_Time_Value oTimeValue(10);
		while( m_phEndEvent->wait(&oTimeValue,0))
		{
			nodeState = prcObj.askForNodeState();
                	if(nodeState == 1)//Active node
       			{ 
				DEBUG("%s","Cleanup activity started for SCX on active side...\n");

				DeleteExtraFiles(ptrParams.iLogTypCnt,dwTimerLowValue,
					dwTimerHighValue);
				DEBUG("%s","Cleanup activity stopped for CMX...\n");

			}
			else if(nodeState == 2) //Passive node
			{
				DEBUG("%s","Cleanup activity not started for CMX as passive side...\n");
			}
			else
        		{
				DEBUG("%s","Node State is Undefined!! in ACS_ACSC_Logmaint::cleanCMXLogFilesCoreMiddleWare()...\n");
       			} 



			 DEBUG("%s","ACS_ACSC_Logmaint::cleanCMXLogFilesCoreMiddleWare() - Calling GetServiceparams() in wait call");

			serviceParamsStatus = GetServiceparams(&ptrParams);

			if( serviceParamsStatus  == SUCCESS )
			{
				double dSecondsToWait = 0;

				dSecondsToWait = 60*ptrParams.iFileCntFreq;

				//ACE_Time_Value oTimeValueDay(dSecondsToWait);
				//oTimeValue = oTimeValueDay;	

				DEBUG("Sleeping for %f seconds dSecondsToWait in cleanCMXLogFilesCoreMiddleWare  ...",dSecondsToWait);
				ACE_Time_Value oTimeValueDay;
				oTimeValueDay.set(dSecondsToWait);
				oTimeValue = oTimeValueDay;	
				DEBUG("Sleeping for %d seconds in cleanCMXLogFilesCoreMiddleWare ...",oTimeValue.sec());

			}
			else
			{
				ERROR("%s", "Error occured while reading Service Parameters");
				return;
			}
		}//end of while

	}
	DEBUG("%s","ACS_ACSC_Logmaint CMX Thread Exiting....");
	DEBUG("%s", "Leaving cleanCMXLogFilesCoreMiddleWare ");
}//End of cleanCMXLogFilesCoreMiddleWare
/*===================================================================
	ROUTINE: cleanLogFilesCoreMiddleWare
=================================================================== */

void ACS_ACSC_Logmaint::cleanLogFilesCoreMiddleWare()
{
	DEBUG("%s", "Entering cleanLogFilesCoreMiddleWare");
#if 0
	//Register the exit handler for Core MiddleWare Health Check.
	if( ThrExitHandler::init() == false )
	{
		ERROR("%s", "Error occured while registering exit handler");
		ERROR("%s", "Leaving cleanLogFilesCoreMiddleWare");
		return ;
	}
#endif
	double dSecondsToWaitTest;

	_ACS_LOGM_SERVICEPARAMS ptrParams;
	memset( &ptrParams, 0, sizeof( _ACS_LOGM_SERVICEPARAMS ));

	DEBUG("%s","Fetching service trigger parameters from IMM...\n");
	DEBUG("%s","ACS_ACSC_Logmaint::cleanLogFilesCoreMiddleWare() - Calling GetServiceparams()");
	ACE_INT32 serviceParamsStatus = GetServiceparams(&ptrParams);

	DEBUG("Status of fetching service trigger parameters from IMM : %d",serviceParamsStatus);

	if(serviceParamsStatus == SUCCESS )
	{
		DEBUG("%s","All parameters retrieved successfully from  IMM!!! \n");

		DEBUG("Received service parameters (Hour) from IMM : %d",(ptrParams.iTimeHH));
		DEBUG("Received service parameters (MIN) from IMM :  %d",(ptrParams.iTimeMM));
		DEBUG("Received service parameters (FREQ) from IMM : %d",(ptrParams.iFreq));
		DEBUG("Received service parameters (LogTypeCount) from IMM :  %d",(ptrParams.iLogTypCnt));
		DEBUG("Received service parameters (iStsTrigger) from IMM : %d\n",(ptrParams.iStsTrg));
		DEBUG("Received service parameters (iFileCntFreq) from IMM : %d\n",(ptrParams.iFileCntFreq));

		GetInitialTimeToWait(&ptrParams,&dSecondsToWaitTest);
		if( dSecondsToWaitTest < 0 )
		{
			dSecondsToWaitTest = 0;
		}


		DEBUG("Initial time to wait for service trigger : %d Seconds",(dSecondsToWaitTest));
		double timeForWait = 0;
		timeForWait = dSecondsToWaitTest;
		ACE_Time_Value oTimeValue(timeForWait);
		DEBUG("Number of times the cleanup activity has executed since service startup : %d\n",(oTimeValue.sec()));

		//***************************************************************************
		//Waits till service trigger time is elapsed or
		//Waits for a day or week based on the parameter from IMM
		//***************************************************************************
		ACE_UINT32 dwTimerLowValue = 0;
		ACE_UINT32 dwTimerHighValue = 0;

		while( m_phEndEvent->wait(&oTimeValue,0))
		{
			DEBUG("%s","Cleanup activity started ...\n");

			DeleteLogFiles (ptrParams.iLogTypCnt, dwTimerLowValue, dwTimerHighValue);

			DEBUG("%s","Cleanup activity stopped ...\n");


			DEBUG("%s","ACS_ACSC_Logmaint::cleanLogFilesCoreMiddleWare() - Calling GetServiceparams() in wait call");

			serviceParamsStatus = GetServiceparams(&ptrParams);

			if( serviceParamsStatus  == SUCCESS )
			{
				double dSecondsToWait = 0;

				GetInitialTimeToWait(&ptrParams,&dSecondsToWaitTest);

				if(ptrParams.iFreq == DAILY)
				{
					DEBUG("%s","******Cleanup activities will take place daily********");

					dSecondsToWait = SECONDS_IN_DAY + dSecondsToWaitTest;

					DEBUG("Sleeping for %f seconds (1 Day) ...",dSecondsToWait);

					ACE_Time_Value oTimeValueDay;
					oTimeValueDay.set(dSecondsToWait);
					oTimeValue = oTimeValueDay;
					
					DEBUG("Sleeping for %d seconds (1 Day) ...",oTimeValue.sec());

					//ACE_Time_Value oTimeValueDay(dSecondsToWait);
					//oTimeValue = oTimeValueDay;
						
				}
				else if(ptrParams.iFreq == WEEKLY)
				{
					DEBUG("%s","\n\n******Cleanup activities will take place Weekly********");

					dSecondsToWait = SECONDS_IN_WEEK + dSecondsToWaitTest;

					DEBUG("Sleeping for %f seconds (1 Week) ...", dSecondsToWait);
					ACE_Time_Value oTimeValueDay(dSecondsToWait);
					oTimeValue = oTimeValueDay;
				}
				else
				{
					ERROR("%s","Configuration error : Invalid frequency!!!\n");
				}
			}
			else
			{
				ERROR("%s", "Error occured while reading Service Parameters");
				return;
			}
		}

	}
	DEBUG("%s","ACS_ACSC_Logmaint Thread Exiting....");
	DEBUG("%s", "Leaving cleanLogFilesCoreMiddleWare ");
}//End of cleanLogFilesCoreMiddleWare



/*===================================================================
	ROUTINE: cleanSCXLogFilesCoreMiddleWare
=================================================================== */

void ACS_ACSC_Logmaint::cleanSCXLogFilesCoreMiddleWare()
{
	DEBUG("%s", "Entering cleanSCXLogFilesCoreMiddleWare");
	int nodeState = 0;
        ACS_PRC_API prcObj;
#if 0
	//Register the exit handler for Core MiddleWare Health Check.
	if( ThrExitHandler::init() == false )
	{
		ERROR("%s", "Error occured while registering exit handler");
		ERROR("%s", "Leaving cleanSCXLogFilesCoreMiddleWare");
		return ;
	}
#endif
	_ACS_LOGM_SERVICEPARAMS ptrParams;
	memset( &ptrParams, 0, sizeof( _ACS_LOGM_SERVICEPARAMS ));

	DEBUG("%s","Fetching service trigger parameters from IMM...\n");
	DEBUG("%s","ACS_ACSC_Logmaint::cleanSCXLogFilesCoreMiddleWare() - Calling GetServiceparams() ");
	ACE_INT32 serviceParamsStatus = GetServiceparams(&ptrParams);

	DEBUG("Status of fetching service trigger parameters from IMM : %d",serviceParamsStatus);

	if(serviceParamsStatus == SUCCESS )
	{
		DEBUG("%s","All parameters retrieved successfully from  IMM!!! \n");

		DEBUG("Received service parameters (Hour) from IMM : %d",(ptrParams.iTimeHH));
		DEBUG("Received service parameters (MIN) from IMM :  %d",(ptrParams.iTimeMM));
		DEBUG("Received service parameters (FREQ) from IMM : %d",(ptrParams.iFreq));
		DEBUG("Received service parameters (LogTypeCount) from IMM :  %d",(ptrParams.iLogTypCnt));
		DEBUG("Received service parameters (iStsTrigger) from IMM : %d\n",(ptrParams.iStsTrg));
		DEBUG("Received service parameters (iFileCntFreq) from IMM : %d\n",(ptrParams.iFileCntFreq));

		//Case of SCX Log Files. Every 15 Min NUmber of files is counted and if gteater than 
		//threshold value remove extra files.

		ACE_UINT32 dwTimerLowValue = 0;
		ACE_UINT32 dwTimerHighValue = 0;
		ACE_Time_Value oTimeValue(10);
		while( m_phEndEvent->wait(&oTimeValue,0))
		{
			nodeState = prcObj.askForNodeState();
                	if(nodeState == 1)//Active node
       			{ 
				DEBUG("%s","Cleanup activity started for SCX on active side...\n");

				DeleteExtraFiles(ptrParams.iLogTypCnt,dwTimerLowValue,
					dwTimerHighValue);
				DEBUG("%s","Cleanup activity stopped for SCX...\n");

			}
			else if(nodeState == 2) //Passive node
			{
				DEBUG("%s","Cleanup activity not started for SCX as passive side...\n");
			}
			else
        		{
				DEBUG("%s","Node State is Undefined!! in ACS_ACSC_Logmaint::cleanSCXLogFilesCoreMiddleWare()...\n");
       			} 



			 DEBUG("%s","ACS_ACSC_Logmaint::cleanSCXLogFilesCoreMiddleWare() - Calling GetServiceparams() in wait call");

			serviceParamsStatus = GetServiceparams(&ptrParams);

			if( serviceParamsStatus  == SUCCESS )
			{
				double dSecondsToWait = 0;

				dSecondsToWait = 60*ptrParams.iFileCntFreq;

				//ACE_Time_Value oTimeValueDay(dSecondsToWait);
				//oTimeValue = oTimeValueDay;	

				DEBUG("Sleeping for %f seconds dSecondsToWait in cleanSCXLogFilesCoreMiddleWare  ...",dSecondsToWait);
				ACE_Time_Value oTimeValueDay;
				oTimeValueDay.set(dSecondsToWait);
				oTimeValue = oTimeValueDay;	
				DEBUG("Sleeping for %d seconds in cleanSCXLogFilesCoreMiddleWare ...",oTimeValue.sec());

			}
			else
			{
				ERROR("%s", "Error occured while reading Service Parameters");
				return;
			}
		}//end of while

	}
	DEBUG("%s","ACS_ACSC_Logmaint SCX Thread Exiting....");
	DEBUG("%s", "Leaving cleanSCXLogFilesCoreMiddleWare ");
}//End of cleanSCXLogFilesCoreMiddleWare


/*===================================================================
	ROUTINE: getCurrentTime
=================================================================== */

struct tm* ACS_ACSC_Logmaint :: getCurrentTime()
{
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	return timeinfo;
}//End of getCurrentTime


/*===================================================================
   ROUTINE: GetLogFileParams
=================================================================== */

ACE_INT32  ACS_ACSC_Logmaint:: GetLogFileParams(_ACS_LOGM_LOGFILESPEC *ptrParams,
		ACE_INT32 iIndex)
{
	DEBUG("%s", "Entering New GetLogFileParams");

	std::string myLogName(""), myLogParamDN("");
	int logNoofFiles = 0;
	int logNoofDays = 0;
	switch(iIndex)
	{
//	case 1:
//		myLogParamDN = LOGM_DATASTSIN_OBJ_DN;
//		break;
//	case 2:
//		myLogParamDN = LOGM_DATASTSAG_OBJ_DN;
//		break;
//	case 3:
//		myLogParamDN = LOGM_DDIRM_OBJ_DN;
//		break;
	case 1:
		myLogParamDN = LOGM_OUTPUTSTSIN_OBJ_DN;
		logNoofDays = 3;
		break;
	case 2:
		myLogParamDN = LOGM_OUTPUTSTSAG_OBJ_DN;
		logNoofDays = 60;
		break;
	case 3:
		myLogParamDN = LOGM_TRALOG_OBJ_DN;
		logNoofDays = 7;
		break;
	case 4:
		myLogParamDN = LOGM_SCXLG_OBJ_DN;
		logNoofFiles =3;
		logNoofDays = 7;
		break;
	case 5:
		myLogParamDN = LOGM_CMXLG_OBJ_DN;
		logNoofFiles =3;
		logNoofDays = 7;
		break;
	default:
		return PAR_ERROR;
		break;

	}

	std::string strLogFileParam("");

	strLogFileParam.append(myLogParamDN);
	strLogFileParam.append(",");
	strLogFileParam.append(ACS_LOGM_Common::parentObjDNofLOGM);
	ACS_CC_ImmParameter paramToFind;
#if 0
	DEBUG("Fetching %s from IMM", SERVICE_LOG_NAME );

	paramToFind.attrName = (char*)SERVICE_LOG_NAME;


	if(  theOmHandler.getAttribute( strLogFileParam.c_str(), &paramToFind )  == ACS_CC_SUCCESS )
	{
		std::string myString((reinterpret_cast<char*>(*(paramToFind.attrValues))));
		ptrParams->strPath_LOGNAME = myString;
		//ACE_OS::strcpy(ptrParams->strPath_LOGNAME, (reinterpret_cast<char*>(*(paramToFind.attrValues))));
		DEBUG( "%s : %s ", SERVICE_LOG_NAME , ptrParams->strPath_LOGNAME.c_str() ) ;
		
		if((ACE_OS::strcmp(ptrParams->strPath_LOGNAME.c_str(), "TRALOG") != 0) &&
		   (ACE_OS::strcmp(ptrParams->strPath_LOGNAME.c_str(), "OUTPUTSTSIn") != 0) &&
		   (ACE_OS::strcmp(ptrParams->strPath_LOGNAME.c_str(), "OUTPUTSTSag") != 0) &&
   		   (ACE_OS::strcmp(ptrParams->strPath_LOGNAME.c_str(), "SCXlg") != 0) &&
		   (ACE_OS::strcmp(ptrParams->strPath_LOGNAME.c_str(), "CMXlg") != 0))
		{
		
			DEBUG( "%s ","Invalid  SERVICE_LOG_NAME " ) ;
			return PAR_ERROR;
		}
		
	}

        if (!myLogParamDN.empty())

        {
                int position=myLogParamDN.find("=");

                ptrParams->strPath_LOGNAME=myLogParamDN.substr(position+1,strlen(myLogParamDN.c_str()));
        }
	
#endif
	if (!myLogParamDN.empty())

	{
		int position=myLogParamDN.find("=");

		ptrParams->strPath_LOGNAME=myLogParamDN.substr(position+1,strlen(myLogParamDN.c_str()));
	}
	else
	{
		ACE_TCHAR szText[128] = { 0 };
		snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
				ACE_TEXT("%d:Error occured while reading IMM parameter  \"%s\""),
				__LINE__,
				SERVICE_LOG_NAME);

		//! Error. send event
		(void)ACS_LOGM_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szText,
				READ_PARAM_ERROR);

		ERROR("%s", "Leaving GetLogFileParams");
		return PAR_ERROR;
	}
	DEBUG("Fetching %s from IMM", SERVICE_LOG_PATH );
	paramToFind.attrName = (char*)SERVICE_LOG_PATH;

	if( theOmHandler.getAttribute( strLogFileParam.c_str(), &paramToFind)  == ACS_CC_SUCCESS )
	{

		std::string myString((reinterpret_cast<char*>(*(paramToFind.attrValues))));
		struct stat attrib;

		DEBUG("%s : %s", " File path to be checked for existence", myString.c_str());

		if(!stat(myString.c_str() ,&attrib))
		{
			DEBUG("%s", "LOG FILE PATH FOUND");
		}
		else
		{
			DEBUG("%s","Invalid log file path!!!");
			return PAR_ERROR;
		}
				
		ptrParams->strPath = myString;
		//changes done due to delete empty MP folder
//		if(iIndex == 1)
//		{
//			theDataSTSPath = myString;
//		}
		if(iIndex == 1)
		{
			theOutputSTSPath = myString;
		}
		//	ACE_OS::strcpy(ptrParams->strPath, (reinterpret_cast<char*>(*(paramToFind.attrValues))));
		DEBUG( "%s : %s ", SERVICE_LOG_PATH , ptrParams->strPath.c_str() ) ;
	}
	else
	{
		ACE_TCHAR szText[128];
		snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
				ACE_TEXT("%d:Error occured while reading IMM parameter \"%s\""),
				__LINE__,
				SERVICE_LOG_PATH);

		(void)ACS_LOGM_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szText,
				READ_PARAM_ERROR);

		ERROR("%s", "Leaving GetLogFileParams");
		return PAR_ERROR;
	}

	DEBUG("Fetching %s from IMM", SERVICE_LOG_TYPE );

	paramToFind.attrName = (char*)SERVICE_LOG_TYPE;

	if( theOmHandler.getAttribute( strLogFileParam.c_str(), &paramToFind )  == ACS_CC_SUCCESS )
	{
		std::string myString((reinterpret_cast<char*>(*(paramToFind.attrValues))));
#if 0
		string myLocalLogType = myString;
		std::string myRecLogType(myString);
		if((myRecLogType.compare("*.ddi") != 0))
		{
			if((myRecLogType.compare("*.log") != 0))
			{

				if((myRecLogType.compare("*.*") != 0))
				{
					DEBUG( "%s", "Invalid LOG TYPE  ");
					ERROR("%s", "Leaving GetLogFileParams");
					return PAR_ERROR;
					
				}
			}
		}
#endif
		ptrParams->strType = myString;
		//ACE_OS::strcpy(ptrParams->strType, (reinterpret_cast<char*>(*(paramToFind.attrValues))));
		DEBUG( "%s : %s ", SERVICE_LOG_TYPE , ptrParams->strType.c_str() ) ;
	}
	else
	{
		ACE_TCHAR szText[128] = {0 };
		snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
				ACE_TEXT("%d:Error occured while reading IMM parameter \"%s\""),
				__LINE__,
				SERVICE_LOG_TYPE);

		(void)ACS_LOGM_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szText,
				READ_PARAM_ERROR);

		ERROR("%s", "Leaving GetLogFileParams");
		return PAR_ERROR;
	}

	DEBUG("Fetching %s from IMM", SERVICE_LOG_OPERATION );

	paramToFind.attrName = (char*)SERVICE_LOG_OPERATION;

	if( theOmHandler.getAttribute( strLogFileParam.c_str(), &paramToFind )  == ACS_CC_SUCCESS )
	{
		ptrParams->iNoOfFiles = *((ACE_UINT32*)(*(paramToFind.attrValues)));
		DEBUG( "%s : %d ", SERVICE_LOG_OPERATION , ptrParams->iNoOfFiles ) ;
	/*	if(ptrParams->iNoOfFiles <  0)
		{
			ptrParams->iNoOfFiles = logNoofFiles;
			DEBUG("%s : %d", "NoOfFiles ia given an invalid number so setting default value ", ptrParams->iNoOfFiles);
		}
        */
	}
	else
	{
		ACE_TCHAR szText[128] = { 0 };
		snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
				ACE_TEXT("%d:Error occured while reading IMM parameter  \"%s\""),
				__LINE__,
				SERVICE_LOG_OPERATION);
		(void)ACS_LOGM_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szText,
				READ_PARAM_ERROR);

		ERROR("%s", "Leaving GetLogFileParams");
		return PAR_ERROR;
	}

	DEBUG("Fetching %s from IMM", SERVICE_LOG_OLD_DAYS );
	paramToFind.attrName = (char*)SERVICE_LOG_OLD_DAYS;

	if( theOmHandler.getAttribute( strLogFileParam.c_str(), &paramToFind )  == ACS_CC_SUCCESS )
	{
		ptrParams->iNoOFDays = *((ACE_UINT32*)(*(paramToFind.attrValues)));
		DEBUG( "%s : %d ", SERVICE_LOG_OLD_DAYS , ptrParams->iNoOFDays ) ;
		if(ptrParams->iNoOFDays > 365)
		{
		        ptrParams->iNoOFDays =	logNoofDays;
			DEBUG("%s : %d", "SET DEFAULT VALUE TO iNoOFDays", ptrParams->iNoOFDays);
		}
	}
	else
	{
		ACE_TCHAR szText[128] = { 0 };
		snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
				ACE_TEXT("%d:Error occured while reading IMM parameter \"%s\""),
				__LINE__,
				SERVICE_LOG_OLD_DAYS);

		(void)ACS_LOGM_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szText,
				READ_PARAM_ERROR);
		ERROR("%s", "Leaving GetLogFileParams");
		return PAR_ERROR;
	}

	DEBUG("%s", "Leaving GetLogFileParams");
	return SUCCESS;

}//End of GetLogFileParams


/*===================================================================
   ROUTINE: GetServiceparams
=================================================================== */
ACE_INT32 ACS_ACSC_Logmaint:: GetServiceparams(_ACS_LOGM_SERVICEPARAMS *ptrParams)
{
	std::string dnObject("");
	dnObject.append(ACS_LOGM_Common::parentObjDNofLOGM);

	DEBUG("%s", "Entering New GetServiceparams");
	ACS_CC_ImmParameter paramToFind ;

	//Get srvtrghour
	paramToFind.attrName = (char*)  SERVICE_TRIGGER_HOUR;

	DEBUG("%s","Fetching service trigger Hour");

	if( theOmHandler.getAttribute( dnObject.c_str(), &paramToFind ) == ACS_CC_FAILURE )
	{
		ACE_TCHAR szText[128];
		snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
				ACE_TEXT("%d:Error when reading parameter \"%s\""),
				__LINE__,
				SERVICE_TRIGGER_HOUR);
		ERROR("%s", szText);
		//! PHA error. send event
		(void)ACS_LOGM_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szText,
				READ_PARAM_ERROR);
		ERROR("%s", "Leaving  GetServiceparams");
		return PAR_ERROR;
	}   

	ptrParams->iTimeHH = *( reinterpret_cast < int *> (*(paramToFind.attrValues))) ;

	DEBUG("Service Trigger Hour = %d", ptrParams->iTimeHH );
	//Value of iTimeHH should be in the range 0-23

	if(ptrParams->iTimeHH > 23) 
	{
		ptrParams->iTimeHH = 1;
		DEBUG("%s : %d", "Service Trigger Hour value is greater than 23 or less than 0, so assigning default value ", ptrParams->iTimeHH);	
	}


	//Get srvtrgminute

	paramToFind.attrName = (char*) SERVICE_TRIGGER_MINUTES;

	DEBUG("%s","Fetching service trigger minute");

	if( theOmHandler.getAttribute( dnObject.c_str(), &paramToFind ) == ACS_CC_FAILURE )
	{
		ACE_TCHAR szText[128];
		snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
				ACE_TEXT("%d:Error when reading parameter \"%s\""),
				__LINE__,
				SERVICE_TRIGGER_MINUTES);
		ERROR("%s", szText);

		//! PHA error. send event
		(void)ACS_LOGM_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szText,
				READ_PARAM_ERROR);
		ERROR("%s", "Leaving  GetServiceparams");
		return PAR_ERROR;
	}
	ptrParams->iTimeMM = *( reinterpret_cast < int *> (*(paramToFind.attrValues))) ;

	DEBUG("Service Trigger Minute = %d", ptrParams->iTimeMM );
	//Value of srvTrgMinutes should be in the range 0-59.

	if( ptrParams->iTimeMM > 59) 
	{
		ptrParams->iTimeMM = 0;
		DEBUG("%s : %d", "Service Trigger Minutes is greater than 59 or less than 0, assign default value", ptrParams->iTimeMM);
	}

	//Get srvtrgfrq
	DEBUG("%s","Fetching service trigger frequency");

	paramToFind.attrName = (char*) SERVICE_TRIGGER_FREQUENCY;

	if( theOmHandler.getAttribute( dnObject.c_str(), &paramToFind ) == ACS_CC_FAILURE )
	{
		ACE_TCHAR szText[128] = { 0 };
		snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
				ACE_TEXT("%d:Error when reading parameter \"%s\""),
				__LINE__,
				SERVICE_TRIGGER_FREQUENCY);

		(void)ACS_LOGM_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szText,
				READ_PARAM_ERROR);
		ERROR("%s", szText);
		ERROR("%s", "Leaving  GetServiceparams");
		return PAR_ERROR;
	}

	ptrParams->iFreq = *( reinterpret_cast < int *> (*(paramToFind.attrValues))) ;

	DEBUG("Service Trigger Frequency = %d",  ptrParams->iFreq);

	if( ptrParams->iFreq != 0 && ptrParams->iFreq != 1)
	{
		ptrParams->iFreq = 0;
		DEBUG("%s : %d", " srvTrgFreq value is neither 0 nor 1. Assigning Default Value", ptrParams->iFreq);
	}

	//Get LogTypCount
	DEBUG("%s","Fetching log type count.");

	/*paramToFind.attrName = (char*) LOG_FILE_COUNT;

	if( theOmHandler.getAttribute( dnObject.c_str(), &paramToFind ) == ACS_CC_FAILURE )
	{
		ACE_TCHAR szText[128] = { 0 };
                snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
                        ACE_TEXT("%d:Error when reading parameter \"%s\""),
                        __LINE__,
                        LOG_FILE_COUNT);

		ERROR("%s", szText);

		//! PHA error. send event
		(void)ACS_LOGM_AEH::ReportEvent(1003,
										ACE_TEXT("EVENT"),
										CAUSE_AP_INTERNAL_FAULT,
										ACE_TEXT(""),
										szText,
										READ_PARAM_ERROR);
		ERROR("%s", "Leaving  GetServiceparams");
                return PAR_ERROR;
        }*/
	//LOGM_LOG_PARAM_CLASS_NAME
	std::vector<std::string> dnList;

	if (theOmHandler.getClassInstances(LOGM_LOG_PARAM_CLASS_NAME, dnList) != ACS_CC_SUCCESS)
	{   //ERROR
		ERROR("%s", "Error occured in getting class instances");
		ACE_TCHAR szText[128] = { 0 };
		snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
				ACE_TEXT("%d:Error when reading parameter \"%s\""),
				__LINE__,
				LOG_FILE_COUNT);

		ERROR("%s", szText);

		//! PHA error. send event
		(void)ACS_LOGM_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szText,
				READ_PARAM_ERROR);
		ERROR("%s", "Leaving  GetServiceparams");
		return PAR_ERROR;
	}

	//ptrParams->iLogTypCnt = *( reinterpret_cast < int *> (*(paramToFind.attrValues))) ;
	ptrParams->iLogTypCnt = dnList.size();
	DEBUG("Log Type Count = %d", ptrParams->iLogTypCnt );

	if(ptrParams->iLogTypCnt != 7)
	{
		ptrParams->iLogTypCnt = 4;
		DEBUG("%s : %d", "LogTypeCnt is not equal to 7. Assigning default value ", ptrParams->iLogTypCnt);
	}

	//Get ststrag
	DEBUG("%s","Fetching service trigger value");

	paramToFind.attrName = (char*) STS_STATUS_LOG_DEL;

	if( theOmHandler.getAttribute( dnObject.c_str(), &paramToFind ) == ACS_CC_FAILURE )
	{
		ACE_TCHAR szText[128] = { 0 };
		snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
				ACE_TEXT("%d:Error when reading parameter \"%s\""),
				__LINE__,
				STS_STATUS_LOG_DEL);

		//! PHA error. send event
		(void)ACS_LOGM_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szText,
				READ_PARAM_ERROR);
		ERROR("%s", "Leaving  GetServiceparams");
		return PAR_ERROR;
	}
	ptrParams->iStsTrg = *( reinterpret_cast < int *> (*(paramToFind.attrValues))) ;

	DEBUG("Service Trigger Value = %d", ptrParams->iStsTrg );

	if( ptrParams->iStsTrg != 0 && ptrParams->iStsTrg != 1)
	{
		ptrParams->iStsTrg = 1;
		DEBUG("%s : %d", " Service trigger Status del count is neither 0 nor 1. Assigning default Value", ptrParams->iStsTrg );
	}

	//Case of SCXlg  or CMXlg 
	//Get fileCntFreq
	DEBUG("%s","Fetching File count frequency");

	paramToFind.attrName = (char*) FILE_COUNT_FREQUNCY;

	if( theOmHandler.getAttribute( dnObject.c_str(), &paramToFind ) == ACS_CC_FAILURE )
	{
		ACE_TCHAR szText[128] = { 0 };
		snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
				ACE_TEXT("%d:Error when reading parameter \"%s\""),
				__LINE__,
				FILE_COUNT_FREQUNCY);

		//! PHA error. send event
		(void)ACS_LOGM_AEH::ReportEvent(1003,
				ACE_TEXT("EVENT"),
				CAUSE_AP_INTERNAL_FAULT,
				ACE_TEXT(""),
				szText,
				READ_PARAM_ERROR);
		ERROR("%s", "Leaving  GetServiceparams");
		return PAR_ERROR;
	}
	ptrParams->iFileCntFreq = *( reinterpret_cast < int *> (*(paramToFind.attrValues))) ;

	DEBUG("File Count Frequency = %d", ptrParams->iFileCntFreq);

	if( ptrParams->iFileCntFreq > 59 )
	{
		ptrParams->iFileCntFreq = 15;
		DEBUG("%s : %d", "fileCntFreq value is greater than 59. Assigning default value", ptrParams->iFileCntFreq);
	}

	return SUCCESS;

}//End of GetServiceparams


/*===================================================================
   ROUTINE: GetInitialTimeToWait
=================================================================== */

ACE_INT32 ACS_ACSC_Logmaint:: GetInitialTimeToWait(_ACS_LOGM_SERVICEPARAMS *ptrParams,double *dSecondsToWait)
{

	bool bResult=false;

	struct tm *currTime;
	struct tm *serviceTriggerTime;

	currTime=getCurrentTime();
	time_t currTimeT = mktime(currTime);
	DEBUG("Current system time is : %s",asctime(currTime));

	serviceTriggerTime=currTime;

	serviceTriggerTime->tm_hour= (int)ptrParams->iTimeHH;
	serviceTriggerTime->tm_min=(int)ptrParams->iTimeMM;
	serviceTriggerTime->tm_sec=0;

	time_t serviceTriggerTimeT = mktime(serviceTriggerTime);
	DEBUG("Service trigger time is : %s\n",asctime(serviceTriggerTime));

	double elapsedTimeDifference = difftime(serviceTriggerTimeT,currTimeT);
	DEBUG("Difference between current time and service trigger time (In Seconds) : %f\n",(elapsedTimeDifference));

	if(elapsedTimeDifference>0)
	{
		DEBUG("Service trigger time not yet reached. Waiting for %f Seconds\n\n\n",(elapsedTimeDifference));
		*dSecondsToWait = elapsedTimeDifference;
		bResult = true;
	}
	else
	{
		DEBUG("%s","Service trigger time already elapsed!!!\n\n");
		*dSecondsToWait=elapsedTimeDifference;
		bResult = true;
	}

	return bResult;
}//End of GetInitialTimeToWait


/*===================================================================
   ROUTINE: isDirectory
=================================================================== */

bool ACS_ACSC_Logmaint::isDirectory(string scannedPath)
{
	const char* pzPath = scannedPath.c_str();
	if(pzPath == NULL) return false;

	DIR *pDir;
	bool bExists = false;
	pDir = opendir(pzPath);
	if(pDir != NULL)
	{
		bExists = true;
		(void) closedir (pDir);
	}

	return bExists;

}//End of isDirectory



/*===================================================================
   ROUTINE: ElapsedTimeCal
=================================================================== */

double ACS_ACSC_Logmaint::ElapsedTimeCal(struct tm *stFile)
{
	struct tm		*stTime;
	double			dElapsedTime;
	time_t			currTime,fileTime;

	fileTime = mktime(stFile);

	stTime		 = getCurrentTime();
	currTime = mktime(stTime);

	dElapsedTime = difftime( currTime,fileTime );

	return dElapsedTime;
}//End of ElapsedTimeCal



/*===================================================================
   ROUTINE: convertToString
=================================================================== */

/*
 * TODO: To be refactored
 */
string ACS_ACSC_Logmaint::convertToString(char* recString)
{
	string word;
	word = recString;
	return word;
}//End of convertToString


/*===================================================================
   ROUTINE: FindFileAndFolders
=================================================================== */

void ACS_ACSC_Logmaint :: FindFileAndFolders(string sRelativePath,_ACS_LOGM_LOGFILESPEC *ptrParams , int iTrgType )
{
	FHANDLE					hFindFile = NULL;
	BOOL					fMoreFiles = FALSE;
	LIN_64_FIND_DATA		fdFindFileData;
	deque<FileDetails> 		filesList;
	string					sFilePattern, sDirName, sWildCard;
	double					dElapsedTime;
	std::string myLogPath;
	if(!strcmp("ddiRm", ptrParams->strPath_LOGNAME.c_str()))
	{
		sWildCard = "*.ddi";
		DEBUG("NUMBER OF RECENT *.ddi FILES TO BE RETAINED : %d\n\n",(ptrParams->iNoOFDays));
	}
	else if(!strcmp("TRALOG", ptrParams->strPath_LOGNAME.c_str()))
	{
		sWildCard = ptrParams->strType;
		DEBUG("TRA LOG DELETION AGE : %d\n",(ptrParams->iNoOFDays));
	}
	else
	{
		sWildCard = "*.*";
		DEBUG("MP FOLDER DELETION AGE : %d\n\n",(ptrParams->iNoOFDays));
		myLogPath = ptrParams->strPath;
	}

	sFilePattern  += sRelativePath;
	sFilePattern  +=  sWildCard;
	hFindFile = FindFirstFile(sFilePattern.c_str(), &fdFindFileData);
	printDeque();

	if ((INVALID_HANDLE_VALUE == (int)hFindFile)||(ERROR_FILE_NOT_FOUND == (int)hFindFile))
	{
		ERROR(" %s","Incorrect path or file not found!!!!\n");
		return;
	}
	else
	{
		fMoreFiles = true;
	}


	while ((fMoreFiles))
	{
		string completeFilePath = (fdFindFileData.cFileName);
		if ((isDirectory((fdFindFileData.cFileName))==true))
		{
			if ((strcmp( ".", ((extractFileNameFrompath(convertToString(fdFindFileData.cFileName)))).c_str()) != 0) && (strcmp("..",(extractFileNameFrompath(convertToString(fdFindFileData.cFileName))).c_str()) != 0))
			{
				sDirName = fdFindFileData.cFileName;
			}
		}
		// To delete the absolete ddi files
#if 0
		else if(!strcmp("ddiRm", ptrParams->strPath_LOGNAME.c_str()))
		{
			dElapsedTime = ElapsedTimeCal(fdFindFileData.ftFileCreationTime);

			filesList.push_back(FileDetails(fdFindFileData.cFileName,dElapsedTime));
		}
#endif
		else if(!strcmp("TRALOG", ptrParams->strPath_LOGNAME.c_str()))
		{
			//char strPath_Append[MAX_FILE_PATH];
			BOOL fDeleted = FALSE;
			DEBUG("Searching for TRA log files older than %d days...\n",(ptrParams->iNoOFDays));
			//strcpy(strPath_Append,ptrParams->strPath.c_str());

			DEBUG("File Name : %s",(fdFindFileData.cFileName));
			DEBUG("File Creation Time : %s",(asctime(fdFindFileData.ftFileCreationTime)));

			dElapsedTime = ElapsedTimeCal( fdFindFileData.ftFileCreationTime );

			if (dElapsedTime < (SECONDS_IN_DAY * ptrParams->iNoOFDays))
			{
				//					ACE_TCHAR arr[256];
				//					ACE_OS::memset(arr,0, sizeof(arr));
				//					ACE_OS::strcpy(arr,"Age of the log file");
				//					ACE_OS::strcat(arr,fdFindFileData.cFileName);
				//					ACE_OS::strcat(arr," is less!\n");
//				std::string myString;
//				myString.append("Age of the log file ");
//				myString.append(fdFindFileData.cFileName);
//				myString.append(" is less!\n");
//
//				DEBUG("%s",myString.c_str());

				fMoreFiles = FindNextFile(&hFindFile, &fdFindFileData);
				continue;
			}

			DEBUG("Deleting file : %s\n",fdFindFileData.cFileName);
			fDeleted = removeFile(fdFindFileData.cFileName);
			if (!fDeleted)
			{
				DEBUG("Log file deletion attempt failed!! Attempting to delete again after %d seconds...\n",(SLEEP_IF_FIRST_DELETION_FAILS));
				sleep(SLEEP_IF_FIRST_DELETION_FAILS);
				DEBUG("%s","Removing file...\n");
				fDeleted = removeFile(fdFindFileData.cFileName);
				if (!fDeleted)
				{
					ACE_TCHAR szText[128];
					/*
					 * TODO: Implement GetLastError() function and change the error messages
					 */
					snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
							ACE_TEXT("%d:Can not delete log file \"%s\""),
							__LINE__,
							fdFindFileData.cFileName);

					//! Log file deletion error. send event
					(void)ACS_LOGM_AEH::ReportEvent(1003,
							ACE_TEXT("EVENT"),
							CAUSE_AP_INTERNAL_FAULT,
							ACE_TEXT(""),
							szText,
							LOG_DELETION_ERROR);
					DEBUG("%s","ERROR DELETING LOG FILE!!!\n");
					break;
				}
			}
		}
		else // it's a file
		{
			string sSubDummy("Dummy");
			string sSubStatus("status");
			string sFileName = extractFileNameFrompath(fdFindFileData.cFileName);
			string sSubString = "";

			sSubString = sFileName.substr(0,5);
			int nCompare = sSubString.compare(sSubDummy);

			if(sFileName.size() > 5 && nCompare)
			{
				sSubString = sFileName.substr(0,6);
			}
			else
			{
				sSubString = sFileName.substr(0,5);
			}

			if(sSubString.compare(sSubDummy) == 0)
			{
				DEBUG(" \nDummy file found in MP folder. Path of this file is %s",fdFindFileData.cFileName);
				char cBuffer[1024];
				memset(cBuffer,'\0',1024);
				sRelativePath = searchPathFromWildCard(fdFindFileData.cFileName);
				dElapsedTime = ElapsedTimeCal( getTimeStampOfFile(sRelativePath));
				DEBUG(" \nCreation time of MP folder : %s",asctime(getTimeStampOfFile(sRelativePath)));

				if (dElapsedTime < (SECONDS_IN_DAY * ptrParams->iNoOFDays))
				{
					DEBUG("Age of the MP folder %s is less for deletion\n",fdFindFileData.cFileName);
					fMoreFiles = FindNextFile(&hFindFile, &fdFindFileData);
					continue;
				}

				DEBUG("%s","MP folder is old!! Deleting the MP folder...\n");
				bool nSuccess = removedirectoryrecursively(sRelativePath.c_str(),myLogPath);
				//Deleting other status containing directory
				if(nSuccess)
                                {
                                        DEBUG("%s","Calling FindFileAndFolders() recursively...");
                                        DEBUG("%s","It updates log file container after every deletion of directory containing Dummy file");
                                        FindFileAndFolders(myLogPath,ptrParams ,iTrgType);

                                }
                                else
                                {
					DEBUG("MP folder deletion attempt failed!! Attempting to delete again after %d seconds...\n",SLEEP_IF_FIRST_DELETION_FAILS);
					sleep(SLEEP_IF_FIRST_DELETION_FAILS);
					DEBUG("Deleting MP folder %s ...\n",sRelativePath.c_str());
					int nFlag = removedirectoryrecursively(sRelativePath.c_str(),myLogPath);

					if (!nFlag)
					{
						ACE_TCHAR szText[128];
						/*
						 * TODO: Implement GetLastError() function and change the error messages
						 */
						snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
								ACE_TEXT("%d:Can not delete folder \"%s\""),
								__LINE__,
								sRelativePath.c_str());

						//! Log file deletion error. send event
						(void)ACS_LOGM_AEH::ReportEvent(1003,
								ACE_TEXT("EVENT"),
								CAUSE_AP_INTERNAL_FAULT,
								ACE_TEXT(""),
								szText,
								FOLDER_DELETION_ERROR);

						DEBUG("%s","\nERROR : UNABLE TO DELETE MP FOLDER!!!\n\n" );
						return;
					}
				}
			}

			else
			{
				if ((sSubString.compare(sSubStatus) == 0) && (isStatusActive == true))
				{

					DEBUG("status file found . Path of this file is %s",fdFindFileData.cFileName);

					char cBuffer[1024];
					memset(cBuffer,'\0',1024);
					sRelativePath.copy(cBuffer,(sRelativePath.size()-1));// Need to convert String to char for SHFILEOPSTRUCT
					sRelativePath = searchPathFromWildCard(fdFindFileData.cFileName);

					dElapsedTime = ElapsedTimeCal( getTimeStampOfFile(sRelativePath));
					DEBUG(" \nCreation time of folder containing status file: %s",asctime(getTimeStampOfFile(sRelativePath)));

					if (iTrgType == 0)
					{
						break;
					}

					if(dElapsedTime < (SECONDS_IN_DAY * ptrParams->iNoOFDays))
					{

						DEBUG("%s","Age of the MP folder age is less!\n");
						fMoreFiles = FindNextFile(&hFindFile, &fdFindFileData);
						continue;
					}

					DEBUG("MP folder %s is old!! Deleting the MP folder...\n",sRelativePath.c_str());
					bool nSuccess = removedirectoryrecursively(sRelativePath.c_str(),myLogPath);
					//Deleting other status containing directory 
					if(nSuccess)
                                        {
                                                DEBUG("%s","Calling FindFileAndFolders() recursively...");
                                                DEBUG("%s","It updates log file container after every deletion of directory containing status file");
                                                FindFileAndFolders(myLogPath,ptrParams ,iTrgType);

                                        }
                                        else
                                        {
						DEBUG("MP folder deletion attempt failed!! Attempting to delete again after %d seconds...\n",SLEEP_IF_FIRST_DELETION_FAILS);
						sleep(SLEEP_IF_FIRST_DELETION_FAILS);
						DEBUG("Deleting MP folder %s ...\n",sRelativePath.c_str());

						int nFlag = removedirectoryrecursively(sRelativePath.c_str(),myLogPath);

						if (!nFlag)
						{
							DEBUG("%s","\nERROR : UNABLE TO DELETE MP FOLDER!!!\n\n" );
							return;
						}
					}
				}
			}
		}

		fMoreFiles = FindNextFile(&hFindFile, &fdFindFileData);

	}
#if 0
	if(hFindFile != 0)
		// sort .ddi files based on elapsed time
		if(!filesList.empty())
		{
			deque<FileDetails>::iterator p1= filesList.begin();
			if(filesList.size() > ptrParams->iNoOFDays)
			{
				for (int i = 0; i < (int)filesList.size() - 1; i++)
				{
					for (int j = 0; j < ((int)filesList.size()) - i -1; j++)
					{
						if(((p1+j)->ftElapsedTime) < ((p1+(j+1))->ftElapsedTime))
						{
							double temptime = (p1+j)->ftElapsedTime;
							char tempcFileName[256];
							strcpy(tempcFileName,((p1+j)->cFileName));
							(p1+j)->ftElapsedTime = (p1+j+1)->ftElapsedTime;
							strcpy(((p1+j)->cFileName),((p1+j+1)->cFileName));
							(p1+j+1)->ftElapsedTime =  temptime;
							strcpy((p1+j+1)->cFileName,tempcFileName);
						}
					}
				}

				// Delete absolete .ddi files
				//Here for this functionality ,iNoOfdays is used as number of .ddi files to retain
				DEBUG("%s","Deleting ddi files except latest three ones...\n" );
				while(filesList.size() > ptrParams->iNoOFDays )
				{
					char fnTemp[256];
					deque<FileDetails>::iterator p2= filesList.begin();
					strcpy(fnTemp,p2->cFileName);
					DEBUG("Deleting %s\n",(p2->cFileName));
					remove(fnTemp);
					(void)filesList.erase(filesList.begin());
				}
			}
		}
#endif

	return;
}//End of FindFileAndFolders




/*===================================================================
   ROUTINE: isWildCard
=================================================================== */

BOOL ACS_ACSC_Logmaint::isWildCard(string wildCardString)
{
	bool status = false;

	for(unsigned int i=0;i<wildCardString.length();++i)
	{
		if(wildCardString[i]=='*')
		{
			status = true;
		}
	}

	return status;
}//End of isWildCard


/*===================================================================
   ROUTINE: wildCardMatch
=================================================================== */


BOOL ACS_ACSC_Logmaint::wildCardMatch(string wildCardString, string path)
{
	bool result = false;
	if(isWildCard(wildCardString))
	{
		if(wildCardString[wildCardString.length()-1] == '*')
		{
			string noWildCardString;

			for(unsigned int i=0; i<wildCardString.length()-1;++i)
			{
				noWildCardString +=  wildCardString[i];
			}

			if(path.find(noWildCardString)!=string::npos)
			{
				result = true;
			}
			else
			{
				result = false;
			}
		}

		if((wildCardString[wildCardString.find('.')-1] == '*')&&((wildCardString[wildCardString.find('.')+1] != '*')))
		{
			string noWildCardString;
			bool flag = false;

			for(unsigned int i=0; i<wildCardString.length();++i)
			{
				if(wildCardString[i]=='*')
				{
					flag = true;
					++i;
				}
				if(flag == true)
					noWildCardString +=  wildCardString[i];
			}

			if(path.find(noWildCardString)!=string::npos)
			{
				result = true;
			}
			else
			{
				result = false;
			}
		}

		if((wildCardString[wildCardString.find('.')-1] == '*')&&(wildCardString[wildCardString.find('.')+1] == '*'))
		{
			if((isDirectory(path)==0))
			{
				result = true;
			}
			else
			{
				result = false;
			}
		}
	}
	else
	{
		if(path.find(wildCardString)!=string::npos)
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}
	return result;
}//End of wildCardMatch


/*===================================================================
   ROUTINE: DeleteLogFiles
=================================================================== */


VOID ACS_ACSC_Logmaint:: DeleteLogFiles (ACE_UINT32 iLogTypCnt, ACE_UINT32 dwTimerLowValue, ACE_UINT32 dwTimerHighValue)
{
	//	 BOOL fDeleted = FALSE;
	//LIN_64_FIND_DATA	FileData,fdFindFileData;
	_ACS_LOGM_SERVICEPARAMS rServiceParams;
	_ACS_LOGM_LOGFILESPEC ptrParams;
	//BOOL              fMoreFiles=false;
	FHANDLE			hSearch = NULL;//,hHandle = NULL;
	//FHANDLE					hFindFile = NULL;
	//double			dElapsedTime;
	string sDirName;
	//char strPath_Append[MAX_FILE_PATH];
	DEBUG("%s","ACS_ACSC_Logmaint::DeleteLogFiles() - Calling GetServiceparams()");

	ACE_INT32 serviceParamsStatus = GetServiceparams(&rServiceParams);

	if(!serviceParamsStatus)
	{
		ERROR("%s %d %d","Error reading service trigger parameters from IMM Database",dwTimerLowValue,dwTimerHighValue);
		return;
	}

	for(unsigned int i = 0 ; i < iLogTypCnt ; i++)
	{
		//  BOOL fDeleted = FALSE,fFinished = FALSE,fSearchFirstFile = true;
		DEBUG("%s","Loading log file parameters from PHA : \n");

		ACE_INT32 status = GetLogFileParams(&ptrParams,i+1);

		if(status)
		{
			DEBUG("%s","These parameters are : \n");
			DEBUG("LOGNAME : %s",ptrParams.strPath_LOGNAME.c_str());
			DEBUG("LOG PATH : %s",ptrParams.strPath.c_str());
			DEBUG("LOG TYPE : %s",ptrParams.strType.c_str());
			DEBUG("iNoOFDays : %d",ptrParams.iNoOFDays);
			DEBUG("NoOfFiles : %d",ptrParams.iNoOfFiles);
		}
		else
		{
			ERROR("%s","Error in fetching log file parameters from PHA..\n");
			continue;
		}


#if 0
		// cni apz21230/4-754
		string FTPLOG=(string)ptrParams.strPath+"/"+(string)ptrParams.strType;
		if(!strcmp("FTLOG",ptrParams.strPath_LOGNAME.c_str()))
		{
			DEBUG("LOG DELETION AGE : %d\n",ptrParams.iNoOFDays);
			hFindFile = FindFirstFile(FTPLOG.c_str(), &fdFindFileData);
			printDeque();
			if ((INVALID_HANDLE_VALUE == (int)hFindFile)||(ERROR_FILE_NOT_FOUND == (int)hFindFile))
			{
				ERROR(" %s","Incorrect path or file not found!!!!\n");
				return;
			}
			else
			{
				fMoreFiles = true;
			}
			while ((fMoreFiles))
			{
				string completeFilePath = (fdFindFileData.cFileName);
				if ((isDirectory((fdFindFileData.cFileName))==true))
				{
					if ((strcmp( ".", ((extractFileNameFrompath(convertToString(fdFindFileData.cFileName)))).c_str()) != 0) && (strcmp("..",(extractFileNameFrompath(convertToString(fdFindFileData.cFileName))).c_str()) != 0))
					{
						sDirName = fdFindFileData.cFileName;
					}
				}
				//char strPath_Append[MAX_FILE_PATH];
				BOOL fDeleted = FALSE;
				DEBUG("Searching for FT log files older than %d days...\n",(ptrParams.iNoOFDays));
				//strcpy(strPath_Append,ptrParams.strPath);

				DEBUG("File Name : %s",(fdFindFileData.cFileName));
				DEBUG("File Creation Time : %s",(asctime(fdFindFileData.ftFileCreationTime)));

				dElapsedTime = ElapsedTimeCal( fdFindFileData.ftFileCreationTime );

				if (dElapsedTime < (SECONDS_IN_DAY * ptrParams.iNoOFDays))
				{
					ACE_TCHAR arr[256];
					ACE_OS::memset(arr,0, sizeof(arr));
					ACE_OS::strcpy(arr,"Age of the log file");
					ACE_OS::strcat(arr,fdFindFileData.cFileName);
					ACE_OS::strcat(arr," is less!\n");

					DEBUG("%s",arr);

					fMoreFiles = FindNextFile(&hFindFile, &fdFindFileData);
					continue;
				}

				DEBUG("Deleting file : %s\n",fdFindFileData.cFileName);
				fDeleted = removeFile(fdFindFileData.cFileName);
				if (!fDeleted)
				{
					ACE_TCHAR szText[128];
					snprintf(szText,sizeof(szText)/sizeof(*szText) - 1,
							ACE_TEXT("%d:Can not delete log file \"%s\""),__LINE__,FileData.cFileName);

					//! Log file deletion error. send event
					(void)ACS_LOGM_AEH::ReportEvent(1003,
							ACE_TEXT("EVENT"),
							CAUSE_AP_INTERNAL_FAULT,
							ACE_TEXT(""),
							szText,
							LOG_DELETION_ERROR);
					DEBUG("%s","ERROR DELETING LOG FILE!!!\n");

				}
			}

		}	// End of if loop
#endif

		////////////////////// START //////////////////////////////////////////////////
#if 0
		if(!strcmp("DATASTSIn",ptrParams.strPath_LOGNAME.c_str()))	//	 check for "STSin"
		{
			int nodeState = 0;
			ACS_PRC_API prcObj;
			nodeState = prcObj.askForNodeState();
			if(nodeState==1)//Active node
			{
				isStatusActive = false;
				string sRelativePath;
				sRelativePath = ptrParams.strPath;
				sRelativePath += "/";
				if((sRelativePath.compare("//") != 0) && (sRelativePath.compare("/") != 0))
				{
					FindFileAndFolders(sRelativePath.c_str(),&ptrParams , rServiceParams.iStsTrg);
				}
				else
				{
					ERROR("%s","sRelativePath Fetch from the imm is '/' or '//' which is not correct");
				}
			}
			else
			{
				DEBUG("%s","Node state is Passive");
			}
		}
		//else if(!strcmp("OUTPUTSTSIn",ptrParams.strPath_LOGNAME.c_str()))	//	 check for "STSin"

#endif
		 if(!strcmp("OUTPUTSTSIn",ptrParams.strPath_LOGNAME.c_str()))	//	 check for "STSin"
		{
			DEBUG("%s","Inside DeleteLogFiles():OUTPUTSTSIn ");
			int nodeState = 0;
			ACS_PRC_API prcObj;
			nodeState = prcObj.askForNodeState();
			if(nodeState==1)//Active node
			{
				isStatusActive = false;
				string sRelativePath;
				sRelativePath = ptrParams.strPath;
				sRelativePath += "/";
				if((sRelativePath.compare("//") != 0) && (sRelativePath.compare("/") != 0))
				{
					FindFileAndFolders(sRelativePath.c_str(),&ptrParams , rServiceParams.iStsTrg);
				}
				else
				{
					ERROR("%s","sRelativePath Fetch from the imm is '/' or '//' which is not correct");
				}
			}
			else
			{
				DEBUG("%s","Node state is Passive");
			}
		}
#if 0
		else if(!strcmp("DATASTSag", ptrParams.strPath_LOGNAME.c_str()))	//	 check for "STSag"
		{
			int nodeState = 0;
			ACS_PRC_API prcObj;
			nodeState = prcObj.askForNodeState();
			if(nodeState==1)//Active node
			{
				isStatusActive = true;
				string sRelativePath;
				sRelativePath = ptrParams.strPath;
				sRelativePath += "/";
				if((sRelativePath.compare("//") != 0) && (sRelativePath.compare("/") != 0))
				{
					FindFileAndFolders(sRelativePath.c_str(),&ptrParams , rServiceParams.iStsTrg);
				}
				else
				{
					ERROR("%s","sRelativePath Fetch from the imm is '/' or '//' which is not correct");
				}
			}
			else
			{
				DEBUG("%s","Node state is passive...");
			}
		}
#endif
		else if(!strcmp("OUTPUTSTSag", ptrParams.strPath_LOGNAME.c_str()))	//	 check for "STSag"
		{
			DEBUG("%s","Inside DeleteLogFiles():OUTPUTSTSag ");
			int nodeState = 0;
			ACS_PRC_API prcObj;
			nodeState = prcObj.askForNodeState();
			if(nodeState==1)//Active node
			{
				isStatusActive = true;
				string sRelativePath;
				sRelativePath = ptrParams.strPath;
				sRelativePath += "/";
				if((sRelativePath.compare("//") != 0) && (sRelativePath.compare("/") != 0))
				{
					FindFileAndFolders(sRelativePath.c_str(),&ptrParams , rServiceParams.iStsTrg);
				}
				else
				{
					ERROR("%s","sRelativePath Fetch from the imm is '/' or '//' which is not correct");
				}
			}
			else
			{
				DEBUG("%s","Node state is Passive");
			}
		}
		else if(!strcmp("TRALOG", ptrParams.strPath_LOGNAME.c_str()))	//	 check for "TRAlog"
		{
			string sRelativePath;
			sRelativePath = ptrParams.strPath;
			sRelativePath += "/";
			if((sRelativePath.compare("//") != 0) && (sRelativePath.compare("/") != 0))
			{
				FindFileAndFolders(sRelativePath.c_str(),&ptrParams , rServiceParams.iStsTrg);
			}
			else
			{
				ERROR("%s","sRelativePath Fetch from the imm is '/' or '//' which is not correct");
			}
		}
		else if(!strcmp("SCXlg", ptrParams.strPath_LOGNAME.c_str()))   //       check for "SCXlog"
		{
			int nodeState = 0;
			ACS_PRC_API prcObj;
			nodeState = prcObj.askForNodeState();
			if(nodeState==1)//Active node
			{
				//				  int sDays = ptrParams.iNoOFDays;
				//				  deleteSCXFiles(sRelativePath, sDays);
				string sRelativePath;
				sRelativePath = ptrParams.strPath;
				sRelativePath += "/";
				if((sRelativePath.compare("//") != 0) && (sRelativePath.compare("/") != 0))
				{
					int sDays = ptrParams.iNoOFDays;
					deleteSCXFiles(sRelativePath, sDays);
				}
				else
				{
					ERROR("%s","sRelativePath Fetch from the imm is '/' or '//' which is not correct");
				}
			}
			else
			{
				DEBUG("%s","Node state is Passive");
			}
		}
		else if(!strcmp("CMXlg", ptrParams.strPath_LOGNAME.c_str()))
   //       check for "CMXlog"
		{
			int nodeState = 0;
			ACS_PRC_API prcObj;
			nodeState = prcObj.askForNodeState();
			if(nodeState==1)//Active node
			{
				//				  int sDays = ptrParams.iNoOFDays;
				//				  deleteCMXFiles(sRelativePath, sDays);
				string sRelativePath;
				sRelativePath = ptrParams.strPath;
				sRelativePath += "/";
				if((sRelativePath.compare("//") != 0) && (sRelativePath.compare("/") != 0))
				{
					int sDays = ptrParams.iNoOFDays;
					deleteCMXFiles(sRelativePath, sDays);
				}
				else
				{
					ERROR("%s","sRelativePath Fetch from the imm is '/' or '//' which is not correct");
				}
			}
			else
			{
				DEBUG("%s","Node state is Passive");
			}
		}

		////////////////////// END ////////////////////////////////////////////////////
		// for deleting the incomplete STS folders for CNI - apz21230/4-1136

		// For deleting the absolete .ddi files
#if 0
		else if(!strcmp("ddiRm", ptrParams.strPath_LOGNAME.c_str()))	//	 check for "ddiRm"
		{
			int nodeState = 0;
			ACS_PRC_API prcObj;
			nodeState = prcObj.askForNodeState();
			if(nodeState==1)//Active node
			{
				string sRelativePath;
				sRelativePath = ptrParams.strPath;
				sRelativePath += "/";
				if((sRelativePath.compare("//") != 0) && (sRelativePath.compare("/") != 0))
				{
					FindFileAndFolders(sRelativePath.c_str(),&ptrParams , rServiceParams.iStsTrg);
				}
				else
				{
					ERROR("%s","sRelativePath Fetch from the imm is '/' or '//' which is not correct");
				}
			}
			else
			{
				DEBUG("%s","Node state is Passive");
			}
		}
#endif
		else
		{
			DEBUG("%s","Invalid path to monitored !!");
		}
	}	// end of for loop

	// Close the search handle.
	if ( hSearch != 0)
	{
		logFileContainer.clear();
		//hHandle = 0;
		hSearch = 0;
	}
}//End of DeleteLogFiles

/*===================================================================
    ROUTINE: stringReverse
 =================================================================== */

string ACS_ACSC_Logmaint:: stringReverse(string receiveString)
{
	int indexFirstString, indexSecondString, receiveStringLength;
	string reversedString;

	receiveStringLength = receiveString.length();

	for(indexFirstString = receiveStringLength, indexSecondString = 0; indexFirstString>=0; --indexFirstString, ++indexSecondString )
	{
		reversedString += receiveString[indexFirstString];
	}

	return reversedString;
}//End of stringReverse

/*===================================================================
    ROUTINE: FindFirstFile
 =================================================================== */
FHANDLE ACS_ACSC_Logmaint::FindFirstFile(string directoryName,LIN_64_FIND_DATA *ldFindFileData)
{
	if((isDirectory(directoryName)==true)&&(!isWildCard(directoryName)))
	{
		directoryName.append("/");
	}
	logFileContainerWC.clear();
	logFileContainer.clear();
	logFileContainerfolder.clear();
	int status = showDirectoriesWildCard(directoryName,false);
	if(status ==INVALID_HANDLE_VALUE)
	{
		return INVALID_HANDLE_VALUE;
	}

	if(status == ERROR_FILE_NOT_FOUND )
	{
		return ERROR_FILE_NOT_FOUND;
	}
	else
	{
		if (!logFileContainerfolder.empty())
		{
			deque<string>::iterator iter;

			for (iter=logFileContainerfolder.begin();iter!=logFileContainerfolder.end();++iter)
			{
				string emptyFolder=(*iter).append("/Dummy");
				FILE *pFile;
				pFile = fopen(emptyFolder.c_str(),"w");
				if (pFile != NULL)
				{
					fclose(pFile);
				}
				else
				{
					DEBUG("%s","Failed to open the file in  logFileContainerfolder !!!");
				}
			}
		}
		if(!logFileContainerWC.empty())
		{
			DEBUG("%s","Before strcpy if the logFileContainerWC is not empty !!!");
			strcpy(ldFindFileData->cFileName,logFileContainerWC[0].c_str());
			DEBUG("%s","After strcpy if the logFileContainerWC is not empty !!!");

			struct tm *fileModificationTime;
			struct stat attrib;

			if(!stat((char*) ldFindFileData->cFileName,&attrib))
			{
				FHANDLE handle;
				fileModificationTime = gmtime(&(attrib.st_mtime));
				ldFindFileData->ftFileCreationTime = fileModificationTime;
				handle = 0;
				return handle;

			}
			else
			{
				DEBUG("%s","Invalid file path!!!");
				return INVALID_HANDLE_VALUE;
			}
		}
		else
		{
			DEBUG("%s","Folder is Empty!!!");
			return INVALID_HANDLE_VALUE;
		}
	}
}//End of FindFirstFile

/*===================================================================
    ROUTINE: FindFirstFileSCX
 =================================================================== */
FHANDLE ACS_ACSC_Logmaint::FindFirstFileSCX(string directoryName,LIN_64_FIND_DATA_SCX *ldFindFileData)
{
	if((isDirectory(directoryName)==true)&&(!isWildCard(directoryName)))
	{
		directoryName.append("/");
	}
	logFileContainerWC_SCX.clear();
	logFileContainerSCX.clear();
	int status = showDirectoriesWildCard(directoryName,true);
	if(status ==INVALID_HANDLE_VALUE)
	{
		return INVALID_HANDLE_VALUE;
	}

	if(status == ERROR_FILE_NOT_FOUND )
	{
		return ERROR_FILE_NOT_FOUND;
	}
	else
	{
		if(!logFileContainerWC_SCX.empty())
		{
			DEBUG("%s","Before strcpy if the logFileContainerWC_SCX is not empty !!!");
			strcpy(ldFindFileData->cFileName,logFileContainerWC_SCX[0].c_str());
			DEBUG("%s","After strcpy if the logFileContainerWC_SCX is not empty !!!");
			struct tm *fileModificationTime;
			struct stat attrib;

			if(!stat((char*) ldFindFileData->cFileName,&attrib))
			{
				FHANDLE handle;
				fileModificationTime = gmtime(&(attrib.st_mtime));
				ldFindFileData->ftFileCreationTime = fileModificationTime;
				handle = 0;
				return handle;

			}
			else
			{
				DEBUG("%s","Invalid file path!!!");
				return INVALID_HANDLE_VALUE;
			}
		}
		else
		{
			DEBUG("%s","Folder is Empty!!!");
			return INVALID_HANDLE_VALUE;
		}
	}
}//End of FindFirstFileSCX
*===================================================================
    ROUTINE: FindFirstFileCMX
 =================================================================== */
FHANDLE ACS_ACSC_Logmaint::FindFirstFileCMX(string directoryName,LIN_64_FIND_DATA_CMX *ldFindFileData)
{
	if((isDirectory(directoryName)==true)&&(!isWildCard(directoryName)))
	{
		directoryName.append("/");
	}
	logFileContainerWC_CMX.clear();
	logFileContainerCMX.clear();
	int status = showCmxDirectoriesWildCard(directoryName);
	if(status ==INVALID_HANDLE_VALUE)
	{
		return INVALID_HANDLE_VALUE;
	}

	if(status == ERROR_FILE_NOT_FOUND )
	{
		return ERROR_FILE_NOT_FOUND;
	}
	else
	{
		if(!logFileContainerWC_CMX.empty())
		{
			DEBUG("%s","Before strcpy if the logFileContainerWC_CMX is not empty !!!");
			strcpy(ldFindFileData->cFileName,logFileContainerWC_CMX[0].c_str());
			DEBUG("%s","After strcpy if the logFileContainerWC_CMX is not empty !!!");
			struct tm *fileModificationTime;
			struct stat attrib;

			if(!stat((char*) ldFindFileData->cFileName,&attrib))
			{
				FHANDLE handle;
				fileModificationTime = gmtime(&(attrib.st_mtime));
				ldFindFileData->ftFileCreationTime = fileModificationTime;
				handle = 0;
				return handle;

			}
			else
			{
				DEBUG("%s","Invalid file path!!!");
				return INVALID_HANDLE_VALUE;
			}
		}
		else
		{
			DEBUG("%s","Folder is Empty!!!");
			return INVALID_HANDLE_VALUE;
		}
	}
}//End of FindFirstFileCMX

/*===================================================================
    ROUTINE: FindFirstFileInQueue
 =================================================================== */
FHANDLE	ACS_ACSC_Logmaint:: FindFirstFileInQueue(LIN_64_FIND_DATA *ldFindFileData)
{
	int status =0 ;

	if(logFileContainerWC.empty())
	{
		status = ERROR_FILE_NOT_FOUND;
	}

	if((status) == ERROR_FILE_NOT_FOUND )
	{
		return ERROR_FILE_NOT_FOUND;
	}
	else
	{
		strcpy(ldFindFileData->cFileName,logFileContainerWC[0].c_str());
		struct tm *fileModificationTime;
		struct stat attrib;
		if(!stat((char*) ldFindFileData->cFileName,&attrib))
		{
			FHANDLE handle;
			fileModificationTime = gmtime(&(attrib.st_mtime));
			ldFindFileData->ftFileCreationTime = fileModificationTime;
			handle = 0;
			return handle;
		}
		else
		{
			DEBUG("%s","Invalid file path!!!");
			return INVALID_HANDLE_VALUE;
		}
	}
}//End of FindFirstFileInQueue

/*===================================================================
    ROUTINE: FindNextFile
 =================================================================== */
BOOL	ACS_ACSC_Logmaint:: FindNextFile(FHANDLE *fileHandle,LIN_64_FIND_DATA *ldFindNextFileData)
{
	DEBUG("%s","Entering ACS_ACSC_Logmaint:: FindNextFile()");
	bool fileStatus = false;

	if(((int)*fileHandle==ERROR_FILE_NOT_FOUND)||((int)*fileHandle==INVALID_HANDLE_VALUE))
	{
		return fileStatus;
	}
	else if((*fileHandle+1)>=logFileContainerWC.size())
	{
		return fileStatus;
	}
	else
	{
		*fileHandle = *fileHandle +1;

		strcpy(ldFindNextFileData->cFileName,"");
		strcpy(ldFindNextFileData->cFileName, logFileContainerWC[*fileHandle].c_str());
		
		struct tm *fileModificationTime;
		struct stat attrib;
		if(!stat((char*) ldFindNextFileData->cFileName,&attrib))
		{
			fileModificationTime = gmtime(&(attrib.st_mtime));
			ldFindNextFileData->ftFileCreationTime = fileModificationTime;
			fileStatus = true;
			return fileStatus;
		}
		else
		
		{	
			return fileStatus;
		}
	}
}//End of FindNextFile

/*===================================================================
    ROUTINE: FindNextFileSCX
 =================================================================== */
BOOL	ACS_ACSC_Logmaint:: FindNextFileSCX(FHANDLE *fileHandle,LIN_64_FIND_DATA_SCX *ldFindNextFileData)
{
	bool fileStatus = false;

	if(((int)*fileHandle==ERROR_FILE_NOT_FOUND)||((int)*fileHandle==INVALID_HANDLE_VALUE))
	{
		return fileStatus;
	}
	else if((*fileHandle+1)>=logFileContainerWC_SCX.size())
	{
		return fileStatus;
	}
	else
	{
		*fileHandle = *fileHandle +1;

		strcpy(ldFindNextFileData->cFileName,"");
		strcpy(ldFindNextFileData->cFileName, logFileContainerWC_SCX[*fileHandle].c_str());

		struct tm *fileModificationTime;
		struct stat attrib;
		if(!stat((char*) ldFindNextFileData->cFileName,&attrib))
		{
			fileModificationTime = gmtime(&(attrib.st_mtime));
			ldFindNextFileData->ftFileCreationTime = fileModificationTime;
			fileStatus = true;
			return fileStatus;
		}
		else
		{
			return fileStatus;
		}
	}
}//End of FindNextFileSCX

/*===================================================================
    ROUTINE: FindNextFileCMX
 =================================================================== */
BOOL	ACS_ACSC_Logmaint:: FindNextFileCMX(FHANDLE *fileHandle,LIN_64_FIND_DATA_CMX *ldFindNextFileData)
{
	bool fileStatus = false;

	if(((int)*fileHandle==ERROR_FILE_NOT_FOUND)||((int)*fileHandle==INVALID_HANDLE_VALUE))
	{
		return fileStatus;
	}
	else if((*fileHandle+1)>=logFileContainerWC_CMX.size())
	{
		return fileStatus;
	}
	else
	{
		*fileHandle = *fileHandle +1;

		strcpy(ldFindNextFileData->cFileName,"");
		strcpy(ldFindNextFileData->cFileName, logFileContainerWC_CMX[*fileHandle].c_str());

		struct tm *fileModificationTime;
		struct stat attrib;
		if(!stat((char*) ldFindNextFileData->cFileName,&attrib))
		{
			fileModificationTime = gmtime(&(attrib.st_mtime));
			ldFindNextFileData->ftFileCreationTime = fileModificationTime;
			fileStatus = true;
			return fileStatus;
		}
		else
		{
			return fileStatus;
		}
	}
}//End of FindNextFileCMX

/*===================================================================
    ROUTINE: removedirectoryrecursively
 =================================================================== */
int ACS_ACSC_Logmaint :: removedirectoryrecursively(const char *dirname,string rootLogPath)
{
	DIR *dir;
	bool returnStatus = true;
	struct dirent *entry;
	char path[PATH_MAX];

	dir = opendir(dirname);

	if (dir == NULL)
	{
		//perror("Error opendir()");
		returnStatus = false;
	}
	else
	{
		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
			{
				snprintf(path, (size_t) PATH_MAX, "%s/%s", dirname, entry->d_name);
				if (entry->d_type == DT_DIR)
				{
					int myStatus = -1;
					myStatus = removedirectoryrecursively(path,rootLogPath);
				}
				int status  = remove(path);
				if(status != 0)
				{
					returnStatus = false;
				}
			}

		}

		closedir(dir);
		int status = -1;
		rootLogPath ==rootLogPath.append("/");
		if(dirname == rootLogPath)
		{

			DEBUG("%s","The Present Directory is the root directory.It can not be deleted.\n\n");
		}
		else
		{
			status  = remove(dirname);
		}
		if(status != 0)
		{
			returnStatus = false;
		}
		else
		{
			returnStatus = true;
		}
	}

	return returnStatus;
}//End of removedirectoryrecursively

/*===================================================================
    ROUTINE: extractFileNameFrompath
 =================================================================== */

string ACS_ACSC_Logmaint :: extractFileNameFrompath(string path)
{
	int i;
	string fileName;

	for(i = path.length();i>=0;--i)
	{
		if(path[i] == '/')
			break;
	}

	for(unsigned int j=i+1;j<path.length();++j)
	{
		fileName+= path[j];
	}
	return fileName;
}//End of extractFileNameFrompath


/*===================================================================
   ROUTINE: findPathSearchPatternFromWildCardDirectory
=================================================================== */

string ACS_ACSC_Logmaint :: findPathSearchPatternFromWildCardDirectory(string wildcard)
{
	string returnString("1");
	DEBUG("WilCard %s",wildcard.c_str());
	for(int i=0;wildcard[i]!='*';++i)
	{
		DEBUG("\n Each Char %c",wildcard[i]);
		if( i == 0)
		{
			returnString.at(0) = wildcard[0];
		}
		else
		{
			returnString += wildcard[i];
		}
	}
	DEBUG("%s","After for loop...\n\n");

	return returnString;
}//End of findPathSearchPatternFromWildCardDirectory


/*===================================================================
   ROUTINE: searchPathFromWildCard
=================================================================== */
string ACS_ACSC_Logmaint :: searchPathFromWildCard(string receiveDir)
{
	string searchpath;
	int i;

	for(i=receiveDir.length()-1;i>0;--i)
	{
		if(receiveDir[i] == '/')
		{
			break;
		}
	}

	for(int j=0;j<=i;++j)
	{
		searchpath += receiveDir[j];
	}

	return searchpath;
}//End of searchPathFromWildCard

/*===================================================================
   ROUTINE: removeFile
=================================================================== */
BOOL ACS_ACSC_Logmaint :: removeFile(char* fileName)
{
	bool status = false;
	if(!remove(fileName))
		status = true;
	return status;
}//End of removeFile

/*===================================================================
   ROUTINE: printDeque
=================================================================== */
VOID ACS_ACSC_Logmaint :: printDeque()
{
	DEBUG("The total no of searched files and folders are : %d",logFileContainerWC.size());
//	for(unsigned int i=0;i<logFileContainerWC.size();++i)
//	{
//		DEBUG("PATH = %s",logFileContainerWC[i].c_str());
//	}
}//End of printDeque
/*===================================================================
   ROUTINE: printDequeSCX
=================================================================== */
VOID ACS_ACSC_Logmaint :: printDequeSCX()
{
	DEBUG("The total number of searched files and folders  for SCX path are : %d",logFileContainerWC_SCX.size());
//	for(unsigned int i=0;i<logFileContainerWC_SCX.size();++i)
//	{
//		DEBUG("PATH = %s",logFileContainerWC_SCX[i].c_str());
//	}
}//End of printDequeSCX
/*===================================================================
   ROUTINE: printDequeCMX
=================================================================== */
VOID ACS_ACSC_Logmaint :: printDequeCMX()
{
	DEBUG("The total number of searched files and folders  for CMX path are : %d",logFileContainerWC_CMX.size());
//	for(unsigned int i=0;i<logFileContainerWC_CMX.size();++i)
//	{
//		DEBUG("PATH = %s",logFileContainerWC_CMX[i].c_str());
//	}
}//End of printDequeCMX
/*===================================================================
   ROUTINE: getTimeStampOfFile
=================================================================== */
struct tm* ACS_ACSC_Logmaint :: getTimeStampOfFile(string fileName)
{
	struct tm *fileModificationTime = NULL;
	struct stat attrib;

	if(!stat((char*) fileName.c_str(),&attrib))
	{
		fileModificationTime = gmtime(&(attrib.st_mtime));
	}
	else
	{
		DEBUG("%s","Invalid file path!!! \n");
	}

	return fileModificationTime;
}//End of getTimeStampOfFile

/*===================================================================
   ROUTINE: showDirectoriesWildCard
=================================================================== */
int ACS_ACSC_Logmaint :: showDirectoriesWildCard(string receiveDirWildCard,bool isSCXLog)
{
	int status;
	string receiveDir = searchPathFromWildCard(receiveDirWildCard);
	DEBUG("%s","Before showFilesandDirectories \n");
	status = showFilesandDirectories(receiveDir,isSCXLog);
	DEBUG("%s","After showFilesandDirectories \n");
	if(!isSCXLog)
	{
		for(unsigned int i=0;i<logFileContainer.size();++i)
		{
			if(wildCardMatch(receiveDirWildCard, logFileContainer[i]))
			{
				logFileContainerWC.push_back(logFileContainer[i]);
			}
		}
	}
	else
	{ // FOR SCX Logging
		for(unsigned int i=0;i<logFileContainerSCX.size();++i)
		{
			if(wildCardMatch(receiveDirWildCard, logFileContainerSCX[i]))
			{
				logFileContainerWC_SCX.push_back(logFileContainerSCX[i]);
			}
		}
	}
	return status;
}//End of showDirectoriesWildCard

//This function filter deque by extension and updates it

/*===================================================================
   ROUTINE: showCmxDirectoriesWildCard
=================================================================== */
int ACS_ACSC_Logmaint :: showCmxDirectoriesWildCard(string receiveDirWildCard)
{
	int status;
	string receiveDir = searchPathFromWildCard(receiveDirWildCard);
	DEBUG("%s","Before showCmxFilesandDirectories \n");
	status = showCmxFilesandDirectories(receiveDir);
	DEBUG("%s","After showCmxFilesandDirectories \n");
	// FOR CMX Logging
		for(unsigned int i=0;i<logFileContainerCMX.size();++i)
		{
			if(wildCardMatch(receiveDirWildCard, logFileContainerCMX[i]))
			{
				logFileContainerWC_CMX.push_back(logFileContainerCMX[i]);
			}
		}
	
	return status;
}//End of showCmxDirectoriesWildCard
/*===================================================================
   ROUTINE: filterByExtension
=================================================================== */

void ACS_ACSC_Logmaint :: filterByExtension(string extensionString)
{
	deque<string> dq;

	for(unsigned int i=0;i<logFileContainerWC.size();++i)
	{
		if(wildCardMatch(extensionString,logFileContainerWC[i]))
		{
			dq.push_back(logFileContainerWC[i]);
		}
	}

	logFileContainerWC.clear();
	logFileContainerWC = dq;
}//End of filterByExtension

/*===================================================================
   ROUTINE: showFilesandDirectories
=================================================================== */
int ACS_ACSC_Logmaint :: showFilesandDirectories(string receiveDir,bool isSCXLog)
{
	struct dirent *dp;
	string pathBeforeWildCard;
	string wildCardPattern;
	DIR *dir;

	if(isWildCard(receiveDir))
	{
		pathBeforeWildCard = searchPathFromWildCard(receiveDir);

	}
	else
	{
		pathBeforeWildCard = receiveDir;
	}
	DEBUG("%s","Before opening the directory in showFilesandDirectories.....\n");
	DEBUG("showFilesandDirectories...\n %s", pathBeforeWildCard.c_str());
	dir=opendir((char*)pathBeforeWildCard.c_str());
	DEBUG("%s","After opening the directory in showFilesandDirectories.....\n");
	if(dir == NULL)
	{
		DEBUG("%s","Given path is not correct.....\n");
		return INVALID_HANDLE_VALUE;
	}
	else
	{
		while((dp=readdir(dir))!= NULL)
		{
			string tmpString;

			tmpString = (string)pathBeforeWildCard + (string) (dp->d_name);

			if(!isSCXLog)
			{
				if((tmpString[tmpString.length()-1])!='.')
				{
					//DEBUG("logFileContainer.push_back.. %s", tmpString.c_str());
					logFileContainer.push_back(tmpString);
				}
			}
			else
			{ //For SCX Log container
				if(!(tmpString.compare(tmpString.size()-3,3,".gz")))
				{
					//DEBUG("logFileContainerSCX.push_back.. %s", tmpString.c_str());
					logFileContainerSCX.push_back(tmpString);
				}
			}
			if((isDirectory(tmpString)==true)&&((tmpString[tmpString.length()-1])!='.'))
			{
				string directoryPath=searchPathFromWildCard(tmpString);
				int emptyDirectory=isDirectoryEmpty(tmpString);
				if(emptyDirectory == 2)//Enter if directory is empty except ". and .." folder
				{
					DEBUG("  %s", "emptyDirectory == 2");
					//int directorysize=directoryPath.length();
					int stsOutputdirlength= theOutputSTSPath.length();
					//string myDirectory = directoryPath.substr(0,stsOutputdirlength-1);
					string myDirectory = directoryPath.substr(0,stsOutputdirlength);
					//if((!(myDirectory.compare(theDataSTSPath))) || (!(myDirectory.compare(theOutputSTSPath))))
					if(!(myDirectory.compare(theOutputSTSPath)))
					{
						//DEBUG("logFileContainerfolder.push_back.. %s", tmpString.c_str());
						logFileContainerfolder.push_back(tmpString);
					}
					else
					{
						//		DEBUG("  %s", "NOT STS");
					}
				}
				else
				{
					DEBUG("%s","Directory is not empty...\n");
				}

				tmpString=tmpString+"/";
				DEBUG("calling showFilesandDirectories  %s", tmpString.c_str());
				showFilesandDirectories(tmpString,isSCXLog);
			}
			else if((isDirectory(tmpString)==false)&&((tmpString[tmpString.length()-1])!='.'))
			{

			}

		}

		closedir(dir);

		if(logFileContainer.empty())
		{
			return ERROR_FILE_NOT_FOUND;
		}
		else
		{
			return SUCCESS;
		}
	}
}//End of showFilesandDirectories
/*===================================================================
   ROUTINE: showCmxFilesandDirectories
=================================================================== */
int ACS_ACSC_Logmaint :: showCmxFilesandDirectories(string receiveDir)
{
	struct dirent *dp;
	string pathBeforeWildCard;
	string wildCardPattern;
	DIR *dir;

	if(isWildCard(receiveDir))
	{
		pathBeforeWildCard = searchPathFromWildCard(receiveDir);

	}
	else
	{
		pathBeforeWildCard = receiveDir;
	}
	DEBUG("%s","Before opening the directory in showCmxFilesandDirectories.....\n");
	DEBUG("showCmxFilesandDirectories...\n %s", pathBeforeWildCard.c_str());
	dir=opendir((char*)pathBeforeWildCard.c_str());
	DEBUG("%s","After opening the directory in showCmxFilesandDirectories.....\n");
	if(dir == NULL)
	{
		DEBUG("%s","Given path is not correct.....\n");
		return INVALID_HANDLE_VALUE;
	}
	else
	{
		while((dp=readdir(dir))!= NULL)
		{
			string tmpString;

			tmpString = (string)pathBeforeWildCard + (string) (dp->d_name);
			//For CMX Log container
				if(!(tmpString.compare(tmpString.size()-3,3,".gz")))
				{
					//DEBUG("logFileContainerCMX.push_back.. %s", tmpString.c_str());
					logFileContainerCMX.push_back(tmpString);
				}
			
			if((isDirectory(tmpString)==true)&&((tmpString[tmpString.length()-1])!='.'))
			{
				string directoryPath=searchPathFromWildCard(tmpString);
				int emptyDirectory=isDirectoryEmpty(tmpString);
				if(emptyDirectory == 2)//Enter if directory is empty except ". and .." folder
				{
					DEBUG("  %s", "emptyDirectory == 2");
					//int directorysize=directoryPath.length();
					int stsOutputdirlength= theOutputSTSPath.length();
					//string myDirectory = directoryPath.substr(0,stsOutputdirlength-1);
					string myDirectory = directoryPath.substr(0,stsOutputdirlength);
					//if((!(myDirectory.compare(theDataSTSPath))) || (!(myDirectory.compare(theOutputSTSPath))))
					if(!(myDirectory.compare(theOutputSTSPath)))
					{
						//DEBUG("logFileContainerfolder.push_back.. %s", tmpString.c_str());
						logFileContainerfolder.push_back(tmpString);
					}
					else
					{
						//		DEBUG("  %s", "NOT STS");
					}
				}
				else
				{
					DEBUG("%s","Directory is not empty...\n");
				}

				tmpString=tmpString+"/";
				DEBUG("calling showCmxFilesandDirectories  %s", tmpString.c_str());
				showCmxFilesandDirectories(tmpString);
			}
			else if((isDirectory(tmpString)==false)&&((tmpString[tmpString.length()-1])!='.'))
			{

			}

		}

		closedir(dir);

		if(logFileContainer.empty())
		{
			return ERROR_FILE_NOT_FOUND;
		}
		else
		{
			return SUCCESS;
		}
	}
}//End of showCmxFilesandDirectories
//============================================================================
// isDirectoryEmpty checks given directory is empty or not
// 0 on success if directory is empty
// non zero in case of failure or not empty
//============================================================================
ACE_INT32 ACS_ACSC_Logmaint::isDirectoryEmpty(string dname)
{
	int count=0;
	//dirent* dir;
	const char* pzPath = dname.c_str();
	DIR *pDir;
	pDir = opendir(pzPath);
	if(pDir == NULL)
	{
		return 0;
	}
	while(readdir(pDir)!= NULL)
	{
		count++;
	}
	(void) closedir (pDir);

	return count;
}// End of IsDirectoryEmpty

//============================================================================
// deleteSCXFiles delete SCX log files.
//============================================================================
void ACS_ACSC_Logmaint::deleteSCXFiles(string dirPath,int numOfDays)
{
	DEBUG("%s","Entering ::deleteSCXFiles to delete SCX logs older than m days");
	FHANDLE          hFile = NULL;
	std::string     strFilePath;
	std::string     strPattern;
	LIN_64_FIND_DATA_SCX FileInformation;
	strPattern = dirPath + "/*.*";
	hFile = FindFirstFileSCX(strPattern.c_str(), &FileInformation);
	printDequeSCX();

	if ((INVALID_HANDLE_VALUE == (int)hFile)||(ERROR_FILE_NOT_FOUND == (int)hFile))
	{
		ERROR(" %s","Incorrect path or file not found!!!!\n");
		return;
	}

	do
	{
		strFilePath.erase();
		strFilePath = FileInformation.cFileName;
		//Check for .gz file in the directory
		if(strFilePath.compare(strFilePath.size()-3,3,".gz"))
		{
			DEBUG("%s","Non .gz file found");
			continue;
		}
		double dElapsedTime = ElapsedTimeCal(FileInformation.ftFileCreationTime);

		if (dElapsedTime >= (SECONDS_IN_DAY*numOfDays))
		{
			// Set file attributes //TODO
			/*if(::SetFileAttributes(strFilePath.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE)
            {
                 std::cout << " SetFileAttributes failed ! " << std::endl <<
                 "File: " << strFilePath.c_str() << " - GetLastError: " << GetLastError() << std::endl;
            }*/

			// Delete file
			DEBUG("Deleting file : %s",strFilePath.c_str());
			if(removeFile(const_cast<char*>(strFilePath.c_str())) == FALSE)
			{
				DEBUG("%s","Remove file failed !...\n");
				DEBUG("File: %s", strFilePath.c_str()) ;
			}
		} //if (dElapsedTime >= (SECONDS_IN_DAY*numOfDays))
	}
	while(FindNextFileSCX(&hFile, &FileInformation) == TRUE);

}//end of deleteSCXFiles
//============================================================================
// deleteCMXFiles delete CMX log files.
//============================================================================
void ACS_ACSC_Logmaint::deleteCMXFiles(string dirPath,int numOfDays)
{
	DEBUG("%s","Entering ::deleteCMXFiles to delete CMX logs older than m days");
	FHANDLE          hFile = NULL;
	std::string     strFilePath;
	std::string     strPattern;
	LIN_64_FIND_DATA_CMX FileInformation;
	strPattern = dirPath + "/*.*";
	hFile = FindFirstFileCMX(strPattern.c_str(), &FileInformation);
	printDequeCMX();

	if ((INVALID_HANDLE_VALUE == (int)hFile)||(ERROR_FILE_NOT_FOUND == (int)hFile))
	{
		ERROR(" %s","Incorrect path or file not found!!!!\n");
		return;
	}

	do
	{
		strFilePath.erase();
		strFilePath = FileInformation.cFileName;
		//Check for .gz file in the directory
		if(strFilePath.compare(strFilePath.size()-3,3,".gz"))
		{
			DEBUG("%s","Non .gz file found");
			continue;
		}
		double dElapsedTime = ElapsedTimeCal(FileInformation.ftFileCreationTime);

		if (dElapsedTime >= (SECONDS_IN_DAY*numOfDays))
		{
			// Set file attributes //TODO
			/*if(::SetFileAttributes(strFilePath.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE)
            {
                 std::cout << " SetFileAttributes failed ! " << std::endl <<
                 "File: " << strFilePath.c_str() << " - GetLastError: " << GetLastError() << std::endl;
            }*/

			// Delete file
			DEBUG("Deleting file : %s",strFilePath.c_str());
			if(removeFile(const_cast<char*>(strFilePath.c_str())) == FALSE)
			{
				DEBUG("%s","Remove file failed !...\n");
				DEBUG("File: %s", strFilePath.c_str()) ;
			}
		} //if (dElapsedTime >= (SECONDS_IN_DAY*numOfDays))
	}
	while(FindNextFileCMX(&hFile, &FileInformation) == TRUE);

}//end of deleteCMXFiles
//============================================================================
// DeleteMoreThen delete SCX log files based on the number of files
//============================================================================
void ACS_ACSC_Logmaint::DeleteMoreThen(const std::string &refcstrRootDirectory, 
		int fileNum)
{
	DEBUG("%s","Entering DeleteMoreThen for deleting more than n number of files...!!\n");
	FHANDLE     hFile = NULL;
	std::string strFilePath;
	std::string strPattern;
	LIN_64_FIND_DATA_SCX FileInformation;
	std::map<std::string, double> fileMap;
	std::multimap<std::string,std::string> scxFileMap;
	std::vector<std::string> logFilePath;
	strPattern = refcstrRootDirectory + "/*.*";
	hFile = FindFirstFileSCX(strPattern.c_str(), &FileInformation);
	printDequeSCX();
	if ((INVALID_HANDLE_VALUE == (int)hFile)||(ERROR_FILE_NOT_FOUND == (int)hFile))
	{
		ERROR(" %s","Incorrect path or file not found!!!!\n");
		return;
	}
	do
	{
		strFilePath.erase();
		strFilePath = FileInformation.cFileName;
		//Check for .gz file
		int result = strFilePath.compare(((strFilePath).size())-3,3,".gz");
		if(result)
		{	
			continue;
		}
		DEBUG("SCX Log File Path =%s", strFilePath.c_str());
		std::string scxLogFileName(extractFileNameFrompath(strFilePath));
		std::string scxLogFilePath = strFilePath.substr(0,strFilePath.find(scxLogFileName));
		logFilePath.push_back(scxLogFilePath);	
		scxFileMap.insert(pair<std::string,std::string>(scxLogFilePath,strFilePath));
		double elapsedTime = ElapsedTimeCal(FileInformation.ftFileCreationTime);
		fileMap[strFilePath] = elapsedTime;
	}
	while(FindNextFileSCX(&hFile, &FileInformation) == TRUE);

	if(fileMap.size() < 1)
	{
		DEBUG("%s", "No .gz file found...!! ");
		return; 
	}
	//Removing duplicate values from vector
	for(size_t vecIndex = 0; vecIndex < (logFilePath.size()-1); vecIndex++)
	{
		for(size_t vIndex = vecIndex+1; vIndex < logFilePath.size(); vIndex++)
		{
			std::string first = logFilePath[vecIndex];
			std::string second = logFilePath[vIndex];
			if(first == second)
			{
				logFilePath.erase(logFilePath.begin()+vIndex);
				vIndex--;
			}
		}
	}
	//Filling multimap with all files in a subdirectoy 
	for (size_t index = 0; index< logFilePath.size(); index++)
	{
		std::multimap<double, std::string> sudDirFileMap;
		std::multimap<std::string, std::string>::iterator itMultiMap;
		pair<multimap<std::string,std::string>::iterator,multimap<std::string,std::string>::iterator> ret;
		ret = scxFileMap.equal_range(logFilePath[index]);
		for (itMultiMap=ret.first; itMultiMap!=ret.second; ++itMultiMap)
		{
			//Filling the multi map with timestamp as key and all files with the timestamp as values.
			sudDirFileMap.insert(pair<double,std::string>(fileMap.at((*itMultiMap).second),(*itMultiMap).second));
		}
		DEBUG("%s","Calling DeleteSCXLogsInSubDir with all files in a subdirectory...\n");
		DeleteSCXLogsInSubDir(sudDirFileMap,fileNum);
		sudDirFileMap.clear();
	}
	scxFileMap.clear();
	fileMap.clear();
}//end of DeleteMoreThen
//============================================================================
// DeleteCmxMoreThen delete CMX log files based on the number of files
//============================================================================
void ACS_ACSC_Logmaint::DeleteCmxMoreThen(const std::string &refcstrRootDirectory, 
		int fileNum)
{
	DEBUG("%s","Entering DeleteCmxMoreThen for deleting more than n number of files...!!\n");
	FHANDLE     hFile = NULL;
	std::string strFilePath;
	std::string strPattern;
	LIN_64_FIND_DATA_CMX FileInformation;
	std::map<std::string, double> fileMap;
	std::multimap<std::string,std::string> cmxFileMap;
	std::vector<std::string> logFilePath;
	strPattern = refcstrRootDirectory + "/*.*";
	hFile = FindFirstFileCMX(strPattern.c_str(), &FileInformation);
	printDequeCMX();
	if ((INVALID_HANDLE_VALUE == (int)hFile)||(ERROR_FILE_NOT_FOUND == (int)hFile))
	{
		ERROR(" %s","Incorrect path or file not found!!!!\n");
		return;
	}
	do
	{
		strFilePath.erase();
		strFilePath = FileInformation.cFileName;
		//Check for .gz file
		int result = strFilePath.compare(((strFilePath).size())-3,3,".gz");
		if(result)
		{	
			continue;
		}
		DEBUG("CMX Log File Path =%s", strFilePath.c_str());
		std::string cmxLogFileName(extractFileNameFrompath(strFilePath));
		std::string cmxLogFilePath = strFilePath.substr(0,strFilePath.find(cmxLogFileName));
		logFilePath.push_back(cmxLogFilePath);	
		cmxFileMap.insert(pair<std::string,std::string>(cmxLogFilePath,strFilePath));
		double elapsedTime = ElapsedTimeCal(FileInformation.ftFileCreationTime);
		fileMap[strFilePath] = elapsedTime;
	}
	while(FindNextFileCMX(&hFile, &FileInformation) == TRUE);

	if(fileMap.size() < 1)
	{
		DEBUG("%s", "No .gz file found...!! ");
		return; 
	}
	//Removing duplicate values from vector
	for(size_t vecIndex = 0; vecIndex < (logFilePath.size()-1); vecIndex++)
	{
		for(size_t vIndex = vecIndex+1; vIndex < logFilePath.size(); vIndex++)
		{
			std::string first = logFilePath[vecIndex];
			std::string second = logFilePath[vIndex];
			if(first == second)
			{
				logFilePath.erase(logFilePath.begin()+vIndex);
				vIndex--;
			}
		}
	}
	//Filling multimap with all files in a subdirectoy 
	for (size_t index = 0; index< logFilePath.size(); index++)
	{
		std::multimap<double, std::string> sudDirFileMap;
		std::multimap<std::string, std::string>::iterator itMultiMap;
		pair<multimap<std::string,std::string>::iterator,multimap<std::string,std::string>::iterator> ret;
		ret = cmxFileMap.equal_range(logFilePath[index]);
		for (itMultiMap=ret.first; itMultiMap!=ret.second; ++itMultiMap)
		{
			//Filling the multi map with timestamp as key and all files with the timestamp as values.
			sudDirFileMap.insert(pair<double,std::string>(fileMap.at((*itMultiMap).second),(*itMultiMap).second));
		}
		DEBUG("%s","Calling DeleteCMXLogsInSubDir with all files in a subdirectory...\n");
		DeleteCMXLogsInSubDir(sudDirFileMap,fileNum);
		sudDirFileMap.clear();
	}
	cmxFileMap.clear();
	fileMap.clear();
}//end of DeleteCmxMoreThen
//==================================================================================
// Routine DeleteSCXLogsInSubDir deletes more than n number of  SCX logs in a subdir 
//==================================================================================
void ACS_ACSC_Logmaint::DeleteSCXLogsInSubDir(std::multimap<double,std::string> fileMap, int fileNum)
{
	DEBUG("%s","Entering ::DeleteSCXLogsInSubDir to delete more than n files in a subdirectory...");
	std::vector<double> fileElapsedTime;
	std::multimap<double, std::string>::iterator itMap;

	for( itMap = fileMap.begin(); itMap != fileMap.end() ; itMap++)
	{
		DEBUG("%s%f%s%s","Age of log file in sec  = ",itMap->first," Log file name = ",itMap->second.c_str());
		fileElapsedTime.push_back((*itMap).first);
	}

	//Removing duplicate values from vector
	DEBUG("%s","Removing duplicate file elapsed time from vector...")
	for(size_t vecIndex = 0; vecIndex < (fileElapsedTime.size()-1); vecIndex++)
	{
		for(size_t vIndex = vecIndex+1; vIndex < fileElapsedTime.size(); vIndex++)
		{
			double first = fileElapsedTime[vecIndex];
			double second = fileElapsedTime[vIndex];
			if(first == second)
			{
				fileElapsedTime.erase(fileElapsedTime.begin()+vIndex);
				vIndex--;
			}
		}
	}

	//Sort file age in the decreasing order
	DEBUG("%s","Sorting file elapsed time in descending order...")
	for(size_t vecIndex = 0;vecIndex < fileElapsedTime.size()-1;vecIndex++)
	{
		for(size_t vIndex = vecIndex+1;vIndex < fileElapsedTime.size();vIndex++)
		{
			if(fileElapsedTime[vecIndex]<fileElapsedTime[vIndex])
			{
				double tmp = fileElapsedTime[vIndex];
				fileElapsedTime[vIndex] = fileElapsedTime[vecIndex];
				fileElapsedTime[vecIndex] = tmp;
			}           
		}
	}

	if(fileMap.size() > (unsigned int) fileNum)
	{
		unsigned int fileCount = 0;

		for (size_t index = 0; index< fileElapsedTime.size(); index++)
		{
			pair<multimap<double,std::string>::iterator,multimap<double,std::string>::iterator> ret;
			ret = fileMap.equal_range(fileElapsedTime[index]);
			std::multimap<double, std::string>::iterator itMultiMap;
			for (itMultiMap=ret.first; itMultiMap!=ret.second; ++itMultiMap)
			{
				std::string fileName = (*itMultiMap).second;

				fileCount++;
				if ((fileMap.size() - fileNum) < fileCount)
				{
					//This files will not delete !
					DEBUG("%s","Now number of log files is not greather than min number of files!");
					DEBUG("%s","So can't be deleted... !\n");
				}
				else
				{
					// Set file attributes TODO
					/*if(::SetFileAttributes(it->second.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE)
               		{
                   		std::cout << " SetFileAttributes failed ! " << std::endl <<
                       	"File: " << it->second.c_str() << " - GetLastError: " << GetLastError() << std::endl;
               		}*/
					// Delete file
					DEBUG("Deleting extra file : %s",fileName.c_str());
					if(removeFile(const_cast<char*>(fileName.c_str())) == FALSE)
					{
						DEBUG("%s","Remove file failed !...\n");
						DEBUG("File: %s", fileName.c_str());
					}
				}
			}
		}
	}
	else
	{
		DEBUG("%s","Log file count is not greather than min file count...!!\n");
	}

	fileMap.clear();
}//end of DeleteSCXLogsInSubDir 

//==================================================================================
// Routine DeleteCMXLogsInSubDir deletes more than n number of  CMX logs in a subdir 
//==================================================================================
void ACS_ACSC_Logmaint::DeleteCMXLogsInSubDir(std::multimap<double,std::string> fileMap, int fileNum)
{
	DEBUG("%s","Entering ::DeleteCMXLogsInSubDir to delete more than n files in a subdirectory...");
	std::vector<double> fileElapsedTime;
	std::multimap<double, std::string>::iterator itMap;

	for( itMap = fileMap.begin(); itMap != fileMap.end() ; itMap++)
	{
		DEBUG("%s%f%s%s","Age of log file in sec  = ",itMap->first," Log file name = ",itMap->second.c_str());
		fileElapsedTime.push_back((*itMap).first);
	}

	//Removing duplicate values from vector
	DEBUG("%s","Removing duplicate file elapsed time from vector...")
	for(size_t vecIndex = 0; vecIndex < (fileElapsedTime.size()-1); vecIndex++)
	{
		for(size_t vIndex = vecIndex+1; vIndex < fileElapsedTime.size(); vIndex++)
		{
			double first = fileElapsedTime[vecIndex];
			double second = fileElapsedTime[vIndex];
			if(first == second)
			{
				fileElapsedTime.erase(fileElapsedTime.begin()+vIndex);
				vIndex--;
			}
		}
	}

	//Sort file age in the decreasing order
	DEBUG("%s","Sorting file elapsed time in descending order...")
	for(size_t vecIndex = 0;vecIndex < fileElapsedTime.size()-1;vecIndex++)
	{
		for(size_t vIndex = vecIndex+1;vIndex < fileElapsedTime.size();vIndex++)
		{
			if(fileElapsedTime[vecIndex]<fileElapsedTime[vIndex])
			{
				double tmp = fileElapsedTime[vIndex];
				fileElapsedTime[vIndex] = fileElapsedTime[vecIndex];
				fileElapsedTime[vecIndex] = tmp;
			}           
		}
	}

	if(fileMap.size() > (unsigned int) fileNum)
	{
		unsigned int fileCount = 0;

		for (size_t index = 0; index< fileElapsedTime.size(); index++)
		{
			pair<multimap<double,std::string>::iterator,multimap<double,std::string>::iterator> ret;
			ret = fileMap.equal_range(fileElapsedTime[index]);
			std::multimap<double, std::string>::iterator itMultiMap;
			for (itMultiMap=ret.first; itMultiMap!=ret.second; ++itMultiMap)
			{
				std::string fileName = (*itMultiMap).second;

				fileCount++;
				if ((fileMap.size() - fileNum) < fileCount)
				{
					//This files will not delete !
					DEBUG("%s","Now number of log files is not greather than min number of files!");
					DEBUG("%s","So can't be deleted... !\n");
				}
				else
				{
					// Set file attributes TODO
					/*if(::SetFileAttributes(it->second.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE)
               		{
                   		std::cout << " SetFileAttributes failed ! " << std::endl <<
                       	"File: " << it->second.c_str() << " - GetLastError: " << GetLastError() << std::endl;
               		}*/
					// Delete file
					DEBUG("Deleting extra file : %s",fileName.c_str());
					if(removeFile(const_cast<char*>(fileName.c_str())) == FALSE)
					{
						DEBUG("%s","Remove file failed !...\n");
						DEBUG("File: %s", fileName.c_str());
					}
				}
			}
		}
	}
	else
	{
		DEBUG("%s","Log file count is not greather than min file count...!!\n");
	}

	fileMap.clear();
}//end of DeleteCMXLogsInSubDir 
//============================================================================
// Routine DeleteExtraFiles calls DeleteMoreThen
//============================================================================
void ACS_ACSC_Logmaint::DeleteExtraFiles (ACE_UINT32 iLogTypCnt,
		ACE_UINT32 dwTimerLowValue, ACE_UINT32 dwTimerHighValue)
{
	_ACS_LOGM_LOGFILESPEC     ptrParams;
	string sRelativePath,sCmxRelativePath;
	int numOfFiles = 0,numOfCmxFiles = 0;
	if(!iLogTypCnt)
	{
		ERROR("%s %d %d","Error reading service trigger parameters from IMM Database",dwTimerLowValue,dwTimerHighValue);
		return;
	}
	for(size_t index = 0 ; index < iLogTypCnt ; index++)
	{
		if(GetLogFileParams(&ptrParams,(index+1)) == 0)
		{
			continue;
		}
		else if(!strcmp("SCXlg",ptrParams.strPath_LOGNAME.c_str()))
		{
			numOfFiles = ptrParams.iNoOfFiles;
			sRelativePath = ptrParams.strPath;
		}
		else if(!strcmp("CMXlg",ptrParams.strPath_LOGNAME.c_str()))
		{
			numOfCmxFiles = ptrParams.iNoOfFiles;
			sCmxRelativePath = ptrParams.strPath;
		}
	}
	if (numOfFiles > 0)
	{
		DeleteMoreThen(sRelativePath, numOfFiles);
	}
	if (numOfCmxFiles > 0)
	{
		DeleteCmxMoreThen(sCmxRelativePath, numOfCmxFiles);
	}
	return;
}//end of DeleteExtraFiles 
