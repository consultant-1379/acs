//
//  acs_nclib_rpc_reply_impl.cpp
//  NetConf
//
//  Created by Stefano Volpe on 06/10/12.
//  Copyright (c) 2012 Stefano Volpe. All rights reserved.
//

#include <stdlib.h>
#include "acs_nclib_rpc_reply_impl.h"
#include "acs_nclib_utils.h"
#include "acs_nclib_rpc_error_impl.h"
#include <sstream>

acs_nclib_rpc_reply_impl::acs_nclib_rpc_reply_impl(acs_nclib::ReplyType rep, unsigned int msgId):
acs_nclib_rpc_reply(msgId),
reply_type(rep),
data(0),
data_size(0)
{

}

acs_nclib_rpc_reply_impl::~acs_nclib_rpc_reply_impl()
{
	if (data)
	{
		free(data);
		data = 0;
		data_size = 0;
	}

	clear_rpc_errors();

}

acs_nclib::ReplyType acs_nclib_rpc_reply_impl::get_reply_type() const
{
	return reply_type;
}

void acs_nclib_rpc_reply_impl::set_reply_type(acs_nclib::ReplyType rep)
{
	reply_type = rep;
}

void acs_nclib_rpc_reply_impl::add_rpc_error(acs_nclib_rpc_error* new_error)
{
	acs_nclib_rpc_error* copy = new acs_nclib_rpc_error_impl(*new_error);
	errors.push_back(copy);
}

void acs_nclib_rpc_reply_impl::clear_rpc_errors()
{
	for (unsigned int it = 0; it < errors.size(); it++)
	{
		delete(errors.at(it));
	}
	errors.clear();
}

void acs_nclib_rpc_reply_impl::set_data(const char *new_data)
{
	if (data)
	{
		free(data);
		data = 0;
		data_size = 0;
	}

	if (new_data != 0)
	{
		data_size = strlen(new_data) + 1;
		data = (char*) malloc(data_size * sizeof(char));
		memset(data,0,data_size);
		memcpy(data, new_data, data_size - 1);
	}
}

void acs_nclib_rpc_reply_impl::get_data(std::string & ret_val) const
{
	if (data)
		ret_val = data;
	else
		ret_val = "\0";
}

const char* acs_nclib_rpc_reply_impl::get_data() const
{
	if (data)
		return data;
	else
		return "\0";
}

