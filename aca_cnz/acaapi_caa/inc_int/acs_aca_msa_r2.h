/*=================================================================== */
/**
   @file   acs_aca_msa_r2.h 

   @brief Header file for aca module.

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
   N/A       29/01/2013   XHARBAV   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACAMSA_R2_H
#define ACAMSA_R2_H

/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_aca_msa_msg_store.h"
#include "acs_aca_internal_message_store_access_point.h"
#include "acs_aca_message_store_message.h"

#include <ace/ACE.h>

/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
class ACAMSA_R2 : virtual public ACAMSA_MsgStore,
                  virtual public ACS_ACA_InternalMessageStoreAccessPoint {
/*=====================================================================
                          PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                          CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**
   @brief    Constructor for ACAMSA_R2 class.
             // Constructor. Must define the actual Message Store to access here;
             // This means that one instance can only be used to access a particular^M

   @param    msName

   @param    cpSite 
 */
/*=================================================================== */
	ACAMSA_R2(const std::string & msName, const std::string & cpSite);
/*=================================================================== */
/**
   @brief    Constructor for ACAMSA_R2 class.
             // Constructor. Must define the actual Message Store to access here;
             // This means that one instance can only be used to access a particular^M

   @param    msName

   @param    cpSystemId
 */
/*=================================================================== */
	ACAMSA_R2(const std::string & msName, unsigned cpSystemId);

/*=====================================================================
                          CLASS DESTRUCTOR
==================================================================== */
	virtual ~ACAMSA_R2();

/*=================================================================== */
/**
   @brief    connect
             // Set up a one-to-one association to a Message Store.
             // Returns true if successful; false on failure and error code will be set to one of the following:
             // 'NoSuchMsgStore'
             // 'AlreadyConnected' ^M
             // 'AnotherConnected'^M
             // 'ConnectFailed'  
             

   @return   bool
 */
/*=================================================================== */
	virtual bool connect();
/*=================================================================== */
/**
   @brief    disconnect
             // Take down association to Message Store. Returns true if^M
             // it succeeded to disconnect or already was disconnected.^M
             // Possible error code if unsuccessful: 'DisconnectFailed'.^ 

   @return   bool
 */
/*=================================================================== */
	virtual bool disconnect();
/*=================================================================== */
/**
   @brief    getHandle

   @return   ACE_HANDLE
 */
/*=================================================================== */
 
	//--- The following methods shall only be used in Connected state: ---
	//--------------------------------------------------------------------
	virtual ACE_HANDLE getHandle() const;
/*=================================================================== */
/**
   @brief    getMessage
             // Message access method. Will return the first non-read message if^M
             // available. If the Message Store is empty, an empty message will be^M
             // returned. Will return false if some error occurred, otherwise true.^M
             // Possible error codes: 'NotConnected', 'ConnectionLost' 

   @param    message

   @return   bool
 */
/*=================================================================== */
	virtual bool getMessage(ACS_ACA_MessageStoreMessage & message);
/*=================================================================== */
/**
   @brief    getMessageAndRevisionInfo

   @param    message

   @param    primitiveFormat

   @param    versionNumber

   @return   bool
 */
/*=================================================================== */
	virtual bool getMessageAndRevisionInfo(ACS_ACA_MessageStoreMessage & message, int & primitiveFormat, int & versionNumber);
/*=================================================================== */
/**
   @brief    retrieveRevisionInfo
             
   @param    primitiveFormat

   @param    state  

   @return   bool
 */
/*=================================================================== */
	virtual bool retrieveRevisionInfo(int & primitiveFormat, int & versionNumber, ACS_ACA_MessageStoreMessage::State & state);
/*=================================================================== */
/** 
   @brief    commit
             // Commitment of the specified number of read messages. The transactionIdData is handled^M
             // transparently by the Message Store (has no meaning to it) ^M
             // and can be read by a 'getLastTransactionId()' call. ^M
             // Returns true if successful, otherwise false and sets the error code to: 'commitFailed'.


   @param    transactionIdData

   @param    transactionIdDataLength

   @return   bool    
 */
/*=================================================================== */
	virtual bool commit(const unsigned char * transactionIdData = 0, const int transactionIdDataLength = 0);
/*=================================================================== */
/**
   @brief    commit
             // Commitment of the specified number of read messages. The transactionIdData is handled^M
             // transparently by the Message Store (has no meaning to it) ^M
             // and can be read by a 'getLastTransactionId()' call. ^M
             // Returns true if successful, otherwise false and sets the error code to: 'commitFailed'.  

   @param    numOfMessages

   @param    transactionIdData

   @param    transactionIdDataLength

   @return   bool
 */
/*=================================================================== */
	virtual bool commit(ULONGLONG numOfMessages, const unsigned char * transactionIdData = 0, const int transactionIdDataLength = 0);
/*=================================================================== */
/**
   @brief    rollback
             // Move back to last committed message. Returns true if successful, ^M
             // otherwise false and sets the error code to one of the following: ^M
             // 'NotConnected', 'ConnectionLost'.  

   @return   bool
 */
/*=================================================================== */
	virtual bool rollback();
/*=================================================================== */
/**
   @brief    getLastTransactionId
             // Transaction id access methods. Returns the data from last commit ^M
             // operation. The contents of the returned pointer reference^M
             // is only vaild until the next call, and the maximum data size is^M
             // 'TransactionIdDataMaxLength'. Actual size is returned in the^M
             // 'transactionIdDataLength' reference parameter.^M
             // Returns true if successful, otherwise false and sets the ^M
             // error code to one of the following: ^M
             // 'NotConnected', 'ConnectionLost'.

   @param    transactionIdData

   @param    transactionIdDataLength

   @return   bool
 */
/*=================================================================== */
	virtual bool getLastTransactionId(unsigned char * & transactionIdData, int & transactionIdDataLength);

/*=====================================================================
                          PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=====================================================================
                          CLASS CONSTRUCTOR
==================================================================== */
	// Copy constructor and assignment operator not allowed to use.
	ACAMSA_R2(const ACAMSA_R2 & anotherMSAP);
	const ACAMSA_R2 & operator=(const ACAMSA_R2 & anotherMSAP);

/*=====================================================================
                          DATA MEMBER DECLARATION SECTION
==================================================================== */
	unsigned char * _transactionIdDataBuffer;
	int _transactionIdDataLength;
};

#endif
