#pragma once

#include <string>
#include <string.h>

// Define for TCastcParser class
#define STR_AADTS_CASTC_LINE_SEPARATOR						"\n\r"
#define STR_AADTS_CASTC_LINE_ORDERED							"ORDERED"
#define STR_AADTS_CASTC_LINE_EXECUTED							"EXECUTED"
#define STR_AADTS_CASTC_LINE_NOTACCEPTED					"NOT ACCEPTED"
#define STR_AADTS_CASTC_LINE_TIMEOUT							"TIME OUT"

// Define for TCastcSender class
	// Integer
#define INT_CASTCSENDER_MMLSECONDSTIMEOUT					180
#define INT_CASTCSENDER_CACHE_READ								1024
	// Integer return code
#define INT_CASTCSENDER_EXEC_FAIL									3
	// Char
#define CHR_STRING_TERMINATOR											'\0'
	// Functional mml String
#define STR_CASTCSENDER_MMLPATH										"usr/bin/mml"
#define STR_CASTCSENDER_MMLTIMEOUTPARAM						"-w"
#define STR_CASTCSENDER_MMLTIMEOUTARG							"0"
#define STR_CASTCSENDER_MMLCPCOMMAND							"-cp"
// Message String
#define STR_CASTCSENDER_FAILMASTER								"Fail to select on master file descriptor."
#define STR_CASTSENDER_FAIL_LAUNCH_MML						"Fail to launch mml command."
#define STR_CASTCSENDER_FAILFORK 									"Fail to execute fork."
#define STR_CASTCSENDER_FAILPTY 									"Fail to execute openpty."
#define STR_CASTCSENDER_FAILLOGIN									"Fail to login in the tty."
#define STR_CASTCSENDER_TIMEOUT										"Timeout occur. MML is hung."
#define STR_CASTCSENDER_KILLMML										"Unable to kill child mml process."
#define STR_CASTCSENDER_NULLPOINTER 							"One of input pointer arguments is null."
#define STR_CASTSENDER_TTYNAME 										"TTY NAME"
#define STR_CASTPARSER_GENERICERROR								"Parsing error"
#define STR_CASTPARSER_BUFFER											"Buffer received:"
#define STR_CASTPARSER_VOIDBUFFER									"Buffer is void."
#define STR_CASTPARSER_PARSINGLINE								"Parsing line:"
// Global definition
#define STR_CFACILITY_PARSERCLASS									"TCastcParser::%s - %s", __FUNCTION__
#define STR_CFACILITY_SENDERCLASS									"TCastcSender::%s - %s", __FUNCTION__
#define STR_CFACILITY_OPENGROUP										" ["
#define STR_CFACILITY_CLOSEGROUP									"]"
#define STR_CFACILITY_SPACE												" "

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// CLASS: TCastcParser //////////////////////////////////////////////
class TCastcParser
{
	public:
	////////////////////////////////////////////// Costructor/Distructor
		TCastcParser();
		virtual ~TCastcParser();
	////////////////////////////////////////////// Public method
		// This method return true if the castc is correctly executed; false otherwise.
		// The "strBuffer" is mml castc response.
		bool IsCastcOrdered(const std::string &strBuffer);
	////////////////////////////////////////////// Private Enumerative
	private:
		// Response inline enumerative
		enum enmParserStatus{
			eStatusOrdered = 0,
			eStatusExecute,
			eStatusError,
		};
		// Collection output parser structure
		struct strctParserResponse{
			const char *szFieldName;
			enmParserStatus enmStatus;
		};
	////////////////////////////////////////////// Private method
	private:
		// Get the first line and remove unprintable chars
		bool StringToken(const std::string &szBuffer, std::string *pstrLine, size_t *piPointer);
		// Remove unprintable char
		void RemoveUnprintableChars(std::string *pStr);
		// Check if in the line there is the response and associate it to an enumerative
		bool GetLineStatus(const std::string &szBuffer, enmParserStatus *penmStatus);
		// Check the correct execution of command castc in MCP configuratione to OG
		bool MultiLineParser(const std::string &szBuffer, size_t *piPointer, enmParserStatus *penmStatus);
	////////////////////////////////////////////// Private Attributes
	private:
		// Contains all the possible cp response and status
		const static strctParserResponse m_strctResponse[];
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// CLASS: TCastcSender //////////////////////////////////////////////
class TCastcSender
{
	public:
	////////////////////////////////////////////// Costructor/Distructor
		TCastcSender();
		virtual ~TCastcSender();
	////////////////////////////////////////////// Public method
		// Launch mml "strCmd" command and capture the output in pStrOut and the execution status in piError.
		// If the method return true, the mml command is executed wit success. The method return false
		// otherwise.
		bool LaunchMmlCmd(const std::string &strCmd, const std::string &strCP, std::string *pStrOut, int *piError);
	////////////////////////////////////////////// Private method
	private:
		// Read from terminal and append to "pStrOt"
		void ReadAndAppend(int ptyfd, std::string *pStrOut);
		// Read from terminal while the child "pifChild" is live.
		bool ReadFromTerminal(int ptyfd, pid_t pidChild, std::string *pStrOut, int *piStatus);
};
