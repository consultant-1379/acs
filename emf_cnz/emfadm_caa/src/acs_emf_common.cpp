/*=================================================================== */
/**
  @file acs_emf_commandhandler.cpp

  Class method implementation for EMF module.

  This module contains the implementation of class declared in
  the EMF Module
  @version 1.0.0

  HISTORY
  This section contains reference to problem report and related
  software correction performed inside this module

  PR           DATE      INITIALS    DESCRIPTION
  -----------------------------------------------------------
  N/A       DD/MM/YYYY     XRAMMAT       Initial Release

                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <stdio.h>
#include <string.h>
#include <ctime>
#include <sys/stat.h>
#include <mntent.h>
#include <ace/ACE.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/OS_NS_dlfcn.h>
#include <ace/Reactor.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ace/Signal.h>
#include <ace/Get_Opt.h>
#include <ace/streams.h>
#include <ace/OS_NS_errno.h>
#include <ace/SString.h>
#include <ace/OS_NS_sys_wait.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_sys_stat.h>
#include <ace/OS_NS_sys_resource.h>
#include <ace/OS_NS_fcntl.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <saAis.h>
#include <acs_apgcc_omhandler.h>
#include <ACS_APGCC_Util.H>
#include <ACS_APGCC_CommonLib.h>
#include "acs_emf_common.h"
#include "acs_emf_tra.h"
#include "acs_emf_TScsiDev.h"
#include "acs_emf_aeh.h"
#include <ace/Guard_T.h>
#include <ACS_DSD_Client.h>
#include <limits.h>
#include <sys/types.h>
using namespace ItsScsiUtilityNamespace;

/* For SIM AP instead of /dev/sr0 use /dev/dvd1 */
#ifdef EMFONSIMAP
/* FOR SIMAP */
ACE_TCHAR cdRecordCmd [100] = "cdrecord -scanbus 2>/dev/null | grep 'CD-ROM' | awk '{print $1}'";
ACE_TCHAR dvdMediaInfoCmd [100] = "dvd+rw-mediainfo ";
ACE_TCHAR cdInfoCmd [100] = "cdinfo ";
ACE_TCHAR mountCmd [100] = "mount -t udf ";
ACE_TCHAR unmountCmd [100] = "umount /data/opt/ap/internal_root/media 1>/dev/null 2>/dev/null";
ACE_TCHAR getDVDMediumTypeCmd [100] = "dvd+rw-mediainfo ";
ACE_TCHAR getCDMediumTypeCmd [100] = "cdinfo ";
ACE_TCHAR createImageForDVDCmd [100] = "mkisofs -input-charset iso8859-1 -allow-limited-size -udf ";
ACE_TCHAR createImageForCDCmd [100] = "mkisofs -joliet-long -input-charset iso8859-1 ";
ACE_TCHAR writeImageOnRewritableMediumCmd [100] = "cdrecord -v -sao dev=";
ACE_TCHAR writeImageOnReadOnlyNewDVDCmd [100] = "growisofs -Z  ";
ACE_TCHAR writeImageOnReadOnlyDVDCmd [100] = "growisofs -M ";
ACE_TCHAR writeImageOnReadOnlyCDCmd [100] = "cdrecord -v -multi -data dev=";
ACE_TCHAR checkForRewrtiableCDCmd [100] = "cdrecord -atip dev=";
ACE_TCHAR formatDVDCmd [100] = "dvd+rw-format -force ";
ACE_TCHAR formatDVDPlusRWCmd [100] = "dvd+rw-format -force ";
ACE_TCHAR formatCDCmd [100] = "cdrecord blank=fast dev=";
#else
/* FOR STP */
ACE_TCHAR cdRecordCmd [100] = "cdrecord -scanbus 2>/dev/null | grep 'CD-ROM' | awk '{print $1}'";
ACE_TCHAR dvdMediaInfoCmd [100] = "dvd+rw-mediainfo ";
ACE_TCHAR cdInfoCmd [100] = "cdinfo ";
ACE_TCHAR softnfsmountCmd [100] = "mount -t nfs -o soft,timeo=5,retry=5";
ACE_TCHAR mountCmd [100] = "mount -t udf ";
ACE_TCHAR unmountCmd [100] = "umount ";
ACE_TCHAR getDVDMediumTypeCmd [100] = "dvd+rw-mediainfo ";
ACE_TCHAR getCDMediumTypeCmd [100] = "cdinfo ";
ACE_TCHAR createImageForDVDCmd [100] = "mkisofs  -input-charset iso8859-1 -allow-limited-size -udf ";
ACE_TCHAR createImageForCDCmd [100] = "mkisofs -joliet-long -input-charset iso8859-1 ";
ACE_TCHAR writeImageOnRewritableMediumCmd [100] = "cdrecord -v -sao dev=";
ACE_TCHAR writeImageOnDVDNewSessionCmd [100] = "growisofs -use-the-force-luke=notray -Z ";
ACE_TCHAR writeImageOnDVDAppendingSessionCmd [100] = "growisofs -use-the-force-luke=notray -M ";
ACE_TCHAR writeImageOnReadOnlyCDCmd [100] = "cdrecord -v -multi -data dev=";
ACE_TCHAR checkForRewrtiableCDCmd [100] = "cdrecord -atip dev=";
ACE_TCHAR formatDVDCmd [100] = "dvd+rw-format -force ";
ACE_TCHAR formatDVDPlusRWCmd [100] = "dvd+rw-format -force ";
ACE_TCHAR formatCDCmd [100] = "cdrecord blank=fast dev=";
ACE_TCHAR formatThumbDriveCmd[100] ="mkfs.ext3 -F";
ACE_TCHAR copyFromMediaCmd[] ="cp -R";


#endif

ACE_TCHAR mediaType[40] = "THUMBDRIVE";
ACE_TCHAR unmountDVDOnPassiveCmd[100] = "umount /media 1>/dev/null 2>/dev/null";
std::string chksumFile = "/emf/emfsrc_sum.sha1";  
std::string chksumFilewithFullPath = "/data/acs/data/emf/emfsrc_sum.sha1";
std::string acsDataDiskPath = "/data/acs/data";
std::string imageFilePath   = "/data/acs/data/emf/";
std::string emfDirPath = "/emf/";
ACE_TCHAR mediaInfoPath[100] = "/cluster/emf_PersistantInfo";
const char * EMF_CLUSTERIP_AP1A  = "169.254.208.1";
const char * EMF_CLUSTERIP_AP1B  = "169.254.208.2";
const char * EMF_CLUSTERIP_AP2A  = "169.254.208.3";
const char * EMF_CLUSTERIP_AP2B  = "169.254.208.4";
ACE_TCHAR g_devicefile[EMF_DEVNAME_MAXLEN] = "/dev/sr0";
acs_emf_operation_status_t stateOfDVDOperation = EMF_MEDIA_OPERATION_IS_IDLE;
//TR HU81933 /dev/sdm changed in /dev/eri_thumbdrive
//ACE_TCHAR devicefileforusb[50] = "/dev/sdm";
ACE_TCHAR devicefileforusb[50] = "/dev/eri_thumbdrive";
ACE_TCHAR copypath[50] = "/data/opt/ap/internal_root/media";
ACE_TCHAR copypathforpassive[50] = "/media";

const char* mode = "r";
const char* space = " ";
const char* slash = "/";
const char* move_err_mesgs_to_null = " 1>/dev/null 2>/dev/null";

static int hwVariant = EMF_RC_ERROR;
static int g_iVersion = EMF_RC_ERROR;
pid_t pid_in_progress = EMF_RC_ERROR;
std::string ACS_EMF_Common::parentDNofEMF = "";
//const char* splitFilename (const string& str);
pid_t popen_with_pid(const char *command, int *infp, int *outfp);

/*===================================================================
   ROUTINE: splitFilename
            extracts file name from full path
=================================================================== */
//const char* splitFilename (const string& str)
//{
//	size_t found;
//	found=str.find_last_of("/\\");
//	return str.substr(found+1).c_str();
//}

/*===================================================================
   ROUTINE: popen_with_pid
            Executes the job in background and returns the PID
            It is equivalent to popen and it pid of job
=================================================================== */
pid_t popen_with_pid(const char *command, int *infp, int *outfp)
{
	int p_stdin[2], p_stdout[2];
	pid_t pid = EMF_RC_ERROR;

	if (pipe(p_stdin) != EMF_RC_OK || pipe(p_stdout) != EMF_RC_OK)
		return EMF_RC_ERROR;

	pid = fork();

	if (pid < 0)
		return pid;
	else if (pid == 0)
	{
		close(p_stdin[POPEN_WRITE]);
		dup2(p_stdin[POPEN_READ], POPEN_READ);
		close(p_stdout[POPEN_READ]);
		dup2(p_stdout[POPEN_WRITE], POPEN_WRITE);
		dup2(p_stdout[POPEN_WRITE],2);

		execl("/bin/sh", "sh", "-c", command, NULL);
		perror("execl");
		exit(1);
	}

	if (infp == NULL)
		close(p_stdin[POPEN_WRITE]);
	else
		*infp = p_stdin[POPEN_WRITE];

	if (outfp == NULL)
		close(p_stdout[POPEN_READ]);
	else
		*outfp = p_stdout[POPEN_READ];

	return pid;
}

/*===================================================================
   ROUTINE: setDeviceName
=================================================================== */
void ACS_EMF_Common::setDeviceName(const std::string &strDevName)
{
	// Check for error
	if(strDevName.length() < std::string::size_type(EMF_DEVNAME_MAXLEN)){
		// Copy a string in the global variable
		sprintf(g_devicefile,"/%s",strDevName.c_str());
	}else{
		// Error buffer overflow
		ERROR(1,"%s","ACS_EMF_Common::setDeviceName - Buffer overflow.");
	}
}

const char * ACS_EMF_Common::getDeviceName(){
	return g_devicefile;
}

/*===================================================================
   ROUTINE: CheckActiveNode
=================================================================== */
//bool ACS_EMF_Common::CheckActiveNode( )
//{
//	FILE* fp;
//	std::string cmd("cmwea tipcaddress-get | cut -d , -f 3");
//	char path[10];
//	fp = popen(cmd.c_str(), "r");
//      if(fp != NULL){  
//		while (fgets(path, 10, fp) != NULL)
//		{
//			cout<< "The NodeId is = "<<path<<endl;
//		}
//      }
//      else
//              return false;
//	std::string myNodeStr("1");
//	myNodeStr.at(0)= path[0];
//	char path1[10];
//	std::string myStr1("immlist -a saAmfSISUHAState \"safSISU=safSu=SC-");
//	std::string myStr3("\\,safSg=2N\\,safApp=OpenSAF,safSi=SC-2N,safApp=OpenSAF\" | cut -d = -f2");
//	cmd = myStr1+myNodeStr+myStr3;
//
//	pclose(fp);
//	fp = popen(cmd.c_str(), "r");
//      if (fp != NULL){
//		while (fgets(path1,10, fp) != NULL)
//		{
//			//cout<< "The NodeState is = "<<path1<<endl;
//		}
//      }
//      else
//             return false;
//	bool myIsActiveNode;
//	if(path1[0] == '1')
//		myIsActiveNode = true;
//	else
//		myIsActiveNode = false;
//
//	pclose(fp);
//
//	return myIsActiveNode;
//
//}

/*===================================================================
   ROUTINE: CheckFile
=================================================================== */
int ACS_EMF_Common::CheckFile(const ACE_TCHAR* lpszFile)
{
	ACE_stat fileStat;
	ACE_INT32 s32Type = 0 ;
	if ( ACE_OS::stat(lpszFile,&fileStat) != EMF_RC_OK ){
		int stat_error = errno;
		DEBUG(1,"ACS_EMF_Common::CheckFile - Stat return an error[ %i ]", stat_error);
		if(stat_error == ESTALE || stat_error == EIO)
			s32Type = 3;	//access to media via NFS sharing failed!
		else
			s32Type = EMF_RC_ERROR;
	}
	DEBUG(1,"ACS_EMF_Common::CheckFile - Value of s32Type in CheckFile is %d",s32Type);
	if( s32Type != EMF_RC_ERROR && s32Type != 3 )
	{
		if ( S_ISDIR(fileStat.st_mode) )
			s32Type = 2; //Directory
		else if( S_ISREG(fileStat.st_mode) )
			s32Type = 1; //File
		else
			s32Type = 0; //Others
	}
	return s32Type;
}

/*===================================================================
   ROUTINE: CheckDirForWriteAccess
=================================================================== */
int ACS_EMF_Common::CheckDirForWriteAccess(const ACE_TCHAR* lpszFile)
{
	ACE_INT32 s32Type = EMF_RC_OK ;
	s32Type =  access(lpszFile,W_OK);
	return s32Type;
}

/*===================================================================
   ROUTINE: isEmptyDir
=================================================================== */
ACE_INT32 ACS_EMF_Common::isEmptyDir(const ACE_TCHAR* dname)
{
	ACE_UINT32 rCode=TRUE;
	//dirent* d;
	DIR* dir = opendir(dname);
	if (dir){
		ACE_UINT32 n=0;
		while(readdir(dir) != NULL) n++;
		closedir(dir);
		if (n > 2) //here 2 is  referring to ., .. folders
			rCode=FALSE;
	}
	return rCode;
}

/*===================================================================
   ROUTINE: GetDateTimeString
=================================================================== */
const ACE_TCHAR* ACS_EMF_Common::GetDateTimeString()
{
	static ACE_TCHAR szDateTime[32] = ACE_TEXT("");

	time_t LocalTime;
	struct tm* time1;
	ACE_OS::time(&LocalTime);
	time1 = ACE_OS::localtime(&LocalTime);
	ACE_OS::sprintf(szDateTime,
			ACE_TEXT("%04d-%02d-%02d  %02d:%02d:%02d"),
			time1->tm_year+1900,
			(time1->tm_mon+1)%13,
			time1->tm_mday,
			time1->tm_hour,
			time1->tm_min,
			time1->tm_sec);

	return szDateTime;
}

/*===================================================================
   ROUTINE: GetDateTimeWithoutOffset
=================================================================== */
const ACE_TCHAR* ACS_EMF_Common::GetDateTimeWithoutOffset()
{
	static ACE_TCHAR szDateTime[32] = ACE_TEXT("");

	time_t LocalTime;
	struct tm* time1;
	ACE_OS::time(&LocalTime);
	time1 = ACE_OS::localtime(&LocalTime);
	ACE_OS::sprintf(szDateTime,
			ACE_TEXT("%04d-%02d-%02dT%02d:%02d:%02d"),
			time1->tm_year+1900,
			(time1->tm_mon+1)%13,
			time1->tm_mday,
			time1->tm_hour,
			time1->tm_min,
			time1->tm_sec);

	return szDateTime;
}

/*===================================================================
   ROUTINE: GetDateTimeString
=================================================================== */
const ACE_TCHAR* ACS_EMF_Common::GetDateTimeString(time_t lpFileTime)
{
	static ACE_TCHAR szDateTime[32] = ACE_TEXT("");

	struct tm* time1 = NULL ;
	ACE_OS::time(&lpFileTime);
	ACE_OS::localtime_r(&lpFileTime,time1);
        if(time1 != NULL){
	    ACE_OS::sprintf(szDateTime,
			    ACE_TEXT("%04d-%02d-%02d  %02d:%02d:%02d"),
			    time1->tm_year+1900,
			    time1->tm_mon,
			    time1->tm_mday,
			    time1->tm_hour,
			    time1->tm_min,
			    time1->tm_sec);
        }
	return szDateTime;
}

