//
//  acs_nclib_edit_config.cpp
//  NetConf
//
//  Created by Stefano Volpe on 06/10/12.
//  Copyright (c) 2012 Stefano Volpe. All rights reserved.
//

#include "acs_nclib_edit_config.h"
#include "acs_nclib_utils.h"
#include <stdlib.h>

acs_nclib_edit_config::acs_nclib_edit_config(acs_nclib::Datastore ds, acs_nclib::DefaultOperation def_op,
		acs_nclib::TestOption test_op, acs_nclib::ErrorOption err_op):
		acs_nclib_rpc_impl(acs_nclib::OP_EDITCONFIG),
		datastore(ds),
		default_operation(def_op),
		test_option(test_op),
		error_option(err_op),
		configuration(0),
		config_size(0)
{

}

acs_nclib_edit_config::acs_nclib_edit_config(const acs_nclib_edit_config& new_conf):
				acs_nclib_rpc_impl(acs_nclib::OP_EDITCONFIG),
				configuration(0),
				config_size(0)
{
	*this = new_conf;
}

acs_nclib_edit_config::~acs_nclib_edit_config()
{
    if (configuration)
    {
        free(configuration);
        configuration = 0;
        config_size = 0;
    }
}

void acs_nclib_edit_config::set_config(const char* new_config)
{
    if (configuration)
    {
        free(configuration);
        configuration = 0;
        config_size = 0;
    }
    
    if (new_config != 0) {
    	config_size = strlen(new_config) + 1;
    	configuration = (char*) malloc(config_size * sizeof(char));

    	memset(configuration, 0, config_size);
    	memcpy(configuration, new_config, config_size - 1);
    }
}

char* acs_nclib_edit_config::dump_operation() const
{
    
    unsigned long dump_size = config_size + 512;
    char* return_value = new char[dump_size];
    memset(return_value, 0, dump_size);
    
    strcpy(return_value, ACS_NCLIB_EDITCONFIG_TAG::HEADER);

    switch (datastore) {
        case acs_nclib::DATASTORE_RUNNING:
        	strcat(return_value, ACS_NCLIB_DATASTORE_TAG::TARGET_HEADER);
            strcat(return_value, ACS_NCLIB_DATASTORE_TAG::RUNNING);
            strcat(return_value, ACS_NCLIB_DATASTORE_TAG::TARGET_TRAILER);
            break;
            
        case acs_nclib::DATASTORE_CANDIDATE:
        	strcat(return_value, ACS_NCLIB_DATASTORE_TAG::TARGET_HEADER);
        	strcat(return_value, ACS_NCLIB_DATASTORE_TAG::CANDIDATE);
        	strcat(return_value, ACS_NCLIB_DATASTORE_TAG::TARGET_TRAILER);
        	break;

        case acs_nclib::DATASTORE_UNKNOWN:
        default:
            break;
    }
    

    switch (default_operation) {
        case acs_nclib::DEF_OP_MERGE:
            strcat(return_value, "\n");
            strcat(return_value, ACS_NCLIB_DEFAULT_OPERATION_TAG::MERGE);
            break;
            
        case acs_nclib::DEF_OP_REPLACE:
            strcat(return_value, "\n");
            strcat(return_value, ACS_NCLIB_DEFAULT_OPERATION_TAG::REPLACE);
            break;
            
        case acs_nclib::DEF_OP_NONE:
            strcat(return_value, "\n");
            strcat(return_value, ACS_NCLIB_DEFAULT_OPERATION_TAG::NONE);
            break;
        case acs_nclib::DEF_OP_UNDEFINED:
        default:
            strcat(return_value, "\n");
            break;
    }
    
    switch (error_option) {
        case acs_nclib::CONTINUE_ON_ERROR:
            strcat(return_value,ACS_NCLIB_ERROR_OPTION_TAG::CONTINUE);
            break;
        case acs_nclib::ROLLBACK_ON_ERROR:
            strcat(return_value,ACS_NCLIB_ERROR_OPTION_TAG::ROLLBACK);
            break;
        case acs_nclib::STOP_ON_ERROR:
            strcat(return_value,ACS_NCLIB_ERROR_OPTION_TAG::STOP);
            break;
        case acs_nclib::ERR_OP_UNDEFINED:
        default:
            break;
    }
    
    switch (test_option) {
        case acs_nclib::TEST_OP_SET:
            strcat(return_value,ACS_NCLIB_TEST_OPTION_TAG::SET);
            break;
        case acs_nclib::TEST_OP_TEST_THEN_SET:
            strcat(return_value,ACS_NCLIB_TEST_OPTION_TAG::TEST_THEN_SET);
            break;
        case acs_nclib::TEST_OP_TEST_ONLY:
        	strcat(return_value,ACS_NCLIB_TEST_OPTION_TAG::TEST_ONLY);
        	break;
        case acs_nclib::TEST_OP_UNDEFINED:
        default:
            break;
    }
    
    if (config_size > 0)
    {
    	strcat(return_value, ACS_NCLIB_CONFIG_TAG::HEADER_1_0);
    	strcat(return_value, configuration);
    	strcat(return_value, ACS_NCLIB_CONFIG_TAG::TRAILER);
    }
    
    
    strcat(return_value, ACS_NCLIB_EDITCONFIG_TAG::TRAILER);
    
    return return_value;
    
}

