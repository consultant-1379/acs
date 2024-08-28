/*=================================================================== */
/**
  @file acs_emf_root_ObjectImpl.cpp

  Class method implementation for ACA module.

  This module contains the implementation of class declared in
  the ACA Module

  @version 1.0.0

  HISTORY
  This section contains reference to problem report and related
  software correction performed inside this module

  PR           DATE      INITIALS    DESCRIPTION
  -----------------------------------------------------------
  N/A       DD/MM/YYYY     XRAMMAT       Initial Release

  INCLUDE DECLARATION SECTION
  =================================================================== */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <mqueue.h>

#include <iostream>

#include "ace/Signal.h"

#include "acs_apgcc_omhandler.h"

#include "acs_aca_logger.h"
#include "acs_aca_defs.h"
#include "acs_aca_constants.h"
#include "acs_aca_root_objectImpl.h"

ACS_ACA_ObjectImpl::ACS_ACA_ObjectImpl (std::vector<std::string> aca_classes_name, std::string impl_name, ACS_APGCC_ScopeT p_scope)
	: acs_apgcc_objectimplementereventhandler_V2(aca_classes_name.front(), impl_name, p_scope),
	  _oi_handler(), _TP_reactor_impl(0), _reactor(0) {
	_aca_classes_name = aca_classes_name;
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

ACE_INT32 ACS_ACA_ObjectImpl::svc () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	_TP_reactor_impl = new (std::nothrow) ACE_TP_Reactor();
	if (!_TP_reactor_impl) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Memory allocation failed for _TP_reactor_impl object");
		return -1;
	}

	_reactor = new (std::nothrow) ACE_Reactor(_TP_reactor_impl, true);	// Delete also reactor implementation
	if (!_reactor) {
		ACS_ACA_LOG(LOG_LEVEL_ERROR, "Memory allocation failed for _reactor object");
		delete _TP_reactor_impl; _TP_reactor_impl = 0;
		return -1;
	}

	int number_of_retries = 10;
	for (int i = 0; i < number_of_retries; i++) {
		if ( _oi_handler.addMultipleClassImpl(this, _aca_classes_name) != ACS_CC_SUCCESS ) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to set the implementer for the ACA classes");
			ACE_OS::sleep(1);
		}
		else {
			ACS_ACA_LOG(LOG_LEVEL_INFO, "Class implementer for ACA classes correctly set!");
			break;
		}
	}

	_reactor->open(1);

	dispatch(_reactor, ACS_APGCC_DISPATCH_ALL);

	_reactor->run_reactor_event_loop();

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return 0;
}

void ACS_ACA_ObjectImpl::shutdown () {
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");

	_reactor->end_reactor_event_loop();

	int number_of_retries = 10;
	for (int i = 0; i < number_of_retries; i++) {
		if ( _oi_handler.removeMultipleClassImpl(this, _aca_classes_name) != ACS_CC_SUCCESS ) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Failed to remove the implementer for the ACA classes");
			ACE_OS::sleep(1);
		}
		else {
			ACS_ACA_LOG(LOG_LEVEL_INFO, "Class implementer removed for ACA classes !");
			break;
		}
	}

	delete _reactor;
	_reactor = 0;
	_TP_reactor_impl = 0; // deleted into ACE_Reactor destructor

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
}

ACS_CC_ReturnType ACS_ACA_ObjectImpl::create (ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/, const char * /*className*/,	const char * /*parentName*/, ACS_APGCC_AttrValues ** /*attr*/) {
	ACS_ACA_LOG(LOG_LEVEL_INFO, "ObjectCreateCallback invoked!");
	return ACS_CC_FAILURE;
}

ACS_CC_ReturnType ACS_ACA_ObjectImpl::deleted (ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/, const char * /*objName*/) {
	ACS_ACA_LOG(LOG_LEVEL_INFO, "ObjectDeleteCallback invoked!");
	return ACS_CC_FAILURE;
}

