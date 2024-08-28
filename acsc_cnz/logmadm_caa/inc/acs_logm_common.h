/*=================================================================== */
/**
	@file		acs_logm_common.h

	@brief		Header file for LOGM Common  modules.

				Common class having common functionalities is
				present in this file

	@version 	1.0.0

 **/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       2010-09-23     SJ       Initial Release
N/A       2012-03-22     GB       Initial Release
N/A       17/01/2013    XCSRPAD   Deletion of unwanted CMX log files
N/A       10/3/2016     XCSRPAD   Deletion of unwanted SMX log files
==================================================================== */
//***********************************************************************

/*=====================================================================
					DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_LOGM_COMMON_H_
#define ACS_LOGM_COMMON_H_

/*====================================================================
					INCLUDE DECLARATION SECTION
==================================================================== */
#include <map>
#include <unistd.h>
#include <syslog.h>
#include <ace/ACE.h>
#include <ace/Task.h>
//#include <ace/Task.h>
#include <ace/OS_NS_poll.h>
#include <ace/Thread_Manager.h>
#include <ACS_APGCC_Util.H>
#include <ACS_APGCC_ApplicationManager.h>
#include <ACS_TRA_trace.h>
#include <ACS_TRA_Logging.h>
#include <acs_logm_aeh.h>
#include <acs_logm_svc_loader.h>


using namespace std;

#define 	LOGM_ROOT_CLASS_NAME	"LogMaintenance"
#define 	LOGM_ROOT_CLASS_OBJ_DN	"logMaintenanceId=1"
//#define		LOGM_DDIRM_OBJ_DN	"logMaintenanceConfigurationId=ddiRm"
#define		LOGM_TRALOG_OBJ_DN	"logMaintenanceConfigurationId=TRALOG"
//#define		LOGM_DATASTSIN_OBJ_DN	"logMaintenanceConfigurationId=DATASTSIn"
//#define		LOGM_DATASTSAG_OBJ_DN	"logMaintenanceConfigurationId=DATASTSag"
#define	 	LOGM_OUTPUTSTSAG_OBJ_DN	"logMaintenanceConfigurationId=OUTPUTSTSag"
#define		LOGM_OUTPUTSTSIN_OBJ_DN	"logMaintenanceConfigurationId=OUTPUTSTSIn"
#define		LOGM_SCXLG_OBJ_DN		"logMaintenanceConfigurationId=SCXlg"
#define		LOGM_CMXLG_OBJ_DN		"logMaintenanceConfigurationId=CMXlg"
#define		LOGM_SMXLG_OBJ_DN		"logMaintenanceConfigurationId=SMXlg"
#define		LOGM_SERVICE_PARAM_IMPL_NAME	"LogmSrvParamImpl"
#define		LOGM_LOG_PARAM_IMPL_NAME	"LogmLogParamImpl"
#define		LOGM_LOG_PARAM_CLASS_NAME	"LogMaintenanceConfiguration"

/*=====================================================================
					CLASS DECLARATION SECTION
=======================================================================*/

/*=====================================================================*/
/**
	@brief		ACS_LOGM_AMFService

				This class implements the application manager related activities
 */
/*======================================================================= */
class ACS_LOGM_AMFService : public ACS_APGCC_ApplicationManager
{
	/*====================================================================
				   PRIVATE DECLARATION SECTION
	====================================================================== */
private:
	/*====================================================================*/
	/**

		@brief		readWritePipe


	 */
	/*==================================================================== */
	int readWritePipe[2];
	/*====================================================================*/
	/**

		@brief		Is_terminated


	 */
	/*==================================================================== */
	ACS_APGCC_BOOL Is_terminated;
	/*====================================================================*/
	/**

		@brief		LOGMServiceThreadId


	 */
	/*==================================================================== */
//	ACE_thread_t LOGMServiceThreadId ;

	LOGM_Service_Loader *m_logmApplObj;

	ACS_APGCC_ReturnType activateApp();
	ACS_APGCC_ReturnType passifyApp();

	/*====================================================================
				   PUBLIC DECLARATION SECTION
	====================================================================== */
public:
	/*=====================================================================
				CLASS CONSTRUCTORS
	==================================================================== */
	/*====================================================================*/
	/**

		@brief		ACS_LOGM_AMFService
					Constructor for ACS_LOGM_AMFService class.

		@par 		daemon_name
					Name of Deamon

		@par 		username
	 */
	/*=================================================================== */
	ACS_LOGM_AMFService(const char* daemon_name, const char* username);
	/*=================================================================== */
	/**

		@brief		performStateTransitionToActiveJobs


		@param		previousHAState

		@return		ACS_APGCC_ReturnType
	 */
	/*=================================================================== */
	ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	/*=================================================================== */

