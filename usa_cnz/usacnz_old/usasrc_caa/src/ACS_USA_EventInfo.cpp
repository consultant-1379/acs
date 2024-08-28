//******************************************************************************
//
// NAME
//      ACS_USA_EventInfo.cpp
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

#include "ACS_USA_EventInfo.h"
#include "ACS_USA_File.h"



//******************************************************************************
//  ACS_USA_EventInfo()
//******************************************************************************
ACS_USA_EventInfo::ACS_USA_EventInfo()
        : m_alarmTime( initialTime ), 
          m_ceaseDuration( initialTime ),
          m_specificProblem( 0 ),
          m_objectOfReference()
{}

//******************************************************************************
//  ACS_USA_EventInfo(...)
//******************************************************************************
ACS_USA_EventInfo::ACS_USA_EventInfo(ACS_USA_TimeType          alarmTime, 
                                     ACS_USA_TimeType          ceaseDuration,
                                     ACS_USA_SpecificProblem   specificProblem,
                                     String                    objectOfReference
                                )
        : m_alarmTime( alarmTime ), 
          m_ceaseDuration( ceaseDuration ),
          m_specificProblem( specificProblem ),
          m_objectOfReference( objectOfReference )
{}

//******************************************************************************
//  ~ACS_USA_EventInfo()
//******************************************************************************
ACS_USA_EventInfo::~ACS_USA_EventInfo() 
{}

//******************************************************************************
//  read()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_EventInfo::read(File& file)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                        !
	// ! if changing that function will impacts the file ‘usa.tmp’,   !
	// ! Then increment the revision ‘ACS_USA_UsaTempRevision’.       !
	//                                                                !
	// ! if changing that function will impacts the file ‘cease.tmp’, !
	// ! Then increment the revision ‘ACS_USA_CeaseTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // 
    // Read the data from 'file' into current object
    //
    try
    {
        file >> m_objectOfReference;

        if (file.Error() ||        
            file.Read(m_alarmTime)       == false ||
            file.Read(m_ceaseDuration)   == false ||
            file.Read(m_specificProblem) == false ) 
        {
            error.setError(ACS_USA_FileIOError);
            error.setErrorText(ACS_USA_ReadFailed);
            USA_DEBUG(logMsg("EI:read error:\n"));
            return ACS_USA_error;
        }
    }
    catch (...)
    {
        error.setError(ACS_USA_FileIOError);
        error.setErrorText(ACS_USA_ReadFailed);
        USA_DEBUG(logMsg("EI:read error: %s\n", error.getErrorText()));
        return ACS_USA_error;
    }
    return ACS_USA_ok;
}

//******************************************************************************
//  write()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_EventInfo::write(File& file)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                        !
	// ! if changing that function will impacts the file ‘usa.tmp’,   !
	// ! Then increment the revision ‘ACS_USA_UsaTempRevision’.       !
	//                                                                !
	// ! if changing that function will impacts the file ‘cease.tmp’, !
	// ! Then increment the revision ‘ACS_USA_CeaseTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // 
    // Write data to 'file'
    //
    try
    {
        file << m_objectOfReference;

        if (file.Error() || 
            file.Write(m_alarmTime)         == false ||
            file.Write(m_ceaseDuration)     == false ||
            file.Write(m_specificProblem)   == false )
        {
            error.setError(ACS_USA_FileIOError);
            error.setErrorText(ACS_USA_WriteFailed);
            USA_DEBUG(logMsg("EI:write error:\n"));
            return ACS_USA_error;
        }
    }
    catch(...)
    {
        error.setError(ACS_USA_FileIOError);
        error.setErrorText(ACS_USA_WriteFailed);
        USA_DEBUG(logMsg("EI:write error:\n"));
        return ACS_USA_error;
    }
    return ACS_USA_ok;
}


//******************************************************************************