ACS_CC_ReturnType ACS_ACA_ObjectImpl::modify (ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/, const char * objName, ACS_APGCC_AttrModification ** attrMods)
{
	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Entering");
	ACS_ACA_LOG(LOG_LEVEL_INFO, "ObjectModifyCallback invoked for the object '%s'!", objName);

	int ms_index;
	char objtemp[128] = {0};
	::strcpy(objtemp, objName);

	// Check if some attributes have empty values
	int i = 0;
	while (attrMods[i]) {
		if (attrMods[i]->modAttr.attrValuesNum <= 0) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "NULL value provided for attribute '%s', returning error and rejecting the modify!", attrMods[i]->modAttr.attrName);
			return ACS_CC_FAILURE;
		}
		i++;
	}
#ifdef ACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND

	if(!ACE_OS::strcmp(objName, "AxeDataRecorddataRecordMId=1")){
		i = 0;
		while (attrMods[i]) {
			if (!ACE_OS::strcmp(attrMods[i]->modAttr.attrName, ACA_OS_CACHING_ENABLED)) {
				short os_caching_value = *(reinterpret_cast< short *> (attrMods[i]->modAttr.attrValues[0]));
				ACS_ACA_LOG(LOG_LEVEL_DEBUG, "ACA_OS_CACHING_VALUE IS  '%d'",os_caching_value);
				if(os_caching_value != 0 && os_caching_value != 1){
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "The given ACA_OS_CACHING_VALUE (%d) is not valid", os_caching_value);
					return ACS_CC_FAILURE;
				}
				if( (ACS_MSD_Service::isOsCachingEnabled() == false  &&  os_caching_value == 0)  ||
						(ACS_MSD_Service::isOsCachingEnabled() == true   &&  os_caching_value == 1) )
				{
					ACS_ACA_LOG(LOG_LEVEL_WARN, "The given ACA_OS_CACHING_VALUE is inconsistent with current value: NO changes ! ");
				}
				else {
						if (os_caching_value == 1)  ACS_MSD_Service::set_os_caching_status (true);		//  caching state modify
						else  						ACS_MSD_Service::set_os_caching_status (false);
				}
			}
			i ++;
		}
		ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
		return ACS_CC_SUCCESS;
	}