	/**
                @brief          performStateTransitionToPassiveJobs

                @param          previousHAState

                @return         ACS_APGCC_ReturnType

                @exception      none
	 */
	/*=================================================================== */
	//ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	/*=================================================================== */

	/**

		@brief		performStateTransitionToQueisingJobs


		@param		previousHAState

		@return		ACS_APGCC_ReturnType
	 */
	/*=================================================================== */
	ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	/*=================================================================== */
	/**

		@brief		performStateTransitionToQuiescedJobs


		@param		previousHAState

		@return		ACS_APGCC_ReturnType
	 */
	/*=================================================================== */
	ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState);
	/*=================================================================== */
	/**

		@brief		performComponentHealthCheck

		@return		ACS_APGCC_ReturnType
	 */
	/*=================================================================== */
	ACS_APGCC_ReturnType performComponentHealthCheck(void);
	/*=================================================================== */
	/**

		@brief		performComponentTerminateJobs

		@return		ACS_APGCC_ReturnType
	 */
	/*=================================================================== */
	ACS_APGCC_ReturnType performComponentTerminateJobs(void);
	/*=================================================================== */
	/**

		@brief		performComponentRemoveJobs

		@return		ACS_APGCC_ReturnType
	 */
	/*=================================================================== */
	ACS_APGCC_ReturnType performComponentRemoveJobs (void);
	/*=================================================================== */
	/**

		@brief		performApplicationShutdownJobs

		@return		ACS_APGCC_ReturnType
	 */
	/*=================================================================== */
	ACS_APGCC_ReturnType performApplicationShutdownJobs(void);
	/*=================================================================== */
	/**

		@brief		logm_svc

		@return		ACS_APGCC_ReturnType
	 */
	/*=================================================================== */
//	ACS_APGCC_ReturnType logm_svc(void);
	/*=================================================================== */


	~ACS_LOGM_AMFService();
};


/**
 *      @brief          ThrExitHandler
 *                      Class for handling thread exit.
 *
 */
