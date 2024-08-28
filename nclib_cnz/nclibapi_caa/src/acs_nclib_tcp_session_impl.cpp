/*
 * acs_nclib_tcp_session_impl.cpp
 *
 *  Created on: Jul 5, 2013
 *      Author: estevol
 */

#include "acs_nclib_tcp_session_impl.h"
#include "acs_nclib_trace.h"

#include "acs_nclib_rpc.h"
#include "acs_nclib_rpc_reply.h"
#include "acs_nclib_builder.h"
#include <sys/time.h>
#include <fcntl.h>

#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"

#define RCV_BUFFER_SIZE 256
#define RPC_CLOSE_TAG "]]>]]>"

#define FD_MAX(max_now,x)     ((x) > (max_now) ? (x) : (max_now))

typedef struct timeval TIMEVAL;

// To avoid warning about unused parameter
#define UNUSED(expr) do { (void)(expr); } while (0)

ACS_NCLIB_TRACE_DEFINE(acs_nclib_tcp_session_impl);

int acs_nclib_tcp_session_impl::setPassword(const char * pwd){
	ACS_NCLIB_TRACE_FUNCTION;

	set_error_info(acs_nclib::ERR_NO_ERRORS);

	if (pwd != 0) {
		unsigned long len = strlen(pwd);

		if (len > MAXSIZE_PWD){
			set_error_info(acs_nclib::ERR_BUFFER_EXCEEDED, "Password provided is longer than %d", MAXSIZE_PWD);
			ACS_NCLIB_TRACE_MESSAGE("ERROR: setPassword failed! Password provided is longer than %d", MAXSIZE_PWD);

			return acs_nclib::ERR_SIGNALS_ERROR;
		}

		memset(_pwd, 0, MAXSIZE_PWD + 1);
		memcpy(_pwd, pwd, len);

	}
	else{
		set_error_info(acs_nclib::ERR_NULL_POINTER, "NULL pointer to pwd");
		return acs_nclib::ERR_SIGNALS_ERROR;
	}
	return acs_nclib::ERR_NO_ERRORS;
}


acs_nclib_tcp_session_impl::~acs_nclib_tcp_session_impl()
{
	if (_client)
	{
		close();
	}
}


int acs_nclib_tcp_session_impl::open (const char * server_host_addr, uint16_t server_host_port, acs_nclib::UserAuthMethods auth_method, const char * username) {
	ACS_NCLIB_TRACE_FUNCTION;

	//TODO: Check if authentication method and user name are needed
	UNUSED(auth_method);
	UNUSED(username);

	if (_client)
	{
		_client->close();
		delete _client;
		_client = 0;
	}

	ACE_SOCK_Connector connector;
	ACE_SOCK_Stream stream;

	int connected = -1;

	ACE_INET_Addr addr;

	char target[32] = {0};
	sprintf(target, "%s:%d", server_host_addr, server_host_port);

	addr.set(target);
	ACE_Time_Value timeout(3,0);

	connected = connector.connect(stream, addr, &timeout);

	if (connected == 0)
	{
		_client = new acs_nclib_tcp_client(stream, addr);

		//Hello exchange...
		if (send_hello() != acs_nclib::ERR_NO_ERRORS || receive_hello() != acs_nclib::ERR_NO_ERRORS)
		{
			ACS_NCLIB_TRACE_MESSAGE("ERROR: session open failed! Failed to exchange hello messages with the server");
			_client->close();
			delete _client;
			_client = 0;

			return acs_nclib::ERR_SIGNALS_ERROR;
		}

		set_error_info(acs_nclib::ERR_NO_ERRORS);
		return acs_nclib::ERR_NO_ERRORS;
	}
	else
	{
		ACS_NCLIB_TRACE_MESSAGE("ERROR: session open failed! Connection timeout");

		set_error_info(acs_nclib::ERR_CONNECT_FAILED);
		return acs_nclib::ERR_SIGNALS_ERROR;
	}

}

/*
 * Here server_host_addr and server_host_port parameters must be in host-byte-order (human form)
 */
