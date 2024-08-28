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
  * @file ACS_XBRM_MMLMessageHandler.cpp
  *
  * @brief
  * ACS_XBRM_MMLMessageHandler Class for MML command message handler
  *
  * @details
  * Implementation of ACS_XBRM_MMLMessageHandler class to handle MML Messages
  *
  * @author XHARBAV
  *
-------------------------------------------------------------------------*/ /*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * -------------------------------------------
 * 2023-07-11  XHARBAV  Created First Revision
 *
 ****************************************************************************/

#ifndef ACS_XBRM_MMLMESSAGEHANDLER_H
#define ACS_XBRM_MMLMESSAGEHANDLER_H

#include <ace/Task.h>
#include <string.h>

using namespace std;
#include "ACS_XBRM_MMLEventHandler.h"
class ACS_XBRM_MMLEventHandler;

//----------------------------------------------------------------------------
class ACS_XBRM_MMLMessageHandler: public ACE_Task<ACE_MT_SYNCH> {
public:
	ACS_XBRM_MMLMessageHandler();
	virtual ~ACS_XBRM_MMLMessageHandler();
	virtual int open(const char* cpName);
	virtual int close(u_long);
   void stop();
   bool sigTermReceived;
	std::string getBackupSwLevel();
   std::string getBackupErrorCode();
   std::string getBackupRelVolume();
protected:
   virtual int svc (void );

private:
	ACS_XBRM_MMLEventHandler* m_mmlEventHandler;
   std::string backupSwLevel, backupErrorCode, backupRelVolume;
};

#endif /* MCS_MLS_MML_SESSION_H_ */
