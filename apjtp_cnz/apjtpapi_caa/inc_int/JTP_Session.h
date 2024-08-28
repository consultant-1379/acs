/*=================================================================== */
/**
   @file   JTP_Session.h

   @brief  Header file for APJTP type module.

          This module contains all the declarations useful to
          specify the class JTP_Session.

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
#if !defined(_JTP_Session_H_)
#define _JTP_Session_H_
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <JTP_Service.h>
#include <ACS_DSD_Client.h>
#include <ACS_DSD_Session.h>
/*=================================================================== */
	/**
		@class		JTP_Session

		@brief		This is internal class for sessions.
	**/
/*=================================================================== */
class JTP_Session {

	friend class JTP_Service;

public:
	/*=====================================================================
						CONSTANT DECLARATION SECTION
	==================================================================== */
	/*=================================================================== */
		/**
			@brief			This const defines the default delay time.

							It defines the number of seconds which an application waits between
							tries when it connects to the remote destinator.
		*/
	/*=================================================================== */

	static const int JTP_DEFAULT_DELAY_TIME = 0;


public:
	/*=====================================================================
						ENUMERATED DECLARATION SECTION
	==================================================================== */
	/*=================================================================== */
	/**
		@brief			This enum tells the side of the application.

						It could be either initiator or destinator.
	*/
	enum JTP_Side {
		JTPClient = 1,
		JTPServer
	};
	
	/**
		@brief			This enum tells the type of the session.

						It could be either job or conversation.
	*/
	enum JTP_Type {
		JTPJob = 1,
		JTPConversation
	};
	
	/**
		@brief			This enum tells the type of the message
						exchanged between applications.

	*/
	enum JTP_Msg {
		JEXINITREQ = 1,
		JEXINITRSP = 2,
		JEXDATAREQ = 3,
		JEXDATAREQ_WR = 4,
		JEXDISCREQ = 5,
		JINITREQ   = 6,
		JINITRSP   = 7,
		JRESULTREQ = 8
	};
	
	/**
		@brief			This enum tells the state of the application.

	*/
	enum JTP_State {
		StateCreated = 0,
		StateConnected,
		StateAccepted,
		StateJobRunning,
		StateWaitForData
	};
	
	/**
		@brief			This enum tells the state and transfer type info
						about the application.

	*/
	enum JTP_IntState {
		ServerJob = 0,
		ClientJob = 1,
		ServerConv = 2,
		ClientConv = 3
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

			@param			jtype
							Either JTPJob or JTPConversation

			@param			Service
							Name of Application.

			@return			None

			@exception		None
		*/
	JTP_Session(JTP_Type jtype,		//IN
				char* Service	//IN
				);

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
							Length of buffer.

			@return			None

			@exception		None
		 */
	JTP_Session(JTP_Type jtype,		//IN
				char* Service,	//IN
				short iMaxBuf	//IN
				);


public:

	/*===================================================================
						   CLASS DESTRUCTOR
	=================================================================== */
	/*=================================================================== */
	/**

			  @brief           Destructor

			  @pre             None

			  @post            None

			  @return          None

			  @exception       None
	 */
	/*=================================================================== */
	virtual ~JTP_Session();


public:

	/*=================================================================== */
		/**

			@brief           This method is used to return JType as a
						   string.

			@pre             None

			@post            None

			@return          const ACE_TCHAE*

			@exception       None
		*/
	/*=================================================================== */
	const char* JTypeAsString();

	/*=================================================================== */
		/**

			@brief           This method is used to return JState as string.

			@pre             None

			@post            None

			@return          const char*

			@exception       None
		*/
	/*=================================================================== */
	const char* JStateAsString();

public:

