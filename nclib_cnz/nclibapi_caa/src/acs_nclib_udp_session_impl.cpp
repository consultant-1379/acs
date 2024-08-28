/*
 * acs_nclib_udp_session_impl.cpp
 *
 *  Created on: Jul 5, 2013
 *      Author: estevol
 */

#include "acs_nclib_udp_session_impl.h"
#include "acs_nclib_trace.h"

#include "acs_nclib_builder.h"
#include <sys/time.h>
#include <fcntl.h>

#include "ace/INET_Addr.h"
#include "ace/SOCK_Dgram.h"

#define RCV_BUFFER_SIZE 256

#define FD_MAX(max_now,x)     ((x) > (max_now) ? (x) : (max_now))

typedef struct timeval TIMEVAL;

// To avoid warning about unused parameter
#define UNUSED(expr) do { (void)(expr); } while (0)

ACS_NCLIB_TRACE_DEFINE(acs_nclib_udp_session_impl);

int acs_nclib_udp_session_impl::setPassword(const char * pwd){
	ACS_NCLIB_TRACE_FUNCTION;
	UNUSED(pwd);
	return acs_nclib::ERR_NO_ERRORS;
}


acs_nclib_udp_session_impl::~acs_nclib_udp_session_impl()
{
	if (_client)
	{
		close();
	}
}


int acs_nclib_udp_session_impl::open (const char * server_host_addr, uint16_t server_host_port, acs_nclib::UserAuthMethods auth_method, const char * username) {
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

	ACE_SOCK_Dgram stream;

	ACE_INET_Addr addr;

	char target[32] = {0};
	sprintf(target, "%s:%d", server_host_addr, server_host_port);

	addr.set(target);
	_client = new acs_nclib_udp_client(stream, addr);

	set_error_info(acs_nclib::ERR_NO_ERRORS);
	return acs_nclib::ERR_NO_ERRORS;
}

/*
 * Here server_host_addr and server_host_port parameters must be in host-byte-order (human form)
 */
