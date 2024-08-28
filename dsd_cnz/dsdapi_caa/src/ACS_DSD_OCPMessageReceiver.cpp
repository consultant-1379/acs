#include "ACS_DSD_TraTracer.h"

#include "ACS_DSD_OCPMessageReceiver.h"

#ifdef ACS_DSD_API_TRACING_ACTIVE
ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__)(ACS_DSD_STRINGIZE(__CLASS_NAME__));
#endif

ssize_t __CLASS_NAME__::recv (void * buf, size_t len, const ACE_Time_Value * timeout, size_t * bytes_transferred) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	ssize_t bytes_received = 0;
	uint8_t headers_buffer[OCP_HEADER_FIELD_MESSAGE_LENGTH_SIZE] = {0};

	errno = 0;
	if ((bytes_received = stream()->recv_n(headers_buffer, OCP_HEADER_FIELD_MESSAGE_LENGTH_SIZE, MSG_PEEK, timeout)) <= 0)	{
		//ERROR: call_result < 0  : reading the message size
		//			 call_result == 0 : peer closed the connection
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: call 'bytes_received = stream()->recv_n(...' failed: bytes_received == %zd", bytes_received);

		return bytes_received;
	}

	if (bytes_received < OCP_HEADER_FIELD_MESSAGE_LENGTH_SIZE) {
		//ERROR: No data available on TCP/IP stream queue
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: call 'bytes_received = stream()->recv_n(...' failed: No data available on TCP/IP stream queue: "
				"Waiting for %d bytes but received only %zd bytes", OCP_HEADER_FIELD_MESSAGE_LENGTH_SIZE, bytes_received);

		if (bytes_received > 0) {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
					"DUMPING - OCP Message Header (size == %zd)", bytes_received);
			ACS_DSD_API_TRACE_DUMP(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), headers_buffer, bytes_received);
		}

		return acs_dsd::ERR_SYSTEM_RECEIVE;
	}

	size_t ocp_size_to_receive = ::ntohs(*reinterpret_cast<uint16_t *>(headers_buffer));

	uint8_t buffer[ocp_size_to_receive + OCP_HEADER_SIZE];
	size_t bytes_transferred_ = 0;
	errno = 0;
	bytes_received = stream()->recv_n(buffer, ocp_size_to_receive + OCP_HEADER_SIZE, timeout, &bytes_transferred_);

	if (bytes_received <= 0)	{
		//ERROR: call_result < 0  : reading the message
		//			 call_result == 0 : peer closed the connection
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: call 'bytes_received = stream()->recv_n(...' failed: bytes_received == %zd", bytes_received);

		return bytes_received;
	}

	short ocp_send_request_primitive_id = buffer[2];

	//Check the primitive id in OCP message header. The OCP send request primitive should be 6
	if (ocp_send_request_primitive_id ^ 6) { //ERROR: Unexpected send request primitive id received.
		//This TCP packet will be discarded also

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(ocp_send_request_primitive_id ^ 6): Unexpected send request primitive id received: "
				"ocp_send_request_primitive_id == %d: packet discarded",
				ocp_send_request_primitive_id);

		if (bytes_received > 0) {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "DUMPING - OCP Message Received (size == %zd)",
					bytes_received);
			ACS_DSD_API_TRACE_DUMP(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), buffer, bytes_received);
		}

		return acs_dsd::ERR_OCP_SEND_REQUEST_BAD_PRIM_ID_IN_HEADER;
	}

	size_t size_to_receive = ::ntohl(*reinterpret_cast<uint32_t *>(buffer + OCP_HEADER_SIZE)) & 0x7FFFFFFFUL;

	//Are the two sizes consistent?
	if ((size_to_receive + 4) ^ ocp_size_to_receive) { //ERROR: Sizes mismatch
		//This TCP packet will be discarded also
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION((size_to_receive + 4) ^ ocp_size_to_receive): Sizes mismatch: size_to_receive == %zu, "
				"ocp_size_to_receive == %zu: packet discarded", size_to_receive, ocp_size_to_receive);

		if (bytes_received > 0) {
			ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "DUMPING - OCP Message Received (size == %zd)",
					bytes_received);
			ACS_DSD_API_TRACE_DUMP(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), buffer, bytes_received);
		}

		return acs_dsd::ERR_OCP_RECEIVED_HEADER_SIZES_MISMATCH;
	}

	//Is the user buffer buf capable to hold the whole buffer?
	if (size_to_receive > len) { //NO
		bytes_transferred && (*bytes_transferred = size_to_receive); //Return the size needed to store the data

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(size_to_receive > len): user buffer too small to hold data packet: "
				"size_to_receive == %zu, len == %zu", size_to_receive, len);

		return acs_dsd::ERR_NOT_ENOUGH_SPACE;
	}

	//Buffer dumping by trace
	if (bytes_received > 0) {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "DUMPING - OCP Message Received (size == %zd)", bytes_received);
		ACS_DSD_API_TRACE_DUMP(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), buffer, bytes_received);
	}

	size_t data_size = ((bytes_transferred_ > OCP_DSD_HEADERS_SIZE_TOTAL)
			? bytes_transferred_ - OCP_DSD_HEADERS_SIZE_TOTAL
			: 0);
	bytes_transferred && (*bytes_transferred = data_size);

	if (bytes_received >= OCP_DSD_HEADERS_SIZE_TOTAL) {//OK: Data received correctly
		bytes_received -= OCP_DSD_HEADERS_SIZE_TOTAL;
		::memcpy(buf, buffer + OCP_DSD_HEADERS_SIZE_TOTAL, data_size);
	} else {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(bytes_received < OCP_DSD_HEADERS_SIZE_TOTAL): bad data received: bytes_received == %zd",
				bytes_received);
	}

	return bytes_received;
}


