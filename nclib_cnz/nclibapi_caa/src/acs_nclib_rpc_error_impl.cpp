//
//  acs_nclib_rpc_error_impl.cpp
//  NetConf
//
//  Created by Stefano Volpe on 07/10/12.
//  Copyright (c) 2012 Stefano Volpe. All rights reserved.
//

#include "acs_nclib_rpc_error_impl.h"
#include "acs_nclib_utils.h"
#include <string.h>


acs_nclib_rpc_error_impl::acs_nclib_rpc_error_impl(acs_nclib::ErrorType err_type, acs_nclib::ErrorSeverity err_sev):
error_type(err_type),
error_severity(err_sev),
error_tag(0),
error_app_tag(0),
error_path(0),
error_message(0),
error_info(0)
{

}

acs_nclib_rpc_error_impl::acs_nclib_rpc_error_impl(const acs_nclib_rpc_error& err):
		error_tag(0),
		error_app_tag(0),
		error_path(0),
		error_message(0),
		error_info(0)
{
	*this = err;
}

acs_nclib_rpc_error_impl::~acs_nclib_rpc_error_impl()
{

	if (error_tag)
	{
		delete[] error_tag;
		error_tag = 0;
	}
	if (error_app_tag)
	{
		delete[] error_app_tag;
		error_app_tag = 0;
	}
	if (error_path)
	{
		delete[] error_path;
		error_path = 0;
	}
	if (error_message)
	{
		delete[] error_message;
		error_message = 0;
	}
	if (error_info)
	{
		delete[] error_info;
		error_info = 0;
	}

}

void acs_nclib_rpc_error_impl::set_error_type(acs_nclib::ErrorType new_val)
{
    error_type = new_val;
}
void acs_nclib_rpc_error_impl::set_error_severity(acs_nclib::ErrorSeverity new_val)
{
    error_severity = new_val;
}
void acs_nclib_rpc_error_impl::set_error_tag(const char* new_val)
{
	if (error_tag)
	{
		delete[] error_tag;
		error_tag = 0;
	}

	if (new_val != 0)
	{
		unsigned long len = strlen(new_val)+1;

		error_tag = new char[len];

		memset(error_tag,0,len);
		memcpy(error_tag,new_val,len-1);
	}
}
void acs_nclib_rpc_error_impl::set_error_app_tag(const char* new_val)
{
	if (error_app_tag)
	{
		delete[] error_app_tag;
		error_app_tag = 0;
	}

	if (new_val != 0)
	{
		unsigned long len = strlen(new_val)+1;

		error_app_tag = new char[len];

		memset(error_app_tag,0,len);
		memcpy(error_app_tag,new_val,len-1);
	}
}
void acs_nclib_rpc_error_impl::set_error_path(const char* new_val)
{
	if (error_path)
	{
		delete[] error_path;
		error_path = 0;
	}

	if (new_val != 0)
	{
		unsigned long len = strlen(new_val)+1;

		error_path = new char[len];

		memset(error_path,0,len);
		memcpy(error_path,new_val,len-1);
	}
}
void acs_nclib_rpc_error_impl::set_error_message(const char* new_val)
{
	if (error_message)
	{
		delete[] error_message;
		error_message = 0;
	}

	if (new_val != 0)
	{
		unsigned long len = strlen(new_val)+1;

		error_message = new char[len];

		memset(error_message,0,len);
		memcpy(error_message,new_val,len-1);
	}
}
void acs_nclib_rpc_error_impl::set_error_info(const char* new_val)
{
	if (error_info)
	{
		delete[] error_info;
		error_info = 0;
	}

	if (new_val != 0)
	{
		unsigned long len = strlen(new_val)+1;

		error_info = new char[len];

		memset(error_info,0,len);
		memcpy(error_info,new_val,len-1);
	}
}

