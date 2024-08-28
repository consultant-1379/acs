/*=================================================================== */
/**
   @file   ACS_JTP_Job_R3A.h

   @brief  Header file for APJTP type module.

          This module contains all the declarations useful to
          specify the class ACS_JTP_Job_R3A.

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
#if !defined(_ACS_JTP_Job_R3A_H_)
#define _ACS_JTP_Job_R3A_H_

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
    @class		ACS_JTP_Job_R3A

    @brief		This is the interface class for job mode.
**/
  /*=================================================================== */
class  ACS_JTP_Job_R3A {

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
		@brief  This enum tells the about the state of the job.

	*/
	enum JTP_State {
		StateCreated = 0,
		StateConnected = 1,
		StateAccepted = 2,
		StateJobRunning = 3
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
							system_id of the node.

			@par			system_name
							Name of the system.

			@par 			system_type
							System Type of the node.

			@par 			node_state
							State of the node.

			@par			node_name
							Name of the node.

			@par			node_side
							Side of the JTP Node.
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

			@return			None

			@exception		None
		*/
	ACS_JTP_Job_R3A(char* Service,		//IN
					unsigned short MaxBufLen	//IN
					);

		/**
			@brief			Client constructor

			@pre

			@post

			@return			None

			@exception		None
		*/
	ACS_JTP_Job_R3A();

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
	virtual ~ACS_JTP_Job_R3A();

	/*=================================================================== */
		/**
			@brief			This is the initiator routine that is to be used
							when the client wants to start a JTP job.

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
			@brief			This is the initiator routine that is to be used
							when the client wants to start a JTP job.

			@pre			None

			@post			None

			@param			Node
							struct Node

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
	bool jinitreq(JTP_Node* Node,		//IN
				  unsigned short U1,		//IN
				  unsigned short U2,		//IN
				  unsigned short BufLen,	//IN
				  char* Buf		//IN
				  );

	/*=================================================================== */
		/**
			@brief			This method queries a subset of the cluster for the symbolic service
							name,identified by  serviceName/domain parameters.the scope parameter
							defines the type of nodes to be queried.

			@pre			None

			@post			None

			@param			scope
							AP/CP/BC

			@param			nodes
							vector of nodes where service was registered.

			@param			notReachNodes
							vector of unreachable nodes

			@return			bool
			true			The query was successful.
			false			Unsuccessful query.

			@exception		None
		*/
	/*=================================================================== */
	bool query(JTP_SystemType scope,					//IN
			   std::vector<JTP_Node>& nodes,			//OUT
			   std::vector<JTP_Node>& notReachNodes		//OUT
			   );

	/*=================================================================== */
		/**
			@brief			This method queries a subset of the cluster for the symbolic service
							name, identified by  serviceName/domain parameters.

			@param			scope
							The scope parameter defines the type of nodes to be queried.

			@param			nodes
							vector of nodes where service was registered.

			@param			notReachNodes
							vector of unreachable nodes

			@param			milliseconds
							Timeout in milliseconds

			@pre			None

			@post			None

			@return			bool
			true			The query was successful.
			false			Unsuccessful query.

			@exception		None
		*/
	/*=================================================================== */
	bool query(JTP_SystemType scope,					//IN
			   std::vector<JTP_Node>& nodes,			//OUT
			   std::vector<JTP_Node>& notReachNodes,	//OUT
			   int  milliseconds					//IN
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
							when the destinator receives a request for a JTP job.

			@pre			None

			@post			None

			@param			Node
							struct Node

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
	bool jinitind(JTP_Node& Node,		//OUT
				  unsigned short& U1,		//OUT
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
							R = 0 means job accepted.
							R = 1, 2, 3 reserved for JTP.
							R = (4..65535) means reject of job.

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
							when the initiator receives confirmation of a JTP job.

			@pre			None

			@post			None

			@param			U1
							User data 1, which is received from destinator.

			@param			U2
							User data 2, which is received from destinator.

			@param			R
							Result code, which is received from destinator.
							R = 0 means job accepted.
							R = 1 Congestion in JTP.
							R = 2 Breakdown on transport or lower layer.
							R = 3 Application not available.
							R = (4..65535) means reject of job.

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
							when either a client or destinator transmits data.

			@pre			None

			@post			None

			@param			U1
							User data 1, which is transferred to remote side.

			@param			U2
							User data 2, which is transferred to remote side.

			@param			R
							Result code.
							R = 0 means job successfully completed.
							R > 0 means failed job.

			@param			BufLen
							Length of sent data buffer.

			@param			Buf
							Sent data buffer.

			@return			bool
			true			The message was received successfully.
			false			Receiving of message failed, or faulty state.

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
			@brief			This is data reception routine that is to be used
							when either a client or destinator receives data

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
			@brief			This is jfault reception routine. Fault data is fetched

			@param			U1
							User data 1.

			@param			U2
							User data 2.

			@param			R
							Reason code, which is received from remote side.

			@pre			None

			@post			None

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
							JTPClient, JTPServer

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
							StateCreated,
							StateConnected,
							StateAccepted,
							StateJobRunning

			@exception		None
		*/
	/*=================================================================== */
		JTP_State State();

	/*=================================================================== */
		/**
			@brief			Will return a JTP_HANDLE of the job session

			@pre			None

			@post			None

			@return			JTP_HANDLE
							JTP_HANDLE to the Conversation session.

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
							Default value is one (1) try

			@pre			None

			@post			None

			@param			Times
							Number of tries '0' means for ever

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
							Default value is 0 seconds

			@pre			None

			@post			None

			@param			Sec
							Number of seconds, delay

			@return			void

			@exception		None
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
	const char* ServiceName; //service Name
	JTP_Session_R3A* Internal;
};

#endif //!defined(_ACS_JTP_Job_R3A_H_)
