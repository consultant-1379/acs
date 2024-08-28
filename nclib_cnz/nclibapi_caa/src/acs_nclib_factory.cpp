/*
 * acs_nclib_api.cpp
 *
 *  Created on: Oct 9, 2012
 *      Author: estevol
 */

#include "acs_nclib_trace.h"
#include "acs_nclib_factory.h"
#include "acs_nclib_get.h"
#include "acs_nclib_get_config.h"
#include "acs_nclib_edit_config.h"
#include "acs_nclib_filter_impl.h"
#include "acs_nclib_rpc_reply_impl.h"
#include "acs_nclib_rpc_error_impl.h"
#include "acs_nclib_hello_impl.h"
#include "acs_nclib_close_session.h"
#include "acs_nclib_commit.h"
#include "acs_nclib_lock.h"
#include "acs_nclib_unlock.h"
#include "acs_nclib_action.h"
#include "acs_nclib_udp_impl.h"
#include "acs_nclib_udp_reply_impl.h"

ACS_NCLIB_TRACE_DEFINE(acs_nclib_factory);


acs_nclib_factory::acs_nclib_factory() {


}

acs_nclib_factory::~acs_nclib_factory() {

}

acs_nclib_rpc* acs_nclib_factory::create_rpc(acs_nclib::Operation op)
{
	return create_rpc(op, acs_nclib::DATASTORE_UNKNOWN);
}

acs_nclib_rpc* acs_nclib_factory::create_rpc(acs_nclib::Operation op, acs_nclib::Datastore ds)
{
	ACS_NCLIB_TRACE_FUNCTION;
	//	ACS_NCLIB_TRACE_MESSAGE("ERROR: Create rpc failed! Unsupported operation.");
	switch(op)
	{
	case acs_nclib::OP_GET:
		return new acs_nclib_get(ds);
	case acs_nclib::OP_GETCONFIG:
		return new acs_nclib_get_config(ds);
	case acs_nclib::OP_EDITCONFIG:
		return new acs_nclib_edit_config(ds);
	case acs_nclib::OP_CLOSESESSION:
		return new acs_nclib_close_session();
	case acs_nclib::OP_COMMIT:
		return new acs_nclib_commit();
	case acs_nclib::OP_LOCK:
			return new acs_nclib_lock(ds);
	case acs_nclib::OP_UNLOCK:
			return new acs_nclib_unlock(ds);
	case acs_nclib::OP_ACTION:
			return new acs_nclib_action();
	default:
		return 0;
	}
}

void acs_nclib_factory::dereference(acs_nclib_message* instance)
{
	if(instance)
		delete instance;
}

acs_nclib_filter* acs_nclib_factory::create_filter(acs_nclib::FilterType filter_type)
{
	return new acs_nclib_filter_impl(filter_type);

}

void acs_nclib_factory::dereference(acs_nclib_filter* instance)
{
	if(instance)
		delete instance;
}

acs_nclib_rpc_reply* acs_nclib_factory::create_rpc_reply(acs_nclib::ReplyType rep_type)
{
	return new acs_nclib_rpc_reply_impl(rep_type);
}


acs_nclib_rpc_error* acs_nclib_factory::create_rpc_error(acs_nclib::ErrorType err_type, acs_nclib::ErrorSeverity err_sev)
{
	return new acs_nclib_rpc_error_impl(err_type, err_sev);
}

void acs_nclib_factory::dereference(acs_nclib_rpc_error* instance)
{
	if(instance)
		delete instance;
}

void acs_nclib_factory::dereference(std::vector<acs_nclib_rpc_error*> inst_vec)
{
	for (unsigned int it = 0; it < inst_vec.size(); it++)
	{
		dereference(inst_vec.at(it));
	}
}

acs_nclib_hello* acs_nclib_factory::create_hello()
{
    return new acs_nclib_hello_impl();
}


acs_nclib_udp* acs_nclib_factory::create_udp(acs_nclib::OperationUDP op)
{
		
	switch(op)
	{
		case acs_nclib::OP_UDP_GET:
			return new acs_nclib_udp_impl();
		default:
			return 0;
	}
}

acs_nclib_udp_reply* acs_nclib_factory::create_udp_reply(acs_nclib::OperationUDP op)
{
	switch(op)
        {
                case acs_nclib::OP_UDP_REPLY:
                        return new acs_nclib_udp_reply_impl();
                default:
                        return 0;
        }
}