	/*=================================================================== */
		/**

			@brief			This method is used to receive incoming conversation
							indication.

			@pre			None

			@post			None

			@param			MsgLen
							Length of message.

			@param			protocolVersion
							Protocol Version in message.

			@param			serviceName
							Name of application.

			@param			Session
							Pointer to ACS_DSD_Session

			@param			service_Buffer
							Message Buffer.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool newSessionServer(unsigned long MsgLen,			//IN
						  char protocolVersion,	//IN
						  char* serviceName,		//IN
						  ACS_DSD_Session& Session,		//IN
						  char * service_Buffer	//IN
						  );

	/*=================================================================== */
		/**

			@brief			This method is used to receive incoming conversation
							indication.

			@pre			None

			@post			None

			@param			MsgLen
							Length of message.

			@param			protocolVersion
							Protocol version in message.

			@param			serviceName
							Name of application.

			@param			Session
							Pointer to ACS_DSD_Session.

			@param			service_Buffer
							Message buffer.

			@param			service_readBufferLen
							Read Buffer length.

			@param			service_bufferCursor
							Read buffer cursor.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool newSessionServer(unsigned long MsgLen,				//IN
						  char protocolVersion,		//IN
						  char* serviceName,			//IN
						  ACS_DSD_Session& Session,			//IN
						  char* service_Buffer,		//IN
						  int service_readBufferLen, 	//IN
						  int service_bufferCursor	//IN
						  );

	/*=================================================================== */
		/**

			@brief			This method copies application and address data into
							conversation object.

			@pre			None

			@post			None

			@param			serviceName
							Name of application.

			@param			Session
							Pointer to ACS_DSD_Session.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool newSession(char* serviceName,		//IN
					ACS_DSD_Session& Session	//IN
					);

	/*=================================================================== */
		/**

			@brief			This method is used to receive incoming job indication.

			@pre			None

			@post			None

			@param			MsgLen
							Length of message.

			@param			protocolVersion
							Protocol Version.

			@param			serviceName
							Name of application.

			@param			Session
							Pointer to ACS_DSD_Session.

			@param			service_Buffer
							Service Buffer.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool newJobServer(unsigned long MsgLen,			//IN
					  char protocolVersion,	//IN
					  char* serviceName,		//IN
					  ACS_DSD_Session& Session,		//IN
					  char* service_Buffer		//IN
					  );

	/*=================================================================== */
		/**

			@brief			This method is used to receive incoming job indication.

			@pre			None

			@post			None

			@param			MsgLen
							Length of message.

			@param			protocolVersion
							Protocol Version.

			@param			serviceName
							Name of application.

			@param			Session
							Pointer to ACS_DSD_Session.

			@param			service_Buffer
							Service Buffer.

			@param			service_readBufferLen
							Buffer Length.

			@param			service_bufferCursor
							Buffer Cursor.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool newJobServer(unsigned long MsgLen,				//IN
					  char protocolVersion,		//IN
					  char* serviceName,			//IN
					  ACS_DSD_Session& Session,			//IN
					  char* service_Buffer,		//IN
					  int service_readBufferLen,	//IN
					  int service_bufferCursor	//IN
					  );

	/*=================================================================== */
		/**

			@brief			This method copies job and application data into
							job object.

			@pre			None

			@post			None

			@param			serviceName
							Name of application.

			@param			Session
							Pointer to session.

			@return			bool
			true			Success.
			false			Failure.

			@exception		None
		*/
	/*=================================================================== */
	bool newJob(char* serviceName,		//IN
				ACS_DSD_Session& Session	//IN
				);

	/*=================================================================== */
		/**

			@brief			This method implements polling function
							for reception of job or conversation initiation
							response.

			@pre			None

			@post			None

			@param			handle
							ACE HANDLE

			@param			omittAtZero

			@return			bool
			true			Success.
			false			Failure.

			@exception		None
		*/
	/*=================================================================== */
	bool pollReply(ACE_HANDLE handle,		//IN
				   bool omittAtZero			//IN
				   );

	/*=================================================================== */
		/**

			@brief			This method is used to make a pause in execution.

			@pre			None

			@post			None

			@param			sleepTime
							Pause Time.

			@return			void.

			@exception		None
		*/
	/*=================================================================== */
	void delay(int sleepTime		//IN
				);

	/*=================================================================== */
		/**

			@brief			This method is used to override the default value of
							number of retries when connecting to remote destinator.

			@pre			None

			@post			None

			@param			Tries
							No of retries.

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	void setNoOfTries(int Tries		//IN
						);

	/*=================================================================== */
		/**

			@brief			This method is used to override the default value
							of number of seconds to wait between tries when
							connecting to remote destinator.

			@pre			None

			@post			None

			@param			Sec
							No of seconds.

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	void setDelayTime(int Sec		//IN
						);

	/*=================================================================== */
		/**

			@brief			This method is used to override the default value of
							number of seconds to wait for initiation response from
							remote destinator.

			@pre			None

			@post			None

			@param			Sec
							No of seconds.

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	void setTimeOut(int Sec		//IN
					);

	/*=================================================================== */
		/**

			@brief			This method returns the side of the application.
							It could be either initiator or destinator.

			@pre			None

			@post			None

			@return			JTP_Side.

			@exception		None
		*/
	/*=================================================================== */
	JTP_Side Side() const;

