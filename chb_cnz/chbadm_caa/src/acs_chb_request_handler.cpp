/*=================================================================== */
   /**
   @file acs_chb_request_handler.cpp

   Class method implementationn for CHB module.

   This module contains the implementation of class declared in
   the acs_chb_request_handler.h module

   @version 1.0.0
	*/
	/*

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/01/2011   XNADNAR   Initial Release
	*/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ace/ACE.h>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ace/Event.h>
#include <ace/ACE.h>
#include <ace/OS_NS_sys_select.h>
#include <ACS_DSD_Server.h>
#include <ACS_DSD_Client.h>
#include <acs_chb_request_handler.h>
#include <acs_chb_heartbeat_def.h>
#include <acs_chb_tra.h>

// Trace points.

/*===================================================================
                        GLOBAL DECLARATION
=================================================================== */
/*=================================================================== */
/**
   @brief ACS_CHB_CPObjectList

 */
/*=================================================================== */
extern ACS_CHB_CPObjectList *HeadCPObjectList;

/*=================================================================== */
/**
   @brief requestFromHeartBeatPending
		  Status of request from HeartBeat.
 */
/*=================================================================== */
extern int requestFromHeartBeatPending;

/*=================================================================== */
/**
   @brief requestFromHeartBeat
		  Data for request from Heartbeat
 */
/*=================================================================== */
extern char requestFromHeartBeat[];		// Data for request from


/*===================================================================
   ROUTINE: ACS_CHB_RequestHandler
=================================================================== */

ACS_CHB_RequestHandler::ACS_CHB_RequestHandler() : ACS_CHB_CPObjectList(),
ACS_CHB_Event (ACS_CHB_processName)
{
	//  Default head for CP object list list.

	pchbServer = 0;
	pcSession = 0;
	psSession = 0;
	pchbClient = 0;
	
	headCPObjectList = new ACS_CHB_CPObjectList();
	HeadCPObjectList = headCPObjectList;	
	
	// Setup list with initial object data,
	initCPObjectList(headCPObjectList);
} // End of constructor

/*===================================================================
ROUTINE: ACS_CHB_RequestHandler
=================================================================== */

ACS_CHB_RequestHandler::~ACS_CHB_RequestHandler()
{

	if( pchbServer != 0)
	{
		delete pchbServer;
		pchbServer = 0;
	}
	//Delete the session object for HeartBeat child.
	if( pcSession != 0)
	{
		delete pcSession;
		pcSession = 0;
	}

	if( pchbClient != 0 )
	{
		delete pchbClient ;
		pchbClient = 0;
	}

	//Delete the session object for HeartBeat.
	if( psSession != 0)
	{
		delete psSession;
		psSession = 0;
	}
} // End of destructor

/*===================================================================
   ROUTINE: initPipeFromHeartBeat
=================================================================== */

ACS_CHB_returnType ACS_CHB_RequestHandler::initPipeFromHeartBeat()
{
	pchbClient = new ACS_DSD_Client();

	pcSession = new ACS_DSD_Session();

	int retry = 10;
	while( retry--)
	{
		int ret = pchbClient->connect(*pcSession, "HeartBeat", "CHB", 5000);
		INFO(1,"Status of child connect():%d",ret);

		if(ret < 0)
		{
			 ERROR(1,"Failure in connecting to  heartbeat server :errno %d errorText: %s",pchbClient->last_error(), pchbClient->last_error_text());
			// sleep(10);
			 sleep(2);
		}
		else
		{
			return ACS_CHB_OK;
		}
		
	}
	return(ACS_CHB_FAIL);
} // End of initPipeFromHeartBeat

/*===================================================================
   ROUTINE: initPipeToEventManager
=================================================================== */

