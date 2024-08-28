//******************************************************************************

// .NAME 
//  	  ACS_USA_ObjectManager - Storing and retrieving of Analysis Objects

// .LIBRARY 3C++
// .PAGENAME ACS_USA_ObjectManager
// .HEADER ACS  
// .LEFT_FOOTER Ericsson AB
// .INCLUDE ACS_USA_ObjectManager.h

// .COPYRIGHT
//  COPYRIGHT Ericsson AB, Sweden 1995, 1999, 2004.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
// 	    This class contains methods for storing and retrieving Analysis Objects.

// .ERROR HANDLING
//	    General rule:
//	    The error handling is specified for each method.
//
//	    No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	    190 89-CAA 109 0545

// AUTHOR 
// 	    1995-06-20 by ETX/TX/T XKKHEIN

// REV  DATE    NAME     DESCRIPTION
// A		950829	XKKHEIN	 First Revision
// B		990915	UABDMT	 Ported to Windows NT4.0
// C		040429	QVINKAL	 Removal of RougeWave Tools.h++.
// D		041223	UABPEK	 Supervision of three new event logs.
// E		080311	EKAMSBA	 PRC alarms handling.

// .LINKAGE
//

// .SEE ALSO 
// 	    ACS_USA_AnalysisObject

//******************************************************************************

#ifndef ACS_USA_OBJECT_MANAGER_H 
#define ACS_USA_OBJECT_MANAGER_H

#include <sys/types.h>

#include "ACS_USA_Global.h"
#include "ACS_USA_AnalysisObject.h"



//******************************************************************************
// Type definitions local to this class
//******************************************************************************
typedef  size_t	Size;			// Type for vector size


//******************************************************************************
// Constants used in this class
//******************************************************************************
const	Size		initialSize  = 6;	// initial size	
const	Size		resizeValue  = 5;	// when resizing
const	ACS_USA_Index	initialIndex = 0;	// getFirst


//******************************************************************************
//Forward declaration(s)
//******************************************************************************


//******************************************************************************
//Member functions, constructors, destructors, operators
//******************************************************************************

class ACS_USA_ObjectManager
{

 public:

  ACS_USA_ObjectManager(const String& src);
  // Description:
  // 	Default constructor. Does initialisations.
  // Parameters: 
  //    src			event source for the object manager
  // Return value: 
  //	none

  virtual ~ACS_USA_ObjectManager();
  // Description:
  // 	Destructor. Does nothing
  // Parameters: 
  //	none
  // Return value: 
  //	none

  void append(ACS_USA_AnalysisObject* object);
  // Description:
  // 	Adds pointer to an Analysis Object into internal list
  // Parameters: 
  //    object			pointer to an Analysis Object
  // Return value: 
  //    ACS_USA_error 		appending of the object failed
  //	ACS_USA_ok		appending of the object successful

  ACS_USA_AnalysisObject* getFirst();
  // Description:
  // 	Returns the first Analysis Object. 
  // Parameters: 
  //	none
  // Return value: 
  //	0			if empty
  //	ACS_USA_AnalysisObject* pointer of the first Analysis Object 
  //				in internal list

  ACS_USA_AnalysisObject* getNext();
  // Description:
  // 	Returns next Analysis Object from the list
  // Parameters: 
  //	none
  // Return value: 
  //	0 			if empty
  //	ACS_USA_AnalysisObject* pointer of the first Analysis Object 
  //				in internal list

  ACS_USA_Index getObjectCount() const;
  // Description:
  // 	Returns the number of objects in vector
  // Parameters: 
  //	none
  // Return value: 
  //	0 			if empty
  //	ACS_USA_AnalysisObject* pointer of the first Analysis Object 
  //				in internal list

  String getSource() const;
  // Description:
  // 	Returns the event source handled by the object manager.
  // Parameters: 
  //	none
  // Return value: 
  //	String 			the event source.

  ACS_USA_AnalysisObject* selectAnalysisObject(const String& fileName);
  // Description:
  // 	Return the Analysis Object corresponding to the fileName.
  // Parameters: 
  //	fileName	the log file name.
  // Return value: 
  //	NULL						if not found.
  //	ACS_USA_AnalysisObject*		pointer of the selected Analysis Object

private:
    ACS_USA_ObjectManager ( const ACS_USA_ObjectManager& );             // copy constructor
    ACS_USA_ObjectManager& operator= ( const ACS_USA_ObjectManager& );  // copy assignment


  vector<ACS_USA_AnalysisObject *>  ptrVec ;	// vector	
  vector<ACS_USA_AnalysisObject> vec ;	// vector
  ACS_USA_Index	index; 		// Index of the current object
  Size		size;		// Current size
  ACS_USA_Index	last;		// last item in vector

  String eventSource;		// the event source that the manager handles.
};

//******************************************************************************
// Type definitions local to this class
//******************************************************************************
inline
ACS_USA_Index
ACS_USA_ObjectManager::getObjectCount() const
{
    return last;
}

inline
String
ACS_USA_ObjectManager::getSource() const
{
	return eventSource;
}

#endif
