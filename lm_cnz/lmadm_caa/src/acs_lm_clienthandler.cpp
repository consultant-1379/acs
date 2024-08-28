//******************************************************************************
//
//  NAME
//     acs_lm_clienthandler.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2008. All rights reserved.
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
//	    190 89-CAA nnn nnnn
//
//  AUTHOR
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_lm_clienthandler.h"
#include "acs_lm_persistent.h"
#include "acs_lm_lksender.h"
#include <algorithm>
/* LM Maintenance Mode */

#include "ACS_APGCC_CommonLib.h" //XLUIDIR vAPZ LM
#include <ACS_CS_API.h>

bool graceOnInMaintenance; 
/*=================================================================
	ROUTINE: ACS_LM_ClientHandler constructor
=================================================================== */
ACS_LM_ClientHandler::ACS_LM_ClientHandler(ACS_LM_Sentinel* sentinel, 
		ACS_LM_Persistent* persistFile,
		ACS_LM_EventHandler* eventHandler)
:sentinel(sentinel),
 persistFile(persistFile),
 eventHandler(eventHandler)
{
	DEBUG("ACS_LM_ClientHandler::ACS_LM_ClientHandler()  %s","Entering");
	testLkf = persistFile->testLkFile();
	DEBUG("ACS_LM_ClientHandler::ACS_LM_ClientHandler()  %s","Leaving");
}//end of constructor
/*=================================================================
	ROUTINE: ACS_LM_ClientHandler destructor
=================================================================== */
ACS_LM_ClientHandler::~ACS_LM_ClientHandler()
{
	DEBUG("ACS_LM_ClientHandler::~ACS_LM_ClientHandler()  %s","Entering");
	DEBUG("ACS_LM_ClientHandler::~ACS_LM_ClientHandler()  %s","Leaving");
}//end of destructor
/*=================================================================
	ROUTINE: handleClient
=================================================================== */
void ACS_LM_ClientHandler::handleClient(ACS_LM_CmdServer &cmdServer)
{
	DEBUG("ACS_LM_ClientHandler::handleClient()  %s","Entering");
	ACS_LM_Cmd cmdReceived;
	cmdServer.receive(cmdReceived);
	int cmdCode = cmdReceived.commandCode();
	INFO("In handleClient(): cmdCode = %d",cmdCode);
	switch(cmdCode)
	{
	case ACS_LM_CMD_LKINST:
	{
		DEBUG("ACS_LM_ClientHandler::handleClient()  %s","ACS_LM_CMD_LKINST");
		handleLkInst(cmdServer,cmdReceived);
		break;
	}
	case ACS_LM_CMD_LKLIST:
	{
		DEBUG("ACS_LM_ClientHandler::handleClient()  %s","ACS_LM_CMD_LKLIST");
		handleLkLs(cmdServer,cmdReceived);
		break;
	}
	case ACS_LM_CMD_LKMAPLIST:
	{
		DEBUG("ACS_LM_ClientHandler::handleClient()  %s","ACS_LM_CMD_LKMAPLIST");
		handleLkMapLs(cmdServer);
		break;
	}
	case ACS_LM_CMD_FPPRINT:
	{
		DEBUG("ACS_LM_ClientHandler::handleClient()  %s","ACS_LM_CMD_FPPRINT");
		handleFpprint(cmdServer);
		break;
	}
	case ACS_LM_CMD_TESTLKADD:
	{
		DEBUG("ACS_LM_ClientHandler::handleClient()  %s","ACS_LM_CMD_TESTLKADD");
		handleTestLkAdd(cmdServer,cmdReceived);
		break;
	}
	case ACS_LM_CMD_TESTLKREM:
	{
		DEBUG("ACS_LM_ClientHandler::handleClient()  %s","ACS_LM_CMD_TESTLKREM");
		handleTestLkRm(cmdServer,cmdReceived);
		break;
	}
	case ACS_LM_CMD_TESTLKLIST:
	{
		DEBUG("ACS_LM_ClientHandler::handleClient()  %s","ACS_LM_CMD_TESTLKLIST");
		handleTestLkLs(cmdServer);
		break;
	}
	case ACS_LM_CMD_TESTLKACT:
	{

		DEBUG("ACS_LM_ClientHandler::handleClient()  %s","ACS_LM_CMD_TESTLKACT");
		handleTestLkAct(cmdServer);
		break;
	}
	case ACS_LM_CMD_TESTLKDEACT:
	{
		DEBUG("ACS_LM_ClientHandler::handleClient()  %s","ACS_LM_CMD_TESTLKDEACT");
		handleTestLkDeAct(cmdServer);
		break;
	}
	case ACS_LM_CMD_LKEMSTART:
	{
		DEBUG("ACS_LM_ClientHandler::handleClient()  %s","ACS_LM_CMD_LKEMSTART");
		//	handleLkEmStart(cmdServer);
		break;
	}
	case ACS_LM_CMD_SHOWLICENSE:
	{
		DEBUG("ACS_LM_ClientHandler::handleClient()  %s","ACS_LM_CMD_SHOWLICENSE");
		handleShowLicense(cmdServer);
		//cmdServer.close();
		break;
	}
	default:
	{
		DEBUG("ACS_LM_ClientHandler::handleClient()  %s","Deafult value");
		break;
	}
	}
	return;
}//end of handleClient
/*=================================================================
	ROUTINE: handleFpprint
=================================================================== */
void ACS_LM_ClientHandler::handleFpprint(ACS_LM_CmdServer& cmdServer)
{
	INFO("%s","In ACS_LM_ClientHandler::handleFpprint()");
	ACS_LM_AppExitCode retnCode = ACS_LM_RC_OK;
	std::string fingerPrint = sentinel->getFingerPrint();
	char** resultantChar = new char*[1];
	resultantChar[0] = new char[(int)fingerPrint.length()+1];
	ACE_OS::strncpy(const_cast<char*>(fingerPrint.c_str()),resultantChar[0],(int)fingerPrint.length()+1);
	INFO("handleFpprint(): retnCode = %d , fingerPrint = %s",retnCode,fingerPrint.c_str());
	ACS_LM_Cmd cmdToSend(retnCode, 1, resultantChar);
	cmdServer.send(cmdToSend);

	delete[] resultantChar[0];
	delete[] resultantChar;	
}//end of handleFpprint
/*=================================================================
	ROUTINE: handleTestLkAdd
=================================================================== */
void ACS_LM_ClientHandler::handleTestLkAdd(ACS_LM_CmdServer& cmdServer,const ACS_LM_Cmd& cmdReceived)
{
	DEBUG("%s","handleTestLkAdd()");
	std::list<std::string> argList = cmdReceived.commandArguments();
	std::list<std::string>::iterator itr;
	bool nflag,pflag,sflag,tflag,vflag;
	//check if lmdata is unlocked and read original persistent data
	//reload lmdata in case of lmdata lock
	handleLmDataLock();
	LkData lmData;
	ACS_LM_AppExitCode retnCode = ACS_LM_RC_NOK;
	nflag=pflag=sflag=tflag=vflag=false;
	/* Test mode should be restricted during MM */
	if((!persistFile->isLmInVirginMode()) && ((persistFile->getLmMode() & ACS_LM_MODE_GRACE) != ACS_LM_MODE_GRACE)
					      && ((persistFile->getLmMode() & ACS_LM_MODE_MAINTENANCE) != ACS_LM_MODE_MAINTENANCE))
	{
		if(sentinel->isTestModeAllowed())
		{
			for(itr = argList.begin(); itr != argList.end();++itr)
			{
				if(!::strcmp((*itr).c_str(),"-n"))
				{
					++itr;
					lmData.lkId.assign((*itr).c_str());
					nflag = true;
				}
				else if(!::strcmp((*itr).c_str(),"-p"))
				{
					++itr;
					lmData.paramName.assign((*itr).c_str());
					pflag = true;
				}
				else if(!::strcmp((*itr).c_str(),"-s"))
				{
					++itr;
					lmData.setName.assign((*itr).c_str());
					sflag = true;
				}
				else if(!::strcmp((*itr).c_str(),"-t"))
				{
					++itr;
					lmData.status = ::atoi((*itr).c_str());
					tflag = true;
				}
				else if(!::strcmp((*itr).c_str(),"-v"))
				{
					++itr;
					lmData.value = ::atoi((*itr).c_str());
					vflag = true;
				}
				else
				{
					//do nothing
				}
				if (nflag && pflag && sflag && tflag && vflag)
				{
					retnCode = testLkf->addTestLk(lmData);
					if(retnCode == ACS_LM_RC_OK)
					{
						if(persistFile->commit())
						{
							DEBUG("%s","handleTestLkAdd()Commit() successful");
						}
						else
						{
							retnCode = ACS_LM_RC_PHYFILEERROR;
							ERROR("%s","handleTestLkAdd()Commit()  failed");
						}
					}
					break;
				}
			}
		}
		else
		{
			retnCode = ACS_LM_RC_TESTMODENOTALLOWED;
		}
	}
	else
	{
		retnCode = ACS_LM_RC_TESTMODENOTALLOWED;
	}
	DEBUG("handleTestLkAdd(): retnCode = %d",retnCode);

	ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
	cmdServer.send(cmdToSend);
}//end of handleTestLkAdd
/*=================================================================
	ROUTINE: handleTestLkRm
=================================================================== */
void ACS_LM_ClientHandler::handleTestLkRm(ACS_LM_CmdServer& cmdServer,const ACS_LM_Cmd& cmdReceived)
{	
	DEBUG("%s","In ACS_LM_ClientHandler::handleTestLkRm()-Entering");
	//reload lmdata in case of lmdata lock
	handleLmDataLock();
	std::list<std::string> argList = cmdReceived.commandArguments();
	std::list<std::string>::iterator itr;
	ACS_LM_AppExitCode retnCode = ACS_LM_RC_NOK;
	/* Test mode should be restricted during MM */
	if((!persistFile->isLmInVirginMode()) && ((persistFile->getLmMode() & ACS_LM_MODE_GRACE) != ACS_LM_MODE_GRACE)
					      && ((persistFile->getLmMode() & ACS_LM_MODE_MAINTENANCE) != ACS_LM_MODE_MAINTENANCE))
	{
		if(sentinel->isTestModeAllowed())
		{

			for(itr = argList.begin(); itr != argList.end();++itr)
			{
				if(!::strcmp((*itr).c_str(),"-n"))
				{
					++itr;
					retnCode = testLkf->removeTestLk(*itr);
					break;
				}
			}
		}
		else
		{
			retnCode = ACS_LM_RC_TESTMODENOTALLOWED;
		}
	}
	else
	{
		retnCode = ACS_LM_RC_TESTMODENOTALLOWED;
	}
	INFO("handleTestLkRm(): retnCode = %d",retnCode);
	ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
	cmdServer.send(cmdToSend);
	DEBUG("%s","In ACS_LM_ClientHandler::handleTestLkRm()-Leaving");
}//end of handleTestLkRm
/*=================================================================
	ROUTINE: handleTestLkLs
=================================================================== */
void ACS_LM_ClientHandler::handleTestLkLs(ACS_LM_CmdServer& cmdServer)
{
	DEBUG("%s","In ACS_LM_ClientHandler::handleTestLkLs()-Entering");

	ACS_LM_AppExitCode retnCode;
	//reload lmdata in case of lmdata lock
	handleLmDataLock();
	/* Test mode should be restricted during MM */
	if((!persistFile->isLmInVirginMode()) && ((persistFile->getLmMode() & ACS_LM_MODE_GRACE) != ACS_LM_MODE_GRACE)
					      && ((persistFile->getLmMode() & ACS_LM_MODE_MAINTENANCE) != ACS_LM_MODE_MAINTENANCE))
	{
		if(sentinel->isTestModeAllowed())
		{
			retnCode = ACS_LM_RC_OK;

			std::list<LkData*> argList;
			(void)testLkf->getTestLks(argList);

			int totalLks = (int)argList.size();
			if(totalLks == 0)
			{
				INFO("In ACS_LM_ClientHandler::handleTestLkLs():retnCode = ",retnCode);
				ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
				cmdServer.send(cmdToSend);
			}
			else
			{
				retnCode = ACS_LM_RC_MOREDATA;
				int i=0;
				for(std::list<LkData*>::iterator itr=argList.begin();
						itr != argList.end(); ++itr)
				{
					LkData* lkData = (*itr);
					char** resultantChar = new char*[5];

					resultantChar[0] = new char[(int)lkData->lkId.size()+1];
					ACE_OS::strncpy(resultantChar[0], const_cast<char*>(lkData->lkId.c_str()), (int)lkData->lkId.size()+1);

					resultantChar[1] = new char[(int)lkData->paramName.size()+1];
					ACE_OS::strncpy(resultantChar[1], const_cast<char*>(lkData->paramName.c_str()), (int)lkData->paramName.size()+1);

					resultantChar[2] = new char[(int)lkData->setName.size()+1];
					ACE_OS::strncpy(resultantChar[2], const_cast<char*>(lkData->setName.c_str()), (int)lkData->setName.size()+1);

					char chStatus[11]={0};
					ACE_OS::itoa(lkData->status, chStatus, 10);
					resultantChar[3] = new char[2];
					ACE_OS::strncpy(resultantChar[3], chStatus, 2);

					char chValue[11]={0};
					ACE_OS::itoa(lkData->value, chValue, 10);
					resultantChar[4] = new char[6];
					ACE_OS::strncpy(resultantChar[4], chValue, 6);

					if(++i == totalLks)
					{
						retnCode = ACS_LM_RC_OK;
					}
					DEBUG("handleTestLkLs(): retnCode = %d",retnCode);
					ACS_LM_Cmd cmdToSend(retnCode, 5, resultantChar);
					cmdServer.send(cmdToSend);
					for(int j=0;j<5;++j)
					{
						delete[] resultantChar[j];
					}
					delete[] resultantChar;
				}
			}

			INFO("%s","handleTestLkLs() argList.clear()");

			for(std::list<LkData*>::iterator itr = argList.begin(); itr != argList.end(); ++itr)
			{
				delete (*itr);
			}
			argList.clear();
		}
		else
		{
			retnCode = ACS_LM_RC_TESTMODENOTALLOWED;
			ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
			cmdServer.send(cmdToSend);
			DEBUG("handleTestLkLs(): retnCode = %d",retnCode);
		}
	}
	else
	{
		retnCode = ACS_LM_RC_TESTMODENOTALLOWED;
		ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
		cmdServer.send(cmdToSend);
		DEBUG("handleTestLkLs(): retnCode = %d",retnCode);
	}
}//end of handleTestLkLs
/*=================================================================
	ROUTINE: handleTestLkAct
=================================================================== */
void ACS_LM_ClientHandler::handleTestLkAct(ACS_LM_CmdServer& cmdServer)
{
	DEBUG("%s","In ACS_LM_ClientHandler::handleTestLkAct()-Entering");
	ACS_LM_AppExitCode retnCode;
	//reload lmdata in case of lmdata lock
	handleLmDataLock();
	/* Test mode should be restricted during MM */
	if((!persistFile->isLmInVirginMode()) && ((persistFile->getLmMode() & ACS_LM_MODE_GRACE) != ACS_LM_MODE_GRACE) 
                                              && ((persistFile->getLmMode() & ACS_LM_MODE_MAINTENANCE) != ACS_LM_MODE_MAINTENANCE))
	{
		if(sentinel->isTestModeAllowed())
		{
			retnCode = ACS_LM_RC_OK;
			ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
			cmdServer.send(cmdToSend);

			//make the command like an ordering command that doesn't wait
			//for the sending and state it in the man pages.
			retnCode = ACS_LM_LkSender::sendLks(ACS_LM_SENDER_TESTACT, sentinel, persistFile, eventHandler);
			//DEBUG("handleTestLkAct(): retnCode = ",retnCode);
			INFO("ACS_LM_ClientHandler::handleTestLkAct(): Test Mode Allowed, retnCode = %d",retnCode);
		}
		else
		{
			retnCode = ACS_LM_RC_TESTMODENOTALLOWED;
			ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
			cmdServer.send(cmdToSend);
			//    DEBUG("handleTestLkAct(): retnCode = ",retnCode);
			INFO("ACS_LM_ClientHandler::handleTestLkAct(): Test Mode Not Allowed, retnCode = %d",retnCode);
		}
	}
	else
	{
		retnCode = ACS_LM_RC_TESTMODENOTALLOWED;
		ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
		cmdServer.send(cmdToSend);
		INFO("ACS_LM_ClientHandler::handleTestLkAct(): Test Mode Not Allowed, retnCode = %d",retnCode);
	}
}//end of handleTestLkAct
/*=================================================================
	ROUTINE: handleLkInst
=================================================================== */
void ACS_LM_ClientHandler::handleLkInst(ACS_LM_CmdServer& cmdServer,const ACS_LM_Cmd& cmdReceived)
{
	DEBUG("%s","In ACS_LM_ClientHandler::handleLkInst()-Entering ");

	ACS_LM_AppExitCode retnCode = ACS_LM_RC_OK;

	std::list<std::string> argList = cmdReceived.commandArguments();
	std::list<std::string>::iterator itr = argList.begin();
	std::string lservPath = *(itr++);

	INFO("handleLkInst() lservPath = %s",lservPath.c_str() );
	bool aNodeInitialized = false;
	bool bNodeInitialized = false;
	// To store values of persistent in case commit fails
	bool emergencyCntReset = false;
	int emergencyCounter = persistFile->getEmergencyCounter();
	ACE_UINT32 lkfChecksum = persistFile->getLkfChecksum();
	int mode = persistFile->getLmMode();
	bool isVirginModeActive = persistFile->isLmInVirginMode();
	bool aNodeInitializedStatus, bNodeInitializedStatus;
	persistFile->updateInitializedNodeStatus(aNodeInitializedStatus, bNodeInitializedStatus);

	retnCode = sentinel->installLkf(lservPath, aNodeInitialized, bNodeInitialized);
	if(retnCode == ACS_LM_RC_OK)
	{
		std::list<LkData*> lkList1;	
		std::list<LkData*> discLKList;  //LMCleanup
		std::list<LkData*> connLKList;  //LMCleanup
		//to get testmode and emergency mode lk's
		(void)sentinel->getValidLks(lkList1,true,true,discLKList,connLKList);  //LMCleanup
		bool foundDummyFeature = false; //XLUIDIR vAPZ LM
		for(std::list<LkData*>::iterator itr = lkList1.begin(); itr != lkList1.end(); ++itr)
		{
			if(((*itr)->lkId.compare(CXCTESTMODE)==0) || ((*itr)->lkId.compare(CXCEMERGENCYMODE)==0))
			{
				if( ((int)(*itr)->paramName.size() >15) ||
						((int)(*itr)->setName.size() >15) || ((*itr)->value > 65535))
				{
					retnCode = ACS_LM_RC_INVALIDLKF;
					break;
				}
			}
			else
			{
				if(((int)(*itr)->lkId.size() > 15) || ((int)(*itr)->paramName.size() >15) ||
						((int)(*itr)->setName.size() >15) || ((*itr)->value > 65535))
				{
					retnCode = ACS_LM_RC_INVALIDLKF;
					break;
				}
			}
		}
		//LMCleanup start
		if(retnCode == ACS_LM_RC_OK)
		{
			for(std::list<LkData*>::iterator itrd = discLKList.begin(); itrd != discLKList.end(); ++itrd)
			{
				if(((int)(*itrd)->paramName.size() > 15) || ((int)(*itrd)->setName.size() >15) ||
						(*itrd)->value != 0 || (*itrd)->status != 0 )
				{
					retnCode = ACS_LM_RC_INVALIDLKF;
					break;
				}
			}
		}

		if(retnCode == ACS_LM_RC_OK)
		{
			for(std::list<LkData*>::iterator itrc = connLKList.begin(); itrc != connLKList.end(); ++itrc)
			{
				if(((int)(*itrc)->paramName.size() > 15) || ((int)(*itrc)->setName.size() >15) ||
						(*itrc)->value != 0 || (*itrc)->status != 0 )
				{
					retnCode = ACS_LM_RC_INVALIDLKF;
					break;
				}
			}
		}
		//LMCleanup end

		if(retnCode == ACS_LM_RC_OK)
		{
			persistFile->setInitializedNodeStatus(aNodeInitialized, bNodeInitialized);
			persistFile->setLkfChecksum(sentinel->getCheckSum());

			if(isVirginModeActive)
			{
				persistFile->setEmergencyCounter(2);
				emergencyCntReset = true;
			}
			//reset the emergency counter value 		
			else if(sentinel->isEmergencyModeAllowed())
			{
				persistFile->setEmergencyCounter(2);
				emergencyCntReset = true;
			}
			else
			{				
				if(emergencyCounter)
				{
					persistFile->setEmergencyCounter(emergencyCounter);						
				}
				else
				{
					persistFile->setEmergencyCounter(0);	
				}
			}
			retnCode = testLkf->removeAllTestLksTemp();
			if(retnCode != ACS_LM_RC_OK)
			{
				retnCode = ACS_LM_RC_OK;
				DEBUG("%s","handleLkInst() testLkf.removeAllTestLksTemp() failed");
			}

			persistFile->setLmMode(ACS_LM_MODE_GRACE, false);
			persistFile->setLmMode(ACS_LM_MODE_TEST, false);
			persistFile->setLmMode(ACS_LM_MODE_EMERGENCY, false);
			//Whil LM in Maintenance mode, LKF installation is allowed.
			if(!((persistFile->getLmMode() & ACS_LM_MODE_MAINTENANCE) == ACS_LM_MODE_MAINTENANCE))
                        {
				persistFile->disableVirginMode();
			}
			if(persistFile->commit())
			{                
				//Take backup of lservrc to system disk of both nodes
				if(sentinel->backup())
				{
					//clear all alarms
					eventHandler->clearAll();
					retnCode = testLkf->removeAllTestLksPerm();
					if(retnCode != ACS_LM_RC_OK)
					{
						retnCode = ACS_LM_RC_OK;
						DEBUG("%s","handleLkInst() testLkf.removeAllTestLksPerm() failed");
					}
					if(emergencyCntReset)
					{
						eventHandler->ceaseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_EM_STATE_UNAVAIL);

						//Emergency State activation lock reset event
						ACS_LM_EventHandler::LmEvent emStateActLockReset = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_EM_STATE_LOCK_RESET-ACS_LM_EVENT_ID_BASE];
						ACS_LM_EventHandler::reportEvent(emStateActLockReset);
					}
					else
					{
						if(!emergencyCounter)
						{
							eventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_EM_STATE_UNAVAIL);
						}
					}
					//cout<<endl<<"before finishInstallation()";
					sentinel->finishInstallation(true, aNodeInitialized, bNodeInitialized);
					//New License Key File installed
					ACS_LM_EventHandler::LmEvent newLicKeyFileInstall = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_NEW_LK_INSTALL-ACS_LM_EVENT_ID_BASE];
					ACS_LM_EventHandler::reportEvent(newLicKeyFileInstall);

					/* @TODO
					 * //TBD: cease the if alarm is active for missing LKF
					eventHandler->ceaseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_LK_FILE_MISS);
					//CNI55_4_578::Cease all alarm incase of new LKF installation
					eventHandler->clearAll();
					//TBD: cease the alarm for LKey about to expire
					eventHandler->verifyAndRaiseAlarm(lkList1);
					 */
					DEBUG("handleLkInst(): retnCode = %d",retnCode);
					ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
					cmdServer.send(cmdToSend);
					// start sending the LKs to CP in non-blocking mode
					DEBUG("handleLkInst(): retnCode = %d",retnCode);
					// delete the lservrc file from its original path
					if(ACE_OS::unlink(lservPath.c_str())==-1)
					{
						//                        unsigned long ec = ::GetLastError();
						ACE_UINT32 s32Result=ACE_OS::last_error();
						INFO("handleLkInst():() DeleteFile() failed, lservPath = %s , error no = %d",lservPath.c_str(),s32Result);
					}

				}
				else
				{
					testLkf->restoreAllTestLks();
					persistFile->reLoad(emergencyCounter, lkfChecksum, mode, isVirginModeActive, aNodeInitialized, bNodeInitialized);
					persistFile->commit();
					//CNI30_4_1719(TR NO HN66027/HN40528)
					sentinel->finishInstallation(false, aNodeInitialized, bNodeInitialized,lkfChecksum);
					retnCode = ACS_LM_RC_NOK;
					ERROR("handleLkInst() sentinel->backup() failed retnCode %d= ",retnCode);
					ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
					cmdServer.send(cmdToSend);
				}
			}
			else
			{
				testLkf->restoreAllTestLks();
				persistFile->reLoad(emergencyCounter, lkfChecksum, mode, isVirginModeActive, aNodeInitializedStatus, bNodeInitializedStatus);
				//CNI30_4_1719(TR NO HN66027/HN40528)
				sentinel->finishInstallation(false, aNodeInitialized, bNodeInitialized,lkfChecksum);
				retnCode = ACS_LM_RC_NOK;
				ERROR("handleLkInst() persistFile->commit() failed retnCode %d= ",retnCode);
				ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
				cmdServer.send(cmdToSend);
			}
		}
		else
		{
			testLkf->restoreAllTestLks();
			persistFile->reLoad(emergencyCounter, lkfChecksum, mode, isVirginModeActive, aNodeInitializedStatus, bNodeInitializedStatus);
			//CNI30_4_1719(TR NO HN66027/HN40528)
			sentinel->finishInstallation(false, aNodeInitialized, bNodeInitialized,lkfChecksum);
			DEBUG(" handleLkInst(): retnCode = %d",retnCode);
			ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
			cmdServer.send(cmdToSend);
		}
		for(std::list<LkData*>::iterator itr = lkList1.begin(); itr != lkList1.end(); ++itr)
		{
			delete (*itr);
		}
		lkList1.clear();
		//LMCleanup start
		for(std::list<LkData*>::iterator itrd = discLKList.begin(); itrd != discLKList.end(); ++itrd)
		{
			delete (*itrd);
		}
		discLKList.clear();
		for(std::list<LkData*>::iterator itrc = connLKList.begin(); itrc != connLKList.end(); ++itrc)
		{
			delete (*itrc);
		}
		connLKList.clear();
		//LMCleanup end

	}
	else /* INSTALLING LKF FAILED */
	{
		testLkf->restoreAllTestLks();
		persistFile->reLoad(emergencyCounter, lkfChecksum, mode, isVirginModeActive, aNodeInitializedStatus, bNodeInitializedStatus);
		persistFile->commit();
		//CNI30_4_1719(TR NO HN66027/HN40528)
		sentinel->finishInstallation(false, aNodeInitialized, bNodeInitialized,lkfChecksum);
		INFO(" handleLkInst(): retnCode = %d",retnCode);
		ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
		cmdServer.send(cmdToSend);
	}		 
}//end of handleLkInst
/*=================================================================
	ROUTINE: handleLkInstViaIMM
=================================================================== */

