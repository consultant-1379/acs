/*=================================================================== */
/**
   @file   acs_aca_msa_msg_store.h

   @brief Header file for MSA API

          This module contains all the declarations useful to
          specify the class ACAMSA_MsgStore.

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
#ifndef ACAMSA_MsgStore_H
#define ACAMSA_MsgStore_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include "acs_aca_ms_msg_store.h"
#include "acs_aca_message_store_access_point.h"
#include "acs_aca_ms_commit_file.h"

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ACAMSA_MsgStore

                 This class implements the reading part of Message Store.
		 It inherits from ACAMS_MsgStore.
*/
/*=================================================================== */
class ACAMSA_MsgStore : public ACAMS_MsgStore
{

/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */

public:

/*=====================================================================
                        CLASS CONSTRUCTORS
==================================================================== */

	/*=================================================================== */
	/**
		@brief 		ACAMSA_MsgStore
	*/
	/*=================================================================== */
	ACAMSA_MsgStore();
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		ACAMSA_MsgStore

		@param		msName
	
		@param		cpSite
	*/	
	/*=================================================================== */
	ACAMSA_MsgStore(const std::string & msName, const std::string & cpSite);
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		ACAMSA_MsgStore

		@param		msName
	
		@param		cpSystemId
	*/	
	/*=================================================================== */
	ACAMSA_MsgStore(const std::string & msName, unsigned cpSystemId);
	/*=================================================================== */

	/*===================================================================
                        CLASS DESTRUCTOR
	=================================================================== */
	/*=================================================================== */
	/**
		@brief		~ACAMSA_MsgStore
	*/
	/*=================================================================== */
	virtual ~ACAMSA_MsgStore();
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		connect

		@return		ACS_ACA_MessageStoreAccessPoint::ErrorCode
	*/	
	/*=================================================================== */
	/*=================================================================== */
	ACS_ACA_MessageStoreAccessPoint::ErrorCode connect() ;
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		disconnect

		@return		ACS_ACA_MessageStoreAccessPoint::ErrorCode
	*/	
	/*=================================================================== */
	ACS_ACA_MessageStoreAccessPoint::ErrorCode disconnect(bool sendApplDiscPrim = true);
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		commit

		@param		transactionData

		@param		transactionDataLength
	
		@return		ACS_ACA_MessageStoreAccessPoint::ErrorCode
	*/	
	/*=================================================================== */
	ACS_ACA_MessageStoreAccessPoint::ErrorCode commit(const unsigned char * transactionData, const int transactionDataLength);
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		commit

		@param		numOfMessages

		@param		transactionData
	
		@param		transactionDataLength

		@return		ACS_ACA_MessageStoreAccessPoint::ErrorCode
	*/	
	/*=================================================================== */
	ACS_ACA_MessageStoreAccessPoint::ErrorCode commit(unsigned long long numOfMessages, const unsigned char * transactionData,	const int transactionDataLength);
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		getHandle
				This method gets the handle to MSIP-Port.

		@return		ACE_HANDLE
	*/	
	/*=================================================================== */
	ACE_HANDLE getHandle() const;
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		getMsg
				This method gets the message from Message Store.

		@param		msgNumber

		@param		msgData
	
		@param		dataLength
		
		@param		state

		@return		ACS_ACA_MessageStoreAccessPoint::ErrorCode
	*/	
	/*=================================================================== */
	ACS_ACA_MessageStoreAccessPoint::ErrorCode getMsg(unsigned long long & msgNumber,
								unsigned char * & msgData,
								int & dataLength,
								ACS_ACA_MessageStoreMessage::State & state);
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		getMsgAndRevInfo

		@param		msgNumber

		@param		msgData
	
		@param		dataLength
		
		@param		state

		@param		primitiveFormat

		@param		versionNumber

		@param		keep

		@return		ACS_ACA_MessageStoreAccessPoint::ErrorCode
	*/	
	/*=================================================================== */
	ACS_ACA_MessageStoreAccessPoint::ErrorCode getMsgAndRevInfo(unsigned long long & msgNumber,
									unsigned char * & msgData,
									int & dataLength,
									ACS_ACA_MessageStoreMessage::State & state,
									int & primitiveFormat,
									int & versionNumber,
									int keep);  
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		getMsgNumber_LastCommitted
				This method gets the message number
				of last committed message.

		@return		unsigned long long
	*/	
	/*=================================================================== */
	unsigned long long getMsgNumber_LastCommitted() ;
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		getMsgNumber_LastRead
				This method gets the message number of last read message.

		@return		unsigned long long
	*/	
	/*=================================================================== */
	unsigned long long getMsgNumber_LastRead() const;
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		getTransactionData

