/*=================================================================== */
/**
   @file   ACS_JTP_Conversation_R2A.h

   @brief  Header file for APJTP type module.

          This module contains all the declarations useful to
          specify the class ACS_JTP_Conversation_R2A.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/26/2010     XTANAGG        APG43 on Linux.
==================================================================== */
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#if !defined(_ACS_JTP_Conversation_R2A_H_)
#define _ACS_JTP_Conversation_R2A_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "ACS_JTP.h"

/*=====================================================================
                        FORWARD DECLARATION SECTION
==================================================================== */
class JTP_Service_R2A;
class JTP_Session_R2A;

/*=================================================================== */
/**
    @class		ACS_JTP_Conversation_R2A

    @brief		This is the interface class for conversation mode.
**/
 /*=================================================================== */
class ACS_JTP_Conversation_R2A {

	friend class JTP_Service_R2A;
	friend class JTP_Session_R2A;

	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
	                        ENUMERATED DECLARATION SECTION
	==================================================================== */
	/**
		@brief  This enum tells the side of the application.

				It could be either initiator or destinator.
	*/
	enum JTP_Side {
		JTPClient = 1,
		JTPServer
  };
	
	/**
		@brief  This enum tells the state of the application.

	*/
	enum JTP_State {
		StateCreated = 0,
		StateConnected = 1,
		StateAccepted = 2,
		StateJobRunning = 3,
		StateWaitForData = 4
	};

public:
	/*=====================================================================
	                        CLASS CONSTRUCTORS
	==================================================================== */

	/*=================================================================== */

		/**

			@brief			Client constructor

			@pre

			@post

			@param			Service
							JTP application name.

			@param			MaxBufLen
							Max buffer length in conversation.

			@return			none

			@exception		none
		*/
	ACS_JTP_Conversation_R2A(char* Service, unsigned short MaxBufLen);

		/**

			@brief			Client constructor

			@pre

			@post

			@return			none

			@exception		none
		*/

	ACS_JTP_Conversation_R2A();



public:
	/*===================================================================
	                        CLASS DESTRUCTOR
	=================================================================== */
	/*=================================================================== */
		/**

			@brief			Destructor

			@pre			none

			@post			none

			@return			none

			@exception		none
		*/
	/*=================================================================== */
	virtual ~ACS_JTP_Conversation_R2A();

	/*===================================================================
	                           PUBLIC METHOD
	=================================================================== */
public:
	/*=================================================================== */
		/**
			@brief			This is the initiator routine that is to be used
							when the client wants to start a JTP Conversation.

			@pre			none

			@post			none

			@param			U1
							User data 1, which is transferred to destinator.

			@param			U2
							User data 2, which is transferred to destinator.

			@return			bool
			true			The message was sent successfully.
			false			Sending of message failed, or faulty state.

			@exception		none
		*/
	/*=================================================================== */
	bool jexinitreq(unsigned short U1,	//IN
					unsigned short U2	//IN
					);


	/*=================================================================== */
		/**
			@brief			This is the destinator routine that is to be used
							when the destinator receives a request for a JTP Conversation.

			@pre			none

			@post			none

			@param			U1
							User data 1, which is received from initiator.

			@param			U2
							User data 2, which is received from initiator.

			@return			bool
			true			The message was received successfully.
			false			Receiving of message failed, or faulty state.

			@exception		none
		*/
	/*=================================================================== */
	bool jexinitind(unsigned short& U1,		//OUT
					unsigned short& U2		//OUT
					);


	/*=================================================================== */
		/**
			@brief			This is the destinator routine that is to be used
							when the destinator wants to accept a JTP Conversation.

			@pre			none

			@post			none

			@param			U1
							User data 1, which is transferred to initiator.

			@param			U2
							User data 2, which is transferred to initiator.

			@param			R
							Result code, which is transferred to initiator.
							R = 0 means conversation accepted.
							R > 0 means failure.

			@return			bool
			true			The message was received successfully.
			false			Receiving of message failed, or faulty state.

			@exception		none
		*/
	/*=================================================================== */
	bool jexinitrsp(unsigned short U1,		//IN
					unsigned short U2,		//IN
					unsigned short R		//IN
					);


	/*=================================================================== */
		/**
			@brief			This is the initiator routine that is to be used
							when the initiator receives confirmation of a JTP Conversation.

			@pre			none

			@post			none

			@param			U1
							User data 1, which is received from destinator.

			@param			U2
							User data 2, which is received from destinator.

			@param			R
							Result code, which is received from destinator.
							R = 0 means conversation accepted.
							R > 0 error.

			@return			bool
			true			The message was received successfully.
			false			Receiving of message failed, or faulty state.

			@exception		none
		*/
	/*=================================================================== */
	bool jexinitconf(unsigned short& U1,	//OUT
						unsigned short& U2,	//OUT
						unsigned short& R	//OUT
					 );

