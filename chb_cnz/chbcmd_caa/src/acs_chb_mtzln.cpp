/*=================================================================== */
   /**
   @file acs_chb_mtzln.cpp

   This file contains the code for the mtzln command. This command
   is used to add a timezone for supervision, remove a timezone,
   update a time zone for a particular time zone number and view the
   supervised time zones by the AP.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       25/11/2010     XTANAGG       Initial Release
   **/
/*=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <string>
#include <time.h>
#include <acs_chb_file_handler.h>
#include <acs_prc_api.h>
#include <acs_apgcc_omhandler.h>
#include <acs_chb_common.h>
using namespace std;

//Create a object of ACS_CHB_filehandler.
static ACS_CHB_filehandler MTZObj;

/**
 * @brief	This function extracts the time zone string for a particular
 * 			time zone number.
 *
 * @param 	TMZ_str
 * 			Name of Time zone.
 *
 * @param	TMZ_value
 * 			Time zone number.0-23
 *
 *
 * @return	void
 */
void extractTMZ(ACE_TCHAR* TMZ_str, ACE_INT32& TMZ_value);

/**
 * @brief	This function extracts the TIME_ZONE_INFORMATION structure for
 * 			for a time zone number.
 *
 * @param	TZ_str
 *			TZ_name entered by user.
 *
 * @param	TZ_name
 * 			Name of time zone.
 *
 *
 * @return	void
 */
void extractTZ(ACE_TCHAR* TZ_str, ACE_TCHAR* TZ_name);

/**
 * @brief	This function prints the usage of the command.
 *
 * @param	argv0
 * 			Name of the command.
 *
 * @return	void
 */
void printUsage(const ACE_TCHAR* argv0);

/**
 * @brief	This function prints the exit message.
 *
 * @param 	argv0
 * 			Name of the command.
 *
 * @param 	exitcode
 * 			Exit code of the program.
 *
 * @param	slogan
 * 			Exit message.
 */
void ExitPrint(const ACE_TCHAR* argv0, ACE_INT32 exitcode, const string slogan);

/**
 * @brief	getLastError
 *
 * @param	omhandler
 * 			Reference to OMHandlerV2
 *
 * @param 	ccbId
 * 			CCBId
 *
 * @param 	errorId
 * 			Last Error Id
 *
 * @param 	errorText
 * 			ErrorText
 *
 * @return	ACE_IN32
 */
void getLastError(OmHandler& omhandler, int& ccbId, int &errorId, string& errorText);
/**
 * @brief	Main function
 *
 * @param 	argc
 * 			Number of arguments.
 *
 * @param 	argv
 * 			Array of arguments.
 *
 * @return	ACE_IN32
 */
