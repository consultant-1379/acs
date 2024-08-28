//*******************************************************************************
//
// NAME
//      ACS_USA_Initialiser.cpp
//
// COPYRIGHT Ericsson AB, Sweden 1995-2004.
// All rights reserved.
//
// The Copyright to the computer program(s) herein 
// is the property of Ericsson AB, Sweden.
// The program(s) may be used and/or copied only with 
// the written permission from Ericsson AB or in 
// accordance with the terms and conditions stipulated in the 
// agreement/contract under which the program(s) have been 
// supplied .

// .DESCRIPTION
//      ACS_USA_Initialiser handles storage and retrievel of
//		configuration parameters in the temporary storage.

// DOCUMENT NO
//      190 89-CAA 109 0259

// AUTHOR 
// 	    1995-06-30 by ETX/TX/T XKKHEIN

// REV	DATE		NAME 		DESCRIPTION
// A	950829		XKKHEIN		First version.
// B	990915		UABDMT		Ported to Windows NT 4.0.
// C	040215		UABPEK		Reducing risk of locking of file usa.tmp.
// D	040429		QVINKAL		Removal of RougeWave Tools.h++.
// E	071231		EKAMSBA		General Error filtering.
// F	080314		EKAMSBA		PRC alarms handling.
// G	080409		EKAMSBA		Temp. file versioning.
// SEE ALSO
// 	
//
//******************************************************************************

#include <string>
#include "ACS_USA_Regexp.h"
#include "ACS_USA_ObjectManager.h"
#include "ACS_USA_AnalysisObject.h"
#include "ACS_USA_Criterion.h"
#include "ACS_USA_Initialiser.h"
#include "ACS_USA_File.h"
#include "ACS_USA_ObjectManagerCollection.h"
#include "ACS_USA_FileVersioning.h"

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
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(filename, &FindFileData);

	if( hFind == INVALID_HANDLE_VALUE )  
	{
		if( ERROR_FILE_NOT_FOUND == GetLastError() )
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
	else 
	{
		// The file exists.		
		FindClose(hFind);
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
ACS_USA_StatusType
ACS_USA_Initialiser::loadTempStorage(
				     ACS_USA_ObjectManagerCollection& managers,		 
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
		int		count = 0;
		int 	no = 0;
		    
		//
		// Validate file object
		//
		if (file.isValid() == false) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_CannotOpenFile);
			return ACS_USA_error;
		}
    

		//
		// Read the file version
		//
		if (file.Read(version) == false) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_ReadFailed);
			return ACS_USA_error;
		}

		// Check compatibility of the file version
		ACS_USA_FileVersioning usaTemp( ACS_USA_UsaTempRevision );
		if( false == usaTemp.isCompatible(version) )
		{
			error.setError(ACS_USA_BadState);
            error.setErrorText( "ACS_USA_Initialiser::loadTempStorage",
                                filename,
                                "Bad revision or old file format" );

			return ACS_USA_error;
		}


		//
		// Read the number of object managers
		//
		if (file.Read(count) == false) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_ReadFailed);
			return ACS_USA_error;
		}


		//
		// Load object managers sequentially
		//
		for(int om = 0; om < count; om++)
		{
			// Read the event source of the object manager.
			String eventSrc;
			file >> eventSrc;

			ACS_USA_ObjectManager* mgr = managers.addManager( eventSrc );
			if (mgr == NULL) 
			{
				error.setError(ACS_USA_Memory);
				error.setErrorText("new() failed in ACS_USA_Initialiser::loadTempStorage().");
				return ACS_USA_error;	
			}

			//
			// Read the number of Analysis Objects
			//
			if (file.Read(no) == false) 
			{
				error.setError(ACS_USA_FileIOError);
				error.setErrorText(a.data(), ACS_USA_ReadFailed);
				return ACS_USA_error;
			}

			//
			// Read data for Analysis objects by creating 
			// new Analysis Object that reads data itself
			//
			for (int i = 0; i < no; i++) 
			{
				ACS_USA_AnalysisObject *object = new ACS_USA_AnalysisObject;
				if (object == NULL) 
				{
					error.setError(ACS_USA_Memory);
					error.setErrorText("new() failed in ACS_USA_Initialiser::loadTempStorage().");
					return ACS_USA_error;	
				}

				if (object->read(file) == ACS_USA_error)
				{
					if (error.getError() == ACS_USA_FileIOError) 
					{
						error.setErrorText(a.data(), error.getErrorText());
					}

					delete object; //Tr HI60745
					return ACS_USA_error;
				}

				mgr->append(object);
			}
		}
	}
	catch(...)
	{
		error.setError(ACS_USA_FileIOError);
		error.setErrorText(a.data(), ACS_USA_ReadFailed);
		return ACS_USA_error;
	}
    return ACS_USA_ok;   
}

