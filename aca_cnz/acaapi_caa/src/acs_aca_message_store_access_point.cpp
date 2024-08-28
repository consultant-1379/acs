/*=================================================================== */
   /**
   @file acs_aca_message_store_access_point.cpp

   Class method implementationn for MSA API. 

   This module contains the implementation of class declared in
   the acs_aca_message_store_access_point.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       16/08/2011     XTANAGG       Initial Release            **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_aca_ms_const_values.h"
#include "acs_aca_msa_r2.h"
#include "acs_aca_api_tracer.h"
#include "acs_aca_message_store_access_point.h"

ACS_ACA_TRACE_DEFINE(ACS_ACA_MessageStoreAccessPointR3)


/*===================================================================
                        CONST DECLARATION SECTION
=================================================================== */
const int ACS_ACA_MessageStoreAccessPointR3::TransactionIdDataMaxLength = ACAMS_MaxSizeOfTransactionData;

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: ACS_ACA_MessageStoreAccessPointR3
=================================================================== */
ACS_ACA_MessageStoreAccessPointR3::ACS_ACA_MessageStoreAccessPointR3(const char * msName, const char * cpSite) {
	ACS_ACA_TRACE_FUNCTION;
	try {
		internals = new ACAMSA_R2(msName, cpSite);
	}
	catch (...) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Exception occurred during creating ACAMSA_R2 object");
		throw;
	}
} 

/*===================================================================
   ROUTINE: ACS_ACA_MessageStoreAccessPointR3
=================================================================== */
ACS_ACA_MessageStoreAccessPointR3::ACS_ACA_MessageStoreAccessPointR3(const char * msName, unsigned cpSystemId) {
	ACS_ACA_TRACE_FUNCTION;
	try {
		internals = new ACAMSA_R2(msName, cpSystemId);
	}
	catch (...)	{
		ACS_ACA_TRACE_MESSAGE("ERROR: Exception occurred during creating ACAMSA_R2 object");
		throw;
	}
} 

/*===================================================================
   ROUTINE: ~ACS_ACA_MessageStoreAccessPointR3
=================================================================== */
ACS_ACA_MessageStoreAccessPointR3::~ACS_ACA_MessageStoreAccessPointR3() 
{
	ACS_ACA_TRACE_FUNCTION;
	delete internals; internals = 0;
}

/*===================================================================
   ROUTINE: getState
=================================================================== */
ACS_ACA_MessageStoreAccessPoint::State ACS_ACA_MessageStoreAccessPointR3::getState() const {
	ACS_ACA_TRACE_FUNCTION;
	return internals->getState();
}

/*===================================================================
   ROUTINE: getLastError
=================================================================== */
ACS_ACA_MessageStoreAccessPoint::ErrorCode ACS_ACA_MessageStoreAccessPointR3::getLastError() const {
	ACS_ACA_TRACE_FUNCTION;
	return internals->getLastError();
}

/*===================================================================
   ROUTINE: getLastErrorName
=================================================================== */
std::string ACS_ACA_MessageStoreAccessPointR3::getLastErrorName() const {
	ACS_ACA_TRACE_FUNCTION;
	ACS_ACA_MessageStoreAccessPoint::ErrorCode error_code = getLastError();
	ACS_ACA_TRACE_MESSAGE("error_code == %d", error_code);

	switch (error_code) {
		case ACS_ACA_MessageStoreAccessPoint::Undefined:
			return std::string("Undefined");
		case ACS_ACA_MessageStoreAccessPoint::NoSuchMsgStore:
			return std::string("NoSuchMsgStore");
		case ACS_ACA_MessageStoreAccessPoint::ConnectFailed:
			return std::string("ConnectFailed");
		case ACS_ACA_MessageStoreAccessPoint::DisconnectFailed:
			return std::string("DisconnectFailed");
		case ACS_ACA_MessageStoreAccessPoint::CommitFailed:
			return std::string("CommitFailed");
		case ACS_ACA_MessageStoreAccessPoint::AlreadyConnected:
			return std::string("AlreadyConnected");
		case ACS_ACA_MessageStoreAccessPoint::AnotherConnected:
			return std::string("AnotherConnected");
		case ACS_ACA_MessageStoreAccessPoint::NotConnected:
			return std::string("NotConnected");
		case ACS_ACA_MessageStoreAccessPoint::ConnectionLost:
			return std::string("ConnectionLost");
		case ACS_ACA_MessageStoreAccessPoint::ConnectionRefused:
			return std::string("ConnectionRefused");
		case ACS_ACA_MessageStoreAccessPoint::MemoryError:
			return std::string("MemoryError");
		case ACS_ACA_MessageStoreAccessPoint::CommitFileError:
			return std::string("CommitFileError");
		case ACS_ACA_MessageStoreAccessPoint::SocketError:
			return std::string("SocketError");
		case ACS_ACA_MessageStoreAccessPoint::SendPrimitiveFailed:
			return std::string("SendPrimitiveFailed");
		case ACS_ACA_MessageStoreAccessPoint::RecvPrimitiveFailed:
			return std::string("RecvPrimitiveFailed");
		case ACS_ACA_MessageStoreAccessPoint::ProtocolError:
			return std::string("ProtocolError");
		case ACS_ACA_MessageStoreAccessPoint::DiskReadFailed:
			return std::string("DiskReadFailed");
		case ACS_ACA_MessageStoreAccessPoint::NoMessageToCommit:
			return std::string("NoMessageToCommit");
	}

	return std::string("UNDEFINED ERROR");
}