ACE_INT32 main( ACE_INT32 argc, ACE_TCHAR* argv[ ] ) 
{
	// The program name, typical usage are three cases :
	// ./mtzln [-f] TZ_name TMZ_value
	// ./mtzln -p
	// ./mtzln -f TMZ_value
	
	// check arguments.
	ACE_INT32 TMZ_value = -1;
	ACE_TCHAR  TZ_name[NTZoneSize];

	//Initialise the memory to zero.
	ACE_OS::memset(TZ_name,0, sizeof(TZ_name));

	// Step 1. Check of arguments.
	
	// First check if we have enough of arguments and not to many.
	if( argc < 2 || argc > 4) 
	{
		if( argc < 2 ) {
			ExitPrint(argv[0], 2, "Too few arguments\n"); 
		}else {
			ExitPrint(argv[0], 2, "Too many arguments\n"); 
		}	
	}

	//Create a PRC object.
	ACS_PRC_API prcObj;

	//Check if node state is active or not.
	int nState = prcObj.askForNodeState();

	if( nState != 1)	//Node is not active. 1-->active
	{
		ExitPrint(argv[0], 9,
		"Can not access the data disk.\nCheck that you are working on the active node.\n");
		
	}
	// Step 2 Read the time zones from IMM
	
	// A map to return data in.
	FileMapType	fMap;

	// An iterator to search the map.
	FileMapType::iterator p;
	
	//Create a OmHandler object.
	OmHandler omHandler;

	//Initialize OmHandler
	//Commented, functionality not available in OmHandler.

	if( (omHandler.Init(REGISTERED_OI)) == ACS_CC_FAILURE )
	{
		ExitPrint(argv[0], 117, "Unable to connect to server \n");

	}
	
	// Step 3. Check if inparameters are correct.
	switch ( argc ) {
		// One argument
	case 2: {	// mtzln -p
			if ( ACE_OS::strcmp( argv[1], "-p") == 0)
			{
				if( MTZObj.ReadTZsFromIMM(omHandler, &fMap ) == false)
				{
					omHandler.Finalize();
					ExitPrint(argv[0], 117, "Unable to connect to server \n");
					
				}

				// The whole map starts at.
				p = fMap.begin();
				
				cout << "TMZ : TZ                            " << endl;
				cout << "------------------------------------" << endl;
				while (p!=fMap.end())
				{
					cout << " " << p->first << " = " << (p->second).get() << endl;
					++p;
				}
				omHandler.Finalize();
				ExitPrint("", 0, "\n");
			}
			else
			{
				// Check the arguments we know of for a good error printout.
				if( ACE_OS::strcmp( argv[1], "-f") == 0 )
				{
					string	output = "Option ";
					output = output + argv[1] + " needs two arguments\n";
					omHandler.Finalize();
					ExitPrint(argv[0], 2, output); 
					
				}
				if( ACE_OS::strcmp( argv[1], "-r") == 0 )
				{
					string	output = "Option ";
					output = output + argv[1] + " needs one argument\n";
					omHandler.Finalize();
					ExitPrint(argv[0], 2, output); 
				}
				if( argv[1][0] == '-' )
				{
					string output = "Option ";
					output = output + argv[1] + "\n";
					omHandler.Finalize();
					ExitPrint(argv[0], 2, output); 
				}
				else
				{
					string output = "Illegal operand ";
					output = output + argv[1] + "\n";
					omHandler.Finalize();
					ExitPrint(argv[0], 2, output); 				
				}
			}
		} break;
		
		// Two arguments
	case 3: {	
		// mtzln -r TMZ_value
		// OR 
		// mtzln TZ_name TMZId
		// Check for faulty options.
		if(ACE_OS::strlen( argv[1] ) >= (ACE_INT32)NTZoneSize)
		{
             string output = "Illegal operand ";
             output = output + "TZ_name too long \n";
             omHandler.Finalize();
             ExitPrint(argv[0], 5, output);
		}
		if( ACE_OS::strlen( argv[1] ) == 2 && ACE_OS::strcmp( argv[1], "-r") != 0 )
		{
			// This is an option we don't know of
			if( ACE_OS::strcmp( argv[1], "-f") == 0 )
			{
				string	output = "Option ";
				output = output + argv[1] + " needs two arguments\n";
				omHandler.Finalize();
				ExitPrint(argv[0], 2, output); 
				
			}
			if( ACE_OS::strcmp( argv[1], "-p") == 0 )
			{
				string	output = "Option ";
				output = output + argv[1] + " can not take an argument\n";
				omHandler.Finalize();
				ExitPrint(argv[0], 2, output); 
			}
			// Nothing we nows about!
			string	output = "Illegal option ";
			output = output + argv[1] + " \n";
			omHandler.Finalize();
			ExitPrint(argv[0], 2, output); 
		}
		if( ACE_OS::strcmp( argv[1], "-r") == 0 )
		{	// mtzln -r TMZ_value
			if( MTZObj.ValidateTMZNumber(argv[2]) == false )
			{
				omHandler.Finalize();
				ExitPrint(argv[0], 3, 
					"Operand TMZ_value should be an integer between 0-23 \n");
			}

			extractTMZ(argv[2], TMZ_value);

			if( (TMZ_value < 0 ) || ( TMZ_value > 23 ))
			{
				omHandler.Finalize();
				ExitPrint(argv[0], 3, 
					"Operand TMZ_value should be an integer between 0-23 \n");
			}

			if( MTZObj.ReadTZsFromIMM( omHandler, &fMap ) == false )
			{
				omHandler.Finalize();
				ExitPrint( argv[0], 117, "Unable to connect to server \n");
			}
			p = fMap.find(TMZ_value);
			
			if( p != fMap.end())
			{
				if( MTZObj.RemoveTZFromIMM( omHandler, TMZ_value) == false )
				{
					int errorId = 0;
					string errorText("");
					int ccbId = 0;
					omHandler.getExitCode(errorId, errorText);
    	            getLastError(omHandler,ccbId,errorId,errorText);
					omHandler.Finalize();
					ExitPrint(argv[0], errorId, errorText);
				}
			}
			else
			{
				omHandler.Finalize();
				ExitPrint(argv[0], 4,"TMZ_value not defined \n");
			}
			omHandler.Finalize();
			ExitPrint(argv[0], 0, "\n");

		} // end mtzln -r TMZ_value
		else
		{
			if( MTZObj.ValidateTMZNumber( argv[2]) == false )
			{
				omHandler.Finalize();
				ExitPrint(argv[0], 3, 
					"Operand TMZ_value should be an integer between 0-23 \n");
			}
			extractTMZ(argv[2], TMZ_value);
			extractTZ(argv[1], TZ_name);

			std::string tzName(TZ_name);
			if(!ACS_CHB_Common::TrimSpaces(tzName))
			{	
				string output = "Illegal operand ";
				output = output + TZ_name + " Is not a defined time zone\n";
				omHandler.Finalize();
				ExitPrint(argv[0], 6, output);
			}

			if( MTZObj.ReadTZsFromIMM( omHandler, &fMap ) == false )
			{
				omHandler.Finalize();
				ExitPrint( argv[0], 117, "Unable to connect to server \n");
			}
			p = fMap.find(TMZ_value);
			if( p != fMap.end())
			{
				if( (ACE_INT32)NTZoneSize <= ACE_OS::strlen(TZ_name))
				{
					string output = "Illegal operand ";
					output = output + "TZ_name too long \n";
					omHandler.Finalize();
					ExitPrint(argv[0], 5, output);
				}

				TIME_ZONE_INFORMATION Tzi;
				if( !MTZObj.CheckTimeZone(tzName.c_str(), &Tzi))
				{
					string output = "Illegal operand ";	
					output = output + TZ_name + " Is not a defined time zone\n";
					omHandler.Finalize();
					ExitPrint(argv[0], 6, output);
				}
			
					
				cerr << "TMZ_value already bound ";
				cerr << "No action performed " << endl;
				cerr << "Use -f option to overwrite existing link \n" << endl;
				omHandler.Finalize();
				exit(7);	

			}
			if( MTZObj.AddTZToIMM( omHandler, TMZ_value, (char *)tzName.c_str()) == false )
			{
				int errorId = 0;
				string errorText("");
				int ccbId = 0;
				omHandler.getExitCode(errorId, errorText);
				getLastError(omHandler,ccbId,errorId,errorText);
				omHandler.Finalize();
				ExitPrint(argv[0], errorId, errorText);
			}
				
			string output = " will be bound to ";
			output = argv[2] + output ;
			output = " : " + output ;
			output = output + tzName.c_str() + "\n" ;
			omHandler.Finalize();
			ExitPrint(argv[0], 0, output);
		} // end mtzln TZ_name TMZ_value
		
		} break;
		
	case 4: {	// mtzln -f TZ_name TMZ_value
		 if(ACE_OS::strlen( argv[2] ) >= (ACE_INT32)NTZoneSize)
         {
             string output = "Illegal operand ";
             output = output + "TZ_name too long \n";
             omHandler.Finalize();
             ExitPrint(argv[0], 5, output);
         }
		// Check for faulty options.
		if( ACE_OS::strlen( argv[1] ) == 2 && ACE_OS::strcmp( argv[1], "-f") != 0 )
		{
			if( ACE_OS::strcmp( argv[1], "-p") == 0 )
			{
				string	output = "Option ";
				output = output + argv[1] + " can not take an argument\n";
				omHandler.Finalize();
				ExitPrint(argv[0], 2, output); 
			}
			if( ACE_OS::strcmp( argv[1], "-r") == 0 )
			{
				string	output = "Option ";
				output = output + argv[1] + " needs one argument\n";
				omHandler.Finalize();
				ExitPrint(argv[0], 2, output); 
			}
			// Nothing we nows about!
			string	output = "Illegal option ";
			output = output + argv[1] + " \n";
			omHandler.Finalize();
			ExitPrint(argv[0], 2, output); 
		}
		if( ACE_OS::strcmp( argv[1], "-f") == 0 )
		{   // Let's create a association.

			if( MTZObj.ValidateTMZNumber( argv[3]) == false )
			{
				omHandler.Finalize();
				ExitPrint(argv[0], 3, 
					"Operand TMZ_value should be an integer between 0-23 \n");
			}
			extractTZ(argv[2], TZ_name);
			extractTMZ(argv[3], TMZ_value);
		}	
		else 
		{ 
			// We don't have an option at start.
			// Which is a must!
			string output = "Illegal operand ";
			output = output + argv[1] + "\n";
			omHandler.Finalize();
			ExitPrint(argv[0], 2, output); 	
		}
		if( MTZObj.ReadTZsFromIMM(omHandler, &fMap ) == false)
		{
			omHandler.Finalize();
			ExitPrint(argv[0], 117, "Unable to connect to server \n");
		}

		std::string tzName(TZ_name);
		if(!ACS_CHB_Common::TrimSpaces(tzName))
		{
			string output = "Illegal operand ";
			output = output + TZ_name + " Is not a defined time zone\n";
			omHandler.Finalize();
			ExitPrint(argv[0], 6, output);
		}

		p = fMap.find(TMZ_value);
		if( p != fMap.end() )
		{
			if( MTZObj.UpdateTZIMM(omHandler, TMZ_value, (char *)tzName.c_str()) == false )
			{
				int errorId = 0;
				string errorText("");
				int ccbId = 0;
				omHandler.getExitCode(errorId, errorText);
                getLastError(omHandler,ccbId,errorId,errorText);
				omHandler.Finalize();
				ExitPrint(argv[0], errorId, errorText);

			}
			omHandler.Finalize();
			ExitPrint(argv[0], 0, "\n");

		}
		else
		{
			if( MTZObj.AddTZToIMM(omHandler, TMZ_value, (char *) tzName.c_str()) == false )
			{
				int errorId = 0;
				string errorText("");
                int ccbId = 0;
				omHandler.getExitCode(errorId, errorText);
				getLastError(omHandler,ccbId,errorId,errorText);
				omHandler.Finalize();
				ExitPrint(argv[0], errorId, errorText);

			}
			omHandler.Finalize();
			ExitPrint(argv[0], 0, "\n");
		}
	}
	break;
	} // end switch
	// If success !
	omHandler.Finalize();
	ExitPrint(argv[0], 0, "\n"); 
	return 0;
}


