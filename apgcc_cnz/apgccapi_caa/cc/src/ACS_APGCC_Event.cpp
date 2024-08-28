// #pragma warning(disable:4786)
#include <algorithm>
//#include <strstream>
#include <ace/OS.h>
#include "ACS_APGCC_Event.H"
#include "ACS_APGCC_Util.H"
#include <sstream>
#include <ace/Reactor.h>
#include <ace/Task.h>
#include "ACS_TRA_Logging.h"

//#include "ACS_AEH_EvReport.H"
extern "C" {
#include <stdio.h>
#include <stdarg.h>
}

#include <iostream>
#include <fstream>
using namespace std;

ACS_TRA_Logging ACS_APGCC_Event_logging;

// Support functions

bool is_empty(const string& str)
{
   for (unsigned int i=0; i<str.length(); i++) {
      if ( !::iscntrl(str[i]) )
         return false;
   }
   return true;
}


ACS_APGCC_Event::ACS_APGCC_Event()
:alarmSent_(false)
{
   // Assemble process name
	char procName[64];
	ACE_OS::memset(procName,0,64);
	ACS_APGCC::getProcessName( procName );
	processName_ = procName;
	header_        = "";
	objectRef_     = "";
	objectClass_   = "APZ";
	severity_      = "EVENT";
	event_ = new acs_aeh_evreport;

	ACS_APGCC_Event_logging.Open("APGCC");
}

ACS_APGCC_Event::~ACS_APGCC_Event()
{
   delete event_;
   ACS_APGCC_Event_logging.Close();
}

void ACS_APGCC_Event::processName(const char* name)
{
   processName_ = name;
}

void ACS_APGCC_Event::header(string p_headerText)
{

	header_ = p_headerText;

}
void ACS_APGCC_Event::header(ACS_APGCC_Text textNo)
{

	header_ = ACS_APGCC::getText(textNo);

}

void ACS_APGCC_Event::objectOfRef(const char* faultyObject, int refNo)
{
   char tmp[80];

   if ( refNo != -1 ) {
      sprintf(tmp,"%s/%i",faultyObject,refNo);
      objectRef_ = tmp;
   }
   else {
      objectRef_ = faultyObject;
   }
}

void ACS_APGCC_Event::objectClassOfRef(ACS_APGCC_Event::Category cat)
{

   switch (cat)
   {
   case CAT_APT:
      objectClass_ = "APT";
      break;
   case CAT_EXT:
      objectClass_ = "EXT";
      break;
   case CAT_PWR:
      objectClass_ = "PWR";
      break;
   default:
      objectClass_ = "APZ";
   }
}


void ACS_APGCC_Event::problemData(const char* text)
{
   problemData_ = text;
}