#if 0
class ThrExitHandler : public ACE_At_Thread_Exit
{
public:
	static bool init()
	{
		ThrExitHandler *thrExitHandler = new ThrExitHandler();
		if( thrExitHandler == 0 )
		{
			return false;
		}
		thrExitHandler->setThrStatusInMap(ACE_Thread_Manager::instance()->thr_self(), true);

		int result = ACE_Thread_Manager::instance()->at_exit( thrExitHandler );

		if( result == -1 )
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	static void cleanup()
	{
		theThrMutex.remove();
		theThrStatusMap.clear();
	}

	static bool CheckAllFuncThreads( void )
	{
		//Lock the mutex.
		theThrMutex.acquire();

		if( theThrStatusMap.empty())
		{
			theThrMutex.release();
			return false;
		}


		//Check if any thread has exited or not.
		for( map<ACE_thread_t, bool>::iterator itr = theThrStatusMap.begin();
				itr != theThrStatusMap.end();  ++itr )
		{
			if( itr->second == false )
			{
				theThrMutex.release();
				return false;
			}
		}
		theThrMutex.release();
		return true;
	}



	virtual void apply(void)
	{
		if( !theThrStatusMap.empty())
		{
			setThrStatusInMap( ACE_Thread_Manager::instance()->thr_self(), false);
		}
	}
private:

	void setThrStatusInMap( ACE_thread_t thrId, bool isThrAlive)
	{
		bool isThrPresentInMap = false;
		theThrMutex.acquire();

		//Now update the data in the global map.
		map<ACE_thread_t, bool>::iterator itr =  theThrStatusMap.begin();

		while( itr != theThrStatusMap.end() )
		{
			if( itr->first == thrId )
			{
				isThrPresentInMap = true;
				break;
			}
			++itr;
		}

		if( isThrPresentInMap )
		{
			theThrStatusMap.erase(thrId);
		}
		theThrStatusMap.insert(std::pair<ACE_thread_t, bool>(thrId, isThrAlive));
		theThrMutex.release();
	}

	static std::map<ACE_thread_t, bool> theThrStatusMap;
	static ACE_Recursive_Thread_Mutex  theThrMutex;
};
#endif

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*====================================================================*/
/**
 *	@brief		LOGM_SERVICE_VER
 */
/*====================================================================*/
#define LOGM_SERVICE_VER 	"1.0"
/*====================================================================*/
/**
 *	@brief		DEBUG_PREFIX
 */
/*====================================================================*/
#define DEBUG_PREFIX 		"\n DEBUG%I"
/*====================================================================*/
/**
 *	@brief		INFO_PREFIX
 */
/*====================================================================*/
#define INFO_PREFIX 		"\n INFO%I"
/*====================================================================*/
/**
 *	@brief		NOTICE_PREFIX
 */
/*====================================================================*/
#define NOTICE_PREFIX 		"\n NOTICE%I"
/*====================================================================*/
/**
 *	@brief		WARNING_PREFIX
 */
/*====================================================================*/
#define WARNING_PREFIX 		"\n WARNING%I"
/*====================================================================*/
/**
 *	@brief		ERROR_PREFIX
 */
/*====================================================================*/
#define ERROR_PREFIX 		"\n ERROR%I"
/*====================================================================*/
/**
 *	@brief		CRITICAL_PREFIX
 */
/*====================================================================*/
#define CRITICAL_PREFIX 	"\n CRITICAL%I"
/*====================================================================*/
/**
 *	@brief		ALERT_PREFIX
 */
/*====================================================================*/
#define ALERT_PREFIX 		"\n ALERT%I"
/*====================================================================*/
/**
 *	@brief		EMERGENCY_PREFIX
 */
/*====================================================================*/
#define EMERGENCY_PREFIX 	"\n EMERGENCY%I"


#ifdef LOGMDEBUG
#define DEBUG(FMT, ...) \
		ACE_DEBUG(( LM_DEBUG, \
				DEBUG_PREFIX FMT ,\
				__VA_ARGS__))

#define WARNING(FMT, ...) \
		ACS_LOGM_Common::helperTraceWarning(FMT,__VA_ARGS__);
#define TRA_ERROR(FMT, ...) \
		ACS_LOGM_Common::helperTraceWarning(FMT,__VA_ARGS__);

#define INFO(FMT, ...) \
		ACE_DEBUG(( LM_INFO, \
				INFO_PREFIX FMT ,\
				__VA_ARGS__))

#define NOTICE(FMT, ...) \
		ACE_DEBUG(( LM_NOTICE, \
				NOTICE_PREFIX FMT ,\
				__VA_ARGS__))

#define ERROR(FMT, ...) \
		ACE_DEBUG(( LM_ERROR, \
				ERROR_PREFIX FMT ,\
				__VA_ARGS__))

#define CRITICAL(FMT, ...) \
		ACE_DEBUG(( LM_CRITICAL, \
				CRITICAL_PREFIX FMT ,\
				__VA_ARGS__))

#define ALERT(FMT, ...) \
		ACE_DEBUG(( LM_ALERT, \
				ALERT_PREFIX FMT ,\
				__VA_ARGS__))

#define EMERGENCY(FMT, ...) \
		ACE_DEBUG(( LM_EMERGENCY, \
				EMERGENCY_PREFIX FMT ,\
				__VA_ARGS__))

#else  // LOGMDEBUG
/*====================================================================*/
/**
 *	@brief		DEBUG
 */
/*====================================================================*/
#define DEBUG(FMT, ...) \
		ACS_LOGM_Common::helperTraceInformation(FMT,__VA_ARGS__);
/*====================================================================*/
/**
 *	@brief		WARNING
 */
/*====================================================================*/
#define WARNING(FMT, ...) \
		ACS_LOGM_Common::helperTraceWarning(FMT,__VA_ARGS__);
/*====================================================================*/
/**
 *	@brief		ERROR
 */
/*====================================================================*/
#define ERROR(FMT, ...) \
		ACS_LOGM_Common::helperTraceError(FMT,__VA_ARGS__);
/*====================================================================*/
/**
 *	@brief		INFO
 */
/*====================================================================*/
#define INFO(FMT, ...) \
		ACS_LOGM_Common::helperTraceInformation(FMT,__VA_ARGS__);

#endif
/*====================================================================*/
/**
 *	@brief		_enumNodeState
 *
 */
/*====================================================================*/
typedef enum _enumNodeState
{
	nsActive = 0,
			nsPassive,
			nsUnknown
} enumNodeState;



/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/*===================================================================*/
/**
	@brief		ACS_LOGM_Common

				It contains all the common functions.
 */
/*=================================================================== */


class ACS_LOGM_Common
{

	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
	                        VARIABLE DECLARATION SECTION
	==================================================================== */
	/*====================================================================*/
	/**
	 *	@brief		ACS_LOGM_InformationTrace
	 */
	/*====================================================================*/
	static ACS_TRA_trace 	*ACS_LOGM_InformationTrace;
	/*====================================================================*/
	/**
	 *	@brief		ACS_LOGM_InformationTrace
	 */
	/*====================================================================*/
	static ACS_TRA_trace 	*ACS_LOGM_WarningTrace;
	/*====================================================================*/
	/**
	 *	@brief		ACS_LOGM_InformationTrace
	 */
	/*====================================================================*/
	static ACS_TRA_trace 	*ACS_LOGM_ErrorTrace;
	/*====================================================================*/
	/**
	 *	@brief		ACS_LOGM_InformationTrace
	 */
	/*====================================================================*/
	static ACS_TRA_Logging 	*ACS_LOGM_Logging;
	/*======================================================================*/