/*===================================================================
   ROUTINE: GetDateTimeString
=================================================================== */
const ACE_TCHAR* ACS_EMF_Common::GetDateTimeString(const long nTime,
		const bool bConvToLocalTime)
{
	static ACE_TCHAR szDateTime[32] = ACE_TEXT("");

	time_t tTime = (nTime > 0 ? (time_t)nTime : time(NULL));
	struct tm* pTm = (bConvToLocalTime ? localtime(&tTime) : gmtime(&tTime));
	if (pTm)
	{
		ACE_OS::sprintf(szDateTime,
				ACE_TEXT("%04d-%02d-%02d %02d:%02d:%02d"),
				pTm->tm_year+1900, pTm->tm_mon+1, pTm->tm_mday,
				pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
	}
	else
		ACE_OS::strcpy(szDateTime, ACE_TEXT("-"));
	return szDateTime;
}

/*===================================================================
   ROUTINE: GetEMFImageFilePath
=================================================================== */
//void ACS_EMF_Common::GetEMFImageFilePath(ACE_TCHAR* lpszPath)
//{
//	if (CheckFile(ACE_TEXT("/opt/")) == 2)
//		ACE_OS::strcpy(lpszPath, ACE_TEXT("/opt/AP/ACS/data/EMF"));
//}

/*===================================================================
   ROUTINE: GetEMFLocalLogFilePath
=================================================================== */
//void ACS_EMF_Common::GetEMFLocalLogFilePath(ACE_TCHAR* lpszPath)
//{
//	if (CheckFile(ACE_TEXT("/opt/")) == 2)
//		ACE_OS::strcpy(lpszPath, ACE_TEXT("/opt/AP/ACS/logs/EMF"));
//}

/*===================================================================
   ROUTINE: GetEMFLogFilePath
=================================================================== */
//void ACS_EMF_Common::GetEMFLogFilePath(ACE_TCHAR* lpszPath)
//{
//	if (CheckFile(ACE_TEXT("/opt/")) == 2)
//		ACE_OS::strcpy(lpszPath, ACE_TEXT("/opt/AP/ACS/ACS_LOGS/EMF"));
//}

/*===================================================================
   ROUTINE: GetHWVersion
=================================================================== */
#if 0
int ACS_EMF_Common::GetHWVersion( )
{
	int iLocVersion;
	ACE_INT32 dwLen;
	ACE_TCHAR szVersion[ACS_EMF_APGCC_HWVERSION];
	ACS_APGCC_CommonLib obj;
	// Initialization
	iLocVersion = -1;
	dwLen = ACS_EMF_APGCC_HWVERSION;
	szVersion[0] = '\0';
	DEBUG(1, "%s", "ACS_EMF_Common::GetHWVersion - Entering");
	// Check if ACS_APGCC_CommonLib::GetHWVersion has already been called
	if (g_iVersion == EMF_RC_ERROR){
		// Get the hardware info
		iLocVersion = obj.GetHWVersion(szVersion,dwLen);
		// Check for error
		if(iLocVersion == ACS_APGCC_HWVER_SUCCESS){
			// No error found. Switch on hardware version
			if(ACE_OS::strcmp(szVersion, ACS_APGCC_HWVER_APG43_GEP1_STRING) == 0){
				// Set Version APG43
				g_iVersion = ACS_EMF_APGCC_APG43;
				//  Set Variant GEP1
				hwVariant = ACS_EMF_APGCC_GEP1;
				DEBUG(1, "%s", "ACS_EMF_Common::GetHWVersion - HWVersion is APG43 - GEP1");
			}else if(ACE_OS::strcmp(szVersion, ACS_APGCC_HWVER_APG43_GEP2_STRING) == 0){
				// Set Version APG43
				g_iVersion = ACS_EMF_APGCC_APG43;
				//  Set Variant GEP2
				hwVariant = ACS_EMF_APGCC_GEP2;
				DEBUG(1, "%s", "ACS_EMF_Common::GetHWVersion - HWVersion is APG43 - GEP2");
			}else if(ACE_OS::strcmp(szVersion,ACS_APGCC_HWVER_APG43_GEP4_STRING) == 0){
				// Set Version APG43
				g_iVersion = ACS_EMF_APGCC_APG43;
				//  Set Variant GEP4 (same as GEP5; was skipped)
				hwVariant = ACS_EMF_APGCC_GEP4;
				DEBUG(1, "%s", "ACS_EMF_Common::GetHWVersion - HWVersion is APG43 - GEP4");
			}else if(ACE_OS::strcmp(szVersion,ACS_APGCC_HWVER_APG43_GEP5_STRING) == 0){
				// Set Version APG43
				g_iVersion = ACS_EMF_APGCC_APG43;
				//  Set Variant GEP5 				
				hwVariant = ACS_EMF_APGCC_GEP5;
				DEBUG(1, "%s", "ACS_EMF_Common::GetHWVersion - HWVersion is APG43 - GEP5");
			}else{
				// Unsupported hardware version
				DEBUG(1,"ACS_EMF_Common::GetHWVersion - Unsupported hardware version [%s]", szVersion);
			}
		}else{
			// APGCC return error. Debug it.
			DEBUG(1,"ACS_EMF_Common::GetHWVersion - GetHWVersion of APGCC return a error [%i]", iLocVersion);
		}
	}
	DEBUG(1, "%s", "ACS_EMF_Common::GetHWVersion - Leaving");
	// Exit from method
	return g_iVersion;
}//End of GetHWVersion
#endif

/*===================================================================
   ROUTINE: GetHWVersion
=================================================================== */
//Rewritten to use the latest api - GetHwInfo
int ACS_EMF_Common::GetHWVersion( )
{
	DEBUG(1, "%s", "ACS_EMF_Common::GetHWVersion - Entering");
	// Check if ACS_APGCC_CommonLib::GetHwInfo has already been called
	if (g_iVersion == EMF_RC_ERROR){
		ACS_APGCC_CommonLib apgccLibObj;
		ACS_APGCC_HWINFO hwInfo;
		ACS_APGCC_HWINFO_RESULT hwInfoRes;
		//ACE_TCHAR szVersion[ACS_EMF_APGCC_HWVERSION] = {'\0'};

		apgccLibObj.GetHwInfo( &hwInfo, &hwInfoRes, ACS_APGCC_GET_HWVERSION );

		if(hwInfoRes.hwVersionResult == ACS_APGCC_HWINFO_SUCCESS){
			switch(hwInfo.hwVersion){
				case ACS_APGCC_HWVER_GEP1:
					//strncpy(szVersion,ACS_APGCC_HWVER_APG43_GEP1_STRING,strlen(ACS_APGCC_HWVER_APG43_GEP1_STRING));
					g_iVersion = ACS_EMF_APGCC_APG43;
					hwVariant = ACS_EMF_APGCC_GEP1;
					DEBUG(1, "%s", "ACS_EMF_Common::GetHWVersion - HWVersion is APG43 - GEP1");
					break;

				case ACS_APGCC_HWVER_GEP2:
					//strncpy(szVersion,ACS_APGCC_HWVER_APG43_GEP2_STRING,strlen(ACS_APGCC_HWVER_APG43_GEP2_STRING));
					g_iVersion = ACS_EMF_APGCC_APG43;
					hwVariant = ACS_EMF_APGCC_GEP2;
					DEBUG(1, "%s", "ACS_EMF_Common::GetHWVersion - HWVersion is APG43 - GEP2");
					break;

				case ACS_APGCC_HWVER_GEP5:
					//strncpy(szVersion,ACS_APGCC_HWVER_APG43_GEP5_STRING,strlen(ACS_APGCC_HWVER_APG43_GEP5_STRING));
					g_iVersion = ACS_EMF_APGCC_APG43;
					hwVariant = ACS_EMF_APGCC_GEP5;
					DEBUG(1, "%s", "ACS_EMF_Common::GetHWVersion - HWVersion is APG43 - GEP5");
					break;

				case ACS_APGCC_HWVER_GEP7:
                                        //strncpy(szVersion,ACS_APGCC_HWVER_APG43_GEP7L_STRING,strlen(ACS_APGCC_HWVER_APG43_GEP7L_STRING));
                                        g_iVersion = ACS_EMF_APGCC_APG43;
                                        hwVariant = ACS_EMF_APGCC_GEP7;
                                        DEBUG(1, "%s", "ACS_EMF_Common::GetHWVersion - HWVersion is APG43 - GEP7");
                                        break;

				case ACS_APGCC_HWVER_VM:
				default:
					DEBUG(1,"ACS_EMF_Common::GetHWVersion - Unsupported Hardware version [%d]",hwInfo.hwVersion);
					break;
			}
		}
		else{
			DEBUG(1,"ACS_EMF_Common::GetHWVersion - GetHWVersion of APGCC return a error [%i]", hwInfoRes.hwVersionResult);
		}
	}

	DEBUG(1, "%s", "ACS_EMF_Common::GetHWVersion - Leaving");
	return g_iVersion;
}

/*===================================================================
   ROUTINE: GetHWVariant
=================================================================== */
int ACS_EMF_Common::GetHWVariant( )
{
	return hwVariant;
}

// Common Methods for EMF Copy
ACE_INT16 ACS_EMF_Common::checkForDVDStatus(std::string & deviceFileName)
{
	int iMediaCount;
	bool bIsDvdInCs;
	bool bOk;
	ACE_INT16 iRet;
	int32_t iSystemId;
	acs_dsd::NodeStateConstants enmNodeState;
	TScsiDev objScsiDev;
	std::string strDevice;
	std::list<const TScsiItem*> *plstMedia;
	// Initialization
	iMediaCount = EMF_RC_ERROR;
	bIsDvdInCs = false;
	bOk = false;
	iRet = CHECKDVD_MASK_NOTPRESENT;
	iSystemId = acs_dsd::SYSTEM_ID_UNKNOWN;
	enmNodeState = acs_dsd::NODE_STATE_UNDEFINED;
	plstMedia = NULL;
	//Message
	DEBUG(1,"%s","ACS_EMF_Common::checkForDVDStatus - Entering");
	// Get all cd/dvd device in link mode
	objScsiDev.getDevices(&plstMedia, false, SCSI_TYP_CDDVD);
	// Get number of media
	iMediaCount = static_cast<unsigned int>(plstMedia->size());
	// Verifies that there is only one dvd
	if(iMediaCount == 1){
		// Copy device link in output
		(*(plstMedia->begin()))->getMount(&strDevice);
		INFO(1,"ACS_EMF_Common::checkForDVDStatus - Found physical dvd device at [%s]", strDevice.c_str());
		// Get node info
		ACS_EMF_Common::getLocalNode(&iSystemId, &enmNodeState);
		 //Check if the DVD is present in CS
		bOk = objScsiDev.isFbnDvdConfigured(iSystemId, &bIsDvdInCs);
		// Check for error
		if(bOk == true){
			// Check if DVD is present in CS
			if(bIsDvdInCs == true){
				// Set status to dvd found
				iRet = CHECKDVD_MASK_PRESENT;
			}else{
				// Debug message: DVD is not present in CS
				INFO(1,"%s","ACS_EMF_Common::checkForDVDStatus - DVD is not configured in CS");
				iRet = CHECKDVD_MASK_NOTCONFIG;
			}
//			// Check if media is physical mounted
//			if(objScsiDev.isMounted(strDevice) == true){
//				// Media is mounted. Set flag
//				iRet |= CHECKDVD_MASK_MOUNTED;
//			}
		}else{
			// Error to query CS
			ERROR(1,"%s","ACS_EMF_Common::checkForDVDStatus - Unable to obtain if the DVD is configured in CS");
			iRet = CHECKDVD_MASK_NOTCONFIG;
		}
	}else if(iMediaCount > 1){
		// Too many DVD
		INFO(1,"%s","AACS_EMF_Common::checkForDVDStatus - Found more than one CD/DVD drive are available. Can not decide which medium to use. Hence exiting..");
	}else{
		// No DVD found
		INFO(1,"%s","ACS_EMF_Common::checkForDVDStatus - did not find any CD/DVD Drive Configured on this Node");
	}
	deviceFileName = strDevice;
	//Message
	DEBUG(1,"%s","ACS_EMF_Common::checkForDVDStatus - Leaving");
	// Exit from method
	return (iRet);
}

bool ACS_EMF_Common::isDVDMounted()
{
	//bool retCode = true;
	TScsiDev objScsiDev;
//	if (!objScsiDev.isMounted(&(getDeviceName()[1]))){
//		retCode = false;
	return (objScsiDev.isMounted(&(getDeviceName()[1])));
}

/*===================================================================
   ROUTINE: checkForDVDStatus
=================================================================== */
ACE_INT32 ACS_EMF_Common::checkForPassiveDVDStatus(ACE_TCHAR * device)
{
	FILE *fp = NULL;
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_TCHAR data[10000];
	ACE_INT32 ret = EMF_RC_ERROR;
	ACE_INT32 dvdCount = 0;
	
	DEBUG(1,"%s","ACS_EMF_Common::checkForPassiveDVDStatus - Entering");

	fp = popen(cdRecordCmd, mode);
        if(fp != NULL){  /* Modified due to Coverity Defect : Dereference NULL return value */
		while( fgets(data, 10000, fp) != NULL){
			FILE * pFile = NULL;
			ACE_TCHAR  chkDriveCmd [100];
			ACE_INT32 len = strlen(data);
			if( data[len-1] == '\n' )
				data[len-1] = '\0';
			ACE_OS::strncpy(device,data,len);
			sprintf(chkDriveCmd,"%s%s%s","cdrecord -checkdrive dev=",data," 2>/dev/null 1>/dev/null");
			pFile = popen(chkDriveCmd, mode);

                	if (pFile != NULL)
                        	status = pclose(pFile);
			else{
				ERROR(1,"ACS_EMF_Common::checkForPassiveDVDStatus - Error in executing the %s",chkDriveCmd);
				pclose(fp);
				DEBUG(1,"%s","ACS_EMF_Common::checkForPassiveDVDStatus - Leaving");
				return EMF_RC_ERROR;
			}
			if (status != EMF_RC_ERROR && WIFEXITED(status)){
				ret=WEXITSTATUS(status);
			}
			if (ret == 0)
				dvdCount ++;
		}
		status = pclose(fp);
        }
        else{
                ERROR(1,"ACS_EMF_Common::checkForPassiveDVDStatus - Error in executing the %s",cdRecordCmd);
                DEBUG(1,"%s","ACS_EMF_Common::checkForPassiveDVDStatus - Leaving");
                return ret;
        }
        if (status != EMF_RC_ERROR && WIFEXITED(status) ){
        	ret = WEXITSTATUS(status);
        }
        if (dvdCount > 1){
        	INFO(1,"%s","ACS_EMF_Common::checkForPassiveDVDStatus - More than one CD/DVD mediums are available. Can not decide which medium to use. Hence exiting..");
        	DEBUG(1,"%s","ACS_EMF_Common::checkForPassiveDVDStatus - Leaving");
        	return EMF_RC_ERROR;
        }
        if (dvdCount == EMF_RC_OK){
        	INFO(1,"%s","ACS_EMF_Common::checkForPassiveDVDStatus - CD/DVD Drive is not Configured to this Node");
        	DEBUG(1,"%s","ACS_EMF_Common::checkForPassiveDVDStatus - Leaving");
        	return EMF_RC_ERROR;
        }
        DEBUG(1,"%s","ACS_EMF_Common::checkForPassiveDVDStatus - Leaving");
        return ret;
}

/*===================================================================
   ROUTINE: checkForMediumPresence
=================================================================== */
ACE_INT32 ACS_EMF_Common::checkForMediumPresence()
{
	FILE *fp = NULL;
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_TCHAR cmd [100];
	ACE_TCHAR cdCmd[100];
	ACE_INT32 ret = acs_emf_common::DVDMEDIA_STATE_ERROR;

	DEBUG(1,"%s","ACS_EMF_Common::checkForMediumPresence - Entering");

	sprintf(cmd,"%s %s %s",dvdMediaInfoCmd,g_devicefile," 1>/dev/null 2>/dev/null");

	fp = popen(cmd, mode);
        if (fp != NULL)
	        status = pclose(fp);
        else{
                ERROR(1,"ACS_EMF_Common::checkForMediumPresence - Error in executing the %s",cmd);
                DEBUG(1,"%s","ACS_EMF_Common::checkForMediumPresence - Leaving");
                return acs_emf_common::DVDMEDIA_STATE_ERROR;
        }

	if (status != EMF_RC_ERROR && WIFEXITED(status)){
		ret=WEXITSTATUS(status);
		//DEBUG(1,"%s execution status = %d",cmd,ret);
	}
	if(ret == 0){

		INFO(1,"%s","ACS_EMF_Common::checkForMediumPresence - Medium Type is DVD");
		DEBUG(1,"%s","ACS_EMF_Common::checkForMediumPresence - Leaving");
		return acs_emf_common::DVDMEDIA_PRESENT;
	}
	else{
		sprintf(cdCmd,"%s %s %s",cdInfoCmd,g_devicefile," 2>/dev/null 1>/dev/null");
		//ACE_OS::strcpy(cdInfoCmd,cmd);
		fp = popen(cdCmd , mode);
                if (fp != NULL)
		        status = pclose(fp);
                else{
                        ERROR(1,"ACS_EMF_Common::checkForMediumPresence - Error in executing the %s",cdCmd);
                        DEBUG(1,"%s","ACS_EMF_Common::checkForMediumPresence - Leaving");
                        return EMF_RC_ERROR;
                }

		if (status != EMF_RC_ERROR && WIFEXITED(status) ){
			ret=WEXITSTATUS(status);
			//DEBUG(1,"%s execution status = %d",cdCmd,ret);
		}
		if (ret == 0 ){
			INFO(1,"%s","ACS_EMF_Common::checkForMediumPresence - Medium Type is CD");
			DEBUG(1,"%s","ACS_EMF_Common::checkForMediumPresence - Leaving");
			return acs_emf_common::DVDMEDIA_CD_MEDIATYPE;
		}
		else{
			INFO(1,"%s","ACS_EMF_Common::checkForMediumPresence - Medium Not Found");
			DEBUG(1,"%s","ACS_EMF_Common::checkForMediumPresence - Leaving");
			return acs_emf_common::DVDMEDIA_NOT_PRESENT;
		}
	}
	DEBUG(1,"%s","ACS_EMF_Common::checkForMediumPresence - Leaving");
}

/*===================================================================
   ROUTINE: mountDVDData
=================================================================== */
ACE_INT32 ACS_EMF_Common::mountDVDData()
{
	FILE *fp = NULL;
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_TCHAR cmd[100];
	ACE_INT32 ret = EMF_RC_ERROR;
	ACE_INT32 iMountFlag = EMF_MEDIA_NOK;

	DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - Entering");

	sleep(1);
	// Check if the folder "/data/opt/ap/internal_root/media" is already mounted on DVD
	iMountFlag = ACS_EMF_Common::isDvdCorrectlyMounted(true);
	// Check previews flag
	if(iMountFlag == EMF_MEDIA_MOUNTED){
		// Success: Media is mounted on correct path
		DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - device is already correctly mounted [return EMF_MEDIA_SUCCESS]");
		DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - Leaving");
		return EMF_MEDIA_SUCCESS;
	}else if(iMountFlag == EMF_MEDIA_MOUNTEDBAD){
		// Error: Media is mounted on bad path
		DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - device is already bad mounted [return EMF_MEDIA_MOUNTEDBAD]");
		DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - Leaving");
		return EMF_MEDIA_MOUNTEDBAD;
	}else if(iMountFlag == EMF_MEDIA_NOK){
		// Error to call method
		DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - Unable to check mount path of the DVD [return EMF_MEDIA_NOK]. ACS_EMF_Common::isDvdCorrectlyMounted fail");
		DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - Leaving");
		return EMF_MEDIA_NOK;
	}else if(iMountFlag == EMF_MEDIA_UMOUNTREQ){
		// Require to umount from /media
		DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - Umount required from /media");
		if( ACS_EMF_Common::unmountPassiveDVDData(false) == 0 ){
			DEBUG(1,"%s","ACS_EMF_Common::mountDVDData() - unmounted the DVD which is mounted to /media");
		}else{
			DEBUG(1,"%s","ACS_EMF_Common::mountDVDData() - Failed to unmount the DVD which is mounted to /media");
			DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - Leaving");
			return EMF_MEDIA_MOUNTEDBAD;
		}	
	}else if(iMountFlag != EMF_MEDIA_NOTMOUNTED){
		// Error: unexpected return value
		DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - isDvdCorrectlyMounted return with unexpected code");
		DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - Leaving");
		return EMF_MEDIA_NOK;
	}
	// This code is executed if iMountFlag == EMF_RC_MEDIANOTMOUNTED
	sprintf(cmd,"mount %s /data/opt/ap/internal_root/media %s",g_devicefile," 1>/dev/null 2>/dev/null");
	fp = popen(cmd , mode);
        if (fp != NULL)
	        status = pclose(fp);
        else{
                ERROR(1,"ACS_EMF_Common::mountDVDData() - Error in executing the %s",cmd);
                DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - Leaving");
                return EMF_MEDIA_NOK;
        }

	if (status != EMF_RC_ERROR && WIFEXITED(status) ){
		ret = WEXITSTATUS(status);
		//DEBUG(0,"%s execution status = %d",cmd,ret);
	}

	if (ret == 0 ){
		INFO(1,"%s","ACS_EMF_Common::mountDVDData() - Mounting the device is Success");
		DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - Leaving");
		return EMF_MEDIA_SUCCESS;
	}
	else{
		ERROR(1,"%s","ACS_EMF_Common::mountDVDData() - Mounting the device is Failed!!!");
		ERROR(1,"%s","ACS_EMF_Common::mountDVDData() - Reason could be DVD is already mounted or empty or DVD is corrupted !!");
		DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - Leaving");
		return EMF_MEDIA_NOK;

#if 0 //------ NOT IMPROVEMENT. THIS CODE IS DISABLED
		sprintf(cmd,"%s %s /data/opt/ap/internal_root/media %s",mountCmd,g_devicefile," 1>/dev/null 2>/dev/null");
		fp = popen(cmd , mode);
		if(fp != NULL)
                        status = pclose(fp);
                else{
                        DEBUG(1,"ACS_EMF_Common::mountDVDData() - Error in executing the %s\n",cmd);
                        return EMF_MEDIA_NOK;
                }
		if (WIFEXITED(status) )
		{
			ret=WEXITSTATUS(status);
			DEBUG(1,"ACS_EMF_Common::mountDVDData() - %s execution status = %d\n",cmd,ret);
		}

		if (ret == 0 )
		{
			DEBUG(1,"%s","ACS_EMF_Common::mountDVDData() - Mounting in UDF is Success\n");
			DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - Leaving");
			return EMF_MEDIA_SUCCESS;
		}
		else
		{
			DEBUG(1,"%s","ACS_EMF_Common::mountDVDData() - Mounting in UDF failed\n");
			// Mounting of DVD+R,DVD-R is done only in iso9660
			sprintf(cmd,"%s %s /data/opt/ap/internal_root/media %s","mount -t iso9660 ",g_devicefile," 1>/dev/null 2>/dev/null");
			fp = popen(cmd , mode);
                        if(fp != NULL)
			        status = pclose(fp);
                        else{
                                DEBUG(1,"ACS_EMF_Common::mountDVDData() - Error in executing the %s\n",cmd);
                                DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - Leaving");
                                return EMF_MEDIA_NOK;
                        }
			if (WIFEXITED(status) )
			{
				ret=WEXITSTATUS(status);
				DEBUG(1,"ACS_EMF_Common::mountDVDData() - %s execution status = %d\n",cmd,ret);
			}

			if (ret == 0 )
			{
				DEBUG(1,"%s","ACS_EMF_Common::mountDVDData() - Mounting in ISO9660 is Success\n");
				DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - Leaving");
				return EMF_MEDIA_SUCCESS;
			}
			else
			{
				DEBUG(1,"%s","ACS_EMF_Common::mountDVDData() - Mounting in ISO9660 is failed\n");
				DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - Leaving");
				return EMF_MEDIA_NOK;
			}
		}
#endif // END
	}
	DEBUG(1,"%s", "ACS_EMF_Common::mountDVDData() - Leaving");
}//End of mountDVDData

/*===================================================================
   ROUTINE: unmountDVDData
=================================================================== */
ACE_INT32 ACS_EMF_Common::unmountDVDData()
{
	FILE *fp = NULL;
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_INT32 ret = EMF_RC_ERROR;
	ACE_TCHAR cmd[100];

	DEBUG(1,"%s", "ACS_EMF_Common::unmountDVDData() - Entering");

	sprintf(cmd,"%s %s %s",unmountCmd,g_devicefile," 1>/dev/null 2>/dev/null");
	fp = popen(cmd , mode);
        if(fp != NULL)
	        status = pclose(fp);
        else{
                ERROR(1,"ACS_EMF_Common::unmountDVDData() - Error in executing the %s",unmountCmd);
                DEBUG(1,"%s", "ACS_EMF_Common::unmountDVDData() - Leaving");
                return EMF_RC_ERROR;
        }

	if (status != EMF_RC_ERROR && WIFEXITED(status)){
		ret=WEXITSTATUS(status);
		//DEBUG(1,"%s execution status = %d",unmountCmd,ret);
	}

	if (ret == 0 ){
		INFO(1,"%s","ACS_EMF_Common::unmountDVDData() - Unmounting Successfull");
		DEBUG(1,"%s", "ACS_EMF_Common::unmountDVDData() - Leaving");
		return EMF_RC_OK;
	}
	else{
		ERROR(1,"%s","Unmounting failed");
		DEBUG(1,"%s", "ACS_EMF_Common::unmountDVDData() - Leaving");
		return EMF_RC_ERROR;
	}
	DEBUG(1,"%s", "ACS_EMF_Common::unmountDVDData() - Leaving");
}//End of unmountDVDData
/*===================================================================
   ROUTINE: mountDVDOnPassive
=================================================================== */
ACE_INT32 ACS_EMF_Common::mountDVDOnPassive()
{
	FILE *fp = NULL;
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_TCHAR cmd[100];
	ACE_INT32 ret = EMF_RC_ERROR;
	
	DEBUG(1,"%s", "ACS_EMF_Common::mountDVDOnPassive() - Entering");
	sprintf(cmd,"mount %s /media %s",g_devicefile," 1>/dev/null 2>/dev/null");
	fp = popen(cmd , mode);
        if(fp != NULL)
	        status = pclose(fp);
        else{
                ERROR(1,"ACS_EMF_Common::mountDVDOnPassive() - Error in executing the %s",cmd);
                DEBUG(1,"%s", "ACS_EMF_Common::mountDVDOnPassive() - Leaving");
                return EMF_RC_ERROR;
        }

	if(status != EMF_RC_ERROR && WIFEXITED(status)){
		ret=WEXITSTATUS(status);
		//DEBUG(1,"%s execution status = %d",cmd,ret);
	}

	if(ret != EMF_RC_OK){
		ERROR(1,"ACS_EMF_Common::mountDVDOnPassive() - Mounting the device is Failed!!!, < errno == %d >", ret);
		ERROR(1,"%s","ACS_EMF_Common::mountDVDOnPassive() - Reason could be DVD is empty or already mounted or DVD is corrupted !!");
		DEBUG(1,"%s", "ACS_EMF_Common::mountDVDOnPassive() - Leaving");
		return EMF_RC_ERROR;
	}
	INFO(1,"%s","ACS_EMF_Common::mountDVDOnPassive() - Mounting the device is Success");
	DEBUG(1,"%s", "ACS_EMF_Common::mountDVDOnPassive() - Leaving");
	return EMF_RC_OK;
}//End of mountDVDOnPassive

//--------------------------------------------------------------------
ACE_INT32 ACS_EMF_Common::getNodePeerID(ACE_TCHAR * peer_id)
{
	string line;
	ifstream myfile (ACS_EMF_FILE_PEER_ID);
	if (myfile.is_open())
	{
		int peerID;
		while ( getline (myfile,line)!= NULL){
			peerID = atoi(line.c_str());
			ACE_OS::strcpy(peer_id,line.c_str());
			DEBUG(1,"ACS_EMF_Common::getNodePeerID - peerID is: %d",peerID);
		}
		myfile.close();
		return EMF_RC_OK;
	}

	else
	{
		DEBUG(1,"%s","ACS_EMF_Common::getNodePeerID - Unable to open file");
		return EMF_RC_ERROR;
	}
}//End of getNodePeerID

//--------------------------------------------------------------------
ACE_INT32 ACS_EMF_Common::getNodeClusterIp(char (& NodeIp)[EMF_IPADDRESS_MAXLEN], int32_t sysId, acs_dsd::NodeSideConstants nodeSide)
{
	// Check for nodeside
	if(nodeSide == acs_dsd::NODE_SIDE_UNDEFINED){
		ERROR(1,"%s", "ACS_EMF_Common::getNodeClusterIp() - failed! - Node side is Undefined");
		return EMF_RC_ERROR;
	}
	// Check for found correct peerId
	if((sysId == AP1_SYSID) && (nodeSide == acs_dsd::NODE_SIDE_A)){ // Check for AP1 node A
		strncpy(NodeIp, EMF_CLUSTERIP_AP1A, ACS_EMF_ARRAY_SIZE(NodeIp));

	}else if((sysId == AP1_SYSID) && (nodeSide == acs_dsd::NODE_SIDE_B)){ // Check for AP1 node B
		strncpy(NodeIp, EMF_CLUSTERIP_AP1B, ACS_EMF_ARRAY_SIZE(NodeIp));

	}else if((sysId == AP2_SYSID) && (nodeSide == acs_dsd::NODE_SIDE_A)){ // Check for AP2 node A
		strncpy(NodeIp, EMF_CLUSTERIP_AP2A, ACS_EMF_ARRAY_SIZE(NodeIp));

	}else if((sysId == AP2_SYSID) && (nodeSide == acs_dsd::NODE_SIDE_B)){ // Check for AP2 node B
		strncpy(NodeIp, EMF_CLUSTERIP_AP2B, ACS_EMF_ARRAY_SIZE(NodeIp));
	}
	return EMF_RC_OK;
}//End of getNodeClusterIp

/*===================================================================
   ROUTINE: isDvdCorrectlyMounted
=================================================================== */
ACE_INT32 ACS_EMF_Common::isDvdCorrectlyMounted(bool bIsLocal)
{
	ACE_INT32 iRet;
	std::string strMntPath;
	TScsiDev objScsiDev;
	// Initialization
	iRet = EMF_MEDIA_NOK;
	// Entering message
	DEBUG(1,"%s", "ACS_EMF_Common::isDvdCorrectlyMounted - Entering");
	// Get dvd mount path
	if(objScsiDev.getMountedDevOn(g_devicefile, &strMntPath) == true){
		// Check if the media is mounted or not
		if(strMntPath.empty() == false){
			// Check the node (local or remote)
			if(bIsLocal == true){
				// Check if the strMntPath is "/data/opt/ap/internal_root/media"
				if(strMntPath.compare(copypath) == EMF_RC_OK){
					// DVD is correctly mounted
					iRet = EMF_MEDIA_MOUNTED;
					DEBUG(1,"ACS_EMF_Common::isDvdCorrectlyMounted - DVD is correctly mounted on [%s]", strMntPath.c_str());
				}else if(strMntPath.compare(copypathforpassive) == 0){
					// DVD is mounted on /media. Umount the DVD (CNI requirement)
					DEBUG(1,"ACS_EMF_Common::isDvdCorrectlyMounted - DVD is mounted on [%s]. Required umount it.", strMntPath.c_str());
					// Set Flag to "umount required".
					iRet = EMF_MEDIA_UMOUNTREQ;
				}else{
					// DVD is bad mounted
					iRet = EMF_MEDIA_MOUNTEDBAD;
					DEBUG(1,"ACS_EMF_Common::isDvdCorrectlyMounted - DVD is bad mounted on [%s]", strMntPath.c_str());
				}
			}else{
			// node is remote (correctly mount path is /media)
				if(strMntPath.compare(copypathforpassive) == EMF_RC_OK){
					// DVD is correctly mounted
					iRet = EMF_MEDIA_MOUNTED;
					DEBUG(1,"ACS_EMF_Common::isDvdCorrectlyMounted - DVD is correctly mounted on [%s]", copypathforpassive);
				}else if(strMntPath.compare(copypath) == 0){
					// DVD is mounted on /data/opt/ap/internal_root/media. Umount the DVD (CNI requirement)
					DEBUG(1,"ACS_EMF_Common::isDvdCorrectlyMounted - DVD is mounted on [%s]. Required umount it.", strMntPath.c_str())
					// Set Flag to "umount required".
					iRet = EMF_MEDIA_UMOUNTREQ;
				}else{
					// DVD is bad mounted
					iRet = EMF_MEDIA_MOUNTEDBAD;
					DEBUG(1,"ACS_EMF_Common::isDvdCorrectlyMounted - DVD is bad mounted on [%s]", strMntPath.c_str())
				}
			}
		}else{
			// Media not mounted
			iRet = EMF_MEDIA_NOTMOUNTED;
			DEBUG(1,"%s", "ACS_EMF_Common::isDvdCorrectlyMounted - DVD is not mounted")
		}
	}else{
		DEBUG(1,"%s","ACS_EMF_Common::isDvdCorrectlyMounted - getMountedDevOn fail!")
	}
	// Exiting
	DEBUG(1,"%s", "ACS_EMF_Common::isDvdCorrectlyMounted - Leaving")
	// Exit from method
	return (iRet);
}

/*===================================================================
   ROUTINE: mountPassiveDVDOnActive
=================================================================== */
ACE_INT32 ACS_EMF_Common::mountPassiveDVDOnActive(int32_t sysId, acs_dsd::NodeSideConstants nodeSide)
{
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
	//ACE_TCHAR peer_id[10];
	char peerNodeIp[EMF_IPADDRESS_MAXLEN]= {0};
	DEBUG(1,"%s", "ACS_EMF_Common::mountPassiveDVDOnActive - Entering");
	if(GetHWVariant() < 3){
		TScsiDev objScsiDev;
		//ACS_EMF_Common::getNodePeerID(peer_id);
		/* Check if the mount exists */
		if(objScsiDev.isDirMounted(ACS_EMF_ACTIVE_MEDIA) == true){
			DEBUG(1,"ACS_EMF_Common::mountPassiveDVDOnActive - [%s]:%s",ACS_EMF_ACTIVE_MEDIA, "Acive Mount found");
			DEBUG(1,"%s", "ACS_EMF_Common::mountPassiveDVDOnActive - Leaving")
			return EMF_RC_OK;
		}
		if (getNodeClusterIp(peerNodeIp, sysId, nodeSide)){
			ERROR(1,"%s","ACS_EMF_Common::mountPassiveDVDOnActive - Call 'getNodeClusterIp()' failure! Cannot Mount the device on Active node!!!");
			DEBUG(1,"%s", "ACS_EMF_Common::mountPassiveDVDOnActive - Leaving")
			return EMF_RC_ERROR;
		}
	}
	if(GetHWVariant() >= 3)
		sprintf(cmd,"%s %s %s %s", ACS_EMF_CMD_NFS_OPERATIONS, ACS_EMF_MOUNT_NFS_OPTS, ACS_EMF_PASSIVE_MEDIA, ACS_EMF_ACTIVE_MEDIA);
	else
		// nfstimeout option can fail if the NFS mount is hard
		// and stat() does not return until the file system is available.
		// if a soft mount of the NFS file system is done, when an error occurs,
		// the stat() function returns with an error
		sprintf(cmd,"%s %s:/media %s %s", softnfsmountCmd, peerNodeIp,ACS_EMF_ACTIVE_MEDIA,"1>/dev/null 2>/dev/null");
	DEBUG(1,"ACS_EMF_Common::mountPassiveDVDOnActive - is trying to execute %s",cmd);
	if(_execlp(cmd) != EMF_RC_OK){
		DEBUG(1,"%s","ACS_EMF_Common::mountPassiveDVDOnActive - Sharing of /media path on node is Failed");
		DEBUG(1,"%s", "ACS_EMF_Common::mountPassiveDVDOnActive - Leaving");
		return EMF_RC_ERROR;
	}else{
		DEBUG(1,"%s","ACS_EMF_Common::mountPassiveDVDOnActive - Sharing of /media path on node is Successfull");
		DEBUG(1,"%s", "ACS_EMF_Common::mountPassiveDVDOnActive - Leaving");
		return EMF_RC_OK;
	}
	DEBUG(1,"%s", "ACS_EMF_Common::mountPassiveDVDOnActive - Leaving");
	return EMF_RC_ERROR;
}//End of mountPassiveDVDOnActive
/*===================================================================
   ROUTINE: unmountPassiveDVDOnActive
=================================================================== */
ACE_INT32 ACS_EMF_Common::unmountPassiveDVDOnActive()
{
	FILE *fp = NULL;
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
	ACE_INT32 returnCode = EMF_RC_ERROR;

	DEBUG(1,"%s", "ACS_EMF_Common::unmountPassiveDVDOnActive - Entering");
	// ******************************************** FIX: BEGIN TR HR66265 ********************************************
	if(GetHWVariant() < 3){
		TScsiDev objScsiDev;
		// Check if the folder "/data/opt/ap/internal_root/media/" is mounted on.
		if(objScsiDev.isDirMounted("/data/opt/ap/internal_root/media") == false){
			// Debug string
			DEBUG(1,"%s","ACS_EMF_Common::unmountPassiveDVDOnActive - isDirMounted return [false]");
			DEBUG(1,"%s", "ACS_EMF_Common::unmountPassiveDVDOnActive - Leaving");
			// Do not execute umount and return ok.
			return (EMF_RC_OK);
		}
	}
	// ********************************************* FIX : END TR HR66265 *********************************************
	//sprintf(cmd,"umount 169.254.208.1:/media/ %s ","1>/dev/null 2>/dev/null");
	sprintf(cmd,"umount -f -l /data/opt/ap/internal_root/media/ %s ","1>/dev/null 2>/dev/null");
	fp = popen(cmd, mode);
        if(fp != NULL)
	        status = pclose(fp);
        else{
                ERROR(1,"ACS_EMF_Common::unmountPassiveDVDOnActive - Error in executing the %s",cmd);
                DEBUG(1,"%s", "ACS_EMF_Common::unmountPassiveDVDOnActive - Leaving")
                return returnCode;
        }

	if (status == EMF_RC_ERROR){
		ERROR(1,"ACS_EMF_Common::unmountPassiveDVDOnActive - call 'pclose' failure!: Unexpected error in executing the %s",cmd);
			DEBUG(1,"%s", "ACS_EMF_Common::unmountPassiveDVDOnActive - Leaving");
		return returnCode;
	}
	if (WIFEXITED(status) ){
		int op_res = WEXITSTATUS(status);
		DEBUG(1,"ACS_EMF_Common::unmountPassiveDVDOnActive - %s execution status == %d", cmd, op_res);
		if (op_res == 0)	returnCode = EMF_RC_OK;  //Success
	}
	else {
		ERROR(1,"ACS_EMF_Common::unmountPassiveDVDOnActive - Error in executing the %s - execution state is unavailable!",cmd);
	}

	if (returnCode == EMF_RC_OK){ DEBUG(1,"%s","ACS_EMF_Common::unmountPassiveDVDOnActive - Unmounting the device is Success");}
	else  ERROR(1,"%s","ACS_EMF_Common::unmountPassiveDVDOnActive - Unmounting the device is Failed!!!");
	
	DEBUG(1,"%s", "ACS_EMF_Common::unmountPassiveDVDOnActive - Leaving");
	return returnCode;

}//End of mountPassiveDVDOnActive
/*===================================================================
   ROUTINE: unmountDVDOnPassive
=================================================================== */
ACE_INT32 ACS_EMF_Common::unmountDVDOnPassive()
{
	FILE *fp = NULL;
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_INT32 ret = EMF_RC_ERROR;
	
	DEBUG(1,"%s", "ACS_EMF_Common::unmountDVDOnPassive - Entering");
        if ((GetHWVariant() >= 3) && (!isMountd())){
                DEBUG(1,"%s","ACS_EMF_Common::unmountDVDOnPassive - Media is already unmounted");
                return EMF_RC_OK;
        }
	fp = popen(unmountDVDOnPassiveCmd , mode);
        if(fp != NULL)
	        status = pclose(fp);
        else{
                ERROR(1,"ACS_EMF_Common::unmountDVDOnPassive - Error in executing the %s",unmountDVDOnPassiveCmd);
                DEBUG(1,"%s", "ACS_EMF_Common::unmountDVDOnPassive - Leaving")
                return EMF_RC_ERROR;
        }
       
	if (status != EMF_RC_ERROR && WIFEXITED(status) ){
		ret=WEXITSTATUS(status);
		DEBUG(1,"ACS_EMF_Common::unmountDVDOnPassive - %s execution status = %d",unmountDVDOnPassiveCmd,ret);
	}
	if (ret == 0 ){
		DEBUG(1,"%s","ACS_EMF_Common::unmountDVDOnPassive - Unmounting Successfull");
		DEBUG(1,"%s", "ACS_EMF_Common::unmountDVDOnPassive - Leaving");
		return EMF_RC_OK;
	}
	else{
		DEBUG(1,"%s","ACS_EMF_Common::unmountDVDOnPassive - Unmounting failed");
		DEBUG(1,"%s", "ACS_EMF_Common::unmountDVDOnPassive - Leaving");
		return EMF_RC_ERROR;
	}
	DEBUG(1,"%s", "ACS_EMF_Common::unmountDVDOnPassive - Leaving");
}//End of unmountDVDOnPassive 

/*===================================================================
   ROUTINE: copyDataToDestFolder
=================================================================== */
ACE_INT32 ACS_EMF_Common::copyDataToDestFolder (const ACE_TCHAR* filenames[EMF_COPY_MAX_NUMFILES], 
												ACE_INT32 nooffiles,
												bool overwrite,
												ACE_TCHAR * errorText)
{
	std::string fname;
	std::string dname;
	std::string sname;
	ACE_TCHAR cmd[1000] = {0};

	DEBUG(1,"%s", "ACS_EMF_Common::copyDataToDestFolder - Entering");

	dname.clear();
	dname.append(filenames[nooffiles-1]);  // last filename in filenames[] is the destination path
	//INFO(1,"ACS_EMF_Common::copyDataToDestFolder:: Destination folder is = %s",dname.c_str());

	for (int i = 0; i < nooffiles-1; i++)
	{
		fname.clear();
		fname.append(dname);
		fname.append(ACE_TEXT("/"));
		std::string name(filenames[i]);
		fname.append(name.substr((name.find_last_of("/\\")+1)));
		INFO(1,"ACS_EMF_Common::copyDataToDestFolder - File to be copied from DVD is = %s",fname.c_str());
		ACE_INT32 nFileType = ACS_EMF_Common::CheckFile(fname.c_str());
		//DEBUG(1,"CheckFile Status = %d",nFileType);
		//DEBUG(1,"overwrite %s", (overwrite)?"true":"false");
		if (!overwrite && nFileType != EMF_RC_ERROR)
		{
			ACE_OS::strcpy(errorText,"FILE/FOLDER IS ALREADY PRESENT IN DESTINATION");
			ERROR(1,"ACS_EMF_Common::copyDataToDestFolder - Copy Failed because ... %s is already present in %s Folder",filenames[i],dname.c_str());
			DEBUG(1,"%s", "ACS_EMF_Common::copyDataToDestFolder - Leaving");
			return EMF_RC_ERROR;
		}
		sname.append(space);
		sname.append(NBI_root); 
		sname.append(filenames[i]);
	}
	sprintf(cmd,"%s %s %s %s", ACS_EMF_CMD_GNRL_OPERATIONS, ACS_EMF_COPY_OPTS, sname.c_str(),dname.c_str());
	if(_execlp(cmd) != EMF_RC_OK){
		ERROR(1,"ACS_EMF_Common::copyDataToDestFolder - Copying specified files to %s Folder failed",dname.c_str());
		ACE_OS::strcpy(errorText,FAILED);
		DEBUG(1,"%s", "ACS_EMF_Common::copyDataToDestFolder - Leaving");
		return EMF_RC_ERROR;
	}else{
		INFO(1,"ACS_EMF_Common::copyDataToDestFolder - Copying specified files to %s Folder is Successful",dname.c_str());
		DEBUG(1,"%s", "ACS_EMF_Common::copyDataToDestFolder - Leaving");
		return EMF_RC_OK;
	}
	// Prevent warning
	DEBUG(1,"%s", "ACS_EMF_Common::copyDataToDestFolder - Leaving"); // Called if remove else
	return EMF_RC_ERROR;
}//End of copyDataToDestFolder

/*===================================================================
   ROUTINE: getMediumType
=================================================================== */
ACE_INT32 ACS_EMF_Common::getMediumType(ACE_TCHAR * mediumType)
{
	FILE *fp = NULL;
        ACE_INT32 status;
	ACE_TCHAR data[10000];
	ACE_INT32 ret = -1;
	ACE_TCHAR cmd[EMF_CMD_COPY_BUFF_LEN];

	DEBUG(1,"%s", "ACS_EMF_Common::getMediumType - Entering");

	sprintf(cmd,"%s %s %s",getDVDMediumTypeCmd,g_devicefile," 2>/dev/null | grep 'Mounted Media' | awk '{print $4}'");

	fp = popen(cmd , mode);
        if(fp != NULL){
		if((fgets(data, 10000, fp) != NULL)){
			ACE_INT32 len = strlen(data);
			if( data[len-1] == '\n' )
				data[len-1] = '\0';
			strncpy(mediumType,data,EMF_MEDIATYPE_MAXLEN);
		}
		status = pclose(fp);
        }
        else{
                ERROR(1,"ACS_EMF_Common::getMediumType - Error in executing the %s",cmd);
                DEBUG(1,"%s", "ACS_EMF_Common::getMediumType - Leaving");
                return -1;
        }

	if (status != EMF_RC_ERROR && WIFEXITED(status) ){
		ret=WEXITSTATUS(status);
		DEBUG(1,"ACS_EMF_Common::getMediumType - %s execution status = %d\n",cmd,ret);
	}

	if (ret != 0 ){
		INFO(1,"%s","ACS_EMF_Common::getMediumType - failed! EMF Cannot retrieve MediumType");
		DEBUG(1,"%s", "ACS_EMF_Common::getMediumType - Leaving");
		return -1;
	}

	INFO(1,"ACS_EMF_Common::getMediumType - Successfull executed <mediumType == %s >", mediumType);
	DEBUG(1,"%s", "ACS_EMF_Common::getMediumType - Leaving");
	return 0;

}//End of getMediumType

/*===================================================================
   ROUTINE: getNodeState
=================================================================== */
ACE_INT32 ACS_EMF_Common::getNodeState()
{
	DEBUG(0,"%s","Entering ACS_EMF_Common::getNodeState()");
	ACS_PRC_API prcObj;
	ACE_INT32 state = prcObj.askForNodeState();
	if( state == 1){
		DEBUG(1,"%s","Node state received is ACTIVE !");}
	else if (state == 2){
		DEBUG(1,"%s","Node state received is PASSIVE !");}
	else{
		DEBUG(1,"%s","Node state received is UNDEFINED !");}
	DEBUG(0,"%s","Exiting ACS_EMF_Common::getNodeState()");
	return state;
}//End of getNodeState

/*===================================================================
   ROUTINE: createImage
=================================================================== */
ACE_INT32 ACS_EMF_Common::createImage(const ACE_TCHAR* filenames[100],ACE_INT32 nooffiles,const ACE_TCHAR* imageName,ACE_INT32 flag)
{
	FILE *fp = NULL;
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_INT32 ret = EMF_RC_ERROR;
	ACE_TCHAR cmd[4000];

	// To fetch ACS_DATA disk path
	std::string dataDiskSymName("ACS_DATA");
	std::string dataDiskPath;
	
	DEBUG(1,"%s","ACS_EMF_Common::createImage - Entering");

	if(ACS_EMF_Common::getNodeState() == 1){
		// IN CASE OF ACTIVE NODE
		bool myResult = ACS_EMF_Common::GetDataDiskPath(dataDiskSymName, dataDiskPath);
		if (myResult){
			DEBUG(0,"ACS_EMF_Common::createImage - ACS_DATA Disk path = %s",dataDiskPath.c_str());
			acsDataDiskPath = dataDiskPath;
			dataDiskPath.append(emfDirPath);
			DEBUG(0,"ACS_EMF_Common::createImage - EMF Image file Path  = %s",dataDiskPath.c_str());
			imageFilePath = dataDiskPath;
		}
		else{
			ERROR(1,"ACS_EMF_Common::createImage - Error in Obtaining Datapath for  %s from Common APGCC LIB",dataDiskSymName.c_str());
			INFO(1,"%s","ACS_EMF_Common::createImage - Checksum is not calculated !!!");
		}
	}
	else{
		// IN CASE OF PASSIVE NODE
		imageFilePath = "/emfsharemnt/data/";
	}
	if (flag == EMF_RC_OK){
		// FOR DVD
		sprintf(cmd,"%s -V %s -o %s%s%s %s",createImageForDVDCmd,imageName,imageFilePath.c_str(),imageName,".iso ",filenames[0]);
	}
	else{
		// FOR CD
		sprintf(cmd,"%s -V %s -o %s%s%s %s",createImageForCDCmd,imageName,imageFilePath.c_str(),imageName,".iso ",filenames[0]);
	}

	for (ACE_INT32 i=1; i<nooffiles; i++){
		ACE_OS::strcat(cmd,space);
		ACE_OS::strcat(cmd,filenames[i]);
	}

	ACE_OS::strcat(cmd,move_err_mesgs_to_null);

	//DEBUG(1,"cmd = %s",cmd);

	fp = popen(cmd , mode);
        if (fp != NULL)
		status = pclose(fp);
        else{
                ERROR(1,"ACS_EMF_Common::createImage - Error in executing the %s",cmd);
                DEBUG(1,"%s","ACS_EMF_Common::createImage - Leaving");
                return EMF_RC_ERROR;
        }
	if (status != EMF_RC_ERROR && WIFEXITED(status)){
		ret=WEXITSTATUS(status);
		//DEBUG(1,"%s execution status = %d\n",cmd,ret);
	}

	if (ret == 0 ){
		INFO(1,"%s","ACS_EMF_Common::createImage - Image creation is Successfull");
		DEBUG(1,"%s","ACS_EMF_Common::createImage - Leaving");
		return EMF_RC_OK;
	}
	else{
		INFO(1,"%s","ACS_EMF_Common::createImage - Image creation failed");
		DEBUG(1,"%s","ACS_EMF_Common::createImage - Leaving");
		return EMF_RC_ERROR;
	}

	// Prevent warning
	DEBUG(1,"%s","ACS_EMF_Common::createImage - Leaving");
	return EMF_RC_ERROR;
}

/*===================================================================
   ROUTINE: checkForRewritableCD
=================================================================== */
ACE_INT32 ACS_EMF_Common::checkForRewritableCD (ACE_TCHAR * device)
{
	FILE *fp = NULL;
	ACE_TCHAR data[10000];
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_INT32 ret = EMF_RC_ERROR;
	ACE_INT32 rewritableCD = EMF_RC_ERROR;
	ACE_TCHAR cmd[100];

	DEBUG(1,"%s","ACS_EMF_Common::checkForRewritableCD - Entering");

	sprintf(cmd,"%s%s%s",checkForRewrtiableCDCmd,device," 2>/dev/null | grep 'Disk sub type: High speed Rewritable' | awk '{print $6}'");

	//DEBUG(1,"cmd = %s\n",cmd);

	fp = popen(cmd , mode);
        if(fp != NULL){
		if((fgets(data, 10000, fp) != NULL)){
			ACE_INT32 len = strlen(data);
			if( data[len-1] == '\n' )
				data[len-1] = '\0';
			//DEBUG(1,"%s\n",data);
			if (strcmp(data,"Rewritable") == 0) rewritableCD = 0;
		}
		status = pclose(fp);
        }
        else{
                ERROR(1,"ACS_EMF_Common::checkForRewritableCD - Error in executing the %s\n",cmd);
                DEBUG(1,"%s","ACS_EMF_Common::checkForRewritableCD - Leaving");
                return EMF_RC_ERROR;
        }

	if (status != EMF_RC_ERROR  && WIFEXITED(status)){
		ret=WEXITSTATUS(status);
		//DEBUG(1,"%s execution status = %d\n",cmd,ret);
	}

	if (ret == 0 ){
		INFO(1,"%s","ACS_EMF_Common::checkForRewritableCD - checkForRewritableCD is Successfull");
		DEBUG(1,"%s","ACS_EMF_Common::checkForRewritableCD - Leaving");
		return rewritableCD;
	}
	
	INFO(1,"%s","checkForRewritableCD failed");
	DEBUG(1,"%s","ACS_EMF_Common::checkForRewritableCD - Leaving");
	return EMF_RC_ERROR;

}//End of checkForRewritableCD

/*===================================================================
   ROUTINE: formatMedium
=================================================================== */
ACE_INT32 ACS_EMF_Common::formatMedium (ACE_INT32 mediumType,ACE_TCHAR * device,ACE_TCHAR * typeofmedium)
{
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
	DEBUG(1,"%s","ACS_EMF_Common::formatMedium - Entering");
	if(GetHWVariant() >= 3)
	{
		ACE_OS::memset(cmd, 0, EMF_CMD_MAXLEN);
		ACE_OS::snprintf(cmd, EMF_CMD_MAXLEN, ACS_EMF_CMD_GNRL_OPERATIONS ACS_EMF_FORMAT_OPTS);
		if(_execlp(cmd) != 0){
			DEBUG(1,"%s","ACS_EMF_Common::formatMedium - formatMedium is Failed");
			DEBUG(1,"%s","ACS_EMF_Common::formatMedium - Leaving");
			return EMF_RC_ERROR;
		}else{
			DEBUG(1,"%s","ACS_EMF_Common::formatMedium - formatMedium is Successfull");
			DEBUG(1,"%s","ACS_EMF_Common::formatMedium - Leaving");
			return EMF_RC_OK;
		}
	}
	else
	{
		if (mediumType < 0 || mediumType > 1){
			ERROR(1,"%s","ACS_EMF_Common::formatMedium - Invalid DVD Medium");
			DEBUG(1,"%s","ACS_EMF_Common::formatMedium - Leaving");
			return EMF_RC_ERROR;
		}

		ACE_TCHAR cmd[100];
		if(mediumType == 0){
			if ((ACE_OS::strcmp(typeofmedium,"DVD-RW") == 0))
				sprintf(cmd,"%s %s %s",formatDVDCmd,g_devicefile," 1>/dev/null 2>/dev/null");
			else
				sprintf(cmd,"%s %s %s",formatDVDPlusRWCmd,g_devicefile," 1>/dev/null 2>/dev/null");
			INFO(1,"ACS_EMF_Common::formatMedium()... trying to format the DVD launching cmd == %s",cmd);

		}
		else if (mediumType == 1){
			DEBUG(1,"%s","ACS_EMF_Common::formatMedium - Formating Medium Type is CD");
			sprintf(cmd,"%s%s%s",formatCDCmd,device," 1>/dev/null 2>/dev/null");
		}
		int infp, outfp;
		if ((pid_in_progress = popen_with_pid(cmd, &infp, &outfp)) <= 0){
			ERROR(1,"ACS_EMF_Common::formatMedium - Error in executing the %s",cmd);
			DEBUG(1,"%s","ACS_EMF_Common::formatMedium - Leaving");
			return EMF_RC_ERROR;
		}
		INFO(1,"ACS_EMF_Common::formatMedium - PID of background Job %d",pid_in_progress);
		ACE_INT32 status = EMF_RC_ERROR;
		ACE_INT32 ret = EMF_RC_ERROR;
		waitpid(pid_in_progress,&status,0);

		if (WIFEXITED(status) ){
			ret=WEXITSTATUS(status);
			pid_in_progress = EMF_RC_ERROR;
			INFO(1,"ACS_EMF_Common::formatMedium - %s execution status = %d",cmd,ret);
		}
		if (ret == 0){
			DEBUG(1,"%s","ACS_EMF_Common::formatMedium - formatMedium is Successfull");
			DEBUG(1,"%s","ACS_EMF_Common::formatMedium - Leaving");
			return EMF_RC_OK;
		}
		else{
			DEBUG(1,"%s","ACS_EMF_Common::formatMedium - formatMedium failed");
			DEBUG(1,"%s","ACS_EMF_Common::formatMedium - Leaving");
			return EMF_RC_ERROR;
		}
	}
}//End of formatMedium

/*===================================================================
   ROUTINE: writeImageOnMedium
=================================================================== */
ACE_INT32 ACS_EMF_Common::writeImageOnMedium(const ACE_TCHAR* fileList[100], 
											ACE_INT32 nooffiles, 
											ACE_TCHAR* device, 
											ACE_TCHAR* mediumType,
											acs_emf_common::EMF_DVD_WRITETYPE  writeType,
											bool verifyFlag, 
											ACE_TCHAR* imageName)
{
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_INT32 ret = EMF_RC_ERROR;
	ACE_TCHAR cmd[EMF_LCMD_MAXLEN];
			
	DEBUG(1,"%s","ACS_EMF_Common::writeImageOnMedium (writeType == %d) Entering", writeType);

	(void) (verifyFlag);

#if 0// No improvement in this code: it's disabled

	if (verifyFlag){
		if(ACS_EMF_Common::getNodeState() == 1){
			// INCASE OF ACTIVE NODE
			std::string dataPath(acsDataDiskPath);
			//DEBUG(1,"ACS_DATA Disk path = %s",dataPath.c_str());
			dataPath.append(chksumFile);
			//DEBUG(1,"Checksum file = %s",dataPath.c_str());
			chksumFilewithFullPath = dataPath;
		}
		else
			// INCASE OF PASSIVE NODE
			//chksumFilewithFullPath = "/emfsharemnt/data/emfsrc_sum.sha1";
			chksumFilewithFullPath = "/cluster/active_data/emfsrc_sum.sha1";
		if (calculateChecksum(fileList,nooffiles,chksumFilewithFullPath.c_str()) != 0){
			DEBUG(1,"%s","ACS_EMF_Common::calculateChecksum calculation failed or interrupted");
			return EMF_RC_ERROR;
		}
		DEBUG(1,"%s","Checksum calculation is SUCCESS");
	}
#endif	// End

	//if ((ACE_OS::strcmp(mediumType,"DVD-RW") == 0) || (ACE_OS::strcmp(mediumType,"DVD+RW") == 0)){
		//DEBUG(1,"%s","Medium Type is DVD-RW or DVD+RW");
		if (writeType == acs_emf_common::WRITETYPE_FORMAT_WRITE){
			if(formatMedium(0,device,mediumType) == EMF_RC_ERROR ){ // 0 means DVD
				ERROR(1,"%s","ACS_EMF_Common::writeImageOnMedium - Formatting is failed... hence existing from write operation");
				DEBUG(1,"%s","ACS_EMF_Common::writeImageOnMedium - Leaving");
				return EMF_RC_ERROR;
			}
			sprintf(cmd,"%s %s %s %s",writeImageOnDVDNewSessionCmd,g_devicefile," -R -J -input-charset iso8859-1 -V",imageName);
			for(ACE_INT32 i=0; i<nooffiles; i++){
				ACE_OS::strcat(cmd,space);
				ACE_OS::strcat(cmd,fileList[i]);
			}
			ACE_OS::strcat(cmd,move_err_mesgs_to_null);
		}
		else if (writeType == acs_emf_common::WRITETYPE_APPENDING){
			// APPENDING
			sprintf(cmd,"%s %s %s %s",writeImageOnDVDAppendingSessionCmd,g_devicefile," -R -J -input-charset iso8859-1 -V",imageName);
			for(ACE_INT32 i=0; i<nooffiles; i++){
				ACE_OS::strcat(cmd,space);
				ACE_OS::strcat(cmd,fileList[i]);
			}
			ACE_OS::strcat(cmd,move_err_mesgs_to_null);
		}

		if (writeType == acs_emf_common::WRITETYPE_NEWSESSION){
			sprintf(cmd,"%s %s %s %s",writeImageOnDVDNewSessionCmd,g_devicefile," -R -J -input-charset iso8859-1 -V",imageName);
			for(ACE_INT32 i=0; i<nooffiles; i++){
				ACE_OS::strcat(cmd,space);
				ACE_OS::strcat(cmd,fileList[i]);
			}

			ACE_OS::strcat(cmd,move_err_mesgs_to_null);
		}
	
// ************************ CD media type is not supported in APG43L *****************************
// This line can be removed!
//	else if (ACE_OS::strcmp(mediumType,"CD") == 0){
//		//DEBUG(1,"%s","Medium type is CD");
//		if(checkForRewritableCD(device) == 0){
//			DEBUG(1,"%s","CD Medium is Rewritable i.e CD+RW or CD-RW");
//			if (writeType == 1) {
//				if(formatMedium(1,device,mediumType) == EMF_RC_ERROR ){ // 1 means CD
//					DEBUG(1,"%s","ACS_EMF_Common::writeImageOnMedium - Formatting is failed... hence existing from write operation\n");
//					DEBUG(1,"%s","ACS_EMF_Common::writeImageOnMedium - Leaving");
//					return EMF_RC_ERROR;
//				}
//			}
//			DEBUG(1,"ACS_EMF_Common::writeImageOnMedium - Image creation status = %d\n",createImage(fileList,nooffiles,imageName,1));
//			sprintf(cmd,"%s%s %s%s%s",writeImageOnRewritableMediumCmd,device,imageFilePath.c_str(),imageName,".iso 1>/dev/null 2>/dev/null");
//		}
//		else{
//			DEBUG(1,"%s","CD Medium is Readonly i.e CD+R or CD-R\n");
//			DEBUG(1,"Image creation status = %d\n",createImage(fileList,nooffiles,imageName,1));
//			sprintf(cmd,"%s%s %s%s%s",writeImageOnReadOnlyCDCmd,device,imageFilePath.c_str(),imageName,".iso 1>/dev/null 2>/dev/null");
//		}
//	}
// *********************************************************************************************

	int infp, outfp;
	if ((pid_in_progress = popen_with_pid(cmd, &infp, &outfp)) <= 0){
		ERROR(1,"ACS_EMF_Common::writeImageOnMedium - Error in executing the %s",cmd);
		DEBUG(1,"%s","ACS_EMF_Common::writeImageOnMedium - Leaving");
		return EMF_RC_ERROR;
	}
	INFO(1,"ACS_EMF_Common::writeImageOnMedium - PID of background Job %d",pid_in_progress);
	waitpid(pid_in_progress,&status,0);

	if (status != EMF_RC_ERROR  && WIFEXITED(status)){
		ret=WEXITSTATUS(status);
		pid_in_progress = EMF_RC_ERROR;
		INFO(1,"ACS_EMF_Common::writeImageOnMedium - %s execution status = %d",cmd,ret);
	}
	if (ret == 0 ){
		INFO(1,"%s","ACS_EMF_Common::writeImageOnMedium - writeImageOnMedium is Successfull");
		DEBUG(1,"%s","ACS_EMF_Common::writeImageOnMedium - Leaving");
		return 0;
	}
	else if (ret == 252){
		INFO(1,"%s","ACS_EMF_Common::writeImageOnMedium - writeImageOnMedium due to media is not appendable");
		DEBUG(1,"%s","ACS_EMF_Common::writeImageOnMedium - Leaving");
		return 252;
	}
	else{
		INFO(1,"%s","ACS_EMF_Common::writeImageOnMedium - writeImageOnMedium failed");
		DEBUG(1,"%s","ACS_EMF_Common::writeImageOnMedium - Leaving");
		return EMF_RC_ERROR;
	}
}//End of writeImageOnMedium

/*===================================================================
   ROUTINE: writeDataOnUSBMedium
=================================================================== */
ACE_INT32 ACS_EMF_Common::writeDataOnUSBMedium(const ACE_TCHAR* fileList[100], ACE_INT32 nooffiles,ACE_INT32 writeType,bool verifyFlag)
{
	ACE_TCHAR cmd[EMF_CMD_COPY_BUFF_LEN] = {0};
	std::string sname;
	std::string dname;

	DEBUG(1,"%s","ACS_EMF_Common::writeDataOnUSBMedium - Entering");

	if(writeType < 0 || writeType > 1){
		DEBUG(1,"%s","Invalid Write Type");
		DEBUG(1,"%s","ACS_EMF_Common::writeDataOnUSBMedium - Leaving");
		return EMF_RC_ERROR;
	}
	(void) (verifyFlag);
	if(mountMedia() != EMF_RC_OK)
		ERROR(1,"%s","ACS_EMF_Common::writeDataOnUSBMedium - Mounting the Device  Failed!!! ");
	if (writeType == 1){
		if(formatAndMountMedia()== EMF_RC_ERROR ){
			DEBUG(1,"%s","ACS_EMF_Common::writeDataOnUSBMedium - Formatting is failed... hence existing from write operation\n");
			DEBUG(1,"%s","ACS_EMF_Common::writeDataOnUSBMedium - Leaving");
			return EMF_RC_ERROR;
		}
		for(ACE_INT32 i=0; i<nooffiles; i++){
			sname.append(fileList[i]);
			sname.append(space);
		}
		if(getNodeState() == 1){
			dname.append(ACS_EMF_ACTIVE_MEDIA);
		}
		else{
			dname.append(ACS_EMF_PASSIVE_MEDIA);
		}
	}
	else{
		for(ACE_INT32 i=0; i<nooffiles; i++){
			sname.append(fileList[i]);
			sname.append(space);
		}
		if(getNodeState() == 1)
			dname.append(ACS_EMF_ACTIVE_MEDIA);
		else
			dname.append(ACS_EMF_PASSIVE_MEDIA);
	}
	
	sprintf(cmd,"%s %s %s %s", ACS_EMF_CMD_GNRL_OPERATIONS, ACS_EMF_COPY_OPTS, sname.c_str(), dname.c_str());
	DEBUG(1,"cmd =%s",cmd);
	if(_execlp(cmd) != 0){
		DEBUG(1,"%s","ACS_EMF_Common::writeDataOnUSBMedium - writeDataOnUSBMedium failed");
		DEBUG(1,"%s","ACS_EMF_Common::writeDataOnUSBMedium - Leaving");
		return EMF_RC_ERROR;
	}

	DEBUG(1,"%s","ACS_EMF_Common::writeDataOnUSBMedium - writeDataOnUSBMedium is Successfull");
	DEBUG(1,"%s","ACS_EMF_Common::writeDataOnUSBMedium - Leaving");
	return EMF_RC_OK;
	
}//End of writeDataOnUSBMedium

/*===================================================================
   ROUTINE: GetHostName
=================================================================== */
const ACE_TCHAR* ACS_EMF_Common::GetHostName(){
	static ACE_TCHAR sznodename[64] = ACE_TEXT("");
	gethostname(sznodename,sizeof(sznodename));
	return sznodename;
}//End of GetHostName

/*===================================================================
   ROUTINE: getNode
=================================================================== */
NODE_NAME ACS_EMF_Common::getNode()
{
	string line;
	int node_id=UNDEFINED;
	ifstream myfile (ACS_EMF_FILE_NODE_ID);
	if (myfile.is_open())
	{
		while(getline (myfile,line)!= NULL){
			node_id = atoi(line.c_str());
			DEBUG(1,"ACS_EMF_Common::getNode - Node ID is:%d",node_id);
		}
		myfile.close();
	}
	else  DEBUG(1,"%s", "ACS_EMF_Common::getNode - Unable to open file");

	if (node_id == 1)
		return NODE_A;
	else if (node_id == 2)
		return NODE_B;
	return UNDEFINED;
}//End of getNode

/*===================================================================
   ROUTINE: GetNodeName
=================================================================== */
ACE_INT32 ACS_EMF_Common::GetNodeName(ACE_INT32 node, ACE_TCHAR *nodeName)
{
	//TODO:This is workaround as DSD has some issues.
	//This logic reads the clusterconf files and fetch the name of given node.
	FILE *fp = NULL;
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_TCHAR data[1000];
	ACE_TCHAR cmd[1000];
	ACE_INT32 ret = EMF_RC_ERROR;
	const char *mesg = " ' | awk {'print $5'} 2>/dev/null";
	sprintf(cmd,"%s%d","/opt/ap/apos/bin/clusterconf/clusterconf node -D | grep 'node ",node);
	ACE_OS::strcat(cmd,mesg);
	fp = popen(cmd, mode);
        if( fp != NULL){
		if((fgets(data, 1000, fp) != NULL)){
			int len = strlen(data);
			if( data[len-1] == '\n' )
				data[len-1] = '\0';

			INFO(1,"ACS_EMF_Common::GetNodeName - Node Name = %s",data);
			ACE_OS::strcpy(nodeName,data);
		}
		status = pclose(fp);
        }
        else{
                ERROR(1,"ACS_EMF_Common::GetNodeName - Error in executing the %s",cmd);
                return EMF_RC_ERROR;
        }

	if (status != EMF_RC_ERROR && WIFEXITED(status)){
		ret=WEXITSTATUS(status);
		//DEBUG(1,"%s execution status = %d\n",cmd,ret);
	}

	if (ret == 0 )
		return ret;
	else{
		ERROR(1,"ACS_EMF_Common::GetNodeName - %s failed",cmd);
		return EMF_RC_ERROR;
	}

}//End of GetNodeName

/*===================================================================
   ROUTINE: GetUserName
=================================================================== */
const ACE_TCHAR* ACS_EMF_Common::GetUserName()
{
	static ACE_TCHAR szUsername[64] = ACE_TEXT("");
	DEBUG(1,"User Name = %s",ACS_APGCC::getUserLoginName());
	ACE_OS::strcpy(szUsername,ACS_APGCC::getUserLoginName());
	return szUsername;
}//End of GetUserName

/*===================================================================
   ROUTINE: GenerateVolumeName
=================================================================== */
const ACE_TCHAR* ACS_EMF_Common::GenerateVolumeName()
{
	static ACE_TCHAR szDateTime[128] = ACE_TEXT("");
	ACE_TCHAR szHostname[64];
	ACE_OS::strcpy(szHostname,ACS_EMF_Common::GetHostName());
	time_t LocalTime;
	struct tm* time1;
	ACE_OS::time(&LocalTime);
	time1 = ACE_OS::localtime(&LocalTime);
	sprintf(szDateTime,
			ACE_TEXT("%02d%02d%02d_%s"),
			(time1->tm_year+1900),
			(time1->tm_mon+1),
			time1->tm_mday,
			szHostname);

	return szDateTime;
}//End of GenerateVolumeName

/*===================================================================
   ROUTINE: calculateChecksum
=================================================================== */
//ACE_INT32 ACS_EMF_Common::calculateChecksum(const ACE_TCHAR* fileList[100], ACE_INT32 nooffiles, const ACE_TCHAR* fname)
//{
//	ACE_INT32 status  = EMF_RC_ERROR ;
//	ACE_TCHAR cmd[10000]= {'\0'};
//	ACE_INT32 ret = -1;
//
//	for(ACE_INT32 i = 0;i < nooffiles; i++){
//		if (ACS_EMF_Common::CheckFile(fileList[i]) == 1){
//			string str(fileList[i]);
//			if (i == 0)
//				sprintf(cmd,"%s %s %s%s%s%s","sha1sum",fileList[i]," | awk '{print $1 \"  /data/opt/ap/internal_root/media/",splitFilename(str),"\"}' >",fname);
//			else
//				sprintf(cmd,"%s %s %s%s%s%s","sha1sum",fileList[i]," | awk '{print $1 \"  /data/opt/ap/internal_root/media/",splitFilename(str),"\"}' >>",fname);
//		}
//		else if (ACS_EMF_Common::CheckFile(fileList[i]) == 2){
//			if (i == 0) sprintf(cmd,"%s %s %s%s","cd ",fileList[i],"; find . -type f -print0 | xargs -0 sha1sum | awk '{print $1 \"  /data/opt/ap/internal_root/media\" substr($2,2)}' >",fname);
//			else sprintf(cmd,"%s %s %s%s","cd ",fileList[i],"; find . -type f -print0 | xargs -0 sha1sum | awk '{print $1 \"  /data/opt/ap/internal_root/media\" substr($2,2)}' >>",fname);
//		}
//
//		int infp, outfp;
//		if ((pid_in_progress = popen_with_pid(cmd, &infp, &outfp)) <= 0){
//			ERROR(1,"Error in executing the %s",cmd);
//			return EMF_RC_ERROR;
//		}
//		DEBUG(1,"PID of background Job %d",pid_in_progress);
//		waitpid(pid_in_progress,&status,0);
//		if (status != EMF_RC_ERROR && WIFEXITED(status)){
//			ret=WEXITSTATUS(status);
//			pid_in_progress = EMF_RC_ERROR;
//			//DEBUG(1,"%s execution status = %d",cmd,ret);
//		}
//
//		if ( ret != 0){
//			ERROR(1,"%s failed",cmd);
//			return EMF_RC_ERROR;
//		}
//	}
//	if (ret == EMF_RC_OK )
//		return ret;
//	return ret;
//}//End of calculateChecksum

/*===================================================================
   ROUTINE: verifychecksum
=================================================================== */
//ACE_INT32 ACS_EMF_Common::verifychecksum()
//{
//	FILE *fp = NULL;
//	ACE_INT32 status = EMF_RC_ERROR;
//	ACE_TCHAR data[1000];
//	ACE_TCHAR cmd[100];
//	ACE_INT32 ret = EMF_RC_ERROR;
//	ACE_INT32 chksum = EMF_RC_ERROR;
//	sprintf(cmd,"%s %s %s","sha1sum -c  ",chksumFilewithFullPath.c_str()," 2>/dev/null | grep -v OK$ 1>/dev/null 2>/dev/null | wc -l");
//	fp = popen(cmd, mode);
//
//	if (fp == NULL){
//		ERROR(1,"Error in executing the %s",cmd);
//		return EMF_RC_ERROR;
//	}
//	if(GetHWVersion() !=3){
//		if((fgets(data, 1000, fp) != NULL)){
//			ACE_INT32 len = strlen(data);
//			if( data[len-1] == '\n' )
//				data[len-1] = '\0';
//			INFO(1,"No. of files failed in sha1sum verification = %s", data);
//			if (strcmp(data,"0") == 0) chksum = 0;
//		}}
//	status = pclose(fp);
//	if (status != EMF_RC_ERROR && WIFEXITED(status)){
//		ret=WEXITSTATUS(status);
//		//DEBUG(1,"%s execution status = %d\n",cmd,ret);
//	}
//	if (ret == EMF_RC_OK ){
//		if(GetHWVersion() == 3)
//			return EMF_RC_OK;
//		else{
//			if (chksum == EMF_RC_OK)
//				return EMF_RC_OK;
//			else
//				return EMF_RC_ERROR;
//		}
//	}
//	else{
//		ERROR(1,"%s failed",cmd);
//		return EMF_RC_ERROR;
//	}
//}//End of verifychecksum

/*===================================================================
   ROUTINE: getMediaSpaceInfo
=================================================================== */
ACE_INT32 ACS_EMF_Common::getMediaSpaceInfo(ACE_INT32 nodeState,ACE_TCHAR* availablespace,ACE_TCHAR* usedspace, ACE_TCHAR* totalspace)
{
	struct statvfs vfs;
	if(nodeState ==1)
		statvfs(ACS_EMF_ACTIVE_MEDIA, &vfs);
	else 
		statvfs(ACS_EMF_PASSIVE_MEDIA, &vfs);

	unsigned long total = vfs.f_blocks * vfs.f_frsize / 1024;
	unsigned long available = vfs.f_bavail * vfs.f_frsize / 1024;
	unsigned long free = vfs.f_bfree * vfs.f_frsize / 1024;
	unsigned long used = total - free;

//	DEBUG(1,"Total space  = %lu",total);
	sprintf(usedspace,"%lu",used);
	sprintf(totalspace,"%lu",total);
	sprintf(availablespace,"%lu",available);
	return 0;
}
ACE_INT32 ACS_EMF_Common::getMediaUsedSpaceMediaInfo(ACE_UINT64 *iUsedSpace)
{
	FILE *fp = NULL;
	ACE_TCHAR cmd[EMF_CMD_MAXLEN];
	ACE_INT32 retcode = -1;
	DEBUG(1,"%s", "ACS_EMF_Common::getMediaUsedSpaceMediaInfo - Entering");

	sprintf(cmd,"%s %s %s","dvd+rw-mediainfo",g_devicefile, "| grep \"CAPACITY\" ");

	fp = popen(cmd, "r");
	if (fp == NULL){
		DEBUG(1,"Error in executing the %s",cmd);
		DEBUG(1,"%s", "ACS_EMF_Common::getMediaUsedSpaceMediaInfo - Leaving");
		return -1;
	}
	//long int dvdcapacity= -1;
	ACE_TCHAR data[1000];
	while(fgets(data, 1000, fp) != NULL)
	{
		int data_len = strlen(data);
		if (data_len > 1) {
			if( data[data_len-1] == '\n' )  data[data_len-1] = '\0';}
		ACE_TCHAR token1[50],token2[50],token3[50];

		sscanf(data,"%s %s %s",token1, token2, token3);
		DEBUG(1,"ACS_EMF_Common::getMediaUsedSpaceMediaInfo(): token1 == %s token2 == %s  token3 == %s",token1,token2,token3);
		if(strcmp(token1,"READ") == 0 && strcmp(token2,"CAPACITY:") == 0){
			ACE_TCHAR *pch = strchr(token3, '=');
			DEBUG(1,"ACS_EMF_Common::getMediaUsedSpaceMediaInfo(): pch == %s", pch);
			if(pch != NULL && strlen(pch)>1) {
				//snprintf(usedspace, 20,"%s", pch+1);
				*iUsedSpace = strtoull(pch+1, NULL, 10);
				//dvdcapacity = atol(usedspace);
				retcode = 0;
			}
			break;
		}
	}
	DEBUG(1,"ACS_EMF_Common::getMediaUsedSpaceMediaInfo():  iUsedSpace = %ld (in bytes)", *iUsedSpace);

	int status = pclose(fp);
	int exitstatus = -1;
	if (WIFEXITED(status)) exitstatus=WEXITSTATUS(status);

	 if (exitstatus != 0 ) {DEBUG(1,"ACS_EMF_Common::getMediaUsedSpaceMediaInfo(): %s failed",cmd);}
	 else DEBUG(1,"ACS_EMF_Common::getMediaUsedSpaceMediaInfo(): %s successful executed",cmd);

	DEBUG(1,"%s", "ACS_EMF_Common::getMediaUsedSpaceMediaInfo - Leaving");
	return retcode;
}

ACE_INT32 ACS_EMF_Common::getMediaUsedSpaceIsoInfo(ACE_UINT64 *iUsedSpace)
{
	ACE_TCHAR *pCh = NULL;
	ACE_TCHAR szCmd[EMF_CMD_MAXLEN] = {'\0'};;
	ACE_TCHAR szData[EMF_CMD_MAXLEN] ={'\0'};
	ACE_INT32 iRet = -1;
	int iLen = 0;
	int iIndex = 0;
	int iClose = -1;
	ACE_UINT64 iDvdParam[2];
	// Initialization
	iDvdParam[0] = 0;
	iDvdParam[1] = 0;
	*iUsedSpace = 0;
	// Message
	DEBUG(1,"%s", "ACS_EMF_Common::getMediaUsedSpaceIsoInfo - Entering");
	// Command isoinfo. This script return isoinfo error code.
	sprintf(szCmd,"isoinfo -d -i %s 2>/dev/null | awk '/Logical block size is:|Volume size is:/';exit ${PIPESTATUS[0]}",g_devicefile);
	// Launch command
	FILE *fp = popen(szCmd, "r");
	// Check for error
	if(fp != NULL){
		while(fgets(szData, EMF_CMD_MAXLEN, fp) != NULL){
			// length of the string. fgets automatically appended a null char after the characters copied to szData
			iLen = strlen(szData);
			// Check the length of the string and if latest char is a new line
			if ((iLen > 1) && (szData[iLen - 1] == '\n')) {
				// Remove latest "\n"
				szData[iLen-1] = '\0';
			}
			// search ":"
			pCh = strchr(szData, ':');
			// Verify if ":" exist and if the next char is valid
			if((pCh != NULL) && ((pCh - szData + 1) < iLen)){
				// Set the pointer to next char (the number)
				++pCh;
				// Check the index
				if(iIndex < 2){
					// Store the parameter
					iDvdParam[iIndex] = strtoull(pCh, NULL, 10);
				}
			}
			// Next line
			++iIndex;
		}// while
	}else{
		// Unable to launch command
		DEBUG(1, "ACS_EMF_Common::getMediaUsedSpaceIsoInfo - Error to launch command: [%s]", szCmd);
	}
	// Close the pipe
	if(fp != NULL){
		iClose = pclose(fp);
	}
	// Recode the pclose return
	iClose = WEXITSTATUS(iClose);
	// Check return code
	if(iClose == 0){
		// Evalutate the used space
		*iUsedSpace = iDvdParam[0] * iDvdParam[1];
		// Print debug information
		DEBUG(1,"ACS_EMF_Common::getMediaUsedSpaceIsoInfo - Logical block size: [%llu]", iDvdParam[0]);
		DEBUG(1,"ACS_EMF_Common::getMediaUsedSpaceIsoInfo -        Volume size: [%llu]", iDvdParam[1]);
		// Set return code to ok
		iRet = 0;
	} else if(iClose == 1){
		// The DVD is not in iso9660 format. Probably not formatted. Assume used space at 0.
		// Print debug message
		DEBUG(1, "%s", "ACS_EMF_Common::getMediaUsedSpaceIsoInfo - isoinfo return that the dvd is not in iso9660 format. Assume used space 0.");
		// Set return code to ok
		iRet = 0;
	}else{
		// Error to close the pipe
		DEBUG(1, "ACS_EMF_Common::getMediaUsedSpaceIsoInfo - isoinfo or pclose return an error. Unable to get used space [%i].", iClose);
	}
	DEBUG(1,"%s", "ACS_EMF_Common::getMediaUsedSpaceIsoInfo - Leaving");
	// Exit from method
	return iRet;
}

ACE_INT32 ACS_EMF_Common::getMediaUsedSpace(ACE_UINT64 *iUsedSpace)
{
	ACE_INT32 iRet = EMF_RC_ERROR;	//Set tu error
	ACE_TCHAR mediumType [EMF_MEDIATYPE_MAXLEN] = {'\0'};
	DEBUG(1,"%s", "ACS_EMF_Common::getMediaUsedSpace - Entering");
	// Get Medium type: If the medium is DVD-R, DVD-RW, DVD+R then use dvd+rw-mediainfo
	// to get the used space on dvd. Else, if the medium is DVD+RW use
	if (ACS_EMF_Common::getMediumType(mediumType) == 0){
		if (ACE_OS::strcmp(mediumType,"DVD+RW") == 0){
			// Detect DVD+RW
			DEBUG(1,"%s", "ACS_EMF_Common::getMediaUsedSpace - Detect DVD+RW");
			// The DVD+RW have a track always closed. So the used space will be calculate with "isoinfo" because
			// dvd+rw-mediainfo does not work.
			iRet = ACS_EMF_Common::getMediaUsedSpaceIsoInfo(iUsedSpace);
		}else{
			// Other DVD type
			DEBUG(1,"%s", "ACS_EMF_Common::getMediaUsedSpace - Detect DVD+-R or -RW");
			// For others DVD type, dvd+rw-mediainfo work fine.
			iRet = ACS_EMF_Common::getMediaUsedSpaceMediaInfo(iUsedSpace);
		}
	}else{
		// Error to get medium type. Set used space to 0
		DEBUG(1,"%s", "ACS_EMF_Common::getMediaUsedSpace: Unable to get medium type. Set used space to 0");
		iUsedSpace = 0;
	}
	DEBUG(1,"%s", "ACS_EMF_Common::getMediaUsedSpace - Leaving");
	// Exit From method
	return (iRet);
}//end of getMediaUsedSpace

/*===================================================================
   ROUTINE: getMediaUsedSpaceOnPassive
=================================================================== */
ACE_INT32 ACS_EMF_Common::getMediaUsedSpaceOnPassive(ACE_TCHAR* usedspace)
{
	FILE *fp = NULL;
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_TCHAR data[1000];
	ACE_TCHAR cmd[100];
	ACE_INT32 ret = EMF_RC_ERROR;
	int ret2 = EMF_RC_ERROR;
	int isMediaMounted = 0;
	if(GetHWVariant() < 3){
		isMediaMounted = ACS_EMF_Common::isMediaAlreadyMountedOnPassive();
		if (isMediaMounted == 1) // Not mounted
		{
			ret2 = ACS_EMF_Common::mountDVDOnPassive();
			for(int i=3; i>0 ; i--)
			{
				if (ret2 == EMF_RC_ERROR)
				{
					sleep(10);
					ret2 = ACS_EMF_Common::mountDVDOnPassive();
				}
			}
		}
	}
	sprintf(cmd,"%s ","du -sk /media | awk '{print $1}'");
	fp = popen(cmd, mode);
        if(fp != NULL)
        {
		if((fgets(data, 1000, fp) != NULL)){
			int len = strlen(data);
			if( data[len-1] == '\n' )
				data[len-1] = '\0';

			DEBUG(1,"ACS_EMF_Common::getMediaUsedSpaceOnPassive - Used space = %s",data);
			sprintf(usedspace,"%s",data);
		}
		status = pclose(fp);
        }
        else{
                ERROR(1,"ACS_EMF_Common::getMediaUsedSpaceOnPassive - Error in executing the %s",cmd);
                return EMF_RC_ERROR;
        }

	if(GetHWVariant() < 3){
		if (isMediaMounted == 1)
		{
			ret2 = ACS_EMF_Common::unmountDVDOnPassive();
			for(int i=3; i>0 ; i--)
			{
				if (ret2 == EMF_RC_ERROR)
				{
					sleep(10);
					ret2 = ACS_EMF_Common::unmountDVDOnPassive();
				}
			}
		}
	}
	if (status != EMF_RC_ERROR && WIFEXITED(status)){
		ret=WEXITSTATUS(status);
		//DEBUG(1,"%s execution status = %d",cmd,ret);
	}
	if (ret == 0 )
		return ret;
	else{
		ERROR(1,"ACS_EMF_Common::getMediaUsedSpaceOnPassive - %s failed",cmd);
		return EMF_RC_ERROR;
	}
}//End of getMediaUsedSpaceOnPassive

/*===================================================================
   ROUTINE: getGivenFilesConsumedSpace
=================================================================== */
ACE_INT32 ACS_EMF_Common::getGivenFilesConsumedSpace(const ACE_TCHAR* fileList[100], ACE_INT32 nooffiles, ACE_TCHAR* usedspace)
{
	FILE *fp = NULL;
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_TCHAR data[1000];
	ACE_TCHAR cmd[1000];
	ACE_INT32 ret = EMF_RC_ERROR;
	sprintf(cmd,"%s ","du -sck ");
	for(ACE_INT32 i=0; i< nooffiles; i++)
	{
		ACE_OS::strcat(cmd,space);
		ACE_OS::strcat(cmd,fileList[i]);
	}
	const char* grep_mesg = " | grep total | awk '{print $1}' 2>/dev/null";
	ACE_OS::strcat(cmd,grep_mesg);

	fp = popen(cmd, mode);
        if (fp !=NULL){
		if((fgets(data, 1000, fp) != NULL)){
			int len = strlen(data);
			if( data[len-1] == '\n' )
				data[len-1] = '\0';

			DEBUG(1,"ACS_EMF_Common::getGivenFilesConsumedSpace - Total files size = %s (in KB)",data);
			sprintf(usedspace,"%s",data);
		}
		status = pclose(fp);
        }
        else{
                ERROR(1,"ACS_EMF_Common::getGivenFilesConsumedSpace - Error in executing the %s",cmd);
                return EMF_RC_ERROR;
        }
	if (status != EMF_RC_ERROR && WIFEXITED(status)){
		ret=WEXITSTATUS(status);
		//DEBUG(1,"%s execution status = %d\n",cmd,ret);
	}
	if (ret == 0 )
		return ret;
	else{
		ERROR(1,"ACS_EMF_Common::getGivenFilesConsumedSpace - %s failed",cmd);
		return EMF_RC_ERROR;
	}

}//End of getGivenFilesConsumedSpace

/*===================================================================
   ROUTINE: removeTemporaryImage
=================================================================== */
ACE_INT32 ACS_EMF_Common::removeTemporaryImage(const ACE_TCHAR* imageName)
{
	FILE *filePtr= NULL;
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_TCHAR cmd[300];
	ACE_INT32 ret = EMF_RC_ERROR;
	DEBUG(1,"%s","ACS_EMF_Common::removeTemporaryImage - Entering");
	if(GetHWVariant() >= 3)
		sprintf(cmd,"find /data/acs/data/emf/ | awk 'NR>1' | grep -v mediaInfo.conf | xargs rm -rf %s","&>/dev/null");
	else
		sprintf(cmd,"%s %s%s%s %s %s","rm -f ",imageFilePath.c_str(),imageName,".iso ",chksumFilewithFullPath.c_str(),"1>/dev/null 2>/dev/null");

	filePtr = popen(cmd, mode);
	if (filePtr == NULL){
		ERROR(1,"ACS_EMF_Common::removeTemporaryImage - Error in executing the %s",cmd);
		DEBUG(1,"%s","ACS_EMF_Common::removeTemporaryImage - Leaving");
		return EMF_RC_ERROR;
	}
	status = pclose(filePtr);
	if (status != EMF_RC_ERROR && WIFEXITED(status)){
		ret=WEXITSTATUS(status);
		DEBUG(1,"ACS_EMF_Common::removeTemporaryImage - %s execution status = %d\n",cmd,ret);
	}
	if (ret == 0 ){
		DEBUG(1,"%s","ACS_EMF_Common::removeTemporaryImage - Leaving");
		return ret;
	}
	else{
		ERROR(1,"%s failed",cmd);
		DEBUG(1,"%s","ACS_EMF_Common::removeTemporaryImage - Leaving");
		return EMF_RC_ERROR;
	}
}//End of removeTemporaryImage

/*===================================================================
   ROUTINE: getDVDOwner
=================================================================== */
ACE_INT32 ACS_EMF_Common::getDVDOwner(const ACE_TCHAR* hostname, bool remote)
{
	FILE *fp = NULL;
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_TCHAR cmd[100];
	ACE_INT32 ret = EMF_RC_ERROR;

	//	if (strcmp(hostname,EMF_NODEA) == 0 ||  strcmp(hostname,EMF_NODEB) == 0)
	//	{
	if (!remote)
		sprintf(cmd,"%s","lsscsi | grep DVD 1>/dev/null 2>/dev/null");
	else
		sprintf(cmd,"%s%s%s","rsh ",hostname," lsscsi | grep DVD 1>/dev/null 2>/dev/null");

	fp = popen(cmd, mode);
        if (fp != NULL)
		status = pclose(fp);
        else{
                ERROR(1,"ACS_EMF_Common::getDVDOwner - Error in executing the %s",cmd);
                return EMF_RC_ERROR;
        }

	if (status != EMF_RC_ERROR && WIFEXITED(status)){
		ret=WEXITSTATUS(status);
		//DEBUG(1,"%s execution status = %d\n",cmd,ret);
	}
	if (ret == 0 )
		return ret;
	else{
		ERROR(1,"ACS_EMF_Common::getDVDOwner - %s failed",cmd);
		return EMF_RC_ERROR;
	}
}//End of getDVDOwner

/*===================================================================
   ROUTINE: fetchEMFHistoryAttribute
=================================================================== */
ACE_INT32 ACS_EMF_Common::fetchEMFHistoryAttribute(std::string& aObjectDN , char* attrName,char* pszConfigType)
{
	ACS_CC_ImmParameter paramToFind;
	OmHandler* omHandlerPtr = new OmHandler ();
	paramToFind.attrName = new char [30];
	ACS_CC_ReturnType enResult;
	char* pszAttrValue;
	if( omHandlerPtr->Init() == ACS_CC_FAILURE ){
		delete [] paramToFind.attrName;
		return 117;
	}
	ACE_OS::strcpy( paramToFind.attrName , attrName);
	if( ( enResult = omHandlerPtr->getAttribute( aObjectDN.c_str(), &paramToFind ) ) == ACS_CC_SUCCESS ){
		pszAttrValue = (reinterpret_cast<char*>(*(paramToFind.attrValues)));
		ACE_OS::sprintf(pszConfigType,"%s", pszAttrValue);
	}
	else{
		omHandlerPtr->Finalize();
		delete [] paramToFind.attrName;
		delete omHandlerPtr;
		return 117;
	}
	omHandlerPtr->Finalize();
	delete [] paramToFind.attrName;
	delete omHandlerPtr;
	return 0;
}//End of fetchEMFHistoryAttribute
/*===================================================================
   ROUTINE: fetchEMFHistoryIntegerAttribute
=================================================================== */
ACE_INT32 ACS_EMF_Common::fetchEMFHistoryIntegerAttribute(std::string& aObjectDN , char* attrName,int& operType)
{
	ACS_CC_ImmParameter paramToFind;
	OmHandler* omHandlerPtr = new OmHandler ();
	paramToFind.attrName = new char [30];
	ACS_CC_ReturnType enResult;
	if( omHandlerPtr->Init() == ACS_CC_FAILURE ){
		delete [] paramToFind.attrName;
		return 117;
	}
	ACE_OS::strcpy( paramToFind.attrName , attrName);
	if( ( enResult = omHandlerPtr->getAttribute( aObjectDN.c_str(), &paramToFind ) ) == ACS_CC_SUCCESS ){
		operType = *(reinterpret_cast<int*>(*(paramToFind.attrValues)));
	}
	else{
		omHandlerPtr->Finalize();
		delete [] paramToFind.attrName;
		delete omHandlerPtr;
		return 117;
	}
	omHandlerPtr->Finalize();
	delete [] paramToFind.attrName;
	delete omHandlerPtr;
	return 0;
}//End of fetchEMFHistoryIntegerAttribute
/*===================================================================
   ROUTINE: s32Spawn
=================================================================== */
ACE_INT32 ACS_EMF_Common::s32Spawn(ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),void * Arglist,const
		ACE_TCHAR* lpszThreadName,ACE_INT32 s32GrpId)
{
	ACE_INT32 s32Result =  ACE_Thread_Manager::instance()->spawn( ThreadFunc,
			Arglist,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			0,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			s32GrpId,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&lpszThreadName);


	return s32Result;
}//End of s32Spawn
/*===================================================================
   ROUTINE: StartThread
=================================================================== */
ACE_INT32 ACS_EMF_Common::StartThread( ACE_THR_FUNC_RETURN (*ThreadFunc)(void*),
		void* Arglist,
		const ACE_TCHAR* lpszThreadName,
		ACE_INT32 s32GrpId)
{

	ACE_INT32 s32Result =  s32Spawn( ThreadFunc,
			Arglist,
			lpszThreadName,
			s32GrpId);

	if ( s32Result == EMF_RC_ERROR )
	{
		// Report error
		ACE_TCHAR szData[256];

		snprintf(szData,sizeof(szData)/sizeof(*szData) - 1,
				ACE_TEXT("Unable to create thread: %s due to the following error:\n %m"),
				lpszThreadName);
		ERROR(1,"%s",szData);

		//ACE_DEBUG((LM_INFO, ACE_TEXT(szData)));

		//! Send event
		/*(void)ACS_EMF_AEH::ReportEvent(1002,
                                     ACE_TEXT("EVENT"),
                                     CAUSE_AP_INTERNAL_FAULT,
                                     ACE_TEXT(""),
                                     szData,
                                     ACE_TEXT("CREATE THREAD FAILED IN EMF")); */
	}

	return s32Result;

}//End of StartThread
/*===================================================================
   ROUTINE: setErrorText
=================================================================== */
ACS_CC_ReturnType ACS_EMF_Common::setErrorText(unsigned long long ccbid, int errorId, std::string errorText)
{
	ACS_CC_ReturnType result = ACS_CC_SUCCESS;
	DEBUG(1,"ERROR TEXT: %s",errorText.c_str());
	DEBUG(1,"ERROR ID: %d",errorId);
	DEBUG(1,"ccbID: %ld",ccbid);
#if 0
	// modification of ErrorHandler Runtime Object.

	//char* objName = (char *)theEMFErrorObjectDn;
	std::string dnOfErrorhandlerObj(theEMFErrorHandlerRDNValue);
	dnOfErrorhandlerObj.append(",");
	dnOfErrorhandlerObj.append(ACS_EMF_Common::parentDNofEMF);

	ACS_CC_ImmParameter parToModify;

	// ccbId Modification
	parToModify.attrName = (char *)attrEMFCCID;
	parToModify.attrType = ATTR_UINT64T;
	parToModify.attrValuesNum = 1;
	void* valuesccb[1] = {reinterpret_cast<void*>(&ccbid) };
	parToModify.attrValues = valuesccb;

	result = errorObj.modifyRuntimeObj(dnOfErrorhandlerObj.c_str(), &parToModify);

	if (result != ACS_CC_SUCCESS )
		DEBUG(1,"%s","Modification of ccbId in EMF ErrorHandler Object is failed\n");

	// errorId Modification

	parToModify.attrName = (char *)attrEMFErrorId;
	parToModify.attrType = ATTR_INT32T;
	parToModify.attrValuesNum = 1;
	void* valuesERR[1] = {reinterpret_cast<void*>(&errorId)};
	parToModify.attrValues = valuesERR;

	result = errorObj.modifyRuntimeObj(dnOfErrorhandlerObj.c_str(), &parToModify);
	if (result != ACS_CC_SUCCESS )
	{
		DEBUG(1,"%s","Modification of errorId in EMF ErrorHandler Object is failed\n");

	}

	// errorText Modification

	parToModify.attrName = (char *)attrEMFErrorText;
	parToModify.attrType = ATTR_STRINGT;
	parToModify.attrValuesNum = 1;
	void* new_valueString[1]={reinterpret_cast<void*>((char *)errorText.c_str())};
	parToModify.attrValues = new_valueString;

	result = errorObj.modifyRuntimeObj(dnOfErrorhandlerObj.c_str(), &parToModify);

	if (result != ACS_CC_SUCCESS )
	{
		DEBUG(1,"%s","Modification of errorText in EMF ErrorHandler Object is failed\n");

	}
#endif
	return result;
}
/*===================================================================
   ROUTINE: ValidateFilename
=================================================================== */
bool ACS_EMF_Common::ValidateFilename(const ACE_TCHAR* lpszFilename,ACE_INT32 fileDirectoryType)
{
	const ACE_TCHAR* lpszInvalidChar;
	size_t nLen = ACE_OS::strlen(lpszFilename);
	for (size_t nIdx = 0; nIdx < nLen; nIdx++){
		if (fileDirectoryType == 2 )
			lpszInvalidChar = ACE_OS::strchr(EMF_INVALID_PATHNAME_CHARS, lpszFilename[nIdx]);
		else
			lpszInvalidChar = ACE_OS::strchr(EMF_INVALID_FILENAME_CHARS, lpszFilename[nIdx]);
		if (lpszInvalidChar != NULL)
			return false;
	}
	return true;
} // End of ValidateFilename

