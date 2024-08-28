/*=================================================================== */
/**
  @file acs_aca_msa_msg_store.cpp

  Class method implementation for MSA API.

  This module contains the implementation of class declared in
  the acs_aca_msa_msg_store.h module

  @version 1.0.0

  HISTORY
  This section contains reference to problem report and related
  software correction performed inside this module

  PR           DATE      INITIALS    DESCRIPTION
  -----------------------------------------------------------
  N/A       17/08/2011     XTANAGG       Initial Release            **/
/*=================================================================== */

/*===================================================================
  INCLUDE DECLARATION SECTION
  =================================================================== */
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "ace/ACE.h"

#include "acs_aca_message_store_message.h"
#include "acs_aca_ms_commit_file.h"
#include "acs_aca_ms_msip_protocol.h"
#include "acs_aca_utility.h"
#include "acs_aca_defs.h"
#include "acs_aca_api_tracer.h"
#include "acs_aca_msa_msg_store.h"

namespace {
	const unsigned int IO_BUFFER_SIZE = ACAMS_MaxMsgSize + ACAMS_FileOverhead;
}

using namespace std;

ACS_ACA_TRACE_DEFINE(ACS_ACA_ACAMSA_MsgStore)


	/*===================================================================
	  CONST DECLARATION SECTION
	  =================================================================== */
	const int DataOffset = 8;

	/*===================================================================
	  ROUTINE DECLARATION SECTION
	  =================================================================== */

	/*===================================================================
ROUTINE: ACAMSA_MsgStore
=================================================================== */
ACAMSA_MsgStore::ACAMSA_MsgStore(const string & msName, const string & cpSite)
	: ACAMS_MsgStore(msName, cpSite, 1),
	myMSIP_socket(ACE_INVALID_HANDLE), myCurrentFile(ACE_INVALID_HANDLE), myLastReadMsgNumber(0),
	myCommitFile(0), myConnectionState(Disconnect), myNextMsgRead(false), checkMSIP_input_timeout(false), myMsgSize(0),
	myIO_Buffer(0), myCurrentFileName(), myLastFinishedFile(), MSIPport(0),_isMultipleCPSystem(false),
	_cpSystemId(~0U), bReadbfCommit(0), bytesDone(0), msgSize(0), newFile(true) {

		ACS_ACA_TRACE_FUNCTION;
		myIO_Buffer = new (std::nothrow) unsigned char[IO_BUFFER_SIZE];
		::memset(myIO_Buffer, 0, IO_BUFFER_SIZE);
		MSIPport = ACAMS_MSIP_Protocol::getMSIPportNo(cpSite.c_str(), msName.c_str());
	}

/*===================================================================
ROUTINE: ACAMSA_MsgStore
=================================================================== */
ACAMSA_MsgStore::ACAMSA_MsgStore(const string & msName, unsigned cpSystemId)
	: ACAMS_MsgStore(msName, cpSystemId, 1),
	myMSIP_socket(ACE_INVALID_HANDLE), myCurrentFile(ACE_INVALID_HANDLE), myLastReadMsgNumber(0),
	myCommitFile(0), myConnectionState(Disconnect), myNextMsgRead(false), checkMSIP_input_timeout(false), myMsgSize(0),
	myIO_Buffer(0), myCurrentFileName(), myLastFinishedFile(), MSIPport(0), _isMultipleCPSystem(true),
	_cpSystemId(cpSystemId), bReadbfCommit(0), bytesDone(0), msgSize(0), newFile(true) {

		ACS_ACA_TRACE_FUNCTION;
		myIO_Buffer = new (std::nothrow) unsigned char[IO_BUFFER_SIZE];
		::memset(myIO_Buffer, 0, IO_BUFFER_SIZE);
		MSIPport = ACAMS_MSIP_Protocol::getMSIPportNo(msName.c_str());
	}

/*===================================================================
ROUTINE: ~ACAMSA_MsgStore
=================================================================== */
ACAMSA_MsgStore::~ACAMSA_MsgStore() {
	ACS_ACA_TRACE_FUNCTION;

	disconnect();
	delete[] myIO_Buffer;	myIO_Buffer = 0;

	if (isValidHandle(__FILE__,__FUNCTION__,__LINE__,myCurrentFile)) {
		ACE_OS::close(myCurrentFile);
		myCurrentFile = ACE_INVALID_HANDLE;
	}

	delete myCommitFile; myCommitFile = 0;
}

/*===================================================================
ROUTINE: disconnect
=================================================================== */
ACS_ACA_MessageStoreAccessPoint::ErrorCode ACAMSA_MsgStore::disconnect(bool sendApplDiscPrim) {
	ACS_ACA_TRACE_FUNCTION;

	if (myMSIP_socket != ACE_INVALID_HANDLE) {
		if (sendApplDiscPrim) {
			ACAMS_MSIP_Protocol::sendApplDisc(myMSIP_socket);
			ACS_ACA_TRACE_MESSAGE("ApplDisc primitive sent to the server!");
		}

		if (myMSIP_socket != ACE_INVALID_HANDLE) {
			ACE_OS::close(myMSIP_socket);
			myMSIP_socket = ACE_INVALID_HANDLE;
			myConnectionState = Disconnect;
			myCurrentFileName = "";
			myLastFinishedFile = "";

			if (myCurrentFile != ACE_INVALID_HANDLE) {
				ACE_OS::close(myCurrentFile);
				myCurrentFile = ACE_INVALID_HANDLE;
			}
			return ACS_ACA_MessageStoreAccessPoint::Undefined;
		}
	}

	return ACS_ACA_MessageStoreAccessPoint::DisconnectFailed;
} 

