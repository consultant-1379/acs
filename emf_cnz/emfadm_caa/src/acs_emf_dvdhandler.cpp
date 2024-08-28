/*=================================================================== */
/**
   @file acs_emf_dvdhandler.cpp

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

#include <ctime>
#include <ace/ACE.h>
#include <stdio.h>
#include <sys/stat.h>
#include <mntent.h>
#include <iostream>
#include <string.h>
#include <acs_emf_execute.h>
//For Wait exit status
#include <sys/wait.h>
#include <acs_emf_defs.h>
#include "acs_emf_common.h"
#include "acs_emf_param.h"
#include "acs_emf_dvdhandler.h"
#include "acs_emf_tra.h"
#include "acs_emf_aeh.h"

#define INFO_BUFFER_SIZE 32767

using namespace std;

/*=====================================================================
                        GLOBAL VARIABLE
==================================================================== */
OmHandler immHandle;
ACE_UINT64 CDDiskSpace = 683593; // in KBs
ACE_UINT64 DVDDiskSpace = 4092000; // in KBs
//ACE_INT32 ACS_EMF_DVDHandler::cntValue = 0;
std::string emfInfoImplName;
extern const char* mediaOwnerFile;
/*===================================================================
   ROUTINE: ACS_EMF_DVDHandler
=================================================================== */
ACS_EMF_DVDHandler::ACS_EMF_DVDHandler(ACE_HANDLE endEvent)
{
	DEBUG(1,"%s","Entering ACS_EMF_DVDHandler::ACS_EMF_DVDHandler");

	if ( immHandle.Init() != ACS_CC_SUCCESS )
	{
		ERROR(1,"%s","ERROR: OmHandler Initialization FAILURE!!!");
	}
	m_stopHandleforDVD = endEvent;
	_lastOperation_error_text = 0;
	_historyResult_text = 0;
	// DVD locked (false = DVD is free)
	m_bDVDLocked = false;
	// Owner apg is unknow here
	m_iApgOwner = acs_dsd::SYSTEM_ID_UNKNOWN;
	cntValue = getHistoryInstancesCnt()+1;
	createStructObj();
	setLastHistOperStateToIDLEIfPROCESSING();
	DEBUG(1,"%s","Leaving ACS_EMF_DVDHandler::ACS_EMF_DVDHandler");
}//End of ACS_EMF_DVDHandler

/*===================================================================
   ROUTINE: ~ACS_EMF_DVDHandler
=================================================================== */
ACS_EMF_DVDHandler::~ACS_EMF_DVDHandler()
{
	DEBUG(1,"%s","Entering ACS_EMF_DVDHandler::~ACS_EMF_DVDHandler");
	// If still the operation is in PROCESSING state then set to failed
	char retriveData[512];
	ACE_INT32 result;

	std::string dnName;
	dnName.append(ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
	dnName.append(",");
	dnName.append(ACS_EMF_Common::parentDNofEMF);

	if((result = ACS_EMF_Common::fetchEMFHistoryAttribute(dnName,(ACE_TCHAR*) theEMFHistoryInfoNodeState,retriveData)) == ACS_CC_SUCCESS){
		if (ACE_OS::strcmp(retriveData,(ACE_TCHAR*)PROCESSING) == 0)
		{
			modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() ,(ACE_TCHAR*) theEMFHistoryInfoNodeState,(ACE_TCHAR*)IDLE);
			modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() ,(ACE_TCHAR*) theEMFHistoryInfoOperResult,(ACE_TCHAR*)FAILED);
		}
	}

	dnName.clear();
	if(immHandle.Finalize() != ACS_CC_SUCCESS)
	{
		DEBUG(1,"%s","Method OmHandler::Finalize Failure!!!");
	}
	ItsScsiUtilityNamespace::TScsiDev objScsiDev;
	// Check if the folder "/media" is mounted on.
	if(objScsiDev.isDirMounted("/media") == true){
		ACS_EMF_Common::unmountPassiveDVDData(false);
	}
	DEBUG(1,"%s","Leaving ACS_EMF_DVDHandler::~ACS_EMF_DVDHandler");
}

