//=============================================================================
/**
   @file    acs_hcs_global_ObjectImpl.h

   @brief Header file for HC module.
          It acts as global class for all OI handlers.

   @version 1.0.0

 HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A			   XMALRAO   Initial Release
 */
//=============================================================================

#ifndef ACS_HC_GLOBAL_OBJECTIMPL_H
#define ACS_HC_GLOBAL_OBJECTIMPL_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include <ACS_TRA_trace.h>
#include "acs_hcs_tra.h"
#include <cstdarg>
#include <set>

using namespace std;

/*====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */

//define declarations for functions
#define EXECUTE			1

//define declarations for errors
#define ACS_HC_INVALID_TIME		5	/*Invalid scheduled time*/
#define ACS_HC_INVALID_TIMEZONE		6	/*Invalid time zone*/
#define ACS_HC_MANDATTR_MISSED		8	/*Unset Mandatory attribute*/
#define ACS_HC_INVALID_CATEGORY		9	/*Invalid category*/
#define ACS_HC_INVALID_TRIGGERJOB	10	/*Invalid job*/
#define ACS_HC_JOB_ONGOING		11	/*Job is ongoing */
#define ACS_HC_JOBAS_TRIGGERJOB		12	/*This job is referenced in another job*/
#define ACS_HC_SCHEDULER_UNLOCK		13 	/*Admin state for job Scheduler is not locked*/
#define ACS_HC_STARTTIME_EXPIRED	14	/*Start/scheduled Time has expired*/
#define ACS_HC_STOPTIME_EXPIRED		15	/*Stop Time has expired*/
#define ACS_HC_INVALID_STOPTIME		16	/*Stop time not valid */
#define ACS_HC_INVALID_TARGET       	17	/*Invalid systemComponentTarget*/
#define ACS_HC_INVALID_TQ		18	/*Invalid destinationUri*/
#define ACS_HC_RC_TQUNDEF           	19      /*Transfer queue not defined*/
#define ACS_HC_RC_NOAFPACESS        	20      /*No AFP server access*/
#define ACS_HC_RC_INTPROGERR        	21      /*Internal error code*/
#define ACS_HC_INVALID_ATTR		22	/*Invalid rule file */
#define ACS_HC_INVALID_PARAMS		23	/*Mismatch in number of parameters*/
#define ACS_HC_INVALID_JOBNAME		24
#define ACS_HC_AUGMENT_FAILED		25	/*Augmentation failure for the event/job*/
#define ACS_HC_ENABLE			0	
#define ACS_HC_DISABLE			1
#define ACS_HC_OK			0
#define ACS_HC_NOTOK			1
#define ACS_HC_NA			3
#define FRST          			1
#define SCND           			2
#define BOTH           			3
#define STATE_RUNNING                   1
#define STATE_NOT_RUNNING		0


//define declarations for OI attributes
#define scheduledTimeAttr		"scheduledTime"
#define singleEventIdAttr		"singleEventId"
#define stopTimeAttr			"stopTime"
#define startTimeAttr			"startTime"
#define weeksAttr			"weeks"
#define daysAttr			"days"
#define monthsAttr			"months"
#define	hoursAttr			"hours"
#define minutesAttr			"minutes"
#define periodicEventIdAttr		"periodicEventId"
#define targetAttr			"systemComponentTarget"
#define transferQueueAttr               "destinationUri"
#define transferQueueDNAttr		"destinationDn"
#define jobIdAttr			"jobId"
#define jobToTriggerAttr		"jobToTrigger"
#define categoriesAttr			"categories"
#define compressionAttr			"compression"
#define successPrintoutsAttr		"successLogs"
#define timeAttr			"time"
#define dayOfWeekAttr			"dayOfWeek"
#define dayOfMonthAttr			"dayOfMonth"
#define	dayOfWeekOccurenceAttr		"dayOfWeekOccurrence"
#define monthAttr			"month"
#define calendarPeriodicEventIdAttr	"calendarPeriodicEventId"
#define	adminStateAttr			"adminState"
#define schedulerStateAttr		"schedulerState"
#define nextScheduledTimeAttr		"nextScheduledTime"
#define LASTSCHEDULEDATE		"lastScheduledDate"
#define JOBSCHEDULER			"jobSchedulerId=1,"
#define JOBSCHEDULERID			"jobSchedulerId=1"
#define executionStatusAttr		"execStatus"
#define categoriesIdAttr		"categoryId"
#define reportIdAttr			"reportId"
#define TQID				"fileTransferQueueId"
#define systemTypeAttr			"systemType"
#define AXEFUNCTIONS			"axeFunctionsId=1"
#define jobSchedulerAttr  		"jobSchedulerId"
#define statusAttr			"status"
#define timeZoneIdAttr			"timeZoneId"
#define timeZoneStringAttr		"timeZoneString"
#define networkElementIdAttr            "networkManagedElementId"

