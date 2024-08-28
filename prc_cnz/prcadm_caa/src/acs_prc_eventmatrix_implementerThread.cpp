#include "acs_prc_eventmatrix_implementerThread.h"

#define ACS_PRC_STRINGIZER(s) #s
#define ACS_PRC_STRINGIZE(s) ACS_PRC_STRINGIZER(s)

#if defined (ACS_PRCBIN_REVISION)
#define PRCBIN_REVISION ACS_PRC_STRINGIZE(ACS_PRCBIN_REVISION)
#else
#define PRCBIN_REVISION "UNKNOW"
#endif

acs_prc_eventmatrix_implementerThread::acs_prc_eventmatrix_implementerThread():sleep(true),implementer(0) {
}

acs_prc_eventmatrix_implementerThread::~acs_prc_eventmatrix_implementerThread() {
}

int acs_prc_eventmatrix_implementerThread:: svc ( void ){

    int ret;

    struct pollfd fds[1];

    fds[0].fd = implementer->getSelObj();
    fds[0].events = POLLIN;

    while( !sleep )
    {
		ret = poll(fds, 1, 400);

		if ( ret == 0 ) continue;

		if (ret == -1)
				printf("poll error: \n");
		else {
			if (sleep) {
				return 0;
			}
			implementer->dispatch(ACS_APGCC_DISPATCH_ALL);
		}
    }

    return 0;
}