ACS_CHB_returnType ACS_CHB_RequestHandler::initPipeToEventManager()
{
	pchbServer = new ACS_DSD_Server(acs_dsd:: SERVICE_MODE_UNIX_SOCKET);

	int ret=pchbServer->open();
	INFO(1,"Status of open() : %d",ret);

	if (ret < 0)
	{
		  DEBUG(1,"Failure occurred in opening the linux local socket, errno: %d ",pchbServer->last_error());
		  return ACS_CHB_FAIL;
	}

	ret = pchbServer->publish("HeartBeat", "CHB", acs_dsd::SERVICE_VISIBILITY_LOCAL_SCOPE);
	INFO(1,"Status of publish() : %d",ret);
	if (ret < 0)
	{
	  DEBUG(1,"Failure occurred in publishing the linux local socket, errno: %d",pchbServer->last_error());
	  return ACS_CHB_FAIL;
	}

	//Get the server handles and wait for some request from the client.

	ACE_HANDLE serverHandle = ACE_INVALID_HANDLE;
        int handle_count = 1;

	ret = pchbServer->get_handles(&serverHandle, handle_count);

        if( ret < 0)
        {
                ERROR(1, "%s", "Error in get handles for heart beat server, returning.. ");
                return ACS_CHB_FAIL;
        }
        else
        {
                //Now, we have to wait on the handle for some data from the child.
                //If some data arrives, we will recieve a signal on the handle and then we can receive the data.

                 //Create a FD set.
                fd_set readHandleSet;
                FD_ZERO(&readHandleSet);
                FD_SET (serverHandle, &readHandleSet);
		ACE_Time_Value tv(25, 0);

                int ret1 = ACE_OS::select((int)(serverHandle+1), &readHandleSet, 0, 0, &tv);
		   //Check for timeout.
                if (ret1 > 0)
                {
                    //Check if the handle is set.
                        if ( !FD_ISSET(serverHandle, &readHandleSet))
                        {
                                ERROR(1, "%s", "handle for  heartbeat server is not signaled ");
                                return ACS_CHB_FALSE;
                        }
                        DEBUG(1, "%s", "Handle signalled while waiting for request from client");
                }
		else if( ret1 == 0 )
		{
			 ERROR(1, "%s", "Timeout occured while waiting for request from child");
			 return ACS_CHB_FALSE;
		}
                else
                {
                        ERROR(1, "%s", "Error occurred in select system call in request handler");
                        return ACS_CHB_FALSE;
                }

	}

	psSession = new ACS_DSD_Session();

	ret = pchbServer->accept(*psSession);
	INFO(1,"Status of accept() : %d",ret);

	if (ret < 0)
	{
		DEBUG(1,"Error occurred in accepting the heartbeat child(client), errno: %d",pchbServer->last_error());
		delete psSession;
		psSession = 0;
		return ACS_CHB_FAIL;
	}
	
	return(ACS_CHB_OK);
} // End of initPipeToChild

/*===================================================================
   ROUTINE: isRequestFromHeartBeatPending
=================================================================== */

ACS_CHB_returnType 
ACS_CHB_RequestHandler::isRequestFromHeartBeatPending(int *requestCode)
{
	// Return status of request.
	
	return(*requestCode ? ACS_CHB_OK : ACS_CHB_FAIL);
} // End of isRequestFromHeartBeatPending

/*===================================================================
   ROUTINE: isRequestFromCPPending
=================================================================== */

ACS_CHB_returnType 
ACS_CHB_RequestHandler::isRequestFromCPPending(unsigned int  requestFromCP,
											   unsigned int  dataFromCP,
											   unsigned char *requestBuffer,
                                               unsigned int   bufflen)
{
	char cp_object_item[ACS_CHB_SMALL_BUFSIZE];
	char cp_object_value[ACS_CHB_SMALL_BUFSIZE];
	unsigned char rBuffer[ACS_CHB_SMALL_BUFSIZE];
	ACS_CHB_returnType return_code = ACS_CHB_FAIL;
	
	// Check whether incoming request is a CP object to be routed further.
	ACE_OS::memcpy(rBuffer,requestBuffer,bufflen);
	rBuffer[bufflen] = '\0';
	if (decodeRequestFromCP(requestFromCP,dataFromCP,rBuffer,bufflen,
		cp_object_item,cp_object_value) == ACS_CHB_OK) 
	{
		// Legal CP object request found, create CP object list message.
		ACE_OS::sprintf(requestFromHeartBeat,"[CPOBJECT][%s][%s]",cp_object_item,
            cp_object_value);
		return_code = ACS_CHB_OK;
	}
	else 
	{
		return_code = ACS_CHB_FAIL;
	}
	
	// Trace point
		if (return_code == ACS_CHB_OK) 
		{
			char request_from_HeartBeat[ACS_CHB_SMALL_BUFSIZE];
			ACE_OS::sprintf(request_from_HeartBeat,
				"CP object request received. requestFromHeartBeat=%s",
				requestFromHeartBeat);

			DEBUG(1,"%s",request_from_HeartBeat);
		} 
		else 
		{
			DEBUG(1,"%s","Not supported CP request received");
		}
	
	// Return status
	
	return return_code;
} // End of isRequestFromCPPending

