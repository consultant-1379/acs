//******************************************************************************
//
// .NAME 
//  	  ACS_USA_FileVersioning.h
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.
//
// .DESCRIPTION 
// 	    This class contains methods for setting and checking file version.
//
// DOCUMENT NO
//	    .
//
// AUTHOR 
// 	    2008-04-10 by DR/SD/M EKAMSBA
//
// REV  DATE    NAME     DESCRIPTION
// A	080410	EKAMSBA	 First Revision
//
//******************************************************************************

#ifndef ACS_USA_FILEVERSIONING_H 
#define ACS_USA_FILEVERSIONING_H



//******************************************************************************
// Type definitions 
//******************************************************************************
typedef long     ACS_USA_FileVersion;        // File version in USA


//******************************************************************************
//Member functions, constructors, destructors, operators
//******************************************************************************
class ACS_USA_FileVersioning
{
public:
	
    ACS_USA_FileVersioning( short revision );
	// Description:
	// 	Constructor				Does initialisation.
	// Parameters: 
	//    revision				revision of the file
	// Return value: 
	//	  none

	~ACS_USA_FileVersioning();
	// Description:
	// 	Destructor				Does nothing.
	// Parameters: 
	//	none
	// Return value: 
	//	none

	ACS_USA_FileVersion getVersion() const;
	// Description:
	// 	Calculate the file version from the revision.
	// Parameters: 
	//	none
	// Return value: 
	//	ACS_USA_FileVersion		the file version.

	bool isCompatible(ACS_USA_FileVersion version) const;
	// Description:
	// 	Check if the file versions are compatible.
	// Parameters: 
	//	version		version of the file to verify.
	// Return value: 
	//	true		file are compatible.
	//	false		file aren't compatible.


private:
	const short	revision_;		// The revision of the file.
};


#endif
