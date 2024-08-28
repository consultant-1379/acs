#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "acs_apbm_trace.h"
#include "acs_apbm_ipmiapi_level2_impl.h"


ACS_APBM_TRACE_DEFINE(acs_apbm_ipmiapi);

/*
static void dump (uint8_t * buffer, size_t size, size_t dumping_size, size_t dumping_line_length) {
	size_t output_buffer_size = 2 + 8 + 2 + 3*dumping_line_length + 16;
	char output_buffer[output_buffer_size];
	printf("--------------> output_buffer == ");
	for (size_t i = 0; (i < size) && (i < dumping_size); ) {
		int chars = ::snprintf(output_buffer, output_buffer_size, "  %08zX:", i);
		for (size_t col = 0; (col < dumping_line_length) && (i < size) && (i < dumping_size); ++i, ++col)
			chars += ::snprintf(output_buffer + chars, output_buffer_size - chars, " %02X", buffer[i]);
		printf("%s\n", output_buffer);
	}
}
*/


acs_apbm_ipmiapi_level2_impl::acs_apbm_ipmiapi_level2_impl(const char * device_name){

	if(device_name == 0)
		*_device_name = 0;
	else
		::strncpy(_device_name, device_name, IPMIAPI_DEVICE_NAME_MAX_LEN);

	_dev_fd = -1;
	_l2_send_state = _l2_recv_state = L2STATE_UNDEFINED;
	_p_l2msg_being_sent = 0;
	_l2timeout_send.tv_sec = _l2timeout_send.tv_usec = 0;
	_l2timeout_recv.tv_sec = _l2timeout_recv.tv_usec = 0;
}

acs_apbm_ipmiapi_level2_impl::~acs_apbm_ipmiapi_level2_impl(){

}

l2_message::l2_message() {

	_l2_data_size = 0;
	memset(_l2_data, 0, L2DATA_SIZE_MAX );
}

