//******************************************************************************
// 
// .NAME 
//  	ACS_USA_Initialiser - Initialisation and deinitialisation
//				of USA
// .LIBRARY 3C++
// .PAGENAME ACS_USA_Initialiser
// .HEADER  TMOS/ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE ACS_USA_Initialiser.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1995-1999.
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
//	ACS_USA_Initialiser retrieves and stores configuration data 
//	from the Temporary Storage. 

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 0259

// AUTHOR 
// 	1995-06-27 by ETX/TX/T XKKHEIN

// .REVISION 
//	PA2	990915

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//	PA1     950829  XKKHEIN First Revision
//	PA2     990915  UABDMT  Ported to Windows NT4.0
//	PA3     040429  QVINKAL Removal of RougeWave Tools.h++
//	PA4     071231  EKAMSBA General Error filtering.
//	PA5     080314  EKAMSBA PRC alarms handling.
//	PA6     080409  EKAMSBA Temp. file versioning.
// .LINKAGE
//	

// .SEE ALSO 
// 	ACS_USA_AnalysisObject

//******************************************************************************

#ifndef ACS_USA_INITIALISER_H 
#define ACS_USA_INITIALISER_H

#include "ACS_USA_Global.h"

//******************************************************************************
// Forward declaration(s)
//******************************************************************************
class ACS_USA_ObjectManager;
class ACS_USA_ObjectManagerCollection;

//******************************************************************************
// Member functions, constructors, destructors, operators
//******************************************************************************


class ACS_USA_Initialiser
{

 public:

  ACS_USA_Initialiser();
  // Description:
  // 	Constructor 
  // Parameters:
  //	none
  // Return value: 
  //    none

  virtual ~ACS_USA_Initialiser();
  // Description:
  // 	Destructor. Releases all Analysis Objects
  // Parameters:
  //	none
  // Return value: 
  //    none

  ACS_USA_Boolean integrityCheck(const char*filename) const;
  // Description:
  // 	Check the integrity of the file. 
  // Parameters:
  //	filename		file to check
  // Return value: 
  //    ACS_USA_True			file is Ok.
  //	ACS_USA_False			an error occurred.

  ACS_USA_StatusType loadTempStorage(
				     ACS_USA_ObjectManagerCollection& managers,
				     const char*filename);
  // Description:
  // 	Loads Temporary  Storage file 
  // Parameters:
  //	managers	list of managers
  // Return value: 
  //    ACS_USA_ok		everything OK
  //	ACS_USA_error		error occurred
    
  ACS_USA_StatusType update(
			    ACS_USA_ObjectManagerCollection& managers,
			    const char*filename);
  // Description:
  // 	Updates alarm information in Analysis Objects. This
  //	information is retrieved from the Temporary Storage file 
  // Parameters:
  //	managers		list of managers
  // Return value: 
  //    ACS_USA_ok		everything OK
  //	ACS_USA_error		error occurred

  ACS_USA_StatusType flush(
				ACS_USA_ObjectManagerCollection& managers,
				const char*filename);
  // Description:
  //	Flushes Analysis Objects to Temporary Storage
  // Parameters:
  //	managers		list of managers
  // Return value: 
  //    ACS_USA_ok		everything OK
  //	ACS_USA_error		error occurred

  ACS_USA_StatusType removeGeneralError(ACS_USA_ObjectManager& manager,
					const String& filter);
  // Description:
  //    Remove General Error criteria from Analysis Objects
  // Parameters:
  //    manager			class where Analysis Objects are stored
  //    filter			filters the criteria to remove
  // Return value: 
  //    ACS_USA_ok		everything OK
  //    ACS_USA_error		error occurred

private:


};

 
#endif
