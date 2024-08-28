
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <winbase.h>

using namespace std;

BOOL TUSALogEvent(char * lpSource,
				  long lEventID,
				  unsigned short lType,
				  unsigned short lStringNo,
				  const char** lplpStrings){
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
	char z;
	LPCTSTR a;
	LPCTSTR*b=&a;

	cout << "Service Control Manager Error 7024 for Beep: USA should not trigger. "<< endl;
	a="Beep";
	TUSALogEvent("Service Control Manager",7024,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7024 for ClusSvc: USA should not trigger. "<< endl;
	a="ClusSvc";
	TUSALogEvent("Service Control Manager",7024,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7024 for i8042prt: USA should not trigger. "<< endl;
	a="i8042prt";
	TUSALogEvent("Service Control Manager",7024,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7024 for Kbdclass: USA should not trigger. "<< endl;
	a="Kbdclass";
	TUSALogEvent("Service Control Manager",7024,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7024 for LicenseService: USA should not trigger. "<< endl;
	a="LicenseService";
	TUSALogEvent("Service Control Manager",7024,EVENTLOG_ERROR_TYPE,1,b);
	
	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;

	cout << "Service Control Manager Error 7024 for Mouclass: USA should not trigger. "<< endl;
	a="Mouclass";
	TUSALogEvent("Service Control Manager",7024,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7024 for Null: USA should not trigger. "<< endl;
	a="Null";
	TUSALogEvent("Service Control Manager",7024,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7024 for vga: USA should not trigger. "<< endl;
	a="vga";
	TUSALogEvent("Service Control Manager",7024,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7024 for VgaSave: USA should not trigger. "<< endl;
	a="VgaSave";
	TUSALogEvent("Service Control Manager",7024,EVENTLOG_ERROR_TYPE,1,b);

	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;

}