/*=================================================================== */
/**
	@file		acs_nsf_server.h

	@brief		Header file for network surveklliance functionality for NSF.

				This module contains all the declarations useful to
				specify the class.

	@version 	1.0.0

**/
/*
HISTORY
This section contains reference to problem report and related
software correction performed inside this module


PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
N/A       25/12/2010   XCHEMAD     Initial Release
N/A       19/04/2011   XTANAGG     Added doxygen comments.
N/A       25/02/2014   XFABPAG     TR HS31786 Fix
N/A       12/05/2014   XQUYDAO     Update for cable-less environment
==================================================================== */
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#if !defined(_ACS_NSF_Server_H_)
#define _ACS_NSF_Server_H_

#define BRFC_NSF_OBJ_DNNAME "brfPersistentDataOwnerId=ERIC-APG-ACS-NSF,brfParticipantContainerId=1"

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <vector>
#include <ace/ace_wchar.h>
//#include <ace/Recursive_Thread_Mutex.h>
//#include <ace/Synch.h>
//#include <ace/Task_T.h>
#include <ace/Monotonic_Time_Policy.h>
#include <ace/Event_Handler.h>
#include <acs_nsf_types.h>
#include <acs_nsf_common.h>
#include <acs_nsf_aeh.h>
#include <acs_nsf_command_handler.h>
#include <ACS_DSD_Server.h>
#include <ACS_DSD_Client.h>
#include <acs_nsf_service.h>

using namespace std;

/**
 * @brief	ReportEvent
 * @param 	specificProblem
 * @param 	severity
 * @param 	probableCause
 * @param 	objectOfReference
 * @param 	problemData
 * @param 	problemText
 * @return
 */
ACE_INT32  ReportEvent(ACE_UINT32 specificProblem, const char* severity, const char* probableCause, const char* objectOfReference, const char* problemData, const char* problemText);

/**
 * @brief	SendAlarm
 * @param 	IPAddrStr
 * @param 	gw
 * @param 	node
 * @param 	cease
 * @return
 */
int SendAlarm(string IPAddrStr,
				int gw,
				int node,
				int cease);
/**
 * @brief	SendAlarm2
 * @param 	IPAddrStr
 * @param 	gw
 * @param 	node
 * @param 	cease
 * @return
 */
int SendAlarm2(string IPAddrStr,
				int gw,
				int node,
				int cease);

/**
 * @brief	SendAlarmCableLess
 * @param 	isActiveNode
 * @param 	node
 * @param 	cease
 * @return
 */
int SendAlarmCableLess(bool isActiveNode, 
				int node, 
				int cease);
/**
 * @brief	CeaseAlarm
 * @param 	IPAddrStr
 * @param 	gw
 * @param 	node
 * @return
 */
int CeaseAlarm(string IPAddrStr,
				int gw,
				int node);

/**
 * @brief	CeaseAlarm2
 * @param 	IPAddrStr
 * @param 	gw
 * @param 	node
 * @return
 */
int CeaseAlarm2(string IPAddrStr,
				int gw,
				int node);

/**
 * @brief	controlAlarmBit
 * @param 	gw
 * @param 	network
 * @param 	node
 * @param 	mark
 * @return
 */
int controlAlarmBit(int gw,
				int network,
				int node,
				bool mark);

/**
 *      @brief          ThrExitHandler
 *                      Class for handling thread exit.
 *
 */
#if 0
class ThrExitHandler : public ACE_At_Thread_Exit
{
 
public:
	static bool init()
	{
		ThrExitHandler *thrExitHandler = new ThrExitHandler();
		if( thrExitHandler == 0 )
		{
			return false;
		}
                thrExitHandler->setThrStatusInMap(ACE_Thread_Manager::instance()->thr_self(), true);

		int result = ACE_Thread_Manager::instance()->at_exit( thrExitHandler );

		if( result == -1 )
		{
			return false;
		}
		else
		{	
			return true;
		}
	}

	static bool CheckNoThreads()
	{
		theThrMutex.acquire();

		map<ACE_thread_t, bool>::iterator itr = theThrStatusMap.begin();
                while ( itr != theThrStatusMap.end())
                {
			if( itr->second == true )
			{
				theThrMutex.release();
				return false;
			}
                        ++itr;
                }
		theThrMutex.release();
		return true;
	}