bool ACS_APGCC_Event::raise(int eventNo, ...)
{
   char numstr[20];
   char eventName[30];
   ACS_AEH_ReturnType sent=ACS_AEH_error;
   string tmpProbData("");
   string tmpProb_s("");
   ostringstream tmpProb ("");
   ostringstream tmpAlarm (stringstream::in | stringstream::out);
   StringList::iterator it;
   // PR_01 HMODULE hModule = NULL;
   LPSTR msgBuf =0;
   DWORD bufLen	=0;
   DWORD last_error = 0;
   // Check for alarm and return if already sent
   if ( severity_ != "EVENT" && alarmSent_ )
      return alarmSent_;
   if ( processName_.length() == 0 || objectRef_.length() == 0 )
      return false;
  // Save event number
   if ( eventNo > 0 )
      eventNo_ = eventNo;
  if ( last_error == 0 ) {
	  va_list args;
     // va_list* pargs = 0;
	  va_start(args, eventNo);
	  string msgBuffer;
      const char* tmp = 0;
      tmp = va_arg( args,const char* );
      while (tmp!=0 && strcmp(tmp,"")!=0 ) {
    	  msgBuffer += string(tmp);
    	  tmp=0;
    	  tmp = va_arg( args, const char* );
      }
      va_end(args);
     // Check if we got the message
    if ( msgBuffer.length() != 0 ) {
    // Assemble event description and problem data
          tmpProbData = msgBuffer;
          tmpProb << tmpProbData.c_str();
   }
  }
  if ( last_error != 0 ) {
     // Store fault code
	  ::sprintf(numstr,"%d",last_error);
      tmpProbData = "Failed to retreive message; System error=";
      if ( bufLen > 0 && msgBuf != NULL )
         tmpProbData += msgBuf;
            tmpProb << tmpProbData.c_str();
       tmpProb << " (";
       tmpProb << numstr;
       tmpProb << ")";
    }

  //tmpProb << tmpProbData.c_str();
  // Assemble process name
   ::sprintf(eventName, "%s:%d", processName_.c_str(), ::getpid());
  if ( problemData_.length() > 0 ) {
	  tmpProb << endl;
      tmpProb << problemData_.c_str();
     }

   // Assemble problem (alarm) text
   for (it=alarmText_.begin(); it!=alarmText_.end(); it++) {
      tmpAlarm << it->c_str();
      tmpAlarm << endl;
    }
   // Append null signs
    tmpProb << ends;
    tmpAlarm << ends;
   // Freeze streams
      // Send event report
    sent = event_->sendEventMessage(eventName,               // programName:pid
									eventNo,                // event number
									severity_.c_str(),       // severity
                                   header_.c_str(),
                                   objectClass_.c_str(),
                                   objectRef_.c_str(),      // object reference
                                   tmpProb.str().c_str(),                      // problem data
                                   tmpAlarm.str().c_str());                     // problem (alarm) text
	if (sent== ACS_AEH_error)
    {
            string error="ACS_APGCC_Alarm::send() Failed to send event; AEH error=";
            error+=event_->getErrorText();
            error+=ACS_APGCC_NL;
            //To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_DEBUG,error.c_str()));
            ACS_APGCC_Event_logging.Write(error.c_str(),LOG_LEVEL_DEBUG);
    }
   // Release allocated message buffer
   if ( msgBuf )
      //PR_01 ::LocalFree(msgBuf);
	   delete(msgBuf);
   alarmSent_ = (sent == ACS_AEH_ok);
   return alarmSent_;
}
/*deprecated*/
bool ACS_APGCC_Event::send(ACS_APGCC_EventCode /*p_eventNo*/,
							ACS_APGCC_Text p_header,
							const char* /*objectOfRef*/,
							const char* problemText,
                     ...)
{
	string headerText(ACS_APGCC::getText(p_header));
	   char eventName[30];
	   string tmpProbData;
	   stringstream tmpProb (stringstream::in | stringstream::out);
	   DWORD bufLen;
	   DWORD last_error = 0;
	   if ( last_error == 0 ) {
	      va_list args;
	      va_list* pargs = NULL;
	      char msgBuffer[256];
	      va_start(args, problemText);
	      pargs = &args;
	      bufLen= vsprintf (msgBuffer,problemText, args);
	      va_end(args);
	      // Check if we got the message
	      if ( bufLen > 0 && msgBuffer != 0 ) {
	    	  msgBuffer[bufLen] = 0;
	        // Assemble event description and problem data
	         tmpProbData = msgBuffer;
	      }
	     }
	   // if we failed to load  or retreive message
	   // assemble #  include "acs_aeh_evreport.h"an error message
	   if ( last_error != 0 ) {

	      tmpProbData = "Failed to retreive message; System error=";
	    }
	   // Assemble process name
	   char procName[64];
	  // memset (procName,0,64);
	   ACE_OS::memset(procName,0,64);
	   ACS_APGCC::getProcessName( procName );
	   ::sprintf(eventName, "%s:%d", procName, ::getpid());
	  // Copy to stream
	    tmpProb << tmpProbData.c_str();
	   /*** Problem data is now moved to fill the problem text area*/
	   if ( problemText && ::strlen(problemText) > 0 ) {
		   tmpProb << endl;
	   }
	    // Append null sign
	   tmpProb << ends;
	  // Freeze stream
	//   char* pd = (char*)(tmpProb.str()).c_str();
	   // Send event report
	  // ACS_AEH_EvReport event;
	   /* PR_01 START
	    *
	    *sent = event.sendEventMessage(eventName,              // programName:pid
	                                 eventNo,                // event number
	                                 "EVENT",                // severity
	                                 header.c_str(),
	                                 "APZ",
	                                 objectOfRef,            // object reference
	                                 pd,                     // problem data
	                                 (problemText?problemText:"")); // problem text
		*/
	   return true;
}

