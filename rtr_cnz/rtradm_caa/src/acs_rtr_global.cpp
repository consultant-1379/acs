//
/** @file acs_rtr_global.cpp
 *	@brief
 *	@author qvincon (Vincenzo Conforti)
 *	@date 2013-12-06
 *	@version 1.0.0
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
 *	+========+============+==============+=====================================+
 *	| REV    | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+========+============+==============+=====================================+
 *	| 1.0.0  | 2013-12-06 | qvincon      | File created.                       |
 *	+========+============+==============+=====================================+
 */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_rtr_global.h"

#include "acs_rtr_tracer.h"
#include "acs_rtr_logger.h"

ACS_RTR_TRACE_DEFINE(ACS_RTR_Util)

namespace rtr_imm {

	// AxeDataRecord Class Attributes
	char IPaddingAttribute[] = "iPadding";
	char OPaddingAttribute[] = "oPadding";

	// File Base Job Class attributes
	char FileBaseJobId[] = "fileBasedJobId";

	char TransferQueueAttribute[] = "transferQueue";
	char HoldTimeAttrribute[] = "holdTime";
	char PaddingCharAttribute[] = "paddingChar";

	char CdrCounterFlagAttribute[] = "cdrCounterFlag";
	char FileRecordlengthAttribute[] = "recordlength";
	char FixedFileRecordsFlagAttribute[] = "fixedFileRecordsFlag";
	char FileSizeAttribute[] = "fileSize";
	char MinFileSizeAttribute[] = "minFileSize";

	char HashKeyAttribute[] = "hashKey";
	char HashKeyId[]	= "id";
	// ECIM Password Structure
	char PasswordAttribute[] = "password";

	// Block Base Job Class attributes
	char BlockBaseJobId[] = "blockBasedJobId";
	char BlockLengthAttribute[] = "length";
	char BlockLengthTypeAttribute[] = "lengthType";

	// CpStatisticsInfo Class attributes
	char CpStatisticsInfoId[] = "cPStatisticsInfoId";
	char RecordsReadAttribute[] = "recordsRead";
	char RecordsLostAttribute[] = "recordsLost";
	char RecordsSkippedAttribute[] = "recordsSkipped";

	// StatisticsInfo Class attributes
	char StatisticsInfoId[] = "statisticsInfoId";
	char VolumeReadAttribute[] = "volumeRead";
	char VolumeReportedAttribute[] = "volumeReported";

	// Message Store Class attributes
	char MessageStoreNameAttribute[] = "messageStoreName";
	char RecordSizeAttibute[] = "recordSize";

}

namespace actionResult
{
	char ErrorCodeAttribute[] = "errorCode";
	char ErrorMessageAttribute[] = "errorComCliMessage";
}

namespace ACS_RTR_Util
{
	void dump (const char* msg_to_print, const unsigned char* buffer, ssize_t size, ssize_t dumping_size, ssize_t dumping_line_length)
	{
		ACS_RTR_LOG(LOG_LEVEL_DEBUG, "------------------------------------------------");
		ACS_RTR_LOG(LOG_LEVEL_DEBUG, "%s", msg_to_print);

		size_t output_buffer_size = 2 + 8 + 2 + 4 * dumping_line_length + 16 + 10;
		char output_buffer[output_buffer_size];

		size_t ascii_line_size = dumping_line_length * 2;
		char ascii_line[ascii_line_size];

		size_t trace_buffer_size = 3 * output_buffer_size * (dumping_size % dumping_line_length);
		char trace_buffer[trace_buffer_size];
		int trace_chars = 0;

		for (ssize_t i = 0; (i < size) && (i < dumping_size); )
		{
			int chars = snprintf(output_buffer, output_buffer_size, "  %08zX:", static_cast<size_t>(i));
			int ascii_chars = 0;

			for (ssize_t col = 0; (col < dumping_line_length) && (i < size) && (i < dumping_size); ++i, ++col)
			{
				chars += snprintf(output_buffer + chars, output_buffer_size - chars, " %02X", buffer[i]);
				ascii_chars += snprintf(ascii_line + ascii_chars, ascii_line_size - ascii_chars, "%c", isprint(buffer[i]) ? buffer[i] : '.');
			}
			chars += snprintf(output_buffer + chars, output_buffer_size - chars, " | %s", ascii_line);

			ACS_RTR_LOG(LOG_LEVEL_DEBUG, output_buffer);

		}
	}
}
