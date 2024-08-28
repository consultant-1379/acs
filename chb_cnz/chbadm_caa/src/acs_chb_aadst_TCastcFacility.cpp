#include <unistd.h>
#include <pty.h>
#include <utmp.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "acs_chb_tra.h"
#include "acs_chb_aadst_TCastcFacility.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// CLASS: TCastcParser //////////////////////////////////////////////

// Inline Parser castc response (static type)
const TCastcParser::strctParserResponse TCastcParser::m_strctResponse[] = {
	{STR_AADTS_CASTC_LINE_ORDERED, eStatusOrdered},
	{STR_AADTS_CASTC_LINE_EXECUTED, eStatusExecute},
	{STR_AADTS_CASTC_LINE_NOTACCEPTED, eStatusError}
};

TCastcParser::TCastcParser()
{
}

TCastcParser::~TCastcParser()
{
}

bool TCastcParser::IsCastcOrdered(const std::string &strBuffer)
{
	bool bRet;
	bool bNext;
	size_t iPointer;
	enmParserStatus enmStatus;
	int iLen;
	std::string strLine;
	std::string strMsg;
	// Initialization
	bRet = false;
	bNext = false;
	iPointer = 0;
	enmStatus = eStatusError;
	iLen = strBuffer.length();
	// Check for size
	if(iLen > 0){
		// Log the buffer to parse
		DEBUG(1, STR_CFACILITY_PARSERCLASS, STR_CASTPARSER_BUFFER);
		DEBUG(1, STR_CFACILITY_PARSERCLASS, strBuffer.c_str());
		// Get first line of the result buffer
		//pToken = strtok_r(szBuffer, STR_AADTS_CASTC_LINE_SEPARATOR, &pPointer);
		bNext = StringToken(strBuffer, &strLine, &iPointer);
		// Check if the result is inline (e.g. first line is ORDERED, EXECUTE, NOT ACCEPT, ...)
		if(GetLineStatus(strLine, &enmStatus) == true){
			// The result is inline
			if(enmStatus != eStatusError){
				// SUCCESS. Set return flag to true
				bRet = true;
			}else{
				// Error in response. Prepare the string message
				strMsg.assign(STR_CASTPARSER_GENERICERROR);
				strMsg.append(STR_CFACILITY_OPENGROUP);
				strMsg.append(strLine);
				strMsg.append(STR_CFACILITY_CLOSEGROUP);
				// Send to debug
				DEBUG(1, STR_CFACILITY_PARSERCLASS, strMsg.c_str());
			}
		}else if(bNext == true){
			// Parsin miltiline response
			bRet = MultiLineParser(strBuffer, &iPointer, &enmStatus);
		}
	}
	// Exit fronm method
	return(bRet);
}

////////////////////////////////////////////// Private method
bool TCastcParser::StringToken(const std::string &szBuffer, std::string *pstrLine, size_t *piPointer)
{
	bool bRet;
	size_t iLen;
	size_t iNewPos;
	std::string strMsg;
	// Initialization
	bRet = false;
	iNewPos = 0;
	iLen = szBuffer.length();
	// Check for output arguments
	if((pstrLine != NULL) && (piPointer != NULL)){
		// Clear the output string
		pstrLine->clear();
		// Remove delimitator char from the left of the string
		*piPointer = szBuffer.find_first_not_of(STR_AADTS_CASTC_LINE_SEPARATOR, *piPointer);
		// Check for delimitator found
		if (*piPointer != std::string::npos){
			// Get the first delimitator char
			iNewPos = szBuffer.find_first_of(STR_AADTS_CASTC_LINE_SEPARATOR, *piPointer);
			// Check for delimitator found
			if (iNewPos != std::string::npos){
				// Set index to next character (skip delimitator)
				++iNewPos;
				// Check for pointer overflow
				if(iNewPos <= iLen){
					// Retrive substring (the line)
					*pstrLine = szBuffer.substr(*piPointer, iNewPos - *piPointer - 1);
					// Update the pointer
					*piPointer = iNewPos;
					// Set exit flag to success
					bRet = true;
				}
			}else if (*piPointer < iLen){
				// Return all the remaining string
				*pstrLine = szBuffer.substr(*piPointer, std::string::npos);
					// Update the pointer
					*piPointer = std::string::npos;
			}
		}
	}
	// Check for error
	if(bRet == true){
		// Remove unprintable char from the string
		RemoveUnprintableChars(pstrLine);
		// Debug token string
		strMsg.assign(STR_CASTPARSER_PARSINGLINE);
		strMsg.append(STR_CFACILITY_OPENGROUP);
		strMsg.append(*pstrLine);
		strMsg.append(STR_CFACILITY_CLOSEGROUP);
		DEBUG(1, STR_CFACILITY_PARSERCLASS, strMsg.c_str());
	}
	// Exit from method
	return (bRet);
}

