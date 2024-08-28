//*****************************************************************************
//
// .NAME
//     MCS_Event - Internal Classes for event and alarm sending.
// .LIBRARY 3C++
// .PAGENAME APGCC
// .HEADER  APGCC
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE ACS_APGCC_Event.H

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1998.
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
//  Encapsulates the ACS AEH interface.
//  Objects for handling events and alarms.
//  Rules for event and alarm reporting can be found
//  in the design rule "AP Event Reporting Design Rules".
//  To be used internally only.

// .ERROR HANDLING
//
//  .fill_off
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.
//  .fill_on

// DOCUMENT NO
//      190 89-CAA 109 0171

// AUTHOR
//      1998-03-18 by UAB/I/LN  UABSJEN

// .REVISION
//      A       98-03-18

// CHANGES
//
//      RELEASE REVISION HISTORY
//
//      REV NO          DATE            NAME            DESCRIPTION
//      A               98-03-18        SJEN            First release
//      A               10-10-25        xantior         First release in APG43_linux

// .LINKAGE
//

//*****************************************************************************

#ifndef ACS_APGCC_EVENT_H_
#define ACS_APGCC_EVENT_H_
#  include <string>
#  include <list>
#  include "ACS_APGCC_OS.H"
#  include "ACS_APGCC_EventCodes.H"
#  include "ACS_APGCC_Texts.H"
#  include "acs_aeh_evreport.h"
using namespace std;


//
// Class ACS_APGCC_Event
// Class for reporting Events
//
class ACS_APGCC_Event
{
public:

   typedef std::list<std::string> StringList;

   typedef enum {
      CAT_APZ = 0,
      CAT_APT = 1,
      CAT_EXT = 2,
      CAT_PWR = 3
   } Category;


   ACS_APGCC_Event();
   // Initiation.

   virtual ~ACS_APGCC_Event();
   // Termination.

   virtual bool raise(int eventNo, ...);
   // Description:
   //    Send the event report.
   // Parameters:
   //    eventNo     Event report number.
   //    ...         Variable number of data values that
   //                are to be inserted in to the event report
   //                messages.
   //
   // Return value:
   //    bool        True  - Report was sent successfully.
   //                False - Failed to send event report.
   // Additional information:
   //    -

   void processName(const char* name);
   // Description:
   //    Set the process name.
   // Parameters:
   //    name        Name of process.
   //
   // Return value:
   //    -
   // Additional information:
   //    If no process name is given, it will be retrieved by accessing
   //    the name of the service; e.g. ACS_APGCC::getProcessName.
   void header(ACS_APGCC_Text textNo);/*deprecated*/
   void header(string p_headerText);
    // Description:
   //    Set the header of the event report.
   // Parameters:
   //    header      Text code for header.
   //                See ACS_APGCC_Texts.H.
   //
   // Return value:
   //    -
   // Additional information:
   //    If no header is given, the default header is "AP IO FAULT".

   void objectOfRef(const char* faultyObject, int refNo=-1);
   // Description:
   //    Set the reference of the faulty object.
   // Parameters:
   //    faultyObject   Name of faulty object.
   //    refNo          Instance of the faulty object.
   //
   // Return value:
   //    -
   // Additional information:
   //    The value for objectOfRef is assembled as "faultyObject/refNo".
   //    E.g.  objectOfRef("EXT-1-1",2) becomes "EXT-1-1/2".

   void objectClassOfRef(Category cat);
   // Description:
   //    Set the class of the faulty object.
   // Parameters:
   //    cat      Class of object.
   //
   // Return value:
   //    -
   // Additional information:
   //    Default value is CAT_APZ.

