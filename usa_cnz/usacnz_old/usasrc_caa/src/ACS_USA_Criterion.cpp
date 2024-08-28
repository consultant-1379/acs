//******************************************************************************
//
// NAME
//      ACS_USA_Criterion.cpp
//
// COPYRIGHT
//      Ericsson AB 2005 - All rights reserved
//
//      The Copyright to the computer program(s) herein 
//      is the property of Ericsson AB, Sweden.
//      The program(s) may be used and/or copied only with the 
//      written permission from Ericsson AB or in accordance 
//      with the terms and conditions stipulated in the 
//      agreement/contract under which the program(s) have been 
//      supplied.
//
// DESCRIPTION
//      ACS_USA_Criterion holds all data conerning a criterion for a 
//	    single log file. 
//	    Methods support initialisation of the object, adding/removing 
//	    criteria, matching a record against criteria, and updating 
//	    alarm generation.
//      Based on 190 89-CAA 109 0259.
//
// DOCUMENT NO
//	    190 89-CAA 109 0545
//
// AUTHOR 
// 	    1995-06-26 by ETX/TX/T XKKHEIN
//
// REV	DATE	NAME 	 DESCRIPTION
// A	950829	XKKHEIN	 First version.
// B	990915	UABDMT	 Ported to Windows NT4.0.
// C	030212	UABPEK	 New AP VIRUS alarms.
// D	040429	QVINKAL	 Removal of RougeWave Tools.h++.
// E	041223	UABPEK	 Supervision of three new event logs.
// F	051011	UABPEK	 Suppression of alarms by named PHA parameter table.
//                       New alarm AP EXTERNAL NETWORK FAULT.
// G	071231	EKAMSBA	 General Error filtering.
// H	080215	EKAMSBA	 Auto cease alarm.
// I	080318	EKAMSBA	 PRC alarms handling.
//
//******************************************************************************

#include <errno.h>
#include <fstream>
#include <iostream>  
#include <stdio.h>
#include <stdlib.h>
#include <windows.h> 

#include <ACS_TRA_Trace.H>
#include "ACS_USA_AnalysisObject.h"
#include "ACS_USA_Common.h"
#include "ACS_USA_Criterion.h"
#include "ACS_USA_Event.h"
#include "ACS_USA_EventManager.h"
#include "ACS_USA_File.h"
#include "ACS_USA_Regexp.h"

using namespace std;   // Using std/stl adapted RW.
                       // The namespace statement has to be placed 
                       // after the include statements to avoid 
                       // conflict with the other headerfiles.

/////////////////////////////////////////////////////////////////////////////
//******************************************************************************
//  Trace control block instance declarations
//******************************************************************************
ACS_TRA_trace traceRaised_and_sent_Alarm =
                                ACS_TRA_DEF("ACS_USA_Raised_and_sent_Alarm", "C300");

ACS_TRA_trace traceAlarmFilterInterval = ACS_TRA_DEF("ACS_USA_Analyser", "C57I");
const char* const ACS_USA_traceAlarmFilterText = 
						"Too early to send alarm on the same event type, interval:";

ACS_TRA_trace ACS_USA_CriterionParameters =
  ACS_TRA_DEF("ACS_USA_CriterionParameters", "l, i, i, C, C");




//========================================================================================
// Constants used in this class
//========================================================================================
const	int ACS_USA_aehStringCount = 8;  // number of inserted strings made by AEH


// Help to auto backup and restore attributes 
// of the ACS_USA_Criterion
ACS_USA_Criterion::AutoBackUpRestoreData::AutoBackUpRestoreData(ACS_USA_Criterion& obj)
			:obj_(obj)
{	
	specificProblem_	= obj_.specificProblem;
	perceivedSeverity_	= obj_.perceivedSeverity; 
	probableCause_		= obj_.probableCause;
	problemText_		= obj_.problemText;
}

ACS_USA_Criterion::AutoBackUpRestoreData::~AutoBackUpRestoreData()
{
	obj_.specificProblem	= specificProblem_;
	obj_.perceivedSeverity	= perceivedSeverity_;
	obj_.probableCause		= probableCause_;
	obj_.problemText		= problemText_;
}






//******************************************************************************
//      ACS_USA_Criterion()
//******************************************************************************
ACS_USA_Criterion::ACS_USA_Criterion() :
  object(),
  scope(initialTime),
  maxAllowed(invalidInteger),
  matchType(unknowknownMatch),
  pattern(),
  specificProblem(0),
  perceivedSeverity(),
  probableCause(),
  objectOfRefSuffix(),
  problemText(),
  command(),
  node(),
  disableAlarmFilter(),
  generalErrorFilter(),
  ceaseDuration(initialTime),
  lastAlarm(initialTime),
  counterShort(initialTime),
  counterLong(initialTime),
  soFarShort(0),
  soFarLong(0),
  subsequent(ACS_USA_False),
  subString(),
  encountered(),
  matchedRecord(),
  regExp(),
  hasRaisedAlarm(ACS_USA_False), // Handling the "zero events allowed" case.
  rcInsertedStrings(),
  manualCeaseData()
{
}