/*===================================================================
   ROUTINE: ValidateFileArgumentToMedia
=================================================================== */
ACE_INT32 ACS_EMF_Common::ValidateFileArgumentToMedia(const ACE_TCHAR* lpszFilePath)
{
	ACE_INT32 nLen = ACE_OS::strlen(lpszFilePath);
	if (nLen > EMF_MAX_SOURCE)
		return EMF_RC_SOURCETOOLONG;
	ACE_INT32 nFileType = ACS_EMF_Common::CheckFile(lpszFilePath);
	DEBUG(1,"ACS_EMF_Common::ValidateFileArgumentToMedia - Return value of checkfile: %d",nFileType);
	if(nFileType == 0)
		return EMF_RC_PERMISSIONSDENIED;
	if(nFileType != 1 && nFileType != 2)
		return EMF_RC_FILENOTFOUND;
	if(!ACS_EMF_Common::ValidateFilename(lpszFilePath,nFileType)){
		if(nFileType == 1 )
			return EMF_RC_INVALIDFILENAME;
		else
			return EMF_RC_INVALIDPATHNAME;
	}
	return EMF_RC_OK;
}//End of ValidateFileArgumentToMedia

/*===================================================================
   ROUTINE: ValidateFileArgumentFromMedia
=================================================================== */
ACE_INT32 ACS_EMF_Common::ValidateFileArgumentFromMedia(const ACE_TCHAR* lpszFilePath)
{
	ACE_INT32 nLen = ACE_OS::strlen(lpszFilePath);
	if (nLen > EMF_MAX_SOURCE)
		return EMF_RC_SOURCETOOLONG;
	ACE_INT32 nFileType = ACS_EMF_Common::CheckFile(lpszFilePath);
	if(nFileType == 0)
		return EMF_RC_PERMISSIONSDENIED;
	if(nFileType == 3)
		return EMF_RC_CANNOTACCESSTOMEDIA;
	if(nFileType != 2)
		return EMF_RC_PATHNOTFOUND;
	if(!ACS_EMF_Common::ValidateFilename(lpszFilePath,2))
		return EMF_RC_INVALIDPATHNAME;
	if(ACS_EMF_Common::CheckDirForWriteAccess(lpszFilePath) != 0)
		return EMF_RC_PERMISSIONSDENIED;
	return EMF_RC_OK;
}//End of ValidateFileArgumentFromMedia

