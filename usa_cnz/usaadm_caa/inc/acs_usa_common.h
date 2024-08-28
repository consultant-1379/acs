
#include <stdlib.h>
#include <string>
#include <list>
#include "ace/ACE.h"
#include "acs_usa_types.h"
using namespace std;
//typedef string String;

#define rwnil -1
#define nilstring 0

/*To convert string in uppercase*/
int usacc_toUpper(String &str);

/*To get last occurence of 'Ch' in string */
int usacc_last(String str,char ch);

/*To get first occurence of 'Ch' in string */
int usacc_first(String str,char ch);

/*To strip 'Ch' in string from both ends.*/
int usacc_strip(String &str,char ch);

/* to milli sleep */
void usacc_msec_sleep(ACE_UINT32 m_sec);

std::string get_ap_hostname ();
std::string get_ap_node_id ();
std::string get_partner_ap_node_id ();
int read_string_from_file (const char * filename, char * buffer, size_t size);

//----------------------------------------------------------------------------
//
//  COPYRIGHT Ericsson AB 2010
//
//  The copyright to the computer program(s) herein is the property of
//  ERICSSON AB, Sweden. The programs may be used and/or copied only
//  with the written permission from ERICSSON AB or in accordance with
//  the terms and conditions stipulated in the agreement/contract under
//  which the program(s) have been supplied.
//
//----------------------------------------------------------------------------

