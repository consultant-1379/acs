//******************************************************************************
//
// NAME
//      acs_usa_analysisObject.cpp
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
// 	    acs_usa_analysisObject holds all data conerning a single log file. 
//	    Methods support initialisation of the object, adding/removing 
//	    criteria, matching a record against all criteria, and updating 
//	    data in criteria stored in objects of this class.
//	    Based on 190 89-CAA 109 0259.
//
// DOCUMENT NO
//
// AUTHOR 
//
//
//******************************************************************************

#include "acs_usa_analysisObject.h"
#include "acs_usa_common.h"
#include "acs_usa_criterion.h"
#include "acs_usa_eventManager.h"
#include "acs_usa_file.h"
#include "acs_usa_regexp.h"
#include "acs_usa_time.h"
#include "acs_usa_autoCeaseManager.h"
#include "acs_usa_tratrace.h"
#include "acs_usa_error.h"
#include "acs_usa_logtrace.h"

#include <iostream>

using namespace std; // Using std/stl adapted RW.


/////////////////////////////////////////////////////////////////////////////
//******************************************************************************
//      Trace point definitions
//******************************************************************************

ACS_USA_Trace traObj("ACS_USA_AnalysisObject	");

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
	USA_TRACE_ENTER();
	USA_TRACE_LEAVE();	
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
	USA_TRACE_ENTER2("Constructor");
	USA_TRACE_LEAVE2("Constructor");	
}


//******************************************************************************
//	ACS_USA_AnalysisObject()
//******************************************************************************
ACS_USA_AnalysisObject::~ACS_USA_AnalysisObject()
{
	USA_TRACE_ENTER2("Destructor");
	
	criteriaList.clear();
	eventsToCease.clear();

	USA_TRACE_LEAVE2("Destructor");	
}

//******************************************************************************
//	DestroyCtrList()
//******************************************************************************
void ACS_USA_AnalysisObject::DestroyPtrList()
{
	USA_TRACE_ENTER();

	try
	{
		std::list<ACS_USA_Criterion *> ::iterator ctrit = ptrList.begin();
			for(;ctrit != ptrList.end();ctrit++)
				if (*ctrit != NULL)
					delete *ctrit;		
		ptrList.clear();
	}
	catch(...)
	{
	}

	USA_TRACE_ENTER();
}

//******************************************************************************
//	appendCriterion()
//******************************************************************************
ACS_USA_ReturnType 
ACS_USA_AnalysisObject::appendCriterion(const ACS_USA_Criterion& criterion)
{
	USA_TRACE_ENTER();
    	ACS_USA_Criterion *c = new ACS_USA_Criterion(criterion);
	if (c == NULL) 
	{
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_AnalysisObject::appendCriterion().");
		return ACS_USA_Error;	
	}
	criteriaList.push_back(*c);
	ptrList.push_back(c);
  
    	++criteriaNumber;

	USA_TRACE_LEAVE();
    	return ACS_USA_Ok;
}

//******************************************************************************
//	write()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_AnalysisObject::write(File& file)
{

	USA_TRACE_ENTER2("ACS_USA_AnalysisObject: criteriaNumber[%d]", criteriaNumber);

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                      !
	// ! if changing that function will impacts the file ‘usa.tmp’, !
	// ! Then increment the revision ‘ACS_USA_UsaTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	unsigned int eventCount = eventsToCease.size();
	try
	{
		// Write data from current object
		//file << fileName;
		//file << format;
		//file << position;
	    
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
			USA_TRACE_LEAVE2("ACS_USA_AnalysisObject:write [%s]", ACS_USA_WriteFailed);
			return ACS_USA_Error;
		}
	}
	catch(...)
	{
		error.setError(ACS_USA_FileIOError);
		error.setErrorText(ACS_USA_WriteFailed);
		USA_TRACE_LEAVE2("ACS_USA_AnalysisObject:write [%s]", ACS_USA_WriteFailed);
		return ACS_USA_Error;
	}
    
	// Write criteria
	std::list<ACS_USA_Criterion>::iterator next = criteriaList.begin();
	
	for( ;next != criteriaList.end() ; next++) 
	{
		if (next->write(file) == ACS_USA_Error)
		{
			USA_TRACE_LEAVE();
			return ACS_USA_Error;
		}
	}

	// Write pending events to cease.
	vector<ACS_USA_EventInfo>::iterator event = eventsToCease.begin();
	
	for( ; event != eventsToCease.end(); event++)
	{
		if(event->write(file) == ACS_USA_Error)
		{
			USA_TRACE_LEAVE();
			return ACS_USA_Error;
		}
	}

	USA_TRACE_LEAVE();
	return ACS_USA_Ok;    
}

