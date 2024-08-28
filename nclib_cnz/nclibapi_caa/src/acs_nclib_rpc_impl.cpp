//
//  acs_nclib_rpc_impl.cpp
//  NetConf
//
//  Created by Stefano Volpe on 06/10/12.
//  Copyright (c) 2012 Stefano Volpe. All rights reserved.
//

#include "acs_nclib_rpc_impl.h"
#include "acs_nclib_utils.h"
#include "acs_nclib_get.h"
#include "acs_nclib_get_config.h"
#include "acs_nclib_edit_config.h"
#include <sstream>


acs_nclib_rpc_impl::acs_nclib_rpc_impl(acs_nclib::Operation operType, unsigned int msgId):
acs_nclib_rpc(msgId),
operation(operType)
{
    switch (operation) {

        case acs_nclib::OP_GET:
        case acs_nclib::OP_GETCONFIG:
            rpc_type = acs_nclib::RPC_DATASTORE_READ;
            break;
        case (acs_nclib::OP_EDITCONFIG):
        case (acs_nclib::OP_COPYCONFIG):
        case (acs_nclib::OP_DELETECONFIG):
        case (acs_nclib::OP_LOCK):
        case (acs_nclib::OP_UNLOCK):
        case (acs_nclib::OP_COMMIT):
            rpc_type = acs_nclib::RPC_DATASTORE_WRITE;
            break;
        case (acs_nclib::OP_CLOSESESSION):
        case (acs_nclib::OP_KILLSESSION):
            rpc_type = acs_nclib::RPC_SESSION;
            break;

        default:
            rpc_type = acs_nclib::RPC_UNKNOWN;
            break;

    }
}

acs_nclib_rpc_impl::~acs_nclib_rpc_impl()
{

}

acs_nclib::Operation acs_nclib_rpc_impl::get_operation() const
{
    return operation;
}

//void acs_nclib_rpc_impl::set_operation(acs_nclib::Operation new_operation)
//{
//    operation = new_operation;
//}

acs_nclib::RPCType acs_nclib_rpc_impl::get_rpc_type() const
{
    return rpc_type;
}

const char* acs_nclib_rpc_impl::dump() const
{
    char * dump_op = dump_operation();
    unsigned long dump_size = 512 + strlen(dump_op);
    
    char* ret = new char[dump_size];
    memset(ret,0,dump_size);

    strcat(ret, ACS_NCLIB_XML_TAG::XML_HEADER);
    
    unsigned int msg_id = get_message_id();

    if (msg_id != 0) {
        strcat(ret, ACS_NCLIB_RPC_TAG::MESSAGEID_HEADER);
        strcat(ret, "\"");

        char id[32] = {0};
        sprintf(id,"%d",msg_id);

        strcat(ret, id);
        strcat(ret, "\" ");
    } else {
		strcat(ret,ACS_NCLIB_RPC_TAG::HEADER);
	}
    
    strcat(ret, ACS_NCLIB_NS_TAG::NAMESPACE_1_0);

    strcat(ret, dump_op);
    strcat(ret, ACS_NCLIB_RPC_TAG::TRAILER);

    delete[] dump_op;

    return ret;
}

void acs_nclib_rpc_impl::dump(std::string& ret) const
{

	ret = ACS_NCLIB_XML_TAG::XML_HEADER;
	unsigned int msg_id = get_message_id();

	if (msg_id != 0) {
		ret += ACS_NCLIB_RPC_TAG::MESSAGEID_HEADER;

		std::stringstream ss;
		ss << msg_id;

		ret += "\"" + ss.str() + "\" ";
	} else {
		ret += ACS_NCLIB_RPC_TAG::HEADER;
	}

    ret += ACS_NCLIB_NS_TAG::NAMESPACE_1_0;

    std::string dump_op;
    dump_operation(dump_op);

    ret += dump_op;
    ret += ACS_NCLIB_RPC_TAG::TRAILER;

}

