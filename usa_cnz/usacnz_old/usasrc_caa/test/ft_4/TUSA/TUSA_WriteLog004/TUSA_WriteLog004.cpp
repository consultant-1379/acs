
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
	/*
	1000,1010,
1011,1015,
1016,1021,
1034,1035,
1036,1037,
1038,1066,
1067,1077,
1080,1096,
1104,1105,
1107,1109
*/

	cout << "ClusSvc Error 1000: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1000,EVENTLOG_ERROR_TYPE,0,b);

/* USA will trigger on same criteria in the criteria file. 
A new AP Event only if the time between the events is more than 60 seconds.

	cout << "ClusSvc Error 1010: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1010,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ClusSvc Error 1011: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1011,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ClusSvc Error 1015: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1015,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ClusSvc Error 1016: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1016,EVENTLOG_ERROR_TYPE,0,b);

	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;

	cout << "ClusSvc Error 1021: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1021,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ClusSvc Error 1034: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1034,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ClusSvc Error 1035: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1035,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ClusSvc Error 1036: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1036,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ClusSvc Error 1037: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1037,EVENTLOG_ERROR_TYPE,0,b);

	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;
*/
		/*
	1000,1010,
1011,1015,
1016,1021,
1034,1035,
1036,1037,
1038,1066,
1067,1077,
1080,1096,
1104,1105,
1107,1109
*/

/*	cout << "ClusSvc Error 1038: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1038,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ClusSvc Error 1066: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1066,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ClusSvc Error 1067: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1067,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ClusSvc Error 1077: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1077,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ClusSvc Error 1080: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1080,EVENTLOG_ERROR_TYPE,0,b);

	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;

	cout << "ClusSvc Error 1096: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1096,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ClusSvc Error 1104: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1104,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ClusSvc Error 1105: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1105,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ClusSvc Error 1107: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1107,EVENTLOG_ERROR_TYPE,0,b);

	cout << "ClusSvc Error 1109: USA should trigger. "<< endl;
	TUSALogEvent("ClusSvc",1109,EVENTLOG_ERROR_TYPE,0,b);
*/

	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;

	

}