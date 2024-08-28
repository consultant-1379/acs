//******************************************************************************
// 
//******************************************************************************

#ifndef ACS_USA_INITIALISER_H 
#define ACS_USA_INITIALISER_H

#include "acs_usa_types.h"

//******************************************************************************
// Forward declaration(s)
//******************************************************************************

class ACS_USA_AnalysisObject;
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

  ACS_USA_Boolean integrityCheck(const char* filename) const;
  // Description:
  // 	Check the integrity of the file. 
  // Parameters:
  //	filename		file to check
  // Return value: 
  //    ACS_USA_True			file is Ok.
  //	ACS_USA_False			an error occurred.

  ACS_USA_ReturnType loadTempStorage(
				     ACS_USA_AnalysisObject* analysisObj,
				     const char* filename);
  // Description:
  // 	Loads Temporary  Storage file 
  // Parameters:
  //	managers	list of managers
  // Return value: 
  //    ACS_USA_ok		everything OK
  //	ACS_USA_error		error occurred
    
  ACS_USA_ReturnType update(
				ACS_USA_AnalysisObject& analysisObj,
			    	const char* filename);
  // Description:
  // 	Updates alarm information in Analysis Objects. This
  //	information is retrieved from the Temporary Storage file 
  // Parameters:
  //	managers		list of managers
  // Return value: 
  //    ACS_USA_ok		everything OK
  //	ACS_USA_error		error occurred

  ACS_USA_ReturnType flush(
				ACS_USA_AnalysisObject& analysisObj,
				const char*filename);
  // Description:
  //	Flushes Analysis Objects to Temporary Storage
  // Parameters:
  //	managers		list of managers
  // Return value: 
  //    ACS_USA_ok		everything OK
  //	ACS_USA_error		error occurred


private:


};

 
#endif
