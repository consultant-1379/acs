#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>
#include "ACS_CS_Internal_Table.h"

#include "ACS_CS_API_BoardSearch_Implementation.h"
#include "ACS_CS_API_HWC_R1.h"
#include "ACS_CS_API_R1.h"
#include "ACS_CS_API.h"

using namespace std;



TEST(ACS_CS, getVLANAddress)
{
	ACS_CS_INTERNAL_API::ACS_CS_Internal_Table * internalTable = ACS_CS_INTERNAL_API::ACS_CS_Internal_Table::createTableInstance();

	string ipAddressString;
	BoardID boardId = 5;


	internalTable->getVLANAddress(boardId, "APZ-A", ipAddressString);

	cout << "getVLANAddress for APZ-A returned: " << ipAddressString << endl;

	internalTable->getVLANAddress(boardId, "APZ-B", ipAddressString);

	cout << "getVLANAddress for APZ-B returned: " << ipAddressString << endl;

	delete internalTable;

	EXPECT_TRUE(true);
}



TEST(ACS_CS_API, bs_setIPEthB)
{
    ACS_CS_API_HWC * hwc =  ACS_CS_API::createHWCInstance();
    ACS_CS_API_BoardSearch * boardSearch = hwc->createBoardSearchInstance();

    boardSearch->reset();

    boardSearch->setIPEthB(0xC0A8AA3A);

    ACS_CS_API_IdList boards;
    ACS_CS_API_NS::CS_API_Result api_call_result = ACS_CS_API_NS::Result_NoValue;
    api_call_result = hwc->getBoardIds(boards, boardSearch);
    int hits = boards.size();

    if(api_call_result != ACS_CS_API_NS::Result_Success){
    	FAIL() << "BOARDSEARCH FAILED";
    }

    if(hits>0){
    	for(int i= 0; i < hits; i++){
    		cout << "BoardId=" << boards[i] << endl;
    	}
    }
    else{
    	cout << "NO HITS" << endl;
    }

	EXPECT_TRUE(true);
}

TEST(ACS_CS_API, getAPGCount)
{
    ACS_CS_API_HWC * hwc =  ACS_CS_API::createHWCInstance();
    ACS_CS_API_BoardSearch * boardSearch = hwc->createBoardSearchInstance();

    boardSearch->reset();

    ACS_CS_API_IdList boards;
    ACS_CS_API_NS::CS_API_Result api_call_result = ACS_CS_API_NS::Result_NoValue;
    api_call_result = hwc->getBoardIds(boards, boardSearch);
    int hits = boards.size();

    if(api_call_result != ACS_CS_API_NS::Result_Success){
    	FAIL() << "BOARDSEARCH FAILED";
    }

    if(hits>0){
    	for(int i= 0; i < hits; i++){
    		cout << "BoardId=" << boards[i] << endl;
    	}
    }
    else{
    	cout << "NO HITS" << endl;
    }

	EXPECT_TRUE(true);
}

