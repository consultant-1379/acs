/*
 * ACS_CHB_aadst_handler.cpp
 *
 *  Created on: Jul 24, 2014
 *      Author: xassore
 */
#include  <ACS_CS_API.h>
#include  <ACS_APGCC_CommonLib.h>
#include  <ACS_APGCC_Util.H>

#include  <sstream>
#include  <iostream>
#include  <stdlib.h>
#include  <stdio.h>
#include  <fstream>
#include  <string.h>
#include  <cctype>
#include  <list>
#include  <regex.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <dirent.h>
#include  <time.h>
#include  <signal.h>
#include "acs_chb_tra.h"
#include "acs_chb_aadst_common.h"
#include "acs_chb_aadst_alarmHandler.h"
#include "acs_chb_aadst_handler.h"
#include "acs_chb_aadst_TDstExtractor.h"
#include "acs_chb_aadst_TDstCore.h"

using namespace std;

struct APtimeZones     TimeZones[MAX_NO_TIME_ZONES];
struct InternalTZdump  intTZdumpTab[SIZE_TZDUMP_TAB];



/* ------------------------  ACS_CHB_aadst_CastcParams  CLASS   ---------------------- */


int  ACS_CHB_aadst_CastcParams::getClockAdj (long gmtOffs_1, long gmtOffs_2)
{

	int  clockAdj;

	if (gmtOffs_2 > gmtOffs_1)  clockAdj = (gmtOffs_2 - gmtOffs_1) / 60;
	else 						clockAdj = (gmtOffs_1 - gmtOffs_2) / 60;

	clockAdj = clockAdj - (clockAdj % 15);		//  Rif :  CASTC, Command Description

	return clockAdj;
}


void  ACS_CHB_aadst_CastcParams::stepToNextDay (struct tm *dateInfo)
{

	dateInfo->tm_mday ++;

	switch (dateInfo->tm_mon)
	{
		case  1  :
		case  3  :
		case  5  :
		case  7  :
		case  8  :
		case  10 :	if (dateInfo->tm_mday > 31)
			    	{
							dateInfo->tm_mday = 1;	dateInfo->tm_mon ++;
			    	}
			    	break;

		case  4  :
		case  6  :
		case  9  :
		case  11 :  if (dateInfo->tm_mday > 30)
			    	{
							dateInfo->tm_mday = 1;	dateInfo->tm_mon ++;
			    	}
			    	break;

		case  2  :  if (dateInfo->tm_mday > 29)
			    	{
							dateInfo->tm_mday = 1;	dateInfo->tm_mon = 3;
			    	}
					else if (dateInfo->tm_mday == 29)
					{
							if (isleap (dateInfo->tm_year) == false)
							{
									dateInfo->tm_mday = 1;	dateInfo->tm_mon = 3;
							}
					}
			    	break;

		case  12 :  if (dateInfo->tm_mday > 31)
			    	{
							dateInfo->tm_mday = 1;	dateInfo->tm_mon = 1;
							dateInfo->tm_year ++;
			    	}
			    	break;

		default :  DEBUG(1, "%s", "stepToNextDay :  Inconsistent Information !!");
    }
}


void  ACS_CHB_aadst_CastcParams::DLInfoDateSettings (int ix, struct tm info)
{

		if (info.tm_hour == 23  &&  info.tm_min == 59)		//  Summer Time on 24:00
		{													//  it is necessary to step the date to tomorrow
				stepToNextDay (&info);
		}

		intTZdumpTab[ix].day   = info.tm_mday;
		intTZdumpTab[ix].month = info.tm_mon;
		intTZdumpTab[ix].year  = info.tm_year;

		if ((info.tm_min + 1) == 60)  intTZdumpTab[ix].min = 0;
		else						  intTZdumpTab[ix].min = info.tm_min + 1;

		if ((info.tm_min + 1) == 60)  intTZdumpTab[ix].hour = info.tm_hour + 1;
		else						  intTZdumpTab[ix].hour = info.tm_hour;

		if (intTZdumpTab[ix].hour == 24)  intTZdumpTab[ix].hour = 0;
}


bool  ACS_CHB_aadst_CastcParams::checkIntTZdump (void)
{

		int  tzdIx = 0;

		while (tzdIx < SIZE_TZDUMP_TAB  &&  intTZdumpTab[tzdIx].type != NOT_USED)
		{
				if (intTZdumpTab[tzdIx].type == intTZdumpTab[tzdIx+1].type)  return false;		//  Error
				tzdIx ++;
		}

		return true;		// The content is correct :  the ISDST succession 0-1 or 1-0 is guaranteed
}