//******************************************************************************
//      ACS_USA_Criterion()
//******************************************************************************
ACS_USA_Criterion::ACS_USA_Criterion(const ACS_USA_Criterion& other) :
  object(other.object),
  scope(other.scope),
  maxAllowed(other.maxAllowed),
  matchType(other.matchType),
  pattern(other.pattern),
  specificProblem(other.specificProblem),
  perceivedSeverity(other.perceivedSeverity),
  probableCause(other.probableCause),
  objectOfRefSuffix(other.objectOfRefSuffix),
  problemText(other.problemText),
  command(other.command),
  node(other.node),
  disableAlarmFilter(other.disableAlarmFilter),
  generalErrorFilter(other.generalErrorFilter),
  ceaseDuration(other.ceaseDuration),
  lastAlarm(other.lastAlarm),
  counterShort(other.counterShort),
  counterLong(other.counterLong),
  soFarShort(other.soFarShort),
  soFarLong(other.soFarLong),
  subsequent(other.subsequent),
  subString(other.subString),
  encountered(other.encountered),
  matchedRecord(other.matchedRecord),
  regExp(other.regExp),
  hasRaisedAlarm(other.hasRaisedAlarm), // Handling the "zero events allowed" case.
  rcInsertedStrings(other.rcInsertedStrings),
  manualCeaseData(other.manualCeaseData)
{
}

//******************************************************************************
//      ~ACS_USA_Criterion()
//******************************************************************************
ACS_USA_Criterion::~ACS_USA_Criterion()
{
}

bool ACS_USA_Criterion::firstMatch (string record, Regexp_MatchType& cas)
{

	string code = "Perceived Severity:";
	string object = "";
	string problem = "";
	string cause = "";
	int i, j = 0;
	cas = ACS_USA_Regexp_noMatch;
    
	// fetch the "Perceived Severity" from event viewer record 
    i,j = 0;
	i = record.find(code);
	if (i > 0)
		i = i + code.length();
	else 
		return false;
	if ((i+1) <  record.length())
		j = record.find_first_of(" ", i+1);
	else 
		return false;

	if (j > 0) 
		cause = record.substr(i+1, j-i-1);
	else 
		return false;	

	// fetch the process Name (object) from pattern
	i = pattern.find_last_of("+");

	if (i > 1 && (i+2) < pattern.length()) 
		object = pattern.substr(i+2);
	else 
		return false;

	//fetch the problem cause from pattern
	problem = pattern.substr(0,i-2);

	j = problem.find_last_of(".+");
	if ((j >0) && (j+2) < problem.length())
		problem = problem.substr(j+2);
	else 
		return false;	

	// perform the match
	if (string::npos != record.find(object))
		if (string::npos != record.find(problem))
			if (string::npos != pattern.find(cause))
				cas = ACS_USA_Regexp_match;

	return true;
}

