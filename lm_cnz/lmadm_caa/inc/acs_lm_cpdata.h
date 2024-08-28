

#ifndef ACS_LM_CPDATA_H_
#define ACS_LM_CPDATA_H_


#include "acs_lm_common.h"
#include <ACS_JTP.h>
#include "acs_lm_jtp_conversation.h"
#include <ACS_JTP_Conversation_R3A.h>

class acs_lm_cpdata
{

public:
#if 0
	struct Buffer32
	{
	public:
		unsigned char buffer[JTP_BUFFER_SIZE];
		unsigned int capacity;
		unsigned int size;

		Buffer32()
		:capacity(JTP_BUFFER_SIZE), size(0)
		{
			ACE_OS::memset(buffer, 0, JTP_BUFFER_SIZE);
		}

		friend std::ostream& operator<<(std::ostream& out, const Buffer32& buffer32)
		{
			out<<std::endl;
			//print hreader
			unsigned int i=0;
			for(; i<10 && i<buffer32.size; i++)
			{
				int ln = (buffer32.buffer[i]&0x0F);
				int hn = (buffer32.buffer[i]>>4);
				out<<hexCode[hn]<<hexCode[ln]<<" ";
			}
			out<<std::endl;

			while(i<buffer32.size)
			{
				for(unsigned int j=0; j<36 && i<buffer32.size; j++, i++)
				{
					//Print each LK
					int ln = (buffer32.buffer[i]&0x0F);
					int hn = (buffer32.buffer[i]>>4);
					out<<hexCode[hn]<<hexCode[ln]<<" ";
				}
				out<<std::endl;
			}
			out<<std::endl;
			return out;
		}
	};
#endif
private:
	std::list<ACS_LM_JTP_Conversation::Buffer32*> theBufferList;
	std::list<ACS_LM_JTP_Conversation::Buffer32*> theDiscbufferList; //LMcleaup
	std::list<ACS_LM_JTP_Conversation::Buffer32*> theConnbufferList; //LMcleaup
	ACS_JTP_Conversation_R3A::JTP_Node theJTPNode;
	int theRetries;
	std::string theCPService;
public:
	acs_lm_cpdata();
	~acs_lm_cpdata();
	acs_lm_cpdata(const acs_lm_cpdata & aCPData);
	acs_lm_cpdata& operator=(const acs_lm_cpdata & aCPData);
	void setBufferList(std::list<ACS_LM_JTP_Conversation::Buffer32*> aBufferList);
	void setDiscbufferList(std::list<ACS_LM_JTP_Conversation::Buffer32*> aDiscbufferList);
	void setConnbufferList(std::list<ACS_LM_JTP_Conversation::Buffer32*> aConnbufferList);
	void setJTPNode(const ACS_JTP_Conversation_R3A::JTP_Node& aJTPNode);
	void setRetries(int aRetries);
	void setService(std::string aCPService);
	std::list<ACS_LM_JTP_Conversation::Buffer32*> getBufferList();
	std::list<ACS_LM_JTP_Conversation::Buffer32*> getDiscbufferList();
	std::list<ACS_LM_JTP_Conversation::Buffer32*> getConnbufferList();
	int getRetries()const;
	std::string getCPService()const;
	ACS_JTP_Conversation_R3A::JTP_Node getJTPNode()const;


};


#endif /* ACS_LM_CPDATA_H_ */
