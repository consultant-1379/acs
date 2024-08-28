/*
 * neapitest.cpp
 *
 *  Created on: Oct 19, 2010
 *      Author: mann
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "ACS_CS_API.h"
#include "ACS_CS_API_Set.h"
//#include "ACS_CS_API_Util_Internal.h"
//#include "ACS_CS_Internal_Table.h"
//#include "ACS_CS_Protocol.h"

using namespace std;

int main(int argc, char* argv[])
{

	string App;
	string slot;

	App = argv[1];
	slot= argv[2];

	ACS_CS_API_SET_NS::CS_API_Set_Result res;

	res = ACS_CS_API_Set::setSoftwareVersion(App,string("1_2_0_4"),atoi(slot.c_str()));

	if(res != 0)
	{
		cout << "Error setSoftwareVersion " << endl;
	}
	
	return 0;
}