//******************************************************************************
//      match()
//******************************************************************************
ACS_USA_MatchType
ACS_USA_Criterion::match(
                        const ACS_USA_RecordType record,
						const String fileName,
                        const ACS_USA_TimeType recordTime,
						const ACS_USA_ActivationType act,
						const vector<String>& insertedStrings,
						LPBYTE dataBuffer,
						DWORD  dataLength)
{
    ACS_USA_StatusType ret = Criterion_noMatch;
    ACS_USA_Occurrences* soFar = &soFarShort;
    ACS_USA_TimeType* counterStart = &counterShort;
   
    if ((act == Activation_longTerm) 
		|| (act == Activation_FirstLongTerm))
	{
		soFar = &soFarLong;
		counterStart = &counterLong;
    } 

    //
    // Do appropriate matching
    //
	if (matchType == subStringMatch) 		// Subsequent substrings
	{
			ACS_USA_ByteCount start  = 0;
			ACS_USA_ByteCount length = 0;
			
			// Get the substring
			
			switch(regExp.checkMatch(record, &start, &length)) 
			{
			case ACS_USA_Regexp_match:
				
				// record matched the regular expression.
				// See if it is subsequent
				if (subsequent != ACS_USA_True) 
				{
					// first time
					String a(record + start, length);
					subsequent = ACS_USA_True;
					*soFar = 1;			// one event encountered
					*counterStart = recordTime;
					subString = a;
				} 
				else 
				{
					// another time in a row. Is this same kind of record ?
					String a(record + start, length);
					if (a == subString) 
					{
						if (recordTime <= (*counterStart + scope)) 
						{
							// within the time scope
							
							++(*soFar);
							USA_DEBUG(logMsg("CR:SUBmatch within scope\n"));
								if (*soFar > maxAllowed)
								{
									// .. and more than allowed then 
									// an alarm condition is encountered
									USA_DEBUG(logMsg("CR:SUBmatch cr encountered: %s\n", record));
										
										// New block (1999) for Unix bug correction, uabdmt:
										//
										if (hasRaisedAlarm) 
										{
											// Ignore alarm condition since the criteria 
											// already has caused an alarm during this analysis.
										}
										else  // Raise alarm. Condition not previously
											// detected during this analysis.
										{	
											encountered.push_back(recordTime);
											matchedRecord.push_back(record);
											rcInsertedStrings.push_back( insertedStrings );

											bool manualCease = getManualCeaseData(dataBuffer, dataLength);
											manualCeaseData.push_back( manualCease );

											*soFar = 0;		
											hasRaisedAlarm = ACS_USA_True;
										}
								} 			
						} 
						else 
						{
							// bigger than the time scope
							USA_DEBUG(logMsg("CR:SUBmatch bigger:\n"));
							*counterStart = recordTime;
							*soFar = 1;
						}
						ret = Criterion_match;
					} 
					else 
					{
						String s(record + start, length);
						subsequent = ACS_USA_True;
						*soFar = 1;		// one event encountered
						*counterStart = recordTime;
						subString = s;
					}
				}
				break;
				
			case ACS_USA_Regexp_noMatch:
				
				// 
				// Regular expression didn't match. Clear all "subsequent" data
				//
				*soFar = 0;
				subsequent = ACS_USA_False;
				*counterStart = 0;
				break;
				
			default:
				
				USA_DEBUG(logMsg("CR:SUBmatch error: %s\n", error.getErrorText()));
					// something wrong
					ret = Criterion_matchError;
				break;
			}			// end of switch
	} 
	else 
	{	// Normal matching (both long and short term criteria).

		//switch(regExp.checkMatch(record)) {

		Regexp_MatchType cas;
		if (!firstMatch(record, cas)) {
			cas = regExp.checkMatch(record);
		} 
		switch(cas) {

			case ACS_USA_Regexp_match:
			// Match. Validate time scope
			
			if (*counterStart == 0) {	
				*counterStart = recordTime;
			}
			
			if (recordTime <= (*counterStart + scope)) 
			{
				//tEST
				/*fstream f("c:\\USAmATCH.txt",ios::out|ios::app);
				string s = "RECORDSTRING is: " ;
				f << s<<endl;
				f<< record<<endl;
				s = "mATCHPATTERN IS is: " ;
				f << s<<endl;
				f << this->pattern<<endl;
				s = "Severity IS is: " ;
				f << s<<endl;
				f << this->perceivedSeverity<<endl;	
				s = "*************************************************************" ;
				f << s<<endl<<endl;
				f.close();*/
				// within the time scope
				
				++(*soFar);
				if (*soFar > maxAllowed)
				{
					// .. and more than allowed then 
					// an alarm condition is encountered
					
					// New block (1999) for Unix bug correction, uabdmt:
					//
					if (hasRaisedAlarm) 
					{
						// Ignore alarm condition since the criteria 
						// already has caused an alarm during this analysis.
					}
					else // Raise alarm. Condition not previously
						// detected during this analysis.
					{
						encountered.push_back(recordTime);
						matchedRecord.push_back(record);
						rcInsertedStrings.push_back( insertedStrings );
						
						bool manualCease = getManualCeaseData(dataBuffer, dataLength);
						manualCeaseData.push_back( manualCease );

						*soFar = 0;		
						hasRaisedAlarm = ACS_USA_True;
					}
				}
			} else
			{
				// bigger than the time scope. 
	   			// Save the new start position
				*counterStart = recordTime;
				*soFar = 1;
				//
				// New block (1999) for Unix bug correction, uabdmt:
				if (*soFar > maxAllowed)   
				{        // If no occurences of an event allowed
				encountered.push_back(recordTime);
				matchedRecord.push_back(record);
				rcInsertedStrings.push_back( insertedStrings );			  
					
				bool manualCease = getManualCeaseData(dataBuffer, dataLength);
				manualCeaseData.push_back( manualCease );

				hasRaisedAlarm = ACS_USA_True; 
						// No need for another alarm during this 
						// criterion analysis
				}
			}
			ret = Criterion_match;
		
			case ACS_USA_Regexp_noMatch:

			// Didn't match
			break;

			default:

			// something wrong
			ret = Criterion_matchError;
			break;
		}
    }

    return ret;
}

//******************************************************************************
//      update()
//******************************************************************************
void
ACS_USA_Criterion::update(const ACS_USA_Criterion& other)
{
    USA_DEBUG(logMsg("CR:update match: %s\n", pattern.data()));
    lastAlarm = other.lastAlarm;
}

