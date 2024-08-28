/*=================================================================== */
/**
   @file   ACS_JTP_Conversation_R3A.h

   @brief  Header file for APJTP type module.

          This module contains all the declarations useful to
          specify the class ACS_JTP_Conversation_R3A.

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
#if !defined(_ACS_JTP_Conversation_R3A_H_)
#define _ACS_JTP_Conversation_R3A_H_
/*=====================================================================
                         DIRECTIVE DECLARATION SECTION
==================================================================== */
#include "ACS_JTP.h"
#include <vector>
using namespace std;

/*=====================================================================
                         FORWARD DECLARATION SECTION
==================================================================== */
class JTP_Service_R3A;
class JTP_Session_R3A;

/*=================================================================== */
	/**
		@class		ACS_JTP_Conversation_R3A

		@brief		This is the interface class for conversation mode.
	**/
 /*=================================================================== */
class ACS_JTP_Conversation_R3A {

	friend class JTP_Service_R3A;
	friend class JTP_Session_R3A;

	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
	                        ENUMERATED DECLARATION SECTION
	==================================================================== */
	/**
		@brief  This enum tells the about the side of the application.

	*/
	enum JTP_Side {
		JTPClient = 1, 
		JTPServer
	};
	
	/**
		@brief  This enum tells the about the state of the conversation/job.

	*/
	enum JTP_State {
		StateCreated = 0,
		StateConnected = 1,
		StateAccepted = 2,
		StateJobRunning = 3,
		StateWaitForData = 4
	};

	/**
		@brief  This enum tells the about the state of the JTP Node.

	*/
	enum JTP_NodeState {
		NODE_STATE_ACTIVE = 0,
		NODE_STATE_PASSIVE = 1,
		NODE_STATE_UNKNOWN = 2
	};

	/**
		@brief  This enum is the identifier of the JTP Node.

	*/
	enum JTP_SystemId {
		SYSTEM_ID_UNKNOWN = -1,
		SYSTEM_ID_THIS_NODE = 50000,
		SYSTEM_ID_PARTNER_NODE,
		SYSTEM_ID_FRONT_END_AP,
		SYSTEM_ID_CP_CLOCK_MASTER,
		SYSTEM_ID_CP_ALARM_MASTER
	};

	/**
		@brief  This enum gives the information about the system.

	*/
	enum JTP_SystemType {
			SYSTEM_TYPE_UNKNOWN = -1,
			SYSTEM_TYPE_AP = 0,
			SYSTEM_TYPE_CP = 1,
			SYSTEM_TYPE_BC = 2
		};

	/**
		@brief  This enum is the side of the JTP Node.

	*/
	enum JTP_NodeSide	{
				NODE_SIDE_UNDEFINED	=	-1,
				NODE_SIDE_A			=	0,
				NODE_SIDE_B			=	1
	};
	/*=================================================================== */
		/**
			@struct			JTP_Node

			@brief			It represents the structure of a node.

			@par			system_id
							Identifier of the node.
			@par			system_name
							Name of the system
			@par			system_type
							System Type of the node.
			@par			node_state
							State of the node.
			@par			node_name
							Name of the node.
			@par			node_side
							Side of the node.
		*/
	/*=================================================================== */
  	typedef struct {
		/**
		 * @brief	system_id
		 */
		int32_t system_id;

		/**
		 * @brief	system_name
		 */
		char system_name[16];
		/**
		 * @brief	system_type
		 */
		JTP_SystemType system_type;

		/**
		 * @brief	node_state
		 */
		JTP_NodeState node_state;

		/**
		 * @brief	node_name
		 */
		char node_name[16];

		/**
		 * @brief	node_side
		 */
		JTP_NodeSide node_side;


	} JTP_Node;

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
	ACS_JTP_Conversation_R3A(char* Service,		//IN
							 unsigned short MaxBufLen		//IN
							 );

		/**
			@brief			Client constructor

			@par			none

			@pre

			@post

			@return			none

			@exception		none
		*/
	ACS_JTP_Conversation_R3A();

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
	virtual ~ACS_JTP_Conversation_R3A();

	/*===================================================================
	                           PUBLIC METHOD
	=================================================================== */

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
	bool jexinitreq(unsigned short U1,		//IN
					unsigned short U2		//IN
					);

