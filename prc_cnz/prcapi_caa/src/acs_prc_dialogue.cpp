//========================================================================================
// 
// NAME
//      ACS_PRC_Dialogue.cpp
//
// COPYRIGHT
//		COPYRIGHT Ericsson AB, Sweden 2002.
//		All rights reserved.
//
//		The Copyright to the computer program(s) herein 
//		is the property of Ericsson AB, Sweden.
//		The program(s) may be used and/or copied only with the 
//		written permission from Ericsson AB or in accordance 
//		with the terms and conditions stipulated in the 
//		agreement/contract under which the program(s) have been 
//		supplied.
//
// DOCUMENT NO
//		19089-CAA 109 0520 
//
// AUTHOR 
//		2002-07-25 by UAB/UKY/GM UABTSO
//
// REVISION
//		-	
// 
// CHANGES
//
// RELEASE REVISION HISTORY
//
//		REV NO	DATE 	NAME		DESCRIPTION
//		R3A		021001  UABTSO		INGO4 
//
// SEE ALSO 
//		-
//
//========================================================================================

#include "acs_prc_dialogue.h"
#include <iostream>
#include <sstream>
#include <unistd.h>

std::string Dialogue::prompt_("\x03:");		// ETX :

using namespace std;

//========================================================================================
//	Match a string towards a keyword
//========================================================================================

bool
Dialogue::matchKeyWord(string keyWord, string str)
{
	if (str.empty()) return false;

	string temp = str;

	int i = 0;
	while ( i < (int)temp.size() ){
		temp[i] = toupper(temp[i]);
		i++;
	}

	return !keyWord.find(temp);
}

//========================================================================================
//	Strip leading and trailing spaces from a string buffer
//========================================================================================

string
Dialogue::strip(const char buf[])
{
	string str(buf);
	string::size_type first = str.find_first_not_of(" \t");
	if (first != string::npos)
	{
		string::size_type last = str.find_last_not_of(" \t");
		return str.substr(first, last - first + 1);
	}
	else
	{
		return "";
	}
}

//========================================================================================
//	Expect "YES" or "NO" answer
//========================================================================================

bool 
Dialogue::affirm(string text)
{
	char ibuf[128];
	string answer;

	cout << text << "[y(es)/n(o)]" << prompt_;
	while (1)
	{
		cin.clear();
		cin.getline(ibuf, 128);
		sleep(1);
		answer = strip(ibuf);
		if (matchKeyWord("YES", answer))
		{
			return true;
		}
		if (matchKeyWord("NO", answer))
		{
			return false;
		}
		cout << "Enter 'y(es)' or 'n(o)'" << prompt_;
	} // End of while
} // End of affirm
