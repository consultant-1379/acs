//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%4C6DA5750065.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%4C6DA5750065.cm

//## begin module%4C6DA5750065.cp preserve=no
//	*********************************************************
//	 COPYRIGHT Ericsson 2010.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2010.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2010 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	*********************************************************
//## end module%4C6DA5750065.cp

//## Module: ACS_CS_API_UsageLogger%4C6DA5750065; Package body
//## Subsystem: ACS_CS::csapi_caa::src%4586AE7D000F
//## Source file: T:\ntacs\cs_cnz\csapi_caa\src\ACS_CS_API_UsageLogger.cpp

//## begin module%4C6DA5750065.additionalIncludes preserve=no
//## end module%4C6DA5750065.additionalIncludes

//## begin module%4C6DA5750065.includes preserve=yes
#include "ACS_CS_API_Tracer.h"
//#include <Windows.h>
//#include "halPlatformDefinitions.hxx"
//## end module%4C6DA5750065.includes

// ACS_CS_API_UsageLogger
#include "ACS_CS_API_UsageLogger.h"
//## begin module%4C6DA5750065.declarations preserve=no
//## end module%4C6DA5750065.declarations

//## begin module%4C6DA5750065.additionalDeclarations preserve=yes
namespace
{
	ACS_CS_API_TRACER_DEFINE(ACS_CS_API_Usage);
}
//## end module%4C6DA5750065.additionalDeclarations


// Class ACS_CS_API_UsageLogger 

//## begin ACS_CS_API_UsageLogger::S_LOG_INTERVAL%4C6DA6F800B6.attr preserve=no  private: static const u_int32  {U} 10U
const uint32_t  ACS_CS_API_UsageLogger::S_LOG_INTERVAL = 10U;
//## end ACS_CS_API_UsageLogger::S_LOG_INTERVAL%4C6DA6F800B6.attr

//## begin ACS_CS_API_UsageLogger::S_LOG_INTERVAL_THRESHOLD%4C6DA7020011.attr preserve=no  private: static const u_int32  {U} S_LOG_INTERVAL*2U
const uint32_t  ACS_CS_API_UsageLogger::S_LOG_INTERVAL_THRESHOLD = S_LOG_INTERVAL*2U;
//## end ACS_CS_API_UsageLogger::S_LOG_INTERVAL_THRESHOLD%4C6DA7020011.attr

//## begin ACS_CS_API_UsageLogger::S_MUTEX_WAIT_MSEC%4C6DA70D0096.attr preserve=no  private: static const DWORD  {U} 500U
const uint32_t  ACS_CS_API_UsageLogger::S_MUTEX_WAIT_MSEC = 500U;
//## end ACS_CS_API_UsageLogger::S_MUTEX_WAIT_MSEC%4C6DA70D0096.attr

//## begin ACS_CS_API_UsageLogger::s_instance%4C6DA7150204.attr preserve=no  private: static ACS_CS_API_UsageLogger {U} 
ACS_CS_API_UsageLogger ACS_CS_API_UsageLogger::s_instance;
//## end ACS_CS_API_UsageLogger::s_instance%4C6DA7150204.attr

ACS_CS_API_UsageLogger::ACS_CS_API_UsageLogger()
  //## begin ACS_CS_API_UsageLogger::ACS_CS_API_UsageLogger%4C6DA27E0208_const.hasinit preserve=no
  //## end ACS_CS_API_UsageLogger::ACS_CS_API_UsageLogger%4C6DA27E0208_const.hasinit
  //## begin ACS_CS_API_UsageLogger::ACS_CS_API_UsageLogger%4C6DA27E0208_const.initialization preserve=yes
  : m_usageCounter()//,
  //m_usageCounterMutex(CreateMutex(NULL, FALSE, NULL))
  //## end ACS_CS_API_UsageLogger::ACS_CS_API_UsageLogger%4C6DA27E0208_const.initialization
{
  //## begin ACS_CS_API_UsageLogger::ACS_CS_API_UsageLogger%4C6DA27E0208_const.body preserve=yes
  //## end ACS_CS_API_UsageLogger::ACS_CS_API_UsageLogger%4C6DA27E0208_const.body
}


ACS_CS_API_UsageLogger::~ACS_CS_API_UsageLogger()
{
  //## begin ACS_CS_API_UsageLogger::~ACS_CS_API_UsageLogger%4C6DA27E0208_dest.body preserve=yes
  //## end ACS_CS_API_UsageLogger::~ACS_CS_API_UsageLogger%4C6DA27E0208_dest.body
}



//## Other Operations (implementation)
 void ACS_CS_API_UsageLogger::logUsage (char const */*methodName*/)
{
  /*
   if (ACS_TRA_ON(ACS_CS_API_Usage))
   {
      u_int32 currentCount(0U);

      DWORD mutexWaitResult(WaitForSingleObject(m_usageCounterMutex, S_MUTEX_WAIT_MSEC));

      switch (mutexWaitResult) 
      {
      case WAIT_OBJECT_0: // The thread got ownership of the mutex
         currentCount = ++m_usageCounter[methodName]; // created on the fly the first time
         break;
      default:
         // no op; will log failure below
         break;
      }
      ReleaseMutex(m_usageCounterMutex);

      if (0U == currentCount) // it is assumed that this will never roll over
      {
         ACS_CS_API_TRACER_MESSAGE(("(%t) %s: Process failed to get mutex to log use of method %s; wait result = %d",
            __FUNCTION__,
            methodName,
            mutexWaitResult));
      }
      else if (S_LOG_INTERVAL_THRESHOLD >= currentCount || 0U == (currentCount % S_LOG_INTERVAL))
      {
         // Note __FUNCTION__ context in this trace will be from caller, not ACS_CS_API_UsageLogger::logUsage
         ACS_CS_API_TRACER_MESSAGE(("(%t) %s: Called by this process %u times",
            methodName,
            currentCount));
      }
   }
  */
}

// Additional Declarations
  //## begin ACS_CS_API_UsageLogger%4C6DA27E0208.declarations preserve=yes
  //## end ACS_CS_API_UsageLogger%4C6DA27E0208.declarations

//## begin module%4C6DA5750065.epilog preserve=yes
//## end module%4C6DA5750065.epilog