int acs_apbm_ipmiapi_level2_impl::init(){

	ACS_APBM_TRACE_FUNCTION;

	if (_dev_fd >= 0) {
		/* is already open, skip it... */
		ACS_APBM_TRACE_MESSAGE("Warning: l2_device already opened!");
		return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
	}

	if (!_send_pool.init(SEND_POOL_SIZE)) {
		/* could not initialize l2 send pool, skip it... */
		ACS_APBM_TRACE_MESSAGE("Cannot initialize l2 send pool!");
		return acs_apbm_ipmiapi_ns::ERR_NO_SYSTEM_RESOURCE;
	}

	if (!_recv_pool.init(RECV_POOL_SIZE)) {
		/* could not initialize l2 recv pool, skip it... */
		ACS_APBM_TRACE_MESSAGE("Cannot initialize l2 recv pool!");
		return acs_apbm_ipmiapi_ns::ERR_NO_SYSTEM_RESOURCE;
	}

	/* power op state machines... */
	_l2_send_state = L2STATE_SEND_IDLE;
	_l2_recv_state = L2STATE_RECV_IDLE;

	/* open the device port... */
	_dev_fd = open_device();
	if(_dev_fd < 0)
		return acs_apbm_ipmiapi_ns::ERR_OPEN_DEVICE_FAILURE;

	/*
	 if (set_nonblocking(_dev_fd) < 0) {
		ACS_APBM_TRACE_MESSAGE("Cannot set NON BLOCKING mode on opened device");
		_send_pool.reset();
		_recv_pool.reset();
		close(_dev_fd);
		_dev_fd = -1;
		return acs_apbm_ipmiapi_ns::ERR_GENERIC_ERROR;
	}
	*/

	return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

int acs_apbm_ipmiapi_level2_impl::finalize() {

	ACS_APBM_TRACE_FUNCTION;

	_send_pool.reset();
	_recv_pool.reset();
	close(_dev_fd);
	_dev_fd = -1;
	return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

int acs_apbm_ipmiapi_level2_impl::l2_write(const eri_ipmi_message & l3_message) {

	ACS_APBM_TRACE_FUNCTION;

	l2_message *l2_message = NULL;

	if (_dev_fd < 0) {
		ACS_APBM_TRACE_MESSAGE("Cannot write on serial device: invalid file descriptor !");
		return L2_WRITE_FAILED;
	}

	if (l3_message.size > ERI_IPMI_DATA_MAX_SIZE) {
		ACS_APBM_TRACE_MESSAGE("Cannot write on serial device: l3 message has invalid size !");
		return L2_WRITE_FAILED;
	}

	l2_message = _send_pool.find_empty_buffer();
	if (l2_message == NULL) {
		/* no space in pool, packel2_writet is discarded */
		ACS_APBM_TRACE_MESSAGE("Cannot write on serial device: l2 send queue is full !");
		return L2_WRITE_FAILED_MEMORY;
	}

	l2_message->encode_from_l3_message(l3_message);

	handle_sendstm_event(L2EVENT_SEND_NEXT);

	return L2_WRITE_OK;

}


int acs_apbm_ipmiapi_level2_impl::l2_read() {

	ACS_APBM_TRACE_FUNCTION;

	l2_event_t l2event_send;
	l2_event_t l2event_recv;
	unsigned char read_value;
	int read_result;
	bool done = false;

	if (_dev_fd < 0) {
		ACS_APBM_TRACE_MESSAGE("Error: invalid device fd !");
		return L2_READ_FAILED;
	}

	while (!done) {

		read_result = ::read(_dev_fd, & read_value, 1);

		l2event_send = L2EVENT_NO;
		l2event_recv = L2EVENT_NO;

		switch (read_result) {

		case 1:
			if (read_value == L2BYTE_ACK) {
				l2event_send = L2EVENT_RECEIVED_ACK;
			} else if (read_value == L2BYTE_NACK) {
				l2event_send = L2EVENT_RECEIVED_NACK;
			} else if (read_value >= L2BYTE_LENGTH_MIN && read_value <= L2BYTE_LENGTH_MAX) {
				l2event_recv = L2EVENT_RECEIVED_LENGTH;
			}
			else if (read_value <= L2BYTE_DATA_MAX) {
				l2event_recv = L2EVENT_RECEIVED_DATA;
			} else {
				ACS_APBM_TRACE_MESSAGE("Error: received ILLEGAL BYTE 0x%02x", read_value);
				/* illegal value (0x81, 0x82, 0x83), ignore data */
			}
			break;

		case 0:
			/* no more data to read... */
			ACS_APBM_TRACE_MESSAGE("no more data to read !");
			//printf("no more data to read !\n");
			done = true;
			break;

		case -1:
			ACS_APBM_TRACE_MESSAGE("read failed ! read_result == -1, errno == %d ", errno);
			//printf("read failed ! read_result == -1, errno == %d \n", errno);
			if (errno == EAGAIN) {
				sleep(2);
				done = true;
			} else {
				/* read() returned error, more info in errno... */
				return L2_READ_FAILED;
			}
			break;

		default:
			ACS_APBM_TRACE_MESSAGE("read failed ! read_result == %d, errno == %d ", read_result, errno);
			//printf("read failed ! read_result == %d, errno == %d \n", read_result, errno);
			return L2_READ_FAILED;
		}

		if ((l2event_send == L2EVENT_NO) && tm_isset(& _l2timeout_send) && tm_isexpired(& _l2timeout_send)) {
			l2event_send = L2EVENT_TIMEOUT;
		}

		if ((l2event_recv == L2EVENT_NO) && tm_isset(& _l2timeout_recv) && tm_isexpired(& _l2timeout_recv)) {
			l2event_recv = L2EVENT_TIMEOUT;
		}

		handle_sendstm_event(l2event_send);
		handle_recvstm_event(l2event_recv, read_value);

		if (_recv_pool.find_tail_buffer() != NULL) {
			/* good news, a completely received l2 packet (or more) is ready for delivery to l3 */
			ACS_APBM_TRACE_MESSAGE("l2 packet (or more) is ready for delivery to l3 ");
			//printf("l2 packet (or more) is ready for delivery to l3 \n");
			return L2_READ_L3MESSAGE_READY;
		}
	}	/* while... */

	if ((_l2_send_state == L2STATE_SEND_IDLE) && (_l2_recv_state == L2STATE_RECV_IDLE)) {
		ACS_APBM_TRACE_MESSAGE("returning (L2_READ_COMPLETE)");
		return L2_READ_COMPLETE;
	} else {
		ACS_APBM_TRACE_MESSAGE("returning (L2_READ_CONTINUE)");
		return L2_READ_CONTINUE;
	}

}

int acs_apbm_ipmiapi_level2_impl::getpacket(eri_ipmi_message * l3_message)
{
	ACS_APBM_TRACE_FUNCTION;

	l2_message * l2_message;

	if (l3_message == NULL) {
		ACS_APBM_TRACE_MESSAGE("Error: invalid parameter 'l3_message' !");
		return L2_GETPACKET_FAILED;
	}

	l2_message = _recv_pool.find_tail_buffer();
	if (l2_message == NULL) {
		ACS_APBM_TRACE_MESSAGE("Error: No L2 packet available !");
		return L2_GETPACKET_FAILED;
	}

	/* a received l2message is waiting in the queue to be delivered... */
	int call_result = l2_message->decode_into_l3_message(*l3_message);
	if(call_result) {
		ACS_APBM_TRACE_MESSAGE("Error: unable to decode L2 packet into L3 message. Call 'l2_message::decode_into_l3_message()' returned  %d !", call_result);
	}

	_recv_pool.free_tail_buffer(l2_message);

	/* L2 packet successfully decoded into <l3_message> */
	return (call_result ?  L2_GETPACKET_FAILED: L2_GETPACKET_OK);
}


int acs_apbm_ipmiapi_level2_impl::open_device(){

	ACS_APBM_TRACE_FUNCTION;

	struct termios io_port_attr;

	// int fd = open(_device_name, O_RDWR | O_NOCTTY | O_NONBLOCK);

	int fd = open(_device_name, O_RDWR | O_NOCTTY);

	if (fd < 0) {
		ACS_APBM_TRACE_MESSAGE("Call 'open()' Failed: ret_code == %d, errno == %d", fd, errno);
		return fd;
	}

	memset(&io_port_attr, 0, sizeof(io_port_attr));

	/*  B115200: Set bps rate */
	/*  CRTSCTS : output hardware flow control
	   (only used if the cable has lines) needed???  */
	/*  CS8     : 8n1 (8bit,no parity,1 stopbit) */
	/*  CLOCAL  : local connection, no modem contol */
	/*  CREAD   : enable receiving characters */
	io_port_attr.c_cflag = B115200 | CS8 | CLOCAL | CREAD;

	/*  IGNPAR  : ignore bytes with parity errors */
	/*  BRKINT  : ignore break */
	io_port_attr.c_iflag = IGNPAR | BRKINT;

	/*  Raw output. */
	io_port_attr.c_oflag = 0;

	/* set input mode (non-canonical, no echo,...) */
	io_port_attr.c_lflag = 0;
	io_port_attr.c_cc[VTIME] = 0;	/* inter-character timer unused */
	io_port_attr.c_cc[VMIN] = 1;	/* blocking read until 1 char is received */

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &io_port_attr);

	return fd;
}

int acs_apbm_ipmiapi_level2_impl::handle_sendstm_event(l2_event_t event) {

	ACS_APBM_TRACE_FUNCTION;
	ACS_APBM_TRACE_MESSAGE("Handling event '%d' (%s) ! Current Send State is '%d'", event, str_event(event),_l2_send_state);

	switch (event) {
		case L2EVENT_NO:
			/* no event */
		break;

		case L2EVENT_SEND_NEXT:
			switch (_l2_send_state) {
				case L2STATE_SEND_IDLE:
					_p_l2msg_being_sent = _send_pool.find_tail_buffer();
					if (_p_l2msg_being_sent != NULL) {
						_l2_send_state = L2STATE_SEND_WAIT_ACK;
						tm_set(& _l2timeout_send, L2TIMEOUT_SEND_WAIT_ACK);
						send_l2message(_p_l2msg_being_sent);
					}
				break;

				case L2STATE_SEND_WAIT_ACK:
					/* ignore event, we're busy sendind already, try again later */
				break;

				default:
					ACS_APBM_TRACE_MESSAGE("illegal send state %d", _l2_send_state);
					_l2_send_state = L2STATE_SEND_IDLE;
					tm_clear(& _l2timeout_send);
			}

		break;

		case L2EVENT_RECEIVED_ACK:
			switch (_l2_send_state) {
				case L2STATE_SEND_IDLE:
					/* ignore event */
					tm_clear(&_l2timeout_send);
				break;

				case L2STATE_SEND_WAIT_ACK:
					// ACK received. Check if there are other L2 messages to send
					_send_pool.free_tail_buffer(_p_l2msg_being_sent);
					_p_l2msg_being_sent = _send_pool.find_tail_buffer();
					if (_p_l2msg_being_sent){
						// found another L2 message to send. Send it and wait for ACK !
						_l2_send_state = L2STATE_SEND_WAIT_ACK;
						tm_set(& _l2timeout_send, L2TIMEOUT_SEND_WAIT_ACK);
						send_l2message(_p_l2msg_being_sent);
					}
					else {
						// no more L2 message to send
						_l2_send_state = L2STATE_SEND_IDLE;
						tm_clear(&_l2timeout_send);
					}

				break;

				default:
					ACS_APBM_TRACE_MESSAGE("illegal send state %d", _l2_send_state);
					_l2_send_state = L2STATE_SEND_IDLE;
					tm_clear(&_l2timeout_send);
			}

		break;

		case L2EVENT_TIMEOUT:
		case L2EVENT_RECEIVED_NACK:
			switch (_l2_send_state) {
				case L2STATE_SEND_IDLE:
					tm_clear(& _l2timeout_send);
				break;

				case L2STATE_SEND_WAIT_ACK:
					ACS_APBM_TRACE_MESSAGE("Skipping packet, received NACK/L2TIMEOUT");
					_send_pool.free_tail_buffer(_p_l2msg_being_sent);

					// Check if there are other L2 messages to send
					_p_l2msg_being_sent = _send_pool.find_tail_buffer();
					if (_p_l2msg_being_sent) {
						// found another L2 message to send. Send it and wait for ACK !
						_l2_send_state = L2STATE_SEND_WAIT_ACK;
						tm_set(& _l2timeout_send, L2TIMEOUT_SEND_WAIT_ACK);
						send_l2message(_p_l2msg_being_sent);
					} else {
						// no more L2 message to send
						_l2_send_state = L2STATE_SEND_IDLE;
						tm_clear( & _l2timeout_send);
					}
				break;

				default:
					ACS_APBM_TRACE_MESSAGE("illegal send state %d", _l2_send_state);
					_l2_send_state = L2STATE_SEND_IDLE;
					tm_clear(&_l2timeout_send);
			}

		break;

		default:
			ACS_APBM_TRACE_MESSAGE("illegal event received !");
			_l2_send_state = L2STATE_SEND_IDLE;
			tm_clear(&_l2timeout_send);
		}

		return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}

int  acs_apbm_ipmiapi_level2_impl::send_l2message(const l2_message *message){

	ACS_APBM_TRACE_FUNCTION;

	//msg_len is obtained adding 2 to 'message->_l2_data_size' because we must
    // take care of size byte and checksum byte (see l2_message::encode_from_l3message() for details)
	int msg_len = message->_l2_data_size + 2;

	return write_on_device(message->_l2_data, msg_len);

}

int acs_apbm_ipmiapi_level2_impl::send_l2response(unsigned char response_byte){
	ACS_APBM_TRACE_FUNCTION;
	ACS_APBM_TRACE_MESSAGE("Sending response byte '%u' ", response_byte);

	return write_on_device(&response_byte, 1);
}

int  acs_apbm_ipmiapi_level2_impl::write_on_device( const void *data_buffer, int buffer_len ){

	ACS_APBM_TRACE_FUNCTION;
	void *buffer = const_cast <void *> (data_buffer);
	ACS_APBM_TRACE_DUMP((reinterpret_cast <uint8_t *>( buffer)), buffer_len, 512);

	int l2data_written = write(_dev_fd, data_buffer, buffer_len);
	if (l2data_written < 0)
		ACS_APBM_TRACE_MESSAGE("Cannot write data on device: Call 'write()' returned errcode == '%d', errno == %d !", l2data_written, errno);
	else if (l2data_written != buffer_len)
		ACS_APBM_TRACE_MESSAGE("send_l2message: failed, sent only first %d out of %d bytes !", l2data_written, buffer_len);
	else {
		//void *buffer = const_cast <void *> (data_buffer);
		ACS_APBM_TRACE_MESSAGE("send_l2message successful ! MESSAGE DUMP follows ... ");

	}

	return (l2data_written != buffer_len ? acs_apbm_ipmiapi_ns::ERR_WRITE_LEVEL2_ERROR : acs_apbm_ipmiapi_ns::ERR_NO_ERRORS);
}


int acs_apbm_ipmiapi_level2_impl::handle_recvstm_event(l2_event_t event, char read_value){

	ACS_APBM_TRACE_FUNCTION;
	ACS_APBM_TRACE_MESSAGE("Handling event '%d' (%s) ! Current Recv State is '%d'", event, str_event(event),_l2_send_state);

	switch(event)
	{
		case L2EVENT_NO:
		// No event
		break;
		case L2EVENT_RECEIVED_LENGTH:
			// received size field of L2 message
			switch(_l2_recv_state) {
				case L2STATE_RECV_IDLE:
				case L2STATE_RECV_WAIT_DATA:
					_l2msg_being_recv._l2_data_size = 1;
					_l2msg_being_recv._l2_data[0] = read_value;
					// In a L2 size field the MS bit is always 1
					// and the data length is coded in the remaining bits
					_l2msg_being_recv_bytesExpected = 2 + (read_value & 0x7f);  // 2 is due to size field byte and checksum field byte
					tm_set(&_l2timeout_recv, L2TIMEOUT_RECV_WAIT_DATA);
					_l2_recv_state = L2STATE_RECV_WAIT_DATA;
				break;
				default:
					ACS_APBM_TRACE_MESSAGE("illegal recv state %d", _l2_recv_state);
					tm_clear(&_l2timeout_recv);
					_l2_recv_state = L2STATE_RECV_IDLE;
			}
		break;

		case L2EVENT_RECEIVED_DATA:
			switch(_l2_recv_state) {
				case L2STATE_RECV_IDLE:
					ACS_APBM_TRACE_MESSAGE("Ignorhandle_sendstm_evente data, we must have length first");
					// Ignore data, we must have length first
				break;

				case L2STATE_RECV_WAIT_DATA:
					_l2msg_being_recv._l2_data[_l2msg_being_recv._l2_data_size] = read_value;
					++_l2msg_being_recv._l2_data_size;
				 	if (_l2msg_being_recv._l2_data_size < _l2msg_being_recv_bytesExpected)
				 		// other bytes expected ...
				 		tm_set(& _l2timeout_recv, L2TIMEOUT_RECV_WAIT_DATA);
				 	else {
				 		// received last byte (checksum byte field)
				 		if (_l2msg_being_recv.checksum(1, _l2msg_being_recv._l2_data_size -2 ) == read_value){
				 			// Checksum is OK
				 			l2_message* l2_msg_buffer = _recv_pool.find_empty_buffer();
				 			if (l2_msg_buffer == NULL)
				 				// No space in pool, packet is discarded
				 				ACS_APBM_TRACE_MESSAGE("Skipping packet, L2 receive queue is full");
				 			else
				 			{
				 				*l2_msg_buffer = _l2msg_being_recv;

				 				// send ACK to peer
				 				send_l2response(L2BYTE_ACK);
				 			}
				 		}
				 		else  // Checksum is not OK
				 		{
			 				ACS_APBM_TRACE_MESSAGE("Skipping packet, checksum error");

			 				// send NACK to peer
			 				send_l2response(L2BYTE_NACK);
				 		}

				 		// prepare for next reading
				 		tm_clear(& _l2timeout_recv);
				 		_l2_recv_state = L2STATE_RECV_IDLE;
				  }
				break;

				default:
					ACS_APBM_TRACE_MESSAGE("illegal recv state %d", _l2_recv_state);
					tm_clear(& _l2timeout_recv);
					_l2_recv_state = L2STATE_RECV_IDLE;
				}
		break;

		case L2EVENT_TIMEOUT:
			switch(_l2_recv_state) {
				case L2STATE_RECV_IDLE:
					// unexpected event. Ignore it !
				break;

				case L2STATE_RECV_WAIT_DATA:
					// Data is missing
					ACS_APBM_TRACE_MESSAGE("Skipping packet, data timeout");

					// send NACK to peer
					send_l2response(L2BYTE_NACK);
					_l2_recv_state = L2STATE_RECV_IDLE;
					tm_clear(& _l2timeout_recv);
				break;

				default:
					ACS_APBM_TRACE_MESSAGE("illegal recv state %d", _l2_recv_state);
					tm_clear(& _l2timeout_recv);
					_l2_recv_state = L2STATE_RECV_IDLE;
			}
		break;

		default:
			ACS_APBM_TRACE_MESSAGE("illegal event received !");
			_l2_recv_state = L2STATE_RECV_IDLE;
			tm_clear(& _l2timeout_recv);
	}

	return acs_apbm_ipmiapi_ns::ERR_NO_ERRORS;
}



// utility methods

int acs_apbm_ipmiapi_level2_impl::set_nonblocking(int fd) {
	long flags;
	int rv = 0;
	if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
//		fprintf(stderr, "fcntl(F_GETFL) failed, errno=%d (%s)\n", errno, strerror(errno));
		rv = -1;
	} else if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
//		fprintf(stderr, "fcntl(F_SETFL) failed, errno=%d (%s)\n", errno, strerror(errno));
		rv = -1;
	}
	return rv;
}

