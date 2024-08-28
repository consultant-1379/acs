/*
 * ACS_APSESH_TestableJtpMgr.cpp
 *
 *  Created on: May 10, 2011
 *      Author: bjax
 */


#include "ACS_APSESH_TestableJtpMgr.h"


ACS_APSESH_JtpReturnType ACS_APSESH_TestableJtpMgr::init()
{

   ACS_APSESH_JtpReturnType returnVal;

   returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_OK;
   returnVal.returnMsg = "ACS_APSESH_TestableJtpMgr::init returns ACS_APSESH_JtpReturnType::JTP_OK";
   return returnVal;

}


ACS_APSESH_JtpReturnType ACS_APSESH_TestableJtpMgr::getData(char*& /*buffer*/, int& /*bufLen*/)
{
   ACS_APSESH_JtpReturnType returnVal;

   returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_QUORUM_DISSOLVE_DISCONNECT;
   returnVal.returnMsg = "ACS_APSESH_TestableJtpMgr::getData returns ACS_APSESH_JtpReturnType::JTP_QUORUM_DISSOLVE_DISCONNECT";
   return returnVal;
}

ACS_APSESH_JtpReturnType ACS_APSESH_TestableJtpMgr::sendStatusCs(short /*status*/)
{
   ACS_APSESH_JtpReturnType returnVal;

   returnVal.returnCode = ACS_APSESH_JtpReturnType::JTP_OK;
   returnVal.returnMsg = "ACS_APSESH_TestableJtpMgr::init returns ACS_APSESH_JtpReturnType::JTP_OK";
   return returnVal;


}

void ACS_APSESH_TestableJtpMgr::setJtpWaitTimeout(const unsigned int& /*timeout*/) {}

void ACS_APSESH_TestableJtpMgr::resetJtpWaitTimeout() {}


