/*=================================================================== */
/**
@file acs_chb_file_handler.cpp

Class method implementationn for ACS_CHB_filehandler class.

@version 1.0.0

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       25/10/2010   XTANAGG   Initial Release
**/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ace/Configuration.h>
#include <ace/Configuration_Import_Export.h>
#include <ace/OS_NS_ctype.h>
#include <ace/OS_NS_errno.h>
#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_unistd.h>
#include <acs_chb_file_handler.h>
#include <ACS_APGCC_CommonLib.h>
#include <ace/OS.h>
#include <map>
#include <acs_chb_common.h>

using namespace std;

static ACS_TRA_trace ACS_CHB_MTZ_FH_DebugTrace("ACS_CHB_MTZ_FH_DebugTrace", "C512");
static ACS_TRA_trace ACS_CHB_MTZ_FH_ErrorTrace("ACS_CHB_MTZ_FH_ErrorTrace", "C512");
ACE_Mutex ACS_CHB_filehandler::theReadWriteMutex;

char ACS_CHB_Common::dnOfTzLinkRootObj[512] = {0};

const char* comma = ",";
/*===================================================================
   ROUTINE: ACS_CHB_filehandler
=================================================================== */
ACS_CHB_filehandler::ACS_CHB_filehandler():theRecursiveMutex()
{
	int filepathLength = PATHLENGTH;
        ACS_APGCC_CommonLib oComLib;
        oComLib.GetDataDiskPath("ACS_DATA", datadiskPath, filepathLength);
        ACS_CHB_Common::fetchDnOfTzLinkRootObjFromIMM();

} // End of constructor

/*===================================================================
   ROUTINE: ~ACS_CHB_filehandler
=================================================================== */
ACS_CHB_filehandler::~ACS_CHB_filehandler()
{
    //Destroy the mutex.
	theRecursiveMutex.remove();
} // End of destructor

/*===================================================================
   ROUTINE: ReadTZsFromIMM
=================================================================== */
bool ACS_CHB_filehandler::ReadTZsFromIMM(OmHandler& omHandler, FileMapType* pOneMap)
{	
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_filehandler::ReadTZsFromIMM");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	//Fetch from IMM using OM handler
	ACS_CC_ReturnType enResult;
	FileMapType LocalMap;
	ACE_INT32 tzId = -1;
	ACE_TCHAR tzName[NTZoneSize];
	ACE_OS::memset(tzName, 0, sizeof(tzName));

	for( ACE_INT32 itr = 0 ; itr < MAX_NO_TIME_ZONES ; itr++)
	{
		ACE_TCHAR tzParameter[MAX_DN_SIZE], strIndex[MAX_TZ_VALUE_SIZE];
		ACE_OS::memset(tzParameter,0, sizeof(tzParameter));
		ACE_OS::memset(strIndex,0,sizeof(strIndex));
		ACS_CC_ImmParameter paramToFind;

		//Make distinguish names for objects
		ACE_OS::sprintf(strIndex,"%d", itr);
		ACE_OS::strcpy(tzParameter,TZ_DN_BASE);
		ACE_OS::strcat(tzParameter,strIndex);
		ACE_OS::strcat(tzParameter,comma);
		ACE_OS::strcat(tzParameter,ACS_CHB_Common::dnOfTzLinkRootObj);

		//Set paramToFind
		paramToFind.attrName = (char*)TZ_ID;

		if( ( enResult = omHandler.getAttribute( tzParameter, &paramToFind ) ) == ACS_CC_SUCCESS )
		{
			tzId = *((ACE_UINT32*)(*(paramToFind.attrValues)));
		}
		else
		{
			//TBD error handling.
			continue;
		}
		paramToFind.attrName = (char*)TZ_STRING;

		if( ( enResult = omHandler.getAttribute( tzParameter, &paramToFind ) ) == ACS_CC_SUCCESS )
		{
			ACE_OS::strcpy(tzName, (reinterpret_cast<char*>(*(paramToFind.attrValues))));
		}
		else
		{
			if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "omHandler.getAttribute() Failed.");
				ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}
			if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::ReadTZsFromIMM");
				ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
			}
			return false;
		}

		TIME_ZONE_INFORMATION Tzi;
		if( CheckTimeZone(tzName, &Tzi) )
		{	
			// Put the information in the map.
			LocalMap.insert(FilePairType( tzId, 
					ACS_CHB_NtZone( tzName, &Tzi )) );
		}
	}
	FileMapType::iterator p;

	if( !LocalMap.empty() )
	{
		for( p = LocalMap.begin(); p != LocalMap.end(); ++p )
		{
			if( pOneMap != NULL )
			{
				pOneMap->insert(FilePairType(p->first, p->second)) ;
			}
		}

	}
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::ReadTZsFromIMM");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	return true;
} // End of ReadTZsFromIMM