//******************************************************************************
//  raiseAlarm()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Criterion::raiseAlarm(
			ACS_USA_AnalysisObject *ao,
			unsigned int alarmFilterInterval,
			const vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms,
			vector<ACS_USA_EventInfo>& eventsToCease)
{
  ACS_USA_StatusType ret = ACS_USA_ok;
  if ((encountered.empty() == FALSE) ||
    (matchedRecord.empty() == FALSE)) 
  {
	  if (encountered.size() !=  matchedRecord.size()) {
	    // Something went wrong during the saving of matchedRecord 
	    // and encountered time. 
	    return ACS_USA_error; //return immediately.
	  }

  	// Create Object of reference from log file in the Analysis Object
	  // the criterion belongs to.
	  String objectOfReference(objectOfRefPrefix);
	  String logFileName(ao->getLogFile());
    size_t idx = last(logFileName,slashChar);
		if (idx != String::npos) {
			logFileName.erase(0, idx + 1);
		}
	  toUpper(logFileName);
	  objectOfReference += logFileName;
	  objectOfReference += "-";
	  objectOfReference += objectOfRefSuffix;

	  // Every entry in the vector for either encountered or 
	  // matchedRecord is each an alarm, go through the vectors
	  // and raise alarm for each entry.
    for (unsigned int vecNr = 0; vecNr < encountered.size(); vecNr++) {
	  	if (encountered[vecNr] > lastAlarm) {
        if (maxAllowed == 0) {
				  // Filter alarms. Don't generate a burst of the same 
  				// alarm on "no-occurences-allowed-events".
		  		// If max is zero, ie no occurrences of the event are allowed, 
			  	// only raise an alarm if a time period that is long enough has passed.
				  // The time period is a USA PHA parameter, and is as such configurable.
  				if (disableAlarmFilter != "yes") {
		  		  // Only filter alarms if the default filter is enabled for the 
			    	// criterion. A filter can only be disabled in the 
				    // ACS_USA_Config.acf file. The filter shall in general only be 
	  		  	// disabled if an alarm-cease combination is used.
	  				if (encountered[vecNr] < lastAlarm + static_cast<long>(alarmFilterInterval)) {										 
					  	// The default value of alarmFilterInterval is 1 minute,
					  	// making sure that the same alarm is not sent more often.
					  	if(ACS_TRA_ON(traceAlarmFilterInterval)) {
					  		ACS_TRA_event(
                  &traceAlarmFilterInterval,
							    ACS_USA_traceAlarmFilterText,alarmFilterInterval);
						  } 
						  vecNr++ ;	// Don't raise alarm -
						  continue;	// ignore the matched record by stepping	in the loop.
					  }
				  }
        }
        // Replace illegal chars with '.' characters
		    // to ensure that AP event report will not fail.
		    int changeIllegal = 0;
		    String tmpStr = matchedRecord[vecNr];
		    int lastIndex = tmpStr.length();
		    for (int i = 0; i < lastIndex; i++) {
			    char c = tmpStr[(size_t)i]; // String needs "size_t".
			    if ((c < ' ' && c != '\t') || c > '~') {
				    tmpStr[(size_t)i] = '.'; // String needs "size_t".
				    changeIllegal = 1; 
			    }
		    }
		    if (changeIllegal == 1) {
		      matchedRecord[vecNr] = tmpStr;
			    changeIllegal = 0;
		    }

	      // Check if alarm should be suppressed or not.
	      vector<ACS_USA_SuppressedAlarmsElement*>::const_iterator it;
        // Scan row by row the suppressed alarms table to find a match
	      // between the event instance and the suppressed alarms row.
	      for (it = suppressedAlarms.begin(); it != suppressedAlarms.end(); it++) {
		      ACS_USA_SuppressedAlarmsElement* suppressedAlarmsElement = *it;
		      if (suppressedAlarmsElement->specificProblem == specificProblem) {
			      // The 'specificProblem' field  matches.
			      if (suppressedAlarmsElement->objectOfRefSuffix == objectOfRefSuffix) {
				      // Also the 'objectOfRefSuffix' field matches.
              return ret; // Suppress alarm.
            }
          }
        }

	// Try to report.
	if (eventManager.report(
		specificProblem,
		perceivedSeverity,
		probableCause,
		objectOfReference,
		matchedRecord[vecNr],
		problemText,
		node,
		0,
		(isAutoCeaseEvent() == ACS_USA_False)
		) == ACS_USA_error) 
	{
		ret = ACS_USA_error;
	}
	else 
	{
		// Consider alarm time instead of event time
		ACS_USA_Time currentTime;

		if(ACS_TRA_ON(traceRaised_and_sent_Alarm)) 
		{
			String alm(matchedRecord[vecNr]);
			String balm("Raised and sent alarm to AEHEvReport ==>");
			balm.append(alm);
			ACS_TRA_event(&traceRaised_and_sent_Alarm, balm.data());
		}

		// Report successful. Mark the last alarm generation time.
		lastAlarm = encountered[vecNr];

		if ( isAutoCeaseEvent() == ACS_USA_True )
		{
			eventsToCease.push_back( 
				ACS_USA_EventInfo( currentTime.seconds() // Alarm time   
							, ceaseDuration      // Alarm duration
							, specificProblem   
							, objectOfReference) 
					);
		}
	}

	    USA_DEBUG(logMsg("CR:raising Alarm: %s\n", pattern.data()));
    
        // Execute the command if supplied.
	      if (!(command.empty())) {
	  	    if (system(command.data()) == systemCallError) {
		    	  String a(beginQuote);
			      error.setError(ACS_USA_SystemCallError);
		        a += Sys_systemCall;
		   	    a += endQuote;
		   	    a += ACS_USA_systemCallResult;
		        error.setErrorText(sysErrorPrefix, a.data(), errno);

		        // Try to report only.
			      if (eventManager.report(
              specificProblem,
					    String(Key_perceivedSeverity_EVENT),
					    String(ACS_USA_FaultCause),
				      String(),
				      String(error.getErrorText()),
				      String(error.getProblemText(ACS_USA_SystemCallError)),
			        "",	
			        0) == ACS_USA_error) 
		        {
			        ret = ACS_USA_error;
	    	    }
            else {
		          error.setError(ACS_USA_NoError);
	      	    error.setErrorText(noErrorText);
            } 
	    	  }
	      }
	    }
	  } // End of for-loop.
  }
  return ret;
}