	/*=================================================================== */
		/**
			@brief			This is the initiator routine that is to be used
							when the client wants to start a JTP Conversation.


			@pre			none

			@post			none

			@param			Node
							struct Node

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
	bool jexinitreq(JTP_Node* Node,		//IN
					unsigned short U1,		//IN
					unsigned short U2		//IN
					);

	/*=================================================================== */
		/**
			@brief			This method queries a subset of the cluster for the symbolic service
							name, identified by serviceName/domain parameters.the scope parameter
							defines the type of nodes to be queried.

			@pre			none

			@post			none

			@param			scope
							AP/CP/BC, It defines the type of nodes to be queried.

			@param			nodes
							vector of nodes where service was registered.

			@param			notReachNodes
							vector of unreachable nodes

			@return			bool
			true			The query was successful.
			false			Unsuccessful query.

			@exception		none
		*/
	/*=================================================================== */
	bool query(JTP_SystemType scope,				//IN
			   std::vector<JTP_Node>& nodes,		//OUT
			   std::vector<JTP_Node>& notReachNodes	//OUT
			   );

	/*=================================================================== */
		/**
			@brief			This method queries a subset of the cluster for the symbolic service
							name,identified by  serviceName/domain parameters.the scope parameter
							defines the type of nodes to be queried.

			@pre			none

			@post			none

			@param			scope
							AP/CP/BC, It defines the type of nodes to be queried.

			@param			nodes
							vector of nodes where service was registered.

			@param			notReachNodes
							vector of unreachable nodes

			@param			milliseconds
							a timeout in milliseconds

			@return			bool
			true			The query was successful.
			false			Unsuccessful query.

			@exception		none
		*/
	/*=================================================================== */
	bool query(JTP_SystemType scope,					//IN
			   std::vector<JTP_Node>& nodes,			//OUT
			   std::vector<JTP_Node>& notReachNodes,	//OUT
			   int milliseconds					//IN
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
							when the destinator receives a request for a JTP Conversation.

			@pre			none

			@post			none

			@param			Node
							struct Node

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
	bool jexinitind(JTP_Node& Node,		//OUT
					unsigned short& U1,		//OUT
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
							R = 1, 2, 3 reserved for JTP.
							R = (4..65535) means reject of conversation.

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
							R > 0 means failure.

			@return			bool
			true			The message was received successfully.
			false			Receiving of message failed, or faulty state.

			@exception		none
		*/
	/*=================================================================== */
	bool jexinitconf(unsigned short& U1,		//OUT
					 unsigned short& U2,		//OUT
					 unsigned short& R			//OUT
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
	bool jexdataind(unsigned short& U1,			//OUT
					unsigned short& U2,			//OUT
					unsigned short& BufLen,		//OUT
					char*& Buf			//OUT
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
							Reason code > 0 means failure.
							Reason code = 0, means success.

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
							User data 1, which is received from remote side.

			@param			U2
							User data 2, which is received from remote side.

			@param			Reason
							Reason code, which is received from remote side.
							Reason = 0 means normal disconnect.
							Reason > 0 means failure.

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

			@return			JTP_State
							StateCreated, StateConnected,
							StateAccepted, StateJobRunning

			@exception		none
		*/
	/*=================================================================== */
	JTP_State State();

	/*=================================================================== */
		/**
			@brief			Will return a JTP_HANDLE of the Conversation session

			@pre			none

			@post			none

			@return			JTP_HANDLE
							JTP_HANDLE to the Conversation session.

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
							Number of seconds, timeout

			@return			void

			@exception		none
		*/
	/*=================================================================== */
	void setTimeOut(int Sec		//IN
					);

	/*=================================================================== */
		/**
			@brief			Override default number of connect tries per
							whole set of addresses to a remote application
							Default value is one (1) try

			@param			Times
							Number of tries.

			@pre			none

			@post			none

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
							Default value is 0 seconds

			@pre			none

			@post			none

			@param			Sec
							Number of seconds, delay

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
	const char* ServiceName; //service name
	JTP_Session_R3A*	Internal; //pointer to JTP_Session_R3A class
};

#endif //!defined(_ACS_JTP_Conversation_R3A_H_)
