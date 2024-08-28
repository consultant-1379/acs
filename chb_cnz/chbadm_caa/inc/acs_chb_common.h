/*=================================================================== */
/**
   @file   acs_chb_common.h

   @brief Header file for CHB module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       19/01/2011   XNADNAR   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_CHB_COMMON_H_
#define ACS_CHB_COMMON_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Thread_Manager.h>
#include <ACS_APGCC_Util.H>
#include <acs_chb_heartbeat_def.h>
#include <acs_chb_event.h>
#include <acs_chb_aeh.h>
#include <acs_chb_tra.h>
#include <acs_apgcc_omhandler.h>
#include <ACS_APGCC_CommonLib.h>
#include <ACS_DSD_Session.h>
#include <ACS_DSD_Server.h>
#include <ACS_DSD_Client.h>
#include <ACS_CS_API.h>
/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  CHB_SERVICE_VER

 */
/*=================================================================== */
#define CHB_SERVICE_VER "1.0"
/**
 *	@brief		ACS_IMM_CHBMTZ_ROOT_CLASS_NAME
 */
#define ACS_IMM_HBEAT_ROOT_CLASS_NAME "HeartBeat"
#define ACS_IMM_CSYNC_ROOT_CLASS_NAME "ClockSync"
#define ACS_IMM_TZLINK_ROOT_CLASS_NAME "TimeZoneMonitor"

#define ACS_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))

typedef enum _enumNodeState
{
   nsActive = 0,
   nsPassive,
   nsUnknown
} enumNodeState;

/**
 * @brief	g_nodeState
 */

extern int g_nodeState;