	static void cleanup()
	{
		theThrMutex.acquire();

		if( !theThrStatusMap.empty())
		{
			theThrStatusMap.clear();
		}
		theThrMutex.release();
		theThrMutex.remove();
	}
#if 0
	static bool CheckAllFuncThreads( void )
	{
		//Lock the mutex.
		theThrMutex.acquire();

		if( theThrStatusMap.empty())
		{
			theThrMutex.release();
			return false;	//No Thread has been spawned till now.
		}

		//Check if any thread has exited or not.
		for( map<ACE_thread_t, bool>::iterator itr = theThrStatusMap.begin();
			itr != theThrStatusMap.end();  ++itr )
		{
			if( itr->second == false )
			{
				theThrMutex.release();
				return false;
			}
		}
		theThrMutex.release();
		return true;
	}

#endif
        virtual void apply(void)
        {
		if( !theThrStatusMap.empty())
		{
			setThrStatusInMap( ACE_Thread_Manager::instance()->thr_self(), false);
		}
        }

private:

	void setThrStatusInMap( ACE_thread_t thrId, bool isThrAlive)
        {
		//Acquire the Mutex
                theThrMutex.acquire();

		map<ACE_thread_t, bool>::iterator itr = theThrStatusMap.find( thrId ); 
		if ( itr  != theThrStatusMap.end() )
		{
			theThrStatusMap.erase( itr );
		}
		theThrStatusMap.insert(std::pair<ACE_thread_t, bool>(thrId, isThrAlive));
                theThrMutex.release();
        }

        static map<ACE_thread_t, bool> theThrStatusMap;
        static ACE_Recursive_Thread_Mutex  theThrMutex;
};
#endif 


/*=================================================================== */
/**
    @class              ACS_NSF_Server

    @brief              This class implements the NSF Network Surveillance
**/
 /*=================================================================== */

class ACS_NSF_Server:public ACE_Task<ACE_SYNCH,ACE_Monotonic_Time_Policy>
{
/*=============================================================================
            PUBLIC DECLARATION SECTION
  =============================================================================*/

   public:

/*=====================================================================
                                CLASS CONSTRUCTORS
 ==================================================================== */

/*=================================================================== */

		/**

				@brief                  Server constructor

				@pre

				@post

				@return                 none

				@exception              none
		*/
		/*=================================================================== */
		ACS_NSF_Server();
		
		/**

                                @brief                  Server parameterized constructor

                                @pre

                                @post

                                @return                 none

                                @exception              none
                */
                /*=================================================================== */

		ACS_NSF_Server(ACS_NSF_Service* nsfServicePtr);
		/*=================================================================== */



public:
		/*===================================================================
						CLASS DESTRUCTOR
		=================================================================== */
		/*=================================================================== */
		/**

				@brief                  Destructor

				@pre                    none

				@post                   none

				@return                 none

				@exception              none
		*/
		/*=================================================================== */
		~ACS_NSF_Server();
		/*=================================================================== */

public:
		
		

		/*=============================================================================
						PUBLIC METHODS
		=============================================================================*/
		int active(ACS_NSF_Service* haObj);
		int passive(ACS_NSF_Service* haObj);
		void stop();
		
		/*=================================================================== */
		/**
				@brief		GetPhaParameters

				@return		ACE_INT32
		*/
		/*=================================================================== */
		static ACE_INT32 GetPhaParameters();
		/*=================================================================== */
		/**
				@brief	GetConfigParameters

				@return	ACE_INT32
		*/
		/*=================================================================== */
		ACE_INT32 GetConfigParameters();
		/*=================================================================== */
		/*=================================================================== */
		/**
				@brief	sendUpdatedParamsToPassiveNode

				@return	ACS_CC_ReturnType
		*/
		/*=================================================================== */
		ACS_CC_ReturnType sendUpdatedParamsToPassiveNode();
		/*=================================================================== */
		/*=================================================================== */
		/**
				@brief                  To Start the Network Surveillance Server

				@pre                    none

				@post                   none

				@return                 void

				@exception              none
		*/
		/*=================================================================== */
		void StartServer();
		/*=================================================================== */

