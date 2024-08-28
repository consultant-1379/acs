#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "SocketConnectorCuteTester.h"
#include "LSocketConnectorCuteTester.h"
//#include "SPipeConnectorCuteTester.h"

int main(int argc, char * argv[]) {
	cute::suite s;

	s.push_back(SocketConnectorCuteTester());
	s.push_back(LSocketConnectorCuteTester());
	//s.push_back(SPipeConnectorCuteTester());

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "[Connectors Cute Test Suite]");

	return 0;
}