//******************************************************************************
//  raiseCustomAlarm()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Criterion::raiseCustomAlarm(
								 ACS_USA_AnalysisObject *ao,
								 unsigned int alarmFilterInterval,
								 const vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms,
								 vector<ACS_USA_EventInfo>& eventsToCease)
{
	// 
	// This function, in difference to raiseAlarm(), is called when we want to use the event log 
	// data instead of the ACF data.



	// Save some class attributes 
	// because we'll override them...
	AutoBackUpRestoreData backUp(*this);



	ACS_USA_StatusType ret = ACS_USA_ok;
	if ((encountered.empty() == FALSE) ||
		(matchedRecord.empty() == FALSE)) 
	{
		if (encountered.size() !=  matchedRecord.size()) 
		{
			// Something went wrong during the saving of matchedRecord 
			// and encountered time. 
			return ACS_USA_error; //return immediately.
		}

		// Every entry in the vector for either encountered or 
		// matchedRecord is each an alarm, go through the vectors
		// and raise alarm for each entry.
		for (unsigned int vecNr = 0; vecNr < encountered.size(); vecNr++) 
		{
			String processName( "" );		   // process name to use;
			String objectOfReference( "" );	   // Object of reference to use;
			String fetchedMatchedRecord( "" ); // Matched record to use;

			// strings are expected to be logged by AEH, 
			// so the number should be 8 or more, if AEH has been extended.
			const ACS_USA_InsertedStrings& param = rcInsertedStrings[vecNr];
			if( param.size() < ACS_USA_aehStringCount )
			{
				// Something wrong with received inserted strings
				error.setError(ACS_USA_BadState);
				error.setErrorText("ACS_USA_Criterion::raiseCustomAlarm(): wrong number of inserted strings.");
				return ACS_USA_error;
			}

			// update criterion attributes with the new value
			for(int index=0; index< param.size(); index++)
			{
				int result = 0;

				String str = param[index];
				switch( index )
				{
				case 0:
					// Process Name:
					processName = str;
					break;
				case 1:
					// Specific problem: 
					// override only if not defined in ACF file.
					if(specificProblem == 0)
					{
						result = sscanf(str.data(), "%d", &specificProblem);
						if (result == 0 || result == EOF) 
						{
							ret = ACS_USA_error;
						}
					}
					break;
				case 2:
					// Perceived Severity:					
					// => Always use the one from the custom ACF file
					break;
				case 3:
					// Probable Cause:
					// override only if not defined in ACF file.
					if( probableCause.empty() )
					{
						probableCause = str;
					}
					break;
				case 4:
					// Object Class of Reference:	
					// already defined to APZ
					break;
				case 5:
					// Object of Reference:
					objectOfReference = str;
					break;
				case 6:
					// Problem Data:
					fetchedMatchedRecord = str;					
					break;
				case 7:
					// Problem Text: 
					// override only if not defined in ACF file.
					if( problemText.empty() )
					{
						problemText = str;
					}
					break;
				default:
					break;
				}

				// Something wrong with received inserted strings
				if(ret == ACS_USA_error)
				{					
					error.setError(ACS_USA_BadState);
					error.setErrorText("ACS_USA_Criterion::raiseCustomAlarm(): received wrong inserted strings.");
					return ACS_USA_error;
				}
			}


			if (encountered[vecNr] > lastAlarm) 
			{
				if (maxAllowed == 0) 
				{
					// Filter alarms. Don't generate a burst of the same 
					// alarm on "no-occurences-allowed-events".
					// If max is zero, ie no occurrences of the event are allowed, 
					// only raise an alarm if a time period that is long enough has passed.
					// The time period is a USA PHA parameter, and is as such configurable.
					if (disableAlarmFilter != "yes") 
					{
						// Only filter alarms if the default filter is enabled for the 
						// criterion. A filter can only be disabled in the 
						// ACS_USA_Config.acf file. The filter shall in general only be 
						// disabled if an alarm-cease combination is used.
						if (encountered[vecNr] < lastAlarm + static_cast<long>(alarmFilterInterval)) 
						{										 
							// The default value of alarmFilterInterval is 1 minute,
							// making sure that the same alarm is not sent more often.
							if(ACS_TRA_ON(traceAlarmFilterInterval)) 
							{
								ACS_TRA_event(
									&traceAlarmFilterInterval,
									ACS_USA_traceAlarmFilterText,alarmFilterInterval);
							} 
							vecNr++ ;	// Don't raise alarm -
							continue;	// ignore the matched record by stepping	in the loop.
						}
					}
				}

				
				// Check if alarm should be suppressed or not.
				vector<ACS_USA_SuppressedAlarmsElement*>::const_iterator it;
				// Scan row by row the suppressed alarms table to find a match
				// between the event instance and the suppressed alarms row.
				for (it = suppressedAlarms.begin(); it != suppressedAlarms.end(); it++) 
				{
					ACS_USA_SuppressedAlarmsElement* suppressedAlarmsElement = *it;
					if (suppressedAlarmsElement->specificProblem == specificProblem) 
					{
						// The 'specificProblem' field  matches.
						if (suppressedAlarmsElement->objectOfRefSuffix == objectOfRefSuffix) 
						{
							// Also the 'objectOfRefSuffix' field matches.
							return ret; // Suppress alarm.
						}
					}
				}

				// Try to report.
				if (eventManager.report(
									processName,
									specificProblem,
									perceivedSeverity,
									probableCause,
									objectOfReference,
									fetchedMatchedRecord,
									problemText,
									node,
									0,
									manualCeaseData[vecNr]
									) == ACS_USA_error) 
				{
					ret = ACS_USA_error;
				}
				else 
				{
					// Consider alarm time instead of event time
					ACS_USA_Time currentTime;

					if(ACS_TRA_ON(traceRaised_and_sent_Alarm)) 
					{
						String alm(matchedRecord[vecNr]);
						String balm("Raised and sent alarm to AEHEvReport ==>");
						balm.append(alm);
						ACS_TRA_event(&traceRaised_and_sent_Alarm, balm.data());
					}

					// Report successful. Mark the last alarm generation time.
					lastAlarm = encountered[vecNr];

					if ( isAutoCeaseEvent() == ACS_USA_True )
					{
						eventsToCease.push_back( 
							ACS_USA_EventInfo( currentTime.seconds() // Alarm time   
							, ceaseDuration      // Alarm duration
							, specificProblem   
							, objectOfReference) 
							);
					}
				}

				USA_DEBUG(logMsg("CR:raising Alarm: %s\n", pattern.data()));

				// Execute the command if supplied.
				if (!(command.empty())) 
				{
					if (system(command.data()) == systemCallError) 
					{
						String a(beginQuote);
						error.setError(ACS_USA_SystemCallError);
						a += Sys_systemCall;
						a += endQuote;
						a += ACS_USA_systemCallResult;
						error.setErrorText(sysErrorPrefix, a.data(), errno);

						// Try to report only.
						if (eventManager.report(
							specificProblem,
							String(Key_perceivedSeverity_EVENT),
							String(ACS_USA_FaultCause),
							String(),
							String(error.getErrorText()),
							String(error.getProblemText(ACS_USA_SystemCallError)),
							"",	
							0) == ACS_USA_error) 
						{
							ret = ACS_USA_error;
						}
						else 
						{
							error.setError(ACS_USA_NoError);
							error.setErrorText(noErrorText);
						} 
					}
				}
			}
		} // End of for-loop.
	}

	return ret;
}