   void problemData(const char* text);
   // Description:
   //    Free text description of the problem.
   // Parameters:
   //    -
   // Return value:
   //    -
   // Additional information:
   //    -
   static bool send( int eventNo,
                       string header,
                       const char* objectOfRef,
                       const char* problemText=0,
                       ...);/*deprecated*/
   static bool send( ACS_APGCC_EventCode eventNo,
                       ACS_APGCC_Text header,
                       const char* objectOfRef,
                       const char* problemText=0,
                       ...); /*deprecated*/
    static  void format_str(string& out, const char* format, va_list& args);
    static bool send(int event, double interval, const char* header, const char* objectOfRef, const char* problem, const char* format, ...);

   // Description:
   //    Shortcut version for sending an event.
   //    No need to define a ACS_APGCC_Event object prior to calling this.
   // Parameters:
   //    event        	Event number
    //   interval       Not used
   //    header         Code for event header
   //    objectOfRef    Referred (faulty) object
   //    problem	    Free text describing the fault which
   //                   is added to the Problem Text field in the
   //                   event report.
   //    ...            Variable number of arguments that correspond
   //                   to the given event number. See ACS_APGCC_ErrorCodes.H.
   //                   These data MUST confirm to the variables given
   //                   in the message file for the event reports.
   //                   The actual problem data text should/must be defined
   //                   in the message file as well.
   // Return value:
   //    -
   // Additional information:
   //    -

protected:

   int eventNo_;
   std::string processName_;
   std::string header_;
   std::string severity_;
   std::string objectRef_;
   std::string objectClass_;
   std::string problemData_;
   StringList alarmText_;
   acs_aeh_evreport* event_;
   bool alarmSent_;

private:

   // Disallow copy
   ACS_APGCC_Event(const ACS_APGCC_Event&);
   ACS_APGCC_Event& operator=(const ACS_APGCC_Event&);
};



//
// Class ACS_APGCC_Alarm
// Class for reporting CP alarms
//
class  ACS_APGCC_Alarm: public ACS_APGCC_Event
{
public:

   typedef std::list<std::string> StringList;

   typedef enum {
      SEV_A1    = 0,
      SEV_A2    = 1,
      SEV_A3    = 2,
      SEV_O1    = 3,
      SEV_O2    = 4,
      SEV_EVENT = 5
   } Severity;


   ACS_APGCC_Alarm(bool ceaseOnExit=true);
   // Initiation.

   virtual ~ACS_APGCC_Alarm();
   // Termination.

   void severity(Severity sev);
   // Description:
   //    Sets the severity of the alarm.
   // Parameters:
   //    sev         Alarm severity
   // Return value:
   //    -
   // Additional information:
   //    Default is A3.

   //void alarmText(const char* text,const char* last, ...);
   void alarmText(const char* text, const char* last=0,...);/*deprecated*/
     void alarmTexts(const char* text,...);
   // Description:
   //    Sets the data strings for problem text.
   // Parameters:
   //    text     Alarm text info.
   //             Each given string is ended with a new line, thus,
   //             the statement alarmText("Line 1", "Line 2", 0) results
   //             in:
   //             Line 1
   //             Line 2
   //
   // Return value:
   //    -
   // Additional information:
   //    WARNING: When using the method with variable arguments make
   //             sure that the last argument is NULL (0).
   //             E.g. alarmText("Line 1","Line 2",0);

   void cease();
   // Description:
   //    Cease the alarm.
   // Parameters:
   //    -
   // Return value:
   //    -
   // Additional information:
   //    -

   void ceaseOnExit(bool TrueFalse);
   // Description:
   //    Set if the alarm should be ceased at exit.
   // Parameters:
   //    TrueFalse      true  - Alarm is ceased at exit.
   //                           This is the default.
   //                   false - Alarm will not be ceased at exit.
   // Return value:
   //    -
   // Additional information:
   //    -

private:

   bool ceaseOnExit_;

private:

   // Disallow copy
   ACS_APGCC_Alarm(const ACS_APGCC_Alarm&);
   ACS_APGCC_Alarm& operator=(const ACS_APGCC_Alarm&);
};



#endif /* ACS_APGCC_EVENT_H_ */