//******************************************************************************
//	read()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_AnalysisObject::read(File& file)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                      !
	// ! if changing that function will impacts the file ‘usa.tmp’, !
	// ! Then increment the revision ‘ACS_USA_UsaTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Read data into current object.
	USA_TRACE_ENTER();
	unsigned int eventCount = 0;

	try
	{
		//file >> fileName;
		//file >> format;
		//file >> position;
		
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
			USA_TRACE_LEAVE();
			return ACS_USA_Error;
		}
        }
	catch (...)
	{
		error.setError(ACS_USA_FileIOError);
		error.setErrorText(ACS_USA_ReadFailed);
		USA_TRACE_LEAVE();
		return ACS_USA_Error;
	}

	USA_TRACE_1("ACS_USA_AnalysisObject: criteriaNumber[%d]", criteriaNumber);
	// Read all criteria
	for (unsigned int i = 0; i < criteriaNumber; i++) 
	{
		USA_TRACE_1("ACS_USA_AnalysisObject: reading criteria[%d]", i);
		ACS_USA_Criterion *c = new ACS_USA_Criterion;
		if (c == NULL) 
		{
			error.setError(ACS_USA_Memory);
			error.setErrorText("new() failed in ACS_USA_AnalysisObject::read().");
			//traObj.traceAndLogFmt(INFO, "ACS_USA_AnalysisObject:%s() - new() failed in ACS_USA_AnalysisObject::read(). leaving", __func__);
			USA_TRACE_LEAVE2("ACS_USA_AnalysisObject:new() failed in ACS_USA_AnalysisObject::read(). leaving");
			return ACS_USA_Error;	
		}
		if (c->read(file) == ACS_USA_Error) 
		{
			delete c; //TR HI60745 : first to return delete the new object
			traObj.traceAndLogFmt(INFO, "ACS_USA_AnalysisObject:%s() - c->read() failed [%d] Leaving", __func__, i);
			USA_TRACE_LEAVE2("ACS_USA_AnalysisObject: c->read() failed [%d] Leaving", i);
			return ACS_USA_Error;
		}
		if (c->validate() == ACS_USA_Error) 
		{
			delete c; //TR HI60745 : first to return delete the new object
			traObj.traceAndLogFmt(INFO, "ACS_USA_AnalysisObject:%s() - c->validate() failed [%d] Leaving", __func__, i);
			USA_TRACE_LEAVE2("ACS_USA_AnalysisObject: c->validate() failed [%d] Leaving",i);
			return ACS_USA_Error;
		}
		
		criteriaList.push_back(*c);
		ptrList.push_back(c);
		
	}
    
    // Read pending events to cease.
	for (unsigned int i = 0; i < eventCount; i++) 
	{
		ACS_USA_EventInfo   event;

		if (event.read(file) == ACS_USA_Error) 
		{
			USA_TRACE_LEAVE();
			return ACS_USA_Error;
		}
        
		eventsToCease.push_back(event);
	}

	USA_TRACE_LEAVE();
	return ACS_USA_Ok;    
}