void  ACS_CHB_aadst_CastcParams::prepareCastcParams (int ix, int TZid)
{

	TCastcItem  castcItem;
	bool  DSTperiod;

	if (intTZdumpTab[ix].type == START_PERIOD)		//  prepare the CASTC Format 1 params :  refer to IP
	{
			if (intTZdumpTab[ix+1].type != END_PERIOD  ||
				intTZdumpTab[ix].clockAdj != intTZdumpTab[ix+1].clockAdj)
			{
					DEBUG(1, "%s", "prepareCastcParams :  Inconsistent Information !!");
					return;
			}
			castcItem.setDatBeg (intTZdumpTab[ix].day, intTZdumpTab[ix].month, intTZdumpTab[ix].year);
			castcItem.setTimBeg (intTZdumpTab[ix].hour, intTZdumpTab[ix].min);
			castcItem.setDatEnd (intTZdumpTab[ix+1].day, intTZdumpTab[ix+1].month, intTZdumpTab[ix+1].year);
			castcItem.setTimEnd (intTZdumpTab[ix+1].hour, intTZdumpTab[ix+1].min);
			castcItem.setClkAdj (intTZdumpTab[ix].clockAdj);
			castcItem.setTimeZoneId (TZid);

			DSTperiod = false;
	}

	if (intTZdumpTab[ix].type == END_PERIOD)		//  prepare the CASTC Format 2 params :  refer to IP
	{
			castcItem.setDatEnd (intTZdumpTab[ix].day, intTZdumpTab[ix].month, intTZdumpTab[ix].year);
			castcItem.setTimEnd (intTZdumpTab[ix].hour, intTZdumpTab[ix].min);
			castcItem.setClkAdj (intTZdumpTab[ix].clockAdj);
			castcItem.setTimeZoneId (TZid);

			DSTperiod = true;
	}

	if (castcItem.buildCastcParameter (&m_paramsForCASTC, DSTperiod) == false)
	{
			DEBUG(1, "%s", "prepareCastcParams :  Error returned by TCastcItem::buildCastc() !!");
			m_paramsForCASTC = "";
	}
}

/*-----------------------------------------------------------------------------*
 *  For a given TimeZone, this method evaluates the possibility that the 	   *
 *  CASTC command must be sent to the CP, following the next main steps :	   *
 *  																		   *
 *  		1.  loads the zdump list with the DST periods in a 2-years range   *
 *  		2.  compares the current time of the TZ with the start/end of the  *
 *  			DST periods and with the number of days before the DST change  *
 *  		3.  set the m_paramsForCASTC with the right parameters, if CASTC   *
 *  			must be sent or, otherwise, with an empty value				   *
 * 																			   *
 *  Return values :		 		  											   *
 *-----------------------------------------------------------------------------*/