/*===================================================================
   ROUTINE: ReadZoneFile
=================================================================== */
bool ACS_CHB_filehandler::ReadZoneFile(FileMapType* pOneMap)
{
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_filehandler::ReadZoneFile");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	//Acquiring lock for reading ACS_TZ_links.cfg
	theReadWriteMutex.acquire();
	char FilePath[PATHLENGTH];
	ACE_OS::memset(FilePath,0,sizeof(FilePath));

	//Now construct the path to .cfg file
	//Use the APGCC method GetDataDiskPath to get the path to data disk.
	strcpy(FilePath, datadiskPath );
	
	ACE_OS::strcat(FilePath, "/");
	ACE_OS::strcat(FilePath, MTZDIR);
	ACE_OS::strcat(FilePath, FILENAME);

	//Now acquire the lock on file.
	ACE_HANDLE handle = ACE_INVALID_HANDLE;
	ACE_File_Lock ACS_TZ_links_Fl(handle, 0);
	if(ACS_TZ_links_Fl.open(FilePath, O_RDWR) != 0 )
	{
		theReadWriteMutex.release();
		if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
                {
                        char traceBuffer[512];
                        memset(&traceBuffer, 0, sizeof(traceBuffer));
                        sprintf(traceBuffer, "%s%d", "ACS_CHB_filehandler::ReadZoneFile, Failed to open ACE_File_Lock,errno : ",errno);
                        ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
                }
		if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
		{
                char traceBuffer[512];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::ReadZoneFile");
                ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
	return false;
	}
	if(ACS_TZ_links_Fl.acquire() == -1)
	{
		theReadWriteMutex.release();
		if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
                {
                        char traceBuffer[512];
                        memset(&traceBuffer, 0, sizeof(traceBuffer));
                        sprintf(traceBuffer, "%s%d", "ACS_CHB_filehandler::ReadZoneFile, Failed to acquire ACE_File_Lock, errno : ",errno);
                        ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
                }
		if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
		{
                char traceBuffer[512];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::ReadZoneFile");
                ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
                return false;
	}

	FileMapType LocalMap;
	ACE_Configuration_Heap cf;
	if(cf.open ()!= 0)
	{
		ACS_TZ_links_Fl.remove(0);
		theReadWriteMutex.release();
		if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s%d", "ACS_CHB_filehandler::ReadZoneFile, Failed to open ACE_Configuration_Heap, errno : ",errno);
			ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
		if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::ReadZoneFile");
			ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
		return false;
	}

	ACE_Ini_ImpExp importCf (cf);

	importCf.import_config (FilePath);
	int index = 0;
	ACE_TString sect_name;

	const ACE_Configuration_Section_Key &root = cf.root_section ();
	for (index = 0; (cf.enumerate_sections (root, index, sect_name) == 0); ++index)
	{
		if (sect_name == ACE_TEXT (TIMEZONESECTNAME))
		{
			// Check for values in this section.
			ACE_Configuration_Section_Key sect1;
			if((cf.open_section (root, sect_name.c_str (), 0, sect1)) != 0)
			{
				ACS_TZ_links_Fl.remove(0);
				theReadWriteMutex.release();
				if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s%d", "ACS_CHB_filehandler::ReadZoneFile, Failed to open_section for ACE_Configuration_Heap, errno = ",errno);
					ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}
				if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::ReadZoneFile");
					ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
				}
				return false;
			}
			int val_index = 0;
			ACE_TString val_name, value;
			ACE_Configuration::VALUETYPE val_type;

			while (( cf.enumerate_values (sect1, val_index, val_name, val_type)) == 0)
			{
				 cf.get_string_value (sect1, val_name.c_str (), value);
				TIME_ZONE_INFORMATION Tzi;
				if( CheckTimeZone(value.c_str(), &Tzi))
				{
					LocalMap.insert(FilePairType(ACE_OS::atol(val_name.c_str()),ACS_CHB_NtZone(value.c_str(), &Tzi)));
				}
				++val_index;
			}
		}
		
	}

	FileMapType::iterator p;
	if( !LocalMap.empty() )
	{
		// Signal the Critical section.
		theRecursiveMutex.acquire();
		// Clean FileMap.
		FileMap.erase( FileMap.begin(), FileMap.end() );
		for( p = LocalMap.begin(); p != LocalMap.end(); ++p )
		{
			FileMap.insert(FilePairType(p->first, p->second)) ;
			if( pOneMap != NULL )
			{
				pOneMap->insert(FilePairType(p->first, p->second)) ;
			}
		}
		// Signal the Critical section.
		theRecursiveMutex.release();
	}
	else
	{
		if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	        {
        	        char traceBuffer[512];
                	memset(&traceBuffer, 0, sizeof(traceBuffer));
                	sprintf(traceBuffer, "%s", "Local map is found empty, erasing the FileMap...");
                	ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
        	}
		// Signal the Critical section.
		theRecursiveMutex.acquire();
		// Clean FileMap.
		FileMap.erase( FileMap.begin(), FileMap.end() );
		// Signal the Critical section.
		theRecursiveMutex.release();
	}

	ACS_TZ_links_Fl.remove(0);
	theReadWriteMutex.release();
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::ReadZoneFile");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	return true;
}

/*===================================================================
   ROUTINE: ReadZoneFileCPtime
=================================================================== */
bool ACS_CHB_filehandler::ReadZoneFileCPtime(time_t &CPtimeSec, 
		time_t &written)
{
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_filehandler::ReadZoneFileCPtime");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	//Acquire lock on the read operation on ACS_TZ_links.cfg
	theReadWriteMutex.acquire();
	char FilePath[PATHLENGTH];
	ACE_OS::memset(FilePath,0,sizeof(FilePath));

	strcpy(FilePath, datadiskPath );
	//Now construct the path to .cfg file
	ACE_OS::strcat(FilePath, "/");
	ACE_OS::strcat(FilePath, MTZDIR);
	ACE_OS::strcat(FilePath, FILENAME);

	//Acquire the lock on file.
	ACE_HANDLE handle = ACE_INVALID_HANDLE;
	ACE_File_Lock ACS_TZ_links_Fl(handle, 0);
	ACS_TZ_links_Fl.open(FilePath, O_RDWR);
	ACS_TZ_links_Fl.acquire();
	CPtimeSec = 0;
	written = 0;
	//Now read the ACS_CPTime section.
	ACE_Configuration_Heap cf;
	cf.open ();

	ACE_Ini_ImpExp importCf (cf);

	importCf.import_config (FilePath);

	int index = -1;
	ACE_TString sect_name, strCPtime, strUpdated;

	const ACE_Configuration_Section_Key &root = cf.root_section ();
	for (index = 0; (cf.enumerate_sections (root, index, sect_name) == 0); ++index)
	{

		if (sect_name == ACE_TEXT (CPTIMESECTNAME))
		{

			// Check for values in this section.
			ACE_Configuration_Section_Key sect1;
			cf.open_section (root, sect_name.c_str (), 0, sect1);
			int val_index = 0;
			ACE_TString val_name, value;
			ACE_Configuration::VALUETYPE val_type;

			while ((cf.enumerate_values (sect1, val_index, val_name, val_type)) == 0)
			{
				cf.get_string_value (sect1, val_name.c_str (), value);

				if( val_name == CPTIMEKEYNAME)
				{
					strCPtime = value;

				}
				else if( val_name == UPDATEDKEYNAME)
				{
					strUpdated = value;

				}

				++val_index;
			}
		}
	}

	CPtimeSec = ACE_OS::atol(strCPtime.c_str());


	if( CPtimeSec != 0)
	{
		written = ACE_OS::atol(strUpdated.c_str());

		if( written == 0)
		{
			ACS_TZ_links_Fl.remove(0);
			theReadWriteMutex.release();
			if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::ReadZoneFileCPtime:written=0");
				ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
			}
			return false;
		}
	}
	else
	{
		ACS_TZ_links_Fl.remove(0);
		theReadWriteMutex.release();
		if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::ReadZoneFileCPtime:CPtimeSec=0");
			ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return false;
	}

	ACS_TZ_links_Fl.remove(0);
	theReadWriteMutex.release();
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::ReadZoneFileCPtime");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	return true;


} // end ReadTZsFromIMMCPtime

