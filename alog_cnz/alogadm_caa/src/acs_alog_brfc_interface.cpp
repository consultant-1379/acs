//******************************************************************************
//
//  NAME
//     acs_alog_brfc_interface.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//          190 89-CAA nnn nnnn
//
//  AUTHOR
//     2012-07-12 by XCSSUBM PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
  INCLUDE DECLARATION SECTION
  =================================================================== */
#include <acs_apgcc_omhandler.h>
#include <acs_apgcc_adminoperation.h>
#include <acs_alog_command.h>
#include <acs_alog_brfc_interface.h>
#include <acs_alog_defs.h>
#include <acs_alog_ha_AppManager.h>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <sys/epoll.h>

extern bool  MCpSystem;
extern char  PathCLog[];

extern ACS_ALOG_HA_AppManager *haObj; 					// defined in 'acs_alog_main.cpp'
extern acs_alog_BrfImplementer  BRFCimpl;				// defined in 'acs_alog_ha_AppManager.cpp'
extern acs_alog_BrfcThread  BRFCthread;					// defined in 'acs_alog_ha_AppManager.cpp'

using namespace std;

bool acs_alog_BrfImplementer::isBrfInProgress = false;


acs_alog_BrfImplementer::acs_alog_BrfImplementer()
{
}

acs_alog_BrfImplementer::acs_alog_BrfImplementer(string p_impName ):acs_apgcc_objectimplementerinterface_V3(p_impName)
{
}

acs_alog_BrfImplementer::acs_alog_BrfImplementer(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope ): acs_apgcc_objectimplementerinterface_V3(p_objName, p_impName, p_scope)
{
}

bool acs_alog_BrfImplementer::getBrfStatus()
{
	return isBrfInProgress;
}

void acs_alog_BrfImplementer:: adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType** paramList)
{
ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("BRFC-class :  adminOperationCallback"));

	switch (operationId)
	{
		case   BRM_PARTICIPANT__PERMIT_BACKUP:
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Case 1 :  PERMIT_BACKUP"));
			isBrfInProgress = true;
			break;

		case BRM_PARTICIPANT__COMMIT_BACKUP:
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Case 2 :  COMMIT_BACKUP"));
			isBrfInProgress = false;
			break;

		case BRM_PERSISTENT_DATA_OWNER__PREPARE_BACKUP:
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Case 3 :  PREPARE_BACKUP"));
			isBrfInProgress = true;
			break;

		case BRM_PARTICIPANT__CANCEL_BACKUP:
			ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("Case 4 :  CANCEL_BACKUP"));
			isBrfInProgress = false;
			break;
	}

	int dim=0;
	int i=0;
	while(paramList[i])
	{
		i++;
		dim++;
	}

	unsigned long long requestId = 0;
	i = 0;
	while( paramList[i] )
	{
		switch ( paramList[i]->attrType )
		{
			// ONLY FOR requested parameter
			case ATTR_UINT64T:
				if(strcmp(paramList[i]->attrName,"requestId")==0)
				{
					requestId = *reinterpret_cast<unsigned long long *>(paramList[i]->attrValues);
				}
				break;

			default:
				break;
		}
		i++;
	}

	int retVal = acs_alog_command::isBrfcPermitted();
	if (retVal)
	{
	}
	else
	{
	}

	int retAdminOperationResult = this->adminOperationResult( oiHandle , invocation, retVal );

	if (operationId == BRM_PERSISTENT_DATA_OWNER__PREPARE_BACKUP)
	{
		retVal = this->ResponseToBrfc (requestId, BRF_SUCCESS, BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS);

		if (MCpSystem == true)
		{
				string  backupPath, clearFile;

				getAlogDataDiskPath (backupPath);
				backupPath += "/ACS_ALOG/command_logs/";

				char  linuxCmd[500];

		        ACSALOGLOG (LOG_LEVEL_INFO, TEXTERROR("deleting of the Backup CLOG folder"));
				strcpy (linuxCmd, "/bin/rm -r ");
				strcat (linuxCmd, backupPath.c_str());
				system (linuxCmd);

		        ACSALOGLOG (LOG_LEVEL_INFO, TEXTERROR("copying of the CLOG folder"));
				strcpy (linuxCmd, "/bin/cp -r ");
				strcat (linuxCmd, PathCLog);
				strcat (linuxCmd, " ");
				strcat (linuxCmd, backupPath.c_str());
				system (linuxCmd);

				getClearDataDiskPath (clearFile);
				clearFile += "/AlogRestore.txt";

				FILE  *fp = ACE_OS::fopen (clearFile.c_str(), "w");
				if (fp != NULL)
				{
						ACE_OS::fputs ("OK\n", fp);
						ACE_OS::fflush (fp);
						ACE_OS::fclose (fp);
				}
				else  ACSALOGLOG (LOG_LEVEL_ERROR, TEXTERROR("ALOG restore file creation failed"));

				string  clogFolderCreated;

				clogFolderCreated.assign(PathCLog);
				clogFolderCreated += "ClogFolderCreated.txt";
				unlink (clogFolderCreated.c_str());

				sleep (5);
		}
		retVal = this->ResponseToBrfc (requestId, BRF_SUCCESS, BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
	}
	else
	{
		retVal = this->ResponseToBrfc(requestId, BRF_SUCCESS, BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
	}
}


ACS_CC_ReturnType acs_alog_BrfImplementer::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("BRFC-class :  create"));
	(void)oiHandle;
	(void)ccbId;
	(void)className;
	(void)parentName;
	(void)attr;
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType acs_alog_BrfImplementer::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("BRFC-class :  deleted"));
	(void)oiHandle;
	(void)ccbId;
	(void)objName;
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType acs_alog_BrfImplementer::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("BRFC-class :  modify"));
	(void)oiHandle;
	(void)ccbId;
	(void)objName;
	(void)attrMods;
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType acs_alog_BrfImplementer::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("BRFC-class :  complete"));
	(void)oiHandle;
	(void)ccbId;
	return ACS_CC_SUCCESS;
}

