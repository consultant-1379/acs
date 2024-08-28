//******************************************************************************
// File for basic test of Reg Exp functionality
//******************************************************************************

#include <windows.h>
#include <iostream>

#include <string.h>
#include <stdlib.h>
#include <string>
#include "ACS_USA_ATLRegExp.h"
#include "tusa_general.h"

using namespace std;
typedef string String;
ACS_USA_Error error;

//******************************************************************************
//	ACS_USA_Regexp()
//******************************************************************************
ACS_USA_Regexp::ACS_USA_Regexp()
    : compiled(0),
      status(Status_notCompiled)  
{
}  

//******************************************************************************
//	ACS_USA_Regexp() - Copy constructor
//******************************************************************************
ACS_USA_Regexp::ACS_USA_Regexp(const ACS_USA_Regexp& other)
    : compiled(0),
      status(other.status)  
{
    // If the expression is compiled, make a copy of the regexp
	//
	if (status == Status_compiled) 
	{
		compiled =	new Regexp(*(other.compiled));
		if (compiled == NULL) 
		{
			error.setError(ACS_USA_Memory);
			error.setErrorText("new() failed in ACS_USA_Regexp::ACS_USA_Regexp().");
		}
	}	
}

//******************************************************************************
//	~ACS_USA_Regexp()
//******************************************************************************
ACS_USA_Regexp::~ACS_USA_Regexp()
{
    if (compiled != NULL)
	{
		delete compiled;
	}
}


//******************************************************************************
//	compilePattern
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Regexp::compilePattern(const String& pattern)
{
    // "Compile" (construct) the regular expression from incoming pattern
	//
	Regexp* re = new Regexp(pattern.data());
	
	if (re == NULL) 
	{
		error.setError(ACS_USA_Memory);
		error.setErrorText("new() failed in ACS_USA_Regexp::compilePattern(). Criterion:");
		return ACS_USA_error;	
	}
	if (re->status() == Regexp::OK)
	{
		compiled = re;
	}
	else									// failure
    {		
		error.setError(ACS_USA_SyntaxError);
		error.setErrorText(Regex_prefix, Regex_failed, re->status());
		delete re;
		return ACS_USA_error;
    }
    status = Status_compiled;
    return ACS_USA_ok;
}  


//******************************************************************************
//	checkMatch()
//******************************************************************************
Regexp_MatchType
ACS_USA_Regexp::checkMatch(const ACS_USA_RecordType record) const
{
    if (status == Status_notCompiled)		// Something may be wrong
	{
		error.setError(ACS_USA_BadState);
		error.setErrorText(Regexp_BadState);
		return ACS_USA_Regexp_error;
    }
   
	String recordstr(record);
	try
	{ 
		// Check if the incoming string (record) matches the regexp
		//
		size_t tmp;
		if (compiled->index(recordstr,&tmp, (size_t)0) != string::npos)
		{
			return ACS_USA_Regexp_match;
		}
	}
	catch(...)								// Probably RWxmsg exception
	{			
		error.setError(ACS_USA_SyntaxError);
		error.setErrorText(Regex_prefix, Regex_failed, compiled->status());
		return ACS_USA_error;
	}

    return ACS_USA_Regexp_noMatch;
}


//******************************************************************************
//	checkMatch()
//******************************************************************************
Regexp_MatchType
ACS_USA_Regexp::checkMatch(
			const ACS_USA_RecordType record,
			ACS_USA_ByteCount* unMatched) const
{
    if (status == Status_notCompiled)		// Something may be wrong
	{
		error.setError(ACS_USA_BadState);
		error.setErrorText(Regexp_BadState);
		return ACS_USA_Regexp_error;
    }

	String recordstr (record);  	
	size_t matchlength;
	size_t matchstart;

	// Check if incoming record matches the regexp
	//
	matchstart = compiled->index(recordstr, &matchlength, (size_t)0);
	
	if (matchstart != string::npos)
	{
		// Length of match + start of match is 
		// first non-matching char after matched text
		//
		*unMatched = matchlength + matchstart; 

		return ACS_USA_Regexp_match;
    }
    return ACS_USA_Regexp_noMatch;
}


