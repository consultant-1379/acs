#include "ACS_DSD_TraTracer.h"

#include "ACS_DSD_MessageSender.h"

#ifdef ACS_DSD_API_TRACING_ACTIVE
ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__)(ACS_DSD_STRINGIZE(__CLASS_NAME__));
#endif

//ssize_t __CLASS_NAME__::send (const void * buf, size_t len, const ACE_Time_Value * timeout, size_t * bytes_transferred) const {
ssize_t __CLASS_NAME__::sendf (const void * buf, size_t len, int flags, const ACE_Time_Value * timeout, size_t * bytes_transferred) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	uint8_t buffer[len + 4];

	*reinterpret_cast<uint32_t *>(buffer) = htonl(len);
	memcpy(buffer + 4, buf, len);

	//Buffer dumping by trace
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "DUMPING - DSD Message Sent (size == %zu)", len + 4);
	ACS_DSD_API_TRACE_DUMP(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), buffer, len + 4);

	size_t bytes_transferred_ = 0;
	errno = 0;

	ssize_t call_result = stream() ->
#if !defined (ACS_DSD_API_FLAGGED_SEND_DISABLED)
			send_n(buffer, len + 4, flags, timeout, &bytes_transferred_);
#else
			send_n(buffer, len + 4, timeout, &bytes_transferred_);
	static_cast<void>(flags);
#endif

	bytes_transferred && (*bytes_transferred = ((bytes_transferred_ > 4) ? (bytes_transferred_ - 4) : 0));

	if (call_result < 4) { //Sending error
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(call_result < 4): bad data sent: call_result == %zd", call_result);
	}

	return (call_result >= 4 ? (call_result - 4) : call_result);
}
