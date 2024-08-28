//******************************************************************************
//
// NAME
//      EventReporter.cpp
//
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
//  See EventReporter.H

//  DOCUMENT NO
//  <Container file>

//  AUTHOR 
//  2005-04-18 by EAB/UZ/DE Peter Johansson (Contactor Data AB)

//  SEE ALSO 
//  N/A.
//
//******************************************************************************
#include "EventReporter.H"
#include "MultiPlatformMutex.hxx"
#include "ACS_AEH_EvReport.H"
#include <string.h>
#include <stdio.h>

using namespace std;

namespace
{
   MultiPlatformMutex MAS_Common_eventReporterMutex;
   ACS_AEH_EvReport MAS_Common_eventLog;
};

//******************************************************************************
// Description:
//    The method parses input of the same format as would be accepted by
//    printf and returns the formatted text as a string. It's called by the
//    operators below.
// Parameters:
//    All input data (format and ...) is of the same syntax as would be
//    accepted by for instance printf.
// Returns:
//    The formatted data as a string member.
//******************************************************************************
void EventData::parse(const char* format, va_list& arguments)
{
   // Make sure all access is thread safe. It's done in calling methods

   memset(m_formattedData, 0, MaxLength);

   // Extract the ellipsis arguments. This has already been done in caller method
   //va_list arguments;  
   //va_start(arguments, format);

   if(vsnprintf(m_formattedData, MaxLength, format, arguments) == -1)
   {
      // vsnprintf failed, at least add the format string to the event log
      // since it may give some indication of what the user wanted to put
      // there from the beginning.
      if(sprintf(m_formattedData, "%s", format) == -1)
      {
         // Not event the format string could be appended to the event data.
         // Add at least the filename and line number to the event log that
         // wanted to inform us of something.
         strcpy(m_formattedData, "[EventReporter failed to extract the event data that is was handed]");
      }
   }
   va_end(arguments);

   return;
}

//******************************************************************************
// Description:
//    The method parses input of the same format as would be accepted by
//    printf and returns the formatted text as a string.
// Parameters:
//    All input data (format and ...) is of the same syntax as would be
//    accepted by for instance printf.
// Returns:
//    The formatted data as a string.
//******************************************************************************
string EventText::operator()(const char* format, ...)
{
   // Make sure all access is thread safe
   SafeMutex mutex(MAS_Common_eventReporterMutex); 

   va_list arguments;
   // Extract the ellipsis arguments. This must be done before calling the
   // parse() method, as the arguments parameter in va_start() otherwise
   // becomes corrupted!!
   va_start(arguments, format);

   parse(format, arguments);

   return getFormattedData();
}

//******************************************************************************
// Description:
//    The method parses input of the same format as would be accepted by
//    printf and returns the formatted text as a string.
// Parameters:
//    All input data (format and ...) is of the same syntax as would be
//    accepted by for instance printf.
// Returns:
//    The 'this' instance containing the formatted data as a string.
//******************************************************************************
ProblemData ProblemData::operator()(const char* format, ...)
{
   // Make sure all access is thread safe
   SafeMutex mutex(MAS_Common_eventReporterMutex); 

   va_list arguments;
   // Extract the ellipsis arguments. This must be done before calling the
   // parse() method, as the 'arguments' parameter in va_start() otherwise
   // becomes corrupted!!
   va_start(arguments, format);

   parse(format, arguments);

   return *this;
}

//******************************************************************************
// Description:
//    The method parses input of the same format as would be accepted by
//    printf and returns the formatted text as a string.
// Parameters:
//    All input data (format and ...) is of the same syntax as would be
//    accepted by for instance printf.
// Returns:
//    The 'this' instance containing the formatted data as a string.
//******************************************************************************
ProblemText ProblemText::operator()(const char* format, ...)
{
   // Make sure all access is thread safe
   SafeMutex mutex(MAS_Common_eventReporterMutex); 

   va_list arguments;
   // Extract the ellipsis arguments. This must be done before calling the
   // parse() method, as the 'arguments' parameter in va_start() otherwise
   // becomes corrupted!!
   va_start(arguments, format);

   parse(format, arguments);

   return *this;
}