void TCastcParser::RemoveUnprintableChars(std::string *pStr)
{
	std::string::iterator it;
	// Initialize iterator to begin of the list
	it = pStr->begin();
	while(it != pStr->end()){
		if(isprint(*it) == false){
			// Not printable: remove it
			it = pStr->erase(it);
		}else{
			// Next element
			++it;
		}
	}
}

bool TCastcParser::GetLineStatus(const std::string &szBuffer, enmParserStatus *penmStatus)
{
	bool bFound;
	int iCount;
	int iNumEntry;
	// Initialization
	bFound = false;
	iCount = 0;
	iNumEntry = sizeof(TCastcParser::m_strctResponse)/sizeof(TCastcParser::strctParserResponse);
	// Check for output argument
	if((szBuffer.length() > 0) && (penmStatus != NULL)){
		// Init argument
		*penmStatus = eStatusError;
		// Check if the response is in m_lstResponseStatus
		while ((iCount < iNumEntry) && (bFound == false)){
			// Check for array presence
			if(szBuffer.compare(TCastcParser::m_strctResponse[iCount].szFieldName) == 0){
				// Found! Set status to appropriate enum
				*penmStatus = TCastcParser::m_strctResponse[iCount].enmStatus;
				// Set flag to exit
				bFound = true;
			}
			// Next entry
			++iCount;
		} // while
	}
	// Exit from method
	return bFound;
}

