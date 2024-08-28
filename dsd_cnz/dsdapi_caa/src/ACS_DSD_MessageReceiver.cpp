#include "ACS_DSD_TraTracer.h"

#include "ACS_DSD_MessageReceiver.h"

#ifdef ACS_DSD_API_TRACING_ACTIVE
ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__)(ACS_DSD_STRINGIZE(__CLASS_NAME__));
#endif

ssize_t __CLASS_NAME__::recv (void * buf, size_t len, const ACE_Time_Value * timeout, size_t * bytes_transferred) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	ssize_t call_result = 0;
	uint32_t size_to_receive_net_order = 0;
	errno = 0;
	if ((call_result = stream()->recv_n(&size_to_receive_net_order, 4, MSG_PEEK, timeout)) <= 0) {
		//ERROR: call_result < 0  : reading the message size
		//			 call_result == 0 : peer closed the connection
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERROR: call 'call_result = stream()->recv_n(...' failed: call_result == %zd", call_result);
		return call_result;
	}

	size_t size_to_receive = ntohl(size_to_receive_net_order);

	//The user buffer buf is capable to hold the whole buffer?
	if (size_to_receive > len) { //NO
		bytes_transferred && (*bytes_transferred = size_to_receive); //Return the size needed to store the data

		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(size_to_receive > len): user buffer too small to hold data packet: size_to_receive == %zu, len == %zu",
				size_to_receive, len);

		return acs_dsd::ERR_NOT_ENOUGH_SPACE;
	}

	uint8_t buffer[size_to_receive + 4]; //uint8_t buffer[buf_size + 4];
	size_t bytes_transferred_ = 0;
	errno = 0;
	call_result = stream()->recv_n(buffer, size_to_receive + 4, timeout, &bytes_transferred_);

	//Buffer dumping by trace
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "DUMPING - DSD Message Received (size == %zd)", call_result);
	if (call_result > 0) { ACS_DSD_API_TRACE_DUMP(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), buffer, call_result); }

	size_t data_size = (bytes_transferred_ > 4 ? bytes_transferred_ - 4 : 0);
	bytes_transferred && (*bytes_transferred = data_size);

	if (call_result >= 4) {//OK: Data received correctly
		call_result -= 4;
		memcpy(buf, buffer + 4, data_size);
	} else {
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(call_result < 4): bad data received: call_result == %zd",
				call_result);
	}

	return call_result;
}
