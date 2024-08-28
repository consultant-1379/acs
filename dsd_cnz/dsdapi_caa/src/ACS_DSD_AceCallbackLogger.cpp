#include "ace/Log_Msg.h"
#include "ace/Log_Record.h"

#include "ACS_DSD_AceCallbackLogger.h"

void __CLASS_NAME__::log (ACE_Log_Record & log_record) {
	if (verbose()) {
    char verbose_msg[ACE_Log_Record::MAXVERBOSELOGMSGLEN];
    log_record.format_msg(ACE_LOG_MSG->local_host(), ACE_LOG_MSG->flags(), verbose_msg) || ACS_DSD_Logger::log(verbose_msg);
	} else ACS_DSD_Logger::log(log_record.msg_data());
}
