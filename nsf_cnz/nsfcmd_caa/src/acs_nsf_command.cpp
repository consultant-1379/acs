/*=================================================================== */
   /**
   @file acs_nsf_command.cpp

   This file contains the code for the nsfstart/nsfstop/nsfreset
   commands. 
   nsfstart/nsfstop commands are used to start/stop surveillance of the external network.  
   nsfreset command is used to reset the number of failovers.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       30/11/2010     XCHVKUM   Initial Release
   N/A       19/04/2011     XTANAGG   Added doxygen comments and corrected
				      error messages.
   **/
/*=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <fstream>
#include <string>
#include <ace/ACE.h>
#include <acs_nsf_common.h>
using namespace std;

/*=============================================================================
			GLOBAL DECLARATION 
============================================================================= */
/*=============================================================================
	ROUTINE: getLastError
============================================================================= */
void getLastError(int& ccbId,int &errorId, string& errorText)
{
	(void)ccbId;
	errorText = "Unable to connect to server";
	errorId = 117;
	return;
}


/*=============================================================================
	ROUTINE: ExitPrint
============================================================================= */
void ExitPrint(ACE_INT32 exitcode, const string slogan)
{
	cout<<slogan<<"\n"<<endl;
	exit(exitcode);
}

/*=============================================================================
	ROUTINE: nsfstart
============================================================================= */
int nsfstart(void)
{
	ACS_NSF_Common m_poNsfCommon;
	int  eResult  = -1;
	eResult = m_poNsfCommon.setNodeSpecificParams();
	char *message = NULL;

	string errorText("");
	int errorId = 0;
	if (eResult == -1)
	{
		errorId 	= 117;
		errorText = "Unable to connect to server";
	}
	else
	{
		eResult = m_poNsfCommon.setRegWord(NSF_SURVELLIENCEACTIVEFLAG, 1, &message);

		if (eResult == -1)
		{
			errorId 	= 117;
			errorText = "Unable to connect to server";
		}
		else
		{
			errorId = eResult;
			errorText = message;
		}
		/*else if ( eResult == 1 )
		{
			int ccbId = 0;
			getLastError(ccbId,errorId, errorText);
		}
		else if ( eResult == 0 )
		{
			errorId = 0;
			errorText = "Network surveillance started";
		}*/
	}
	ExitPrint(errorId, errorText);
	if(message != NULL)
	{
		delete[] message;
		message = NULL;
	}
	return 1;
}

/*=============================================================================
	ROUTINE: nsfstop
============================================================================= */
int nsfstop(void)
{
	ACS_NSF_Common m_poNsfCommon;
	int eResult  = -1;
	eResult = m_poNsfCommon.setNodeSpecificParams();
	char *message = NULL;

	string errorText("");
	int errorId = 0;
	if (eResult == -1)
	{
		errorId = 117;
		errorText = "Unable to connect to server";
	}
	else
	{
		eResult = m_poNsfCommon.setRegWord(NSF_SURVELLIENCEACTIVEFLAG, 0, &message);

		if (eResult == -1)
		{
			errorId = 117;
			errorText = "Unable to connect to server";
		}
		else
		{
			errorId = eResult;
			errorText = message;
		}
		/*else if ( eResult == 1 )
		{
			int ccbId = 0;
			getLastError(ccbId,errorId, errorText);
		}
		else if ( eResult == 0 )
		{
			errorId = 0;
			errorText = "Network surveillance stopped";
		}*/
	}
	ExitPrint(errorId, errorText);
	if(message != NULL)
	{
		delete[] message;
		message = NULL;
	}
	return 1;
}


/*=============================================================================
	ROUTINE: nsfreset
============================================================================= */
int nsfreset(void)
{
	ACS_NSF_Common m_poNsfCommon;
	int eResult = -1 ;
	eResult = m_poNsfCommon.setNodeSpecificParams();
	string errorText("");
	int errorId = 0;
	if (eResult == -1)
	{
		errorId 	= 117;
		errorText = "Unable to connect to server";
	}
	else
	{
		eResult = m_poNsfCommon.setRegWord(NSF_NOOFFAILOVER,0);

		if (eResult == -1)
		{
			errorId 	= 117;
			errorText = "Unable to connect to server";
		}
		else if ( eResult == 1 )
		{
			int ccbId = 0;
			getLastError(ccbId,errorId, errorText);
		}
		else if ( eResult == 0 )
		{
			errorId = 0;
			errorText = "Failover counter reset";
		}
	}
	ExitPrint(errorId, errorText);
	return 1;
}


/*=============================================================================
	ROUTINE: ACE_TMAIN
============================================================================= */
ACE_INT32 ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
	string str ;
	str = argv[0];
	size_t found;
	found = str.find_last_of("/\\");
	string subStr ;
	if( found != string::npos )
	{
		subStr.assign(str.begin() + found + 1, str.end());
	}
	else
	{
		subStr.assign(str);
	}

	if (argc > 1)
	{
		if ( ( strcmp(subStr.c_str(),"nsfstart") == 0 ) || 
		     ( strcmp(subStr.c_str(),"nsfstop") == 0 ) || 
		     ( strcmp(subStr.c_str(),"nsfreset") == 0  ) )
		{
			cout << "Usage : "<< subStr << "\n" << endl;
		}
		exit(2);
	}
	else
	{
		//bool bResult = 1;
		if (strcmp(subStr.c_str(),"nsfstart") == 0 )
		{
			nsfstart() ;
		}
		else if (strcmp(subStr.c_str(),"nsfstop") == 0  )
		{
			nsfstop() ;
		}
		else if (strcmp(subStr.c_str(),"nsfreset") == 0  )
		{
			nsfreset() ;
		}
	}
	return 0 ;
} // End of ACE_TMAIN
