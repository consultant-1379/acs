//******************************************************************************
// File for basic test of Reg Exp functionality on NT
//******************************************************************************


// This file (and its header file) is used in the NT version of USA only.
// The Unix version of USA uses the files regExpTest_NT.C/H instead.

//#ifdef _WIN32  // NT

#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <stdio.h>

#include <rw/rwerr.h>  // For handling RW Exceptions
//#include <rw/compiler.h>

#include <rw/cstring.h>
#include <rw/re.h>
//#include <rw/regexp.h> // On NT, support for regular expressions can be found in RW only.

#include "regExpTest_NT.H" // identical to (apart form every types )"ACS_USA_Regexp_NT.H"

#include <windows.h>
#include <winbase.h>


//******************************************************************************
//	ACS_USA_Regexp()
//******************************************************************************
ACS_USA_Regexp::ACS_USA_Regexp()
    : compiled(0),
      status(Status_notCompiled)  // INitieringen av regLength(0) borttagen 981023
{
}  

//******************************************************************************
//	ACS_USA_Regexp() - Copy constructor
//******************************************************************************
ACS_USA_Regexp::ACS_USA_Regexp(const ACS_USA_Regexp& other)
    : compiled(0),
      status(other.status)      // INitieringen av regLength(0) borttagen 981023
{
    // If the expression is compiled make a copy of the regexp

	if (status == Status_compiled) 
	{   // Use RWCRExpr copy constructor:
		compiled = new RWCRExpr(*(other.compiled));  
	}	
}

//******************************************************************************
//	~ACS_USA_Regexp()
//******************************************************************************
ACS_USA_Regexp::~ACS_USA_Regexp()
{
   delete compiled; 
}


