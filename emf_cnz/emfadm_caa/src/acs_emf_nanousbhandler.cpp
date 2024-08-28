/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2013
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*//**
  *
  * @file acs_emf_nanousbhandler.cpp
  *
  * @brief
  *
  *
  *
  *
  * @author Shyam Chirania
  *
 ------------------------------------------------------------------------*/

#include <acs_emf_nanousbhandler.h>
#include "acs_emf_param.h"
#include <acs_emf_commandhandler.h>

//-----------------Global Section--------------------------------------
extern ACE_HANDLE stopThreadFds[2];
ACE_HANDLE ACS_EMF_NANOUSBHandler::m_stopHandlefornanoUSB=ACE_INVALID_HANDLE;
//-----------------End-------------------------------------------------

//---------------------------------------------------------------------
ACS_EMF_NANOUSBHandler::ACS_EMF_NANOUSBHandler(ACE_HANDLE endEvent)
{
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ACS_EMF_NANOUSBHandler - Entering");
	if ( immHandle.Init() != ACS_CC_SUCCESS ) {
		ERROR(1,"%s","ERROR: OmHandler Initialization FAILURE!!!");
	}
	//m_stopFlag=false;
	m_stopHandlefornanoUSB = endEvent;
	cntValue = getHistoryInstancesCnt()+1;
	createStructObj();
	setLastHistOperStateToIDLEIfPROCESSING();
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ACS_EMF_NANOUSBHandler - Leaving");
}

//---------------------------------------------------------------------
ACS_EMF_NANOUSBHandler::~ACS_EMF_NANOUSBHandler()
{
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::~ACS_EMF_NANOUSBHandler - Entering");
	char retriveData[ACS_EMF_ARRAYSIZE];
	ACE_INT32 result;

	std::string dnName;
	dnName.append(ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
	dnName.append(",");
	dnName.append(ACS_EMF_Common::parentDNofEMF);

	if((result = ACS_EMF_Common::fetchEMFHistoryAttribute(dnName,(ACE_TCHAR*) theEMFHistoryInfoNodeState,retriveData)) == ACS_CC_SUCCESS)        {
		if (ACE_OS::strcmp(retriveData,(ACE_TCHAR*)PROCESSING) == 0) {
			modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() ,(ACE_TCHAR*) theEMFHistoryInfoNodeState,(ACE_TCHAR*)IDLE);
			modifyEMFHistoryAttribute((ACE_TCHAR*) dnName.c_str() ,(ACE_TCHAR*) theEMFHistoryInfoOperResult,(ACE_TCHAR*)FAILED);
		}
	}

	dnName.clear();
	if(immHandle.Finalize() != ACS_CC_SUCCESS) {
		DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::~ACS_EMF_NANOUSBHandler - Method OmHandler::Finalize Failure!!!");
	}
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::~ACS_EMF_NANOUSBHandler - Leaving");
}