//******************************************************************************
//	checkMatch()
//******************************************************************************
Regexp_MatchType
ACS_USA_Regexp::checkMatch(
			const ACS_USA_RecordType record,
			ACS_USA_ByteCount* begin,
			ACS_USA_ByteCount* extent) const
{
    if (status == Status_notCompiled)		// Something may be wrong 
	{
		error.setError(ACS_USA_BadState);
		error.setErrorText(Regexp_BadState);
		return ACS_USA_Regexp_error;
    }

	String recordstr = String(record);  	
	size_t matchlength;
	size_t matchstart;

	// Check if incoming record matches the regexp
	//
	try
	{
		matchstart = compiled->index(recordstr, &matchlength, (size_t)0);		
	}
	catch(...)						// If an error occurs, we don't have a match 
	{
		return ACS_USA_Regexp_noMatch;
	}

	if (matchstart != string::npos)
	{
		*begin = matchstart;        // First char matched
		*extent = matchlength;		// Length of match 
    	return ACS_USA_Regexp_match;
    }
    return ACS_USA_Regexp_noMatch;
}


//******************************************************************************
//	operator =
//******************************************************************************
const ACS_USA_Regexp&
ACS_USA_Regexp::operator=(const ACS_USA_Regexp& other)
{
    if (this != &other)				// if not myself
	{			
		status = other.status;
		
		if (status == Status_compiled) 
		{
			delete compiled;
			compiled =	new Regexp(*(other.compiled)); 
			if (compiled == NULL) 
			{
				error.setError(ACS_USA_Memory);
				error.setErrorText("new() failed in ACS_USA_Regexp::operator=(). Criterion:");
			}
		}
    }
    return *this;
}

//******************************************************************************
// createUserDefinedEvent()
//******************************************************************************

int createUserDefinedEvent()
{ 
	system("cls");
	int ret = 1;
//	char sourceName [100] = "";
	char choice [100] = "";
	char log [100] = "";

	HANDLE sourceHandl;                           // Handle to source name


	//cout << endl << "Enter the Log for the event (s)ystem, (a)pplication or S(e)curity > " << flush;
	//cin >> log;
	strcpy(log,"Application");
	HANDLE hndl = OpenEventLog(NULL,log);  

	cout << endl << "The event will be written to the log that is defined for the source, defualt is the Application log " << endl << endl;
	cout << endl << "NOTE!" << endl << "If the event to be written is to be a simulated event, looking like an event written by an existing application or device driver, see the Registry key for that application/device driver, on path HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\EventLog" << endl << endl;

	//cout << endl << "Because of cin problems handling spaces in strings:"<< flush;
	cout << endl << "If you enter SCM, the Source for the event will be Service Control Manager" << flush;
	cout << endl << "If you enter APOP, the Source for the event will be Application Popup" << flush;

	cin.ignore();
	cout << endl << endl << "Enter the Source for the event > " << flush;
	//cin >> choice;
	cin.getline(choice, 100);  //Can handle spaces

	if (strcmp(choice, "SCM") == 0)
	{
		sourceHandl = RegisterEventSource(NULL, "Service Control Manager");
	}
	else if (strcmp(choice, "APOP") == 0)
	{
		sourceHandl = RegisterEventSource(NULL, "Application Popup");
	}
	else 
	{
		sourceHandl = RegisterEventSource(NULL,choice);
	}

	unsigned int eventType = 0;
	cout << endl << "1) EVENTLOG_ERROR_TYPE, 2) EVENTLOG_WARNING_TYPE, 3) EVENTLOG_INFORMATION_TYPE, 4) EVENTLOG_AUDIT_SUCCESS, 5) EVENTLOG_AUDIT_FAILURE " << endl;
	cout << endl << "NOTE ! Be sure to choose an event type that is valid for the specific source, or else your invalid event might confuse the Event log and thereby causing USA to crash!" << endl;
	cout << endl << "Enter the event type for the event (1,2,3,4 or 5)> " << flush;
	cin >> eventType;

	unsigned int eventId= 0;
	cout << endl << "Enter the event id for the event > " << flush;
	cin >> eventId;

//	sprintf(sourceName, "%s", choice);

//LPCTSTR* 
	char descriptionText[300] = "";
	//char * descriptionText = " ";

//	int tmpint = 0;
	char* msgArray [100];

	cin.ignore();
	cout << endl << "Enter a text to be written in the event > " << flush;
	//(char*)descriptionText = new char [300];
	//cin >> descriptionText;
	cin.getline(descriptionText, 300);		//Can handle spaces
	msgArray[0] = descriptionText;
	
//	char* msgArray [1];
//	strcpy(msgArray[0], descriptionText);
	//msgArray[0]="test";
	char** msgPointer =  &msgArray[0];
	
	if (!ReportEvent(sourceHandl, eventType, 0, eventId, 0, 1,
		0, (const char**)msgPointer, NULL)) 
	{
		int errorcode = GetLastError();
		cout << endl << "could't report event, NT ERRORCODE: " << errorcode << endl;
	}
	else cout << endl << "Event written to the event log"  << endl;
	
	try
	{
		CloseHandle(sourceHandl);
		CloseEventLog(hndl);
	}
	catch (...)
	{
		// Ignore invalid handle
	}

	return ret;
}