/*===================================================================
ROUTINE: connect
=================================================================== */
ACS_ACA_MessageStoreAccessPoint::ErrorCode ACAMSA_MsgStore::connect() {
	ACS_ACA_TRACE_FUNCTION;

	// Create a CommitFile to get info about last commited msg from.
	try {
		if (!myCommitFile) {
			myCommitFile = new (std::nothrow) ACAMS_CommitFile(this->getCommitFileName());
			if (!myCommitFile) {
				ACS_ACA_TRACE_MESSAGE("ERROR: Failed to allocate ACAMS_CommitFile object, returning MemoryError");
				return ACS_ACA_MessageStoreAccessPoint::MemoryError;
			}
		}
	}
	catch (std::exception & e) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'ACAMS_CommitFile()' threw an exception '%s'", e.what());
		delete myCommitFile; myCommitFile = 0;
		return ACS_ACA_MessageStoreAccessPoint::CommitFileError;
	}

	struct sockaddr_in msd_sockaddr;
	ACE_OS::memset((char *)&msd_sockaddr, 0, sizeof(msd_sockaddr));
	msd_sockaddr.sin_family = AF_INET;
	msd_sockaddr.sin_port = htons((unsigned int)MSIPport);
	msd_sockaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	if (isValidHandle(__FILE__,__FUNCTION__,__LINE__,myMSIP_socket)) {
		ACE_OS::close(myMSIP_socket);
		myMSIP_socket = ACE_INVALID_HANDLE;
	}

	myMSIP_socket = ACE_OS::socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);
	if (!isValidHandle(__FILE__,__FUNCTION__,__LINE__,myMSIP_socket)) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Failed to create MSIP socket, returning SocketError");
		return ACS_ACA_MessageStoreAccessPoint::SocketError;
	}

	fd_set writeFds;
	FD_ZERO(&writeFds);
	FD_SET(myMSIP_socket, &writeFds);
	int selRes = 0;

	int connRes = ACE_OS::connect(myMSIP_socket, (struct sockaddr *)&msd_sockaddr, sizeof(msd_sockaddr));
	if ((connRes == -1) && (ACE_OS::last_error() == EWOULDBLOCK)) {
		ACS_ACA_TRACE_MESSAGE("Call 'connect' returned -1 and errno is EWOULDBLOCK: Calling select!");
		selRes = ACE_OS::select(myMSIP_socket + 1, 0, &writeFds, 0, NULL);
		ACS_ACA_TRACE_MESSAGE("Call 'select' returned %d, errno = %d", selRes, ACE_OS::last_error());
	}
	else
		selRes = connRes;

	if (selRes == -1) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'connect' failed for port %d!, errno == %d", MSIPport, ACE_OS::last_error());

		// This is normal if the MSD process is not up yet
		if(isValidHandle(__FILE__,__FUNCTION__,__LINE__,myMSIP_socket)) {
			ACE_OS::close(myMSIP_socket);
			myMSIP_socket = ACE_INVALID_HANDLE;
		}

		ACS_ACA_TRACE_MESSAGE("ERROR: The connect call failed...checking the root cause...");
		string theMS_Name = this->getMS_Name();
		ACE_stat fileInfo;

		if (ACE_OS::stat(theMS_Name.c_str(), &fileInfo) == 0) {
			ACS_ACA_TRACE_MESSAGE("ERROR: Call 'connect' failed!");
			return ACS_ACA_MessageStoreAccessPoint::ConnectFailed;
		}

		ACS_ACA_TRACE_MESSAGE("ERROR: The given message store was not found! - name = '%s'", theMS_Name.c_str());
		return ACS_ACA_MessageStoreAccessPoint::NoSuchMsgStore;
	}

	// Connect succeded, now see if it was accepted by msd service.
	unsigned char buffer[2 * 1024] = {0};
	unsigned int primitiveSize;
	ACS_ACA_MessageStoreAccessPoint::ErrorCode errorCode = ACS_ACA_MessageStoreAccessPoint::Undefined;
	ACAMS_MSIP_Protocol::Primitive primitive = ACAMS_MSIP_Protocol::getPrimitive(myMSIP_socket, primitiveSize, buffer);

	switch (primitive) {

		case ACAMS_MSIP_Protocol::ConnectionRefused:
			{
				// MSD process refused the connection. There must be another connected.
				if(isValidHandle(__FILE__,__FUNCTION__,__LINE__,myMSIP_socket))	{
					ACE_OS::close(myMSIP_socket);
					myMSIP_socket= ACE_INVALID_HANDLE;
				}

				myConnectionState = Disconnect;
				std::string message;
				ACAMS_MSIP_Protocol::unpackConnectionRefused(buffer, primitiveSize, message);
				ACS_ACA_TRACE_MESSAGE("Connection refused by MSD process, message '%s'", message.c_str());
				return ACS_ACA_MessageStoreAccessPoint::ConnectionRefused;
			}
			break;

		case ACAMS_MSIP_Protocol::ConnectionAccepted:
			break;

		case ACAMS_MSIP_Protocol::CPSystemIdRequest: //The server is in MultipleCPSystem and requested a CP System ID
			{
				if (fxExchangeCPSystemId(errorCode)) {
					ACS_ACA_TRACE_MESSAGE("ERROR: Failed to exchange CP system ID, error == %d", errorCode);
					return errorCode;
				}
			}
			break;

		default:
			// BAD Primitive received
			if(isValidHandle(__FILE__,__FUNCTION__,__LINE__,myMSIP_socket))	{
				ACE_OS::close(myMSIP_socket);
				myMSIP_socket = ACE_INVALID_HANDLE;
			}
			myConnectionState = Disconnect;
			ACS_ACA_TRACE_MESSAGE("ERROR: Bad primitive received from MSIP server,");
			return ACS_ACA_MessageStoreAccessPoint::ProtocolError;
	}

	myConnectionState = Connect;
	myLastReadMsgNumber = myCommitFile->getMsgNumber();
	ACS_ACA_TRACE_MESSAGE("Connection OK, myLastReadMsgNumber == %llu", myLastReadMsgNumber);
	return ACS_ACA_MessageStoreAccessPoint::Undefined;
}

/*===================================================================
ROUTINE: commit
=================================================================== */
ACS_ACA_MessageStoreAccessPoint::ErrorCode ACAMSA_MsgStore::commit(const unsigned char * transactionData, const int transactionDataLength) {
	ACS_ACA_TRACE_FUNCTION;
	string commitFileName;

	if (myLastReadMsgNumber < this->getFirstMsgNumber(myCurrentFileName)) {
		if (myLastFinishedFile.empty())	{
			ACS_ACA_TRACE_MESSAGE("[myLastFinishedFile.empty], failed to commit!");
			return ACS_ACA_MessageStoreAccessPoint::NoMessageToCommit;
		}
		commitFileName = myLastFinishedFile;
	}
	else {
		if (myCurrentFileName.empty()) {
			ACS_ACA_TRACE_MESSAGE("ERROR: [myCurrentFileName.empty], failed to commit!");
			return ACS_ACA_MessageStoreAccessPoint::NoMessageToCommit;
		}
		commitFileName = myCurrentFileName;
	}

	if (!myCommitFile->commit(myLastReadMsgNumber, commitFileName, transactionData, transactionDataLength))	{
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'myCommitFile->commit' failed!");
		return ACS_ACA_MessageStoreAccessPoint::CommitFailed;
	}

	// Now remove any files before the one containing the just committed message
	if (!myLastFinishedFile.empty() && (myLastFinishedFile != myCommitFile->getFileName())) {
		ACS_ACA_TRACE_MESSAGE("Deleting files up to %s", myLastFinishedFile.c_str());

		if (myConnectionState == Connect)	{
			if (ACAMS_MSIP_Protocol::sendDeleteFile(myMSIP_socket, myLastFinishedFile))	{
				myLastFinishedFile = "";
			}
			else {
				ACS_ACA_TRACE_MESSAGE("ERROR: Failed to send 'sendDeleteFile' primitive, errno == %d", errno);
				disconnect((errno != EPIPE));
				return ACS_ACA_MessageStoreAccessPoint::SendPrimitiveFailed;
			}
		}
	}
	return ACS_ACA_MessageStoreAccessPoint::Undefined;
}

