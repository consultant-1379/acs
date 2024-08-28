//******************************************************************************
//
// NAME
//      acs_usa_criterion.cpp
//
// COPYRIGHT Ericsson AB, Sweden 2003.
// All rights reserved.
//
// The Copyright to the computer program(s) herein is the property
// of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with the written
// permission from Ericsson AB
// or in accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have been supplied.
//
// DESCRIPTION
//     code for usa service.
//
//
// DOCUMENT NO
//      ----
//
// AUTHOR
//
//
// CHANGES
//
//      REV NO          DATE            NAME            DESCRIPTION
//                      2015-02-03      XNAZBEG         syslog alarm handling
//                                                      for TR HT42889 fix
//                      2018-03-16      XFURULL         Fix for HW69050
//                      2018-06-01      XNAZBEG		Fix for HW70461
//							and cleanup of changes done for TR HT42889
//******************************************************************************
#include <errno.h>
#include <fstream>
#include <iostream>  
#include <stdio.h>
#include <stdlib.h>

#include "acs_usa_analysisObject.h"
#include "acs_usa_common.h"
#include "acs_usa_criterion.h"
#include "acs_usa_event.h"
#include "acs_usa_eventManager.h"
#include "acs_usa_file.h"
#include "acs_usa_regexp.h"
#include "acs_usa_tratrace.h"
#include "acs_usa_error.h"
#include "acs_usa_tokenizer.h"
#include "acs_usa_logtrace.h"

using namespace std;   // Using std/stl adapted RW.
                       // The namespace statement has to be placed 
                       // after the include statements to avoid 
                       // conflict with the other headerfiles.

/////////////////////////////////////////////////////////////////////////////
//******************************************************************************
//  Trace control block instance declarations
//******************************************************************************

ACS_USA_Trace traCrit("ACS_USA_Criterion	");

const char* ACS_USA_traceAlarmFilterText = "Too early to send alarm on the same event type object [%s], interval: [%d]";


//========================================================================================
// Constants used in this class
//========================================================================================
const	unsigned int ACS_USA_aehStringCount = 8;  // number of inserted strings made by AEH

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
	obj_.probableCause	= probableCause_;
	obj_.problemText	= problemText_;
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
  ceaseDuration(initialTime),
  lastAlarm(initialTime),
  counterShort(initialTime),
  counterLong(initialTime),
  counterInst(initialTime),
  soFarShort(0),
  soFarLong(0),
  soFarInst(0),
  subsequent(ACS_USA_False),
  subString(),
  encountered(),
  matchedRecord(),
  userNameQue(),
  rcInsertedStrings(),
  manualCeaseData(),
  regExp(),
  hasRaisedAlarm(ACS_USA_False), // Handling the "zero events allowed" case.
  userName(),
  userRecordList(),
  instAlarmTimeStamp(),
  nAlarmFilterInterval(0)
{
	USA_TRACE_ENTER2("Constructor");

	USA_TRACE_LEAVE2("Constructor");
}

//******************************************************************************
//      ACS_USA_Criterion() Copy Constructor
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
  ceaseDuration(other.ceaseDuration),
  lastAlarm(other.lastAlarm),
  counterShort(other.counterShort),
  counterLong(other.counterLong),
  counterInst(other.counterInst),
  soFarShort(other.soFarShort),
  soFarLong(other.soFarLong),
  soFarInst(other.soFarInst),
  subsequent(other.subsequent),
  subString(other.subString),
  encountered(other.encountered),
  matchedRecord(other.matchedRecord),
  userNameQue(other.userNameQue),
  rcInsertedStrings(other.rcInsertedStrings),
  manualCeaseData(other.manualCeaseData),
  regExp(other.regExp),
  hasRaisedAlarm(other.hasRaisedAlarm), // Handling the "zero events allowed" case.
  userName(other.userName),
  userRecordList(other.userRecordList),
  instAlarmTimeStamp(other.instAlarmTimeStamp),
  nAlarmFilterInterval(other.nAlarmFilterInterval)
{
	USA_TRACE_ENTER2("Constructor: ACS_USA_Criterion(ACS_USA_Criterion&)");

	USA_TRACE_LEAVE2("Constructor: ACS_USA_Criterion(ACS_USA_Criterion&)");

}

//******************************************************************************
//      ACS_USA_Criterion() Assignment operator
//******************************************************************************
ACS_USA_Criterion& ACS_USA_Criterion::operator=(const ACS_USA_Criterion& other)
{
	USA_TRACE_ENTER2("Constructor: ACS_USA_Criterion(ACS_USA_Criterion&)");
	if (this != &other)	
	{			
		object = other.object;
		scope = other.scope;
		maxAllowed = other.maxAllowed;
		matchType = other.matchType;
		pattern = other.pattern;
		specificProblem = other.specificProblem;
		perceivedSeverity = other.perceivedSeverity;
		probableCause = other.probableCause;
		objectOfRefSuffix = other.objectOfRefSuffix;
		problemText = other.problemText;
		command = other.command;
		node = other.node;
		disableAlarmFilter = other.disableAlarmFilter;
		ceaseDuration = other.ceaseDuration;
		lastAlarm = other.lastAlarm;
		counterShort = other.counterShort;
		counterLong = other.counterLong;
		counterInst = other.counterInst;
		soFarShort = other.soFarShort;
		soFarLong = other.soFarLong;
		soFarInst = other.soFarInst;
		subsequent = other.subsequent;
		subString = other.subString;
		encountered = other.encountered;
		matchedRecord = other.matchedRecord;
		userNameQue = other.userNameQue;
		rcInsertedStrings = other.rcInsertedStrings;
		manualCeaseData = other.manualCeaseData;
		regExp = other.regExp;
		hasRaisedAlarm = other.hasRaisedAlarm; // Handling the "zero events allowed" case.
		userName = other.userName;
		userRecordList = other.userRecordList;
		instAlarmTimeStamp = other.instAlarmTimeStamp;
		nAlarmFilterInterval = other.nAlarmFilterInterval;
	}
	USA_TRACE_LEAVE2("Constructor: ACS_USA_Criterion(ACS_USA_Criterion&)");

	return *this;
}

