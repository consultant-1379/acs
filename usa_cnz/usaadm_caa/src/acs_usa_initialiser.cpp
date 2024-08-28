//*******************************************************************************
//
//
//******************************************************************************

#include <string>
#include "acs_usa_regexp.h"
#include "acs_usa_analysisObject.h"
#include "acs_usa_criterion.h"
#include "acs_usa_initialiser.h"
#include "acs_usa_file.h"
#include "acs_usa_fileVersioning.h"
#include "acs_usa_error.h"

using namespace std;
typedef string String;


//******************************************************************************
// Constants used locally
//******************************************************************************
const short	ACS_USA_UsaTempRevision = 1;	// Current 'usa.tmp' file revision.




//******************************************************************************
//	ACS_USA_Initialiser()
//******************************************************************************
ACS_USA_Initialiser::ACS_USA_Initialiser()
{
}  

//******************************************************************************
//	~ACS_USA_Initialiser()
//******************************************************************************
ACS_USA_Initialiser::~ACS_USA_Initialiser()
{
}  


//******************************************************************************
//	integrityCheck()
//******************************************************************************
ACS_USA_Boolean
ACS_USA_Initialiser::integrityCheck(const char* filename) const
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                      !
	// ! if changing that function will impacts the file ‘usa.tmp’, !
	// ! Then increment the revision ‘ACS_USA_UsaTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	String 	a(filename);
	a += ioErrorPrefix;

	// Check if the file exists
	int retCode = access(filename, F_OK);

	if( retCode == -1 )  
	{
		if( ENOENT == errno )
		{
			// File not found. 
			// That's fine, it will be created.
			return ACS_USA_True;
		}
		else
		{
			error.setError(ACS_USA_SystemCallError);
			error.setErrorText(a.data(), "Invalid handle in ACS_USA_Initialiser::integrityCheck.");
			return ACS_USA_False;
		}
	} 

	try
	{
		File 	file(filename);
		ACS_USA_FileVersion	version = 0;

		// Is the file object valid
		if (file.isValid() == false) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_CannotOpenFile);
			return ACS_USA_False;
		}
    		
		if (file.isEmpty()  == true) 
		{			
			// The file is empty.
			// That's fine, this case is handled by update() function
			return ACS_USA_True;
		}
    
		//
		// Read the file version
		//
		if (file.Read(version) == false) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_ReadFailed);
			return ACS_USA_False;
		}

		// Check compatibility of the file version
		ACS_USA_FileVersioning usaTemp( ACS_USA_UsaTempRevision );
		if( false == usaTemp.isCompatible(version) )
		{
			error.setError(ACS_USA_BadState);
			error.setErrorText( a.data(), "Bad revision or old file format, ACS_USA_Initialiser::integrityCheck." );
			return ACS_USA_False;
		}
	}
	catch(...)
	{
		error.setError(ACS_USA_FileIOError);
		error.setErrorText(a.data(), ACS_USA_ReadFailed);
		return ACS_USA_False;		
	}

	// File is OK.
    return ACS_USA_True;
}


//******************************************************************************
//	loadTempStorage()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_Initialiser::loadTempStorage(
				     ACS_USA_AnalysisObject* analysisObj,		 
				     const char* filename)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                      !
	// ! if changing that function will impacts the file ‘usa.tmp’, !
	// ! Then increment the revision ‘ACS_USA_UsaTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	String 	a(filename);
	a += ioErrorPrefix;
	try
	{
		File 	file(filename);
		ACS_USA_FileVersion	version = 0;
		    
		// Validate file object
		//
		if (file.isValid() == false) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_CannotOpenFile);
			return ACS_USA_Error;
		}
    

		//
		// Read the file version
		//
		if (file.Read(version) == false) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_ReadFailed);
			return ACS_USA_Error;
		}

		// Check compatibility of the file version
		ACS_USA_FileVersioning usaTemp( ACS_USA_UsaTempRevision );
		if( false == usaTemp.isCompatible(version) )
		{
			error.setError(ACS_USA_BadState);
			error.setErrorText( "ACS_USA_Initialiser::loadTempStorage",
                                filename,
                                "Bad revision or old file format" );

			return ACS_USA_Error;
		}

		if (analysisObj->read(file) == ACS_USA_Error)
		{
			if (error.getError() == ACS_USA_FileIOError) 
			{
				error.setErrorText(a.data(), error.getErrorText());
			}

			return ACS_USA_Error;
		}

	}
	catch(...)
	{
		error.setError(ACS_USA_FileIOError);
		error.setErrorText(a.data(), ACS_USA_ReadFailed);
		return ACS_USA_Error;
	}
    return ACS_USA_Ok;   
}

