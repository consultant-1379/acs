/*=================================================================== */
/**
     i  @file          acs_aca_msd_msg_store.h 

        @brief          Header file for ACA module.

                                This module contains all the declarations useful to
                                specify the ACAMSD_MsgStore class.

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
#ifndef ACAMSD_MsgStore_H
#define ACAMSD_MsgStore_H
/*=====================================================================
                                INCLUDE DECLARATION SECTION
==================================================================== */
#include <vector>
#include <string>

#include "ace/ACE.h"
#include "ace/Event.h"

#include "acs_aca_ms_parameters.h"
#include "acs_aca_defs.h"
#include "acs_aca_common.h"
#include "acs_aca_ms_msg_store.h"

/*=====================================================================
                                FOEWARD DECLARATION SECTION
==================================================================== */
class ACAMS_EventHandler;
class ACAMS_StatisticsFile;
class ACAMSD_MTAP_ChannelManager;
class ACAMSD_MsgCollection;

/*=====================================================================
                                CLASS DECLARATION SECTION
==================================================================== */
class ACAMSD_MsgStore : public ACAMS_MsgStore {
/*=====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                                ENUMERATION DECLARATION SECTION
==================================================================== */
/*===================================================================

   @brief  MsgEcode

=================================================================== */
	enum MsgEcode {
		NoError = 0,
		AccessFileDenied = 1,
		DiskSpaceExhaust = 6,
		MessageStoreFull = 7,
		ErrorNotReady = 15
	};
/*=====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*===================================================================

   @brief  INVALID_CPSYSTEMID

=================================================================== */
	static const unsigned INVALID_CPSYSTEMID = ~0;
/*===================================================================

   @brief  indextoParam

=================================================================== */
	const short indextoParam;
/*=====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                                CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================

        @brief          Constructor for ACAMSD_MsgStore class.

        @param          index

=================================================================== */
	ACAMSD_MsgStore (const short index);
/*===================================================================

        @brief          Constructor for ACAMSD_MsgStore class.

        @param          cpSystemId 

        @param          index

=================================================================== */
	ACAMSD_MsgStore (unsigned cpSystemId, short index);
/*===================================================================

        @brief          Cinitialize_post_ctor

        @param          msgCollection

=================================================================== */
	int initialize_post_ctor (ACAMSD_MsgCollection * & msgCollection);

/*=====================================================================
                                PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                                CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACAMSD_MsgStore ();

#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
/*===================================================================


		@brief          setCachingStrategyForDataFile
						Set caching strategy to be used for writing messages in DataFiles

    	@param          cache_on
    					true 	--> enable caching
    					false 	--> disable caching

		@return         int

=================================================================== */
	int setCachingStrategyForDataFiles(bool cache_on);

#endif


/*===================================================================


        @brief          getNextMsgNumber
                         Get next Message Number to use
 
        @return         ULONGLONG

=================================================================== */
	ULONGLONG getNextMsgNumber () const;
/*===================================================================

        @brief          createDataFile
                         Create and open a new DataFile in the MsgStore, and create a Collection
                         for Messages. Return value false means that the MsgStore is full.

        @param          mtapFileNum

        @param          msgCollection

        @return         MsgEcode

=================================================================== */
	MsgEcode createDataFile (unsigned char mtapFileNum, ACAMSD_MsgCollection * & msgCollection);
/*===================================================================

        @brief          closeDataFile
                         Close the current DataFile (whereby the Collection created by
                         'createDataFile' becomes invalid).

        @return          bool
=================================================================== */
	bool closeDataFile ();
/*===================================================================

        @brief          deleteDataFile
                         Delete DataFile (and any DataFile before it). Returns number of files
                         deleted.

        @param          fileName

        @return          int
=================================================================== */
	int deleteDataFile (const std::string & fileName);
/*===================================================================

        @brief          store
                         Store the Messages in the MsgCollection to disk and acknowledge them
                         through owner. Return number of Msg stored.

        @param          owner

        @param          numberOfMsg

        @return         bool 
=================================================================== */
	bool store (ACAMSD_MTAP_ChannelManager * owner, int & numberOfMsg);
/*===================================================================

        @brief          storeExpRecord

        @param          outData

        @param          outSize

=================================================================== */
	ACAMSD_MsgStore::MsgEcode storeExpRecord (unsigned char * outData, unsigned int outSize);
/*===================================================================

        @brief          putVersion
                         store MTAP protocol version on disc

        @param          v

        @return         bool
=================================================================== */
	bool putVersion (unsigned char v);
/*===================================================================

        @brief          getVersion
                         reads MTAP protocol version from disc

        @return         unsigned char
=================================================================== */
	unsigned char getVersion ();
/*===================================================================

        @brief          getStatisticsData

        @return         ACAMS_StatisticsFile *
=================================================================== */
	inline ACAMS_StatisticsFile * getStatisticsData () { return myStatisticsData; }

/*=====================================================================
                                PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=====================================================================
                                CLASS CONSTRUCTOR
==================================================================== */
/*===================================================================

        @brief          Constructor for ACAMSD_MsgStore class.

=================================================================== */
	ACAMSD_MsgStore ();
/*===================================================================

        @brief          Constructor for ACAMSD_MsgStore class.

        @param          anotherMS

=================================================================== */
	ACAMSD_MsgStore (const ACAMSD_MsgStore & anotherMS);

