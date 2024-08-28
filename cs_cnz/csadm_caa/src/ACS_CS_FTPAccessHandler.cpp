/*
 * ACS_CS_FTPAccessHandler.cpp
 *
 *  Created on: May 23, 2016
 *      Author: xclaele
 */
#include <cstring>
#include "ACS_CS_Trace.h"
#include "ACS_CS_TableEntry.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_FTPAccessHandler.h"
#include <sys/poll.h>

ACS_CS_Trace_TDEF(ACS_CS_FTPAccessHandler_TRACE);

namespace iptables_commands {

const std::string insertCommandPrefix = "/opt/ap/apos/bin/clusterconf/clusterconf iptables -a all -I INPUT 1 -p tcp --dport 21 -s ";
const std::string insertIpTableCommandPrefix = "iptables -I INPUT 1 -p tcp --dport 21 -s ";
const std::string insertCommandSuffix = " -j ACCEPT";

const std::string deleteCommandPrefix = "/opt/ap/apos/bin/clusterconf/clusterconf iptables -d ";
const std::string deleteIpTableCommandPrefix = "iptables -D INPUT ";

const std::string findRuleNumerPrefix = "/opt/ap/apos/bin/clusterconf/clusterconf iptables -D | grep ";
const std::string findRuleIpTableNumerPrefix = "iptables -L --line-number | grep -w ";
const std::string findRuleNumberSuffix = " | awk '{print $1}'";

const std::string restartIpTables = "systemctl restart lde-iptables.service";

const std::string findInIpTables = "iptables -L | grep -w ";

const int MAX_INSERT_RETRY = 3;

}

//******************************************************************************
//
ACS_CS_FTPAccessHandler_Impl::ACS_CS_FTPAccessHandler_Impl()
: shelfArchitecture(NODE_UNDEFINED)
{
	// Check the shelf Architecture
	ACS_CS_Registry::getNodeArchitecture(shelfArchitecture);
}


//******************************************************************************
//
ACS_CS_FTPAccessHandler_Impl::~ACS_CS_FTPAccessHandler_Impl()
{

}

//******************************************************************************
//
bool ACS_CS_FTPAccessHandler_Impl::executeCommand(const std::string& command, std::string& output)
{
	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_FTPAccessHandler::executeCommand() --> IN\n"
			"Running command:<%s>\n", command.c_str()));
	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
                        "(%t) ACS_CS_FTPAccessHandler::Mutex acquiring\n"));
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_commandMutex);
	bool result = false;
	FILE* pipe = popen(command.c_str(), "r");
	if(NULL != pipe)
	{
		char rowOutput[1024]={'\0'};

		while(!feof(pipe))
		{
			// get the cmd output
			if(fgets(rowOutput, 1023, pipe) != NULL)
			{
				std::size_t len = strlen(rowOutput);
				// remove the newline
				if( rowOutput[len-1] == '\n' ) rowOutput[len-1] = 0;

				output.append(rowOutput);
			}
		}
		// wait cmd termination
		int exitCode = pclose(pipe);

		// get the exit code from the exit status
		result = (WEXITSTATUS(exitCode) == 0);
	}


	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_FTPAccessHandler::executeCommand() --> OUT\n"
			"result:<%s>, command : <%s>\n, command output:<%s>\n", (result ? "TRUE" : "FALSE"), command.c_str(), output.c_str()));
	return result;

}

