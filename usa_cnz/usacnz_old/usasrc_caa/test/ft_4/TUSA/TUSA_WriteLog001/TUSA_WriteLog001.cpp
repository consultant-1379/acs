
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <winbase.h>

using namespace std;

BOOL 
TUSALogEvent(char* lpSource, long lEventID, unsigned short lType, unsigned short lStringNo, const char** lplpStrings)
{
	HANDLE hSource;
	BOOL rcBool;
	Sleep(2000);

	hSource = RegisterEventSource(
		NULL,			// Server name for source
		lpSource		// Source name for registered handle
		);
	if (hSource == NULL)
	{
		cout << "ERROR : " << GetLastError() << endl;
		return false;
	};

	rcBool = ReportEvent(
		hSource, 	    // Handle returned by RegisterEventSource
		lType,          // Event type to log
		0, 				// Event category
		lEventID, 		// Event identifier
		NULL, 			// User security identifier (optional)
		lStringNo, 		// Number of strings to merge with message
		0, 				// Size of binary data, in bytes
		lplpStrings, 	// Array of strings to merge with message
		NULL			// Address of binary data
		);
	if (!rcBool)
	{
		cout << "ERROR : "<< GetLastError() << endl;
		return false;
	};

	rcBool = DeregisterEventSource(
		hSource			// Handle to event log
		);
	if (!rcBool)
	{
		cout << "ERROR : " << GetLastError() << endl;
		return false;
	};
	return true;
}


