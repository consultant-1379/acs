//******************************************************************************
//******************************************************************************

#ifndef ACS_USA_ANALYSISOBJECT_H 
#define ACS_USA_ANALYSISOBJECT_H

#include <list>
#include <string>
#include <map>
#include <functional>
#include "acs_usa_control.h"
#include "acs_usa_criterion.h"
#include "acs_usa_global.h"
#include "acs_usa_eventInfo.h"


using namespace std;
//typedef string String;


//Member functions, constructors, destructors, operators
struct StringHash
{
        bool operator()(string a, string b) const
        {
		(void)a;
		(void)b;
                return true;
        }
};

typedef map<String, ACS_USA_Criterion, StringHash > CCriteriaListDictionary;
typedef CCriteriaListDictionary::iterator CCriteriaListDictionaryIterator;
typedef pair<String, ACS_USA_Criterion> CriteriaListPair;

//******************************************************************************
// Forward declaration(s)
//******************************************************************************
class 	ACS_USA_Config;

//******************************************************************************
// Member functions, constructors, destructors, operators
//******************************************************************************

class  ACS_USA_AnalysisObject
{

 friend class ACS_USA_Config;
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

  ACS_USA_ReturnType appendCriterion(const ACS_USA_Criterion& criterion);
  // Description:
  // 	Appends a criterion to the list.
  // Parameters: 
  //    storageCriterion	data for a criterion 
  // Return value: 
  //    ACS_USA_ok		append ok
  //	ACS_USA_error		append failed
 
  ACS_USA_ReturnType write(File& file);
  // Description:
  // 	Writes data to to file including all criteria.
  // Parameters:
  //	file			reference to a file object 
  // Return value: 
  //    ACS_USA_ok		append ok
  //	ACS_USA_error		append failed
  
  ACS_USA_ReturnType read(File& file);
  // Description:
  // 	Reads data from file object.
  // Parameters: 
  //	file			reference to a file object 
  // Return value: 
  //    ACS_USA_ok		append ok
  //	ACS_USA_error		append failed
  
  ACS_USA_ReturnType match(const ACS_USA_RecordType record,
			   const ACS_USA_TimeType recordTime,
			   const ACS_USA_ActivationType activation,
			   const vector<String>& insertedStrings = vector<String>(),
			   LPSTR dataBuffer=NULL,
			   DWORD  dataLength=0);
  // Description:
  // 	Scans the list to find a criterion match. Returns immediately 
  //	if a match was found.
  // Parameters: 
  //    record			record to be checked
  //	recordTime		the time record was logged
  // Return value: 
  //    ACS_USA_ok		everithing went ok
  //	ACS_USA_error		something went wrong

  ACS_USA_MatchType matchRecord(const ACS_USA_RecordType record,
			   const ACS_USA_TimeType recordTime,
			   const ACS_USA_ActivationType activation,
			   const vector<String>& insertedStrings = vector<String>(),
			   LPSTR dataBuffer=NULL,
			   DWORD  dataLength=0);
  // Description:
  // 	Scans the list to find a criterion match. Returns immediately 
  //	if a match was found.
  // Parameters: 
  //    record			record to be checked
  //	recordTime		the time record was logged
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

  ACS_USA_ReturnType validate();
  // Description:
  //	Validates the object
  // Parameters: 
  //    none
  // Return value: 
  //    ACS_USA_ok		everithing went ok
  //	ACS_USA_error		something went wrong
   
  ACS_USA_ReturnType checkAlarms(
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
  ACS_USA_ReturnType reset();
  ACS_USA_ReturnType clearInstAlarmsQue(ACS_USA_TimeType lastShortRecordTime);

  CCriteriaListDictionary criteriaListDictionary;
  std::list<ACS_USA_Criterion> criteriaList;	// List of criteria
  std::list<ACS_USA_Criterion *> ptrList;	// List of criteria

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
  String format; 	// Time stamp format
  String position; 	// Time stamp position
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


//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2010
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------