	/*=================================================================== */
		/**

			@brief			This method returns the state of the application.
							Refer JTP_State for values.

			@pre			None

			@post			None

			@return			JTP_State

			@exception		None
		*/
	/*=================================================================== */
	JTP_State State() const;

	/*=================================================================== */
		/**

			@brief			This method is used to fetch and validate the
							job or conversation initiation response data.

			@pre			None

			@post			None

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool fetch_conf();

	/*=================================================================== */
		/**

			@brief			This method is used to close the channel and
							clear the state.

			@pre			None

			@post			None

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	void reset();

	/*=================================================================== */
		/**

			@brief			This method is used to extract conversation initiation
							data and deliver it to the application.

			@pre			None

			@post			None

			@param			U1
							User data 1.

			@param			U2
							User data 2.

			@param			R
							Result code.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jexinitconf(unsigned short& U1,		//OUT
					 unsigned short& U2,		//OUT
					 unsigned short& R			//OUT
					 );

	/*=================================================================== */
		/**

			@brief			This method is used to read the response data.

			@pre			None

			@post			None

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	void jexinitind_read();

	/*=================================================================== */
		/**

			@brief			This method is used to receive conversation indication
							data.

			@pre			None

			@post			None

			@param			Size
							Size of Message.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jexinitind_rcv(long Size		//IN
						);

	/*=================================================================== */
		/**

		@brief			This method is used to send the response to the
						application.

		@pre			None

		@post			None

		@return			bool
		true			Success
		false			Failure

		@exception		None
		*/
	/*=================================================================== */
	bool jexinitrsp_snd();

	/*=================================================================== */
		/**

			@brief			This method is used to administer sending of conversation
							Initiation response data.

			@pre			None

			@post			None

			@param			U1
							User data 1.

			@param			U2
							User data 2.

			@param			R
							Result code.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jexinitrsp(short U1,		//IN
					short U2,		//IN
					short R			//IN
					);

	/*=================================================================== */
		/**

			@brief			This method is used to send conversation data.

			@pre			None

			@post			None

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jexdatareq_snd();

	/*=================================================================== */
		/**

			@brief			This method is used to administer sending of
							conversation data.

			@pre			None

			@post			None

			@param			U1
							User data 1.

			@param			U2
							User data 2.

			@param			BufferLength
							Length of buffer.

			@param			Buf
							Buffer.

			@return			bool
			true			Success
			false			Failure

			@exception       None
		*/
	/*=================================================================== */
	bool jexdatareq(short U1,			//IN
					short U2,			//IN
					short BufferLength,	//IN
					char* Buf			//IN
					);