/*===================================================================
ROUTINE: commit
=================================================================== */
ACS_ACA_MessageStoreAccessPoint::ErrorCode ACAMSA_MsgStore::commit(unsigned long long numOfMessages,	const unsigned char * transactionData, const int transactionDataLength) {
	ACS_ACA_TRACE_FUNCTION;
	string commitFileName;

	if (myLastReadMsgNumber < this->getFirstMsgNumber(myCurrentFileName)) {
		if (myLastFinishedFile.empty())	{
			ACS_ACA_TRACE_MESSAGE("[myLastFinishedFile.empty], failed to commit!");
			return ACS_ACA_MessageStoreAccessPoint::NoMessageToCommit;
		}
		commitFileName = myLastFinishedFile;
	}
	else {
		if (myCurrentFileName.empty()) {
			ACS_ACA_TRACE_MESSAGE("ERROR: [myCurrentFileName.empty], failed to commit!");
			return ACS_ACA_MessageStoreAccessPoint::NoMessageToCommit;
		}
		commitFileName = myCurrentFileName;
	}

	if (!myCommitFile->commit(numOfMessages, myLastReadMsgNumber, commitFileName, transactionData, transactionDataLength)) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'myCommitFile->commit' failed!");
		return ACS_ACA_MessageStoreAccessPoint::CommitFailed;
	}

	// Now remove any files before the one containing the just committed message
	if (!myLastFinishedFile.empty() && (myLastFinishedFile != myCommitFile->getFileName())) {
		ACS_ACA_TRACE_MESSAGE("Deleting files up to %s", myLastFinishedFile.c_str());

		if (myConnectionState == Connect)	{
			if (ACAMS_MSIP_Protocol::sendDeleteFile(myMSIP_socket, myLastFinishedFile))	{
				myLastFinishedFile = "";
			}
			else {
				ACS_ACA_TRACE_MESSAGE("ERROR: Failed to send 'sendDeleteFile' primitive, errno == %d", errno);
				disconnect((errno != EPIPE));
				return ACS_ACA_MessageStoreAccessPoint::SendPrimitiveFailed;
			}
		}
	}
	return ACS_ACA_MessageStoreAccessPoint::Undefined;
}

/*===================================================================
ROUTINE: getHandle
=================================================================== */
ACE_HANDLE ACAMSA_MsgStore::getHandle() const {
	ACS_ACA_TRACE_FUNCTION;
	return myMSIP_socket;
}