		/**
				@brief					setupIMMCallBacks

				@return					void
		*/
		/*=================================================================== */
		void setupIMMCallBacks( );
		/*=================================================================== */

		/**
				@brief                  To Stop the Network Surveillance Server

				@pre                    none

				@post                   none

				@return                 void

				@exception              none
		*/
		/*=================================================================== */
		void StopServer();
		/*=================================================================== */
		/**
				@brief                  The "main"-routine. The following actions are done:
							Reads parameters from IMM.Decides which node	(A or B) Reads IP addresses of gateways used for external communication. Sets initial values in cluster registry keys (if not already done) Cease all alarms Start supervision of each gateway Enters an eternal loop which supervises all external networks

				@pre                    none

				@post                   none

				@return                 bool

				@exception              none
		*/
		/*=================================================================== */
		bool NetworkSurveillance();
		
		/**
				@brief                  The "main"-routine for cable-less. The following actions are done:
							Reads parameters from IMM.Decides which node	(A or B) Reads IP addresses of gateways used for external communication. Sets initial values in cluster registry keys (if not already done) Cease all alarms Start supervision of each gateway Enters an eternal loop which supervises all external networks

				@pre                    none

				@post                   none

				@return                 bool

				@exception              none
		*/
		/*=================================================================== */
		bool NetworkSurveillanceForCableLess();
		
		/*=================================================================== */
		/**
				@brief                  This routine gets the time of the other node

				@pre                    none

				@post                   none

				@return                 Return status
										0:  Success
										-1: Failure

				@exception              none
		*/
		/*=================================================================== */
		ACE_UINT32 PingThread();
		/*=================================================================== */

		/**
				@brief       isNSFServerStopped
		*/

		/*=================================================================== */
		bool isNSFServerStopped;
		/*=================================================================== */

		/**
				@brief	NSFFunctionalMethod

				@param aNSFPtr

				@return
		*/

		/*=====================================================================*/
		static ACE_THR_FUNC_RETURN NSFFunctionalMethod(void* aNSFPtr);
		/*=====================================================================*/

		/**
				@brief	StartOIThreadsOnActiveNode
		
				@return	ACS_CC_ReturnType
	
		*/
		/*=====================================================================*/
		ACS_CC_ReturnType StartOIThreadsOnActiveNode();
		/*=====================================================================*/

		/**
				@brief       setupIMMCallBacksThreadFunc

				@param		 aNSFPtr

				@return      ACE_THR_FUNC_RETURN
		*/
		/*=====================================================================*/
		static ACE_THR_FUNC_RETURN setupIMMCallBacksThreadFunc(void * aNSFPtr);
		/*=====================================================================*/

		/**
			@brief	setupNSFThread
	
			@param aNSFPtr

			@return	ACS_CC_ReturnType
		*/
		/*=====================================================================*/
		static ACS_CC_ReturnType setupNSFThread(ACS_NSF_Server * aNSFPtr);
		/*=====================================================================*/

		/**
			@brief	SetDefaultNetworkSurveillanceValue

			@param aSurveillanceActiveFlag

			@return ACS_UINT32
		*/
		/*=====================================================================*/
		ACE_UINT32 SetDefaultNetworkSurveillanceValue(ACE_UINT32 aSurveillanceActiveFlag);
		/*=====================================================================*/

		/**
			@brief	SetDefaultFailoverCount

			@return	ACS_UINT32
		*/
		/*=====================================================================*/
		ACE_UINT32 SetDefaultFailoverCount();
		/*=====================================================================*/

		/**
			@brief	SetDefaultLastFailoverTime

			@return ACE_UINT32
		*/
		/*=====================================================================*/
		ACE_UINT32 SetDefaultLastFailoverTime();
		/*=====================================================================*/

		/**
			@brief	SetDefaultAlarmState

			@param aCurrentNode

			@return	ACE_UINT32
		*/
		/*=====================================================================*/
		ACE_UINT32 SetDefaultAlarmState(NODE_NAME aCurrentNode );
		/*=====================================================================*/

		/**
			@brief	objCommon
		*/
		/*=====================================================================*/
		static ACS_NSF_Common objCommon;
		/*=====================================================================*/