//---------------------------------------------------------------------
ACE_UINT32 ACS_EMF_NANOUSBHandler::ExportFiles(	int32_t sysId, 	
												acs_dsd::NodeSideConstants nodeSide,
												ACS_EMF_DATA* pData)
{
	(void)sysId;
	(void)nodeSide;
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ExportFiles - Entering");
	ACS_EMF_ParamList Params;
	Params.Decode(pData->Data);
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ExportFiles - Copy-To-USB action received.");
	initEMFHistRuntimeHandler();
	const ACE_TCHAR* lpszArgs[3] ={
			Params.Data[0],
			Params.Data[1],
			Params.Data[2]
	};
	for (int i = 0; i < 3; i++)
		DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - %s%d = %s\n","Argument",i,lpszArgs[i]);
	ACE_TCHAR imageName[ACS_EMF_MAX_LENGTH];
	if (ACE_OS::strcmp(lpszArgs[0], ACE_TEXT("")) != 0)
		ACE_OS::strcpy(imageName, lpszArgs[0]);
	else{
		ACE_OS::strcpy(imageName,ACS_EMF_Common::GenerateVolumeName()); // incase of -l option absence.
		DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - Generated Image Label Name is %s",imageName);
	}

	bool bVerify = (ACE_OS::strcmp(lpszArgs[1], ACE_TEXT("true")) == 0);
	bool bOverwrite = (ACE_OS::strcmp(lpszArgs[2], ACE_TEXT("true")) == 0);
	DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - Image Label Name is %s",imageName);
	DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - Verify flag status = %d  Overwrite flag status = %d",bVerify,bOverwrite);
	int nNum = (Params.NumOfData() - 3); // No of files
	DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - Total No. of File arguments received from action is %d",nNum);

	const ACE_TCHAR* lpszFile;
	const ACE_TCHAR* fileList[EMF_COPY_MAX_NUMFILES] ={'\0'}; // contains file list
	ACE_INT32 nooffiles = 0;     // No of files

	for (int nIdx = 0; nIdx < nNum; nIdx++){
		lpszFile = (const ACE_TCHAR*)Params.Data[3 + nIdx];
		DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - Files are : %s\n",lpszFile);
		fileList[nooffiles] = lpszFile;
		nooffiles++;
	}
	DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - Total No. of Files is %d",nooffiles);
	std::string filestrlist;
	for (int i = 0 ;i < nooffiles; i++){
		std::string str(fileList[i]);
		size_t found;
		found=str.find_last_of("/\\");
		filestrlist.append(str.substr(found+1));
		if (nooffiles > 1 ) filestrlist.append(" ");
	}
	DEBUG(1, "ACS_EMF_NANOUSBHandler::ExportFiles - filestrlist =  %s",filestrlist.c_str());
	ACE_TCHAR **actualFileList = new ACE_TCHAR*[nooffiles];
	for(int i=0;i<nooffiles;i++)
		actualFileList[i] = new ACE_TCHAR[EMF_MAX_SOURCE];

	if(ACS_EMF_Common::getNodeState()== 2){
		ACS_EMF_Common::createDirectory(ACS_EMF_ACTIVE_DATA);
		for (int i = 0 ;i < nooffiles; i++){
			std::string fileListPath(fileList[i]);
			DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - before calling fileListToCopyToMedia fileListPath is %s",fileListPath.c_str());
			if(fileListToCopyToMedia(fileListPath)!=EMF_RC_OK){
				for( int i = 0 ; i < nooffiles; i++ )
					delete [] actualFileList[i] ;
				delete [] actualFileList;
				DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ExportFiles - Leaving");
				return EMF_RC_NOK;
			}
			DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - After modifying filelist path in fileListToCopyToMedia = %s",fileListPath.c_str());
			ACE_UINT32 columns = 0;
			for(;columns < fileListPath.length();columns++)
				actualFileList[i][columns] =  fileListPath[columns];
			actualFileList[i][columns] = '\0';
			DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - After modifying filelist path  = %s",actualFileList[i]);
			fileList[i] = actualFileList[i];
		}
	}
	//else if(ACS_EMF_Common::getNodeState()== 1){
		//if(ACS_EMF_DSDServer::theMediaOwnerinPassive == MEDIA_ON_CURRENT_NODE){
//			if (cntValue == 1){
//				string rdnOfHistoryObj(ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN);
//				createRuntimeObjectsforEmfHistoryInfoClass(rdnOfHistoryObj);
//				rdnOfHistoryObj.clear();
//				rdnOfHistoryObj.append(ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
//				createRuntimeObjectsforEmfHistoryInfoClass(rdnOfHistoryObj);
//				modifyInitialHistoryAttrs(Params.CmdCode,filestrlist);
//			}else{
//				modifyInitialHistoryAttrs(Params.CmdCode,filestrlist);
//				moveObjectsOneLevelDown(cntValue);
//			}
//			cntValue++;
//			if (cntValue > 20) cntValue = 20;
//			copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
		//}
	//}
	ACE_TCHAR usedSpace[EMF_SIZE_DATA];
	ACE_TCHAR availableSpace[EMF_SIZE_DATA];
	 ACE_TCHAR totalSpace[EMF_SIZE_DATA];
	 for (int i = 0 ;i < nooffiles; i++)
		 DEBUG(0,"ACS_EMF_NANOUSBHandler::ExportFiles - Before consume space actualFileList[i] = %s",actualFileList[i]);
	 if(ACS_EMF_Common::getNodeState()== 2){
		 if(ACS_EMF_Common::getGivenFilesConsumedSpace((const ACE_TCHAR**)actualFileList,nooffiles,usedSpace) == -1)
			 DEBUG(1,"%s","Fetching consumed space for files failed in passive node!!!");
	 }else{
		 if(ACS_EMF_Common::getGivenFilesConsumedSpace(fileList,nooffiles,usedSpace) == -1)
			 DEBUG(1,"%s","Fetching consumed space for files failed in active node!!!");
	 }
	 ACE_UINT64 space = atol(usedSpace);
	 DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - Total space used by file = %ld",space);
	 if(ACS_EMF_DSDServer::theMediaOwnerinPassive == MEDIA_ON_PARTNER_NODE) {
		 DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - Count of History Object Instances = %d\n",cntValue);
		 copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
	 }
	 if(ACS_EMF_Common::getMediaSpaceInfo(ACS_EMF_Common::getNodeState(),availableSpace,usedSpace,totalSpace) == -1){
		 ERROR(1,"ACS_EMF_NANOUSBHandler::ExportFiles - %s","Fetching used space from media is failed");
		 for( int i = 0 ; i < nooffiles; i++ )
			 delete [] actualFileList[i] ;
		 delete [] actualFileList;
		 DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ExportFiles - Leaving");
		 return EMF_RC_COPYTOTRACKFAILED;
	 }
	 ACE_UINT64 totalSize = atol(totalSpace);
	 DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - totalSize of thumb drive = %ld",totalSize);
	 if (space > totalSize){
		 DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - %s","File(s) Data Too large to store on Media");\
			 if(ACS_EMF_DSDServer::theMediaOwnerinPassive == MEDIA_ON_PARTNER_NODE){
				 modifyEndHistoryAttrs((ACE_TCHAR*)"Failed due to enough space is not available on medium to copy");
				 copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
				 modifyStructObjFinalAttr(FAILURE,"File too large");
			 }
		 for( int i = 0 ; i < nooffiles; i++ )
			 delete [] actualFileList[i] ;
		 delete [] actualFileList;
		 DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ExportFiles - Leaving");
		 return EMF_RC_FILETOLARGE;
	 }
	 //	writeType flag is used to distinguish ERASEANDCOPYTOMEDIA and COPYTOMEDIA
	 int writeType=1;
	 bool formatMedia = (Params.CmdCode == EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA)? true: false;
	 if(!formatMedia){
		 //		if(ACS_EMF_Common::getMediaSpaceInfo(ACS_EMF_Common::getNodeState(),availableSpace,usedSpace,totalSpace) == -1){
		 //			ERROR(1,"ACS_EMF_NANOUSBHandler::ExportFiles - %s","Fetching used space from media is failed");
		 //			for( int i = 0 ; i < nooffiles; i++ )
		 //				delete [] actualFileList[i] ;
		 //			delete [] actualFileList;
		 //			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ExportFiles - Leaving");
		 //			return EMF_RC_COPYTOTRACKFAILED;
		 //		}
		 ACE_UINT64 spaceonUSB = atol(usedSpace);
		 DEBUG(1,"space %ld",spaceonUSB);
		 ACE_UINT64 actualSize = atol(availableSpace);
		 if (space > actualSize){
			 DEBUG(1,"%s","File(s) Data Too large to store on media");
			if(ACS_EMF_DSDServer::theMediaOwnerinPassive == MEDIA_ON_PARTNER_NODE){
				modifyEndHistoryAttrs((ACE_TCHAR*)"Failed due to enough space is not available on medium to copy");
				copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
				modifyStructObjFinalAttr(FAILURE,"File too large");
			}
			for( int i = 0 ; i < nooffiles; i++ )
				delete [] actualFileList[i] ;
			delete [] actualFileList;
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ExportFiles - Leaving");
			return EMF_RC_FILETOLARGE;
		}
		writeType = 0;
	}
	// Check stopFlag
	//EXIT_IF_ENDSIGNAL();
	CHECK_ENDSIGNAL();
	for( int i = 0 ; i < nooffiles; i++ )
		delete [] actualFileList[i] ;
	delete [] actualFileList;
	IF_ENDSIGNAL_EXIT();

	if((ACS_EMF_Common::writeDataOnUSBMedium(fileList, nooffiles,(ACE_INT32)writeType,bVerify) == -1)){
		CHECK_ENDSIGNAL();
		for( int i = 0 ; i < nooffiles; i++ )
			delete [] actualFileList[i] ;
		delete [] actualFileList;
		IF_ENDSIGNAL_EXIT();
		if(formatMedia){
			//	ACS_EMF_Common::setMediaFlag(formatPosition,Sync);
			if (ACS_EMF_Common::mountMedia() != 0){
				ERROR(1,"ACS_EMF_NANOUSBHandler::ExportFiles - %s","ACS_EMF_Common::formatAndMountActiveMedia:Mounting the media   Failed!!! ");
			}
		}
		ERROR(1,"ACS_EMF_NANOUSBHandler::ExportFiles - %s","Writing Image on Medium is failed");
		if(acs_emf_commandhandler::getMediaOwner() == MEDIA_ON_PARTNER_NODE){
			modifyEndHistoryAttrs((ACE_TCHAR*)FAILED);
			copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			modifyStructObjFinalAttr(FAILURE,"Media is not writable");
		}
		for( int i = 0 ; i < nooffiles; i++ )
			delete [] actualFileList[i] ;
		delete [] actualFileList;
		DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ExportFiles - Leaving");
		return EMF_RC_WRITETRACKTOMEDIAFAILED;
	}
	else{
		if(formatMedia)
			ACS_EMF_Common::setMediaFlag(formatPosition,Sync);
		DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - %s","ACS_EMF_NANOUSBHandler::ExportFiles - Copy to Media is success ");
	}
	if(ACS_EMF_Common::getNodeState()== 1)
		ACS_EMF_Common::appendVolumeName(imageName);
	CHECK_ENDSIGNAL();
	for( int i = 0 ; i < nooffiles; i++ )
		delete [] actualFileList[i] ;
	delete [] actualFileList;
	IF_ENDSIGNAL_EXIT();

	if(formatMedia){
		ACS_EMF_Common::setMediaFlag(formatPosition,Sync);
	}
	if(ACS_EMF_DSDServer::theMediaOwnerinPassive  == MEDIA_ON_PARTNER_NODE){
		modifyEndHistoryAttrs((ACE_TCHAR*)SUCCESSFUL);
		copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
		modifyStructObjFinalAttr(SUCCESS,"-");
	}

	if(ACS_EMF_Common::removeTemporaryImage(imageName) == -1)
		DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - %s","Removing Temporary image is failed!!!");
	for(int i = 0 ; i < nooffiles; i++)
		delete [] actualFileList[i];
	delete [] actualFileList;
	if(ACS_EMF_Common::getNodeState()== 2){
		// Check stopFlag
		EXIT_IF_ENDSIGNAL();
		if(ACS_EMF_Common::unmountFileM() == EMF_RC_ERROR){
			DEBUG(1,"ACS_EMF_NANOUSBHandler::ExportFiles - %s","UnMounting fileM folder failed on passive node");
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ExportFiles - Leaving");
			return EMF_RC_ERROR;
			//return EMF_RC_DVDDEVICENOTFOUND;
		}
	}
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ExportFiles - Leaving");
	return EMF_RC_OK;
}

