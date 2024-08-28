//****************************************************************
//
//  NAME 
//    ACAMSA_R2.C
//
//  COPYRIGHT
//    COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//    All rights reserved.
//
//    The Copyright to the computer program(s) herein 
//    is the property of Ericsson Utvecklings AB, Sweden.
//    The program(s) may be used and/or copied only with 
//    the written permission from Ericsson Utvecklings AB or in 
//    accordance with the term and conditions stipulated in the
//    agreement/contract under which the program(s) have been 
//    supplied.

//  DOCUMENT NO
//    CAA 109 0313

//  AUTHOR 
//    EAB/UZ/DG Carl Johannesson

//  DESCRIPTION
//    This class is the real implementation of
//    ACS_ACA_MessageStoreAccessPoint, and has the same public
//    interface. It is derived from the abstract class
//    ACS_ACA_InternalMessageStoreAccessPoint and the class
//    ACAMSA_MsgStore.

//  CHANGES
//    RELEASE REVISION HISTORY
//    REV NO          DATE          NAME       DESCRIPTION
//    A               2000-05-10    uabcajn    product release
//    B               2004-03-10    uabcajn    rogue wave removed.
//    C               2007-01-15    qnicmut    Blade Cluster adaptations
//****************************************************************
#include "acs_aca_api_tracer.h"
#include "acs_aca_msa_r2.h"

ACS_ACA_TRACE_DEFINE(ACS_ACA_ACAMSA_R2);


//// ---------------------------------------------------------------
ACAMSA_R2::ACAMSA_R2(const std::string & msName, const std::string & cpSite)
: ACAMSA_MsgStore(msName, cpSite), _transactionIdDataBuffer(0), _transactionIdDataLength(0) {

	ACS_ACA_TRACE_FUNCTION;
	anyMessageRead = false;
}

//// ---------------------------------------------------------------
ACAMSA_R2::ACAMSA_R2(const std::string & msName, unsigned cpSystemId)
: ACAMSA_MsgStore(msName, cpSystemId), _transactionIdDataBuffer(0), _transactionIdDataLength(0) {
	ACS_ACA_TRACE_FUNCTION;
	anyMessageRead = false;
}

// ---------------------------------------------------------------
ACAMSA_R2::~ACAMSA_R2() {
	ACS_ACA_TRACE_FUNCTION;
	delete [] _transactionIdDataBuffer; _transactionIdDataBuffer = 0;
}

// ---------------------------------------------------------------
bool ACAMSA_R2::connect() {
	ACS_ACA_TRACE_FUNCTION;

	if (stateM == ACS_ACA_MessageStoreAccessPoint::Connected) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Already connected, calling rollback");
		errorCodeM = ACS_ACA_MessageStoreAccessPoint::AlreadyConnected;
		return rollback(); 
	}

	if ((errorCodeM = ACAMSA_MsgStore::connect()) == ACS_ACA_MessageStoreAccessPoint::Undefined) {
		// We succeeded to connect!
		ACS_ACA_TRACE_MESSAGE("Succeeded to connect!");
		anyMessageRead = false; 
		stateM = ACS_ACA_MessageStoreAccessPoint::Connected;
		return rollback();
	}
	else {
		ACS_ACA_TRACE_MESSAGE("[errorCodeM != Undefined], returning false");
		return false;
	}
}

// ---------------------------------------------------------------
bool ACAMSA_R2::disconnect() {
	ACS_ACA_TRACE_FUNCTION;

	if((errorCodeM = ACAMSA_MsgStore::disconnect()) == ACS_ACA_MessageStoreAccessPoint::Undefined) {
		stateM = ACS_ACA_MessageStoreAccessPoint::Disconnected;
		anyMessageRead = false;
		ACS_ACA_TRACE_MESSAGE("[errorCodeM == Undefined], returning true");
		return true;
	}
	else {
		ACS_ACA_TRACE_MESSAGE("[errorCodeM != Undefined], returning false");
		return false;
	}
}

