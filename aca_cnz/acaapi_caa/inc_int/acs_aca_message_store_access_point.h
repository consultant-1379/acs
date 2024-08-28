/*=================================================================== */
/**
   @file   acs_aca_message_store_access_point.h

   @brief Header file for MSA API.

	This module contains all the declarations useful to
	specify the class ACS_ACA_MessageStoreAccessPoint
	This class gives a client application access to the received messages
	in a Message Store (MS). In order to connect, the client must supply the
	name of the Message Store and the AXE site name. If no other client is
	connected and the MS is up and running, a connection is established and
	the client can start to fetch messages.
	Reading is always done in a non-blocking fashion, i.e. when
	asking for the next message, it will return instantly and deliver an
	empty message, if no message was available. If event-driven access is
	wanted, the client's process can fetch the socket identity used for the
	communication to the service process and then be notified when a message
	is available.

	When the client application has safely processed one or more messages,
	it must commit them, implying that MS can delete them.
	If no committing is done, the MS will just grow and grow, and finally
	be filled up.

	If a connection towards an MS is broken and re-established, an explicit
	rollback() will be made, i.e the MS will move back to the first
	non-committed message.

	All (dis)connect/access methods are boolean, and return true on success,
	and if they fail, false will be returned and an error code will be set.


	@version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       29/01/2013     XHARBAV       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_ACA_MessageStoreAccessPoint_H
#define ACS_ACA_MessageStoreAccessPoint_H


/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>

#include <ace/ACE.h>

#include "acs_aca_message_store_message.h" // A message for MS

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACS_ACA_MessageStoreAccessPointR3 

*/
/*=================================================================== */
class ACS_ACA_InternalMessageStoreAccessPoint;
class ACS_ACA_MessageStoreAccessPointR3 {
	/*=====================================================================
                        PUBLIC DECLARATION SECTION
	==================================================================== */
	public:
	/*=====================================================================
                        ENUMERATED DECLARATION SECTION
	==================================================================== */

	/*=================================================================== */
	/**
		@brief  ErrorCode 
	*/
	/*=================================================================== */
	enum ErrorCode {					// Result code from operations in MS
		Undefined,		    			// Undefined error code - All OK
		NoSuchMsgStore,					// Message Store not found
		ConnectFailed,					// Connection to ACA server failed
		DisconnectFailed,				// Disconnection from ACA server failed
		CommitFailed,						// Committing of message failed
		AlreadyConnected,				// Connection to ACA server already done
		AnotherConnected,				// Another client is already connected to ACA server
		NotConnected,						// Client already disconnected
		ConnectionLost,					// Spontaneous disconnection from ACA server
		ConnectionRefused,			// ACA server refused the connection
		MemoryError,						// Failed to allocate memory
		CommitFileError,				// Error while creating the commit file object
		SocketError,						// Failed to create the socket object
		SendPrimitiveFailed,		// Failed to send an MSIP protocol primitive
		RecvPrimitiveFailed,		// Failed to receive an MSIP protocol primitive
		ProtocolError,					// Bad use of MSIP protocol
		DiskReadFailed,					// Failed to read from the disk
		NoMessageToCommit				// No message is available to be committed
	};
	/*=================================================================== */
	/**
		@brief  State 
	*/
	/*=================================================================== */
	enum State { 		// State of Message Store (MS)
		Disconnected, // Disconnected from client
		Connected     // Connected to client
	};

	/*=================================================================== */
	/**
		@brief	TransactionIdDataMaxLength
			
			Defines the maximum length of a transaction ID data unit.
	*/
	/*=================================================================== */
	static const int TransactionIdDataMaxLength;
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief		ACS_ACA_MessageStoreAccessPointR3
				Constructor of ACS_ACA_MessageStoreAccessPoint
				Only one instance can access a particular message
				store at a time.

		@param		msName
				Message Store Name
	
		@param		cpSite
				Central Processor Name

