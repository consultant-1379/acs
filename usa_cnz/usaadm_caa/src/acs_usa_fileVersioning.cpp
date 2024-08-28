//******************************************************************************
//
//
//******************************************************************************

#include "acs_usa_fileVersioning.h"


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