int acs_nclib_udp_session_impl::open (uint32_t server_host_addr, uint16_t server_host_port, acs_nclib::UserAuthMethods auth_method, const char * username) {
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

int acs_nclib_udp_session_impl::send(acs_nclib_message* msg)
{
	ACS_NCLIB_TRACE_FUNCTION;
	if (msg == 0)
	{
		set_error_info(acs_nclib::ERR_NULL_POINTER, "Sending a NULL message");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Send message failed! Trying to send a NULL message");
		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	if (_client == 0)
	{
		set_error_info(acs_nclib::ERR_TCP_CHANNEL_INVALID, "Trying to send over an invalid udp channel");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Send message failed! Trying to send over an invalid udp channel");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	//const char * tmp = 0;
	unsigned char* buffer = 0;

	msg->set_message_id(++_msg_id_count);
//	sprintf(_msgid, "%04d",_msg_id_count);
	ACS_NCLIB_TRACE_MESSAGE("Sending message wiht ID: %d over Netconf session", msg->get_message_id());
	std::string msg_temp;
	msg->dump(msg_temp);	

//	tmp = msg->dump();
	size_t dump_len = 0;
	size_t buf_len = 0;

	size_t i = 0;
	dump_len = msg_temp.length();

	if(dump_len)
	{
		buf_len=dump_len;
		buffer=new unsigned char[buf_len + 1];
		memset(buffer,0,buf_len+1);
		memcpy(buffer,msg_temp.c_str(),buf_len);
	}

	acs_nclib_udp_client::LinkStatus_t status = acs_nclib_udp_client::OK;

	// Send data stored in buffer
	status = _client->send(buffer, buf_len, i);

	if (status != acs_nclib_udp_client::OK) {
		set_error_info(acs_nclib::ERR_UDP_WRITE_FAILED, "Failure on writing a buffer onto the channel");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Send message failed! Failed to write buffer over UDP channel");

		if (buffer)
			delete[] buffer;

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	if (buffer)
		delete[] buffer;

	return acs_nclib::ERR_NO_ERRORS;

}



int acs_nclib_udp_session_impl::receive(acs_nclib_message* &msg, int timeout)
{
	ACS_NCLIB_TRACE_FUNCTION;

	msg = 0;

	if (_client == 0)
	{
		set_error_info(acs_nclib::ERR_UDP_READ_FAILED, "Receiving a message over an invalid channel");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Trying to receive a message over an invalid channel");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	int len = 0;
	size_t rd = 0;

	char * buffer = 0;
	char * tmp = 0;

	int handle = _client->getHandle();

	int result = wait_for_events(1, &handle, timeout);

	if (result == -2)
	{
		set_error_info(acs_nclib::ERR_TIMEOUT_RECEIVE, "Timeout occurred while receiving");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Timeout occurred while receiving");
		return acs_nclib::ERR_SIGNALS_ERROR;
	}
	else if (result == -1 || result != 0)
	{
		set_error_info(acs_nclib::ERR_UDP_READ_FAILED, "Failure on polling on UDP socket");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Failed to poll on UDP socket");
		return acs_nclib::ERR_SIGNALS_ERROR;
	}

	acs_nclib_udp_client::LinkStatus_t status = acs_nclib_udp_client::OK;

	size_t tmp_len = RCV_BUFFER_SIZE - 1;

	{
		buffer = new char[tmp_len + 1];
		memset(buffer,0,tmp_len +1);
		tmp = buffer;
	}
	
	status = _client->read(tmp, tmp_len , len);

	if (len == 0)
	{
		if (status != acs_nclib_udp_client::OK)
		{
			set_error_info(acs_nclib::ERR_UDP_READ_FAILED, "Receiving a message over a channel closed by remote host");
			ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Trying to receive a message over a channel closed by remote host");

			if (buffer)
				delete[] buffer;

			return acs_nclib::ERR_SIGNALS_ERROR;
		}
		else if (status != acs_nclib_udp_client::OK) {
			set_error_info(acs_nclib::ERR_UDP_READ_FAILED, "Failure on reading a buffer from the channel");
			ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Failed to read buffer from UDP channel");
			if (buffer)
				delete[] buffer;
			return acs_nclib::ERR_SIGNALS_ERROR;
		}
	}
	rd =len;
	buffer[rd] = 0;

//	std::cout << "\n new_buf:" << buffer << std::endl;
	msg = acs_nclib_builder::build_udp_reply_message(buffer);
	
	delete [] buffer;

	if (msg == 0)
	{
		std::cout << "\n message size zero" << std::endl;
		set_error_info(acs_nclib::ERR_UDP_READ_FAILED, "Failed to build udp message from buffer content");
		ACS_NCLIB_TRACE_MESSAGE("ERROR: Receive message failed! Failed to build udp message from buffer content");

		return acs_nclib::ERR_SIGNALS_ERROR;
	}


	return acs_nclib::ERR_NO_ERRORS;

}


int acs_nclib_udp_session_impl::close()
{
	ACS_NCLIB_TRACE_FUNCTION;

	int call_result = acs_nclib::ERR_NO_ERRORS;
	set_error_info(acs_nclib::ERR_NO_ERRORS);

	if (_client != 0)
	{
		_client->close();
		delete _client;
		_client = 0;
	}

	return call_result;

}

/*int acs_nclib_udp_session_impl::getId(int& ret)
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
} */

int acs_nclib_udp_session_impl::wait_for_events(int count, int *eventHandles, int timeoutInMilliseconds)
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

int acs_nclib_udp_session_impl::send_sync(acs_nclib_udp* request, acs_nclib_udp_reply* &reply, int timeout)
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

                                if (answer && answer->get_message_id() == request->get_message_id())
                                {
                                        reply = dynamic_cast<acs_nclib_udp_reply*>(answer);
					if (reply == 0)
                                        {
                                                set_error_info(acs_nclib::ERR_REPLY_RCV_FAILED, "Failed to cast answer message as an udp reply");
                                                ACS_NCLIB_TRACE_MESSAGE("ERROR: send_sync() failed! Unable to read udp message received");

                                                return acs_nclib::ERR_SIGNALS_ERROR;
                                        }
                                        else
                                        {
                                                ACS_NCLIB_TRACE_MESSAGE("Expected udp-reply received, send_sync() successfully completed");
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
                                                        "Trying again to receive expected udp-reply, attempt %d", retry);
                                }
                        }
                        else
                        {
                                if (answer)
                                {
                                        acs_nclib_factory::dereference(answer);
                                        answer = 0;
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
                        set_error_info(acs_nclib::ERR_REPLY_RCV_FAILED, "Receive of udp reply message failed");
			return acs_nclib::ERR_SIGNALS_ERROR;
                }

        } else
        {
                return acs_nclib::ERR_SIGNALS_ERROR;
        }

        return acs_nclib::ERR_NO_ERRORS;
}
	