/*===================================================================
ROUTINE: getMsg
=================================================================== */
ACS_ACA_MessageStoreAccessPoint::ErrorCode ACAMSA_MsgStore::getMsg(
		unsigned long long & msgNumber,
		unsigned char * & msgData,
		int & dataLength,
		ACS_ACA_MessageStoreMessage::State & state) {

	ACS_ACA_TRACE_FUNCTION;
	unsigned long long nextMsgNumber = 0;
	msgData = 0;
	dataLength = 0;
	state = ACS_ACA_MessageStoreMessage::Empty;

	// Flush any input on the MSIP-connection and attempt to set up a new connection if it has been lost.
	if (this->checkMSIP_Input()) {
		this->disconnect();

		for(int i=0;i<=2;i++) {
			if (this->connect() != ACS_ACA_MessageStoreAccessPoint::Undefined) {
				sleep(1);

				if(i == 2) {
					ACS_ACA_TRACE_MESSAGE("ERROR: Connection lost!");
					return ACS_ACA_MessageStoreAccessPoint::ConnectionLost;
				}
			}
			else {
				this->rollback();

				if (this->checkMSIP_Input()) {
					this->disconnect();
					ACS_ACA_TRACE_MESSAGE("ERROR: Connection lost!");
					return ACS_ACA_MessageStoreAccessPoint::ConnectionLost;
				}
				else break;
			}//else
		}//for
	}//if
	if (checkMSIP_input_timeout == true)
	{

		// The message is not in MsgStore yet. Tell the MSD process that the MSIP-client is waiting
		nextMsgNumber = myLastReadMsgNumber + 1;
		msgNumber = nextMsgNumber;
		ACS_ACA_TRACE_MESSAGE("No message to read, sending 'requestMsg' to the server");
		return this->requestMsg(nextMsgNumber);

	}

	if (myCurrentFile == ACE_INVALID_HANDLE) {
		// We have no file open yet. Find file to open and locate next message to read in it.
		string fileName = myCurrentFileName;
		if (!this->locateNextMsgToRead(myCurrentFile, fileName, nextMsgNumber)) {
			ACS_ACA_TRACE_MESSAGE("ERROR: Unable to locate the next message to read");
			this->disconnect();
			return ACS_ACA_MessageStoreAccessPoint::ConnectionLost;
		}

		if (myCurrentFile == ACE_INVALID_HANDLE) {
			ACS_ACA_TRACE_MESSAGE("No file to read, requesting message");
			msgNumber = nextMsgNumber;
			return this->requestMsg(nextMsgNumber);
		}

		// Now we have a new file open. Remove old file(s) at next commit
		if (fileName != myCurrentFileName) {
			myLastFinishedFile = myCurrentFileName;
			myCurrentFileName = fileName;
		}
	}

	// Now we have a file open. Read next message from it (unless we did
	// that last time we were here, and found a hole in the sequence).
	while (true) {
		if (!myNextMsgRead) {
			// First the header
			bool readHeader = true;
			ACE_INT32 bytesRead = 0;

			while (readHeader) {
				if(isValidHandle(__FILE__,__FUNCTION__,__LINE__,myCurrentFile))
					bytesRead = ACE_OS::read(myCurrentFile, (char *)myIO_Buffer, ACAMS_MsgHeaderSize);
				else
					ACS_ACA_TRACE_MESSAGE("Failed in reading at getMsg");

				if((bytesRead == -1) || ((bytesRead < ACAMS_MsgHeaderSize) && (bytesRead != 0))) {
					// Failed to read data file. That is serious.
					this->disconnect();
					ACS_ACA_TRACE_MESSAGE("ERROR: Failed to read data file");
					return ACS_ACA_MessageStoreAccessPoint::DiskReadFailed;
				}

				switch (bytesRead) {

					case 0: // The message is not in MsgStore yet. Tell the MSD process that the MSIP-client is waiting
						nextMsgNumber = myLastReadMsgNumber + 1;
						msgNumber = nextMsgNumber;
						ACS_ACA_TRACE_MESSAGE("No message to read, sending 'requestMsg' to the server");
						return this->requestMsg(nextMsgNumber);

					case 12: //ACAMS_MsgHeaderSize: Header read OK.
						if (ACE_OS::memcmp(myIO_Buffer, ACAMS_Eof_Mark, ACAMS_Eof_Mark_Length) == 0) {
							// Time to switch file
							string fileName = myCurrentFileName;

							if (!this->locateNextMsgToRead(myCurrentFile, fileName, nextMsgNumber)) {
								this->disconnect();
								ACS_ACA_TRACE_MESSAGE("ERROR: Failed to locate the next message to read");
								return ACS_ACA_MessageStoreAccessPoint::DiskReadFailed;
							}

							if (myCurrentFile == ACE_INVALID_HANDLE) {
								// No more file in store. Inform the MSD process that we are waiting.
								msgNumber = nextMsgNumber;
								ACS_ACA_TRACE_MESSAGE("No more messages to read, sending 'requestMsg' to the server");
								return this->requestMsg(nextMsgNumber);
							} 

							// Now we have a new file open. Update member data
							myLastFinishedFile = myCurrentFileName;
							myCurrentFileName = fileName;
						}
						else
							readHeader = false;

						break;

					default:
						break;
				} // End switch
			} // End while

			// Now read the rest of the message.
			memcpy((void*)&myMsgSize, (const void*)(myIO_Buffer + 8), sizeof(unsigned long)/2);

			if (myMsgSize > IO_BUFFER_SIZE) {
				disconnect();
				ACS_ACA_TRACE_MESSAGE("ERROR: Invalid 'myMsgSize' value read from file %s: myMsgSize == %lu, returning DiskReadFailed!",
						(getDataFileDirectoryName() + myCurrentFileName).c_str(), myMsgSize);
				return ACS_ACA_MessageStoreAccessPoint::DiskReadFailed;
			}

			if(isValidHandle(__FILE__,__FUNCTION__,__LINE__,myCurrentFile))
				bytesRead = ACE_OS::read( myCurrentFile, (myIO_Buffer + ACAMS_MsgHeaderSize), myMsgSize);
			else
				ACS_ACA_TRACE_MESSAGE("ERROR: Failed to read, invalid handle");

			if ((bytesRead < 0) || ( static_cast<unsigned int>(bytesRead) < myMsgSize)) {
				// Failed to read data file. That is serious.
				this->disconnect();

				ACS_ACA_TRACE_MESSAGE("Call 'read' failed for file %s, bytes read == %d, errno == %d",
						(getDataFileDirectoryName() + myCurrentFileName).c_str(), bytesRead, errno);
				return ACS_ACA_MessageStoreAccessPoint::DiskReadFailed;
			}
			myNextMsgRead = true;
		}

		// Extract message number out of file
		unsigned long long readMsgNumber = convertToUll(myIO_Buffer);
		bool NotZeroMessage = (readMsgNumber > 0);

		// Verify correct sequence of messages
		if ((readMsgNumber == (myLastReadMsgNumber + 1)) || (!NotZeroMessage)) {
			// Message read OK
			msgData = myIO_Buffer + ACAMS_MsgHeaderSize;
			dataLength = myMsgSize - DataOffset;

			if (dataLength > 0)	// A good, healthy message is found
				state = ACS_ACA_MessageStoreMessage::Filled;
			else	// Empty message means skipped by MTAP
				state = ACS_ACA_MessageStoreMessage::Skipped;

			myNextMsgRead = false;
			ACS_ACA_TRACE_MESSAGE("Message read OK, readMsgNumber == %llu", readMsgNumber);
		}
		else {
			if (readMsgNumber <= myLastReadMsgNumber)	{
				myNextMsgRead = false;
				continue;
			}

			// There is a hole in the sequence. Must return an empty message.
			msgData = 0;
			dataLength = 0;
			state = ACS_ACA_MessageStoreMessage::Lost;
			ACS_ACA_TRACE_MESSAGE("Message hole in the sequence, readMsgNumber == %llu", readMsgNumber);
		}   

		if (NotZeroMessage) {
			++myLastReadMsgNumber;
			msgNumber = myLastReadMsgNumber;
		}
		else
			msgNumber = readMsgNumber;

		break;
	} // end while (true)   

	return ACS_ACA_MessageStoreAccessPoint::Undefined;
}