/*===================================================================
   ROUTINE: WriteZoneFile
=================================================================== */
bool ACS_CHB_filehandler::WriteZoneFile(FileMapType* pOneMap)
{
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_filehandler::WriteZoneFile");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	theReadWriteMutex.acquire();
	ACE_TString sect_name, strCPtime(ACE_TEXT("0")), strUpdated(ACE_TEXT("0"));
	char FilePath[PATHLENGTH];
	ACE_OS::memset(FilePath,0,sizeof(FilePath));

	bool islocked = false;
	//Now construct the path to .cfg file
	 //Use the APGCC method GetDataDiskPath to get the path to data disk.
	strcpy( FilePath, datadiskPath );


	ACE_stat statBuff;
	if( ACE_OS::stat(FilePath, &statBuff)  != 0 )
	{
	        // data disk is not mounted. Cannot create MTZ file.	
		if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving WriteZoneFile: data disk not present.");
			ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		theReadWriteMutex.release();
		return false;
	}

	ACE_OS::strcat(FilePath, "/");
	ACE_OS::strcat(FilePath, MTZDIR);

	if( ACE_OS::stat(FilePath, &statBuff)  != 0 )
        {
                // data disk is not mounted. Cannot create MTZ file.
                if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
                {
                        char traceBuffer[512];
                        memset(&traceBuffer, 0, sizeof(traceBuffer));
                        sprintf(traceBuffer, "%s", "MTZ directory not present, creating it now.");
                        ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
                }
		int status = ACE_OS::mkdir(ACE_TEXT(CHB_MTZ_DIRPATH), S_IRWXU | S_IRWXG );	
		if( status != 0 )
		{
			theReadWriteMutex.release();
			if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving WriteZoneFile: Unable to create MTZ directory.");
				ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
			}
			return false;
		}
        }


	ACE_OS::strcat(FilePath, FILENAME);
	FileMapType::iterator p;

	//Create a File Lock
	ACE_HANDLE handle = ACE_INVALID_HANDLE;
	ACE_File_Lock ACS_TZ_links_Fl(handle, 0);

	if( ACE_OS::stat(FilePath, &statBuff) == 0 ) //if the file does exist
	{
		//Acquire the lock.
			ACS_TZ_links_Fl.open(FilePath, O_RDWR);
			ACS_TZ_links_Fl.acquire();

			islocked = true;
			//The changes are performed by IMM.
			//Add the zones in the ACS_TZ_links.cfg file.
			//Create a map.
			//Now read the ACS_CPTime section.
			ACE_Configuration_Heap cf;
			cf.open ();

			ACE_Ini_ImpExp importCf (cf);

			importCf.import_config (FilePath);

			int index;

			const ACE_Configuration_Section_Key &root = cf.root_section ();
			for (index = 0; (cf.enumerate_sections (root, index, sect_name) == 0); ++index)
			{
				if (sect_name == ACE_TEXT (CPTIMESECTNAME))
				{
					// Check for values in this section.
					ACE_Configuration_Section_Key sect1;
					cf.open_section (root, sect_name.c_str (), 0, sect1);
					int val_index = 0;
					ACE_TString val_name, value;
					ACE_Configuration::VALUETYPE val_type;

					while ((cf.enumerate_values (sect1, val_index, val_name, val_type)) == 0)
					{
						 cf.get_string_value (sect1, val_name.c_str (), value);

						if( val_name == ACE_TEXT(CPTIMEKEYNAME))
						{
							strCPtime = value;
						}
						else if( val_name == ACE_TEXT(UPDATEDKEYNAME))
						{
							strUpdated = value;
						}

						++val_index;
					}
				}
			}
		}
		//Now write the new contents in the file.

		ACE_Configuration_Heap  cf1;
		cf1.open ();

		// add a section and value to heap1
		ACE_Configuration_Section_Key root1 = cf1.root_section ();


		ACE_Configuration_Section_Key CPTimeSection;
		cf1.open_section (root1, ACE_TEXT (CPTIMESECTNAME), 1, CPTimeSection);

		//Add CP Time.
		cf1.set_string_value(CPTimeSection, ACE_TEXT(UPDATEDKEYNAME), strUpdated);
		cf1.set_string_value(CPTimeSection, ACE_TEXT(CPTIMEKEYNAME),strCPtime );

		ACE_Configuration_Section_Key timeZoneSection;
		cf1.open_section (root1, ACE_TEXT (TIMEZONESECTNAME), 1, timeZoneSection);

		p = pOneMap->begin();
		while( p != pOneMap->end())
		{
			ACE_TCHAR tz_value[MAX_TZ_VALUE_SIZE];
			ACE_OS::sprintf(tz_value, "%d",p->first);
			cf1.set_string_value (timeZoneSection, tz_value, ACE_TString((p->second).get()));
			++p;
		}


		//Now export the configuration.
		ACE_Ini_ImpExp exportCf1 (cf1);
		exportCf1.export_config (FilePath);
		
		if( islocked)
		{
			ACS_TZ_links_Fl.remove(0);
		}
		theReadWriteMutex.release();
		if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::WriteZoneFile");
			ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
		return true;
	}

	/*===================================================================
	   ROUTINE: WriteZoneFileCPtime
	=================================================================== */
	bool ACS_CHB_filehandler::WriteZoneFileCPtime(time_t &CPtimeSec, 
			time_t &writtenSec)
	{
		if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Entering ACS_CHB_filehandler::WriteZoneFileCPtime");
			ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}

		theReadWriteMutex.acquire();
		ACE_TCHAR FilePath[PATHLENGTH];
		ACE_OS::memset(FilePath,0,sizeof(FilePath));

		//Now construct the path to .cfg file
		 //Use the APGCC method GetDataDiskPath to get the path to data disk.
		strcpy(FilePath, datadiskPath );

		ACE_stat statBuff;
		if(  ACE_OS::stat(FilePath, &statBuff) != 0 )
		{
			//MTZ file cannot be accessed.
			theReadWriteMutex.release();
			if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving WriteZoneFileCPtime: data disk cannot be accessed.");
				ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
			}
			return false;
		}

		ACE_OS::strcat(FilePath, "/");
		ACE_OS::strcat(FilePath, MTZDIR);

		if( ACE_OS::stat(FilePath, &statBuff)  != 0 )
		{
			// data disk is not mounted. Cannot create MTZ file.
			if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "MTZ directory not present, creating it now.");
				ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}

			int status = ACE_OS::mkdir(ACE_TEXT(CHB_MTZ_DIRPATH), S_IRWXU | S_IRWXG );
			if( status != 0 )
			{
				theReadWriteMutex.release();
				if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving WriteZoneFileCPtime: Unable to create MTZ directory.");
					ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
				}
				return false;
			}
		}


		ACE_OS::strcat(FilePath, FILENAME);

		//Create a File Lock
		ACE_HANDLE handle = ACE_INVALID_HANDLE;
		ACE_File_Lock ACS_TZ_links_Fl(handle, 0);
		bool isLocked = false;

		map<ACE_TString, ACE_TString> zoneMap;
	ACE_TCHAR strCPtime[PATHLENGTH], strUpdated[PATHLENGTH];

	if( ACE_OS::stat(FilePath, &statBuff) == 0 ) //if the file does exist
	{
		ACS_TZ_links_Fl.open(FilePath, O_RDWR);
		ACS_TZ_links_Fl.acquire();

		isLocked = true;
		//Now read the ACS_TZ_links section.
		ACE_Configuration_Heap cf;
		cf.open ();

		ACE_Ini_ImpExp importCf (cf);

		importCf.import_config (FilePath);

		int index = -1;
		ACE_TString sect_name;

		const ACE_Configuration_Section_Key &root = cf.root_section ();
		for (index = 0; (cf.enumerate_sections (root, index, sect_name) == 0); ++index)
		{
			if (sect_name == ACE_TEXT (TIMEZONESECTNAME))
			{
				// Check for values in this section.
				ACE_Configuration_Section_Key sect1;
				cf.open_section (root, sect_name.c_str (), 0, sect1);
				int val_index = 0;
				ACE_TString val_name, value;
				ACE_Configuration::VALUETYPE val_type;

				while (( cf.enumerate_values (sect1, val_index, val_name, val_type)) == 0)
				{
					cf.get_string_value (sect1, val_name.c_str (), value);
					zoneMap.insert(pair<ACE_TString, ACE_TString>(val_name,value));
					++val_index;
				}
			}
		}
	}
	//Now write the new contents in the file.

	ACE_Configuration_Heap  cf1;
	cf1.open ();

	// add a section and value to heap1
	ACE_Configuration_Section_Key root1 = cf1.root_section ();

	ACE_Configuration_Section_Key CPTimeSection;
	cf1.open_section (root1, ACE_TEXT (CPTIMESECTNAME), 1, CPTimeSection);

	ACE_OS::sprintf(strUpdated,"%ld", writtenSec);
	ACE_OS::sprintf(strCPtime,"%ld", CPtimeSec);

	//Add CP Time.
	cf1.set_string_value(CPTimeSection, ACE_TEXT(UPDATEDKEYNAME), ACE_TString(strUpdated));
	cf1.set_string_value(CPTimeSection, ACE_TEXT(CPTIMEKEYNAME),ACE_TString(strCPtime ));

	ACE_Configuration_Section_Key timeZoneSection;
	cf1.open_section (root1, ACE_TEXT (TIMEZONESECTNAME), 1, timeZoneSection);

	map<ACE_TString, ACE_TString>::iterator p;

	p = zoneMap.begin();
	while( p != zoneMap.end())
	{
		cf1.set_string_value (timeZoneSection, (p->first).c_str(), ACE_TString(p->second));
		++p;
	}

	//Now export the configuration.
	ACE_Ini_ImpExp exportCf1 (cf1);
	exportCf1.export_config (FilePath);

	if(isLocked)
	{
		ACS_TZ_links_Fl.remove(0);
	}
	theReadWriteMutex.release();
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::WriteZoneFileCPtime");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	return true;

} // end WriteZoneFileCPtime

