#ifndef ACS_LM_SENDDATA_H_
#define ACS_LM_SENDDATA_H_

#include <ACS_JTP.h>
#include "acs_lm_jtp_conversation.h"
#include <ACS_JTP_Conversation_R3A.h>
#include <acs_lm_tra.h>
#include <acs_lm_cpdata.h>
#include <acs_lm_common.h>
#include <ace/ACE.h>

#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/OS_NS_dlfcn.h>
#include <ace/Reactor.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ace/Task_T.h>
#include <ace/OS.h>
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
#include<acs_lm_eventhandler.h>

class acs_lm_senddata:public ACE_Task_Base
{
private:
	acs_lm_cpdata theCPData;
	bool theLMInstCommandOption;
	bool theStopSignal;
	static  ACE_Recursive_Thread_Mutex theSenddataMutex;
	static ACE_Recursive_Thread_Mutex theReportDataMutex;
public:
	enum functionCode
	{
		Notification,
		Connection,
		Disconnection,
		Not_Used
	};
	int svc();
	acs_lm_senddata(acs_lm_cpdata& aCPData ,ACE_Thread_Manager* aThrManager);
	~acs_lm_senddata();
	bool sendDisconnectionBufferList();
	bool sendConnectionBufferList();
	bool sendNormalKeyList();
	bool sendKeyList(std::list<ACS_LM_JTP_Conversation::Buffer32*> aList, bool aProtocolCheck,acs_lm_senddata::functionCode aFunctionCode);
	bool protocolCheck(int cpVersion , int apVersion);
	void setLMInstCommandOption(bool aStatus);

	bool connect(ACS_JTP_Conversation_R3A*& jtpConv,
								  ACS_JTP_Conversation_R3A::JTP_Node* node,
								  unsigned short &protoCol);

	bool disconnect(ACS_JTP_Conversation_R3A*& jtpConv,
								   ACS_JTP_Conversation_R3A::JTP_Node* node);

	bool send(	ACS_JTP_Conversation_R3A*& jtpConv,
											ACS_JTP_Conversation_R3A::JTP_Node* node,
											const std::list<ACS_LM_JTP_Conversation::Buffer32*>& bufferList,
											functionCode fnCode);//lmcleanup
	void setStopSignal(bool aStopFlag);
	bool getStopSignal();
        unsigned short getProtocolFromCP();

};

#endif /* ACS_LM_SENDDATA_H_ */