/*===================================================================
   ROUTINE: ValidateForSpecificFileCopy
=================================================================== */
ACE_INT32 ACS_EMF_Common::ValidateForSpecificFileCopy(const ACE_TCHAR* lpszFilePath)
{
	ACE_TCHAR filepath [1024];
	//	ACE_OS::strcpy(filepath,ACE_TEXT("/data/opt/ap/internal_root/media"));
	//	ACE_OS::strcat(filepath,lpszFilePath);
	ACE_OS::strcpy(filepath,lpszFilePath);
	//DEBUG(1,"FILE PATH Recieved = %s",filepath);
	// Commented as per latest model design
	//mountDVDData();
	ACE_INT32 nLen = ACE_OS::strlen(filepath);
	if (nLen > EMF_MAX_SOURCE)
		return EMF_RC_SOURCETOOLONG;
	ACE_INT32 nFileType = ACS_EMF_Common::CheckFile(filepath);

	// Commented as per latest model design
	//unmountDVDData();
	if(nFileType == 0)
		return EMF_RC_PERMISSIONSDENIED;
	if(nFileType == 3)
		return EMF_RC_CANNOTACCESSTOMEDIA;
	if(nFileType != 1 && nFileType != 2)
		return EMF_RC_FILENOTFOUND;
	// TO INCLUDE SUPPORT FOR FOLDERS ALSO.
	//	if (nFileType == 2)
	//		return EMF_RC_INVALIDFILENAME;
	if(!ACS_EMF_Common::ValidateFilename(lpszFilePath,nFileType))
		return EMF_RC_INVALIDFILENAME;
	return EMF_RC_OK;
}//End of ValidateForSpecificFileCopy