void acs_alog_BrfImplementer::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("BRFC-class :  abort"));
	(void)oiHandle;
	(void)ccbId;
}

void acs_alog_BrfImplementer::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("BRFC-class :  apply"));
	(void)oiHandle;
	(void)ccbId;
}

ACS_CC_ReturnType acs_alog_BrfImplementer::updateRuntime(const char* p_objName, const char** p_attrName)
{
ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("BRFC-class :  updateRuntime"));
	(void)p_objName;
	(void)p_attrName;
	return ACS_CC_SUCCESS;
}


int acs_alog_BrfImplementer::ResponseToBrfc(unsigned long long requestId,int brfStatus,int responseCode)
{
ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("BRFC-class :  ResponseToBrfc"));
	acs_apgcc_adminoperation admOp;
	std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;
	ACS_APGCC_AdminOperationParamType firstElem;
	ACS_APGCC_AdminOperationParamType secondElem;

	char firstAttName[]= "requestId";
	char secondAttNameVal1[]= "resultCode";
	char secondAttNameVal2[]= "progressCount";
	char thirdAttName[]= "message";
	unsigned long long requestIdValue = requestId;
	int brfStatusVal = brfStatus; // BRF_SUCCESS
	char* strValue = const_cast<char*>("");
	void* valueStr[1]={ reinterpret_cast<void*>(strValue) };
	int intAttrValue = 50; // 50 %

	if( responseCode  == BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT )
	{
		ACS_APGCC_AdminOperationParamType thirdElem;

		//create first Element of parameter list
		firstElem.attrName =firstAttName;
		firstElem.attrType=ATTR_UINT64T;
		// return requestId previously checked
		firstElem.attrValues=reinterpret_cast<void*>(&requestIdValue);

		// create second Element of parameter list
		secondElem.attrName =secondAttNameVal1;
		secondElem.attrType=ATTR_INT32T ;
		secondElem.attrValues=reinterpret_cast<void*>(&brfStatusVal);

		//create the third Element of parameter list
		thirdElem.attrName =thirdAttName;
		thirdElem.attrType=ATTR_STRINGT ;
		thirdElem.attrValues=valueStr;

		//insert into the vector
		vectorIN.push_back(firstElem);
		vectorIN.push_back(secondElem);
		vectorIN.push_back(thirdElem);
	}
	else if (responseCode == BRM_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS)
	{
		//create first Element of parameter list
		firstElem.attrName =firstAttName;
		firstElem.attrType=ATTR_UINT64T;
		firstElem.attrValues=reinterpret_cast<void*>(&requestIdValue);

		// create second Element of parameter list
		secondElem.attrName =secondAttNameVal2;
		secondElem.attrType=ATTR_UINT32T ;
		secondElem.attrValues=reinterpret_cast<void*>(&intAttrValue);

		//insert into the vector
		vectorIN.push_back(firstElem);
		vectorIN.push_back(secondElem);
	}

	const char *dnObjName1 = "brfParticipantContainerId=1";
	long long int timeOutVal_30sec = 30*(1000000000LL);

	int res1 = admOp.init();

	if (res1 != ACS_CC_SUCCESS)
	{
		return -1;
	}

	int returnValue1 = 1;
	/*Wait at least one second before invoke administrative operations in order to synchronize whit BRFC*/
	sleep(1);
	/* Invoke operation  */
	int retry = 0;
	while (admOp.adminOperationInvoke(dnObjName1, 0 , responseCode , vectorIN , &returnValue1, timeOutVal_30sec) == ACS_CC_FAILURE)
	{
		std::string tempErrMsg ="ERROR CODE = ";
		tempErrMsg += admOp.getInternalLastError();
		tempErrMsg += " ERROR MESSAGE = " ;
		tempErrMsg+= admOp.getInternalLastErrorText();


		if (admOp.getInternalLastError()!=((-1)*SA_AIS_ERR_EXIST)) break;

		sleep(1);
		if (++retry > 60) return -1;
	}

	return 0;
}


