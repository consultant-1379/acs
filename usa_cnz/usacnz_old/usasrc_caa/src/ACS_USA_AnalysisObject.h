//******************************************************************************
// 
// NAME 
//      ACS_USA_AnalysisObject - This class holds all data needed for analysis
//      of a log file.
//
//  COPYRIGHT
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
//      ACS_USA_AnalysisObject holds all data conerning a single log file. 
//      Methods support initialisation of the object, adding/removing 
//      criteria, matching a record against all criteria, and updating 
//      data in criteria stored in objects of this class.
//      Based on 190 89-CAA 109 0259.
//
// DOCUMENT NO
//	    190 89-CAA 109 0259
//
// AUTHOR 
// 	    1995-06-26 by ETX/TX/T XKKHEIN
//
// REV  DATE    NAME     DESCRIPTION
// A    950829  XKKHEIN  First Revision
// B    990915  UABDMT   Ported to Windows NT4.0
// C    040429  QVINKAL  Removal of RougeWave Tools.h++
// D    051011  UABPEK   Suppression of alarms by named PHA parameter table.
// E    071231  EKAMSBA  General Error filtering.
// F    080215  EKAMSBA  Auto cease alarm.
// H    080318  EKAMSBA  PRC alarms handling.
//
//******************************************************************************

#ifndef ACS_USA_ANALYSISOBJECT_H 
#define ACS_USA_ANALYSISOBJECT_H

#include <list>
#include <string>

#include "ACS_USA_Control.h"
#include "ACS_USA_Criterion.h"
#include "ACS_USA_Global.h"
#include "ACS_USA_EventInfo.h"


using namespace std;
typedef string String;

//******************************************************************************
// Forward declaration(s)
//******************************************************************************
class 	ACS_USA_Config;

//******************************************************************************
// Member functions, constructors, destructors, operators
//******************************************************************************

class  ACS_USA_AnalysisObject
{
friend ACS_USA_Config;
 public:

  ACS_USA_AnalysisObject();
  // Description:
  // 	Default constructor.
  // Parameters: 
  //    none
  // Return value: 
  //    none
  
  ACS_USA_AnalysisObject(const ACS_USA_AnalysisObject& other);
  // Description:
  // 	Copy constructor. Constructs the object instance with data from 
  //	another Analysis Object instance.
  // Parameters: 
  //    other			another Analysis Object 
  // Return value: 
  //    none

  virtual ~ACS_USA_AnalysisObject();
  // Description:
  // 	Destructor. Releases allocated memory.
  // Parameters: 
  //    storageObject		data for Analysis Object 
  // Return value: 
  //    none

  ACS_USA_StatusType appendCriterion(const ACS_USA_Criterion& criterion);
  // Description:
  // 	Appends a criterion to the list.
  // Parameters: 
  //    storageCriterion	data for a criterion 
  // Return value: 
  //    ACS_USA_ok		append ok
  //	ACS_USA_error		append failed
 
  ACS_USA_StatusType removeGeneralErrorCriterion(const String& filter);
  // Description:
  //    Removes a criterion from the criteria list.
  // Parameters: 
  //    filter      filters the criteria to remove
  // Return value: 
  //    ACS_USA_ok          everything OK
  //    ACS_USA_error       error occurred

  ACS_USA_StatusType write(File& file);
  // Description:
  // 	Writes data to to file including all criteria.
  // Parameters:
  //	file			reference to a file object 
  // Return value: 
  //    ACS_USA_ok		append ok
  //	ACS_USA_error		append failed
  
  ACS_USA_StatusType read(File& file);
  // Description:
  // 	Reads data from file object.
  // Parameters: 
  //	file			reference to a file object 
  // Return value: 
  //    ACS_USA_ok		append ok
  //	ACS_USA_error		append failed
  
  ACS_USA_StatusType match(const ACS_USA_RecordType record,
			   const ACS_USA_TimeType recordTime,
			   const ACS_USA_ActivationType activation,
			   const vector<String>& insertedStrings = vector<String>(),
  			   LPBYTE dataBuffer = NULL,
			   DWORD  dataLength = 0);
  // Description:
  // 	Scans the list to find a criterion match. Returns immediately 
  //	if a match was found.
  // Parameters: 
  //    record			record to be checked
  //	recordTime		the time record was logged
  //	activation		flag indicatig long or short term analysis
  //	insertedStrings	strings from the event record
  //	dataBuffer		buffer containing event-specific data
  //	dataLength		size of the buffer, in bytes
  // Return value: 
  //    ACS_USA_ok		everithing went ok
  //	ACS_USA_error		something went wrong

  void update(ACS_USA_AnalysisObject& other);
  // Description:
  // 	Compares the current analysis object with other. If they are applied 
  //	to the same log file the criteria are compared and updated with data 
  //	from other if needed.
  // Parameters: 
  //    other			another analysis object
  // Return value: 
  //    none
  
