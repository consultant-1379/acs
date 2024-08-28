/*=================================================================== */
/**
   @file   acs_aca_internal_message_store_access_point.h 

   @brief Header file for API module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       28/01/2013   XHARBAV   Initial Release
==================================================================== */
//****************************************************************************
// DESCRIPTION 
//   This class is the real implementation of a ACS_ACA_MessageStoreAccessPoint,
//   and has the same public interface.
//
//   It gives an application access to the received messages of a Message 
//   Store. In order to connect, the client must supply the name of the 
//   MessageStore and the AXE site name. If no other client is connected 
//   and the MessageStore is up and running, a connection is established 
//   and the client can start to fetch messages. Reading is always done 
//   in a non-blocking fashion, i.e. when asking for the next message, it 
//   will return instantly and deliver an empty message, if no message was 
//   available. If event-driven access is wanted, the client's  process 
//   can get a socket handle to supervise in order to be notified when 
//   there really is a message available.
//
//   When the client application has safely processed one or more messages,
//   it must commit them, implying that Message Store can delete them.
//   If no commitment is done, the Message Store will grow and grow, and 
//   finally be filled up.
//
//   If a connection towards a Message Store is broken and re-established
//   again, an explicit rollback will be made, i.e the Message Store will move
//   back to the first non-committed message.
//
//   All (dis)connect/access methods are boolean, and return true on success;
//   if they fail, false will be returned and an error code will be set.


// CHANGES
//   RELEASE REVISION HISTORY
//   REV NO  DATE         NAME      DESCRIPTION
//   A	     2000-05-10   uabcajn   product release
//   B       2007-01-15   qnicmut   Blade Cluster adaptations
//******************************************************************************
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_ACA_InternalMessageStoreAccessPoint_H
#define ACS_ACA_InternalMessageStoreAccessPoint_H
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>


/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief     ACS_ACA_InternalMessageStoreAccessPoint

 */
/*=================================================================== */
class ACS_ACA_InternalMessageStoreAccessPoint {
/*=====================================================================
                          PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                          CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief     Constructor for ACS_ACA_InternalMessageStoreAccessPoint class.

 */
/*=================================================================== */
	// Constructor
	ACS_ACA_InternalMessageStoreAccessPoint();
/*=====================================================================
                          CLASS DESTRUCTOR
==================================================================== */
	// Destructor.
	virtual ~ACS_ACA_InternalMessageStoreAccessPoint();
/*=================================================================== */
/**
   @brief     getState

   @return    State

 */
/*=================================================================== */
	// Misc. get information methods.
	ACS_ACA_MessageStoreAccessPoint::State getState() const;
/*=================================================================== */
/**
   @brief     getLastError

   @return    ErrorCode

 */
/*=================================================================== */
	ACS_ACA_MessageStoreAccessPoint::ErrorCode getLastError() const;
/*=================================================================== */
/**
   @brief     clearLastError

   @return    void

 */
/*=================================================================== */
	// Set the error code to 'Undefined'.
	void clearLastError();
/*=================================================================== */
/**
   @brief     connect

   @return    bool

 */
/*=================================================================== */

	// Setup a one-to-one association to a Message Store.
	// If re-establishment after a lost connection, the message
	// sequence will not be broken. 
	// Returns true if successful; false on failure and error code
	// will be set to one of the following:
	// 'NoSuchMsgStore', 'AlreadyConnected', 'AnotherConnected',
	// 'ConnectFailed'.
	virtual bool connect() /*throw(...)*/ = 0;
/*=================================================================== */
/**
   @brief     disconnect

   @return    bool

 */
/*=================================================================== */

	// Remove the association to Message Store. Returns true if
	// it succeeded to disconnect or already was disconnected.
	// Possible error code if unsuccessful: 'DisconnectFailed'.
	//
	virtual bool disconnect() = 0;
/*=================================================================== */
/**
   @brief     getHandle

   @return    ACE_HANDLE

 */
/*=================================================================== */
 
	//--- The following methods shall only be used in Connected state: ---
	//--------------------------------------------------------------------
   