ACE_UINT32 ACS_EMF_DVDHandler::ExportFiles(int32_t sysId, acs_dsd::NodeSideConstants nodeSide, ACS_EMF_DATA* pData)
{
	DEBUG(1,"%s","Entering ACS_EMF_DVDHandler::ExportFiles");
	ACS_EMF_ParamList Params;
	//int result;
	Params.Decode(pData->Data);
	ACE_TCHAR dev_info[10];
	acs_emf_common::EMF_DVD_WRITETYPE writeType = acs_emf_common::WRITETYPE_NEWSESSION;

	if (Params.CmdCode == EMF_CMD_TYPE_COPYTOMEDIA){
		DEBUG(1,"%s","Copy-To-DVD action received.");
	}
	else if (Params.CmdCode == EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA){
		DEBUG(1,"%s","Erase and Copy-To-DVD action received.");
		writeType = acs_emf_common::WRITETYPE_FORMAT_WRITE;
	}
	// Add a char pointer to each argument
	const ACE_TCHAR* lpszArgs[3] = {
			Params.Data[0],
			Params.Data[1],
			Params.Data[2]};

	for (int i = 0; i < 3; i++) DEBUG(1,"%s%d = %s\n","Argument",i,lpszArgs[i]);

	ACE_TCHAR imageName[EMF_IMAGENAME_MAXLEN+1] = {0};
	if (ACE_OS::strcmp(lpszArgs[0], ACE_TEXT("")) != 0)
	{
		ACE_OS::strncpy(imageName, lpszArgs[0],EMF_IMAGENAME_MAXLEN);
	}
	else
	{
		// Generate a new volume name for the new session
		ACE_OS::strncpy(imageName,ACS_EMF_Common::GenerateVolumeName(),EMF_IMAGENAME_MAXLEN); // incase of -l option absence.
		DEBUG(1,"Generated Image Label Name is %s",imageName);
	}

	bool bVerify = (ACE_OS::strcmp(lpszArgs[1], ACE_TEXT("true")) == 0);
	bool bOverwrite = (ACE_OS::strcmp(lpszArgs[2], ACE_TEXT("true")) == 0);
	DEBUG(1,"Image Label Name is %s",imageName);
	DEBUG(1,"Verify flag status = %d  Overwrite flag status = %d",bVerify,bOverwrite);
	int nNum = (Params.NumOfData() - 3); // No of files
	DEBUG(1,"Total No. of File arguments received from action is %d",nNum);

	const ACE_TCHAR* lpszFile;
	const ACE_TCHAR* fileList[EMF_COPY_MAX_NUMFILES]; // contains file list
	ACE_INT32 nooffiles = 0;     // No of files

	for (int nIdx = 0; nIdx < nNum; nIdx++)
	{
		lpszFile = (const ACE_TCHAR*)Params.Data[3 + nIdx];
		DEBUG(1,"Files are : %s\n",lpszFile);
		fileList[nooffiles] = lpszFile;
		nooffiles++;
	}
	DEBUG(1,"Total No. of Files is %d",nooffiles);
	std::string filestrlist;
	for (int i = 0 ;i < nooffiles; i++)
	{
		std::string str(fileList[i]);
		size_t found;
		found=str.find_last_of("/\\");
		filestrlist.append(str.substr(found+1));
		if (nooffiles > 1 ) filestrlist.append(" ");
	}
	DEBUG(1, "filestrlist =  %s",filestrlist.c_str());

	DVDHDLR_EXIT_IF_ENDSIGNAL();

	ACE_TCHAR mediumType [EMF_MEDIATYPE_MAXLEN] = {'\0'};
	if(ACS_EMF_Common::getMediumType(mediumType) == -1)
	{
		DEBUG(1,"%s\n","Unable to fetch Medium type !!!");

		historyResult_text(FAILED);
		lastOperation_error_text("Media is not writable");
		return EMF_RC_DISCMEDIAERROR;
	}
	DEBUG(1,"Medium Type returned = %s",mediumType);

	ACE_INT64 mediaState = ACS_EMF_Common::isDVDEmpty();
	if(mediaState == acs_emf_common::DVDMEDIA_STATE_ERROR)
	{
		ERROR(1,"%s","Unable to fetch size of Media !!!");
		historyResult_text(FAILED);
		lastOperation_error_text("Media is not readable");
		return EMF_RC_DISCMEDIAERROR;
	}

	DVDHDLR_EXIT_IF_ENDSIGNAL();
	ACS_EMF_Common::createDirectory(ACS_EMF_ACTIVE_DATA);
	ACS_EMF_Common::createDirectory(ACS_EMF_CLUSTER_ACTIVEDATA);

	DVDHDLR_EXIT_IF_ENDSIGNAL();
	if(ACS_EMF_Common::moutFileMOnPassive(sysId, nodeSide) == -1){
		DEBUG(1,"%s","Mounting fileM folder failed on passive node");
		historyResult_text(FAILED);
		lastOperation_error_text("NFS SHARING IS FAILED.TRY AGAIN.");
		return EMF_RC_DVDDEVICENOTFOUND;
	}
	if (ACS_EMF_Common::removeFilesOnPassive() != 0){
		ERROR(1,"%s","Removing files on passive node failed!!");
	}
	DVDHDLR_EXIT_IF_ENDSIGNAL();

	ACS_EMF_Common::copyFileListOnPassive(fileList,nooffiles);
	//Dynamically allocation of 2d char array
	ACE_TCHAR **actualFileList = new ACE_TCHAR*[nooffiles];
	for(int i=0;i<nooffiles;i++)
	{
		actualFileList[i] = new ACE_TCHAR[260];
	}
	//Modify fileList with actual path on passive node
	std::string fileName("");
	std::string fileMPath = NBI_root;

	for (int i = 0 ;i < nooffiles; i++)
	{
		DEBUG(0," Before modifying filelist path = %s",fileList[i]);
		fileName.clear();
		fileName = fileList[i];
		size_t found;
		found=fileName.find_last_of("/\\");
		fileName = fileName.substr((found));
		std::string activeDataPath(ACS_EMF_CLUSTER_ACTIVEDATA);
		activeDataPath.append(fileName);

		ACE_UINT32 columns = 0;
		for(;columns < activeDataPath.length();columns++)
		{
			actualFileList[i][columns] =  activeDataPath[columns];
		}
		actualFileList[i][columns] = '\0';
		DEBUG(1,"After modifying filelist path  = %s",actualFileList[i]);
	}

	// Image size check before writing on disk
	ACE_TCHAR filesize_str[20];
	for (int i = 0 ;i < nooffiles; i++)
	{
		DEBUG(0," Before consume space actualFileList[i] = %s",actualFileList[i]);
	}
	if(ACS_EMF_Common::getGivenFilesConsumedSpace((const ACE_TCHAR**)actualFileList,nooffiles,filesize_str) == -1)
	{
		DEBUG(1,"%s","Fetching consumed space for files failed!!!");
	}
	ACE_INT64 fileSize = atol(filesize_str);
	//ACE_TCHAR usedSpace_str[20]= {0};
	ACE_UINT64 iUsedSpace = 0;
	ACE_UINT64 usedSpace = 0;
	if (mediaState != acs_emf_common::DVDMEDIA_STATE_EMPTY &&
			Params.CmdCode != EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA)
	{
		 writeType = acs_emf_common::WRITETYPE_APPENDING;
		// Medium already has some data
        //ACS_EMF_Common::getMediaUsedSpaceOnPassive(useSpace);
        ACS_EMF_Common::getMediaUsedSpace(&iUsedSpace);
        usedSpace = iUsedSpace/1024;
        DEBUG(1,"Current Size of media %ld in KB",usedSpace);
	}
	if ( (fileSize + usedSpace) > DVDDiskSpace )
	{
		DEBUG(1,"%s","Copy TO DVD Failed! Not enough space on medium ");
		historyResult_text("Failed due to enough space is not available on medium to copy");
		lastOperation_error_text("File too large (or) Not enough space in medium to copy");

		//Deallocation of 2d char array before leaving
		for( int i = 0 ; i < nooffiles; i++ )
			delete [] actualFileList[i] ;
		delete [] actualFileList;
		ACS_EMF_Common::unmountDVDOnPassive();
		return EMF_RC_FILETOLARGE;
	}

	DVDHDLR_CHECK_ENDSIGNAL();
		for( int i = 0 ; i < nooffiles; i++ ) delete [] actualFileList[i] ;
		delete [] actualFileList;
	DVDHDLR_IF_ENDSIGNAL_EXIT();

	int writeImageExitCode;
	if( (writeImageExitCode= ACS_EMF_Common::writeImageOnMedium((const ACE_TCHAR**)actualFileList, nooffiles, dev_info, mediumType,writeType,bVerify,imageName)!=0 ))
	{
		DEBUG(1,"%s","Writing Image on Medium is failed!!!");
		historyResult_text("Failed");
		(writeImageExitCode == 252)? lastOperation_error_text("FAILED DUE TO MEDIA IS NOT IN APPENDABLE STATE. USE formatAndExportToMedia ACTION, IF OLD FILES ARE NOT REQUIRED."):
				lastOperation_error_text("Exporting on media is failed.");
		//return EMF_RC_COPYTOTRACKFAILED;
	}
	else{
		historyResult_text("SUCCESSFUL");
		lastOperation_error_text("-");
	}
	DVDHDLR_CHECK_ENDSIGNAL();
	for( int i = 0 ; i < nooffiles; i++ ) delete [] actualFileList[i] ;
	delete [] actualFileList;
	DVDHDLR_IF_ENDSIGNAL_EXIT();

	if(resetMedia(MEDIA_ON_PARTNER_NODE) != EMFMEDIAHANDLER_OK)
	{
		//Deallocation of 2d char array before leaving
		for( int i = 0 ; i < nooffiles; i++ )
			delete [] actualFileList[i] ;
		delete [] actualFileList;
		DEBUG(1,"%s","ACS_EMF_DVDHandler::ExportFiles, resetMedia failed");
		historyResult_text("FAILED WHILE RESET MEDIA");
		lastOperation_error_text("-");
		return EMF_RC_NOK;
	}

	DEBUG(1,"%s","Data verification on Medium is SUCCESS\n");
	ACS_EMF_Common::unmountDVDOnPassive();

	// Removing Temp image i.e /tmp/image.iso
	if(ACS_EMF_Common::removeTemporaryImage(imageName) == -1)
	{
		DEBUG(1,"%s","Removing Temporary image is failed!!!");
	}

	//Deallocation of 2d char array before leaving
	for( int i = 0 ; i < nooffiles; i++ )
			delete [] actualFileList[i] ;
		delete [] actualFileList;

	DEBUG(1,"%s","Leaving ACS_EMF_DVDHandler::ExportFiles");
	return (writeImageExitCode == 0)? EMF_RC_OK: EMF_RC_COPYTOTRACKFAILED;
}

