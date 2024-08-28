//******************************************************************************
//
// NAME
//      ACS_USA_AnalysisObject.cpp
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
// 	    ACS_USA_AnalysisObject holds all data conerning a single log file. 
//	    Methods support initialisation of the object, adding/removing 
//	    criteria, matching a record against all criteria, and updating 
//	    data in criteria stored in objects of this class.
//      Based on 190 89-CAA 109 0259.
//
// DOCUMENT NO
//	    190 89-CAA 109 0259
//
// AUTHOR 
// 	    1995-06-26 by ETX/TX/T XKKHEIN
//
// REV	DATE	NAME		DESCRIPTION
// A	950829	XKKHEIN		First version.
// B	990915	UABDMT		Ported to Windows NT4.0.
// C	030212	UABPEK		New AP VIRUS alarms.
// D	040429	QVINKAL		Removal of RougeWave Tools.h++
// E	051011	UABPEK		Suppression of alarms by named PHA parameter table.
// F	071231	EKAMSBA		General Error filtering.
// G	080215	EKAMSBA		Auto cease alarm.
// H	080318	EKAMSBA		PRC alarms handling.
//
//******************************************************************************

#include <ACS_TRA_Trace.h>
#include "ACS_USA_AnalysisObject.h"
#include "ACS_USA_Common.h"
#include "ACS_USA_Criterion.h"
#include "ACS_USA_EventManager.h"
#include "ACS_USA_File.h"
#include "ACS_USA_Regexp.h"
#include "ACS_USA_Time.h"
#include "ACS_USA_AutoCeaseManager.h"

#include <iostream>
using namespace std; // Using std/stl adapted RW.


/////////////////////////////////////////////////////////////////////////////
//******************************************************************************
//      Trace point definitions
//******************************************************************************
ACS_TRA_trace traceRecMatchfrom_WIN32 =
                                ACS_TRA_DEF("ACS_USA_RecMatchfrom_WIN32", "C300");



//******************************************************************************
//	ACS_USA_AnalysisObject()
//******************************************************************************
ACS_USA_AnalysisObject::ACS_USA_AnalysisObject() :
criteriaNumber(0),
lastLongRecord(0),
lastShortRecord(0),
lastErroneous(0),
equalsLong(0),
equalsShort(0),
equalErroneous(0),
currErroneous(0),
equals(0),
erroneous(0)
{
	
}  

//******************************************************************************
//	ACS_USA_AnalysisObject()
//******************************************************************************
ACS_USA_AnalysisObject::ACS_USA_AnalysisObject(const ACS_USA_AnalysisObject& other) :
fileName(other.fileName),
format(other.format),
position(other.position),
criteriaNumber(other.criteriaNumber),
lastLongRecord(other.lastLongRecord),
lastShortRecord(other.lastShortRecord),
lastErroneous(other.lastErroneous),
equalsLong(other.equalsLong),
equalsShort(other.equalsShort),
equalErroneous(other.equalErroneous),
currErroneous(other.currErroneous),
equals(other.equals),
erroneous(other.erroneous)
{
}


//******************************************************************************
//	ACS_USA_AnalysisObject()
//******************************************************************************
ACS_USA_AnalysisObject::~ACS_USA_AnalysisObject()
{
	criteriaList.clear();
	eventsToCease.clear();
}

//******************************************************************************
//	DestroyCtrList()
//******************************************************************************
void ACS_USA_AnalysisObject::DestroyPtrList()
{
	try
	{
		list<ACS_USA_Criterion *> ::iterator ctrit = ptrList.begin();
			for(;ctrit != ptrList.end();ctrit++)
				if (*ctrit != NULL)
					delete *ctrit;		
		ptrList.clear();
	}
	catch(...)
	{
	}
}