/*===================================================================
ROUTINE: getMsgAndRevInfo
=================================================================== */
ACS_ACA_MessageStoreAccessPoint::ErrorCode ACAMSA_MsgStore::getMsgAndRevInfo(
		ULONGLONG & msgNumber,
		unsigned char * & msgData,
		int & dataLength,
		ACS_ACA_MessageStoreMessage::State & state,
		int & primitiveFormat,
		int & versionNumber,
		int keep) {
	ACS_ACA_TRACE_FUNCTION;

	// keep: 1 = retrieve, 0 = get
	unsigned long long  nextMsgNumber = 0;
	msgData = 0;
	dataLength = 0;
	primitiveFormat = 0;  // Default value
	versionNumber = 0;    // Default value
	state = ACS_ACA_MessageStoreMessage::Empty;

	// Flush any input on the MSIP-connection and attempt to set up a new connection if it has been lost.
	if (this->checkMSIP_Input()) {
		this->disconnect();
		ACS_ACA_TRACE_MESSAGE("Connection Lost");
		return ACS_ACA_MessageStoreAccessPoint::ConnectionLost;
	}

        if (checkMSIP_input_timeout == true)
        {

                // The message is not in MsgStore yet. Tell the MSD process that the MSIP-client is waiting
                nextMsgNumber = myLastReadMsgNumber + 1;
                msgNumber = nextMsgNumber;
                ACS_ACA_TRACE_MESSAGE("No message to read, sending 'requestMsg' to the server");
                return this->requestMsg(nextMsgNumber);

        }

	if (myCurrentFile == ACE_INVALID_HANDLE) {
		// We have no file open yet. Find file to open and locate next message to read in it.
		string fileName = myCurrentFileName;
		if (!this->locateNextMsgToRead(myCurrentFile, fileName, nextMsgNumber)) { // Serious trouble.
			ACS_ACA_TRACE_MESSAGE("ERROR: Unable to locate the next message to read");
			this->disconnect();
			return ACS_ACA_MessageStoreAccessPoint::ConnectionLost;
		}

		if (myCurrentFile == ACE_INVALID_HANDLE) {
			// Still no file. Inform the MSD process that we are waiting for next message.
			ACS_ACA_TRACE_MESSAGE("No file to read, requesting message");
			msgNumber = nextMsgNumber;
			return this->requestMsg(nextMsgNumber);
		}

		// Now we have a new file open. Remove old file(s) at next commit
		if (fileName != myCurrentFileName) {
			myLastFinishedFile = myCurrentFileName;
			myCurrentFileName = fileName;
		}
	}

	// Now we have a file open. Read next message from it (unless we did
	// that last time we were here, and found a hole in the sequence).
	while (true) {
		if (!myNextMsgRead) {
			// First the header
			bool readHeader = true;	
			ACE_INT32 bytesRead = 0;

			while (readHeader) {
				if (isValidHandle(__FILE__,__FUNCTION__,__LINE__,myCurrentFile))
					bytesRead = ACE_OS::read(myCurrentFile, (char*)myIO_Buffer, ACAMS_MsgHeaderSize);
				else
					ACS_ACA_TRACE_MESSAGE("ERROR: Failed to read, invalid handle");

				if ((bytesRead == -1) || ((bytesRead < ACAMS_MsgHeaderSize) && (bytesRead != 0))) {
					// Failed to read data file. That is serious.
					this->disconnect();
					ACS_ACA_TRACE_MESSAGE("ERROR: Failed to read data file");
					return ACS_ACA_MessageStoreAccessPoint::DiskReadFailed;
				}

				switch (bytesRead) {

					case 0: // The message is not in MsgStore yet. Tell the MSD process that the MSIP-client is waiting
						nextMsgNumber = myLastReadMsgNumber + 1;
						msgNumber = nextMsgNumber;
						ACS_ACA_TRACE_MESSAGE("No message to read, sending 'requestMsg' to the server");
						return this->requestMsg(nextMsgNumber);

					case 12: //ACAMS_MsgHeaderSize: Header read OK.
						if (ACE_OS::memcmp(myIO_Buffer, ACAMS_Eof_Mark, ACAMS_Eof_Mark_Length) == 0) {
							// Time to switch file
							string fileName = myCurrentFileName;

							if (!this->locateNextMsgToRead(myCurrentFile, fileName, nextMsgNumber))	{
								this->disconnect();
								ACS_ACA_TRACE_MESSAGE("ERROR: Failed to locate the next message to read");
								return ACS_ACA_MessageStoreAccessPoint::DiskReadFailed;
							} 

							if (myCurrentFile == ACE_INVALID_HANDLE) {
								// No more file in store. Inform the MSD process that we are waiting.
								msgNumber = nextMsgNumber;
								ACS_ACA_TRACE_MESSAGE("No more messages to read, sending 'requestMsg' to the server");
								return this->requestMsg(nextMsgNumber);
							} 

							// Now we have a new file open. Update member data
							myLastFinishedFile = myCurrentFileName;
							myCurrentFileName = fileName;
						}
						else
							readHeader = false;
						break;

					default:
						break;
				} // End switch
			} // End while

			// Now read the rest of the message.
			memcpy((void*)&myMsgSize, (const void*)(myIO_Buffer+8), sizeof(unsigned long)/2);

			if (myMsgSize > IO_BUFFER_SIZE) {
				disconnect();
				ACS_ACA_TRACE_MESSAGE("ERROR: Invalid 'myMsgSize' value read from file %s: myMsgSize == %lu, returning DiskReadFailed!",
						(getDataFileDirectoryName() + myCurrentFileName).c_str(), myMsgSize);
				return ACS_ACA_MessageStoreAccessPoint::DiskReadFailed;
			}


			if (isValidHandle(__FILE__,__FUNCTION__,__LINE__,myCurrentFile))
				bytesRead = ACE_OS::read( myCurrentFile, (myIO_Buffer + ACAMS_MsgHeaderSize), myMsgSize );
			else
				ACS_ACA_TRACE_MESSAGE("ERROR: Failed in reading at getMsgAndRevInfo");

			if ((bytesRead < 0) || ( static_cast<unsigned int>(bytesRead) < myMsgSize)) {
				// Failed to read data file. That is serious.
				if(myCurrentFileName.empty()|| getDataFileDirectoryName().empty())	{
					//HR70638 getting segmentation while appending two string
					ACS_ACA_TRACE_MESSAGE("ERROR: Failed to read data file reporting event filename is empty");
				}

				this->disconnect();
				ACS_ACA_TRACE_MESSAGE("Call 'read' failed for file %s, bytes read == %d",
						(getDataFileDirectoryName() + myCurrentFileName).c_str(), bytesRead);
				return ACS_ACA_MessageStoreAccessPoint::DiskReadFailed;
			}
			myNextMsgRead = true;
		}

		// Extract message number out of file
		unsigned long long readMsgNumber = convertToUll(myIO_Buffer);
		bool NotZeroMessage = (readMsgNumber > 0);

		// Verify correct sequence of messages
		if ((readMsgNumber == (myLastReadMsgNumber + 1)) || (!NotZeroMessage)) {
			// Message read OK
			msgData = myIO_Buffer + ACAMS_MsgHeaderSize; // + 12 bytes
			dataLength = myMsgSize - DataOffset; // - 8 bytes
			unsigned char tempv[4];
			long tempOffset = 0;
			ACE_OS::memcpy(tempv, msgData + dataLength, 2);
			primitiveFormat = tempv[0];
			versionNumber = tempv[1];
			versionNumber = (versionNumber >> 1); 

			if ((primitiveFormat > 0) || (versionNumber > 0)) {
				//must check which mtap protocol version it is
				ACE_OS::memcpy(tempv, msgData + dataLength, 4);
				tempOffset = tempv[0] + (tempv[1] * 256);
				if ((dataLength + 12 == tempOffset) && (tempv[2] == 0) && (tempv[3] == 0)) {
					//version 2
					primitiveFormat = 0;
					versionNumber = 0;
				}
			}

			ACS_ACA_TRACE_MESSAGE("primitiveFormat == %d, versionNumber == %d, dataLength == %d, tempOffset == %ld",
					primitiveFormat, versionNumber, dataLength, tempOffset);

			if (!NotZeroMessage)
				versionNumber = 128; // Expedited data message

			if (dataLength > 0) // A good, healthy message is found
				state = ACS_ACA_MessageStoreMessage::Filled;
			else // Empty message means skipped by MTAP
				state = ACS_ACA_MessageStoreMessage::Skipped;

			if (keep == 0)
				myNextMsgRead = false;

			ACS_ACA_TRACE_MESSAGE("Message read OK, readMsgNumber == %llu", readMsgNumber);
		}
		else {
			if (readMsgNumber <= myLastReadMsgNumber)	{
				myNextMsgRead = false;
				continue;  // Next message
			}

			// There is a hole in the sequence. Then return an empty message.
			msgData = 0;
			dataLength = 0;
			state = ACS_ACA_MessageStoreMessage::Lost;
			ACS_ACA_TRACE_MESSAGE("Message hole in the sequence, readMsgNumber == %llu", readMsgNumber);
		}

		if (NotZeroMessage) {
			unsigned long long savedMsgNumber = myLastReadMsgNumber;
			++myLastReadMsgNumber;
			msgNumber = myLastReadMsgNumber;

			if (keep == 1)
				myLastReadMsgNumber = savedMsgNumber;
		}
		else
			msgNumber = readMsgNumber;

		break;
	} // end while (true)

	return ACS_ACA_MessageStoreAccessPoint::Undefined;
}

