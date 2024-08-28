#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"

#include "SocketAcceptorCuteTester.h"
#include "LSocketAcceptorCuteTester.h"
//#include "SPipeAcceptorCuteTester.h"

int main(int argc, char * argv[]) {
	cute::suite s;

	s.push_back(SocketAcceptorCuteTester());
	s.push_back(LSocketAcceptorCuteTester());
	//s.push_back(SPipeAcceptorCuteTester());

	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "[Acceptors Cute Test Suite]");

	return 0;
}