//******************************************************************************
//	match()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_AnalysisObject::match(
			      	const ACS_USA_RecordType record,
			      	const ACS_USA_TimeType recordTime,
			      	const ACS_USA_ActivationType activation,
				const vector<String>& insertedStrings,
				LPSTR dataBuffer,
				DWORD  dataLength)
{	
	USA_TRACE_ENTER();
	if (recordTime < 0) {
		// An erroneous record. Determine if it has been seen already. 
		// If not, send an AP Event report.
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
				String(ACS_USA_PERCEIVED_SEVERITY_EVENT),
				String(ACS_USA_INTLFAULT),
				String(),
				String(error.getErrorText()),
				String(error.getProblemText(error.getError())),
				"", // Node state is not interesting here.
				0
				) == ACS_USA_Error) {
				USA_TRACE_LEAVE();
		    		return ACS_USA_Error;
			}
	    	}
	}

	// 
	// Clear error before return
	//
	error.setError(ACS_USA_NoError);
	error.setErrorText(noErrorText);
	USA_TRACE_LEAVE();
	return ACS_USA_Ok;

    } else if (currErroneous != recordTime) {
	    erroneous = 0;
	    currErroneous = recordTime;
    	}

	if ((activation == Activation_longTerm)
		|| (activation == Activation_FirstLongTerm))
	{
		//
		// Check if the record has been seen during long term analysis  
		//
		if (recordTime < lastLongRecord) 
		{
			return ACS_USA_Ok;
		} 
		else if (recordTime == lastLongRecord) 
		{
			if (equals < equalsLong) 
			{
				equals++;
				//return ACS_USA_Ok;  //TR HK66539, HK87737 and HK91508
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
			(activation == Activation_startUp)||
			((activation == Activation_instantShortTerm))) 
		{
			if (recordTime < lastLongRecord) 
			{
				if (lastShortRecord < lastLongRecord) 
				{
					lastShortRecord = lastLongRecord;
					equalsShort = equalsLong;
				}
				USA_TRACE_LEAVE();
				return ACS_USA_Ok;
			} 
			else if (recordTime < lastShortRecord) 
			{
				USA_TRACE_LEAVE();
				return ACS_USA_Ok;
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
					//return ACS_USA_Ok;  //TR HK66539, HK87737 and HK91508
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
    
	std::list<ACS_USA_Criterion>::iterator next = criteriaList.begin();
	
	ACS_USA_MatchType ret = Criterion_noMatch;
	ACS_USA_Criterion crit ;

	USA_TRACE_1("ACS_USA_AnalysisObject: criteriaList.size [%d]", (int)criteriaList.size());
	if(record == NULL)
	{
		USA_TRACE_1("ACS_USA_AnalysisObject: match record NULL");
		return ACS_USA_Error;
	}

	for( ;next != criteriaList.end() ; next++) 
	{
		crit = *next;
		if ((ret = crit.match(	record, 
					fileName,
					recordTime,
					activation,
					insertedStrings,
					dataBuffer,
					dataLength)) ==  Criterion_matchError) 
		{
			USA_TRACE_LEAVE2("ACS_USA_AnalysisObject:crit.match failed Leaving");
			return ACS_USA_Error;
		}
		else if (ret == Criterion_match) 
		{
			*next = crit;
			// TRACE ON here
			//traObj.traceAndLogFmt(INFO, "ACS_USA_AnalysisObject:%s() - Criterion_match Leaving", __func__);
			USA_TRACE_LEAVE2("ACS_USA_AnalysisObject: Criterion_match Leaving");
			return ACS_USA_Ok;
		} 
		else 
		{
			//traObj.traceAndLogFmt(INFO, "ACS_USA_AnalysisObject:%s() - crit.match return [%d]", __func__, ret);
			//USA_TRACE_1("ACS_USA_AnalysisObject: crit.match return [%d]", ret);
		}  
	}// End of for

	USA_TRACE_LEAVE();
	//traObj.traceAndLogFmt(INFO, "ACS_USA_AnalysisObject:%s() - Leaving", __func__);
	return ACS_USA_Ok;
}

//----------------------------------------------------------------------------------------------------------------------------------------
ACS_USA_ReturnType ACS_USA_AnalysisObject::reset()
{
	std::list<ACS_USA_Criterion>::iterator next = criteriaList.begin();
	

	for( ;next != criteriaList.end() ; next++) 
	{
		(*next).reset();
	}// End of for

	
	return ACS_USA_Ok;
}
                        
//----------------------------------------------------------------------------------------------------------------------------------------
ACS_USA_ReturnType ACS_USA_AnalysisObject::clearInstAlarmsQue(ACS_USA_TimeType lastShortRecordTime)
{
	std::list<ACS_USA_Criterion>::iterator next = criteriaList.begin();
	

	for( ;next != criteriaList.end() ; next++) 
	{
		(*next).clearInstAlarmsQue(lastShortRecordTime);
	}// End of for

	
	return ACS_USA_Ok;
}
                        
//----------------------------------------------------------------------------------------------------------------------------------------
ACS_USA_MatchType
ACS_USA_AnalysisObject::matchRecord( const ACS_USA_RecordType record,
			      	     const ACS_USA_TimeType recordTime,
			      	     const ACS_USA_ActivationType activation,
				     const vector<String>& insertedStrings,
				     LPSTR dataBuffer,
				     DWORD  dataLength)
{	
	USA_TRACE_ENTER();

	if (recordTime < 0) {
		// An erroneous record. Determine if it has been seen already. 
		// If not, send an AP Event report.
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
	
	   	 	// Report the erroneous record
	   	 	//
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
				String(ACS_USA_PERCEIVED_SEVERITY_EVENT),
				String(ACS_USA_INTLFAULT),
				String(),
				String(error.getErrorText()),
				String(error.getProblemText(error.getError())),
				"", // Node state is not interesting here.
				0
				) == ACS_USA_Error) {
				USA_TRACE_LEAVE();
		    		return ACS_USA_Error;
			}
		}

		// 
		// Clear error before return
		//
		error.setError(ACS_USA_NoError);
		error.setErrorText(noErrorText);
		USA_TRACE_LEAVE();
		return ACS_USA_Ok;

    	} else if (currErroneous != recordTime) {
	    	erroneous = 0;
	    	currErroneous = recordTime;
    	}

	if (	(activation == Activation_longTerm)
		|| (activation == Activation_FirstLongTerm))
	{
		//
		// Check if the record has been seen during long term analysis  
		//
		if (recordTime < lastLongRecord) 
		{
			return ACS_USA_Ok;
		} 
		else if (recordTime == lastLongRecord) 
		{
			if (equals < equalsLong) 
			{
				equals++;
				//return ACS_USA_Ok;  //TR HK66539, HK87737 and HK91508
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
			(activation == Activation_startUp)||
			((activation == Activation_instantShortTerm))) 
		{
			if (recordTime < lastLongRecord) 
			{
				if (lastShortRecord < lastLongRecord) 
				{
					lastShortRecord = lastLongRecord;
					equalsShort = equalsLong;
				}
				USA_TRACE_LEAVE();
				return ACS_USA_Ok;
			} 
			else if (recordTime < lastShortRecord) 
			{
				USA_TRACE_LEAVE();
				return ACS_USA_Ok;
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
					//return ACS_USA_Ok;  //TR HK66539, HK87737 and HK91508
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
    
	std::list<ACS_USA_Criterion>::iterator next = criteriaList.begin();
	
	ACS_USA_MatchType ret = Criterion_noMatch;
	ACS_USA_Criterion crit ;

	//USA_TRACE_1("ACS_USA_AnalysisObject: criteriaList.size [%d]", (int)criteriaList.size());

	for( ;next != criteriaList.end() ; next++) 
	{
		crit = *next;
		if ((ret = crit.match(	record, 
					fileName,
					recordTime,
					activation,
					insertedStrings,
					dataBuffer,
					dataLength)) ==  Criterion_matchError) 
		{
			USA_TRACE_LEAVE2("ACS_USA_AnalysisObject:crit.match failed Leaving");
		}
		else if (ret == Criterion_match) 
		{
			*next = crit;
			// TRACE ON here
			//traObj.traceAndLogFmt(INFO, "ACS_USA_AnalysisObject:%s() - Criterion_match Leaving", __func__);
			USA_TRACE_LEAVE2("ACS_USA_AnalysisObject: Criterion_match Leaving");
			return ret;
		} 
		else 
		{
			//traObj.traceAndLogFmt(INFO, "ACS_USA_AnalysisObject:%s() - crit.match return [%d]", __func__, ret);
			//USA_TRACE_1("ACS_USA_AnalysisObject: crit.match return [%d]", ret);
		}  
	}// End of for

	USA_TRACE_LEAVE();

	return ret;
}

//******************************************************************************
//	update()
//******************************************************************************
void
ACS_USA_AnalysisObject::update(ACS_USA_AnalysisObject& other)
{
    // If file names are equal get the appropriate record times
    	USA_TRACE_ENTER();
    	if (fileName == other.fileName) 
	{	
		std::list<ACS_USA_Criterion>::iterator next = criteriaList.begin();
		std::list<ACS_USA_Criterion>::iterator other_next = other.criteriaList.begin();
		
		lastLongRecord  = other.lastLongRecord;
		lastShortRecord = other.lastShortRecord;
		lastErroneous	= other.lastErroneous;
		equalErroneous	= other.equalErroneous;
		equalsLong	= other.equalsLong;
		equalsShort	= other.equalsShort;
		
		
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
    	USA_TRACE_LEAVE();
    	return;
}


//******************************************************************************
//	validate()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_AnalysisObject::validate()
{
	USA_TRACE_ENTER();
    	
	if (fileName.empty() ||
		format.empty()) 
	{
			error.setError(ACS_USA_SyntaxError);
			error.setErrorText(ACS_USA_MissingVitalParameter);
			USA_TRACE_LEAVE();
			return ACS_USA_Error;
    	}
    
    	//
    	// Check if the regular expression is valid
    	//
    
	if (!(position.empty())) 
	{
		ACS_USA_Regexp re;
    		if (re.compilePattern(position) == ACS_USA_Error) 
		{
			USA_TRACE_LEAVE();
			return ACS_USA_Error;
		}
    	}
	
	USA_TRACE_LEAVE();
	return ACS_USA_Ok;
}


//******************************************************************************
//	checkAlarms()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_AnalysisObject::checkAlarms(
				unsigned int alarmFilterInterval,
				const vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms,
				bool bDefaultLogging )
{
	USA_TRACE_ENTER();

	std::list<ACS_USA_Criterion>::iterator next = criteriaList.begin();
	ACS_USA_Criterion crit;

	for(; next != criteriaList.end(); next++) 
	{
		crit = *next;

		ACS_USA_ReturnType ret = ACS_USA_Ok;
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

		if ( ret == ACS_USA_Error)
		{
			ACS_USA_ErrorType err = error.getError();
			if (err == ACS_USA_APeventHandlingError || err == ACS_USA_QueueExceeded) 
			{
				return ACS_USA_Error;
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
			return ACS_USA_Error;
		}
	}
	
	USA_TRACE_LEAVE();
	return ACS_USA_Ok;
}


//******************************************************************************
//	dump()
//******************************************************************************
void
ACS_USA_AnalysisObject::dump()
{
	USA_TRACE_ENTER();

	std::list<ACS_USA_Criterion>::iterator next = criteriaList.begin();
/*
	USA_DEBUG(logMsg("---------- Analysis Object ----------\n"));
    	USA_DEBUG(logMsg("File name:         %s\n", fileName.data()));
    	USA_DEBUG(logMsg("Format:            %s\n", format.data()));
	USA_DEBUG(logMsg("Position:          %s\n", position.data()));
    	USA_DEBUG(logMsg("Criteria number:   %d\n", criteriaNumber));
    	USA_DEBUG(logMsg("Last long record:  %d\n", lastLongRecord));
    	USA_DEBUG(logMsg("Last short record: %d\n", lastShortRecord));
    	//USA_DEBUG(logMsg("object status:     %d\n", objectStatus));
*/
	for( ;next != criteriaList.end() ; next++) 
	{
		next->dump(); 
    	}

	USA_TRACE_LEAVE();
    	return;
}


//******************************************************************************