void acs_apbm_ipmiapi_level2_impl::tm_set(struct timeval *tvp, int timeout_msec)
{
	gettimeofday(tvp, NULL);
	tvp->tv_usec += timeout_msec * 1000;
	tvp->tv_sec += tvp->tv_usec / 1000000;
	tvp->tv_usec = tvp->tv_usec % 1000000;
}

/****************************************************************************/
void acs_apbm_ipmiapi_level2_impl::tm_clear(struct timeval *tvp)
{
	tvp->tv_sec = tvp->tv_usec = 0;
}

/****************************************************************************/
int acs_apbm_ipmiapi_level2_impl::tm_isset(struct timeval *tvp)
{
	return (tvp->tv_sec || tvp->tv_usec);
}

/****************************************************************************/
int acs_apbm_ipmiapi_level2_impl::tm_isexpired(struct timeval *tvp)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return ((now.tv_sec > tvp->tv_sec) ||
		((now.tv_sec == tvp->tv_sec) && (now.tv_usec >= tvp->tv_usec)));
}


const char * acs_apbm_ipmiapi_level2_impl::str_event(l2_event_t event){

	switch(event){
		case L2EVENT_NO:
			return("L2EVENT_NO");
		case L2EVENT_RECEIVED_ACK:
			return("L2EVENT_RECEIVED_ACK");
		case L2EVENT_RECEIVED_NACK:
			return("L2EVENT_RECEIVED_NACK");
		case L2EVENT_RECEIVED_LENGTH:
			return("L2EVENT_RECEIVED_LENGTH");
		case L2EVENT_RECEIVED_DATA:
			return("L2EVENT_RECEIVED_DATA");
		case L2EVENT_SEND_NEXT:
			return("L2EVENT_SEND_NEXT");
		case L2EVENT_TIMEOUT:
			return("L2EVENT_TIMEOUT");
		default:
			return("UNKNOWN_EVENT");
		}
}

