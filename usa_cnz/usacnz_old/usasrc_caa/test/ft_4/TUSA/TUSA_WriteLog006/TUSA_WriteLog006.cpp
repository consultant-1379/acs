
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <winbase.h>

using namespace std;

BOOL TUSALogEvent2(HANDLE hSource,
				  long lEventID,
				  unsigned short lType,
				  unsigned short lStringNo,
				  const char** lplpStrings){
	BOOL rcBool;
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
	return true;
}


void main()
{
	char z;
	LPCTSTR a;
	LPCTSTR*b=&a;
	char c[100];
	BOOL rcBool;

	cout << "Send number of events"<< endl << endl;

	long n;
	long id;
	cout << "Give source: " ;
	cin.getline(c, 100);
	cout << "Give event id: " ; cin >> id;
	cout << "Give number of events: " ; cin >> n;

	HANDLE hSource;
	hSource=RegisterEventSource(
	NULL,  // server name for source
	c //LPCTSTR lpSourceName      // source name for registered handle
	);
	if(hSource==NULL){
			cout << "ERROR : "<< GetLastError() << endl ;
	}else{

		for(long i=1;i<=n;i++){
	
			TUSALogEvent2(hSource,id,EVENTLOG_ERROR_TYPE,0,b);
		};
	};
	rcBool=DeregisterEventSource(
	hSource   // handle to event log
	);
	if(!rcBool){
		cout << "ERROR : "<< GetLastError() << endl ;
	};
	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;	cin.get(z) ;

	

}