ACS_LM_AppExitCode ACS_LM_ClientHandler::handleLkInstViaIMM(string aLKFFilePath)
{
	INFO(" In ACS_LM_ClientHandler::handleLkInstViaIMM() %s","Entering");;
	ACS_LM_AppExitCode retnCode = ACS_LM_RC_OK;
	std::string lservPath = aLKFFilePath;
	DEBUG("In ACS_LM_ClientHandler::handleLkInstViaIMM() lservPath = %s",lservPath.c_str() );
	bool aNodeInitialized = false;
	bool bNodeInitialized = false;
	// To store values of persistent in case commit fails
	bool emergencyCntReset = false;
	int emergencyCounter = persistFile->getEmergencyCounter();
	ACE_UINT32 lkfChecksum = persistFile->getLkfChecksum();
	int mode = persistFile->getLmMode();
	bool isVirginModeActive = persistFile->isLmInVirginMode();
	bool aNodeInitializedStatus, bNodeInitializedStatus;
	persistFile->updateInitializedNodeStatus(aNodeInitializedStatus, bNodeInitializedStatus);

	retnCode = sentinel->installLkf(lservPath, aNodeInitialized, bNodeInitialized);
	if(retnCode == ACS_LM_RC_OK)
	{
		std::list<LkData*> lkList1;
		std::list<LkData*> discLKList;  //LMCleanup
		std::list<LkData*> connLKList;  //LMCleanup
		(void)sentinel->getValidLks(lkList1,true,true,discLKList,connLKList);  //LMCleanup

		bool foundDummyFeature = false;
		bool foundMSCDummyFeature = false; 
		bool foundHLRDummyFeature = false; 
		bool foundBSCDummyFeature = false;


		for(std::list<LkData*>::iterator itr = lkList1.begin(); itr != lkList1.end(); ++itr)
		{
			if(((*itr)->lkId.compare(CXCTESTMODE)==0) || ((*itr)->lkId.compare(CXCEMERGENCYMODE)==0))
			{
				if( ((int)(*itr)->paramName.size() >15) ||
						((int)(*itr)->setName.size() >15) || ((*itr)->value > 65535))
				{
					retnCode = ACS_LM_RC_INVALIDLKF;
					break;
				}
			}
			else if((*itr)->lkId.compare(CXC_VIRT_MSC_IPSTP_DF)==0)	// special LK for vMSC
			{
				foundMSCDummyFeature = true;
				DEBUG("%s","handleLkInstViaIMM(): Found vMSC Dummy Feature");
			}
			else if((*itr)->lkId.compare(CXC_VIRT_HLR_DF)==0)	// special LK for vHLR
			{
				foundHLRDummyFeature = true;
				DEBUG("%s","handleLkInstViaIMM(): Found vHLR Dummy Feature");
			}
			else if((*itr)->lkId.compare(CXC_VIRT_BSC_DF)==0)	// special LK for vBSC
			{
				foundBSCDummyFeature = true;
				DEBUG("%s","handleLkInstViaIMM(): Found vBSC Dummy Feature");
			}
			else
			{
				if(((int)(*itr)->lkId.size() > 15) || ((int)(*itr)->paramName.size() >15) ||
						((int)(*itr)->setName.size() >15) || ((*itr)->value > 65535))
				{
					retnCode = ACS_LM_RC_INVALIDLKF;
					break;
				}
			}

		}

		//Get the apt type using CS API
		string aptType="MSC"; //assign aptType with default value
		if(ACS_CS_API_NetworkElement::getAPTType(aptType) != ACS_CS_API_NS::Result_Success)
		{
			DEBUG("%s","Unable to fetch the apt type");
		}

		bool isVirtualized = false;

		ACS_CS_API_CommonBasedArchitecture::ArchitectureValue nodeArchitecture;

		if(ACS_CS_API_NetworkElement::getNodeArchitecture(nodeArchitecture) == ACS_CS_API_NS::Result_Success)
		{
			if (nodeArchitecture == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED){
				isVirtualized = true;
				DEBUG("%s","handleLkInstViaIMM(): Node architecture is VIRTUALIZED");
				DEBUG("aptType is %s",aptType.c_str());
				if((((aptType.compare("MSC")==0)||(aptType.compare("IPSTP")==0)) && foundMSCDummyFeature) //TR HW48901 - for IPSTP
						|| ((aptType.compare("HLR")==0) && foundHLRDummyFeature)
						|| ((aptType.compare("BSC")==0) && foundBSCDummyFeature))
				{
					foundDummyFeature = true;
				}
			}
			else
				DEBUG("%s","handleLkInstViaIMM(): Node architecture is NATIVE");
		}
		else
			DEBUG("%s","handleLkInstViaIMM(): Node architecture UNKNOWN");

		// for vMSC,vHLR & vBSC, the LKF should contain respective special LK which identifies virtual environment
		// if on virtual APG these virtual node application LKs are not present, LKF installation shall be rejected
		if(retnCode == ACS_LM_RC_OK && isVirtualized && !foundDummyFeature)
		{
			retnCode = ACS_LM_RC_INVALIDLKF;
			DEBUG("%s","handleLkInstViaIMM(): Invalid LKF in virtualized: missing Dummy Feature");
		}

		//LMCleanup start
		if(retnCode == ACS_LM_RC_OK)
		{
			for(std::list<LkData*>::iterator itrd = discLKList.begin(); itrd != discLKList.end(); ++itrd)
			{
				if(((int)(*itrd)->paramName.size() > 15) || ((int)(*itrd)->setName.size() >15) ||
						(*itrd)->value != 0 || (*itrd)->status != 0 )
				{
					retnCode = ACS_LM_RC_INVALIDLKF;
					break;
				}
			}
		}

		if(retnCode == ACS_LM_RC_OK)
		{
			for(std::list<LkData*>::iterator itrc = connLKList.begin(); itrc != connLKList.end(); ++itrc)
			{
				if(((int)(*itrc)->paramName.size() > 15) || ((int)(*itrc)->setName.size() >15) ||
						(*itrc)->value != 0 || (*itrc)->status != 0 )
				{
					retnCode = ACS_LM_RC_INVALIDLKF;
					break;
				}
			}
		}
		//LMCleanup end

		if(retnCode == ACS_LM_RC_OK)
		{
			//cout<<endl<<"ALL KEYS OK";
			persistFile->setInitializedNodeStatus(aNodeInitialized, bNodeInitialized);
			persistFile->setLkfChecksum(sentinel->getCheckSum());
			persistFile->getLkfChecksum();
			//Change to make the emergency count as 2 when intstalled from virgin
			//mode to Emergency mode without emergency key in the LKF
			if(isVirginModeActive)
			{
				persistFile->setEmergencyCounter(2);
				emergencyCntReset = true;
			}
			//reset the emergency counter value
			else if(sentinel->isEmergencyModeAllowed())
			{
				persistFile->setEmergencyCounter(2);
				emergencyCntReset = true;
			}
			else
			{
				if(emergencyCounter)
				{
					persistFile->setEmergencyCounter(emergencyCounter);
				}
				else
				{
					persistFile->setEmergencyCounter(0);
				}
			}
			retnCode = testLkf->removeAllTestLksTemp();
			if(retnCode != ACS_LM_RC_OK)
			{
				retnCode = ACS_LM_RC_OK;
				DEBUG("%s","handleLkInstViaIMM() testLkf.removeAllTestLksTemp() failed");
			}

			persistFile->setLmMode(ACS_LM_MODE_GRACE, false);
			persistFile->setLmMode(ACS_LM_MODE_TEST, false);
			persistFile->setLmMode(ACS_LM_MODE_EMERGENCY, false);
			persistFile->setLmMode(ACS_LM_MODE_VIRGIN, false);   //TR HW50815
			//Whil LM in Maintenance mode, LKF installation is allowed.
			if(!((persistFile->getLmMode() & ACS_LM_MODE_MAINTENANCE) == ACS_LM_MODE_MAINTENANCE))		
			{
				persistFile->disableVirginMode();
			}

			if(persistFile->commit())
			{
				//Take backup of lservrc to system disk of both nodes
				if(sentinel->backup())
				{
					//clear all alarms
					eventHandler->clearAll();
					retnCode = testLkf->removeAllTestLksPerm();
					if(retnCode != ACS_LM_RC_OK)
					{
						retnCode = ACS_LM_RC_OK;
						DEBUG("%s","handleLkInstViaIMM() testLkf.removeAllTestLksPerm() failed");
					}
					if(emergencyCntReset)
					{
						eventHandler->ceaseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_EM_STATE_UNAVAIL);

						//Emergency State activation lock reset event
						ACS_LM_EventHandler::LmEvent emStateActLockReset = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_EM_STATE_LOCK_RESET-ACS_LM_EVENT_ID_BASE];
						ACS_LM_EventHandler::reportEvent(emStateActLockReset);
					}
					else
					{
						if(!emergencyCounter)
						{
							eventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_EM_STATE_UNAVAIL);
						}
					}
					sentinel->finishInstallation(true, aNodeInitialized, bNodeInitialized);
					//New License Key File installed
					ACS_LM_EventHandler::LmEvent newLicKeyFileInstall = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_NEW_LK_INSTALL-ACS_LM_EVENT_ID_BASE];
					ACS_LM_EventHandler::reportEvent(newLicKeyFileInstall);

					eventHandler->ceaseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_LK_FILE_MISS);

					//CNI55_4_578::Cease all alarm incase of new LKF installation
					eventHandler->clearAll();
					eventHandler->verifyAndRaiseAlarm(lkList1);
					retnCode = ACS_LM_LkSender::sendLks(ACS_LM_SENDER_LKINST, sentinel, persistFile, eventHandler);
					DEBUG("handleLkInstViaIMM(): retnCode = %d",retnCode);
					if( ACS_LM_Common::deleteFile(lservPath)== false)
					{
						DEBUG("handleLkInstViaIMM():() DeleteFile() failed, lservPath = %s ",lservPath.c_str());
					}
					retnCode = ACS_LM_RC_OK;
				}
				else
				{
					testLkf->restoreAllTestLks();
					persistFile->reLoad(emergencyCounter, lkfChecksum, mode, isVirginModeActive, aNodeInitialized, bNodeInitialized);
					persistFile->commit();
					//CNI30_4_1719(TR NO HN66027/HN40528)
					sentinel->finishInstallation(false, aNodeInitialized, bNodeInitialized,lkfChecksum);
					DEBUG("handleLkInstViaIMM() sentinel->backup() failed retnCode %d= ",retnCode);
				}
			}
			else
			{
				testLkf->restoreAllTestLks();
				persistFile->reLoad(emergencyCounter, lkfChecksum, mode, isVirginModeActive, aNodeInitializedStatus, bNodeInitializedStatus);
				//CNI30_4_1719(TR NO HN66027/HN40528)
				sentinel->finishInstallation(false, aNodeInitialized, bNodeInitialized,lkfChecksum);
				DEBUG("handleLkInstViaIMM() persistFile->commit() failed retnCode %d= ",retnCode);
			}
		}
		else
		{
			testLkf->restoreAllTestLks();
			persistFile->reLoad(emergencyCounter, lkfChecksum, mode, isVirginModeActive, aNodeInitializedStatus, bNodeInitializedStatus);
			//CNI30_4_1719(TR NO HN66027/HN40528)
			sentinel->finishInstallation(false, aNodeInitialized, bNodeInitialized,lkfChecksum);
			DEBUG(" handleLkInstViaIMM(): retnCode = %d",retnCode);
			//Raise an Alarm, when trying to install invalid/corrupted LKF during LM in Maintenance mode.
		}
		for(std::list<LkData*>::iterator itr = lkList1.begin(); itr != lkList1.end(); ++itr)
		{
			delete (*itr);
		}
		lkList1.clear();
		//LMCleanup start
		for(std::list<LkData*>::iterator itrd = discLKList.begin(); itrd != discLKList.end(); ++itrd)
		{
			delete (*itrd);
		}
		discLKList.clear();
		for(std::list<LkData*>::iterator itrc = connLKList.begin(); itrc != connLKList.end(); ++itrc)
		{
			delete (*itrc);
		}
		connLKList.clear();
	//LMCleanup end
	}
	else /* INSTALLING LKF FAILED */
	{
		testLkf->restoreAllTestLks();
		persistFile->reLoad(emergencyCounter, lkfChecksum, mode, isVirginModeActive, aNodeInitializedStatus, bNodeInitializedStatus);
		persistFile->commit();
		//CNI30_4_1719(TR NO HN66027/HN40528)
		sentinel->finishInstallation(false, aNodeInitialized, bNodeInitialized,lkfChecksum);
		INFO(" handleLkInstViaIMM(): retnCode = %d",retnCode);
	}
	return retnCode;

}//end of handleLkInstViaIMM

