/*=================================================================== */
/**
   @file   JTP_Session_R3A.h

   @brief  Header file for APJTP type module.

          This module contains all the declarations useful to
          specify the class JTP_Session_R3A.

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
#if !defined(_JTP_Session_R3A_H_)
#define _JTP_Session_R3A_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <JTP_Service_R3A.h>
#include <JTP_Session_R2A.h>

/*=================================================================== */
	/**
		@class			JTP_Session_R3A

		@brief			This is internal class for sessions.
	**/
/*=================================================================== */
class JTP_Session_R3A : public JTP_Session_R2A {


	friend class JTP_Service_R3A;


public:
	/*=====================================================================
						CLASS CONSTRUCTORS
	==================================================================== */

	/*=================================================================== */

		/**
			@brief			Client constructor

			@pre

			@post

			@param			jtype
							Either JTPJob or JTPConversation

			@param			Service
							Name of Application.

			@return			None

			@exception		None
		*/
	inline JTP_Session_R3A(JTP_Type jtype, char* Service) : JTP_Session_R2A(jtype, Service) {}

	/*=================================================================== */

		/**

			@brief			Client constructor

			@pre

			@post

			@param			jtype
							Either JTPJob or JTPConversation

			@param			Service
							Name of Application.

			@param			iMaxBuf
							Maximum Buffer

			@return			None

			@exception		None
		*/
	inline JTP_Session_R3A(JTP_Type jtype, char* Service, short iMaxBuf) : JTP_Session_R2A(jtype, Service, iMaxBuf) {}


public:
	/*===================================================================
	                       CLASS DESTRUCTOR
	=================================================================== */
	/*=================================================================== */
		/**

			@brief			Destructor

			@pre			None

			@post			None

			@return			None

			@exception		None
		*/
	/*=================================================================== */
	inline virtual ~JTP_Session_R3A() {}


public:
	using JTP_Session_R2A::jexinitreq;
	using JTP_Session_R2A::jexinitind;
	using JTP_Session_R2A::jinitreq;
	using JTP_Session_R2A::jinitind;

	/*=================================================================== */
		/**
			@brief			This is the initiator routine that is to be used
							when the client wants to start a JTP Conversation.

			@pre			None

			@post			None

			@param			Node
							ACS_DSD_Node.

			@param			U1
							User data 1, which is transferred to destinator.

			@param			U2
							User data 2, which is transferred to destinator.

			@return			bool
			true			The message was sent successfully.
			false			Sending of message failed, or faulty state.

			@exception		None
		*/
	/*=================================================================== */
	bool jexinitreq(ACS_DSD_Node* Node,					//IN
					unsigned short U1,					//IN
					unsigned short U2					//IN
					);

	/*=================================================================== */
		/**
			@brief			This is the destinator routine that is to be used
							when the destinator receives a request for a JTP Conversation.

			@pre			None

			@post			None

			@param			Node
							ACS_DSD_Node.

			@param			U1
							User data 1, which is received from initiator.

			@param			U2
							User data 2, which is received from initiator.

			@return			bool
			true			The message was received successfully.
			false			Receiving of message failed, or faulty state.

			@exception		None
		*/
	/*=================================================================== */
	bool jexinitind(ACS_DSD_Node& Node,	//OUT
					unsigned short& U1,					//OUT
					unsigned short& U2					//OUT
					);

	/*=================================================================== */
		/**
			@brief			This is the initiator routine that is to be used
							when the client wants to start a JTP job.

			@pre			None

			@post			None

			@param			Node
							JTP_NOde

			@param			U1
							User data 1, which is transferred to destinator.

			@param			U2
							User data 2, which is transferred to destinator.

			@param			BufferLength
							Length of sent data buffer.

			@param			Buf
							Sent data buffer.

			@return			bool
			true			The message was sent successfully.
			false			Sending of message failed, or faulty state.

			@exception		None
		*/
	/*=================================================================== */
	bool jinitreq(ACS_DSD_Node* Node,		//IN
				  unsigned short U1,					//IN
				  unsigned short U2,					//IN
				  unsigned short BufferLength,			//IN
				  char* Buf					//IN
				  );


	/*=================================================================== */
		/**
			@brief			This is the destinator routine that is to be used
							when the destinator receives a request for a JTP job.

			@pre			None

			@post			None

			@param			Node
							ACS_DSD_Node

			@param			U1
							User data 1, which is received from initiator.

			@param			U2
							User data 2, which is received from initiator.

			@param			BufferLength
							Length of incoming data buffer.

			@param			Buf
							Incoming data buffer.

			@return			bool
			true			The message was received successfully.
			false			Receiving of message failed, or faulty state.

			@exception		None
		*/
	/*=================================================================== */
	bool jinitind(ACS_DSD_Node& Node,		//OUT
				  unsigned short& U1,					//OUT
				  unsigned short& U2,					//OUT
				  unsigned short& BufferLength,			//OUT
				  char*& Buf					//OUT
				  );
};//end JTP_Session_R3A class

#endif //!defined(_JTP_Session_R3A_H_)
