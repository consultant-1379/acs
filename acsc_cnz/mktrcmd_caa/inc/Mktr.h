
#ifndef MKTR_H_
#define MKTR_H_

#include <string>
#include <fstream>
#include "MktrCommonDefs.h"
#include "MktrDate.h"
#include "MktrFile.h"
#include "MktrConfig.h"
#include "APGInfo.h"
#include <ace/ACE.h>


class Mktr
{
	friend class MktrSignalHandler;

public:
    void fetchFileLogs(const MktrCommonDefs::File* fileTable);
    void fetchCmdLogs(const MktrCommonDefs::Command* cmdTable);
	void fetchCltLogs(const MktrCommonDefs::Collector* cltTable);
    void fetchCoreDumpAtTime(const MktrDate& date, bool bTime);
    void fetchMemoryDumpAtTime(const MktrDate& date, bool bTime);
    int createOutZippedArchive();
    void addLogFileToArchiveAndDeleteIt();
    void stop(int signum = -1);
    bool isStopped() {return stopped; }
    bool isAvailableForWork() { return isAvailForWork; }
    std::string get_unavailable_reason() { return not_available_reason; }
    int runOnOtherNode(std::string commandLine);
    void setVerbose(bool bVerbose);
    MktrCommonDefs::NodeType NodeState() { return apgInfo.nodeStatus(); }
    std::string getMktrLogFilePath() { return this->mktrLogFile; }
    FILE * getLogStream() { return mktrLog; }
	// Init capability and propagates it to childs of this process
	void initCapability();

	// static methods
    static void getHostname(std::string &outputHostname);
    static int getPartnerHostName(std::string & outputPartnerHostname);
	static Mktr * createMktrCmdExecutor(const std::string& path, const std::string& file);
	static Mktr * getMktrCmdExecutor() { return instance; };
	static void releaseMktrCmdExecutor();
	static void setAlarm(int nSeconds);
    static bool isAlarmRaised() { return Mktr::alarm_raised; }

private:

	static Mktr *instance;

	Mktr(const std::string& path, const std::string& file);
	Mktr(const Mktr &obj) {ACE_UNUSED_ARG (obj);};

	Mktr & operator=(const Mktr & obj) {ACE_UNUSED_ARG (obj); return *this; };

	~Mktr();

    bool isSuitableToRun(bool match_ok_when_info_is_not_available, const unsigned int apType, const unsigned int apgType, const unsigned int apgShelfArchitectureType, const unsigned int apzType, const MktrCommonDefs::NodeType nodeType, const unsigned int gepType = MktrCommonDefs::APG_HW_GEP_ALL);
    void executeCmd(const MktrCommonDefs::Command& cmd);
	void executeClt(const MktrCommonDefs::Collector& clt);
    void fetchFile(const MktrCommonDefs::File& file);
    bool fetchFile(MktrFile& srcfile, const char* dstFile, bool & no_files_to_fetch);
    int replaceHostName(std::string& file);


    // Add "logFile" to the MKTR tar archive.
    // Note that there are three cases for the logFile parameter:
    // 1) logFile is a FILE
    // 2) logFile is a DIRECTORY
    // 3) logFile is an expression containing wildcards ( representing tipically a set of files; for example: *.txt)
    int archieve(const std::string& logFile, const bool isFile = true);


    // Creates a zipped archive named "archiveFile", using TAR utility. The archive will contain "logFile". There are
    // three cases for "logFile" (see description of "archieve" method)
    int addToArchieve(const std::string& logFile, const std::string& archiveFile, const bool isFile = true);

    void printStreamOut(const std::string& strFetching, bool bError = false, bool bPrintNewline = true);

	APGInfo apgInfo;
    bool stopped;
    FILE *mktrLog;
    std::string mktrHome;
    std::string mktrLogFile;
  	std::string folderSwitch;
    std::string zipFile;
    bool isAvailForWork;
    std::string not_available_reason;
    std::string mktrArchiveFileName;
    std::string mktrArchiveFilePathName;
    bool bVerboseOption;

    static bool alarm_raised;
};

#endif /* MKTR_H_ */