#endif

	// Parse object DN in order to retrieve MS index
	char * token = ::strtok(objtemp, "=,");
	token = ::strtok(0, "=,");

	if (::strcmp(token, "CHS") == 0) {
		ms_index = 0;
	} else {
		token = ::strtok(token, "CHS");
		ms_index = ::atoi(token);
	}

	ACS_ACA_LOG(LOG_LEVEL_INFO, "Received a request to modify some parameters for the message store having index %d!", ms_index);
	i = 0;
	while (attrMods[i]) {
		// Before changing the parameter, understand if it's the recordSize or messageStoreName parameter,
		// since these parameters require extra actions to be performed

		if (!ACE_OS::strcmp(attrMods[i]->modAttr.attrName, ACA_RECORD_SIZE)) {	// Record Size parameter to be changed
			ACS_MSD_Service::ParameterChangeErrorCode call_result;
			unsigned short new_record_size = *(reinterpret_cast<unsigned short *> (attrMods[i]->modAttr.attrValues[0]));

			// FIRST: understand if the given record size is compliant to the requirement: ACA_REC_SIZE <= RTR_REC_SIZE
			bool is_job_present = false;

			if ((call_result = ACS_MSD_Service::is_record_size_valid(ms_index, new_record_size, is_job_present)) != ACS_MSD_Service::NO_ERROR) {
				setExitCode(1, get_error_message(call_result, ACA_RECORD_SIZE));
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "The given record size (%hu) is not valid, error message is '%s'",
						new_record_size, get_error_message(call_result, ACA_RECORD_SIZE));
				return ACS_CC_FAILURE;
			}
			ACS_ACA_LOG(LOG_LEVEL_INFO, "The given record size is valid, checking if the change is applicable");

			// SECOND: check if the change can be applied: all the messages in the message store must be acknowledged
			if ((call_result = ACS_MSD_Service::is_change_applicable(ms_index)) != ACS_MSD_Service::NO_ERROR) {
				setExitCode(2, get_error_message(call_result, ACA_RECORD_SIZE));
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "The change is not applicable, error message is '%s'",
						get_error_message(call_result, ACA_RECORD_SIZE));
				return ACS_CC_FAILURE;
			}
			ACS_ACA_LOG(LOG_LEVEL_INFO, "The change is applicable!");

			// THIRD: notify RTR server that the recordSize value is changed for the current message store, only if a job is present
			if (is_job_present) {
				mqd_t mq_descriptor;
				char mq_name[aca_rtr_communication::MESSAGE_QUEUE_NAME_MAX_SIZE];
				(!ms_index)
										?	::snprintf(mq_name, sizeof(mq_name), "%s%s", aca_rtr_communication::MESSAGE_QUEUE_PREFIX,	aca_rtr_communication::MS_MESS_QUEUE_NAME_PREFIX)
												: ::snprintf(mq_name, sizeof(mq_name), "%s%s%d", aca_rtr_communication::MESSAGE_QUEUE_PREFIX,	aca_rtr_communication::MS_MESS_QUEUE_NAME_PREFIX, ms_index);

				ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Trying to open the message queue %s", mq_name);
				if ((mq_descriptor = ::mq_open(mq_name, O_WRONLY)) == -1) {
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'mq_open' failed for the message queue %s, errno = %d", mq_name, errno);
					setExitCode(3, get_error_message(ACS_MSD_Service::ERROR_TRY_AGAIN, ACA_RECORD_SIZE));
					return ACS_CC_FAILURE;
				}

				char buffer[aca_rtr_communication::MESSAGE_QUEUE_MAX_MSG_SIZE];
				::snprintf(buffer, sizeof(buffer), "%s%hu", aca_rtr_communication::RECORD_SIZE_CHANGE_MESSAGE, new_record_size);

				if (::mq_send(mq_descriptor, buffer, ::strlen(buffer), aca_rtr_communication::MESSAGE_QUEUE_MSG_DEFAULT_PRIORITY) == -1) {
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'mq_send' failed, errno = %d", errno);
					::mq_close(mq_descriptor);
					setExitCode(4, get_error_message(ACS_MSD_Service::ERROR_TRY_AGAIN, ACA_RECORD_SIZE));
					return ACS_CC_FAILURE;
				}

				ACS_ACA_LOG(LOG_LEVEL_INFO, "The new record size has been correctly notified on the message queue %s!", mq_name);
				if (::mq_close(mq_descriptor) == -1) {
					ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'mq_close' failed, errno = %d", errno);
				}
			}
			else {
				ACS_ACA_LOG(LOG_LEVEL_INFO, "No Job is present for the message store having index %d, nothing to notify!", ms_index);
			}
		}
		else if (!ACE_OS::strcmp(attrMods[i]->modAttr.attrName, ACA_MESSAGESTORE_NAME)) {	// Message Store Name parameter to be changed
			// FIRST: before updating the parameter, check if the given name is not already used
			char tempname[16] = {0};
			::strcpy(tempname, reinterpret_cast<const char *>(attrMods[i]->modAttr.attrValues[0]));

			for (int idx = 0; idx < 8; idx++) {
				if ( ::strcmp(ACS_MSD_Service::getAddress(idx)->name, tempname) == 0 ) {
					setExitCode(5, "The given value is already in use");
					ACS_ACA_LOG(LOG_LEVEL_ERROR, "The given value (%s) is already used by the message store having index %d", tempname, idx);
					return ACS_CC_FAILURE;
				}
			}

			//SECOND: check if the given name is already define for DDT  DataSource name
			std::vector<std::string>  dsNames;
			if(ACS_ACA_Common::GetDDTDataSourceNames(dsNames))
			{
				for(std::vector<std::string>::const_iterator it = dsNames.begin(); it != dsNames.end(); ++it)
				{
					if (0 == ACE_OS::strcasecmp((*it).c_str(), tempname))
					{
						ACS_ACA_LOG(LOG_LEVEL_ERROR, "Message Store :<%s> already defined in DDT DataSource ", tempname );
						setExitCode(10, get_error_message(ACS_MSD_Service::ERROR_MSNAME_CONFLICT_WITH_DATASOURCE, ACA_MESSAGESTORE_NAME));
						return ACS_CC_FAILURE;
					}
				}
			}
			else
			{
				const char *err_msg =  get_error_message(ACS_MSD_Service::ERROR_TRY_AGAIN , ACA_MESSAGESTORE_NAME);
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "The change is not applicable, error message is '%s'",	err_msg);
				setExitCode(11,err_msg);
				return ACS_CC_FAILURE;
			}


			// THIRD: check if the change can be applied: all the messages in the message store must be acknowledged

			ACS_MSD_Service::ParameterChangeErrorCode call_result;
			if ((call_result = ACS_MSD_Service::is_change_applicable(ms_index)) != ACS_MSD_Service::NO_ERROR) {
				setExitCode(6, get_error_message(call_result, ACA_MESSAGESTORE_NAME));
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "The change is not applicable, error message is '%s'",
						get_error_message(call_result, ACA_MESSAGESTORE_NAME));
				return ACS_CC_FAILURE;
			}

			// FOURTH : notify RTR server that the messageStoreName value is changed for the current message store
			mqd_t mq_descriptor;
			char mq_name[aca_rtr_communication::MESSAGE_QUEUE_NAME_MAX_SIZE];
			::snprintf(mq_name, sizeof(mq_name), "%s%s", aca_rtr_communication::MESSAGE_QUEUE_PREFIX,	aca_rtr_communication::MS_NAME_CHANGE_MESS_QUEUE_NAME);

			ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Trying to open the message queue %s", mq_name);
			if ((mq_descriptor = ::mq_open(mq_name, O_WRONLY)) == -1) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'mq_open' failed for the message queue %s, errno = %d", mq_name, errno);
				setExitCode(7, get_error_message(ACS_MSD_Service::ERROR_TRY_AGAIN, ACA_MESSAGESTORE_NAME));
				return ACS_CC_FAILURE;
			}

			char buffer[aca_rtr_communication::MESSAGE_QUEUE_MAX_MSG_SIZE];
			::snprintf(buffer, sizeof(buffer), "%s%s%s", ACS_MSD_Service::getAddress(ms_index)->name, aca_rtr_communication::MS_NAME_CHANGE_SEPARATOR, tempname);

			if (::mq_send(mq_descriptor, buffer, ::strlen(buffer), aca_rtr_communication::MESSAGE_QUEUE_MSG_DEFAULT_PRIORITY) == -1) {
				ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'mq_send' failed, errno = %d", errno);
				::mq_close(mq_descriptor);
				setExitCode(8, get_error_message(ACS_MSD_Service::ERROR_TRY_AGAIN, ACA_MESSAGESTORE_NAME));
				return ACS_CC_FAILURE;
			}

			ACS_ACA_LOG(LOG_LEVEL_INFO, "The message store name change has been correctly notified on the message queue %s!", mq_name);
			if (::mq_close(mq_descriptor) == -1) {
				ACS_ACA_LOG(LOG_LEVEL_WARN, "Call 'mq_close' failed, errno = %d", errno);
			}
		}

		// After checking if some extra action must be performed (only in case of recordSize or messageStoreName change)
		// store the new value into the internal data structure
		if (ACS_MSD_Service::modify_ms_parameter(ms_index, attrMods[i]->modAttr.attrName, attrMods[i]->modAttr.attrValues[0])) {
			ACS_ACA_LOG(LOG_LEVEL_ERROR, "Call 'modify_ms_parameter' failed, returning error!");
			setExitCode(9, get_error_message(ACS_MSD_Service::ERROR_TRY_AGAIN, attrMods[i]->modAttr.attrName));
			return ACS_CC_FAILURE;
		}

		i++;
	}

	ACS_ACA_LOG(LOG_LEVEL_TRACE, "Leaving");
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_ACA_ObjectImpl::complete (ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/) {
	ACS_ACA_LOG(LOG_LEVEL_INFO, "CcbCompleteCallback invoked!");
	return ACS_CC_SUCCESS;
}

