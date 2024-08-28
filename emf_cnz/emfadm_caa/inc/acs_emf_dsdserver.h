//******************************************************************************
//
//  NAME
//     ACS_EMF_DSDSERVER.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2008. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//          190 89-CAA nnn nnnn
//
//  AUTHOR
//     2008-11-28 by XRAMMAT PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
#ifndef _ACS_EMF_DSDSERVER_H_
#define _ACS_EMF_DSDSERVER_H_

/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
//Include file for Sentinel RMS
#include <iostream>
#include <ace/ACE.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/OS_NS_dlfcn.h>
#include <ace/Reactor.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ace/Signal.h>
#include <ace/Get_Opt.h>
#include <ace/streams.h>
#include <ace/OS_NS_errno.h>
#include <ace/SString.h>
#include <ace/OS_NS_sys_wait.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_sys_stat.h>
#include <ace/OS_NS_sys_resource.h>
#include <ace/OS_NS_fcntl.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>

#include "acs_emf_cmdserver.h"
#include "acs_emf_dvdhandler.h"
#include "acs_emf_commandhandler.h"

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class ACS_EMF_DSDServer: public ACE_Task_Base
{
public :
	// Constants used as return codes
	enum  EMFServer_constant
		{
			EMFSERVER_OK = 0,
			EMFSERVER_ERROR = 1,
			EMFSERVER_MEDIAMOUNTEDONOTHERDIR = 2,
			EMFSERVER_MEDIANOTPRESENT = 3,
			EMFSERVER_MEDIANOTCONFIGURED = 4,
			EMFSERVER_MEDIABUSY = 5,
			EMFSERVER_MEDIANOTMOUNTED = 6,
			EMFSERVER_MEDIAUNMOUNTERROR = 7,
			EMFSERVER_MEDIAFORMATERROR = 8,
			EMFSERVER_MEDIAMOUNTERROR  = 9,
			EMFSERVER_MEDIAALREADYENABLED = 10
		};
	/*===================================================================
			   				 PUBLIC ATTRIBUTE
	=================================================================== */

	/*===================================================================
			   				PUBLIC METHOD
	=================================================================== */
	/*=================================================================== */
	/**
				@brief       Default constructor for ACS_EMF_DSDServer

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	ACS_EMF_DSDServer(ACE_HANDLE stopThreadFds);

	/*=================================================================== */
	/**
				@brief       Default destructor for ACS_EMF_DSDServer

				@par         None

				@pre         None

				@post        None

				@exception   None
	 */
	/*=================================================================== */
	~ACS_EMF_DSDServer();
	/*=================================================================== */
	/**
	 *  @brief start
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void start();
	/*=================================================================== */
	/**
	 *  @brief stop
	 *
	 *  @return void
	 **/
	/*=================================================================== */
	void stop();

	//svc method
	/*=================================================================== */
	/**
	 *  @brief svc
	 *
	 *  @return int
	 **/
	/*=================================================================== */
	int svc(void);
	/*=================================================================== */
	/**
	 *  @brief setClienthandler
	 *
	 *  @param aClienthandler      			: ACS_LM_ClientHandler
	 *
	 *  @return void
	 **/
	/*=================================================================== */

	void setClienthandler(ACS_EMF_MEDIAHandler* aClienthandler);

	/*=================================================================== */
	/**
	 *  @brief setCommandhandler
	 *
	 *  @param aCmdhandler      			: acs_emf_commandhandler
	 *
	 *  @return void
	 **/
	/*=================================================================== */

	void setCommandhandler(acs_emf_commandhandler* aCmdhandler);
        /*=================================================================== */
        /**
         *  @brief setOperationalStateinPassive
         *
         *  @param 		                        : ACE_INT32
         *
         *  @return void
         **/
        /*=================================================================== */

	static void setOperationalStateinPassive(ACE_INT32);
        /*=================================================================== */
        /**
         *  @brief getOperationalStateinPassive
         *
         *  @return ACE_INT32 
         **/
        /*=================================================================== */

	static ACE_INT32 getOperationalStateinPassive();
        /*=================================================================== */
        /**
         *  @brief  theOperationalStateinPassive
         **/
        /*=================================================================== */

	static ACE_INT32 theOperationalStateinPassive;
        /*=================================================================== */
        /**
         *  @brief  theMediaOwnerinPassive
         **/
        /*=================================================================== */

	static ACE_UINT16 theMediaOwnerinPassive;
	bool isDSDPublished;
	/*=================================================================== */

private:
	/*===================================================================
							 PRIVATE ATTRIBUTE
	 =================================================================== */

	/*===================================================================
							 PRIVATE METHOD
	 =================================================================== */
        /*=================================================================== */
        /**
         *  @brief serverAccept				      	
         *
         *  @param  cmdServer                                 : ACS_EMF_CmdServer
         *
         *  @return int
         **/
        /*=================================================================== */

	int serverAccept(ACS_EMF_CmdServer &cmdServer);
        /*=================================================================== */
        /**
         *  @brief executeCommand			     :This method is used to execute commands	
         *
         *  @param  iCmdCode                                 : int
	 *	
         *  @param  <list>lstStrArgs                         : const std::string
	 *         
	 *  @param  remoteNodeInfo                           : ACS_DSD_Node
	 *
	 *  @param  <list>lstStrArgsOut                      : const std::string    		
	 *
	 *  @return ACE_INT16
         **/
        /*=================================================================== */


	ACE_INT16 executeCommand(int iCmdCode, const std::list<string> &lstStrArgs, const ACS_DSD_Node &remoteNodeInfo, std::list<string> &lstStrArgsOut);
        /*=================================================================== */
        /**
         *  @brief parserRequest
         *
         *  @param  remoteNodeInfo                                 : const ACS_DSD_Node
         *
         *  @return bool
         **/
        /*=================================================================== */

	bool parserRequest(const ACS_DSD_Node &remoteNodeInfo);
	// Server function
        /*=================================================================== */
        /**
         *  @brief serverCheckForDVDPresence                      :This method checks for DVD presence.
         *
         *  @param  remoteNodeInfo                                 : const ACS_DSD_Node
         *
         *  @return ACE_INT16
         **/
        /*=================================================================== */

	ACE_INT16 serverCheckForDVDPresence(const ACS_DSD_Node &remoteNodeInfo);
        /*=================================================================== */
        /**
         *  @brief serverCheckForDVDMediaPresence                  :This method checks for DVD presence.
         *
         *  @param  <list>lstStrArgsOut                            :std::string 
         *
         *  @return ACE_INT16
         **/
        /*=================================================================== */

	ACE_INT16 serverCheckForDVDMediaPresence(std::list<string> &lstStrArgsOut);
        /*=================================================================== */
        /**
         *  @brief serverCopyToDVD                           :This method is used to export files to passive node.
         *
         *  @param  remoteNodeInfo                           :const ACS_DSD_Node
         *
         *  @param  <list>lstStrArgs                         : const std::string
         *
         *  @param  <list>lstStrArgsOut                      : const std::string
         *
         *  @return ACE_INT16
         **/
        /*=================================================================== */

	ACE_INT16 serverCopyToDVD(const ACS_DSD_Node &remoteNodeInfo, const std::list<string> &lstStrArgs, std::list<string> &lstStrArgsOut, bool format);
        /*=================================================================== */
        /**
         *  @brief serverEraseAndCopyToDVD                   :This method is used to format & export files to passive node.
         *
         *  @param  remoteNodeInfo                           :const ACS_DSD_Node
         *
         *  @param  <list>lstStrArgs                         : const std::string
         *
         *  @param  <list>lstStrArgsOut                      : const std::string
         *
         *  @return ACE_INT16
         **/
        /*=================================================================== */

	ACE_INT16 serverEraseAndCopyToDVD(const ACS_DSD_Node &remoteNodeInfo, const std::list<string> &lstStrArgs, std::list<string> &lstStrArgsOut);
        /*=================================================================== */
        /**
         *  @brief serverFetchDVDMediaInfo                   :This method is used to fetch DVD info
         *
         *  @param  remoteNodeInfo                           :const ACS_DSD_Node
         *
	 *  @param  <list>lstStrArgsOut                      : const std::string	
         *
	 *  @return ACE_INT16
         **/
        /*=================================================================== */

	ACE_INT16 serverFetchDVDMediaInfo(const ACS_DSD_Node &remoteNodeInfo, std::list<string> &lstStrArgsOut);
        /*=================================================================== */
        /**
         *  @brief  serverMountMediaOnDVDOwner               :This method is used to Setup NFS share on passive node
         *
         *  @param  remoteNodeInfo                           :const ACS_DSD_Node
         *
         *  @return ACE_INT16
         **/
        /*=================================================================== */

	ACE_INT16 serverMountMediaOnDVDOwner(const ACS_DSD_Node &remoteNodeInfo, const std::list<string> &lstStrArgs);
	/*=================================================================== */
        /**
         *  @brief  serverMountMediaOnAccessibleNode               :This method is used to mount media on active node
         *
         *  @param  remoteNodeInfo                           :const ACS_DSD_Node
         *
         *  @return ACE_INT16
         **/
        /*=================================================================== */

	ACE_INT16 serverMountMediaOnAccessibleNode(const ACS_DSD_Node &remoteNodeInfo, const std::list<string> &lstStrArgs);

        /*=================================================================== */
        /**
         *  @brief  serverUnmountMediaOnDVDOwner             :This method is used to Setup NFS share for DVD data on passive node
         *
         *  @param  <list>lstStrArgs                         : const std::string
         *
         *  @return ACE_INT16
         **/
        /*=================================================================== */

	ACE_INT16 serverUnmountMediaOnDVDOwner(const std::list<string> &lstStrArgs);
	/*=================================================================== */
        /**
         *  @brief  serverUnmountActiveMediaOnDVDOwner             :This method is used to remove DVD mount on active node
         *
         *  @param  <list>lstStrArgs                         : const std::string
         *
         *  @return ACE_INT16
         **/
        /*=================================================================== */

	//ACE_INT16 serverUnmountActiveMediaOnDVDOwner(const std::list<string> &lstStrArgs);
	ACE_INT16 serverUnmountActiveMediaOnDVDOwner();

        /*=================================================================== */
        /**
         *  @brief  serverSyncMedia                           :This method is used to perform syncmedia
         *
         *  @return ACE_INT16
         **/
        /*=================================================================== */

	ACE_INT16 serverSyncMedia();
        /*=================================================================== */
        /**
         *  @brief  serverRemovePassiveShare                           		
         *
         *  @return ACE_INT16
         **/
        /*=================================================================== */

	ACE_INT16 serverRemovePassiveShare(const std::list<string> &lstStrArgs);
	ACE_INT16 serverPassiveShare();
	ACE_INT16 serverSyncMediaOnPassive();
	ACE_INT16 serverFormatToMedia();
	// Private struct
	struct StrctServerSession{
		ACS_DSD_Node remoteClientInfo;
	};
	// Private Attributes
	bool isStop;

        /*=================================================================== */
        /**
         *  @brief  theClienthandlerPtr
         **/
        /*================================================================== */ 

	ACS_EMF_MEDIAHandler* theClienthandlerPtr;
        /*=================================================================== */
        /**
         *  @brief  m_cmdServer
         **/
        /*=================================================================== */


	ACS_EMF_CmdServer m_cmdServer;
	
	//ACE_Event * serviceStopEvent;
	
	ACE_HANDLE stopThreadFds;
	acs_emf_commandhandler* m_pCmdHandler;
};
// ACS_EMF_DSDServer END
#endif




