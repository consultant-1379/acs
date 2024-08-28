/*=================================================================== */
/**
   @file  acs_rtr_statistics.h 

   @brief Header file for rtr module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       23/01/2013   XHARBAV   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_RTR_statistics_h
#define ACS_RTR_statistics_h
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>


#include <ace/Recursive_Thread_Mutex.h>
#include "ace/ACE.h"

/*=====================================================================
                          FORWARD DECLARATION SECTION
==================================================================== */
struct CpStatisticsInfo;
struct StatisticsInfo;

/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */

class RTR_statistics {

 public:

	/**
	   @brief     Constructor for RTR_statistics class.
	*/
	RTR_statistics(const std::string& messageStoreName);

	/**
   	   @brief     Constructor for RTR_statistics class.
   	   @param     string
	*/
	RTR_statistics(short cpId, const std::string& messageStoreName);

	/**
   	   @brief     Destructor for RTR_statistics class.
	*/
    virtual ~RTR_statistics();

    /**
	   @brief     statisticsInfoInit.
	   This method initialize the message store statistic object
	*/
	bool statisticsInfoInit(const std::string& jobDn);

	/**
	   @brief     cpStatisticsInfoInit.
	   This method initialize the message store statistic object for a CP/BC element
	*/
	bool cpStatisticsInfoInit(const std::string& jobDn);

	/**
	   @brief     getCpStatisticsInfo.
	   This method gets statistics info for a CP/BC element
	*/
	void getCpStatisticsInfo(CpStatisticsInfo* cpStatisticsInfo);

	/**
	   @brief     getMessageStoreStatistics.
	   This method gets statistics info for a message store
	*/
	void getMessageStoreStatistics(StatisticsInfo* msStatisticsInfo);

	/**
   	   @brief     incFilled
	*/
	void incFilled();

	/**
   	   @brief     incLost
    */
	void incLost();

	/**
		@brief     incSkipped
	*/
	void incSkipped();

	/**
	   @brief     incConnMSattempts
	 */
	void incConnMSattempts();

	/**
   	   @brief     incAttachGOHattempts
	*/
	void incAttachGOHattempts();

	/**
   	   @brief     addReadMSdata
   	   @param     MSbytes
	 */
	void addReadMSdata(unsigned int MSbytes);

	/**
		@brief     addReportedData
		@param     GOHbytes
	*/
	void addReportedData(unsigned int GOHbytes);

	/**
	   @brief     getFormattedBuffer
	   @param     formBuf
	   @param     bufSize
	   @param     typeOfTransfer
	 */
	void getFormattedBuffer(char* formBuf, size_t bufSize, int typeOfTransfer); 


	/**
   	   @brief     clearBLD
  	  @return    void
	*/
	void clearBLD();

	/**
   	   @brief     delStatFile
	*/
	int delStatFile();

	/**
   	   @brief     statParFormat
   	   @param     str
   	   @param     size
   	   @param     len
   	*/
	void statParFormat(char* formBuf, size_t bufSize, int typeOfTransfer);

	/**
   	   @brief     cpStatFormat
	   @param     str
	   @param     size
	   @param     cp
	*/
	void cpStatFormat(char* formBuf, size_t bufSize, const std::string cp);

 private:

	/**
	   @brief     setFilePath
	   Set the statistic file path
	*/
	void setStatisticFilePath(const std::string& messageStoreName);

	/**
	   @brief     setFilePath
	   Set the statistic file path
	*/
	void setCpStatisticFilePath(const std::string& messageStoreName);

	/**
	   @brief     getRTRInternalPath
	*/
	void getRTRInternalPath(std::string& path);

	/**
	   @brief     getRTRInternalPathForCp
	*/
	void getRTRInternalPathForCp(const unsigned int& cpId, std::string& path);

	/**
	  @brief  statisticIMMObjectCreate
	*/
	bool statisticIMMObjectCreate(const std::string& parentDN );

	/**
	  @brief  cpStatisticIMMObjectCreate
	*/
	bool cpStatisticIMMObjectCreate(const std::string& jobDN );

	/**
		 @brief   openStatisticFile :
		 *  This method opens the statistic file.
	*/
	ACE_HANDLE openStatisticFile(bool& fileCreated);

	/**
		 @brief  checkFolder :
		 *  This method opens the statistic file.
	*/
	void checkFolders();

	/**
	  @brief  mapToMemoryStatisticFile :
	 *  This method maps into the memory the statistic file.
	*/
	bool mapToMemoryStatisticFile();

	/**
	  @brief  objectInitRecovery :
	 *  This method tries to recovery a failed statistic initialization
	*/
	void objectInitRecovery();

	/**
	   @brief     m_mutex
	 */
	ACE_Recursive_Thread_Mutex m_mutex;

	/**
	   @brief     m_initialized
	*/
	bool m_initialized;

	/**
	   @brief     m_isIMMObjectInit
	*/
	bool m_isIMMObjectInit;

	/**
	   @brief     m_isFileMapInit
	*/
	bool m_isFileMapInit;

	/**
		   @brief     _initialized
	*/
	short m_RDNKey;

	/**
	   @brief     m_isCpObject
	*/
	bool m_isCpObject;

	/**
	   @brief   m_jobDN
	*/
	std::string m_jobDN;

	/**
	   @brief     statFileName
	*/
	std::string m_statisticFilePath;

	/**
	   @brief   m_sharedMemoryAddr
	*/
	void* m_mappedMemoryAddr;

	/**
   	   @brief     posFileCreationTime
	*/
	char* posFileCreationTime;

	/**
   	   @brief     posKbACA
	*/
	int64_t* posKbACA;

	/**
   	   @brief     posKbGOH
	*/
	int64_t* posKbGOH;

	/**
   	   @brief     posFilled
	*/
	uint32_t* posFilled;

	/**
   	   @brief     posLost
	*/
	uint32_t* posLost;

	/**
		@brief     posSkipped
	*/
	uint32_t* posSkipped;

	/**
		@brief     posConnMS
	*/
	uint32_t* posConnMS;

	/**
		@brief     posAttachGOH
	*/
	uint32_t* posAttachGOH;

	/**
		@brief     posLastComBno
	*/
	uint32_t* posLastComBno;

	/**
   	   @brief     tempFilled
	*/
	uint32_t tempFilled;

	/**
   	   @brief     tempLost
	*/
	uint32_t tempLost;

	/**
	@brief     tempSkipped
	*/
	uint32_t tempSkipped;

};

#endif