/*===================================================================
   ROUTINE: ValidateLabel
=================================================================== */
ACE_INT32 ACS_EMF_Common::ValidateLabel(const ACE_TCHAR* lpszLabel)
{
	size_t nLen = ACE_OS::strlen(lpszLabel);
	//DEBUG(1,"GetHWVersion() %d",GetHWVersion());
	//DEBUG(1,"GetHWVariant(): %d",GetHWVariant());
	//DEBUG(1,"size_t nLen %d",nLen);
	if(GetHWVariant() >= 3)
	{
		if (nLen == 0)
			return EMF_RC_INVALIDLABELNAME;
		else if(nLen > 16)
			return EMF_RC_LABELNAMETOOLONG;
	}
	else{
		if (nLen == 0 || nLen > 32)
			return EMF_RC_LABELNAMETOOLONG;}
	const ACE_TCHAR* lpszValidChar;
	for (size_t nIdx = 0; nIdx < nLen; nIdx++){
		lpszValidChar = ACE_OS::strchr(EMF_VALID_LABEL_CHARS, lpszLabel[nIdx]);
		if (!lpszValidChar)
			return EMF_RC_INVALIDLABELNAME;
	}
	return EMF_RC_OK;
}//End of ValidateLabel

/*===================================================================
   ROUTINE: ExpandPath
=================================================================== */
void ACS_EMF_Common::ExpandPath(std::string& lpszPath)
{
	string fileMFuncName = "internalRoot";
	std::string path(lpszPath);
	ACS_APGCC_CommonLib objCommonLib;
	char folderPath[1024] = { 0 };
	int folderPathLen = 1024;
	ACS_APGCC_DNFPath_ReturnTypeT returnType = ACS_APGCC_DNFPATH_FAILURE;

	returnType = objCommonLib.GetFileMPath(fileMFuncName.c_str(),
			folderPath,
			folderPathLen );

	if( returnType != ACS_APGCC_DNFPATH_SUCCESS )
	{
		// DEBUG("%s", " Error occured while getting the InternalRoot Path.");
	}

	lpszPath.clear();
	lpszPath.append(folderPath);
	lpszPath.append(path);
} // End of ExpandPath

/*===================================================================
   ROUTINE: CheckValidNodename
=================================================================== */
bool ACS_EMF_Common::CheckValidNodename(const ACE_TCHAR* lpszNodename)
{
	bool bFound = false;
	if (ACE_OS::strcmp(lpszNodename,EMF_NODEA) == 0 || ACE_OS::strcmp(lpszNodename,EMF_NODEB) == 0)
		bFound = true;
	return bFound;
	//return 0;
}//End of CheckValidNodename

/*===================================================================
   ROUTINE: setMediaOperationStateToIDLE
=================================================================== */
void ACS_EMF_Common::setMediaOperationStateToIDLE()
{
	stateOfDVDOperation = EMF_MEDIA_OPERATION_IS_IDLE;
}//End of setMediaOperationStateToIDLE

/*===================================================================
   ROUTINE: setMediaOperationStateToProgress
=================================================================== */
void ACS_EMF_Common::setMediaOperationStateToProgress(acs_emf_operation_status_t state)
{
	stateOfDVDOperation = state;
}//End of setMediaOperationStateToProgress

/*===================================================================
   ROUTINE: getMediaOperationState
=================================================================== */
acs_emf_operation_status_t ACS_EMF_Common::getMediaOperationState()
{
	return stateOfDVDOperation;
}//End of getMediaOperationState

/*===================================================================
   ROUTINE: createDirectory
=================================================================== */
void ACS_EMF_Common::createDirectory(const ACE_TCHAR* lpszDirname)
{
	ACE_INT32 nFileType = ACS_EMF_Common::CheckFile(lpszDirname);
	if (nFileType == EMF_RC_ERROR){
		if (mkdir(lpszDirname,775) == EMF_RC_ERROR)
		{
			DEBUG(1,"%s","ACS_EMF_Common::createDirectory - Directory creation failed ");
		}
	}
	else
		DEBUG(1,"ACS_EMF_Common::createDirectory - [%s] is Already Exists",lpszDirname);
}//End of createDirectory