//******************************************************************************
//	appendCriterion()
//******************************************************************************
ACS_USA_StatusType 
ACS_USA_AnalysisObject::appendCriterion(const ACS_USA_Criterion& criterion)
{
    ACS_USA_Criterion *c = new ACS_USA_Criterion(criterion);
	if (c == NULL) 
	{
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_AnalysisObject::appendCriterion().");
		return ACS_USA_error;	
	}
	criteriaList.push_back(*c);
	ptrList.push_back(c);
  
    ++criteriaNumber;
    return ACS_USA_ok;
}


//******************************************************************************
//  removeGeneralErrorCriterion()
//******************************************************************************
ACS_USA_StatusType 
ACS_USA_AnalysisObject::removeGeneralErrorCriterion(const String& filter)
{
    String strFilter = filter;
    toUpper(strFilter);

    //
    // Check criteria in the list
    //

    list<ACS_USA_Criterion>::iterator next = criteriaList.begin();
    
    ACS_USA_Criterion crit ;

    for( ;next != criteriaList.end() ; next++) 
    {
        crit = *next;

        String geFilter = crit.getGeneralErrorFilter();
        toUpper(geFilter);

        // The rule is not filtred, i.e. it is always selected.
        if ( geFilter.empty() ) 
        {
            continue;
        }

        // If filter is ON, remove GE-rules that are 'OFF'
        // If filter is OFF, remove GE-rules that are 'ON'
        if ((strFilter == Key_GEFilterON  && geFilter == Key_GEFilterOFF) || 
            (strFilter == Key_GEFilterOFF && geFilter == Key_GEFilterON))
        {
            next = criteriaList.erase(next);
            next--;
            
            --criteriaNumber;
        }       
    }

    return ACS_USA_ok;
}


//******************************************************************************
//	write()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_AnalysisObject::write(File& file)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                      !
	// ! if changing that function will impacts the file ‘usa.tmp’, !
	// ! Then increment the revision ‘ACS_USA_UsaTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	//VRK-try loop adaded

	unsigned int eventCount = eventsToCease.size();

	try
	{
		// Write data from current object
		file << fileName;
		file << format;
		file << position;
	    
		if (file.Error() ||
		file.Write(criteriaNumber) == false ||
		file.Write(lastLongRecord) == false ||
		file.Write(lastShortRecord)== false ||
		file.Write(lastErroneous)  == false ||
		file.Write(equalsLong)	   == false ||
		file.Write(equalsShort)    == false ||
		file.Write(equalErroneous) == false ||
		file.Write(eventCount)     == false )
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(ACS_USA_WriteFailed);
			USA_DEBUG(logMsg("AO:read error: %s\n", error.getErrorText()));
			return ACS_USA_error;
		}
	}
	catch(...)
	{
		error.setError(ACS_USA_FileIOError);
		error.setErrorText(ACS_USA_WriteFailed);
		USA_DEBUG(logMsg("AO:read error: %s\n", error.getErrorText()));
		return ACS_USA_error;
	}
    
    // Write criteria
	list<ACS_USA_Criterion>::iterator next = criteriaList.begin();
	
	ACS_USA_Criterion crit;

	for( ;next != criteriaList.end() ; next++) 
	{
		crit = (*next);
		if (crit.write(file) == ACS_USA_error)
		{
			USA_DEBUG(logMsg("AO:read error: %s\n", error.getErrorText()));
			return ACS_USA_error;
		}
	}

	// Write pending events to cease.
	vector<ACS_USA_EventInfo>::iterator event = eventsToCease.begin();
	
	for( ; event != eventsToCease.end(); event++)
	{
		if(event->write(file) == ACS_USA_error)
		{
			USA_DEBUG(logMsg("AO:read error: %s\n", error.getErrorText()));
			return ACS_USA_error;
		}
	}

	return ACS_USA_ok;    
}