//define declarations of OI class names
#define ACS_HC_SINGLEEVENT_CLASS_NAME		"HealthCheckSingleEvent"
#define ACS_HC_PERIODICEVENT_CLASS_NAME		"HealthCheckPeriodicEvent"
#define ACS_HC_JOBBASED_CLASS_NAME 		"HealthCheckJob"
#define ACS_HC_REPORT_CLASS_NAME 		"HealthCheckReport"
#define ACS_HC_INUSE_CLASS_NAME			"HealthCheckInUse"
#define ACS_HC_READYTOUSE_CLASS_NAME		"HealthCheckReadyToUse"
#define ACS_HC_CALENDERPERIODICEVENT_CLASS_NAME "HealthCheckCalendarPeriodicEvent"
#define ACS_HC_JOBSCHEDULER_CLASS_NAME		"HealthCheckJobScheduler"
#define ACS_HC_CATEGORY_CLASS_NAME		"HealthCheckCategory"
#define ACS_HC_ID				"HealthCheckhealthCheckMId=1"
#define ACS_HC_RULE_CLASS_NAME			"HealthCheckRule"
#define ACS_HC_RULESETFILE_CLASS_NAME		"HealthCheckRuleSetFile"
#define ACS_HC_ASYNCACTION_STRUCT_NAME		"HealthCheckAsyncActionProgress"
#define ACS_HC_RULEFILEMANAGER_CLASS_NAME	"HealthCheckRuleFileManager"
#define ACS_HC_TQ_CLASS_NAME			"AxeDataTransferFileTransferQueue"
#define TIME_ZONE_LINK				"TimeZoneLink"
#define MANAGED_ELEMENT                         "ManagedElement"
#define CATEGORY_NAME				"HealthCheckCategory"

//define declarations for IMMCallBacks
#define ACS_HC_READYTOUSE_IMPL			"ReadyToUseImplementer"
#define ACS_HC_INUSE_IMPL			"InUseImplementer"
#define ACS_HC_JOBBASED_IMPL 			"JobBasedImplementer"
#define ACS_HC_JOBSCHEDULER_IMPL 		"JobSchedulerImplementer"
#define ACS_HC_SINGLEEVENT_IMPL   		"SingleEventImplementer"
#define ACS_HC_PERIODICEVENT_IMPL 		"PeriodicEventImplementer"
#define ACS_HC_CALENDARPERIODIC_IMPL 		"CalendarPeriodicEventImplementer"

#define NBI_RULES 	"/data/opt/ap/internal_root/health_check/rules/"
#define NBI_REPORTS 	"/data/opt/ap/internal_root/health_check/reports/"
#define INTERNAL_PATH 	"/cluster/storage/system/config/acs_hc/"
#define READYTOUSEDN 	"readyToUseId=1,ruleFileManagerId=1,HealthCheckhealthCheckMId=1"
#define INUSEDN 	"inUseId=1,ruleFileManagerId=1,HealthCheckhealthCheckMId=1"
#define NOJOB           "No job for execution"
#define NONEXTSCHEDULE	"0000-00-00T00:00:00"	/*Indicates there is no further scheduling*/
#define ALL_CATEGORIES  "ALL"
#define MAX_STOPTIME	"2099-12-31T23:59:59" 

//define error messages to display in ProgressReport
#define INTERNAL_ERR_PR			"Internal Error"
#define XMLFILELOAD_ERR_PR		"Error reading XML file"
#define SAMERID_UNLOAD_ERR_PR		"Cannot be unloaded"
#define CATEGORYUSED_UNLOAD_ERR_PR	"Categories referenced"
#define SUCESS_UNLOAD_PR		"Successfully unloaded"
#define SAMEFILENM_LOAD_ERR_PR 		"Cannot be loaded as an instance with same file name exists"
#define CANNOT_LOAD_ERR_PR		"Cannot be loaded"
#define SUCCESS_LOAD_PR			"Successfully loaded"