/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief EXPECTED_NO_OF_BRACKETS_IN_MESSAGE

 */
/*=================================================================== */
const int EXPECTED_NO_OF_BRACKETS_IN_MESSAGE = 6;

/*===================================================================
   ROUTINE: decodeCPObjectMessage
=================================================================== */
ACS_CHB_returnType 
ACS_CHB_RequestHandler::decodeCPObjectMessage(char cpObjectItem[],
											  char cpObjectValue[])
{
	unsigned int i;
	int read_no_of_brackets = 0;
	
	// Do a simple syntax check by verifying the number of brackets
	// in the message.
	
	for (i = 0;i < ACE_OS::strlen(requestFromHeartBeat);++i)
	{
		if (requestFromHeartBeat[i] == '[' || requestFromHeartBeat[i] == ']') 
		{
			++read_no_of_brackets;
		}
	} // End of for-loop
	
	// Format message if syntax check successful.
	
	char *tag_header;
	char *tag_item;
	char *tag_data;
	ACS_CHB_returnType status = ACS_CHB_FAIL;
	if (read_no_of_brackets == EXPECTED_NO_OF_BRACKETS_IN_MESSAGE) 
	{
		// Skip header
		tag_header = ACE_OS::strtok(requestFromHeartBeat,"][");
		// Extract data
		tag_item = tag_header + strlen(tag_header) + 2;
		ACE_OS::strcpy(cpObjectItem,tag_item);
		// Extract data
		tag_data = tag_item + strlen(tag_item) + 2;
		tag_data = ACE_OS::strtok(tag_data,"]");
		if (tag_data != NULL)		// If CP header is empty 040909 UABMAGN
			ACE_OS::strcpy(cpObjectValue,tag_data);
		else
			ACE_OS::strcpy(cpObjectValue," ");
		// Indicate that formatting succeeded.
		status = ACS_CHB_OK;
	} 
	
	// Return status.
	
	return status;
} // End of decodeCPObjectMessage

/*===================================================================
   ROUTINE: searchForAndUpdateObjectItemInList
=================================================================== */

void 
ACS_CHB_RequestHandler::searchForAndUpdateObjectItemInList(char cpObjectItem[],
														   char cpObjectValue[])
{
	ACS_CHB_CPObjectList *ptrCPObjectList = headCPObjectList;
	
	// Search through object list.
	
	while (ptrCPObjectList != (ACS_CHB_CPObjectList *)0) 
	{
		// Request pending, decode and check if available in list.
		if (isCPObjectInListAndValueChanged(ptrCPObjectList,cpObjectItem,
			cpObjectValue) == ACS_CHB_OK ) 
		{
			DEBUG(1,"CP object value change detected.cpObjectItem= %s, cpObjectValue=%s",cpObjectItem,cpObjectValue);
			// item found with value that differs, indicate change of value.
			updateCPObjectItem(cpObjectItem,cpObjectValue);
		}
		ptrCPObjectList = ptrCPObjectList->get_next();
	} // End of while
} // End of searchForAndUpdateObjectItemInList

/*===================================================================
   ROUTINE: findUpdatedObjectItemInList
=================================================================== */

ACS_CHB_returnType 
ACS_CHB_RequestHandler::findUpdatedObjectItemInList(char cpObjectItem[],
													char cpObjectValue[])
{
	ACS_CHB_CPObjectList *ptrCPObjectList = headCPObjectList;
	ACS_CHB_returnType status = ACS_CHB_FAIL;
	
	// Search through object list.
	
	while (ptrCPObjectList != (ACS_CHB_CPObjectList *)0) 
	{
		// Check if value changed.
		if (ptrCPObjectList -> get_CPObjectValueChanged() == ACS_CHB_OK) 
		{
			// Copy item an value into supplied buffer.
			ACE_OS::strcpy(cpObjectItem,ptrCPObjectList -> get_CPObjectItemPtr());
			ACE_OS::strcpy(cpObjectValue,ptrCPObjectList -> get_CPObjectValuePtr());
			// Indicate link found and reset.
			status = ACS_CHB_OK;
			// Link values now exported.
			ptrCPObjectList -> set_CPObjectValueChanged(ACS_CHB_FAIL);
		}
		ptrCPObjectList = ptrCPObjectList->get_next();
	} // End of while
	
	// Return status of operation.
	
	return status;
} // End of findUpdatedObjectItemInList