//******************************************************************************
//
void ACS_CS_FTPAccessHandler_Impl::insertRuleInClusterConfAndApply(const std::string& ipEthA, const std::string& ipEthB, int retry)
{
	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_FTPAccessHandler::insertRuleInClusterConfAndApply() --> IN\n"));

	// Assemble command
	std::string ruleInsertCommand(iptables_commands::insertCommandPrefix);
	ruleInsertCommand += ipEthA;
	ruleInsertCommand += ",";
	ruleInsertCommand += ipEthB;
	ruleInsertCommand += iptables_commands::insertCommandSuffix;

	// Execute command
	std::string commandOutput;
	executeCommand(ruleInsertCommand, commandOutput);

	// Apply change restarting ip-tables
	std::string commandOutput2;
	executeCommand(iptables_commands::restartIpTables, commandOutput2);

	// Ensure rule is present in ip-tables
	if (!CheckRuleInIpTables(ipEthA) || !CheckRuleInIpTables(ipEthB))
	{
		// First delete rule in cluster.conf
		deleteRuleFromClusterConfAndApply(ipEthA, ipEthB);

		// Retry to insert rule
		if (retry < iptables_commands::MAX_INSERT_RETRY)
		{
			retry++;
			insertRuleInClusterConfAndApply(ipEthA, ipEthB, retry);
		}
		else
		{
			ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_ERROR,
					"(%t) ACS_CS_FTPAccessHandler::insertRuleInClusterConfAndApply()\n"
					"ERROR: max retry reached --> Failed to insert FTP rules for ipEthA:<%s> and ipEthB:<%s>\n", ipEthA.c_str(), ipEthB.c_str()));
		}
	}

	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_FTPAccessHandler::insertRuleInClusterConfAndApply() --> OUT\n"));
}

//******************************************************************************
//
void ACS_CS_FTPAccessHandler_Impl::deleteRuleFromClusterConfAndApply(const std::string& ipEthA, const std::string& ipEthB)
{
	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_FTPAccessHandler::deleteRuleFromClusterConfAndApply() --> IN\n"));

	// Find rule number in cluster conf
	std::string ruleNumberFindCommand(iptables_commands::findRuleNumerPrefix);
	ruleNumberFindCommand += ipEthA;
	ruleNumberFindCommand += ",";
	ruleNumberFindCommand += ipEthB;
	ruleNumberFindCommand += iptables_commands::findRuleNumberSuffix;
	std::string commandOutput;
	executeCommand(ruleNumberFindCommand, commandOutput);

	if(!commandOutput.empty())
	{
		// Assemble command
		std::string ruleDeleteCommand(iptables_commands::deleteCommandPrefix);
		ruleDeleteCommand += commandOutput;

		// Execute command
		std::string commandOutput2;
		executeCommand(ruleDeleteCommand, commandOutput2);

		// Apply change restarting ip-tables
		std::string commandOutput3;
		executeCommand(iptables_commands::restartIpTables, commandOutput3);
	}

	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_FTPAccessHandler::deleteRuleFromClusterConfAndApply() --> OUT\n"));
}

//******************************************************************************
//
bool ACS_CS_FTPAccessHandler_Impl::CheckRuleInIpTables(const std::string& ipEth)
{
	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_FTPAccessHandler::CheckRuleInIpTables() --> IN\n"));

	bool result = false;

	std::string findInIpTablesCommand(iptables_commands::findInIpTables);
	findInIpTablesCommand += ipEth;
	std::string commandOutput;
	executeCommand(findInIpTablesCommand, commandOutput);

	if(!commandOutput.empty())
	{
		ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
				"(%t) ACS_CS_FTPAccessHandler::CheckRuleInIpTables()\n"
				"OK: FTP rule for ip:<%s> is present in ip-tables\n", ipEth.c_str()));

		result = true;
	}
	else
	{
		ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_WARN,
				"(%t) ACS_CS_FTPAccessHandler::CheckRuleInIpTables()\n"
				"WARNING: FTP rule for ip address:<%s> NOT present in ip-tables\n", ipEth.c_str()));
	}

	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_FTPAccessHandler::CheckRuleInIpTables() --> OUT\n"));

	return result;
}

//******************************************************************************
//
void ACS_CS_FTPAccessHandler_Impl::enableBladeForFTP(const ACS_CS_ImBlade* blade)
{
	//insertRuleInClusterConfAndApply(blade->ipAddressEthA, blade->ipAddressEthB);
	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
                        "(%t) ACS_CS_FTPAccessHandler::enableBladeForFTP() --> IN\n"));
	insertRuleInIpTables(blade->ipAddressEthA, blade->ipAddressEthB);
	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
                        "(%t) ACS_CS_FTPAccessHandler::enableBladeForFTP() --> OUT\n"));
}