/**
 *      @brief          ThrExitHandler
 *                      Class for handling thread exit.
 *
 */

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

		static void cleanup()
		{
			theThrMutex.acquire();

			if( !theThrStatusMap.empty())
			{
				theThrStatusMap.clear();
			}
			theThrMutex.release();

			theThrStatusMap.clear();
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


		static bool CheckAllFuncThreads( void )
		{
#if 0
			//Lock the mutex.
			theThrMutex.acquire();

			if( theThrStatusMap.empty())
			{
				theThrMutex.release();
				return false;
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
#endif
			return true;
		}


		virtual void apply(void)
		{
			setThrStatusInMap( ACE_Thread_Manager::instance()->thr_self(), false);
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



	/*==================================================================
	*                     CLASS DECLARATION SECTION
	==================================================================== */
	/*===================================================================*/
	/**
	@brief     ACS_CHB_Common

	*/
	/*=================================================================== */

class ACS_CHB_Common
{

/*=====================================================================
	                        PROTECTED DECLARATION SECTION
==================================================================== */
protected:

/*=====================================================================
                        CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
   /**

      @brief           Constructor for ACS_CHB_AEH class.

                       Not allowed to instantiate an object of this class.


   */
/*=================================================================== */
   ACS_CHB_Common() { };

public:

   static 	bool isDSDServerRunning()
   {
	   //TR HV80771
	   /*FILE *fp = 0;
	  // ACE_INT32 status = -1;
	  // ACE_INT32 ret = -1;
	   char buff[512] = { };
	   fp = popen("ps -ef | grep acs_dsdd | wc -l", "r");
	   if (fp == NULL)
	   {
		   DEBUG(1,"%s","Error occurred in executing the ps -ef | grep acs_dsdd command\n");
		   return false;
	   }

	   if( fgets(buff, sizeof(buff), fp) != 0 )
	   {
		   int noOfLines = atoi(buff);
		   if( noOfLines == 3)
		   {
			   pclose(fp);
		   }
		   else
		   {
			   pclose(fp);
			   DEBUG(1, "%s", "acs_dsdd is not running yet, waiting for it to come up");
			   return false;
		   }
	   }
	   else		
	   {
		pclose(fp);
		DEBUG(1, "%s", "fgets returned 0 records");
	   }*/

	   ACS_DSD_Server *pchbServer;
	   pchbServer = new ACS_DSD_Server(acs_dsd:: SERVICE_MODE_UNIX_SOCKET);
	
	   if(pchbServer!=NULL)	
	   {
	   int ret=pchbServer->open();
	   INFO(1,"Status of open() : %d",ret);

	   if (ret < 0)
	   {
	     DEBUG(1,"Failure occurred in opening the linux local socket, errno: %d ",pchbServer->last_error());
	     //TR HV80771
	     if( pchbServer != 0)
	     {
	    	 delete pchbServer;
	    	 pchbServer = 0;
	     }
	     return false;
	   }

	   ret = pchbServer->publish("DummyServer", "CHBTEST", acs_dsd::SERVICE_VISIBILITY_LOCAL_SCOPE);
	   INFO(1,"Status of publish() : %d",ret);

	   if (ret < 0)
	   {

	     DEBUG(1,"Failure occurred in publishing the linux local socket, errno: %d",pchbServer->last_error());
	     if( pchbServer != 0)
	     {
	       delete pchbServer;
	       pchbServer = 0;
	     }
	     return false;
	   }
#if 0
	   fp = popen("dsdls 1>/dev/null 2>/dev/null", "r");

	   if (fp == NULL)
	   {
		   DEBUG(1,"%s","Error occurred in executing the dsdls command\n");
		   return false;
	   }

	   status = pclose(fp);

	   if (WIFEXITED(status) )
	   {
		   ret=WEXITSTATUS(status);
		   DEBUG(1,"dsdls command Execution status = %d",ret);
	   }
#endif

	   if (ret == 0 )
	   {

	     if( pchbServer != 0)
	     {
	       delete pchbServer;
	       pchbServer = 0;
	     }
	     return true;
	   }
	   else
	   {
	     if( pchbServer != 0)
	     {
	       delete pchbServer;
	       pchbServer = 0;
	     }
	     return false;
	   }
	}	
	return false;
   }

   static bool isDataDiskAvailable()
   {
	   ACS_APGCC_CommonLib oComLib;
	   char datadiskPath[200] ;
	   int filepathLength = 200;
	   if( oComLib.GetDataDiskPath("ACS_DATA", datadiskPath, filepathLength) != ACS_APGCC_DNFPATH_SUCCESS )
	   {
		   ERROR( 1, "%s","Error occurred while getting data disk path");
		   return false;
	   }

	   ACE_stat statBuff;
	   if( ACE_OS::stat(datadiskPath, &statBuff)  != 0 )
	   {
		   ERROR(1, "%s is not present", datadiskPath);
		   return false;
	   }
	   return true;
   }
/*===================================================================
     					PUBLIC DECLARATION SECTION
=================================================================== */
/*===================================================================
                           PUBLIC METHOD
=================================================================== */
static void CHB_serviceError(const char* string, ACE_UINT32 status)
   {

   	char problemData [PROBLEMDATASIZE];
   	ACE_OS::memset( problemData, 0, PROBLEMDATASIZE);
   	ACS_CHB_Event myEvent(ACS_CHB_processName);

   	if ( status == 0 )
   	{
   		ACE_OS::strcpy(problemData, string);
   	}
   	else
   	{
   		ACE_OS::sprintf(problemData, "%s%d", string, status);
   	}

   	myEvent.event(ACS_CHB_SERVICEERROR,
   				  ACS_CHB_EVENT_STG,
   				  ACS_CHB_Cause_APfault,
   				  problemData,
   				  "CHB_serviceError");

   	DEBUG(1,"%s", problemData );

   }//end of CHB_serviceError

	/*=================================================================== */
	/**
		         @brief       countSpacesInString

		         @param       str

				 @return      int
		   					  returns the no of spaces in given string

	 */
	/*=================================================================== */
	static int countSpacesInString(string str)
	{
		int count = 0;
		char c = 0x20;
		for (unsigned int i=0; i < str.length(); i++)
		{
			if (str.at(i) == c)
			{
				count++;
			}
		}
		return count;
	}
	/*=================================================================== */
	/**
			         @brief       TrimSpaces

			         @param       str

					 @return      bool
			   					  true/false

	 */
	/*=================================================================== */
	static bool TrimSpaces( string& str)
	{
		size_t startpos = str.find_first_not_of(" \t");
		size_t endpos = str.find_last_not_of(" \t");

		// if all spaces or empty return an empty string
		if(( string::npos == startpos ) || ( string::npos == endpos))
		{
			return false;
		}
		else
		{
			str = str.substr( startpos, endpos-startpos+1 );
		}
		return true;
	}

        /*=================================================================== */
        /**
                         @brief       fetchDnOfHbeatRootObjFromIMM

                         @return      ACE_INT32
                                        0  on success
                                        -1 on failure

         */
        /*=================================================================== */
        static ACE_INT32 fetchDnOfHbeatRootObjFromIMM()
        {
                static int infoAlreadyLoad = 0;
                static char dnOfHbeatRootClass[512] = {0};

                if (infoAlreadyLoad)
                {
                        strcpy(dnOfHbeatRootObj,dnOfHbeatRootClass);
                        return 0;
                }

                ACE_Recursive_Thread_Mutex loadingSyncObject;

                ACE_Guard<ACE_Recursive_Thread_Mutex> guard(loadingSyncObject, true);

                if (!guard.locked())
                {
                        return -1;
                }

                OmHandler omHandler;

                if (omHandler.Init() != ACS_CC_SUCCESS)
                {
                        return -1;
                }

                std::vector<std::string> dn_list;
                int returnCode = 0;

                if (omHandler.getClassInstances(ACS_IMM_HBEAT_ROOT_CLASS_NAME, dn_list) != ACS_CC_SUCCESS)
                {
                        //ERROR
                        returnCode = -1;
                }
                else
                {
                        //OK: Checking how much dn items was found in IMM
                        if (dn_list.size() ^ 1)
                        {
                                //WARNING: 0 (zero) or more than one node found
                                returnCode = -1;
                        }
                        else
                        {
                                //OK: Only one root node
                                strncpy(dnOfHbeatRootClass, dn_list[0].c_str(), ACS_ARRAY_SIZE(dnOfHbeatRootClass));
                                dnOfHbeatRootClass[ACS_ARRAY_SIZE(dnOfHbeatRootClass) - 1] = '\0';
                                infoAlreadyLoad = 1;
                                strcpy(dnOfHbeatRootObj,dnOfHbeatRootClass);
                        }
                }

                omHandler.Finalize();
                return returnCode;
        }

        /*=================================================================== */
        /**
                         @brief       fetchDnOfHbeatRootObjFromIMM

                         @return      ACE_INT32
                                        0  on success
                                        -1 on failure

         */
        /*=================================================================== */
        static ACE_INT32 fetchDnOfCsyncRootObjFromIMM()
        {
                static int infoAlreadyLoad = 0;
                static char dnOfCsyncRootClass[512] = {0};
                if (infoAlreadyLoad)
                {
                        strcpy(dnOfCsyncRootObj,dnOfCsyncRootClass);
                        return 0;
                }

                ACE_Recursive_Thread_Mutex loadingSyncObject;

                ACE_Guard<ACE_Recursive_Thread_Mutex> guard(loadingSyncObject, true);

                if (!guard.locked())
                {
                        return -1;
                }

                OmHandler omHandler;

                if (omHandler.Init() != ACS_CC_SUCCESS)
                {
                        return -1;
                }

                std::vector<std::string> dn_list;
                int returnCode = 0;

                if (omHandler.getClassInstances(ACS_IMM_CSYNC_ROOT_CLASS_NAME, dn_list) != ACS_CC_SUCCESS)
                {
                        //ERROR
                        returnCode = -1;
                }
                else
                {
                        //OK: Checking how much dn items was found in IMM
                        if (dn_list.size() ^ 1)
                        {
                                //WARNING: 0 (zero) or more than one node found
                                returnCode = -1;
                        }
                        else
                        {
                                //OK: Only one root node
                                strncpy(dnOfCsyncRootClass, dn_list[0].c_str(), ACS_ARRAY_SIZE(dnOfCsyncRootClass));
                                dnOfCsyncRootClass[ACS_ARRAY_SIZE(dnOfCsyncRootClass) - 1] = '\0';
                                infoAlreadyLoad = 1;
                                strcpy(dnOfCsyncRootObj,dnOfCsyncRootClass);
                        }
                }

                omHandler.Finalize();
                return returnCode;
        }

        /*=================================================================== */
        /**
                         @brief       fetchDnOfHbeatRootObjFromIMM

                         @return      ACE_INT32
                                        0  on success
                                        -1 on failure

         */
        /*=================================================================== */
        static ACE_INT32 fetchDnOfTzLinkRootObjFromIMM()
        {
                static int infoAlreadyLoad = 0;
                static char dnOfTzLinkRootClass[512] = {0};

                if (infoAlreadyLoad)
                {
                        strcpy(dnOfTzLinkRootObj,dnOfTzLinkRootClass);
                        return 0;
                }

                ACE_Recursive_Thread_Mutex loadingSyncObject;

                ACE_Guard<ACE_Recursive_Thread_Mutex> guard(loadingSyncObject, true);

                if (!guard.locked())
                {
                        return -1;
                }

                OmHandler omHandler;

                if (omHandler.Init() != ACS_CC_SUCCESS)
                {
                        return -1;
                }

                std::vector<std::string> dn_list;
                int returnCode = 0;

                if (omHandler.getClassInstances(ACS_IMM_TZLINK_ROOT_CLASS_NAME, dn_list) != ACS_CC_SUCCESS)
                {
                        //ERROR
                        returnCode = -1;
                }
                else
                {
                        //OK: Checking how much dn items was found in IMM
                        if (dn_list.size() ^ 1)
                        {
                                //WARNING: 0 (zero) or more than one node found
                                returnCode = -1;
                        }
                        else
                        {
                                //OK: Only one root node
                                strncpy(dnOfTzLinkRootClass, dn_list[0].c_str(), ACS_ARRAY_SIZE(dnOfTzLinkRootClass));
                                dnOfTzLinkRootClass[ACS_ARRAY_SIZE(dnOfTzLinkRootClass) - 1] = '\0';
                                infoAlreadyLoad = 1;
                                strcpy(dnOfTzLinkRootObj,dnOfTzLinkRootClass);
                        }
                }

                omHandler.Finalize();
                return returnCode;
        }
	/*=================================================================== */
	/**
		         @brief       fetchDnOfRootObjFromIMM

			 @return      ACE_INT32
					0  on success
					-1 on failure

	 */
	/*=================================================================== */
#if 0
        static ACE_INT32 fetchDnOfRootObjFromIMM()
	{
		static int infoAlreadyLoad = 0;
		static char dnOfRootClass[512] = {0};

		if (infoAlreadyLoad)
		{
			strcpy(dnOfCHBRootObj,dnOfRootClass);
			return 0;
		}

		ACE_Recursive_Thread_Mutex loadingSyncObject;

		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(loadingSyncObject, true);

		if (!guard.locked())
		{
			return -1;
		}

		OmHandler omHandler;

		if (omHandler.Init() != ACS_CC_SUCCESS)
		{
			return -1;
		}

		std::vector<std::string> dn_list;
		int returnCode = 0;

		if (omHandler.getClassInstances(ACS_IMM_CHBMTZ_ROOT_CLASS_NAME, dn_list) != ACS_CC_SUCCESS)
		{
			//ERROR
			returnCode = -1;
		}
		else
		{
			//OK: Checking how much dn items was found in IMM
			if (dn_list.size() ^ 1)
			{
				//WARNING: 0 (zero) or more than one node found
				returnCode = -1;
			}
			else
			{
				//OK: Only one root node
				strncpy(dnOfRootClass, dn_list[0].c_str(), ACS_ARRAY_SIZE(dnOfRootClass));
				dnOfRootClass[ACS_ARRAY_SIZE(dnOfRootClass) - 1] = '\0';
				infoAlreadyLoad = 1;
				strcpy(dnOfCHBRootObj,dnOfRootClass);
			}
		}

		omHandler.Finalize();
		return returnCode;
	}
#endif
	/*===================================================================*/
	/**
                         @brief       isVirtualEnvironment

                         @return      bool
                                        true VIRTUAL Environment
                                        false NONVIRTUAL Environment/UNKNOWN
										
	*/
	/*=================================================================== */
	static bool isVirtualEnvironment()
	{
		bool virtualEnv=false;		
		ACS_CS_API_CommonBasedArchitecture::ArchitectureValue nodeArchitecture;

		if(ACS_CS_API_NetworkElement::getNodeArchitecture(nodeArchitecture) == ACS_CS_API_NS::Result_Success)
		{
			if (nodeArchitecture == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)		
				virtualEnv =true;
		}				
		DEBUG(1, "Call isVirtualEnvironment = %d", virtualEnv);				
		return virtualEnv;
	}
public:
	static char dnOfHbeatRootObj[512];
	static char dnOfCsyncRootObj[512];
	static char dnOfTzLinkRootObj[512];
};
#endif
