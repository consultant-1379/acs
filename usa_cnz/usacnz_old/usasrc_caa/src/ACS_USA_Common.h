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
//		Common function used in USA.
//
//
// DOCUMENT NO
//	----
//
// AUTHOR
//20040429		QVINKAL		First Release
//
// CHANGES
// 20040429		QVINKAL		First Release
//
////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdlib.h>
#include <string>
#include <list>

using namespace std;
typedef string String;

#define rwnil -1
#define nilstring 0

/*To convert string in uppercase*/
int toUpper(String &str);

/*To get last occurence of 'Ch' in string */
int last(String str,char ch);

/*To get first occurence of 'Ch' in string */
int first(String str,char ch);

/*To strip 'Ch' in string from both ends.*/
int strip(String &str,char ch);