		/**
			@brief	NSFFunctionalThreadId
		*/
		/*=====================================================================*/
		ACE_thread_t NSFFunctionalThreadId;
		/*=====================================================================*/

		/**
			@brief	m_threadGroupId
		*/
		/*=====================================================================*/
		ACE_INT32 m_threadGroupId;
		/*=====================================================================*/		


	/*=============================================================================
			PRIVATE DECLARATION SECTION
	=============================================================================*/
private:
		OmHandler theNSFServerOmHandler;

	/*=============================================================================
			PRIVATE METHODS 
	=============================================================================*/

	/*=================================================================== */
	/**
		@brief                  This routine writes the current system time in
					Timestamp file in Unix epoch format.
					The time in the timestamp file will be used by
					Biostime Recovery functionality to calculated the POWERLOSS.

		@pre                    none

		@post                   none

		@return                 Return Status
					0:	Success
					-1:	Failure

		@exception              none
	*/
	/*=================================================================== */
	ACE_INT32 SaveSystemTime();
	/*=================================================================== */

	/**
		@brief                  This routine reads the IP addresses of the default
					gateways specified for the public network adapter

		@pre                    none

		@post                   none

		@return                 Return Status
					0:	Success
					-1:	Failure

		@exception              none
	*/
	/*=================================================================== */
	ACE_INT32 GetDefaultGateways();
	/*=================================================================== */


	/**
		@brief                  This routine reads the IP addresses of the default
					gateways specified for the public network 2 adapter

		@pre                    none

		@post                   none

		@return                 Return Status
					0:	Success
					-1:	Failure

		@exception              none
	*/
	/*=================================================================== */
	ACE_INT32 GetDefaultGateways2();
	/*=================================================================== */

	/*=================================================================== */
	/**
		@brief                  This routine is used to set the default NSF IMM attributes

		@pre                    none

		@post                   none
		
		@param                  aSurveillanceActiveFlag

		@return                 Return status
					0:      OK
					-1:     Error detected and reported				

		@exception              none
	*/
	/*=================================================================== */
	ACE_INT32 SetDefaultIMMParams(ACE_UINT32 aSurveillanceActiveFlag);
	/*=================================================================== */
	/**
		@brief                  This routine is used to cease all alarms.
					It is called when the service terminates, before failover
					or at startup. This is to avoid hanging alarms. The alarms
					will be raised again if the errors still exists after reboot.
		@pre                    none

		@post                   none

		@param                  state
					0:      NSF Deactivated
					2,3:    NSF Activated

		@return                 Return status
					0: Ok, all alarms cleared.
					-1:     Error detected and reported

		@exception              none
	*/
	/*=================================================================== */
	int CeaseAllAlarms(ACE_UINT32 state);
	/*=================================================================== */
	/**
		@brief                  This routine is used to cease all alarms.
					It is called when the service terminates, before failover
					or at startup. This is to avoid hanging alarms. The alarms
					will be raised again if the errors still exists after reboot.
		@pre                    none

		@post                   none

		@param                  state

		@return                 Return status
					0: Ok, all alarms cleared.
					-1:     Error detected and reported
		@exception              none
	*/
	/*=================================================================== */
	int CeaseAllAlarms2(ACE_UINT32 state);
	/*=================================================================== */

	/*=================================================================== */

	/**
		@brief                  This routine is used to terminated supervision of
					gateways. It is called when the service is shut-down
					or before failover.

		@pre                    none

		@post                   none

		@param                  forceTermination
					1:  Forced termination
					2:  Wait for the thread to finish
		@return                 void

		@exception              none
	*/
	/*=================================================================== */
	void TerminateGatewaySup(int forceTermination);
	/*=================================================================== */

	/**
		@brief                  This routine creates the pipe, thread and event handles

		@pre                    none

		@post                   none

		@return                 bool
					true:   If pipe, thread and event handles are properly created
					false:  If handles are not created

		@exception              none
	*/
	/*=================================================================== */
	bool StartPingServer();
	/*=================================================================== */