	*/
	/*=================================================================== */
	ACS_ACA_MessageStoreAccessPointR3(const char * msName, const char * cpSite);
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief		ACS_ACA_MessageStoreAccessPointR3
				Constructor of ACS_ACA_MessageStoreAccessPoint
				Only one instance can access a particular message
				store at a time.

		@param		msName
				Message Store Name
	
		@param		cpSystemId
				Central Processor Name

	*/
	/*=================================================================== */
	ACS_ACA_MessageStoreAccessPointR3(const char * msName, unsigned cpSystemId);
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief		~ACS_ACA_MessageStoreAccessPointR3
				 Destructor of ACS_ACA_MessageStoreAccessPoint
	*/
	/*=================================================================== */
	virtual ~ACS_ACA_MessageStoreAccessPointR3();
	/*=================================================================== */
    
	/*=================================================================== */
	/**
		@brief		getState
		
		@return		State
				This method returns the current state of Message Store.
				Either Connected or Disconnected.
	*/
	/*=================================================================== */
	State getState() const;
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief		getLastError

		@return		ErrorCode
				This method returns the error code
				from previous operation.
	*/
	/*=================================================================== */
	ErrorCode getLastError() const;
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief		getLastErrorName
		
		@return		std::string
				This method returns the error code name
				from previous operation.
	*/
	/*=================================================================== */
	std::string getLastErrorName() const;
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief		clearLastError
				This method clears the error code.
	
		@return		void
	*/
	/*=================================================================== */
	void clearLastError();
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief		connect
				This method setups a one-to-one association
				with a message store. If re-establishment
				happens after losing connection, then the message
				sequence is kept intact.

		@return		bool
				true - Success
				false - Failure
	*/
	/*=================================================================== */
	bool connect();
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief		disconnect
				This method removes the association with a 
				message store.

		@return		bool
				true- Successful disconnection or already
				disconnected.
				false- Failure.
	*/
	/*=================================================================== */
	bool disconnect();
	/*=================================================================== */
  
	/*=================================================================== */
	/**
		@brief		getHandle
				This method gives access to a socket which can
				be used for notification when a new message is 
				available.

		@return		ACE_HANDLE
	*/
	/*=================================================================== */
	ACE_HANDLE getHandle() const;
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief		getMessage
				This method is used to get a message from a 
				message store. It will return the first un-read
				message, if available. If the message store is empty,
				then an empty message will be returned.

		@return		bool
				true - Successful
				false- Failure. 
	*/
	/*=================================================================== */
	bool getMessage(ACS_ACA_MessageStoreMessage & message);
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief		getMessageAndRevisionInfo
				This method is similar to getMessage() method
				, but it also returns the additionalData and
				versionNumber for the returned message.
				additionalData and versionNumber is set to 0
				if the message is Empty' or 'Lost' or if 
				the method returns false.
		
		@param		message
				The read message.

		@param		additionalData
				A number which indicates the type of the data.
				Value 0 indicates 'old format', that is, the
				message does not contain information about
				type of the data
				Values in the range 1-254 indicates legal formats.

		@param		versionNumber
				Indicates the revision step of the data.
				Value 0 indicates 'old format', that is, the
				message does not contain information about
				revision step of the data. Values in the range 1-126
				indicates legal versions.
				Value 127 is reserved for future use.
				Value 128 is used for Expedited Data Messages.
		
		@return		bool
				true - Successful
				false- Failure
	*/
	/*=================================================================== */
	bool getMessageAndRevisionInfo( ACS_ACA_MessageStoreMessage & message, 
					int & additionalData, 
					int & versionNumber);
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief		retrieveRevisionInfo
				This method retrieves the additionalData and version number for
				the next message but it does not deliver the message itself.
				The method is useful when you want to check the revision info
				for a message before actually reading the message.
				Warning: There is a performace penalty for users which uses both
				retrieveRevisionInfo and getMessageAndRevisionInfo or getMessage
				for each message. It gives better performace to use only
				getMessageAndRevisionInfo.
				additionalData and versionNumber is set to 0 if the message is
				'Empty' or 'Lost' or if the method returns false.