/*========================================================================
	ROUTINE:	GetDataDiskPath
==========================================================================*/
bool ACS_EMF_Common::GetDataDiskPath( std::string& pszLogicalName, std::string&  szPath)
{
	ACS_APGCC_CommonLib myAPGCCCommonLib;
	ACS_APGCC_DNFPath_ReturnTypeT  errorCode;
	char* pszPath = new char[1000];
	int myLen = 100;
	errorCode = myAPGCCCommonLib.GetDataDiskPath(pszLogicalName.c_str(),pszPath,myLen) ;
	bool myResult = false;
	if ( errorCode == ACS_APGCC_STRING_BUFFER_SMALL ){
		ERROR(1,"%s","ACS_EMF_Common::GetDataDiskPath - The return code is : ACS_APGCC_STRING_BUFFER_SMALL ");
		delete []pszPath;
	}
	else if ( errorCode == ACS_APGCC_FAULT_LOGICAL_NAME ){
		ERROR(1,"%s","ACS_EMF_Common::GetDataDiskPath - The return code is : ACS_APGCC_FAULT_LOGICAL_NAME ");
		delete []pszPath;
	}
	else if ( errorCode == ACS_APGCC_DNFPATH_SUCCESS ){
		INFO(1,"%s","ACS_EMF_Common::GetDataDiskPath - The return code is : ACS_APGCC_DNFPATH_SUCCESS ");
		std::string myStr(pszPath);
		szPath = myStr;
		//		   INFO("The pszPath is : ",pszPath);
		delete []pszPath;
		myResult = true;
	}
	else if ( errorCode == ACS_APGCC_DNFPATH_FAILURE ){
		ERROR(1,"%s","ACS_EMF_Common::GetDataDiskPath - The return code is  ACS_APGCC_DNFPATH_FAILURE ");
		delete []pszPath;
	}
	return myResult;
}//End of GetDataDiskPath

/*========================================================================
	ROUTINE:	killAllOutstandingPIDs
==========================================================================*/
ACE_INT32 ACS_EMF_Common::killAllOutstandingPIDs()
{
	ACE_TCHAR cmd[EMF_CMD_MAXLEN];
	ACE_INT32 rCode=0;
	DEBUG(1,"ACS_EMF_Common::killAllOutstandingPIDs - Process id to be Killed :%d",pid_in_progress);
	sprintf(cmd,"%s %s %d", ACS_EMF_CMD_NFS_OPERATIONS, ACS_EMF_CLEANUP_OPTS, pid_in_progress);
	if(_execlp(cmd) != EMF_RC_OK){
		DEBUG(1,"%s","ACS_EMF_Common::killAllOutstandingPIDs - Kill All Pid is Failed");
		rCode = EMF_RC_ERROR;
	}else{
		DEBUG(1,"%s","ACS_EMF_Common::killAllOutstandingPIDs - Kill All Pid is Successfull");
	}
	return rCode;
}//End of killAllOutstandingPIDs

/*========================================================================
	ROUTINE:	fetchDnOfRootObjFromIMM
==========================================================================*/
int ACS_EMF_Common::fetchDnOfRootObjFromIMM ()
{
	//static int infoAlreadyLoad = 0;
	static char dnOfEMFRootObj[512] = {0};
	OmHandler omHandler;

	if (omHandler.Init() != ACS_CC_SUCCESS)
		return EMF_RC_ERROR;

	std::vector<std::string> dnList;
	int returnCode = EMF_RC_OK;
	if ((omHandler.getClassInstances(ACS_EMF_ROOT_CLASS_NAME, dnList)) != ACS_CC_SUCCESS)
		returnCode = EMF_RC_ERROR;//ERROR
	else
	{
		//OK: Checking how much dn items was found in IMM
		if (dnList.size() ^ 1)
			returnCode = EMF_RC_ERROR;//WARNING: 0 (zero) or more than one node found
		else{
			//OK: Only one root node
			strncpy(dnOfEMFRootObj, dnList[0].c_str(), ACS_EMF_ARRAY_SIZE(dnOfEMFRootObj));
			dnOfEMFRootObj[ACS_EMF_ARRAY_SIZE(dnOfEMFRootObj) - 1] = 0;
			//infoAlreadyLoad = 1;
			parentDNofEMF = std::string(dnOfEMFRootObj);
		}
	}
	omHandler.Finalize();
	return returnCode;
}//End of fetchDnOfRootObjFromIMM

//-----------------------------------------------------------------------
ACE_INT32 ACS_EMF_Common::ejectDVDMedia()
{

	ACE_INT32 status = EMF_RC_ERROR;
	ACE_TCHAR cmd[32];
	ACE_INT32 ret = EMF_RC_ERROR;
	DEBUG(1,"%s","ACS_EMF_Common::ejectDVDMedia - Entering");
	sprintf(cmd,"cdrecord -eject %s ","1>/dev/null 2>/dev/null");
	int infp, outfp;
	if ((pid_in_progress = popen_with_pid(cmd, &infp, &outfp)) <= 0) {
		ERROR(1,"ACS_EMF_Common::ejectDVDMedia - Error in executing the %s",cmd);
		return EMF_RC_ERROR;
	}
	DEBUG(1,"ACS_EMF_Common::ejectDVDMedia - PID of background Job %d",pid_in_progress);
	waitpid(pid_in_progress,&status,0);
	if (WIFEXITED(status) ){
		ret=WEXITSTATUS(status);
		pid_in_progress = EMF_RC_ERROR;
		//DEBUG(1,"%s execution status = %d\n",cmd,ret);
	}
	if (ret == 0 ){
		INFO(1,"%s","ACS_EMF_Common::ejectDVDMedia - Ejecting of media is Success");
		return EMF_RC_OK;
	}
	DEBUG(1,"%s","ACS_EMF_Common::ejectDVDMedia - Leaving");
	return EMF_RC_ERROR;

}//End of ejectDVDMedia

//-----------------------------------------------------------------------
ACE_INT32 ACS_EMF_Common::setupNFSOnActive()
{
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
	ACE_INT32 rCode=EMF_RC_OK;
	DEBUG(1,"%s","ACS_EMF_Common::setupNFSOnActive - Entering");
	if(GetHWVariant() < 3){
		sprintf(cmd,"%s","/opt/ap/acs/bin/acs_emf_activeshare.sh");
		INFO(1,"%s","ACS_EMF_Common::setupNFSOnActive - Calling  '/opt/ap/acs/bin/acs_emf_activeshare.sh' ...");
	}else{
		sprintf(cmd,"%s %s",ACS_EMF_CMD_NFS_OPERATIONS, ACS_EMF_EXPORT_NBI_OPTS);
		INFO(1,"%s [%s]","ACS_EMF_Common::setupNFSOnActive - Calling:", cmd);
	}

	if(_execlp(cmd) != EMF_RC_OK){
		DEBUG(1,"%s","ACS_EMF_Common::setupNFSOnActive - Sharing of NBI path is Failed");
		rCode = EMF_RC_ERROR;
	}else{
		DEBUG(1,"%s","ACS_EMF_Common::setupNFSOnActive - Sharing of NBI path is Successfull");
	}
	DEBUG(1,"%s","ACS_EMF_Common::setupNFSOnActive - Leaving");
	return rCode;
}//end of setupNFSOnActive

//-----------------------------------------------------------------------
ACE_INT32 ACS_EMF_Common::mountMediaOnOtherNode()
{
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
	ACE_INT32 rCode=EMF_RC_OK;
	DEBUG(1,"%s","ACS_EMF_Common::mountMediaOnOtherNode - Entering");
	if(getNodeState() == 1){
		sprintf(cmd,"%s %s %s %s", ACS_EMF_CMD_NFS_OPERATIONS, ACS_EMF_MOUNT_NFS_OPTS, ACS_EMF_PASSIVE_MEDIA, ACS_EMF_ACTIVE_DATA);
		DEBUG(1,"ACS_EMF_Common::mountMediaOnOtherNode - launch[%s]", cmd);
	}else{
		sprintf(cmd,"%s %s %s %s", ACS_EMF_CMD_NFS_OPERATIONS, ACS_EMF_MOUNT_NFS_OPTS, ACS_EMF_ACTIVE_MEDIA, ACS_EMF_ACTIVE_DATA);
		DEBUG(1,"ACS_EMF_Common::mountMediaOnOtherNode - launch[%s]", cmd);
	}	
	
	if(_execlp(cmd) != EMF_RC_OK){
		DEBUG(1,"%s","ACS_EMF_Common::mountMediaOnOtherNode - Mounting of thumbdrive path on node is Failed");
		rCode = EMF_RC_ERROR;
	}else{
		DEBUG(1,"%s","ACS_EMF_Common::mountMediaOnOtherNode - Mounting of thumbdrive path on node is Successfull");
	}
	DEBUG(1,"%s","ACS_EMF_Common::mountMediaOnOtherNode - Leaving");
	return rCode;
}//end of mountMediaOnOtherNode

//-----------------------------------------------------------------------
ACE_INT32 ACS_EMF_Common::moutFileMOnPassive()
{
	ACE_TCHAR cmd[EMF_CMD_MAXLEN];
	ACE_TCHAR peer_id[10]; // peer ID
	DEBUG(1,"%s","ACS_EMF_Common::moutFileMOnPassive - Entering");
	ACS_EMF_Common::getNodePeerID(peer_id);
	ACE_INT32 rCode=EMF_RC_OK;
	sprintf(cmd,"%s %s %s %s",ACS_EMF_CMD_NFS_OPERATIONS, ACS_EMF_MOUNT_NFS_OPTS, NBI_root, ACS_EMF_ACTIVE_DATA);
	DEBUG(1,"ACS_EMF_Common::moutFileMOnPassive - launch [%s]", cmd);
	if(_execlp(cmd) != EMF_RC_OK){
		DEBUG(1,"%s","ACS_EMF_Common::moutFileMOnPassive - Mounting of FileM path on node is Failed");
		rCode = EMF_RC_ERROR;
	}else{
		DEBUG(1,"%s","ACS_EMF_Common::moutFileMOnPassive - Mounting of FileM path on node is Successfull");
	}
	DEBUG(1,"%s","ACS_EMF_Common::moutFileMOnPassive - Leaving");
	return rCode;

}//end of moutFileMOnPassive

//-----------------------------------------------------------------------
ACE_INT32 ACS_EMF_Common::moutFileMOnPassive(int32_t sysId, acs_dsd::NodeSideConstants nodeSide)
{
	ACE_INT32 retCode = EMF_RC_ERROR;
	char peerNodeIp[EMF_IPADDRESS_MAXLEN]= {0};
	DEBUG(1,"%s","ACS_EMF_Common::moutFileMOnPassive(int32_t,acs_dsd::NodeSideConstants) - Entering");
	if ( getNodeClusterIp(peerNodeIp, sysId, nodeSide) ){
		ERROR(1,"%s","ACS_EMF_Common::moutFileMOnPassive(int32_t,acs_dsd::NodeSideConstants) - Call 'getNodeClusterIp()' failure! Cannot Mount the device on Active node!!!");
		DEBUG(1,"%s","ACS_EMF_Common::moutFileMOnPassive(int32_t,acs_dsd::NodeSideConstants) - Entering");
		return EMF_RC_ERROR;
	}
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] ={0};
	sprintf(cmd,"mount -t nfs %s:/data/opt/ap/internal_root /active_data %s",peerNodeIp,"1>/dev/null 2>/dev/null");
	int infp, outfp;
	if ((pid_in_progress = popen_with_pid(cmd, &infp, &outfp)) <= 0){
		ERROR(1,"ACS_EMF_Common::moutFileMOnPassive(int32_t,acs_dsd::NodeSideConstants) - Error in executing the %s",cmd);
		DEBUG(1,"%s","ACS_EMF_Common::moutFileMOnPassive(int32_t,acs_dsd::NodeSideConstants) - Entering");
		return EMF_RC_ERROR;
	}
	INFO(1,"ACS_EMF_Common::moutFileMOnPassive(int32_t,acs_dsd::NodeSideConstants) - PID of background Job %d",pid_in_progress);
	ACE_INT32 status = EMF_RC_ERROR;
	waitpid(pid_in_progress,&status,0);

	if (WIFEXITED(status) ){
		retCode=WEXITSTATUS(status);
		pid_in_progress = EMF_RC_ERROR;
		INFO(1,"ACS_EMF_Common::moutFileMOnPassive(int32_t,acs_dsd::NodeSideConstants) - %s execution status = %d",cmd,retCode);
	}

	if(retCode == 0){
		INFO(1,"ACS_EMF_Common::moutFileMOnPassive(int32_t,acs_dsd::NodeSideConstants) - %s successfully executed: FileM has been mounted on DVD-owner node!", cmd)
	}else  ERROR(1,"ACS_EMF_Common::moutFileMOnPassive(int32_t,acs_dsd::NodeSideConstants) - %s Failed: FileM has been NOT mounted on DVD-owner node!", cmd)

	DEBUG(1,"%s","ACS_EMF_Common::moutFileMOnPassive(int32_t,acs_dsd::NodeSideConstants) - Entering");
	return retCode;

}//end of moutFileMOnPassive

//removeNFSOnActive
//-----------------------------------------------------------------------
ACE_INT32 ACS_EMF_Common::removeNFSOnActive()
{
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
	ACE_INT32 rCode=EMF_RC_OK;
	DEBUG(1,"%s","ACS_EMF_Common::removeNFSOnActive - Entering");
	GetHWVersion();
	if(GetHWVariant() < 3){
		// stop nfsserver
		sprintf(cmd,"%s","/opt/ap/acs/bin/acs_emf_removeshare.sh");
		INFO(1,"%s","ACS_EMF_Common::removeNFSOnActive - Calling  '/opt/ap/acs/bin/acs_emf_removeshare.sh' ...");
	} else {
		sprintf(cmd,"%s %s",ACS_EMF_CMD_NFS_OPERATIONS, ACS_EMF_RMVE_NBI_OPTS);
		INFO(1,"ACS_EMF_Common::removeNFSOnActive - %s [%s]","Calling:", cmd);
	}
	if(_execlp(cmd) != EMF_RC_OK){
		DEBUG(1,"%s","ACS_EMF_Common::removeNFSOnActive - Removing of NBI path on node is Failed");
		rCode = EMF_RC_ERROR;
	}else{
		DEBUG(1,"%s","ACS_EMF_Common::removeNFSOnActive - Removing of NBI path on node is Successfull");
	}
	DEBUG(1,"%s","ACS_EMF_Common::removeNFSOnActive - Leaving");
	return rCode;
}//end of removeNFSOnActive

//-----------------------------------------------------------------------
ACE_INT32 ACS_EMF_Common::unmountFileM()
{
	ACE_INT32 rCode=EMF_RC_OK;
	DEBUG(1,"%s","ACS_EMF_Common::unmountFileM - Entering");
	if(isFileMmounted()) {
		ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
		sprintf(cmd,"%s %s %s",ACS_EMF_CMD_GNRL_OPERATIONS, ACS_EMF_UNMOUNT_OPTS, ACS_EMF_ACTIVE_DATA);
		if(_execlp(cmd) != EMF_RC_OK){
			DEBUG(1,"%s","ACS_EMF_Common::unmountFileM - Failed to Unmount the fileM");
			rCode = EMF_RC_ERROR;
		}else{
			DEBUG(1,"%s","ACS_EMF_Common::unmountFileM - fileM has been unmounted successfully");
		}
	}
	else {
		DEBUG(1,"%s","ACS_EMF_Common::unmountFileM - fileM is not mounted");
	}
	DEBUG(1,"%s","ACS_EMF_Common::unmountFileM - Leaving");
	return rCode;
}//end of unmountFileM

//-----------------------------------------------------------------------
ACE_INT32 ACS_EMF_Common::copyFileListOnPassive(const ACE_TCHAR *fileList[], ACE_INT32 nooffiles)
{
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
	std::string phyFilePath;
	std::string fileMPath(NBI_root);
	phyFilePath.append(ACS_EMF_CLUSTER);
	phyFilePath.append(ACS_EMF_ACTIVE_DATA);
	for (int i = 0; i < nooffiles; i++){
		std::string str(fileList[i]);
		std::string fileName = str.substr(str.find(fileMPath) + fileMPath.length());
		std::string activeDataPath(ACS_EMF_ACTIVE_DATA);
		activeDataPath.append(fileName);
		DEBUG(1," activeDataPath.append(fileName); %s",activeDataPath.c_str());
		sprintf(cmd,"%s %s %s %s", ACS_EMF_CMD_GNRL_OPERATIONS, ACS_EMF_COPY_OPTS, activeDataPath.c_str(),phyFilePath.c_str());
		if(_execlp(cmd) != EMF_RC_OK){
			DEBUG(1,"%s","ACS_EMF_Common::copyFileListOnPassive - Copying file on Passive is  failed");
			return EMF_RC_ERROR;
		}else
			DEBUG(1,"%s","ACS_EMF_Common::copyFileListOnPassive - Copying file on Passive is Success");
	}
	return EMF_RC_OK;
}

ACE_INT32 ACS_EMF_Common::removeFilesOnPassive()
{
	std::string rmString;
	rmString.append(ACS_EMF_CLUSTER);
	rmString.append(ACS_EMF_ACTIVE_DATA);
	rmString.append(ACS_EMF_SLASH);
	rmString.append(ACS_EMF_ASTERISK);
	if (removeaDataFromFileM(rmString) == 0){
		DEBUG(1,"%s","ACS_EMF_Common::removeFilesOnPassive - Removing files on Passive node is Success");
		return EMF_RC_OK;
	}
	DEBUG(1,"%s","ACS_EMF_Common::removeFilesOnPassive - Removing files on Passive node FAILED");
	return EMF_RC_ERROR;
}
ACE_INT32 ACS_EMF_Common::unmountPassiveDVDData(bool bisShutdown, bool bUnmountMedia)
{
	INFO(1,"%s","ACS_EMF_Common::unmountPassiveDVDData - Entering");
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
	ACE_INT32 ret = EMF_RC_ERROR;
	GetHWVersion(); // Why it needs call hardware version if hardware version is already get in EMF_SERVICE_MODULE() ctor?

	if(GetHWVariant() < 3){
		sprintf(cmd,"%s",ACS_EMF_CMD_NFS_REMOVESHARE);
		INFO(1,"ACS_EMF_Common::unmountPassiveDVDData - Calling  '%s' ...", cmd);
	}
	else{
		sprintf(cmd,"%s %s %s", ACS_EMF_CMD_NFS_OPERATIONS, ACS_EMF_RMVE_MEDIA_OPTS, bisShutdown? "true":"false" );
		INFO(1,"ACS_EMF_Common::unmountPassiveDVDData - %s [%s]","Calling:", cmd);
	}
	int infp, outfp;
	if ((pid_in_progress = popen_with_pid(cmd, &infp, &outfp)) <= 0) {
		ERROR(1,"ACS_EMF_Common::unmountPassiveDVDData - Error in executing the %s",cmd);
		DEBUG(1,"%s","ACS_EMF_Common::unmountPassiveDVDData - Leaving");
		return EMF_RC_ERROR;
	}
	DEBUG(1,"ACS_EMF_Common::unmountPassiveDVDData - PID for removing /media Path through NFS: %d",pid_in_progress);
	waitpid(pid_in_progress,&status,0);
	if (WIFEXITED(status) ){
		ret=WEXITSTATUS(status);
		pid_in_progress = EMF_RC_ERROR;
		//DEBUG(1,"%s execution status = %d\n",cmd,ret);
	}
	// ******************************************** FIX: BEGIN TR HR66265 ********************************************
	if ((ret == 0) /*&& (GetHWVariant() != 3)*/){ // This fix is applicable even for GEP5 HW
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// The script "acs_emf_remove_passivedvd.sh" not unmount DVD if no "media" is mounted in nfs.
		// When send lockMedia, if "/media" is busy, umount fail. The DVD is then umounted in nfs but remain mounted
		// in mtab. This causes an abnormal behavior when lockMedia is send.
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (bUnmountMedia == true) {
			TScsiDev objScsiDev;
			// Check if the folder "/media" is mounted on.
			if(objScsiDev.isDirMounted("/media") == true){
				DEBUG(1,"%s","ACS_EMF_Common::unmountPassiveDVDData() - isDirMounted return [true]. Execute umount...");
				if(GetHWVariant() < 3){
					ret = unmountDVDOnPassive();
				}
				if(GetHWVariant() >= 3){
					ret = unmountUSBOnPassive();	
				}
			}
		}
		else{
			ret = EMF_RC_OK;
		}

	}
	// ********************************************* FIX: END TR HR66265 *********************************************
	//DEBUG(1,"%s","ACS_EMF_Common::unmountPassiveDVDData - successfully executed: NFS share on DVD-owner has been removed!");
		//DEBUG(1,"%s","ACS_EMF_Common::unmountPassiveDVDData - Leaving");
		//return ret;
	if (ret != EMF_RC_OK){
		DEBUG(1,"%s","ACS_EMF_Common::unmountPassiveDVDData - Removal of NFS share on DVD-owner Failed!");
		DEBUG(1,"%s","ACS_EMF_Common::unmountPassiveDVDData - Leaving");
		return EMF_RC_OK;
	}
	DEBUG(1,"%s","ACS_EMF_Common::unmountPassiveDVDData - Leaving");
	return ret;
}

//-----------------------------------------------------------------------
ACE_INT32 ACS_EMF_Common::setupNFSOnPassive()
{
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
	ACE_INT32 rCode = EMF_RC_ERROR;
	DEBUG(1,"%s","ACS_EMF_Common::setupNFSOnPassive - Entering");

	GetHWVersion();
	if(GetHWVariant() < 3){
		sprintf(cmd,"%s","/opt/ap/acs/bin/acs_emf_passiveshare.sh");
		INFO(1,"%s","ACS_EMF_Common::setupNFSOnPassive - Calling  '/opt/ap/acs/bin/acs_emf_passiveshare.sh' ...");
	} else {
		sprintf(cmd,"%s %s",ACS_EMF_CMD_NFS_OPERATIONS, ACS_EMF_EXPORT_MEDIA_OPTS);
		INFO(1,"ACS_EMF_Common::setupNFSOnPassive - %s [%s]","Invoking:", cmd);
	}
	if(_execlp(cmd) != EMF_RC_OK){
		DEBUG(1,"%s","ACS_EMF_Common::setupNFSOnPassive - Sharing of /media path on node is Failed");
	}else{
		DEBUG(1,"%s","ACS_EMF_Common::setupNFSOnPassive - Sharing of /media path on node is Successfull");
		rCode=EMF_RC_OK;
	}
	DEBUG(1,"%s","ACS_EMF_Common::setupNFSOnPassive - Leaving");
	return rCode;
}//end of setupNFSOnActive

const char *getDeviceName(){return g_devicefile;}

//isDVDEmpty
ACE_INT32 ACS_EMF_Common::isDVDEmpty()
{
	ACE_UINT64 iUsedSpace;
		ACE_UINT32 iRet;
		// Initialization
		iUsedSpace = 0;
		iRet = acs_emf_common::DVDMEDIA_STATE_EMPTY;
		// Get the used space on DVD
		iRet = getMediaUsedSpace(&iUsedSpace);
		// Check for error
		if(iRet == EMF_RC_OK){
			if(iUsedSpace > 0){
				// Set not empty
				iRet = acs_emf_common::DVDMEDIA_STATE_NOT_EMPTY;
				DEBUG(1,"%s","ACS_EMF_Common::isDVDEmpty - DVD is not empty");
			}else{
				// DVD is empty
				DEBUG(1,"%s","ACS_EMF_Common::isDVDEmpty - DVD is empty");
			}
		}else{
			// Unable to calculate used space
			DEBUG(1,"%s","ACS_EMF_Common::isDVDEmpty - getMediaUsedSpace return an error!");
		}
		// Exit from method
		return (iRet);

}//end of isDVDEmpty

/*===================================================================
   ROUTINE: checkForUSBStatus
=================================================================== */
ACE_INT16 ACS_EMF_Common::checkForUSBStatus()
{
	ACE_INT32 rCode = EMF_RC_ERROR;
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
	sprintf(cmd,"%s %s",ACS_EMF_CMD_GNRL_OPERATIONS, ACS_EMF_CHCK_USB_OPTS);
	if(_execlp(cmd) != EMF_RC_OK){
		DEBUG(1,"%s","ACS_EMF_Common::checkForUSBStatus - Nano USB is  not present in the drive");
	}else{
		DEBUG(1,"%s","ACS_EMF_Common::checkForUSBStatus - Nano USB is present in the drive");
		rCode=EMF_RC_OK;
	}
	return rCode;
}//End of checkForUSBStatus