/*deprecated*/
bool ACS_APGCC_Event::send(int /*event*/, string /*header*/,const char* /*objectOfRef*/, const char* format, ...)
{
	string text;
		va_list args;
		string name;
		char procName[64];
		char processName[64];
		memset (processName,0,64);
		time_t now;
			   //  ACS_AEH_EvReport apEvent;
			  // ACS_AEH_ReturnType aehRes;
	    try {
			if ( format ) {
				va_start(args, format);
				format_str(text,format,args);
				va_end(args);
			}
			ACS_APGCC::getProcessName(procName);
			sprintf( processName, "%s:%u", procName, ::getpid());
			time( &now );
			 // Send event report
		/*	      aehRes = apEvent.sendEventMessage(processName,            // programName:pid
			                                        event,                  // event number
			                                        "EVENT",                // severity
			                                        header,
			                                        "APZ",
			                                        processName,            // object reference
			                                        text.c_str(),           // problem data
			                                        problem);               // problem (alarm) text

			 */  }
			   catch(...) {
			      return false;
			   }
	return true;
}



bool ACS_APGCC_Event::send(int event,  double /*interval*/,const char* header,const char* objectOfRef, const char* problem, const char* format, ...)
{	//ACE_UNUSED(interval);
	string text;
	va_list args;
	string name;
	char procName[64];
	char processName[64];
	memset (processName,0,64);
    time_t now;
    acs_aeh_evreport apEvent;
	ACS_AEH_ReturnType aehRes;
    try {
		if ( format ) {
			va_start(args, format);
			format_str(text,format,args);
			va_end(args);
		}
		ACS_APGCC::getProcessName(procName);
		sprintf( processName, "%s:%u", procName, ::getpid());
		time( &now );
	     // Send event report
		      aehRes = apEvent.sendEventMessage(processName,            // programName:pid
		                                        event,                  // event number
		                                        "EVENT",                // severity
		                                        header,
		                                        "APZ",
		                                        objectOfRef,            // object reference
		                                        text.c_str(),           // problem data
		                                        problem);               // problem (alarm) text
         if (aehRes== ACS_AEH_error)
         {
        	 string error="ACS_APGCC_Alarm::send() Failed to send event; AEH error=";
        	 error+=apEvent.getErrorText();
        	 error+= ACS_APGCC_NL;
        	 //To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_DEBUG,error.c_str()));
        	 ACS_APGCC_Event_logging.Write(error.c_str(),LOG_LEVEL_DEBUG);
        	 return false;
         }
    }
	catch(...) {
		string error="ACS_APGCC_Alarm::send() Failed to send event; AEH error=";
		error+=apEvent.getErrorText();
		error+=ACS_APGCC_NL;
		//To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_DEBUG,error.c_str()));
		ACS_APGCC_Event_logging.Write(error.c_str(),LOG_LEVEL_DEBUG);
		return false;
	}
	return true;
}


void ACS_APGCC_Event::format_str(string& out, const char* format, va_list& args)
{

   // Assemble output string
   int i = 0;
   int len = ::strlen(format);
   //strstream str;
   stringstream str (stringstream::in | stringstream::out);
   const char* cp;
   int c;
   int n;
   unsigned int un;
   char ns[10];

   try {

      while ( i < len ) {

         switch ( format[i] )
         {
         case '%':
            i++;
            switch (format[i])
            {
            case 'c':
               c = va_arg(args, int);
               str << c;
               break;

            case 's':
               cp = va_arg(args, const char*);
               if (cp)
                  str << cp;
               break;

            case 'i':
               n = va_arg(args, int);
               ::sprintf(ns,"%i",n);
               str << ns;
               break;

            case 'u':
               un = va_arg(args, unsigned int);
               ::sprintf(ns,"%u",un);
               str << ns;
               break;

            case 'd':
               un = va_arg(args, int);
               ::sprintf(ns,"%d",un);
               str << ns;
               break;

            case 'x':
               n = va_arg(args, int);
               ::sprintf(ns,"%x",n);
               str << ns;
               break;

            case 'X':
               n = va_arg(args, int);
               ::sprintf(ns,"%X",n);
               str << ns;
               break;

            case 'p':
             //  str << MCS::getSystemErrorText();
               break;

            case 'q':
               n = va_arg(args, int);
              // str << MCS::getSystemErrorText(n);
               break;

            case 'n':
               //str << endl;
               str << "\r\n";
               break;

            default:
               str << format[i];
            }
            break;

         default:
            str << format[i];
         }

         i++;
      }//while

   }
   catch(...) {
      str << "Caught exception for format string:" << format << endl;
   }

   // Freeze stream
   //str << ends;
  // char* sp = (char*)(str.str()).c_str();
   out = str.str();
  // delete [] sp;
}
//=============================================================
//=============================================================
// Implementation of ACS_APGCC_Alarm
//=============================================================
//=============================================================

