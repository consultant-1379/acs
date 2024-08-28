//========================================================================================
// 
// NAME
//		ACS_PRC_Dialogue.h - include file for PRC file
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
// DESCRIPTION 
//		Dialogue routines for PRC
//
// ERROR HANDLING
//		-
//
// DOCUMENT NO
//		CAA 109 0520
//
// AUTHOR 
//		2003-02-06 by EAB/UKY/GB UABPEAA
//
// REVISION
//		-	
// 
// CHANGES
//
// RELEASE REVISION HISTORY
//
//		REV NO	DATE 	NAME		DESCRIPTION
//		R1A		030206  UABPEAA		First version
//
// LINKAGE
//
// SEE ALSO 
//		-
//
//========================================================================================

#ifndef ACS_PRC_DIALOGUE_H
#define ACS_PRC_DIALOGUE_H

#include <string.h>
#include <stdio.h>
#include <iostream>


class Dialogue
{
public:
	static bool matchKeyWord(std::string keyWord, std::string str);
	
	static bool affirm(std::string str); //, std::string abortmsg);
	
private:
	static std::string strip(const char buf[]);

	static std::string prompt_;
};

#endif
