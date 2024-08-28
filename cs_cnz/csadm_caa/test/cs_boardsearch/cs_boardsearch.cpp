#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"

#include <vector>
#include <iostream>
#include <assert.h>
#include <stdlib.h>


using namespace std;
using namespace ACS_CS_API_SET_NS;

//
// Definitions
//

//------------------------------------------------------------------------------
// Function prototypes
//------------------------------------------------------------------------------



int main(int argc, char *argv[])
{

	cout << endl << endl << "Testing boardsearch..." << endl << endl;
	cout << "Magazine: 0x04000201" << endl << "FBN: ACS_CS_API_HWC_NS::FBN_SCBRP" << endl << endl;

	ACS_CS_API_HWC* hwc = ACS_CS_API::createHWCInstance();

	if (hwc)
	{
		ACS_CS_API_BoardSearch* boardSearch = ACS_CS_API_HWC::createBoardSearchInstance();

		if (boardSearch)
		{

			ACS_CS_API_IdList boardList;

			boardSearch->reset();
			boardSearch->setFBN(ACS_CS_API_HWC_NS::FBN_SCBRP);
			boardSearch->setMagazine(0x04000201);

			(void) hwc->getBoardIds(boardList, boardSearch);
			int hits = boardList.size();
			cout << "Number of boards: " << hits << endl << endl;

			ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NS::Result_NoValue;
			cs_call_result = hwc->getBoardIds(boardList, boardSearch);
			hits = boardList.size();
			cout << "Number of boards: " << hits << ", cs_call_result: " << cs_call_result << endl << endl;
		}
		ACS_CS_API::deleteHWCInstance(hwc);
	}
	else
	{
		cout<<"ERROR: ACS_CS_API::createHWCInstance"<< endl;
	}

    return 0;
}



