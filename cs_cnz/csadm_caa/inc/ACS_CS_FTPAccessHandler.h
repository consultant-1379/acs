/*
 * ACS_CS_FTPAccessHandler.h
 *
 *  Created on: May 23, 2016
 *      Author: xclaele
 */

#ifndef CSADM_CAA_INC_ACS_CS_FTPACCESSHANDLER_H_
#define CSADM_CAA_INC_ACS_CS_FTPACCESSHANDLER_H_


#include <ace/Singleton.h>

#include "ACS_CS_ImBlade.h"
#include "ACS_CS_Table.h"

class ACS_CS_FTPAccessHandler_Impl {

	friend class ACE_Singleton<ACS_CS_FTPAccessHandler_Impl, ACE_Recursive_Thread_Mutex>;

private:

	ACS_CS_FTPAccessHandler_Impl();

	~ACS_CS_FTPAccessHandler_Impl();

public:

	// Enable blade for file transfer
	void enableBladeForFTP(const ACS_CS_ImBlade* blade);

	// Disable blade for file transfer
	void disableBladeForFTP(const ACS_CS_ImBlade* blade);

	// Check HWC Table for adding rules in IP-tables
	void enableCPandAPBladesForFTP(ACS_CS_Table* HWCTable);
	
	// Check HWC Table to delete rules from IP-tables
	void disableCPandAPBladesForFTP(ACS_CS_Table* HWCTable);

private:
	ACE_Recursive_Thread_Mutex m_commandMutex;
	// Add an ip-tables rule
	void insertRuleInClusterConfAndApply(const std::string& ipEthA, const std::string& ipEthB, int retry = 0);

	// Remove an ip-tables rule
	void deleteRuleFromClusterConfAndApply(const std::string& ipEthA, const std::string& ipEthB);

	// Check a rule is present in ip-tables
	bool CheckRuleInIpTables(const std::string& ipEth);

	// Execute a command
	bool executeCommand(const std::string& command, std::string& output);

	//Add in ip-tables
	void insertRuleInIpTables(const std::string& ipEthA, const std::string& ipEthB, int retry = 0);

	//Remove from ip-tables
	void deleteRuleFromIpTable(const std::string& ipEth);

        //shelfArchitecture
	int shelfArchitecture;
};

typedef ACE_Singleton<ACS_CS_FTPAccessHandler_Impl, ACE_Recursive_Thread_Mutex> ACS_CS_FTPAccessHandler;

#endif /* CSADM_CAA_INC_ACS_CS_FTPACCESSHANDLER_H_ */