ACE_UINT32 ACS_EMF_DVDHandler::ImportFiles(ACS_EMF_DATA* pData)
{
	DEBUG(1,"%s","Entering ACS_EMF_DVDHandler::ImportFiles");
	ACS_EMF_ParamList Params;
	Params.Decode(pData->Data);
	if (Params.CmdCode == EMF_CMD_TYPE_COPYFROMMEDIA)
	{
		DEBUG(1,"%s","Copy-from-DVD action received.");
		initEMFHistRuntimeHandler();

		const ACE_TCHAR* lpszArgs = Params.Data[0];
		bool bOverwrite = (ACE_OS::strcmp(lpszArgs, ACE_TEXT("yes")) == 0);

		int nNum = Params.NumOfData(); // No of files
		DEBUG(1,"No of arguments received from action is %d",nNum);

		const ACE_TCHAR* lpszFile;
		const ACE_TCHAR* fileList[100]; // contains file list

		ACE_INT32 nooffiles = 0;     // No of files

		for (int nIdx = 1; nIdx < nNum; nIdx++)
		{
			lpszFile = (const ACE_TCHAR*)Params.Data[nIdx];
			// Check all file/s are exist or not
			DEBUG(1,"%s\n",lpszFile);

			fileList[nooffiles] = lpszFile;
			nooffiles++;
		}

		DEBUG(1,"Total No of file Arguments is %d\n",nooffiles);

		std::string filestrlist;
		for (int i = 0 ;i < nooffiles; i++)
		{
			if(nooffiles > 1)
			{
				if (i < nooffiles-1)
				{
					std::string str(fileList[i]);
					size_t found;
					found=str.find_last_of("/\\");
					filestrlist.append(str.substr(found+1));
					filestrlist.append(" ");
				}
			}
			else
			{
				filestrlist.append("NONE");
			}
		}
		for (int i = 0 ;i < nooffiles; i++)
		{
			DEBUG(1,"File list for import operation = %s",fileList[i]);
		}
		/* For EMF History Log Info--> push the runtime object attributes info in LIFO Order*/

		DEBUG(1,"Count of History Object Instances is %d",cntValue);
		modifyStructObjInitialAttr(IMPORT_FROM_MEDIA);

		if (cntValue == 1)
		{
			string rdnOfHistoryObj(ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN);
			createRuntimeObjectsforEmfHistoryInfoClass(rdnOfHistoryObj);
			rdnOfHistoryObj.clear();
			rdnOfHistoryObj.append(ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			createRuntimeObjectsforEmfHistoryInfoClass(rdnOfHistoryObj);

			modifyInitialHistoryAttrs(Params.CmdCode,filestrlist);
		}
		else
		{
			modifyInitialHistoryAttrs(Params.CmdCode,filestrlist);
			/*cntValue > 1 */
			if(!moveObjectsOneLevelDown(cntValue))
			{
				DEBUG(1,"%s","moveObjectsOneLevelDown failed !!!");
				ERROR(1,"%s","Stop signal is initiated by service!!!");

				// modify result and endtime
				modifyEndHistoryAttrs((ACE_TCHAR*) FAILED);
				copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
				modifyStructObjFinalAttr(FAILURE,"DVD operation is interrupted because of service is stopped");
				return EMF_RC_SERVERNOTRESPONDING;
			}
		}

		cntValue++;
		/*cntValue > 20 */
		if (cntValue > 20) cntValue = 20;

		EXIT_IF_ENDSIGNAL();
		ACE_TCHAR errorText[256];
		if (ACS_EMF_Common::copyDataToDestFolder(fileList,nooffiles,bOverwrite,errorText) != 0)
		{
			DEBUG(1,"%s","Copying CD/DVD Data to the given destination is failed!!!");

			// modify result and endtime
			modifyEndHistoryAttrs(errorText);
			copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			modifyStructObjFinalAttr(FAILURE,errorText);
			DEBUG(1,"%s","Leaving ACS_EMF_DVDHandler::ImportFiles");
			return EMF_RC_COPYFROMDVDFAILED;
		}

		// modify result and endtime
		modifyEndHistoryAttrs((ACE_TCHAR*)SUCCESSFUL);
		modifyStructObjFinalAttr(SUCCESS,"-");
		copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);

	}
	DEBUG(1,"%s","Leaving ACS_EMF_DVDHandler::ImportFiles");
	return EMF_RC_OK;
}
/*===================================================================
   ROUTINE: DVDOperation
=================================================================== */
ACE_UINT32 ACS_EMF_DVDHandler::mediaOperation(ACS_EMF_DATA* pData /*, ACE_HANDLE hPipe */)
{
	DEBUG(1,"%s","Entering ACS_EMF_DVDHandler::DVDOperation");
	ACS_EMF_ParamList Params;
	Params.Decode(pData->Data);

	//int result;
	ACE_TCHAR dev_info[10]; // device no
	DEBUG(1,"Type of DVDOperation Received is %d",Params.CmdCode);

	if (Params.CmdCode == EMF_CMD_TYPE_COPYFROMMEDIA)
	{
		INFO(1,"%s","Copy-from-DVD action received.");

		const ACE_TCHAR* lpszArgs = Params.Data[0];
		bool bOverwrite = (ACE_OS::strcmp(lpszArgs, ACE_TEXT("yes")) == 0);

		int nNum = Params.NumOfData(); // No of files
		DEBUG(1,"No of arguments received from action is %d",nNum);

		const ACE_TCHAR* lpszFile;
		const ACE_TCHAR* fileList[100]; // contains file list

		ACE_INT32 nooffiles = 0;     // No of files

		for (int nIdx = 1; nIdx < nNum; nIdx++)
		{
			lpszFile = (const ACE_TCHAR*)Params.Data[nIdx];
			// Check all file/s are exist or not
			DEBUG(1,"%d:%s\n",nooffiles,lpszFile);

			fileList[nooffiles] = lpszFile;
			nooffiles++;
		}

		DEBUG(1,"Total No of file Arguments is %d",nooffiles);
		for (int nIdx = 0; nIdx < nooffiles; nIdx++)
		{
			DEBUG(1,"The list of File is/are: %s\n",fileList[nIdx]);
		}

		std::string filestrlist;
		for (int i = 0 ;i < nooffiles; i++)
		{
			if(nooffiles > 1)
			{
				if (i < nooffiles-1)
				{
					std::string str(fileList[i]);
					size_t found;
					found=str.find_last_of("/\\");
					filestrlist.append(str.substr(found+1));
					filestrlist.append(" ");
				}
			}
			else
			{
				filestrlist.append("NONE");
			}
		}

		/* For EMF History Log Info--> push the runtime object attributes info in LIFO Order*/

		INFO(1,"Count of History Object Instances is %d",cntValue);
		modifyStructObjInitialAttr(IMPORT_FROM_MEDIA);
		if (cntValue == 1)
		{
			string rdnOfHistoryObj(ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN);
			createRuntimeObjectsforEmfHistoryInfoClass(rdnOfHistoryObj);
			rdnOfHistoryObj.clear();
			rdnOfHistoryObj.append(ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			createRuntimeObjectsforEmfHistoryInfoClass(rdnOfHistoryObj);
			modifyInitialHistoryAttrs(Params.CmdCode,filestrlist);
		}
		else
		{
			modifyInitialHistoryAttrs(Params.CmdCode,filestrlist);
			/*cntValue > 1 */
			moveObjectsOneLevelDown(cntValue);
		}

		cntValue++;
		/*cntValue > 20 */
		if (cntValue > 20) cntValue = 20;
		EXIT_IF_ENDSIGNAL();

		ACE_TCHAR errorText[256];
		if (ACS_EMF_Common::copyDataToDestFolder(fileList,nooffiles,bOverwrite,errorText) != 0)
		{
			ERROR(1,"%s","Copying DVD Data to the given destination folder is failed!!!");

			// modify result and endtime
			modifyEndHistoryAttrs(errorText);
			copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			modifyStructObjFinalAttr(FAILURE,errorText);
			return EMF_RC_COPYFROMDVDFAILED;
		}

		// modify result and endtime
		modifyEndHistoryAttrs((ACE_TCHAR*)SUCCESSFUL);
		copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
		modifyStructObjFinalAttr(SUCCESS,"-");
		return EMF_RC_OK;
	}
	else if (Params.CmdCode == EMF_CMD_TYPE_COPYTOMEDIA)
	{
		DEBUG(1,"%s","Copy-To-DVD command received.");
		// Add a char pointer to each argument
		const ACE_TCHAR* lpszArgs[3] = {
				Params.Data[0],
				Params.Data[1],
				Params.Data[2]
		};

		for (int i = 0; i < 3; i++)
		{
			DEBUG(1,"%s%d = %s","Argument",i,lpszArgs[i]);
		}

		ACE_TCHAR imageName[128];

		if (ACE_OS::strcmp(lpszArgs[0], ACE_TEXT("")) != 0)
		{
			ACE_OS::strcpy(imageName, lpszArgs[0]);
		}
		else
		{
			// Generate a new volume name for the new session
			ACE_OS::strcpy(imageName,ACS_EMF_Common::GenerateVolumeName()); // incase of -l option absence.
			DEBUG(1,"Generated Image Label Name is %s",imageName);
		}

		bool bVerify = (ACE_OS::strcmp(lpszArgs[1], ACE_TEXT("true")) == 0);
		bool bOverwrite = (ACE_OS::strcmp(lpszArgs[2], ACE_TEXT("true")) == 0);
		DEBUG(1,"Image Label Name is %s",imageName);
		DEBUG(1,"Verify flag status = %d  Overwrite flag status = %d",bVerify,bOverwrite);
		int nNum = (Params.NumOfData() - 3); // No of files
		DEBUG(1,"Total No. of File arguments received is %d",nNum);

		const ACE_TCHAR* lpszFile;
		const ACE_TCHAR* fileList[100] = {'\0'}; // contains file list
		ACE_INT32 nooffiles = 0;     // No of files

		for (int nIdx = 0; nIdx < nNum; nIdx++)
		{
			lpszFile = (const ACE_TCHAR*)Params.Data[3 + nIdx];
			// Check all file/s are exist or not
			DEBUG(1,"%s",lpszFile);

			fileList[nooffiles] = lpszFile;
			nooffiles++;
		}

		DEBUG(1,"Total No. of Files is %d",nooffiles);

		std::string filestrlist;
		for (int i = 0 ;i < nooffiles; i++)
		{
			std::string str(fileList[i]);
			size_t found;
			found=str.find_last_of("/\\");
			filestrlist.append(str.substr(found+1));
			if (nooffiles > 1 ) filestrlist.append(" ");
		}

		/* For EMF History Log Info--> push the runtime object attributes info in LIFO Order*/

		DEBUG(1,"Count of History Object Instances = %d\n",cntValue);

		if(m_stopFlag)
		{
			ERROR(1,"%s","EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
			DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, stopFlag is true");
			return EMF_RC_NOK;
		}
		modifyStructObjInitialAttr(EXPORT_TO_MEDIA);
		if (cntValue == 1)
		{
			string rdnOfHistoryObj(ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN);
			createRuntimeObjectsforEmfHistoryInfoClass(rdnOfHistoryObj);
			rdnOfHistoryObj.clear();
			rdnOfHistoryObj.append(ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			createRuntimeObjectsforEmfHistoryInfoClass(rdnOfHistoryObj);

			modifyInitialHistoryAttrs(Params.CmdCode,filestrlist);
		}
		else
		{
			modifyInitialHistoryAttrs(Params.CmdCode,filestrlist);
			/*cntValue > 1 */
			moveObjectsOneLevelDown(cntValue);
		}

		cntValue++;
		/*cntValue > 20 */
		if (cntValue > 20) cntValue = 20;

		EXIT_IF_ENDSIGNAL();
		ACE_TCHAR mediumType [EMF_MEDIATYPE_MAXLEN] = {'\0'};
		if(ACS_EMF_Common::getMediumType(mediumType) == -1)
		{
			ERROR(1,"%s","Unable to fetch type of Media !!!");
			modifyEndHistoryAttrs((ACE_TCHAR*) FAILED);
			//copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			modifyStructObjFinalAttr(FAILURE,"Media is not writable");
			return EMF_RC_DISCMEDIAERROR;
		}

		DEBUG(1,"Medium Type returned = %s",mediumType);

		// Image size check before writing on disk
		ACE_TCHAR usedSpace[20];
		if(ACS_EMF_Common::getGivenFilesConsumedSpace(fileList,nooffiles,usedSpace) == -1)
		{
			ERROR(1,"%s","Fetching consumed space for files failed!!!");
		}
		ACE_UINT64 fileSize = atol(usedSpace);
		if (fileSize > DVDDiskSpace)
		{
			ERROR(1,"%s","File(s) Data Too large to store on DVD");
			modifyEndHistoryAttrs((ACE_TCHAR*)"Failed due to enough space is not available on medium to copy");
			copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			modifyStructObjFinalAttr(FAILURE,"File too large");
			return EMF_RC_FILETOLARGE;
		}
		EXIT_IF_ENDSIGNAL();
		ACE_INT32 mediaStatus;
		mediaStatus = ACS_EMF_Common::isDVDEmpty();

		if (mediaStatus == acs_emf_common::DVDMEDIA_STATE_EMPTY)
		{
			// Medium is New
			acs_emf_common::EMF_DVD_WRITETYPE writeType = acs_emf_common::WRITETYPE_NEWSESSION;

			EXIT_IF_ENDSIGNAL();
			int writeImageExitCode;
			if((writeImageExitCode=ACS_EMF_Common::writeImageOnMedium(fileList, nooffiles, dev_info, mediumType,writeType,bVerify,imageName) !=0))
			{
				ERROR(1,"%s","Writing Image on Medium is failed!!!");
				modifyEndHistoryAttrs((ACE_TCHAR*)"Failed");
				if (writeImageExitCode == 252)
					modifyStructObjFinalAttr(FAILURE,"FAILED DUE TO MEDIA IS NOT IN APPENDABLE STATE. USE formatAndExportToMedia ACTION, IF OLD FILES ARE NOT REQUIRED.");
				else
					modifyStructObjFinalAttr(FAILURE,"Exporting on media is failed.");
				copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);

				int oper_result = resetMedia(MEDIA_ON_CURRENT_NODE);
				if( oper_result == EMFMEDIAHANDLER_STOPRECVD)
				{
					DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, resetMedia failed");
					modifyStructObjFinalAttr(FAILURE,"DVD operation is interrupted because of service is stopped");
				}
				else if( oper_result != EMFMEDIAHANDLER_OK)
				{
					DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, resetMedia failed");
					modifyStructObjFinalAttr(FAILURE,"FAILED WHILE RESET MEDIA");
				}
				ACS_EMF_Common::unmountDVDData();
				return EMF_RC_COPYTOTRACKFAILED;
			}
			// modify result and endtime
			modifyEndHistoryAttrs((ACE_TCHAR*)SUCCESSFUL);
			modifyStructObjFinalAttr(SUCCESS,"-");
			copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
		}
		else
		{
			//isMediaEmpty = false;
			ACS_EMF_Common::unmountDVDData();
			//ACE_INT64 mediaSize = ACS_EMF_Common::isDVDEmpty();
			// Medium already has some data
			//ACE_TCHAR useSpace[20];
			ACE_UINT64 iUsedSpace=0;
			ACE_UINT64 usedSpace = 0;
//			ACS_EMF_Common::getMediaUsedSpaceOnPassive(useSpace);
			ACS_EMF_Common::getMediaUsedSpace(&iUsedSpace);
			usedSpace = iUsedSpace / 1024;
			DEBUG(1,"Current Size of media %llu in KB",usedSpace);
			//ACE_INT32 actualSize = 4092000 - atol(useSpace);

			//if (fileSize > actualSize)

			if ( (fileSize + usedSpace) > DVDDiskSpace )
			{
				DEBUG(1,"%s","File(s) Data Too large to store on media");
				modifyEndHistoryAttrs((ACE_TCHAR*)"Failed due to enough space is not available on medium to copy");
				copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
				modifyStructObjFinalAttr(FAILURE,"File too large (or) Not enough space in medium to copy");
				ACS_EMF_Common::unmountDVDData();
				return EMF_RC_FILETOLARGE;
			}
			acs_emf_common::EMF_DVD_WRITETYPE writeType = acs_emf_common::WRITETYPE_APPENDING;
			if(m_stopFlag)
			{
				DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, stopFlag true");
				return -1;
			}
			int writeImageExitCode;

			EXIT_IF_ENDSIGNAL()
			if((writeImageExitCode=ACS_EMF_Common::writeImageOnMedium(fileList, nooffiles, dev_info, mediumType,writeType,bVerify,imageName)) != 0)
			{
				ERROR(1,"%s","Writing Image on Medium is failed");
				// modify result and end time
				modifyEndHistoryAttrs((ACE_TCHAR*)"Failed");
				copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
				if (writeImageExitCode == 252)
					modifyStructObjFinalAttr(FAILURE,"FAILED DUE TO MEDIA IS NOT IN APPENDABLE STATE. USE formatAndExportToMedia ACTION, IF OLD FILES ARE NOT REQUIRED.");
				else
					modifyStructObjFinalAttr(FAILURE,"Exporting on media is failed.");

				int oper_result = resetMedia(MEDIA_ON_CURRENT_NODE);
				if( oper_result == EMFMEDIAHANDLER_STOPRECVD)
				{
					DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, resetMedia failed");
					modifyStructObjFinalAttr(FAILURE,"DVD operation is interrupted because of service is stopped");
				}
				else if( oper_result != EMFMEDIAHANDLER_OK)
				{
					DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, resetMedia failed");
					modifyStructObjFinalAttr(FAILURE,"FAILED WHILE RESET MEDIA");
				}
				ACS_EMF_Common::unmountDVDData();
				return EMF_RC_COPYTOTRACKFAILED;
			}
			// modify result and endtime
			if (!bVerify)
			{
				modifyEndHistoryAttrs((ACE_TCHAR*)SUCCESSFUL);
				modifyStructObjFinalAttr(SUCCESS,"-");
				copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			}
		}

		if (bVerify)
		{
			int oper_result = resetMedia(MEDIA_ON_CURRENT_NODE);
			if( oper_result == EMFMEDIAHANDLER_STOPRECVD)
			{
				DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, resetMedia failed");
				modifyEndHistoryAttrs((ACE_TCHAR*)"Failed");
				copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
				modifyStructObjFinalAttr(FAILURE,"DVD operation is interrupted because of service is stopped");
			}
			else if( oper_result != EMFMEDIAHANDLER_OK)
			{
				DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, resetMedia failed");
				modifyEndHistoryAttrs((ACE_TCHAR*)"Failed");
				copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
				modifyStructObjFinalAttr(FAILURE,"FAILED WHILE RESET MEDIA");
			}


#if 0
/*	Reason for ignoring verfication of checksum : growisofs command having
	in-built mechanism to verify checksum. Moreover, verifychecksum() holds
	huge memory which leads to reboot for some times.*/
			// checksum verification
			if(m_stopFlag ||(ACS_EMF_Common::verifychecksum() == -1))
			{
				if(m_stopFlag)
				{
					ERROR(1,"%s","EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
					DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, stopFlag true");
					modifyEndHistoryAttrs((ACE_TCHAR*) FAILED);
					copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
					modifyStructObjFinalAttr(FAILURE,"DVD operation is interrupted because of service is stopped");
					return EMF_RC_NOK;
				}
				modifyEndHistoryAttrs((ACE_TCHAR*)"Failed while doing Data verification");
				copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
				DEBUG(1,"%s","Data verification on Medium is FAILED!!!");
			}
			DEBUG(1,"%s","Data verification on Medium is SUCCESS\n");

#endif

			else{
				modifyEndHistoryAttrs((ACE_TCHAR*)SUCCESSFUL);
				modifyStructObjFinalAttr(SUCCESS,"-");
				copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
				ACS_EMF_Common::unmountDVDData();
			}
		}
		EXIT_IF_ENDSIGNAL();

		// Removing Temp image i.e /tmp/image.iso
		if(ACS_EMF_Common::removeTemporaryImage(imageName) == -1)
		{
			DEBUG(1,"%s","Removing Temporary image is failed!!!");

		}
		return EMF_RC_OK;
	}
	else if (Params.CmdCode == EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA)
	{
		DEBUG(1,"%s","Erase and Copy-To-DVD action received.");

		// Add a char pointer to each argument
		const ACE_TCHAR* lpszArgs[4] = {
				Params.Data[0],
				Params.Data[1],
				Params.Data[2]
		};

		for (int i = 0; i < 3; i++)
		{
			DEBUG(1,"%s%d = %s\n","Argument",i,lpszArgs[i]);
		}

		ACE_TCHAR imageName[128];

		if (ACE_OS::strcmp(lpszArgs[0], ACE_TEXT("")) != 0)
		{
			ACE_OS::strcpy(imageName, lpszArgs[0]);
		}
		else
		{
			// Generate a new volume name for the new session
			ACE_OS::strcpy(imageName,ACS_EMF_Common::GenerateVolumeName()); // incase of -l option absence.
			DEBUG(1,"Generated Image Label Name is %s",imageName);
		}

		bool bVerify = (ACE_OS::strcmp(lpszArgs[1], ACE_TEXT("true")) == 0);
		bool bOverwrite = (ACE_OS::strcmp(lpszArgs[2], ACE_TEXT("true")) == 0);
		DEBUG(1,"Image Label Name is %s",imageName);
		DEBUG(1,"Verify flag status = %d  Overwrite flag status = %d",bVerify,bOverwrite);
		int nNum = (Params.NumOfData() - 3); // No of files
		DEBUG(1,"Total No. of File arguments received is %d",nNum);

		const ACE_TCHAR* lpszFile;
		const ACE_TCHAR* fileList[100] = {'\0'}; // contains file list
		ACE_INT32 nooffiles = 0;     // No of files

		for (int nIdx = 0; nIdx < nNum; nIdx++)
		{
			lpszFile = (const ACE_TCHAR*)Params.Data[3 + nIdx];
			// Check all file/s are exist or not
			DEBUG(1,"%s",lpszFile);

			fileList[nooffiles] = lpszFile;
			nooffiles++;
		}

		DEBUG(1,"Total No. of Files is %d",nooffiles);

		std::string filestrlist;
		for (int i = 0 ;i < nooffiles; i++)
		{
			std::string str(fileList[i]);
			size_t found;
			found=str.find_last_of("/\\");
			filestrlist.append(str.substr(found+1));
			if (nooffiles > 1 ) filestrlist.append(" ");
		}

		/* For EMF History Log Info--> push the runtime object attributes info in LIFO Order*/

		DEBUG(1,"Count of History Object Instances = %d",cntValue);
		if(m_stopFlag)
		{
			ERROR(1,"%s","EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
			DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, stopFlag is true");
			return EMF_RC_NOK;
		}
		modifyStructObjInitialAttr(FORMAT_AND_EXPORT_TO_MEDIA);
		if (cntValue == 1)
		{
			string rdnOfHistoryObj(ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN);
			createRuntimeObjectsforEmfHistoryInfoClass(rdnOfHistoryObj);
			rdnOfHistoryObj.clear();
			rdnOfHistoryObj.append(ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			createRuntimeObjectsforEmfHistoryInfoClass(rdnOfHistoryObj);

			modifyInitialHistoryAttrs(Params.CmdCode,filestrlist);
		}
		else
		{
			modifyInitialHistoryAttrs(Params.CmdCode,filestrlist);
			/*cntValue > 1 */
			if(!moveObjectsOneLevelDown(cntValue))
			{
				DEBUG(1,"%s","moveObjectsOneLevelDown failed !!!");
				ERROR(1,"%s","Stop signal is initiated by service!!!");

				// modify result and endtime
				modifyEndHistoryAttrs((ACE_TCHAR*) FAILED);
				copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
				modifyStructObjFinalAttr(FAILURE,"DVD operation is interrupted because of service is stopped");
				return EMF_RC_SERVERNOTRESPONDING;
			}
		}

		cntValue++;
		/*cntValue > 20 */
		if (cntValue > 20) cntValue = 20;
		EXIT_IF_ENDSIGNAL();

		ACE_TCHAR mediumType [EMF_MEDIATYPE_MAXLEN] = {'\0'};
		if(ACS_EMF_Common::getMediumType(mediumType) == -1)
		{
			DEBUG(1,"%s","Unable to fetch Medium type !!!");
			modifyEndHistoryAttrs((ACE_TCHAR*) FAILED);
			copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			modifyStructObjFinalAttr(FAILURE,"Media is not writable");
			return EMF_RC_DISCMEDIAERROR;
		}

		DEBUG(1,"Medium Type returned = %s",mediumType);

		// Image size check before writing on disk
		ACE_TCHAR usedSpace[20];
		if(ACS_EMF_Common::getGivenFilesConsumedSpace(fileList,nooffiles,usedSpace) == -1)
		{
			DEBUG(1,"%s","Fetching consumed space for files failed!!!");
		}
		ACE_UINT64 fileSize = atol(usedSpace);

		if (fileSize > DVDDiskSpace)
		{
			DEBUG(1,"%s","File(s) Data Too large to store on DVD");
			modifyEndHistoryAttrs((ACE_TCHAR*)"Failed due to enough space is not available on medium to copy");
			copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			modifyStructObjFinalAttr(FAILURE,"File too large");
			return EMF_RC_FILETOLARGE;
		}
		ACE_OS::sleep(2);
		// Mount DVD
		EXIT_IF_ENDSIGNAL();
		int writeImageExitCode;
		acs_emf_common::EMF_DVD_WRITETYPE writeType = acs_emf_common::WRITETYPE_FORMAT_WRITE;
		if(( writeImageExitCode=ACS_EMF_Common::writeImageOnMedium(fileList, nooffiles, dev_info, mediumType,writeType,bVerify,imageName)!=0 ))
		{
			ERROR(1,"%s","Writing Image on Medium is failed");
			// modify result and end time
			modifyEndHistoryAttrs((ACE_TCHAR*)"Failed");
			copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			if (writeImageExitCode == 252)
				modifyStructObjFinalAttr(FAILURE,"FAILED DUE TO MEDIA IS NOT IN APPENDABLE STATE. USE formatAndExportToMedia ACTION, IF OLD FILES ARE NOT REQUIRED.");
			else
				modifyStructObjFinalAttr(FAILURE,"Exporting on media is failed.");
			if(resetMedia(MEDIA_ON_CURRENT_NODE) != EMFMEDIAHANDLER_OK)
			{
				DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, resetMedia failed");
				modifyStructObjFinalAttr(FAILURE,"FAILED WHILE RESET MEDIA");
                                copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			}
			ACS_EMF_Common::unmountDVDData();
			return EMF_RC_COPYTOTRACKFAILED;
		}
		// modify result and endtime
		if (!bVerify)
		{
			modifyEndHistoryAttrs((ACE_TCHAR*)SUCCESSFUL);
			modifyStructObjFinalAttr(SUCCESS,"-");
			copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
		}

		if (bVerify)
		{
			if(resetMedia(MEDIA_ON_CURRENT_NODE) != EMFMEDIAHANDLER_OK)
			{
				DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, resetMedia failed");
				modifyStructObjFinalAttr(FAILURE,"FAILED WHILE RESET MEDIA");
				copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
				return EMF_RC_NOK;
			}
#if 0
/*	Reason for ignoring verfication of checksum : growisofs command having
	in-built mechanism to verify checksum. Moreover, verifychecksum() holds
	huge memory which leads to reboot for some times.*/
			// checksum verification
			if(m_stopFlag ||(ACS_EMF_Common::verifychecksum() == -1))
			{
				if(m_stopFlag)
				{
					ERROR(1,"%s","EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
					DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, stopFlag true");
					modifyEndHistoryAttrs((ACE_TCHAR*) FAILED);
					copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
					modifyStructObjFinalAttr(FAILURE,"DVD operation is interrupted because of service is stopped");
					return EMF_RC_NOK;
				}
				modifyEndHistoryAttrs((ACE_TCHAR*)"Failed while doing Data verification");
				copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
				DEBUG(1,"%s","Data verification on Medium is FAILED!!!");
			}
			if(m_stopFlag)
			{
				ERROR(1,"%s","EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
				DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, stopFlag true");
				modifyEndHistoryAttrs((ACE_TCHAR*) FAILED);
				copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
				modifyStructObjFinalAttr(FAILURE,"DVD operation is interrupted because of service is stopped");
				return EMF_RC_NOK;
			}
			DEBUG(1,"%s","Data verification on Medium is SUCCESS");
#endif
			modifyEndHistoryAttrs((ACE_TCHAR*)SUCCESSFUL);
			modifyStructObjFinalAttr(SUCCESS,"");
			copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			ACS_EMF_Common::unmountDVDData();
		}
		// Removing Temp image i.e /tmp/image.iso
		if(ACS_EMF_Common::removeTemporaryImage(imageName) == -1)
		{
			DEBUG(1,"%s","Removing Temporary image is failed!!!");

		}
		return EMF_RC_OK;
	}
	DEBUG(1,"%s","Leaving ACS_EMF_DVDHandler::DVDOperation");
	return EMF_RC_OK;
}//End of DVDOperation
/*===================================================================
ROUTINE: resetMedia
=================================================================== */
ACE_INT32 ACS_EMF_DVDHandler::resetMedia(int node)
{
	DEBUG(1,"%s","Entering ACS_EMF_DVDHandler::resetMedia");
	sleep (10);

	//Eject DVD to reflect the written data on disk

	if(m_stopFlag)
	{
		ERROR(1,"%s","EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
		DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, stopFlag true");
		return EMFMEDIAHANDLER_STOPRECVD;
	}
	ACE_INT32 ejectStatus = ACS_EMF_Common::ejectDVDMedia();
	if(ejectStatus == -1)
	{
		DEBUG(1,"%s","Writing to DVD success but Ejecting DVD failed so written data will not be reflected!!!");
	}
	if(m_stopFlag)
	{
		ERROR(1,"%s","EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
		DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, stopFlag true");
		return EMFMEDIAHANDLER_STOPRECVD;
	}
	ACE_OS::sleep(2);
	// Mount DVD
	if(m_stopFlag)
	{
		ERROR(1,"%s","EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
		DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, stopFlag true");
		return EMFMEDIAHANDLER_STOPRECVD;
	}
	int maxNoofRetrys = 5;
	int mount_status = -1;
	while (mount_status != 0 && maxNoofRetrys > 0 && !m_stopFlag )
	{
		if (node == MEDIA_ON_CURRENT_NODE)
			mount_status = ACS_EMF_Common::mountDVDData();
		else
			mount_status = ACS_EMF_Common::mountDVDOnPassive();

		if(mount_status != 0){
			DEBUG(1,"%s","Mounting failed and trying again as DVD is not in ready state");
			ACE_OS::sleep(20);
			maxNoofRetrys--;
		}
	}
	if (maxNoofRetrys <= 0 && mount_status != 0)
	{
		DEBUG(1,"%s","ACS_EMF_DVDHandler::resetMedia, tried to mountDVDOnPassive for maximum number of times on passive node, but failed");
		return EMFMEDIAHANDLER_MOUNTFAILURE;
	}
	if(m_stopFlag)
	{
		ERROR(1,"%s","EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
		DEBUG(1,"%s","ACS_EMF_DVDHandler::DVDOperation, stopFlag true");
		//				modifyEndHistoryAttrs((ACE_TCHAR*) FAILED);
		//				copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
		//				modifyStructObjFinalAttr(FAILURE,"DVD operation is interrupted because of service is stopped");
		return EMFMEDIAHANDLER_STOPRECVD;
	}
	sleep (10);
	DEBUG(1,"%s","Leaving ACS_EMF_DVDHandler::resetMedia");
	return EMFMEDIAHANDLER_OK;
}
/*===================================================================
   ROUTINE: GetDVDOwner
=================================================================== */
ACE_UINT32 ACS_EMF_DVDHandler::GetDVDOwner(std::string &mediaOwner)
{
	DEBUG(1,"%s","Entering ACS_EMF_DVDHandler::GetDVDOwner");
	char myOwner[64] = "EMFINFO";
	//	mediaOwner = myOwner;
	//	return EMF_RC_OK;
	// FETCHING DVDOWNER
	ACE_TCHAR hostname[64];
	ACE_TCHAR partnerNode[64];

	sprintf(hostname,"%s",ACS_EMF_Common::GetHostName());
	DEBUG(1,"Host name Retrieved is %s",hostname);

	int status = ACS_EMF_Common::getDVDOwner(hostname,false);
	if (status == -1)
	{
		DEBUG(1,"%s","DVD is not present on Current Node. Checking on Partner node");
		if(ACS_EMF_Common::getNode() == NODE_A)
		{
			ACS_EMF_Common::GetNodeName(NODE_B, partnerNode);
			DEBUG(1,"Partner Node name is %s",partnerNode);
		}
		else if (ACS_EMF_Common::getNode() == NODE_B)
		{
			ACS_EMF_Common::GetNodeName(NODE_A, partnerNode);
			DEBUG(1,"Partner Node name is %s",partnerNode);
		}
		else
		{
			DEBUG(1,"%s","Partner node detected by EMF is UNKNOWN");
			sprintf(partnerNode,"%s","UNKNOWN");
			return EMF_RC_NOK;
		}

		status = ACS_EMF_Common::getDVDOwner(partnerNode,true);
		if (status == -1)
		{
			DEBUG(1,"%s","DVD is not present on Partner Node also\n");
			DEBUG(1,"%s","No external media board available\n");
			ACS_EMF_Common::setErrorText(0, 21, "No external media board available");
			return EMF_RC_NOK; // In case of failure
		}

		DEBUG(1,"%s","DVD is present on partnerNode");
		sprintf(myOwner,"%s",partnerNode);
	}
	else
	{
		sprintf(myOwner,"%s",hostname);
	}

	mediaOwner = myOwner;
	DEBUG(1,"MEDIA OWNER = %s",mediaOwner.c_str());
	// END
	return EMF_RC_OK;
}//End of GetDVDOwner

/*===================================================================
   ROUTINE: GetMediaInformation
=================================================================== */
ACE_UINT32 ACS_EMF_DVDHandler::GetMediaInformation(ACE_INT32& mediaType,ACE_UINT16& freeSpace,ACE_UINT16& usedSpace,ACE_UINT32& totalSpace)//(ACS_EMF_Data* pData)
{
	DEBUG(1,"%s","Entering ACS_EMF_DVDHandler::GetMediaInformation");
	// Initialization
	mediaType = -1;
	freeSpace = 0;
	usedSpace = 0;
	totalSpace = 0;

	if(m_stopFlag)
	{
		ERROR(1,"%s","EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
		DEBUG(1,"%s","ACS_EMF_DVDHandler::GetMediaInformation, stopFlag is true");
		return EMF_RC_NOK;
	}
	ACE_TCHAR mediumType[EMF_MEDIATYPE_MAXLEN] = {'\0'};
	if (ACS_EMF_Common::getMediumType(mediumType) == -1)
	{
		DEBUG(1,"%s","Unknown Medium");
		ACS_EMF_Common::setErrorText(0, 22, "No Media in DVD Drive");
		mediaType = -1;
		freeSpace = 0;
		usedSpace = 0;
		totalSpace = 0;
		return EMF_RC_DISCMEDIAERROR;
	}

	DEBUG(1,"Medium Type is %s",mediumType);
	// TR HR44818 - BEGIN
	if (ACE_OS::strcmp(mediumType,"DVD+R")==0)
		mediaType = 0;
	else if (ACE_OS::strcmp(mediumType,"DVD-R")==0)
		mediaType = 1;
	else if (ACE_OS::strcmp(mediumType,"DVD+RW")==0)
		mediaType = 2;
	else
		mediaType = 3;
	// TR HR44818 - END
	if(m_stopFlag)
	{
		ERROR(1,"%s","EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
		DEBUG(1,"%s","ACS_EMF_DVDHandler::GetMediaInformation, stopFlag is true");
		return EMF_RC_NOK;
	}
	int media_state = ACS_EMF_Common::isDVDEmpty();
	if (media_state == acs_emf_common::DVDMEDIA_STATE_ERROR)
	{
		DEBUG(1,"%s","Error occurred in identifying DVD");
		mediaType = -1;
		freeSpace = 0;
		usedSpace = 0;
		totalSpace = 0;
		return -1;
	}

	totalSpace = 4092000;

	if (media_state == acs_emf_common::DVDMEDIA_STATE_EMPTY)
	{
		freeSpace = 100;
		usedSpace = 0;
	}
	else
	{
		// Media has some data
		// Used Space
		//ACE_TCHAR useSpace[20];
		ACE_UINT64 uiUsedSpace=0;

		// For Other mediums i.e CD-R, CD+R, DVD-R and DVD+R
		if(ACS_EMF_Common::getMediaUsedSpace(&uiUsedSpace) == -1)
		{
			DEBUG(1,"%s","Fetching used space failed");
			mediaType = -1;
			freeSpace = 0;
			usedSpace = 0;
			totalSpace = 0;
			return -1;
		}
		ACE_UINT64 space = uiUsedSpace / 1024;

		DEBUG(1,"ACS_EMF_DVDHandler::GetMediaInformation() used space on DVD == %lu KB",space);

		double perc;
		// DVD
		if (space > DVDDiskSpace)
		perc = 0;
		else	perc=(double) ((DVDDiskSpace - space)*100)/DVDDiskSpace;
		DEBUG(1,"ACS_EMF_DVDHandler::GetMediaInformation() free space/perc == %f",perc);

		//perc *= 100;
		perc = floor(perc);
		freeSpace = perc;
		DEBUG(1,"ACS_EMF_DVDHandler::GetMediaInformation() free space/perc == %f",perc);
		perc = 100 - perc;
		DEBUG(1,"ACS_EMF_DVDHandler::GetMediaInformation() Used space perc == %f",perc);
		usedSpace = perc;
	}

	DEBUG(1,"%s","Leaving ACS_EMF_DVDHandler::GetMediaInformation");
	return EMF_RC_OK;
}//End of GetMediaInformation

/*===================================================================
ROUTINE: GetMediaInfoOnPassive
=================================================================== */
ACE_UINT32 ACS_EMF_DVDHandler::GetMediaInfoOnPassive(std::string& mediaType,std::string& freeSpace,std::string& usedSpace,std::string& totalSpace)//(ACS_EMF_Data* pData)
{
	DEBUG(1,"%s","Entering ACS_EMF_DVDHandler::GetMediaInfoOnPassive");
	// Initialization
	mediaType = -1;
	freeSpace = "0";
	usedSpace = "0";
	totalSpace = "0";



	//	if(ACS_EMF_Common::getDeviceName() == -1)
	//	{
	//		DEBUG(1,"%s","No Media in DVD Drive");
	//		ACS_EMF_Common::setErrorText(0, 22, "No Media in DVD Drive");
	//		mediaType = -1;
	//		freeSpace = "0";
	//		usedSpace = "0";
	//		totalSpace = "0";
	//		return EMF_RC_NOMEDIAINDRIVE;
	//	}
	if(m_stopFlag)
	{
		ERROR(1,"%s","EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
		DEBUG(1,"%s","ACS_EMF_DVDHandler::GetMediaInfoOnPassive, stopFlag is true");
		return EMF_RC_NOK;
	}
	ACE_TCHAR mediumType [EMF_MEDIATYPE_MAXLEN] = {'\0'};
	if (ACS_EMF_Common::getMediumType(mediumType) == -1)
	{
		DEBUG(1,"%s","Unknown Medium");
		ACS_EMF_Common::setErrorText(0, 22, "No Media in DVD Drive");
		mediaType = -1;
		return EMF_RC_DISCMEDIAERROR;
	}

	// TR HR44818 - BEGIN
	if (ACE_OS::strcmp(mediumType,"DVD+R")==0)
		mediaType = "0";
	else if (ACE_OS::strcmp(mediumType,"DVD-R")==0)
		mediaType = "1";
	else if (ACE_OS::strcmp(mediumType,"DVD+RW")==0)
		mediaType = "2";
	else
		mediaType = "3";

	// TR HR44818 - END

	DEBUG(1,"Medium Type is %s",mediaType.c_str());
	if(m_stopFlag)
	{
		ERROR(1,"%s","EMF Service has received Stop Signal, ongoing EMF operation is going to be stopped");
		DEBUG(1,"%s","ACS_EMF_DVDHandler::GetMediaInfoOnPassive, stopFlag is true");
		return EMF_RC_NOK;
	}
	int media_state = ACS_EMF_Common::isDVDEmpty();

	if (media_state == acs_emf_common::DVDMEDIA_STATE_ERROR)
	{
		DEBUG(1,"%s","Error occurred in identifying DVD");
		mediaType = -1;
		return -1;
	}

	totalSpace = "4092000";

	if (media_state == acs_emf_common::DVDMEDIA_STATE_EMPTY)
	{
		freeSpace = "100";
		usedSpace = "0";
	}
	else
	{
		// Media has some data
		// Used Space
		//ACE_TCHAR useSpace[20] = {0};
		ACE_UINT64 uiUsedSpace=0;

		// For Other mediums i.e CD-R, CD+R, DVD-R and DVD+R
		if(ACS_EMF_Common::getMediaUsedSpace(&uiUsedSpace) == -1)
		{
			DEBUG(1,"%s","Fetching used space failed");
			mediaType = -1;
			freeSpace = "0";
			usedSpace = "0";
			totalSpace = "0";
			return -1;
		}

		DEBUG(1,"  useSpace == %llu ", uiUsedSpace);

		ACE_INT64 space = uiUsedSpace/1024;
		DEBUG(1,"space  == %ld KB",space);

			// DVD
		double perc;
		perc =(double) ((DVDDiskSpace - space)*100)/DVDDiskSpace;
		DEBUG(1,"perc value %f",perc);

		//perc *= 100;
		perc = floor(perc);
		DEBUG(1,"Free space perc = %f",perc);
		int val = (int) perc;
		std::stringstream ss1,ss2;
		ss1 << val;
		freeSpace = ss1.str();
		perc = 100 - val;
		DEBUG(1,"Used space perc = %f",perc);
		val = (int) perc;
		ss2 << val;
		usedSpace = ss2.str();
		DEBUG(1,"Free space = %s Used space = %s",freeSpace.c_str(),usedSpace.c_str());
	}

	DEBUG(1,"%s","Leaving ACS_EMF_DVDHandler::GetMediaInfoOnPassive");
	return EMF_RC_OK;
}//End of GetMediaInfoOnPassive

void ACS_EMF_DVDHandler::setDvdState(int32_t iApgOwner, bool bDVDLocked)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_AceMutex);
	m_iApgOwner = iApgOwner;
	m_bDVDLocked = bDVDLocked;
	DEBUG(1,"** setDvdState m_iApgOwner[%i], m_bDVDLocked[%i]",iApgOwner, bDVDLocked);
}

void ACS_EMF_DVDHandler::getDvdState(int32_t *piApgOwner, bool *pbDVDLocked)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_AceMutex);
	*piApgOwner = m_iApgOwner;
	*pbDVDLocked = m_bDVDLocked;
	DEBUG(1,"** getDvdState return m_iApgOwner[%i], m_bDVDLocked[%i]",m_iApgOwner, m_bDVDLocked);
}

ACE_INT32 ACS_EMF_DVDHandler::enableMediaOnAccessibleNode(int localMediaState)
{
	DEBUG(1,"%s","ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - Entering");
	int32_t iDvdOwnerSysId = acs_dsd::SYSTEM_ID_UNKNOWN;	
	int retCode = EMF_RC_OK;

	ACS_DSD_Client dsdClient;
	ACS_DSD_Node localNode;
	dsdClient.get_local_node(localNode);

	if (localMediaState == MEDIA_FEATURE_ENABLE) {
		iDvdOwnerSysId = localNode.system_id;
	}
	else{
		ifstream mediaFileStrm(mediaOwnerFile);
		if(mediaFileStrm.fail ())
		{
			DEBUG(1,"%s","ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - Failed to open media owner file");
			DEBUG(1,"%s","ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - Leaving");
			return EMF_RC_NOK;
		}
		else{
			int32_t iNodeState = 0;
			std::string stringTemp;
			mediaFileStrm.clear ();
			mediaFileStrm.seekg (0, ios::beg);

			while (getline(mediaFileStrm, stringTemp))
			{
				istringstream inputStream (stringTemp);
				inputStream >> iDvdOwnerSysId >> iNodeState;
				stringTemp.clear ();
			}
			mediaFileStrm.close();
		}
	}

	if(iDvdOwnerSysId != acs_dsd::SYSTEM_ID_UNKNOWN)
	{
		ACS_EMF_CmdClient cmdClient(m_stopHandleforDVD);

		if(!cmdClient.connect(iDvdOwnerSysId , acs_dsd::NODE_STATE_ACTIVE)){
			ERROR(1, "%s", "ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - Connection to service on Media-owner node is failed!");
			return EMF_RC_UNABLETOCONNECTSERV;
		}

		ACS_EMF_Cmd cmdSend(acs_emf_cmd_ns::EMF_CMD_CheckAndMountMedia);
		std::list<string> lstArgs;
		if(acs_dsd::NODE_STATE_ACTIVE == localNode.node_state){
			lstArgs.push_back(DSD_ACTIVE_NODE);
		}
		else if(acs_dsd::NODE_STATE_PASSIVE == localNode.node_state){
			lstArgs.push_back(DSD_PASSIVE_NODE);
		}

                cmdSend.addArguments(lstArgs);
                std::list<std::string> lst = cmdSend.commandArguments();
                std::list<std::string>::const_iterator it;
                DEBUG(1,"ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - Send [%i] elements to Media-owner node.", lst.size());
                for(it = lst.begin(); it !=  lst.end(); ++it)
                        DEBUG(1,"ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - Send to Media-owner node element [%s]", (*it).c_str());

		if(!cmdClient.send(cmdSend)){
			ERROR(1, "%s", "ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - Sending message to Media-owner node is failed!");
			return EMF_RC_CANNOTSENDCMDTOSERVER;
		}

		ACS_EMF_Cmd cmdRecv;
		if(!cmdClient.receive(cmdRecv)){
			ERROR(1, "%s", "ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - Client not able to receive reply from DSD server!");
			return EMF_RC_CANNOTRECEIVEROMSERVER;
		}
		DEBUG(0, "%s", "ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - Successfully Received reply from Media-owner!");
		int cmdCode=(int)cmdRecv.commandCode();
		DEBUG(1,"ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - Code received from Media-owner = %d",cmdCode);

		switch(cmdCode){
			case ACS_EMF_DSDServer::EMFSERVER_ERROR:
				DEBUG(1,"%s", "ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - Error Received from EMF server: Mounting Media on DVD owner fails !");
				retCode = EMF_RC_ERRORRECEIVINGFROMSERVER;
				break;
			case ACS_EMF_DSDServer::EMFSERVER_OK:
				INFO(1,"%s", "ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - UnMounting Media on the Media-owner Node is SUCCESSFUL");
				break;
			case ACS_EMF_DSDServer::EMFSERVER_MEDIANOTMOUNTED:
				INFO(1,"%s", "ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - Media not enabled on the Media-owner Node");
				break;
			case ACS_EMF_DSDServer::EMFSERVER_MEDIAALREADYENABLED:
				INFO(1,"%s", "ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - Media Already enabled on the Media-owner Node");
				retCode = mountMediaOnDVDOwner(iDvdOwnerSysId);
				break;
			default:
				ERROR(1,"ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - Unexpected Code received from Server = %d",cmdCode);
				ERROR(1,"%s", "ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - UnMounting DVD media on the Media-owner Node is FAILED");
				retCode = EMF_RC_NOK;
				break;
		}
	}
	else{
		INFO(1,"%s", "ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - Media owner systemId not Found");
		retCode = EMF_RC_NOK;
	}
	DEBUG(1,"%s","ACS_EMF_DVDHandler::enableMediaOnAccessibleNode - Leaving");
	return retCode;
}

ACE_INT32 ACS_EMF_DVDHandler::mountMediaOnDVDOwner(int32_t SysId)
{
	int isMediaMounted = ACS_EMF_Common::isDvdCorrectlyMounted(false);

	if (isMediaMounted == EMF_MEDIA_MOUNTEDBAD){
		DEBUG(1,"%s","ACS_EMF_DVDHandler::mountMediaOnDVDOwner - Leaving");
		return EMF_MEDIA_MOUNTEDBAD;
	}
	if(isMediaMounted == EMF_MEDIA_UMOUNTREQ){
		// Require to umount from /data/opt/ap/internal_root/media/
		if( ACS_EMF_Common::unmountPassiveDVDOnActive() == EMF_RC_OK ){
			DEBUG(1,"%s","ACS_EMF_DVDHandler::mountMediaOnDVDOwner - unmounted the DVD which is mounted to FileM=media");
		}else{
			DEBUG(1,"%s","ACS_EMF_DVDHandler::mountMediaOnDVDOwner - Failed to unmount the DVD which is mounted to FileM=media");
			DEBUG(1,"%s","ACS_EMF_DVDHandler::mountMediaOnDVDOwner - Leaving");
			return EMF_RC_ERROR;
		}
	}	
	// IF Media is already mounted skip mount operation

	if (isMediaMounted != EMF_MEDIA_MOUNTED &&
			ACS_EMF_Common::isDVDEmpty() == acs_emf_common::DVDMEDIA_STATE_NOT_EMPTY)
	{
		int mount_status = -1;
		int maxNoofRetrys = 5;
		while (mount_status != 0 && maxNoofRetrys > 0 /*&& !m_stopFlag*/ )
		{
			mount_status = ACS_EMF_Common::mountDVDOnPassive();

			if(mount_status != 0){
				ERROR(1,"%s","ACS_EMF_DVDHandler::mountMediaOnDVDOwner - mountDVDOnPassive() failed! ");
				DEBUG(1,"%s","ACS_EMF_DVDHandler::mountMediaOnDVDOwner -retry mount after 20 sec");

				ACE_OS::sleep(20);
				maxNoofRetrys--;
			}
		}
		if (maxNoofRetrys <= 0 && mount_status != 0)
		{
			DEBUG(1,"%s","ACS_EMF_DVDHandler::mountMediaOnDVDOwner - tried to mountDVDOnPassive for maximum number of times on passive node, but failed");
			ERROR(1,"%s","ACS_EMF_DVDHandler::mountMediaOnDVDOwner - mountDVDOnPassive() failed! ");
			DEBUG(1,"%s","ACS_EMF_DVDHandler::mountMediaOnDVDOwner - Leaving");
			return EMF_RC_MOUNTINGFAILED;
		}
	}
	if(ACS_EMF_Common::setupNFSOnPassive() != 0) {
		ERROR(1,"%s","ACS_EMF_DVDHandler::mountMediaOnDVDOwner - Setup of NFS share for DVD data on passive node failed");
		DEBUG(1,"%s","ACS_EMF_DVDHandler::mountMediaOnDVDOwner - Leaving");
		return EMF_RC_ERROR;
	}

	// No error
	//ACS_EMF_DVDHandler *pClienthandler = reinterpret_cast<ACS_EMF_DVDHandler *>(m_pMediaHandler);
	//pClienthandler->setDvdState(SysId, true);
	setDvdState(SysId, true);
	return EMF_RC_OK;
}