bool TCastcParser::MultiLineParser(const std::string &szBuffer, size_t *piPointer, enmParserStatus *penmStatus)
{
	bool bNext;
	bool bExit;
	bool bRet;
	std::string strMsg;
	std::string strLine;
	// Initialization
	bNext = false;
	bRet = false;
	bExit = false;
	*penmStatus = eStatusError;
		// The result is multiline (e.g. ACT tp212ap1a AD-66 TIME ....). Second line contains status
	bNext = StringToken(szBuffer, &strLine, piPointer);
	// For all line in the parsing buffer:
	while((bNext == true) && (bExit == false)){
		// Check for error
		if(GetLineStatus(strLine, penmStatus) == true){
			// Check for error
			if(*penmStatus == eStatusError){
				// Found an error. Prepare the debug string
				strMsg.assign(STR_CASTPARSER_GENERICERROR);
				strMsg.append(STR_CFACILITY_OPENGROUP);
				strMsg.append(strLine);
				strMsg.append(STR_CFACILITY_CLOSEGROUP);
				// Send to debug
				DEBUG(1, STR_CFACILITY_PARSERCLASS, strMsg.c_str());
				// Set exit flag
				bExit = true;
			}else{
				// Next line after ORDERED, EXECUDED or NOT ACCEPTED: should be a header (e.g. ACT tp212ap1a    AD-66 ....)
				bNext = StringToken(szBuffer, &strLine, piPointer);
				// Search "TIME OUT". If "TIME OUT" exist, stop the parsering.
				if(strLine.compare(STR_AADTS_CASTC_LINE_TIMEOUT) == 0){
					// Foce end string.
					bNext = false;
				}else if(bNext == true){
					// Skip line so the next line contains CP status (ORDERED, EXECUTE, ecc.)
					bNext = StringToken(szBuffer, &strLine, piPointer);
				}
			}
		}else{
			// Exit for unexpected entry in castc response
			bExit = true;
			//Prepare the debug string
			strMsg.assign(STR_CASTPARSER_GENERICERROR);
			strMsg.append(STR_CFACILITY_OPENGROUP);
			strMsg.append(strLine);
			strMsg.append(STR_CFACILITY_CLOSEGROUP);
			// Send to debug
			DEBUG(1, STR_CFACILITY_PARSERCLASS, strMsg.c_str());
		}
	} // while
	// Check for latest line
	if((bNext == false) && (bExit == false) && (*penmStatus != eStatusError)){
		// Set exit flag
		bRet = true;
	}
	// Exit from method
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// CLASS: TCastcSender //////////////////////////////////////////////
TCastcSender::TCastcSender()
{
}

TCastcSender::~TCastcSender()
{
}

bool TCastcSender::LaunchMmlCmd(const std::string &strCmd, const std::string &strCP, std::string *pStrOut, int *piError)
{
	bool bRet;
	int iFlag;
	int ptyfd;
	int ttyfd;
	char *szName;
	pid_t pidFork;
	std::string strMsg;
	// Initialization
	bRet = false;
	iFlag = -1;
	ptyfd = -1;
	ttyfd = -1;
	pidFork = 0;
	// Check for error argument
	if((piError != NULL) && (pStrOut != NULL)){
		// Initialize the error status
		*piError = 0;
		// Clear the string buffer
		pStrOut->clear();
		// Open the terminals pair
		iFlag = openpty(&ptyfd, &ttyfd, NULL, NULL, NULL);
		// Check for error on terminal open
		if (iFlag >= 0){
			// Get terminal name
			szName = ttyname(ttyfd);
			//Prepare the debug string
			strMsg.assign(STR_CASTSENDER_TTYNAME);
			strMsg.append(STR_CFACILITY_OPENGROUP);
			strMsg.append(szName);
			strMsg.append(STR_CFACILITY_CLOSEGROUP);
			// Send to debug
			DEBUG(1, STR_CFACILITY_SENDERCLASS, strMsg.c_str());
			// Terminals pair are opened.
			pidFork = fork();
			// Switch for child, father, error
			if (pidFork == 0){
				// Child. Close the master
				close(ptyfd);
				// Prepare the slave terminal
				if (login_tty(ttyfd) > -1){
					// Prepare debug string
					strMsg.assign(STR_CASTCSENDER_MMLPATH);
					strMsg.append(STR_CFACILITY_SPACE);
					strMsg.append(STR_CASTCSENDER_MMLTIMEOUTPARAM);
					strMsg.append(STR_CFACILITY_SPACE);
					strMsg.append(STR_CASTCSENDER_MMLTIMEOUTARG);
					strMsg.append(STR_CFACILITY_SPACE);
					// Switch for command type (on single CP or OPGROUP)
					if(strCP.empty() == true){
						// Append the command to debug string
						strMsg.append(strCmd);
						// Debug message to OPGROUP launch
						DEBUG(1, STR_CFACILITY_SENDERCLASS, strMsg.c_str());
						// Exec the command for opgroup
						execl(STR_CASTCSENDER_MMLPATH, STR_CASTCSENDER_MMLPATH, STR_CASTCSENDER_MMLTIMEOUTPARAM, 
						      STR_CASTCSENDER_MMLTIMEOUTARG, strCmd.c_str(), NULL);
					}else{
						// Append the -cp CPn to debug string
						strMsg.append(STR_CASTCSENDER_MMLCPCOMMAND);
						strMsg.append(STR_CFACILITY_SPACE);
						strMsg.append(strCP);
						strMsg.append(STR_CFACILITY_SPACE);
						strMsg.append(strCmd);
						// Debug message to CP launch
						DEBUG(1, STR_CFACILITY_SENDERCLASS, strMsg.c_str());
						// Exec the command for single CP
						execl(STR_CASTCSENDER_MMLPATH, STR_CASTCSENDER_MMLPATH, STR_CASTCSENDER_MMLTIMEOUTPARAM, 
						      STR_CASTCSENDER_MMLTIMEOUTARG, STR_CASTCSENDER_MMLCPCOMMAND, strCP.c_str(), strCmd.c_str(), NULL);
					}
				}else{
					// Error. Unable to login in tty
					DEBUG(1, STR_CFACILITY_SENDERCLASS, STR_CASTCSENDER_FAILLOGIN);
				}
				// The "execl" return only if there is an error
				DEBUG(1, STR_CFACILITY_SENDERCLASS, STR_CASTSENDER_FAIL_LAUNCH_MML);
				// Force the child exit...
				exit(INT_CASTCSENDER_EXEC_FAIL);
			}else if(pidFork > 0){
				// Father. Close slave terminal
				close(ttyfd);
				// Read from terminal
				bRet = ReadFromTerminal(ptyfd, pidFork, pStrOut, piError);
				// Close master file descriptor
				close(ptyfd);
			}else{
				// Error in fork
				DEBUG(1,STR_CFACILITY_SENDERCLASS, STR_CASTCSENDER_FAILFORK);
			}
		}else{
			// Unable to open terminal
			DEBUG(1,STR_CFACILITY_SENDERCLASS, STR_CASTCSENDER_FAILPTY);
		}
	}else{
		// One of the argument is a invalid pointer
		DEBUG(1,STR_CFACILITY_SENDERCLASS, STR_CASTCSENDER_NULLPOINTER);
	}
	// Exit from method
	return (bRet);
}

////////////////////////////////////////////// Private methods
void TCastcSender::ReadAndAppend(int ptyfd, std::string *pStrOut)
{
	int iReaded;
	char szInternalBuf[INT_CASTCSENDER_CACHE_READ];
	// Initialization
	iReaded = 0;
	szInternalBuf[0] = CHR_STRING_TERMINATOR;
	// Master file descriptor signaled. Read.
	iReaded = read(ptyfd, szInternalBuf, INT_CASTCSENDER_CACHE_READ - 1);
	// Check for read
	if(iReaded > 0){
		// Set terminator tag to the end of the buffer
		szInternalBuf[iReaded] = CHR_STRING_TERMINATOR;
		// Store the bytes in the external buffer
		pStrOut->append(szInternalBuf);
		// For all character
		while(iReaded == INT_CASTCSENDER_CACHE_READ - 1){
			// Read
			iReaded = read(ptyfd, szInternalBuf, INT_CASTCSENDER_CACHE_READ - 1);
			// Check if there are a caracter in buffer
			if(iReaded > 0){
				// Set terminator tag to the end of the buffer
				szInternalBuf[iReaded] = CHR_STRING_TERMINATOR;
				// Store the bytes in the external buffer
				pStrOut->append(szInternalBuf);
			}
		} // while for iReaded
	}
}

bool TCastcSender::ReadFromTerminal(int ptyfd, pid_t pidChild, std::string *pStrOut, int *piStatus)
{
	bool bRet;
	bool bError;
	int iSelect;
	int iReaded;
	int iStatus;
	int iByteCount;
	pid_t pidTask;
	fd_set setChild;
	struct timeval tv;
	// Initialization
	bRet = false;
	bError = false;
	iSelect = -1;
	iReaded = -1;
	iStatus = -1;
	iByteCount = 0;
	tv.tv_usec = 0;
	tv.tv_sec = INT_CASTCSENDER_MMLSECONDSTIMEOUT;
	// Clear file descriptor
  FD_ZERO(&setChild);
	// Check for error argument
	if(piStatus != NULL){
		// Get the pid status
		pidTask = waitpid(pidChild, &iStatus, WNOHANG);
		// Loop for child exit...
		while ((pidTask != pidChild) && (bError == false)){
			// Enable master file descriptor for signal
			FD_SET(ptyfd, &setChild);
			// Wait for child signal...
			iSelect = select(ptyfd+1, &setChild, NULL, NULL, &tv);
			// Check for error
			if(iSelect > 0){
				// Check for signaled file descriptor
				if (FD_ISSET(ptyfd, &setChild)){
					// Read the contents
					ReadAndAppend(ptyfd, pStrOut);
				}
			}else if(iSelect == 0){
				// Timeout
				DEBUG(1, STR_CFACILITY_SENDERCLASS, STR_CASTCSENDER_TIMEOUT);
				// Set exit flag
				bError = true;
				// Kill the process
				if(kill(pidChild, SIGKILL)!=0){
					// Verify if the error is critical
					if(errno != ESRCH){
						// Real kill error
						DEBUG(1, STR_CFACILITY_SENDERCLASS, STR_CASTCSENDER_KILLMML);
					}
				}
			}else{
				// Error to select on a file descriptor
				DEBUG(1, STR_CFACILITY_SENDERCLASS, STR_CASTCSENDER_FAILMASTER);
				// Set flag to exit
				bError = true;
			}
			// Get the pid status
			pidTask = waitpid(pidChild, &iStatus, WNOHANG);
		}	// Main while
		// Check for error
		if((bError ==false) && (iStatus == 0) && (pidTask > 0) &&  (iSelect > 0)){
			// This is the success
			bRet = true;
		}
		// Set mml error status
		*piStatus = WEXITSTATUS(iStatus);
	}else{
		// Invalid argument pointer
		DEBUG(1,STR_CFACILITY_SENDERCLASS, STR_CASTCSENDER_NULLPOINTER);
	}
	// Exit from method
	return (bRet);
}
