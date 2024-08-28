//******************************************************************************
//
//  NAME
//     acs_lm_jtp_conversation_R2.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
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
//	    190 89-CAA nnn nnnn
//
//  AUTHOR
//     2011-12-08 by XCSSATA PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include "acs_lm_jtp_conversation_R2.h"
#include "acs_lm_tra.h"

#define cleanUpProtoCol 1 // Protocol
#define RETRYLIMIT 3
extern bool lminstCmnd ;

/*=================================================================
	ROUTINE: ACS_LM_JTP_Conversation_R2 constructor
=================================================================== */
ACS_LM_JTP_Conversation_R2::ACS_LM_JTP_Conversation_R2()
:jtpConv(NULL)
{
	INFO("%s","In ACS_LM_JTP_Conversation_R2::ACS_LM_JTP_Conversation_R2()");

}//end of constructor
/*=================================================================
	ROUTINE: ACS_LM_JTP_Conversation_R2 destructor
=================================================================== */
ACS_LM_JTP_Conversation_R2::~ACS_LM_JTP_Conversation_R2()
{
	INFO("%s","In ACS_LM_JTP_Conversation_R2::~ACS_LM_JTP_Conversation_R2()");
	disconnect();
}//end of destructor
/*=================================================================
	ROUTINE: connect
=================================================================== */
bool ACS_LM_JTP_Conversation_R2::connect(const std::string& cpService, JtpNode* node)
{
	(void)node;
	INFO("%s","In ACS_LM_JTP_Conversation_R2::connect()");
	this->cpService = cpService ;
	INFO("In ACS_LM_JTP_Conversation_R2::connect() :cpService = %s",cpService.c_str());
	return true ;
}
bool ACS_LM_JTP_Conversation_R2::connect(unsigned short &protoCol)
{
	INFO("%s","In ACS_LM_JTP_Conversation_R2::connect() protocol");
	bool connected = false;
	//unsigned short ud1 = 0;
	unsigned short ud1 = protoCol;//lmcleanup
	unsigned short  ud2 = 0;
	//unsigned short  ret = 0;
	unsigned short  rret = 0;
	unsigned short  rud1 = 0;
	unsigned short  rud2 = 0;

	if(jtpConv == NULL)
	{
		jtpConv = new ACS_JTP_Conversation_R2A((char*)cpService.c_str(), 32*1024);
		INFO("%s", "new jtpConv(ACS_JTP_Conversation_R2A) object created");
	}
	if(jtpConv->jexinitreq(ud1, ud2))
	{
		if(jtpConv->jexinitconf(rud1, rud2, rret))
		{
			connected = true;
		}
		// No need to send discreq if initconf is not successful
		//else
		//{
		//jtpConv->jexdiscreq(ud1, ud2, rret);
		//}

	}
	else
	{
		jtpConv->jexdiscind(rud1, rud2, rret);
	}
	protoCol = rud1 ; //LMcleanup
	INFO("In ACS_LM_JTP_Conversation_R2::connect() :CP replied Protocol value is : = %d",protoCol);
	// Raise Event when unable to connect to JTP
	if(!connected)
	{
		/*! @todo port Event handler class and then uncomment */
		ACS_LM_EventHandler::LmEvent jtpConnProb = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::ACS_LM_ALARM_JTP_CONN_FAULT-ACS_LM_EVENT_ID_BASE];
		ACS_LM_EventHandler::reportEvent(jtpConnProb);		
	}
	/** Use TRA */
	//cout<<"Connect() connected= "<<connected;
	//cout<<", ud1="<<ud1<<", ud2"<<ud2<<", ret="<<ret<<", rud1="<<rud1;
	//cout<<", rud2="<<rud2;
	//cout<<", rret="<<rret<<endl;
	//cout<<", connected="<<connected<<endl;
	return connected;
}//end of connect
/*=================================================================
	ROUTINE: disconnect
=================================================================== */
bool ACS_LM_JTP_Conversation_R2::disconnect()
{
	DEBUG ("%s","ACS_LM_JTP_Conversation_R2::disconnect()Entering");
	bool disconnected = false;
	if(jtpConv != NULL)
	{
		DEBUG ("%s","ACS_LM_JTP_Conversation_R2::disconnect()jtpConv != NULL");
		unsigned short ret = 0;
		unsigned short rud1 = 0;
		unsigned short rud2 = 0;
		DEBUG ("%s","ACS_LM_JTP_Conversation_R2::disconnect()Before calling jexdiscreq");
		jtpConv->jexdiscreq(rud1, rud2, ret);
		DEBUG ("%s","ACS_LM_JTP_Conversation_R2::disconnect()After calling jexdiscreq");
		delete jtpConv;
		jtpConv = NULL;

		disconnected = true;


	}
#if 0
	ACS_LM_PrintTrace(ACS_LM_JTP_Conversation_R2, "disconnect() disconnected="<<disconnected);
#endif
	//cout<<"disconnect() disconnected = "<<disconnected<<endl;
	DEBUG ("%s","ACS_LM_JTP_Conversation_R2::disconnect() Leaving");
	return disconnected;
}//end of disconnect
/*=================================================================
	ROUTINE: ACS_LM_JTP_Conversation_R2 constructor
=================================================================== */
//bool ACS_LM_JTP_Conversation_R2::send(const std::list<LkData*>& lkDataList, int lmMode)
bool ACS_LM_JTP_Conversation_R2::send(const std::list<LkData*>& lkDataList,int lmMode, const std::list<LkData*>& disclkDataList, const std::list<LkData*>& connlkDataList)//lmcleanup
{

	bool bDatasent = false ;
	buffer32List = encode(lkDataList, lmMode);  // encode normal list
	if(lminstCmnd)//LMcleanup Start
	{
		if ((int)disclkDataList.size()!=0)
			discbuffer32List = encode(disclkDataList,lmMode,true);

		if ((int)connlkDataList.size()!=0)
			connbuffer32List = encode(connlkDataList,lmMode,true);
	}//LMcleanup End

	bDatasent = senddata(buffer32List,discbuffer32List,connbuffer32List);
	clearbuffers();

	return  bDatasent ;

}

