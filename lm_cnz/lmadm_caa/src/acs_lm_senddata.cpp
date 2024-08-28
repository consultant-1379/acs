#include<acs_lm_senddata.h>

ACE_Recursive_Thread_Mutex acs_lm_senddata::theSenddataMutex;
ACE_Recursive_Thread_Mutex acs_lm_senddata::theReportDataMutex;

acs_lm_senddata::acs_lm_senddata(acs_lm_cpdata& aCPData,ACE_Thread_Manager* aThrManager)
:ACE_Task_Base(aThrManager)
{
	DEBUG("Entering %s","acs_lm_senddata::acs_lm_senddata() ");
	theCPData = aCPData;
	theLMInstCommandOption = false;
	setStopSignal(false);
	DEBUG("Leaving %s","acs_lm_senddata::acs_lm_senddata() ");
}
acs_lm_senddata::~acs_lm_senddata()
{
	DEBUG("Entering %s","acs_lm_senddata::~acs_lm_senddata() ");

	DEBUG("Leaving %s","acs_lm_senddata::~acs_lm_senddata() ");

}
void acs_lm_senddata::setLMInstCommandOption(bool aStatus)
{
	theLMInstCommandOption = aStatus;
}
bool acs_lm_senddata::protocolCheck(int cpVersion , int apVersion)
{
	DEBUG("Entering %s","acs_lm_senddata::protocolCheck() ");
	bool myResult = true;
	if(cpVersion < apVersion)
	{
		myResult =  false;
		ERROR("acs_lm_senddata::protocolCheck() %s","Failed");
	}
	DEBUG("Leaving %s","acs_lm_senddata::protocolCheck() ");
	return myResult;
}
bool acs_lm_senddata::sendKeyList(std::list<ACS_LM_JTP_Conversation::Buffer32*>  aList,bool aProtocolCheck,acs_lm_senddata::functionCode aFunctionCode)
{
	DEBUG("Entering %s","acs_lm_senddata::sendKeyList()");
	if(getStopSignal() == true)
	{
		DEBUG("acs_lm_senddata::sendKeyList() %s","getStopSignal is true");
		return false;
	}
	bool myResult = false;


	unsigned short protocolVer = 1;//LM CLEANUP

	ACS_JTP_Conversation_R3A::JTP_Node node = theCPData.getJTPNode();

	//DEBUG("acs_lm_senddata::sendKeyList() node.id = %d..node.state=%d,..node.type=%d", "node.system_id,node.node_state,node.system_type");

	ACS_JTP_Conversation_R3A* jtpConv = new ACS_JTP_Conversation_R3A((char*)theCPData.getCPService().c_str(), 32*1024);
	DEBUG("acs_lm_senddata::sendKeyList() Before connect cp protocol  value is : %d",protocolVer);
	if(jtpConv)
	{  
		if(connect(jtpConv, &node,protocolVer))
		{
			unsigned short myAPProtocol = 1 ; //LM CLEANUP
			if(protocolCheck(protocolVer,myAPProtocol) || aProtocolCheck)
			{
				if(send(jtpConv, &node, aList,aFunctionCode))
				{

					myResult =  true;
				}
				else
				{
					ERROR("acs_lm_senddata::sendKeyList()  : %s","send failed");

				}	
			}
		}
		else
		{
			ERROR("acs_lm_senddata::sendKeyList()  : %s","connect failed");
		}

		disconnect(jtpConv, &node);
		delete jtpConv;
	}
	DEBUG("Leaving %s","acs_lm_senddata::sendKeyList()");
	return myResult;

}
bool acs_lm_senddata::sendDisconnectionBufferList()
{
	DEBUG("Entering %s","acs_lm_senddata::sendDisConnectionBufferList()");
	if(getStopSignal() == true)
	{
		return false;
	}
	int myRetries = theCPData.getRetries();
	bool myResult = false;

	if(!theCPData.getDiscbufferList().empty())
	{
		while(myRetries > 0)
		{
			DEBUG("%s"," acs_lm_senddata::sendDisConnectionBufferList() Entering while loop");
			myRetries--;
			myResult = sendKeyList(theCPData.getDiscbufferList(),false,Disconnection);
			if(myResult == true)
			{
				myRetries = 0;
				break;
			}
			if(getStopSignal() == true)
			{
				myRetries = 0;
				break;
			}
			DEBUG("%s","acs_lm_senddata::sendDisConnectionBufferList() Leaving while loop");
		}
	}
	else
	{
		DEBUG("%s" ,"acs_lm_senddata::sendDisConnectionBufferList() There are no disconnection keys to send");
	}
	DEBUG("Leaving %s","acs_lm_senddata::sendDisConnectionBufferList()");
	return myResult;
}
bool acs_lm_senddata::sendConnectionBufferList()
{
	DEBUG("Entering %s","acs_lm_senddata::sendConnectionBufferList()");
	if(getStopSignal() == true)
	{
		return false;
	}
	int myRetries = theCPData.getRetries();
	bool myResult = false;

	if(!theCPData.getConnbufferList().empty())
	{
		while(myRetries > 0)
		{
			DEBUG("%s","acs_lm_senddata::sendConnectionBufferList() Entering while loop");
			myRetries--;
			myResult = sendKeyList(theCPData.getConnbufferList(),false,Connection);
			if(myResult == true)
			{
				myRetries = 0;
				break;
			}
			if(getStopSignal() == true)
			{
				myRetries = 0;
				break;
			}
			DEBUG("%s"," acs_lm_senddata::sendConnectionBufferList() Leaving while loop");
		}
	}
	else
	{
		DEBUG("%s" ,"acs_lm_senddata::sendConnectionBufferList()there is no connection keys to send");
	}
	DEBUG("Leaving %s","acs_lm_senddata::sendConnectionBufferList()");
	return myResult;
}
bool acs_lm_senddata::sendNormalKeyList()
{
	DEBUG("Entering %s","acs_lm_senddata::sendNormalKeyList()");
	if(getStopSignal() == true)
	{
		return false;
	}
	int myRetries = theCPData.getRetries();
	bool myResult = false;

	if(!theCPData.getBufferList().empty())
	{
		while(myRetries > 0)
		{
			DEBUG("%s","acs_lm_senddata::sendNormalKeyList(): Entering while loop");
			myRetries--;
			myResult = sendKeyList(theCPData.getBufferList(),true,Notification);
			if(myResult == true)
			{
				myRetries = 0;
				break;
			}
			if(getStopSignal() == true)
			{
				myRetries = 0;
				break;
			}
			DEBUG("%s","acs_lm_senddata::sendNormalKeyList(): Leaving while loop");
		}
	}
	else
	{
		DEBUG("%s" ,"acs_lm_senddata::sendNormalKeyList() .There are no Normal keys to send");
	}
	DEBUG("Leaving %s","acs_lm_senddata::sendNormalKeyList()");
	return myResult;
}