	const ACAMSD_MsgStore & operator= (const ACAMSD_MsgStore & anotherMS);
/*===================================================================

        @brief          assignFileName

        @param          firstMsgNum

        @param          mtapFileNum

        @return         string

=================================================================== */
	std::string assignFileName (ULONGLONG firstMsgNum, const unsigned char mtapFileNum) const;
/*===================================================================

        @brief          checkAlarmStatus
                         Raise or cease alarm 'Message Store Warning'

        @param          forcedCease

        @return         void

=================================================================== */
	void checkAlarmStatus (bool forcedCease = false);
/*===================================================================

        @brief          raise_msg_full_alarm

        @return         int

=================================================================== */
	int raise_msg_full_alarm ();

/*===================================================================

        @brief          cease_msg_full_alarm

        @return         int

=================================================================== */
	int cease_msg_full_alarm ();
/*===================================================================

        @brief          gotoEnd
                         Locate the end of file and return next MsgNumber to write

        @param          ACE_HANDLE fhand

        @param          fileName

        @param          nextMsgNum

        @param          fileFull  

        @return         bool

=================================================================== */
	bool gotoEnd (ACE_HANDLE fhand, const std::string & fileName, unsigned long long & nextMsgNum, bool & fileFull);
/*===================================================================

        @brief          endOfMsg
                         checks whether an End-of-Message flag is next in file.

        @param          fhand

        @return         bool

=================================================================== */
	bool endOfMsg (ACE_HANDLE fhand);
/*==================================================================
 *
		@brief			writeEndOfFileMarker()
						 Write End-Of-File mark to the current open disk-file (myCurrentFile).
						 Return value false indicates success.

		@return			bool
====================================================================*/
	bool writeEndOfFileMarker();
/*===================================================================

        @brief          setup
                         Change current directory to this message store (create directorys if they do
                         not already exist. Return value false indicates failure, and 'path' will
                         then point out the directory that could not be created.

        @param          path

        @return         bool

=================================================================== */   
	bool setup (std::string & path);
/*=====================================================================
                                DATA MEMBER DECLARATION SECTION
==================================================================== */
	bool myAlarmStatus;     // true if alarm raised
	ACE_HANDLE myCurrentFile;     // Handle to currently open data file
	std::string myCurrentFileName; // name of the currently open data file
	ULONGLONG myNextMsgNum; //ACS_ACA_MessageNumber myNextMsgNum;
	ACAMSD_MsgCollection * myMsgColl;
	bool myDiskSpaceExhausted;
	int myNumberOfFiles;   // Current number of entries in 'myFiles'
	std::string myFiles[2048]; // All dataFiles
	ACAMS_StatisticsFile * myStatisticsData;
	ACE_HANDLE version_fptr;     // Handle to MTAP protocol version file
	std::vector<std::string> failedOldFiles;  // files that wasn't deleted because of the virus scanner.
	unsigned _cpSystemId;
#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
	int _open_flags;
#endif
};
#endif