//******************************************************************************
//	read()
//******************************************************************************

ACS_USA_StatusType
ACS_USA_Criterion::read(File& file)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                      !
	// ! if changing that function will impacts the file ‘usa.tmp’, !
	// ! Then increment the revision ‘ACS_USA_UsaTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// 
  // Read the data from 'file' into current object
  //
	try
	{
		file >> pattern;
		file >> perceivedSeverity;
		file >> probableCause;
		file >> objectOfRefSuffix;
		file >> problemText;
		file >> command;
		file >> node;
		file >> disableAlarmFilter;
		file >> generalErrorFilter;

		file >> subString;
		
 		if (file.Error() ||
			  file.Read(scope)           == FALSE ||
			  file.Read(maxAllowed)      == FALSE ||
			  file.Read(matchType)       == FALSE ||
			  file.Read(specificProblem) == FALSE ||
			  file.Read(ceaseDuration)   == FALSE ||

			  file.Read(lastAlarm)       == FALSE ||
			  file.Read(counterShort)    == FALSE ||
			  file.Read(counterLong)     == FALSE ||
			  file.Read(soFarShort)      == FALSE ||
			  file.Read(soFarLong)       == FALSE ||
			  file.Read(subsequent)      == FALSE) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(ACS_USA_ReadFailed);
			USA_DEBUG(logMsg("CR:read error:\n"));
			return ACS_USA_error;
		}
	}
	catch (...)
	{
		error.setError(ACS_USA_FileIOError);
		error.setErrorText(ACS_USA_ReadFailed);
		USA_DEBUG(logMsg("AO:read error: %s\n", error.getErrorText()));
		return ACS_USA_error;
	}
	return ACS_USA_ok;
}

//******************************************************************************
//	write()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Criterion::write(File& file)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                      !
	// ! if changing that function will impacts the file ‘usa.tmp’, !
	// ! Then increment the revision ‘ACS_USA_UsaTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  // 
  // Write data to 'file'
  //
  //VRK added try loop
  try
  {
		file << pattern;
		file << perceivedSeverity;
		file << probableCause;
		file << objectOfRefSuffix;
		file << problemText;
		file << command;
 		file << node;
		file << disableAlarmFilter;
		file << generalErrorFilter;

		file << subString;

		if (file.Error() || 
			file.Write(scope)           == FALSE ||
			file.Write(maxAllowed)      == FALSE ||
			file.Write(matchType)       == FALSE ||
			file.Write(specificProblem) == FALSE ||
			file.Write(ceaseDuration)   == FALSE ||

  			file.Write(lastAlarm)       == FALSE ||
  			file.Write(counterShort)    == FALSE ||
  			file.Write(counterLong)     == FALSE ||
  			file.Write(soFarShort)      == FALSE ||
  			file.Write(soFarLong)       == FALSE ||
  			file.Write(subsequent)      == FALSE)
		{
  			error.setError(ACS_USA_FileIOError);
  			error.setErrorText(ACS_USA_WriteFailed);
  			USA_DEBUG(logMsg("CR:write error:\n"));
			return ACS_USA_error;
		}
  }
  catch(...)
  {
		error.setError(ACS_USA_FileIOError);
  		error.setErrorText(ACS_USA_WriteFailed);
  		USA_DEBUG(logMsg("CR:write error:\n"));
		return ACS_USA_error;
  }
  return ACS_USA_ok;
}