//******************************************************************************
//      ~ACS_USA_Criterion()
//******************************************************************************
ACS_USA_Criterion::~ACS_USA_Criterion()
{
	USA_TRACE_ENTER2("Destuctor");
	USA_TRACE_LEAVE2("Destuctor");
}

bool ACS_USA_Criterion::isObjectMatched (string &record)
{
	USA_TRACE_ENTER2("Enter->");

	size_t objPos = record.rfind(object);
	if ((std::string::npos) != objPos){
		size_t delimPos = record.find_first_of (" :[", objPos);
		if ((std::string::npos) != delimPos){
			std::string actualObj =  record.substr(objPos,(delimPos-objPos));
			if( (object.length()) == (actualObj.length()) ){
				USA_TRACE_LEAVE2("<-Leave");
				return true;
			}
		}
	}
	traCrit.traceAndLogFmt(INFO, "match: object:[%s updated with position:[%u]", object.data(), objPos);
	USA_TRACE_LEAVE2("<-Leave");
	return false;
}

bool ACS_USA_Criterion::firstMatch (string record, Regexp_MatchType& cas)
{
	USA_TRACE_ENTER2("Pattern[%s]", pattern.data());
	cas = ACS_USA_Regexp_noMatch;
	//USA_TRACE_1("record:[%s]", record.data());

	// perform the match
	if( isObjectMatched(record) ) {
		if (string::npos != record.find(pattern)){
			//USA_TRACE_1("first match found");
			cas = ACS_USA_Regexp_match;
			String pamfor("pam for");
			String invaliduser("invalid user");
			size_t pos = record.find(pamfor);
			if(pos != string::npos) {
				//USA_TRACE_1("pam for found");
				String str1 = record.substr(pos+pamfor.length()+1, record.length());
				pos = str1.find(invaliduser);
				if(pos != string::npos) {
					//USA_TRACE_1("invaid user found");
					str1 =  str1.substr(pos+invaliduser.length()+1, str1.length());
				}
				pos = str1.find(" ");
				userName = str1.substr(0, pos);
				USA_TRACE_1("userName [%s]", userName.data());
			}
		}
	}
	
	USA_TRACE_LEAVE();

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
			LPSTR dataBuffer,
			DWORD  dataLength)
{

	USA_TRACE_ENTER();

	ACS_USA_MatchType ret = Criterion_noMatch;
	ACS_USA_Occurrences* soFar = &soFarShort;
	ACS_USA_TimeType* counterStart = &counterShort;

	(void)fileName;

	if ((act == Activation_longTerm) || (act == Activation_FirstLongTerm)) {
		soFar = &soFarLong;
		counterStart = &counterLong;
	} 
	if(act == ACS_USA_Instant_Analysis) {
		soFar = &soFarInst;
		counterStart = &counterInst;
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
								else  // Raise alarm. Condition not previously
									// detected during this analysis.
								{	
									encountered.push_back(recordTime);
									matchedRecord.push_back(record);
									userNameQue.push_back(userName);
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
				
					// something wrong
					ret = Criterion_matchError;
				break;
		}			// end of switch
	} 
	else 
	{	// Normal matching (both long and short term criteria).

		//switch(regExp.checkMatch(record)) 
		USA_TRACE_1("else case of match");

		Regexp_MatchType cas;
		if (!firstMatch(record, cas)) {
			cas = regExp.checkMatch(record);
		}
		USA_TRACE_1("object[%s] firstMatch[%s]", object.data(), (cas == ACS_USA_Regexp_match)?"Match Found" : "Not matched");

		switch(cas) {

			case ACS_USA_Regexp_match:
			// Match. Validate time scope
			if (*counterStart == 0) {	
				*counterStart = recordTime;
			}
					
			if ( maxAllowed != 0 ) {
				++(*soFar);
				if ( (*soFar > maxAllowed) || recordTime <= (*counterStart + scope) ) {
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

					if (*soFar > maxAllowed)
					{
						if(recordTime > (lastAlarm + static_cast<long>(nAlarmFilterInterval))) {
							hasRaisedAlarm = ACS_USA_False;
						}
						// .. and more than allowed then 
						// an alarm condition is encountered
						if( (true == ACS_USA_Global::instance()->m_ignrSshdEvents) && ((object.compare("sshd") == 0) || (object.compare("dhcpd") == 0)) ) {
							lastAlarm = recordTime;
							traCrit.traceAndLogFmt(INFO, "match: lastAlarm:[%u] updated with recordTime:[%u]", lastAlarm, recordTime);
							hasRaisedAlarm = ACS_USA_False;
						}

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
							int instAlarmCount = instAlarmTimeStamp.size();
							bool instAlarmSent = false;
							if((act == ACS_USA_ShortTerm_Analysis) && (instAlarmCount > 0)) {
								for(int i = 0; i < instAlarmCount; i++){
									if(instAlarmTimeStamp[i] == recordTime) {
										instAlarmSent = true;
									}
								}
							}

							if(!instAlarmSent)
							{
								String a(record);
								//size_t lastString = a.find_last_of(':');
								size_t lastString = a.rfind("Stopping");
								if(lastString == String::npos)
								lastString = a.rfind("Stopped");
								else if(lastString == String::npos)
								lastString = a.rfind("Failed");
								if(lastString != String::npos)
								{
									a = a.substr(lastString, a.length()-lastString);
								}
								usacc_strip(a, ' ');
								encountered.push_back(recordTime);
								matchedRecord.push_back(a);
								userNameQue.push_back(userName);
								rcInsertedStrings.push_back( insertedStrings );
								USA_TRACE_1("object[%s] matchedRecord.size[%lu]", object.data(), matchedRecord.size());

								bool manualCease = getManualCeaseData(dataBuffer, dataLength);
								manualCeaseData.push_back( manualCease );

								if(act == ACS_USA_Instant_Analysis) {
									instAlarmTimeStamp.push_back(recordTime);
								}
							}

							*soFar = 0;		
							hasRaisedAlarm = ACS_USA_True;
							*counterStart = recordTime;
						}
					}
				}
				else{
					*counterStart = recordTime;
				}
			}
		       	else {
				// bigger than the time scope. 
	   			// Save the new start position
				*counterStart = recordTime;
				++(*soFar);
				//
				// New block (1999) for Unix bug correction, uabdmt:
				if (*soFar > maxAllowed)   
				{        // If no occurences of an event allowed
					int instAlarmCount = instAlarmTimeStamp.size();
					bool instAlarmSent = false;
					if((act == ACS_USA_ShortTerm_Analysis) && (instAlarmCount > 0)) {
						for(int i = 0; i < instAlarmCount; i++){
							if(instAlarmTimeStamp[i] == recordTime) {
								instAlarmSent = true;
							}
						}
					}

					if(!instAlarmSent)
					{
						String a(record);
						 //size_t lastString = a.find_last_of(':');
                                                size_t lastString = a.rfind("Stopping");
                                                if(lastString == String::npos)
                                                lastString = a.rfind("Stopped");
                                                else if(lastString == String::npos)
                                                lastString = a.rfind("Failed");
                                                if(lastString != String::npos)
                                                {
                                                  a = a.substr(lastString, a.length()-lastString);
                                                }

						usacc_strip(a, ' ');
						encountered.push_back(recordTime);
						matchedRecord.push_back(a);
						userNameQue.push_back(userName);
						rcInsertedStrings.push_back( insertedStrings );			  
					
						bool manualCease = getManualCeaseData(dataBuffer, dataLength);
						manualCeaseData.push_back( manualCease );
						if(act == ACS_USA_Instant_Analysis) {
							instAlarmTimeStamp.push_back(recordTime);
						}
					}

					*soFar = 0;		
					//hasRaisedAlarm = ACS_USA_True; 
						// No need for another alarm during this 
						// criterion analysis
				}
			}

			ret = Criterion_match;
			break;	
			case ACS_USA_Regexp_noMatch:
			// Didn't match
			break;

			default:

			// something wrong
			ret = Criterion_matchError;
			break;
		}
    	}

	USA_TRACE_LEAVE();
    	return ret;
}

