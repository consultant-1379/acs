/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2012
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 *----------------------------------------------------------------------*//**
 *
 * @file acs_cs_moddcommands.h
 *
 * @brief
 *
 *
 * @details
 *
 *
 * @author XNHAPHA
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 * 2012-05-18  XMAGLEX  Removed all code not needed for hwcls
 * 2013-11-05  XNHAPHA  Add opensession -a|-f [username] format to support 
 *                      change request O&M traffic relay to AP2
 * 2014-04-07  XNHAPHA  Fix the TRs: HS46371 and HS42399
 *
 ****************************************************************************/

#include <map>
#include <vector>
#include <stdarg.h>
#include <iostream>
#include <cstring>
#include <stdio.h>

#include "ACS_CS_Attribute.h"

#include "ACS_CS_ImBlade.h"
#include <ACS_TRA_trace.h>

namespace NSCommand
{	
	//HWC Command Exit Codes
	enum Exit_Code
	{
		Exit_Success =					0,
		Exit_Error_Executing =			1,
		Exit_Incorrect_Usage =			2,
		Exit_Illegal_Option =			5,
		Exit_NoReply_Cs =			7,
		Exit_OnlyAllowed_From_AP1 =		10,
		Exit_Hardware_NotPresent=		14,
		Exit_Slot_Occupied =			15,
		Exit_No_Such_Entry =			16,
		Exit_Incorrect_Mag =			21,
		Exit_Incorrect_Slot =			22,
		Exit_Incorrect_FBN =			23,
		Exit_Incorrect_SysNum =			24,
		Exit_Incorrect_SeqNum =			25,
		Exit_Incorrect_Side =			26,
		Exit_Incorrect_IPAlias =		27,
		Exit_Incorrect_Netmask =		28,
		Exit_Incorrect_DHCPReq =		29,
		Exit_Incorrect_SysType =		30,
		Exit_Incorrect_OutputFormat =	31,
		Exit_CPID_OutsideRange		=	32,
		Exit_Fnb_Type_forbidden 	=	33,
		Exit_FuncBoard_NotAllowed =             112,
		Exit_Illegal_Command        = 	115,
		Exit_Unable_Contact_Server =	117, // Blade Cluster exit code
		Exit_Connection_Failure =       119,
		Exit_No_Config_Info = 120,
		Exit_Incorrect_Usage_Full = 121
	};

	//Command Output Format
	enum HWC_Output_Format
	{
		Output_Format_SHORT =			0,
		Output_Format_LONG =			1,
		Output_Format_XML =				2
	};
   
	enum Open_Session_Type
	{
		TELNET      = 0,
		SSH         = 1,
		SFTP        = 2,
		SSH_4422	= 3
	};
}

//struct for all data related to command options
struct commandStruct
{
	string currCommand;			//current command executed
	
	bool boolMagazine;
	uint32_t magazine;
	bool boolSlot;
	uint16_t slot;
	bool boolFbn;
	uint16_t fbn;
	bool boolSystemType;
	uint16_t systemType;
	bool boolSystemNum;
	uint16_t systemNum;
	bool boolSeqNum;
	uint16_t seqNum;
	bool boolSide;
	uint16_t side;
	bool boolDhcpReq;
	uint16_t dhcpReq;
	bool boolOutputFormat;
	uint16_t outputFormat;
	string strIpEthA;
	string strIpEthB;
	unsigned short exitCode;		//the exit code for the command
	string exitCodeDescriptionArg;	//the description argument for the command's exit code
	bool openSessionFormat1;
	bool openSessionFormat2;
	bool openSessionFormat3;
	bool isAPZTypeAvail;   // This flag to indicate if the APZ Type is available
	bool isAPGCountAvail;  // This flag to indicate if APG count is available
	NSCommand::Open_Session_Type openSessionType;
	string username;
};

//struct for all data related to printout
struct printOut
{
	string MAG;
	uint32_t UL_MAG;
	string SLOT;
	uint16_t US_SLOT;
	string SYSTYPE;
	string SYSNUM;
	string FBN;
	string SIDE;
	string SEQNUM;
	string IPA;
	string IPB;
	string ALIASA;
	string MASKA;
	string ALIASB;
	string MASKB;
	string DHCP;
	string PRODID;
};

struct IPAssignment
{
	uint32_t iPEthA;
	uint32_t iPEthB;
};

struct MountFile
{
	char source[128];
	char target[128];
};

class ACS_CS_MODDCommands
{
	public:			
		ACS_CS_MODDCommands(string commandName);
		virtual ~ACS_CS_MODDCommands();
		
		bool handleHWCLS(int argc, char* argv[]);
		bool checkOpenSessionFormat(int argc, char* argv[]);
		//functions for handling exit codes and descriptions for them
		unsigned short getExitCode();
		string getExitCodeDescription(const unsigned short exitCode);
		string getExitCodeDescriptionArg();
		bool handleOpenSession();

