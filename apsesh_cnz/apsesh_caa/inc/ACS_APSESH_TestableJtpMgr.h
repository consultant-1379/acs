/*
 * ACS_APSESH_TestableJtpMgr.h
 *
 *  Created on: May 10, 2011
 *      Author: bjax
 */

#ifndef ACS_APSESH_TESTABLEJTPMGR_H_
#define ACS_APSESH_TESTABLEJTPMGR_H_

#include "ACS_APSESH_JtpMgrInterface.h"
#include "ACS_APSESH_JtpReturnType.h"

class ACS_APSESH_TestableJtpMgr : public ACS_APSESH_JtpMgrInterface
{

public:

   virtual ACS_APSESH_JtpReturnType init();
   virtual ACS_APSESH_JtpReturnType getData(char*& buffer, int& bufLen);
   virtual ACS_APSESH_JtpReturnType sendStatusCs(short status);
   virtual void setJtpWaitTimeout(const unsigned int& timeout);
   virtual void resetJtpWaitTimeout();



};




#endif /* ACS_APSESH_TESTABLEJTPMGR_H_ */
