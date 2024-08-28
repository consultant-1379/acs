//******************************************************************************
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