//******************************************************************************
//
void ACS_CS_FTPAccessHandler_Impl::disableBladeForFTP(const ACS_CS_ImBlade* blade)
{
	//deleteRuleFromClusterConfAndApply(blade->ipAddressEthA, blade->ipAddressEthB);
	deleteRuleFromIpTable(blade->ipAddressEthA);
	deleteRuleFromIpTable(blade->ipAddressEthB);
}

void ACS_CS_FTPAccessHandler_Impl::disableCPandAPBladesForFTP(ACS_CS_Table* HWCTable)
{
        ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
                        "(%t) ACS_CS_FTPAccessHandler::disableCPandAPBladesForFTP() --> IN\n"));

        std::map<unsigned short, ACS_CS_TableEntry*>* hwcTableMap = HWCTable->getTable();
        std::map<unsigned short, ACS_CS_TableEntry*>::const_iterator it;
        for(it = hwcTableMap->begin(); it != hwcTableMap->end(); ++it)
        {
                ACS_CS_Attribute fbnAttr = it->second->getValue(ACS_CS_Protocol::Attribute_FBN);
                int fbnValue = 0;
                fbnAttr.getValue(reinterpret_cast<char*>(&fbnValue), static_cast<int>(sizeof(fbnValue)));

                if(fbnValue == CPUB || fbnValue == MAUB || fbnValue == APUB || (shelfArchitecture == NODE_VIRTUALIZED && fbnValue == IPLB))
                {
                        ACS_CS_Attribute ipEthAAttr = it->second->getValue(ACS_CS_Protocol::Attribute_IP_EthA);
                        int ipEthALength = ipEthAAttr.getValueLength();
                        char ipEthAValue[ipEthALength + 1];
                        ipEthAAttr.getValue(ipEthAValue, ipEthALength);
                        ipEthAValue[ipEthALength] = 0;

                        ACS_CS_Attribute ipEthBAttr = it->second->getValue(ACS_CS_Protocol::Attribute_IP_EthB);
                        int ipEthBLength = ipEthBAttr.getValueLength();
                        char ipEthBValue[ipEthBLength + 1];
                        ipEthBAttr.getValue(ipEthBValue, ipEthBLength);
                        ipEthBValue[ipEthBLength] = 0;

		        deleteRuleFromIpTable(std::string(ipEthAValue));
		        deleteRuleFromIpTable(std::string(ipEthBValue));
                }
        }
	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
                        "(%t) ACS_CS_FTPAccessHandler::disableCPandAPBladesForFTP() --> OUT\n"
                        "FTP rules have been deleted from ip-tables \n"));
}

//******************************************************************************
//
void ACS_CS_FTPAccessHandler_Impl::enableCPandAPBladesForFTP(ACS_CS_Table* HWCTable)
{
	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_FTPAccessHandler::enableCPandAPBladesForFTP() --> IN\n"));

	int count = 0;
	std::map<unsigned short, ACS_CS_TableEntry*>* hwcTableMap = HWCTable->getTable();
	std::map<unsigned short, ACS_CS_TableEntry*>::const_iterator it;
	for(it = hwcTableMap->begin(); it != hwcTableMap->end(); ++it)
	{
		ACS_CS_Attribute fbnAttr = it->second->getValue(ACS_CS_Protocol::Attribute_FBN);
		int fbnValue = 0;
		fbnAttr.getValue(reinterpret_cast<char*>(&fbnValue), static_cast<int>(sizeof(fbnValue)));

		if(fbnValue == CPUB || fbnValue == MAUB || fbnValue == APUB || (shelfArchitecture == NODE_VIRTUALIZED && fbnValue == IPLB))
		{
			ACS_CS_Attribute ipEthAAttr = it->second->getValue(ACS_CS_Protocol::Attribute_IP_EthA);
			int ipEthALength = ipEthAAttr.getValueLength();
			char ipEthAValue[ipEthALength + 1];
			ipEthAAttr.getValue(ipEthAValue, ipEthALength);
			ipEthAValue[ipEthALength] = 0;

			ACS_CS_Attribute ipEthBAttr = it->second->getValue(ACS_CS_Protocol::Attribute_IP_EthB);
			int ipEthBLength = ipEthBAttr.getValueLength();
			char ipEthBValue[ipEthBLength + 1];
			ipEthBAttr.getValue(ipEthBValue, ipEthBLength);
			ipEthBValue[ipEthBLength] = 0;

			//insertRuleInClusterConfAndApply(std::string(ipEthAValue), std::string(ipEthBValue));
			insertRuleInIpTables(std::string(ipEthAValue), std::string(ipEthBValue));
			count++;
		}
	}

	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_FTPAccessHandler::enableCPandAPBladesForFTP() --> OUT\n"
			"<%i> FTP rules have been inserted in ip-tables \n", count*2));
}