/*===================================================================
   ROUTINE: clearLastError
=================================================================== */
void ACS_ACA_MessageStoreAccessPointR3::clearLastError() {
	ACS_ACA_TRACE_FUNCTION;
	internals->clearLastError();
}

/*===================================================================
   ROUTINE: connect
=================================================================== */
bool ACS_ACA_MessageStoreAccessPointR3::connect() {
	ACS_ACA_TRACE_FUNCTION;
	return internals->connect();
}

/*===================================================================
   ROUTINE: disconnect
=================================================================== */
bool ACS_ACA_MessageStoreAccessPointR3::disconnect() {
	ACS_ACA_TRACE_FUNCTION;
	return internals->disconnect();
}

/*===================================================================
   ROUTINE: getHandle
=================================================================== */
ACE_HANDLE ACS_ACA_MessageStoreAccessPointR3::getHandle() const {
	ACS_ACA_TRACE_FUNCTION;
	return internals->getHandle();
}

/*===================================================================
   ROUTINE: getMessage
=================================================================== */
bool ACS_ACA_MessageStoreAccessPointR3::getMessage(ACS_ACA_MessageStoreMessage & message) {
	ACS_ACA_TRACE_FUNCTION;
	if( internals == 0 ) { //added by raghavendra on 8Sep2013 fro handling coredumps for TR HR70638
		ACS_ACA_TRACE_MESSAGE("ERROR: internals is null!");
		return false;
	}

	return internals->getMessage(message);
}

/*===================================================================
   ROUTINE: getMessageAndRevisionInfo
=================================================================== */
bool ACS_ACA_MessageStoreAccessPointR3::getMessageAndRevisionInfo(ACS_ACA_MessageStoreMessage & message, int & additionalData, int & versionNumber) {
	ACS_ACA_TRACE_FUNCTION;
	if (internals)
		return internals->getMessageAndRevisionInfo(message, additionalData, versionNumber);
	else 
		return false;
}

/*===================================================================
   ROUTINE: retrieveRevisionInfo
=================================================================== */
bool ACS_ACA_MessageStoreAccessPointR3::retrieveRevisionInfo(int & additionalData, int & versionNumber, ACS_ACA_MessageStoreMessage::State & state) {
	ACS_ACA_TRACE_FUNCTION;
	if (internals)
		return internals->retrieveRevisionInfo(additionalData, versionNumber, state);
	else
		return false;
}

/*===================================================================
   ROUTINE: commit
=================================================================== */
bool ACS_ACA_MessageStoreAccessPointR3::commit(const unsigned char * transactionIdData, const int transactionIdDataLength) {
	ACS_ACA_TRACE_FUNCTION;
	return internals->commit(transactionIdData, transactionIdDataLength);
}

/*===================================================================
   ROUTINE: commit
=================================================================== */
bool ACS_ACA_MessageStoreAccessPointR3::commit(ULONGLONG numOfMessages, const unsigned char * transactionIdData, const int transactionIdDataLength) {
	ACS_ACA_TRACE_FUNCTION;
	return internals->commit(numOfMessages, transactionIdData, transactionIdDataLength);
}

/*===================================================================
   ROUTINE: rollback
=================================================================== */
bool ACS_ACA_MessageStoreAccessPointR3::rollback() {
	ACS_ACA_TRACE_FUNCTION;
	return internals->rollback();
}

/*===================================================================
   ROUTINE: getLastTransactionId
=================================================================== */
bool ACS_ACA_MessageStoreAccessPointR3::getLastTransactionId(unsigned char * & transactionIdData, int & transactionIdDataLength) {
	ACS_ACA_TRACE_FUNCTION;
	return internals->getLastTransactionId(transactionIdData, transactionIdDataLength);
}
