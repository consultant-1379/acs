
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <winbase.h>

using namespace std;

BOOL TUSALogEvent(char * lpSource,long lEventID,unsigned short lType,unsigned short lStringNo,const char** lplpStrings){
	HANDLE hSource;
	BOOL rcBool;
	Sleep(2000);
	hSource=RegisterEventSource(
		NULL,  // server name for source
		lpSource //LPCTSTR lpSourceName      // source name for registered handle
	);
	if(hSource==NULL){
			cout << "ERROR : "<< GetLastError() << endl ;
			return false;
	};
	rcBool=ReportEvent(
		hSource,    // handle returned by RegisterEventSource
		lType,          // event type to log
		0,      // event category
		lEventID,     // event identifier
		NULL,      // user security identifier (optional)
		lStringNo,    // number of strings to merge with message
		0,    // size of binary data, in bytes
		lplpStrings,  // array of strings to merge with message
		NULL     // address of binary data
	);
	if(!rcBool){
		cout << "ERROR : "<< GetLastError() << endl ;
		return false;
	};
	rcBool=DeregisterEventSource(
	hSource   // handle to event log
	);
	if(!rcBool){
		cout << "ERROR : "<< GetLastError() << endl ;
		return false;
	};
	return true;
}


void main()
{
	LPCTSTR a;
	LPCTSTR*b=&a;
	a="Tjenna!\n";
	char z;

	cout << "Application Popup Error: USA should not trigger. "<< endl;
	TUSALogEvent("Application Popup",1,EVENTLOG_ERROR_TYPE,0,b);

	cout << "Beep Error: USA should not trigger. "<< endl;
	TUSALogEvent("Beep",1,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ClusSvc Error 1: USA should not trigger. "<< endl;
	TUSALogEvent("ClusSvc",1,EVENTLOG_ERROR_TYPE,0,b);

	cout << "i8042prt Error: USA should not trigger. "<< endl;
	TUSALogEvent("i8042prt",1,EVENTLOG_ERROR_TYPE,0,b);

	cout << "LicenseService Error: USA should not trigger. "<< endl;
	TUSALogEvent("LicenseService",1,EVENTLOG_ERROR_TYPE,0,b);

	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;

	cout << "mouclass Error: USA should not trigger. "<< endl;
	TUSALogEvent("mouclass",1,EVENTLOG_ERROR_TYPE,0,b);

	cout << "null Error: USA should not trigger. "<< endl;
	TUSALogEvent("null",1,EVENTLOG_ERROR_TYPE,0,b);

	cout << "vga Error: USA should not trigger. "<< endl;
	TUSALogEvent("vga",1,EVENTLOG_ERROR_TYPE,0,b);

	cout << "TelnetServer Error 1291: USA should not trigger. "<< endl;
	TUSALogEvent("TelnetServer",1291,EVENTLOG_ERROR_TYPE,0,b);

	cout << "VgaSave Error: USA should not trigger. "<< endl;
	TUSALogEvent("VgaSave",1,EVENTLOG_ERROR_TYPE,0,b);

	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;

	cout << "disk Error 9: USA should not trigger. "<< endl;
	TUSALogEvent("disk",9,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ethernet Information: USA should not trigger. "<< endl;
	TUSALogEvent("E100B",1,EVENTLOG_INFORMATION_TYPE,0,b);

	cout << "ethernet Warning: USA should not trigger. "<< endl;
	TUSALogEvent("E100B",1,EVENTLOG_WARNING_TYPE,0,b);

	cout << "kbdclass Error: USA should not trigger. "<< endl;
	TUSALogEvent("kbdclass",1,EVENTLOG_ERROR_TYPE,0,b);

	cout << "mrxsmb Error 8003: USA should not trigger. "<< endl;
	TUSALogEvent("mrxsmb",8003,EVENTLOG_ERROR_TYPE,0,b);

	cout << "EventLog Error 6008: USA should not trigger. "<< endl;
	TUSALogEvent("EventLog",6008,EVENTLOG_ERROR_TYPE,0,b);

	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;

	cout << "atapi Error 9: USA should not trigger. "<< endl;
	a="The device, \\Device\\ScsiPort0, did not respond within the timeout period";
	TUSALogEvent("atapi",9,EVENTLOG_ERROR_TYPE,1,b);
	a="Tjenna!\n";

	cout << "dpti Error 9: USA should not trigger. "<< endl;
	TUSALogEvent("dpti2o",9,EVENTLOG_ERROR_TYPE,0,b);

	cout << "TermServDevices Error 1114: USA should not trigger. "<< endl;
	TUSALogEvent("TermServDevices",1114,EVENTLOG_ERROR_TYPE,0,b);

	cout << "eTrust Antivirus Error: USA should not trigger. "<< endl;
	TUSALogEvent("eTrust Antivirus",1,EVENTLOG_ERROR_TYPE,0,b);

	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;

	cout << "dptelog Error 1002: USA should not trigger. "<< endl;
	TUSALogEvent("dptelog",1002,EVENTLOG_ERROR_TYPE,0,b);

	cout << "dptelog Error 1003: USA should not trigger. "<< endl;
	TUSALogEvent("dptelog",1003,EVENTLOG_ERROR_TYPE,0,b);

	cout << "w32Time Error: USA should not trigger. "<< endl;
	TUSALogEvent("w32Time",1,EVENTLOG_ERROR_TYPE,0,b);

	cout << "perflib Error 1001: USA should not trigger. "<< endl;
	a="xyz DWORD 0";
	TUSALogEvent("perflib",1001,EVENTLOG_ERROR_TYPE,1,b);
	a="Tjenna!\n";

	cout << "perflib Error 1010: USA should not trigger. "<< endl;
	TUSALogEvent("perflib",1010,EVENTLOG_ERROR_TYPE,0,b);

	cout << "UserEnv Error 1097: USA should not trigger. "<< endl;
	TUSALogEvent("UserEnv",1097,EVENTLOG_ERROR_TYPE,0,b);

	cout << "NTDS Replication Error 1864: USA should not trigger."<< endl;
	TUSALogEvent("NTDS Replication", 1864, EVENTLOG_ERROR_TYPE, 0, b);

	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;

}