//******************************************************************************
// Description:
//    These two operator methods interpret the input and log it to Windows' event
//    log using ACS_AEH_EvReport.
// Parameters:
//    code  The event code that should be used when reporting the supplied
//          information to the event log.
//    problemData    Text description of the event, will not be sent to the CP.
//    problemText    Detailed description of the event, may be sent to the CP in
//                   the case of an alarm or ceasing.
//    probableCause  The CP alarm slogan if specified. Default is "INFORMATION"
//                   only.
// Returns:
//    OK when the the information was delivered to the event log.
//    NotOK when the information was not delivered to the event log.
//******************************************************************************
EventReporter::Status EventReporter::operator()(Code code,
                                                const ProblemData& problemData,
                                                const ProblemText& problemText,
                                                const string probableCause) const
{
   // Make sure all access is thread safe.
   SafeMutex mutex(MAS_Common_eventReporterMutex);

   const int EventDataMaxLength = 2000;
   char eventData[EventDataMaxLength];
   int offset;
   const int MaxProcessIDLength = 30;
   const char* logType;
   char processIdWithPID[MaxProcessIDLength];

   // Make sure to align the MAS_Common_ProcessIdentifier to the left in the
   // processIdWithPID while ensuring not to copy more data than what is
   // allowed for a process name (30 characters).
   if(sprintf(processIdWithPID, "%.30s:%d",
			  ACS_CS_NS::ACS_CS_PROCESS_NAME,
              GetCurrentProcessId()) == -1)
   {
      // We were unable to concatenate the MAS_Common_ProcessIdentifier with
      // the process ID. Use the MAS_Common_ProcessIdentifier instead.
      strncpy(processIdWithPID, ACS_CS_NS::ACS_CS_PROCESS_NAME, MaxProcessIDLength);
   }

   switch(m_Type)
   {
   case AlarmTypeA1:
      // An alarm that must be attended immediately, regardless of time of
      // day.
      logType = "A1";
      break;

   case AlarmTypeA2:
      // An alarm that must be attended as soon as possible, but only during
      // normal working hours.
      logType = "A2";
      break;

   case AlarmTypeA3:
      // An alarm that should be attended at an apropriate time, preferably
      // within one week.
      logType = "A3";
      break;

   case AlarmTypeO1:
      // An alarm that should be attended as soon as possible during work
      // hours. The reason for this alarm is operator intervention.
      logType = "O1";
      break;

   case AlarmTypeO2:
      // An alarm that should be attended at an apropriate time. The reason
      // for this alarm is operator intervention.
      logType = "O2";
      break;

   case CeasingType:
      // The cause of a prevoiusly reported alarm is no longer present.
      logType = "CEASING";
      break;

   case EventType:
   default:
      logType = "EVENT";
   }

   // Add the reporting file and line to the event data.
   offset = sprintf(eventData, "%s@%d: ", m_File, m_Line);

   if(offset == -1)
   {
      // Adding the reporting file and line failed, let this problem be
      // visible in the event log.
      strcpy(eventData, "Unknown source reports: ");
      offset = static_cast<int>(strlen(eventData));
   }

   // Append the problem text to the eventData.
   sprintf(&eventData[offset], "%s", problemData.getFormattedData().c_str());

   ACS_AEH_ReturnType returnValue;
   
   returnValue = MAS_Common_eventLog.sendEventMessage(processIdWithPID,
                                                      code,
                                                      logType,
                                                      probableCause.c_str(), // Will become the slogan in the CP larm
                                                      "APZ",
                                                      "", // Object of reference
                                                      eventData, // Problem data (this information is never sent to the CP)
                                                      problemText.getFormattedData().c_str()); // Problem text (this information is sent to the CP in the case of an alarm or ceasing)

   DBGLOG("Event log data for event type %s:\n   Problem data: \"%s\"\n   Problem text: \"%s\"\n   Probable cause: \"%s\"\n",
          logType,
          eventData,
          problemText.getFormattedData().c_str(),
          probableCause.c_str());

   if(returnValue != ACS_AEH_ok)
   {
      ACS_AEH_ErrorReturnType theError = MAS_Common_eventLog.getError();

      switch(theError)
      {
      case ACS_AEH_syntaxError:
         DBGLOG("ACS_AEH_syntaxError when passing code %d with description \"%s\"\n", code, eventData);
         break;
         
      case ACS_AEH_eventDeliveryFailure:
         DBGLOG("ACS_AEH_eventDeliveryFailure when passing code %d with description \"%s\"\n", code, eventData);
         break;
         
      default:
         DBGLOG("Invalid value returned from ACS_AEH_EvReport.getError(): %d\n", theError);
      }
   }

   return returnValue == ACS_AEH_ok ? OK : NotOK;
}

//******************************************************************************
// Description:
//    See the overlayed operator method above.
//******************************************************************************
EventReporter::Status EventReporter::operator()(Code code,
                                                const ProblemText& problemText,
                                                const ProblemData& problemData,
                                                const string probableCause) const
{
   return EventReporter(m_Type, m_File, m_Line)(code, problemData, problemText, probableCause);
}
