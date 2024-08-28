#ifndef _ACS_EMF_CMDSERVER_H_
#define _ACS_EMF_CMDSERVER_H_
/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include "acs_emf_common.h"
#include "acs_emf_cmd.h"
#include <ace/ACE.h>
#include <ace/Event.h>
#include <ACS_DSD_Server.h>
#include <ACS_DSD_Client.h>

#define INT_PIPE_BUFFERSIZE			(int)2048
#define DSDSERVER_LOCK_MEDIA				"L+"
#define DSDSERVER_UMOUNTONLY_MEDIA	"L"

#define LOGICALMOUNT_MODE  "LM"   // it is used for locking the media without mount operation
#define DSDSERVER_REMOVE_MEDIA_OWN_FILE	"RM" //it is used to remove the temporty file created to hold dvd owner information
#define SYNC_NFS_REMOVE "SN" //it is used to control unmount operation after sync media
#define DSD_ACTIVE_NODE "AN" //it is used to communicate with DVD attached node
#define DSD_PASSIVE_NODE "PN" //it is used to communicate with DVD attached node

// Error define
#define CMDSRV_ERROR							-1
#define CMDSRV_OK								0
#define CMDSRV_STOPSIGNAL						1
// Recive timeout
#define CMDSRV_RECIVE_TOUTMS				5000
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_EMF_CmdServer
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
					@brief       Default constructor for ACS_EMF_CmdServer

					@par         None

					@pre         None

					@post        None

					@exception   None
	 */
	/*=================================================================== */
	ACS_EMF_CmdServer();
	/*=================================================================== */
	/**
					@brief       Default destructor for ACS_EMF_CmdServer

					@par         None

					@pre         None

					@post        None

					@exception   None
	 */
	/*=================================================================== */
	~ACS_EMF_CmdServer();
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
	 ACE_INT32 accept(ACE_HANDLE stopCmdThreadFds);
	//*=================================================================== */
	/**
					@brief      send			:Method used to recieve data to the server

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

	bool receive(ACS_EMF_Cmd& cmd);
	//*=================================================================== */
	/**
					@brief      close			:Method used to close the open session


					@return 	true/false      :bool
	 */
	/*=================================================================== */

	bool close();

        //*=================================================================== */
        /**
                                        @brief      get_remote_node              :Methos used to get remote node info

                                        @param          node                     : ACS_DSD_Node

                                        @return         int 
         */
        /*=================================================================== */

	int get_remote_node (ACS_DSD_Node & node) const;

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
        //   ACE_HANDLE               m_hServerEvent;
        /*=================================================================== */
        /**
        @brief   clientConnectEvent
         */
        /*=================================================================== */

	ACE_Event * clientConnectEvent;
        /*=================================================================== */
        /**
        @brief   theEMFCmdServer
         */
        /*=================================================================== */

	ACS_DSD_Server *theEMFCmdServer;
        /*=================================================================== */
        /**
        @brief   theDSDSession
         */
        /*=================================================================== */

	ACS_DSD_Session* theDSDSession;
        /*=================================================================== */
        /**
        @brief   forceClose
         */
        /*=================================================================== */

	bool forceClose;

private: //Copy constructor and '=' operator are disabled
	//*=================================================================== */
	/**
				@brief      copy constructor
	 */
	/*=================================================================== */
	ACS_EMF_CmdServer(const ACS_EMF_CmdServer&);
	//*=================================================================== */
	/**
						@brief      operator
	 */
	/*=================================================================== */
	void operator=(const ACS_EMF_CmdServer&);
};
#endif