/*===================================================================
   ROUTINE: GetNTZone
=================================================================== */
bool ACS_CHB_filehandler::GetNTZone( const cpTime lookUpTime, 
		TIME_ZONE_INFORMATION &NTTime)
{
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_filehandler::GetNTZone");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}

        FileMapType::iterator p;

	// Signal the Critical section.
	theRecursiveMutex.acquire();

	p = FileMap.find(lookUpTime);

	// Test the result.
	bool test = ( p!= FileMap.end() );
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "test: %d", test);
                ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }

	// Leave the critical section.
	// Inside the if/else branch below.
	if( test )
	{	
		if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	        {
                	char traceBuffer[512];
	                memset(&traceBuffer, 0, sizeof(traceBuffer));
        	        sprintf(traceBuffer, "p->first: %d", p->first);
                	ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
        	}

		TIME_ZONE_INFORMATION *pTZI = (p->second).getTZI();
		NTTime = *pTZI;
		theRecursiveMutex.release();
		if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::GetNTZone");
			ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
		return true;
	}
	else
	{
		theRecursiveMutex.release();
		// Fill it with 0.
		ACE_OS::memset(&NTTime, 0, sizeof(TIME_ZONE_INFORMATION) );
		if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving GetNTZone:TZ not present.");
			ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return false;
	}
} // End of GetNTZone