//******************************************************************************
//	read()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_AnalysisObject::read(File& file)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                      !
	// ! if changing that function will impacts the file ‘usa.tmp’, !
	// ! Then increment the revision ‘ACS_USA_UsaTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Read data into current object.
	unsigned int eventCount = 0;

	try
	{
		file >> fileName;
		file >> format;
		file >> position;
		
		if (file.Error() ||
			file.Read(criteriaNumber) == false ||
			file.Read(lastLongRecord) == false ||
			file.Read(lastShortRecord)== false ||
			file.Read(lastErroneous)  == false ||
			file.Read(equalsLong)     == false ||
			file.Read(equalsShort)    == false ||
			file.Read(equalErroneous) == false ||
			file.Read(eventCount)     == false ) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(ACS_USA_ReadFailed);
			USA_DEBUG(logMsg("AO:read error: %s\n", error.getErrorText()));
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

    // Read all criteria
    for (unsigned int i = 0; i < criteriaNumber; i++) 
	{
		//QVINCON
		ACS_USA_Criterion *c = new ACS_USA_Criterion;
		if (c == NULL) 
		{
			error.setError(ACS_USA_Memory);
			error.setErrorText("new() failed in ACS_USA_AnalysisObject::read().");
			return ACS_USA_error;	
		}
		if (c->read(file) == ACS_USA_error) 
		{
			delete c; //TR HI60745 : first to return delete the new object
			return ACS_USA_error;
		}
		if (c->validate() == ACS_USA_error) 
		{
			delete c; //TR HI60745 : first to return delete the new object
			return ACS_USA_error;
		}
		
		criteriaList.push_back(*c);
		ptrList.push_back(c);
		
    }
    
    // Read pending events to cease.
    for (i = 0; i < eventCount; i++) 
    {
        ACS_USA_EventInfo   event;

        if (event.read(file) == ACS_USA_error) 
        {
            return ACS_USA_error;
        }
        
        eventsToCease.push_back(event);
    }

    return ACS_USA_ok;    
}