//******************************************************************************
//	update()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Initialiser::update(
				ACS_USA_ObjectManagerCollection& managers,
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
		int		count = 0;
		int 	no = 0;
		
		// Is the file object valid
		if (file.isValid() == false) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_CannotOpenFile);
			return ACS_USA_error;
		}
    
		// The temp storage doesn't exist if USA is run for the first time
		if (file.isEmpty()  == true) 
		{
			USA_DEBUG(logMsg("Update: no temp storage\n"));
			return ACS_USA_ok;
		}
    
		//
		// Read the file version
		//
		if (file.Read(version) == false) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_ReadFailed);
			return ACS_USA_error;
		}

		// Check compatibility of the file version
		ACS_USA_FileVersioning usaTemp( ACS_USA_UsaTempRevision );
		if( false == usaTemp.isCompatible(version) )
		{
			error.setError(ACS_USA_BadState);
            error.setErrorText( "ACS_USA_Initialiser::update",
                                filename,
                                "Bad revision or old file format" );

			return ACS_USA_error;
		}


		//
		// Read the number of object managers
		//
		if (file.Read(count) == false) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_ReadFailed);
			return ACS_USA_error;
		}

		// Retrieve the list of managers
		vector<ACS_USA_ObjectManager*> aManagers = managers.getManagers();
		if ( count > aManagers.size() )
		{
			error.setError(ACS_USA_BadState);
			error.setErrorText("Object manager count mismatch.");
			return ACS_USA_error;
		}


		//
		// Load object managers sequentially
		//
		for(int om = 0; om < count; om++)
		{
			ACS_USA_ObjectManager* manager = aManagers[om];
			if( NULL == manager )
			{
				error.setError(ACS_USA_BadState);
				error.setErrorText("NULL ACS_USA_ObjectManager object in ACS_USA_Initialiser::update().");
				return ACS_USA_error;	
			}


			// Read the event source of the object manager.
			String eventSrc;
			file >> eventSrc;

			if (file.Read(no) == false) 
			{
				error.setError(ACS_USA_FileIOError);
				error.setErrorText(a.data(), ACS_USA_ReadFailed);
				return ACS_USA_error;
			}

			//
			// Read all Analysis Objects from the disk one by one. Each read object
			// is compared with Analysis Objects in the Object Manager.
			//
			for (int i = 0; i < no; i++) 
			{
				ACS_USA_AnalysisObject *object = new ACS_USA_AnalysisObject;
				if (object == NULL) 
				{
					error.setError(ACS_USA_Memory);
					error.setErrorText("new() failed in ACS_USA_Initialiser::update().");
					return ACS_USA_error;	
				}

				if (object->read(file) == ACS_USA_error) 
				{
					if (error.getError() == ACS_USA_FileIOError)
					{
						error.setErrorText(a.data(), error.getErrorText());
					}

					delete object; // TR HI60745
					return ACS_USA_error;
				}


				int nb = manager->getObjectCount();
				ACS_USA_AnalysisObject *mo = manager->getFirst();

				for (int j = 0; j < nb; j++) 
				{
					mo->update(*object);
					mo = manager->getNext();
				}
				object->DestroyPtrList();
				delete object;
			}
		}
	}
    catch(...)
	{
		error.setError(ACS_USA_FileIOError);
		error.setErrorText(a.data(), ACS_USA_ReadFailed);
		return ACS_USA_error;
		
	}

    return ACS_USA_ok;   
}

//******************************************************************************
//	flush()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Initialiser::flush(
			   ACS_USA_ObjectManagerCollection& managers,
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
		int 	no = 0;
	
		if (file.isValid() == false) {
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_CannotOpenFile);
			return ACS_USA_error;
		}


		// Write the file version
		ACS_USA_FileVersioning usaTemp( ACS_USA_UsaTempRevision );		
		if (file.Write( usaTemp.getVersion() ) == false) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_WriteFailed);
			return ACS_USA_error;
		}


		// Retrieve the list of managers
		vector<ACS_USA_ObjectManager*> aManagers = managers.getManagers();
		
		// Write the number of object managers.
		if (file.Write(aManagers.size()) == false) 
		{
			error.setError(ACS_USA_FileIOError);
			error.setErrorText(a.data(), ACS_USA_WriteFailed);
			return ACS_USA_error;
		}

		// - Save object managers sequentially...
		vector<ACS_USA_ObjectManager*>::iterator iElem = aManagers.begin();
		for( ; iElem != aManagers.end(); iElem++)
		{
			ACS_USA_ObjectManager* mgr = *iElem;

			if( NULL == mgr )
			{
				error.setError(ACS_USA_BadState);
				error.setErrorText("NULL ACS_USA_ObjectManager object in ACS_USA_Initialiser::flush().");
				return ACS_USA_error;	
			}


			// Write the event source of the object manager.
			file << mgr->getSource();

			no = mgr->getObjectCount();
	    
			// Write the number of object in object manager.
			if (file.Write(no) == false) 
			{
				error.setError(ACS_USA_FileIOError);
				error.setErrorText(a.data(), ACS_USA_WriteFailed);
				return ACS_USA_error;
			}

			ACS_USA_AnalysisObject *o = mgr->getFirst();

			// Write all analysis objects.
			for ( int i = 0; i < no; i++) 
			{
				if (o->write(file) == ACS_USA_error)
				{
					if (error.getError() == ACS_USA_FileIOError) 
					{
						error.setErrorText(a.data(), error.getErrorText());
					}
					return ACS_USA_error;
				}
				o = mgr->getNext();
			}
		}
	}
    catch(...)
	{
		error.setError(ACS_USA_FileIOError);
		error.setErrorText(a.data(), ACS_USA_WriteFailed);
		return ACS_USA_error;
	}

    return ACS_USA_ok;
}  

//******************************************************************************
//  removeGeneralError()
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Initialiser::removeGeneralError(
					ACS_USA_ObjectManager& manager,
					const String& filter)
{
    ACS_USA_AnalysisObject  *o = manager.getFirst();
    int no = manager.getObjectCount();

    // Remove not needed criteria from all analysis objects.
    for ( int i = 0; i < no; i++) 
    {
        if (o->removeGeneralErrorCriterion(filter) == ACS_USA_error) 
        {
            return ACS_USA_error;
        }

        o = manager.getNext();
    }

    return ACS_USA_ok;
}
//******************************************************************************