/*=================================================================
ROUTINE: ACS_LM_BrfcThread
=================================================================== */
acs_alog_BrfcThread::acs_alog_BrfcThread(acs_alog_BrfImplementer *impl)
{
	theBrfcALOGImplementer = impl;
	theIsStop = true;
	isBrfcALOGImplementerSet = false;
}

/*=================================================================
ROUTINE: ACS_LM_BrfcThread destructor
=================================================================== */
acs_alog_BrfcThread::~acs_alog_BrfcThread()
{

}

/*=================================================================
ROUTINE: setImpl
=================================================================== */
int acs_alog_BrfcThread::setImpl(acs_alog_BrfImplementer *pImpl)
{
	if(isBrfcALOGImplementerSet)
		return 0;

	if (oiHandler.addObjectImpl(pImpl) != ACS_CC_SUCCESS)
	{
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("AddObjectImpl :  failure"));
		return -1;
	}

	isBrfcALOGImplementerSet = true;

	return 0;
}

/*=================================================================
ROUTINE: deleteImpl
=================================================================== */
int acs_alog_BrfcThread::deleteImpl(acs_alog_BrfImplementer *pImpl)
{
	if(!isBrfcALOGImplementerSet)
		return 0;

	if(oiHandler.removeObjectImpl(pImpl) != ACS_CC_SUCCESS)
	{
		ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("RemoveObjectImpl :  failure"));
		return -1;
	}

	isBrfcALOGImplementerSet = false;

	return 0;
}

/*=================================================================
ROUTINE: stop
=================================================================== */
void acs_alog_BrfcThread::stop()
{
	theIsStop = true;
}

/*=================================================================
ROUTINE: start
=================================================================== */
void acs_alog_BrfcThread::start()
{
	theIsStop = false;
	isBrfcALOGImplementerSet = false;
}

/*=================================================================
ROUTINE: isRunning
=================================================================== */
bool  acs_alog_BrfcThread::isRunning()
{
	if (theIsStop == false)  return true;
	return false;
}

