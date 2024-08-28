//******************************************************************************
// 
// .NAME
//    EventReporter - Provides an interface to add entries to the EventLog
//                    similar to that of printf.
// .LIBRARY 3C++
// .PAGENAME EventReporter
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE <filename>

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2005.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//  The EventReporter may be used to add entries to the operating system's
//  event log. The interface provides the ability to enter events for the
//  range associated with MAS (25000 - 25999).
//  Use of the EventReporter is thread safe.
//
//  Usage:
//  To log a non critical problem simply do:
//  EVENT(EventReporter::<specific code>,
//        PROBLEM_DATA("This error (%d) is not critical",
//                     GetLastError()),
//        PROBLEM_TEXT(""));
//
//  or to raise an A2 alarm do:
//  ALARM_A2(EventReporter::<specific code>,
//           PROBLEM_DATA("Raising an A2 alarm"),
//           PROBLEM_TEXT("<ALARM TO SEND TO THE CP>"),
//           PROBLABLE_CAUSE("<ALARM SLOGAN TO SEND TO THE CP>");
//
//  This requires that the calling process has defined a process identifier
//  (const char* MAS_Common_ProcessIdentifier) in the global variable scope.
//  This string is used by the EventReporter to identify the process that is
//  using the EventReporter to access the event log.

// .ERROR HANDLING
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-06-09 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

// .LINKAGE
//  No specific order required.

// .SEE ALSO 
//  N/A.

//******************************************************************************
#ifndef EVENTREPORTER_H 
#define EVENTREPORTER_H

#include "Log.H"
#include <string>

//extern const char* MAS_Common_ProcessIdentifier;
#include "ACS_CS_Util.h"

#define EVENT (EventReporter(EventReporter::EventType, __FILE__, __LINE__))
#define ALARM_A1 (EventReporter(EventReporter::AlarmTypeA1, __FILE__, __LINE__))
#define ALARM_A2 (EventReporter(EventReporter::AlarmTypeA2, __FILE__, __LINE__))
#define ALARM_A3 (EventReporter(EventReporter::AlarmTypeA3, __FILE__, __LINE__))
#define ALARM_O1 (EventReporter(EventReporter::AlarmTypeO1, __FILE__, __LINE__))
#define ALARM_O2 (EventReporter(EventReporter::AlarmTypeO2, __FILE__, __LINE__))
#define CEASING (EventReporter(EventReporter::CeasingType, __FILE__, __LINE__))

#define PROBLEM_DATA (ProblemData(__FILE__, __LINE__))
#define PROBLEM_TEXT (ProblemText(__FILE__, __LINE__))
#define PROBABLE_CAUSE (EventText(__FILE__, __LINE__))

class EventReporter;

// Member functions, constructors, destructors, operators

class EventData
{
protected:
   // The constructor records the file and line location where the instance
   // was created.
   EventData(const char* file, int line) :
      m_Line(line),
      m_File(Log::fileWithoutPath(file))
   {
      m_formattedData[0] = 0;
   }

   // This method parses input of the same format as would be accepted by
   // printf and returns the formatted text as a string.
   void parse(const char* format, va_list& arguments);

public:
   // This method returns the parsed data
   std::string getFormattedData() const
   {
      return m_formattedData;
   }

private:
   friend class EventReporter;

#if (defined _MSC_VER  && (_MSC_VER > 1200)) || !defined _MSC_VER
   // The maximum length of the formatted output.
   static const int MaxLength = 1800;
#else
   enum Constant
   {
      MaxLength = 1800
   };
#endif // _MSC_VER

   // The line at which the EventReporter instance was created.
   const int m_Line;

   // The file in which the EventReporter instance was created.
   const char* m_File;

   // The input va_list data which has been formatted by parser.
   char m_formattedData[MaxLength];
};

class EventText : public EventData
{
public:
   // The constructor records the file and line location where the instance
   // was created.
   EventText(const char* file, int line) :
      EventData(file, line)
      {
      }

   // This operator parses input of the same format as would be accepted by
   // printf and returns the formatted text in a string instance.
   std::string operator()(const char* format, ...);
};

class ProblemData : public EventData
{
public:
   // The constructor records the file and line location where the instance
   // was created.
   ProblemData(const char* file, int line) :
      EventData(file, line)
      {
      }

   // This operator parses input of the same format as would be accepted by
   // printf and returns the formatted text in a 'this' instance.
   ProblemData operator()(const char* format, ...);
};

class ProblemText : public EventData
{
public:
   // The constructor records the file and line location where the instance
   // was created.
   ProblemText(const char* file, int line) :
      EventData(file, line)
   {
   }

   // This operator parses input of the same format as would be accepted by
   // printf and returns the formatted text in a 'this' instance.
   ProblemText operator()(const char* format, ...);
};


class EventReporter
{
public:
   enum Status
   {
      OK,
      NotOK
   };

   enum Type
   {
      EventType,
      AlarmTypeA1,
      AlarmTypeA2,
      AlarmTypeA3,
      AlarmTypeO1,
      AlarmTypeO2,
      CeasingType
   };

   enum Code
   {
      BASE = 25000, // Codes 25000 - 25999 are MAS specific

      NonCritical                = BASE,      // Things that should not happen but will not interfere with the overall function.
      ProgrammingError           = BASE + 1,  // Things that should not really happen.
      StartupFailure             = BASE + 2,  // Something in the initialization process has gone wrong.
      SocketAPI                  = BASE + 3,  // Problems related with Windows socket API functions.
      SubSystemError             = BASE + 4,  // Problems with general Windows functions.
      PTBConnection              = BASE + 5,  // Problems with PTB connections.
      PTBConnectionAlarm         = BASE + 6,  // Problems with PTB connections that are logged as an alarm.
      CommunicationFailure       = BASE + 7,  // Problems related to connections between CPTMML, Central and Distributor
      DHCPFailure                = BASE + 8,  // Problems related to accesing the DHCP server,
      InvalidCPTREADMAUREGRAlarm = BASE + 9,  // Problems related to received CPTREADMAUREGR signals that do not contain valid information.
      ParseCPTREADMAUREGRAlarm   = BASE + 10, // Problems related to problems with the data layout of a received CPTREADMAUREGR signal.
      ApplicationError           = BASE + 11  // ServiceControl detected problems when calling the registered process' functions.
   };

   // The constructor records the file and line location where the instance
   // was created.
   EventReporter(Type type, const char* file, int line) :
      m_Type(type),
      m_Line(line),
      m_File(Log::fileWithoutPath(file))
   {
   }

   // These operators use the parsed input from the EventText, ProblemData 
   // and ProblemText. They put in the Code to the event log. It reports 
   // whether the information was delivered to the event log or not.
   Status operator()(Code code,
                     const ProblemText& problemText,
                     const ProblemData& problemData, 
                     const std::string probableCause = "INFORMATION") const;
   Status operator()(Code code,
                     const ProblemData& problemData,
                     const ProblemText& problemText, 
                     const std::string probableCause = "INFORMATION") const;
 
private:
#if (defined _MSC_VER  && (_MSC_VER > 1200)) || !defined _MSC_VER
   // The maximum length of the formatted output.
   static const int MaxLength = EventData::MaxLength + 200;
#else
   enum Constant
   {
      MaxLength = EventData::MaxLength + 200
   };
#endif // _MSC_VER

   // The type of log that should be handled.
   const Type m_Type;

   // The line at which the EventReporter instance was created.
   const int m_Line;

   // The file in which the EventReporter instance was created.
   const char* m_File;
};

#endif