void acs_nclib_edit_config::dump_operation(std::string& return_value) const {
    
    return_value = ACS_NCLIB_EDITCONFIG_TAG::HEADER;
    

    switch (datastore) {
        case acs_nclib::DATASTORE_RUNNING:
        	return_value += ACS_NCLIB_DATASTORE_TAG::TARGET_HEADER;
            return_value += ACS_NCLIB_DATASTORE_TAG::RUNNING;
            return_value += ACS_NCLIB_DATASTORE_TAG::TARGET_TRAILER;
            break;
        case acs_nclib::DATASTORE_CANDIDATE:
        	return_value += ACS_NCLIB_DATASTORE_TAG::TARGET_HEADER;
        	return_value += ACS_NCLIB_DATASTORE_TAG::CANDIDATE;
        	return_value += ACS_NCLIB_DATASTORE_TAG::TARGET_TRAILER;
        	break;

        case acs_nclib::DATASTORE_UNKNOWN:
        default:
            break;
    }
    
    switch (default_operation) {
        case acs_nclib::DEF_OP_MERGE:
            return_value += "\n";
            return_value += ACS_NCLIB_DEFAULT_OPERATION_TAG::MERGE;
            break;
            
        case acs_nclib::DEF_OP_REPLACE:
            return_value += "\n";
            return_value += ACS_NCLIB_DEFAULT_OPERATION_TAG::REPLACE;
            break;
            
        case acs_nclib::DEF_OP_NONE:
            return_value += "\n";
            return_value += ACS_NCLIB_DEFAULT_OPERATION_TAG::NONE;
            break;
        case acs_nclib::DEF_OP_UNDEFINED:
        default:
            return_value += "\n";
            break;
    }
    
    switch (error_option) {
        case acs_nclib::CONTINUE_ON_ERROR:
            return_value += ACS_NCLIB_ERROR_OPTION_TAG::CONTINUE;
            break;
        case acs_nclib::ROLLBACK_ON_ERROR:
            return_value += ACS_NCLIB_ERROR_OPTION_TAG::ROLLBACK;
            break;
        case acs_nclib::STOP_ON_ERROR:
            return_value += ACS_NCLIB_ERROR_OPTION_TAG::STOP;
            break;
        case acs_nclib::ERR_OP_UNDEFINED:
        default:
            break;
    }
    
    switch (test_option) {
        case acs_nclib::TEST_OP_SET:
            return_value += ACS_NCLIB_TEST_OPTION_TAG::SET;
            break;
        case acs_nclib::TEST_OP_TEST_THEN_SET:
            return_value += ACS_NCLIB_TEST_OPTION_TAG::TEST_THEN_SET;
            break;
        case acs_nclib::TEST_OP_TEST_ONLY:
        	return_value += ACS_NCLIB_TEST_OPTION_TAG::TEST_ONLY;
        	break;
        case acs_nclib::TEST_OP_UNDEFINED:
        default:
            break;
    }
    
    if (config_size > 0)
    {
    	return_value += ACS_NCLIB_CONFIG_TAG::HEADER_1_0;
    	return_value += configuration;
    	return_value += ACS_NCLIB_CONFIG_TAG::TRAILER;
    }
    
    
    return_value += ACS_NCLIB_EDITCONFIG_TAG::TRAILER;
}


acs_nclib_edit_config& acs_nclib_edit_config::operator=(const acs_nclib_edit_config& new_conf)
{
	datastore = new_conf.datastore;
	default_operation = new_conf.default_operation;
	test_option = new_conf.test_option;
	error_option = new_conf.error_option;
	set_config(new_conf.configuration);

	return *this;
}

void acs_nclib_edit_config::set_target(acs_nclib::Datastore dat)
{
	datastore = dat;
}

void acs_nclib_edit_config::set_default_operation(acs_nclib::DefaultOperation new_val)
{
    default_operation = new_val;
}

void acs_nclib_edit_config::set_test_option(acs_nclib::TestOption new_val)
{
    test_option = new_val;
}

void acs_nclib_edit_config::set_error_option(acs_nclib::ErrorOption new_val)
{
    error_option = new_val;
}

