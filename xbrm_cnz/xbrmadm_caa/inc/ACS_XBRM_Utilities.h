#ifndef ACS_XBRM_UTILITIES_H_
#define ACS_XBRM_UTILITIES_H_

#include "acs_apgcc_omhandler.h"
#include <sec/crypto_api.h>
#include <sec/crypto_status.h>

#include <cstdarg>
#include <set>


#define TQ_ATTACH_FAILED 222
#define TQ_STATE_FAILED 223
#define TQ_SEND_FAILED 224
#define FRST          			1
#define SCND           			2
#define BOTH           			3
#define JAN 1
#define FEB 2
#define DEC 12
#define WK  7
#define GMT				"GMT"
#define NONEXTSCHEDULE	"0000-00-00T00:00:00"	/*Indicates there is no further scheduling*/
#define TIME_ZONE_LINK				"TimeZoneLink"
#define timeZoneIdAttr			"timeZoneId"
#define timeZoneStringAttr		"timeZoneString"
#define ACS_XBRM_INVALID_STOPTIME		16	/*Stop time not valid */
#define CREATE_T		1
#define MODIFY		2
#define PERIODIC	2
#define	NOEVENT				4
#define LASTSCHEDULEDATE		"lastScheduledDate"
#define nextScheduledTimeAttr		"nextScheduledEventTime"
#define COMMA_STR			","
#define	adminStateAttr			"adminState"
#define startTimeAttr			"startTime"
#define weeksAttr			"weeks"
#define daysAttr			"days"
#define monthsAttr			"months"
#define	hoursAttr			"hours"
#define minutesAttr			"minutes"
#define AlarmClass          "AlarmHandler"
struct DT
{
	int dd;
	int mon;
	int yy;
	int hh;
	int mm;
	int ss;
};

/*
* @brief   This structure is used to hold the periodicity values
*/
struct TM
{
	int mon;
	int wk;
	int dd;
	int hh;
	int mm;
	int ss;
};
namespace ACS_XBRM_UTILITY
{
    const string IMPL_SYSTEM_BRM_ASYNC_NAME = "SystemBrMAsyncObjImpl";
    const string IMPL_SYSTEM_BRM_BACKUP_SCHEDULER_ASYNC_NAME = "SystemBrMAsyncObjImpl";

    //EVENTS
    const string EVENT_SHUTDOWN_NAME = "ACS_XBRM_SHUTDOWN";
    const string EVENT_SYSTEM_BACKUPS_START_NAME = "ACS_XBRM_SYSTEM_BACKUPS_START";
    const string EVENT_SYSTEM_BACKUPS_END_NAME = "ACS_XBRM_SYSTEM_BACKUPS_END";
    const string EVENT_EVENT_SCHEDULING_START_NAME = "ACS_XBRM_EVENT_SCHEDULING_START";
    const string EVENT_EVENT_SCHEDULING_END_NAME = "ACS_XBRM_EVENT_SCHEDULING_END";

    //Alarm
    const string alarmDir = "/data/acs/data/xbrm/";
    const string alarmFile = "backupAlarm";
    extern char* reportProgressRdn[2];
    extern char* cpBackupName[6];
    

    //tq validations
    const std::string SUBSYS("ACS");
    const std::string APPNAME("acs_xbrmd");

    typedef enum{
        ON_DEMAND    = 0,
        SCHEDULED   = 1
    }BackupCategory;

    typedef enum{
        DOMAIN_TYPE = 0,
        NODE_TYPE   = 1
    }SystemBackupType;
    
    typedef enum{
        LOCKED = 0,
        UNLOCKED = 1
    }adminState;

    typedef enum{
        ALL         = 0,
        APG         = 1,
        CP1         = 2,
        CP2         = 3,
        CPCLUSTER   = 4,
        CP          = 5
    }BackupType;

    typedef enum{
        TQ_ATTACH_FAIL,
        TQ_DETACH_FAILED,
        TQ_INVALID_INITIATING_DESTINATION_SET,
        TQ_INVALID_RESPONDING_DESTINATION_SET,
        TQ_INVALID_TRANSFER_PROTOCOL,
        TQ_INTERNAL_ERROR,
        TQ_VALID
    }ExitCodes;

    typedef enum{
		SUCCESS         = 1,
		FAILURE         = 2, 
		NOT_AVAILABLE   = 3
	}result;

	typedef enum{
		CANCELLING  = 1, 
		RUNNING     = 2, 
		FINISHED    = 3, 
		CANCELLED   = 4
	}state;

	typedef enum{
		CREATE = 1,
		EXPORT = 2
	}action;

    typedef enum
    {
        BACKUPCREATE = 1,
        BACKUPEXPORT = 2
    } alarmType;

    typedef struct{
        std::string backupType;
        std::string backupFileName;
        std::string backupPath;
        std::string backupRelVolume;
    }backupInfo;

} // namespace ACS_XBRM_UTILITY

extern "C"{
 extern SecCryptoStatus sec_crypto_decrypt_ecimpassword(/*OUT*/ char** plaintext, const char* ciphertext);
}