ACS_USA_ReturnType ACS_USA_Criterion::reset()
{

	USA_TRACE_ENTER();
	
	if(hasRaisedAlarm == ACS_USA_True)
	{
		//encountered.clear();
		//matchedRecord.clear();
		//userNameQue.clear();
		//rcInsertedStrings.clear();
		//manualCeaseData.clear();
		
		encountered.erase(encountered.begin(), encountered.end());
		matchedRecord.erase(matchedRecord.begin(), matchedRecord.end());
		userNameQue.erase(userNameQue.begin(), userNameQue.end());
		rcInsertedStrings.erase(rcInsertedStrings.begin(), rcInsertedStrings.end());
		manualCeaseData.erase(manualCeaseData.begin(), manualCeaseData.end());
		
	/*	std::vector<ACS_USA_TimeType>(encountered).swap(encountered);
		std::vector<String>(matchedRecord).swap(matchedRecord);
		std::vector<String>(userNameQue).swap(userNameQue);
		std::vector<ACS_USA_InsertedStrings>(rcInsertedStrings).swap(rcInsertedStrings);
		std::vector<bool>(manualCeaseData).swap(manualCeaseData); */
		encountered.resize(1);
		matchedRecord.resize(1);
		userNameQue.resize(1);
		rcInsertedStrings.resize(1);
		manualCeaseData.resize(1);

		hasRaisedAlarm = ACS_USA_False;
	}

	USA_TRACE_LEAVE();

	return ACS_USA_Ok;
}