		@param		additionalData
				A number which indicates the type of the data.
				Value 0 indicates 'old format', that is, the
				message does not contain information about
				type of the data.
				Values in the range 1-254 indicates legal formats.
				Value 255 is reserved for future use.

		@param		versionNumber
				Indicates the revision step of the data.
				Value 0 indicates 'old format', that is, the
				message does not contain information about
				revision step of the data.
				Values in the range 1-126 indicates legal
				versions.
				Value 127 is reserved for future use.
				Value 128 is used for Expedited Data Messages.
		
		@param		state
				State of message. Can be one of
				'Filled',  indicating that the message now
				is valid.
				'Skipped', implying that a message was wasted
				by the CP side.
				'Lost',    telling that Message Store (i.e.
				the AP side) has lost a message.
				'Empty',   the message queue is empty


		@return		bool
				true - Successful
				false - Failure.
	*/	
	/*=================================================================== */
	bool retrieveRevisionInfo(int& additionalData,
				  int& versionNumber,
				  ACS_ACA_MessageStoreMessage::State& state);
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief		commit
				Committing of all read messages.  The transactionIdData
				is handled transparently by the Message Store (has no
				meaning to it) and can be read by a 
				'getLastTransactionId()' call.

		@param		transactionIdData

		@param		transactionIdDataLength

		@return		bool
				true - Success
				false - Failure
	*/
	/*=================================================================== */
	bool commit( const unsigned char * transactionIdData = 0,
		     const int transactionIdDataLength = 0);
	/*===================================================================

		@brief		commit
				This method commits the specified number of 
				read messages. The transactionIdData is handled
				transparently by the Message Store and can be read by a
				'getLastTransactionId()' call.

		@param		numOfMessages
				Number of read messages that will be committed.

		@param		transactionIdData

		@param		transactionIdDataLength

		@return		bool
				true - Success
				false - Failure.

	=====================================================================*/
	bool commit( unsigned long long int numOfMessages, 
		     const unsigned char * transactionIdData = 0,
		     const int transactionIdDataLength = 0);
	/*=====================================================================


		@brief		rollback
				This method moves back the state to the 
				last committed message.

		@return		true - Success
				false - Failure.
	=====================================================================*/
	bool rollback();
	/*=====================================================================


		@brief		getLastTransactionId
				Transaction id access methods. This method returns
				the data from last commit operation. The contents of the
				returned pointer reference is only vaild until
				the next call, and the maximum data size is
				'TransactionIdDataMaxLength'. Actual size is returned
				in the 'transactionIdDataLength' reference parameter.

		@param		transactionIdData

		@param		transactionIdDataLength

		@return 	bool
				true - Successful
				false - Failure.

	=====================================================================*/
	bool getLastTransactionId(unsigned char * & transactionIdData,
				  int & transactionIdDataLength);

private:
	/*=====================================================================*/
	/**
		@brief		ACS_ACA_MessageStoreAccessPointR3
				Copy Constructor for ACS_ACA_MessageStoreAccessPointR3.
	
		@param		anotherMSAP
	=====================================================================*/
	ACS_ACA_MessageStoreAccessPointR3(const ACS_ACA_MessageStoreAccessPointR3 & anotherMSAP);
	/*=====================================================================


		@brief		operator=
				Overloaded assignment for ACS_ACA_MessageStoreAccessPointR3.

	=====================================================================*/
	const ACS_ACA_MessageStoreAccessPointR3 & operator=(const ACS_ACA_MessageStoreAccessPointR3 & anotherMSAP);

	/*=====================================================================

		@brief	internals
	=====================================================================*/

	ACS_ACA_InternalMessageStoreAccessPoint* internals;
	//=====================================================================
};

typedef ACS_ACA_MessageStoreAccessPointR3 ACS_ACA_MessageStoreAccessPoint;
/*=====================================================================
                        TYPE DECLARATION SECTION
==================================================================== */

/*=====================================================================

	@brief	ACS_ACA_MessageStoreAccessPointR3

=====================================================================*/

/*=====================================================================//
*/


#endif
