/*
 * ACS_CHB_aadst_handler.h
 *
 *  Created on: Jul 24, 2014
 *      Author: xassore
 */

#ifndef ACS_CHB_AADST_HANDLER_H_
#define ACS_CHB_AADST_HANDLER_H_


#include "acs_chb_aadst_ThreadManager.h"
#include "acs_chb_aadst_TCastcFacility.h"
#include <list>

#define  DAYS_FOR_A1_ALARM				 2		//  number of days before to change the DST period, used to raise the A1 alarm
#define  DAYS_FOR_A2_ALARM				 5		//  number of days to check the consecutive failures, before to raise the A2 alarm
#define  ALL_DAYS						-1		//  used when the CASTC command has to be sent every day
#define  DELAY_FOR_RETRY_SESSION		600		//  the retry session starts after 10 minutes

#define  NOT_USED			0		//  used to establish the start/end of the DST period
#define  START_PERIOD		1
#define  END_PERIOD			2

#define  SIZE_TZDUMP_TAB		50


struct  InternalTZdump			//  internal representation of the TZdump list, for a TZ
{
	int       type;
	int       clockAdj;
	int		  day, month, year;
	int		  hour, min;
	time_t    dateInSec;
};


struct  MmlParams
{
	int      CPid;
	string   CastcCommand;
};


class ACS_CHB_aadst_CastcParams
{
private :
	void  stepToNextDay (struct tm *dateInfo);
	void  DLInfoDateSettings (int ix, struct tm info);
	void  prepareCastcParams (int ix, int TZid);
	int   getClockAdj (long gmtOffs_1, long gmtOffs_2);
	bool  checkIntTZdump (void);

	string  m_paramsForCASTC;

public :
	inline  ACS_CHB_aadst_CastcParams() {};
	inline  ~ACS_CHB_aadst_CastcParams() {};

	void  setDLperiodForTZ (int TZid, string TZname, ACE_INT32 numDaysBefore);

	inline string  getParamsForCASTC (void)
	{
			return  m_paramsForCASTC;
	}
};


class ACS_CHB_aadst_handler
{
private:
	int     sendCASTCcommand (int CPid, string command, string *cmdPrintout);
	bool    checkTheNrOfDaysToTheDSTperiod (ACE_UINT32 numTZ);
	// Send mml "strCmd" to CP "uiCpCount". If "uiCpCount" is 0, the mml is send without "-cp" params.
	// If send\parsing fail, the fail mml command is pushed in lstRetry list and method return false.
	// If lstRetry is NULL, no retry object is pushed in "lstRetry" list.
	bool sendAndParsCastc(uint32_t uiCpCount, const string& strCmd, TCastcParser& objParser, std::list<struct MmlParams>* lstRetry);
	ACS_CHB_aadst_ThreadManager  *objTM;

public:
	inline ACS_CHB_aadst_handler(){};
	ACS_CHB_aadst_handler (ACS_CHB_aadst_ThreadManager *obj);
	inline ~ACS_CHB_aadst_handler(){};

	int  aadst_execute ();
};


#endif /* ACS_CHB_AADST_HANDLER_H_ */
