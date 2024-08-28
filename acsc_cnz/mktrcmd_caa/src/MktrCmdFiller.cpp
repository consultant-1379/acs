#include "MktrCmdFiller.h"


MktrCmdFiller::MktrCmdFiller(const std::string& cmd)
{
    cmdLine = cmd;
}

std::list<std::string> MktrCmdFiller::combinations()
{
    std::list<std::string> cmdList;
    cmdList.push_front(cmdLine);
    return cmdList;
}

std::string MktrCmdFiller::replace(const std::string& str, const std::string& from, const std::string& to)
{
    std::string::size_type start = 0;
    std::string::size_type pos = 0;
    std::string::size_type fsz = from.size();
    //std::string::size_type tsz = to.size();
    std::string newStr = "";

    while((pos = str.find(from, pos)) != std::string::npos)
    {
        newStr += str.substr(start, pos);
        newStr += to;
        start = pos+fsz;
        pos += fsz;
    }
    newStr += str.substr(start, str.size()-start);
    return newStr;
}
