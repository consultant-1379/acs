//******************************************************************************
// File for basic test of Reg Exp functionality on NT
//******************************************************************************


// This file (and its header file) is used in the NT version of USA only.
// The Unix version of USA uses the files regExpTest_NT.C/H instead.

#ifdef _WIN32  // NT

#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <stdio.h>

#include <rw/rwerr.h>  // For handling RW Exceptions
//#include <rw/compiler.h>

#include <rw/cstring.h>
#include <rw/regexp.h> // On NT, support for regular expressions can be found in RW only.

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
	{   // Use RWCRegexp copy constructor:
		compiled = new RWCRegexp(*(other.compiled));  
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
	RWCRegexp* re = new RWCRegexp(pattern.data()); // OK! Survives outside of the method.
	//RWCRegexp re(pattern.data());   

	if (re->status() == RWCRegexp::OK)
	{
		compiled = re; 
		
RWCString teststr("Goomorron, gooomorron! De e Veiron i ottan!");
cout << endl << "teststr: " <<teststr <<endl<<"First test of regexp, i.e. *compiled : " <<teststr(*compiled) <<endl;

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
			cout << endl << endl << "recordstr: " <<recordstr <<endl<<"Second test of regexp, i.e. *compiled : " <<recordstr(*compiled) <<endl;
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
		cout << endl << endl << "recordstr: " <<recordstr <<endl<<"Forth test of regexp, i.e. *compiled : " <<recordstr(*compiled) <<endl;
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

            // Use RWCRegexp copy constructor:
			compiled = new RWCRegexp(*(other.compiled));   
		}
    }
else cout << endl << "inregexp == utregexp" << endl;
    return *this;
}

//******************************************************************************
// main()
//*******************************************************


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
		
		RWCString pattern = "[U-Z][a-z]*on";
		RWCString theText = "Goomorron, gooomorron! De eVeironQ i ottan!";
		
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
		
		cout << endl << endl << "match result (2=match, 1 = no match, 0 = error) :" << matchresult << endl << endl;
		
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
		Sleep (2000);   // Wait 2 sec
		
		ACS_USA_Regexp nyreg = ACS_USA_Regexp();
		
		cout << endl << "First test of '='";
		reg = reg;  // "operator=" ska märka att regexparna är fysiskt desamma 
		cout << endl << "Second test of '='";
		nyreg = reg; // "operator=" ska märka att regexparna är olika
		
		ACS_USA_Regexp treg = ACS_USA_Regexp(reg); // Testa copy-konstruktorn
		treg = treg;
		return 0;
		
	} 
    else	if (choice == 2)
		// Här börjar test av getLastError & ReadEventLog
	{
		
		SetLastError(0);
		HANDLE hndl = OpenEventLog(NULL,"Security");
		if (hndl == NULL)
		{
			cout << endl << "Open EventLog-Felet: " << GetLastError() << endl;
			char* lpMsgBuf;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
				FORMAT_MESSAGE_FROM_SYSTEM|
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, GetLastError(), 0, 
				(LPTSTR)&lpMsgBuf, 0, NULL);
			
			char* nystr, *tmpstr;
			//	nystr = strcpy (tmpstr, (char*)lpMsgBuf);
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
		// Här börjar test av skrivning till event-loggen!
	{
		
		cout << endl << "Choose; single event in (a)pplication log" << endl <<"(c)ontinious logging to application log," << endl <<"or e(xit)  >" << flush;
				
        char in1, in2; 
		char* log = 0;

		cin >> in1;

		if(in1 == 'a' || in1 =='c') log  = "application";
		else if (in1 == 'e') 
		{
			exit(0);
		}



while (1)
		{
// 		if (in1 == 's') log = "system";                 // Choose log
	
		HANDLE hndl = OpenEventLog(NULL,log); // 
		if (hndl == NULL)
		{
			cout << endl << "Open EventLog error: " << GetLastError() << endl;
	        Sleep(3000);  // Milliseconds on NT
			exit(-1);  // Drop Dead...
		}


        HANDLE sourceHandl;                           // Handle to source name
		char* msgArray [1];

		if (in1 == 'a' )                              // Chose event
		{ 
			cout << endl << "Choose event; (d)ummy_APP or (f)aked_APP >" << flush;
			cin >> in2;     
			if (in2 == 'd') 
				sourceHandl = RegisterEventSource(NULL, "dummy_APP"); 
			else            
				sourceHandl = RegisterEventSource(NULL, "faked_APP");

		}
		else if (in1 == 'c')
		{
		    sourceHandl = RegisterEventSource(NULL, "USAtest_APP");
		}
/*		else if (in1 == 's')
		{
			cout << endl << "Choose event; (p)erfmon or (w)indows 3.1 Migration>" << flush;
            cin >> in2;     
			if (in2 == 'p') 				
				sourceHandl = RegisterEventSource(NULL, "Perfmon"); 
			else            
				sourceHandl = RegisterEventSource(NULL, "Windows 3.1 Migration");

		}
*/

        char** msgPointer =  &msgArray[0];

		if (!ReportEvent(sourceHandl, EVENTLOG_WARNING_TYPE, 1, 1, NULL, 1,
			0, (const char**)msgPointer, NULL)) 
			cout << endl << "could't report event " << endl;
		else cout << endl << "Event written to Application Log" << endl;
	    
		CloseEventLog(hndl);
		Sleep (4000); // 4 sec

	} // While
		return 0;
	}
	return 0;
	
}

#endif  // _WIN32


