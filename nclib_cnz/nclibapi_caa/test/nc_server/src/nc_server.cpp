#include "acs_nclib_session.h"
#include "acs_nclib_message.h"
#include "acs_nclib_hello.h"
#include "acs_nclib_rpc.h"
#include "acs_nclib_factory.h"
#include "acs_nclib_library.h"
#include "stdio.h"
#include "stdlib.h"

static const char FILTER_EXAMPLE[] = "<ManagedElement xmlns=\"urn:com:ericsson:ecim:ComTop\">"
"\n<managedElementId>1</managedElementId>"
"\n<SystemFunctions>"
"\n<systemFunctionsId>1</systemFunctionsId>"
"\n<Fm xmlns=\"urn:com:ericsson:ecim:ComFm\">"
"\n<fmId>1</fmId>"
 "\n<lastSequenceNo/>"
"\n</Fm>"
"\n</SystemFunctions>"
"\n</ManagedElement>\n";


static char HELLO[] = "<hello xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
"<capabilities>"
"<capability>urn:ietf:params:netconf:base:1.0</capability>"
"</capabilities>"
		"<session-id>23</session-id>"
"</hello>]]>]]>";



static char RESPONSE[] = "<rpc-reply message-id=\"2\" xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
"<data>"
"<ManagedElement xmlns=\"urn:com:ericsson:ecim:ComTop\">"
"<managedElementId>1</managedElementId>"
"<SystemFunctions>"
"<systemFunctionsId>1</systemFunctionsId>"
"<Fm xmlns=\"urn:com:ericsson:ecim:ComFm\">"
"<fmId>1</fmId>"
"<lastSequenceNo>3</lastSequenceNo>"
"</Fm>"
"</SystemFunctions>"
"</ManagedElement>"
"</filter>"
"</rpc-reply>]]>]]>";


static char OK[] = "<rpc-reply message-id=\"3\" xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
"<ok/>"
"</rpc-reply>]]>]]>";


#define FD_MAX(max_now,x)     ((x) > (max_now) ? (x) : (max_now))

#include "acs_nclib_tcp_server.h"
#include "acs_nclib_tcp_client.h"

int WaitForEvents(int count, int *eventHandles, int timeoutInMilliseconds);
bool ResetEvent(int event);

int main (int argc, char * argv []) {

	acs_nclib_tcp_server* server = new acs_nclib_tcp_server();

	uint32_t ip = 0x7F000001;

	server->addTcpServer(ip, 4444);

	int count = 0;

	// start the servers
	unsigned int active = server->startListen();

	std::vector<int> eventVector;

	server->addActiveServerHandlesToVector(eventVector);

	std::vector<acs_nclib_tcp_client*> clients;

	int *handleArr = 0;

	while(true)
	{
		// Create array with all handles
		size_t slaveOffset = eventVector.size();
		size_t handleArrSize = slaveOffset + clients.size();

		delete[] handleArr; //don't worry, deleting NULL does not cause a crash
		handleArr = new int[handleArrSize];

		// Loop through vector and add handles to array
		for (size_t i = 0; i < handleArrSize; i++) {
			handleArr[i] = eventVector[i];
		}

		// Loop through all connected slaves and add handles to array
		for (size_t i = 0;i < clients.size(); i++) {
			handleArr[i + slaveOffset] = clients[i]->getHandle();
		}

		std::cout << "Waiting for incoming connections...\n" << std::endl;

		int eventIndex = WaitForEvents(handleArrSize, handleArr, 0);

		if (eventIndex == -1)
			break;


		if (server->isServerHandle(handleArr[eventIndex]))
		{

			acs_nclib_tcp_client * tcpClient = server->acceptConnectionForHandle(handleArr[eventIndex]);

			clients.push_back(tcpClient);

			std::cout << "New Client connected sending hello msg to " << tcpClient << "\n"  << std::endl;
			tcpClient->send(HELLO, strlen(HELLO));

		}

		else if (eventIndex >= (int) slaveOffset && eventIndex < (int) handleArrSize )
		{
			acs_nclib_tcp_client * tcpClient  = clients[eventIndex - slaveOffset];

			char buff[512] = {0};
			int read_size = 0;

			acs_nclib_tcp_client::LinkStatus_t status = acs_nclib_tcp_client::OK;

			status = tcpClient->read(buff,511,read_size);

			if (status != acs_nclib_tcp_client::OK || read_size < 0)
			{
				std::cout << "Disconnected client " << tcpClient << "\n" << std::endl;
				tcpClient->close();

				count = 0;
				std::vector<acs_nclib_tcp_client *>::iterator it;

				for (it = clients.begin(); it != clients.end(); ++it)
				{
					if (tcpClient == *it)
					{
						clients.erase(it);
						delete tcpClient;
						break;
					}
				}
			}
			else
			{

				std::cout << "Received Data from client " << tcpClient << std::endl;
				std::cout << "Data read " << read_size << " status " << status << "\n\n" << buff << "\n" << std::endl;

				std::cout << "Sending response" << std::endl;

				count++;
				if (count == 1)
				{
					sleep(1);
					tcpClient->send(RESPONSE, strlen(RESPONSE));
				} else if (count == 2)
				{
					sleep(1);
					tcpClient->send(OK, strlen(OK));
				}
			}
		}
	}

}