  //const ACS_USA_AnalysisObject& operator=(const ACS_USA_AnalysisObject& other);
  // Description:
  //    Assignment operator
  // Parameters:
  //    other                   reference to an existing criterion object, in
  // Return value:
  //    none

  ACS_USA_StatusType validate();
  // Description:
  //	Validates the object
  // Parameters: 
  //    none
  // Return value: 
  //    ACS_USA_ok		everithing went ok
  //	ACS_USA_error		something went wrong
   
  ACS_USA_StatusType checkAlarms(
							unsigned int alarmFilterInterval,
							const vector<ACS_USA_SuppressedAlarmsElement*> suppressedAlarms,
							bool bDefaultLogging = true);
  // Description:
  //	 Checks for alarm condition occurrences in criteria
  // Parameters: 
  //	 alarmFilterInterval  Min interval between alarms on the same error.
  //	 suppressedAlarms     Criteria that should not raise alarm.
  //	 bDefaultLogging      Indicates how to log APEvents.
  // Return value: 
  //   ACS_USA_ok		  Everything is ok.
  //	 ACS_USA_error	Something went wrong.
  
  String& getLogFile();
  // Description:
  //	Returns a reference to file name string
  // Parameters: 
  //    none
  // Return value: 
  //    see description

  String& getPosition();
  // Description:
  //	Returns a reference to file name string
  // Parameters: 
  //    none
  // Return value: 
  //    see description
  
  String& getFormat();

  // Description:
  //	Returns a reference to file name string
  // Parameters: 
  //    none
  // Return value: 
  //    see description

  void dump();
  // Description:
  //	Dumps the contents of the Analysis Object. Can be used if 
  //	compiled with -DDEBUG option
  // Parameters: 
  //    none
  // Return value: 
  //	none  

  void DestroyPtrList();
private:
	void setLogFile(String& str);
  // Description:
  //	Copies str into fileName
  // Parameters: 
  //    str			string from where to copy
  // Return value: 
  //    ACS_USA_ok		everithing went ok
  //	ACS_USA_error		something went wrong
  
	void setFormat(String& str);	
  // Description:
  //	Copies str into format
  // Parameters: 
  //    str			string from where to copy
  // Return value: 
  //    ACS_USA_ok		everithing went ok
  //	ACS_USA_error		something went wrong
  
	void setPosition(String& str);
  // Description:
  //	Copies str into position
  // Parameters: 
  //    str			string from where to copy
  // Return value: 
  //    ACS_USA_ok		everithing went ok
  //	ACS_USA_error		something went wrong

  //
  // Parameters from ACF
  //

  String fileName;   	// log file name
  String format; 		// Time stamp format
  String position; 		// Time stamp position
  // 
  // Parameters stored/retrieved from temporary storage (in addition to ACF)
  //
  ACS_USA_CriteriaCount		criteriaNumber;	// Number of criteria
  ACS_USA_TimeType		lastLongRecord;	// Last record analysed
  ACS_USA_TimeType		lastShortRecord;// Last record analysed
  ACS_USA_TimeType		lastErroneous;	// relative to last ok record
  int                           equalsLong;     // Equal recs during Long term 
  int                           equalsShort;    // Equal recs during Short
  int				equalErroneous;	// erroneous after last ok
  
  // 
  // Parameters related to object instance
  //
  
  list<ACS_USA_Criterion> criteriaList;	// List of criteria
  list<ACS_USA_Criterion *> ptrList;	// List of criteria
  ACS_USA_TimeType		currErroneous;	// current eroneous
  int                           equals;         // equal records during long or
                                                // short term analysis
  int                           erroneous;	// # of erroneous records so far

  vector<ACS_USA_EventInfo>     eventsToCease;  // List of pending events to cease
};



//******************************************************************************
//      setLogFile()
//******************************************************************************
inline
void
ACS_USA_AnalysisObject::setLogFile(String& str)
{
    fileName = str;
}



//******************************************************************************
//      setPosition()
//******************************************************************************
inline
void
ACS_USA_AnalysisObject::setPosition(String& str)
{
    position = str;
}



//******************************************************************************
//      setFormat()
//******************************************************************************
inline
void
ACS_USA_AnalysisObject::setFormat(String& str)
{
    format = str;
}



//******************************************************************************
//      getLogFile()
//******************************************************************************
inline
String&
ACS_USA_AnalysisObject::getLogFile()
{
    return fileName;
}



//******************************************************************************
//      getPosition()
//******************************************************************************
inline
String&
ACS_USA_AnalysisObject::getPosition()
{
    return position;
}



//******************************************************************************
//      getFormat()
//******************************************************************************
inline
String&
ACS_USA_AnalysisObject::getFormat()
{
    return format;
}
#endif
