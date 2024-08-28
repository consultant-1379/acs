/*=================================================================== */
/**
        @file           acs_aca_msd_service.h 

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the ACS_MSD_Service class.

        @version        1.0.0

        HISTORY
                                This section contains reference to problem report and related
                                software correction performed inside this module


        PR           DATE      INITIALS    DESCRIPTION
        -----------------------------------------------------------
        N/A       19/11/2012     XHARBAV   APG43 on Linux.

==================================================================== */

/*=====================================================================
                                DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACAMSD_SERVICE_H
#define ACAMSD_SERVICE_H

/*=====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
#include <iostream>

#include "ace/ACE.h"
#include "ace/Event_Handler.h"
#include "ace/Synch.h"
#include "ace/Task_T.h"
#include "ace/RW_Thread_Mutex.h"

#include "ACS_CS_API.h"
#include "acs_apgcc_omhandler.h"

#include "acs_aca_msd_ms_manager.h"

class ACS_ACA_ObjectImpl;

/*=====================================================================
                                CLASS DECLARATION SECTION
==================================================================== */
class ACS_MSD_Service : public ACE_Task<ACE_SYNCH> {
/*=====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
	static const unsigned long 	STOP_MS_MANAGERS_RETRY_NUM 		= 100;
	static const ACE_INT32 			STOP_MS_MANAGERS_TIMEOUT 			= 50; //50 milliseconds

	enum ParameterChangeErrorCode {
		NO_ERROR = 0,
		ERROR_TRY_AGAIN,
		ERROR_NOT_ACK_MSG,
		ERROR_OM_INIT_FAILED,
		ERROR_GET_ATTRBUTE_FAILED,
		ERROR_NOT_VALID_FOR_FILE_JOB,
		ERROR_NOT_VALID_FOR_BLOCK_JOB,
		ERROR_COMMIT_FILE_OPEN_FAILED,
		ERROR_MSNAME_CONFLICT_WITH_DATASOURCE
	};

/*===================================================================

        @brief          Constructor for ACS_MSD_Service class.

=================================================================== */
	ACS_MSD_Service ();

public:
/*=====================================================================
                                CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACS_MSD_Service () {}
/*=====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*===================================================================

        @brief          serviceName

        @return         const char *

=================================================================== */
	static inline const char * serviceName () { return "ACS_MSD_Service"; }
/*===================================================================

        @brief          serviceMain

        @return         void

=================================================================== */
	void  serviceMain ();
/*===================================================================

        @brief          setupACAThread

        @param          aACAPtr

        @return         ACS_CC_ReturnType

=================================================================== */
	static ACS_CC_ReturnType setupACAThread (ACS_MSD_Service * aACAPtr);
/*===================================================================

        @brief          ACAFunctionalMethod

        @param          aACAPtr

        @return         ACE_THR_FUNC_RETURN

=================================================================== */
	static ACE_THR_FUNC_RETURN ACAFunctionalMethod (void * aACAPtr);
/*===================================================================

        @brief          StartServer

        @return         void

=================================================================== */
	void StartServer ();
/*===================================================================

        @brief          setupIMMCallBacks

        @return         void

=================================================================== */
	void setupIMMCallBacks ();
/*===================================================================

        @brief          getAddress

        @param          indextoParam 

        @return         ACAMS_Parameters

=================================================================== */
	static ACAMS_Parameters * getAddress (short indextoParam);
/*===================================================================

        @brief          StopServer

        @return         void

=================================================================== */
	void StopServer ();
/*===================================================================

        @brief          StartOIThreadsOnActiveNode

        @return         ACS_CC_ReturnType

=================================================================== */
	ACS_CC_ReturnType StartOIThreadsOnActiveNode ();
/*===================================================================

        @brief          setupIMMCallBacksThreadFunc

        @param          aACAPtr

        @return         ACE_THR_FUNC_RETURN

=================================================================== */
	static ACE_THR_FUNC_RETURN setupIMMCallBacksThreadFunc (void * aACAPtr);
/*===================================================================

        @brief          stop

        @return         void

=================================================================== */
	void stop ();
/*===================================================================

        @brief          getMSManagerByMsgStore

        @param          msname

        @return         ACAMSD_MS_Manager

=================================================================== */
	static ACAMSD_MS_Manager * getMSManagerByMsgStore (const char * msname);
/*===================================================================

        @brief          getAllMSManagers
                         vector

=================================================================== */
	static std::vector<ACAMSD_MS_Manager *> getAllMSManagers ();
/*===================================================================

        @brief          printUsage

        @return         void

=================================================================== */
	static void printUsage ();

/*=====================================================================
                                PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*===================================================================

        @brief          fxFreeMessageStoreParameters

        @return         bool

=================================================================== */
	static bool fxFreeMessageStoreParameters ();
/*===================================================================

        @brief          fxFreeMessageStoreParameters

        @param          msParameters

        @param          size

        @return         bool

=================================================================== */
	static bool fxFreeMessageStoreParameters (ACAMS_Parameters ** msParameters, int size);
/*===================================================================

        @brief          fxCreateMSManagers

        @return         void

=================================================================== */
	static void fxCreateMSManagers ();
/*===================================================================

        @brief          fxStartMSManagers

        @return         void

=================================================================== */
	static void fxStartMSManagers ();
/*===================================================================

        @brief          fxDeleteMSManagers

        @return         void

=================================================================== */
	static void fxDeleteMSManagers ();
/*===================================================================

        @brief          fxCheckMSManagers

        @return         void

=================================================================== */
	static void fxCheckMSManagers ();
/*===================================================================

        @brief          fxStopMSManagers

        @param          retryNum

        @param          milliseconds

        @return         void

=================================================================== */
	static void fxStopMSManagers (
			unsigned long retryNum = STOP_MS_MANAGERS_RETRY_NUM,
			ACE_INT32 milliseconds = STOP_MS_MANAGERS_TIMEOUT);
/*===================================================================

        @brief          fxGetCpIds

        @return         int

=================================================================== */
	static int fxGetCpIds ();
/*===================================================================

        @brief          fxGetCpIds

        @param          retries

        @param          timeout

        @return         int

=================================================================== */
	static int fxGetCpIds (unsigned retries, unsigned timeout);
/*===================================================================

        @brief          fxCheckMultipleCP

        @return         int

=================================================================== */
	static int fxCheckMultipleCP ();
/*===================================================================

        @brief          fxCheckMultipleCP

        @param          retries

        @param          timeout  

        @return         int

=================================================================== */
	static int fxCheckMultipleCP (unsigned retries, unsigned timeout);
/*=====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*===================================================================

        @brief          fxGetDefaultCPName

        @param          id

        @param          dname

        @return         int

=================================================================== */
	static int fxGetDefaultCPName (short id, std::string & dname);
/*===================================================================

        @brief          getDefaultCPName_aca

        @param          cpId

        @param          defName

        @return         int

=================================================================== */
	static int getDefaultCPName_aca (unsigned short cpId, std::string & defName); //CR639
	
private:
	static ParameterChangeErrorCode check_change_applicable_for_cp (unsigned cp_id, char * ms_name);
#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
	static int checkDiskReintegrationOperationOngoing ();
	static int notify_os_caching_status(bool caching_status);
#endif

public:
	void stopIMMThread ();
	static int fetchDnOfRootObjFromIMM ();
	static int load_ms_parameters_from_IMM ();
	static int checkDataDisk(const unsigned long maxRetries, const ACE_INT32 timeoutMS);
	static int checkDataDisk();
	static int modify_ms_parameter(int ms_index, const char * param_to_modify, void * new_value);
	static void reset_name_changed_attribute(int ms_index);
	static void notify_cptable_change (unsigned cp_id, int op_type);
	static ParameterChangeErrorCode is_change_applicable (int ms_index);
	static ParameterChangeErrorCode is_record_size_valid (int ms_index, unsigned short new_record_size, bool & is_job_present);
#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
	static inline bool isOsCachingEnabled () { return _isOSCachingEnabled; }
	static int set_os_caching_status(bool status);
#endif

private:
	ACE_thread_t _root_worker_thread_id;
	ACE_thread_t _IMM_thread_id;
	ACS_ACA_ObjectImpl * m_poMsgStoreCommandHandler;

	static std::string parentObjDNofACA;
	static std::string dnObject;
	static ACE_RW_Thread_Mutex _ms_parameters_access_sync;
	static ACAMS_Parameters ** gParams; // to delete
	static bool _serviceMode;
	static int _messageStoreNum;
	static ACE_Event * _stopEvent; //to delete
	static bool _stopRequested;
	static ACAMSD_MS_Manager ** _msManagers; //to delete
	static std::vector<unsigned> _cpIds;
	static ACE_Recursive_Thread_Mutex _cpIds_sync;
	static bool _isMultipleCP;
#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
	static bool _isOSCachingEnabled;
#endif

};
#endif
