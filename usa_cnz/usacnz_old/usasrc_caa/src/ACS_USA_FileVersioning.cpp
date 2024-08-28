//******************************************************************************
//
// .NAME 
//  	  ACS_USA_FileVersioning.cpp
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

#include "ACS_USA_FileVersioning.h"


//******************************************************************************
// Constants used locally
//******************************************************************************
const short	MAGIC_NUMBER = 0xFF00;			// Used to build the file version.




//******************************************************************************
//	ACS_USA_FileVersioning()
//******************************************************************************
ACS_USA_FileVersioning::ACS_USA_FileVersioning( short revision )
	: revision_( revision )
{
}

//******************************************************************************
//	~ACS_USA_FileVersioning()
//******************************************************************************
ACS_USA_FileVersioning::~ACS_USA_FileVersioning()
{
}

//******************************************************************************
//	getVersion()
//******************************************************************************
ACS_USA_FileVersion
ACS_USA_FileVersioning::getVersion() const
{
	//                  11
	//	31              65              0
	//   |------||------||------||------|
	//   |  MagicNumber ||   Revision   |
	//	 |------||------||------||------|		
	ACS_USA_FileVersion version = (MAGIC_NUMBER << 16) | revision_;			 
	return version;
}

//******************************************************************************
//	isCompatible()
//******************************************************************************
bool
ACS_USA_FileVersioning::isCompatible(ACS_USA_FileVersion version) const
{
	//                  11
	//	31              65              0
	//   |------||------||------||------|
	//   |  MagicNumber ||   Revision   |
	//	 |------||------||------||------|
	if( MAGIC_NUMBER == (version >> 16) )
	{
		if( revision_ == (version & 0xFFFF) )
		{
			// Compatibility
			return true;
		}

		// Old revision !
		return false;
	}

	// Old file format !
	return false;
}

//******************************************************************************



