acs_nclib::ErrorType acs_nclib_rpc_error_impl::get_error_type() const
{
    return error_type;
}
acs_nclib::ErrorSeverity acs_nclib_rpc_error_impl::get_error_severity() const
{
    return error_severity;
}
const char* acs_nclib_rpc_error_impl::get_error_tag() const
{
    return error_tag;
}
const char* acs_nclib_rpc_error_impl::get_error_app_tag() const
{
    return error_app_tag;
}
const char* acs_nclib_rpc_error_impl::get_error_path() const
{
    return error_path;
}
const char* acs_nclib_rpc_error_impl::get_error_message() const
{
    return error_message;
}
const char* acs_nclib_rpc_error_impl::get_error_info() const
{
    return error_info;
}

void acs_nclib_rpc_error_impl::dump(std::string &ret_val) const{

	ret_val = ACS_NCLIB_ERROR_TAG::HEADER;
	ret_val += "\n";

	if (error_type != acs_nclib::NC_ERR_UNKNOWN)
	{
		ret_val += ACS_NCLIB_ERROR_TAG::ERR_TYPE_HAEDER;
		switch (error_type) {
            case acs_nclib::NC_ERR_UNKNOWN:
                break;
            case acs_nclib::NC_ERR_TRANSPORT:
                ret_val += "transport";
                break;
            case acs_nclib::NC_ERR_RPC:
                ret_val += "rpc";
                break;
            case acs_nclib::NC_ERR_PROTOCOL:
                ret_val += "protocol";
                break;
            case acs_nclib::NC_ERR_APPLICATION:
                ret_val += "application";
                break;
		}
		ret_val += ACS_NCLIB_ERROR_TAG::ERR_TYPE_TRAILER;
		ret_val += "\n";
	}

	if (error_tag != 0)
	{
		ret_val += ACS_NCLIB_ERROR_TAG::ERR_TAG_HAEDER;
		ret_val += error_tag;
		ret_val += ACS_NCLIB_ERROR_TAG::ERR_TAG_TRAILER;
		ret_val += "\n";
	}

	ret_val += ACS_NCLIB_ERROR_TAG::ERR_SEV_HAEDER;
	switch(error_severity) {
	case acs_nclib::SEVERITY_ERROR:
		ret_val += "error";
		break;
	case acs_nclib::SEVERITY_WARNING:
		ret_val += "warning";
		break;
	}
	ret_val += ACS_NCLIB_ERROR_TAG::ERR_SEV_TRAILER;
	ret_val += "\n";

	if (error_app_tag != 0)
	{
		ret_val += ACS_NCLIB_ERROR_TAG::ERR_APP_TAG_HAEDER;
		ret_val += error_app_tag;
		ret_val += ACS_NCLIB_ERROR_TAG::ERR_APP_TAG_TRAILER;
		ret_val += "\n";
	}

	if (error_path != 0)
	{
		ret_val += ACS_NCLIB_ERROR_TAG::ERR_PATH_HAEDER;
		ret_val += error_path;
		ret_val += ACS_NCLIB_ERROR_TAG::ERR_PATH_TRAILER;
		ret_val += "\n";
	}

	if (error_message != 0)
	{
		ret_val += ACS_NCLIB_ERROR_TAG::ERR_MSG_HAEDER;
		ret_val += error_message;
		ret_val += ACS_NCLIB_ERROR_TAG::ERR_MSG_TRAILER;
		ret_val += "\n";
	}

	if (error_info != 0)
	{
		ret_val += ACS_NCLIB_ERROR_TAG::ERR_INFO_HAEDER;
		ret_val += "\n";
		ret_val += error_info;
		ret_val += "\n";
		ret_val += ACS_NCLIB_ERROR_TAG::ERR_INFO_TRAILER;
	}

	ret_val += ACS_NCLIB_ERROR_TAG::TRAILER;

}