/*===================================================================
   ROUTINE: sendRequestToEventManager
=================================================================== */

ACS_CHB_returnType 
ACS_CHB_RequestHandler::sendRequestToEventManager(char cpObjectItem[],
					   char cpObjectValue[])
{
		char request_from_HeartBeat_message[ACS_CHB_SMALL_BUFSIZE];
		memset( request_from_HeartBeat_message, 0, sizeof(request_from_HeartBeat_message));

		ssize_t bytesWritten = 0;			// Used for write message
		size_t maxBytes = ACS_CHB_SMALL_BUFSIZE;
		ACS_CHB_returnType fresult = ACS_CHB_OK;                // Result of send

		// Create request message
		ACE_OS::sprintf(request_from_HeartBeat_message,"[CPOBJECT][%s][%s]",
		cpObjectItem,cpObjectValue);

		// Trace point
		DEBUG(1,"Request sent from HeartBeat to event manager,message= %s",request_from_HeartBeat_message);

		// Send request to EventManager.
		if( psSession != 0)
		{
			bytesWritten = psSession->send(request_from_HeartBeat_message,maxBytes);
			INFO(1,"%d: bytes of data sent to child", bytesWritten);
		}

		if (bytesWritten <= 0)
		{
                	ERROR(1,"Error: message not sent to Event manager, status:",errno);
			fresult=ACS_CHB_FAIL;
		}

		ACE_OS::sleep(1);       //Just wait for Heart Beat Child has finished.
		return fresult;

} // End of sendRequestToEventManager


/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief ILLEGAL_CP_OBJECT_CODE

 */
/*=================================================================== */
const int ILLEGAL_CP_OBJECT_CODE 	= 1;
/*=================================================================== */
/**
   @brief ILLEGAL_CP_OBJECT_CODE

 */
/*=================================================================== */
const int ILLEGAL_CP_OBJECT_VALUE 	= 2;