/*=================================================================
	ROUTINE: handleLkLs
=================================================================== */
void ACS_LM_ClientHandler::handleLkLs(ACS_LM_CmdServer& cmdServer,const ACS_LM_Cmd& cmdReceived)
{
	DEBUG("%s","In ACS_LM_ClientHandler::handleLkLs()");
	ACS_LM_AppExitCode retnCode = ACS_LM_RC_OK;
	//reload lmdata in case of lmdata lock
	handleLmDataLock();
	ACE_UINT32 daysToExpire = 0;
	std::list<std::string> argList = cmdReceived.commandArguments();
	if((int)argList.size() > 1)
	{
		std::list<std::string>::iterator itr;
		for(itr = argList.begin(); itr != argList.end(); ++itr)
		{
			if(!ACE_OS::strcmp((*itr).c_str(),"-e"))
			{
				++itr;
				daysToExpire  = ACE_OS::atoi((*itr).c_str());
				break;
			}
		}
	}
	std::list<LkData*> lklist;
	if(persistFile->getLkfChecksum() != 0)
	{
		if(sentinel->getAllLks(lklist, daysToExpire))
		{	
			ACE_UINT32 totalLks = (ACE_UINT32)lklist.size();
			INFO("handleLkLs(): totalLks = %d ",totalLks);
			if(totalLks == 0)
			{
				DEBUG("handleLkLs(): retnCode = ",retnCode);
				ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
				cmdServer.send(cmdToSend);
			}
			else
			{
				retnCode = ACS_LM_RC_MOREDATA;
				ACE_UINT16 i=0;
				for(std::list<LkData*>::iterator itr=lklist.begin(); itr != lklist.end(); ++itr)
				{
					char** resultantChar = new char*[5];

					resultantChar[0] = new char[(int)(*itr)->lkId.size()+1];
					ACE_OS::strncpy(resultantChar[0], const_cast<char*>((*itr)->lkId.c_str()), (int)(*itr)->lkId.size()+1);

					resultantChar[1] = new char[(int)(*itr)->startDate.size()+1];
					ACE_OS::strncpy(resultantChar[1], const_cast<char*>((*itr)->startDate.c_str()), (int)(*itr)->startDate.size()+1);

					resultantChar[2] = new char[(int)(*itr)->endDate.size()+1];
					ACE_OS::strncpy(resultantChar[2], const_cast<char*>((*itr)->endDate.c_str()), (int)(*itr)->endDate.size()+1);

					char chValue[11] = {0};
					ACE_OS::itoa((*itr)->value, chValue, 10);
					resultantChar[3] = new char[(int)strlen(chValue)+1];
					ACE_OS::strncpy(resultantChar[3], const_cast<char*>(chValue), (int)strlen(chValue)+1);

					resultantChar[4] = new char[(int)(*itr)->vendorInfo.size()+1];
					ACE_OS::strncpy(resultantChar[4], const_cast<char*>((*itr)->vendorInfo.c_str()), (int)((*itr)->vendorInfo.size()+1));

					if(i++ == totalLks)
					{
						retnCode = ACS_LM_RC_OK;
					}

					ACS_LM_Cmd cmdToSend(retnCode, 5, resultantChar);
					cmdServer.send(cmdToSend);

					for(int j=0;j<5;++j)
					{
						delete[] resultantChar[j];
					}
					delete[] resultantChar;
				}
			}
		}	
		for(std::list<LkData*>::iterator itr = lklist.begin(); itr != lklist.end(); ++itr)
		{
			delete (*itr);
		}
		lklist.clear();
	}
	else
	{
		ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
		cmdServer.send(cmdToSend);
		DEBUG("%s","handleLkLs() LM IN GRACE MODE");
	}
}//end of handleLkLs
/*=================================================================
	ROUTINE: handleLkEmStartViaIMM
=================================================================== */
ACS_LM_AppExitCode ACS_LM_ClientHandler::handleLkEmStartViaIMM()
{
	INFO("%s","In ACS_LM_ClientHandler::handleLkEmStartViaIMM()");
	ACS_LM_AppExitCode retnCode = ACS_LM_RC_OK;
	//reload lmdata in case of lmdata lock
	handleLmDataLock();
	if((persistFile->getLmMode() & ACS_LM_MODE_EMERGENCY) != ACS_LM_MODE_EMERGENCY)
	{
		int emCount = persistFile->getEmergencyCounter();
		if((emCount > 0))
		{
			persistFile->setEmergencyCounter(--emCount);
			ACS_LM_EventHandler::LmEvent emStateAct = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_EM_MODE_ACT-ACS_LM_EVENT_ID_BASE];
			ACS_LM_EventHandler::reportEvent(emStateAct);

			persistFile->setLmMode(ACS_LM_MODE_EMERGENCY, true);

			if(!persistFile->commit())
			{
				persistFile->setLmMode(ACS_LM_MODE_EMERGENCY, false);
				persistFile->setEmergencyCounter(emCount++);
				ERROR("%s","handleLkEmStart() persistFile->commit() failed");
				retnCode = ACS_LM_RC_NOK;
			}

			retnCode = ACS_LM_LkSender::sendLks(ACS_LM_SENDER_EMSTART, sentinel, persistFile, eventHandler);
			DEBUG("handleLkEmStartViaIMM(): retnCode = %d",retnCode);

			if(emCount == 0)
			{
				eventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_EM_STATE_UNAVAIL);
			}
		}
		else
		{
			retnCode = ACS_LM_RC_EMERGENCYMODENOTALLOWED;
			DEBUG("handleLkEmStart(): retnCode = %d,emCount = %d",retnCode,emCount);
		}
	}
	else
	{
		retnCode = ACS_LM_RC_EMERGENCYMODEALREADYACTIVATED;
		DEBUG("handleLkEmStart(): retnCode = %d",retnCode);
	}
	return retnCode;
}//end of handleLkEmStartViaIMM
/* LM Maintenance Mode */
/*=================================================================
        ROUTINE: handleLkMaintenanceStartViaIMM 
=================================================================== */
/*=================================================================
Bydefault, Maintenance mode is in Lock State
aMaintananceFlag = 0 -> LOCKED
aMaintananceFlag = 1 -> UNLOCKED
=================================================================== */
ACS_LM_AppExitCode ACS_LM_ClientHandler::handleLkMaintenanceStartViaIMM(bool aMaintananceFlag)
{
        INFO("\n %s..%d","In ACS_LM_ClientHandler::handleLkMaintenanceStartViaIMM()",aMaintananceFlag ) ;
        ACS_LM_AppExitCode retnCode = ACS_LM_RC_OK;

        //reload lmdata in case of lmdata lock
        handleLmDataLock();
	
	// To store values of persistent in case commit fails
	int emergencyCounter = persistFile->getEmergencyCounter();
	ACE_UINT32 lkfChecksum = persistFile->getLkfChecksum();
	int mode = persistFile->getLmMode();
	bool isVirginModeActive = persistFile->isLmInVirginMode();
	bool aNodeInitializedStatus, bNodeInitializedStatus;
	persistFile->updateInitializedNodeStatus(aNodeInitializedStatus, bNodeInitializedStatus);
	DEBUG("%s,licenseMode=%d","ACS_LM_ClientHandler::handleLkMaintenanceStartViaIMM()",mode);

	switch ( aMaintananceFlag )
	{
	case MAINTEMODE_UNLOCKED:
	{
		/* Maintenance mode is in Lock state. Now need to make it to Unlock. */
		if((persistFile->getLmMode() & ACS_LM_MODE_MAINTENANCE) != ACS_LM_MODE_MAINTENANCE)
		{
			DEBUG("%s", "ACS_LM_ClientHandler::handleLkMaintenanceStartViaIMM() begin Maintenance mode Unlock handling");
			/* If LM mode is in either GRACE or TEST or EMERGENCY mode, then do not set activate Maintenance mode.*/
                        ACE_UINT8 currentLmMode;
                        currentLmMode = persistFile->getLmMode();
                        if( !(((currentLmMode & ACS_LM_MODE_GRACE ) == ACS_LM_MODE_GRACE) ||
                             ((currentLmMode & ACS_LM_MODE_EMERGENCY) == ACS_LM_MODE_EMERGENCY) ||
                             ((currentLmMode & ACS_LM_MODE_TEST) == ACS_LM_MODE_TEST)) )
                        {
                                graceOnInMaintenance = false;
                                persistFile->setLmMode(ACS_LM_MODE_MAINTENANCE, true);   // True -> Unlock
                                if(!persistFile->commit())
                                {
                                        INFO("%s","In ACS_LM_ClientHandler::handleLkMaintenanceStartViaIMM() commit failed");
                                        /* set maintenance mode to false if persistent file commit is not success    false -> Lock */
                                        //persistFile->setLmMode(ACS_LM_MODE_MAINTENANCE, false);
                                        retnCode = ACS_LM_RC_NOK;
                                }
                                else
                                {
                                        /* Raise the Maintenance mode activate event & Observation alarm if persistnt file commit is success */
                                        INFO("%s","In ACS_LM_ClientHandler::handleLkMaintenanceStartViaIMM() commit success");
                                        ACS_LM_EventHandler::LmEvent maintenanceModeAct = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_MAINTENANCE_MODE_ACT-ACS_LM_EVENT_ID_BASE];
                                        ACS_LM_EventHandler::reportEvent(maintenanceModeAct);
                                        eventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_MAINTENANCE_MODE);
				}
			}
                        else
                        {
                                retnCode = ACS_LM_RC_MAINTENANCEMODENOTALLOWED;
                                DEBUG("%s", "handleLkMaintenanceStartViaIMM() MAINTENANCEMODE not to be activated when LM in either GRACE or TEST or EMERGENCY mode");
                        }
                 }
                else
                 {
                        DEBUG("%s","handleLkMaintenanceStartViaIMM() MAINTENANCEMODE ALREADY UNLOCKED");
                        retnCode = ACS_LM_RC_MAINTENANCEMODEALREADYACTIVATED;   //ACS_LM_RC_MAINTENANCEMODEALREADYDEACTIVATED;
                 }
	}	
	break;
	case MAINTEMODE_LOCKED:
	{
		/* Maintenance mode is in Unlock state. Now need to make it to lock. */
		if((persistFile->getLmMode() & ACS_LM_MODE_MAINTENANCE) == ACS_LM_MODE_MAINTENANCE)
		{
			DEBUG("%s", "ACS_LM_ClientHandler::handleLkMaintenanceStartViaIMM() begin Maintenance mode Lock handling");
 			persistFile->setLmMode(ACS_LM_MODE_MAINTENANCE, false);  // false -> Lock
                        if(!persistFile->commit())
                        {
                                retnCode = ACS_LM_RC_NOK;
				INFO("%s","In ACS_LM_ClientHandler::handleLkMaintenanceStartViaIMM() commit failed");
                        }
                        else
                        {
                               	/* Raise the Maintenance mode deactivate event & cease Observation alarm */ 
                               	ACS_LM_EventHandler::LmEvent maintenanceModeDeaAct = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_MAINTENANCE_MODE_DEACT-ACS_LM_EVENT_ID_BASE];
                               	ACS_LM_EventHandler::reportEvent(maintenanceModeDeaAct);
				eventHandler->ceaseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_MAINTENANCE_MODE);

				//Check conditions to start Grace mode. If satisfy, then set LM mode to Grace mode.
			        INFO("acs_lm_clienthandler.cpp() : fpChangeNodeName %d..fpChangeClusterIP %d..graceOnInMaintenance=%d\n",fpChangeNodeName, 														fpChangeClusterIP,graceOnInMaintenance);
			 	if( fpChangeNodeName || fpChangeClusterIP || graceOnInMaintenance  )
				{
					INFO("%s\n", "FingerPrint/LKF corrupted. So setting to Grace mode");
					persistFile->setLmMode(ACS_LM_MODE_GRACE, true);
					if(persistFile->commit())
					{	
						eventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_LK_FILE_MISS);
						eventHandler->clearAll(); //LM set to Grace mode. So clear all A2/A3 alarms. 
					}
					graceOnInMaintenance = false;
				}
			}
		 }
		else
		{
			DEBUG("%s","handleLkMaintenanceStartViaIMM() MAINTENANCEMODE ALREADY LOCKED");
			retnCode = ACS_LM_RC_MAINTENANCEMODEALREADYDEACTIVATED;
		}
         }
	break;
	default:
			DEBUG("%s","handleLkMaintenanceStartViaIMM() MAINTENANCEMODE Switch Default case");
	break;
       }
	//send LK's
	if ( retnCode == ACS_LM_RC_OK)
	{
		ACE_thread_t threadId;
	        const ACE_TCHAR * pthreadName1 = "lkSenderDuringMMThreadFunc";

		//spwan thread;
		hThread = ACE_Thread_Manager::instance()->spawn(&sendLKsThreadFunc,
    		(void*)this ,
    		THR_NEW_LWP | THR_DETACHED | THR_INHERIT_SCHED,
    		&threadId,
    		0,
    		ACE_DEFAULT_THREAD_PRIORITY,
    		-1,
    		0,
    		ACE_DEFAULT_THREAD_STACKSIZE,
    		&pthreadName1);	
	}
	else
	{
		DEBUG("%s", "ACS_LM_ClientHandler::handleLkMaintenanceStartViaIMM() Return code is not OK. So reverting the changes");
		persistFile->setMaintenanceUnlockToLock();	
		persistFile->reLoad(emergencyCounter, lkfChecksum, mode, isVirginModeActive, aNodeInitializedStatus, bNodeInitializedStatus);
		persistFile->commit();
	}
	
	
	DEBUG("%s, %d\n","handleLkMaintenanceStartViaIMM() leaving",retnCode);
        return retnCode;
}//end of handleLkMaintenanceStartViaIMM

