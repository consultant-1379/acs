//******************************************************************************
//
//  NAME
//     acs_lm_lksender.cpp
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
#include "acs_lm_lksender.h"
#include "acs_lm_testlkf.h"
#include "acs_lm_tra.h"
#include "acs_lm_brfc_interface.h"
#include "acs_lm_server.h"
#include "acs_lm_defs.h"
extern bool lminstCmnd ;
//bool fpChangeNodeName;
//bool fpChangeClusterIP;

ACS_LM_JTP_Conversation* ACS_LM_LkSender::jtpConv = NULL;
//TR-HS41812 Senddata Mutex initialization. 
ACE_Recursive_Thread_Mutex ACS_LM_LkSender::senddataMutex;
/*=================================================================
	ROUTINE: sendLks
=================================================================== */
ACS_LM_AppExitCode ACS_LM_LkSender::sendLks(LkSenderType senderType, 
		ACS_LM_Sentinel* sentinel,
		ACS_LM_Persistent* persistent,
		ACS_LM_EventHandler* eventHandler,
		JtpNode* node)
{
	ACS_LM_AppExitCode retnCode = ACS_LM_RC_OK;

	ACS_LM_TestLkf* testLkf = persistent->testLkFile();

        ACS_LM_BrfImplementer brfImplementer;
        bool brfStatus = brfImplementer.getBrfStatus();
        bool graceModeDueToLmdataFlag1 = false;
        bool graceModeDueToLmdataFlag2 = false;

	std::list<LkData*> testLkList;
	std::list<LkData*> sentinelLkList;
	std::list<LkData*> discLklist;   //LMcleanup
	std::list<LkData*> connectionLKlist;   //LMcleanup
	bool isServiceStartup = false;
	bool lmdataCorrupted = false; // to check whether LMDATA file got corrutped/missed/locked in both primary and secondary paths.

	ACE_UINT32 lkfCheckSum = persistent->getLkfChecksum();
	ACE_UINT16 lmdataCheckSum = persistent->getLmdataChecksum();
	std::string persistentFile(LM_PERSISTENT_PRIMARY_PATH);
	ACE_UINT16 lmdataFileCheckSum = ACS_LM_Common::getLmdataMemoryCheckSum(persistentFile); //function will
		// calculate the checksum value of lmdata file.

	switch(senderType)
	{
	case ACS_LM_SENDER_SERVICE_STARTUP:
	{
		DEBUG("%s", "ACS_LM_SENDER_SERVICE_STARTUP");
		isServiceStartup = true;
		//break //Don't break here
	}
	case ACS_LM_SENDER_SCHEDULER:
	{
		//try reloading lmdata if its locked
		if(ACS_LM_Common::getLmdataLockStatus())
		{
			persistent->reloadOrigPersistentData();

		}
		//TODO remove the comments
		//Check Emergency State [ after 7 days elapses from emstart]
                if(((persistent->getLmMode() & ACS_LM_MODE_EMERGENCY) == ACS_LM_MODE_EMERGENCY))
                {
                        INFO("ACS_LM_LkSender::sendLks(): Emergency  Auto Cease befoe while shutDown status=%d",ACS_LM_Server::isShutdown);
                        while((brfStatus) && (ACS_LM_Server::isShutdown==false))
                        {
                                sleep(1);
                                brfStatus = brfImplementer.getBrfStatus();
                        }
                        if(!brfStatus)
                        {  
			        DEBUG("ACS_LM_LkSender::sendLks(): %s", "ACS_LM_SENDER_SCHEDULER if!brfStatus for emergency");
                                if (persistent->isEmergencyModeExpired()) 
				{
                                	ACS_LM_EventHandler::LmEvent emStateDeAct = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_EM_MODE_DEACT-ACS_LM_EVENT_ID_BASE];
                                	ACS_LM_EventHandler::reportEvent(emStateDeAct);
				}
                        }
                }
                /* LM Maintenance Mode */
                //LM Maintenance mode should be stopped 25 day's after it is invoked.
                if(((persistent->getLmMode() & ACS_LM_MODE_MAINTENANCE) == ACS_LM_MODE_MAINTENANCE))
                {
                        INFO ("ACS_LM_LkSender::sendLks(): Maintenance Auto Cease before while shutDown status=%d",ACS_LM_Server::isShutdown);
                        while((brfStatus) && (ACS_LM_Server::isShutdown==false))
                        {
                                sleep(1);
                                brfStatus = brfImplementer.getBrfStatus();
                        }
                        if(!brfStatus)
                        {       
				DEBUG("ACS_LM_LkSender::sendLks(): %s", "ACS_LM_SENDER_SCHEDULER if!brfStatus for maintenance");
				if (persistent->isMaintenanceModeExpired())
				{
					DEBUG("ACS_LM_LkSender::sendLks(): %s", "Maintenance mode is expired");
                                	ACS_LM_EventHandler::LmEvent maintenanceStateDeAct = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_MAINTENANCE_MODE_DEACT-ACS_LM_EVENT_ID_BASE];
                                	ACS_LM_EventHandler::reportEvent(maintenanceStateDeAct);
                                	eventHandler->ceaseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_MAINTENANCE_MODE);
                                	isFromAutoCease = false;
                                	if( fpChangeNodeName || fpChangeClusterIP || graceOnInMaintenance )
                                	{
                                        	eventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_LK_FILE_MISS);
						eventHandler->clearAll(); //LM set to Grace mode. So clear all A2/A3 alarms.
                                        	graceOnInMaintenance = false;
                                	}
				}
                        }
                }

		/* Export file - House keeping: exported file need to be deleted after 30 days from it's exported date */
                if( persistent->isExportLKFExpired())
                {
			/* Fetch the exported LKF containing path (i.e, "/data/opt/ap/internal_root/license_file/") */
			string licenseKeyFilePath("");
	                if((ACS_LM_Common::getLicenseKeyFilePath(licenseKeyFilePath)))
        	        {
                        	DEBUG("ACS_LM_LkSender::sendLks() Exported LKF path = %s", licenseKeyFilePath.c_str());
                		bool status = ACS_LM_Common::doFileDelete(licenseKeyFilePath);
                        	if(status == true)
                        	{
                                	DEBUG("ACS_LM_LkSender::sendLks() %s","All files deleted successfully")
                        	}
                        	else
                       	 	{
					retnCode = ACS_LM_RC_PHYFILEERROR;
                                	DEBUG("ACS_LM_LkSender::sendLks() %s","All files deletion failed")
                        	}
			}	
			else
			{
                                ERROR("ACS_LM_LkSender::sendLks() %s","Error occurred while getting License Key File path");
                                retnCode = ACS_LM_RC_INVALIDPATH;
                        }
                }

		//Check Test mode[deactivate after 7 days of lmtestact]
		if((persistent->getLmMode() & ACS_LM_MODE_TEST) == ACS_LM_MODE_TEST)
		{
			if(sentinel->isTestModeAllowed())
			{
				DEBUG("%s", "ACS_LM_SENDER_SCHEDULER");
				(void)testLkf->getTestLks(testLkList);
			}
			else
			{
				persistent->setLmMode(ACS_LM_MODE_TEST, false);
				ACS_LM_EventHandler::LmEvent testModeDeAct = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_TEST_MODE_DEACT-ACS_LM_EVENT_ID_BASE];
				ACS_LM_EventHandler::reportEvent(testModeDeAct);
			}
		}
		// start of TR HV60364
		if((lmdataCheckSum != lmdataFileCheckSum)||(lmdataCheckSum == 0 && lmdataFileCheckSum == 0))	//Fix for TR HY34394
			{
			DEBUG("lmdataCheckSum = %d lmdataFileCheckSum = %d ",lmdataCheckSum,lmdataFileCheckSum);
			if(persistent->restore())
			{
				lmdataFileCheckSum = ACS_LM_Common::getLmdataMemoryCheckSum(persistentFile);
				if(lmdataCheckSum != lmdataFileCheckSum)
				{
					graceModeDueToLmdataFlag1 = true;
					DEBUG("%s","graceModeDueToLmdataFlag1 is set to true");
				}
			}
		    else
		    {
		    	graceModeDueToLmdataFlag2 = true;
		    	DEBUG("%s","graceModeDueToLmdataFlag2 is set to true as persistent->restore() failed ");
		    }
			}
		if(graceModeDueToLmdataFlag1 || graceModeDueToLmdataFlag2)
		{
			persistent->setEmergencyCounter(0);
			eventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_EM_STATE_UNAVAIL);
			lmdataCorrupted = true;
			DEBUG("%s","ACS_LM_ALARM_EM_STATE_UNAVAIL alarm is raised due to lmdata corrupt/missing/Locked ");
		}
		// end of TR HV60364
		//TODO modify condition
		INFO("The value of  lkfCheckSum from persistent: %d",lkfCheckSum);
		INFO("sendlks() : fpChangeNodeName %d fpChangeClusterIP %d",fpChangeNodeName, fpChangeClusterIP);
		if( !fpChangeNodeName && !fpChangeClusterIP && !lmdataCorrupted )
		{
			if((lkfCheckSum != 0) && (lkfCheckSum == sentinel->getCheckSum()))
			{
				INFO("The value of  lkfCheckSum from sentinel->getCheckSum(): %d",sentinel->getCheckSum());
				(void)sentinel->getValidLks(sentinelLkList);
				eventHandler->verifyAndRaiseAlarm(sentinelLkList, isServiceStartup);
			}
			else
			{
				bool aNodeInitialized = false;
				bool bNodeInitialized = false;
				bool initialize = false;
				if(sentinel->restore())
				{
					sentinel->clean();
					initialize = sentinel->init(aNodeInitialized, bNodeInitialized, false, lkfCheckSum);
					if (initialize == 0)
					{
						persistent->setLmMode(ACS_LM_MODE_GRACE, true);
						INFO("%s","sendLks() sentinel->init() failed.Entering into Grace Mode");
						eventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_LK_FILE_MISS);
					}
				}
				else
				{
					//sentinel->deleteSentinelFile();
					if(!persistent->isLmInVirginMode())
					{
						eventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_LK_FILE_MISS);		//HK32313
					}
					persistent->setLmMode(ACS_LM_MODE_GRACE, true);
					//sentinel->init(aNodeInitialized, bNodeInitialized, false);
					ERROR("%s","sendLks() sentinel->restoreLkf() failed");
					DEBUG("%s","sendLks() Grace mode activated");
				}
			}
		}
		else
		{
			persistent->setLmMode(ACS_LM_MODE_GRACE, true);
			INFO("%s","sendLks() :  fingerprint changed or lmdata corrupted. Entering into Grace Mode");
			eventHandler->raiseAlarm(ACS_LM_EventHandler::ACS_LM_ALARM_LK_FILE_MISS);
			fpChangeNodeName = false;
			fpChangeClusterIP = false;
		}
		if(!persistent->commit())
		{
			retnCode = ACS_LM_RC_PHYFILEERROR;
			ERROR("%s","sendLks() persistent->commit() failed");
		}
		break;
	}
	case ACS_LM_SENDER_TESTACT:
	{
		DEBUG("%s", "ACS_LM_SENDER_TESTACT");
		if((persistent->getLmMode() & ACS_LM_MODE_TEST) != ACS_LM_MODE_TEST)
		{
			persistent->setLmMode(ACS_LM_MODE_TEST, true);
			if(!persistent->commit())
			{
				// set testmode to false if persstent file commit is not success
				persistent->setLmMode(ACS_LM_MODE_TEST, false);
				retnCode = ACS_LM_RC_PHYFILEERROR;
				ERROR("%s","sendLks() persistFile->commit() failed");
			}
			else
			{
				//raise the event if persistnt file commit is success
				ACS_LM_EventHandler::LmEvent testModeAct = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_TEST_MODE_ACT-ACS_LM_EVENT_ID_BASE];
				ACS_LM_EventHandler::reportEvent(testModeAct);
			}
		}
		else
		{
			INFO("%s","sendLks() TESTMODE ALREADY ACTIVATED");
		}
		if((retnCode == ACS_LM_RC_OK) || (retnCode == ACS_LM_RC_PHYFILEERROR))
		{
			(void)testLkf->getTestLks(testLkList);
			if(lkfCheckSum != 0)
			{
				(void)sentinel->getValidLks(sentinelLkList);
			}
		}
		break;
	}

	case ACS_LM_SENDER_TESTDEACT:
	{
		DEBUG("%s", "ACS_LM_SENDER_TESTDEACT");
		if((persistent->getLmMode() & ACS_LM_MODE_TEST) == ACS_LM_MODE_TEST)
		{
			persistent->setLmMode(ACS_LM_MODE_TEST, false);
			if(!persistent->commit())
			{
				testLkf->restoreAllTestLks();
				persistent->setLmMode(ACS_LM_MODE_TEST, true);
				retnCode = ACS_LM_RC_PHYFILEERROR;
				ERROR("%s","handleTestLkDeAct() Commit() failed");
			}
			else
			{
				retnCode = testLkf->removeAllTestLksPerm();
				//raise the event if persistnt file commit is success
				ACS_LM_EventHandler::LmEvent testModeAct = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_TEST_MODE_DEACT-ACS_LM_EVENT_ID_BASE];
				ACS_LM_EventHandler::reportEvent(testModeAct);
			}
		}
		else
		{
			retnCode = testLkf->removeAllTestLksPerm();
			if(!persistent->commit())
			{
				testLkf->restoreAllTestLks();
				retnCode = ACS_LM_RC_PHYFILEERROR;
				ERROR("%s","handleTestLkDeAct() Commit() failed");
			}
			INFO("%s","sendLks() TESTMODE ALREADY DEACTIVATED");
		}
		if((retnCode == ACS_LM_RC_OK) || ( retnCode == ACS_LM_RC_PHYFILEERROR))
		{
			(void)testLkf->getTestLks(testLkList);
			if(lkfCheckSum != 0)
			{
				(void)sentinel->getValidLks(sentinelLkList);
			}
		}
		break;
	}
	case ACS_LM_SENDER_CPRESTART:
	{
		DEBUG("%s", "ACS_LM_SENDER_CPRESTART");
		if((persistent->getLmMode() & ACS_LM_MODE_TEST) == ACS_LM_MODE_TEST)
		{
			if(sentinel->isTestModeAllowed())
			{
				(void)testLkf->getTestLks(testLkList);
			}
			else
			{
				INFO("%s","sendLks() Test mode Expired");
				persistent->setLmMode(ACS_LM_MODE_TEST, false);
				if(persistent->commit())
				{
					ACS_LM_EventHandler::LmEvent testModeDeAct = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_TEST_MODE_DEACT-ACS_LM_EVENT_ID_BASE];
					ACS_LM_EventHandler::reportEvent(testModeDeAct);
				}
				else
				{
					retnCode = ACS_LM_RC_PHYFILEERROR;
					ERROR("%s","sendLks() persistFile->commit() failed");
				}
			}
		}
		if(lkfCheckSum != 0)
		{
			(void)sentinel->getValidLks(sentinelLkList);
		}
		break;
	}
	case ACS_LM_SENDER_LKINST:
	{
		// load defined disconnection and connection keys here
		if(sentinel->isTestModeAllowed())
		{
			DEBUG("%s","Entering is isTestModeAllowed() in sendLks()");
			if((persistent->getLmMode() & ACS_LM_MODE_TEST) == ACS_LM_MODE_TEST)
			{
				(void)testLkf->getTestLks(testLkList);
			}
			else
			{
				INFO("%s","sendLks() Test mode allowed but deactivated");
			}
		}
		if(lkfCheckSum != 0)
		{
			//(void)sentinel->getValidLks(sentinelLkList); LMcleanup
			DEBUG("%s","sendLks().Before entering sentinel->getValidLks");
			(void)sentinel->getValidLks(sentinelLkList, false,true,discLklist,connectionLKlist);
			lminstCmnd = true ;

			DEBUG("%s","sendLks().After entering sentinel->getValidLks");
		}
		break;
	}

	case ACS_LM_SENDER_EMSTART:
	{
		DEBUG("%s", "ACS_LM_SENDER_CPRESTART");
		if(sentinel->isTestModeAllowed())
		{
			if((persistent->getLmMode() & ACS_LM_MODE_TEST) == ACS_LM_MODE_TEST)
			{
				(void)testLkf->getTestLks(testLkList);
			}
			else
			{
				INFO("%s","sendLks() Test mode allowed but deactivated");
			}
		}
		if(lkfCheckSum != 0)
		{
			(void)sentinel->getValidLks(sentinelLkList);
		}
		break;
	}
	/* LM Maintenance Mode */
	case ACS_LM_SENDER_MAINTENANCE:
	{
		DEBUG("%s", "ACS_LM_SENDER_MAINTENANCESTART");
                if(lkfCheckSum != 0)
                {
                        (void)sentinel->getValidLks(sentinelLkList);
                }
                break;
        }

	}

	std::list<LkData*> lkList;

	//append the Real LKlist after TestLks

	for(std::list<LkData*>::iterator itr1 = testLkList.begin();
			itr1!= testLkList.end(); ++itr1)
	{
		LkData* lkData = (*itr1);
		lkList.push_back(lkData);
	}
	testLkList.clear();

	for(std::list<LkData*>::iterator itr2 = sentinelLkList.begin();
			itr2!= sentinelLkList.end(); ++itr2)
	{
		LkData* lkData = (*itr2);
		lkList.push_back(lkData);
	}
	sentinelLkList.clear();

	size_t size = lkList.size(); 
	DEBUG("sendLks():lkList.size() = %d",(int)size);
	size_t discsize = discLklist.size();
	DEBUG("sendLks():discLklist.size() = %d",(int)discsize);
	size_t connsize = connectionLKlist.size();
	DEBUG("sendLks():connectionLKlist.size() = %d",(int)connsize);


	int mode = persistent->getLmMode();
	//CNI55_4_744
	if(persistent->isLmInVirginMode())
	{
		mode =mode | ACS_LM_MODE_VIRGIN;
	}

	int emCount = persistent->getEmergencyCounter();
	if((retnCode == ACS_LM_RC_OK) || retnCode == ACS_LM_RC_PHYFILEERROR)
	{
		bool emergencyModeActive = ((mode & ACS_LM_MODE_EMERGENCY) == ACS_LM_MODE_EMERGENCY);
		bool testModeActive = ((mode & ACS_LM_MODE_TEST) == ACS_LM_MODE_TEST);
		bool graceModeActive = ((mode & ACS_LM_MODE_GRACE) == ACS_LM_MODE_GRACE);
		bool virginModeActive = ((mode & ACS_LM_MODE_VIRGIN) == ACS_LM_MODE_VIRGIN);
		/* LM Maintenance Mode */
		bool maintenanceModeActive = ((mode & ACS_LM_MODE_MAINTENANCE) == ACS_LM_MODE_MAINTENANCE);

		DEBUG("sendLks() EMERGENCYMODE = %d",(int)emergencyModeActive);
		DEBUG(" TESTMODE =  %d",(int)testModeActive);
		DEBUG(" GRACEMODE =  %d",(int)graceModeActive);
		DEBUG(" VIRGINMODE =  %d",(int)virginModeActive);
		DEBUG(" VIRGINMODE =  %d",(int)persistent->isLmInVirginMode());
		DEBUG(" EMERGENCY COUNTER =  %d",(int)emCount);
		DEBUG(" MAINTENANCEMODE = %d", (int)maintenanceModeActive);

		// To cease the alarms in case of gracemode
		//if(!ACS_LM_Common::getLmdataLockStatus())--> need to remove to ceas alarms
		{
			if((lkList.size()==0) && (graceModeActive) )
			{
				eventHandler->verifyAndRaiseAlarm(lkList, isServiceStartup);
			}
		}
 
		//int retryAttempts = 1;
		retnCode = ACS_LM_RC_NOK;

		//TR-HS41812 Data transfer from AP to CP start. So acquiring the mutex to synchronize the processes.
		ACS_LM_LkSender::senddataMutex.acquire();   
		//check if conversation is active
		if(jtpConv != NULL)
		{
			jtpConv->disconnect();
			delete jtpConv;
			jtpConv = NULL;
		}

		jtpConv = ACS_LM_JTP_Conversation::create(3);

		DEBUG("ACS_LM_LkSender::sendLks() %s","jtpConv object is created in sendLks() method");
		if(jtpConv != NULL)
		{
			if(jtpConv->connect(CP_JTP_SERVICE, node))
			{
				if(jtpConv!=NULL)
				{
					/* LM Maintenance mode */
                			if ( maintenanceModeActive )
                			{
                        			DEBUG("ACS_LM_LkSender::sendLks() %s", "Maintenance mode is activated");
						/* Till CM003 version release, CP versions can be considered as Older versions
                                                   after CM003 version release, CP versions can be consider as Newer versions */
                                		/* For the Older CP versions, the CP returns protocal value as 1. Then AP send the Grace mode bit when Maintenance mode activated. 
                                   		   For the Newer CP versions, the CP returns protocal value as 2. Then AP send the Maintenance mode bit when Maintenance mode activated. */

                                		unsigned short protocolVer_CP = jtpConv->getProtocolFromCP(CP_JTP_SERVICE);
                                        	DEBUG("ACS_LM_LkSender::sendLks() recived CP protocol value as %d", protocolVer_CP);

                                		if ( protocolVer_CP  == NEW_VER_CP_PROTOCOL )
                                		{
                                        		DEBUG("ACS_LM_LkSender::sendLks() %s", "In new CP Version");
                                        		if ( emergencyModeActive && maintenanceModeActive )
                                        		{
                                                		DEBUG("ACS_LM_LkSender::sendLks() %s", "setting Maintenance & Emergency modes bit");
                                                		mode = ACS_LM_MODE_MAINTENANCE | ACS_LM_MODE_EMERGENCY ;
                                        		}
                                        		else
                                        		{
                                                		DEBUG("ACS_LM_LkSender::sendLks() %s", "setting Maintenance mode bit");
                                                		mode = ACS_LM_MODE_MAINTENANCE ;
                                        		}
                                		}
                                		else
                                		{
                                        		DEBUG("ACS_LM_LkSender::sendLks() %s", "In Old CP Version");
							if ( emergencyModeActive && maintenanceModeActive )
                                                        {
                                                                DEBUG("ACS_LM_LkSender::sendLks() %s", "setting Grace & Emergency modes bit");
                                                                mode = ACS_LM_MODE_GRACE | ACS_LM_MODE_EMERGENCY ;
                                                        }
							else
							{
                                        			DEBUG("ACS_LM_LkSender::sendLks() %s", "setting Grace mode bit");
                                        			mode = ACS_LM_MODE_GRACE ;
							}
                                		}
                			}

					if(senderType == ACS_LM_SENDER_LKINST)
					{
						DEBUG("%s","ACS_LM_LkSender::sendLks() senderType == ACS_LM_SENDER_LKINST");
						jtpConv->setLMINSTFlag(true);
					}
					else
					{
						DEBUG("%s","ACS_LM_LkSender::sendLks() senderType != ACS_LM_SENDER_LKINST");
						jtpConv->setLMINSTFlag(false);
					}
					if(jtpConv->send(lkList, mode,discLklist,connectionLKlist))  //LMcleanup
					{
						DEBUG("%s","Send sucessful");
						retnCode = ACS_LM_RC_OK;
						//	break;
					}
				}
			}
		}
		if(jtpConv != NULL)
		{
			INFO("%s","ACS_LM_LkSender::sendLks :jtpConv != NULL outside attempt loop");
			(void)jtpConv->disconnect();
			delete jtpConv;
			jtpConv = NULL;
		}
		//TR-HS41812 Data transfer completed. So release the mutex. 
		ACS_LM_LkSender::senddataMutex.release(); 
	}

	for(std::list<LkData*>::iterator it = lkList.begin(); 
			it != lkList.end(); ++it)
	{
		delete (*it);
	}
	lkList.clear();
	//LmCleanUp_start
	for(std::list<LkData*>::iterator it = discLklist.begin();
			it != discLklist.end(); ++it)
	{
		delete (*it);
	}
	discLklist.clear();

	for(std::list<LkData*>::iterator it = connectionLKlist.begin();
			it != connectionLKlist.end(); ++it)
	{
		delete (*it);
	}
	connectionLKlist.clear();
	//LmCleanUp_end

	DEBUG("sendLks() retnCode = %d",retnCode);

	return retnCode;
}//end of sendLks
/*=================================================================
	ROUTINE: cancel
=================================================================== */
void ACS_LM_LkSender::cancel()
{
	DEBUG("ACS_LM_LkSender::cancel() %s","jtpConv object is going to be canceled ");
	//check if conversation is active
	if(jtpConv != NULL)
	{
		DEBUG("%s","ACS_LM_LkSender::cancel()..calling disconnect function");
		jtpConv->setStopSignal(true);
		//jtpConv->disconnect();
		//delete jtpConv;
		//jtpConv = NULL;
	}
}//end of cancel