//define error codes for validations
#define STARTTIME_EXPIRED		"Start time has expired"
#define INVALID_TIME			"Invalid scheduled time"
#define INVALID_TIMEZONE		"Invalid Time zone"
#define STOPTIME_EXPIRED		"Stop Time has expired"
#define INVALID_STOPTIME		"Stop time not valid"
#define SCHEDULEDTIME_EXPIRED		"Scheduled time has expired"
#define INVALID_SCHDTIME		"Invalid scheduled time"	
#define INVALID_JOB			"Invalid job"
#define INVALID_JOBNAME			"Invalid job name"
#define INVALID_CATEGORY		"Invalid category"
#define INVALID_TARGET			"Invalid systemComponentTarget"
#define INVALID_TQ			"Invalid destinationURI"
#define JOB_ONGOING			"Job in progress"
#define JOBAS_TRIGGERJOB		"This job is referenced in another job"
#define SCHEDULER_UNLOCK		"Admin state for job Scheduler is not locked"
#define TQ_NT_DEFINED			"Transfer queue not defined"
#define	AFP_NO_ACCESS			"No AFP server access"
#define INTERNAL_ERR_CODE		"Internal error code"
#define ANOTHERJOBINPRGRES		"Another job is in progress" 
#define RULESETFILE_ALREADY_EXIST       "Instance of RuleSetFile is already under ReadyToUse"
#define RULESETFILE_EXIST_ALREADY	"Instance of RuleSetFile is already under InUse"
#define DUPLICATE_INSTANCE		"Cannot be loaded as an instance with same Rule Set ID exists"
#define rulesetFileIdAttr_with_EQU	"ruleSetFileId="
#define rulesetFileIdAttr       	"ruleSetFileId"
#define ruleReferenceAttr		"ruleReferences"
#define fileNameAttr			"fileName"
#define progressReport_Attr 		"progressReport"
#define categoriesAttr_with_EQU		"categoryId="
#define idEquOne_Comma			"id=1,"
#define destinationUpdate               ",SystemFunctions=1,AxeFunctions=1,DataOutputHandling=1,DataTransferM=1,FileTransferManager=1,FileTransferQueue="

#define EQUALTO_STR			"="
#define EMPTY_STR			""
#define COMMA_STR			","
#define ruleID_with_EQU			"ruleId="
#define ruleID_STR		        "ruleId"
#define name_STR			"name"
#define description_STR			"description"
#define CP_STR				"CP"
#define BC_STR				"BC"
#define actionName_STR			"actionName"
#define additionalInfo_STR		"additionalInfo"
#define progressInfo_STR		"progressInfo"
#define progressPercentage_STR		"progressPercentage"
#define result_STR			"result"
#define resultInfo_STR			"resultInfo"
#define state_STR			"state"
#define actionId_STR			"actionId"
#define timeActionStarted_STR		"timeActionStarted"
#define timeActionCompleted_STR		"timeActionCompleted"
#define timeOfLastStatusUpdate_STR	"timeOfLastStatusUpdate"
#define LASTREPORTNAME			"lastReportName"
#define LastupdateTime_STR		"lastUpdateTime"
#define id_STR				"id"
#define GMT				"GMT"
#define RuleSetFile			"RuleSetFile="

#define UNLOCK				1				
#define LOCK				0
#define ENABLE				1
#define DISABLE				0
#define EXECUTED			1	
#define NOTEXECUTED			0
#define STATE_CANCELLED			4
#define STATE_RUNNING			2
#define CREATE				1
#define MODIFY				2
#define SINGLE				1
#define PERIODIC			2
#define CALENDAR			3
#define	NOEVENT				4
/*============================================================================
                      STRUCT FOR CALENDAR OBJECT PARAMETERS
==============================================================================*/
struct CALENDER_Parameters
{
        char StartTime[32];
        char Time[10];
        char StopTime[32];
        unsigned int DayOfMonth;
        unsigned int DayOfWeek;
        unsigned int DayOfWeekOccurence;
        unsigned int Month;
};