//******************************************************************************
//	compilePattern
//******************************************************************************
ACS_USA_StatusType
ACS_USA_Regexp::compilePattern(const RWCString& pattern)
{
    // "Compile" (construct) the regular expression.

    // Construct a regexp from incoming pattern
	RWCRExpr* re = new RWCRExpr(pattern.data()); // OK! Survives outside of the method.
	//RWCRegexp re(pattern.data());   

	if (re->status() == RWCRExpr::OK)
	{
		compiled = re; 
		
RWCString teststr("Goomorron, gooomorron! De e Veiron i ottan!");
//cout << endl << "teststr: " <<teststr <<endl<<"First test of regexp, i.e. *compiled : " <<teststr(*compiled) <<endl;

	}
	else
    {		// failed
	//	error.setError(ACS_USA_SyntaxError);
	//	error.setErrorText(Regex_prefix, Regex_failed, re.status());
cout << "ERROR: compilePattern, couldn't create regexp" << endl;
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
    // Something may be wrong
    if (status == Status_notCompiled) {
	//	error.setError(ACS_USA_BadState);
	//	error.setErrorText(Regexp_BadState);
cout << "ERROR: checkMatch(), status = not Compiled" << endl;
		return ACS_USA_Regexp_error;
    }
      
	RWCString recordstr(record);  // Construct an RWCString from the incoming record (char*) 
	try
	{    // check if the string (record) matches the RE 
		if (recordstr.index(*compiled,(size_t)0) != RW_NPOS)  // Does the record match the Regexp?
		{
//			cout << endl << endl << "recordstr: " <<recordstr <<endl<<"Second test of regexp, i.e. *compiled : " <<recordstr(*compiled) <<endl;
			return ACS_USA_Regexp_match;
		}
	}			
	
	catch (RWxmsg& e)    // RWInternalErr also OK.
	{
		cout << "RW-exception fångat i checkMatch 1: "  << endl << e.why() << endl;
	}
    return ACS_USA_Regexp_noMatch;
}


//***************************************************************************
//	checkMatch()
//******************************************************************************
Regexp_MatchType
ACS_USA_Regexp::checkMatch(
			const ACS_USA_RecordType record,
			ACS_USA_ByteCount* unMatched) const
{
    // Something may be wrong
    if (status == Status_notCompiled) 
	{
	//	error.setError(ACS_USA_BadState);
	// error.setErrorText(Regexp_BadState);
cout << "ERROR: checkMatch(), status = not Compiled" << endl;
		return ACS_USA_Regexp_error;
    }

	// Construct an RWCString from the incoming record (char*)
	RWCString recordstr(record);  
	
	size_t *matchlength = new size_t;
	size_t matchstart;

	// Check if incoming record matches the RE
	matchstart = recordstr.index(*compiled,matchlength,(size_t)0);
	if (matchstart != RW_NPOS)
	{
		// Length of match + start of match=1:st non-matching char after matched text
		*unMatched = *matchlength + matchstart; 
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
    // Something may be wrong
    if (status == Status_notCompiled) 
	{
	//	error.setError(ACS_USA_BadState);
	//	error.setErrorText(Regexp_BadState);
		cout << "ERROR: checkMatch(), status = not Compiled" << endl;
		return ACS_USA_Regexp_error;
    }

	// Construct an RWCString from the incoming record (char*)
	RWCString recordstr(record);  
	
	size_t *matchlength = new size_t;
	size_t matchstart;

	// Check if incoming record matches the RE
	try 
	{
		matchstart = recordstr.index(*compiled,matchlength,(size_t)0);
//		cout << endl << endl << "recordstr: " <<recordstr <<endl<<"Forth test of regexp, i.e. *compiled : " <<recordstr(*compiled) <<endl;
	}
	catch (...)//(RWxmsg& e)
	{
	cout << endl << "Exception" << endl;
	//	cout << "RW-exception fångat i checkMatch 3 (): "  << endl << e.why() << endl;
	return ACS_USA_Regexp_noMatch;
	}

	if (matchstart != RW_NPOS)
	{
		*begin = matchstart;        // First char matched
		*extent = *matchlength;     // Length of match 
		delete matchlength;         // Seems safe? 
    	return ACS_USA_Regexp_match;
    }

	delete matchlength;         // Safe?
    return ACS_USA_Regexp_noMatch;
}


//******************************************************************************
//	operator =
//******************************************************************************
const ACS_USA_Regexp&
ACS_USA_Regexp::operator=(const ACS_USA_Regexp& other)
{
    if (this != &other)     // if not myself
	{			
		status = other.status;
cout << endl << "inregexp != utregexp" << endl;
		if (status == Status_compiled) {
			delete compiled;

            // Use RWCRExpr copy constructor:
			compiled = new RWCRExpr(*(other.compiled));   
		}
    }
else cout << endl << "inregexp == utregexp" << endl;
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

	cout << endl << "Because of cin problems handling spaces in strings:"<< flush;
	cout << endl << "If you enter SCM, the Source for the event will be Service Control Manager" << flush;
	cout << endl << "If you enter APOP, the Source for the event will be Application Popup" << flush;

	cout << endl << endl << "Enter the Source for the event > " << flush;
	cin >> choice;

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
	//char descriptionText[300] = "";
	char * descriptionText = " ";

	cout << endl << "How many words do you want the Description field of the event to contain > " << flush;
	int noOfWords = 1;
	cin >> noOfWords;

//	if (noOfWords == 1) strcpy(descriptionText," ");

	int tmpint = 0;
	char* msgArray [100];
	if (noOfWords>0)
	{   
		int i = 0;
		for (i=noOfWords; i>0; i--)
		{
			cout << endl << "Enter a word to be written in the event > " << flush;
			(char*)descriptionText = new char [300];
			cin >> descriptionText;

			msgArray[tmpint++] = descriptionText;
		}
	}
	
//	char* msgArray [1];
//	strcpy(msgArray[0], descriptionText);
	//msgArray[0]="test";
	char** msgPointer =  &msgArray[0];
	
	if (!ReportEvent(sourceHandl, eventType, 0, eventId, 0, noOfWords,
		0, (const char**)msgPointer, NULL)) 
	{
		int errorcode = GetLastError();
		cout << endl << "could't report event, NT ERRORCODE: " << errorcode << endl;
	}
	else cout << endl << "Event written to the event log"  << endl;
	
	if (noOfWords !=0 ) delete descriptionText; 

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
	if (choice == 1)   // Här börjar Regexp-test
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
RWCString pattern = "[0-9]+ [Ww][Oo][Rr][Kk][Ss][Tt][Aa][Tt][Ii][Oo][Nn] EVENTLOG_ERROR_TYPE";
RWCString theText = " Mon Jul 10 19:52:01 1970 3422 UPC7026 258 89 workStatioN EVENTLOG_ERROR_TYPE 4.0 1381";	



	
	/*	if (intkn1 != "q") {}
		else {pattern = intkn1;}
		if (intkn2 != "q") {}
		else {theText = intkn2;}
		*/

		cout << endl << "pattern: "  << pattern << endl << endl << "text: " << '\t' << theText << endl;

		
	//	RWCString pattern = "[U-Z][a-z]*on";
//		RWCString theText = "Goomorron, gooomorron! De eVeironQ i ottan!";
		
		ACS_USA_StatusType  answer = reg.compilePattern(pattern);
		cout << endl << "compilePattern returnerar (1=ok) : " << answer  << endl;
		
		Regexp_MatchType matchresult;
		
		// Testcase 1, checkMatch 1
		try
		{
			matchresult = reg.checkMatch((char* const)theText.data());	
		}
		catch(...)
		{
			cout << "exception fångat i main()" << endl;	 
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
		catch(RWxmsg& e)    // RWInternalErr also OK to catch.
		{
			cout << "RW-exception fångat i main(): "  << endl << e.why() << endl;	 
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
		catch(RWxmsg& e)    // RWInternalErr also OK to catch.
		{
			cout << "RW-exception fångat i main(): "  << endl << e.why() << endl;	 
		}
		
		cout << endl << "Startposition: " << *start << ",  matchad längd: " << *length << endl << endl;
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
	////////////// Här börjar test av getLastError & ReadEventLog ///////////////////
	{
		
		SetLastError(0);
		HANDLE hndl = OpenEventLog(NULL,"System");
		if (hndl == NULL)
		{
			cout << endl << "Open EventLog-Felet: " << GetLastError() << endl;
			char* lpMsgBuf;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
				FORMAT_MESSAGE_FROM_SYSTEM|
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, GetLastError(), 0, 
				(LPTSTR)&lpMsgBuf, 0, NULL);
			
			cout << endl << "NT returns: " << lpMsgBuf << endl; 
			//	error.setErrorText((char*)lpMsgBuf); //?
			LocalFree(lpMsgBuf); //?
			exit(-1);  // Drop Dead...
		}
		else cout << hndl <<endl;
		
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
			cout << endl << "GetNumberOfLogRecords-Felet: " << GetLastError() << endl;
		}
		
		// Last log record in chronological order
        lastRecordNumber = *ptrNumberOfRecords;  
		
		// If the log is to be read from the "top" (latest first)
		// set "recordNumber=lastRecordNumber", and do 
		// "recordNumber--" instead of "recordNumber++" after 
		// each read event record.
		
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
			
			recordNumber++; // Step forward (++) in log, in chronological order	
			
			cout <<endl << "OK?: " << ok << endl;
			
			if (!ok) 
			{
				cout << endl << "Felet: " << GetLastError() << endl;
			}
			else
			{
				cout <<endl<< "EventLogRecord: " << flush;
				cout << "TimeGenerated: "<<logrec->TimeGenerated << endl;
				printf ("%02d Event ID: %08d ", dwThisRecord++, logrec->EventID);
				LPSTR sourceName = (LPSTR)((LPBYTE) logrec + sizeof(EVENTLOGRECORD));
				cout << endl << "length of sourceName: " << strlen(sourceName);
				LPSTR computer = (LPSTR)((LPBYTE) sourceName + strlen(sourceName) +1);// +1 for the ending '\0'
				printf ("EventType: %d Source: %s Computer: %s\n", logrec->EventType,
					sourceName, computer);
				
			}
			cout << endl << "Read again? (y/n)>" << flush;
			char ans;
			cin >> ans;
			
			if (ans == 'n' || ans == 'N') break; 
			
		} // while
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
		else if (in1 == 'x') 
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

			if (in1 == 'c' ) in8--; // Decrease number of rounds in loop left



		if (in1 == 'e' )                              // Chose event
		{ 
			cout << endl << "Choose event;" << endl << "(d)ummy_APP" << endl << "(f)aked_APP" << endl << "(s)ervice Control Manager (syslog) " << endl << "(i) I8042prt (syslog)" << endl << "(c)reate your own event >"<< flush;
			cin >> in2;     
			if (in2 == 'd') 
				sourceHandl = RegisterEventSource(NULL, "dummy_APP"); 
			else  if  (in2 == 'f')         
				sourceHandl = RegisterEventSource(NULL, "faked_APP");
			else  if  (in2 == 's')
				sourceHandl = RegisterEventSource(NULL, "Service Control Manager");

			else  if  (in2 == 'c') {createUserDefinedEvent(); continue;}
			else
				sourceHandl = RegisterEventSource(NULL, "I8042prt"); 

			//  Ends up in the systemlog 
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

//#endif  // _WIN32