	/*=================================================================== */
		/**
			@brief			This is data sending routine that is to be used
							when either a client or destinator transmits data.

			@pre			none

			@post			none

			@param			U1
							User data 1, which is transferred to remote side.

			@param			U2
							User data 2, which is transferred to remote side.

			@param			BufLen
							Length of sent data buffer.

			@param			Buf
							Sent data buffer.

			@return			bool
			true			The message was received successfully.
			false			Receiving of message failed, or faulty state.

			@exception		none
		*/
	/*=================================================================== */
	bool jexdatareq(unsigned short U1,		//IN
					unsigned short U2,		//IN
					unsigned short BufLen,	//IN
					char* Buf		//IN
					);



	/*=================================================================== */
		/**
			@brief			This is data reception routine that is to be used
							when either a client or destinator receives data

			@pre			none

			@post			none

			@param			U1
							User data 1, which is received from remote side.

			@param			U2
							User data 2, which is received from remote side.

			@param			BufLen
							Length of incoming data buffer.

			@param			Buf
							Incoming data buffer.

			@return			bool
			true			The message was received successfully.
			false			Receiving of message failed, or faulty state.

			@exception		none
		*/
	/*=================================================================== */
	bool jexdataind(unsigned short& U1,		//OUT
					unsigned short& U2,		//OUT
					unsigned short& BufLen,	//OUT
					char*& Buf		//OUT
					);

	/*=================================================================== */
		/**
			@brief			This is the routine that is to be used when an application
							wants to disconnect (close) a JTP Conversation.

			@pre			none

			@post			none

			@param			U1
							User data 1, which is transferred to remote side.

			@param			U2
							User data 2, which is transferred to remote side.

			@param			Reason
							Reason code, to send to remote side.
							R > 0 means failure.
							Reason = 0 means by convention a normal disconnect.


			@return			bool
			true			The conversation was successfully disconnected.
			false			Faulty state.

			@exception		none
		*/
	/*=================================================================== */
	bool jexdiscreq(unsigned short U1,		//IN
					unsigned short U2,		//IN
					unsigned short Reason	//IN
					);

	/*=================================================================== */
		/**
			@brief			This is disconnect reception routine that is to be used
							when either a client or destinator receives disconnection.

			@pre			none

			@post			none


			@param			U1
							User data 1, which is received from remote end.

			@param			U2
							User data 2, which is received from remote end.

			@param			Reason
							Result code, which is received from remote end.
							R = 0 means conversation accepted.
							R > 0 means failure.


			@return			bool
			true			The message was received successfully.
			false			Receiving of message failed, or faulty state.

			@exception		none
		*/
	/*=================================================================== */
	bool jexdiscind(unsigned short& U1,		//OUT
					unsigned short& U2,		//OUT
					unsigned short& Reason	//OUT
					);

	/*=================================================================== */
		/**
			@brief			Get working mode of current Conversation session.

			@pre			none

			@post			none

			@return			JTP_SIDE
			JTP_Side	JTPClient, JTPServer

			@exception		none
		*/
	/*=================================================================== */
	JTP_Side Side();

	/*=================================================================== */
		/**
			@brief			Get state of current Conversation session

			@pre			none

			@post			none

			@return
			JTP_State	StateCreated, StateConnected,
			StateAccepted, StateJobRunning

			@exception		none
		*/
	/*=================================================================== */
	JTP_State State();

	/*=================================================================== */
		/**
			@brief			Will return a JTP_HANDLE of the Conversation session

			@pre      	  	none

			@post			none

			@return
			JTP_HANDLE		JTP_HANDLE to the Conversation session.

			@exception		none
		 */
	/*=================================================================== */
	JTP_HANDLE getHandle();

	/*=================================================================== */
		/**
			@brief			Override default timeout time at transfer of jexinitconf
							or jinitconf Default value is five (5) seconds. The value
							is also the timeout supervision of data transmissions by
							jexdatareq() when you are an initiator. If the destinator
							reply (jexdataind()) exceeds this timeout, the connection
							will be closed.If no time supervision of jexdatareq() is
							wanted at all, the value can be set to zero.

			@pre			none

			@post			none

			@param			Sec
							Number of seconds, timeout (=0) => No time supervision
							of jexdatareq() but 5 seconds for jexinitreq() & jinitreq().

			@return			void

			@exception		none
		*/
	/*=================================================================== */
	void setTimeOut(int Sec	//IN
					);

	/*=================================================================== */
		/**
			@brief			Override default number of connect tries per
							whole set of addresses to a remote application
							Default value is (1) try

			@pre			none

			@post			none

			@param			Times
							Number of tries.

			@return			void

			@exception		none
		*/
	/*=================================================================== */
	void setNoOfTries(int Times		//IN
						);

	/*=================================================================== */
		/**
			@brief			Override default delay time between connect
							tries with the whole set of addresses to application
							Default value is (0) seconds

			@pre			none

			@post			none

			@param			Sec
							Number of seconds, delay.

			@return			void

			@exception		none
		*/
	/*=================================================================== */
	void setDelayTime(int Sec		//IN
						);
	/*===================================================================
	                        PRIVATE DECLARATION SECTION
	=================================================================== */
private:
	/*===================================================================
	                        PRIVATE ATTRIBUTE
	=================================================================== */
	JTP_Session_R2A* Internal;
};

#endif //(_ACS_JTP_Conversation_R2A_H_)
