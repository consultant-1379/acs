//******************************************************************************
// 
// NAME 
//        ACS_USA_EventInfo - Keeps track of events to auto cease in USA.
//
// COPYRIGHT
//      Ericsson AB 2008 - All rights reserved
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
//      ACS_USA_EventInfo, a class holding info. about events to auto cease.
//
// DOCUMENT NO
//      .
//
// AUTHOR 
//      2008-02-15	EUS/DR/DB EKAMSBA
//
// REV  DATE      NAME     DESCRIPTION
// A    080215    EKAMSBA  First version.
//
//******************************************************************************

#ifndef ACS_USA_EVENTINFO_H
#define ACS_USA_EVENTINFO_H


#include "ACS_USA_Global.h"
#include "ACS_USA_Time.h"


class File;


//******************************************************************************
// Member functions, constructors, destructors, operators
//******************************************************************************

class ACS_USA_EventInfo
{

public:

    ACS_USA_EventInfo();
    // Description:
    //   Default constructor.
    // Parameters: 
    //   none
    // Return value: 
    //   none

    ACS_USA_EventInfo(ACS_USA_TimeType          alarmTime, 
                      ACS_USA_TimeType          ceaseDuration,
                      ACS_USA_SpecificProblem   specificProblem,
                      String                    objectOfReference
                      );
    // Description:
    //   Constructor.
    // Parameters: 
    //   eventTime          Time when the alarm is raised
    //   ceaseDuration      Time to wait before raising a cease event
    //   specificProblem    Specific problem in AP event reports
    //   objectOfReference  Object of reference
    // Return value: 
    //   none

    ~ACS_USA_EventInfo();
    // Description:
    //   Destructor.
    // Parameters: 
    //   none
    // Return value: 
    //   none
    
	bool operator==(const ACS_USA_EventInfo& compare) const;
    // Description:
    //    equal operator.
    // Parameters:
    //    compare	object to compare to.
    // Return value:
    //    true	if equals
	//    false otherwise

    ACS_USA_TimeType alarmTime() const;
    // Description:
    //    returns the alarm time.
    // Parameters:
    //    none
    // Return value:
    //    ACS_USA_TimeType

    ACS_USA_TimeType ceaseDuration() const;
    // Description:
    //    returns the cease duration.
    // Parameters:
    //    none
    // Return value:
    //    ACS_USA_TimeType

    ACS_USA_SpecificProblem specificProblem() const;
    // Description:
    //    returns the specific problem of the event.
    // Parameters:
    //    none
    // Return value:
    //    ACS_USA_SpecificProblem

    const String& objectOfReference() const;
    // Description:
    //    returns the object of reference.
    // Parameters:
    //    none
    // Return value:
    //    const String&

    ACS_USA_StatusType read(File& file);
    // Description:
    //    Reads data from file.
    // Parameters:
    //    file          reference to a RWFile object to read from, in
    // Return value:
    //    ACS_USA_error           Something went wrong
    //    ACS_USA_ok              Everything ok

    ACS_USA_StatusType write(File& file);
    // Description:
    //    Writes data to file.
    // Parameters:
    //    file          reference to a RWFile object to write to, in
    // Return value:
    //    ACS_USA_error           Something went wrong
    //    ACS_USA_ok              Everything ok


private: 
    
    // 
    // Parameters related to object instance
    //

    ACS_USA_TimeType        m_alarmTime;         // Time when the alarm is raised.
    ACS_USA_TimeType        m_ceaseDuration;     // Time to wait before raising a cease event.
    ACS_USA_SpecificProblem m_specificProblem;   // Specific problem in AP event reports.
    String                  m_objectOfReference; // Object of reference.
};


//******************************************************************************
// operator==()
//******************************************************************************
inline
bool 
ACS_USA_EventInfo::operator==(const ACS_USA_EventInfo& compare) const
{
	return m_specificProblem == compare.m_specificProblem &&
		   m_objectOfReference == compare.m_objectOfReference ;
}

//******************************************************************************
// alarmTime()
//******************************************************************************
inline
ACS_USA_TimeType
ACS_USA_EventInfo::alarmTime() const
{
    return m_alarmTime;
}

//******************************************************************************
// ceaseDuration()
//******************************************************************************
inline
ACS_USA_TimeType
ACS_USA_EventInfo::ceaseDuration() const
{
    return m_ceaseDuration;
}

//******************************************************************************
// specificProblem()
//******************************************************************************
inline
ACS_USA_SpecificProblem
ACS_USA_EventInfo::specificProblem() const
{
    return m_specificProblem;
}

//******************************************************************************
// objectOfReference()
//******************************************************************************
inline
const String&
ACS_USA_EventInfo::objectOfReference() const
{
    return m_objectOfReference;
}


#endif