ACS_USA_ReturnType ACS_USA_Criterion::clearInstAlarmsQue(ACS_USA_TimeType lastShortRecordTime)
{
	USA_TRACE_1("Enter-> ACS_USA_Criterion::clearInstAlarmsQue -  instAlarmTimeStamp-size: %d", (int)instAlarmTimeStamp.size());
        vector<ACS_USA_TimeType>::iterator instAlarmTimeStampIter = instAlarmTimeStamp.begin();
        while(instAlarmTimeStampIter != instAlarmTimeStamp.end())
        {
                if(lastShortRecordTime > *instAlarmTimeStampIter) {
                        instAlarmTimeStampIter = instAlarmTimeStamp.erase(instAlarmTimeStampIter);
                }
                else {
                        ++instAlarmTimeStampIter;
                }
        }
	USA_TRACE_1("<-Leave ACS_USA_Criterion::clearInstAlarmsQue -  instAlarmTimeStamp-size: %d", (int)instAlarmTimeStamp.size());
	return ACS_USA_Ok;
}
//******************************************************************************
//      update()
//******************************************************************************
void
ACS_USA_Criterion::update(const ACS_USA_Criterion& other)
{
	USA_TRACE_ENTER();

    	lastAlarm = other.lastAlarm;
	
	USA_TRACE_LEAVE();
}