/*===================================================================
   ROUTINE: mountMediaOnNode
=================================================================== */
ACE_INT32 ACS_EMF_Common::mountMediaOnNode() //mountToMedia
{
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
	ACE_INT32 rCode = EMF_RC_ERROR;
	sprintf(cmd,"%s %s %d",ACS_EMF_CMD_GNRL_OPERATIONS, ACS_EMF_MOUNT_OPTS, getNodeState());
	if(_execlp(cmd) != EMF_RC_OK){
		ERROR(1,"%s","ACS_EMF_Common::mountMediaOnNode() - Mounting thumbdrive to /media is  Failed!!!");
	}else{
		DEBUG(1,"%s","ACS_EMF_Common::mountMediaOnNode() - Mounting thumbdrive to /media is succsessful");
		if(removeLFFromMedia() != EMF_RC_OK)
			DEBUG(1,"%s","ACS_EMF_Common::mountMediaOnNode() - Failed to delete the lost+found in /media");
		rCode =EMF_RC_OK;
	}
	return rCode;
}//end of mountMediaOnNode

/*===================================================================
 ROUTINE: umountUSBData
=================================================================== */
ACE_INT32 ACS_EMF_Common::umountUSBData()
{
	DEBUG(1,"%s","ACS_EMF_Common::umountUSBData - Entering");
	ACE_INT32 rCode = EMF_RC_ERROR;
	if (!isMountd()){
                DEBUG(1,"%s","ACS_EMF_Common::umountUSBData - Media is already unmounted");
		rCode=EMF_RC_OK;
	}
	else{
		/*TR HU81933-Start*/
		FILE *fp = NULL;
		ACE_TCHAR cmdLs[100];
		size_t buffer_size = 256;
		char buffer[buffer_size] ;
		char thumbdriveDir[buffer_size];

		sprintf(cmdLs,"ls -l /dev | grep eri_thumbdrive | /usr/bin/awk '{print $NF}'");
		fp = popen(cmdLs , mode);
		if(fp != NULL){//ok
			if (!(fgets(buffer, buffer_size, fp))) {
			       ERROR(1, "ACS_EMF_Common::umountUSBData - %s", "ls: grep eri_thumbdrive not found");
					pclose(fp);
			        }
			else{
					//log succes read thumbdrive
					sprintf(thumbdriveDir,"/dev/%s",buffer);
					//delete NL as last char of thumbdriveDir
					int len= strlen(thumbdriveDir);
					thumbdriveDir[len-1]=0;
					DEBUG(1, "ACS_EMF_Common::umountUSBData - %s %s", "fgets thumbdriveDir success", thumbdriveDir);
					//DEBUG(1, "ACS_EMF_Common::umountUSBData - %s %d", "fgets thumbdriveDir len", strlen(thumbdriveDir));
					pclose(fp);

			        }
			  }
		else{
			//log error: not success in popen
			ERROR(1, "ACS_EMF_Common::umountUSBData - %s", "popen failire");
			        }
		/*TR HU81933-End*/
		ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
		/*TR HU81933 used thumbdriveDir instead of ACS_EMF_THUMBDRIVE: the script looks up into /proc/mounts
		 * so it is needed to provide the device not the link*/
		//sprintf(cmd,"%s %s %s",ACS_EMF_CMD_GNRL_OPERATIONS, ACS_EMF_UNMOUNT_OPTS, ACS_EMF_THUMBDRIVE);
		sprintf(cmd,"%s %s %s",ACS_EMF_CMD_GNRL_OPERATIONS, ACS_EMF_UNMOUNT_OPTS, thumbdriveDir);
		if(_execlp(cmd) != EMF_RC_OK){
			DEBUG(1,"%s","ACS_EMF_Common::umountUSBData - Failed to Unmount the media");
			DEBUG(1,"%s","ACS_EMF_Common::umountUSBData - Leaving");
		}else{
			DEBUG(1,"%s","ACS_EMF_Common::umountUSBData - Unmounting the media  is Success");
			DEBUG(1,"%s","ACS_EMF_Common::umountUSBData - Leaving");
			rCode=EMF_RC_OK;
		}
	}
	return rCode;
}//end of umountUSBData
ACE_INT32 ACS_EMF_Common::unmountUSBOnPassive()
{
        DEBUG(1,"%s","ACS_EMF_Common::unmountUSBOnPassive - Entering");
        ACE_INT32 rCode = EMF_RC_ERROR;
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
	sprintf(cmd,"%s %s %s",ACS_EMF_CMD_GNRL_OPERATIONS, ACS_EMF_UNMOUNT_OPTS, ACS_EMF_PASSIVE_MEDIA);
	DEBUG(1,"ACS_EMF_Common::unmountUSBOnPassive - cmd is: %s",cmd);
	if(_execlp(cmd) != EMF_RC_OK){
		DEBUG(1,"%s","ACS_EMF_Common::unmountUSBOnPassive - Failed to Unmount the media");
	}else{
		DEBUG(1,"%s","ACS_EMF_Common::unmountUSBOnPassive - Unmounting the media  is Success");
		rCode=EMF_RC_OK;
	}
	DEBUG(1,"%s","ACS_EMF_Common::unmountUSBOnPassive - Leaving");
	return rCode;
}//end of unmountUSBOnPassive

bool ACS_EMF_Common::isMountd()
{
	DEBUG(1,"%s","ACS_EMF_Common::isMountd - Entering");
	bool rCode= false;
	string str;
	std::string mountP;
	if(getNodeState() == 1)
		mountP.append(ACS_EMF_ACTIVE_MEDIA);
	else
		mountP.append(ACS_EMF_PASSIVE_MEDIA);
	/*get the mount point for the thumb drive*/
	str = getMediaMount();
	DEBUG(1,"ACS_EMF_Common::isMountd - point recieved :%s",str.c_str());
	DEBUG(1,"ACS_EMF_Common::isMountd - mount point to be compared :%s",mountP.c_str());
	if(std::strcmp(str.c_str(),mountP.c_str()) == EMF_RC_OK){
		DEBUG(1,"%s","ACS_EMF_Common::isMountd - Media mount path matched on node");
		rCode=true;
	}else{
		DEBUG(1,"%s","ACS_EMF_Common::isMountd - Media mount path not matched on node");
		rCode=false;
	}
	DEBUG(1,"%s","ACS_EMF_Common::isMountd - Leaving");
	return rCode;
}
/*===================================================================
ROUTINE: mountcheckForMedia
=================================================================== */
ACE_INT32 ACS_EMF_Common::mountMedia() //mountcheckForMedia
{
	ACE_INT32 rCode=EMF_RC_OK;

	DEBUG(1,"%s","ACS_EMF_Common::mountMedia - Entering");

	/* check if the media is already mounted */
	if (isMountd() == TRUE){
		DEBUG(1,"%s","ACS_EMF_Common::mountMedia - Media is already mounted on active node");
	} else{
		/* mount media */
		DEBUG(1,"%s","ACS_EMF_Common::mountMedia - Mounting Media on active node");
		if (mountMediaOnNode() != EMF_RC_OK){
			ERROR(1,"%s","ACS_EMF_Common::mountMedia - Mounting the Device on active node Failed!!!");
			DEBUG(1,"%s","ACS_EMF_Common::mountMedia - Leaving");
			rCode = EMF_RC_MOUNTINGFAILED;
		}
	}
	DEBUG(1,"%s","ACS_EMF_Common::mountMedia - Leaving");
	return rCode;
}
/*===================================================================
ROUTINE: formatAndMountActiveMedia
=================================================================== */
ACE_INT32 ACS_EMF_Common::formatAndMountMedia()
{
	if((CheckFilesystem() != EMF_RC_OK) || (isThumbDriveHealthy() == false))
	{
		if(umountUSBData() != EMF_RC_OK)
			DEBUG(1,"%s","ACS_EMF_Common::formatAndMountActiveMedia - Failed to unmount media  on passive node");
		setMediaFlag(formatPosition,NO_Sync);
		if(formatMedium() == EMF_RC_OK){
			if (mountMediaOnNode() != EMF_RC_OK){
				ERROR(1,"%s","ACS_EMF_Common::formatAndMountActiveMedia - Mounting the Device Failed!!! ");
				return EMF_RC_ERROR;
			}
		}
		else{
			DEBUG(1,"%s","ACS_EMF_Common::formatAndMountActiveMedia - Formating thumb drive failed");
			return EMF_RC_ERROR;
		}
	}else{
		if(removeMediaData(getNodeState()) != EMF_RC_OK){
			DEBUG(1,"%s","ACS_EMF_Common::formatAndMountActiveMedia - Cleanup of thumb drive failed");
		}
		setMediaFlag(Sync_Pos,NO_Sync);
	}
	return EMF_RC_OK;
}//End of formatAndMountMedia
/*===================================================================
ROUTINE: calculateChecksumOfUSB
=================================================================== */

ACE_INT32 ACS_EMF_Common::calculateChecksumOfUSB(const ACE_TCHAR* filePath, const ACE_TCHAR* fname)
{
	FILE *fp = NULL;
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_TCHAR cksumcmd[200];
	ACE_TCHAR cmd[200];
	ACE_INT32 ret = EMF_RC_ERROR;
	snprintf(cksumcmd,sizeof(cksumcmd),"cd %s; find . -type f | xargs sha1sum | sort >> %s",filePath,fname);
	fp = popen(cksumcmd ,mode);
        if (fp != NULL){
		fgets(cmd,sizeof(cksumcmd),fp);
		status = pclose(fp);
        }
        else{
                ERROR(1,"ACS_EMF_Common::calculateChecksumOfUSB - Error in executing the command %s",cksumcmd);
                return EMF_RC_ERROR;
        }

	if (WIFEXITED(status) ){
		ret=WEXITSTATUS(status);
		DEBUG(1,"ACS_EMF_Common::calculateChecksumOfUSB - %s execution status = %d",cksumcmd,ret);
	}
	if(ret == 0){
		DEBUG(1,"%s","ACS_EMF_Common::calculateChecksumOfUSB - Successfully calculated the checksum of media folder on active node");
		return ret;

	}
	else{
		DEBUG(1,"%s","ACS_EMF_Common::calculateChecksumOfUSB - Calculation of checksum of media folder on active node failed");
		return EMF_RC_ERROR;
	}
}//End of calculateChecksumOfUSB

/*===================================================================
ROUTINE: copyFromMasterUsbMedia
=================================================================== */
ACE_INT32 ACS_EMF_Common::copyFromMasterUsbMedia()
{
	DEBUG(1,"%s","ACS_EMF_Common::copyFromMasterUsbMedia - Entering");

	if(mountMediaOnOtherNode() == EMF_RC_ERROR){
		DEBUG(1,"%s","ACS_EMF_Common::copyFromMasterUsbMedia():Mounting fileM folder failed on passive node");
		DEBUG(1,"%s","ACS_EMF_Common::copyFromMasterUsbMedia - Leaving");
		return EMF_RC_ERROR;
	}
	if(copyToMediaFolder() !=EMF_RC_OK){
		DEBUG(1,"%s","ACS_EMF_Common::copyFromMasterUsbMedia():copyFromMasterUsbMedia operation failed");
		if(unmountFileM() != EMF_RC_OK){
			DEBUG(1,"%s","ACS_EMF_Common::copyFromMasterUsbMedia():un Mounting fileM folder failed on passive node");
			DEBUG(1,"%s","ACS_EMF_Common::copyFromMasterUsbMedia - Leaving");
			return EMF_RC_ERROR;
		}
		DEBUG(1,"%s","ACS_EMF_Common::copyFromMasterUsbMedia - Leaving");
		return EMF_RC_ERROR;
	}
	if(unmountFileM() != EMF_RC_OK){
		DEBUG(1,"%s","ACS_EMF_Common::copyFromMasterUsbMedia():un Mounting fileM folder failed on passive node");
		DEBUG(1,"%s","ACS_EMF_Common::copyFromMasterUsbMedia - Leaving");
	}
	DEBUG(1,"%s","ACS_EMF_Common::copyFromMasterUsbMedia - Leaving");
	return EMF_RC_OK;
}//End of copyFromMasterUsbMedia

/*===================================================================
ROUTINE: copyToMediaFolder
=================================================================== */
ACE_INT32 ACS_EMF_Common::copyToMediaFolder()
{

	DEBUG(1,"%s","ACS_EMF_Common::copyToMediaFolder - Entering");
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
	ACE_TCHAR source2cp[130];
	ACE_TCHAR destin2cp[130];
	ACE_INT32 rCode=EMF_RC_OK;

	snprintf(source2cp,sizeof(source2cp),"%s%s%s",ACS_EMF_ACTIVE_DATA,ACS_EMF_SLASH,ACS_EMF_ASTERISK);
	if(getNodeState() == 1)
		snprintf(destin2cp,sizeof(destin2cp),"%s%s",ACS_EMF_ACTIVE_MEDIA,ACS_EMF_SLASH);
	else
		snprintf(destin2cp,sizeof(destin2cp),"%s%s",ACS_EMF_PASSIVE_MEDIA,ACS_EMF_SLASH);

	sprintf(cmd,"%s %s %s %s", ACS_EMF_CMD_GNRL_OPERATIONS, ACS_EMF_COPY_OPTS, source2cp,destin2cp);
	DEBUG(1,"ACS_EMF_Common::copyToMediaFolder - cmd is:%s",cmd);
	if(_execlp(cmd) != EMF_RC_OK){
		DEBUG(1,"%s","ACS_EMF_Common::copyToMediaFolder - copy to passive node  failed");
		rCode = EMF_RC_ERROR;
	}else{
		DEBUG(1,"%s","ACS_EMF_Common::copyToMediaFolder - copy to passive node succsesfull\n");
	}
	DEBUG(1,"%s","ACS_EMF_Common::copyToMediaFolder - Leaving");
	return rCode;
}//End of copyToMediaFolder

/*===================================================================
ROUTINE: appendVolumeName
=================================================================== */
ACE_INT32 ACS_EMF_Common::appendVolumeName(const ACE_TCHAR *imageName)
{
	DEBUG(1,"%s","ACS_EMF_Common::appendVolumeName - Entering");
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
	ACE_INT32 rCode=EMF_RC_OK;
	sprintf(cmd,"%s %s %s",ACS_EMF_CMD_GNRL_OPERATIONS, ACS_EMF_LABL_APPLY_OPTS, imageName);
	if(_execlp(cmd) != 0){
		ERROR(1,"%s","ACS_EMF_Common::appendVolumeName - Image name  appended Failed!!!");
		rCode = EMF_RC_ERROR;
	}else{
		DEBUG(1,"%s"," ACS_EMF_Common::appendVolumeName - Image name  appended  succsessfully");
	}
	return rCode;
	DEBUG(1,"%s","ACS_EMF_Common::appendVolumeName - Leaving");
}//End of appendVolumeName

/*===================================================================
ROUTINE: CheckFilesystem
=================================================================== */
ACE_INT32 ACS_EMF_Common::CheckFilesystem()
{
	DEBUG(1,"%s","ACS_EMF_Common::CheckFilesystem - Entering");
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
	ACE_INT32 rCode=EMF_RC_OK;
	sprintf(cmd,"%s %s",ACS_EMF_CMD_GNRL_OPERATIONS, ACS_EMF_CHCK_FS_OPTS);
	int rv =_execlp(cmd);
	DEBUG(1,"rv = %s",cmd);
	DEBUG(1,"rv = %d",rv);
	if(rv != EMF_RC_OK){
		DEBUG(1,"%s","ACS_EMF_Common::CheckFilesystem - File system is not ext3");
		rCode = EMF_RC_ERROR;
	}else{
		DEBUG(1,"%s","ACS_EMF_Common::CheckFilesystem - File system is ext3");
	}
	DEBUG(1,"%s","ACS_EMF_Common::CheckFilesystem - Leaving");
	return rCode;
}//End of CheckFilesystem

/*===================================================================
ROUTINE: mountUSBInActive
=================================================================== */
ACE_INT32 ACS_EMF_Common::mountThumbDrive()
{
	DEBUG(1,"%s","ACS_EMF_Common::mountThumbDrive - Entering");
	ACE_INT32 rCode=EMF_RC_OK;
	ACE_INT32 mountResult = mountMedia();
	if(mountResult != EMF_RC_OK){
		DEBUG(1,"%s","ACS_EMF_Common::mountThumbDrive - ENTERING (mountMedia() != EMF_RC_OK)");
		ERROR(1,"%s","ACS_EMF_Common::mountThumbDrive - Mounting the Device  Failed!!! ");
		rCode = mountResult;
	}
	if((rCode != EMF_RC_ERROR  && CheckFilesystem() != EMF_RC_OK) || (getMediaFlag(formatPosition) == 2)) {
		if(umountUSBData() !=EMF_RC_OK) {
			DEBUG(1,"%s","ACS_EMF_Common::mountThumbDrive - Failed to unmount media partition");
		}	
		if(formatMedium() != EMF_RC_OK) {
			DEBUG(1,"%s","ACS_EMF_Common::mountThumbDrive - Formating failed");
			rCode = EMF_RC_ERROR;
		}
		if (rCode != EMF_RC_ERROR && mountMediaOnNode() != EMF_RC_OK){
			ERROR(1,"%s","ACS_EMF_Common::mountThumbDrive - Mounting the Device  Failed!!! ");
			rCode= EMF_RC_ERROR;
		}

	}

	DEBUG(1,"%s","ACS_EMF_Common::mountThumbDrive - Leaving");
	return rCode;
}//END OF mountThumbDrive
/*===================================================================
ROUTINE: getLocalNode
=================================================================== */
void ACS_EMF_Common::getLocalNode(int32_t *piSystemId, acs_dsd::NodeStateConstants *penmNodeState)
{
	ACS_DSD_Client *pClient;
	ACS_DSD_Node objNode;
	// Initialization
	pClient = NULL;
	// Check pointers
	if((piSystemId != NULL) && (penmNodeState != NULL)){
		// Create "client" instancd
		pClient = new ACS_DSD_Client();
		// Initialize output params
		*piSystemId = 0;
		*penmNodeState = acs_dsd::NODE_STATE_UNDEFINED;
		// get this node
		pClient->get_local_node(objNode);
		// Debug message
		DEBUG(1,"ACS_EMF_Common::getLocalNode - SystemId=[%i], NodeState=[%i]", objNode.system_id, objNode.node_state);
		// Then set output params
		*piSystemId = objNode.system_id;
		*penmNodeState = objNode.node_state;
		// Release client
		delete(pClient);
		pClient = NULL;
	}
}//End of getLocalNode

/*===================================================================
ROUTINE: isMediaAlreadyMountedOnPassive
=================================================================== */
ACE_INT32 ACS_EMF_Common::isMediaAlreadyMountedOnPassive()
{
	FILE *fp = NULL;
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_TCHAR data[1000];
	ACE_TCHAR cmd[1000];
	ACE_INT32 ret = EMF_RC_ERROR;

	DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnPassive - Entering");
	
	sprintf(cmd,"mount -l | grep %s 1>/dev/null 2>/dev/null",g_devicefile);
	fp = popen(cmd, mode);
        if ( fp != NULL)
		status = pclose(fp);
        else{
                ERROR(1,"ACS_EMF_Common::isMediaAlreadyMountedOnPassive - Error in executing the %s",cmd);
                DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnPassive - Leaving");
                return EMF_RC_ERROR;
        }
	if (WIFEXITED(status)){
		ret=WEXITSTATUS(status);
		//DEBUG(1,"%s execution status = %d\n",cmd,ret);
	}

	if (ret == 1){
		DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnPassive - The DVD is not mounted yet");
		DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnPassive - Leaving");
		return 1;
	}
	else if (ret == 0){
		sprintf(cmd,"mount -l | grep %s | awk '{print $3}'",g_devicefile);
		fp = popen(cmd, mode);
                if (fp != NULL){
			if((fgets(data, 1000, fp) != NULL)){
				int len = strlen(data);
				if( data[len-1] == '\n' )
					data[len-1] = '\0';
			}
			status = pclose(fp);
                }
                else{
                        ERROR(1,"ACS_EMF_Common::isMediaAlreadyMountedOnPassive - Error in executing the %s",cmd);
                        DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnPassive - Leaving");
                        return EMF_RC_ERROR;
                }
		if (WIFEXITED(status) ){
			ret=WEXITSTATUS(status);
			//DEBUG(1,"%s execution status = %d\n",cmd,ret);
		}

		if (ret == 0){
			if( strcmp(data,"/data/opt/ap/internal_root/media") == 0){
				if( ACS_EMF_Common::unmountPassiveDVDOnActive() == 0 ){
					DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnPassive - unmounted the DVD which is mounted to FileM=media");
					DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnPassive - Leaving");
					return 1;
				}
				else{
					DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnPassive - Failed to unmount the DVD which is mounted to FileM=media");
					DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnPassive - Leaving");
					return 2;
				}
			}
			else if( strcmp(data,"/media") == 0){
				DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnPassive - The DVD is already mounted to /media, no need to mount again");
				DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnPassive - Leaving");
				return 0;
			}
			else{
				DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnPassive - ACS_EMF_Common::isMediaAlreadyMountedOnPassive, The DVD is mounted to other than 'media' folder");
				DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnPassive - Leaving");
				return 2;
			}
		}
		else{
			ERROR(1,"ACS_EMF_Common::isMediaAlreadyMountedOnPassive - %s failed",cmd);
			DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnPassive - Leaving");
			return EMF_RC_ERROR;
		}
	}
	else{
		ERROR(1,"ACS_EMF_Common::isMediaAlreadyMountedOnPassive - %s failed",cmd);
		DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnPassive - Leaving");
		return EMF_RC_ERROR;
	}
}// end of isMediaAlreadyMountedOnPassive

/*===================================================================
ROUTINE: //isMediaAlreadyMountedOnActive
=================================================================== */
ACE_INT32 ACS_EMF_Common::isMediaAlreadyMountedOnActive()
{

	FILE *fp = NULL;
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_TCHAR data[1000];
	ACE_TCHAR cmd[1000];
	ACE_INT32 ret = EMF_RC_ERROR;

	sprintf(cmd,"mount -l | grep %s 1>/dev/null 2>/dev/null",g_devicefile);
	fp = popen(cmd, mode);
        if (fp != NULL)
		status = pclose(fp);
        else{
                ERROR(1,"Error in executing the %s",cmd);
                return EMF_RC_ERROR;
        }
	if (WIFEXITED(status) ){
		ret=WEXITSTATUS(status);
		//DEBUG(1,"%s execution status = %d\n",cmd,ret);
	}

	if (ret == 1){
		DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnActiveThe DVD is not mounted yet");
		return 1;
	}
	else if (ret == 0){
		sprintf(cmd,"mount -l | grep %s | awk '{print $3}'",g_devicefile);
		fp = popen(cmd, mode);
                if (fp != NULL){
			if((fgets(data, 1000, fp) != NULL)){
				int len = strlen(data);
				if( data[len-1] == '\n' )
					data[len-1] = '\0';
				DEBUG(1,"ACS_EMF_Common::isMediaAlreadyMountedOnActive - The DVD is already mounted to = %s folder",data);
			}
			status = pclose(fp);
                }
                else{
                        ERROR(1,"ACS_EMF_Common::isMediaAlreadyMountedOnActive - Error in executing the %s",cmd);
                        return EMF_RC_ERROR;
                }
		if (WIFEXITED(status) ){
			ret=WEXITSTATUS(status);
			//DEBUG(1,"%s execution status = %d\n",cmd,ret);
		}

		if (ret == 0){
			if( strcmp(data,"/data/opt/ap/internal_root/media") == 0){
				DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnActive - The DVD is already mounted to /data/opt/ap/internal_root/media, no need to mount again");
				return 0;
			}
			else if( strcmp(data,"/media") == 0){
				if( ACS_EMF_Common::unmountPassiveDVDData(false) == 0 ){
					DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnActive - unmounted the DVD which is mounted to /media");
					return 1;
				}
				else{
					DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnActive - Failed to unmount the DVD which is mounted to /media");
					return 2;
				}
			}
			else{
				DEBUG(1,"%s","ACS_EMF_Common::isMediaAlreadyMountedOnActive - The DVD is mounted to other than 'media' folder");
				return 2;
			}
		}
		else{
			ERROR(1,"ACS_EMF_Common::isMediaAlreadyMountedOnActive - %s failed",cmd);
			return EMF_RC_ERROR;
		}
	}
	else{
		ERROR(1,"ACS_EMF_Common::isMediaAlreadyMountedOnActive - %s failed",cmd);
		return EMF_RC_ERROR;
	}
}// end of isMediaAlreadyMountedOnActive
/*===================================================================
ROUTINE: setMediaFlag
=================================================================== */
ACE_INT32 ACS_EMF_Common::setMediaFlag(ACE_INT16 pos,ACE_INT16 flag)
{
	DEBUG(1,"%s","ACS_EMF_Common:::setMiediaFlag - Entering");
	//DEBUG(1," ACS_EMF_Common::setMaterUsb:iflag is %d",flag);
	//DEBUG(1," ACS_EMF_Common::setMaterUsb:pos is %d",pos);
	ACE_stat fileStat;
	if ( ACE_OS::stat(mediaInfoPath,&fileStat) != 0 )
	{
		ofstream file (mediaInfoPath);
		if (file.is_open())
		{
			file<<2;
			file<<",";
			file<<2;
			file<<",";
			file<<1;
		}
		file.close();
	}
	int temp= 0;
	unsigned int v1=0,v2=0,v3=0;
	std::ifstream in(mediaInfoPath,std::ios::in);
	in.seekg(in.tellg(),std::ios::end);
	int posit=in.tellg();
	while (posit--) {
		in.seekg(posit);
		if(posit == 0){
			temp = 0;
			temp = (int)in.get();
			v1 = temp - 48 ;
		}
		else if(posit == 2){
			temp = 0;
			temp = (int)in.get();
			v2 = temp - 48;
		}
		else if(posit == 4){
			temp = 0;
			temp = (int)in.get();
			v3 = temp - 48;
		}
	}
	DEBUG(1,"ACS_EMF_Common:::setMiediaFlag - values of v1== %d v2== %d v3== %d",v1,v2,v3);
	in.close();

	ofstream file (mediaInfoPath);
	if (file.is_open()){
		if(pos == 0){
			v1 = flag;
		}
		else if(pos == 2){
			v2 = flag;
		}
		else if(pos == 4){
			v3 = flag;
		}
		DEBUG(1,"ACS_EMF_Common:::setMiediaFlag - values to add v1== %d v2== %d v3== %d",v1,v2,v3);
		file<<v1;
		file<<",";
		file<<v2;
		file<<",";
		file<<v3;
	}
	else
		DEBUG(1,"%s","ACS_EMF_Common:::setMiediaFlag - Unable to open file");
	file.close();
	DEBUG(1,"%s","ACS_EMF_Common:::setMiediaFlag - Leaving");

	return EMF_RC_OK;
}//end of setMediaFlag

