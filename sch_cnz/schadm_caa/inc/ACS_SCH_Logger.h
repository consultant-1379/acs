/*
 * ACS_SCH_Logger.h
 *
 *  Created on: Mar 12, 2012
 *      Author: estevol
 */

#ifndef ACS_SCH_LOGGER_H_
#define ACS_SCH_LOGGER_H_

#include "ACS_TRA_Logging.h"
#include  <syslog.h>
#include "iostream"
#include <ACS_APGCC_Util.H>
#include <ACS_APGCC_CommonLib.h>
#include <ace/OS_NS_dirent.h>
#include <ACS_TRA_trace.h>
#include <ACS_TRA_Logging.h>
#include <ostream>

#define SCH_makeOrigin() ACS_SCH_Origin(__FILE__,__LINE__)
class ACS_SCH_Logger;

class ACS_SCH_Origin
{
	public:
  ACS_SCH_Origin(const std::string& aFileName, ACE_UINT32 aLineNum) :
  m_FileName(aFileName), m_LineNum(aLineNum)
 {}

  friend ostream& operator<<(ostream& aStream, const ACS_SCH_Origin& anOrigin)
  {
   aStream << "\"" << anOrigin.fileName() << "\", line " << anOrigin.lineNum() ;
   return aStream;

  }

  std::string fileName() const { return m_FileName; }
  ACE_UINT32 lineNum() const { return m_LineNum; }
	private:
 	std::string m_FileName;
	ACE_UINT32 m_LineNum;

};

class ACS_SCH_Logger {

public:
	static ACS_TRA_Logging * getLogInstance ();
	static void closeLogInstance ();
  static bool createTRAObjects();
  static void deleteTRAObjects();
	static bool isTralogInstanceExist();
	static void log(std::string anErrMsg,const ACS_SCH_Origin& anOrigin,ACS_TRA_LogLevel aLogLevel);
private:
	ACS_SCH_Logger();
	virtual ~ACS_SCH_Logger();
	static ACS_TRA_Logging* ACS_SCH_logging;
};

#endif /* ACS_SCH_LOGGER_H_ */