/*
* @brief   This static array consists of last day for every month
*/
static int lastDay[]={31,28,31,30,31,30,31,31,30,31,30,31};
/*============================================================================
                      STRUCT FOR event details
==============================================================================*/
struct eventDetails 
{
        string scheduleDate;
        string event_rdn;
        /*
         * @brief  constructor with two parameters
         */
        eventDetails(string date, string rdn)
        {
                event_rdn        = rdn;
                scheduleDate   = date;
        }
};
static const char* CLASS_BRM_PERIODIC_EVENT_NAME = "SystemBrMSystemBrmPeriodicEvent";

class ACS_XBRM_Utilities
{
private:
    /* data */
public:
    ACS_XBRM_Utilities(/* args */);
    ~ACS_XBRM_Utilities();
    int isValidTransferQueue(const string &transferQueue, string &transferQueueDn);
    ACS_CC_ReturnType updateReportProgress(char *objName, char *actionName, int state = 3, int result = 3, char *resultInfo = (char *)"", bool completed = 0, char *additionalInfo = (char *)"", int progressPercentage = -1, char *progressInfo = (char *)"", bool isRestore = false);
    ACS_CC_ReturnType resetReportProgress(char *objName);
    ACS_CC_ReturnType updateReportProgressAttribute(char *objName, char* attributeName, ACS_CC_AttrValueType attributeType, int attributeValuesNum, void* attributeValuesp[]);
    string getExitCodeText(int exitCode);
    static string getCurrentTime();
    static string getCurrentTimeFormat();
    static int sendFilesToGOH (string auditDir, string TQ,string fileToSend);
    static std::string decryptString(const std::string &strPwdCrypt);

    static int exportCpBackupToApgFileSystem(string backupFileName, string backupType, string backupRelVolume, string* exportError);
    void updateNextScheduledEventTime();
    void updateLastScheduledEventTime(string curScheduledTime);

    static int getPercentage(int currentDomainNumber, int noOfDomains);
    string getImmAttribute(string dnName,string attributeName);
    bool compareDates(string d1, string d2);
    ACE_INT32 timeValidation(string &time, string attributeType);
    ACE_INT32 timezone_validation(string startTime);
    ACE_INT32 date_format_validation(string startTime);
    string localZoneTime(string date);
    string getNearestTime(string d1, string d2);
    string nextSchedule(string d1,string d2,int day);
    string decreaseOneDay(string date);
    bool isLeapYear(DT dt);
    int lastDayOfMonth(DT &dt) ;
    string getLocalTimeZone();
    int getImmInstances(const char* className, vector<string> &dnList);
    unsigned int getImmIntAttribute(string dnName,string attributeName);
    bool isAdminStateLOCKED(string rdn);
    void updateNextScheduledTime(string eventRDN,vector<eventDetails> events = vector<eventDetails>(),int eventType = NOEVENT, int changeType = CREATE_T );
    void getSchedulesOfPeriodicEvents(set<string> &scheduledTimes,string eventRDN,vector<eventDetails> events=vector<eventDetails>(),int eventType=NOEVENT);
    string parseRDN(string eventRDN);
    ACS_CC_ReturnType updateImmAttribute(string rdn,string attr,ACS_CC_AttrValueType type,void** value);
    void periodic_updationAfterJobExecution(string rdn);


    /**
    *  isLeapYear method
    *  This method will check whether the provided year is a leap year or not
    *  @param year  : year Ex:1900
    *  @return bool : return 'true' if the year is leap year else 'false'
    */
    bool isLeapYear(int year);
    

    void addWeeks(DT &dt,int wk);

	/**
    *  addDays method: This method is used to add days to the last scheduled date.
    *  @param  dt   : date in DT format as reference
    *  @param  int  : number of dayss in periodicity
    *  @return void : no return value
    */	
	void addDays(DT &dt,int days);

    /**
    *  addTime method: This method is used to add time(hours and minutes) to the last scheduled date.
    *  @param  dt  : date in DT format as reference
    *  @param  t1  : time in TM structure format
    *  @return void : no return value
    */
    void addTime(DT &dt,TM t1);

    

    /** 
    *  nextDay method  : Given a Date in crnDay, Return the next day in nxtDay .
    *  @param  dt      : date in DT format as reference
    *  @param  nxtDay  : reference of DT
    *  @return void : no return value
    */
    void nextDay(DT crntDay, DT &nxtDay);

    /**
    *  addDates method : Given a Date in crnDay,It will add dates.
    *  @param  dt      : date in DT format as reference
    *  @param  nxtDay  : reference of DT
    *  @return void    : no return value
    */
    void addDates(DT &dt,DT dt1);

    /**
    *  convHour method : to convert the hours.
    *  @param  dt      : date in DT format as reference
    *  @param  t1	    : periodicity in TM format
    *  @return void    : no return value
    */
    void convHour(DT &dt1,TM t1);

    /**
    *  convMin method : to convert the minutes.
    *  @param  dt      : date in DT format as reference
    *  @param  t1      : periodicity in TM format
    *  @return void    : no return value
    */
    void convMin(DT &dt1,TM t1);

    /**
    *  convSec method  : to convert the seconds.
    *  @param  dt      : date in DT format as reference
    *  @param  t1      : periodicity in TM format
    *  @return void    : no return value
    */
    void convSec(DT &dt1,TM t1);
    OmHandler immHandler;
};

#endif