/*=================================================================
        ROUTINE: sendLKsThreadFunc
=================================================================== */
ACE_THR_FUNC_RETURN ACS_LM_ClientHandler::sendLKsThreadFunc(void* args)
{
        INFO("%s","In ACS_LM_ClientHandler::sendLKsThreadFunc() Entering");
	return reinterpret_cast<void*>(ACS_LM_ClientHandler::sendLKsThread(args));
        INFO("%s","In ACS_LM_ClientHandler::sendLKsThreadFunc() Leaving");
}

/*=================================================================
        ROUTINE: sendLKsThread 
=================================================================== */
ACE_UINT64 ACS_LM_ClientHandler::sendLKsThread(void* pArg)
{
        INFO("%s","In ACS_LM_ClientHandler::sendLKsThread() Entering");
        ACS_LM_ClientHandler* lkSender = (ACS_LM_ClientHandler*) pArg;

	DEBUG("%s", "ACS_LM_ClientHandler::sendLKsThread() sending Maintenance Mode LK's");
	(void)ACS_LM_LkSender::sendLks(ACS_LM_SENDER_MAINTENANCE, lkSender->sentinel,lkSender->persistFile,lkSender->eventHandler);
        INFO("%s","In ACS_LM_ClientHandler::sendLKsThread() Leaving");
	return 0;
}//end of sendLKsThread