	/**
	 *	@brief		fetchDnOfRootObjFromIMM
	 */
	/*======================================================================*/
	static int fetchDnOfRootObjFromIMM ();
	/*======================================================================*/
	/**
	 *	@brief		parentObjDNofLOGM
	 */
	/*======================================================================*/
	static std::string parentObjDNofLOGM;
	/*=====================================================================
	                        PROTECTED DECLARATION SECTION
	==================================================================== */
protected:

	/*=====================================================================
	                        CLASS CONSTRUCTORS
	==================================================================== */
	/*====================================================================*/
	/**

		@brief		ACS_LOGM_Common
					Constructor for ACS_LOGM_Common class.

	 */
	/*=================================================================== */
	ACS_LOGM_Common() { };

	/*=====================================================================
	                        CLASS DESTRUCTOR
	==================================================================== */
	/*====================================================================*/
	/**

   		@brief		Destructor for ACS_ACSC_Logmaint class.
					Protected default destructor
					Not allowed to instantiate an object of this class

	 */
	/*=================================================================== */
	~ ACS_LOGM_Common() {
		if( ACS_LOGM_InformationTrace != 0)
		{
			delete ACS_LOGM_InformationTrace;
			ACS_LOGM_InformationTrace = 0;
		}
		if( ACS_LOGM_WarningTrace != 0)
		{
			delete ACS_LOGM_WarningTrace;
			ACS_LOGM_WarningTrace = 0;
		}
		if( ACS_LOGM_ErrorTrace != 0)
		{
			delete ACS_LOGM_ErrorTrace;
			ACS_LOGM_ErrorTrace = 0;
		}
		if( ACS_LOGM_Logging != 0)
		{
			delete ACS_LOGM_Logging;
			ACS_LOGM_Logging = 0;
		}
	};

	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*===================================================================
		   ROUTINE: helperTraceInformation
	=================================================================== */
	/*=================================================================== */
	/**
	 *	@brief			helperTraceInformation
						static function used to pass the TRA information trace.
	 *
	 * 	@param			messageFormat
	 *					Format of the message
	 *
	 *	@return			void
	 */
	/*=================================================================== */
	static void helperTraceInformation(const ACE_TCHAR* messageFormat, ...)
	{
		if( ACS_LOGM_InformationTrace==0 )
			return;
		const ACE_UINT32 TRACE_BUF_SIZE = 1024;
		ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
		memset( traceBuffer, 0, sizeof( traceBuffer ));

		if ( messageFormat && *messageFormat )
		{
			va_list params;
			va_start(params, messageFormat);
			ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
			traceBuffer[TRACE_BUF_SIZE-1]='\0';

			//! If Trace if ON .Send trace to TRA
			if( ACS_LOGM_InformationTrace->ACS_TRA_ON() )
			{
				ACS_LOGM_InformationTrace->ACS_TRA_event(1,traceBuffer);
			}

			ACS_LOGM_Logging->ACS_TRA_Log(traceBuffer,LOG_LEVEL_INFO);
			va_end(params);
		}
	}//End of helperTraceInformation


	/*===================================================================
     	   ROUTINE: helperTraceWarning
  	=================================================================== */
	/*=================================================================== */
	/**
	 *	@brief			static function used to pass the TRA warning trace.
	 *
	 * 	@param			messageFormat	
	 *				Format of the message
	 *
	 *	@return			void
	 */
	/*=================================================================== */
	static void helperTraceWarning(const ACE_TCHAR* messageFormat, ...)
	{
		if( ACS_LOGM_WarningTrace==0 )
			return;

		static const ACE_UINT32 TRACE_BUF_SIZE = 1024;
		ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
		memset( traceBuffer, 0, sizeof( traceBuffer ));

		if ( messageFormat && *messageFormat )
		{
			va_list params;
			va_start(params, messageFormat);

			ACE_OS::vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1 , messageFormat, params);
			//! If Trace if ON .Send trace to TRA

			if( ACS_LOGM_WarningTrace->ACS_TRA_ON() )
			{
				ACS_LOGM_WarningTrace->ACS_TRA_event(1,traceBuffer);
			}
			ACS_LOGM_Logging->ACS_TRA_Log(traceBuffer,LOG_LEVEL_WARN);
			va_end(params);
		}
	}//End of helperTraceWarning


