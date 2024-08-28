//## begin module%1.10%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.10%.codegen_version

//## begin module%4C6DA565012F.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%4C6DA565012F.cm

//## begin module%4C6DA565012F.cp preserve=no
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
//## end module%4C6DA565012F.cp

//## Module: ACS_CS_API_UsageLogger%4C6DA565012F; Package specification
//## Subsystem: ACS_CS::csapi_caa::src%4586AE7D000F
//## Source file: T:\ntacs\cs_cnz\csapi_caa\src\ACS_CS_API_UsageLogger.h

#ifndef ACS_CS_API_UsageLogger_h
#define ACS_CS_API_UsageLogger_h 1

//## begin module%4C6DA565012F.additionalIncludes preserve=no
//## end module%4C6DA565012F.additionalIncludes

//## begin module%4C6DA565012F.includes preserve=yes
//#include "halPlatformDefinitions.hxx"
#include <map>
//#include <Windows.h>
//## end module%4C6DA565012F.includes

//## begin module%4C6DA565012F.declarations preserve=no
//## end module%4C6DA565012F.declarations

//## begin module%4C6DA565012F.additionalDeclarations preserve=yes
//## end module%4C6DA565012F.additionalDeclarations


//## begin ACS_CS_API_UsageLogger%4C6DA27E0208.preface preserve=yes
//## end ACS_CS_API_UsageLogger%4C6DA27E0208.preface

//## Class: ACS_CS_API_UsageLogger%4C6DA27E0208
//	A class that logs usage by API clients of methods that
//	trigger socket creation (i.e. communicate with CS
//	Server).
//	When the specific trace ACS_CS_API_UsageLogger is
//	enabled, this class counts accesses to each method
//	and logs the accumulated counts to the trace log.
//## Category: csapi%45869FE8034B
//## Subsystem: ACS_CS::csapi_caa::src%4586AE7D000F
//## Persistence: Transient
//## Cardinality/Multiplicity: 1



class ACS_CS_API_UsageLogger 
{
  //## begin ACS_CS_API_UsageLogger%4C6DA27E0208.initialDeclarations preserve=yes
  //## end ACS_CS_API_UsageLogger%4C6DA27E0208.initialDeclarations

  private:
    //## begin ACS_CS_API_UsageLogger::UsageCounter%4C6DB9CD00C7.preface preserve=yes
    //## end ACS_CS_API_UsageLogger::UsageCounter%4C6DB9CD00C7.preface

    //## Class: UsageCounter%4C6DB9CD00C7; private
    //## Category: csapi%45869FE8034B
    //## Subsystem: ACS_CS::csapi_caa::src%4586AE7D000F
    //## Persistence: Transient
    //## Cardinality/Multiplicity: n



    typedef std::map<char const *, uint32_t> UsageCounter;
    //## begin ACS_CS_API_UsageLogger::UsageCounter%4C6DB9CD00C7.postscript preserve=yes
    //## end ACS_CS_API_UsageLogger::UsageCounter%4C6DB9CD00C7.postscript

  public:

    //## Other Operations (specified)
      //## Operation: logUsage%4C6DA6380221
      void logUsage (char const *methodName);

      //## Operation: instance%4C6DA6B401AE
      static  ACS_CS_API_UsageLogger & instance ();

    // Additional Public Declarations
      //## begin ACS_CS_API_UsageLogger%4C6DA27E0208.public preserve=yes
      //## end ACS_CS_API_UsageLogger%4C6DA27E0208.public

  protected:
    // Additional Protected Declarations
      //## begin ACS_CS_API_UsageLogger%4C6DA27E0208.protected preserve=yes
      //## end ACS_CS_API_UsageLogger%4C6DA27E0208.protected

  private:
    //## Constructors (generated)
      ACS_CS_API_UsageLogger();

      ACS_CS_API_UsageLogger(const ACS_CS_API_UsageLogger &right);

    //## Destructor (generated)
      virtual ~ACS_CS_API_UsageLogger();