		@param		transactionDataLength

		@return		unsigned char* 
	*/	
	/*=================================================================== */
	const unsigned char * getTransactionData(int & transactionDataLength) const;
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		rollback
				This method returns to the state after last
				commit.

		@return		ACS_ACA_MessageStoreAccessPoint::ErrorCode
	*/	
	/*=================================================================== */
	ACS_ACA_MessageStoreAccessPoint::ErrorCode rollback();
	/*=================================================================== */

	/*===================================================================
                        PRIVATE DECLARATION SECTION
	=================================================================== */

private:
	/*=================================================================== */
	/**
		@brief 		ACAMSA_MsgStore
				This is the  copy constructor for thr class
				ACAMSA_MsgStore.

		@param		anotherMS
	*/	
	/*=================================================================== */
	ACAMSA_MsgStore(const ACAMSA_MsgStore & anotherMS);
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		operator=
				Overloaded assignment operator. It is not allowed
				to use it.
	*/	
	/*=================================================================== */
	const ACAMSA_MsgStore & operator=(const ACAMSA_MsgStore & anotherMS);
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		checkMSIP_Input
				This method flushes any input on the MSIP-connection
				and tries to re-establish lost connection.

		@return		int
	*/	
	/*=================================================================== */
	int checkMSIP_Input();
	/*=================================================================== */
	
	/*=================================================================== */
	/**
		@brief 		locateNextMsgToRead

		@param		currentFile

		@param		currentFileName
	
		@param		nextMsgNumber

		@return		bool
	*/	
	/*=================================================================== */
	bool locateNextMsgToRead(ACE_HANDLE  & currentFile,
				std::string & currentFileName,
				unsigned long long & nextMsgNumber);
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		requestMsg

		@param		reqNum

		@return		ACS_ACA_MessageStoreAccessPoint::ErrorCode
	*/	
	/*=================================================================== */
	ACS_ACA_MessageStoreAccessPoint::ErrorCode requestMsg(const unsigned long long & reqNum);
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		fxExchnageCPSystemId

		@param		errorCode

		@return		int
	*/	
	/*=================================================================== */
	int fxExchangeCPSystemId(ACS_ACA_MessageStoreAccessPoint::ErrorCode & errorCode);
	/*=================================================================== */
	
	/*=================================================================== */
	/**
		@brief 		ConnectionState
	*/	
	/*=================================================================== */
	enum ConnectionState
	{
		Disconnect,
		Connect,
		Lost
	};
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		myMSIP_socket
	*/	
	/*=================================================================== */
	ACE_HANDLE  myMSIP_socket;  
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		myCurrentFile
				This is the handle to currently open data file.
	*/	
	/*=================================================================== */
	ACE_HANDLE  myCurrentFile; 
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		myLastReadMsgNumber
	*/	
	/*=================================================================== */
	unsigned long long myLastReadMsgNumber;
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		myCommitFile
	*/	
	/*=================================================================== */
	ACAMS_CommitFile * myCommitFile;
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		myConnectionState
	*/	
	/*=================================================================== */
	ConnectionState myConnectionState;
	/*=================================================================== */
	/**
		@brief 		myNextMsgRead
	*/	
	/*=================================================================== */
	bool myNextMsgRead;   
	/*=================================================================== */
       
        /*=================================================================== */
        /**
                @brief         checkMSIP_input_timeout
        */
        /*=================================================================== */
        bool checkMSIP_input_timeout;
        /*=================================================================== */
	/**
		@brief 		myMsgSize
				Size indicator for msg currently in IO_Buffer
	*/	
	/*=================================================================== */
	unsigned long myMsgSize; 
	/*=================================================================== */
	/**
		@brief 		myIO_Buffer
	*/	
	/*=================================================================== */
	unsigned char * myIO_Buffer;
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		myCurrentFileName
	*/	
	/*=================================================================== */
	std::string myCurrentFileName;
	/*=================================================================== */
	/**
		@brief 		myLastFinishedFile
	*/	
	/*=================================================================== */
	std::string myLastFinishedFile;
	/*=================================================================== */
	/*=================================================================== */
	/**
		@brief 		MSIport
	*/	
	/*=================================================================== */
	int MSIPport;
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		_isMultipleCPSystem
	*/	
	/*=================================================================== */
	bool _isMultipleCPSystem;
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief 		_cpSystemId
	*/	
	/*=================================================================== */
	unsigned _cpSystemId;

	int bReadbfCommit;
	unsigned long bytesDone;
	unsigned long msgSize;
	bool newFile;
	/*=================================================================== */
};

#endif