/*===================================================================
   ROUTINE: CheckTimeZone
=================================================================== */
bool ACS_CHB_filehandler::CheckTimeZone(const char* TimeZone, TIME_ZONE_INFORMATION *Tzi)
{
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_filehandler::CheckTimeZone");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	ACE_TCHAR* timeZoneString = 0;
	timeZoneString =  new ACE_TCHAR[ACE_OS::strlen(TimeZone) + 1];
	ACE_OS::memset(timeZoneString, 0, strlen(TimeZone) + 1);
	unsigned int i=0, j=0;

	string tzName(TimeZone);
	int noOfSpaces = ACS_CHB_Common::countSpacesInString(tzName);
	// If number of spaces in string is more than 1
	if(noOfSpaces == 0 || noOfSpaces > 1)
	{
		delete[] timeZoneString;
		timeZoneString = 0;
		if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving CheckTimeZone: More than one space is present.");
			ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
		return false;
	}
	// Check for next character after ')' should be space
	int pos = tzName.find(")");
	if(tzName.at(pos+1) != 0x20)
	{
		delete[] timeZoneString;
		timeZoneString = 0;
		if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving CheckTimeZone: More than one space is present.");
			ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
		return false;
	}

	//Remove spaces, if any.
	for( i=0, j=0; i < ACE_OS::strlen(TimeZone) ; i++)
	{
		if( TimeZone[i] != ' ')
		{
			timeZoneString[j] = TimeZone[i];
			j++;
		}
	}
	timeZoneString[j] = '\0';

	ACE_OS::memset(Tzi,0,sizeof(*Tzi));
	if( validateTimeZone(timeZoneString) == false)
	{
		delete[] timeZoneString;
		timeZoneString = 0;
		if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving CheckTimeZone: validateTimeZone() Failed");
			ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
		return false;
	}

	//Get the Tzi data from /usr/share/zoneinfo.
	if( getTzi(timeZoneString, Tzi) == false )
	{
		delete[] timeZoneString;
		timeZoneString = 0;
		if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving CheckTimeZone:get Tzi failed.");
			ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return false;
	}

	delete[] timeZoneString;
	timeZoneString = 0;
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::CheckTimeZone");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	return true;
} // End of CheckTimeZone

#if 0
// COMMENTED TO RESOLVE CPPCHECK ERRORS
/*===================================================================
   ROUTINE: RemoveTrailingBlank
=================================================================== */
bool ACS_CHB_filehandler::RemoveTrailingBlank(char *String)
{
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_filehandler::RemoveTrailingBlank");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	// String contains:
	// Before:
	// asdf asdf asd894 asdf  \0
	// After :
	// asdf asdf asd894 asdf\0

	char* pPek = String;

	// Find end of string.
	while (*pPek != '\0') ++pPek;
	// Do not point at \0
	--pPek;
	// Find first none blank counting backwards.
	// Or the string is just blanks.
	while (*pPek == ' ' && pPek != String) --pPek;

	if(pPek != String) {
		*(pPek+1) = '\0';
	}
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::RemoveTrailingBlank");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	return true;
}
#endif
/*===================================================================
   ROUTINE: RemoveTZFromIMM
=================================================================== */
bool ACS_CHB_filehandler::RemoveTZFromIMM(OmHandler &omHandler, ACE_INT32 TMZ_value)
{
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_filehandler::RemoveTZFromIMM");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	ACE_TCHAR tzParameter[MAX_DN_SIZE], strIndex[MAX_TZ_VALUE_SIZE];

	//Create the distinguished name.
	ACE_OS::sprintf(strIndex,"%d", TMZ_value);
	ACE_OS::strcpy(tzParameter,TZ_DN_BASE);
	ACE_OS::strcat(tzParameter,strIndex);
	ACE_OS::strcat(tzParameter,comma);
	ACE_OS::strcat(tzParameter,ACS_CHB_Common::dnOfTzLinkRootObj);
	if(omHandler.deleteObject(tzParameter) == ACS_CC_FAILURE)
	{
		if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving RemoveTZFromIMM: deleteObject Failed.");
			ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return false;
	}
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::RemoveTZFromIMM");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	return true;
}