/*===================================================================
   ROUTINE: extractTMZ
=================================================================== */
void extractTMZ(ACE_TCHAR* TMZ_str, ACE_INT32 &TMZ_value)
{
	TMZ_value = ACE_OS::atoi(TMZ_str);

}


/*===================================================================
   ROUTINE: extractTZ
=================================================================== */
void extractTZ(ACE_TCHAR* TZ_str, ACE_TCHAR* TZ_name)
{
	ACE_OS::strcpy(TZ_name,TZ_str);
}


/*===================================================================
   ROUTINE: ExitPrint
=================================================================== */
void ExitPrint(const ACE_TCHAR* argv0, ACE_INT32 exitcode, const string slogan)
{
	const ACE_TCHAR* chPoint = slogan.c_str();
	switch (exitcode) {
		
		// Command executed succesfully
	case 0 :
		cout << "\nExecuted " << slogan << endl;
		exit(0);
		break;
	case 2 :
		cerr << "ERROR : " << chPoint << endl;
		printUsage(argv0);
		exit(2);
		break;
	default :
		cerr << "ERROR : " << chPoint << endl;
		exit(exitcode);
		break;
		
	}
	
}

/*===================================================================
   ROUTINE: printUsage
=================================================================== */
void printUsage(const ACE_TCHAR* argv0)
{
	cout << "Usage : Create association:  " << ACE::basename(argv0, ACE_DIRECTORY_SEPARATOR_CHAR) << " [-f] \"TZ_name\" TMZ_value" << endl;
	cout << "Usage : Remove association:  " << ACE::basename(argv0, ACE_DIRECTORY_SEPARATOR_CHAR) << " -r TMZ_value" << endl;
	cout << "Usage : Print associations:  " << ACE::basename(argv0, ACE_DIRECTORY_SEPARATOR_CHAR) << " -p" << "\n" << endl;
}


/*===================================================================
   ROUTINE: getLastError
=================================================================== */
void getLastError(OmHandler& omHandler, int& ccbId, int &errorId, string& errorText)
{
	(void)omHandler;
	(void)ccbId;
	if(errorText.rfind("integer between 0-23 \n") != string::npos)
	{
		errorId = 3;
	}
	else if(errorText.rfind("TMZ_value not defined \n") != string::npos)
    {
        errorId = 4;
    }
	else if(errorText.rfind("TZ_name too long \n") != string::npos)
	{
		errorId = 5;
	}
	else if(errorText.rfind(" Is not a defined time zone\n") != string::npos)
	{
		errorId = 6;
	}
	else if(errorText.rfind("overwrite existing link \n") != string::npos)
	{
		errorId = 7;
	}
	else if(errorText.rfind("The MTZ file can't be accessed \n") != string::npos)
    {
        errorId = 8;
    }
	else
	{
		errorId = 117;
		errorText = "Unable to connect to server \n";
	}
	return ;
}