/*=================================================================
        ROUTINE: handleLkExportLKFViaIMM 
=================================================================== */
/*===================================================================================================================
When this action invoked,
1.Checkes the 'lservrc' file in primary path is tampered or not. If file is safe, go to step 3.
2.If found tampered, do check the 'lservrc' file in secondary path is tampered or not. If file is safe, go to step 3.
3.copy the LKF from source to the destination folder.
4.If file tampered in both paths, throw error and exit.
Source folder:  Primary Path --> "/storage/system/config/ACS-LM/lservrc"
                Secondary Path --> "/cluster/etc/ap/acs/lm/lservrc"
Destination folder: "/data/opt/ap/internal_root/license_file/"
File format in destination folder: fingerprint_lkf.
This action would not allowed when LM is in Virgin and Grace modes.
====================================================================================================================== */
ACS_LM_AppExitCode ACS_LM_ClientHandler::handleLkExportLKFViaIMM( std::string nodeFingerPrint)
{
	DEBUG("%s","handleLkExportLKFViaIMM() Entering");
        ACS_LM_AppExitCode retnCode = ACS_LM_RC_OK;
	
	if((!persistFile->isLmInVirginMode()) && ((persistFile->getLmMode() & ACS_LM_MODE_GRACE) != ACS_LM_MODE_GRACE))
	{
		ACE_HANDLE src;
		ACE_HANDLE dest;
		ssize_t bytesToBeWritten;
		ssize_t bytesWrote;
		ACE_stat stat_buf;  /* hold information about input file */
		std::string srcPath;

		ACE_UINT32 lkfCheckSum = persistFile->getLkfChecksum();
		ACE_UINT32 lkfPPCheckSum = sentinel->getCheckSum();
		ACE_UINT32 lkfSPCheckSum = sentinel->getCheckSumBackup();
		DEBUG("ACS_LM_ClientHandler::handleLkExportLKFViaIMM()lkfCheckSum= %d",lkfCheckSum);
		DEBUG("ACS_LM_ClientHandler::handleLkExportLKFViaIMM()lkfCheckSum from Primary path =%d",lkfPPCheckSum);
		DEBUG("ACS_LM_ClientHandler::handleLkExportLKFViaIMM()lkfCheckSum from Secondary path = %d",lkfSPCheckSum);

		if ( lkfCheckSum == lkfPPCheckSum )
		{
			/* Fetching Source: Primary Path */
			srcPath = LM_LKF_PRIMARY_PATH;
		}
		else if ( lkfCheckSum == lkfSPCheckSum )
		{
			/* Fetching Source: Secondary Path */
			srcPath = LM_LKF_BACKUP_PATH;
		}
		else
		{
			DEBUG("ACS_LM_ClientHandler::handleLkExportLKFViaIMM() %s", "file tampered in both paths");
			retnCode = ACS_LM_RC_SRC_NOFILE_EXPORTLKFNOTALLOWED; 
			return retnCode;
		}


		/* Fetching Destination Path */
		string licenseKeyFilePath("");
		if(ACS_LM_Common::getLicenseKeyFilePath(licenseKeyFilePath) == false)
		{
			ERROR("ACS_LM_ClientHandler::handleLkExportLKFViaIMM() %s","Error occurred while getting license key file path");
			retnCode = ACS_LM_RC_EXPORTNOTOK; 
			return retnCode;
		}
		if(nodeFingerPrint.find(",") != string::npos)		// Dual Stack - Decide which FP to Export LKF with
			getFPDualStackLKFExport(srcPath,nodeFingerPrint);
		DEBUG("ACS_LM_ClientHandler::handleLkExportLKFViaIMM() - Fingerprint to be used in LKF name [%s]", nodeFingerPrint.c_str() );
		if(nodeFingerPrint.find(":") != string::npos)
			std::replace(nodeFingerPrint.begin(),nodeFingerPrint.end(),':','_');
		std::string destPath = "";
		std::string backupLKFName = "";

		destPath += licenseKeyFilePath;			/*   "/data/opt/ap/internal_root/license_file"    */
		destPath += "/";				/*   "/data/opt/ap/internal_root/license_file/"  */
		backupLKFName = nodeFingerPrint + "_lkf";	/*   "fingerPrint_lkf"	*/	
		destPath += backupLKFName;			/*   "/data/opt/ap/internal_root/license_file/fingerprint_lkf"	*/

		DEBUG("ACS_LM_ClientHandler::handleLkExportLKFViaIMM() the Source Path is %s", srcPath.c_str() );
		DEBUG("ACS_LM_ClientHandler::handleLkExportLKFViaIMM() the Destination Path is %s", destPath.c_str() );

		if ((src = ACE_OS::open(srcPath.c_str(), O_RDONLY)) == -1)
		{
			INFO("ACS_LM_ClientHandler::handleLkExportLKFViaIMM() Cant open the file SrcPath- %s",srcPath.c_str());
			retnCode = ACS_LM_RC_EXPORTNOTOK;
			return retnCode;
		}

		ACE_OS::fstat(src, &stat_buf);
		ACE_TCHAR buf[stat_buf.st_size];

		bytesToBeWritten = ACE_OS::read(src, buf, stat_buf.st_size);

		INFO("ACS_LM_ClientHandler::handleLkExportLKFViaIMM() bytesToBeWritten from SrcPath to DesPath- %d",bytesToBeWritten);

		if ((dest = ACE_OS::open(destPath.c_str(), O_WRONLY|O_CREAT|O_TRUNC)) == -1)
		{
			ERROR("ACS_LM_ClientHandler::handleLkExportLKFViaIMM() %s","Error occured while creating the file in destination path");
			ACE_OS::close(src);
			retnCode = ACS_LM_RC_DEST_NOTWRITE_EXPORTLKFNOTALLOWED;
			return retnCode;
		}
		if ( bytesToBeWritten != -1)
		{
			bytesWrote = ACE_OS::write(dest, buf, bytesToBeWritten);

			ACE_OS::close(dest);
			ACE_OS::close(src);
			if(bytesWrote != bytesToBeWritten)
			{
				ERROR("ACS_LM_ClientHandler::handleLkExportLKFViaIMM()  %s","Error occured while writing the file in destination path");
				retnCode = ACS_LM_RC_DEST_NOTWRITE_EXPORTLKFNOTALLOWED;
				return retnCode;
			}
		}
	}
	else
	{
		if(persistFile->isLmInVirginMode())
		{
			retnCode = ACS_LM_RC_VIRGIN_EXPORTLKFNOTALLOWED;
                	DEBUG("%s", "ACS_LM_ClientHandler::handleLkExportLKFViaIMM() exportLicenseKeyFile action cannot be invoked when LM in VIRGIN mode");	
		}	
		else if ((persistFile->getLmMode() & ACS_LM_MODE_GRACE) == ACS_LM_MODE_GRACE)
		{
			retnCode = ACS_LM_RC_GRACE_EXPORTLKFNOTALLOWED;
                	DEBUG("%s", "ACS_LM_ClientHandler::handleLkExportLKFViaIMM() exportLicenseKeyFile action cannot be invoked when LM in GRACE mode");
		}
	}
	/* Export file - House keeping: Exported file should be deleted after 30 days from the file exported date.
	   Calculate exported file expiry date and store it into persist file. */
	if(retnCode==ACS_LM_RC_OK)    
	{
		persistFile->calculateExportFileExpiryDate();
		if(persistFile->commit())
		{
			DEBUG("%s","handleLkExportLKFViaIMM()  Commit() for exported file date successful");
		}
		else
		{
			ERROR("%s","handleLkExportLKFViaIMM() Commit() for exported file date failed");
		}
	}  
	DEBUG("%s","handleLkExportLKFViaIMM() Leaving");
	return retnCode;
}