/*===================================================================
   ROUTINE: decodeRequestFromCP
=================================================================== */
ACS_CHB_returnType 
ACS_CHB_RequestHandler::decodeRequestFromCP(unsigned int requestFromCP,
											unsigned int dataFromCP,
											unsigned char requestBuffer[],
                                            unsigned int bufflen,
											char cpObjectItem[],
											char cpObjectValue[])
{
	ACS_CHB_returnType return_code = ACS_CHB_OK;
	int type_of_error = 0;		// 1 = illegal name, 2 = illegal value
	unsigned char* rbuf;
	// Format incoming request string.
	
	switch (requestFromCP) 
	{
    case ACS_CHB_APZ_TYPE:
		// Create Object item and value,
		ACE_OS::strcpy(cpObjectItem,"APZ_type");
		switch (dataFromCP) 
		{
        case ACS_CHB_APZ210:
			ACE_OS::strcpy(cpObjectValue,"APZ210");
			break;
        case ACS_CHB_APZ211:
			ACE_OS::strcpy(cpObjectValue,"APZ211");
			break;
        case ACS_CHB_APZ212:
			ACE_OS::strcpy(cpObjectValue,"APZ212");
			break;
        case ACS_CHB_APZ213:
			ACE_OS::strcpy(cpObjectValue,"APZ213");
			break;
		case ACS_CHB_APZ214:
			ACE_OS::strcpy(cpObjectValue,"APZ214");
			break;
        default:
			// Code not supported.
			type_of_error = ILLEGAL_CP_OBJECT_VALUE;	// Illegal value
			return_code = ACS_CHB_FAIL;
			break;
		} // End of swith (dataFromCP) in case ACS_CHB_APZ_TYPE
		break;
		case ACS_CHB_EX_SIDE:
			// Create Object item and value,
			ACE_OS::strcpy(cpObjectItem,"EX_side");
			switch (dataFromCP) 
			{
			case ACS_CHB_SIDE_A:
				ACE_OS::strcpy(cpObjectValue,"A");
				break;
			case ACS_CHB_SIDE_B:
				ACE_OS::strcpy(cpObjectValue,"B");
				break;
			default:
				// Code not supported.
				type_of_error = ILLEGAL_CP_OBJECT_VALUE;	// Illegal value
				return_code = ACS_CHB_FAIL;
				break;
			} // End of switch (dataFromCP) in case ACS_CHB_EX_SIDE
			break;
			case ACS_CHB_SB_STATUS:
				// Create Object item and value,
				ACE_OS::strcpy(cpObjectItem,"SB_status");
				switch (dataFromCP) 
				{
				case ACS_CHB_SBSE:
					ACE_OS::strcpy(cpObjectValue,"SBSE");
					break;
				case ACS_CHB_SBHA:
					ACE_OS::strcpy(cpObjectValue,"SBHA");
					break;
				case ACS_CHB_SBUP:
					ACE_OS::strcpy(cpObjectValue,"SBUP");
					break;
				case ACS_CHB_SBWO:
					ACE_OS::strcpy(cpObjectValue,"SBWO");
					break;
				case ACS_CHB_NOINFO:
					ACE_OS::strcpy(cpObjectValue,"NOINFO");
					break;
				default:
					// Code not supported.
					type_of_error = ILLEGAL_CP_OBJECT_VALUE;	// Illegal value
					return_code = ACS_CHB_FAIL;
					break;
				} // End of switch (dataFromCP) in case ACS_CHB_SB_STATUS
				break;
				case ACS_CHB_CP_STATUS:
					// Create Object item and value,
					ACE_OS::strcpy(cpObjectItem,"CP_status");
					switch (dataFromCP) 
					{
					case ACS_CHB_CP_PARWO:
						ACE_OS::strcpy(cpObjectValue,"PARWO");
						break;
					case ACS_CHB_CP_SINEX:
						ACE_OS::strcpy(cpObjectValue,"SINEX");
						break;
					case ACS_CHB_CP_SBSE:
						ACE_OS::strcpy(cpObjectValue,"SBSE");
						break;
					default:
						// Code not supported.
						type_of_error = ILLEGAL_CP_OBJECT_VALUE;	// Illegal value
						return_code = ACS_CHB_FAIL;
						break;
					} // End of switch (dataFromCP) in case ACS_CHB_CP_STATUS
					break;
					case ACS_CHB_CP_CONNECTION:
						// Create Object item and value,
						ACE_OS::strcpy(cpObjectItem,"CP_connection");
						switch (dataFromCP) 
						{
						case ACS_CHB_CONNECTED:
							ACE_OS::strcpy(cpObjectValue,"CONNECTED");
							break;
						case ACS_CHB_NOT_CONNECTED:
							ACE_OS::strcpy(cpObjectValue,"NOT_CONNECTED");
							break;
						default:
							// Code not supported.
							type_of_error = ILLEGAL_CP_OBJECT_VALUE;	// Illegal value
							return_code = ACS_CHB_FAIL;
							break;
						} // End of switch (dataFromCP) in case ACS_CHB_CP_CONNECTION
						break;
						case ACS_CHB_EX_EXCHANGE_IDENTITY:
							// Create Object item and value,
							ACE_OS::strcpy(cpObjectItem,"EX_Exchange_identity");

							// Copy exchange identity as value.
							// for TR : HF 53059 
							// suspecting some unprintable characters ASCII codes (less than 33 and Greater than 126)
							// read frm file, skipping the same
							if (bufflen > 7) 
							{
							  bufflen = bufflen - 7; 
							  rbuf = requestBuffer + 7; 
							} 
							else 
							{ 
							   rbuf = requestBuffer;               
							}

							while (*rbuf < 33 || *rbuf  > 126) 
							{
							   rbuf++; 
							   bufflen--; 
							   if (bufflen == 0)
								   break; 
							} // End of while 
							// TR HF 53059 ....


							ACE_OS::memcpy(cpObjectValue,rbuf,bufflen);
							cpObjectValue[bufflen] = '\0';
							break;
						case ACS_CHB_SB_EXCHANGE_IDENTITY:
							// Create Object item and value,
							ACE_OS::strcpy(cpObjectItem,"SB_Exchange_identity");
							// Copy exchange identity as value.
							ACE_OS::memcpy(cpObjectValue,requestBuffer,bufflen);
							break;
						case ACS_CHB_LAST_RESTART_VALUE:
							// Create Object item and value,
							ACE_OS::strcpy(cpObjectItem,"Last_Restart_Value");
							switch (dataFromCP) 
							{
							case ACS_CHB_SMALL_C:
								ACE_OS::strcpy(cpObjectValue,"SMALL_C");
								break;
							case ACS_CHB_SMALL_F:
								ACE_OS::strcpy(cpObjectValue,"SMALL_F");
								break;
							case ACS_CHB_LARGE_C:
								ACE_OS::strcpy(cpObjectValue,"LARGE_C");
								break;
							case ACS_CHB_LARGE_F:
								ACE_OS::strcpy(cpObjectValue,"LARGE_F");
								break;
							case ACS_CHB_RELOAD_C:
								ACE_OS::strcpy(cpObjectValue,"RELOAD_C");
								break;
							case ACS_CHB_RELOAD_F:
								ACE_OS::strcpy(cpObjectValue,"RELOAD_F");
								break; 
							default:
								// Value not supported.
								type_of_error = ILLEGAL_CP_OBJECT_VALUE;	// Illegal value
								return_code = ACS_CHB_FAIL;
								break;
							} // End of switch (dataFromCP) in case ACS_CHB_LAST_RESTART_VALUE
							break;
							case ACS_CHB_TMZ_ALARM :
								// Create Object item and value,
								ACE_OS::strcpy(cpObjectItem,"Time_Zone_Alarm");
								ACE_OS::memcpy(cpObjectValue,requestBuffer,24);
								cpObjectValue[24]='\0';
								break;
							default:
								// Code not supported.
								type_of_error = ILLEGAL_CP_OBJECT_CODE;		// Illegal code.
								return_code = ACS_CHB_FAIL;
								break;
  } // End of switch (requestFromCP)
  
  // Verify whether the request was legal or not. If not indicate.
  
  if (return_code == ACS_CHB_FAIL) 
  {
	  // Set and indicate error type.
	  if (type_of_error == ILLEGAL_CP_OBJECT_CODE) 
	  {
		  // Object code not supported.
		  // Trace point
		  
			DEBUG(1,"Not supported CP Object code received, code =%d",requestFromCP);
	  } 
	  else 
	  {
		  // Value not supported.
		  // Trace point
		  DEBUG(1,"Not supported CP Object code received, code =%d",requestFromCP);
	  }
  }
  
  return return_code;
} // End of decodeRequestFromCP