/*===================================================================
ROUTINE: getMediaFlag
=================================================================== */
ACE_INT32 ACS_EMF_Common::getMediaFlag(ACE_INT16 pos)
{
	DEBUG(1,"%s","ACS_EMF_Common:::getMiediaFlag - Entering");
	int temp= 0;
	int flag = EMF_RC_ERROR;
	ACE_stat fileStat;
	if ( ACE_OS::stat(mediaInfoPath,&fileStat) != 0 )
	{
		DEBUG(1,"%s","ACS_EMF_Common:::getMiediaFlag - Leaving");
		return flag;
	}
	std::ifstream in(mediaInfoPath,std::ios::in);
	in.seekg(in.tellg(),std::ios::end);
	int posit=in.tellg();
	while (posit--) {
		in.seekg(posit);
		if((posit == 0) && (pos == 0)){
			temp = 0;
			temp = (int)in.get();
			DEBUG(1,"ACS_EMF_Common:::getMiediaFlag - value temp :: %d",temp);
			flag = temp - 48 ;
			ACS_PRC_API prcObj;
			DEBUG(1,"ACS_EMF_Common:::getMiediaFlag - value flag :: %d",flag);
			DEBUG(1,"ACS_EMF_Common:::getMiediaFlag - value getNode ::%d",getNode());
			if(getNode() == flag){
				//master usb is active node usb
				DEBUG(1,"%s","ACS_EMF_Common:::getMiediaFlag - USB in active node is the master usb");
				DEBUG(1,"%s","ACS_EMF_Common:::getMiediaFlag - Leaving");
				return 0;
			}
			else if((getNode() != flag) && (flag == 2)){
				//master usb is passive node usb
				DEBUG(1,"%s","ACS_EMF_Common:::getMiediaFlag - USB in passive node is the master usb");
				DEBUG(1,"%s","ACS_EMF_Common:::getMiediaFlag - Leaving");
				return 1;
			}

		}
		else if((posit == 2) && (pos == 2))
		{
			temp = 0;
			temp = (int)in.get();
			DEBUG(1,"ACS_EMF_Common:::getMiediaFlag - flag value %d",temp);
			flag = temp - 48;
			DEBUG(1,"ACS_EMF_Common:::getMiediaFlag - flag value %d",flag);

		}else if((posit == 4) && (pos == 4)){
			temp = 0;
			temp = (int)in.get();
			DEBUG(1,"ACS_EMF_Common:::getMiediaFlag - flag value %d",temp);
			flag = temp - 48;
			DEBUG(1,"ACS_EMF_Common:::getMiediaFlag - flag value %d",flag);
		}
	}
	in.close();
	DEBUG(1,"ACS_EMF_Common:::getMiediaFlag - flag value %d",flag);

	DEBUG(1,"%s","ACS_EMF_Common:::getMiediaFlag - Leaving");

	return flag;
}//end of getMediaFlag

/*===================================================================
ROUTINE: removeMediaData
=================================================================== */
ACE_INT32 ACS_EMF_Common::removeMediaData(ACE_INT16 node)
{
	DEBUG(1,"%s","ACS_EMF_Common:::removeMediaData - Entering");
	std::string rmCmd;
	ACE_INT32 ret = EMF_RC_ERROR;
	if(node == 1){
		rmCmd.append(ACS_EMF_ACTIVE_MEDIA);
		rmCmd.append(ACS_EMF_SLASH);
		rmCmd.append(ACS_EMF_ASTERISK);
	}else if(node == 2){
		rmCmd.append(ACS_EMF_PASSIVE_MEDIA);
		rmCmd.append(ACS_EMF_SLASH);
		rmCmd.append(ACS_EMF_ASTERISK);
	}
	DEBUG(1,"ACS_EMF_Common:::removeMediaData - rmCmd is :%s",rmCmd.c_str());
	ret = removeaDataFromFileM(rmCmd);
	if (ret == 0 )
	{
		DEBUG(1,"%s","ACS_EMF_Common:::removeMediaData - the data from /media path deleted");
		DEBUG(1,"%s","ACS_EMF_Common:::removeMediaData - Leaving");
		return EMF_RC_OK;
	}
	else
	{
		DEBUG(1,"%s","ACS_EMF_Common:::removeMediaData - failed to delete the data");
		DEBUG(1,"%s","ACS_EMF_Common:::removeMediaData - Leaving");
		return EMF_RC_ERROR;
	}
	DEBUG(1,"%s","ACS_EMF_Common:::removeMediaData - Leaving");
	return EMF_RC_OK;
}//end of removeMediaData

/*===================================================================
ROUTINE: removeLFFromMedia 
=================================================================== */
ACE_INT32 ACS_EMF_Common::removeLFFromMedia()
{
	DEBUG(1,"%s","ACS_EMF_Common::removeLFFromMedia - Entering");
	std::string rmCmd;
	ACE_INT32 ret = EMF_RC_ERROR;
	if(getNodeState() == 1){
		rmCmd.append(ACS_EMF_ACTIVE_MEDIA);
		rmCmd.append(ACS_EMF_SLASH);
		rmCmd.append(ACS_EMF_LOST_FOUND);
	}
	else{
		rmCmd.append(ACS_EMF_PASSIVE_MEDIA);
		rmCmd.append(ACS_EMF_SLASH);
		rmCmd.append(ACS_EMF_LOST_FOUND);
	}

	ret = removeaDataFromFileM(rmCmd);
	if (ret == EMF_RC_OK )
	{
		DEBUG(1,"%s","ACS_EMF_Common::removeLFFromMedia - the lost+foun from /media path deleted");
		return EMF_RC_OK;
	}

	DEBUG(1,"%s","ACS_EMF_Common::removeLFFromMedia - failed to delete the lost+foun");
	DEBUG(1,"%s","ACS_EMF_Common::removeLFFromMedia - Leaving");
	return EMF_RC_ERROR;

}//End of removeLFFromMedia

/*===================================================================
ROUTINE: createEMFFolder
======================================================================*/
ACE_INT32 ACS_EMF_Common::createEMFFolder()
{
	//char datadiskPath[200] ;
	string datadiskPath;
	string acs_data("ACS_DATA");
	DEBUG(1,"%s","ACS_EMF_Common::createEMFFolder - Entering");
	if(!GetDataDiskPath(acs_data, datadiskPath)){
		ERROR( 1, "%s","ACS_EMF_Common::createEMFFolder - Error occurred while getting data disk path");
		DEBUG(1,"%s","ACS_EMF_Common::createEMFFolder - Leaving");
		return EMF_RC_ERROR;
	}
	ACE_stat statBuff;
	if( ACE_OS::stat(datadiskPath.c_str(), &statBuff)  != 0 ){
		ERROR(1, "ACS_EMF_Common::createEMFFolder - %s is not present", datadiskPath.c_str());
		DEBUG(1,"%s","ACS_EMF_Common::createEMFFolder - Leaving");
		return EMF_RC_ERROR;
	}
	else{
		datadiskPath.append(ACS_EMF_DATA_FOLDER);
		INFO(1,"ACS_EMF_Common::createEMFFolder - ACS EMF Folder Path  = %s",datadiskPath.c_str());
		ACE_stat emfStatBuff;
		if( ACE_OS::stat(datadiskPath.c_str(), &emfStatBuff)  != 0 )
			createDirectory(datadiskPath.c_str());
	}
	DEBUG(1,"%s","ACS_EMF_Common::createEMFFolder - Leaving");
	return EMF_RC_OK;
}//End of createEMFFolder

int ACS_EMF_Common::removeaDataFromFileM(string aPath)
{	
	ACE_TCHAR cmd[EMF_CMD_MAXLEN] = {0};
	ACE_INT32 rCode = EMF_RC_OK;
	DEBUG(1,"%s","ACS_EMF_Common::removeaDataFromFileM - Entering");
	sprintf(cmd,"%s %s %s ",ACS_EMF_CMD_GNRL_OPERATIONS, ACS_EMF_RMVE_OPTS, aPath.c_str());
	INFO(1, "ACS_EMF_Common::removeaDataFromFileM - launch [%s]", cmd);
	if(_execlp(cmd) != EMF_RC_OK){
		ERROR(1,"%s","ACS_EMF_Common::removeaDataFromFileM - Removing specified files failed");
		rCode = EMF_RC_ERROR;
	}else{
		INFO(1,"%s","ACS_EMF_Common::removeaDataFromFileM - Removing specified files success");
	}
	DEBUG(1,"%s","ACS_EMF_Common::removeaDataFromFileM - Leaving");
	return rCode;
}

/*===================================================================
ROUTINE: match
======================================================================*/
ACE_TCHAR* ACS_EMF_Common::match(ACE_TCHAR* Str1, ACE_TCHAR* Str2)
{
	/*TR HU81933-Start*/
	FILE *fp = NULL;
	ACE_TCHAR cmd[100];
	size_t buffer_size = 1024;
	char buffer[buffer_size] ;
	char thumbdriveDir[buffer_size];

	sprintf(cmd,"ls -l /dev | grep eri_thumbdrive | /usr/bin/awk '{print $NF}'");
	fp = popen(cmd , mode);
	if(fp != NULL){//ok
		if (!(fgets(buffer, buffer_size, fp))) {
		       ERROR(1, "ACS_EMF_Common::match - %s", "ls: grep eri_thumbdrive not found");
				pclose(fp);
		        }
		else{
				//log succes read thumbdrive
				sprintf(thumbdriveDir,"/dev/%s",buffer);
				//delete NL as last char of thumbdriveDir
				int len= strlen(thumbdriveDir);
				thumbdriveDir[len-1]=0;
				DEBUG(1, "ACS_EMF_Common::match - %s %s", "fgets thumbdriveDir success", thumbdriveDir);
				//DEBUG(1, "ACS_EMF_Common::match - %s %d", "fgets thumbdriveDir len", strlen(thumbdriveDir));
				pclose(fp);

		        }
		  }
	else{
		//log error: not success in popen
		ERROR(1, "ACS_EMF_Common::match - %s", "popen failire");
		        }


	/*TR HU81933-End*/

	/* Check if the main string starts with the substring */
	//TR HU81933 for(unsigned int index=0; index<strlen(Str2); index++) {
	for(unsigned int index=0; index<strlen(thumbdriveDir); index++) {
		//TR HU81933 if (Str2[index] != Str1[index]) {
		if (thumbdriveDir[index] != Str1[index]) {
			//TR HU81933  DEBUG(1, "ACS_EMF_Common::match - %s %s %s", "string mismatch", Str1, Str2);
			DEBUG(1, "ACS_EMF_Common::match - %s %s %s", "string mismatch", Str1, thumbdriveDir);
			return (NULL);
		}
	}
	/* extract 'mount(/media)' and return */
	// TR HU81933-End char *Str = Str1 + strlen(ACS_EMF_THUMBDRIVE) + 1;
	char *Str = Str1 + strlen(thumbdriveDir) + 1;
	unsigned int index=0;
	while (!isspace(Str[++index])){};
	Str[index]='\0';
	return Str;
}

/*===================================================================
ROUTINE: getMediaMount
======================================================================*/
string ACS_EMF_Common::getMediaMount()
{
	DEBUG(1,"%s","ACS_EMF_Common::getMediaMount - Entering");
	std::string line, mPoint("");
	ACE_TCHAR* tmpStr= NULL;
	ifstream myfile (ACS_EMF_FILE_PROC_MOUNTS);
	if (myfile.is_open()) {
		while (getline (myfile, line)!= NULL){
			tmpStr=match((char *)line.c_str(), (char *)ACS_EMF_THUMBDRIVE);
			if (tmpStr != NULL){
				mPoint.assign(tmpStr);
				break;
			}	
		}	
		myfile.close();
	}else{
		DEBUG(1,"%s","ACS_EMF_Common::getMediaMount - Unable to open file");
	}
	DEBUG(1,"%s","ACS_EMF_Common::getMediaMount - Leaving");
	return mPoint;
}

/*===================================================================
ROUTINE: isFileMmounted
======================================================================*/
bool ACS_EMF_Common::isFileMmounted()
{
	DEBUG(1,"%s","ACS_EMF_Common::isFileMmounted - Entering");
	bool bFileMmounted=false;
	std::string line;
	std::size_t pos;  
	ifstream myfile (ACS_EMF_FILE_PROC_MOUNTS);
	if (myfile.is_open()) {
		while (getline (myfile, line)!= NULL){
			pos = line.find(ACS_EMF_ACTIVE_DATA);
			if (pos != std::string::npos){
				bFileMmounted=true;
				break;
			}
		}	
		myfile.close();
	}else{
		DEBUG(1,"%s","ACS_EMF_Common::isFileMmounted - Unable to open file %s",ACS_EMF_FILE_PROC_MOUNTS);
	}
	DEBUG(1,"%s","ACS_EMF_Common::isFileMmounted - Leaving");
	return bFileMmounted;
}


/*===================================================================
ROUTINE: _execlp
======================================================================*/
ACE_INT32 ACS_EMF_Common::_execlp(const ACE_TCHAR *cmdStr)
{
	ACE_INT32 rCode=EMF_RC_OK,status;
	pid_t pid = fork();
	if (pid == EMF_RC_OK) {
		if(execlp("sh","sh", "-c", cmdStr, (ACE_TCHAR *) NULL) == EMF_RC_ERROR) {
			rCode = EMF_RC_ERROR;
		}
		exit(EMF_RC_OK);
	} else { if (pid < EMF_RC_OK) rCode = EMF_RC_ERROR; }

	if (rCode != EMF_RC_ERROR) {
		waitpid(pid, &status, 0);
		if (status != EMF_RC_OK) rCode = EMF_RC_ERROR;
	}

	pid_in_progress = pid;
	return rCode;
}

/*===================================================================
ROUTINE: removeFilesOrFolders
======================================================================*/
ACE_INT32 ACS_EMF_Common::removeFilesOrFolders(const ACE_TCHAR* fileList[EMF_COPY_MAX_NUMFILES],ACE_INT32 nooffiles)
{
	ACE_INT32 status = EMF_RC_ERROR;
	ACE_INT32 ret = EMF_RC_ERROR;
	ACE_TCHAR cmd[10000];

	DEBUG(1,"%s","ACS_EMF_Common::removeFilesOrFolders - Entering");
	DEBUG(1,"ACS_EMF_Common::removeFilesOrFolders - Total No. of Files is %d",nooffiles);
	
	sprintf(cmd,"%s ","rm -rf");
	for (int nIdx = 0; nIdx < nooffiles; nIdx++)
	{
		// Check all file/s are exist or not
		std::string fileMPath(ACS_EMF_ACTIVE_MEDIA);
		fileMPath.append(ACS_EMF_SLASH);
		std::string fileName(fileList[nIdx]);
		while (true){
			size_t found;
			found=fileName.find_last_of("/\\");
			if ( found == (fileName.size()-1) ){
				fileName = fileName.substr(0,found);
			}
			else{
				fileName = fileName.substr((found+1));
				break;
			}
		}

		fileMPath.append(fileName);
		ACE_TCHAR fileMPath1[EMF_CMD_MAXLEN];
		ACE_OS::strcpy(fileMPath1,fileMPath.c_str());
		fileList[nIdx] = fileMPath1;
		ACE_OS::strcat(cmd,space);
		ACE_OS::strcat(cmd,fileList[nIdx]);
	}
	// This logic for normal popen call
	int infp, outfp;
	DEBUG(1,"ACS_EMF_Common::removeFilesOrFolders - This logic for normal popen cmd value [%s]",cmd);
	if ((pid_in_progress = popen_with_pid(cmd, &infp, &outfp)) <= 0){
		DEBUG(1,"ACS_EMF_Common::removeFilesOrFolders - Error in executing the [%s]",cmd);
		DEBUG(1,"%s","ACS_EMF_Common::removeFilesOrFolders - Leaving");
		return EMF_RC_ERROR;
	}
	DEBUG(1,"ACS_EMF_Common::removeFilesOrFolders - PID of Job %d",pid_in_progress);
	waitpid(pid_in_progress,&status,0);
	if (WIFEXITED(status) ){
		ret=WEXITSTATUS(status);
		pid_in_progress = EMF_RC_ERROR;
		DEBUG(1,"ACS_EMF_Common::removeFilesOrFolders - [%s] execution status=[%d]",cmd,ret);
	}
	//      ret = 0;
	if (ret == 0 ){
		DEBUG(1,"%s","ACS_EMF_Common::removeFilesOrFolders - removeFilesOrFolders is Successfull");
		DEBUG(1,"%s","ACS_EMF_Common::removeFilesOrFolders - Leaving");
		return EMF_RC_OK;
	}
	else{
		DEBUG(1,"%s","ACS_EMF_Common::removeFilesOrFolders - removeFilesOrFolders failed");
		DEBUG(1,"%s","ACS_EMF_Common::removeFilesOrFolders - Leaving");
		return EMF_RC_ERROR;
	}
	DEBUG(1,"%s","ACS_EMF_Common::removeFilesOrFolders - Leaving");
	return EMF_RC_OK;
}

bool ACS_EMF_Common::isDVDConfigured(ACE_INT16 systemId){
	TScsiDev objScsiDev;
	bool bIsDvdInCs = false;
	objScsiDev.isFbnDvdConfigured(systemId, &bIsDvdInCs);
	return bIsDvdInCs;
}
bool ACS_EMF_Common::isThumbDriveHealthy()
{
	DEBUG(1,"%s","ACS_EMF_Common::isThumbDriveHealthy - Entering");
	bool myHealth = true;
	std::string myDummyFilePath("");
	if(getNodeState() == 1)
		myDummyFilePath += ACS_EMF_ACTIVE_MEDIA;
	else
		myDummyFilePath += ACS_EMF_PASSIVE_MEDIA;
	myDummyFilePath += "/DummyFile";
	DEBUG(1,"ACS_EMF_Common::isThumbDriveHealthy - Path of the file - %s",myDummyFilePath.c_str()); 
	ofstream myHealchCheckFile;
	myHealchCheckFile.open(myDummyFilePath.c_str());
	if(myHealchCheckFile.good()){
		if(myHealchCheckFile.is_open()){
			myHealchCheckFile << "This text is to check the healtiness of filesystem";
			DEBUG(1,"%s","ACS_EMF_Common::isThumbDriveHealthy - FileSystem is healthy");
		}else  
			myHealth = false;
	}else{
		myHealth = false;
		DEBUG(1,"%s","ACS_EMF_Common::isThumbDriveHealthy - FileSystem is not healthy");
	}
	myHealchCheckFile.close();
	DEBUG(1,"%s","ACS_EMF_Common::isThumbDriveHealthy - Leaving");
	return myHealth ;
}

ACE_INT32 ACS_EMF_Common::getPeerNodeState(int32_t &nodeState)
{
	int rCode=true;
	nodeState = 0;
	ACS_DSD_Client *pClient;
	ACS_DSD_Node localNode;

	pClient = new ACS_DSD_Client();
	pClient->get_local_node(localNode);

	DEBUG(1, "ACS_EMF_Common::getPeerNodeState Local Node  - nodeId:[%d], systemName:[%s], systemType:[%d], nodeState:[%d], nodeName [%s], nodeSide:[%d]",localNode.system_id, localNode.system_name, localNode.system_type, localNode.node_state, localNode.node_name, localNode.node_side);

	std::vector<ACS_DSD_Node> reachable_nodes;
	std::vector<ACS_DSD_Node> unreachable_nodes;

	rCode = pClient->query("EMFCMDSERVER", "EMF", acs_dsd::SYSTEM_TYPE_AP, reachable_nodes, unreachable_nodes);
	DEBUG(1, "ACS_EMF_Common::getPeerNodeState - Query from DSD return [%i]",rCode);
	if(!rCode) {
		ACE_UINT32 reachNodeSz=0, unReachNodeSz=0, configrdNodes=0;
		reachNodeSz = reachable_nodes.size();
		unReachNodeSz = unreachable_nodes.size();
		configrdNodes = reachNodeSz + unReachNodeSz;
		DEBUG(1, "ACS_EMF_Common::getPeerNodeState - reachable node vector size = [%d]", reachNodeSz);
		DEBUG(1, "ACS_EMF_Common::getPeerNodeState - unreachable node vector size = [%d]", unReachNodeSz);
		if (reachable_nodes.size() > 0) {

			std::vector<ACS_DSD_Node>::iterator it;
			it = reachable_nodes.begin();
			while( (it != reachable_nodes.end()) ){
				DEBUG(1, "ACS_EMF_Common::getPeerNodeState RemoteNode - nodeId:[%d], systemName:[%s], systemType:[%d], nodeState:[%d], nodeName [%s], nodeSide:[%d]",it->system_id, it->system_name, it->system_type,it->node_state, it->node_name, it->node_side);

				//if(reachable_nodes.size() <= 2) { //Single AP with DVD or USB
				if(configrdNodes <= 2) { //Single AP with DVD or USB
					DEBUG(1,"%s","ACS_EMF_Common::getPeerNodeState - Single AP and Media is configured on other Node.");
					if (it->node_state == 1) {
						DEBUG(1,"ACS_EMF_Common::getPeerNodeState - Passive node state - %d",it->node_state);
						nodeState = 1;
						break;
					}
				}
				else if(configrdNodes > 2){ //Dual AP with DVD
					DEBUG(1,"%s","ACS_EMF_Common::getPeerNodeState - Dual AP and DVD is configured on other Node.");
					if ((isDVDConfigured((*it).system_id)) && (it->node_state != 2)){
						if (((it->system_id) == (localNode.system_id)) && (it->node_state == 1)){
							nodeState = 1;
							break;
						}
						else if (((it->system_id) != (localNode.system_id)) && (it->node_state != 2)){
							nodeState = 1;
							break;
						}
						DEBUG(1,"ACS_EMF_Common::getPeerNodeState - Node configured with DVD is up and running: Node State - %d", it->node_state);
					}
				}
				++it;
			}
		}
	}
	else {
		DEBUG(1, "%s", "ACS_EMF_Common::getPeerNodeState - Client query is failed");
		DEBUG(1, "%s", "ACS_EMF_Common::getPeerNodeState - Leaving");
		rCode = EMF_RC_ERROR;
	}

	if(NULL != pClient) {
		delete(pClient);
		pClient = NULL;
	}
	return rCode;
}
//-----------------------------------------------------------------------------