int acs_nclib_tcp_session_impl::open (uint32_t server_host_addr, uint16_t server_host_port, acs_nclib::UserAuthMethods auth_method, const char * username) {
	ACS_NCLIB_TRACE_FUNCTION;

	char server_host_addr_str[INET_ADDRSTRLEN + 1];
	in_addr_t tempValue = ::htonl(server_host_addr);
	server_host_addr_str[INET_ADDRSTRLEN] = 0;

	if (!::inet_ntop(AF_INET, &tempValue, server_host_addr_str, INET_ADDRSTRLEN))
	{
		set_error_info(acs_nclib::ERR_IP_ADDRESS_INVALID, "The server host IP address is not valid");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: session open failed! The server host IP address %d is not valid", server_host_addr);

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	return open(server_host_addr_str, server_host_port, auth_method, username);

}



int acs_nclib_tcp_session_impl::send_hello()
{
	ACS_NCLIB_TRACE_FUNCTION;
	acs_nclib_hello* hello = 0;
	hello = acs_nclib_factory::create_hello();

	if (hello)
	{
		hello->add_capability("urn:ietf:params:netconf:base:1.0");
		if (send(hello) != acs_nclib::ERR_NO_ERRORS)
		{
			set_error_info(acs_nclib::ERR_HELLO_SND_FAILED, "Failed to send hello message");
			ACS_NCLIB_TRACE_MESSAGE("ERROR: Hello Exchange failed! Failed to send hello message");

			acs_nclib_factory::dereference(hello);
			return acs_nclib::ERR_SIGNALS_ERROR;
		}
		acs_nclib_factory::dereference(hello);
	}
	else
	{
		set_error_info(acs_nclib::ERR_HELLO_SND_FAILED, "Failed to create hello message");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Hello Exchange failed! Failed to create hello message");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	return acs_nclib::ERR_NO_ERRORS;
}


int acs_nclib_tcp_session_impl::receive_hello()
{
	ACS_NCLIB_TRACE_FUNCTION;
	acs_nclib_message* msg = 0;
	acs_nclib_hello* hello = 0;
	std::vector<std::string> caps;


	if (receive(msg) == acs_nclib::ERR_NO_ERRORS)
	{
		hello = dynamic_cast<acs_nclib_hello*>(msg);

		if (hello)
		{
			_session_id = hello->get_session_id();
			_server_cap.clear();

			hello->get_capabilities(caps);

			for (unsigned int i = 0; i < caps.size(); i++)
			{
				std::string cap = caps.at(i);
				char* tmp = new char[cap.size() + 1];
				memset(tmp,0,cap.size() + 1);
				memcpy(tmp,cap.c_str(),cap.size());

				_server_cap.push_back(tmp);
			}
		}
		else
		{
			set_error_info(acs_nclib::ERR_HELLO_RCV_FAILED, "Failed to receive hello message from server");
			ACS_NCLIB_TRACE_MESSAGE("ERROR: Hello Exchange failed! Failed to receive hello message from server");

			if (msg)
				acs_nclib_factory::dereference(msg);
			return acs_nclib::ERR_SIGNALS_ERROR;
		}
	}
	else
	{
		set_error_info(acs_nclib::ERR_HELLO_RCV_FAILED, "Failed to receive hello message from server");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Hello Exchange failed! Failed to receive hello message from server");

		if (msg)
			acs_nclib_factory::dereference(msg);
		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	if (msg)
		acs_nclib_factory::dereference(msg);

	return acs_nclib::ERR_NO_ERRORS;
}


int acs_nclib_tcp_session_impl::send(acs_nclib_message* msg)
{
	ACS_NCLIB_TRACE_FUNCTION;
	if (msg == 0)
	{
		set_error_info(acs_nclib::ERR_NULL_POINTER, "Sending a NULL rpc message");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Send message failed! Trying to send a NULL rpc message");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	if (_client == 0)
	{
		set_error_info(acs_nclib::ERR_TCP_CHANNEL_INVALID, "Trying to send over an invalid TCP channel");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Send message failed! Trying to send over an invalid TCP channel");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	const char * tmp = 0;
	char* buffer = 0;

	//Assign a new message_id to the message
	msg->set_message_id(++_msg_id_count);

	ACS_NCLIB_TRACE_MESSAGE("Sending message wiht ID: %d over Netconf session with ID: %d", msg->get_message_id(), _session_id);

	tmp = msg->dump();
	size_t dump_len = 0;
	size_t buf_len = 0;

	//TODO: Check if we can use a fixed size buffer
	if (tmp)
	{
		dump_len = strlen(tmp);
		buf_len = dump_len + strlen(RPC_CLOSE_TAG);
		buffer = new char[buf_len + 1];

		memset(buffer,0,buf_len + 1);
		memcpy(buffer,tmp,dump_len);
		delete[] tmp;
	}
	else
	{
		set_error_info(acs_nclib::ERR_RPC_DUMP_FAILED, "Failure on dumping an rpc message");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Send message failed! Failure on dumping the rpc message %d", msg->get_message_id());

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	strcat(buffer,RPC_CLOSE_TAG);
	//buf_len = strlen(buffer);

	size_t i = 0;
	size_t wr = 0;

	acs_nclib_tcp_client::LinkStatus_t status = acs_nclib_tcp_client::OK;

	do {
		// Send data stored in buffer
		status = _client->send(buffer, buf_len, i);

		if (status != acs_nclib_tcp_client::OK) {
			set_error_info(acs_nclib::ERR_TCP_CHANNEL_WRITE_FAILED, "Failure on writing a buffer onto the channel");
			ACS_NCLIB_TRACE_MESSAGE("ERROR: Send message failed! Failed to write buffer over TCP channel");

			if (buffer)
				delete[] buffer;

			return acs_nclib::ERR_SIGNALS_ERROR;
		}
		wr += i;
	} while (status == acs_nclib_tcp_client::OK && wr < buf_len);

	if (buffer)
		delete[] buffer;

	return acs_nclib::ERR_NO_ERRORS;

}



int acs_nclib_tcp_session_impl::receive(acs_nclib_message* &msg, int timeout)
{
	ACS_NCLIB_TRACE_FUNCTION;

	msg = 0;
	timeval t1, t2;
	double elapsedTime;

	if (_client == 0)
	{
		set_error_info(acs_nclib::ERR_TCP_CHANNEL_INVALID, "Receiving a message over an invalid channel");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Trying to receive a message over an invalid channel");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	int len = 0;
	size_t rd = 0;
	char  *specialsequence = 0;

	char * buffer = 0;
	char * tmp = 0;

	gettimeofday(&t1, 0);

	if (_previous_buffer)
	{
		buffer = _previous_buffer;
		_previous_buffer = 0;
	}

	int handle = _client->getHandle();

	int result = wait_for_events(1, &handle, timeout);

	if (result == -2)
	{
		set_error_info(acs_nclib::ERR_TIMEOUT_RECEIVE, "Timeout occurred while receiving");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Timeout occurred while receiving");
		std::cout << "Timeout Occurred\n";

		return acs_nclib::ERR_SIGNALS_ERROR;
	}
	else if (result == -1 || result != 0)
	{
		set_error_info(acs_nclib::ERR_TCP_CHANNEL_READ_FAILED, "Failure on polling on TCP socket");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Failed to poll on TCP socket");
		return acs_nclib::ERR_SIGNALS_ERROR;
	}


	acs_nclib_tcp_client::LinkStatus_t status = acs_nclib_tcp_client::OK;

	unsigned int tries = 0;


	while (!specialsequence) {

		rd = 0;
		size_t tmp_len = RCV_BUFFER_SIZE - 1;

		if (buffer)
		{
			char* new_buffer = new char[strlen(buffer) + tmp_len + 1];
			memset(new_buffer,0, strlen(buffer) + tmp_len + 1);
			memcpy(new_buffer,buffer,strlen(buffer));

			tmp = new_buffer + strlen(buffer);

			delete[] buffer;
			buffer = new_buffer;
		}
		else
		{
			buffer = new char[tmp_len + 1];
			memset(buffer,0,tmp_len +1);
			tmp = buffer;
		}

		do {

			status = _client->read(tmp + rd, tmp_len - rd, len);

			if (len == 0)
			{
				if (status != acs_nclib_tcp_client::OK)
				{
					set_error_info(acs_nclib::ERR_TCP_CHANNEL_CLOSED_BY_REMOTE_HOST, "Receiving a message over a channel closed by remote host");
					ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Trying to receive a message over a channel closed by remote host");

					if (buffer)
						delete[] buffer;

					return acs_nclib::ERR_SIGNALS_ERROR;
				}

				usleep(NCLIB_RETRY_USLEEP);
				tries++;
			}
			else if (status != acs_nclib_tcp_client::OK) {
				set_error_info(acs_nclib::ERR_TCP_CHANNEL_READ_FAILED, "Failure on reading a buffer from the channel");
				ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Failed to read buffer from TCP channel");

				if (buffer)
					delete[] buffer;

				return acs_nclib::ERR_SIGNALS_ERROR;
			}

			else
			{
				rd += len;
				/* read more data until we see a rpc-reply closing tag followed by
				 * the special sequence ]]>]]> */

				if ((tmp - buffer) >  (int) tmp_len)
				{
					specialsequence = strstr(tmp - tmp_len, RPC_CLOSE_TAG);
				}
				else
				{
					specialsequence = strstr(buffer, RPC_CLOSE_TAG);
				}

			}

			gettimeofday(&t2, 0);

			elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
			elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;

			if(elapsedTime > timeout)
			{
				if (buffer)
				{
					delete[] buffer;
				}
				set_error_info(acs_nclib::ERR_TIMEOUT_RECEIVE, "Timeout occurred while receiving");
				ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Timeout occurred while receiving");

				return acs_nclib::ERR_SIGNALS_ERROR;
			}

		} while (rd < tmp_len && !specialsequence);

	}

	if (!specialsequence) {
		set_error_info(acs_nclib::ERR_RPC_TERM_NOTFOUND, "RPC message terminator not found");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! RPC message terminator not found");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	/* discard the special sequence so that only XML is returned */
	rd = specialsequence - buffer;

	//Save previous buffer for next receive
	char* saved_buffer = specialsequence + strlen(RPC_CLOSE_TAG) + 1;
	size_t prev_buff_len = strlen(saved_buffer);
	if (prev_buff_len > 0)
	{
		_previous_buffer = new char[prev_buff_len + 1];
		memset(_previous_buffer, 0, prev_buff_len + 1);
		memcpy(_previous_buffer, saved_buffer, prev_buff_len);
	}

	buffer[rd] = 0;

	msg = acs_nclib_builder::build_message(buffer);

	delete[] buffer;

	if (msg == 0)
	{
		set_error_info(acs_nclib::ERR_RPC_BUILD_FAILED, "Failed to build rpc message from buffer content");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Failed to build rpc message from buffer content");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}


	return acs_nclib::ERR_NO_ERRORS;

}


int acs_nclib_tcp_session_impl::close()
{
	ACS_NCLIB_TRACE_FUNCTION;

	int call_result = acs_nclib::ERR_NO_ERRORS;
	set_error_info(acs_nclib::ERR_NO_ERRORS);

	if (_client != 0)
	{
		acs_nclib_rpc* rpc = acs_nclib_factory::create_rpc(acs_nclib::OP_CLOSESESSION);

		if (send(rpc) == acs_nclib::ERR_NO_ERRORS)
		{
			acs_nclib_message* answer = 0;
			if (receive(answer) == acs_nclib::ERR_NO_ERRORS && answer->get_msg_type() == acs_nclib::RPC_REPLY_MSG)
			{
				acs_nclib_rpc_reply * reply = dynamic_cast<acs_nclib_rpc_reply*>(answer);
				if (!reply || reply->get_reply_type() != acs_nclib::REPLY_OK)
				{
					set_error_info(acs_nclib::ERR_SESSION_CLOSE_FAILED, "Reply not OK when closing session");
					ACS_NCLIB_TRACE_MESSAGE("ERROR: session close failed! Reply not OK when closing session");

					call_result = acs_nclib::ERR_SIGNALS_ERROR;
				}
				else
				{
					ACS_NCLIB_TRACE_MESSAGE("Session %d successfully closed!", _session_id);
				}

			} else
			{
				set_error_info(acs_nclib::ERR_SESSION_CLOSE_FAILED, "Receive answer to <close-session> rpc failed");
				ACS_NCLIB_TRACE_MESSAGE("ERROR: session close failed! Receive answer to <close-session> rpc failed");

				call_result = acs_nclib::ERR_SIGNALS_ERROR;
			}
			if (answer)
				// free answer message;
				acs_nclib_factory::dereference(answer);
		}
		else{
			//std::cout << "Sending <close-session> rpc failed" << std::endl;
			set_error_info(acs_nclib::ERR_SESSION_CLOSE_FAILED, "Sending <close-session> rpc failed");
			ACS_NCLIB_TRACE_MESSAGE("ERROR: session close failed! Sending <close-session> rpc failed");

			call_result = acs_nclib::ERR_SIGNALS_ERROR;
		}

		if (rpc)
		{
			// free answer message;
			acs_nclib_factory::dereference(rpc);
		}

		_client->close();
		delete _client;
		_client = 0;
	}

	for (unsigned int i = 0; i < _server_cap.size(); i++)
	{
		delete[] (_server_cap.at(i));
	}
	_server_cap.clear();

	//Delete saved buffer
	if (_previous_buffer)
	{
		delete[] _previous_buffer;
		_previous_buffer = 0;
	}

	//Reset the session ID
	_session_id = -1;

	return call_result;

}

int acs_nclib_tcp_session_impl::send_sync(acs_nclib_rpc* request, acs_nclib_rpc_reply* &reply, int timeout)
{
	ACS_NCLIB_TRACE_FUNCTION;

	reply = 0;
	unsigned int retry = 0;
	bool reply_rcvd = false;

	if (send(request) == acs_nclib::ERR_NO_ERRORS)
	{
		acs_nclib_message* answer = 0;

		while (!reply_rcvd)
		{
			if (receive(answer, timeout) == acs_nclib::ERR_NO_ERRORS)
			{

				if (answer && answer->get_msg_type() == acs_nclib::RPC_REPLY_MSG && answer->get_message_id() == request->get_message_id())
				{
					reply = dynamic_cast<acs_nclib_rpc_reply*>(answer);

					if (reply == 0)
					{
						set_error_info(acs_nclib::ERR_REPLY_RCV_FAILED, "Failed to cast answer message as an rpc reply");
						ACS_NCLIB_TRACE_MESSAGE("ERROR: send_sync() failed! Unable to read rpc message received");

						return acs_nclib::ERR_SIGNALS_ERROR;
					}
					else
					{
						ACS_NCLIB_TRACE_MESSAGE("Expected rpc-reply received, send_sync() successfully completed");
						reply_rcvd = true;
					}
				}
				else
				{

					if (answer) {
						acs_nclib_factory::dereference(answer);
						answer = 0;
					}
					retry++;
					ACS_NCLIB_TRACE_MESSAGE("WARNING: Unexpected message received and discarded in send_sync() operation. "
							"Trying again to receive expected rpc-reply, attempt %d", retry);
				}
			}
			else
			{
				if (answer)
				{
					acs_nclib_factory::dereference(answer);
					answer = 0;
				}

				if (last_error_code() == acs_nclib::ERR_RPC_BUILD_FAILED)
				{
					retry++;
					ACS_NCLIB_TRACE_MESSAGE("WARNING: Unrecognized data received and discarded in send_sync() operation. "
							"Trying again to receive expected rpc-reply, attempt %d", retry);
				}
				else
				{
					return acs_nclib::ERR_SIGNALS_ERROR;
				}
			}
		}

		if (!reply_rcvd)
		{
			if (answer)
				acs_nclib_factory::dereference(answer);
			set_error_info(acs_nclib::ERR_REPLY_RCV_FAILED, "Receive of rpc reply message failed");
			return acs_nclib::ERR_SIGNALS_ERROR;
		}

	} else
	{
		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	return acs_nclib::ERR_NO_ERRORS;

}

int acs_nclib_tcp_session_impl::lock(acs_nclib::Datastore ds)
{
	ACS_NCLIB_TRACE_FUNCTION;

	acs_nclib_rpc* lock_rpc = acs_nclib_factory::create_rpc(acs_nclib::OP_LOCK, ds);
	acs_nclib_rpc_reply* reply = 0;

	if (send_sync(lock_rpc, reply) != acs_nclib::ERR_NO_ERRORS || !reply)
	{
		acs_nclib_factory::dereference(lock_rpc);
		return acs_nclib::ERR_SIGNALS_ERROR;
	}
	else
	{
		if (reply->get_reply_type() != acs_nclib::REPLY_OK)
		{
			if (reply->get_reply_type() == acs_nclib::REPLY_ERROR || reply->get_reply_type() == acs_nclib::REPLY_DATA_WITH_ERRORS)
			{
				std::vector<acs_nclib_rpc_error*> errors;

				reply->get_error_list(errors);

				if (errors.size() > 0)
				{
					char err_msg[512] = {0};
					sprintf(err_msg, "LOCK Operation Failed, error type %d: %s. Error Info: %s",
							errors.at(0)->get_error_type(), errors.at(0)->get_error_tag(), errors.at(0)->get_error_info());

					set_error_info(acs_nclib::ERR_LOCK_FAILED, err_msg);
					ACS_NCLIB_TRACE_MESSAGE("ERROR: %s", err_msg);

					acs_nclib_factory::dereference(errors);
				}

			}
			acs_nclib_factory::dereference(lock_rpc);
			acs_nclib_factory::dereference(reply);
			return acs_nclib::ERR_SIGNALS_ERROR;
		}
	}

	acs_nclib_factory::dereference(lock_rpc);
	acs_nclib_factory::dereference(reply);
	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_tcp_session_impl::unlock(acs_nclib::Datastore ds)
{
	ACS_NCLIB_TRACE_FUNCTION;

	acs_nclib_rpc* unlock_rpc = acs_nclib_factory::create_rpc(acs_nclib::OP_UNLOCK, ds);
	acs_nclib_rpc_reply* reply = 0;

	if (send_sync(unlock_rpc, reply) != acs_nclib::ERR_NO_ERRORS || !reply)
	{
		acs_nclib_factory::dereference(unlock_rpc);
		return acs_nclib::ERR_SIGNALS_ERROR;
	}
	else
	{
		if (reply->get_reply_type() != acs_nclib::REPLY_OK)
		{
			if (reply->get_reply_type() == acs_nclib::REPLY_ERROR || reply->get_reply_type() == acs_nclib::REPLY_DATA_WITH_ERRORS)
			{
				std::vector<acs_nclib_rpc_error*> errors;

				reply->get_error_list(errors);

				if (errors.size() > 0)
				{
					char err_msg[512] = {0};
					sprintf(err_msg, "UNLOCK Operation Failed, error type %d: %s. Error Info: %s",
							errors.at(0)->get_error_type(), errors.at(0)->get_error_tag(), errors.at(0)->get_error_info());

					set_error_info(acs_nclib::ERR_UNLOCK_FAILED, err_msg);
					ACS_NCLIB_TRACE_MESSAGE("ERROR: %s", err_msg);

					acs_nclib_factory::dereference(errors);
				}

			}
			acs_nclib_factory::dereference(unlock_rpc);
			acs_nclib_factory::dereference(reply);
			return acs_nclib::ERR_SIGNALS_ERROR;
		}
	}

	acs_nclib_factory::dereference(unlock_rpc);
	acs_nclib_factory::dereference(reply);
	return acs_nclib::ERR_NO_ERRORS;
}


int acs_nclib_tcp_session_impl::getId(int& ret)
{
	if (_client && _session_id >= 0)
	{
		ret = _session_id;
	}
	else
	{
		set_error_info(acs_nclib::ERR_SESSION_INVALID, "Trying to retrieve Session Identifier for an invalid session");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Trying to retrieve Session Identifier for an invalid session");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_tcp_session_impl::getServerCapabilities(std::vector<const char*>& ret)
{
	if (_client && _session_id >= 0)
	{
		ret = _server_cap;
	}
	else
	{
		set_error_info(acs_nclib::ERR_SESSION_INVALID, "Trying to retrieve Capabilities for an invalid session");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Trying to retrieve Capabilities for an invalid session");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	return acs_nclib::ERR_NO_ERRORS;
}

int acs_nclib_tcp_session_impl::wait_for_events(int count, int *eventHandles, int timeoutInMilliseconds)
{
    fd_set fds;
    int nfds = 0;
    struct timeval *tv = NULL;

    // set timeout value (if any)
    if (timeoutInMilliseconds > 0) {
        tv = new struct timeval;

        unsigned long seconds = timeoutInMilliseconds / 1000;
        unsigned long ms = timeoutInMilliseconds % 1000;

        tv->tv_sec = seconds;
        tv->tv_usec = ms * 1000;
    }

    FD_ZERO(&fds);

    // go thru all fd's
    for (int i = 0;i < count; i++) {

       // Executing FD_CLR() or FD_SET() with
       // a value of fd that is negative or is equal to or larger than FD_SETSIZE
       // will result in undefined behavior.
        if (eventHandles[i] < 0) {
        	if (tv != NULL) {
        		// free allocated memory
        		delete tv;
        		tv = NULL;
        	}
           return -1;
        }

        FD_SET(eventHandles[i], &fds);

        // we want the highest fds
        nfds = FD_MAX(nfds, eventHandles[i]);
    }

    // wait for something to happen
    int ret = select(nfds + 1, &fds, NULL, NULL, tv);

    if (tv != NULL) {
        // free allocated memory
        delete tv;
        tv = NULL;
    }

    if (ret == 0) {
        // timeout has occurred
        return -2;

    } else if (ret == -1) {
        // a fault has occurred
        return -1;
    }    // check the list of unnamed events

    int n = -1;

    // find out which one of the fds that has been signaled
    for (int i = 0;i < count; i++) {
        if (FD_ISSET(eventHandles[i], &fds)) {
            n = i;
            break;
        }
    }

    if (n != -1)
    {
        return n;
    }

    // should never come here
    return -1;
}