bool acs_lm_senddata::connect(ACS_JTP_Conversation_R3A*& jtpConv,
							  ACS_JTP_Conversation_R3A::JTP_Node* node,
							  unsigned short &protoCol)
{
	DEBUG("Entering %s","acs_lm_senddata::connect()");
	bool connected = false;
	if(node != NULL)
	{
		unsigned short ud1 =protoCol;//lmcleanup
		unsigned short ud2 = 0;
		DEBUG("%s","acs_lm_senddata::connect() : jtpConv.jexinitreq(node, ud1, ud2)");
		DEBUG("acs_lm_senddata::connect(), node->id = %d..node->state=%d..node->type=%d",node->system_id,node->node_state,node->system_type );

		if(jtpConv->jexinitreq(node, ud1, ud2))
		{
			unsigned short ret = 0;
			unsigned short rud1 = 0;
			unsigned short rud2 = 0;
			DEBUG("%s","acs_lm_senddata::connect() jtpConv.jexinitconf(rud1, rud2, ret)");
			if(jtpConv->jexinitconf(rud1, rud2, ret))
			{
				connected = true;
			}
			else
			{
				//Dont use this one at present, until the problem is solved by DSD.
				//jtpConv.jexdiscind(rud1, rud2, ret);
			}
			DEBUG ("acs_lm_senddata::connect() rud1 = %d",rud1);
			protoCol = rud1 ; //LMcleanup
		}
	}
	if(!connected)
	{
		sleep(1);  //Fix for HR65217
		acs_lm_senddata::theReportDataMutex.acquire();

		ACS_LM_EventHandler::LmEvent jtpConnProb = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_JTP_CONN_FAULT-ACS_LM_EVENT_ID_BASE];
		ACS_LM_EventHandler::reportEvent(jtpConnProb);

		acs_lm_senddata::theReportDataMutex.release();
		
	}
	DEBUG("Leaving %s","acs_lm_senddata::connect()");
	return connected;
}//end of connect

bool acs_lm_senddata::disconnect(ACS_JTP_Conversation_R3A*& jtpConv, ACS_JTP_Conversation_R3A::JTP_Node* node)
{
	DEBUG("Entering %s","acs_lm_senddata::disconnect()");
	node = NULL;
	bool disconnected = true;
	ACE_UINT16 ret = 0;
	ACE_UINT16 rud1 = 0;
	ACE_UINT16 rud2 = 0;

	jtpConv->jexdiscreq(rud1, rud2, ret);
	disconnected = true;
	DEBUG("Leaving %s","acs_lm_senddata::disconnect()");
	return disconnected;
}//end of disconnect