/*=================================================================
	ROUTINE: handleLkMapLs
=================================================================== */
void ACS_LM_ClientHandler::handleLkMapLs(ACS_LM_CmdServer& cmdServer)
{	
	DEBUG("%s","handleLkMapLs() Entering");
	INFO("%s","In ACS_LM_ClientHandler::handleLkMapLs()");
	ACS_LM_AppExitCode retnCode = ACS_LM_RC_OK;
	//reload lmdata in case of lmdata lock
	handleLmDataLock();

	if(persistFile->getLkfChecksum() != 0)
	{
		std::list<LkData*> argList;
		if(sentinel->isTestModeAllowed())
		{
			DEBUG("%s","handleLkMapLs():Inside isTestModeAllowed Entering");
			(void)testLkf->getTestLks(argList);
			for(std::list<LkData*>::iterator itr=argList.begin();
					itr != argList.end(); ++itr)
			{
				(*itr)->startDate = "-";
				(*itr)->endDate = "-";
				(*itr)->vendorInfo = "-";
			}
			DEBUG("%s","handleLkMapLs():Inside isTestModeAllowed Leaving");
		}
		else
		{
			INFO("%s","handleLkMapLs() TEST MODE KEY EXPIRED");
		}

		//	(void)sentinel->getAllLks(argList);
		(void)sentinel->getAllLks(argList, 0, true);
		int totalLks = (int)argList.size();
		DEBUG("totalLks = %d ",totalLks);
		if(totalLks == 0)
		{
			INFO("handleLkMapLs(): totalLks == 0 .retnCode = %d",retnCode);
			ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
			cmdServer.send(cmdToSend);
		}
		else
		{
			retnCode = ACS_LM_RC_MOREDATA;
			int i=0;
			for(std::list<LkData*>::iterator itr=argList.begin();
					itr != argList.end(); ++itr)
			{
				INFO("handleLkMapLs(): totalLks= %d ,i = %d" ,totalLks,i);
				LkData* lkData = (*itr);
				INFO("lkData->status = %d\n",lkData->status);
				INFO("lkData->value = %d\n",lkData->value);
				INFO("lkData->setName = %s\n",lkData->setName.c_str());
				INFO("lkData->paramName = %s\n",lkData->paramName.c_str());
				INFO("lkData->vendorInfo = %s\n",lkData->vendorInfo.c_str());
				char** resultantChar = new char*[7];

				resultantChar[0] = new char[(int)lkData->lkId.size()+1];
				ACE_OS::strncpy(resultantChar[0], const_cast<char*>(lkData->lkId.c_str()), (int)lkData->lkId.size()+1);

				resultantChar[1] = new char[(int)lkData->paramName.size()+1];
				ACE_OS::strncpy(resultantChar[1], const_cast<char*>(lkData->paramName.c_str()), (int)lkData->paramName.size()+1);
				resultantChar[2] = new char[(int)lkData->setName.size()+1];
				ACE_OS::strncpy(resultantChar[2], const_cast<char*>(lkData->setName.c_str()), (int)lkData->setName.size()+1);
				
				resultantChar[3] = new char[(int)lkData->startDate.size()+1];
				ACE_OS::strncpy(resultantChar[3], const_cast<char*>(lkData->startDate.c_str()), (int)lkData->startDate.size()+1);

				resultantChar[4] = new char[(int)lkData->endDate.size()+1];
				ACE_OS::strncpy(resultantChar[4], const_cast<char*>(lkData->endDate.c_str()), (int)lkData->endDate.size()+1);

				char chValue[11]={0};
				ACE_OS::itoa(lkData->value, chValue, 10);
				resultantChar[5] = new char[6];
				ACE_OS::strncpy(resultantChar[5], chValue, 6);

				resultantChar[6] = new char[(int)lkData->vendorInfo.size()+1];
				ACE_OS::strncpy(resultantChar[6], const_cast<char*>(lkData->vendorInfo.c_str()), (int)lkData->vendorInfo.size()+1);

				if(++i == totalLks)
				{
					retnCode = ACS_LM_RC_OK;
				}

				DEBUG("handleLkMapLs(): retnCode = %d ",retnCode);
				ACS_LM_Cmd cmdToSend(retnCode, 7, resultantChar);
				cmdServer.send(cmdToSend);
				for(int j=0;j<7;++j)
				{
					delete[] resultantChar[j];
				}
				delete[] resultantChar;
			}
		}

		for(std::list<LkData*>::iterator itr = argList.begin(); itr != argList.end(); ++itr)
		{
			delete (*itr);
		}
		argList.clear();

	}
	else
	{		
		ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
		cmdServer.send(cmdToSend);
		DEBUG("%s","handleLkMapLs() in else....LM IN GRACE MODE");
	}
	DEBUG("%s","handleLkMapLs() leaving");
}//end of handleLkMapLs
/*=================================================================
	ROUTINE: handleTestLkDeAct
=================================================================== */
void ACS_LM_ClientHandler::handleTestLkDeAct(ACS_LM_CmdServer& cmdServer)
{
	DEBUG("In ACS_LM_ClientHandler::handleTestLkDeAct() %s","Entering");
	//reload lmdata in case of lmdata lock
	handleLmDataLock();
	ACS_LM_AppExitCode retnCode = ACS_LM_RC_OK;
	/* Test mode should be restricted during MM */
	if((!persistFile->isLmInVirginMode()) && ((persistFile->getLmMode() & ACS_LM_MODE_GRACE) != ACS_LM_MODE_GRACE)
					      && ((persistFile->getLmMode() & ACS_LM_MODE_MAINTENANCE) != ACS_LM_MODE_MAINTENANCE))
	{
		if(sentinel->isTestModeAllowed())
		{
			retnCode = testLkf->removeAllTestLksTemp();
			ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
			cmdServer.send(cmdToSend);
			//	        ACS_LM_PrintTrace(ACS_LM_ClientHandler, "handleTestLkDeAct(): retnCode="<<retnCode);

			//make the command like an ordering command that doesn't wait
			//for the sending and state it in the man pages.
			retnCode = ACS_LM_LkSender::sendLks(ACS_LM_SENDER_TESTDEACT, sentinel, persistFile, eventHandler);
			INFO("ACS_LM_ClientHandler::handleTestLkDeAct() - LM Test Mode Allowed, retnCode = %d",retnCode);
		}
		else
		{
			retnCode = ACS_LM_RC_TESTMODENOTALLOWED;
			ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
			cmdServer.send(cmdToSend);
			INFO("ACS_LM_ClientHandler::handleTestLkDeAct() - LM Test Not Mode Allowed, retnCode = %d",retnCode);
		}
	}
	else
	{
		retnCode = ACS_LM_RC_TESTMODENOTALLOWED;
		ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
		cmdServer.send(cmdToSend);
		INFO("ACS_LM_ClientHandler::handleTestLkDeAct() - LM Test Not Mode Allowed, retnCode = %d",retnCode);
	}
}//end of handleTestLkDeAct
/*=================================================================
	ROUTINE: getEmergencyCount
=================================================================== */
unsigned int ACS_LM_ClientHandler::getEmergencyCount()
{
	return persistFile->getEmergencyCounter();
}//end of getEmergencyCount
/*=================================================================
	ROUTINE: getVirginModeStatus
=================================================================== */
bool ACS_LM_ClientHandler::getVirginModeStatus()
{
	return persistFile->isLmInVirginMode();
}//end of getVirginModeStatus
/*=================================================================
	ROUTINE: getEmergencyModeEndDate
=================================================================== */
ACE_INT64 ACS_LM_ClientHandler::getEmergencyModeEndDate()
{
	return persistFile->getEmergencyModeEndDate();
}//end of getEmergencyModeEndDate
/* LM Maintenance Mode */
/*=================================================================
        ROUTINE: getMaintenanceModeEndDate
=================================================================== */
ACE_INT64 ACS_LM_ClientHandler::getMaintenanceModeEndDate()
{
        return persistFile->getMaintenanceModeEndDate();
}//end of getMaintenanceModeEndDate
/*=================================================================
	ROUTINE: getLMMode
=================================================================== */
void ACS_LM_ClientHandler::getLMMode(bool & aGraceMode,bool & aEmergencyMode,bool & aTestMode, bool & aMaintenanceMode)
{
	aGraceMode = aTestMode = aEmergencyMode = aMaintenanceMode = false;
	if((persistFile->getLmMode() & ACS_LM_MODE_GRACE ) == ACS_LM_MODE_GRACE)
	{
		aGraceMode = true;
	}
	if((persistFile->getLmMode() & ACS_LM_MODE_EMERGENCY ) == ACS_LM_MODE_EMERGENCY)
	{
		aEmergencyMode = true;
	}
	if((persistFile->getLmMode() & ACS_LM_MODE_TEST ) == ACS_LM_MODE_TEST)
	{
		aTestMode = true;
	}
	/* LM Maintenance Mode */
        if((persistFile->getLmMode() & ACS_LM_MODE_MAINTENANCE ) == ACS_LM_MODE_MAINTENANCE)
        {
               aMaintenanceMode = true;
        }
}//end of getLMMode
/*=================================================================
	ROUTINE: handleShowLicense
=================================================================== */
void ACS_LM_ClientHandler::handleShowLicense(ACS_LM_CmdServer& cmdServer)
{    
	DEBUG("%s","In ACS_LM_ClientHandler::handleShowLicense()");
	ACS_LM_AppExitCode retnCode = ACS_LM_RC_OK;
	//reload lmdata in case of lmdata lock
	DEBUG("%s", "Inside handleShowLicense");
	handleLmDataLock();
	if((persistFile->getLkfChecksum() != 0) || ((persistFile->getLkfChecksum() == 0) && ((persistFile->getLmMode() & ACS_LM_MODE_GRACE ) == ACS_LM_MODE_GRACE)))
	{
		std::list<LkData*> argList;
		(void)sentinel->getAllLks(argList);
		int totalLks = (int)argList.size();
		if(totalLks == 0)
		{
			DEBUG("handleShowLicense(): retnCode = %d",retnCode);
			ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
			cmdServer.send(cmdToSend);
		}
		else
		{
			retnCode = ACS_LM_RC_MOREDATA;
			int i=0;
			for(std::list<LkData*>::iterator itr=argList.begin();
					itr != argList.end(); ++itr)
			{
				INFO("handleShowLicense(): totalLks= %d ,i = %d" ,totalLks,i);
				LkData* lkData = (*itr);
				ACE_TCHAR ** resultantChar = new char*[7];
				INFO("lkData->setName = %s\n",lkData->setName.c_str());
				INFO("lkData->paramName = %s\n",lkData->paramName.c_str());
				INFO("lkData->vendorInfo = %s\n",lkData->vendorInfo.c_str());

				resultantChar[0] = new char[(int)lkData->lkId.size()+1];
				ACE_OS::strncpy(resultantChar[0], const_cast<char*>(lkData->lkId.c_str()), (int)lkData->lkId.size()+1);


				resultantChar[1] = new char[(int)lkData->paramName.size()+1];
				ACE_OS::strncpy(resultantChar[1], const_cast<char*>(lkData->paramName.c_str()), (int)lkData->paramName.size()+1);

				resultantChar[2] = new char[(int)lkData->setName.size()+1];
				ACE_OS::strncpy(resultantChar[2], const_cast<char*>(lkData->setName.c_str()), (int)lkData->setName.size()+1);

				resultantChar[3] = new char[(int)lkData->startDate.size()+1];
				ACE_OS::strncpy(resultantChar[3], const_cast<char*>(lkData->startDate.c_str()), (int)lkData->startDate.size()+1);

				resultantChar[4] = new char[(int)lkData->endDate.size()+1];
				ACE_OS::strncpy(resultantChar[4], const_cast<char*>(lkData->endDate.c_str()), (int)lkData->endDate.size()+1);

				char chValue[11]={0};
				ACE_OS::itoa(lkData->value, chValue, 10);
				resultantChar[5] = new char[6];
				ACE_OS::strncpy(resultantChar[5], chValue, 6);

				resultantChar[6] = new char[(int)lkData->vendorInfo.size()+1];
				ACE_OS::strncpy(resultantChar[6], const_cast<char*>(lkData->vendorInfo.c_str()), (int)lkData->vendorInfo.size()+1);

				if(++i == totalLks)
				{
					retnCode = ACS_LM_RC_OK;
				}

				INFO("handleShowLicense() : retnCode = %d",retnCode);
				ACS_LM_Cmd cmdToSend(retnCode, 7, resultantChar);
				cmdServer.send(cmdToSend);
				for(int j=0;j<7;++j)
				{
					delete[] resultantChar[j];
				}
				delete[] resultantChar;
			}        
			for(std::list<LkData*>::iterator itr = argList.begin(); itr != argList.end(); ++itr)
			{
				DEBUG("%s","deleting arglist");
				delete (*itr);
			}
			argList.clear();
		}
	}
	else
	{
		ACS_LM_Cmd cmdToSend(retnCode, 0, NULL);
		cmdServer.send(cmdToSend);
		INFO("%s","handleShowLicense() LM IN GRACE MODE");
	}
}//end of handleShowLicense
/*=================================================================
	ROUTINE: handleLmDataLock
=================================================================== */
void ACS_LM_ClientHandler::handleLmDataLock()
{
	DEBUG("%s","In ACS_LM_ClientHandler::handleLmDataLock()-Entering");
	//	int emergencyCounter;
	if(ACS_LM_Common::getLmdataLockStatus()==true)
	{
		//        bool lmdataLockStatus = false;
		persistFile->reloadOrigPersistentData();
		if(ACS_LM_Common::getLmdataLockStatus()!=true)
		{
			bool aNodeInitialized, bNodeInitialized;
			persistFile->getInitializedNodeStatus(aNodeInitialized,bNodeInitialized);
			// get the data of lservrc:-
			ACE_UINT32 lkfCheckSum = persistFile->getLkfChecksum();
			sentinel->clean();
			bool validLkfFound = false;
			if(sentinel->init(aNodeInitialized, bNodeInitialized, false, lkfCheckSum))
			{
				validLkfFound = true;
				INFO("The validation of Lkf::init() is : " ,validLkfFound);
			}
			else
			{
				//CNI30_4_1761
				if(sentinel->initFromBackup(aNodeInitialized, bNodeInitialized, false, lkfCheckSum))
				{
					validLkfFound = true;
					INFO("The validation of Lkf::initFromBackup() is : " ,validLkfFound);
				}
			}

			std::list<LkData*> sentinelLkList;


			if((lkfCheckSum != 0) && (lkfCheckSum == sentinel->getCheckSum()) && validLkfFound)
			{
				int emergencyCounter;
				INFO(" getting all data validlkf : %d,lkfCheckSum :%d ",validLkfFound,lkfCheckSum);
				INFO("sentinel->getCheckSum(): %d",sentinel->getCheckSum());
				(void)sentinel->getValidLks(sentinelLkList);
				INFO("getvalid lks : " ,sentinelLkList.size());
				//handle alarms
				//LKF alarm
				eventHandler->verifyAndRaiseAlarm(sentinelLkList, true);              
				//emergency alarm
				emergencyCounter = persistFile->getEmergencyCounter();
				if(!emergencyCounter)
				{
					eventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_EM_STATE_UNAVAIL);					
				}
				else
				{
					eventHandler->ceaseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_EM_STATE_UNAVAIL);
				}

			}
		}
		else
		{
			INFO("%s","ACS_LM_ClientHandler::handleLmDataLock():Checking ACS_LM_Common::getLmdataLockStatus()== true ");
			int emergencyCounter = persistFile->getEmergencyCounter();
			if(emergencyCounter)//CNI30-4-1719
			{
				eventHandler->ceaseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_EM_STATE_UNAVAIL);
			}
		}

	}

}//end of handleLmDataLock