//L2_MESSAGE methods


int l2_message::encode_from_l3_message (const eri_ipmi_message & l3_message){

	int i;
	const unsigned char * l3_message_buffer = reinterpret_cast<const unsigned char *> (& l3_message);
	unsigned char * pbuffer = &_l2_data[1];

	int l3size = 4 /* HEADER_SIZE */+ l3_message.size;

	_l2_data_size = (8 * l3size) / 7 + (l3size % 7 == 0 ? 0 : 1);
	memset(pbuffer, 0, _l2_data_size);
	for (i = 0; i < 8 * l3size; i++) {
		pbuffer[i / 7] += (((l3_message_buffer[i / 8]) << (i % 8)) & 0x80) >> (i % 7 + 1);
	}

	_l2_data[0] = acs_apbm_ipmiapi_level2_impl::L2BYTE_LENGTH_FLAG + _l2_data_size;
	_l2_data[1 + _l2_data_size] = checksum(1,  _l2_data_size);

	return 0;
}


int l2_message::decode_into_l3_message(eri_ipmi_message & l3_message) {

	unsigned int i;
	unsigned char * l3_message_buffer = reinterpret_cast<unsigned char *>  (& l3_message);
	unsigned char * pbuffer = &_l2_data[1];

	unsigned int l3size = (_l2_data_size - 2) * 7 / 8;

	if(l3size > sizeof(eri_ipmi_message))
		return -1;

	memset(l3_message_buffer, 0, l3size);

	for (i = 0; i < 8 * l3size; i++) {
		l3_message_buffer [i / 8] += (((pbuffer[i / 7]) << (i % 7 + 1)) & 0x80) >> (i % 8);
	}

	if(l3size  - L3HEADER_SIZE != l3_message.size)
		return -2;

	return 0;
}

