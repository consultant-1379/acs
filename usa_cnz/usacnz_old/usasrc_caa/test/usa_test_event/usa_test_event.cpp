#include <iostream>
#include <windows.h>
using namespace std;
struct eventType {
  const char* source;
  WORD        type;
  DWORD       id;
  const char* parameters[1];
};
eventType event;
int main(DWORD argc, LPSTR *argv) {
  event.source = argv[1];
  event.type = atoi(argv[2]);
  event.id = atoi(argv[3]);
  event.parameters[0] = strdup(argv[4]);
  cout << "source: " << event.source << endl;
  cout << "type: " << event.type << endl;
  cout << "id: " << event.id << endl;
  cout << "parameters: " << event.parameters[0] << endl;
  HANDLE hEventLog = RegisterEventSource(NULL, event.source);
  if (hEventLog == NULL) {
    cerr << "failed to register event source" << endl;
    return 1;
  }
  BOOL ret = ReportEvent(
    hEventLog,
		event.type,
		NULL,
		event.id,
		NULL,
		1,
		0,
		event.parameters,
		NULL);
  DeregisterEventSource(hEventLog);
  if (ret == false) {
    cerr << "failed to report event" << endl;
    return 1;
  }
  return 0;
}
