//******************************************************************************

// .NAME 
//  	  ACS_USA_ObjectManagerCollection - Manages a set of Object Managers.

// .LIBRARY 3C++
// .PAGENAME ACS_USA_ObjectManagerCollection
// .HEADER ACS  
// .LEFT_FOOTER Ericsson AB
// .INCLUDE ACS_USA_ObjectManagerCollection.h

// .COPYRIGHT
//  COPYRIGHT Ericsson AB, Sweden 2008.
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
// 	    This class contains methods for adding and retrieving Object Managers.

// .ERROR HANDLING
//	    General rule:
//	    The error handling is specified for each method.
//
//	    No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	    .

// AUTHOR 
// 	    2008-03-17 by DR/SD/M EKAMSBA

// REV  DATE    NAME     DESCRIPTION
// A	080317	EKAMSBA	 First Revision

// .LINKAGE
//

// .SEE ALSO 
// 	    ACS_USA_ObjectManager

//******************************************************************************

#ifndef ACS_USA_OBJECTMANAGERCOLLECTION_H 
#define ACS_USA_OBJECTMANAGERCOLLECTION_H


#include <vector>
#include <string>

class ACS_USA_ObjectManager;

typedef std::string String;

//******************************************************************************
//Member functions, constructors, destructors, operators
//******************************************************************************
class ACS_USA_ObjectManagerCollection
{
public:
	ACS_USA_ObjectManagerCollection();
	// Description:
	// 	Default constructor		Does nothing.
	// Parameters: 
	//    none
	// Return value: 
	//	  none

	~ACS_USA_ObjectManagerCollection();
	// Description:
	// 	Destructor	Release allocated managers.
	// Parameters: 
	//	none
	// Return value: 
	//	none

	ACS_USA_ObjectManager* addManager(const String& source);
	// Description:
	// 	Create dynamically managers and save them in the list.
	// Parameters: 
	//	source		event source handled by the created manager.
	// Return value: 
	//	pointer to newly allocated manager, or NULL if failed.

	std::vector<ACS_USA_ObjectManager *> getManagers() const;
	// Description:
	// 	return a copy of the manager list.
	// Parameters: 
	//	none
	// Return value: 
	//	the list of the managers.

	ACS_USA_ObjectManager* selectManager(const String& source) const;
	// Description:
	// 	select a manager based on the event source.
	// Parameters: 
	//	source	event source to be fetched.
	// Return value: 
	//	NULL					if not found.
	//	ACS_USA_ObjectManager*	pointer to the selected manager.


private:
	ACS_USA_ObjectManagerCollection( const ACS_USA_ObjectManagerCollection& );              // copy constructor
	ACS_USA_ObjectManagerCollection& operator= ( const ACS_USA_ObjectManagerCollection& );  // copy assignment

	std::vector<ACS_USA_ObjectManager *> m_aList;	// list of the manager
};


//******************************************************************************
// getManagers
//******************************************************************************
inline
std::vector<ACS_USA_ObjectManager *> 
ACS_USA_ObjectManagerCollection::getManagers() const
{
	// Return a COPY
	return m_aList;
}


#endif