void ACS_ACA_ObjectImpl::abort (ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/) {
	ACS_ACA_LOG(LOG_LEVEL_INFO, "CcbAbortCallback invoked!");
}

void ACS_ACA_ObjectImpl::apply (ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_CcbId /*ccbId*/) {
	ACS_ACA_LOG(LOG_LEVEL_INFO, "CcbApplyCallback invoked!");
}

ACS_CC_ReturnType ACS_ACA_ObjectImpl::updateRuntime (const char * /*objName*/, const char * /*attrName*/) {
	ACS_ACA_LOG(LOG_LEVEL_INFO, "CcbUpdateRuntimeCallback invoked!");
	return ACS_CC_FAILURE;
}

void ACS_ACA_ObjectImpl::adminOperationCallback (ACS_APGCC_OiHandle /*oiHandle*/, ACS_APGCC_InvocationType /*invocation*/, const char * /*p_objName*/, ACS_APGCC_AdminOperationIdType /*operationId*/, ACS_APGCC_AdminOperationParamType ** /*paramList*/) {
	ACS_ACA_LOG(LOG_LEVEL_INFO, "adminOperationCallback invoked!");
}

const char * ACS_ACA_ObjectImpl::get_error_message (ACS_MSD_Service::ParameterChangeErrorCode error_code, const char * param_to_change) {
	ACS_ACA_LOG(LOG_LEVEL_DEBUG, "Error code received = %d - Parameter to change is '%s'", error_code, param_to_change);

	switch (error_code) {
	case ACS_MSD_Service::NO_ERROR:
		return "";

	case ACS_MSD_Service::ERROR_TRY_AGAIN:
		return "Connection with message store temporary unavailable, try again";

	case ACS_MSD_Service::ERROR_NOT_ACK_MSG:
		if (!::strcmp(param_to_change, ACA_RECORD_SIZE))
			return "Record size cannot be changed since the message store contains not acknowledged messages";
		else if (!::strcmp(param_to_change, ACA_MESSAGESTORE_NAME))
			return "Message store name cannot be changed since the message store contains not acknowledged messages";
		break;

	case ACS_MSD_Service::ERROR_OM_INIT_FAILED:
	case ACS_MSD_Service::ERROR_GET_ATTRBUTE_FAILED:
	case ACS_MSD_Service::ERROR_COMMIT_FILE_OPEN_FAILED:
		return "Internal error";

	case ACS_MSD_Service::ERROR_NOT_VALID_FOR_FILE_JOB:
		return "The value of "ACA_RECORD_SIZE" attribute should be smaller than or equal to value of "ACA_RECORD_LENGTH" attribute";

	case ACS_MSD_Service::ERROR_NOT_VALID_FOR_BLOCK_JOB:
		return "The value of "ACA_RECORD_SIZE" attribute should be smaller than or equal to value of "ACA_BLOCK_LENGTH" attribute";

	case ACS_MSD_Service::ERROR_MSNAME_CONFLICT_WITH_DATASOURCE:
		return "Message store name cannot be changed since name conflicts with DataSource";

	default:
		break;
	}

	return "Undefined error";
}