//---------------------------------------------------------------------
ACE_UINT32 ACS_EMF_NANOUSBHandler::ImportFiles(ACS_EMF_DATA* pData)
{
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ImportFiles - Entering");
	ACS_EMF_ParamList Params;
	Params.Decode(pData->Data);
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ImportFiles - Copy-from-Media action received.");
	initEMFHistRuntimeHandler();
	const ACE_TCHAR* lpszArgs = Params.Data[0];
	bool bOverwrite = (ACE_OS::strcmp(lpszArgs, ACE_TEXT("yes")) == 0);

	int nNum = Params.NumOfData();
	DEBUG(1,"ACS_EMF_NANOUSBHandler::ImportFiles - No of arguments received from action is %d",nNum);

	const ACE_TCHAR* lpszFile;
	const ACE_TCHAR* fileList[EMF_COPY_MAX_NUMFILES];
	ACE_INT32 nooffiles = 0; 
	for (int nIdx = 1; nIdx < nNum; nIdx++) {
		lpszFile = (const ACE_TCHAR*)Params.Data[nIdx];
		// Check all file/s are exist or not
		DEBUG(1,"ACS_EMF_NANOUSBHandler::ImportFiles - %s\n",lpszFile);
		fileList[nooffiles] = lpszFile;
		nooffiles++;
	}

	DEBUG(1,"ACS_EMF_NANOUSBHandler::ImportFiles - Total No of file Arguments is %d\n",nooffiles);
	std::string filestrlist;
	for (int i = 0 ;i < nooffiles; i++) {
		if(nooffiles > 1) {
			if (i < nooffiles-1) {
				std::string str(fileList[i]);
				size_t found;
				found=str.find_last_of("/\\");
				filestrlist.append(str.substr(found+1));
				filestrlist.append(" ");
			}
		}else{
			filestrlist.append("NONE");
		}
	}
	for (int i = 0 ;i < nooffiles; i++){
		DEBUG(1,"ACS_EMF_NANOUSBHandler::ImportFiles - File list for import operation = %s",fileList[i]);}

	/* For EMF History Log Info--> push the runtime object attributes info in LIFO Order*/
	DEBUG(1,"ACS_EMF_NANOUSBHandler::ImportFiles - Count of History Object Instances is %d",cntValue);
	sleep(1);
	modifyStructObjInitialAttr(IMPORT_FROM_MEDIA);
	if (cntValue == 1) {
		string rdnOfHistoryObj(ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN);
		createRuntimeObjectsforEmfHistoryInfoClass(rdnOfHistoryObj);
		rdnOfHistoryObj.clear();
		rdnOfHistoryObj.append(ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
		createRuntimeObjectsforEmfHistoryInfoClass(rdnOfHistoryObj);
		modifyInitialHistoryAttrs(Params.CmdCode,filestrlist);
	}else{
		modifyInitialHistoryAttrs(Params.CmdCode,filestrlist);
		/*cntValue > 1 */
		if(!moveObjectsOneLevelDown(cntValue)) {
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ImportFiles - moveObjectsOneLevelDown failed !!!");
			// modify result and endtime
			modifyEndHistoryAttrs((ACE_TCHAR*) FAILED);
			copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			modifyStructObjFinalAttr(FAILURE,"Media operation is interrupted because of service is stopped");
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ImportFiles - Leaving");
			return EMF_RC_SERVERNOTRESPONDING;
		}
	}
	cntValue++;
	/*cntValue > 20 */
	if (cntValue > 20) cntValue = 20;
	copyObjToObj((ACE_TCHAR*)ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);

	// Check stopFlag
	EXIT_IF_ENDSIGNAL();
	ACE_TCHAR errorText[EMF_CMD_MAXLEN];
	if ((ACS_EMF_Common::copyDataToDestFolder(fileList,nooffiles,bOverwrite,errorText) != 0)) {
		ERROR(1,"%s","ACS_EMF_NANOUSBHandler::ImportFiles - Copying USB Data to the given destination folder is failed!!!");
		// modify result and endtime
		modifyEndHistoryAttrs(errorText);
		copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
		modifyStructObjFinalAttr(FAILURE,errorText);
		ACS_EMF_Common::unmountDVDData();
		DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ImportFiles - Leaving");
		return EMF_RC_COPYFROMDVDFAILED;
	}

	// modify result and endtime
	modifyEndHistoryAttrs((ACE_TCHAR*)SUCCESSFUL);
	copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
	modifyStructObjFinalAttr(SUCCESS,"-");
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::ImportFiles - Leaving");
	return EMF_RC_OK;
}

//---------------------------------------------------------------------
ACE_UINT32 ACS_EMF_NANOUSBHandler::mediaOperation(ACS_EMF_DATA* pData)
{
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::mediaOperation - Entering");
	ACS_EMF_ParamList Params;
	Params.Decode(pData->Data);
	DEBUG(1,"ACS_EMF_NANOUSBHandler::mediaOperation - Type of mediaOperation Received is %d",Params.CmdCode);
	ACE_UINT32 myRetCode;
	switch(Params.CmdCode){
		case EMF_CMD_TYPE_COPYFROMMEDIA:
			myRetCode = ImportFiles((ACS_EMF_DATA*) pData);
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::mediaOperation - Leaving");
			return myRetCode;
		case EMF_CMD_TYPE_COPYTOMEDIA:
		case EMF_CMD_TYPE_ERASEANDCOPYTOMEDIA:
			{
				//	copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
				int nNum = (Params.NumOfData() - 3); // No of files
				DEBUG(1,"ACS_EMF_NANOUSBHandler::mediaOperation - Total No. of File arguments received from action is %d",nNum);

		const ACE_TCHAR* lpszFile;
		const ACE_TCHAR* fileList[EMF_COPY_MAX_NUMFILES]; // contains file list
		ACE_INT32 nooffiles = 0;     // No of files

		for (int nIdx = 0; nIdx < nNum; nIdx++){
			lpszFile = (const ACE_TCHAR*)Params.Data[3 + nIdx];
			DEBUG(1,"ACS_EMF_NANOUSBHandler::mediaOperation - Files are : %s\n",lpszFile);
			fileList[nooffiles] = lpszFile;
			nooffiles++;
		}
		DEBUG(1,"ACS_EMF_NANOUSBHandler::mediaOperation - Total No. of Files is %d",nooffiles);
		std::string filestrlist;
		for (int i = 0 ;i < nooffiles; i++){
			std::string str(fileList[i]);
			size_t found;
			found=str.find_last_of("/\\");
			filestrlist.append(str.substr(found+1));
			if (nooffiles > 1 ) filestrlist.append(" ");
		}
		if (cntValue == 1){
			string rdnOfHistoryObj(ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN);
			createRuntimeObjectsforEmfHistoryInfoClass(rdnOfHistoryObj);
			rdnOfHistoryObj.clear();
			rdnOfHistoryObj.append(ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			createRuntimeObjectsforEmfHistoryInfoClass(rdnOfHistoryObj);
			modifyInitialHistoryAttrs(Params.CmdCode,filestrlist);
		}else{
			modifyInitialHistoryAttrs(Params.CmdCode,filestrlist);
			moveObjectsOneLevelDown(cntValue);
		}
		cntValue++;
		if (cntValue > 20) cntValue = 20;
		copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);

		ACS_EMF_DATA srctData;
		memset(&srctData, 0, sizeof(ACS_EMF_DATA));
		srctData.Code = Params.CmdCode;
		Params.Encode();
		Params.getEncodedBuffer(srctData.Data);
		acs_dsd::NodeSideConstants enmNodeSide;
		enmNodeSide = acs_dsd::NODE_SIDE_UNDEFINED;
		int32_t iSystemId;
		iSystemId = 0;
		myRetCode = ExportFiles(iSystemId,enmNodeSide,&srctData);
		if(myRetCode == EMF_RC_FILETOLARGE){
			ERROR(1,"%s","ACS_EMF_NANOUSBHandler::mediaOperation - File(s) Data Too large to store on USB");
			modifyEndHistoryAttrs((ACE_TCHAR*)"Failed due to enough space is not available on medium to copy");
			copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			modifyStructObjFinalAttr(FAILURE,"File too large");
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::mediaOperation - Leaving");
			return EMF_RC_FILETOLARGE;
		}
		else if(myRetCode == EMF_RC_COPYTOTRACKFAILED){
			ERROR(1,"%s","ACS_EMF_NANOUSBHandler::mediaOperation - Fetching used space from media is failed");
			// modify result and end time
			modifyEndHistoryAttrs((ACE_TCHAR*)FAILED);
			copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			modifyStructObjFinalAttr(FAILURE,"No Space on media");
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::mediaOperation - Leaving");
			return EMF_RC_COPYTOTRACKFAILED;
		}else if(myRetCode == EMF_RC_WRITETRACKTOMEDIAFAILED){
			ERROR(1,"%s","ACS_EMF_NANOUSBHandler::mediaOperation - Write track on media failed.");
			modifyEndHistoryAttrs((ACE_TCHAR*)FAILED);
			copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			modifyStructObjFinalAttr(FAILURE,"Media is not writable");
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::mediaOperation - Leaving");
			return EMF_RC_WRITETRACKTOMEDIAFAILED;
		}
		ACS_EMF_Common::setMediaFlag(Sync_Pos,NO_Sync);
		EXIT_IF_ENDSIGNAL()	;
		return myRetCode;
	}
	break;
	default:
		break;
	}
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::mediaOperation - Leaving");
	return EMF_RC_OK;
}

//---------------------------------------------------------------------
ACE_UINT32 ACS_EMF_NANOUSBHandler::GetMediaInformation(ACE_INT32& mediaType,
														ACE_UINT16& freeSpace,
														ACE_UINT16& usedSpace,
														ACE_UINT32& totalSpace)
{
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::GetMediaInformation - Entering");
	ACE_INT32 rCode = EMF_RC_OK;
	if(acs_emf_commandhandler::getMediaOwner() != MEDIA_ON_PARTNER_NODE){
		rCode = ACS_EMF_Common::checkForUSBStatus();
	}

	mediaType = 4;
	if (rCode != EMF_RC_OK){
		DEBUG(1,"%s","Error occurred in identifying USB");
		mediaType = -1;
		freeSpace = 0;
		usedSpace = 0;
		totalSpace = 0;
		DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::GetMediaInformation - Leaving");
		return EMF_RC_DISCMEDIAERROR;
	}
	//	totalSpace = ACS_EMF_USBSIZE_KBYTES;
	ACE_TCHAR useSpace[EMF_SIZE_DATA];
	ACE_TCHAR availableSpace[EMF_SIZE_DATA];
	ACE_TCHAR totalSize[EMF_SIZE_DATA];
	if(ACS_EMF_Common::getMediaSpaceInfo(ACS_EMF_Common::getNodeState(),availableSpace,useSpace,totalSize) == EMF_RC_ERROR){
		DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::GetMediaInformation - Fetching used space failed");
		mediaType = -1;
		freeSpace = 0;
		usedSpace = 0;
		totalSpace = 0;
		DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::GetMediaInformation - Leaving");
		return EMF_RC_ERROR;
	}
	ACE_UINT64 space = atol(useSpace);
	totalSpace = atol(totalSize);
	DEBUG(1,"ACS_EMF_NANOUSBHandler::GetMediaInformation - totalSpace %ld",totalSpace);
	if (space <= ACS_EMF_EMPTY_USBSIZE_KBYTES){
		freeSpace = 100;
		usedSpace = 0;
		 DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::GetMediaInformation - thumb drive is empty");
	}else{
		double perc;
		DEBUG(1,"ACS_EMF_NANOUSBHandler::GetMediaInformation - space %ld",space);
		perc =(double) ((totalSpace - space)*100)/totalSpace;
		DEBUG(1,"ACS_EMF_NANOUSBHandler::GetMediaInformation - perc value %f",perc);
		perc = floor(perc);
		DEBUG(1,"ACS_EMF_NANOUSBHandler::GetMediaInformation - Free space perc = %f",perc);
		freeSpace = perc;
		perc = 100 - perc;
		DEBUG(1,"ACS_EMF_NANOUSBHandler::GetMediaInformation - Used space perc = %f",perc);
		usedSpace = perc;
	}
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::GetMediaInformation - Leaving");
	return EMF_RC_OK;
}

//---------------------------------------------------------------------
ACE_INT32 ACS_EMF_NANOUSBHandler::syncMediaOnPassive()
{
	DEBUG(1,"%s", "ACS_EMF_NANOUSBHandler::syncMediaOnPassive - Entering");
	EXIT_IF_ENDSIGNAL()	;
	ACS_EMF_Common::createDirectory(ACS_EMF_ACTIVE_DATA); 
	if(copyDataFromOtherMedia() != EMF_RC_OK){
		DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::syncMediaOnPassive - copy operation failed in the  ACS_EMF_NANOUSBHandler::syncMediaOnPassive");
		return EMF_RC_NOK;
	}
	DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::syncMediaOnPassive - Copy during startUp is Success!!");
	DEBUG(1,"%s", "ACS_EMF_NANOUSBHandler::syncMediaOnPassive - Leaving");
	return EMF_RC_OK;
}

//---------------------------------------------------------------------
ACE_INT32 ACS_EMF_NANOUSBHandler::syncMediaOnActive()
{
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::syncMediaOnActive - Entering");
	int syncFlag = ACS_EMF_Common::getMediaFlag(Sync_Pos);
	if(syncFlag == 1){
		DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::syncMediaOnActive - both the thumbdrives are in sync");
		DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::syncMediaOnActive - Leaving");
		return EMF_RC_OK;
	}
	if(ACS_EMF_Common::getMediaFlag(MasterTD_Pos) != 1){
		if (ACS_EMF_Common::setupNFSOnActive() != EMF_RC_OK){
			ERROR(1,"%s","ACS_EMF_NANOUSBHandler::syncMediaOnActive - NFS setup failed on active node - cannot start the emf service");
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::syncMediaOnActive - Leaving");
			return EMF_RC_ERROR;
		}
		if(syncPassiveMedia(acs_emf_cmd_ns::EMF_CMD_SyncMedia) != EMF_RC_OK){
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::syncMediaOnActive - failed to sync");
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::syncMediaOnActive - Leaving");
			return EMF_RC_ERROR;
		}
	}else{
		ACS_EMF_Common::createDirectory(ACS_EMF_ACTIVE_DATA); 
		if(sendCmdPassive(acs_emf_cmd_ns::EMF_CMD_PassiveShare) != EMF_RC_OK){
			DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::syncMediaOnActive - unable shrae passive /media");
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::syncMediaOnActive - Leaving");
			return EMF_RC_ERROR;
		}	
		if(copyDataFromOtherMedia() != EMF_RC_OK){
			DEBUG(1,"%s", "ACS_EMF_NANOUSBHandler::syncMediaOnActive - failed to sync the media");
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::syncMediaOnActive - Leaving");
			return EMF_RC_ERROR;
		}
		DEBUG(1,"%s", "ACS_EMF_NANOUSBHandler::syncMediaOnActive - sync operation is succsess");
		if(ACS_EMF_Common::setMediaFlag(MasterTD_Pos,ACS_EMF_Common::getNode()) != EMF_RC_OK)
			DEBUG(1, "%s", " Failed to set the flag");

		if(sendCmdPassive(acs_emf_cmd_ns::EMF_CMD_RemovePassiveShare) != EMF_RC_OK){
			DEBUG(1,"%s", "ACS_EMF_NANOUSBHandler::syncMediaOnActive - failed  to remove the passive share");
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::syncMediaOnActive - Leaving");
			return EMF_RC_ERROR;
		}
	}
	EXIT_IF_ENDSIGNAL()	;
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::syncMediaOnActive - Leaving");
	return EMF_RC_OK;
}

//---------------------------------------------------------------------
ACE_INT32 ACS_EMF_NANOUSBHandler::syncPassiveMedia(acs_emf_cmd_ns::emf_cmdCode cmdCode)
{
	DEBUG(1,"%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Entering");
	ACS_EMF_CmdClient cmdClient(m_stopHandlefornanoUSB);
	DEBUG(1,"ACS_EMF_NANOUSBHandler::syncPassiveMedia - theOperationalStateinPassive = %d",ACS_EMF_DSDServer::getOperationalStateinPassive());
	if(ACS_EMF_DSDServer::getOperationalStateinPassive() != MEDIA_FEATURE_ENABLE){
		DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::syncPassiveMedia - Unable to call Perform SyncOperation.USB is not present");
	}
	if(!cmdClient.connect(acs_dsd::SYSTEM_ID_UNKNOWN, acs_dsd::NODE_STATE_UNDEFINED)){
		ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Connection with server is failed");
		EXIT_IF_ENDSIGNAL()	;
		if(ACS_EMF_Common::removeNFSOnActive() != EMF_RC_OK)
			ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Removing NFS mount on Active node failed");
		DEBUG(1,"%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Leaving");
		return EMF_RC_ERROR;
	}	

	DEBUG(0, "%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Connection to service on passive node from active node is successful");
	DEBUG(1, "cmdCode Received[%d]",cmdCode);
	ACS_EMF_Cmd	cmdSend(cmdCode);
	if(!cmdClient.send(cmdSend)){
		ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Sending message to server is failed");
		EXIT_IF_ENDSIGNAL()	;
		if(ACS_EMF_Common::removeNFSOnActive() != EMF_RC_OK)
			ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Removing NFS mount on Active node failed");
		DEBUG(1,"%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Leaving");
		return EMF_RC_ERROR;
	}	

	DEBUG(0, "%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Received reply from DSD server to client successfully");
	ACS_EMF_Cmd cmdRecv;
	if(!cmdClient.receive(cmdRecv)){
		ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Client not able to receive reply from server");
		EXIT_IF_ENDSIGNAL()	;
		if(ACS_EMF_Common::removeNFSOnActive() != EMF_RC_OK)
			ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Removing NFS mount on Active node failed");
		DEBUG(1,"%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Leaving");
		return EMF_RC_ERROR;
	}	

	DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Client received reply from server successfully");
	std::list<std::string> argList = cmdRecv.commandArguments();
	int exitCode=(int)cmdRecv.commandCode();
	INFO(1,"ACS_EMF_NANOUSBHandler::syncPassiveMedia - Code received from Server = %d",exitCode);
	if (exitCode != 0){
		ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - failed to sync/format both the thumb drives");
		EXIT_IF_ENDSIGNAL()	;
		if(ACS_EMF_Common::removeNFSOnActive() != EMF_RC_OK)
			ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Removing NFS mount on Active node failed");
		DEBUG(1,"%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Leaving");
		return EMF_RC_ERROR;
	}	
	DEBUG(1,"%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - ACS_EMF_NANOUSBHandler::syncPassiveMedia sync operation is Success!!");
	if(ACS_EMF_Common::setMediaFlag(MasterTD_Pos,ACS_EMF_Common::getNode()) != EMF_RC_OK ) {
		DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia- MaterUsb set to Active node ");		
	}

	INFO(1,"%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - checkSum calculation is SUCCESSFUL");
	// Check stopFlag
	EXIT_IF_ENDSIGNAL();
	if(ACS_EMF_Common::removeNFSOnActive() != EMF_RC_OK)
		ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Removing NFS mount on Active node failed");
	DEBUG(1,"%s", "ACS_EMF_NANOUSBHandler::syncPassiveMedia - Leaving");
	return EMF_RC_OK;	
}

//---------------------------------------------------------------------
ACE_INT32 ACS_EMF_NANOUSBHandler::sendCmdPassive(acs_emf_cmd_ns::emf_cmdCode cmdCode)
{
	DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::sendCmdPassive - Entering");
	ACS_EMF_CmdClient cmdClient(stopThreadFds[0]);

	if(!cmdClient.connect(acs_dsd::SYSTEM_ID_UNKNOWN,acs_dsd::NODE_STATE_UNDEFINED)){
		ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::sendCmdPassive - Connection with server is failed");
		DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::sendCmdPassive - Leaving");
		return EMF_RC_ERROR;
	}	

	DEBUG(0, "%s", "ACS_EMF_NANOUSBHandler::sendCmdPassive - Connection to service on passive node from active node is successful");
	if(acs_emf_cmd_ns::EMF_CMD_RemovePassiveShare == cmdCode){
		std::list<string> lstArgs;
		lstArgs.push_back(SYNC_NFS_REMOVE);

		ACS_EMF_Cmd cmdSend(acs_emf_cmd_ns::EMF_CMD_RemovePassiveShare);
		cmdSend.addArguments(lstArgs);
		std::list<std::string> lst = cmdSend.commandArguments();
		std::list<std::string>::const_iterator it;
		DEBUG(1,"ACS_EMF_NANOUSBHandler::sendCmdPassive - Send [%i] elements to Passive node.", lst.size());
		for(it = lst.begin(); it !=  lst.end(); ++it)
			DEBUG(1,"ACS_EMF_NANOUSBHandler::sendCmdPassive - Send to Passive node element [%s]", (*it).c_str());

		if(!cmdClient.send(cmdSend)){
			ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::sendCmdPassive - Sending message to DVD-owner node is failed!");
			return EMF_RC_ERROR;
		}
	}
	else{
		ACS_EMF_Cmd cmdSend(cmdCode);
		if(!cmdClient.send(cmdSend)){
			ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::sendCmdPassive - Sending message to server is failed");
			DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::sendCmdPassive - Leaving");
			return EMF_RC_ERROR;
		}
	}
	DEBUG(0, "%s", "ACS_EMF_NANOUSBHandler::sendCmdPassive - Received reply from DSD server to client successfully");
	ACS_EMF_Cmd cmdRecv;
	if(!cmdClient.receive(cmdRecv)){
		ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::sendCmdPassive - Client not able to receive reply from server");
		DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::sendCmdPassive - Leaving");
		return EMF_RC_ERROR;
	}	

	DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::sendCmdPassive - Client received reply from server successfully");
	std::list<std::string> argList = cmdRecv.commandArguments();
	int exitCode1=(int)cmdRecv.commandCode();
	INFO(1,"ACS_EMF_NANOUSBHandler::sendCmdPassive - Code received from Server = %d",exitCode1);
	if (exitCode1 != 0){
		INFO(1,"%s", "ACS_EMF_NANOUSBHandler::sendCmdPassive - Failed");
		DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::sendCmdPassive - Leaving");
		return EMF_RC_ERROR;
	}		
	INFO(1,"%s", "ACS_EMF_NANOUSBHandler::sendCmdPassive - Success")
	DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::sendCmdPassive - Leaving");
	return EMF_RC_OK;
}

//---------------------------------------------------------------------
ACE_INT32 ACS_EMF_NANOUSBHandler::copyDataFromOtherMedia()
{
	if(ACS_EMF_Common::formatAndMountMedia() == EMF_RC_ERROR ){
		ERROR(1,"%s","ACS_EMF_NANOUSBHandler::copyDataFromOtherMedia - Formating thumbdrive  on node failed!!! ");
		return EMF_RC_ERROR;
	}
	if(ACS_EMF_Common::copyFromMasterUsbMedia() != EMF_RC_OK){
		DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::copyDataFromOtherMedia - operation failed");
		return EMF_RC_ERROR;
	}
	DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::copyDataFromOtherMedia - Copy during synchronization is Success for THUMBDRIVE!!");
	return EMF_RC_OK;
}//end of copyDataFromOtherMedia

//---------------------------------------------------------------------
ACE_UINT32 ACS_EMF_NANOUSBHandler::fileListToCopyToMedia(std::string& fileListPath)
{
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::fileListtocopyToMedia - Entering");
	ACE_INT32 dwResult= EMF_RC_ERROR;
	std::string fileName("");
	std::string fileMPath (NBI_root);
	std::string activeDataPath(ACS_EMF_ACTIVE_DATA);
	if(ACS_EMF_DSDServer::theMediaOwnerinPassive != MEDIA_ON_PARTNER_NODE)
	{
		if(ACS_EMF_Common::unmountFileM() == EMF_RC_ERROR)
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::fileListtocopyToMedia - UnMounting fileM folder failed on passive node");
		EXIT_IF_ENDSIGNAL()	;
		if(ACS_EMF_Common::mountMediaOnOtherNode() == EMF_RC_ERROR){
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::fileListtocopyToMedia - Mounting fileM folder failed on passive node");
			EXIT_IF_ENDSIGNAL();
		}
		//Modify fileList with actual path on passive node
		fileName.clear();
		std::string path = fileListPath;
		DEBUG(1,"ACS_EMF_NANOUSBHandler::fileListtocopyToMedia - std::string path(fileListPath)  = %s",path.c_str());
		fileName = path;
		while (true)
		{
			size_t found;
			found=fileName.find_last_of("/\\");
			if (found == (fileName.size()-1))
				fileName = fileName.substr(0,found);
			else{
				fileName = fileName.substr((found));
				DEBUG(1,"ACS_EMF_NANOUSBHandler::fileListtocopyToMedia - Finally modified path = %s\n",fileName.c_str());
				break;
			}
		}
		DEBUG(1,"ACS_EMF_NANOUSBHandler::fileListtocopyToMedia - Folder name = %s",fileName.c_str());
		activeDataPath.append(fileName);
		DEBUG(1,"ACS_EMF_NANOUSBHandler::fileListtocopyToMedia - activeDataPath after modifying = %s",activeDataPath.c_str());
		ACE_TCHAR szFilePath[512];
		ACE_INT16 a;
		ACE_INT16 len = activeDataPath.size();
		for (a=0;a<=len;a++)
			szFilePath[a] = activeDataPath[a];
		dwResult = ACS_EMF_Common::ValidateFileArgumentToMedia(szFilePath);
	}
	if (dwResult != EMF_RC_OK){
		if(ACS_EMF_Common::unmountFileM() == EMF_RC_ERROR)
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::fileListtocopyToMedia - UnMounting fileM folder failed on passive node");
		EXIT_IF_ENDSIGNAL()	;
		if(ACS_EMF_Common::moutFileMOnPassive() == EMF_RC_ERROR){
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::fileListtocopyToMedia - Mounting fileM folder failed on passive node");
			EXIT_IF_ENDSIGNAL()	;
			modifyEndHistoryAttrs((ACE_TCHAR*) FAILED);
			copyObjToObj((ACE_TCHAR*) ACS_EMF_CURRENT_HISTORY_INSTANCE_RDN, (ACE_TCHAR*) ACS_EMF_FIRST_HISTORY_INSTANCE_RDN);
			modifyStructObjFinalAttr(FAILURE,"NFS SHARING IS FAILED.TRY AGAIN.");
			DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::fileListtocopyToMedia - Leaving");
			return EMF_RC_DVDDEVICENOTFOUND;
		}
		DEBUG(1,"ACS_EMF_NANOUSBHandler::fileListtocopyToMedia - Before modifying filelist path = %s",fileListPath.c_str());
		fileName.clear();
		std::string path(fileListPath);
		fileName = path.substr(path.find(fileMPath) + fileMPath.length());
		activeDataPath.clear();
		activeDataPath.append(ACS_EMF_ACTIVE_DATA);
		activeDataPath.append(fileName);
		DEBUG(1,"ACS_EMF_NANOUSBHandler::fileListtocopyToMedia - activeDataPath after modifying = %s",activeDataPath.c_str());
	}
	fileListPath.clear();
	fileListPath.append(activeDataPath);
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::fileListtocopyToMedia - Leaving");
	return EMF_RC_OK;
}//end of fileListtocopyToMedia

//---------------------------------------------------------------------
ACE_INT32 ACS_EMF_NANOUSBHandler::formatMediaInUnlock()
{
	DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::formatMediaInUnlock - Entering");
	ACE_INT32 rCode = EMF_RC_ERROR;
	if((ACS_EMF_Common::checkForUSBStatus() == EMF_RC_OK) && (ACS_EMF_Common::formatMedium() == EMF_RC_OK)){
		DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::formatMediaInUnlock - success on Active Node");
		rCode=EMF_RC_OK;
	}

	if(syncPassiveMedia(acs_emf_cmd_ns::EMF_CMD_FormatMedia) == 0){
		DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::formatMediaInUnlock - success on passive Node");
		rCode=EMF_RC_OK;
	}
	DEBUG(1, "%s", "ACS_EMF_NANOUSBHandler::formatMediaInUnlock - Leaving");
	return rCode;
}

ACE_INT32 ACS_EMF_NANOUSBHandler::enableMediaOnAccessibleNode(int localMediaState)
{
	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::enableMediaOnAccessibleNode - Entering");
	int32_t mediaOwnerSysId = acs_dsd::SYSTEM_ID_UNKNOWN;	
	int retCode = EMF_RC_OK;

	if (localMediaState == MEDIA_FEATURE_ENABLE) {
		ACS_DSD_Client dsdClient;
		ACS_DSD_Node localNode;
		dsdClient.get_local_node(localNode);
		mediaOwnerSysId = localNode.system_id;

		if(mediaOwnerSysId != acs_dsd::SYSTEM_ID_UNKNOWN)
		{
			ACS_EMF_CmdClient cmdClient(m_stopHandlefornanoUSB);

			if(!cmdClient.connect(mediaOwnerSysId , acs_dsd::NODE_STATE_ACTIVE)){
				ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::enableMediaOnAccessibleNode - Connection to service on Media-owner node is failed!");
				return EMF_RC_UNABLETOCONNECTSERV;
			}

			ACS_EMF_Cmd cmdSend(acs_emf_cmd_ns::EMF_CMD_CheckAndMountMedia);

			if(!cmdClient.send(cmdSend)){
				ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::enableMediaOnAccessibleNode - Sending message to DVD-owner node is failed!");
				return EMF_RC_CANNOTSENDCMDTOSERVER;
			}

			ACS_EMF_Cmd cmdRecv;
			if(!cmdClient.receive(cmdRecv)){
				ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::enableMediaOnAccessibleNode - Client not able to receive reply from DSD server!");
				return EMF_RC_CANNOTRECEIVEROMSERVER;
			}
			DEBUG(0, "%s", "ACS_EMF_NANOUSBHandler::enableMediaOnAccessibleNode - Successfully Received reply from Media-owner!");
			int cmdCode=(int)cmdRecv.commandCode();
			DEBUG(1,"ACS_EMF_NANOUSBHandler::enableMediaOnAccessibleNode - Code received from Media-owner = %d",cmdCode);

			switch(cmdCode){
				case ACS_EMF_DSDServer::EMFSERVER_ERROR:
					DEBUG(1,"%s", "ACS_EMF_NANOUSBHandler::enableMediaOnAccessibleNode - Error Received from EMF server: Mounting Media on DVD owner fails !");
					retCode = EMF_RC_ERRORRECEIVINGFROMSERVER;
					break;
				case ACS_EMF_DSDServer::EMFSERVER_OK:
					INFO(1,"%s", "ACS_EMF_NANOUSBHandler::enableMediaOnAccessibleNode - UnMounting Media on the Media-owner Node is SUCCESSFUL");
					break;
				case ACS_EMF_DSDServer::EMFSERVER_MEDIANOTMOUNTED:
					INFO(1,"%s", "ACS_EMF_NANOUSBHandler::enableMediaOnAccessibleNode - Media not enabled on the Media-owner Node");
					break;
				case ACS_EMF_DSDServer::EMFSERVER_MEDIAALREADYENABLED:
					INFO(1,"%s", "ACS_EMF_NANOUSBHandler::enableMediaOnAccessibleNode - Media Already enabled on the Media-owner Node");
					retCode = checkAndSyncMedia();
					break;
				default:
					ERROR(1,"ACS_EMF_NANOUSBHandler::enableMediaOnAccessibleNode - Unexpected Code received from Server = %d",cmdCode);
					ERROR(1,"%s", "ACS_EMF_NANOUSBHandler::enableMediaOnAccessibleNode - UnMounting DVD media on the Media-owner Node is FAILED");
					retCode = EMF_RC_NOK;
					break;
			}
		}
		else{
			INFO(1,"%s", "ACS_EMF_NANOUSBHandler::enableMediaOnAccessibleNode - Media owner systemId not Found");
			retCode = EMF_RC_NOK;
		}

	}
	else{
		DEBUG(0, "%s", "ACS_EMF_NANOUSBHandler::enableMediaOnAccessibleNode - Media State NOT ENABLED");
	}

	DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::enableMediaOnAccessibleNode - Leaving");
	return retCode;
}

ACE_INT32 ACS_EMF_NANOUSBHandler::checkAndSyncMedia()
{
	int syncFlag = ACS_EMF_Common::getMediaFlag(Sync_Pos);
	if(syncFlag == 1){
		DEBUG(1,"%s","ACS_EMF_NANOUSBHandler::checkAndSyncMedia - both the thumbdrives are in sync");
	}
	else{
		//If Thumb drive is present on both the Nodes, though Passive Node goes down EMF functionality will be still ENABLED.
		//So, Media has to be synced on both nodes when Passive Node comes UP.
		if(sendCmdPassive(acs_emf_cmd_ns::EMF_CMD_SyncMediaOnPassive) != EMF_RC_OK)
			ERROR(1, "%s", "ACS_EMF_NANOUSBHandler::checkAndSyncMedia - unable to share active /media");
	}
	return EMF_RC_OK;
}

////---------------------------------------------------------------------
//void ACS_EMF_NANOUSBHandler::stopMediaOperation()
//{
//	m_stopFlag=true;
//}
////---------------------------------------------------------------------

