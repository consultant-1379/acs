#ifndef _ACS_EMF_CMDCLIENT_H_
#define _ACS_EMF_CMDCLIENT_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include "acs_emf_common.h"
#include "acs_emf_cmd.h"
#include <ace/ACE.h>
#include <ace/Event.h>
#include "acs_emf_tra.h"

#include <ACS_DSD_Server.h>
#include <ACS_DSD_Client.h>
#include <ACS_DSD_MacrosConstants.h>
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
#define EMF_TIMEOUT_ON_RECEIVE  10000  //select() should block waiting up to 10 sec

class ACS_EMF_CmdClient
{
	/*=====================================================================
						 PUBLIC DECLARATION SECTION
	 ==================================================================== */
public:
	/*===================================================================
	   							   PUBLIC METHOD
	   	=================================================================== */
	/*=================================================================== */
	/**
				@brief       Default constructor for ACS_EMF_CmdClient

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	ACS_EMF_CmdClient(ACE_HANDLE stophandle);
	/*=================================================================== */
	/**
				@brief       Default destructor for ACS_EMF_CmdClient

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	~ACS_EMF_CmdClient();
	//*=================================================================== */
	/**
				@brief      connect		:Method used to connect to the server

				@return 	true/false              :bool
	 */
	/*=================================================================== */
	bool connect(int32_t iSystemId, acs_dsd::NodeStateConstants enmNodeState);
	
	bool connect();
	//*=================================================================== */
	/**
				@brief      send			:Method used to send data to the client

				@param 		cmd  			: const ACS_EMF_Cmd

				@return 	true/false      :bool
	 */
	/*=================================================================== */

	bool send(const ACS_EMF_Cmd& cmd);
	//*=================================================================== */
	/**
				@brief      receive			:Method used to recieve data to the server

				@param 		cmd  			: const ACS_EMF_Cmd

				@return 	true/false      :bool
	 */
	/*=================================================================== */

	bool receive(ACS_EMF_Cmd& cmd, unsigned int timeout_ms = 0);
	//*=================================================================== */
	/**
				@brief      disconnect			:Method used to disconnect the connection

				@return 	true/false      :bool
	 */
	/*=================================================================== */

	bool disconnect();
        //*=================================================================== */
        /**
                                @brief      queryApFromDSD               :Method used to get reachable and unreachable nodes

				@param          pVctSortReachable        : vector ACS_DSD_Node	

                                @return         true/false      :bool
         */
        /*=================================================================== */

	bool queryApFromDSD(std::vector<ACS_DSD_Node> *pVctSortReachable);
    //*=================================================================== */
      /**
                              @brief      queryApFromDSD               :Method used to get reachable and unreachable nodes

				@param          pVctSortReachable        : vector ACS_DSD_Node

                              @return         true/false      :bool
       */
      /*=================================================================== */

	bool queryApFromDSD();

        //*=================================================================== */
        /**
                                @brief      invokeEmfMethod               

                                @param          iCommand                 :int

				@param          iSystemId                :int32_t
							
				@param          enmNodeState		 :acs_dsd::NodeStateConstants 
			
				@param          penmNodeSide             :acs_dsd::NodeStateConstants    

                                @return         ACE_INT16
         */
        /*=================================================================== */


	ACE_INT16 invokeEmfMethod(int iCommand, int32_t iSystemId, acs_dsd::NodeStateConstants enmNodeState, acs_dsd::NodeSideConstants *penmNodeSide);

	//*=================================================================== */
      /**
                              @brief      send_receive

                @param          cmdSend             :ACS_EMF_Cmd

				@param          iSystemId           :int32_t

				@param          enmNodeState		:acs_dsd::NodeStateConstants

				@param          penmNodeSide        :acs_dsd::NodeStateConstants
				@param			cmdRecv  			:ACS_EMF_Cmd

                @return         ACE_INT16
       */
      /*=================================================================== */
	ACE_INT16 send_receive(const ACS_EMF_Cmd & cmdSend, int32_t iSystemId, acs_dsd::NodeStateConstants enmNodeState, ACS_EMF_Cmd & cmdRecv);
	//*=================================================================== */
        /**
                                @brief      closeConnection                  

                                @return         none
         */
        /*=================================================================== */

	void closeConnection();
        /*=================================================================== */
        /**
        @brief  clientConnectEvent
         */
        /*=================================================================== */

	ACE_Event * clientConnectEvent;
private:
	/*===================================================================
								 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
								 PRIVATE METHOD
	=================================================================== */

        /*=================================================================== */
        /**
        @brief  theEMFCmdClient
         */
        /*=================================================================== */

	ACS_DSD_Client* theEMFCmdClient;
        /*=================================================================== */
        /**
        @brief theEMFCmdSession
         */
        /*=================================================================== */

	ACS_DSD_Session* theEMFCmdSession;
        /*=================================================================== */
        /**
        @brief static m_stopEventHandle
         */
        /*=================================================================== */

	static ACE_HANDLE m_stopEventHandle;


private: //Copy constructor and '=' operator are disabled
	/*===================================================================
								 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
								 PRIVATE METHOD
	=================================================================== */

        //*=================================================================== */
        /**
                                @brief      sortReachableNode        

                                @param         pReachable        : vector ACS_DSD_Node

				@param        pSortReachable     : vector ACS_DSD_Node

                                @return        none
         */
        /*=================================================================== */

	void sortReachableNode(std::vector<ACS_DSD_Node> *pReachable, std::vector<ACS_DSD_Node> *pSortReachable);

        //*=================================================================== */
        /**
                                @brief       getNextNode

                                @param        objReachable          : vector ACS_DSD_Node

                                @param        pNode                 :ACS_DSD_Node

                                @return       true/false
         */
        /*=================================================================== */

	void getNextNode(const std::vector<ACS_DSD_Node> &objReachable, ACS_DSD_Node *pNode);

        //*=================================================================== */
        /**
                                @brief       fillNodeAndPartner

                                @param        pReachable          : vector ACS_DSD_Node

                                @param        pSortReachable      : vector ACS_DSD_Node

				@param	      objDsdNode	  :const ACS_DSD_Node

                                @return       true/false
         */
        /*=================================================================== */
 
	bool fillNodeAndPartner(std::vector<ACS_DSD_Node> *pReachable, const ACS_DSD_Node &objDsdNode, std::vector<ACS_DSD_Node> *pSortReachable);
        //*=================================================================== */
        /**
                                @brief       connectToNode    

                                @param       iSystemId          : int32_t

                                @param       enmNodeState       : acs_dsd::NodeStateConstants

                                @param       penmNodeSide       :acs_dsd::NodeSideConstants

                                @return       true/false
         */
        /*=================================================================== */

	bool connectToNode(int32_t iSystemId, acs_dsd::NodeStateConstants enmNodeState, acs_dsd::NodeSideConstants *penmNodeSide);
	//*=================================================================== */
	/**
				@brief      copy constructor
	 */
	/*=================================================================== */
	ACS_EMF_CmdClient(const ACS_EMF_CmdClient&);
	//*=================================================================== */
		/**
					@brief      operator
		 */
		/*=================================================================== */
	void operator=(const ACS_EMF_CmdClient&);
};
#endif