/*============================================================================
                      STRUCT FOR rule status
==============================================================================*/
struct ruleStat
{
        int status;
        string recommendedAction;
};

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

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/**
 * @brief acs_hcs_global_ObjectImpl
 * acs_hcs_global_ObjectImpl class is having common functions using across all the classes.
 */
class acs_hcs_global_ObjectImpl
{
	 /*=====================================================================
                              PUBLIC DECLARATION SECTION
        ==================================================================== */
	public:
		/**
         	* @brief
	        * constructor with two parameters
        	*/
		acs_hcs_global_ObjectImpl();

		/**
		* @brief
		* destructor
		*/
		~acs_hcs_global_ObjectImpl();

		 /**
		 *  date_format_validation method.
		 *  This method will validate whether the provided date is valid or not.
		 *  @param  time      :   string
		 *  @return ACS_INT32 : corresponding return codes
		 */
		ACE_INT32 date_format_validation(string time);

		/**
                 *  getCurrentTime method.
                 *  This method will gives the current time.
                 *  @return string : returns the value of current time in 19 digit format "yyyy-mm-ddThh:mm:ss"
                 */
		string getCurrentTime();

		 /**
                 *  timezone_validation method.
                 *  This method will validate whether the timezone provided in time is valid or not.
                 *  @param  time      :   string
                 *  @return ACS_INT32 : corresponding return codes
                 */
		ACE_INT32 timezone_validation(string time);

		/**
                 *  updateImmAttribute method.
                 *  This method will update the attribute in IMM data base with the value provided.
                 *  @param rdn        : value of rdn that needs to update
		 *  @param attr	      : attribute that need to modify in the rdn
		 *  @param type       : type of the value Ex:ATTR_STRINGT,ATTR_INT32T,...
                 *  @param value      : value with which the attribute needs to update 	 
                 *  @return ACS_INT32 : ACS_CC_SUCCESS/ACS_CC_FAILURE
                 */
		ACS_CC_ReturnType updateImmAttribute(string rdn,string attr,ACS_CC_AttrValueType type,void** value);

		/**
                 *  timeValidation method.
                 *  This method will validate whether the provided time is grater then current time or not.
                 *  @param time       : value of date
                 *  @param attribute  : stoptime/startTime/scheduledTime
                 *  @return ACS_INT32 : corresponding return code 
                 */
		ACE_INT32 timeValidation(string &time, string attribute);

		 /**
                 *  getImmAttribute method.
                 *  This method will give the value of IMM attribute only for ATTR_STRINGT and ATTR_NAMET types
                 *  @param dnName     : value of RDN
                 *  @param attribute  : name of the attribute in the dn
                 *  @return string    : returns the value of the attribute
                 */
		string getImmAttribute(string dnName,string attribute);

		/**
                 *  getImmIntAttribute method.
                 *  This method will give the value of IMM attribute only for ACE_INT32T,ACE_UINT32 types.
                 *  @param dnName     : value of RDN
                 *  @param attribute  : name of the attribute in the dn
                 *  @return string    : returns the value of the attribute
                 */
		unsigned int getImmIntAttribute(string dnName,string attributeName);

		 /**
                 *  getImmInstances method.
                 *  This method will give all the instances for the class provide in a vector.
                 *  @param className  : name of the class Ex:HealthCheckSingleEvent
                 *  @param dnList     : a vector in which the instances will get populated
                 *  @return string    : returns error code in case of failure otherwise zero
                 */
		int getImmInstances(const char* className, vector<string> &dnList);

		 /**
                 *  parseRDN method.
                 *  This method will parse the provided RDN.
                 *  @param  eventRDN : complete dn Ex:periodicEventId=1,jobSchedulerId=1,jobId=HC,HealthCheckhealthCheckMId=1
                 *  @return string   : returns a string in the form "jobId=HC"
                 */
		string parseRDN(string eventRDN);

		 /**
                 *  updateNextScheduleTime method.
                 *  This method will update the job scheduler with least possible time among all scheduling events.This function would get called in case of creation/modification for any scheduling events(single, periodic and calendar).
                 *  @param  events : Vector of rdn and its corresponding scheduled times
		 &  @param changeType : To identify whether the call is from create/modify of event
		 *  @param eventType : Any one among SINGLE, PERIODIC and CALENDAR
                 *  @return void   : no return value
                 */
                void updateNextScheduledTime(string eventRDN,vector<eventDetails> events = vector<eventDetails>(),int eventType = NOEVENT, int changeType = CREATE );