//******************************************************************************
//	match()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_AnalysisObject::match(
			      const ACS_USA_RecordType record,
			      const ACS_USA_TimeType recordTime,
			      const ACS_USA_ActivationType activation,
				  const vector<String>& insertedStrings,
				  LPBYTE dataBuffer,
				  DWORD  dataLength)
{	
	if (recordTime < 0) {
	//
	// An erroneous record. Determine if it has been seen already. 
	// If not, send an AP Event report.
	//
	int saveAndReport  = ACS_USA_False;

	if (currErroneous > lastErroneous) {
	    saveAndReport = ACS_USA_True;
	    if (erroneous == 0) {
	    	erroneous = 1;					// First erroneous record
	    }
	} else if (currErroneous == lastErroneous) {
	    ++erroneous;
	    if (erroneous > equalErroneous) {
		saveAndReport = ACS_USA_True;
	    }
	}

	if (saveAndReport == ACS_USA_True) {
	    // 
	    // Save the position of the erroneous record relative to 
	    // last known good record.
	    //
	    lastErroneous = currErroneous;
	    equalErroneous = erroneous;
	
	    // 
	    // Report the erroneous record
	    //
	    {
			String a(fileName);                                      
	    	String badRecord(record);
		    	
		if (badRecord.length() > 512) 
		{
			badRecord.erase(512);
		}
		int lastIndex = badRecord.length();

		for (int i = 0; i < lastIndex; i++) 
		{
		    char c = badRecord[(size_t)i];	// String needs "size_t".
		    if ((c < ' ' && c != '\t') ||
			c > '~') {
			badRecord[(size_t)i] = '.';		// String needs "size_t".
		    }
		}

		error.setErrorText(fileName.data(), error.getErrorText(), badRecord.data());   
		if (eventManager.report(error.getError() + ACS_USA_FirstErrorCode,
			String(Key_perceivedSeverity_EVENT),
			String(ACS_USA_FaultCause),
			String(),
			String(error.getErrorText()),
			String(error.getProblemText(error.getError())),
			"", // Node state is not interesting here.
			0
			) == ACS_USA_error) {
		    return ACS_USA_error;
		}
	    }
	}

	// 
	// Clear error before return
	//
	error.setError(ACS_USA_NoError);
	error.setErrorText(noErrorText);
	return ACS_USA_ok;

    } else {
	if (currErroneous != recordTime) {
	    erroneous = 0;
	    currErroneous = recordTime;
	}
    }

    if ((activation == Activation_longTerm)
		|| (activation == Activation_FirstLongTerm))
	{
		//
		// Check if the record has been seen during long term analysis  
		//
		if (recordTime < lastLongRecord) 
		{
			return ACS_USA_ok;
		} 
		else if (recordTime == lastLongRecord) 
		{
			if (equals < equalsLong) 
			{
				equals++;
				//return ACS_USA_ok;  //TR HK66539, HK87737 and HK91508
			} 
			else 
			{
				equalsLong++;
				equals++;
			}
		} 
		else 
		{								// rec. not seen before
			equals = 1;
			equalsLong = 1;
			lastLongRecord = recordTime;
		}
    } 
	else 
	{
		//
		// Check if the record has been seen during either short or long
		// term analysis. If that's true mark it in the criterion and return 
		//

		if ((activation == Activation_shortTerm) || 
//			(activation == Activation_FirstShortTerm)||
			((activation == Activation_instantShortTerm))) 
		{
			if (recordTime < lastLongRecord) 
			{
				if (lastShortRecord < lastLongRecord) 
				{
					lastShortRecord = lastLongRecord;
					equalsShort = equalsLong;
				}
				return ACS_USA_ok;
			} 
			else if (recordTime < lastShortRecord) 
			{
				return ACS_USA_ok;
			} 
			else if (recordTime == lastShortRecord) 
			{
				if (lastShortRecord == lastLongRecord && equalsShort < equalsLong) 
				{
					equalsShort = equalsLong;
				}
				if (equals < equalsShort) 
				{
					equals++;
					//return ACS_USA_ok;  //TR HK66539, HK87737 and HK91508
				}
				else 
				{
					equalsShort++;
					equals++;
				}
			} 
			else 
			{
				equalsShort = 1;
				equals = 1;
				lastShortRecord = recordTime;
			}
		}
		//
		// If FirstShortTerm we do not check if events have been seen before
		//
		else if (activation == Activation_FirstShortTerm)
		{
			equalsShort = 1;
			equals = 1;
			lastShortRecord = recordTime;
		}

	}
    //
    // Check all criteria in the list
    //
    
	list<ACS_USA_Criterion>::iterator next = criteriaList.begin();
	
    ACS_USA_MatchType ret = Criterion_noMatch;
    ACS_USA_Criterion crit ;

	for( ;next != criteriaList.end() ; next++) 
	{
		crit = *next;
		if ((ret = crit.match(record, 
							   fileName,
							   recordTime,
							   activation,
							   insertedStrings,
							   dataBuffer,
							   dataLength)) ==  Criterion_matchError) 
		{
			return ACS_USA_error;
		}
		else if (ret == Criterion_match) 
		{
			*next = crit;
			if(ACS_TRA_ON(traceRecMatchfrom_WIN32)) 
			{
				String matched_Record("Matched Record from WIN32 API ==>");
				matched_Record.append(record);
				ACS_TRA_event(&traceRecMatchfrom_WIN32, matched_Record.data());
			}
			return ACS_USA_ok;
		} 
		else 
		{
		}
    }													// End of while
    return ACS_USA_ok;
}
                        

//******************************************************************************
//	update()
//******************************************************************************
void
ACS_USA_AnalysisObject::update(ACS_USA_AnalysisObject& other)
{
    // If file names are equal get the appropriate record times
    if (fileName == other.fileName) 
	{	
		list<ACS_USA_Criterion>::iterator next = criteriaList.begin();
		list<ACS_USA_Criterion>::iterator other_next = other.criteriaList.begin();
		
		lastLongRecord  = other.lastLongRecord;
		lastShortRecord = other.lastShortRecord;
		lastErroneous	= other.lastErroneous;
		equalErroneous	= other.equalErroneous;
		equalsLong	= other.equalsLong;
		equalsShort	= other.equalsShort;
		
		
		USA_DEBUG(logMsg("AO:update match: %s\n", fileName.data()));
		ACS_USA_Criterion a ;
		ACS_USA_Criterion b ;
		
		for( ;next != criteriaList.end() ; next++) 
		{
			a = *next;
			for( other_next = other.criteriaList.begin();
				other_next != other.criteriaList.end() ; other_next++) 
			{

				b = *other_next;
				if (a == b)
				{
					a.update(b);
				}
			}
		}

		eventsToCease = other.eventsToCease;
	}
}


