/**
 * @file Cute_SSU_DiskMon.h
 * @brief header file for Cute_SSU_DiskMon.cpp
 * It consists declaration of the cute test class for ACS_SSU_DiskMon
 * @version 1.1.1
 */
#ifndef _CUTE_SSU_DISKMON_H_
#define _CUTE_SSU_DISKMON_H_
/*Include Section */
/* Inclusion of cute libraries */
#include "cute_suite.h"


//#include "ACS_SSU_DiskMon.h"
/* Forward declaration */
class ACS_SSU_DiskMon;

/**
 * @brief Cute_SSU_DiskMon
 * Cute test class having test functions to test ACS_SSU_DiskMon
 */
class Cute_SSU_DiskMon
{
public:
	/**
	 * @brief
	 * Default constructor
	 * No arguments
	 * @return void
	 */
	Cute_SSU_DiskMon();
	/**
		 * @brief
		 * Test function to test a dummy alarm raising and ceasing for disk partition.
		 * No arguments
		 * @return void
		 *
		 */
		//static void vTestAlarmHandler();
	/**
		 * @brief
		 *static test function to test ACS_SSU_DiskMon::bgetDiskMonitoringSettings()
		 * No arguments
		 * @return void
		 *
		 */
	static void vTestGetDiskMonitorSettings();
	/**
			 * @brief
			 * static test function to test ACS_SSU_DiskMon::bGetDiskData()
			 * No arguments
			 * @return void
			 *
			 */
	static void vTestGetDiskData();

	/**
				 * @brief
				 * static test function to test ACS_SSU_DiskMon::bRemoveSmallPartitions()
				 * No arguments
				 * @return void
				 *
				 */
	static void vTestRemoveSmallPartitions();

	/**
					 * @brief
					 *static test function to test ACS_SSU_DiskMon::bGetSpecificPartitionSettings()
					 * No arguments
					 * @return void
					 *
					 */
	static void vGetSpecificPartitionSettings();

	/**
						 * @brief
						 * static test function to test ACS_SSU_DiskMon::bGetDefaultParitionSettings()
						 * No arguments
						 * @return void
						 *
						 */
	static void vGetDefaultParitionSettings();

	/**
							 * @brief
							 * static test function to test ACS_SSU_DiskMon::vCheckPartitions()
							 * No arguments
							 * @return void
							 *
							 */
	static void vTestCheckPartitions();

	/**
								 * @brief
								 * static test function to test ACS_SSU_DiskMon::bCheckAndRaiseAlarmForPartition()
								 * No arguments
								 * @return void
								 *
								 */
	static void vCheckAndRaiseAlarmForPartition();

  /**
									 * @brief
									 * static test function to check and cease alarms for disk partitions
									 * No arguments
									 * @return void
									 *
									 */
	static void vCheckCeaseAlarmForPartition();

	/**
										 * @brief
										 * static function returns pointer reference of ACS_SSU_DiskMon
										 * No arguments
										 * @return ACS_SSU_DiskMon
										 *
										 */
	static ACS_SSU_DiskMon * getPoDiskMon(){
		return m_poDiskMon;
	}

	static void vDiskMonCleanup();


	/**
											 * @brief
											 * static function to make the test suite for  ACS_SSU_DiskMon
											 * No arguments
											 * @return void
											 *
											 */
	static cute::suite make_suite_Cute_SSU_DiskMon();
//private members declaration
private:
	/**
	 * @brief
	 * its a static reference of type ACS_SSU_DiskMon
	 */
	static ACS_SSU_DiskMon *m_poDiskMon;
};

#endif