void  ACS_CHB_aadst_CastcParams::setDLperiodForTZ (int TZid, string TZname, ACE_INT32 numDaysBefore)
{

	TDstExtractor  tdst;
	TZdumpData    *pzData;
	struct tm     *currTZtime;

	TAadstLocTime  locTime;
	time_t         rawTime;

	m_paramsForCASTC = "";

	time (&rawTime);

	currTZtime = locTime.AADST_localtime (&rawTime, (char *)TZname.c_str());		//  take the current year
																					//  relatively to the Time Zone

	tdst.getDstFromZone (TZname, currTZtime->tm_year + 1900,     			//  the returned list contains the zdump output
						 currTZtime->tm_year + 1900 + 2, &pzData);			//  and it is appropriately sorted

	std::list<TZdumpItem *>::iterator  it;

	if (pzData->getZdumpLst()->empty() == true)
	{
			DEBUG (1, "setDLperiodForTZ :  Empty TZdump list for %s", TZname.c_str());
			return;
	}

	struct tm  timeDst;
	int   counter = 0;
	int   tzdIx = 0;
	long  gmtOffs_1, gmtOffs_2;

	for (it=pzData->getZdumpLst()->begin(); it!=pzData->getZdumpLst()->end(); it++)
	{
			if (tzdIx >= SIZE_TZDUMP_TAB - 1)
			{
					DEBUG (1, "%s", "setDLperiodForTZ :  Insufficient size of the TZdump Tab !!!");
					return;
			}

			if ((counter % 2) == 0)			//  each DST date/time is represented by two elements in the list
			{
					if ((*it)->getDst() == true)  intTZdumpTab[tzdIx].type = END_PERIOD;
					else  						  intTZdumpTab[tzdIx].type = START_PERIOD;

					(*it)->getTzDate (&timeDst.tm_mday, &timeDst.tm_mon, &timeDst.tm_year);
					(*it)->getTzTime (&timeDst.tm_hour, &timeDst.tm_min, &timeDst.tm_sec);

					DLInfoDateSettings (tzdIx, timeDst);

					timeDst.tm_isdst = -1;							//  the DST information is not available
					timeDst.tm_mon --;  timeDst.tm_year -= 1900;
					intTZdumpTab[tzdIx].dateInSec = locTime.AADST_mktime (&timeDst, (char *)TZname.c_str());

					//DEBUG(1,"AADST.GENE:TZ[%s], tm_mday[%i], tm_mon[%i], tm_year[%i], mktime[%ld]",TZname.c_str(), ttt.tm_mday, ttt.tm_mon, ttt.tm_year, t1);

					gmtOffs_1 = (*it)->getGmtOffset();
			}
			else {
					gmtOffs_2 = (*it)->getGmtOffset();
					intTZdumpTab[tzdIx].clockAdj = getClockAdj (gmtOffs_1, gmtOffs_2);

					tzdIx ++;
			}
			counter ++;
	}
	intTZdumpTab[tzdIx].type = NOT_USED;

	if (! checkIntTZdump())
	{
			DEBUG (1, "setDLperiodForTZ :  the TZdump list is wrong for %s", TZname.c_str());
			return;
	}

	time_t  currInSec = locTime.AADST_mktime (currTZtime, (char *)TZname.c_str());   //  take the curr time (in secs) relatively to TZ

	tzdIx = 0;
	while (tzdIx < SIZE_TZDUMP_TAB  &&  intTZdumpTab[tzdIx].type != NOT_USED)
	{
		if (currInSec < intTZdumpTab[tzdIx].dateInSec)			//  consider only the DST date/time bigger than the current time
		{
				time_t  diffSecs = intTZdumpTab[tzdIx].dateInSec - currInSec;
				int     diffDays = diffSecs / SECONDS_IN_A_DAY;

				if (numDaysBefore == ALL_DAYS  ||  diffDays <= numDaysBefore)		//  check if the range to send CASTC is right
				{
						prepareCastcParams (tzdIx, TZid);
						break;
				}
		}
		tzdIx ++;
	}
}


/* -------------------------   ACS_CHB_aadst_handler  CLASS   ------------------------- */


ACS_CHB_aadst_handler::ACS_CHB_aadst_handler (ACS_CHB_aadst_ThreadManager *obj)
{

		objTM = obj;
}

/*----------------------------------------------------------------------------*
 *  Using the TCastcSender class, this method sends the CASTC command to the  *
 *  CP through MML :  in cmdPrintout[] is set the ADH answer.	  			  *
 *  A check is done on the CPid if it is necessary to provide the -cp info 	  *
 *  to the MML command														  *
 * 																			  *
 *  Return values :		0 -  if the MML cmd is correctly executed by ADH 	  *
 *						1 -  if the MML cmd is not executed by ADH or it is	  *
 *							 executed with errors							  *
 *----------------------------------------------------------------------------*/