// ---------------------------------------------------------------
ACE_HANDLE ACAMSA_R2::getHandle() const {
	ACS_ACA_TRACE_FUNCTION;
	return ACAMSA_MsgStore::getHandle();
}

// ---------------------------------------------------------------
bool ACAMSA_R2::getMessage(ACS_ACA_MessageStoreMessage & message) {
	ACS_ACA_TRACE_FUNCTION;
	unsigned char * msgData = 0;

	if(stateM == ACS_ACA_MessageStoreAccessPoint::Disconnected) {
		ACS_ACA_TRACE_MESSAGE("The state is Disconnected, returning false");
		errorCodeM = ACS_ACA_MessageStoreAccessPoint::NotConnected;
		return false;
	}
	else {
		ACS_ACA_MessageStoreMessage::State state;
		ULONGLONG msgNumber;
		int dataLength;
		errorCodeM = ACAMSA_MsgStore::getMsg(msgNumber, msgData, dataLength, state);

		switch (errorCodeM) {
		case ACS_ACA_MessageStoreAccessPoint::Undefined:
			anyMessageRead = true;
		case ACS_ACA_MessageStoreAccessPoint::SendPrimitiveFailed:
		case ACS_ACA_MessageStoreAccessPoint::DiskReadFailed:
			break;

		default:	// ACS_ACA_MessageStoreAccessPoint::ConnectionLost or ACS_ACA_MessageStoreAccessPoint::NotConnected
			stateM = ACS_ACA_MessageStoreAccessPoint::Disconnected;
		}

		ACS_ACA_InternalMessageStoreAccessPoint::setMessageData(message, msgNumber, msgData, dataLength, state);
		ACS_ACA_TRACE_MESSAGE("Set message data, returning %s", ((errorCodeM == ACS_ACA_MessageStoreAccessPoint::Undefined) ? "true" : "false"));
		return (errorCodeM == ACS_ACA_MessageStoreAccessPoint::Undefined) ? true : false;
	}
}

// ---------------------------------------------------------------
bool ACAMSA_R2::getMessageAndRevisionInfo(ACS_ACA_MessageStoreMessage & message, int & primitiveFormat, int & versionNumber) {
	ACS_ACA_TRACE_FUNCTION;
	unsigned char * msgData = 0;

	if(stateM == ACS_ACA_MessageStoreAccessPoint::Disconnected) {
		ACS_ACA_TRACE_MESSAGE("The state is Disconnected, returning false");
		errorCodeM = ACS_ACA_MessageStoreAccessPoint::NotConnected;
		return false;
	}
	else {
		ACS_ACA_MessageStoreMessage::State state;
		ULONGLONG msgNumber;
		int dataLength;
		errorCodeM = ACAMSA_MsgStore::getMsgAndRevInfo(msgNumber, msgData, dataLength, state, primitiveFormat, versionNumber, 0);

		switch (errorCodeM) {
		case ACS_ACA_MessageStoreAccessPoint::Undefined:
			anyMessageRead = true;
		case ACS_ACA_MessageStoreAccessPoint::SendPrimitiveFailed:
		case ACS_ACA_MessageStoreAccessPoint::DiskReadFailed:
			break;

		default:	// ACS_ACA_MessageStoreAccessPoint::ConnectionLost or ACS_ACA_MessageStoreAccessPoint::NotConnected
			stateM = ACS_ACA_MessageStoreAccessPoint::Disconnected;
		}

		ACS_ACA_InternalMessageStoreAccessPoint::setMessageData(message, msgNumber,	msgData, dataLength, state);
		ACS_ACA_TRACE_MESSAGE("Set message data, returning %s", ((errorCodeM == ACS_ACA_MessageStoreAccessPoint::Undefined) ? "true" : "false"));
		return (errorCodeM == ACS_ACA_MessageStoreAccessPoint::Undefined) ? true : false;
	}
}