/*===================================================================
ROUTINE: getMsgNumber_LastCommitted
=================================================================== */
unsigned long long ACAMSA_MsgStore::getMsgNumber_LastCommitted() {
	ACS_ACA_TRACE_FUNCTION;

	if (!myCommitFile) {
		try {
			myCommitFile = new (std::nothrow) ACAMS_CommitFile(this->getCommitFileName());
			if (!myCommitFile) {
				ACS_ACA_TRACE_MESSAGE("ERROR: Failed to allocate ACAMS_CommitFile object");
				return 0;
			}
		}
		catch (std::exception & e) {
			ACS_ACA_TRACE_MESSAGE("ERROR: Call 'ACAMS_CommitFile()' threw an exception '%s'", e.what());
			return 0;
		}
	}

	return myCommitFile->getMsgNumber();
}


/*===================================================================
ROUTINE: getMsgNumber_LastRead
=================================================================== */
unsigned long long ACAMSA_MsgStore::getMsgNumber_LastRead() const {
	ACS_ACA_TRACE_FUNCTION;
	return myLastReadMsgNumber;
}

/*===================================================================
ROUTINE: getTransactionData
=================================================================== */
const unsigned char * ACAMSA_MsgStore::getTransactionData(int & transactionDataLength) const {
	ACS_ACA_TRACE_FUNCTION;

	transactionDataLength = myCommitFile->getTransactionDataLength();
	return myCommitFile->getTransactionData();
}

/*===================================================================
ROUTINE: rollback
=================================================================== */
ACS_ACA_MessageStoreAccessPoint::ErrorCode ACAMSA_MsgStore::rollback() {
	ACS_ACA_TRACE_FUNCTION;

	unsigned long long nextMsgNumber = 0ULL;
	if (isValidHandle(__FILE__,__FUNCTION__,__LINE__,myCurrentFile)) {
		ACE_OS::close(myCurrentFile);
		myCurrentFile = ACE_INVALID_HANDLE;
	}

	myCurrentFileName = "";
	myLastFinishedFile = "";
	myLastReadMsgNumber = myCommitFile->getMsgNumber();
	if (!this->locateNextMsgToRead(myCurrentFile, myCurrentFileName, nextMsgNumber)) {	// Serious trouble.
		this->disconnect();
		ACS_ACA_TRACE_MESSAGE("Failed to locate the next message to read");
		return ACS_ACA_MessageStoreAccessPoint::DiskReadFailed;
	}

	return ACS_ACA_MessageStoreAccessPoint::Undefined;
}

/*===================================================================
ROUTINE: checkMSIP_Input
=================================================================== */
int ACAMSA_MsgStore::checkMSIP_Input() {
	ACS_ACA_TRACE_FUNCTION;

	int isConnectionLost = 0;
	if (myConnectionState == Connect) {
		fd_set readFdSet;
		FD_ZERO(&readFdSet);
		FD_SET(myMSIP_socket, &readFdSet);
		struct timeval noTime = {0,0};
		int rc = ::select(myMSIP_socket + 1, &readFdSet, 0, 0, &noTime);

		if ((rc != -1) && (rc > 0))	{
			unsigned char buffer[2 * 1024];
			unsigned int primitiveSize;

			ACAMS_MSIP_Protocol::Primitive primitive = ACAMS_MSIP_Protocol::getPrimitive(myMSIP_socket, primitiveSize, buffer);
			ACAMS_MSIP_Protocol::ErrorCode result;

			switch (primitive) {
				case ACAMS_MSIP_Protocol::None: // Lost connection
					ACS_ACA_TRACE_MESSAGE("ERROR: MSIP connection lost for CP ID %u", _cpSystemId);
					isConnectionLost = 1;
					break;

				case ACAMS_MSIP_Protocol::MsgAvailable:
					result = ACAMS_MSIP_Protocol::unpackMsgAvailable(buffer, primitiveSize);
					if (result != ACAMS_MSIP_Protocol::NoError)	{
						// This is weird (should never happen). Let's close down
						ACS_ACA_TRACE_MESSAGE("ERROR: Unpacking message failed for the CP having id = %u, call_result == %d", _cpSystemId, result);
						isConnectionLost = 1;
					}
					else
						checkMSIP_input_timeout = false;
					break;

				default:
					ACS_ACA_TRACE_MESSAGE("ERROR: Received an unknown primitive");
			}
		}
		else if (rc < 0) { // select error
			ACS_ACA_TRACE_MESSAGE("ERROR: Call 'select' failed for the CP having id = %u, errno == %d", _cpSystemId, errno);
			isConnectionLost = 1;
		}
		else if (rc==0) {
			//ACS_ACA_TRACE_MESSAGE("ERROR: Call 'select' timed out for the CP having id = %u, errno == %d", _cpSystemId, errno);
			checkMSIP_input_timeout = true;
		}
	}
	else {
		ACS_ACA_TRACE_MESSAGE("ERROR: Bad connection state, state == %d", myConnectionState);
		isConnectionLost = 1;
	}

	return isConnectionLost;
}