char l2_message::checksum(unsigned int start_pos, unsigned int n_bytes )
{
	char chs = 0;
	unsigned int i;
	unsigned char * p_buffer = & _l2_data[start_pos];

	for (i = 0; i < n_bytes; i++) {
		chs += p_buffer[i];
	}

	return (0x7f & (-chs));
}


//  L2_POOL methods
bool l2_pool::init(int pool_size) {

	ACS_APBM_TRACE_FUNCTION;

	if (_buffer_pool != NULL){
		ACS_APBM_TRACE_MESSAGE("init failure: buffer pool already exists");
		return false;
	}

	_buffer_pool = new (std::nothrow) l2_message [pool_size];
	if (_buffer_pool == NULL){
		ACS_APBM_TRACE_MESSAGE("init failure: no memory available");
		return false;
	}

	_buffer_count = pool_size;
	_head_index = 0;
	_tail_index = 0;
	return true;
}

bool l2_pool::reset() {

	ACS_APBM_TRACE_FUNCTION;

	if (_buffer_pool != NULL)
		delete [] _buffer_pool;

	_buffer_pool = NULL;
	_buffer_count = 0;
	_head_index = 0;
	_tail_index = 0;

	return true;
}

l2_message * l2_pool::find_empty_buffer() {

	ACS_APBM_TRACE_FUNCTION;

	l2_message* next_head;
	int next_head_index;

	next_head_index = (_head_index + 1 >= _buffer_count) ? 0 : _head_index + 1;
	if (next_head_index == _tail_index) {
		ACS_APBM_TRACE_MESSAGE("No empty buffer available, returning NULL");
		return NULL;
	}

	next_head = & _buffer_pool[_head_index];
	_head_index = next_head_index;

	return next_head;
}


l2_message * l2_pool::find_tail_buffer() {

	ACS_APBM_TRACE_FUNCTION;

	if (_tail_index == _head_index) {
		// Queue empty
		ACS_APBM_TRACE_MESSAGE("Error: No buffer found");
		return NULL;
	}

	return & _buffer_pool[_tail_index];

}

void l2_pool::free_tail_buffer(l2_message* msg)
{
  if (_head_index == _tail_index) {
	ACS_APBM_TRACE_MESSAGE("Error: No buffer to free");
    return;
  }

  if (msg != & _buffer_pool[_tail_index])
  {
	ACS_APBM_TRACE_MESSAGE("Error: buffer is not tail");
    return;
  }

  _tail_index = (_tail_index + 1 >= _buffer_count) ? 0 :_tail_index + 1;
}