//******************************************************************************
//	validate()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_AnalysisObject::validate()
{
    if (fileName.empty() ||
		format.empty()) 
	{
		error.setError(ACS_USA_SyntaxError);
		error.setErrorText(ACS_USA_MissingVitalParameter);
		return ACS_USA_error;
    }
    
    //
    // Check if the regular expression is valid
    //
    
	if (!(position.empty())) 
	{
		ACS_USA_Regexp re;
    	if (re.compilePattern(position) == ACS_USA_error) 
		{
			USA_DEBUG(logMsg("AO:validate errorText: %s\n", error.getErrorText()));
			return ACS_USA_error;
		}
    }
    return ACS_USA_ok;
}


//******************************************************************************
//	checkAlarms()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_AnalysisObject::checkAlarms(
									unsigned int alarmFilterInterval,
									const vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms,
									bool bDefaultLogging )
{
	list<ACS_USA_Criterion>::iterator next = criteriaList.begin();
	ACS_USA_Criterion crit;

	for(; next != criteriaList.end(); next++) 
	{
		crit = *next;

		ACS_USA_StatusType ret = ACS_USA_ok;
		if( bDefaultLogging )
		{
			// Default behavior
			ret = crit.raiseAlarm(
								this,
								alarmFilterInterval,
								suppressedAlarms,
								eventsToCease );
		}
		else
		{
			// This overrides ACF settings by record info
			ret = crit.raiseCustomAlarm(
									this,
									alarmFilterInterval,
									suppressedAlarms,
									eventsToCease );
		}

		if ( ret == ACS_USA_error)
		{
			ACS_USA_ErrorType err = error.getError();
			if (err == ACS_USA_APeventHandlingError || err == ACS_USA_QueueExceeded) 
			{
				return ACS_USA_error;
			}
		}

		*next = crit;
	}
	
	if( eventsToCease.empty() == false )
	{
		ACS_USA_AutoCeaseManager::ACS_USA_CeaseFaultCode resultCode;
		resultCode = (ACS_USA_AutoCeaseManager::getInstance()).addEvents( eventsToCease );

		switch ( resultCode )
		{
		case ACS_USA_AutoCeaseManager::NoFault:
			eventsToCease.clear();
			break;

		case ACS_USA_AutoCeaseManager::TimeOut:
			// try next time
			break;

		case ACS_USA_AutoCeaseManager::Fault:
		default:
			return ACS_USA_error;
		}
	}

	return ACS_USA_ok;
}


//******************************************************************************
//	dump()
//******************************************************************************
void
ACS_USA_AnalysisObject::dump()
{
	list<ACS_USA_Criterion>::iterator next = criteriaList.begin();
    ACS_USA_Criterion crit ;

	USA_DEBUG(logMsg("---------- Analysis Object ----------\n"));
    USA_DEBUG(logMsg("File name:         %s\n", fileName.data()));
    USA_DEBUG(logMsg("Format:            %s\n", format.data()));
    USA_DEBUG(logMsg("Position:          %s\n", position.data()));
    USA_DEBUG(logMsg("Criteria number:   %d\n", criteriaNumber));
    USA_DEBUG(logMsg("Last long record:  %d\n", lastLongRecord));
    USA_DEBUG(logMsg("Last short record: %d\n", lastShortRecord));
    //USA_DEBUG(logMsg("object status:     %d\n", objectStatus));

	for( ;next != criteriaList.end() ; next++) 
	{
		crit = *next;
		crit.dump(); 
    }
    return;
}


//******************************************************************************

