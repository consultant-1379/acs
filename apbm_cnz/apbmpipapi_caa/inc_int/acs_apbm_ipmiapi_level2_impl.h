#ifndef ACS_APBM_IPMIAPI_LEVEL2_IMPL_H
#define ACS_APBM_IPMIAPI_LEVEL2_IMPL_H

/** @file acs_apbm_ipmiapi_level2_impl.h
 *	@brief
 *	@author
 *	@date 2012-04-05
 *
 *	COPYRIGHT Ericsson AB, 2010
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2012-04-05 |   		 	| Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <string.h>
#include <sys/time.h>
#include "eri_ipmi.h"
#include "acs_apbm_ipmiapi_common.h"

#define L2DATA_SIZE_MAX 			130
#define L3HEADER_SIZE 				4
#define L2TIMEOUT_SEND_WAIT_ACK     100
#define L2TIMEOUT_RECV_WAIT_DATA    100
#define SEND_POOL_SIZE 				4
#define RECV_POOL_SIZE 				8

class  l2_message {
	friend class acs_apbm_ipmiapi_level2_impl;

public:
	l2_message();
	int encode_from_l3_message (const eri_ipmi_message & l3_message);
	int decode_into_l3_message(eri_ipmi_message & l3_message);

private:
	char checksum(unsigned int start_pos, unsigned int n_bytes );

private:
  unsigned int _l2_data_size;
  unsigned char _l2_data[L2DATA_SIZE_MAX];
};


class  l2_pool {
public:
	inline l2_pool(): _buffer_pool(NULL), _buffer_count(0), _head_index (0), _tail_index (0) {};

	~l2_pool() { if(_buffer_pool) delete [] _buffer_pool; }

	bool init(int pool_size);

	bool reset() ;

	l2_message * find_empty_buffer();

	l2_message * find_tail_buffer();

	void free_tail_buffer(l2_message * msg);

private:
	l2_message* _buffer_pool;
	int _buffer_count;
	int _head_index;
	int _tail_index;
};


class acs_apbm_ipmiapi_level2_impl {

public:
	enum l2_send_recv_state_t {
		L2STATE_SEND_IDLE,
		L2STATE_SEND_WAIT_ACK,
		L2STATE_RECV_IDLE,
		L2STATE_RECV_WAIT_DATA,
		L2STATE_UNDEFINED
	};

	enum l2_event_t {
		L2EVENT_NO,
		L2EVENT_RECEIVED_ACK,
		L2EVENT_RECEIVED_NACK,
		L2EVENT_RECEIVED_LENGTH,
		L2EVENT_RECEIVED_DATA,
		L2EVENT_SEND_NEXT,
		L2EVENT_TIMEOUT
	};

	/* return values for read() */
	enum {
		L2_READ_FAILED = 0,
		L2_READ_CLOSE = 1,
		L2_READ_COMPLETE = 2,
		L2_READ_CONTINUE = 3,
		L2_READ_L3MESSAGE_READY = 4
	};

	// return values for write()
	enum {
		L2_WRITE_OK = 0,
		L2_WRITE_FAILED = 1,
		L2_WRITE_FAILED_MEMORY = 2
	};

	/* return values for getpacket() */
	enum {
		L2_GETPACKET_OK = 0,
		L2_GETPACKET_FAILED = 1
	};

	enum  {
		L2BYTE_DATA_MIN = 0x00,	/* data */
		L2BYTE_DATA_MAX = 0x7F,	/* data */
		L2BYTE_ACK = 0x80,	/* acknowledge */
		L2BYTE_LENGTH_FLAG = 0x80,	/* msbit indicating this is a length byte */
		L2BYTE_LENGTH_MIN = 0x84,	/* length */
		L2BYTE_LENGTH_MAX = 0xFE,	/* length */
		L2BYTE_NACK = 0xFF	/* negative acknowledge */
	};

	acs_apbm_ipmiapi_level2_impl(const char *device_name);

	virtual ~acs_apbm_ipmiapi_level2_impl();

	int init();
	int finalize();

	int l2_write(const eri_ipmi_message & l3_message);

	/**************************************************************************
	* l2read
	*
	* Description
	*    Try to read L2 packet from the communications device.
	*
	* Returns
	*    L2_READ_COMPLETE:                 No more L2 data expected.
	*    L2_READ_CONTINUE:                 More L2 data expected.
	*    L2_READ_L3MESSAGE_READY: 		   Read completer. Valid L3 message ready to be delivered.
	*    L2_READ_FAILED:                   Reading from the communications device failed.
	*************************************************************************/
	int l2_read();

	int getpacket(eri_ipmi_message * l3_message);

private:
	int open_device();
	int write_on_device(const void *data_buffer, int buffer_len );

	int handle_sendstm_event(l2_event_t event);
	int handle_recvstm_event(l2_event_t event, char read_value);
	int send_l2message(const l2_message *message);
	int send_l2response(unsigned char response_byte);


	static int set_nonblocking(int fd);
	static void tm_set(struct timeval *tvp, int timeout_msec);
	static void tm_clear(struct timeval *tvp);
	static int tm_isset(struct timeval *tvp);
	static int tm_isexpired(struct timeval *tvp);

	const char *str_event(l2_event_t event);

private:
	int _dev_fd;
	char  _device_name[IPMIAPI_DEVICE_NAME_MAX_LEN];
	l2_pool _send_pool;
	l2_pool _recv_pool;
	l2_message * _p_l2msg_being_sent;
	l2_message _l2msg_being_recv;
	unsigned int _l2msg_being_recv_bytesExpected;
	l2_send_recv_state_t _l2_send_state;
	l2_send_recv_state_t _l2_recv_state;
	struct timeval _l2timeout_send;
	struct timeval _l2timeout_recv;

};

#endif
