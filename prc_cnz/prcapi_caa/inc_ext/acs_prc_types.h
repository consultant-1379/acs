/*
 * acs_prc_types.h
 *
 *  Created on: Feb 25, 2011
 *      Author: xlucpet
 */

#ifndef ACS_PRC_TYPES_H_
#define ACS_PRC_TYPES_H_

enum order_t {first = 0x1, last = 0x2};
enum runLevelType {level_0, level_1, level_2, level_3, level_4, level_5, unknownLevel};
enum nodeStateType {active, passive, unknownNodeState};
enum resourceStateType {started, stopped, failed, inserted, removed, unknownResourceState};

enum reasonType {causedByFailover,		// Caused by failover
				 causedByError,			// Caused by fault
				 functionChange,		// Initiated by FCH
				 softFunctionChange,	// Initiated by SFC
				 userInitiated,			// Initated by anyone
				 causedByEvent,			// Initated by PRC_Eva
				 manualRebootInitiated,	// Manually initated reboot
				 systemInitiated,		// Internal use only
				 unknownReason,			// Unknown
				 referenceFC,			// Function Change FCR
				 nsfInitiated,			// Initiated by NSF
				 resourceFailed,		// Initiated by a resource failing too often
				 hwmResetInitiated,		// Initiated by an hwmreset command
				 hwmBlkInitiated,		// Initiated by an hwmblk command
				 hwmDblkInitiated,		// Initiated by an hwmdblk command
				 manualShutdown,		// Caused by standard windows shutdown procedure
				 bugcheckInitiated,		// Caused by a Bugcheck (Blue Screen)
				 fccReset,				// Caused by an fcc_reset command
				 amBoardWatchdog,		// Caused by the AM-Board: Watchdog
				 amBoardTemperature,	// Caused by the AM-Board: High Temperature
				 amBoardVoltage,		// Caused by the AM-Board: Bad Voltage
				 amBoardPMC,			// Caused by the AM-Board: PMC Power Signal
				 amBoardCPCI};			// Caused by the AM-Board: CPCI Bus Signal

enum
{
	ACS_PRC_IMM_ERROR = -1,
	ACS_PRC_IMM_ERROR_SEARCH = -2,
};

enum
{
	ACS_PRC_LOCAL_NODE = 0,
	ACS_PRC_REMOTE_NODE = 1,
};

enum
{
	ACS_PRC_NODE_ACTIVE = 1,
	ACS_PRC_NODE_PASSIVE = 2,
};

#endif /* ACS_PRC_TYPES_H_ */