		 /**
                 *  getSchedulesOfSingleEvents method.
                 *  This method will fill the set with the values of next schedule time for all single events.
                 *  @param  eventRDN : job id string Ex:"jobId=HC"
		 *  @param  scheduleTimes : a set which will hold the values of all schedule times
		 *  @param eventType : Any one among SINGLE, PERIODIC and CALENDAR, If function is not getting called from events then it will take default value.
                 *  @return void   : no return value
                 */
		void getSchedulesOfSingleEvents(set<string> &scheduledTimes,string eventRDN,vector<eventDetails> events=vector<eventDetails>(),int eventType=NOEVENT);

		/**
                 *  getSchedulesOfPeriodicEvents method.
                 *  This method will fill the set with the values of next schedule time for all periodic events.
                 *  @param  eventRDN : job id string Ex:"jobId=HC"
                 *  @param  scheduleTimes : a set which will hold the values of all schedule times
		 *  @param eventType : Any one among SINGLE, PERIODIC and CALENDAR, If function is not getting called from events then it will take default value.
                 *  @return void   : no return value
                 */
                void getSchedulesOfPeriodicEvents(set<string> &scheduledTimes,string eventRDN,vector<eventDetails> events=vector<eventDetails>(),int eventType=NOEVENT);

		/**
                 *  getSchedulesOfCalendarEvents method.
                 *  This method will fill the set with the values of next schedule time for all calendar events.
                 *  @param  eventRDN : job id string Ex:"jobId=HC"
                 *  @param  scheduleTimes : a set which will hold the values of all schedule times
		 *  @param eventType : Any one among SINGLE, PERIODIC and CALENDAR, If function is not getting called from events then it will take default value.
                 *  @return void   : no return value
                 */
                void getSchedulesOfCalendarEvents(set<string> &scheduledTimes,string eventRDN,vector<eventDetails> events=vector<eventDetails>(),int eventType=NOEVENT);

		 /**
                 *  periodic_updationAfterJobExecution method.
                 *  This method will update the periodic event with its next schedule time after its first execution.
                 *  @param  eventRDN : rdn of the periodic event
				Ex:periodicEventId=1,jobSchedulerId=1,jobId=HC,HealthCheckhealthCheckMId=1
                 *  @return void   : no return value
                 */
		void periodic_updationAfterJobExecution(string rdn);

		 /**
                 *  cal_updationAfterJobExecution method.
                 *  This method will update the calendar event with its next schedule time.
                 *  @param  eventRDN : rdn of the calendar event
                                Ex:calendarPeriodicEventId=1,jobSchedulerId=1,jobId=HC,HealthCheckhealthCheckMId=1
                 *  @return void   : no return value
                 */ 
		void cal_updationAfterJobExecution(string rdn,bool time_flag=false);

		/**
                 *  getCalNextScheduledTime method.
                 *  This method will caliculate the next possible scheduled time according to the input from calendar.
                 *  @param  request : a structure which consists of all the calendar parameters
                 *  @return string   : returns next possible schedule time, if there is no possibility function will returns "0000-00-00T00:00:00"
                 */
		string getCalNextScheduledTime(CALENDER_Parameters *request);

		/**
		 *  isLeapYear method
		 *  This method will check whether the provided year is a leap year or not
		 *  @param year  : year Ex:1900
		 *  @return bool : return 'true' if the year is leap year else 'false'
		 */
		bool isLeapYear(int year);
		
		 /**
                 *  parseFirstString method
                 *  This method will parse the provided value till first comma
                 *  @param eventRDN  : eventRDN Ex:calendarPeriodicEventId=1,jobSchedulerId=1,jobId=HC
                 *  @return bool : return the parsed string Ex:calendarPeriodicEventId=1
                 */
		string parseFirstString(string eventRDN);

		/**
                 *  getNearestTime method
                 *  This method will give the nearest time between two provided times
                 *  @param date1   : date1 in the format "yyyy-mm-ddThh:mm:ss"
		 *  @param date2   : date2 in the format "yyyy-mm-ddThh:mm:ss"
                 *  @return string : return the nearest time as string
                 */
		string getNearestTime(string date1, string date2);