/*===================================================================
   ROUTINE: AddTZToIMM
=================================================================== */
bool ACS_CHB_filehandler::AddTZToIMM(OmHandler &omHandler, ACE_INT32 TMZ_value, ACE_TCHAR* TZ_name)
{
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_filehandler::AddTZToIMM");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	vector<ACS_CC_ValuesDefinitionType> AttrList;
	ACS_CC_ValuesDefinitionType *attributes;

	ACE_INT32  numAttr = 3;

	attributes = new ACS_CC_ValuesDefinitionType[numAttr];

	ACE_TCHAR tzParameter[MAX_DN_SIZE],strIndex[MAX_TZ_VALUE_SIZE];

	//Make distinguish names for objects
	ACE_OS::sprintf(strIndex, "%d", TMZ_value);
	ACE_OS::strcpy(tzParameter,TZ_DN_BASE);
	ACE_OS::strcat(tzParameter,strIndex);

	attributes[0].attrName = new char [ACE_OS::strlen(TZ_CL_ATTRNAME) + 1];
	ACE_OS::strcpy( attributes[0].attrName ,TZ_CL_ATTRNAME);
	attributes[0].attrType = ATTR_STRINGT;
	attributes[0].attrValuesNum = 1;
	attributes[0].attrValues = new void*[1];
	attributes[0].attrValues[0] = reinterpret_cast<void*>(tzParameter);
	AttrList.push_back(attributes[0]);

	attributes[1].attrName = new char [ACE_OS::strlen(TZ_ID) + 1];
	ACE_OS::strcpy( attributes[1].attrName ,TZ_ID);
	attributes[1].attrType = ATTR_INT32T;
	attributes[1].attrValuesNum = 1;
	attributes[1].attrValues = new void*[1];
	attributes[1].attrValues[0] = reinterpret_cast<void*>(&(TMZ_value));
	AttrList.push_back(attributes[1]);


	attributes[2].attrName = new char [ACE_OS::strlen(TZ_STRING) + 1];
	ACE_OS::strcpy( attributes[2].attrName ,TZ_STRING);
	attributes[2].attrType = ATTR_STRINGT;
	attributes[2].attrValuesNum = 1;
	attributes[2].attrValues = new void*[1];
	attributes[2].attrValues[0] = reinterpret_cast<void*>((char*)TZ_name);
	AttrList.push_back(attributes[2]);
	if (omHandler.createObject(TZ_CL_NAME, ACS_CHB_Common::dnOfTzLinkRootObj , AttrList ) == ACS_CC_FAILURE)
	{
		//Free the memory allocated for attribute names.
		delete[] attributes;
		attributes = 0;
		if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving AddTZToIMM: createObject Failed.");
			ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return false;
	}
	else
	{
		delete[] attributes;
		attributes = 0;
	}
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::AddTZToIMM");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	return true;
}

/*===================================================================
   ROUTINE: UpdateTZIMM
=================================================================== */
bool ACS_CHB_filehandler::UpdateTZIMM(OmHandler& omHandler, ACE_INT32 TMZ_value, ACE_TCHAR* TZ_name)
{
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_filehandler::UpdateTZIMM");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	ACE_TCHAR tzParameter[MAX_DN_SIZE],strIndex[MAX_TZ_VALUE_SIZE];

	//Make distinguish names for objects

	ACE_OS::sprintf(strIndex, "%d", TMZ_value);
	ACE_OS::strcpy(tzParameter,TZ_DN_BASE);
	ACE_OS::strcat(tzParameter,strIndex);
	ACE_OS::strcat(tzParameter,comma);
	ACE_OS::strcat(tzParameter,ACS_CHB_Common::dnOfTzLinkRootObj);

	//Fetch from IMM using OM handler
	ACS_CC_ImmParameter paramToChange;

	ACS_CC_ReturnType enResult = ACS_CC_FAILURE;

	paramToChange.attrName = (char*)(TZ_STRING);
	paramToChange.attrType = ATTR_STRINGT;
	paramToChange.attrValuesNum = 1;
	paramToChange.attrValues = new void*[paramToChange.attrValuesNum];
	paramToChange.attrValues[0] = reinterpret_cast<void*>(TZ_name);
	if( (enResult = omHandler.modifyAttribute( tzParameter , &paramToChange)) != ACS_CC_SUCCESS )
	{
		if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving UpdateTZIMM: modifyAttribute failed.");
			ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return false;
	}
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::UpdateTZIMM");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	return true;
}