//******************************************************************************
//
void  ACS_CS_FTPAccessHandler_Impl::insertRuleInIpTables(const std::string& ipEthA, const std::string& ipEthB, int retry)
{
	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_FTPAccessHandler::insertRuleInIpTables() --> IN\n"));
	
        // Ensure rule is present in ip-tables
	if (!CheckRuleInIpTables(ipEthA) || !CheckRuleInIpTables(ipEthB))
	{
		// Assemble command
		std::string ruleInsertCommand(iptables_commands::insertIpTableCommandPrefix);
		ruleInsertCommand += ipEthA;
		ruleInsertCommand += ",";
		ruleInsertCommand += ipEthB;
		ruleInsertCommand += iptables_commands::insertCommandSuffix;

		// Execute command
		std::string commandOutput;
		bool result = executeCommand(ruleInsertCommand, commandOutput);

		if ( result == false )
		{
			//Sleep of time is required
			ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_WARN,
							"(%t) CommandOutput is FALSE \n"));
			int retry_wait =0;
			while((retry_wait = poll(0,0,200)) == -1
					&& errno == EINTR)
			{
				continue;
			}
			// Retry to insert rule
			if (retry < iptables_commands::MAX_INSERT_RETRY)
			{
				retry++;
				insertRuleInIpTables(ipEthA, ipEthB, retry);
			}
			else
			{
				ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_ERROR,
					"(%t) ACS_CS_FTPAccessHandler::insertRuleInIpTables()\n"
					"ERROR: max retry reached --> Failed to insert FTP rules for ipEthA:<%s> and ipEthB:<%s>\n", ipEthA.c_str(), ipEthB.c_str()));
			}
		}
		else
		{
			//no sleep is required
			ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
									"(%t) CommandOutput is TRUE \n"));
		}
	}

	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_FTPAccessHandler::insertRuleInIpTables() --> OUT\n"));
}

//******************************************************************************
//
void ACS_CS_FTPAccessHandler_Impl::deleteRuleFromIpTable(const std::string& ipEth)
{
	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_FTPAccessHandler::deleteRuleFromClusterConfAndApply() --> IN\n"));

	// Find rule number in cluster conf
	std::string ruleNumberFindCommand(iptables_commands::findRuleIpTableNumerPrefix);
	ruleNumberFindCommand += ipEth;
	ruleNumberFindCommand += iptables_commands::findRuleNumberSuffix;
	std::string commandOutput;
	executeCommand(ruleNumberFindCommand, commandOutput);

	if(!commandOutput.empty())
	{
		// Assemble command
		std::string ruleDeleteCommand(iptables_commands::deleteIpTableCommandPrefix);
		ruleDeleteCommand += commandOutput;

		// Execute command
		std::string commandOutput2;
		executeCommand(ruleDeleteCommand, commandOutput2);

	}

	ACS_CS_FTRACE((ACS_CS_FTPAccessHandler_TRACE, LOG_LEVEL_INFO,
			"(%t) ACS_CS_FTPAccessHandler::deleteRuleFromClusterConfAndApply() --> OUT\n"));
}