/* Dispaly Fingerprint in hashed format */
std::string ACS_LM_ClientHandler::getHashedFingerPrint()
{
	INFO("ACS_LM_ClientHandler::getHashedFingerPrint() %s","Entering");
	std::string hashedFingerPrint = sentinel->getFingerPrint();
	INFO("ACS_LM_ClientHandler::getHashedFingerPrint()...hashedFingerprint=%s",hashedFingerPrint.c_str());

	if(hashedFingerPrint.length() > 0)
	{
		hashedFingerPrint = hashedFingerPrint.substr(0, hashedFingerPrint.length()-1);
	}

	INFO("ACS_LM_ClientHandler::getHashedFingerPrint() %s","Leaving");

	return hashedFingerPrint;
}//end of getHashedFingerPrint

/* Fetch the hashed fingerprint from Sentinel*/
std::string ACS_LM_ClientHandler::getHashedFingerprint(bool isDualStack)
{
	INFO("ACS_LM_ClientHandler::getHashedFingerprint(%d) - Entering",isDualStack);
	std::string hashedFingerPrint = sentinel->getFingerprint(isDualStack);
	INFO("ACS_LM_ClientHandler::getHashedFingerprint()...hashedFingerprint=%s",hashedFingerPrint.c_str());
	INFO("ACS_LM_ClientHandler::getHashedFingerprint() %s","Leaving");
	return hashedFingerPrint;
}//end of getHashedFingerPrint