void ACS_LM_JTP_Conversation_R2::clearbuffers()
{
	for(std::list<Buffer32*>::iterator it = discbuffer32List.begin();
			it != discbuffer32List.end(); ++it)
	{
		Buffer32* buffer32 = (*it);
		delete buffer32;
	}
	discbuffer32List.clear();

	for(std::list<Buffer32*>::iterator it = connbuffer32List.begin();
			it != connbuffer32List.end(); ++it)
	{
		Buffer32* buffer32 = (*it);
		delete buffer32;
	}
	connbuffer32List.clear();

	for(std::list<Buffer32*>::iterator it = buffer32List.begin();
			it != buffer32List.end(); ++it)
	{
		Buffer32* buffer32 = (*it);
		delete buffer32;
	}
	buffer32List.clear();

	return ;
}
bool ACS_LM_JTP_Conversation_R2::senddata(std::list<ACS_LM_JTP_Conversation::Buffer32*>& bufferList,std::list<ACS_LM_JTP_Conversation::Buffer32*> discbufferList,
		std::list<ACS_LM_JTP_Conversation::Buffer32*> connbufferList)
{

	DEBUG("%s","Entering the Function ACS_LM_JTP_Conversation_R2 :senddata()");
	int retries = RETRYLIMIT ;
	unsigned short protocolVer = cleanUpProtoCol;   //LMcleanup
	unsigned short apProtcol = protocolVer ;
	//LMcleanup start, disconnection conversatin
	if(lminstCmnd)
	{
		DEBUG("%s","Sending disconnection keys,but first check protocol version...");
		if(!discbufferList.empty())
		{
			while(retries > 0)
			{
				retries--;
				DEBUG("Before connect apProtcol value is:",apProtcol);
				DEBUG("Before connect protocolVer value is:",protocolVer);
				if(connect(protocolVer))
				{
					DEBUG("%s","Connect is successful for sending disconnection keys...");
					if(protocolVer < apProtcol)
					{
						DEBUG("apProtcol version is:",apProtcol);
						DEBUG("protocolVer version is:",protocolVer);
						DEBUG("%s","Disconnect the session, as CP doesn't support the new protocol ...");
						disconnect();
						break;
					}
					if(send(discbufferList,Disconnection))
					{
						DEBUG("%s","Send is successful for sending disconnection keys...");
						disconnect();
						break;
					}
					else
					{
						DEBUG("%s","Send is failed for sending disconnection keys and try again...");
						disconnect();
						if(retries == 0) return false ;

					}
				}
				else
				{
					DEBUG("%s","Connect is failed for sending disconnection keys and try again...");
					disconnect();
//					delete jtpConv; // Problem in connecting, retry
//					jtpConv = NULL ;
					if(retries == 0) return false ;
				}

			}
		}
		else
			DEBUG("%s","There are no disconnection keys to send");
		protocolVer = cleanUpProtoCol;
		retries = RETRYLIMIT ;
		if(!connbufferList.empty())
		{
			DEBUG("%s","Sending Connection keys...");
			while(retries > 0)
			{
				retries--;

				if(connect(protocolVer))
				{
					DEBUG("%s","Connect is successful for sending Connection keys...");
					if(protocolVer < apProtcol)
					{
						DEBUG("apProtcol version is:",apProtcol);
						DEBUG("protocolVer version is:",protocolVer);
						DEBUG("%s","Disconnect the session, as CP doesn't support the new protocol ...");
						disconnect();
						break;
					}
					if(send(connbufferList,Connection))
					{
						DEBUG("%s","Send is successful for sending Connection keys...");
						disconnect();
						break;
					}
					else
					{
						DEBUG("%s","Send is failed for sending Connection keys and try again...");
						disconnect();
						if(retries == 0) return false ;
					}
				}
				else
				{
					DEBUG("%s","Connect is failed for sending Connection keys and try again...");
					disconnect();
//					delete jtpConv; // Problem in connecting, retry
//					jtpConv = NULL ;
					if(retries == 0) return false ;
				}

			}
		}
		else DEBUG("%s","There are no Connection keys to send");
		lminstCmnd = false ;
	}
	protocolVer = cleanUpProtoCol;
	retries = RETRYLIMIT ;
	DEBUG("Sending normal keys...and retries is: %d",retries);
	while(retries > 0)
	{
		retries--;


		if(connect(protocolVer))
		{
			DEBUG("%s","Connect successful for normal keys...");
			if(send(bufferList))
			{
				DEBUG("%s","Send successful for normal keys...");
				disconnect();
				break;
			}
			else
			{
				DEBUG("%s","send failed for normal keys...");
				disconnect();
				if(retries == 0) return false ;
			}
		}
		else
		{
			DEBUG("%s","connect failed for normal keys...");
			disconnect();
//			delete jtpConv; // Problem in connecting, retry
//			jtpConv = NULL ;
			if(retries == 0) return false ;
		}
	}
	return true ;
}
bool ACS_LM_JTP_Conversation_R2::send(std::list<Buffer32*>& bufferList,functionCode fnCode)
{
	DEBUG("%s","Entering ACS_LM_JTP_Conversation_R2::send() Function");
	DEBUG("Function code value is  = %d",fnCode);

	// TRaces and log here
	bool sent = true;
	unsigned short ud1 = fnCode;      //LMcleanup


	if(jtpConv != NULL)
	{
		//Send all the 32K buffers
		for(std::list<Buffer32*>::iterator it = bufferList.begin();
				it != bufferList.end(); ++it)
		{
			Buffer32* buffer32 = (*it);
			//printbuf(*buffer32);
			unsigned short ud2 = 0;
			if(jtpConv->jexdatareq(ud1, ud2, buffer32->size,(char*)buffer32->buffer))
			{
				unsigned short rud1 = 0;
				unsigned short rud2 = 0;
				char *msg = NULL;
				unsigned short size = 0;

				if(!jtpConv->jexdataind(rud1, rud2, size, msg))
				{
					DEBUG("%s","jexdataind failed in send");
					sent = false;
				}

			}
			else    DEBUG("%s","jexdatareq failed in send");
		}
	}

	else
	{
		DEBUG("%s","jtpConv is NULL object");
		sent = false ;
	}



#if 0
	ACS_LM_PrintTrace(ACS_LM_JTP_Conversation_R2, "send() lmMode="<<lmMode<<" lkDataList.size()="<<(int)lkDataList.size());
#endif
	//cout<<"send() lmMode = "<<lmMode<<" lkDataList.size()="<<(int)lkDataList.size();
#if 0
	bool sent = true;

	if(jtpConv != NULL)
	{
		unsigned short ud1 = 0;
		unsigned short ud2 = 0;

		//Encode date into 32K buffers
		std::list<Buffer32*> buffer32List = encode(lkDataList, lmMode);

		//Send all the buffers
		for(std::list<Buffer32*>::iterator it = buffer32List.begin();
				it != buffer32List.end(); ++it)
		{
			Buffer32* buffer32 = (*it);

			//To be removed later
#if 0
			ACS_LM_PrintTrace(ACS_LM_JTP_Conversa:66tion_R2, "send() buffer32->size="<<buffer32->size<<", buffer32="<<(*buffer32));
#endif
			//cout<<"send() buffer32->size = "<<buffer32->size<<", buffer32 = "<<(*buffer32)<<endl;
			if(jtpConv->jexdatareq(ud1, ud2, buffer32->size,(char*)buffer32->buffer))
			{
				unsigned short rud1 = 0;
				unsigned short rud2 = 0;
				unsigned short size = 0;
				char* msg;

				if(!jtpConv->jexdataind(rud1, rud2, size, msg))
				{
					sent = false;
				}
			}
		}
	}
	else
	{
		sent = false;
	}
#endif


	if(!sent)
	{
		/*! @todo Event handling has to be implemented */
#if 0
		ACS_LM_EventHandler::LmEvent jtpConnProb = ACS_LM_EventHandler::lmEvents[ACS_LM_EventHandler::AlarmId::ACS_LM_ALARM_JTP_CONN_FAULT-ACS_LM_EVENT_ID_BASE];
		ACS_LM_EventHandler::reportEvent(jtpConnProb);
#endif
		//cout<<"Sending event to AEH"<<endl;
	}
#if 0
	ACS_LM_PrintTrace(ACS_LM_JTP_Conversation_R2,"send() sent="<<sent);
#endif
	//cout<<"send() sent = "<<sent<<endl;
	return sent;
}//end of send
