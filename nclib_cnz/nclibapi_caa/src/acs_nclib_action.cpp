/*
 * acs_nclib_action.cpp
 *
 *  Created on: Dec 5, 2012
 *      Author: estevol
 */

#include "acs_nclib_action.h"
#include "acs_nclib_utils.h"

acs_nclib_action::acs_nclib_action():
acs_nclib_rpc_impl(acs_nclib::OP_ACTION),
_namespace(0),
_data(0)
{

}

acs_nclib_action::~acs_nclib_action()
{
	if (_namespace)
	{
		delete[] _namespace;
		_namespace = 0;
	}

	if (_data)
	{
		delete[] _data;
		_data = 0;
	}

}

void acs_nclib_action::set_data(const char* new_val)
{
	if (_data)
	{
		delete[] _data;
		_data = 0;
	}

	if (new_val != 0)
	{
		unsigned long len = strlen(new_val)+1;

		_data = new char[len];

		memset(_data,0,len);
		memcpy(_data,new_val,len-1);
	}
}

void acs_nclib_action::set_action_namespace(const char* new_val)
{

	if (_namespace)
	{
		delete[] _namespace;
		_namespace = 0;
	}

	if (new_val != 0)
	{
		unsigned long len = strlen(new_val)+1;

		_namespace = new char[len];

		memset(_namespace,0,len);
		memcpy(_namespace,new_val,len-1);
	}

}


char* acs_nclib_action::dump_operation() const
{
	unsigned long data_size = 0;
	unsigned long ns_size = 0;

	if (_data)
	{
		data_size = strlen(_data);
	}

	if (_namespace)
	{
		ns_size = strlen(_namespace);
	}


	char* return_value = new char[512 + data_size + ns_size];
	memset(return_value, 0, 512 + data_size + ns_size);

	if (_namespace)
	{
		strcat(return_value, ACS_NCLIB_ACTION_TAG::NS_HEADER);
		strcat(return_value, _namespace);
		strcat(return_value, ACS_NCLIB_ACTION_TAG::NS_HEADER_CLOSE);
	}
	else
	{
		strcat(return_value, ACS_NCLIB_ACTION_TAG::HEADER);
	}

	if (_data)
	{
		strcat(return_value, ACS_NCLIB_ACTION_TAG::DATA_HEADER);
		strcat(return_value, _data);
		strcat(return_value, ACS_NCLIB_ACTION_TAG::DATA_TRAILER);
	}

	strcat(return_value, ACS_NCLIB_ACTION_TAG::TRAILER);

	return return_value;
}

void acs_nclib_action::dump_operation(std::string& ret) const
{
	char * tmp = dump_operation();
	ret = tmp;

	delete[] tmp;
}