	/*===================================================================
     	   ROUTINE: helperTraceError
  	=================================================================== */
	/*=================================================================== */
	/**
	 *	@brief			static function used to pass the TRA Error trace.
	 *
	 * 	@param			messageFormat	
	 *				Format of the message
	 *
	 *	@return			void
	 */
	/*=================================================================== */
	static void helperTraceError(const ACE_TCHAR* messageFormat, ...)
	{
		if( ACS_LOGM_ErrorTrace ==0 )
			return;

		static const ACE_UINT32 TRACE_BUF_SIZE = 1024;
		ACE_TCHAR traceBuffer[TRACE_BUF_SIZE];
		memset( traceBuffer, 0, sizeof( traceBuffer ));
		if ( messageFormat && *messageFormat )
		{
			va_list params;
			va_start(params, messageFormat);

			ACE_OS::vsnprintf(traceBuffer , TRACE_BUF_SIZE - 1 , messageFormat, params);

			//! If Trace if ON .Send trace to TRA

			if( ACS_LOGM_ErrorTrace->ACS_TRA_ON() )
			{
				ACS_LOGM_ErrorTrace->ACS_TRA_event(1,traceBuffer);
			}
			ACS_LOGM_Logging->ACS_TRA_Log(traceBuffer,LOG_LEVEL_ERROR);

			va_end(params);
		}
	}//End of helperTraceError


	/*===================================================================
     	   ROUTINE: s32Spawn
  	=================================================================== */
	/*=================================================================== */
	/**
	 *	@brief			Function used to spawn thread.
	 *
	 * 	@param			ThreadFunc

	 * 	@param			Arglist

	 * 	@param			lpszThreadName

	 *	@param              	s32GrpId
	 *
	 *	@return			ACE_INT32
	 */
	/*=================================================================== */
	static ACE_INT32 s32Spawn(ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),void * Arglist,const
			ACE_TCHAR* lpszThreadName,ACE_INT32 s32GrpId)
	{
		ACE_INT32 s32Result =  ACE_Thread_Manager::instance()->spawn( ThreadFunc,
				Arglist,
				THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
				0,
				0,
				ACE_DEFAULT_THREAD_PRIORITY,
				s32GrpId,
				0,
				ACE_DEFAULT_THREAD_STACKSIZE,
				&lpszThreadName);

		return s32Result;
	}//End of s32Spawn

	/*===================================================================
     	   ROUTINE: StartThread
  	=================================================================== */
	/*=================================================================== */
	/**
	 *	@brief			Function used to start thread.
	 *
	 * 	@param			ThreadFunc

	 * 	@param			Arglist

	 * 	@param			lpszThreadName

	 *	@param                	s32GrpId

	 *	@return			ACE_INT32
	 */
	/*=================================================================== */
	static ACE_INT32 StartThread( ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),
			void* Arglist,
			const ACE_TCHAR* lpszThreadName,
			ACE_INT32 s32GrpId = -1)
	{

		ACE_INT32 s32Result =  s32Spawn( ThreadFunc,
				Arglist,
				lpszThreadName,
				s32GrpId);

		if ( s32Result == -1 )
		{
			// Report error
			ACE_TCHAR szData[256];

			snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
					ACE_TEXT("Unable to create thread: %s due to the following error:\n %m"),
					lpszThreadName);

			ACE_DEBUG((LM_INFO, ACE_TEXT(szData)));

			//! Send event
			(void)ACS_LOGM_AEH::ReportEvent(1002,
					ACE_TEXT("EVENT"),
					CAUSE_AP_INTERNAL_FAULT,
					ACE_TEXT(""),
					szData,
					ACE_TEXT("CREATE THREAD FAILED IN LOGM"));
		}

		return s32Result;

	}//End of StartThread

	/*===================================================================
				COMPONENT DECLARATION SECTION
	=================================================================== */
	/*====================================================================*/
	/**

   		@brief		GrpId


	 */
	/*=================================================================== */
	static ACE_INT32 GrpId;


};

#endif /* APG_APP1_CLASS_H */


