#ifndef MKTRCMDCSFILLER_H
#define MKTRCMDCSFILLER_H

#include "MktrCmdFiller.h"
#include <string>
#include <list>

class MktrCmdCsFiller : MktrCmdFiller
{
private:
    std::list<std::string> cpNameList;
    std::list<std::string> dualCPList;
    std::list<std::string> msgStoreList;
    bool isMultiCPSystem;

    inline bool isCP(short id);
    inline bool isCP(const char* name);
    inline std::string getDefaultCPName(short id);
	//CR66-begin
std::list<std::string> getDoubleCpList();
	//CR66-end

    std::list<short> getCPIDList();
    std::list<std::string> getCPNameList();
    std::list<std::string> getMessageStoreList();

public:
    MktrCmdCsFiller(const std::string& cmd);
    //virtual void fill(std::list<std::string>& cmdList);
	virtual std::list<std::string> combinations(); // LLV14
};

#endif