/* Decide which FP to use in filename for exporting LKF */
void ACS_LM_ClientHandler::getFPDualStackLKFExport(const std::string lkfPath, std::string &selectedFingerprint)
{
	DEBUG("ACS_LM_ClientHandler::getFPDualStackLKFExport() - %s","Entering");
	DEBUG("ACS_LM_ClientHandler::getFPDualStackLKFExport() - Dual stack FP is [%s]",selectedFingerprint.c_str());
	size_t posDelim1 = selectedFingerprint.find(",");
	if(posDelim1 != string::npos)
	{
		string hashedIPv4FP(""), hashedIPv6FP(""), IPv4FP(""), IPv6FP("");
		size_t FPlen = selectedFingerprint.length();
		IPv4FP = selectedFingerprint.substr(0,posDelim1);
		IPv6FP = selectedFingerprint.substr(posDelim1+1,FPlen - posDelim1);
		DEBUG("ACS_LM_ClientHandler::getFPDualStackLKFExport() - IPv4 FP [%s]  //  IPv6 FP [%s]",IPv4FP.c_str(),IPv6FP.c_str());
		string hashedFpFromLKF = ACS_LM_Sentinel::getHashedFingerprintFromLKF(lkfPath);
		DEBUG("ACS_LM_ClientHandler::getFPDualStackLKFExport() - Hashed FP from LKF is [%s]",hashedFpFromLKF.c_str());
		string hashedFpFromSentinel = sentinel->getFingerprint(true);
		DEBUG("ACS_LM_ClientHandler::getFPDualStackLKFExport() - Hashed FP from Sentinel is [%s]",hashedFpFromSentinel.c_str());

		size_t posDelim2 = hashedFpFromSentinel.find(",");
		if(posDelim2 != string::npos)
		{
			size_t hashedFPLen = hashedFpFromSentinel.length();
			hashedIPv4FP = hashedFpFromSentinel.substr(0,posDelim2);
			hashedIPv6FP = hashedFpFromSentinel.substr(posDelim2 +1 ,hashedFPLen - posDelim2);
			DEBUG("ACS_LM_ClientHandler::getFPDualStackLKFExport() - Hashed IPv4 FP [%s]  // Hashed IPv6 FP [%s]",hashedIPv4FP.c_str(),hashedIPv6FP.c_str());
			if(hashedFpFromLKF.compare(hashedIPv4FP) == 0)
				selectedFingerprint = IPv4FP;
			else if(hashedFpFromLKF.compare(hashedIPv6FP) == 0)
				selectedFingerprint = IPv6FP;
		}
		else
		{
			//IPV6 Spillover - Added Else part to handle IPv4 and IPv6 LKF.
                        size_t hashedFPLen = hashedFpFromSentinel.length();
                        hashedIPv4FP = hashedFpFromSentinel.substr(0,posDelim2);
                        hashedIPv6FP = hashedFpFromSentinel.substr(posDelim2 +1 ,hashedFPLen - posDelim2);
                        DEBUG("ACS_LM_ClientHandler::getFPDualStackLKFExport() - Hashed IPv4 FP [%s]  // Hashed IPv6 FP [%s]",hashedIPv4FP.c_str(),hashedIPv6FP.c_str());
                        if(hashedFpFromLKF.compare(hashedIPv4FP) == 0)
                                selectedFingerprint = IPv4FP;
                        else if(hashedFpFromLKF.compare(hashedIPv6FP) == 0)
                                selectedFingerprint = IPv6FP;
			else
                                selectedFingerprint = IPv6FP;

		}
	}
	DEBUG("ACS_LM_ClientHandler::getFPDualStackLKFExport() - Returning - Selected FP is [%s]",selectedFingerprint.c_str());
}
