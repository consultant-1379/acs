/*=================================================================== */
/**
   @file acs_ssu_disk_mon.h

   This module contains the implementation for All mounted partitions are monitored using statvfs
   system call. This monitors not only local file systems but also remote file systems which are
   mounted.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       DD/MM/YYYY     NS       Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */
#ifndef _ACS_SSU_DISKMON_H
#define _ACS_SSU_DISKMON_H

//! Standard Libraries
#include <map>
#include <list>
#include <string>

//! Internal Includes
#include "acs_ssu_types.h"

#include <ace/Event.h>
#ifdef ACS_SSU_CUTE_TEST
#include <Cute_SSU_DiskMon.h>
#endif

//! Predeclaration
//class ACE_Event;
class ACS_SSU_AlarmHandler;

class ACS_SSU_DiskMon
{
public:
	/** @brief ACS_SSU_DiskMon Default constructor
	*
	*	ACS_SSU_DiskMon Constructor of class
	*
	*	@remarks Remarks
	*/
	ACS_SSU_DiskMon(ACE_Event* poEvent, ACS_SSU_AlarmHandler* poAlarmHandler);
	/** @brief ACS_SSU_DiskMon destructor
	*
	*	ACS_SSU_RtHanThread destructor of class
	*
	*	@remarks Remarks
	*/
   ~ACS_SSU_DiskMon();

   /*!
	 * @brief SSU Disk Monitor Thread Loop
	 *
	 * SSU Monitor Thread Runs in this loop forever untill Terminated
	 * by SSU Monitor Service Thread Manager
	 */
   void Execute();

private:

    //! Alarm Handler test function
    //bool bTestAlarmHandler();

    /*!
     * @brief Updates internal structures with Default and Specific settings
     *
     * @return TRUE Success
     * @return FALSE Failure
     */
    BOOL bGetDiskMonitorSettings();

    /*!
     * @brief Fetches mounted partitions and their File system information
     * @param refoPartitionsInfo List to be filled with Partition information
     * @return TRUE Success
     * @return FALSE Failure
     */
    BOOL bGetDiskData( std::map<std::string,struct statvfs> &refoPartitionsInfo );

    /*!
     * @brief Print the Structure @c refoPartitionsInfo
     * @param refoPartitionsInfo
     */
    void vPrintPartitions( std::map<std::string,struct statvfs> & refoPartitionsInfo );

    /*!
     * @brief Remove Partitions from
     * @param oPartitionsInfo List of Partitions and their file system information
     * @return TRUE Successfully removed small Partitions less than 4 MB
     * @return FALSE Failure
     */
    BOOL bRemoveSmallPartitions( std::map<std::string,struct statvfs> & oPartitionsInfo);

    /*!
     * @brief Checks Partitions  and raises or ceases the alarms
     * @param refoPartitionsInfo List of Partitions to be checked
     *
     */
    void vCheckPartitions(std::map<std::string,struct statvfs> & refoPartitionsInfo );

    /*!
     * @brief Fetches the Specific disk Partition settings if present
     * @param pszPartition
     * @param refoDiskData
     * @return
     */
    BOOL bGetSpecificPartitionSettings(const std::string &pszPartition,  std::list< SSU_PERF_DATA > &refoDiskData  );

    /*!
     * @brief Retrives default Partition Alarm settings
     * @param oFileSysInfo Filesystem for which Alarms settings to be retrieved
     * @param refoDiskData Alarm settins to be populated to
     * @return TRUE Success
     * @return False Failure
     */
    BOOL bGetDefaultPartitionSettings( struct statvfs & refoFileSysInfo,  std::list< SSU_PERF_DATA > &refoDiskData );
    /*!
     * @brief Check for the Partition Disk and verify if it exceed the Quota
     * Raises the alarms for exceeded quota
     * @param partition - The name of the partition
     * @param oFileSysData - Partition Data
     * @param olsAlarmsForPartition - Alarms list for the partition
     * @return TRUE Success
     * @return False Failure
     */
    BOOL bCheckAndRaiseAlarmForPartition(std::string partition,
    		struct statvfs & oFileSysData,
    		std::list<SSU_PERF_DATA> &olsAlarmsForPartition);

    /*!
    * @brief Retrives all PHA settings from IMM
    * @return TRUE Success
    * @return False Failure
    */
    bool readAllPHAValuesForLogicalDisk();

    /*=================================================================== */
    /** @brief   m_listDiskMonitor.
    */
    /*=================================================================== */
    std::list<LPSSU_DISK_MONITOR>  m_listDiskMonitor;
    /*=================================================================== */
    /** @brief   m_srctDefaultDiskDataLow.
    */
    /*=================================================================== */
    SSU_PERF_DATA                  m_srctDefaultDiskDataLow;
    /*=================================================================== */
    /** @brief   m_srctDefaultDiskDataHigh.
    */
    /*=================================================================== */
    SSU_PERF_DATA                  m_srctDefaultDiskDataHigh;
    /*=================================================================== */
    /** @brief   m_srctDiskAlarmLevelLow.
    */
    /*=================================================================== */
    SSU_DISK_ALARM_LEVEL           m_srctDiskAlarmLevelLow;
    /*=================================================================== */
    /** @brief   m_srctDiskAlarmLevelHigh.
    */
    /*=================================================================== */
    SSU_DISK_ALARM_LEVEL           m_srctDiskAlarmLevelHigh;
    /*=================================================================== */
    /** @brief   m_phEndEvent.
    */
    /*=================================================================== */
    ACE_Event *m_phEndEvent;
    /*=================================================================== */
    /** @brief   m_poAlarmHandler.
    */
    /*=================================================================== */
    ACS_SSU_AlarmHandler* m_poAlarmHandler;
#ifdef ACS_SSU_CUTE_TEST
    friend class Cute_SSU_DiskMon;
#endif
};

#endif

/*! @} */