	/**
		@brief                  This routine sets the heartbeat.
					Checks whether it is time to reset the failover count.
					Sends O2 alarm or ceasing
					Checks whether NSF is running on the other node.

		@pre                    none

		@post                   none

		@return                 Return status:
					0:      NSF is not running on the other node.
					1:      NSF is running on the other node.

		@exception              none
	*/
	/*=================================================================== */
	int VerifyHeartBeat();
	/*=================================================================== */

	/**
		@brief                  This routine counts number of blocked gateways.

		@pre                    none

		@post                   none

		@param                  alarmStatus
					A bitmask. B3-> is used to indicated gw 0->
		@param                  noGW

		@return                 noOfGW
					Returns the number of blocked gateways.

		@exception              none
	*/
	/*=================================================================== */
	ACE_UINT32 CountBlockedGw(ACE_UINT32 alarmStatus, ACE_UINT32 noGW);
	/*=================================================================== */

	/**
		@brief                  This routine reboots the node. When used on the active
					node, this will cause a failover.
					The routine will never return.

		@pre                    none

		@post                   none

		@return                 void

		@exception              none
	*/
	/*=================================================================== */
	void DoFailover();
	/*=================================================================== */

	/**
		@brief                  This routine gets the value from IMM database.

		@pre                    none

		@post                   none

		@param                  str
					NSF Configuration parameter.
		@return                 Value
					Returns the valued of the configuration parameter.

		@exception              none
	*/
	/*=================================================================== */
	int GetValueFromIMM(string str);
	/*=================================================================== */

	/*=================================================================== */

	/**
		@brief		TryToFailoverForPS
	
		@param		activeAlarm

		@param		activeAlarm2
	
		@return		void
	*/
	/*=================================================================== */
	void TryToFailoverForPS( ACE_UINT32 activeAlarm,
					ACE_UINT32 activeAlarm2);
	/*=================================================================== */

	/**
		@brief		CheckIfFailoverForPS

		@return		bool
	*/
	/*=================================================================== */
	bool CheckIfFailoverForPS();
	/*=================================================================== */

	/**
		@brief	error

		@param 	msg

		@return	void
	*/
	/*=================================================================== */
	void error(const char *msg);
	/*=================================================================== */

private:

/*=============================================================================
		PRIVATE ATTRIBUTES 
=============================================================================*/
/**
* @brief	NSFEventList
*/
eventStruct* NSFEventList;

/**
* @brief	sGatewayVector
*/
vector <string> sGatewayVector;

/**
* @brief	sGatewayVector2
*/
vector <string> sGatewayVector2;

/**
* @brief	gwInfo
*/
struct gwStruct gwInfo[10];

/**
* @brief	gwInfo2[10];
*/
struct gwStruct gwInfo2[10];

/**
* @brief	noOfGW
*/
ACE_UINT32 noOfGW;

/**
* @brief	noOfGW2
*/
ACE_UINT32 noOfGW2;

/**
* @brief	m_poCommandHandler
*/
ACS_NSF_CommandHandler* m_poCommandHandler;

/**
* @brief       m_haObj 
*/

//ACS_NSF_Service *m_nsfService;
ACS_NSF_Service *m_haObj;

/**
* @brief       Pointer to  ACE_Thread_Manager Object
*/

ACE_Thread_Manager *threadManager_;

/**
* @brief	Cute_ACS_NSF_Server
*/
friend class Cute_ACS_NSF_Server;
};


/*=================================================================== */
/**
	@brief   This routine is used to supervise one gateway.
            This method executes in its own thread to avoid
            blocking supervision of other gateways.
            The method returns only when the service is shut
            down or when a serious error is detected.

	@pre                    none

	@post                   none

	@param                  value
				Index of the gateway

	@return                 void

	@exception              none
*/
/*=================================================================== */
void Supervise(void* value);
/*=================================================================== */

/*=================================================================== */
/**
	@brief   This routine is used to supervise the bond1 interface status in cable less environment.
            This method executes in its own thread to avoid
            blocking the other functions.
            The method returns only when the service is shut
            down or when a serious error is detected.

	@pre                    none

	@post                   none

	@param                  value
            stop event 
				

	@return                 void

	@exception              none
*/
/*=================================================================== */
void SuperviseForCableLess(void* value);
/*=================================================================== */


#endif /* _ACS_NSF_Server_H_ */




