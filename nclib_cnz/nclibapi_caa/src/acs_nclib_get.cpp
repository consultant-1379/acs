//
//  acs_nclib_get.cpp
//  NetConf
//
//  Created by Stefano Volpe on 06/10/12.
//  Copyright (c) 2012 Stefano Volpe. All rights reserved.
//

#include "acs_nclib_get.h"
#include "acs_nclib_utils.h"
#include "acs_nclib_filter_impl.h"

acs_nclib_get::acs_nclib_get(acs_nclib::Datastore ds):
acs_nclib_rpc_impl(acs_nclib::OP_GET),
datastore(ds)
{
    
}

acs_nclib_get::~acs_nclib_get()
{
	clear_filters();
}

void acs_nclib_get::add_filter(acs_nclib_filter* new_filter)
{
	acs_nclib_filter* copy = new acs_nclib_filter_impl(*new_filter);

    filters.push_back(copy);
}

void acs_nclib_get::clear_filters()
{
	for (unsigned int it = 0; it < filters.size(); it++)
	{
		delete(filters.at(it));
	}
	filters.clear();
}

char* acs_nclib_get::dump_operation() const
{
	int filters_count = filters.size();
	char* filter_content = 0;
	unsigned long filter_size = 0;

	if (filters_count > 0)
	{
		filter_content = filters.at(filters_count - 1)->dump();
		filter_size = strlen(filter_content);
	}

    char* return_value = new char[512 + filter_size];
    memset(return_value, 0, 512 + filter_size);
    
    strcat(return_value, ACS_NCLIB_GET_TAG::HEADER);

    switch (datastore) {
        case acs_nclib::DATASTORE_RUNNING:
        	strcat(return_value, ACS_NCLIB_DATASTORE_TAG::SOURCE_HEADER);
            strcat(return_value, ACS_NCLIB_DATASTORE_TAG::RUNNING);
            strcat(return_value, ACS_NCLIB_DATASTORE_TAG::SOURCE_TRAILER);
            break;
            
        case acs_nclib::DATASTORE_CANDIDATE:
        	strcat(return_value, ACS_NCLIB_DATASTORE_TAG::SOURCE_HEADER);
        	strcat(return_value, ACS_NCLIB_DATASTORE_TAG::CANDIDATE);
        	strcat(return_value, ACS_NCLIB_DATASTORE_TAG::SOURCE_TRAILER);
        	break;

        case acs_nclib::DATASTORE_UNKNOWN:
        default:
            break;
    }
    
    if (filter_content)
    {
    	strcat(return_value, "\n");
    	strcat(return_value, filter_content);

    	delete[] filter_content;
    }



//    for (unsigned int it = 0; it < filters.size(); it++)
//    {
//        char* filter_content = filters.at(it)->dump();
//
//        strcat(return_value, "\n");
//        strcat(return_value, filter_content);
//
//        delete[] filter_content;
//    }
    strcat(return_value, ACS_NCLIB_GET_TAG::TRAILER);
    
    return return_value;
    
}

void acs_nclib_get::dump_operation(std::string& return_value) const {
    
    return_value = ACS_NCLIB_GET_TAG::HEADER;
    

    switch (datastore) {
        case acs_nclib::DATASTORE_RUNNING:
        	return_value += ACS_NCLIB_DATASTORE_TAG::SOURCE_HEADER;
            return_value += ACS_NCLIB_DATASTORE_TAG::RUNNING;
            return_value += ACS_NCLIB_DATASTORE_TAG::SOURCE_TRAILER;
            break;

        case acs_nclib::DATASTORE_CANDIDATE:
        	return_value += ACS_NCLIB_DATASTORE_TAG::SOURCE_HEADER;
        	return_value += ACS_NCLIB_DATASTORE_TAG::CANDIDATE;
        	return_value += ACS_NCLIB_DATASTORE_TAG::SOURCE_TRAILER;
        	break;

        case acs_nclib::DATASTORE_UNKNOWN:
        default:
            break;
    }
    
    int filters_count = filters.size();

    if (filters_count > 0)
    {
    	std::string filter_content;
    	filters.at(filters_count-1)->dump(filter_content);

    	return_value += "\n";
    	return_value += filter_content;
    }
    

//    for (unsigned int it = 0; it < filters.size(); it++)
//    {
//        std::string filter_content;
//        filters.at(it)->dump(filter_content);
//
//        return_value += "\n";
//        return_value += filter_content;
//
//    }

    return_value += ACS_NCLIB_GET_TAG::TRAILER;
}

void acs_nclib_get::set_source(acs_nclib::Datastore dat)
{
	datastore = dat;
}
