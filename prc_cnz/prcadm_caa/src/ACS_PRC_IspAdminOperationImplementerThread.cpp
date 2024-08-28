#include "ACS_PRC_IspAdminOperationImplementerThread.h"

ACS_PRC_IspAdminOperationImplementerThread::ACS_PRC_IspAdminOperationImplementerThread(): sleep(true), implementer(0) {
}

ACS_PRC_IspAdminOperationImplementerThread::~ACS_PRC_IspAdminOperationImplementerThread() {
}

int ACS_PRC_IspAdminOperationImplementerThread:: svc ( void ){

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