//******************************************************************************
// main()
//******************************************************************************


int main()
{
	int choice;
	cout << endl << "Welcome to the ACS_USA test hack" << endl << endl << "       MENU: ";


	cout << endl << "- Test regexp     (1), " << endl;
	cout << endl << "- Read EventLog   (2), " << endl;
    cout << endl << "- Write event " << endl;
	cout << "  to EventLog     (3) >" << flush;

	cin >> choice;
	if (choice == 1)   // Regexp-test starts here
	{
		
		//	ACS_USA_Regexp reg;
		ACS_USA_Regexp reg = ACS_USA_Regexp(); 

		//--
     /*  char intkn1 [500];
	   char intkn2 [500]; 
	//	char* log = 0;
		cout << endl << "Choose event text, default is ' Mon Jul 10 19:52:01 1970 3422 UPC7026 24999 89 test_Event EVENTLOG_ERROR_TYPE' (q skips)> "; 

		cin >> intkn1;
		cout << endl << "Choose event pattern, default is '2[0-3][2-9][3-9][2-9] [0-9]+ test_Event EVENTLOG_ERROR_TYPE' (q skips)> "; 

		cin >> intkn2;

	//	if(in1 == 'a' |
		//--
*/
		//OK RWCString pattern = "203[0-9][0-9] [0-9]+ test_Event EVENTLOG_ERROR_TYPE";
		// OK RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 23999 89 test_Event EVENTLOG_ERROR_TYPE";
		// OK RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 20399 89 test_Event EVENTLOG_ERROR_TYPE";

		// OK RWCString pattern = "110[79] [0-9]+ Service Control Manager";
		// OK RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 1109 89 Service Control Manager EVENTLOG_ERROR_TYPE";

		// OK RWCString pattern = "1011 [0-9]+ ClusSvc";
		// OK RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 1011 189 ClusSvc EVENTLOG_ERROR_TYPE";

	//	OK RWCString pattern = "1066 [0-9]+ ClusSvc";
	//	OK RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 1066 189 ClusSvc EVENTLOG_ERROR_TYPE";

	// OK	RWCString pattern = " [0-9]+ [Aa][Ll][Ee][Rr][Tt][Ee][Rr] EVENTLOG_ERROR_TYPE";
	// OK	RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 1109 89 aLeRteR EVENTLOG_ERROR_TYPE";
// OK RWCString pattern = " [0-9]+ [Bb][Rr][Oo][Ww][Ss][Ee][Rr] EVENTLOG_ERROR_TYPE";
// OK RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 1109 89 browSEr EVENTLOG_ERROR_TYPE";

// OK RWCString pattern = "[0-9]+ D[Ii][Ss][Tt][Ii][Nn][Cc][Tt][Tt][Ff][Tt][Pp][Ss].+ EVENTLOG_ERROR_TYPE";
// Ok RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 1109 89 DistinctTFTPService EVENTLOG_ERROR_TYPE";

//	RWCString pattern = "257 [0-9]+ ethnddrv EVENTLOG_WARNING_TYPE 'No traffic on card ([34])'";
//	RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 257 89 ethnddrv EVENTLOG_WARNING_TYPE 'No traffic on card (4)'";

// ok	RWCString pattern = "[0-9]+ [Ll][Aa][Nn][Mm][Aa][Nn][Ww][Oo][Rr][Kk][Ss][Tt].+ EVENTLOG_ERROR_TYPE";
// ok	RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 1109 89 LanmanWorkstation EVENTLOG_ERROR_TYPE";

// ok	RWCString pattern = "257 [0-9]+ [Ee][Tt][Hh][Nn][Dd][Dd][Rr][Vv] EVENTLOG_WARNING_TYPE";
// ok	RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 257 89 ethNddrv EVENTLOG_WARNING_TYPE";

//RWCString pattern = "702[3-5] [0-9]+ .+ D[Ii][Ss][Tt][Ii][Nn][Cc][Tt][Bb][Oo]+[Tt][Pp][Ss]";
						
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7023 89 pelle EVENTLOG_INFORMATION_TYPE 4.0 1381 Service Pack 5 DistinctBootPService Uniprocessor Free";
//RWCString pattern = "257 [0-9]+ [Ee][Tt][Hh][Nn][Dd][Dd][Rr][Vv] EVENTLOG_WARNING_TYPE .+ No traffic on card [34]";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 257 89 ethnddrv EVENTLOG_WARNING_TYPE 4.0 1381 Service Pack 5 No traffic on card 3 Uniprocessor Free";
// 257 [0-9]+ [Ee][Tt][Hh][Nn][Dd][Dd][Rr][Vv] EVENTLOG_WARNING_TYPE .+ No traffic on card [34]

//RWCString pattern = "702[3-5] [0-9]+ Service Control Manager .+ [Dd][Ii][Ss][Tt][Ii][Nn][Cc][Tt][Bb][Oo]+[Tt]";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 Service Control Manager EVENTLOG_WARNING_TYPE 4.0 1381 Service Pack 5 DistinctBootPService Uniprocessor Free";
//RWCString pattern = "702[3-5] [0-9]+ Service Control Manager .+ [Dd][Ii][Ss][Tt][Ii][Nn][Cc][Tt][Tt][Ff][Tt]";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 Service Control Manager EVENTLOG_WARNING_TYPE 4.0 1381 Service Pack 5 DistinctTFTPService Uniprocessor Free";
//702[3-5] [0-9]+ Service Control Manager .+ [Dd][Ii][Ss][Tt][Ii][Nn][Cc][Tt]+[Ff][Tt][Pp][Ss]
// RWCString pattern = "[0-9]+ [Aa][Ff][Dd] EVENTLOG_ERROR_TYPE";
// RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 AFd EVENTLOG_ERROR_TYPE 4.0 1381 Service Pack 5 DistinctTFTPService Uniprocessor Free";
//RWCString pattern = "[0-9]+ [Aa][Ii][Cc]78[Uu]2 EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 aIc78U2 EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Aa][Tt][Aa][Pp][Ii] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 atapP EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Dd][Cc]21[Xx]4 EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 DC21X4 EVENTLOG_ERROR_TYPE 4.0 1381";	
//RWCString pattern = "[0-9]+ [Dd][Ii][Ss][Kk] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 disK EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Ee]100[Bb] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 e100B EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Nn][Dd][Ii][Ss] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 NdiS EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Mm][Ss][Ff][Ss] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 MSFs EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Mm][Uu][Pp] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 MuP EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Nn][Ee][Tt][Bb][Ii][Oo][Ss] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 NetBIOS EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Bb][Hh] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 bH EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Nn][Pp][Ff][Ss] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 NPfs EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Nn][Tt][Ff][Ss] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 ntfs EVENTLOG_ERROR_TYPE 4.0 1381";	
//RWCString pattern = "[0-9]+ [Rr][Dd][Rr] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 rDr EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Ss][Cc][Ss][Ii][Pp][Rr][Nn][Tt] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 SCSIPRnt EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Ss][Cc][Ss][Ii][Ss][Cc][Aa][Nn] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 SCSIscAn EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Ss][Ee][Rr][Ii][Aa][Ll] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 seriaL EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Ss][Ff][Ll][Oo][Pp][Pp][Yy] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 sfloppy EVENTLOG_ERROR_TYPE 4.0 1381";	
//RWCString pattern = "[0-9]+ [Ss][Rr][Vv] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 srv EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Tt][Cc][Pp][Ii][Pp] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 tcpIp EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Nn][Ee][Tt][Bb][Tt] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 netBt EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Tt][Dd][Ii] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 tdi EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Pp][Ee][Rr][Ff][Cc][Tt][Rr][Ss] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 perfctrs EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Pp][Ee][Rr][Ff][Ll][Ii][Bb] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 perfliB EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Pp][Ee][Rr][Ff][Mm][Oo][Nn] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 perfmoN EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Tt][Ee][Ll][Mm][Ss][Rr][Vv][Cc] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 telmsrVc EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Tt][Ee][Ll][Nn][Ee][Tt][Dd] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 teLnetD EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Aa][Mm]-[Ss][Ee][Rr][Vv][Ii][Cc][Ee] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 Am-ServicE EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Rr][Ee][Pp][Ll][Ii][Cc][Aa][Tt][Oo][Rr] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 7024 89 ReplicatoR EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "258 [0-9]+ [Ee][Tt][Hh][Nn][Dd][Dd][Rr][Vv] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 258 89 ethnddrv EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Dd][Ii][Ss][Kk][Ee][Ee][Pp][Ee][Rr] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 258 89 diskeeper EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Ll][Bb][Bb][Aa][Mm]_[Ss][Ee][Rr][Vv][Ii][Cc][Ee] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 258 89 LBBAM_servIce EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Nn][Ee][Tt][Dd][Dd][Ee] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 258 89 NetDDE EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Pp][Cc][Aa][Nn][Yy][Ww][Hh][Ee][Rr][Ee]32 EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 258 89 pcanywhere32 EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Ss][Cc][Hh][Ee][Dd][Uu][Ll][Ee] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 258 89 schedUle EVENTLOG_ERROR_TYPE 4.0 1381";
//RWCString pattern = "[0-9]+ [Ss][Ee][Rr][Vv][Ee][Rr] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 258 89 server EVENTLOG_ERROR_TYPE 4.0 1381";	
//RWCString pattern = "[0-9]+ [Ww][Oo][Rr][Kk][Ss][Tt][Aa][Tt][Ii][Oo][Nn] EVENTLOG_ERROR_TYPE";
//RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 258 89 workStatioN EVENTLOG_ERROR_TYPE 4.0 1381";
String pattern = "[0-9]+ [Ww][Oo][Rr][Kk][Ss][Tt][Aa][Tt][Ii][Oo][Nn] EVENTLOG_ERROR_TYPE";
String theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 258 89 workStatioN EVENTLOG_ERROR_TYPE 4.0 1381";	



	
	/*	if (intkn1 != "q") {}
		else {pattern = intkn1;}
		if (intkn2 != "q") {}
		else {theText = intkn2;}
		*/

		cout << endl << "pattern: "  << pattern << endl << endl << "text: " << '\t' << theText << endl;

		
	//	RWCString pattern = "[U-Z][a-z]*on";
//		RWCString theText = "Goomorron, gooomorron! De eVeironQ i ottan!";
		
		ACS_USA_StatusType  answer = reg.compilePattern(pattern);
		cout << endl << "compilePattern returns (1=ok) : " << answer  << endl;
		
		Regexp_MatchType matchresult;
		
		// Testcase 1, checkMatch 1
		try
		{
			matchresult = reg.checkMatch((char* const)theText.data());	
		}
		catch(...)
		{
			cout << "exception catched in main()" << endl;	 
		}
		
		cout << endl << "match result (2=match, 1 = no match, 0 = error) :" << matchresult << endl << endl;
		if (matchresult == 2) cout << "MATCH!!!" << endl;
			else cout << " NO match... Sorry" << endl;
		
		Sleep (2000);   // Wait 2 sec
		
		
		// Testcase 2, checkMatch 2
		
		unsigned int* firstNonMatchAfterMatch = new unsigned int;
		
		*firstNonMatchAfterMatch = 0;
		
		matchresult = 0;
		
		try
		{
			matchresult = reg.checkMatch((char* const)theText.data(),firstNonMatchAfterMatch);
		}
//		catch(RWxmsg& e)    // RWInternalErr also OK to catch.
		catch(...)
		{
			//cout << "RW-exception fångat i main(): "  << endl << e.why() << endl;	 
			cout << "exception catched in main() "  << endl << endl;	 
		}
		
		cout << endl << "firstNonMatchAfterMatch: " << *firstNonMatchAfterMatch << endl;
		cout << endl << "match result (2=match, 1 = no match, 0 = error) :" << matchresult << endl << endl;
		
		delete firstNonMatchAfterMatch;
		Sleep (2000);   // Wait 2 sec
		
		
		// Testcase 3, checkMatch 3
		
		unsigned int* start = new unsigned int;
		unsigned int* length = new unsigned int;
		*start = 0;
		*length = 0;
		matchresult = 0;
		
		try
		{
			matchresult = reg.checkMatch((char* const)theText.data(),start,length);	
		}
		//catch(RWxmsg& e)    // RWInternalErr also OK to catch.
		catch(...) 
		{
			//cout << "RW-exception fångat i main(): "  << endl << e.why() << endl;	 
			cout << "exception catched in main(): "  << endl; 
		}
		
		cout << endl << "Start position: " << *start << ",  matched length: " << *length << endl << endl;
		cout << endl << "match result (2=match, 1 = no match, 0 = error) :" << matchresult << endl << endl;
		
		delete start;
		delete length;
		
		ACS_USA_Regexp nyreg = ACS_USA_Regexp();
		
		cout << endl << "First test of '='";
		reg = reg;  // "operator=" ska märka att regexparna är fysiskt desamma 
		cout << endl << "Second test of '='";
		nyreg = reg; // "operator=" ska märka att regexparna är olika
		
		ACS_USA_Regexp treg = ACS_USA_Regexp(reg); // Testa copy-konstruktorn
		treg = treg;
		char slusk;
		cout << endl << "Press a key";
		cin >> slusk; 
		return 0;
		
	} 
    else	if (choice == 2)
	////////////// Start test of getLastError & ReadEventLog ///////////////////
	{
		
		SetLastError(0);
//		HANDLE hndl = OpenEventLog(NULL,"Security");
		HANDLE hndl = OpenEventLog(NULL,"Application");
		if (hndl == NULL)
		{
			cout << endl << "Open EventLog Error: " << GetLastError() << endl;
			char* lpMsgBuf;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
				FORMAT_MESSAGE_FROM_SYSTEM|
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, GetLastError(), 0, 
				(LPTSTR)&lpMsgBuf, 0, NULL);
			
			cout << endl << "Windows returns: " << lpMsgBuf << endl; 
			//	error.setErrorText((char*)lpMsgBuf); //?
			LocalFree(lpMsgBuf); //?
			exit(-1);  // Drop Dead...
		}
//		else cout << hndl <<endl;
		
		LPVOID lpBuffer = 0;
		DWORD dwThisRecord =0;
		EVENTLOGRECORD* logrec;
		BYTE sizeBuf[4096];  // Is 4kb always enough for an event?
		logrec = (EVENTLOGRECORD*) &sizeBuf;
		DWORD bytesRead, bytesNext, bytesNextTmp;
		BOOL ok;
		DWORD recordNumber = 1; // First log record in chronological order
		DWORD lastRecordNumber = 0;
		PDWORD ptrNumberOfRecords = new DWORD(0);
		
		ok = GetNumberOfEventLogRecords(hndl, ptrNumberOfRecords);
		if (!ok) 
		{
			cout << endl << "GetNumberOfLogRecords Error: " << GetLastError() << endl;
		}
		
		// Last log record in chronological order
        lastRecordNumber = *ptrNumberOfRecords;  
		
		// If the log is to be read from the "top" (latest first)
		// set "recordNumber=lastRecordNumber", and do 
		// "recordNumber--" instead of "recordNumber++" after 
		// each read event record.
		recordNumber=lastRecordNumber; //read from the top
		while (1)
		{ 
			
			// First, find out how long next event record to read is (bytesNext)
			ok = ReadEventLog(hndl,EVENTLOG_SEEK_READ|EVENTLOG_FORWARDS_READ,
				recordNumber, logrec, 0, &bytesRead, &bytesNext); 
			
			// "EVENTLOG_BACKWARDS_READ" is only here for syntactical resons. 
			// It does not do any good when "EVENTLOG_SEEK_READ" is specified. 
			// Instead forward/backward reading is controlled by the 
			// recordNumber variable, which is increased or decreased depending 
			// on forward or backward read, respectively.
			
			// Second, read the event record
			ok = ReadEventLog(hndl,EVENTLOG_SEEK_READ|EVENTLOG_FORWARDS_READ,
				recordNumber, logrec, bytesNext, &bytesRead, &bytesNextTmp);
			
//			recordNumber++; // Step forward (++) in log, in chronological order	
			recordNumber--; //read from the top
			
//			cout <<endl << "OK?: " << ok << endl;
			
			if (!ok) 
			{
				cout << endl << "Error: " << GetLastError() << endl;
			}
			else
			{
//				cout << endl << "EventLogRecord: " << flush;
				printf("\nEventLogRecord:\t%02d \n", dwThisRecord++);

//				cout << "TimeGenerated: "<<logrec->TimeGenerated << endl;
				//Print Time Generated in nice format
				FILETIME FileTime, LocalFileTime;
				SYSTEMTIME SysTime;
				__int64 lgTemp;
				__int64 SecsTo1970 = 116444736000000000;

				lgTemp = Int32x32To64(logrec->TimeGenerated,10000000) + SecsTo1970;

				FileTime.dwLowDateTime = (DWORD) lgTemp;
				FileTime.dwHighDateTime = (DWORD)(lgTemp >> 32);

				FileTimeToLocalFileTime(&FileTime, &LocalFileTime);
				FileTimeToSystemTime(&LocalFileTime, &SysTime);

				printf("Date: \t\t%02d/%02d/%02d\nTime: \t\t%02d:%02d:%02d\n",
					SysTime.wMonth,
					SysTime.wDay,
					SysTime.wYear,
					SysTime.wHour,
					SysTime.wMinute,
					SysTime.wSecond);

				//Print Event Type
				cout << "Event type: \t" << flush;
				switch(logrec->EventType)
				{
					case EVENTLOG_SUCCESS:
						printf("Success\n");
						break;
					case EVENTLOG_ERROR_TYPE:
						printf("Error\n");
						break;
					case EVENTLOG_WARNING_TYPE:
						printf("Warning\n");
						break;
					case EVENTLOG_INFORMATION_TYPE:
						printf("Information\n");
						break;
					case EVENTLOG_AUDIT_SUCCESS:
						printf("Success audit\n");
						break;
					case EVENTLOG_AUDIT_FAILURE:
					    printf("Failure audit\n");
					    break;
					default:
					    printf("Unknown\n");
					    break;
				}

				printf ("Event ID: \t%05u \n", (WORD)logrec->EventID);

				LPSTR sourceName = (LPSTR)((LPBYTE) logrec + sizeof(EVENTLOGRECORD));
//				cout << endl << "length of sourceName: " << strlen(sourceName);
				LPSTR computer = (LPSTR)((LPBYTE) sourceName + strlen(sourceName) +1);// +1 for the ending '\0'
				printf ("Source: \t%s \nComputer: \t%s\n", sourceName, computer);
//				printf ("EventType: %d Source: %s Computer: %s\n", logrec->EventType,
//					sourceName, computer);

				//Get User
				PSID lpSid;
				char szName[256];
				char szDomain[256];
				SID_NAME_USE snu;
				DWORD cbName = 256;
				DWORD cbDomain = 256;

				// Point to the SID. 
				lpSid = (PSID)((LPBYTE) logrec + logrec->UserSidOffset); 

				if (LookupAccountSid(NULL, lpSid, szName, &cbName, szDomain,
					&cbDomain, &snu))
				{
					cout << "User: \t\t" << szDomain << "\\" << szName << endl;
				}
				else
				{
 					cout << "User: \t\tN/A" << endl;
				}

				//Print Category	
				printf("Category: \t%d \n", logrec->EventCategory);

				//Print Description, if any
				char* lpMsgBuf;
				LPSTR argument = (LPSTR)((LPBYTE) logrec + logrec->StringOffset);
				//LPSTR nextArgument;
				DWORD argumentOffset = strlen(argument)+1;
				DWORD numStrings = logrec->NumStrings;
				LPSTR* argsBuffer = new LPSTR[numStrings];

				argsBuffer[0] = argument;
				for (int i=1; i<(int)numStrings; i++)
				{
					argument = (LPSTR)((LPBYTE)logrec + logrec->StringOffset + argumentOffset);
					argsBuffer[i] = argument;
					argumentOffset = argumentOffset + strlen(argument) +1;
				}

				DWORD lResult;
				HKEY hResultKey; 
				HMODULE hModule;
				char subKey[100];
				DWORD eventMessageFileSize;
				unsigned char* eventMessageFile = new unsigned char[eventMessageFileSize];

				strcpy(subKey, "SYSTEM\\CurrentControlSet\\Services\\Eventlog\\Application\\");
				strncat(subKey, sourceName, strlen(sourceName));

				lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ, &hResultKey);
				if (lResult == ERROR_SUCCESS){

					TCHAR* lpValueName = "EventMessageFile";
					lResult = RegQueryValueEx(hResultKey, lpValueName, NULL, NULL, eventMessageFile, &eventMessageFileSize);
					if (lResult == ERROR_SUCCESS){
						hModule = LoadLibrary((LPCSTR) eventMessageFile);
						if (hModule != NULL)
						{
							if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
								FORMAT_MESSAGE_FROM_HMODULE|
								FORMAT_MESSAGE_FROM_SYSTEM|
								FORMAT_MESSAGE_ARGUMENT_ARRAY,
								hModule, logrec->EventID, 0, 
								(LPTSTR)&lpMsgBuf, 0, argsBuffer) !=0)
							{
								cout << endl << "Description:\n" << lpMsgBuf << endl; 
							}

							FreeLibrary(hModule);
						}
					}
				}
				LocalFree(lpMsgBuf); //?
				delete [] eventMessageFile;
				delete [] argsBuffer;
			}
			cout << endl << "Read again? (y/n)>" << flush;
			char ans;
			cin >> ans;
			if (ans == 'n' || ans == 'N') break; 
			
		} // while
		CloseEventLog(hndl);
		return 0;
	}

	else if (choice == 3)
		////////// Här börjar test av skrivning till event-loggen //////////////////
	{
		
		cout << endl << "Choose;" << endl << "(e)vent log - single event" << endl << endl <<"(c)ontinious logging to application log" << endl << endl <<"e(x)it  >" << flush;
				
        char in1, in2; 
		char* log = 0;

		cin >> in1;

		if(in1 == 'e' || in1 =='c') log  = "application";
//		else if (in1 == 'x') 
		else
		{
			exit(0);
		}

in2 = 'i';  // for test 990326

//
// 		if (in1 == 's') log = "system";                 // Choose log

        HANDLE sourceHandl;                           // Handle to source name
		int in8 = 1;

		if (in1 == 'c')
		{
		    sourceHandl = RegisterEventSource(NULL, "USAtest_APP");
			cout << endl << "How many events shall be " << endl << "written in the Application log? >" << flush;
			cin >> in8;     
		}

		while (in8)
		{
			HANDLE hndl = OpenEventLog(NULL,log); // 
			if (hndl == NULL)
			{
				cout << endl << "Open EventLog error: " << GetLastError() << endl;
				Sleep(3000);  // Milliseconds on NT
				exit(-1);  // Drop Dead...
			}

			if (in1 == 'c' ) 
				in8--; // Decrease number of rounds in loop left



		if (in1 == 'e' )                              // Chose event
		{ 
			cout << endl << "Choose event;" << endl << "(d)ummy_APP" << endl << "(f)aked_APP" << endl << "(s)ervice Control Manager (syslog) " << endl << "(i) I8042prt (syslog)" << endl << "(c)reate your own event" << endl << "e(x)it >"<< flush;
			cin >> in2;     
			if (in2 == 'd') 
				sourceHandl = RegisterEventSource(NULL, "dummy_APP"); 
			else  if  (in2 == 'f')         
				sourceHandl = RegisterEventSource(NULL, "faked_APP");
			else  if  (in2 == 's')
				sourceHandl = RegisterEventSource(NULL, "Service Control Manager");

			else  if  (in2 == 'c') {createUserDefinedEvent(); continue;}
			else if  (in2 == 'i')
				sourceHandl = RegisterEventSource(NULL, "I8042prt"); 
			//  Ends up in the systemlog
			else
				exit(0);
		}


		char* msgArray [1];
        char** msgPointer =  &msgArray[0];

		if (!ReportEvent(sourceHandl, EVENTLOG_ERROR_TYPE, 1, 1, NULL, 0,
			0, (const char**)msgPointer, NULL)) 
		{
			int errorcode = GetLastError();
			cout << endl << "could't report event, NT ERRORCODE: " << errorcode << endl;
		}
		else cout << endl << "Event written to Log"  << endl;
	    
		CloseHandle(sourceHandl);
		CloseEventLog(hndl);
	//	Sleep (25000); // 25 sec

	} // While
		return 0;
	}
	return 0;
	
}