	/*=================================================================== */
		/**

			@brief			This method is used to read conversation data.

			@pre			None

			@post			None

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jexdataind_read();

	/*=================================================================== */
		/**

			@brief			This method is used to receive conversation data.

			@pre			None

			@post			None

			@param			Size
							Size of data.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jexdataind_rcv(long Size		//IN
						);

	/*=================================================================== */
		/**

			@brief			This method is used to deliver conversation data to
							application.

			@pre			None

			@post			None

			@param			U1
							User data 1.

			@param			U2
							User data 2.

			@param			BufferLength
							Length of buffer.

			@param			Buf
							Buffer

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jexdataind(unsigned short& U1,				//OUT
					unsigned short& U2, 			//OUT
					unsigned short& BufferLength,	//OUT
					char*& Buf				//OUT
					);

	/*=================================================================== */
		/**

			@brief			This method is used to disconnect conversation
							session by sending disconnect primitive to the
							remote application and closing.

			@pre			None

			@post			None

			@param			U1
							User data 1.

			@param			U2
							User data 2.

			@param			R
							Result code.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jexdiscreq(short U1,		//IN
					short U2,		//IN
					short R			//IN
					);

	/*=================================================================== */
		/**

			@brief			This method is used to read jexdiscind data from message
							into session.

			@pre			None

			@post			None

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jexdiscind_read();

	/*=================================================================== */
		/**

			@brief			This method is used to receive conversation
							disconnection.

			@pre			None

			@post			None

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jexdiscind_rcv(long Size		//IN
						);

	/*=================================================================== */
		/**

			@brief			This method is used to deliver disconnection
							information to application.

			@pre			None

			@post			None

			@param			U1
							User data 1.

			@param			U2
							User data 2.

			@param			R
							Result code.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jexdiscind(unsigned short& U1,		//OUT
					unsigned short& U2,		//OUT
					unsigned short& R		//OUT
					);

	/*=================================================================== */
		/**

			@brief			This method is used to extract job indication data
							and deliver to the application.

			@pre			None

			@post			None

			@param			U1
							User data 1.

			@param			U2
							User data 2.

			@param			R
							Result code.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jinitconf(unsigned short& U1,		//OUT
				   unsigned short& U2,		//OUT
				   unsigned short& R		//OUT
				   );

	/*=================================================================== */
		/**

			@brief			This method is used to read the indication data from
							message buffer to session.

			@pre			None

			@post			None

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	void jinitind_read();

	/*=================================================================== */
		/**

			@brief			This method is used to receive job indication.

			@param			Size

			@pre			None

			@post			None

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jinitind_rcv(long Size		//IN
						);

	/*=================================================================== */
		/**

			@brief			This method send job initiation response data.

			@pre			None

			@post			None

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jinitrsp_snd();

	/*=================================================================== */
		/**

			@brief			This method administers sending of job initiation
							response data.

			@pre			None

			@post			None

			@param			U1
							User data 1

			@param			U2
							User data 2

			@param			R
							Result code.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jinitrsp(short U1,		//IN
				  short U2,		//IN
				  short R		//IN
				  );

	/*=================================================================== */
	/**

			  @brief           This method is used to send job result data.

			  @pre             None

			  @post            None

			  @return          bool
			  true			   Success
			  false            Failure

			  @exception       None
	 */
	/*=================================================================== */
	bool jresultreq_snd();

	/*=================================================================== */
		/**

			@brief			This method administers sending of job result data.

			@pre			None

			@post			None

			@param			U1
							User data 1.

			@param			U2
							User data 2.

			@param			R
							Response data.

			@param			BufferLength
							Length of buffer.

			@param			Buf
							Buffer

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jresultreq(short U1,			//IN
					short U2,			//IN
					short R,			//IN
					short BufferLength,	//IN
					char* Buf			//IN
					);

	/*=================================================================== */
		/**

			@brief			This method is used to fetch job result data.

			@pre			None

			@post			None

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jresultind_read();

	/*=================================================================== */
		/**

			@brief			This method is used to receive job result data.

			@param			Size

			@pre			None

			@post			None

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jresultind_rcv(long Size		//IN
						);

	/*=================================================================== */
		/**

			@brief			This method is used to give job result to the
							application.

			@pre			None

			@post			None

			@param			U1
							User data 1.

			@param			U2
							User data 2.

			@param			R
							Result code.

			@param			BufferLength
							Length of buffer.

			@param			Buf
							Buffer

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jresultind(unsigned short& U1,				//OUT
					unsigned short& U2,				//OUT
					unsigned short& R,				//OUT
					unsigned short& BufferLength,	//OUT
					char*& Buf				//OUT
					);

	/*=================================================================== */
		/**

			@brief			This method is used to terminate incomplete job.

			@pre			None

			@post			None

			@return			bool
			true			Successful termination.
			false			Failure.

			@exception		None
		*/
	/*=================================================================== */
	bool jtermreq();

	/*=================================================================== */
		/**

			@brief			This method delivers job result data to
							application.

			@pre			None

			@post			None

			@param			U1
							User data 1.

			@param			U2
							User data 2.

			@param			R
							Result code.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jfault(unsigned short& U1,		//OUT
				unsigned short& U2,		//OUT
				unsigned short& R		//OUT
				);

	/*=================================================================== */
		/**

			@brief			This method calls the DSD getHandles and returns
							the handle for the connecting session.

			@pre			None

			@post			None

			@param			handle
							ACE_HANDLE

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool getHandle(ACE_HANDLE& handle		//OUT
					);

	/*=================================================================== */
		/**

			@brief			This method calls the DSD query method.

			@pre			None

			@post			None

			@param			serviceName
							Name of application

			@param			domain
							Name of domain

			@param			scope
							It defines the subset of nodes to be queried.


			@param			nodes
							Vector of reachable nodes providing the desired service.

			@param			notReachNodes
							Vector of unreachable nodes.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool JTPquery(const char* serviceName,						//IN
				  const char* domain,							//IN
				  acs_dsd::SystemTypeConstants scope,						//IN
				  std::vector<ACS_DSD_Node>& nodes,		//OUT
				  std::vector<ACS_DSD_Node>& notReachNodes	//OUT
				  );

	/*=================================================================== */
		/**

			@brief			This method calls the DSD query method.

			@pre			None

			@post			None

			@param			serviceName
							Name of application

			@param			domain
							Name of domain

			@param			scope
							It defines the subset of nodes to be queried.

			@param			nodes
							Vector of reachable nodes providing the desired service.

			@param			notReachNodes
							Vector of unreachable nodes.

			@param			milliseconds
							This timeout forces the query method to return prematurely,
							if necessary.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		 */
	/*=================================================================== */

