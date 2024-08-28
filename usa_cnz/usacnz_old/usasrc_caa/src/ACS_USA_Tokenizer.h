////////////////////////////////////////////////////////////////////////////
// NAME - 
//
// COPYRIGHT Ericsson AB, Sweden 2004
// All rights reserved.
//
// The Copyright to the computer program(s) herein is the property
// of Ericsson AB, Sweden. 
// The program(s) may be used and/or copied only with the written 
// permission from Ericsson AB
// or in accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have been supplied.
//
// DESCRIPTION
//		Tokenizer class written to replace Rougewave Tools.h++
//
//
// DOCUMENT NO
//	----
//
// AUTHOR
//20040429		QAMMKAL		First Release
//
// CHANGES
// 20040429		QAMMKAL		First Release
//
////////////////////////////////////////////////////////////////////////////


#include <string>

using namespace std;
/********************Class definition : Begin **********************/
class ACS_USA_Tokenizer {
private:
	string orgStr;	//original string
	string parStr;	//partial string
public:
	/* constructor */
	ACS_USA_Tokenizer(string str);

	/* To get the original string */
	string orgData();

	/* to retrieve next avaliable token */
	string operator()(string tokens);

}; /* End of class definition */
/********************Class definition : End **********************/