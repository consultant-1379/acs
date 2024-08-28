/*=================================================================== */
/**
   @file   ACS_JTP_Job_R2A.h

   @brief  Header file for APJTP module.

          This module contains all the declarations useful to
          specify the class ACS_JTP_Job_R2A.

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
#if !defined(_ACS_JTP_Job_R2A_H_)
#define _ACS_JTP_Job_R2A_H_

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
		@class		ACS_JTP_Job_R2A

		@brief		This is the interface class for job mode.
	**/
/*=================================================================== */
class ACS_JTP_Job_R2A {

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
		StateJobRunning = 3
	};
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

			@return			None

			@exception		None
		*/

	ACS_JTP_Job_R2A(char* Service,		//IN
					unsigned short MaxBufLen	//IN
					);

		/**

			@brief			Client constructor

			@pre

			@post

			@return			None

			@exception		None
		*/
	ACS_JTP_Job_R2A();
	/*===================================================================
							CLASS DESTRUCTOR
	=================================================================== */
	/*=================================================================== */
		/**

			@brief			Destructor Job object

			@pre			None

			@post			None

			@return			None

			@exception		None
		*/
	/*=================================================================== */
	virtual ~ACS_JTP_Job_R2A();

	/*=================================================================== */
		/**
			@brief			This is the initiator routine that is to be used
							when the client wants to start a JTP Job.

			@pre			None

			@post			None

			@param			U1
							User data 1, which is transferred to destinator.

			@param			U2
							User data 2, which is transferred to destinator.

			@param			BufLen
							Length of sent data buffer.

			@param			Buf
							Sent data buffer.

			@return			bool
			true			The message was sent successfully.
			false			Sending of message failed, or faulty state.

			@exception		None
		*/
	/*=================================================================== */
	bool jinitreq(unsigned short U1,		//IN
				  unsigned short U2,		//IN
				  unsigned short BufLen,	//IN
				  char* Buf		//IN
				  );

	/*=================================================================== */
		/**
			@brief			This is the destinator routine that is to be used
							when the destinator receives a request for a JTP job.

			@pre			None

			@post			None

			@param			U1
							User data 1, which is received from initiator.

			@param			U2
							User data 2, which is received from initiator.

			@param			BufLen
							Length of incoming data buffer.

			@param			Buf
							Incoming data buffer.

			@return			bool
			true			The message was received successfully.
			false			Receiving of message failed, or faulty state.

			@exception		None
		*/
	/*=================================================================== */
	bool jinitind(unsigned short& U1,		//OUT
				  unsigned short& U2,		//OUT
				  unsigned short& BufLen,	//OUT
				  char*& Buf		//OUT
				  );

	/*=================================================================== */
		/**
			@brief			This is the destinator routine that is to be used
							when the destinator wants to accept a JTP Job.

			@pre			None

			@post			None

			@param			U1
							User data 1, which is transferred to initiator.

			@param			U2
							User data 2, which is transferred to initiator.

			@param			R
							Result code, which is transferred to initiator.
							R == 0, means success.
							R > 0, means failure.

			@return			bool
			true			The message was received successfully.
			false			Receiving of message failed, or faulty state.

			@exception		None
		*/
	/*=================================================================== */
	bool jinitrsp(unsigned short U1,		//IN
				  unsigned short U2,		//IN
				  unsigned short R			//IN
				  );

	/*=================================================================== */
		/**
			@brief			This is the initiator routine that is to be used
							when the initiator receives confirmation of a JTP Conversation.

			@pre			None

			@post			None

			@param			U1
							User data 1, which is received from destinator.

			@param			U2
							User data 2, which is received from destinator.

			@param			R
							Result code, which is received from destinator.
							R = 0 means job accepted.
							R > 0 means reject of job.

			@return			bool
			true			The message was received successfully.
			false			Receiving of message failed, or faulty state.

			@exception		None
		*/
	/*=================================================================== */
	bool jinitconf(unsigned short& U1,		//OUT
				   unsigned short& U2,		//OUT
				   unsigned short& R		//OUT
				   );

	/*=================================================================== */
		/**
			@brief			This is job result sending routine that is to be used
							when either a destinator or initiator transmits data.

			@pre			None

			@post			None

			@param			U1
							User data 1, which is transferred to remote side.

			@param			U2
							User data 2, which is transferred to remote side.

			@param			R
							Result Code.
							R = 0 means job successfully completed.
							R > 0 means failed job.

			@param			BufLen
							Length of sent data buffer.

			@param			Buf
							Sent data buffer.

			@return			bool
			true			The message was send successfully.
			false			Sending of message failed, or faulty state.

			@exception		None
		*/
	/*=================================================================== */
	bool jresultreq(unsigned short U1,		//IN
					unsigned short U2,		//IN
					unsigned short R,		//IN
					unsigned short BufLen,	//IN
					char* Buf		//IN
					);

	/*=================================================================== */
		/**
			@brief			This is job result reception routine that is to be used
							when either a client or destinator receives data.

			@pre			None

			@post			None

			@param			U1
							User data 1, which is received from remote side.

			@param			U2
							User data 2, which is received from remote side.

			@param			R
							Result code, which is received from remote side.
							R = 0 means job successfully completed.
							R > 0 means job failed.

			@param			BufLen
							Length of incoming data buffer.

			@param			Buf
							Incoming data buffer.

			@return			bool
			true			The message was received successfully.
			false			Receiving of message failed, or faulty state.

			@exception		None
		*/
	/*=================================================================== */
	bool jresultind(unsigned short& U1,		//OUT
					unsigned short& U2,		//OUT
					unsigned short& R,		//OUT
					unsigned short& BufLen,	//OUT
					char*& Buf		//OUT
					);

	/*=================================================================== */
		/**
			@brief			This is the routine that is to be used when an application
							wants to disconnect (close) a JTP Job. No data is transferred.

			@pre			None

			@post			None

			@return			bool
			true			The job was terminated.
			false			Faulty state.

			@exception		None
		*/
	/*=================================================================== */
	bool jtermreq();

	/*=================================================================== */
		/**
			@brief			This is jfault reception routine. Fault data is fetched.

			@pre			None

			@post			None

			@param			U1
							User data 1.

			@param			U2
							User data 2.

			@param			R
							Reason code, which is received from remote side.

			@return			bool
			true			The message was received successfully.
			false			Receiving of message failed, or faulty state.

			@exception		None
		*/
	/*=================================================================== */
	bool jfault(unsigned short& U1,		//OUT
				unsigned short& U2,		//OUT
				unsigned short& R		//OUT
				);

	/*=================================================================== */
		/**
			@brief			Get working mode of current job session.

			@pre			None

			@post			None

			@return			JTP_SIDE
			 JTP_Side  JTPClient, JTPServer

			@exception		None
		*/
	/*=================================================================== */
	 JTP_Side Side();

	/*=================================================================== */
		/**
			@brief			Get state of current job session

			@pre			None

			@post			None

			@return			 JTP_State
							StateCreated, StateConnected,
							StateAccepted, StateJobRunning

			@exception		None
		*/
	/*=================================================================== */
		JTP_State State();

	/*=================================================================== */
		/**
			@brief			Will return a JTP_HANDLE of the job session

			@pre			None

			@post			None

			@return
			JTP_HANDLE		JTP_HANDLE to the Conversation session.

			@exception		None
		*/
	/*=================================================================== */
	JTP_HANDLE getHandle();

	/*=================================================================== */
		/**
			@brief			Override default timeout time at transfer of jinitconf
							or jinitconf Default value is five (5) seconds. The value
							is also the timeout supervision of data transmissions by
							jresultreq() when you are an initiator. If the destinator
							reply (jresultind()) exceeds this timeout, the connection
							will be closed.If no time supervision of jresultreq() is
							wanted at all, the value can be set to zero.

			@pre			None

			@post			None

			@param			Sec
							Number of seconds, timeout.

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	void setTimeOut(int Sec		//IN
					);

	/*=================================================================== */
		/**
			@brief			Override default number of connect tries per
							whole set of addresses to a remote application
							Default value is one (1) try.

			@pre			None

			@post			None

			@param			Times
							Number of tries.

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	void setNoOfTries(int Times		//IN
						);

	/*=================================================================== */
		/**
			@brief			Override default delay time between connect
							tries with the whole set of addresses to application
							Default value is (0) seconds

			@pre			None

			@post			None

			@param			Times
							Number of seconds, delay

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	void setDelayTime(int Times		//IN
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

#endif //!defined(_ACS_JTP_Job_R2A_H_)