	//    This method gives access to a file descriptor that can be
	//    used for notification when a new message is available.
	//
	//virtual ACS_handle getHandle() const = 0;
	virtual ACE_HANDLE getHandle() const = 0;
/*=================================================================== */
/**
   @brief     getMessage

   @param     message

   @return    bool

 */
/*=================================================================== */

	// Message access method. Will return the first non-read message if
	// available. If the Message Store is empty, an empty message will be
	// returned. Will return false if some error occurred, otherwise true.
	// Possible error codes: 'NotConnected', 'ConnectionLost'.
	//
	virtual bool getMessage(ACS_ACA_MessageStoreMessage & message ) = 0;
/*=================================================================== */
/**
   @brief     getMessageAndRevisionInfo

   @param     message

   @param     primitiveFormat

   @param     versionNumber

   @return    bool

 */
/*=================================================================== */
	virtual bool getMessageAndRevisionInfo(ACS_ACA_MessageStoreMessage & message, int & primitiveFormat, int & versionNumber) = 0;
/*=================================================================== */
/**
   @brief     retrieveRevisionInfo

   @param     primitiveFormat

   @param     versionNumber

   @param     state

   @return    bool

 */
/*=================================================================== */

	virtual bool retrieveRevisionInfo(int & primitiveFormat, int & versionNumber, ACS_ACA_MessageStoreMessage::State & state) = 0;
/*=================================================================== */
/**
   @brief     commit
               // Commitment of all read messages. The transactionIdData is handled
               // transparently by the Message Store (has no meaning to it)
               // and can be read by a 'getLastTransactionId()' call.
               // Returns true if successful, otherwise false and sets the error code
               // to: 'commitFailed'.

   @param     transactionIdData

   @param     transactionIdDataLength

   @return    State

 */
/*=================================================================== */
	virtual bool commit(const unsigned char * transactionIdData = 0, const int transactionIdDataLength = 0) = 0;
/*=================================================================== */
/**
   @brief     commit
                // Commitment of the specified number of read messages. The transactionIdData
                // is handled transparently by the Message Store (has no meaning to it)
                // and can be read by a 'getLastTransactionId()' call.
                // Returns true if successful, otherwise false and sets the error code
                // to: 'commitFailed'.
 

   @param     numOfMessages

   @param     transactionIdData

   @param     transactionIdDataLength

   @return    bool

 */
/*=================================================================== */
	virtual bool commit(ULONGLONG numOfMessages, const unsigned char * transactionIdData = 0, const int transactionIdDataLength = 0) = 0;
/*=================================================================== */
/**
   @brief     rollback  
                 // Move back to last committed message. Returns true if successful,
                 // otherwise false and sets the error code to one of the following:
                 // 'NotConnected', 'ConnectionLost'. 

   @return    bool

 */
/*=================================================================== */
	virtual bool rollback() = 0;
/*=================================================================== */
/**
   @brief     getLastTransactionId
                 Transaction id access methods. Returns the data from last commit
                 operation. The contents of the returned pointer reference
                 is only vaild until the next call, and the maximum data size is
                 'TransactionIdDataMaxLength'. Actual size is returned in the
                  'transactionIdDataLength' reference parameter.
                  Returns true if successful, otherwise false and sets the
                  error code to one of the following:
                  'NotConnected', 'ConnectionLost'.

   @param     transactionIdData

   @param     transactionIdDataLength

   @return    bool

 */
/*=================================================================== */
	virtual bool getLastTransactionId(unsigned char * & transactionIdData, int & transactionIdDataLength) = 0;
/*=====================================================================
                          PROTECTED DECLARATION SECTION
==================================================================== */
protected:
/*=================================================================== */
/**
   @brief     setMessageData

   @param     message

   @param     msgNumber
   
   @param     msgData

   @param     msgLength

   @param     state

   @return    void

 */
/*=================================================================== */

	void setMessageData(ACS_ACA_MessageStoreMessage & message, unsigned long long msgNumber,/* unsigned long msgNumberUpperHalf,unsigned long msgNumberLowerHalf,*/ unsigned char * msgData, int msgLength, ACS_ACA_MessageStoreMessage::State state);
                 
	ACS_ACA_MessageStoreAccessPoint::State stateM;
	ACS_ACA_MessageStoreAccessPoint::ErrorCode errorCodeM;
	bool anyMessageRead;
};

#endif