      private:
		//functions to check command line arguments
		bool checkFormat(int argc, char* argv[]);
		bool checkOptions(int argc, char* argv[]);
		bool checkOpenSessionOptions(string optionString, int argc, NSCommand::Exit_Code &exitCode, string &illegalOption);
		bool checkOptionArguments(int argc, char* argv[]);
		bool checkOpenSessionOptionArguments(int argc, char* argv[]);
		bool faultyOption(NSCommand::Exit_Code, string faultyOptionArg);
		bool faultyOption(NSCommand::Exit_Code);

		bool checkArgOutputFormat(char* arg);
		bool checkHwcLsOptions(string optionString, NSCommand::Exit_Code &exitCode, string &illegalOption);

		bool checkArgMag(char* arg);
		bool checkArgSlot(char* arg);
		bool checkArgFBN(char* arg);
		bool checkOpenSessionArgFBN(char* arg);
		bool checkArgSysType(char* arg, bool &cpBlade);
		bool checkArgSysNum(char* arg);
		bool checkArgSide(char* arg);
		bool checkArgDHCP(char* arg);

		bool dottedDecToLong(istringstream &istr, uint32_t &value,
									uint32_t lowerLimit1, uint32_t upperLimit1,
									uint32_t lowerLimit2, uint32_t upperLimit2,
									uint32_t lowerLimit3, uint32_t upperLimit3,
									uint32_t lowerLimit4, uint32_t upperLimit4);

		bool checkFigures(istringstream &istr);
		unsigned short numOfChar(istringstream &istr, char searchChar);
		string getArgument(istringstream &istr, uint16_t optionNum);
		void clearStruct(commandStruct* commStruct);
		void setExitCodes(string currCommand);
		string stringToUpper(string strToConvert);
		void reverseDottedDecStr(istringstream &istr);
		bool checkEnv();
		bool getHWCTable();
		bool printTable();
		void sortTable();
	        bool checkOpenSessionBoardSearch();
		bool getAttributes(ACS_CS_Attribute ***array, const ACS_CS_ImBlade * blade, size_t &size, std::map<std::string, std::string> parentMap);
		void hwclsFilter(std::set<const ACS_CS_ImBase *> & objects, std::map<std::string, std::string> parentMap);
		string getParentFromRdn(string rdn);
		void convComstrToIp(uint32_t ipEth,string &strIpEth);
		commandStruct* commStruct;				//struct holding option and option argument values
		printOut* printStruct;					//struct holding info about the printout for hwcls
		map<unsigned int, string> exitCodeMap;	//map holding the exit code descriptions	
		vector<printOut*> printOutVector;		//vector holding the printout structs
		bool isMultipleCpSystem(bool &);
		bool identifyApplicableFormat(bool &, bool &, bool &);
		bool handleTelnetSession();
		bool handleSshSession();
		bool handleSftpSession();
		bool mountDirectorForSftp();
		bool umountDirectorForSftp();
		bool getParentProcessPid(pid_t processId, pid_t &parentProcessId);
		bool runCommand(const string command, string& output);
		std::string removeInvalidCharsFromCmd(const std::string& str, const std::string& fill = " ", const std::string& whitespace = " \t\n");
		std::string trimCmd(const std::string& str, const std::string& whitespace = " \t");
		inline void setUserName(std::string& username) {
			cout<<"login:";
			getline(cin,username);
		}
};

#define TRACE \
      CommandTrace(__FILE__,__LINE__).CommandTracing

class CommandTrace
{
public:
   static ACS_TRA_trace *TraceObject;

   CommandTrace(const char* fileName, int lineNumb):file(CommandTrace::fileWithoutPath(fileName)),line(lineNumb) { };

private:
   const char* file;
   const int line;

public:
   void CommandTracing(const char* messageFormat, ...)
   {
      if( TraceObject==0 )
      {
         return;
      }
      const unsigned int TRACE_BUF_SIZE = 1024;
      char traceBuffer[TRACE_BUF_SIZE];
      char traceOutput[TRACE_BUF_SIZE];
      if ( messageFormat && *messageFormat)
      {
         va_list params;
         va_start(params, messageFormat);
         vsnprintf(traceBuffer, TRACE_BUF_SIZE - 1, messageFormat, params);
         traceBuffer[TRACE_BUF_SIZE-1]='\0';
         traceOutput[TRACE_BUF_SIZE-1]='\0';
         sprintf(traceOutput,"| %ld | %s | %d | %s",pthread_self(),fileWithoutPath(file),line,traceBuffer);

         //! If Trace if ON .Send trace to TRA
         if( TraceObject->ACS_TRA_ON() )
         {
            TraceObject->ACS_TRA_event(1,traceOutput);
         }

         va_end(params);
      }
   };
   
   static const char* fileWithoutPath(const char* fileName)
   {
      if (fileName == 0)
      {
         return "Unknown";
      }

      const char* fileNameStart = strrchr(fileName, '/');

      if (fileNameStart != NULL && (fileNameStart[1] != '\0'))
      {
         return fileNameStart + 1;
      }
      else
      {
         return fileName;
      }
   };
};