char* acs_nclib_rpc_error_impl::dump() const
{
//    std::string temp;
//    dump(temp);
//
//    unsigned long dump_size = temp.size()+1;

	unsigned long dump_size = 512;
	if (error_tag)
		dump_size += strlen(error_tag);
	if (error_app_tag)
		dump_size += strlen(error_app_tag);
	if (error_path)
		dump_size += strlen(error_path);
	if (error_message)
		dump_size += strlen(error_message);
	if (error_info)
		dump_size += strlen(error_info);

    char* ret_val = new char[dump_size];
    memset(ret_val,0,dump_size);

    //memcpy(ret, temp.c_str(), dump_size);
    
    strcat(ret_val,ACS_NCLIB_ERROR_TAG::HEADER);
    strcat(ret_val,"\n");

    if (error_type != acs_nclib::NC_ERR_UNKNOWN)
    {
    	strcat(ret_val,ACS_NCLIB_ERROR_TAG::ERR_TYPE_HAEDER);
    	switch (error_type) {
    	case acs_nclib::NC_ERR_UNKNOWN:
    		break;
    	case acs_nclib::NC_ERR_TRANSPORT:
    		strcat(ret_val,"transport");
    		break;
    	case acs_nclib::NC_ERR_RPC:
    		strcat(ret_val,"rpc");
    		break;
    	case acs_nclib::NC_ERR_PROTOCOL:
    		strcat(ret_val,"protocol");
    		break;
    	case acs_nclib::NC_ERR_APPLICATION:
    		strcat(ret_val,"application");
    		break;
    	}
    	strcat(ret_val,ACS_NCLIB_ERROR_TAG::ERR_TYPE_TRAILER);
    	strcat(ret_val,"\n");
    }

    if (error_tag != 0)
    {
    	strcat(ret_val,ACS_NCLIB_ERROR_TAG::ERR_TAG_HAEDER);
    	strcat(ret_val,error_tag);
    	strcat(ret_val,ACS_NCLIB_ERROR_TAG::ERR_TAG_TRAILER);
    	strcat(ret_val,"\n");
    }

    strcat(ret_val,ACS_NCLIB_ERROR_TAG::ERR_SEV_HAEDER);
    switch(error_severity) {
    case acs_nclib::SEVERITY_ERROR:
    	strcat(ret_val,"error");
    	break;
    case acs_nclib::SEVERITY_WARNING:
    	strcat(ret_val,"warning");
    	break;
    }
    strcat(ret_val,ACS_NCLIB_ERROR_TAG::ERR_SEV_TRAILER);
    strcat(ret_val,"\n");

    if (error_app_tag != 0)
    {
    	strcat(ret_val,ACS_NCLIB_ERROR_TAG::ERR_APP_TAG_HAEDER);
    	strcat(ret_val,error_app_tag);
    	strcat(ret_val,ACS_NCLIB_ERROR_TAG::ERR_APP_TAG_TRAILER);
    	strcat(ret_val,"\n");
    }

    if (error_path != 0)
    {
    	strcat(ret_val,ACS_NCLIB_ERROR_TAG::ERR_PATH_HAEDER);
		strcat(ret_val,error_path);
		strcat(ret_val,ACS_NCLIB_ERROR_TAG::ERR_PATH_TRAILER);
		strcat(ret_val,"\n");
    }

    if (error_message != 0)
    {
    	strcat(ret_val,ACS_NCLIB_ERROR_TAG::ERR_MSG_HAEDER);
    	strcat(ret_val,error_message);
    	strcat(ret_val,ACS_NCLIB_ERROR_TAG::ERR_MSG_TRAILER);
    	strcat(ret_val,"\n");
    }

    if (error_info != 0)
    {
    	strcat(ret_val,ACS_NCLIB_ERROR_TAG::ERR_INFO_HAEDER);
    	strcat(ret_val,"\n");
    	strcat(ret_val,error_info);
    	strcat(ret_val,"\n");
    	strcat(ret_val,ACS_NCLIB_ERROR_TAG::ERR_INFO_TRAILER);
    }

    strcat(ret_val,ACS_NCLIB_ERROR_TAG::TRAILER);
    
    return ret_val;
}

acs_nclib_rpc_error_impl& acs_nclib_rpc_error_impl::operator=(const acs_nclib_rpc_error& err)
{
	set_error_type(err.get_error_type());
	set_error_severity(err.get_error_severity());
	set_error_tag(err.get_error_tag());
	set_error_app_tag(err.get_error_app_tag());
	set_error_path(err.get_error_path());
	set_error_message(err.get_error_message());
	set_error_info(err.get_error_info());

	return *this;
}