		/**
                 *  compareDates method
                 *  This method will compare two dates
                 *  @param date1   : date1 in the format "yyyy-mm-ddThh:mm:ss"
                 *  @param date2   : date2 in the format "yyyy-mm-ddThh:mm:ss"
                 *  @return bool  : return 'true' id date1<date2 else return 'false'
                 */	
		bool compareDates(string date1, string date2);

		 /**
                 *  isAdminStateLOCKED method
                 *  This method will check the admin state of job rdn
                 *  @param rdn   : complete rdn of the job Ex:"jobId=HC,HealthCheckhealthCheckMId=1"
                 *  @return bool : return 'true' if admin state is LOCKED else false
                 */
		bool isAdminStateLOCKED(string rdn);

		 /**
                 *  getProgressReportRunningState method
                 *  This method will check whether any job is in running state
                 *  @return int : return '2' if atleast one progress report is in 'RUNNING' state
                 */
		int getProgressReportRunningState();

		 /**
                 *  getProgressReportState method
                 *  This method will check the state of provided rdn
                 *  @param rdn   : complete rdn of the event Ex:"jobId=HC,HealthCheckhealthCheckMId=1"
                 *  @return int : return '2' if the progress report for the provided rdn is in 'RUNNING' state
                 */
		int getProgressReportState(string eventRDN);

		/**
                 *  removeTrailingSpaces method
                 *  This method will remove any spaces for the categories
                 *  @param job_categories   : categories list Ex:"DAILY  , SHORT"
                 *  @return string : return a string of categories after removing the spaces Ex:"DAILY,SHORT"
                 */
		string removeTrailingSpaces(string job_categories);

		/**
                 *  tokenizeCategories method
                 *  This method will tokenize the provided categories
                 *  @param category   : categories list Ex:"DAILY,SHORT"
		 *  @param categoryList : a set to hold the list of categories
                 *  @return void : no return value
                 */
		void tokenizeCategories(set<std::string> &categoryList, std::string category);

		/**
                 *  getTqDN method
                 *  This method will give the dn for the provided transfer queue
                 *  @param tq   : name of the transfer queue
                 *  @return string : return a string which is the value of dn
                 */
		string getTqDN(string tq);

		/**
                 *  getLocalTimeZone method
                 *  This method will give the local time zone in terms of GMT/UTC difference 
                 *  @return string : return a string of time zone in "(GMT+01:00) Europe/Rome" format
                 */
		string getLocalTimeZone();

		 /**
                 *  localZoneTime method
                 *  This method will give the date according to local time zone
                 *  @param date    : date for which local time is needed
                 *  @return string : return a string which is in "yyyy-mm-ddThh:mm:ss" format
                 */
		string localZoneTime(string date);

		/**
                 *  decreaseOneDay method
                 *  This method will decrease one day from the provided date
                 *  @param date    : date for which a day need to decrease
                 *  @return string : return a string which is in "yyyy-mm-ddThh:mm:ss" format after removing one day from it
                 */
		string decreaseOneDay(string date);

		 /**
                 *  getProgressReportIndex method
                 *  This method will give the index value of vector for a progress report
                 *  @param  parentObject  : name for which we need to find progress report
                 *  @return int : returns the index value 
                 */
		int getProgressReportIndex(string parentObject);

		/**
                 *  createJobScheduler method
                 *  This method will create the job scheduler object for the provided job
                 *  @param rdn     : RDN of the job for which job scheduler need to create Ex:"jobId=HC,HealthCheckhealthCheckMId=1"
                 *  @return void : no return value
                 */
		void createJobScheduler(string rdn);

		/**
		 * waitForSomeTime method
		 * This method will keep the thread to wait for the number of 'seconds' provided.
		 * @param seconds : Number of seconds to sleep.
		 * @return bool   : 'true' if thread successfully wait for provided number of seconds
		                    'false' if a call for graceful termination is received in between.
		 */
		bool waitForSomeTime(int seconds);

		 /**
                 * This method is used to validate the provided load/unload parameter
                 * @param parameter : argument provided for load function
                 * @param funcion   : READYTODB in case of load and INUSEDN in case of unload
                 * @return bool : TRUE if the provided parameter valid otherwise FALSE
                 */
                bool isParameterValid(string parameter, string dn);