ACS_APGCC_Alarm::ACS_APGCC_Alarm(bool ceaseOnExit)
:ceaseOnExit_(ceaseOnExit)
{
   this->severity(SEV_A3);
}


ACS_APGCC_Alarm::~ACS_APGCC_Alarm()
{
   if ( alarmSent_ && ceaseOnExit_ ) {
      // cease alarm
      this->cease();
   }
}


void ACS_APGCC_Alarm::severity(Severity sev)
{

   switch (sev)
   {
   case SEV_A1:
      severity_ = "A1";
      break;
   case SEV_A2:
      severity_ = "A2";
      break;
   case SEV_A3:
      severity_ = "A3";
      break;
   case SEV_O1:
      severity_ = "O1";
      break;
   case SEV_O2:
      severity_ = "O2";
      break;
   default:
      severity_ = "EVENT";
   }
}


void ACS_APGCC_Alarm::cease()
{
   char eventName[30];
   ACS_AEH_ReturnType res;
   string tmpAlarmText;
   if ( alarmSent_ ) {
      // Assemble process name
      ::sprintf(eventName, "%s:%d", processName_.c_str(), ::getpid());
      // Assemble problem (alarm) text
      StringList::iterator it;

      for (it=alarmText_.begin(); it!=alarmText_.end(); it++) {
         tmpAlarmText += it->c_str();
         tmpAlarmText += ACS_APGCC_NL;
      }
       res = event_->sendEventMessage(eventName,              // programName:pid
                                     eventNo_,               // event number
                                     "CEASING",              // this is an event report
                                     header_.c_str(),
                                     objectClass_.c_str(),
                                     objectRef_.c_str(),     // object reference
                                     "",                     // problem data
                                     tmpAlarmText.c_str());   // problem (alarm) text


      if ( res == ACS_AEH_error ) {
    	  string error="ACS_APGCC_Alarm::cease() Failed to cease alarm; AEH error=";
    	  error+=event_->getErrorText();
    	  error+=ACS_APGCC_NL;
    	  //To replace in LSV20 with TRA *DONE* ACE_DEBUG((LM_DEBUG,error.c_str()));
    	  ACS_APGCC_Event_logging.Write(error.c_str(),LOG_LEVEL_DEBUG);
      }
      alarmSent_ = false;
   }
}


void ACS_APGCC_Alarm::ceaseOnExit(bool TrueFalse)
{
   ceaseOnExit_ = TrueFalse;
}
void ACS_APGCC_Alarm::alarmText(const char* data, const char* /*last*/, ...){
	alarmText_.erase(alarmText_.begin(),alarmText_.end()); //HJ13122
	   alarmText_.push_back( data );

	   va_list args;
	 //  va_start(args, data);

	   const char* tmp = 0;

	   tmp = va_arg( args, const char* );

	   while (tmp) {
	      alarmText_.push_back( tmp );
	      tmp = va_arg( args, const char* );
	   }

	   va_end( args );
}
void ACS_APGCC_Alarm::alarmTexts(const char* data/*, const char* last*/, ...)
{
   alarmText_.erase(alarmText_.begin(),alarmText_.end()); //HJ13122
   alarmText_.push_back( data );

   va_list args;
   va_start(args, data);

   const char* tmp = 0;

   tmp = va_arg( args, const char* );

   while (tmp) {
      alarmText_.push_back( tmp );
      tmp = va_arg( args, const char* );
   }

   va_end( args );
}
