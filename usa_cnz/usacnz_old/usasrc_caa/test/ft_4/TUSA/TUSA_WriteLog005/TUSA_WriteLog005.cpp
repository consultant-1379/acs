
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

/* USA will trigger on same criteria in the criteria file. 
A new AP Event only if the time between the events is more than 60 seconds.

	cout << "Test that Service Control Manager supervision works for a selection of non-excluded services."<< endl;
	cout << "Teoretical new service xxx included"<< endl << endl;

	cout << "Service Control Manager Error 7024 for Alerter: USA should trigger. "<< endl;
	a="Alerter";
	TUSALogEvent("Service Control Manager",7024,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7024 for Diskeeper: USA should trigger. "<< endl;
	a="Diskeeper";
	TUSALogEvent("Service Control Manager",7024,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7024 for xxx: USA should trigger. "<< endl;
	a="xxx";
	TUSALogEvent("Service Control Manager",7024,EVENTLOG_ERROR_TYPE,1,b);

	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;

	cout << "Test that Service Control Manager supervision works for all events in group 2 and 4."<< endl;
	cout << "for non-excluded service."<< endl << endl;

	//7024,7023,7022,7014,7013,7003,7002,7000,7015

	cout << "Service Control Manager Error 7024 for xxx: USA should trigger. "<< endl;
	a="xxx";
	TUSALogEvent("Service Control Manager",7024,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7023 for xxx: USA should trigger. "<< endl;
	a="xxx";
	TUSALogEvent("Service Control Manager",7023,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7022 for xxx: USA should trigger. "<< endl;
	a="xxx";
	TUSALogEvent("Service Control Manager",7022,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7014 for xxx: USA should trigger. "<< endl;
	a="xxx";
	TUSALogEvent("Service Control Manager",7014,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7013 for xxx: USA should trigger. "<< endl;
	a="xxx";
	TUSALogEvent("Service Control Manager",7013,EVENTLOG_ERROR_TYPE,1,b);

	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;

	cout << "Service Control Manager Error 7003 for xxx: USA should trigger. "<< endl;
	a="xxx";
	TUSALogEvent("Service Control Manager",7003,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7002 for xxx: USA should trigger. "<< endl;
	a="xxx";
	TUSALogEvent("Service Control Manager",7002,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7000 for xxx: USA should trigger. "<< endl;
	a="xxx";
	TUSALogEvent("Service Control Manager",7000,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7015 for xxx: USA should trigger. "<< endl;
	a="xxx";
	TUSALogEvent("Service Control Manager",7015,EVENTLOG_ERROR_TYPE,1,b);

	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;

	cout << "Test that Service Control Manager supervision works for all events in group 3."<< endl << endl ;

	//7026,7016,7008,7006,7005,7001

	cout << "Service Control Manager Error 7026 : USA should trigger. "<< endl;
	a="Mouclass";
	TUSALogEvent("Service Control Manager",7026,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7016 : USA should trigger. "<< endl;
	a="Mouclass";
	TUSALogEvent("Service Control Manager",7016,EVENTLOG_ERROR_TYPE,1,b);

	//insertion strings not permitted.
	cout << "Service Control Manager Error 7008 : USA should trigger. "<< endl;
	TUSALogEvent("Service Control Manager",7008,EVENTLOG_ERROR_TYPE,0,b);
	
	cout << "Service Control Manager Error 7006 : USA should trigger. "<< endl;
	a="Mouclass";
	TUSALogEvent("Service Control Manager",7006,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7005 : USA should trigger. "<< endl;
	a="Mouclass";
	TUSALogEvent("Service Control Manager",7005,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7001 : USA should trigger. "<< endl;
	a="Mouclass";
	TUSALogEvent("Service Control Manager",7001,EVENTLOG_ERROR_TYPE,1,b);


	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;
*/
/*A1, will cause failover and is therefore removed
	cout << "Service Control Manager Error 7024 for DistinctBootpService: USA should trigger. "<< endl;
	a="DistincBootpService";
	TUSALogEvent("Service Control Manager",7024,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7024 for DistinctFTPService: USA should trigger. "<< endl;
	a="DistincFTPService";
	TUSALogEvent("Service Control Manager",7024,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7024 for Inetd: USA should trigger. "<< endl;
	a="Inetd";
	TUSALogEvent("Service Control Manager",7026,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7027 : USA should trigger. "<< endl;
	a="";
	TUSALogEvent("Service Control Manager",7027,EVENTLOG_ERROR_TYPE,0,b);
*/
	cout << "Service Control Manager Error 7026 for Mouclass x: USA should trigger. "<< endl;
	a="Mouclass x";
	TUSALogEvent("Service Control Manager",7026,EVENTLOG_ERROR_TYPE,1,b);

	cout << "Service Control Manager Error 7024 for xxx: USA should trigger. "<< endl;
	a="xxx";
	TUSALogEvent("Service Control Manager",7024,EVENTLOG_ERROR_TYPE,1,b);

	cout << endl ;
	cout << "Press ENTER to continue. " ;
	cin.get(z) ;


}