#if 0
ssize_t __CLASS_NAME__::recv (void * buf, size_t len, const ACE_Time_Value * timeout, size_t * bytes_transferred) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	ssize_t bytes_received = 0;
	uint8_t headers_buffer[8] = {0};

	errno = 0;
	if ((bytes_received = stream()->recv_n(headers_buffer, 8, MSG_PEEK, timeout)) <= 0)	{
		//ERROR: call_result < 0  : reading the message size
		//			 call_result == 0 : peer closed the connection
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: call 'bytes_received = stream()->recv_n(...' failed: bytes_received == %zd", bytes_received);

		return bytes_received;
	}

	size_t ocp_size_to_receive = ::ntohs(*reinterpret_cast<uint16_t *>(headers_buffer));
	short ocp_send_request_primitive_id = headers_buffer[2];

	//Check the primitive id in OCP message header. The OCP send request primitive should be 6
	if (ocp_send_request_primitive_id ^ 6) { //ERROR: Unexpected send request primitive id received.
		//This TCP packet will be discarded also
		uint8_t buffer[ocp_size_to_receive + 4];
		bytes_received = stream()->recv_n(buffer, ocp_size_to_receive + 4);

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(ocp_send_request_primitive_id ^ 6): Unexpected send request primitive id received: "
				"ocp_send_request_primitive_id == %d: packet discarded",
				ocp_send_request_primitive_id);

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "DUMPING - OCP Message Received (size == %zd)",
				bytes_received);
		if (bytes_received > 0) { ACS_DSD_API_TRACE_DUMP(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), buffer, bytes_received); }

		return acs_dsd::ERR_OCP_SEND_REQUEST_BAD_PRIM_ID_IN_HEADER;
	}

	size_t size_to_receive = ::ntohl(*reinterpret_cast<uint32_t *>(headers_buffer + 4)) & 0x7FFFFFFF;

	//Are the two sizes consistent?
	if ((size_to_receive + 4) ^ ocp_size_to_receive) { //ERROR: Sizes mismatch
		//This TCP packet will be discarded also
		uint8_t buffer[ocp_size_to_receive + 4];
		bytes_received = stream()->recv_n(buffer, ocp_size_to_receive + 4);

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION((size_to_receive + 4) ^ ocp_size_to_receive): Sizes mismatch: size_to_receive == %zu, ocp_size_to_receive == %zu: packet discarded",
				size_to_receive, ocp_size_to_receive);

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "DUMPING - OCP Message Received (size == %zd)",
				bytes_received);
		if (bytes_received > 0) { ACS_DSD_API_TRACE_DUMP(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), buffer, bytes_received); }

		return acs_dsd::ERR_OCP_RECEIVED_HEADER_SIZES_MISMATCH;
	}

	//The user buffer buf is capable to hold the whole buffer?
	if (size_to_receive > len) { //NO
		bytes_transferred && (*bytes_transferred = size_to_receive); //Return the size needed to store the data

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(size_to_receive > len): user buffer too small to hold data packet: size_to_receive == %zu, len == %zu",
				size_to_receive, len);

		return acs_dsd::ERR_NOT_ENOUGH_SPACE;
	}

	uint8_t buffer[size_to_receive + 8];
	size_t bytes_transferred_ = 0;
	errno = 0;
	bytes_received = stream()->recv_n(buffer, size_to_receive + 8, timeout, &bytes_transferred_);

	//Buffer dumping by trace
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "DUMPING - OCP Message Received (size == %zd)", bytes_received);
	if (bytes_received > 0) { ACS_DSD_API_TRACE_DUMP(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), buffer, bytes_received); }

	size_t data_size = (bytes_transferred_ > 8 ? bytes_transferred_ - 8 : 0);
	bytes_transferred && (*bytes_transferred = data_size);

	if (bytes_received >= 8) {//OK: Data received correctly
		bytes_received -= 8;
		::memcpy(buf, buffer + 8, data_size);
	} else {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(bytes_received < 8): bad data received: bytes_received == %zd",
				bytes_received);
	}

	return bytes_received;
}
#endif
