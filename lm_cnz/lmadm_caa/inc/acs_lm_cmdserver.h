#ifndef _ACS_LM_CMDSERVER_H_
#define _ACS_LM_CMDSERVER_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include "acs_lm_common.h"
#include "acs_lm_cmd.h"
#include <ace/ACE.h>
#include <ace/Event.h>
#include <ACS_DSD_Server.h>
#include <ACS_DSD_Client.h>
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_LM_CmdServer
{
	/*=====================================================================
						 PUBLIC DECLARATION SECTION
	 ==================================================================== */
public:
	/*===================================================================
	   						  PUBLIC ATTRIBUTE
	   	=================================================================== */

	/*===================================================================
	   							   PUBLIC METHOD
	   	=================================================================== */
	/*=================================================================== */
	/**
					@brief       Default constructor for ACS_LM_CmdServer

					@par         None

					@pre         None

					@post        None

					@exception   None
	 */
	/*=================================================================== */
	ACS_LM_CmdServer();
	/*=================================================================== */
	/**
					@brief       Default destructor for ACS_LM_CmdServer

					@par         None

					@pre         None

					@post        None

					@exception   None
	 */
	/*=================================================================== */
	~ACS_LM_CmdServer();
	//*=================================================================== */
	/**
					@brief      listen		:Method used to wait for the client
												 to accept the connection

					@return 	true/false              :bool
	 */
	/*=================================================================== */
	bool listen();
	//*=================================================================== */
	/**
					@brief      accept		:Method used to accept the connection

					@param      stopCmdThreadFds					
					
					@return     ACE_INT32
	 */
	/*=================================================================== */
	 //ACE_INT32 accept(ACE_HANDLE stopCmdThreadFds);
	ACE_INT32 accept();
	//*=================================================================== */
	/**
					@brief      send			:Method used to recieve data to the server

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
					@brief      close			:Method used to close the open session


					@return 	true/false      :bool
	 */
	/*=================================================================== */

	bool close();
	/*=================================================================== */
	/**
	 *  @brief setDSDServerStopSignal					:

	 *  @param myStatus to be set in theDSDServerStopSignal attribute
	 **/
	/*=================================================================== */
	void setDSDServerStopSignal(bool myStatus);

private:
	/*===================================================================
							 PRIVATE ATTRIBUTE
	 =================================================================== */

	/*===================================================================
							 PRIVATE METHOD
	 =================================================================== */
	//	bool initPipeSecurity(PSECURITY_DESCRIPTOR pSecDes, PACL pAcl);
	//	bool write(const ACE_TCHAR* buffer, const ACE_UINT64 bytesToWrite, ACE_UINT64& bytesWritten);
	//	bool read(const ACE_TCHAR* buffer, const ACE_UINT64 bytesToRead, ACE_UINT64& bytesRead);

	// ACE_HANDLE pipeHandle;
	ACE_Event * clientConnectEvent;
	ACS_DSD_Server *theLMCmdServer;
	ACS_DSD_Session* theDSDSession;
	static bool theDSDServerStopSignal;
	static ACE_Recursive_Thread_Mutex theDSDServerMutex;

	//bool forceClose;

private: //Copy constructor and '=' operator are disabled
	//*=================================================================== */
	/**
				@brief      copy constructor
	 */
	/*=================================================================== */
	ACS_LM_CmdServer(const ACS_LM_CmdServer&);
	//*=================================================================== */
	/**
						@brief      operator
	 */
	/*=================================================================== */
	void operator=(const ACS_LM_CmdServer&);
	/*=================================================================== */
	/**
	 *  @brief getDSDServerStopSignal					:

	 *  @return Value of theDSDServerStopSignal attribute
	 **/
	/*=================================================================== */
	bool getDSDServerStopSignal();

};
#endif