int  ACS_CHB_aadst_handler::sendCASTCcommand (int CPid, string command, string *cmdPrintout)
{
	TCastcSender  sender;
	int  		  mmlError;
	string		  cpStr = "";

	if (CPid > 0)
	{
			char  buffer[15];
			sprintf (buffer, "%d", CPid);
			cpStr = "cp";  cpStr.append(buffer);
	}

	if (sender.LaunchMmlCmd (command, cpStr, cmdPrintout, &mmlError) == false) {
		ERROR(1, "ACS_CHB_aadst_handler::sendCASTCcommand: sender.LaunchMmlCmd return 'false' - errcode== %d  errstr == %s",mmlError, cmdPrintout->c_str());
		return CHB_AADST_FAILURE;
	}

	if (mmlError){
		ERROR(1, "ACS_CHB_aadst_handler::sendCASTCcommand: sender.LaunchMmlCmd failed' - errcode== %d  errstr == %s",mmlError, cmdPrintout->c_str());
		return CHB_AADST_FAILURE;
	}

	return CHB_AADST_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  For all the configured TZs, this method checks if there are two days      *
 *  before changing the DST, to establish if the A1 alarm must be raised	  *
 * 																			  *
 *  Return values :		true  -  minus than two days (A1 to be raised) 		  *
 *						false -  more than two days  (A1 not to be raised)	  *
 *----------------------------------------------------------------------------*/

bool  ACS_CHB_aadst_handler::checkTheNrOfDaysToTheDSTperiod (ACE_UINT32 numTZ)
{

	bool  minusThanTwoDays = false;		//  contains the result of this algorithm

	TDstExtractor  tdst;
	TZdumpData    *pzData;
	TAadstLocTime  locTime;

	time_t      rawTime, currInSec;			//  contains the date/time values in seconds
	struct tm  *currTZtime, timeDst;

	std::list<TZdumpItem *>::iterator  it;

	for (int j=0; j<(int)numTZ; j++)
	{
			time (&rawTime);

			currTZtime = locTime.AADST_localtime (&rawTime, (char *)TimeZones[j].TZname.c_str());	//  take the current year
																									//  relatively to the Time Zone

			currInSec = locTime.AADST_mktime (currTZtime, (char *)TimeZones[j].TZname.c_str());   	//  take the curr time (in secs) relatively to TZ

			tdst.getDstFromZone (TimeZones[j].TZname, currTZtime->tm_year + 1900,   //  the returned list contains the zdump output
								 currTZtime->tm_year + 1900 + 2, &pzData);			//  and it is appropriately sorted

			int   typeDst, previousTypeDst = NOT_USED;	  //  stores if a date is the start or the end of a DST period
			bool  consistency = true;					  //  for a TZ dump, indicates if the DST periods are coherent
			int   counter = 0;							  //  as indicated in the IP, select the correct record in a TZ dump

			for (it=pzData->getZdumpLst()->begin(); it!=pzData->getZdumpLst()->end(); it++)
			{
				if ((counter % 2) == 0)			//  each DST date/time is represented by two elements in the list
				{
						if ((*it)->getDst() == true)  typeDst = END_PERIOD;		//  only to check the data consistency of the TZdump list
						else  						  typeDst = START_PERIOD;

						if (typeDst == previousTypeDst)
						{
								DEBUG (1, "checkTheNrOfDaysToTheDSTperiod :  the dates for %s are inconsistent", TimeZones[j].TZname.c_str());
								consistency = false;
								break;
						}
						previousTypeDst = typeDst;		//  will be used to next cycle

						(*it)->getTzDate (&timeDst.tm_mday, &timeDst.tm_mon, &timeDst.tm_year);
						(*it)->getTzTime (&timeDst.tm_hour, &timeDst.tm_min, &timeDst.tm_sec);

						timeDst.tm_isdst = -1;							//  the DST information is not available
						timeDst.tm_mon --;  timeDst.tm_year -= 1900;
						time_t  dstTimeInSec = locTime.AADST_mktime (&timeDst, (char *)TimeZones[j].TZname.c_str());

						if (currInSec < dstTimeInSec)			//  consider only the DST date/time bigger than the current time
						{
								time_t  diffSecs = dstTimeInSec - currInSec;
								int     diffDays = diffSecs / SECONDS_IN_A_DAY;

								if (diffDays <= DAYS_FOR_A1_ALARM)		//  check the number of days to raise the A1 alarm
								{
										minusThanTwoDays = true;
								}
						}
				}
				counter ++;
			}
			pzData->getZdumpLst()->clear();

			if (consistency == true  &&  minusThanTwoDays == true)  return true;
	}

	return false;
}

bool ACS_CHB_aadst_handler::sendAndParsCastc(uint32_t uiCpCount, const string& strCmd, TCastcParser& objParser, std::list<struct MmlParams>* lstRetry)
{
	bool bRet;
	string strBuffer;
	struct MmlParams objFail;
	// Initialization
	bRet = false;
	objFail.CPid = -1;
	// Send the command ( FOR ANGELO: thise if could be merged in only one. But the code is not very readable :-) )
	if(sendCASTCcommand(uiCpCount, strCmd, &strBuffer) == CHB_AADST_SUCCESS){
		// Command has been sent with success. Parsing the response.
		if(objParser.IsCastcOrdered(strBuffer) == true){
			// Ok. Success
			bRet = true;
		}
	}
	// Check for retry
	if((lstRetry != NULL) && (bRet == false)){
		// Prepare the object: set CP that return the error
		objFail.CPid = uiCpCount;
		// Set the command to resend
		objFail.CastcCommand = strCmd;
		// Add the retry object to retry section.
		lstRetry->push_back(objFail);
	}
	// Exit from method
	return(bRet);
}

/*-----------------------------------------------------------------------------*
 *  This method is called by the ThreadManager only on the AP1 active Node     *
 *  and executes the general algorithm of the AADST service, following the	   *
 *  next main steps :														   *
 *  																		   *
 *  		1.  for each TZ, sends the CASTC command to all the CPs			   *
 *  		2.  retries the sending of the failed commands					   *
 *  		3.  handles the value of the numDaysRetry						   *
 *  		4.  checks the "Sending Session" result to raise the A2/A1 alarms  *
 * 																			   *
 *  Return values :		0 -  if Success 		  							   *
 *						1 -  if Failure	  									   *
 *-----------------------------------------------------------------------------*/

int ACS_CHB_aadst_handler::aadst_execute ()
{
	bool bIsMultiCp;
	uint32_t uiNumCp;
	uint32_t uiCpCount;
	int iFailCount;

	ACS_CHB_aadst_CastcParams   castcParams;
	acs_chb_aadst_alarmHandler  alarmHandler;

	ACE_UINT32  NumFailure;			//  the current value of the sending sessions consecutively failed
	ACE_INT32   NumDaysBefore;		//  the number of days before the DST change (this value is applicable to all the TZs)
	ACE_UINT32  NumAPtz;			//  the total number of Time Zones configured on the AP system

	// Initialization
	bIsMultiCp = false;
	uiNumCp = 0;
	uiCpCount = 0;
	iFailCount = 0;

	if (acs_chb_aadst_common::takeAPtimeZones (NumAPtz))  return CHB_AADST_FAILURE; 	// ----------------------------------------
	DEBUG (1, "aadst_execute :  %d TimeZones in APG:", NumAPtz);						// --
	acs_chb_aadst_common::timeZones_dump(NumAPtz);										// --			  S T A R T
																						// --	Info Loading and A2 alarm checking
	aadst_alarm_type  alarmType = alarmHandler.check_for_alarm_presence();				// --

	if (alarmType == CHB_AADST_ALARM_ERROR)  return CHB_AADST_FAILURE;

	if (alarmType == CHB_AADST_ALARM_A2)
	{
			DEBUG (1, "%s", "aadst_execute :  The A2 alarm is present, no CASTC will be sent !!!");

			if (checkTheNrOfDaysToTheDSTperiod (NumAPtz) == true)
			{
					DEBUG (1, "%s", "aadst_execute :  The A2 alarm must change in A1 !!!");

					if (alarmHandler.change_alarm_severity_to_A1 ())  return CHB_AADST_FAILURE;
			}
			return CHB_AADST_SUCCESS;
	}

	if (acs_chb_aadst_common::get_hbAADSTdayRetry_attribute (NumFailure))  return CHB_AADST_FAILURE;
	DEBUG (1, "aadst_execute :  Num Failure = %d ", NumFailure);

	if (acs_chb_aadst_common::get_hbAADSTstart_attribute (NumDaysBefore))  return CHB_AADST_FAILURE;
	DEBUG (1, "aadst_execute :  Num Days Before = %d ", NumDaysBefore);

	// Get the environment (single cp or blade cluster)
	if(ACS_CS_API_NetworkElement::isMultipleCPSystem(bIsMultiCp) == ACS_CS_API_NS::Result_Success){
		// switch for single or multi cp
		if(bIsMultiCp == true){
				DEBUG (1, "%s", "ACS_CHB_aadst_handler::aadst_execute - Multi CP environment");
			// Get the number of CP
			if(ACS_CS_API_NetworkElement::getDoubleSidedCPCount(uiNumCp) == ACS_CS_API_NS::Result_Success){
				// Debug the number of CP
				DEBUG (1, "ACS_CHB_aadst_handler::aadst_execute - Number of CP [%i]", uiNumCp);
			}else{
				// Fail to get the number of CP: Exit now
				DEBUG (1, "%s", "ACS_CHB_aadst_handler::aadst_execute - getDoubleSidedCPCount fail");
				return CHB_AADST_FAILURE;
			}
		}else{
			// Single CP environment: for debug only
			DEBUG (1, "%s", "ACS_CHB_aadst_handler::aadst_execute - Single CP environment");
		}
	}else{
		// Error: Unable to get environment.													// --
		DEBUG (1, "%s", "ACS_CHB_aadst_handler::aadst_execute - isMultipleCPSystem fail");		// --			   E N D
		return CHB_AADST_FAILURE;																// --
	}																							// -----------------------------------

	std::list<struct  MmlParams>  CmdToRetry;		//  contains only the CASTC commands that are failed on the first sending
	CmdToRetry.clear();

	TCastcParser  	  parser;
	string  	   	  adhPrintout;		//  contains the ADH answer to a CASTC command
	//struct MmlParams  cmdFailed;

	for (ACE_UINT32 j=0; j < NumAPtz; ++j)
	{
			castcParams.setDLperiodForTZ (TimeZones[j].TZid, TimeZones[j].TZname, NumDaysBefore);
			string  params = castcParams.getParamsForCASTC();		//  get the parameters for the next CASTC command to be sent
			if (params.empty())  continue;
			string  command = "CASTC:" + params;
		// switch for single or multi cp
		if(bIsMultiCp == true){
			// Is multi cp: Send the command for each cp
			for(uiCpCount = 1; uiCpCount <= uiNumCp; ++uiCpCount){
				// Send and persing the CASTC command
				sendAndParsCastc(uiCpCount, command, parser, &CmdToRetry);
			} // for
		}
		// Send CASTC to CP of the single CP or opgroup of the blade cluster environment
		sendAndParsCastc(0, command, parser, &CmdToRetry);
	}

	if (! CmdToRetry.empty())			//  there are some CASTC commands failed during the first sending :
	{									//  after 10 minutes, these commands must be sent again
		DEBUG (1, "%s", "aadst_execute :  a retry session is necessary because of errors on the first sending");

		for (int j=1; j<=DELAY_FOR_RETRY_SESSION; j++)      //  wait for 10 minutes
		{
				if (objTM->get_aadst_is_stopping())  return CHB_AADST_SUCCESS;		//  check if the thread is still running
				sleep (1);
		}

		if (acs_chb_aadst_common::getNodeState() != AADST_NODE_STATE_ACTIVE)  	//  the Node Sate could be changed in Passive
		{
				DEBUG (1, "%s", "aadst_execute :  the Active Node has changed his state !!!");
				return CHB_AADST_SUCCESS;
		}

		std::list<struct MmlParams>::iterator itRetry;
		// Point to first element
		itRetry = CmdToRetry.begin();
		// For each retry object
		while(itRetry != CmdToRetry.end()){
			// Send the command and check error
			if(sendAndParsCastc(itRetry->CPid, itRetry->CastcCommand, parser, NULL) == false){
				// An error occur. Increase the error counter
				++iFailCount;
			}
			// Next element
			++itRetry;
			// check if the thread is still running
			if (objTM->get_aadst_is_stopping()){
				// Force exit
				return CHB_AADST_SUCCESS;
			}
		} // while
	}

	if (iFailCount >= 1)			//  The "Sending session" is failed : check if raising an A2/A1 alarm (or no alarm)
	{							//  Refer to the AADST IP

			DEBUG (1, "%s", "aadst_execute :  the Sending Session is failed !!!");

			if (checkTheNrOfDaysToTheDSTperiod (NumAPtz) == true)
			{
					DEBUG (1, "%s", "aadst_execute :  The A1 alarm must be raised !!!");

					if (alarmHandler.alarm_raise (CHB_AADST_ALARM_A1))  return CHB_AADST_FAILURE;

					if (NumFailure > 0)			//  it is necessary to reset the current value of the Sending Failures
					{
							if (! acs_chb_aadst_common::set_hbAADSTdayRetry_attribute (0))  return CHB_AADST_FAILURE;
					}
			}
			else {
					NumFailure ++;

					if (NumFailure >= DAYS_FOR_A2_ALARM)		//  the Sending Session has consecutively failed for 5 days :
					{											//  A2 alarm must be raised

							DEBUG (1, "%s", "aadst_execute :  The A2 alarm must be raised !!!");

							if (alarmHandler.alarm_raise (CHB_AADST_ALARM_A2))  return CHB_AADST_FAILURE;

							if (acs_chb_aadst_common::set_hbAADSTdayRetry_attribute (0))  return CHB_AADST_FAILURE;
					}
					else {				//  NumFailure is updated in IMM to use it during the next cycle

							if (acs_chb_aadst_common::set_hbAADSTdayRetry_attribute (NumFailure))  return CHB_AADST_FAILURE;
					}
			}
	}
	else {
			if (NumFailure > 0)		//  when the Sending Session has a positive result, could be necessary to reset the current value of the Sending Failures
			{
					if (acs_chb_aadst_common::set_hbAADSTdayRetry_attribute (0))  return CHB_AADST_FAILURE;
			}
	}

	return CHB_AADST_SUCCESS;
}