/*===================================================================
   ROUTINE: isCPObjectInListAndValueChanged
=================================================================== */

ACS_CHB_returnType 
ACS_CHB_RequestHandler::isCPObjectInListAndValueChanged(
														ACS_CHB_CPObjectList *ptrCPObjectListItem,
														char cpObjectItem[],char cpObjectValue[])
{
	ACS_CHB_returnType status = ACS_CHB_FAIL;
	
	// Compare item and value.
	
	if ((!ACE_OS::strcmp(ptrCPObjectListItem -> get_CPObjectItemPtr(),cpObjectItem)) &&
		(ACE_OS::strcmp(ptrCPObjectListItem -> get_CPObjectValuePtr(),cpObjectValue)))
	{
		// Item found, and old value differs fro mthe new one.
		status = ACS_CHB_OK;
	}

	// if Last_Restart_Value then make sure it is always updated
	// Corrected 2001-03-13 UABBJIS
	else if ((0 == ACE_OS::strcmp(ptrCPObjectListItem -> get_CPObjectItemPtr(),cpObjectItem)) &&
		(0 == ACE_OS::strcmp(cpObjectItem,"Last_Restart_Value")))
	{
		// We have a Last_Restart_Value. Make sure it is always updated
		status = ACS_CHB_OK;
	}

	// Return status on whether item found with old value that differs from 
	// the new one.
	
	return status;
} // End of isCPObjectInListAndValueChanged

/*===================================================================
   ROUTINE: updateCPObjectItem
=================================================================== */

void 
ACS_CHB_RequestHandler::updateCPObjectItem(char cpObjectItem[],
					   char cpObjectValue[])
{
	ACS_CHB_CPObjectList *ptrCPObjectList = headCPObjectList;
	
	// Search through list for matching item to update
	
	while (ptrCPObjectList != (ACS_CHB_CPObjectList *)0) 
	{
		// Compare item
		if (!ACE_OS::strcmp(ptrCPObjectList -> get_CPObjectItemPtr(),cpObjectItem))
		{
			// Store new value for item.
			ptrCPObjectList ->set_CPObjectValue(cpObjectValue);
			// Indicate value change.
			ptrCPObjectList -> set_CPObjectValueChanged(ACS_CHB_OK);
		}

		ptrCPObjectList = ptrCPObjectList->get_next();
	} // End of while
} // End of updateCPObjectItem