// ---------------------------------------------------------------
bool ACAMSA_R2::retrieveRevisionInfo(int & primitiveFormat, int & versionNumber, ACS_ACA_MessageStoreMessage::State & state) {
	ACS_ACA_TRACE_FUNCTION;
	unsigned char * msgData = 0;

	if(stateM == ACS_ACA_MessageStoreAccessPoint::Disconnected) {
		errorCodeM = ACS_ACA_MessageStoreAccessPoint::NotConnected;
		ACS_ACA_TRACE_MESSAGE("The state is Disconnected, returning false");
		return false;
	}
	else {
		ULONGLONG msgNumber;
		int dataLength;
		errorCodeM = ACAMSA_MsgStore::getMsgAndRevInfo(msgNumber, msgData, dataLength, state, primitiveFormat, versionNumber, 1);

		switch (errorCodeM) {
		case ACS_ACA_MessageStoreAccessPoint::Undefined:
			anyMessageRead = true;
		case ACS_ACA_MessageStoreAccessPoint::SendPrimitiveFailed:
		case ACS_ACA_MessageStoreAccessPoint::DiskReadFailed:
			break;

		default:	// ACS_ACA_MessageStoreAccessPoint::ConnectionLost or ACS_ACA_MessageStoreAccessPoint::NotConnected
			stateM = ACS_ACA_MessageStoreAccessPoint::Disconnected;
		}

		ACS_ACA_TRACE_MESSAGE("Returning %s", ((errorCodeM == ACS_ACA_MessageStoreAccessPoint::Undefined) ? "true" : "false"));
		return (errorCodeM == ACS_ACA_MessageStoreAccessPoint::Undefined) ? true : false;
	}
}

// ---------------------------------------------------------------
bool ACAMSA_R2::commit(const unsigned char * transactionIdData, const int transactionIdDataLength) {
	ACS_ACA_TRACE_FUNCTION;
	ULONGLONG lastReadMsg = ACAMSA_MsgStore::getMsgNumber_LastRead();
	ULONGLONG lastCommitMsg = ACAMSA_MsgStore::getMsgNumber_LastCommitted();
	ACS_ACA_TRACE_MESSAGE("lastReadMsg == %llu, lastCommitMsg == %llu", lastReadMsg, lastCommitMsg);

	if (stateM == ACS_ACA_MessageStoreAccessPoint::Disconnected) {
		errorCodeM = ACS_ACA_MessageStoreAccessPoint::NotConnected;
		ACS_ACA_TRACE_MESSAGE("The state is Disconnected, returning false");
		return false;
	}
	else {
		if (lastReadMsg > lastCommitMsg) {
			ACS_ACA_TRACE_MESSAGE("[lastReadMsg > lastCommitMsg], calling 'ACAMSA_MsgStore::commit'");
			return ((errorCodeM = ACAMSA_MsgStore::commit(transactionIdData, transactionIdDataLength)) == ACS_ACA_MessageStoreAccessPoint::Undefined) ? true : false;
		}
		else {
			ACS_ACA_TRACE_MESSAGE("[lastReadMsg <= lastCommitMsg], nothing to commit, returning true");
			return true; // Nothing to commit
		}
	}
}

// ---------------------------------------------------------------
bool ACAMSA_R2::commit(ULONGLONG numOfMessages, const unsigned char * transactionIdData, const int transactionIdDataLength) {
	ACS_ACA_TRACE_FUNCTION;
	ULONGLONG lastReadMsg = ACAMSA_MsgStore::getMsgNumber_LastRead();
	ULONGLONG lastCommitMsg = ACAMSA_MsgStore::getMsgNumber_LastCommitted();
	ACS_ACA_TRACE_MESSAGE("numOfMessages == %llu, lastReadMsg == %llu, lastCommitMsg == %llu", numOfMessages, lastReadMsg, lastCommitMsg);

	if (stateM == ACS_ACA_MessageStoreAccessPoint::Disconnected) {
		errorCodeM = ACS_ACA_MessageStoreAccessPoint::NotConnected;
		ACS_ACA_TRACE_MESSAGE("The state is Disconnected, returning false");
		return false;
	}
	else {
		if (lastReadMsg > lastCommitMsg) {
			ACS_ACA_TRACE_MESSAGE("[lastReadMsg > lastCommitMsg], calling 'ACAMSA_MsgStore::commit'");
			return ((errorCodeM = ACAMSA_MsgStore::commit(numOfMessages, transactionIdData, transactionIdDataLength)) == ACS_ACA_MessageStoreAccessPoint::Undefined) ? true : false;
		}
		else {
			ACS_ACA_TRACE_MESSAGE("[lastReadMsg <= lastCommitMsg], nothing to commit, returning true");
			return true; // Nothing to commit
		}
	}
}

