#include "ACS_DSD_TraTracer.h"

#include "ACS_DSD_OCPMessageSender.h"

#ifdef ACS_DSD_API_TRACING_ACTIVE
ACS_DSD_API_CLASS_TRACER_DECL(__CLASS_NAME__)(ACS_DSD_STRINGIZE(__CLASS_NAME__));
#endif

//ssize_t __CLASS_NAME__::send (const void * buf, size_t len, const ACE_Time_Value * timeout, size_t * bytes_transferred) const {
ssize_t __CLASS_NAME__::sendf (
		const void * buf,
		size_t len,
		int flags,
		const ACE_Time_Value * timeout,
		size_t * bytes_transferred
) const {
	ACS_DSD_API_TRACE_FUNCTION(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__));

	//The buffer's size to send cannot be greater than the maximum size supported in OCP protocol
	if (len > (0xFFFFU - 4U)) { //16bits message size in OCP
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "ERR_ASSERTION(len > (0xFFFFU - 4U)): len == %zu", len);
		return acs_dsd::ERR_OCP_BUFFER_SIZE_EXCEEDS_MAXIMUM_ALLOWED;
	}

	uint8_t buffer[len + 8];

	//Put the input buf len in buffer
	*reinterpret_cast<uint32_t *>(buffer + 4) = htonl(len | 0x80000000);

	//Make-up the OCP header
	*reinterpret_cast<uint16_t *>(buffer) = htons(len + 4);
	buffer[2] = 9;
	buffer[3] = 0;

	memcpy(buffer + 8, buf, len);

	//Buffer dumping by trace
	ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), "DUMPING - OCP Message Sent (size == %zu)", len + 8);
	ACS_DSD_API_TRACE_DUMP(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__), buffer, len + 8);

	size_t bytes_transferred_ = 0;
	errno = 0;

	ssize_t bytes_sent = stream() ->
#if !defined (ACS_DSD_API_FLAGGED_SEND_DISABLED)
			send_n(buffer, len + 8, flags, timeout, &bytes_transferred_);
#else
			send_n(buffer, len + 8, timeout, &bytes_transferred_);
	static_cast<void>(flags);
#endif

	bytes_transferred && (*bytes_transferred = ((bytes_transferred_ > 8) ? (bytes_transferred_ - 8) : 0));

	if (bytes_sent < 8) { //Sending error
		ACS_DSD_API_TRACE_MESSAGE(ACS_DSD_CLASS_TRACER_NAME(__CLASS_NAME__),
				"ERR_ASSERTION(bytes_sent < 8): bad data sent: bytes_sent == %zd", bytes_sent);
	}

	return (bytes_sent >= 8 ? (bytes_sent - 8) : bytes_sent);
}