		 /**
                 * This method is used to get the managed element id specific to the node
                 * @param    : no parameters needed
                 * @return string : value of managed element id
                 */
                string getManagedElementId();

		 /**
                 * This method is used to get all the attributes for a calendar event
	         * @param  rdn  : RDN of the event for which we need parameters.
		 * @param cal : It is structure, passed by reference. This structure woild be filled with all the parameters belong to that dn
                 * @return ACS_CC_ReturnType : ACS_CC_FAILURE or ACS_CC_SUCCESS
                 */
		ACS_CC_ReturnType getCalendarEventAttributes(string rdn,CALENDER_Parameters &cal);

		/**
                * This method is used to get all the categories with comma seperation that are available under HealthCheckM
                * @param    : no parameters needed.
                * @return string : all categories values with comma seperation.
                */
		string getAllCategories();

	private:

};

#endif

#ifndef BRFC_DEFINE_H_
#define BRFC_DEFINE_H_

#define BRM_SYSTEM_BACKUP_MANAGER__CREATE_BACKUP        0
#define BRM_SYSTEM_BACKUP_MANAGER__DELETE       	1
#define BRM_SYSTEM_BACKUP__RESTORE 			3
#define BRM_SYSTEM_BACKUP__CONFIRM_RESTORE 		4
#define BRM_SYSTEM_BACKUP__CANCEL_CURRENT_ACTION 	5

#define BRF_VERSION_PRA_

#ifndef BRF_VERSION_PRA_

#define BRM_PARTICIPANT__PERMIT_BACKUP 	6
#define BRM_PARTICIPANT__COMMIT_BACKUP 	7
#define BRM_PARTICIPANT__CANCEL_BACKUP 	8

#define BRM_PERSISTENT_DATA_OWNER__PREPARE_BACKUP 	9

#define BRM_PERSISTENT_STORAGE_OWNER__CREATE_BACKUP 	10
#define BRM_PERSISTENT_STORAGE_OWNER__PERMIT_DELETE 	11
#define BRM_PERSISTENT_STORAGE_OWNER__PREPARE_DELETE 	12
#define BRM_PERSISTENT_STORAGE_OWNER__COMMIT_DELETE 	13
#define BRM_PERSISTENT_STORAGE_OWNER__CANCEL_DELETE 	14
#define BRM_PERSISTENT_STORAGE_OWNER__PERMIT_RESTORE 	15
#define BRM_PERSISTENT_STORAGE_OWNER__PREPARE_RESTORE 	16
#define BRM_PERSISTENT_STORAGE_OWNER__COMMIT_RESTORE 	17
#define BRM_PERSISTENT_STORAGE_OWNER__CANCEL_RESTORE 	18

#define BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS 19
#define BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT   20

#else

#define BRM_PARTICIPANT__PERMIT_BACKUP 0
#define BRM_PARTICIPANT__COMMIT_BACKUP 3
#define BRM_PARTICIPANT__CANCEL_BACKUP 4

#define BRM_PERSISTENT_DATA_OWNER__PREPARE_BACKUP 1

#define BRM_PERSISTENT_STORAGE_OWNER__CREATE_BACKUP 	2
#define BRM_PERSISTENT_STORAGE_OWNER__PERMIT_DELETE 	5
#define BRM_PERSISTENT_STORAGE_OWNER__PREPARE_DELETE 	6
#define BRM_PERSISTENT_STORAGE_OWNER__COMMIT_DELETE 	7
#define BRM_PERSISTENT_STORAGE_OWNER__CANCEL_DELETE 	8
#define BRM_PERSISTENT_STORAGE_OWNER__PERMIT_RESTORE 	9
#define BRM_PERSISTENT_STORAGE_OWNER__PREPARE_RESTORE 	10
#define BRM_PERSISTENT_STORAGE_OWNER__COMMIT_RESTORE 	11
#define BRM_PERSISTENT_STORAGE_OWNER__CANCEL_RESTORE 	12

#define BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS 21
#define BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT   22

#endif

// Define status for backupType parameter

#define BRF_SUCCESS     	0
#define BRF_ERROR_DISKSPACE 	1
#define BRF_ERROR_CONFLICT      2
#define BRF_ERROR_OTHERS        99

#endif /* BRFC_OPERAIONID_H_ */