/*=================================================================
ROUTINE: svc
=================================================================== */
int acs_alog_BrfcThread::svc(void)
{
	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("BRFC-class :  svc"));
	int apgcc_error_code = 0;
	char apgcc_error_message_buf[256]= {0};
	char log_message[512] = {0};

	const int ALOG_IMM_ERR_OBJ_EXIST = -14; /* error code for ACS_APGCC_ERR_IMM_EXIST */
	const char *alog_brf_mo_instance_dn =  "brfPersistentDataOwnerId=ERIC-APG-ACS-ALOG,brfParticipantContainerId=1";

	// PHASE 1: Register ALOG Server as "Persistent Data Owner" BRF partecipant
	while(!theIsStop)
	{
		ACSALOGLOG(LOG_LEVEL_DEBUG, TEXTERROR("BRFC-class : trying to register ALOG Server as 'Persistent Data Owner' BRF partecipant ..."));
		if(haObj)
		{
			int op_res = haObj->createParticipant(& apgcc_error_code, apgcc_error_message_buf, sizeof(apgcc_error_message_buf));
			if(op_res == 0)
			{
				ACSALOGLOG(LOG_LEVEL_INFO, TEXTERROR("BRFC-class : ... ALOG partecipant successfully created. "));
				break; // success ! Exit loop
			}
			else if(apgcc_error_code == ALOG_IMM_ERR_OBJ_EXIST)
			{
				// in most cases this error code means that the MO instance already exists, but there are cases
				// where APGCC can return error code -14 even if the IMM object doesn't exist. So let's double check...
				OmHandler omHandler;
				ACS_APGCC_ImmObject imm_object;

				if(omHandler.Init() != ACS_CC_SUCCESS)
				{
					ACSALOGLOG(LOG_LEVEL_ERROR, TEXTERROR("BRFC-class : Error in 'OmHandler::Init()' while checking ALOG partecipant MO instance !"));
				}
				else
				{
					// check MO instance invoking 'getObject()'
					imm_object.objName = alog_brf_mo_instance_dn;
					ACS_CC_ReturnType getObject_res = omHandler.getObject(&imm_object);

					if(omHandler.Finalize() != ACS_CC_SUCCESS)
						ACSALOGLOG (LOG_LEVEL_ERROR, TEXTERROR("BRFC-class : Error in 'OmHandler::Finalize()' while checking ALOG partecipant MO instance !"));

					if(getObject_res == ACS_CC_SUCCESS)
					{
						ACSALOGLOG(LOG_LEVEL_WARN, TEXTERROR("BRFC-class : ... ALOG partecipant already exists."));
						break; // success (object already exists) ! Exit loop
					}
				}
			}
		}

		snprintf(log_message, sizeof(log_message),"BRFC-class : unable to register ALOG Server as BRF partecipant ! Details : %s", haObj ? apgcc_error_message_buf : "(haObj == NULL)");
		ACSALOGLOG(LOG_LEVEL_ERROR, TEXTERROR(log_message));

		ACE_OS::sleep(3);	// wait a few seconds and try again
	}

	// PHASE 2 : Register Object implementer for ALOG  'PersistenDataOwner' MO
	const char *dnObjName = "brfPersistentDataOwnerId=ERIC-APG-ACS-ALOG,brfParticipantContainerId=1";
	const char *impName = "ACS_ALOGBrfc_Impl";
	ACS_APGCC_ScopeT scope = ACS_APGCC_ONE;
	BRFCimpl.setObjName(dnObjName);
	BRFCimpl.setImpName(impName);
	BRFCimpl.setScope(scope);
	while(!theIsStop)
	{
		ACSALOGLOG(LOG_LEVEL_DEBUG, TEXTERROR("BRFC-class : trying to set object implementer for ALOG 'BrfPersistenDataOwner' MO..."));
		if(BRFCthread.setImpl(&BRFCimpl) == 0)
		{
			ACSALOGLOG(LOG_LEVEL_INFO, TEXTERROR("BRFC-class : ... successfully set object implementer for ALOG 'BrfPersistenDataOwner' MO. "));
			break ;  // success ! Exit loop
		}

		ACSALOGLOG(LOG_LEVEL_ERROR, TEXTERROR("BRFC-class : unable to set object implementer for ALOG 'BrfPersistenDataOwner' MO"));

		ACE_OS::sleep(3); // wait a few seconds and try again
	}

	// PHASE 3: wait for IMM events and 'stop thread' command
    epoll_event  event_to_check_in, event_to_check_out;
    char  strError[1024];
	int   ret, ep_fd = ::epoll_create1 (EPOLL_CLOEXEC);

    event_to_check_in.events = EPOLLIN;

    ::epoll_ctl (ep_fd, EPOLL_CTL_ADD, theBrfcALOGImplementer->getSelObj(), &event_to_check_in );

	while (! theIsStop)
	{
       	ret = ::epoll_wait (ep_fd, &event_to_check_out, 1, 400);

		if (ret == 0)
		{
		}
		else if (ret == -1)
		{
				sprintf (strError,"::epoll_wait return %i, errno = %i", ret, errno);
				ACSALOGLOG (LOG_LEVEL_ERROR,TEXTERROR(strError));
				if (! theIsStop)
				{
						::epoll_ctl (ep_fd, EPOLL_CTL_ADD, theBrfcALOGImplementer->getSelObj(), &event_to_check_in );
				}
		}
		else {
				if (! theIsStop  &&  event_to_check_out.events == EPOLLIN)
				{
						theBrfcALOGImplementer->dispatch (ACS_APGCC_DISPATCH_ALL);
				}
		}
		usleep (20000);
	}

	ACSALOGLOG(LOG_LEVEL_ERROR,TEXTERROR("BRFC-class :  exit from svc"));

	return 0;
}