/*===================================================================
ROUTINE: locateNextMsgToRead
=================================================================== */
bool ACAMSA_MsgStore::locateNextMsgToRead(ACE_HANDLE & currentFile1, string & currentFileName, unsigned long long & nextMsgNumber) {
	ACS_ACA_TRACE_FUNCTION;

	bool getMsgNumberFromCommitFile = false;
	string nextFileName = "";
	bool foundDir = true;

	if (currentFile1 == ACE_INVALID_HANDLE) {
		// No file open. If we haven't started reading from any file yet, we use
		// commitFile to find out where to start reading, otherwise we take next
		// file in sequence	
		if (currentFileName.empty()) {
			nextFileName = myCommitFile->getFileName();
			if (nextFileName.empty()) { // Get first file in message store
				nextFileName = this->findNextFile("", &foundDir);
			}
			else { //-- check that the file exists in the MS.
				ACE_stat statBuff;
				int ret = ACE_OS::stat((getDataFileDirectoryName() + "/" + nextFileName).c_str(), &statBuff);
				if (ret != 0) // Get first file in message store
					nextFileName = this->findNextFile("", &foundDir);
				else
					getMsgNumberFromCommitFile = true;
			}
		}
		else
			nextFileName = this->findNextFile(currentFileName, &foundDir);;
	}
	else {
		// Close the current file and locate the next in sequence, if any
		if(isValidHandle(__FILE__,__FUNCTION__,__LINE__,currentFile1)) {
			int retCode = ACE_OS::close(currentFile1 );
			if (retCode == ACE_INVALID_HANDLE) {
				ACS_ACA_TRACE_MESSAGE("ERROR: Failed to close data file %s, errno = %d", currentFileName.c_str(), ACE_OS::last_error());
			}
			currentFile1 = ACE_INVALID_HANDLE;
		}

		ACS_ACA_TRACE_MESSAGE("Finished to read file '%s'", currentFileName.c_str());
		nextFileName = this->findNextFile(currentFileName, &foundDir);
	}

	if (foundDir == false) { // The dataFiles directory was not even found
		ACS_ACA_TRACE_MESSAGE("Data files directory not found!");
		return false;
	}

	if (nextFileName.empty()) {
		// The file is not yet in the message store
		nextMsgNumber = myLastReadMsgNumber + 1;
		ACS_ACA_TRACE_MESSAGE("The file is not yet in the message store");
		return true;
	}

	currentFileName = nextFileName;
	// Now we have the name of next file to read from. Open it
	string completeFileName = this->getDataFileDirectoryName() +"/"+ currentFileName;

	if(currentFile1 == ACE_INVALID_HANDLE)
		currentFile1 = ACE_OS::open(completeFileName.c_str(), O_RDONLY, ACE_DEFAULT_OPEN_PERMS);
	else
		ACS_ACA_TRACE_MESSAGE("Cannot open file, invalid handle");

	if (!isValidHandle(__FILE__,__FUNCTION__,__LINE__,currentFile1,completeFileName.c_str(),O_RDONLY,0)) {
		ACS_ACA_TRACE_MESSAGE("ERROR: Failed to open data file %s", completeFileName.c_str());
		return false;
	}

	struct stat statBuffer;
	int retStat = stat(completeFileName.c_str(), &statBuffer);
	if( retStat == 0 && statBuffer.st_size == 0) {
		if(isValidHandle(__FILE__,__FUNCTION__,__LINE__,currentFile1)) {
			ACE_OS::close(currentFile1);
			currentFile1 = ACE_INVALID_HANDLE;
		}
		nextMsgNumber = myLastReadMsgNumber + 1;
		ACS_ACA_TRACE_MESSAGE("Call 'stat' OK, returning OK");
		return true;
	}

	// If we haven't read anything before, we must locate the message after
	// last commited message. Otherwise, next message will be the first in file
	nextMsgNumber = this->getFirstMsgNumber(this->getDataFileDirectoryName() +"/"+nextFileName);

	if (!getMsgNumberFromCommitFile) {
		ACS_ACA_TRACE_MESSAGE("[!getMsgNumberFromCommitFile], returning true");
		return true;
	}

	if (nextMsgNumber > myLastReadMsgNumber) {
		ACS_ACA_TRACE_MESSAGE("[nextMsgNumber > myLastReadMsgNumber], returning true");
		return true;
	}

	// Find the message after 'myLastReadMsgNumber'
	unsigned char buffer[ACAMS_MsgHeaderSize];
	unsigned long msgSize ;
	bool finished = false;
	bool lastCommitedPassed = false;
	bool fileEmpty = true;

	while (!finished) {
		ACE_INT32 ret = 0;
		if(isValidHandle(__FILE__,__FUNCTION__,__LINE__,currentFile1))
			ret = ACE_OS::read(currentFile1,(char*)buffer,ACAMS_MsgHeaderSize);
		else
			ACS_ACA_TRACE_MESSAGE("Failed to read file, invalid handle");

		if (ret == -1) {
			ACS_ACA_TRACE_MESSAGE("Call 'read' failed for file %s, errno = %d, returning false",
					completeFileName.c_str(), ACE_OS::last_error());
			return false;
		}

		switch (ret) {
			case 0:
				// We are at the end of the file.
				// A special case, the last Message in file was also the last read one.
				finished = true;
				if (!fileEmpty && lastCommitedPassed)
					++nextMsgNumber;
				else {
					if(isValidHandle(__FILE__,__FUNCTION__,__LINE__,currentFile1)) {
						ACE_OS::close(currentFile1);
						currentFile1 = ACE_INVALID_HANDLE;
					}

					ACS_ACA_TRACE_MESSAGE("[case 0], returning false");
					return false;
				}
				break;

			case ACAMS_MsgHeaderSize: 
				fileEmpty = false;
				if (ACE_OS::memcmp(buffer, ACAMS_Eof_Mark, ACAMS_Eof_Mark_Length) == 0)	{
					// End of file
					finished = true;
					++nextMsgNumber;
					if ( ACE_OS::lseek(currentFile1, -ACAMS_MsgHeaderSize, SEEK_CUR ) == -1 )	{
						ACS_ACA_TRACE_MESSAGE("ERROR: Failed to set data file pointer for file %s, errno = %d",
								completeFileName.c_str(), ACE_OS::last_error());
						return false;
					}
				}
				else {
					if (lastCommitedPassed) {
						finished = true;
						if (ACE_OS::lseek(currentFile1, -ACAMS_MsgHeaderSize, SEEK_CUR) == -1) {
							ACS_ACA_TRACE_MESSAGE("ERROR: Failed to set data file pointer for file %s, errno = %d",
									completeFileName.c_str(), ACE_OS::last_error());
							return false;
						}
					}
					else {
						nextMsgNumber = convertToUll(buffer); 
						if (nextMsgNumber == myLastReadMsgNumber)
							lastCommitedPassed = true;

						msgSize = *((unsigned int*)(buffer + sizeof(unsigned long)));
						bReadbfCommit += (unsigned long)ret+msgSize;

						int retVal =  ACE_OS::lseek(currentFile1, msgSize, SEEK_CUR );
						if ( retVal == -1) {
							ACS_ACA_TRACE_MESSAGE("ERROR: Failed to set data file pointer for file %s, errno = %d",
									completeFileName.c_str(), ACE_OS::last_error());
							return false;
						}
					}
				}
				break;

			default: 
				break;

		} // end switch
	} // End while

	return true;
}