//******************************************************************************
//	update()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_Initialiser::update(
			     ACS_USA_AnalysisObject& analysisObj,		 
			     const char* filename)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                      !
	// ! if changing that function will impacts the file ‘usa.tmp’, !
	// ! Then increment the revision ‘ACS_USA_UsaTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	String 	a(filename);
	a += ioErrorPrefix;
	try
	{
		File 	file(filename);
		ACS_USA_FileVersion	version = 0;
		
		// Is the file object valid
		if (file.isValid() == false) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_CannotOpenFile);
			return ACS_USA_Error;
		}
    
		// The temp storage doesn't exist if USA is run for the first time
		if (file.isEmpty()  == true) 
		{
			return ACS_USA_Ok;
		}
    
		//
		// Read the file version
		//
		if (file.Read(version) == false) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_ReadFailed);
			return ACS_USA_Error;
		}

		// Check compatibility of the file version
		ACS_USA_FileVersioning usaTemp( ACS_USA_UsaTempRevision );
		if( false == usaTemp.isCompatible(version) )
		{
			error.setError(ACS_USA_BadState);
			error.setErrorText( "ACS_USA_Initialiser::update",
                                filename,
                                "Bad revision or old file format" );

			return ACS_USA_Error;
		}



		if (analysisObj.read(file) == ACS_USA_Error) 
		{
			if (error.getError() == ACS_USA_FileIOError)
			{
				error.setErrorText(a.data(), error.getErrorText());
			}

					return ACS_USA_Error;
		}
	}
    catch(...)
	{
		error.setError(ACS_USA_FileIOError);
		error.setErrorText(a.data(), ACS_USA_ReadFailed);
		return ACS_USA_Error;
		
	}

    return ACS_USA_Ok;   
}

//******************************************************************************
//	flush()
//******************************************************************************
ACS_USA_ReturnType
ACS_USA_Initialiser::flush(ACS_USA_AnalysisObject& analysisObj, const char* filename)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ! Note:                                                      !
	// ! if changing that function will impacts the file ‘usa.tmp’, !
	// ! Then increment the revision ‘ACS_USA_UsaTempRevision’.     !
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	String 	a(filename);
	a += ioErrorPrefix;
	try
	{
		File 	file(filename);
	
		if (file.isValid() == false) {
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_CannotOpenFile);
			return ACS_USA_Error;
		}

		// Write the file version
		ACS_USA_FileVersioning usaTemp( ACS_USA_UsaTempRevision );		
		if (file.Write( usaTemp.getVersion() ) == false) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_WriteFailed);
			return ACS_USA_Error;
		}

		if (analysisObj.write(file) == ACS_USA_Error)
		{
			if (error.getError() == ACS_USA_FileIOError) 
			{
				error.setErrorText(a.data(), error.getErrorText());
			}
			return ACS_USA_Error;
		}
	}
    catch(...)
	{
		error.setError(ACS_USA_FileIOError);
		error.setErrorText(a.data(), ACS_USA_WriteFailed);
		return ACS_USA_Error;
	}

    return ACS_USA_Ok;
}  

//******************************************************************************