    //## Assignment Operation (generated)
      ACS_CS_API_UsageLogger & operator=(const ACS_CS_API_UsageLogger &right);

    // Additional Private Declarations
      //## begin ACS_CS_API_UsageLogger%4C6DA27E0208.private preserve=yes
      //## end ACS_CS_API_UsageLogger%4C6DA27E0208.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: S_LOG_INTERVAL%4C6DA6F800B6
      //	After initial burst, log only every S_LOG_INTERVAL uses
      //## begin ACS_CS_API_UsageLogger::S_LOG_INTERVAL%4C6DA6F800B6.attr preserve=no  private: static const u_int32  {U} 10U
      static const uint32_t  S_LOG_INTERVAL;
      //## end ACS_CS_API_UsageLogger::S_LOG_INTERVAL%4C6DA6F800B6.attr

      //## Attribute: S_LOG_INTERVAL_THRESHOLD%4C6DA7020011
      //	log every use until this threshold is reached
      //## begin ACS_CS_API_UsageLogger::S_LOG_INTERVAL_THRESHOLD%4C6DA7020011.attr preserve=no  private: static const u_int32  {U} S_LOG_INTERVAL*2U
      static const uint32_t  S_LOG_INTERVAL_THRESHOLD;
      //## end ACS_CS_API_UsageLogger::S_LOG_INTERVAL_THRESHOLD%4C6DA7020011.attr

      //## Attribute: S_MUTEX_WAIT_MSEC%4C6DA70D0096
      //## begin ACS_CS_API_UsageLogger::S_MUTEX_WAIT_MSEC%4C6DA70D0096.attr preserve=no  private: static const DWORD  {U} 500U
      static const uint32_t  S_MUTEX_WAIT_MSEC;
      //## end ACS_CS_API_UsageLogger::S_MUTEX_WAIT_MSEC%4C6DA70D0096.attr

      //## Attribute: s_instance%4C6DA7150204
      //## begin ACS_CS_API_UsageLogger::s_instance%4C6DA7150204.attr preserve=no  private: static ACS_CS_API_UsageLogger {U} 
      static ACS_CS_API_UsageLogger s_instance;
      //## end ACS_CS_API_UsageLogger::s_instance%4C6DA7150204.attr

      //## Attribute: m_usageCounter%4C6DA71B0246
      //## begin ACS_CS_API_UsageLogger::m_usageCounter%4C6DA71B0246.attr preserve=no  private: UsageCounter {U} 
      UsageCounter m_usageCounter;
      //## end ACS_CS_API_UsageLogger::m_usageCounter%4C6DA71B0246.attr

      //## Attribute: m_usageCounterMutex%4C6DA7220180
      //## begin ACS_CS_API_UsageLogger::m_usageCounterMutex%4C6DA7220180.attr preserve=no  private:  HANDLE {U} 
       //HANDLE m_usageCounterMutex;
      //## end ACS_CS_API_UsageLogger::m_usageCounterMutex%4C6DA7220180.attr

    // Additional Implementation Declarations
      //## begin ACS_CS_API_UsageLogger%4C6DA27E0208.implementation preserve=yes
      //## end ACS_CS_API_UsageLogger%4C6DA27E0208.implementation

};

//## begin ACS_CS_API_UsageLogger%4C6DA27E0208.postscript preserve=yes
//## end ACS_CS_API_UsageLogger%4C6DA27E0208.postscript

// Class ACS_CS_API_UsageLogger 


//## Other Operations (inline)
inline ACS_CS_API_UsageLogger & ACS_CS_API_UsageLogger::instance ()
{
  //## begin ACS_CS_API_UsageLogger::instance%4C6DA6B401AE.body preserve=yes
   return s_instance; // Safe because the only instance lives in static area and is only used by users of the API
  //## end ACS_CS_API_UsageLogger::instance%4C6DA6B401AE.body
}

//## begin module%4C6DA565012F.epilog preserve=yes
//## end module%4C6DA565012F.epilog


#endif
