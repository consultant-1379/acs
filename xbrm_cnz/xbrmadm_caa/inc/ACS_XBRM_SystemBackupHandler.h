/*****************************************************************************
 *
 * COPYRIGHT Ericsson 2023
 *
 * The copyright of the computer program herein is the property of
 * Ericsson 2023. The program may be used and/or copied only with the
 * written permission from Ericsson 2023 or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*/ /**
 *
 * @file ACS_XBRM_SystemBackupHandler.h
 *
 * @brief
 * ACS_XBRM_SystemBackupHandler Thread to manage System Backup Handling
 *
 * @details
 * ACS_XBRM_SystemBackupHandler 
 *
 * @author ZPAGSAI
 *
-------------------------------------------------------------------------*/ /*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * -------------------------------------------
 * 2023-07-31  ZPAGSAI  Created First Revision
 *
 ****************************************************************************/

#ifndef ACS_XBRM_SYSTEMBACKUPHANDLER_H
#define ACS_XBRM_SYSTEMBACKUPHANDLER_H

//ACE libraries
#include <ace/Task.h>
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include "ACS_XBRM_ApBackupHandler.h"
#include "ACS_XBRM_MMLMessageHandler.h"
#include "ACS_XBRM_AlarmHandler.h"

//ACS libraries
#include "ACS_XBRM_Utilities.h"

class ACS_XBRM_SystemBackupHandler : public ACE_Task_Base{
    public:
        ACS_XBRM_SystemBackupHandler(int backupType[6], ACS_XBRM_UTILITY::BackupCategory backupCategory, ACS_XBRM_AlarmHandler* alarmHandler);
        ~ACS_XBRM_SystemBackupHandler();
        int svc(void);
        void stop();
        void cleanupNbiFiles();
        int backupType[6];
        int backupCategory;
        OmHandler immHandle;
        bool sigTermReceived;
        ACS_XBRM_MMLMessageHandler* messageHandler = NULL;
        ACS_XBRM_AlarmHandler* m_alarmHandler = NULL;
        ACS_XBRM_ApBackupHandler* apBackupHandler = NULL;
        vector<ACS_XBRM_UTILITY::backupInfo> applicableBackupInfo;
        ACS_XBRM_Utilities* utilities = NULL;
       
};

#endif