	bool JTPquery(const char* serviceName,							//IN
				  const char* domain,								//IN
				  acs_dsd::SystemTypeConstants scope, 							//IN
				  std::vector<ACS_DSD_Node>& nodes,			//OUT
				  std::vector<ACS_DSD_Node>& notReachNodes,	//OUT
				  int milliseconds								//IN
				  );

	/*=================================================================== */
		/**

			@brief      	This method calls the recvMsg method in DSD
							and keeps track of how much data is copied from
							tcp in buffer to jtp application.

			@pre			None

			@post			None

			@param			buf
							Data copied to jtp application.

			@param			nbytes
							No of bytes copied.

			@return			bool
			true			Success
			false			Failure

			@exception		None
		*/
	/*=================================================================== */
	bool jtpRecvMsg(char* buf,		//OUT
					unsigned short nbytes	//OUT
					);
	/*=================================================================== */
		/**

			@brief			This method is used to get the apNodeNumber.

			@pre			None

			@post			None

			@param			None

			@return			unsigned char

			@exception		None
		*/
	/*=================================================================== */
	unsigned char getOwnNodeId();
	/*=================================================================== */

protected:
	/*=================================================================== */
		/**

			@brief			This method is used to get session information.

			@pre			None

			@post			None

			@param			traceBuf
							Buffer containing session information.

			@param			traceBufLen
							Length of buffer.

			@return			None

			@exception		None
		*/
	/*=================================================================== */
	void getSessionInformation(char* traceBuf,		//OUT
							   int traceBufLen	//OUT
							   );

	/*=================================================================== */
		/**

			@brief			This method is used to administer and perform
							job connection attempts towards remote
							application. Address redundancy function is included.

			@pre			None

			@post			None

			@return			None

			@exception		None
		*/
	/*=================================================================== */
	int	jinitreq_snd();

	/*=================================================================== */
		/**

			@brief			This method is used to administer and perform
							conversation connection attempts towards remote
							application. Address redundancy function is included.

			@pre			None

			@post			None

			@return			int

			@exception		None
		*/
	/*=================================================================== */
	int	jexinitreq_snd();

private:

	/*=================================================================== */
		/**

			@brief			This method is used add information of
							previously disconnected channels into
							initiating primitives.

			@param			s
							Pointer to JTP Session.

			@pre			None

			@post			None

			@return			ACE_UINT32

			@exception		None
		*/
	/*=================================================================== */
	static unsigned int JSLAddBufferData(JTP_Session* s		//IN
										);