//******************************************************************************
//  raiseAlarm()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_Criterion::raiseAlarm(
			ACS_USA_AnalysisObject *ao,
			unsigned int alarmFilterInterval,
			const vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms,
			vector<ACS_USA_EventInfo>& eventsToCease)
{
   
	USA_TRACE_ENTER();
	
	ACE_UNUSED_ARG(ao);
	ACS_USA_ReturnType ret = ACS_USA_Ok;
	if ((encountered.empty() == FALSE) || (matchedRecord.empty() == FALSE)) 
	{
		if (encountered.size() !=  matchedRecord.size()) {
			// Something went wrong during the saving of matchedRecord 
			// and encountered time. 
			return ACS_USA_Error; //return immediately.
		}
		nAlarmFilterInterval = alarmFilterInterval;

		// Create Object of reference from log file in the Analysis Object
		// the criterion belongs to.
		String objectOfReference(objectOfRefPrefix);
		objectOfReference = objectOfRefSuffix;

		// Every entry in the vector for either encountered or 
		// matchedRecord is each an alarm, go through the vectors
		// and raise alarm for each entry.
		
		USA_TRACE_1("object[%s] probableCause[%s] encountered.size() [%lu]", object.data(), probableCause.data(), encountered.size());
		traCrit.traceAndLogFmt(INFO, "object[%s] probableCause[%s] encountered.size() [%lu]", object.data(), probableCause.data(), encountered.size());
		for (unsigned int vecNr = 0; vecNr < encountered.size(); vecNr++) {
			USA_TRACE_1("Alarm  sending vecNr[%d] encountered[%lu] lastAlarm[%lu]", vecNr, encountered[vecNr], lastAlarm);

			//check the ignore sshd events flag, if it is true and object is sshd, then ignore the events - Implemented to ignore events in case of csadm
			if((true == ACS_USA_Global::instance()->m_ignrSshdEvents)){
				bool flag = false;
				if( object.compare("sshd")==0 ){
					traCrit.traceAndLogFmt(INFO, "raiseAlarm: Ignoring sshd events, because of csadm operation");
					flag = true;
				}
				else if( object.compare("dhcpd")==0 ){
					traCrit.traceAndLogFmt(INFO, "raiseAlarm: Ignoring dhcpd events, because of csadm operation");
					flag = true;
				}

				if(flag)
					return ret;
			}

			if (encountered[vecNr] > lastAlarm) {
				if (maxAllowed == 0) {
					// Filter alarms. Don't generate a burst of the same 
					// alarm on "no-occurences-allowed-events".
					// If max is zero, ie no occurrences of the event are allowed, 
					// only raise an alarm if a time period that is long enough has passed.
					// The time period is a USA PHA parameter, and is as such configurable.
					if (strcasecmp(disableAlarmFilter.data(),DISABLE_ALARM_FILTER) == 0){
						// Only filter alarms if the default filter is enabled for the 
						// criterion. A filter can only be disabled in the 
						// ACS_USA_Config.acf file. The filter shall in general only be 
						// disabled if an alarm-cease combination is used.
						if (encountered[vecNr] < lastAlarm + static_cast<long>(alarmFilterInterval)) {										 
							// The default value of alarmFilterInterval is 1 minute,
							// making sure that the same alarm is not sent more often. 
							traCrit.traceAndLogFmt(INFO, ACS_USA_traceAlarmFilterText, object.data(), alarmFilterInterval);

							vecNr++ ;	// Don't raise alarm -
							continue;	// ignore the matched record by stepping	in the loop.
						}
					}
				}
				if ((maxAllowed != 0) && (encountered[vecNr] < lastAlarm + static_cast<long>(alarmFilterInterval))) {
					traCrit.traceAndLogFmt(INFO, ACS_USA_traceAlarmFilterText, object.data(), alarmFilterInterval);
					vecNr++ ;
					USA_TRACE_1("Alarm  is within filter interval discording");
					continue;
				}
				// Replace illegal chars with '.' characters
				// to ensure that AP event report will not fail.
				int changeIllegal = 0;
				String tmpStr = matchedRecord[vecNr];
				int lastIndex = tmpStr.length();
				for (int i = 0; i < lastIndex; i++) {
					char c = tmpStr[(size_t)i]; // String needs "size_t".
					if ((c == 39) || (c < ' ' && c != '\t') || c > '~'){
						tmpStr[(size_t)i] = ' '; // String needs "size_t".
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
				bool userMatched = false;
				if( !userNameQue[vecNr].empty()) {
					ACS_USA_UserTimeMap::iterator iter;
					for(iter = userRecordList.begin(); iter != userRecordList.end(); iter++)
					{
						if(iter->first == userNameQue[vecNr]){
							if(encountered[vecNr] < iter->second + static_cast<long>(alarmFilterInterval)) {
								userMatched = true;
								break;
							}
						}
					}
				}
				
				if(userMatched)
				{

					USA_TRACE_1("Alarm  already raised with the user");
					continue;
				}
				String problemTextStr(problemText);
				int rc=0;
				if(probableCause == String("AP FUNCTION NOT AVAILABLE") )
				{
					time_t curTime =  encountered[vecNr]; //Reopened HQ34970_TR_FIX
					struct tm stTm;
					struct tm *ptm = NULL;
					char tempBuf[1024] = "";
					char curDate[20] = "";
					char curTimeStr[20] = "";
					//::time(&curTime); //Reopened HQ34970_TR_FIX
					ptm = localtime_r(&curTime, &stTm);
					if(ptm != NULL)
					{
						sprintf(curDate, "%04d-%02d-%02d",stTm.tm_year+1900, stTm.tm_mon+1, stTm.tm_mday);
						sprintf(curTimeStr, "%02d:%02d:%02d",stTm.tm_hour, stTm.tm_min, stTm.tm_sec);
					}
		
					//sprintf(tempBuf, "%-16s %s\r\n%-16s %s\r\n\r\n%-16s %-14s %s\r\n%-16s %-14s %s\n", "RESOURCE GROUP", "PROCESS", "", object.data(), "CAUSE", "DATE", "TIME", "Process res", curDate, curTimeStr); //Reopened HQ34970_TR_FIX
					sprintf(tempBuf, "%-16s %s\r\n%-16s %s\r\n\r\n%-20s %-14s %s\r\n%-20s %-14s %s\n", "RESOURCE GROUP", "PROCESS", "", object.data(), "CAUSE", "DATE", "TIME", "Process restarted", curDate, curTimeStr); //Reopened HQ34970_TR_FIX
					problemTextStr = tempBuf;
					matchedRecord[vecNr]="Task terminated";
					manualCeaseAlarm= ACS_USA_False;
					
					rc=eventManager.report(specificProblem, perceivedSeverity, probableCause,
                                                objectOfReference, matchedRecord[vecNr], problemTextStr, node,
                                                0,manualCeaseAlarm, userNameQue[vecNr]);
					traCrit.traceAndLogFmt(INFO, "ReturnCode:[%d] object:[%s] manualcease:[%b]",rc,object.data(),manualCeaseAlarm);

				}
				else if(probableCause == String("AP LDAP COMMUNICATION FAULT") ){
					matchedRecord[vecNr]="User Authorization failure at LDAP server";
					time_t curTime =  encountered[vecNr];
                                        struct tm stTm;
                                        struct tm *ptm = NULL;
                                        char tempBuf[1024] = "";
                                        char curDate[20] = "";
                                        char curTimeStr[20] = "";
                                        //::time(&curTime); 
                                        ptm = localtime_r(&curTime, &stTm);
                                        if(ptm != NULL)
                                        {
                                                sprintf(curDate, "%04d-%02d-%02d",stTm.tm_year+1900, stTm.tm_mon+1, stTm.tm_mday);
                                                sprintf(curTimeStr, "%02d:%02d:%02d",stTm.tm_hour, stTm.tm_min, stTm.tm_sec);
                                        }

                                        //sprintf(tempBuf, "%-16s %s\r\n%-16s %s\r\n\r\n%-16s %-14s %s\r\n%-16s %-14s %s\n", "RESOURCE GROUP", "PROCESS", "", object.data(), "CAUSE", "DATE", "TIME", "Process res", curDate, curTimeStr); //Reopened HQ34970_TR_FIX
                                        sprintf(tempBuf, "%-20s %-14s %s\r\n%-20s %-14s %s\n","CAUSE", "DATE", "TIME", "Authorization Failed", curDate, curTimeStr);
                                        problemTextStr = tempBuf;
					manualCeaseAlarm= ACS_USA_True;
					objectOfReference="ldap";
					traCrit.traceAndLogFmt(INFO, "raising alarm for object:[%s]",object.data());

					rc=eventManager.report(specificProblem, perceivedSeverity, probableCause,
                                                objectOfReference, matchedRecord[vecNr], problemTextStr, node,
                                                0,manualCeaseAlarm, userNameQue[vecNr]);
					traCrit.traceAndLogFmt(INFO, "ReturnCode:[%d] object:[%s] manualcease:[%b]",rc,object.data(),manualCeaseAlarm);
				}
				else{
					rc=eventManager.report(specificProblem, perceivedSeverity, probableCause,
                                                objectOfReference, matchedRecord[vecNr], problemTextStr, node,
                                                0,manualCeaseAlarm, userNameQue[vecNr]);
					traCrit.traceAndLogFmt(INFO, "ReturnCode:[%d] object:[%s] manualcease:[%b]",rc,object.data(),manualCeaseAlarm);
				}
				
				// Try to report.
				if ( rc == ACS_USA_Error ) 
				{
					USA_TRACE_1("Alarm Failed to send vecNr[%d] probableCause[%s]", vecNr, probableCause.data());
					ret = ACS_USA_Error;
				}
				else 
				{
					// Consider alarm time instead of event time
					ACS_USA_Time currentTime;
					// TRACE OUT here
					USA_TRACE_1("[%s] Alarm sent probableCause[%s]",object.data(), probableCause.data());
					traCrit.traceAndLogFmt(INFO, "Raised and sent alarm to AEHEvReport ==> %s - %s",object.data(), matchedRecord[vecNr].data());
					syslog(LOG_INFO,"[%s] Alarm sent probableCause[%s]",object.data(), probableCause.data());
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
					if( !userNameQue[vecNr].empty()) {
						userRecordList.insert(ACS_USA_UserTimePair(userNameQue[vecNr], encountered[vecNr]));	
					}
				} 


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
								String(ACS_USA_PERCEIVED_SEVERITY_EVENT),
								String(ACS_USA_INTLFAULT),
								String(),
								String(error.getErrorText()),
								String(error.getProblemText(ACS_USA_SystemCallError)),
								"",	
								0) == ACS_USA_Error) 
						{
							ret = ACS_USA_Error;
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

	USA_TRACE_LEAVE();

	return ret;
	
}

//******************************************************************************
//  raiseCustomAlarm()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_Criterion::raiseCustomAlarm(
								 ACS_USA_AnalysisObject *ao,
								 unsigned int alarmFilterInterval,
								 const vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms,
								 vector<ACS_USA_EventInfo>& eventsToCease)
{

	USA_TRACE_ENTER();
	ACE_UNUSED_ARG(ao);
	// This function, in difference to raiseAlarm(), is called when we want to use the event log 
	// data instead of the ACF data.



	// Save some class attributes 
	// because we'll override them...
	AutoBackUpRestoreData backUp(*this);

	ACS_USA_ReturnType ret = ACS_USA_Ok;
	if ((encountered.empty() == FALSE) ||
		(matchedRecord.empty() == FALSE)) 
	{
		if (encountered.size() !=  matchedRecord.size()) 
		{
			// Something went wrong during the saving of matchedRecord 
			// and encountered time. 
			return ACS_USA_Error; //return immediately.
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
				return ACS_USA_Error;
			}
			// update criterion attributes with the new value
			for(unsigned int index=0; index< param.size(); index++)
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
						result = sscanf(str.data(), "%d", (int*)&specificProblem);
						if (result == 0 || result == EOF) 
						{
							ret = ACS_USA_Error;
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
				if(ret == ACS_USA_Error)
				{					
					error.setError(ACS_USA_BadState);
					error.setErrorText("ACS_USA_Criterion::raiseCustomAlarm(): received wrong inserted strings.");
					return ACS_USA_Error;
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
					if (strcasecmp(disableAlarmFilter.data(),DISABLE_ALARM_FILTER) != 0){
						// Only filter alarms if the default filter is enabled for the 
						// criterion. A filter can only be disabled in the 
						// ACS_USA_Config.acf file. The filter shall in general only be 
						// disabled if an alarm-cease combination is used.
						if (encountered[vecNr] < lastAlarm + static_cast<long>(alarmFilterInterval)) 
						{										 
							// The default value of alarmFilterInterval is 1 minute,
							// making sure that the same alarm is not sent more often.
							// TRACE OUT here
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
									) == ACS_USA_Error) 
				{
					ret = ACS_USA_Error;
				}
				else 
				{
					// Consider alarm time instead of event time
					ACS_USA_Time currentTime;
					
					// TRACE OUT Here

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
							String(ACS_USA_PERCEIVED_SEVERITY_EVENT),
							String(ACS_USA_INTLFAULT),
							String(),
							String(error.getErrorText()),
							String(error.getProblemText(ACS_USA_SystemCallError)),
							"",	
							0) == ACS_USA_Error) 
						{
							ret = ACS_USA_Error;
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

	USA_TRACE_LEAVE();

	return ret;
}

//******************************************************************************
//	read()
//******************************************************************************

ACS_USA_ReturnType
ACS_USA_Criterion::read(File& file)
{
	USA_TRACE_ENTER();

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
		file >> object;
		file >> pattern;
		file >> perceivedSeverity;
		file >> probableCause;
		file >> objectOfRefSuffix;
		file >> problemText;
		file >> command;
		file >> node;
		file >> disableAlarmFilter;

		file >> subString;
		
 		if (file.Error() ||
			  file.Read(scope)           == FALSE ||
			  file.Read(maxAllowed)      == FALSE ||
			  file.Read(matchType)       == FALSE ||
			  file.Read(specificProblem) == FALSE ||
			  file.Read(ceaseDuration)   == FALSE ||

			  file.Read(lastAlarm)       == FALSE ||
			  file.Read(counterShort)    == FALSE ||
			  file.Read(soFarShort)      == FALSE ||
			  file.Read(subsequent)      == FALSE) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(ACS_USA_ReadFailed);
			USA_TRACE_LEAVE();
			return ACS_USA_Error;
		}
		traCrit.traceAndLogFmt(INFO, "ACS_USA_Criterion:%s() - pattern[%s] perceivedSeverity[%s] probableCause[%s] objectOfRefSuffix[%s] problemText[%s] command[%s] node[%s] disableAlarmFilter[%s]", __func__, pattern.data(), perceivedSeverity.data(), probableCause.data(), objectOfRefSuffix.data(), problemText.data(), command.data(), node.data(), disableAlarmFilter.data());
//		traCrit.traceAndLogFmt(INFO, "ACS_USA_Criterion:%s() - scope[%d] maxAllowed[%d] matchType[%d] specificProblem[%d] ceaseDuration[%d] lastAlarm[%d] counterShort[%d] soFarShort[%d] subsequent[%d]", __func__, scope, maxAllowed, matchType, specificProblem, ceaseDuration, lastAlarm, counterShort, soFarShort, subsequent);
	}
	catch (...)
	{
		error.setError(ACS_USA_FileIOError);
		error.setErrorText(ACS_USA_ReadFailed);
		return ACS_USA_Error;
	}

	USA_TRACE_LEAVE();
	return ACS_USA_Ok;
}

//******************************************************************************
//	write()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_Criterion::write(File& file)
{
	USA_TRACE_ENTER();

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
		traCrit.traceAndLogFmt(INFO, "ACS_USA_Criterion:%s() - pattern[%s] perceivedSeverity[%s] probableCause[%s] objectOfRefSuffix[%s] problemText[%s] command[%s] node[%s] disableAlarmFilter[%s]", __func__, pattern.data(), perceivedSeverity.data(), probableCause.data(), objectOfRefSuffix.data(), problemText.data(), command.data(), node.data(), disableAlarmFilter.data());
	//		traCrit.traceAndLogFmt(INFO, "ACS_USA_Criterion:%s() - scope[%d] maxAllowed[%d] matchType[%d] specificProblem[%d] ceaseDuration[%d] lastAlarm[%d] counterShort[%d] soFarShort[%d] subsequent[%d]", __func__, scope, maxAllowed, matchType, specificProblem, ceaseDuration, lastAlarm, counterShort, soFarShort, subsequent);
		file << object;
		file << pattern;
		file << perceivedSeverity;
		file << probableCause;
		file << objectOfRefSuffix;
		file << problemText;
		file << command;
 		file << node;
		file << disableAlarmFilter;

		file << subString;

		if (file.Error() || 
			file.Write(scope)           == FALSE ||
			file.Write(maxAllowed)      == FALSE ||
			file.Write(matchType)       == FALSE ||
			file.Write(specificProblem) == FALSE ||
			file.Write(ceaseDuration)   == FALSE ||

  			file.Write(lastAlarm)       == FALSE ||
  			file.Write(counterShort)    == FALSE ||
  			file.Write(soFarShort)      == FALSE ||
  			file.Write(subsequent)      == FALSE)
		{
  			error.setError(ACS_USA_FileIOError);
  			error.setErrorText(ACS_USA_WriteFailed);
  			USA_TRACE_LEAVE();
			return ACS_USA_Error;
		}
  }
  catch(...)
  {
		error.setError(ACS_USA_FileIOError);
  		error.setErrorText(ACS_USA_WriteFailed);
  		USA_TRACE_LEAVE();
		return ACS_USA_Error;
  }

  USA_TRACE_LEAVE();
  return ACS_USA_Ok;
}

//******************************************************************************
//	validate()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_Criterion::validate()
{

	USA_TRACE_ENTER();

	// Check if all required data are present.
	if (scope == initialTime ||
		maxAllowed == invalidInteger ||
		matchType == ACS_USA_InvalidType ||
		pattern.empty() ||
		perceivedSeverity.empty())
	{
		// TRACE OUT here  
		error.setError(ACS_USA_SyntaxError);
		error.setErrorText(ACS_USA_MissingVitalParameter);
		USA_TRACE_1("scope[%lu] maxAllowed[%d] matchType[%d] pattern.empty()[%d] perceivedSeverity.empty()[%d] Leaving", scope, maxAllowed, matchType, pattern.empty(), perceivedSeverity.empty());
		USA_TRACE_LEAVE();
		return ACS_USA_Error;
	}
	// Check if the regular expression is ok.
	if (regExp.compilePattern(pattern) == ACS_USA_Error)
	{
		USA_TRACE_LEAVE2("ACS_USA_Criterion:%s() - regExp.compilePattern failed Leaving", __func__);
		return ACS_USA_Error;
	}

	USA_TRACE_LEAVE();
	return ACS_USA_Ok;
}

//******************************************************************************
//	setScope()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_Criterion::setScope(ACS_USA_TimeType value)
{
	scope = value;
	return ACS_USA_Ok;

}

//******************************************************************************
//	setMaxAllowed()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_Criterion::setMaxAllowed(ACS_USA_Occurrences value)
{
	USA_TRACE_ENTER();
	
	maxAllowed = value;
	if (value < minEventsAllowed || value >  maxEventsAllowed)
	{
		return ACS_USA_Error;
	}
	else
	{
		return ACS_USA_Ok;
	}

	USA_TRACE_LEAVE();
}

//******************************************************************************
//      setMatchType()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_Criterion::setMatchType(String& str)
{
	// Is it a "PATTERN" or "SUBSTRING" .. 
    	
	USA_TRACE_ENTER();

	usacc_toUpper(str);	
	if (str == Key_substringMatch) {
	       matchType = subStringMatch;
        }else if (str == Key_paternMatch) {
		matchType = patternMatch;
	} else {
		// .. neither. Return error
		USA_TRACE_LEAVE();
		return ACS_USA_Error;
	}

	USA_TRACE_LEAVE();
	return ACS_USA_Ok;
}

//******************************************************************************
//	setSpecificProblem()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_Criterion::setSpecificProblem(ACS_USA_SpecificProblem value)
{
	USA_TRACE_ENTER();

	specificProblem = value;
	if (value < ACS_USA_SPECIF_PROBELM_MIN || value > ACS_USA_SPECIF_PROBELM_MAX)
	{
		USA_TRACE_LEAVE2("ACS_USA_IllegalValue");
		return ACS_USA_Error;
	}
	else
	{
		USA_TRACE_LEAVE();
		return ACS_USA_Ok;
	}
}

//******************************************************************************
//	setPerceivedSeverity()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_Criterion::setPerceivedSeverity(String& str)
{
	USA_TRACE_ENTER();

	usacc_toUpper(str);
    
	if (	str == ACS_USA_PERCEIVED_SEVERITY_A1 ||
		str == ACS_USA_PERCEIVED_SEVERITY_A2 ||
		str == ACS_USA_PERCEIVED_SEVERITY_A3 ||
		str == ACS_USA_PERCEIVED_SEVERITY_O1 ||
		str == ACS_USA_PERCEIVED_SEVERITY_O2 ||
		str == ACS_USA_PERCEIVED_SEVERITY_CEASING ||
		str == ACS_USA_PERCEIVED_SEVERITY_EVENT ||
		str == ACS_USA_PERCEIVED_SEVERITY_NONE)
	{
		perceivedSeverity = str;
		USA_TRACE_LEAVE();
		return ACS_USA_Ok;
	}
	else
	{
		error.setError(ACS_USA_SyntaxError);
		error.setErrorText(ACS_USA_IllegalValue);
		USA_TRACE_LEAVE2("ACS_USA_IllegalValue");
		return ACS_USA_Error;
	}
}

//******************************************************************************
// setProbableCause()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_Criterion::setProbableCause(String& str)
{
	USA_TRACE_ENTER();

	usacc_toUpper(str);
    
	if (	str == ACS_USA_APFAULT ||
		str == ACS_USA_EXTERNAL_NETWK_FAULT ||
		str == ACS_USA_ILLGAL_LOGON_ATTEMPTS ||
		str == ACS_USA_AP_PROCESS_STOPPED ||
		str == ACS_USA_AP_FUNCTION_NOT_AVAILABLE ||
		str == ACS_USA_AP_LDAP_COMMUNICATION_FAULT )
	{
		probableCause = str;
		USA_TRACE_LEAVE();
		return ACS_USA_Ok;
	}
	else
	{
		USA_TRACE_LEAVE2("ACS_USA_IllegalValue");
		return ACS_USA_Error;
	}
}


//******************************************************************************
//  setCeaseDuration()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_Criterion::setCeaseDuration(ACS_USA_TimeType  value)
{
	USA_TRACE_ENTER();

	ACS_USA_ReturnType ret = ACS_USA_Ok;
	
	ceaseDuration = value;
	if (value < minCeaseDuration || value >  maxCeaseDuration) 
	{
		error.setError(ACS_USA_SyntaxError);
		error.setErrorText(ACS_USA_IllegalValue);
		USA_TRACE_LEAVE2("ACS_USA_IllegalValue");
		return ACS_USA_Error;
	} 

	USA_TRACE_LEAVE();
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
	USA_TRACE_ENTER();

	int val = invalidInteger;
	int ret = sscanf(str.data(), "%d", &val);
    
	if (ret == 0) {
		return invalidInteger;
	} else if (ret == EOF) {
    		return invalidInteger;
	} else {
		if (val < min || val > max) {
			USA_TRACE_LEAVE2("invalidInteger");
			return invalidInteger;
		}
    	}

	USA_TRACE_LEAVE();
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
	USA_TRACE_ENTER();

	ACS_USA_SpecificProblem val = invalidLong;
	int ret = sscanf(str.data(), "%ld", &val);
	if (ret == 0)
	{
		USA_TRACE_LEAVE();
		return invalidLong;
	}
	else if (ret == EOF)
	{
		USA_TRACE_LEAVE();
		return invalidLong;
	}
	else
	{
		if (val < min || val > max)
		{
			USA_TRACE_LEAVE();
			return invalidLong;
		}
	}
	
	USA_TRACE_LEAVE();
	
	return val;
}

//******************************************************************************
//	getManualCeaseData()
//******************************************************************************
bool
ACS_USA_Criterion::getManualCeaseData(
					LPSTR dataBuffer,
					DWORD  dataLength) const
{
	USA_TRACE_ENTER();

	if( dataBuffer == NULL ||
		dataLength != sizeof(bool))
	{
		// return default value
		return true;
	}
	
	USA_TRACE_LEAVE();

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
	USA_TRACE_ENTER();

    	if (matchType == other.matchType) {
		if (pattern == other.pattern) {
	    		return ACS_USA_True;
		} 
    	}

	USA_TRACE_LEAVE();
    
    	return ACS_USA_False;
    
}

//******************************************************************************
//      dump()
//******************************************************************************
void
ACS_USA_Criterion::dump()
{
        USA_TRACE_ENTER();
/*      
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
 */
        USA_TRACE_LEAVE();
}

//******************************************************************************