//******************************************************************************
//	validate()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Criterion::validate()
{
  // Check if all required data are present.
  if (scope == initialTime ||
      maxAllowed == invalidInteger ||
      matchType == invalidType ||
	  pattern.empty() ||
	 perceivedSeverity.empty())
  {
    if (ACS_TRA_ON(ACS_USA_CriterionParameters))
    {
      // Collect criterion parameters.
      ACS_TRA_event(&ACS_USA_CriterionParameters,
        scope,
        maxAllowed,
        matchType, 
        pattern,
        perceivedSeverity);
    }
	  error.setError(ACS_USA_SyntaxError);
    error.setErrorText(ACS_USA_MissingVitalParameter);
    return ACS_USA_error;
  }
  // Check if the regular expression is ok.
  if (regExp.compilePattern(pattern) == ACS_USA_error)
  {
	  // regexp sets the error text & code
	  return ACS_USA_error;
  }
  return ACS_USA_ok;
}

//******************************************************************************
//	setScope()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Criterion::setScope(String& str)
{
    ACS_USA_StatusType ret = ACS_USA_ok;
	toUpper(str);
	String a = ACS_USA_timeSpecificationChars;
	size_t idx;
	for( unsigned int i = 1; i < a.length() -1; i++)
	{
		String tmp = a.substr(i,1);
		 idx = str.find(tmp);
		if (idx != String::npos) 
			break;
	}
        
    if ((scope = toInt(str, minScope, maxScope)) == invalidLong) {
	error.setError(ACS_USA_SyntaxError);
	error.setErrorText(ACS_USA_IllegalValue);
	return ACS_USA_error;
    } 
    
    if (idx == String::npos) {
	return ACS_USA_error;
    }
    
    // Calculate seconds according to specified time scope
    switch(str[idx]) {
    
	case days:	scope = hoursInDay * minutesInHour * secondsInMinute * scope;
			break;
			
	case hours:	scope = minutesInHour * secondsInMinute * scope;
			break;
			
	case minutes:	scope = secondsInMinute * scope;
			break;
			
	case seconds:	// already ok
			break;
			
	default:	ret = ACS_USA_error;
			break;
	
    }
    return ret;

}

//******************************************************************************
//	setMaxAllowed()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Criterion::setMaxAllowed(const String& str)
{
  if ((maxAllowed = toInt(str, minEventsAllowed, maxEventsAllowed)) == invalidInteger)
  {
	  error.setError(ACS_USA_SyntaxError);
	  error.setErrorText(ACS_USA_IllegalValue);
	  return ACS_USA_error;
  }
  else
  {
	  return ACS_USA_ok;
  }
}

//******************************************************************************
//      setMatchType()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Criterion::setMatchType(String& str)
{
    // Is it a "PATTERN" or "SUBSTRING" .. 
	toUpper(str);
 
    if (str == Key_substringMatch) {
	matchType = subStringMatch;
    } else if (str == Key_paternMatch) {
	matchType = patternMatch;
    } else {
	// .. neither. Return error
	error.setError(ACS_USA_SyntaxError);
	error.setErrorText(ACS_USA_IllegalValue);
	return ACS_USA_error;
    }
    return ACS_USA_ok;

}

//******************************************************************************
//	setSpecificProblem()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Criterion::setSpecificProblem(const String& str)
{
  if ((specificProblem = toInt(str, minSpecificProblem, maxSpecificProblem)) ==
      invalidLong)
  {
    error.setError(ACS_USA_SyntaxError);
    error.setErrorText(ACS_USA_IllegalValue);
    return ACS_USA_error;
  }
  else
  {
	  return ACS_USA_ok;
  }
}

//******************************************************************************
//	setPerceivedSeverity()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Criterion::setPerceivedSeverity(String& str)
{
	toUpper(str);
    
  if (str == Key_perceivedSeverity_A1 ||
      str == Key_perceivedSeverity_A2 ||
      str == Key_perceivedSeverity_A3 ||
      str == Key_perceivedSeverity_O1 ||
      str == Key_perceivedSeverity_O2 ||
      str == Key_perceivedSeverity_CEASING ||
      str == Key_perceivedSeverity_EVENT ||
      str == Key_perceivedSeverity_NONE)
  {
    perceivedSeverity = str;
    return ACS_USA_ok;
  }
  else
  {
    error.setError(ACS_USA_SyntaxError);
	  error.setErrorText(ACS_USA_IllegalValue);
	  return ACS_USA_error;
  }
}

//******************************************************************************
// setProbableCause()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Criterion::setProbableCause(String& str)
{
	toUpper(str);
    
  if (str == ACS_USA_APAntivirusFunctionFault ||
      str == ACS_USA_APExternalNetworkFault ||
      str == ACS_USA_APFault ||
      str == ACS_USA_APVirus ||
      str == ACS_USA_IllegalLogonAttempts)
  {
    probableCause = str;
    return ACS_USA_ok;
  }
  else
  {
	  error.setErrorText(ACS_USA_IllegalValue);
	  return ACS_USA_error;
  }
}

