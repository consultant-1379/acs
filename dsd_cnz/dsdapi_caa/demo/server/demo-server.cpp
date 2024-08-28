#include "ACS_DSD_Server.h"

int main(int argc, char * argv[]) {
	ACS_DSD_Server inet_service;
	ACS_DSD_Server unix_service;

	inet_service.open(acs_dsd::SERVICE_MODE_INET_SOCKET);









	return 0;
}