// ---------------------------------------------------------------
bool ACAMSA_R2::rollback() {
	ACS_ACA_TRACE_FUNCTION;
	ULONGLONG lastReadMsg = ACAMSA_MsgStore::getMsgNumber_LastRead();
	ULONGLONG lastCommitMsg = ACAMSA_MsgStore::getMsgNumber_LastCommitted();

	if (stateM == ACS_ACA_MessageStoreAccessPoint::Disconnected) {
		errorCodeM = ACS_ACA_MessageStoreAccessPoint::NotConnected;
		ACS_ACA_TRACE_MESSAGE("The state is Disconnected, returning false");
		return false;
	}

	if (lastReadMsg >= lastCommitMsg) {	// Must move back.
		ACS_ACA_TRACE_MESSAGE("[lastReadMsg >= lastCommitMsg], calling 'ACAMSA_MsgStore::rollback'");

		errorCodeM = ACAMSA_MsgStore::rollback();
		if (errorCodeM != ACS_ACA_MessageStoreAccessPoint::Undefined)	{
			ACS_ACA_TRACE_MESSAGE("ERROR: Call 'rollback' failed, disconnecting the session, error_code = %d", errorCodeM);
			stateM = ACS_ACA_MessageStoreAccessPoint::Disconnected;
			return false;
		}

		ACS_ACA_TRACE_MESSAGE("Call 'rollback' was successful, returning true");
		return true;
	}
	else {
		// We're in big trouble (tremble)). This should never happen
		ACAMSA_MsgStore::disconnect();
		stateM = ACS_ACA_MessageStoreAccessPoint::Disconnected;
		errorCodeM = ACS_ACA_MessageStoreAccessPoint::ConnectionLost;
		ACS_ACA_TRACE_MESSAGE("ERROR: [lastReadMsg < lastCommitMsg], disconnecting");
		return false;
	}
}

// ---------------------------------------------------------------
bool ACAMSA_R2::getLastTransactionId(unsigned char * & transactionIdData, int & transactionIdDataLength) {
	ACS_ACA_TRACE_FUNCTION;

	if(stateM == ACS_ACA_MessageStoreAccessPoint::Disconnected) {
		errorCodeM = ACS_ACA_MessageStoreAccessPoint::NotConnected;
		ACS_ACA_TRACE_MESSAGE("The state is Disconnected, returning false");
		return false;
	}
	else {
		int length = 0;
		const unsigned char * data = ACAMSA_MsgStore::getTransactionData(length);

		if (length > _transactionIdDataLength) {
			delete [] _transactionIdDataBuffer; _transactionIdDataBuffer = 0;
			_transactionIdDataBuffer = new (std::nothrow) unsigned char[length];

			if (!_transactionIdDataBuffer) {
				ACS_ACA_TRACE_MESSAGE("ERROR: Failed to allocate memory for _transactionIdDataBuffer, returning false");
				errorCodeM = ACS_ACA_MessageStoreAccessPoint::MemoryError;
				return false;
			}
		}

		transactionIdDataLength = length;
		memcpy(_transactionIdDataBuffer, data, transactionIdDataLength);
		transactionIdData = _transactionIdDataBuffer;

		errorCodeM = ACS_ACA_MessageStoreAccessPoint::Undefined;
		return true;
	}
}