int WaitForEvents(int count, int *eventHandles, int timeoutInMilliseconds)
{
    fd_set fds;
    int nfds = 0;
    struct timeval *tv = NULL;

    // set timeout value (if any)
    if (timeoutInMilliseconds > 0) {
        tv = new struct timeval;

        unsigned long seconds = timeoutInMilliseconds / 1000;
        unsigned long ms = timeoutInMilliseconds % 1000;

        tv->tv_sec = seconds;
        tv->tv_usec = ms * 1000;
    }

    FD_ZERO(&fds);

    // go thru all fd's
    for (int i = 0;i < count; i++) {

       // Executing FD_CLR() or FD_SET() with
       // a value of fd that is negative or is equal to or larger than FD_SETSIZE
       // will result in undefined behavior.
        if (eventHandles[i] < 0) {
        	if (tv != NULL) {
        		// free allocated memory
        		delete tv;
        		tv = NULL;
        	}

           return -1;
        }

        FD_SET(eventHandles[i], &fds);

        // we want the highest fds
        nfds = FD_MAX(nfds, eventHandles[i]);
    }

    // wait for something to happen
    int ret = select(nfds + 1, &fds, NULL, NULL, tv);

    if (tv != NULL) {
        // free allocated memory
        delete tv;
        tv = NULL;
    }

    if (ret == 0) {
        // timeout has occurred
        return -2;

    } else if (ret == -1) {
        // a fault has occurred
        return -1;
    }    // check the list of unnamed events

    int n = -1;

    // find out which one of the fds that has been signaled
    for (int i = 0;i < count; i++) {
        if (FD_ISSET(eventHandles[i], &fds)) {
            n = i;
            break;
        }
    }

    if (n != -1)
    {
        return n;
    }

    // should never come here
    return -1;
}


bool ResetEvent(int event)
{
	fd_set fds;
	struct timeval tv;

	// poll and make sure that it is possible to reset the event
	tv.tv_sec  = 0;
	tv.tv_usec = 0;

	FD_ZERO(&fds);
	FD_SET(eventHandle, &fds);

	int ret = select(eventHandle + 1, &fds, NULL, NULL, &tv);

	if (ret == -1) {
		// something bad has happened
		return false;

	} else if (ret == 0) {
		// no data available (or rather, event not set)
		return false;
	}

	// reset event
	ssize_t s;
	uint64_t u = 0;

	s = read(event, &u, sizeof(uint64_t));

	if (s != sizeof(uint64_t)) {
		return false;
	}

	return true;

}