/*===================================================================
ROUTINE: requestMsg
=================================================================== */
ACS_ACA_MessageStoreAccessPoint::ErrorCode ACAMSA_MsgStore::requestMsg(const unsigned long long & reqNum) {
	ACS_ACA_TRACE_FUNCTION;

	if (myConnectionState == Connect) {
		if (ACAMS_MSIP_Protocol::sendRequestMsg(myMSIP_socket, reqNum))
			return ACS_ACA_MessageStoreAccessPoint::Undefined;

		// Failed to send MSIP primitive.
		ACS_ACA_TRACE_MESSAGE("ERROR: Call 'sendRequestMsg' failed, errno == %d", errno);
		this->disconnect();
	}
	else {
		ACS_ACA_TRACE_MESSAGE("ERROR: The state is not connected!");
		return ACS_ACA_MessageStoreAccessPoint::NotConnected;
	}

	return ACS_ACA_MessageStoreAccessPoint::SendPrimitiveFailed;
}

/*===================================================================
ROUTINE: fxExchangeCPSystemId
=================================================================== */
int ACAMSA_MsgStore::fxExchangeCPSystemId(ACS_ACA_MessageStoreAccessPoint::ErrorCode & errorCode) {
	ACS_ACA_TRACE_FUNCTION;
	unsigned char buffer[2 * 1024];
	unsigned int primitiveSize;
	ACAMS_MSIP_Protocol::Primitive primitive = ACAMS_MSIP_Protocol::CPSystemIdRequest;

	if (_isMultipleCPSystem) {
		//Multiple CP System
		if (!ACAMS_MSIP_Protocol::sendCPSystemId(myMSIP_socket, _cpSystemId))	{
			ACS_ACA_TRACE_MESSAGE("ERROR: Call 'sendCPSystemId' failed, errno == %d", errno);

			if(isValidHandle(__FILE__,__FUNCTION__,__LINE__,myMSIP_socket))	{
				ACE_OS::close(myMSIP_socket);
				myMSIP_socket = ACE_INVALID_HANDLE;
			}
			myConnectionState = Disconnect;
			errorCode = ACS_ACA_MessageStoreAccessPoint::SendPrimitiveFailed;
			return 1;
		}

		primitive = ACAMS_MSIP_Protocol::getPrimitive(myMSIP_socket, primitiveSize, buffer);

		switch (primitive) {

			case ACAMS_MSIP_Protocol::ConnectionRefused:
				{
					// MSD process refused the connection.
					if(isValidHandle(__FILE__,__FUNCTION__,__LINE__,myMSIP_socket)) {
						ACE_OS::close(myMSIP_socket);
						myMSIP_socket = ACE_INVALID_HANDLE;
					}
					myConnectionState = Disconnect;
					std::string message;
					ACAMS_MSIP_Protocol::unpackConnectionRefused(buffer, primitiveSize, message);
					errorCode = ACS_ACA_MessageStoreAccessPoint::ConnectionRefused;
					ACS_ACA_TRACE_MESSAGE("ERROR: Connection refused by MS Daemon, message '%s'", message.c_str());
					return 1;
				}
				break;

			case ACAMS_MSIP_Protocol::ConnectionAccepted:
				break;

			default:
				if(isValidHandle(__FILE__,__FUNCTION__,__LINE__,myMSIP_socket))	{
					ACE_OS::close(myMSIP_socket);
					myMSIP_socket = ACE_INVALID_HANDLE;
				}
				myConnectionState = Disconnect;
				errorCode = ACS_ACA_MessageStoreAccessPoint::RecvPrimitiveFailed;
				ACS_ACA_TRACE_MESSAGE("ERROR: Bad primitive received");
				return 1;
		}
	}
	else { //Single CP System
		ACS_ACA_TRACE_MESSAGE("MSIP server requested a CP system ID, but i'm in SCP system!");

		if (!ACAMS_MSIP_Protocol::sendCPSystemIdUnavailable(myMSIP_socket)) {
			if(isValidHandle(__FILE__,__FUNCTION__,__LINE__,myMSIP_socket))	{
				ACE_OS::close(myMSIP_socket);
				myMSIP_socket = ACE_INVALID_HANDLE;
			}
			myConnectionState = Disconnect;
			errorCode = ACS_ACA_MessageStoreAccessPoint::SendPrimitiveFailed;
			ACS_ACA_TRACE_MESSAGE("ERROR: Call 'sendCPSystemIdUnavailable' failed, errno == %d", errno);
			return 1;
		}

		primitive = ACAMS_MSIP_Protocol::getPrimitive(myMSIP_socket, primitiveSize, buffer);
		if (primitive == ACAMS_MSIP_Protocol::ConnectionRefused) {
			// MSD process refused the connection correctly.
			std::string message;
			ACAMS_MSIP_Protocol::unpackConnectionRefused(buffer, primitiveSize, message);
			errorCode = ACS_ACA_MessageStoreAccessPoint::ConnectionRefused;
			ACS_ACA_TRACE_MESSAGE("Connection refused correctly, message '%s'", message.c_str());
		}
		else {
			errorCode = ACS_ACA_MessageStoreAccessPoint::RecvPrimitiveFailed;
			ACS_ACA_TRACE_MESSAGE("ERROR: Bad primitive received");
		}

		if(isValidHandle(__FILE__,__FUNCTION__,__LINE__,myMSIP_socket)) {
			ACE_OS::close(myMSIP_socket);
			myMSIP_socket = ACE_INVALID_HANDLE;
		}
		myConnectionState = Disconnect;
		ACS_ACA_TRACE_MESSAGE("Connection disconnected!");
		return 1;
	}

	return 0;
}
