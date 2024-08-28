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

#include "acs_usa_common.h"
#include "acs_usa_logtrace.h"

int usacc_toUpper(String &str)
{ 
   unsigned int i=0; 
   while(i < str.length()) 
   {
	   str[i]= toupper(str[i]); 
	   i++;
   }
   return(0);
} 

int usacc_last(String str,char ch)
{
	return str.find_last_of(ch);
}

int usacc_first(String str,char ch)
{
	return str.find_first_of(ch);
}

int usacc_strip(String &str,char ch)
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
	while(i == 	int(str.length() - 1))
	{
		if ( i == int((str.length() - 1)))
		{
			str = str.substr(0,i);
		}
		i = str.find_last_of(ch);
	}

    return 0;
}

void usacc_msec_sleep(ACE_UINT32 time_in_msec) {

	USA_TRACE_ENTER();

	struct timeval tv;

	tv.tv_sec = time_in_msec / 1000;
	tv.tv_usec = ((time_in_msec) % 1000) * 1000;
	while (select(0, 0, 0, 0, &tv) != 0)
		if (errno == EINTR)
			continue;
	USA_TRACE_LEAVE();

}
int read_string_from_file (const char * filename, char * buffer, size_t size) {
	FILE * file = ::fopen(filename, "r");
	if(!file) { // ERROR: opening the file
		return -1;
	}
	char read_format [32];
	::snprintf(read_format, ACS_USA_ARRAY_SIZE(read_format), "%%%zus", size - 1);
	read_format[ACS_USA_ARRAY_SIZE(read_format) - 1] = '\0';
	int nParsedFields = ::fscanf(file, read_format, buffer);
	::fclose(file);
	if(nParsedFields != 1) {
		return -1;
	}
	return 0;
}
std::string get_ap_node_id () {
	char buffer[8]={0};
	int return_code = read_string_from_file(AP_NODE_ID_FILEPATH, buffer,ACS_USA_ARRAY_SIZE(buffer));
	if (return_code) {
		*buffer = '\0';
	}
	return string(buffer);
}

std::string get_partner_ap_node_id () {
	char buffer[8]={0};
	int return_code = read_string_from_file(AP_OTHER_NODE_ID_FILEPATH, buffer, ACS_USA_ARRAY_SIZE(buffer));
	if (return_code) {
		*buffer = '\0';
	}
	return string(buffer);
}
std::string get_ap_hostname () {
	char buffer[8]={0};
	int return_code = read_string_from_file(AP_NODE_HOSTNAME_FILEPATH, buffer, ACS_USA_ARRAY_SIZE(buffer));
	if (return_code) {
		*buffer = '\0';
	}
	return string(buffer);
}

