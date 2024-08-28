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

#include "ACS_USA_Common.h"

int toUpper(String &str)
{ 
   unsigned int i=0; 
   while(i < str.length()) 
   {
	   str[i]= toupper(str[i]); 
	   i++;
   }
   return(0);
} 

int last(String str,char ch)
{
	return str.find_last_of(ch);
}

int first(String str,char ch)
{
	return str.find_first_of(ch);
}

int strip(String &str,char ch)
{
	int i = 0;
	while(i == 0)
	{
		i = str.find_first_of(ch);
		if ( i == 0)
		{
			str = str.substr(i+1,str.length());
		}
	}
	i = str.find_last_of(ch);
	while(i == 	str.length() - 1)
	{
		if ( i == (str.length() - 1))
		{
			str = str.substr(0,i);
		}
		i = str.find_last_of(ch);
	}

    return 0;
}