//******************************************************************************
//	setGeneralErrorFilter()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Criterion::setGeneralErrorFilter(String& str)
{
    // Is it a "ON" or "OFF" .. 
    toUpper(str);
 
    if (str == Key_GEFilterON || 
        str == Key_GEFilterOFF) 
    {
        generalErrorFilter = str;
    } 
    else 
    {
        // .. neither. Return error
        error.setError(ACS_USA_SyntaxError);
        error.setErrorText(ACS_USA_IllegalValue);
        return ACS_USA_error;
    }
    return ACS_USA_ok;
}

//******************************************************************************
//  setCeaseDuration()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Criterion::setCeaseDuration(String& str)
{
    ACS_USA_StatusType ret = ACS_USA_ok;
    toUpper(str);

    if ((ceaseDuration = toInt(str, minCeaseDuration , maxCeaseDuration)) == invalidLong) 
    {
        error.setError(ACS_USA_SyntaxError);
        error.setErrorText(ACS_USA_IllegalValue);
        return ACS_USA_error;
    } 

    size_t idx = str.find_first_of( ACS_USA_timeSpecificationChars );
    if (idx == String::npos) 
    {
        return ACS_USA_error;
    }


    // Calculate seconds according to the specified delta time
    switch(str[idx]) 
    {
    case days:  
        ceaseDuration = hoursInDay * minutesInHour * secondsInMinute * ceaseDuration;
        break;

    case hours: 
        ceaseDuration = minutesInHour * secondsInMinute * ceaseDuration;
        break;

    case minutes:   
        ceaseDuration = secondsInMinute * ceaseDuration;
        break;

    case seconds:   
        // No need to conversion as, ceaseDuration is already given in seconds
        break;

    default:    
        ret = ACS_USA_error;
        break;

    }

    return ret;
}

//******************************************************************************
//	toInt()
//******************************************************************************
int
ACS_USA_Criterion::toInt(
			 const String& str,
			 const int min,
			 const int max) const
{
    int val = invalidInteger;
    int ret = sscanf(str.data(), "%d", &val);
    
    if (ret == 0) {
    	return invalidInteger;
    } else if (ret == EOF) {
    	return invalidInteger;
    } else {
	if (val < min || val > max) {
	    return invalidInteger;
	}
    }
    return val;
}



//******************************************************************************
//	toInt()
//******************************************************************************
long
ACS_USA_Criterion::toInt(
			 const String& str,
			 const long min,
			 const long max) const
{
  ACS_USA_SpecificProblem val = invalidLong;
  int ret = sscanf(str.data(), "%ld", &val);
  if (ret == 0)
  {
    return invalidLong;
  }
  else if (ret == EOF)
  {
    return invalidLong;
  }
  else
  {
	  if (val < min || val > max)
    {
	    return invalidLong;
	  }
  }
  return val;
}

//******************************************************************************
//	getManualCeaseData()
//******************************************************************************
bool
ACS_USA_Criterion::getManualCeaseData(
						LPBYTE dataBuffer,
						DWORD  dataLength) const
{
	if( dataBuffer == NULL ||
		dataLength != sizeof(bool) 
	)
	{
		// return default value
		return true;
	}

	return *((bool*)dataBuffer);
}

//******************************************************************************
//      operator==()
//******************************************************************************
ACS_USA_Boolean 
ACS_USA_Criterion::operator==(const ACS_USA_Criterion& other) const
{
    //
    // Criteria are considered equal if they are of same type
    // and patterns are equal
    //
    if (matchType == other.matchType) {
	if (pattern == other.pattern) {
	    return ACS_USA_True;
	} 
    }
    
    return ACS_USA_False;
    
}

//******************************************************************************
//      dump()
//******************************************************************************
void
ACS_USA_Criterion::dump()
{
  USA_DEBUG(logMsg("----------------- Criterion ---------------\n"));
  USA_DEBUG(logMsg("object:             %s\n", object.data()));
  USA_DEBUG(logMsg("scope:              %d\n", scope));
  USA_DEBUG(logMsg("maxAllowed:         %d\n", maxAllowed));
  USA_DEBUG(logMsg("matchType:          %d\n", matchType));
  USA_DEBUG(logMsg("pattern:            %s\n", pattern.data()));
  USA_DEBUG(logMsg("specificProblem:    %d\n", specificProblem));
  USA_DEBUG(logMsg("perceivedSeverity:  %s\n", perceivedSeverity.data()));
  USA_DEBUG(logMsg("probableCause:      %s\n", probableCause.data()));
  USA_DEBUG(logMsg("objectOfRefSuffix:  %s\n", objectOfRefSuffix.data()));
  USA_DEBUG(logMsg("problemText:        %s\n", problemText.data()));
  USA_DEBUG(logMsg("command:            %s\n", command.data()));
  USA_DEBUG(logMsg("node:               %s\n", node.data()));
  USA_DEBUG(logMsg("disableAlarmFilter: %s\n", disableAlarmFilter.data()));
  USA_DEBUG(logMsg("generalErrorFilter: %s\n", generalErrorFilter.data()));
  USA_DEBUG(logMsg("ceaseDuration:      %d\n", ceaseDuration));
  USA_DEBUG(logMsg("lastAlarm:          %d\n", lastAlarm));
}

//******************************************************************************
