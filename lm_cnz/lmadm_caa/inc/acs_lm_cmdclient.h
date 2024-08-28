#ifndef _ACS_LM_CMDCLIENT_H_
#define _ACS_LM_CMDCLIENT_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include "acs_lm_common.h"
#include "acs_lm_cmd.h"
#include <ace/ACE.h>
#include <ace/Event.h>
#include "acs_lm_tra.h"

#include <ACS_DSD_Server.h>
#include <ACS_DSD_Client.h>
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_CmdClient
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
				@brief       Default constructor for ACS_LM_CmdClient

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	ACS_LM_CmdClient();
	/*=================================================================== */
	/**
				@brief       Default destructor for ACS_LM_CmdClient

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	~ACS_LM_CmdClient();
	//*=================================================================== */
	/**
				@brief      connect		:Method used to connect to the server

				@return 	true/false              :bool
	 */
	/*=================================================================== */
	bool connect();//const std::string& address, const std::string& serviceName);

	//*=================================================================== */
	/**
				@brief      send			:Method used to send data to the client

				@param 		cmd  			: const ACS_LM_Cmd

				@return 	true/false      :bool
	 */
	/*=================================================================== */

	bool send(const ACS_LM_Cmd& cmd);
	//*=================================================================== */
	/**
				@brief      receive			:Method used to recieve data to the server

				@param 		cmd  			: const ACS_LM_Cmd

				@return 	true/false      :bool
	 */
	/*=================================================================== */

	bool receive(ACS_LM_Cmd& cmd);
	//*=================================================================== */
	/**
				@brief      send			:Method used to disconnect the connection

				@return 	true/false      :bool
	 */
	/*=================================================================== */

	bool disconnect();

private:
	/*===================================================================
								 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
								 PRIVATE METHOD
	=================================================================== */
	ACS_DSD_Client* theLMCmdClient;
	ACS_DSD_Session* theLMCmdSession;


private: //Copy constructor and '=' operator are disabled
	/*===================================================================
								 PRIVATE ATTRIBUTE
	=================================================================== */

	/*===================================================================
								 PRIVATE METHOD
	=================================================================== */
	//*=================================================================== */
	/**
				@brief      copy constructor
	 */
	/*=================================================================== */
	ACS_LM_CmdClient(const ACS_LM_CmdClient&);
	//*=================================================================== */
		/**
					@brief      operator
		 */
		/*=================================================================== */
	void operator=(const ACS_LM_CmdClient&);
};
#endif