const char* acs_nclib_rpc_reply_impl::dump() const {

	char * ret = 0;
	unsigned long cont_size = 0;
	char** err_dumps = 0;
	char* cont = 0;

	switch(reply_type) {
	case acs_nclib::REPLY_UNKNOWN:
		cont = new char('\0');
		break;
	case acs_nclib::REPLY_OK:
		cont_size = strlen(ACS_NCLIB_RPC_REPLY_TAG::OK) + 1;
		cont = new char[cont_size];
		memset(cont,0,cont_size);
		memcpy(cont, ACS_NCLIB_RPC_REPLY_TAG::OK, cont_size-1);
		break;
	case acs_nclib::REPLY_DATA:
	case acs_nclib::REPLY_DATA_WITH_ERRORS:
		cont_size = strlen(ACS_NCLIB_RPC_REPLY_TAG::DATA_HEADER) + strlen(ACS_NCLIB_RPC_REPLY_TAG::DATA_TRAILER) + strlen(get_data()) + 1;

		cont = new char[cont_size];
		memset(cont,0,cont_size);

		strcat(cont,ACS_NCLIB_RPC_REPLY_TAG::DATA_HEADER);
		strcat(cont,get_data());
		strcat(cont,ACS_NCLIB_RPC_REPLY_TAG::DATA_TRAILER);

	case acs_nclib::REPLY_ERROR:
		if (errors.size() > 0)
		{

			char* err_cont = 0;
			unsigned long err_cont_size = 0;

			err_dumps = new char*[errors.size() + 1];
			err_dumps[errors.size()] = 0;

			for (unsigned int i = 0; i < errors.size(); i++)
			{
				err_dumps[i] = errors.at(i)->dump();
				err_cont_size += strlen(err_dumps[i]);

				if (i>0)
					cont_size++;
			}
			err_cont_size++;
			err_cont = new char[err_cont_size];
			memset(err_cont,0,err_cont_size);

			for (unsigned int i = 0; err_dumps[i] != 0; i++)
			{
				strcat(err_cont,err_dumps[i]);
				delete[] err_dumps[i];
			}
			delete[] err_dumps;

			if (cont == 0)
			{
				cont = err_cont;
				cont_size = err_cont_size;
			}
			else
			{
				unsigned long temp_size = cont_size + err_cont_size +1;
				char* temp = new char[temp_size];
				memset(temp, 0, temp_size);
				strcat(temp,cont);
				strcat(temp,err_cont);
				delete [] cont;
				delete [] err_cont;

				cont = temp;
				cont_size = temp_size;
			}
		}
		break;
	}

	if (cont != 0)
	{
		unsigned long dump_size = cont_size + 512;
		ret = new char[dump_size];
		memset(ret, 0, dump_size);

		unsigned int msg_id = get_message_id();

		strcat(ret, ACS_NCLIB_XML_TAG::XML_HEADER);
		if (msg_id != 0) {
			strcat(ret,ACS_NCLIB_RPC_REPLY_TAG::MESSAGEID_HEADER);
			strcat(ret,"\"");

			char id[32] = {0};
			sprintf(id,"%d",msg_id);

			strcat(ret, id);
			strcat(ret,"\" ");
		} else {
			strcat(ret,ACS_NCLIB_RPC_REPLY_TAG::HEADER);
		}

		strcat(ret,ACS_NCLIB_NS_TAG::NAMESPACE_1_0);
		strcat(ret,cont);
		strcat(ret,ACS_NCLIB_RPC_REPLY_TAG::TRAILER);

		delete[] cont;
	}

	return ret;
}

void acs_nclib_rpc_reply_impl::dump(std::string& ret) const{

		ret = ACS_NCLIB_XML_TAG::XML_HEADER;

		unsigned int msg_id = get_message_id();

		if (msg_id != 0) {
			ret += ACS_NCLIB_RPC_REPLY_TAG::MESSAGEID_HEADER;

			std::stringstream ss;
			ss << msg_id;

			ret += "\"" + ss.str() + "\" ";
		} else {
			ret += ACS_NCLIB_RPC_REPLY_TAG::HEADER;
		}

		ret += ACS_NCLIB_NS_TAG::NAMESPACE_1_0;

		switch(reply_type) {
		case acs_nclib::REPLY_UNKNOWN:
			break;
		case acs_nclib::REPLY_OK:
			ret += ACS_NCLIB_RPC_REPLY_TAG::OK;
			break;
		case acs_nclib::REPLY_DATA:
		case acs_nclib::REPLY_DATA_WITH_ERRORS:
		{
			ret += ACS_NCLIB_RPC_REPLY_TAG::DATA_HEADER;
			std::string data_dump;
			get_data(data_dump);
			ret += data_dump;
			ret += ACS_NCLIB_RPC_REPLY_TAG::DATA_TRAILER;
		}
		case acs_nclib::REPLY_ERROR:
			for (unsigned int i = 0; i < errors.size(); i++)
			{
				if (i >0)
					ret += "\n";

				std::string err_dump;
				errors.at(i)->dump(err_dump);
				ret += err_dump;
			}
			break;

		}

		ret += ACS_NCLIB_RPC_REPLY_TAG::TRAILER;
		//ret += "\n";
		//ret += ACS_NCLIB_XML_TAG::XML_END_MSG;

}

void acs_nclib_rpc_reply_impl::get_error_list(std::vector<acs_nclib_rpc_error*>& ret_vec)
{
	for (unsigned int it = 0; it < errors.size(); it++)
	{
		acs_nclib_rpc_error* copy = new acs_nclib_rpc_error_impl(*(errors.at(it)));
		ret_vec.push_back(copy);
	}

}