bool acs_lm_senddata::send(	ACS_JTP_Conversation_R3A*& jtpConv,
										ACS_JTP_Conversation_R3A::JTP_Node* node,
										const std::list<ACS_LM_JTP_Conversation::Buffer32*>& bufferList,
										functionCode fnCode)//lmcleanup
{
	DEBUG("Entering %s","acs_lm_senddata::send()");
	bool sent = true;
	ACE_UINT16 ud1 = fnCode;      //LMcleanup
	ACE_UINT16 ud2 = 0;
	//Send all the 32K buffers
	for(std::list<ACS_LM_JTP_Conversation::Buffer32*>::const_iterator it = bufferList.begin();
			it != bufferList.end(); ++it)
	{

		ACS_LM_JTP_Conversation::Buffer32* buffer32 = (*it);

		if(jtpConv->jexdatareq(ud1, ud2, buffer32->size,(char*)buffer32->buffer))
		{

			ACE_UINT16 rud1 = 0;
			ACE_UINT16 rud2 = 0;
			ACE_TCHAR *msg = NULL;
			ACE_UINT16 size = 0;
			DEBUG("acs_lm_senddata::send() %s","connect() jtpConv.jexdataind(rud1, rud2, size, msg)");
			bool ret = jtpConv->jexdataind(rud1, rud2, size, msg);
			if( ret == false )
			{
				sent = false;
			}
		}
		else
		{
			sent = false ;
		}
		DEBUG ("acs_lm_senddata::send() buffer size is :%d", buffer32->size);
	}
	DEBUG("acs_lm_senddata::send():node->system_id :%d",node->system_id);

	if(!sent)
	{
	
		acs_lm_senddata::theReportDataMutex.acquire();
		ACS_LM_EventHandler::LmEvent jtpConnProb = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_JTP_CONN_FAULT-ACS_LM_EVENT_ID_BASE];
		ACS_LM_EventHandler::reportEvent(jtpConnProb);
		acs_lm_senddata::theReportDataMutex.release();
	}
	DEBUG("Leaving %s","acs_lm_senddata::send()");
	return sent;
}//end of send


int acs_lm_senddata::svc()
{
	DEBUG("Entering %s","acs_lm_senddata::svc()");
	int myRetries = theCPData.getRetries();
	DEBUG("acs_lm_senddata::svc() The return value for theCPData.getRetries() :%d",myRetries);
	if(theLMInstCommandOption)
	{
		DEBUG("acs_lm_senddata::svc() %s","Entering lminstCmnd ");
		sendDisconnectionBufferList();
		sendConnectionBufferList();
		setLMInstCommandOption(false);
		DEBUG("acs_lm_senddata::svc() %s","Leaving lminstCmnd ");
	}
	if(getStopSignal() == true)
	{
		return 0;
	}
	sleep(1);
	sendNormalKeyList();
	DEBUG("Leaving %s","acs_lm_senddata::svc()");
	return 0;
}

void acs_lm_senddata::setStopSignal(bool aStopFlag)
{
	acs_lm_senddata::theSenddataMutex.acquire();
	theStopSignal = aStopFlag;
	acs_lm_senddata::theSenddataMutex.release();
}

bool acs_lm_senddata::getStopSignal()
{
	bool myFlag = false;
	acs_lm_senddata::theSenddataMutex.acquire();
	myFlag = theStopSignal;
	acs_lm_senddata::theSenddataMutex.release();
	return myFlag;

}

/*===============================================================================
        ROUTINE: getProtocolFromCP
        Description: This method fetch the protocol value from CP and returns it. 
=============================================================================== */
unsigned short acs_lm_senddata::getProtocolFromCP()
{
	DEBUG("acs_lm_senddata::getProtocolFromCP() %s","Entering");
	unsigned short protocolVer_CP = 1;

	ACS_JTP_Conversation_R3A::JTP_Node node = theCPData.getJTPNode();
	ACS_JTP_Conversation_R3A* jtpConv = new ACS_JTP_Conversation_R3A((char*)theCPData.getCPService().c_str(), 32*1024);

        DEBUG("acs_lm_senddata::getProtocolFromCP() Before connect cp protocol  value is : %d",protocolVer_CP);
        if(jtpConv)
        {
                if(connect(jtpConv, &node,protocolVer_CP))
                {
			DEBUG("acs_lm_senddata::getProtocolFromCP() After connect cp protocol  value is : %d",protocolVer_CP)
		}
		else
		{
			ERROR("acs_lm_senddata::getProtocolFromCP() %s","connect failed");
		}
	}
	disconnect(jtpConv, &node);
        delete jtpConv;

	DEBUG("acs_lm_senddata::getProtocolFromCP() %s","Leaving");
	return protocolVer_CP;
}