/*===================================================================
   ROUTINE: getTzi
=================================================================== */
bool ACS_CHB_filehandler::getTzi(const ACE_TCHAR* TZOrigString, TIME_ZONE_INFORMATION *tzi )
{

	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
        {
                char traceBuffer[512];
                memset(&traceBuffer, 0, sizeof(traceBuffer));
                sprintf(traceBuffer, "%s %s", "Entering ACS_CHB_filehandler::getTzi", TZOrigString);
                ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
        }
	//TR_HT72009 Changes Begin
	string commandOutput;
	time_t tempTime = time(NULL);
	struct tm structTime,dstStopTime;
	int Year=0,nxtYear=0,count=0,rowIndex=0,i=0,j=2;
	int data[4][8]={{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}};
	char tzCmd[1024],buffer[512];
	string tzName(TZOrigString), BiasName[4]={"\0","\0","\0","\0"};
	time_t localTimeSec(NULL), dstStopSec(NULL);
	structTime= *localtime(&tempTime);

	Year = structTime.tm_year + 1900;
	nxtYear = Year+1;
	const char* timeZoneString = (tzName.substr(11)).c_str();

	memset( tzCmd, 0, sizeof(tzCmd));
	sprintf(tzCmd,"zdump -v %s -c %d,%d | awk '(NR%%2) {print $10,$11,$12,$13,$14,$15,$16}' | grep :",timeZoneString,Year,nxtYear);

	FILE *input;
	input = popen(tzCmd,"r");

	if( input == NULL )
	{
		if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "TimeZone command failed, Leaving ACS_CHB_filehandler::getTzi");
			ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
		//pclose(fp);
		return false;
	}
	else
	{
		char *tmp;
		while(fgets(buffer, sizeof(buffer), input)!=0)
		{
			tmp  = 0;
			count = 0;
			commandOutput.append(buffer);
			tmp = strtok(buffer, " =:");
			while( count < 11 )
			{
				if( tmp != 0 )
					switch(count)
					{
						case 0:
							data[rowIndex][2]=getMonth(tmp);
							break;
						case 1:
							data[rowIndex][1]=atoi(tmp);
							break;
						case 2:
							data[rowIndex][4]=atoi(tmp);
							break;
						case 3:
							data[rowIndex][5]=atoi(tmp);
							break;
						case 4:
							data[rowIndex][6]=atoi(tmp);
							break;
						case 5:
							data[rowIndex][3]=atoi(tmp);
							break;
						case 6:
							BiasName[rowIndex]=tmp;
							break;
						case 7:
							break;
						case 8:
							data[rowIndex][0]=atoi(tmp);
							break;
						case 9:
							break;
						case 10:
							data[rowIndex][7]=atoi(tmp);
							break;
					}
				tmp = strtok(0, " =:");
				count++;
			}
			rowIndex++;
		}
		pclose(input);
	}

	if(data[2][3]!=0)
	{
		dstStopTime.tm_mday = data[1][1];
		dstStopTime.tm_mon = data[1][2];
		dstStopTime.tm_year = data[1][3];
		dstStopTime.tm_hour = data[1][4];
		dstStopTime.tm_min = data[1][5];
		dstStopTime.tm_sec = data[1][6];

		structTime.tm_year =  structTime.tm_year+1990;
		localTimeSec = CalculateSec(structTime);
		dstStopSec = CalculateSec(dstStopTime);

		if(localTimeSec > dstStopSec)
		{
			i=2;
			j=4;
		}
	}

	for(;i<j;i++)
	{
		if(data[i][0]==0)
		{
			if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
		        {
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "DST START Date/Time ");
					ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		        }
			strcpy(tzi->DaylightName ,BiasName[i].c_str());
			tzi->DaylightDate.wDay = data[i][1];
			tzi->DaylightDate.wMonth = data[i][2];
			tzi->DaylightDate.wYear = data[i][3];
			tzi->DaylightDate.wHour = data[i][4];
			tzi->DaylightDate.wMinute = data[i][5];
			tzi->DaylightDate.wSecond = data[i][6];
                        if (data[1][7] > data[0][7])
                        tzi->DaylightBias = (data[0][7]-data[1][7])/60;
                        else
                        tzi->DaylightBias = (data[1][7]-data[0][7])/60;
			tzi->Bias = -(data[i][7]/60);
		}
		else
		{
			if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
		        {
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "SDT START Date/Time ");
					ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		        }
			strcpy(tzi->StandardName , BiasName[i].c_str());
			tzi->StandardDate.wDay = data[i][1];
			tzi->StandardDate.wMonth = data[i][2];
			tzi->StandardDate.wYear = data[i][3];
			tzi->StandardDate.wHour = data[i][4];
			tzi->StandardDate.wMinute = data[i][5];
			tzi->StandardDate.wSecond = data[i][6];
			tzi->StandardBias = 0;
		}
		if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, " %d : %d : %d : %d : %d : %d : %d " ,data[i][1],data[i][2],data[i][3],data[i][4],data[i][5],data[i][6],data[i][7]);
			ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
	}
	//TR_HV63516 changes Start
	if(commandOutput.empty())
	{
		tzi->Bias = timeZone_offset(TZOrigString);
		if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "Time Zone Bias is : %d" ,tzi->Bias);
			ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
	}
	//TR_HV63516 changes End
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::getTzi");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	return true;
	//TR_HT72009 Changes End
}

/*===================================================================
   ROUTINE: ValidateTMZNumber
=================================================================== */
bool ACS_CHB_filehandler::validateTimeZone(ACE_TCHAR* TimeZone)
{
	//Find whether that particular time zone exists in Linux system or not.
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_filehandler::validateTimeZone");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	ACE_TCHAR* zoneFileName = new ACE_TCHAR[ACE_OS::strlen(TimeZone) + ACE_OS::strlen(TZ_LOCATION_LINUX) + 1];
	ACE_OS::memset(zoneFileName, 0,(ACE_OS::strlen(TimeZone) + ACE_OS::strlen(TZ_LOCATION_LINUX) + 1));
	ACE_OS::strcpy(zoneFileName, TZ_LOCATION_LINUX);
	ACE_TCHAR* pZoneName = 0;
	if( (pZoneName = ACE_OS::strchr(TimeZone,')')) != 0 )
	{
		ACE_OS::strcat(zoneFileName, pZoneName+1 );
		ACE_INT32 fd = ACE_OS::open(zoneFileName, O_RDONLY);
		if( fd == -1)
		{
			//The time zone does not exist.
			delete[] zoneFileName;
			zoneFileName = 0;
			if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::validateTimeZone");
				ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
			}
			return false;
		}
		close(fd);
		delete[] zoneFileName;
		zoneFileName = 0;
		if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::validateTimeZone");
			ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
		return true;
	}
	else
	{
		delete[] zoneFileName;
		zoneFileName = 0;
		if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::validateTimeZone");
			ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return false;
	}


}

/*===================================================================
   ROUTINE: ValidateTMZNumber
=================================================================== */
bool ACS_CHB_filehandler::ValidateTMZNumber( char* strTMZValue)
{
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_filehandler::ValidateTMZNumber");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	if( strTMZValue == 0)
	{
		if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "Leaving ValidateTMZNumber: TMZValue is NULL");
			ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
		}
		return false;
	}
	else
	{
		for( unsigned int ctr = 0 ; ctr < ACE_OS::strlen(strTMZValue); ctr++)
		{
			if(!isdigit(strTMZValue[ctr]))
			{
				if( ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_ON())
				{
					char traceBuffer[512];
					memset(&traceBuffer, 0, sizeof(traceBuffer));
					sprintf(traceBuffer, "%s", "Leaving ValidateTMZNumber: TMZValue is not integer.");
					ACS_CHB_MTZ_FH_ErrorTrace.ACS_TRA_event(1, traceBuffer);
				}
				return false;
			}
		}
	}
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Leaving ACS_CHB_filehandler::ValidateTMZNumber");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	return true;
}
//TR_HT72009 Changes Begin
/*===================================================================
 *    ROUTINE: getMonth
 *    =================================================================== */
