#include<acs_lm_cpdata.h>

acs_lm_cpdata::acs_lm_cpdata()
:theRetries(0),
 theCPService("")
{

}

acs_lm_cpdata::~acs_lm_cpdata()
{

	std::list<ACS_LM_JTP_Conversation::Buffer32*>::iterator myIter = theBufferList.begin();
	std::list<ACS_LM_JTP_Conversation::Buffer32*>::iterator myIterEnd = theBufferList.end();
	for(;myIter != myIterEnd;)
	{
		ACS_LM_JTP_Conversation::Buffer32* myBuffer32Ptr = *(myIter);
		delete myBuffer32Ptr;
		theBufferList.erase(myIter++);
	}

	 myIter = theDiscbufferList.begin();
	 myIterEnd = theDiscbufferList.end();
	for(;myIter != myIterEnd;)
	{
		ACS_LM_JTP_Conversation::Buffer32* myBuffer32Ptr = *(myIter);
		delete myBuffer32Ptr;
		theDiscbufferList.erase(myIter++);
	}

	 myIter = theConnbufferList.begin();
	 myIterEnd = theConnbufferList.end();
	for(;myIter != myIterEnd;)
	{
		ACS_LM_JTP_Conversation::Buffer32* myBuffer32Ptr = *(myIter);
		delete myBuffer32Ptr;
		theConnbufferList.erase(myIter++);
	}


}

acs_lm_cpdata::acs_lm_cpdata(const acs_lm_cpdata & aCPData)
{
	setBufferList(aCPData.theBufferList);
	setDiscbufferList(aCPData.theDiscbufferList);
	setConnbufferList(aCPData.theConnbufferList);
	setJTPNode(aCPData.theJTPNode);
	setRetries(aCPData.theRetries);
	setService(aCPData.theCPService);

}

acs_lm_cpdata& acs_lm_cpdata::operator=(const acs_lm_cpdata & aCPData)
{
	setBufferList(aCPData.theBufferList);
	setDiscbufferList(aCPData.theDiscbufferList);
	setConnbufferList(aCPData.theConnbufferList);
	setJTPNode(aCPData.theJTPNode);
	setRetries(aCPData.theRetries);
	setService(aCPData.theCPService);
	return *this;
}

void acs_lm_cpdata::setBufferList(std::list<ACS_LM_JTP_Conversation::Buffer32*> aBufferList)
{
	std::list<ACS_LM_JTP_Conversation::Buffer32*>::iterator myIter = aBufferList.begin();
	std::list<ACS_LM_JTP_Conversation::Buffer32*>::iterator myIterEnd = aBufferList.end();
	for(;myIter != myIterEnd;++myIter)
	{
		ACS_LM_JTP_Conversation::Buffer32* myBuffer32Ptr = *(myIter);
		ACS_LM_JTP_Conversation::Buffer32* myNewBuffer32Ptr = new ACS_LM_JTP_Conversation::Buffer32(*(myBuffer32Ptr));
		theBufferList.push_back(myNewBuffer32Ptr);
	}
}

void acs_lm_cpdata::setDiscbufferList(std::list<ACS_LM_JTP_Conversation::Buffer32*> aDiscbufferList)
{
	std::list<ACS_LM_JTP_Conversation::Buffer32*>::iterator myIter = aDiscbufferList.begin();
	std::list<ACS_LM_JTP_Conversation::Buffer32*>::iterator myIterEnd = aDiscbufferList.end();
	for(;myIter != myIterEnd;++myIter)
	{
		ACS_LM_JTP_Conversation::Buffer32* myBuffer32Ptr = *(myIter);
		ACS_LM_JTP_Conversation::Buffer32* myNewBuffer32Ptr = new ACS_LM_JTP_Conversation::Buffer32(*(myBuffer32Ptr));
		theDiscbufferList.push_back(myNewBuffer32Ptr);
	}

}
void acs_lm_cpdata::setConnbufferList(std::list<ACS_LM_JTP_Conversation::Buffer32*> aConnbufferList)
{
	std::list<ACS_LM_JTP_Conversation::Buffer32*>::iterator myIter = aConnbufferList.begin();
	std::list<ACS_LM_JTP_Conversation::Buffer32*>::iterator myIterEnd = aConnbufferList.end();
	for(;myIter != myIterEnd;++myIter)
	{
		ACS_LM_JTP_Conversation::Buffer32* myBuffer32Ptr = *(myIter);
		ACS_LM_JTP_Conversation::Buffer32* myNewBuffer32Ptr = new ACS_LM_JTP_Conversation::Buffer32(*(myBuffer32Ptr));
		theConnbufferList.push_back(myNewBuffer32Ptr);
	}

}
void acs_lm_cpdata::setJTPNode(const ACS_JTP_Conversation_R3A::JTP_Node& aJTPNode)
{
	theJTPNode = aJTPNode;
}
void acs_lm_cpdata::setRetries(int aRetries)
{
	theRetries = aRetries;
}
void acs_lm_cpdata::setService(std::string aCPService)
{
	theCPService = aCPService;
}
int acs_lm_cpdata::getRetries()const
{
	return theRetries;
}
std::string acs_lm_cpdata::getCPService()const
{
	return theCPService;
}
ACS_JTP_Conversation_R3A::JTP_Node acs_lm_cpdata::getJTPNode()const
{
	return theJTPNode;
}
std::list<ACS_LM_JTP_Conversation::Buffer32*> acs_lm_cpdata::getBufferList()
{
	return theBufferList;
}
std::list<ACS_LM_JTP_Conversation::Buffer32*> acs_lm_cpdata::getDiscbufferList()
{
	return theDiscbufferList;
}

std::list<ACS_LM_JTP_Conversation::Buffer32*> acs_lm_cpdata::getConnbufferList()
{
	return theConnbufferList;
}