	/*=================================================================== */
		/**

			@brief			This method is used to clear all
							sessions from disconnect registrations

			@pre			None

			@post			None

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	static void	JSLClearSessions();

	/*=================================================================== */
		/**

			@brief			This method is used to add disconnect registrations
							to the list.

			@pre			None

			@post			None

			@param			s
							Pointer to JTP Session.

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	static void JSLAddDisconnected(JTP_Session* s		//IN
									);

	/*=================================================================== */
		/**

			@brief			This method is used to expand disconnect registrations
							list.

			@pre			None

			@post			None

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	static void JSLCLgrow();

	/*=================================================================== */
		/**

			@brief			This method is used to add a registration
							element.

			@pre			None

			@post			None

			@param			s
							Pointer to JTP Session.

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	static void	JSLAddId(JTP_Session* s			//IN
						);

	/*=================================================================== */
		/**

			@brief			This method is used to check for half open
							connections.

			@pre			None

			@post			None

			@param			s
							Pointer to JTP Session.

			@param			MsgLen
							Length of message.

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	static void JSLCheckSessions(JTP_Session* s,			//IN
								 unsigned long MsgLen			//IN
								 );

	/*=================================================================== */
		/**

			@brief			This method is used to close half open
							connections.

			@pre			None

			@post   		None

			@param			APNo

			@param			APRestartCntr

			@param			PID

			@param			ConnectionCntr


			@param			parentSysId

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	static void JSLCloseSession(unsigned char APNo,				//IN
								unsigned char APRestartCntr, 	//IN
								unsigned short PID, 			//IN
								unsigned short ConnectionCntr,	//IN
								unsigned int parentSysId		//IN
								);

	/*=================================================================== */
		/**

			@brief			This method is used to remove registration of
							half open connections from list.

			@param			s
							Pointer to JTP Session.

			@pre			None

			@post			None

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	static void JSLRemoveSession(JTP_Session* s		//IN
								);

	/*=================================================================== */
		/**

			@brief			This method is used to expand disconnect registrations
							list.

			@pre			None

			@post			None

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	static void	JSLSLgrow();

	/*=================================================================== */
		/**

			@brief			This method initiates disconnect registrations list.

			@pre			None

			@post			None

			@return			void

			@exception		None
		*/
	/*=================================================================== */
	static void JSLinit();
	

public:
	/*====================================================================
					VARIABLE DECLARATION SECTION
	==================================================================== */
	/**
	 * @brief	APNo
	 */
	JTP_Side JSide;

protected:
	/*===================================================================
					PROTECTED ATTRIBUTE
	=================================================================== */
	/**
	 * @brief	APNo
	 */
	unsigned char	APNo;

	/**
	 * @brief	APRestartCntr
	 */
	unsigned char	APRestartCntr;

	/**
	 * @brief	PID
	 * 			Process Identifier.
	 */
	unsigned short	PID;

	/**
	 * @brief	ConnectionCntr
	 */
	unsigned short 	ConnectionCntr;

	/**
	 * @brief	system_id
	 */
	unsigned int 	system_id;

	/**
	 * @brief	JType
	 * 			Type of communication
	 */
	JTP_Type 		JType;

	/**
	 * @brief	JState
	 * 			State of application.
	 */
	JTP_State 		JState;

	/**
	 * @brief	ServiceName
	 * 			Name of Service
	 */
	char 		ServiceName[32];

	/**
	 * @brief	ProtocolVersion
	 */
	char 				ProtocolVersion;

	/**
	* @brief	IntState
	*/
	JTP_IntState 			IntState;

	/**
	 * @brief	DelayTime
	 */
	int 				DelayTime;

	/**
	 * @brief	NoOfTries
	 */
	int 				NoOfTries;

	/**
	 * @brief	TimeOut
	 */
	int 				TimeOut;

	/**
	 * @brief	u1
	 */
	unsigned short 				u1;

	/**
	 * @brief	Au1
	 */
	unsigned short Au1;	 // Local and AXE format

	/**
	 * @brief 	u2
	 */
	unsigned short 				u2;

	/**
	 * @brief	Au2
	 */
	unsigned short Au2;	 // Local and AXE format

	/**
	 * @brief	r
	 */
	unsigned short 				r;

	/**
	 * @brief	Ar
	 */
	unsigned short Ar;	 // Local and AXE format

	/**
	 * @brief	MaxBuf
	 */
	unsigned short 				MaxBuf;

	/**
	 * @brief	AMaxBuf
	 */
	unsigned short AMaxBuf; // Local and AXE format

	 /**
	  * @brief	BufLen
	  */
	unsigned short 				BufLen;

	/**
	 * @brief	ABufLen
	 */
	unsigned short	 ABufLen; // Local and AXE format

	 /**
	  * @brief	Buffer[64000]
	  */
	char 				Buffer[64000];

	/**
	 * @brief	readBufferLen
	 */
	int 				readBufferLen;

	/**
	 * @brief	bufferCursor
	 */
	int 				bufferCursor;

	/**
	 * @brief	mySession
	 */
	ACS_DSD_Session* 		mySession;  //pointer to ACS_DSD_Session new version

	/**
	 * @brief	pDSDClient
	 */
	ACS_DSD_Client* 		pDSDClient; //pointer to ACS_DSD_Client class

	/**
	 * @brief	NodeSv
	 */
	ACS_DSD_Node*  NodeSv; 	//pointer to Node struct
};//end JTP_Session class

#endif //!defined(_JTP_Session_H_)
