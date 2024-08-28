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

#include "acs_usa_tokenizer.h"

/******************* Class implementation : Begin ************/
/* constructor */
ACS_USA_Tokenizer::ACS_USA_Tokenizer(string str) {
		orgStr = parStr = str;	
	}

/* To get the original string */
string 
ACS_USA_Tokenizer::orgData() {
			 return orgStr;
		 }

/* retrieves next avaliable token */
string 
ACS_USA_Tokenizer::operator()(string tokens) {		

		string temp;
		int i = 0;
		i = parStr.find_first_of(tokens);
		if(i != (int)string::npos) {
			temp.assign(parStr, 0, i);
			parStr.assign(parStr, i+1, parStr.size());
		}
		else {
			temp = parStr;
			parStr = "";
		}
		return temp;
	 }
/******************* Class implementation : End ************/
