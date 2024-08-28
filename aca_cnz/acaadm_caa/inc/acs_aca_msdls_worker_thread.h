/*=================================================================== */
/**
        @file          acs_aca_msdls_worker_thread.h 

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the ACS_ACA_MSDLS_WorkerThread class.

        @version        1.0.0

        HISTORY
                                This section contains reference to problem report and related
                                software correction performed inside this module


        PR           DATE      INITIALS    DESCRIPTION
        -----------------------------------------------------------
        N/A       08/11/2012     XHARBAV   APG43 on Linux.

==================================================================== */

/*=====================================================================
                                DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_ACA_MSDLS_WORKER_THREAD_H
#define ACS_ACA_MSDLS_WORKER_THREAD_H
/*=====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <vector>

#include "ace/ACE.h"
#include "ace/Event.h"

#include "ACS_CC_Types.h"

#include "acs_aca_msd_ms_manager.h"
#include "acs_aca_msgfile.h"

/*=====================================================================
                                DEFINE DECLARATION SECTION
==================================================================== */
#define MAX_DSD_RECV_SIZE 65535
/*=====================================================================
                                CLASS DECLARATION 
==================================================================== */
class ACS_ACA_MSDLS_WorkerThread {
/*=====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                                CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================

        @brief          Constructor for ACS_ACA_MSDLS_WorkerThread class.

        @param          session

        @param          theStackSize 

=================================================================== */
	ACS_ACA_MSDLS_WorkerThread (ACS_DSD_Session * session, unsigned theStackSize = 0U);
/*=====================================================================
                                CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACS_ACA_MSDLS_WorkerThread ();
/*===================================================================

        @brief          stopMSDLSworker

        @return         void
==================================================================== */
	void stopMSDLSworker ();
/*===================================================================

        @brief          workerMain

        @return         int
==================================================================== */
	int workerMain ();
/*===================================================================

        @brief          findFirstFile

        @param          receiveDir 

        @return         int
==================================================================== */
	int findFirstFile (string receiveDir, vector<MsgFile>& dataFiles);
/*===================================================================

        @brief          processRequest

        @param          request

        @return         bool
==================================================================== */
	bool processRequest (const char * request /*, unsigned reqSize*/);
/*=====================================================================
                                PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*===================================================================

        @brief          process_0
    
        @return         bool

=================================================================== */
	//multiple CP elaborations
	bool process_0 (/*const char * request, unsigned reqSize*/);
/*===================================================================

        @brief          process_2

        @param          request

        @return         bool

=================================================================== */
	bool process_2 (const char * request /*, unsigned reqSize*/);
/*===================================================================

        @brief          process_4

        @param          request

        @return         bool

=================================================================== */
	bool process_4 (const char * request /*, unsigned reqSize*/);
/*===================================================================

        @brief          process_6

        @param          request

        @param          all

        @return         bool

=================================================================== */
	bool process_6 (const char * request ,/* unsigned reqSize,*/ bool all = false);
/*===================================================================

        @brief          process_14

        @param          request

        @return         bool

=================================================================== */
	bool process_14 (const char * request /*, unsigned reqSize*/);
/*===================================================================

        @brief          process_22

        @param          request

        @param          num

        @param          trunc

        @return         bool

=================================================================== */
	bool process_22 (const char * request , /* unsigned reqSize,*/ bool num, bool trunc);
/*===================================================================

        @brief          process_134

        @param          request

        @return         bool

=================================================================== */

	bool process_134 (const char * request /*, unsigned reqSize*/);
	//bool process_256(const char* request, const unsigned int reqSize);

	//single CP elaborations
/*===================================================================

        @brief          process_260

        @param          request

        @param          all

        @return         bool

=================================================================== */
	bool process_260 (const char * request , /* unsigned reqSize, */ bool all = false);
/*===================================================================

        @brief          process_388

        @param          request

        @return         bool

=================================================================== */
	bool process_388 (const char * request /*, unsigned reqSize*/);
/*===================================================================

        @brief          process_276

        @param          request
 
        @param          num

        @param          trunc

        @return         bool

=================================================================== */
	bool process_276 (const char * request, /* unsigned reqSize, */ bool num, bool trunc);
/*===================================================================

        @brief          loadDataFile

        @param          fileName

        @return         bool

=================================================================== */
	bool loadDataFile (const string & fileName);
/*===================================================================

        @brief          displayData

        @param          fileName

        @param          num

        @param          trunc

        @return         int

=================================================================== */
	int displayData (const string & fileName, bool num, bool trunc);
/*===================================================================

        @brief          closeDataFile

        @return         void 
=================================================================== */
	void closeDataFile ();
/*===================================================================

        @brief          getDay

        @param          wDayOfWeek

        @return         void
=================================================================== */
	string getDay (ACE_UINT16 wDayOfWeek);
/*===================================================================

        @brief          getMonth

        @param          wMonth

        @return         string
=================================================================== */
	string getMonth (ACE_UINT16 wMonth);
/*===================================================================

        @brief          getSingleCPDataDirectory

        @param          msname

        @param          cpsite  

        @return         string
=================================================================== */
	string getSingleCPDataDirectory (const char * msname, const char * cpsite);
/*===================================================================

        @brief          getSingleCPCommitFileName

        @param          msname

        @param          cpsite

        @return         string
=================================================================== */
	string getSingleCPCommitFileName (const char * msname, const char * cpsite);
/*===================================================================

        @brief          getSingleCPStatisticsFile

        @param          msname

        @param          cpsite

        @return         string
=================================================================== */
	string getSingleCPStatisticsFile (const char * msname, const char * cpsite);
/*===================================================================

        @brief          sendResponse

        @param          data

        @param          cpID

        @return         bool
=================================================================== */
	bool sendResponse (const char * data, unsigned short cpID = 0);
/*===================================================================

        @brief          sendEnd

        @return         bool
=================================================================== */
	bool sendEnd ();
/*===================================================================

        @brief          sendInvalidError

        @param          reason 

        @return         bool
=================================================================== */
	bool sendInvalidError (const char * reason = 0);
/*===================================================================

        @brief          sendCSError_55

        @return         bool
=================================================================== */
	bool sendCSError_55 ();
/*===================================================================

        @brief          sendCSError_56

        @return         bool
=================================================================== */
	bool sendCSError_56 ();
/*===================================================================

        @brief          sendError

        @param          code

        @param          reason   

        @return         bool
=================================================================== */
	bool sendError (unsigned int code, const char * reason = 0);
/*===================================================================

        @brief          isMultipleCPSystem

        @return         bool
=================================================================== */
	bool isMultipleCPSystem ();
/*=====================================================================
                                DATA MEMBERS
==================================================================== */
public:
	thread_state_t _state;
	ACE_thread_t _tid;

private:
	ACE_HANDLE dfile_hand;
	ACE_HANDLE MapDataFile;
	void * file_view;
	size_t _file_map_size;
	ULONGLONG SizeDataFile;

	ACS_DSD_Session * _session;
	ACE_HANDLE connectedH;
	unsigned int seqNo;
};

#endif