int ACS_CHB_filehandler::getMonth(string tmp)
{
	if      (tmp==("Jan"))
		return 1;
	else if (tmp==("Feb"))
		return 2;
	else if (tmp==("Mar"))
		return 3;
	else if (tmp==("Apr"))
		return 4;
	else if (tmp==("May"))
		return 5;
	else if (tmp==("Jun"))
		return 6;
	else if (tmp==("Jul"))
		return 7;
	else if (tmp==("Aug"))
		return 8;
	else if (tmp==("Sep"))
		return 9;
	else if (tmp==("Oct"))
		return 10;
	else if (tmp==("Nov"))
		return 11;
	else if (tmp==("Dec"))
		return 12;
	else
		return 0;
}
/*===================================================================
 *    ROUTINE: CalculateSec
 *    =================================================================== */
time_t ACS_CHB_filehandler::CalculateSec(struct tm st)
{
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s", "Entering ACS_CHB_filehandler::CalculateSec");
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
		int leapyear=0,year=0,days=0;
		time_t sec(NULL),newsec(NULL),leapsec(NULL);
		sec = 31536000;
		leapsec = 86400;
        struct tm timeInfo;
        ACE_OS::memset(&timeInfo,0, sizeof(timeInfo));
        if(st.tm_year !=0)
        {
        	timeInfo.tm_year       =       st.tm_year;
        	timeInfo.tm_mon        =       st.tm_mon+1;
        	timeInfo.tm_wday       =       st.tm_wday;
        	timeInfo.tm_mday       =       st.tm_mday;
        	timeInfo.tm_hour       =       st.tm_hour;
        	timeInfo.tm_min        =       st.tm_min;
        	timeInfo.tm_sec        =       st.tm_sec;
        	leapyear = ((timeInfo.tm_year-1970)/4);
        	year = timeInfo.tm_year-1970;
        	if (timeInfo.tm_year%4 &&  timeInfo.tm_mon <= 2)
                leapyear = leapyear - 1;
        	switch (timeInfo.tm_mon-1)
        	{
                case 1:
                        days = 31;
                        break;
                case 2:
                        days = 59;
                        break;
                case 3:
                        days = 90;
                        break;
                case 4:
                        days = 120;
                        break;
                case 5:
                        days = 151;
                        break;
                case 6:
                        days = 181;
                        break;
                case 7:
                        days = 212;
                        break;
                case 8:
                        days = 243;
                        break;
                case 9:
                        days = 273;
                        break;
                case 10:
                        days = 304;
                        break;
                case 11:
                        days = 334;
                        break;
        	}
        	newsec = (sec * year) + (leapsec * leapyear) + ((((((days +(timeInfo.tm_mday - 1)) * 24) + timeInfo.tm_hour) * 60) + timeInfo.tm_min) * 60) + timeInfo.tm_sec;
        }
        if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
        	{
        		char traceBuffer[512];
        		memset(&traceBuffer, 0, sizeof(traceBuffer));
        		sprintf(traceBuffer, "%s %ld", "Leaving ACS_CHB_filehandler::CalculateSec with value",newsec);
        		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
        	}
        return newsec;
}

//TR_HT72009 Changes End

//TR_HV63516 changes Start
//Calculate Timezone Offset for the timezones which does not have dst
int ACS_CHB_filehandler::timeZone_offset(const char* TZOrigString)
{
	int timezone_bias=0;
	FILE *fp = 0;
	char tzCmd[1024],buffer[512];
	memset(tzCmd, 0, sizeof(tzCmd));
	memset(buffer, 0, sizeof(buffer));

	sprintf(tzCmd, "%s \"%s\"", ACS_CHB_TIMEZONE, TZOrigString);
	if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
	{
		char traceBuffer[512];
		memset(&traceBuffer, 0, sizeof(traceBuffer));
		sprintf(traceBuffer, "%s %s", "tzcdm is ",tzCmd);
		ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
	}
	fp = popen( tzCmd ,"r");
	if( fp == 0 )
	{
		if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
		{
			char traceBuffer[512];
			memset(&traceBuffer, 0, sizeof(traceBuffer));
			sprintf(traceBuffer, "%s", "TimeZone command failed, Leaving ACS_CHB_filehandler::timeZone_offset");
			ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
		}
                //pclose(fp);
		return false;
	}
	else
	{
		if( fgets(buffer, sizeof(buffer), fp) != 0 )
		{
			char *tmp  = 0;
			int count = 0;
			tmp = strtok( buffer, ",");
			while( count < 21 )
			{
				if( tmp != 0 )
				{
					switch( count)
					{
					case 0:
						timezone_bias = atoi( tmp );
						break;
					default :
						break;
					}
				}
				tmp = strtok(0, ",");
				count++;
			}
			if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s %d", "bias is ",timezone_bias);
				ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}
		}
		else
		{
			if( ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_ON())
			{
				char traceBuffer[512];
				memset(&traceBuffer, 0, sizeof(traceBuffer));
				sprintf(traceBuffer, "%s", "Unable to read data from timezone file, Leaving ACS_CHB_filehandler::timeZone_offset");
				ACS_CHB_MTZ_FH_DebugTrace.ACS_TRA_event(1, traceBuffer);
			}
			pclose(fp);
			return false;
		}
		pclose(fp);
	}
	return timezone_bias;
}
//TR_HV63516 changes end
