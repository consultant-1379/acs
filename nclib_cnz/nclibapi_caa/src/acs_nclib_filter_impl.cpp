//
//  acs_nc_filter.cpp
//  NetConf
//
//  Created by Stefano Volpe on 06/10/12.
//  Copyright (c) 2012 Stefano Volpe. All rights reserved.
//

#include "acs_nclib_filter_impl.h"
#include "acs_nclib_utils.h"
#include <stdlib.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
//#include <boost/exception/exception.hpp>

using namespace boost::property_tree;

acs_nclib_filter_impl::acs_nclib_filter_impl(acs_nclib::FilterType filType):
filter_type(filType),filter_content(0), filter_size(0)
{

}

acs_nclib_filter_impl::~acs_nclib_filter_impl()
{
    if (filter_content) {
        free(filter_content);
        filter_content = 0;
        filter_size = 0;
    }

}

acs_nclib_filter_impl::acs_nclib_filter_impl(const acs_nclib_filter& new_filter):
filter_content(0), filter_size(0)
{
	*this = new_filter;
}

acs_nclib::FilterType acs_nclib_filter_impl::get_filter_type() const
{
    return filter_type;
}

void acs_nclib_filter_impl::set_filter_type(acs_nclib::FilterType new_type)
{
    filter_type = new_type;
}

void acs_nclib_filter_impl::set_filter_content(const char* new_content)
{
    if (filter_content) {
        free(filter_content);
        filter_content = 0;
        filter_size = 0;
    }

    if (new_content != 0)
    {
    	filter_size = strlen(new_content)+1;
    	filter_content = (char*) malloc(filter_size * sizeof(char));

    	memset(filter_content, 0, filter_size);
    	memcpy(filter_content, new_content, filter_size - 1);
    }
}

char* acs_nclib_filter_impl::get_filter_content() const
{
	 char* return_value = new char[filter_size];
	 memcpy(return_value, filter_content, filter_size);

	 return return_value;
}

char* acs_nclib_filter_impl::dump() const
{
    unsigned long dump_size = filter_size + strlen(ACS_NCLIB_FILTER_TAG::SUBTREE_HEADER) + strlen(ACS_NCLIB_FILTER_TAG::TRAILER) + 1;
    char* return_value = new char[dump_size];
    memset(return_value, 0, dump_size);

    switch (filter_type) {
        case acs_nclib::FILTER_SUBTREE:
            strcat(return_value, ACS_NCLIB_FILTER_TAG::SUBTREE_HEADER);
            strcat(return_value,filter_content);
            strcat(return_value, ACS_NCLIB_FILTER_TAG::TRAILER);
            break;

        default:
            break;
    }

    return return_value;
}

void acs_nclib_filter_impl::dump(std::string& return_value) const
{
    switch (filter_type) {
        case acs_nclib::FILTER_SUBTREE:
            return_value = ACS_NCLIB_FILTER_TAG::SUBTREE_HEADER;
            return_value += filter_content;
            return_value += ACS_NCLIB_FILTER_TAG::TRAILER;
            break;

        default:
            break;
    }

}

acs_nclib_filter_impl& acs_nclib_filter_impl::operator=(const acs_nclib_filter& copy)
{
	set_filter_type(copy.get_filter_type());
	char * content = copy.get_filter_content();
	set_filter_content(content);
	delete content;

	return *this;
}
