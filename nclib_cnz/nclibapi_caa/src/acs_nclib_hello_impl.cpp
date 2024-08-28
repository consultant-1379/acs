/*
 * acs_nclib_hello_impl.cpp
 *
 *  Created on: Oct 9, 2012
 *      Author: estevol
 */

#include "acs_nclib_hello_impl.h"
#include "acs_nclib_utils.h"
#include <sstream>


acs_nclib_hello_impl::acs_nclib_hello_impl():
acs_nclib_hello(),
sessionId(-1)
{
    
}

acs_nclib_hello_impl::~acs_nclib_hello_impl()
{
    
}



const char* acs_nclib_hello_impl::dump() const
{
    char* ret = 0;
    char* cap = 0;
    unsigned long cap_size = 0;
    
    if (capabilities.size() > 0)
    {
        cap_size = strlen(ACS_NCLIB_HELLO_TAG::CAPABILITIES_HEADER) + strlen( ACS_NCLIB_HELLO_TAG::CAPABILITIES_TRAILER) + 2;
        
        unsigned long offset = strlen(ACS_NCLIB_HELLO_TAG::CAPABILITY_HEADER) + strlen(ACS_NCLIB_HELLO_TAG::CAPABILITY_TRAILER) + 1;
        
        for (unsigned int i = 0; i < capabilities.size(); i++)
        {
            cap_size += capabilities.at(i).size() + offset;
        }
        
        cap = new char[++cap_size];
        memset(cap,0,cap_size);
        strcat(cap, ACS_NCLIB_HELLO_TAG::CAPABILITIES_HEADER);
        strcat(cap,"\n");
        
        for (unsigned int i = 0; i < capabilities.size(); i++)
		{
			strcat(cap, ACS_NCLIB_HELLO_TAG::CAPABILITY_HEADER);
			strcat(cap, capabilities.at(i).c_str());
			strcat(cap,ACS_NCLIB_HELLO_TAG::CAPABILITY_TRAILER);
			strcat(cap, "\n");
		}
        
		strcat(cap,ACS_NCLIB_HELLO_TAG::CAPABILITIES_TRAILER);
		strcat(cap, "\n");
    }
    
    unsigned long hello_size = cap_size + 512;
    ret = new char[hello_size];
    memset(ret,0,hello_size);
    
    strcat(ret,ACS_NCLIB_HELLO_TAG::HEADER);
    strcat(ret,ACS_NCLIB_NS_TAG::NAMESPACE_1_0);
    
    if (cap)
    {
        strcat(ret,cap);
        delete[] cap;
    }
    
    if (sessionId >= 0)
	{
		strcat(ret,ACS_NCLIB_HELLO_TAG::SESSIONID_HEADER);

		char id[32] = {0};
		sprintf(id,"%d",sessionId);

        strcat(ret,id);
        strcat(ret,ACS_NCLIB_HELLO_TAG::SESSIONID_TRAILER);
		strcat(ret,"\n");
	}
    
    strcat(ret,ACS_NCLIB_HELLO_TAG::TRAILER);
    
    return ret;
}



void acs_nclib_hello_impl::dump(std::string& ret_val) const
{
	ret_val = ACS_NCLIB_HELLO_TAG::HEADER;
	ret_val += ACS_NCLIB_NS_TAG::NAMESPACE_1_0;
    
	if (capabilities.size() > 0)
	{
		ret_val += ACS_NCLIB_HELLO_TAG::CAPABILITIES_HEADER;
		ret_val += "\n";
		for (unsigned int i = 0; i < capabilities.size(); i++)
		{
			ret_val += ACS_NCLIB_HELLO_TAG::CAPABILITY_HEADER;
			ret_val += capabilities.at(i);
			ret_val += ACS_NCLIB_HELLO_TAG::CAPABILITY_TRAILER;
			ret_val += "\n";
		}
		ret_val += ACS_NCLIB_HELLO_TAG::CAPABILITIES_TRAILER;
		ret_val += "\n";
	}
	if (sessionId >= 0)
	{
		std::stringstream ss;
		ss << sessionId;

		ret_val += ACS_NCLIB_HELLO_TAG::SESSIONID_HEADER + ss.str() + ACS_NCLIB_HELLO_TAG::SESSIONID_TRAILER;
		ret_val += "\n";
	}
	ret_val += ACS_NCLIB_HELLO_TAG::TRAILER;
    
}

void acs_nclib_hello_impl::set_session_id(int new_val)
{
    sessionId = new_val;
    
}

int acs_nclib_hello_impl::get_session_id() const
{
    return sessionId;
}

void acs_nclib_hello_impl::add_capability(const char* new_cap)
{
    std::string cap(new_cap);
    capabilities.push_back(cap);
}

void acs_nclib_hello_impl::get_capabilities(std::vector<std::string>& ret_vec) const
{
    for (unsigned int i = 0; i < capabilities.size(); i++)
    {
        ret_vec.push_back(capabilities.at(i));
    }
}

void acs_nclib_hello_impl::clear_capabilities()
{
    capabilities.clear();
}