void 
main()
{
	LPCTSTR a;
	LPCTSTR*b=&a;
	a="Tjenna!\n";
	char z;
	
/* USA will trigger on same criteria in the criteria file. 
A new AP Event only if the time between the events is more than 60 seconds.

	cout << "Alerter Error: USA should trigger. " << endl;
	TUSALogEvent("Alerter", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "AM-Service Error: USA should trigger." << endl;
	TUSALogEvent("AM-Service", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "Diskeeper Error: USA should trigger." << endl;
	TUSALogEvent("Diskeeper", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "EventLog Error: USA should trigger." << endl;
	TUSALogEvent("EventLog", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << endl;
	cout << "Press ENTER to continue. ";
	cin.get(z);
	
	cout << "MSFTPSVC Error: USA should trigger." << endl;
	TUSALogEvent("MSFTPSVC", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "Inetd Error: USA should trigger." << endl;
	TUSALogEvent("Inetd", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "LBBAM_Service Error: USA should trigger." << endl;
	TUSALogEvent("LBBAM_Service", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "NetLogon Error: USA should trigger." << endl;
	TUSALogEvent("NetLogon", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << endl;
	cout << "Press ENTER to continue. ";
	cin.get(z);
	
	cout << "NetDDE Error: USA should trigger." << endl;
	TUSALogEvent("NetDDE", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "pcANYWHERE32 Error: USA should trigger." << endl;
	TUSALogEvent("pcANYWHERE32", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "Replicator Error: USA should trigger." << endl;
	TUSALogEvent("Replicator", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "server Error: USA should trigger." << endl;
	TUSALogEvent("server", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "SNMP Error: USA should trigger." << endl;
	TUSALogEvent("SNMP", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "LmHosts Error: USA should trigger." << endl;
	TUSALogEvent("LmHosts", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << endl;
	cout << "Press ENTER to continue. ";
	cin.get(z);
	
	cout << "workstation Error: USA should trigger." << endl;
	TUSALogEvent("workstation", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "Afd Error: USA should trigger." << endl;
	TUSALogEvent("Afd", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "aic78u2 Error: USA should trigger." << endl;
	TUSALogEvent("aic78u2", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "atapi Error: USA should trigger." << endl;
	TUSALogEvent("atapi", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "DC21X4 Error: USA should trigger." << endl;
	TUSALogEvent("DC21X4", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << endl;
	cout << "Press ENTER to continue. ";
	cin.get(z);
	
	cout << "disk Error: USA should trigger." << endl;
	TUSALogEvent("disk", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "E100B Error: USA should trigger." << endl;
	TUSALogEvent("E100B", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "ndis Error: USA should trigger." << endl;
	TUSALogEvent("ndis", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "msfs Error: USA should trigger." << endl;
	TUSALogEvent("msfs", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "Mup Error: USA should trigger." << endl;
	TUSALogEvent("Mup", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << endl;
	cout << "Press ENTER to continue. ";
	cin.get(z);
	
	cout << "NetBIOS Error: USA should trigger." << endl;
	TUSALogEvent("NetBIOS", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "bh Error: USA should trigger." << endl;
	TUSALogEvent("bh", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "npfs Error: USA should trigger." << endl;
	TUSALogEvent("npfs", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "ntfs Error: USA should trigger." << endl;
	TUSALogEvent("ntfs", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "Rdr Error: USA should trigger." << endl;
	TUSALogEvent("Rdr", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << endl;
	cout << "Press ENTER to continue. ";
	cin.get(z);
	
	cout << "scsiprnt Error: USA should trigger." << endl;
	TUSALogEvent("scsiprnt", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "scsiscan Error: USA should trigger." << endl;
	TUSALogEvent("scsiscan", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "serial Error: USA should trigger." << endl;
	TUSALogEvent("serial", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "sfloppy Error: USA should trigger." << endl;
	TUSALogEvent("sfloppy", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "Srv Error: USA should trigger." << endl;
	TUSALogEvent("Srv", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << endl;
	cout << "Press ENTER to continue. ";
	cin.get(z);
	
	cout << "Tcpip Error: USA should trigger." << endl;
	TUSALogEvent("Tcpip", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "NetBT Error: USA should trigger." << endl;
	TUSALogEvent("NetBT", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "Tdi Error: USA should trigger." << endl;
	TUSALogEvent("Tdi", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "Perfctrs Error: USA should trigger." << endl;
	TUSALogEvent("Perfctrs", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "Perflib Error: USA should trigger." << endl;
	TUSALogEvent("Perflib", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << endl;
	cout << "Press ENTER to continue. ";
	cin.get(z);
	
	cout << "Perfmon Error: USA should trigger." << endl;
	TUSALogEvent("Perfmon", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "TelmSrvc Error: USA should trigger." << endl;
	TUSALogEvent("TelmSrvc", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << "TelnetD Error: USA should trigger." << endl;
	TUSALogEvent("TelnetD", 1, EVENTLOG_ERROR_TYPE, 0, b);
	
	cout << endl;
	cout << "Press ENTER to continue. ";
	cin.get(z);	
*/
/* A1, will cause failover and is therefore removed
	cout << "DHCPServer Error: USA should trigger." << endl;
	TUSALogEvent("DHCPServer", 1001, EVENTLOG_ERROR_TYPE, 0, b);
*/	
	cout << "eTrust Antivirus Warning (Scan Job...): USA should trigger." << endl;
	a="Scan Job x had the following results: y";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_WARNING_TYPE, 1, b);

	cout << "eTrust Antivirus Warning (An infection was detected...): USA should trigger." << endl;
	a="An infection was detected x your computer's memory. Please scan your computer immediately!";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_WARNING_TYPE, 1, b);

	cout << "eTrust Antivirus Error (Virus Wall Failed. The original...): USA should trigger." << endl;
	a="Virus Wall Failed. The original x is not restored";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_ERROR_TYPE, 1, b);

	cout << "eTrust Antivirus Error (Scan Job x had the following...): USA should trigger." << endl;
	a="Scan Job x had the following results";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_ERROR_TYPE, 1, b);

	cout << endl;
	cout << "Press ENTER to continue. ";
	cin.get(z);	

	cout << "eTrust Antivirus Error (The x virus was detected...): USA should trigger." << endl;
	a="The x virus was detected in y";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_ERROR_TYPE, 1, b);

	cout << "eTrust Antivirus Error (Virus Wall invoked. The original...): USA should trigger." << endl;
	a="Virus Wall invoked. The original x is restored";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_ERROR_TYPE, 1, b);

	cout << "eTrust Antivirus Error (x failed to initiate...): USA should trigger." << endl;
	a="x failed to initiate - 0";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_ERROR_TYPE, 1, b);

	cout << "eTrust Antivirus Error (Error creating...): USA should trigger." << endl;
	a="Error creating the scan view -";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_ERROR_TYPE, 1, b);

	cout << "eTrust Antivirus Error (Error Scanning Memory!): USA should trigger." << endl;
	a="Error Scanning Memory!";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_ERROR_TYPE, 1, b);

	cout << endl;
	cout << "Press ENTER to continue. ";
	cin.get(z);	

	cout << "eTrust Antivirus Error (An internal error has...): USA should trigger." << endl;
	a="An internal error has occurred while scanning x";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_ERROR_TYPE, 1, b);

	cout << "eTrust Antivirus Error (Failed to load...): USA should trigger." << endl;
	a="Failed to load x";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_ERROR_TYPE, 1, b);

	cout << "eTrust Antivirus Error (Failed to add...): USA should trigger." << endl;
	a="Failed to add a boot job";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_ERROR_TYPE, 1, b);

	cout << "eTrust Antivirus Error (Failed to copy...): USA should trigger." << endl;
	a="Failed to copy the signature file from x to y";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_ERROR_TYPE, 1, b);

	cout << "eTrust Antivirus Error (Failed to create...): USA should trigger." << endl;
	a="Failed to create the scan log database";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_ERROR_TYPE, 1, b);

	cout << endl;
	cout << "Press ENTER to continue. ";
	cin.get(z);	

	cout << "eTrust Antivirus Error (Failed to initialize the InoScan.dll): USA should trigger." << endl;
	a="Failed to initialize the InoScan.dll";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_ERROR_TYPE, 1, b);

	cout << "eTrust Antivirus Error (Failed to initialize the ScanLog.dll): USA should trigger." << endl;
	a="Failed to initialize the ScanLog.dll";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_ERROR_TYPE, 1, b);

	cout << "eTrust Antivirus Error (Failed to open...): USA should trigger." << endl;
	a="Failed to open the scan log database";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_ERROR_TYPE, 1, b);

	cout << "eTrust Antivirus Error (Failed to retrieve...): USA should trigger." << endl;
	a="Failed to retrieve the information of a scan job";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_ERROR_TYPE, 1, b);

	cout << "eTrust Antivirus Warning (The automatic download... ): Three events, USA should trigger once." << endl;
	a="The automatic download has run 9 times unsuccessfully. The next attempt will occur at the regularly scheduled download time.";
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_WARNING_TYPE, 1, b);
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_WARNING_TYPE, 1, b);
	TUSALogEvent("eTrust Antivirus", 1, EVENTLOG_WARNING_TYPE, 1, b);
	
	cout << endl;
	cout << "Press ENTER to continue. ";
	cin.get(z);	

	a="Tjenna!\n";
	cout << "General E100B Error: USA should trigger." << endl;
	TUSALogEvent("E100B", 1, EVENTLOG_ERROR_TYPE, 0, b);

	cout << "AM-Service Error (Application log): USA should trigger." << endl;
	TUSALogEvent("AM-Service", 1, EVENTLOG_ERROR_TYPE, 0, b);

	cout << "Alerter Error (System log): USA should trigger." << endl;
	TUSALogEvent("Alerter", 1, EVENTLOG_ERROR_TYPE, 0, b);

	cout << "NTDS API Error (Directory Service log): USA should trigger." << endl;
	TUSALogEvent("NTDS API", 1, EVENTLOG_ERROR_TYPE, 0, b);

	cout << "DNS Error (DNS Server log): USA should trigger." << endl;
	TUSALogEvent("DNS", 1, EVENTLOG_ERROR_TYPE, 0, b);

	cout << "NtFrs Error (File Replication Service log): USA should trigger." << endl;
	TUSALogEvent("NtFrs", 1, EVENTLOG_ERROR_TYPE, 0, b);

	cout << endl;
	cout << "Press ENTER to continue. ";
	cin.get(z);	

  /*
  a="Adapter link down: x Adapter";
	cout << "External Ethernet Error: USA should trigger." << endl;
	TUSALogEvent("iANSMiniport", 11, EVENTLOG_WARNING_TYPE, 1, b);

	a="Adapter link down: x Adapter #2";
	cout << "External Ethernet Error: USA should trigger." << endl;
	TUSALogEvent("iANSMiniport", 11, EVENTLOG_WARNING_TYPE, 1, b);
  */

	a="Team x: The last adapter has lost link";
	cout << "External Ethernet Error: USA should trigger." << endl;
	TUSALogEvent("iANSMiniport", 16, EVENTLOG_ERROR_TYPE, 1, b);

	cout << endl;
	cout << "Press ENTER to continue. ";
	cin.get(z);	

  /*
	a="Adapter link up: x Adapter";
	cout << "External Ethernet Ceasing: USA should trigger." << endl;
	TUSALogEvent("iANSMiniport", 15, EVENTLOG_INFORMATION_TYPE, 1, b);

	a="Adapter link up: x Adapter #2";
	cout << "External Ethernet Ceasing: USA should trigger." << endl;
	TUSALogEvent("iANSMiniport", 15, EVENTLOG_INFORMATION_TYPE, 1, b);
  */

	a="Team x: An adapter has re-established link";
	cout << "External Ethernet Ceasing: USA should trigger." << endl;
	TUSALogEvent("iANSMiniport", 17, EVENTLOG_INFORMATION_TYPE, 1, b);

	cout << endl;
	cout << "Press ENTER to continue. ";
	cin.get(z);	
}