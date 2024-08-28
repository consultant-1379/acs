#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>
using namespace std;

#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"

#include "acs_apgcc_paramhandling.h"

using namespace ACS_CS_API_SET_NS;

TEST(ACS_CS_API, setAlarmMaster)
{
	CS_API_Set_Result res = ACS_CS_API_Set::setAlarmMaster(1001);

	if (res == Result_Success) {
		cout << "setAlarmMaster Succeded" << endl;
	}
	else{
		cout << "setAlarmMaster Failed" << endl;
	}

	uint16_t code = 666;
	acs_apgcc_paramhandling acsParamHandling;
	ACS_CC_ReturnType read = acsParamHandling.getParameter("bladeClusterInfoId=1,configurationInfoId=1", "alarmMaster", &code);


	if(read == ACS_CC_SUCCESS){
		cout << "ALARM MASTER READ AGAIN: " << code << endl;
	}
	else{
		cout << "ALARM MASTER READ AGAIN: FAILED" << endl;
	}


	EXPECT_TRUE(res == Result_Success);
}



TEST(ACS_CS_API, setClockMaster)
{
	CS_API_Set_Result res = ACS_CS_API_Set::setClockMaster(1001);

	if (res == Result_Success) {
		cout << "setClockMaster Succeded" << endl;
	}
	else
	{
		cout << "setClockMaster Failed" << endl;
	}

	uint16_t code = 666;
	acs_apgcc_paramhandling acsParamHandling;
	ACS_CC_ReturnType read = acsParamHandling.getParameter("bladeClusterInfoId=1,configurationInfoId=1", "clockMaster", &code);

	if(read == ACS_CC_SUCCESS){
		cout << "CLOCK MASTER READ AGAIN: " << code << endl;
	}else{
		cout << "CLOCK MASTER READ AGAIN: FAILED" << endl;
	}

	EXPECT_TRUE(res == Result_Success);
}
